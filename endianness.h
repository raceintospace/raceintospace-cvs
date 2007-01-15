#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

#ifdef __BIG_ENDIAN__

#define Swap32bit(a)  (a) = _Swap32bit((a))
#define Swap16bit(a)  (a) = _Swap16bit((a))
uint32_t _Swap32bit(uint32_t value);
uint16_t _Swap16bit(uint16_t value);
void SwapSimpleHdr(SimpleHdr *hdr);
void SwapPatchHdr(PatchHdr *hdr);
void SwapGameDat(void);

#else

#define Swap32bit(a)
#define Swap16bit(a)
#define SwapGameDat()
#define SwapSimpleHdr(a)
#define SwapPatchHdr(a)

#endif


#endif // __ENDIANNESS_H__
