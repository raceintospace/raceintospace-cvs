#include "Buzz_inc.h"

extern unsigned char *screen;
extern GXHEADER vhptr;

void gxSetUserMalloc (void *(*malloc)(unsigned long),
		      int (*free)(void *),
		      unsigned long (*avail)(void))
{
}

int
gxVirtualSize (int mode, int w, int h)
{
	return w * h;
}

int
gxVirtualFree (int mode)
{
	return 65535;
}

int
gxCreateVirtual (int mode, GXHEADER *hp,
		 int gxVGA_mode, int w, int h)
{
	memset (hp, 0, sizeof *hp);
	hp->w = w;
	hp->h = h;
	hp->vptr = malloc (w * h + 60000);
	return gxSUCCESS;
}

void
gxDestroyVirtual (GXHEADER *hp)
{
	free (hp->vptr);
}

void
gxGetImage (GXHEADER *hp, int x0, int y0, int x1, int y1, int op)
{
	int w, h, from_idx, to_idx, row, col;

	if (op != 0) {
		fprintf (stderr, "unknown last arg to gxGetImage\n");
		exit (1);
	}

	w = x1 - x0 + 1;
	h = y1 - y0 + 1;

	for (row = 0; row < h; row++) {
		for (col = 0; col < w; col++) {
			from_idx = (y0 + row) * 320 + (x0 + col);
			to_idx = row * hp->w + col;

			hp->vptr[to_idx] = screen[from_idx];
		}
	}
}

void
gxPutImage (GXHEADER *hp, int mode, int x, int y, int op2)
{
	int row, col, from_idx, to_idx;

	if (op2 != 0) {
		fprintf (stderr, "bad mode in gxPutImage\n");
		exit (1);
	}

	switch (mode) {
	case gxSET:
		for (row = 0; row < hp->h; row++) {
			for (col = 0; col < hp->w; col++) {
				from_idx = row * hp->w + col;
				to_idx = (y + row) * 320 + (x + col);
				screen[to_idx] = hp->vptr[from_idx];
			}
		}
		break;
	case gxXOR:
		for (row = 0; row < hp->h; row++) {
			for (col = 0; col < hp->w; col++) {
				from_idx = row * hp->w + col;
				to_idx = (y + row) * 320 + (x + col);
				screen[to_idx] ^= hp->vptr[from_idx];
			}
		}
		break;

	default:
		fprintf (stderr, "bad mode in gxPutImage\n");
		exit (1);
	}
}

void
gxClearDisplay (int a, int b)
{
	if (a != 0 || b != 0) {
		fprintf (stderr, "unknown args to gxClearDisplay\n");
		exit (1);
	}
	memset (screen, 0, 320 * 200);
}

void
gxVirtualDisplay (GXHEADER *hp,
		  int from_x, int from_y,
		  int to_x0, int to_y0,
		  int to_x1, int to_y1,
		  int always_zero)
{
	int row, col, from_idx, to_idx;
	int width, height;

	width = to_x1 - to_x0 + 1;
	height = to_y1 - to_y0 + 1;

	for (row = 0; row < height; row++) {
		for (col = 0; col < width; col++) {
			from_idx = (from_y + row) * hp->w + (from_x + col);
			to_idx = (to_y0 + row) * 320 + (to_x0 + col);
			screen[to_idx] = hp->vptr[from_idx];
		}
	}
	if (gr_slow)
		gr_sync ();
}

void
gxSetDisplayPalette (char *pal)
{
}

/*
  gxVirtualVirtual(
   &flaggy,
    FCtr*23,       0,
    FCtr*23+21   ,21,
   &local2,
    0,0,
    gxSET);
*/

void
gxVirtualVirtual (GXHEADER *from,
		  int from_x1, int from_y1,
		  int from_x2, int from_y2,
		  GXHEADER *to,
		  int to_x, int to_y,
		  int mode)
{
	int w, h;
	int row, col;
	int from_idx, to_idx;

	gr_sync ();

	w = from_x2 - from_x1 + 1;
	h = from_y2 - from_y1 + 1;

	for (row = 0; row < h; row++) {
		for (col = 0; col < w; col++) {
			from_idx = (from_y1 + row) * from->w
				+ (from_x1 + col);
			to_idx = (to_y + row) * to->w
				+ (to_x + col);
			to->vptr[to_idx] = from->vptr[from_idx];
		}
	}

	gr_sync ();
}

void
gxClearVirtual (GXHEADER *hp, int a)
{
	if (a != 0) {
		fprintf (stderr, "bad arg to gxClearVirtual\n");
		exit (1);
	}

	memset (hp->vptr, 0, hp->w * hp->h);
}

void
gxVirtualScale (GXHEADER *src, GXHEADER *dest)
{
	int dest_row, dest_col, dest_idx;
	int src_row, src_col, src_idx;

	for (dest_row = 0; dest_row < dest->h; dest_row++) {
		src_row = dest_row * src->h / dest->h;

		for (dest_col = 0; dest_col < dest->w; dest_col++) {
			src_col = dest_col * src->w / dest->w;

			src_idx = src_row * src->w + src_col;
			dest_idx = dest_row * dest->w + dest_col;

			dest->vptr[dest_idx] = src->vptr[src_idx];
		}
	}
}

