#include "Buzz_inc.h"

extern int gtk_mouse_x, gtk_mouse_y, gtk_mouse_pressed;

extern void unimp (void);

int grGetPixel (int x, int y){unimp();return 0;}
void grSetLineStyle (int style, int width){unimp();}

extern char *screen;
extern char pal[];

int
grInitMouse (void)
{
	return gxSUCCESS;
}

void
grTrackMouse (int how)
{
	if (how != grTRACK) {
		fprintf (stderr, "bad arg to grTrackMouse\n");
		exit (1);
	}
}

void
grSetMousePos (int x, int y)
{
}

int
grGetMouseButtons (void)
{
	return (gtk_mouse_pressed);
}

int
grGetMousePos (int *xp, int *yp)
{
	*xp = gtk_mouse_x / 2;
	*yp = gtk_mouse_y / 2;
	return (0);
}

void
grSetMouseStyle (int style, int size)
{
	if (style != grCARROW || size != 8) {
		fprintf (stderr, "bad arg to grSetMouseStyle\n");
		exit (1);
	}
}

void
grDisplayMouse (int mode)
{
}

static int gr_fg_color;
static int gr_bg_color;

void
gr_set_color_map (unsigned char *map)
{
	memcpy (pal, map, 256 * 3);
}

void
gr_sync (void)
{
	unsigned char rgb[320 * 200 * 3];
	unsigned char *outp;
	int in_idx;
	int color_idx;
	char *p;

	outp = rgb;
	for (in_idx = 0; in_idx < 320*200; in_idx++) {
		color_idx = screen[in_idx] & 0xff;
		p = pal + color_idx * 3;
		*outp++ = *p++ * 4;
		*outp++ = *p++ * 4;
		*outp++ = *p++ * 4;
	}

	display_gtk_img (rgb);
}

void
grSetColor (int color)
{
	gr_fg_color = color;
}

void
grSetBkColor (int color)
{
	gr_bg_color = color;
}

void
grClearArea (int x1, int y1, int x2, int y2)
{
	int x, y;
	int idx;

	for (y = y1; y < y2; y++) {
		for (x = x1; x < x2; x++) {
			idx = y * 320 + x;
			screen[idx] = gr_bg_color;
		}
	}
	if (gr_slow)
		gr_sync ();
}

static int gr_cur_x, gr_cur_y;

void
grMoveTo (int x, int y)
{
	gr_cur_x = x;
	gr_cur_y = y;
}

void
grPutPixel (int x, int y, int color)
{
	screen[y * 320 + x] = color;
}


//#define abs(a) (((a) >= 0) ? (a) : (0))
#define swap(a,b) (t = a, a = b, b = t)
void
grLineTo (int x_arg, int y_arg)
{
	int deltax, deltay;
	int error;
	int ystep;
	int x, y;
	int x0, y0, x1, y1;
	int steep;
	int t;

	x0 = gr_cur_x;
	y0 = gr_cur_y;

	x1 = x_arg;
	y1 = y_arg;

	steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	deltax = x1 - x0;
	deltay = abs (y1 - y0);
	error = 0;

	y = y0;
	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (x = x0; x <= x1; x++) {
		if (steep) {
			grPutPixel (y, x, gr_fg_color);
		} else {
			grPutPixel (x, y, gr_fg_color);
		}
		error = error + deltay;
		if (2 * error >= deltax) {
			y = y + ystep;
			error = error - deltax;
		}
	}

	gr_cur_x = x_arg;
	gr_cur_y = y_arg;

	if (gr_slow)
		gr_sync ();
}

void
grLineRel (int dx, int dy)
{
	grLineTo (gr_cur_x + dx, gr_cur_y + dy);
}

void
grMoveRel (int dx, int dy)
{
	grMoveTo (gr_cur_x + dx, gr_cur_y + dy);
}


void
grDrawRect (int x1, int y1, int x2, int y2, int mode)
{
	if (mode != grOUTLINE) {
		fprintf (stderr, "bad arg to grDrawRect\n");
		exit (1);
	}

	grMoveTo (x1, y1);
	grLineTo (x2, y1);
	grLineTo (x2, y2);
	grLineTo (x1, y2);
	grLineTo (x1, y1);
}

void
grDrawLine (int x1, int y1, int x2, int y2)
{
	grMoveTo (x1, y1);
	grLineTo (x2, y2);
}

void
grGetMouseBounds(int *x1p, int *y1p, int *x2p, int *y2p)
{
	*x1p = 0;
	*x2p = 319;
	*y1p = 0;
	*y2p = 199;
}

void
grSetMouseBounds (int x1, int y1, int x2, int y2)
{
}


