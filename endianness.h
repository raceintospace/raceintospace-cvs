#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

uint32_t _Swap32bit(uint32_t value);
uint16_t _Swap16bit(uint16_t value);
void 			SwapGameDat(void);

#define Swap32bit_alt(a)  (a) = _Swap32bit((a))
#define Swap16bit_alt(a)  (a) = _Swap16bit((a))

#ifdef __BIG_ENDIAN__

#define Swap32bit(a)  (a) = _Swap32bit((a))
#define Swap16bit(a)  (a) = _Swap16bit((a))
void SwapSimpleHdr(SimpleHdr *hdr);
void SwapPatchHdr(PatchHdr *hdr);
void SwapPatchHdrSmall(PatchHdrSmall *hdr);

#else

#define Swap32bit(a)
#define Swap16bit(a)
#define SwapSimpleHdr(a)
#define SwapPatchHdr(a)
#define SwapPatchHdrSmall(a)

#endif


#endif // __ENDIANNESS_H__
