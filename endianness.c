

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

float _SwapFloat(float value)
{
	union
	{
		float f;
		unsigned char b[4];
	} dat1, dat2;

	dat1.f = value;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

// This will swap all the player structures
void SwapGameDat(void)
{
	i16 i,j;

	_Swap32bit(*((ui32*)&Data->EMark));
	_Swap32bit(Data->Checksum);

	for (j = 0; j < 28; j++)
	{
		_Swap16bit(Data->Prestige[j].Points[0]);
		_Swap16bit(Data->Prestige[j].Points[1]);
	}
	
	for (j = 0; j < 2 ; j++) {
		_Swap16bit(Data->P[j].Cash);
		_Swap16bit(Data->P[j].Budget);
		_Swap16bit(Data->P[j].Prestige);
		
		for (i = 0; i < 5; i++)
		{
			_Swap16bit(Data->P[j].PrestHist[i][0]);
			_Swap16bit(Data->P[j].PrestHist[i][1]);
			
			_Swap16bit(Data->P[j].PresRev[i]);
			
			_Swap16bit(Data->P[j].Spend[i][0]);
			_Swap16bit(Data->P[j].Spend[i][1]);
			_Swap16bit(Data->P[j].Spend[i][2]);
			_Swap16bit(Data->P[j].Spend[i][3]);
		}
		
		_Swap16bit(Data->P[j].tempPrestige[0]);
		_Swap16bit(Data->P[j].tempPrestige[1]);
		
		for (i = 0; i < 40; i++)
		{
			_Swap16bit(Data->P[j].BudgetHistory[i]);
			_Swap16bit(Data->P[j].BudgetHistoryF[i]);
		}
		
		for (i=0;i<3;i++) {
			_Swap16bit(Data->P[j].Probe[i].InitCost);
			_Swap16bit(Data->P[j].Probe[i].UnitWeight);
			_Swap16bit(Data->P[j].Probe[i].MaxPay);
			_Swap16bit(Data->P[j].Probe[i].Safety);
		};
		for (i=0;i<5;i++) {
			_Swap16bit(Data->P[j].Rocket[i].InitCost);
			_Swap16bit(Data->P[j].Rocket[i].UnitWeight);
			_Swap16bit(Data->P[j].Rocket[i].MaxPay);
			_Swap16bit(Data->P[j].Rocket[i].Safety);
		};
		for (i=0;i<7;i++) {
			_Swap16bit(Data->P[j].Manned[i].InitCost);
			_Swap16bit(Data->P[j].Manned[i].UnitWeight);
			_Swap16bit(Data->P[j].Manned[i].MaxPay);
			_Swap16bit(Data->P[j].Manned[i].Safety);
		};
		for (i=0;i<7;i++) {
			_Swap16bit(Data->P[j].Misc[i].InitCost);
			_Swap16bit(Data->P[j].Misc[i].UnitWeight);
			_Swap16bit(Data->P[j].Misc[i].MaxPay);
			_Swap16bit(Data->P[j].Misc[i].Safety);
		};		
		
		for (i = 0; i< 65; i++)
		{
			_Swap16bit(Data->P[j].Pool[i].Prestige);
		}
		
		
		_Swap16bit(Data->P[j].PastMis);

		for (i = 0; i < 30; i++)
			_Swap16bit(Data->P[j].PastIntel[i].num);

	}
	// End of GameDatSwap
}

#ifdef __BIG_ENDIAN__
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

/* vim: set noet ts=4 sw=4 tw=77: */
