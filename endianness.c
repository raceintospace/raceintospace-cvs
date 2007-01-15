
#if __BIG_ENDIAN__

#include "assert.h"
#include "Buzz_inc.h"
#include "externs.h"

ui32 _SwapLong(ui32 value)
{
      return (( value >> 24) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) | (value << 24));
}

ui16 _SwapWord(ui16 value)
{
      return ((value << 8) | (value >> 8));
}

void SwapSimpleHdr(SimpleHdr *hdr)
{
	assert(hdr);

	SwapWord(hdr->size);
	SwapLong(hdr->offset);
}

void SwapPatchHdr(PatchHdr *hdr)
{
	assert(hdr);

	SwapWord(hdr->w);
	SwapWord(hdr->h);
	SwapWord(hdr->size);
	SwapLong(hdr->offset);
}




// This is only for loading the unmodified original data
void SwapGameDat(void)
{
	i16 i,j;

	SwapLong(*((ui32*)&Data->EMark));
	SwapLong(Data->Checksum);
	SwapWord(Data->P[0].Budget);
	SwapWord(Data->P[1].Budget);

	for (j=0;j<2;j++) {
		for (i=0;i<3;i++) {
			SwapWord(Data->P[j].Probe[i].InitCost);
			SwapWord(Data->P[j].Probe[i].UnitWeight);
			SwapWord(Data->P[j].Probe[i].MaxPay);
			SwapWord(Data->P[j].Probe[i].Safety);
		};
		for (i=0;i<5;i++) {
			SwapWord(Data->P[j].Rocket[i].InitCost);
			SwapWord(Data->P[j].Rocket[i].UnitWeight);
			SwapWord(Data->P[j].Rocket[i].MaxPay);
			SwapWord(Data->P[j].Rocket[i].Safety);
		};
		for (i=0;i<7;i++) {
			SwapWord(Data->P[j].Manned[i].InitCost);
			SwapWord(Data->P[j].Manned[i].UnitWeight);
			SwapWord(Data->P[j].Manned[i].MaxPay);
			SwapWord(Data->P[j].Manned[i].Safety);
		};
		for (i=0;i<5;i++) {
			SwapWord(Data->P[j].Misc[i].InitCost);
			SwapWord(Data->P[j].Misc[i].UnitWeight);
			SwapWord(Data->P[j].Misc[i].MaxPay);
			SwapWord(Data->P[j].Misc[i].Safety);
		};

		SwapWord(Data->P[j].Misc[5].Safety);
	}

  printf("SwapGameDat: Checksum Data = %ld\n",Data->Checksum);
}

#endif

