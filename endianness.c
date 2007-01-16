
#if __BIG_ENDIAN__

#include "assert.h"
#include "Buzz_inc.h"
#include "externs.h"

uint32_t _Swap32bit(uint32_t value)
{
      return (( value >> 24) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) | (value << 24));
}

uint16_t _Swap16bit(uint16_t value)
{
      return ((value << 8) | (value >> 8));
}

void
SwapSimpleHdr(SimpleHdr *hdr)
{
	assert(hdr);

	Swap16bit(hdr->size);
	Swap32bit(hdr->offset);
}

void
SwapPatchHdr(PatchHdr *hdr)
{
	assert(hdr);

	Swap16bit(hdr->w);
	Swap16bit(hdr->h);
	Swap16bit(hdr->size);
	Swap32bit(hdr->offset);
}

void
SwapPatchHdrSmall(PatchHdrSmall *hdr)
{
	assert(hdr);

	Swap16bit(hdr->size);
	Swap32bit(hdr->offset);
}




// This is only for loading the unmodified original data
void SwapGameDat(void)
{
	i16 i,j;

	Swap32bit(*((ui32*)&Data->EMark));
	Swap32bit(Data->Checksum);
	Swap16bit(Data->P[0].Budget);
	Swap16bit(Data->P[1].Budget);

	for (j=0;j<2;j++) {
		for (i=0;i<3;i++) {
			Swap16bit(Data->P[j].Probe[i].InitCost);
			Swap16bit(Data->P[j].Probe[i].UnitWeight);
			Swap16bit(Data->P[j].Probe[i].MaxPay);
			Swap16bit(Data->P[j].Probe[i].Safety);
		};
		for (i=0;i<5;i++) {
			Swap16bit(Data->P[j].Rocket[i].InitCost);
			Swap16bit(Data->P[j].Rocket[i].UnitWeight);
			Swap16bit(Data->P[j].Rocket[i].MaxPay);
			Swap16bit(Data->P[j].Rocket[i].Safety);
		};
		for (i=0;i<7;i++) {
			Swap16bit(Data->P[j].Manned[i].InitCost);
			Swap16bit(Data->P[j].Manned[i].UnitWeight);
			Swap16bit(Data->P[j].Manned[i].MaxPay);
			Swap16bit(Data->P[j].Manned[i].Safety);
		};
		for (i=0;i<5;i++) {
			Swap16bit(Data->P[j].Misc[i].InitCost);
			Swap16bit(Data->P[j].Misc[i].UnitWeight);
			Swap16bit(Data->P[j].Misc[i].MaxPay);
			Swap16bit(Data->P[j].Misc[i].Safety);
		};

		Swap16bit(Data->P[j].Misc[5].Safety);
	}
}

#endif

