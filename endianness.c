

#include "assert.h"
#include "Buzz_inc.h"
#include "externs.h"

// Need these functions to always exist
uint32_t _Swap32bit(uint32_t value)
{
      return (( value >> 24) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) | (value << 24));
}

uint16_t _Swap16bit(uint16_t value)
{
      return ((value << 8) | (value >> 8));
}

// This will swap all the player structures
void SwapGameDat(void)
{
	i16 i,j;

	Swap32bit_alt(*((ui32*)&Data->EMark));
	Swap32bit_alt(Data->Checksum);

	for (j = 0; j < 28; j++)
	{
		Swap16bit_alt(Data->Prestige[j].Points[0]);
		Swap16bit_alt(Data->Prestige[j].Points[1]);
	}
	
	for (j = 0; j < 2 ; j++) {
		Swap16bit_alt(Data->P[j].Cash);
		Swap16bit_alt(Data->P[j].Budget);
		Swap16bit_alt(Data->P[j].Prestige);
		
		for (i = 0; i < 5; i++)
		{
			Swap16bit_alt(Data->P[j].PrestHist[i][0]);
			Swap16bit_alt(Data->P[j].PrestHist[i][1]);
			
			Swap16bit_alt(Data->P[j].PresRev[i]);
			
			Swap16bit_alt(Data->P[j].Spend[i][0]);
			Swap16bit_alt(Data->P[j].Spend[i][1]);
			Swap16bit_alt(Data->P[j].Spend[i][2]);
			Swap16bit_alt(Data->P[j].Spend[i][3]);
		}
		
		Swap16bit_alt(Data->P[j].tempPrestige[0]);
		Swap16bit_alt(Data->P[j].tempPrestige[1]);
		
		for (i = 0; i < 40; i++)
		{
			Swap16bit_alt(Data->P[j].BudgetHistory[i]);
			Swap16bit_alt(Data->P[j].BudgetHistoryF[i]);
		}
		
		for (i=0;i<3;i++) {
			Swap16bit_alt(Data->P[j].Probe[i].InitCost);
			Swap16bit_alt(Data->P[j].Probe[i].UnitWeight);
			Swap16bit_alt(Data->P[j].Probe[i].MaxPay);
			Swap16bit_alt(Data->P[j].Probe[i].Safety);
		};
		for (i=0;i<5;i++) {
			Swap16bit_alt(Data->P[j].Rocket[i].InitCost);
			Swap16bit_alt(Data->P[j].Rocket[i].UnitWeight);
			Swap16bit_alt(Data->P[j].Rocket[i].MaxPay);
			Swap16bit_alt(Data->P[j].Rocket[i].Safety);
		};
		for (i=0;i<7;i++) {
			Swap16bit_alt(Data->P[j].Manned[i].InitCost);
			Swap16bit_alt(Data->P[j].Manned[i].UnitWeight);
			Swap16bit_alt(Data->P[j].Manned[i].MaxPay);
			Swap16bit_alt(Data->P[j].Manned[i].Safety);
		};
		for (i=0;i<7;i++) {
			Swap16bit_alt(Data->P[j].Misc[i].InitCost);
			Swap16bit_alt(Data->P[j].Misc[i].UnitWeight);
			Swap16bit_alt(Data->P[j].Misc[i].MaxPay);
			Swap16bit_alt(Data->P[j].Misc[i].Safety);
		};		
		
		for (i = 0; i< 65; i++)
		{
			Swap16bit_alt(Data->P[j].Pool[i].Prestige);
		}
		
		
		Swap16bit_alt(Data->P[j].PastMis);

		for (i = 0; i < 30; i++)
			Swap16bit_alt(Data->P[j].PastIntel[i].num);

	}
	// End of GameDatSwap
}

#ifdef __BIG_ENDIAN__
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



#endif


