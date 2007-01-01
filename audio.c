		
#define SQLCH	0x40		// Squelch byte flag
#define RESYNC	0x80		// Resync byte flag.

#define DELTAMOD 0x30		// Bit mask for delta mod bits.

#define ONEBIT	0x10 		// Bit pattern for one bit delta mod.
#define TWOBIT	0x20		// Bit pattern for two bit delta mod.
#define FOURBIT 0x30 		// Bit pattern for two bit delta mod.


signed char TRANS[] = {
	-8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8,
	-16,-14,-12,-10,-8,-6,-4,-2,2,4,6,8,10,12,14,16,
	-24,-21,-18,-15,-12,-9,-6,-3,3,6,9,12,15,18,21,24,
	-32,-28,-24,-20,-16,-12,-8,-4,4,8,12,16,20,24,28,32,
	-40,-35,-30,-25,-20,-15,-10,-5,5,10,15,20,25,30,35,40,
	-48,-42,-36,-30,-24,-18,-12,-6,6,12,18,24,30,36,42,48,
	-56,-49,-42,-35,-28,-21,-14,-7,7,14,21,28,35,42,49,56,
	-64,-56,-48,-40,-32,-24,-16,-8,8,16,24,32,40,48,56,64,
	-72,-63,-54,-45,-36,-27,-18,-9,9,18,27,36,45,54,63,72,
	-80,-70,-60,-50,-40,-30,-20,-10,10,20,30,40,50,60,70,80,
	-88,-77,-66,-55,-44,-33,-22,-11,11,22,33,44,55,66,77,88,
	-96,-84,-72,-60,-48,-36,-24,-12,12,24,36,48,60,72,84,96,
	-104,-91,-78,-65,-52,-39,-26,-13,13,26,39,52,65,78,91,104,
	-112,-98,-84,-70,-56,-42,-28,-14,14,28,42,56,70,84,98,112,
	-120,-105,-90,-75,-60,-45,-30,-15,15,30,45,60,75,90,105,120,
	-128,-112,-96,-80,-64,-48,-32,-16,16,32,48,64,80,96,112,127,
};


// Report playback frequency for an ACOMP file.
unsigned int
GetFreq (unsigned char *sound)
{
	return (*(usnigned short *)(sound + 2));
}

unsigned char ah;
unsigned char al;

unsigned short bx;
unsigned short dx;

// DX contains PREVIOUS.
// AH contains bit mask being rotated out.
// BX up/down 1 bit value.
void
delta1 (void)
{
	bitval = ah & 0x80;

	ah <<= 1;

	if (bitval == 0) {
		dx -= bx;
		if (dx_previous >= 0)
			dx = 0;
	} else {
		dx += bx;
		if ((dx & 0xff00) == 0)
			goto store;
		dx = 0x00ff;
	}

	al = dl;
	*di++ = al;
}

// BX-> base address of translate table.
// DX-> previous.
// AL-> index.
void 
demodulate (void)
{
	al = TRANS[al];
	ax = (signed)al;
	dx += ax;		// Do word sized add, into previous.
	jns high;
	dx = 0;			// Underflowed.

high: 
	// Did it overflow?
	if ((dx & 0xff00) == 0)
		goto OK;
	dx = 0x00ff;  		// Maxed out.
ok:
	*di++ = dl;
}


// UnCompressAudio will decompress data which was compressed using ACOMP
// into the destination address provided.  UnCompressAudio returns the
// total size, in bytes, of the uncompressed audio data.
unsigned int
UnCompressAudio(unsigned char *source,unsigned char *dest)
{
	si = source;
	di = dest;

	slen = *(unsigned short *)si; // get length
	si += 2;

	cx = slen;

	freq = *(unsigned short *)si; // frequency
	si += 2;

	frame = *(unsigned char *)si; // frame size
	si++;

	si++; // skip squelch
	si += 2; // skip max err 

	prev_val = *si++ & 0xff;
	*di++ = prev_val; // Get initial previous data point.

	dx = prev_val;

	if (--cx == 0)
		goto done;

	ah = al;  // AH, always the previous.

dcmp:
	al = *si++; // get sample

	if (al == RESYNC)
		goto notr;

	al <<= 1;
	dx = al & 0xff;

	*di++ = al;

	if (--cx)
		goto dcmp;
	goto done;

notr:
	if (al == SQLCH)
		goto fram;

	al &= 0x3f; // Leave just the count.
	save_cx = cx;
	
	cx = al & 0xff;
	bx = cx;
	al = dl;
	
	while (cx--)
		*di++ = al;

	cx = save_cs;

	cx -= bx;
	if (cx)
		goto dcmp;
	goto done;

fram:
	bx = ax; // command byte into BX
	bx &= 0xf; // Multiplier being used.
	bx *= 16;

	al &= DELTAMOD;

	save_cx = cx;

	cx = frame; // Get frame size.
	if (al != ONEBIT)
		goto next1;
	
	cx /= 8;

	bx = TRANS[bx + 8] & 0xff; // Get up amount

go:
	al = *si++;
	t = al; al = ah; ah = t; // Place prev in AL, Bit mask in AH

	for (i = 0; i < 8; i++)
		Delta1 ();

	ah = al;
	if (--cx)
		goto go;

	goto renter;

next1:
	if (al != TOWBIT)
		goto next2;

	bx += 6; // Point at +- 2 bit's in table.
	cx /= 4; // 4 samples per byte.

gogo:
	al = *si++;
	al >>= 6;
	DeModulate ();

	al = si[-1];
	al >>= 4;
	al &= 3;
	DeModulate ();

	al = si[-1];
	al >>= 2;
	al &= 3;
	DeModulate ();

	al = si[-1];
	al &= 3;
	DeModulate ();

	if (--cx)
		goto gogo;
	goto renter;

next2:
	cx /= 2; // Two samples per byte.

go2:
	al = *si++; // Get sample.
	al >>= 4;
	DeModulate ();

	al = si[-1];
	al &= 0x0f;
	DeModulate ();
	if (--cx)
		goto go2;

renter:
	cx = save_cs;
	cx -= frame;
	if (cx)
		goto dcmp;

done:
	ax = slen;

}
