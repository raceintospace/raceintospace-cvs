#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

uint32_t _Swap32bit(uint32_t value);
uint16_t _Swap16bit(uint16_t value);
float _SwapFloat(float value);
void SwapGameDat(void);

#ifdef __BIG_ENDIAN__

#define Swap32bit(a)  (a) = _Swap32bit((a))
#define Swap16bit(a)  (a) = _Swap16bit((a))
#define SwapFloat(a)  (a) = _SwapFloat((a))
void SwapPatchHdr(PatchHdr *hdr);
void SwapPatchHdrSmall(PatchHdrSmall *hdr);

#else

#define Swap32bit(a)
#define Swap16bit(a)
#define SwapFloat(a)
#define SwapPatchHdr(a)
#define SwapPatchHdrSmall(a)

#endif


#endif // __ENDIANNESS_H__
