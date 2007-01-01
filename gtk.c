#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

int color_flag = 1;

extern void gr_sync (void);

void app_key_event (int state, int keyval, int length, char *string);
void app_press (int x, int y, int button, int state);
void app_release (int x, int y, int button, int state);
void app_motion (int x, int y);

int double_size = 1;

void set_double_size (int flag);

static void 
done ()
{
	gtk_main_quit ();
	exit (0);
}

GtkWidget *main_window;
GtkWidget *main_vbox;
GtkWidget *main_area;
GdkPixmap *main_pixmap;
GdkFont *font;

static gint main_configure (GtkWidget *area, GdkEventConfigure *ev);
static gint main_expose (GtkWidget *area, GdkEventExpose *ev);
static gint main_key_press (GtkWidget *area, GdkEventKey *ev);

static gint main_button_press (GtkWidget *area, GdkEventButton *ev);
static gint main_button_release (GtkWidget *area, GdkEventButton *ev);
static gint motion_notify (GtkWidget *widget, GdkEventMotion *event);

static unsigned char *img;
static unsigned char *big_img;

static int need_redraw;
static GdkGC *gc;

static void redraw (void);

static int width, height;

char *filename;
FILE *inf;

int
setup_gtk (int *argcp, char ***argvp)
{
	int c;
	char *fontname;
	int w, h;

	gtk_init (argcp, argvp);

	width = 320;
	height = 200;

	if ((img = calloc (width * height * 3, sizeof *img)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	if ((big_img = calloc (2 * width * 2 * height * 3,
			       sizeof *img)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	fontname = "-adobe-courier-bold-r-normal-*-*-140-*-*-*-*-*-*";
	if ((font = gdk_font_load (fontname)) == NULL) {
		fprintf (stderr, "can't get font %s\n", fontname);
		exit (1);
	}

	main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (main_vbox);

	main_area = gtk_drawing_area_new ();
	
	w = width;
	h = height;
	if (double_size) {
		w *= 2;
		h *= 2;
	}

	gtk_drawing_area_size (GTK_DRAWING_AREA (main_area), w, h);
	gtk_widget_show (main_area);
	gtk_box_pack_start (GTK_BOX (main_vbox),
			    main_area, TRUE, TRUE, 0);
	
	gtk_window_set_title (GTK_WINDOW (main_window), (*argvp)[0]);
	gtk_quit_add_destroy (1, GTK_OBJECT (main_window));
	gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
			    GTK_SIGNAL_FUNC (done), NULL);
	gtk_signal_connect (GTK_OBJECT (main_window), "destroy",
			    GTK_SIGNAL_FUNC (done), NULL);
	gtk_signal_connect (GTK_OBJECT (main_area), "expose_event",
			    GTK_SIGNAL_FUNC (main_expose), NULL);
	gtk_signal_connect (GTK_OBJECT (main_area), "configure_event",
			    GTK_SIGNAL_FUNC (main_configure), NULL);
	gtk_signal_connect (GTK_OBJECT(main_window), "key_press_event",
			    GTK_SIGNAL_FUNC(main_key_press), NULL);

	gtk_signal_connect (GTK_OBJECT(main_area), "button_press_event",
			    GTK_SIGNAL_FUNC(main_button_press), NULL);
	gtk_signal_connect (GTK_OBJECT(main_area), "button_release_event",
			    GTK_SIGNAL_FUNC(main_button_release), NULL);
	gtk_signal_connect (GTK_OBJECT(main_window), "motion_notify_event",
			    GTK_SIGNAL_FUNC(motion_notify), NULL);


	gtk_container_add (GTK_CONTAINER (main_window), main_vbox);

	gtk_widget_set_events (GTK_WIDGET (main_area),
			       GDK_EXPOSURE_MASK|
			       GDK_BUTTON_PRESS_MASK|
			       GDK_BUTTON_RELEASE_MASK|
			       GDK_POINTER_MOTION_MASK|
			       GDK_POINTER_MOTION_HINT_MASK|
			       GDK_KEY_PRESS_MASK);

	gtk_widget_show (main_window);

	return (0);
}

void
step_gtk (void)
{
	while (gtk_events_pending ())
		gtk_main_iteration_do (gtk_false ());

}

static gint
main_configure (GtkWidget *area, GdkEventConfigure *ev)
{
	if (main_pixmap)
		gdk_pixmap_unref (main_pixmap);
	main_pixmap = gdk_pixmap_new (area->window,
				      area->allocation.width,
				      area->allocation.height,
				      -1);
	need_redraw = 1;

	return (TRUE);
}

static gint
main_expose (GtkWidget *area, GdkEventExpose *ev)
{
	if (gc == NULL)
		gc = gdk_gc_new (main_area->window);

	if (need_redraw) {
		need_redraw = 0;
		redraw ();
	}

	gdk_draw_pixmap (area->window,
			 area->style->fg_gc[GTK_WIDGET_STATE (area)],
			 main_pixmap,
			 ev->area.x, ev->area.y,
			 ev->area.x, ev->area.y,
			 ev->area.width, ev->area.height);
	return (FALSE);
}

void
force_redraw (void)
{
 	GdkRectangle area;

	need_redraw = 1;

	area.x = 0;
	area.y = 0;
	area.width = main_area->allocation.width;
	area.height = main_area->allocation.height;
	gtk_widget_draw(main_area, &area);
}

void
set_double_size (int flag)
{
	int w, h;

	double_size = flag;

	if (double_size == 0) {
		w = width;
		h = height;
	} else {
		w = width * 2;
		h = height * 2;
	}
	
	gtk_drawing_area_size (GTK_DRAWING_AREA (main_area), w, h);
	gtk_window_resize (GTK_WINDOW (main_window), w, h);
}

static gint
main_button_press (GtkWidget *area, GdkEventButton *ev)
{
	app_press (ev->x, ev->y, ev->button, ev->state);
	return (TRUE);
}

gint
main_button_release (GtkWidget *area, GdkEventButton *ev)
{
	app_release (ev->x, ev->y, ev->button, ev->state);
	return (TRUE);
}

static gint
motion_notify (GtkWidget *widget, GdkEventMotion *event)
{
	int x;
	int y;
	GdkModifierType state;

	if (event->is_hint) {
		gdk_window_get_pointer(event->window, &x, &y, &state);
	} else {
		x = event->x;
		y = event->y;
		state = event->state;
	}
  
	app_motion (x, y);

	return TRUE;
}

int gtk_mouse_x;
int gtk_mouse_y;
int gtk_mouse_pressed;

void
app_press (int x, int y, int button, int state)
{
	gtk_mouse_pressed = 1;
	gtk_mouse_x = x;
	gtk_mouse_y = y;
}

void
app_release (int x, int y, int button, int state)
{
	gtk_mouse_pressed = 0;
	gtk_mouse_x = x;
	gtk_mouse_y = y;
}

void
app_motion (int x, int y)
{
	gtk_mouse_x = x;
	gtk_mouse_y = y;
}

gint
main_key_press (GtkWidget *area, GdkEventKey *ev)
{
	app_key_event (ev->state, ev->keyval, ev->length, ev->string);
	return (TRUE);
}

#define KEYBUF_SIZE 256
int keybuf[KEYBUF_SIZE];
int keybuf_in_idx, keybuf_out_idx;

void
app_key_event (int state, int keyval, int length, char *string)
{
	int trans;

	trans = keyval;

	switch (keyval) {
	case GDK_Escape: trans = 033; break;
	case GDK_Page_Up: trans = 0x4900; break;
	case GDK_Page_Down: trans = 0x5100; break;
	case GDK_Up: trans = 0x4800; break;
	case GDK_Down: trans = 0x5000; break;
	case GDK_Return: trans = '\r'; break;
	}

	printf ("got key 0x%x (0x%x)\n", keyval, trans);

	keybuf[keybuf_in_idx] = trans;
	keybuf_in_idx = (keybuf_in_idx + 1) % KEYBUF_SIZE;
}

int
bioskey (int peek)
{
	int c;

	gr_sync ();

	step_gtk ();

	if (peek) {
		if (keybuf_in_idx != keybuf_out_idx)
			return (1);
		return (0);
	}

	if (keybuf_in_idx == keybuf_out_idx)
		return (0);

	c = keybuf[keybuf_out_idx];
	keybuf_out_idx = (keybuf_out_idx + 1) % KEYBUF_SIZE;

	return (c);
}

static void
redraw (void)
{
	char buf[1000];
	char *p;
	struct tm tm;
	int w, h;
	int x, y;
	int s_idx, b_idx;
	static double first, cur;
	struct frame_hdr_odo3 *odo3;
	struct frame_hdr_cici *cici;
	int i;

	w = main_area->allocation.width;
	h = main_area->allocation.height;
	gdk_draw_rectangle (main_pixmap, gc, 1, 0, 0, w, h);

	if (w >= width * 2 && h >= height * 2) {
		for (y = 0; y < height*2; y++) {
			for (x = 0; x < width*2; x++) {
				s_idx = y/2 * width + x/2;
				b_idx = y * 2 * width + x;
				if (color_flag == 0) {
					big_img[b_idx] = img[s_idx];
				} else {
					s_idx *= 3;
					b_idx *= 3;
					big_img[b_idx] = img[s_idx];
					big_img[b_idx+1] = img[s_idx+1];
					big_img[b_idx+2] = img[s_idx+2];
				}
			}
		}
		if (color_flag == 0) {
			gdk_draw_gray_image (main_pixmap, gc,
					     0, 0, width*2, height*2,
					     GDK_RGB_DITHER_NONE,
					     big_img, width * 2);
		} else {
			gdk_draw_rgb_image (main_pixmap, gc,
					    0, 0, width*2, height*2,
					    GDK_RGB_DITHER_NONE,
					    big_img, width * 2 * 3);
		}

	} else {
		if (color_flag == 0) {
			gdk_draw_gray_image (main_pixmap, gc,
					     0, 0, width, height,
					     GDK_RGB_DITHER_NONE,
					     img, width);
		} else {
			gdk_draw_rgb_image (main_pixmap, gc,
					    0, 0, width, height,
					    GDK_RGB_DITHER_NONE,
					    img, width * 3);
		}
	}
}

void
display_gtk_img (unsigned char *rgb)
{
	memcpy (img, rgb, 320 * 200 * 3);
	force_redraw ();
	step_gtk ();
}

