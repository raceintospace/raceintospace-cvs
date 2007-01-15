#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

#ifdef __BIG_ENDIAN__

#define SwapLong(a)  (a) = _SwapLong((a))
#define SwapWord(a)  (a) = _SwapWord((a))
ui32 _SwapLong(ui32 value);
ui16 _SwapWord(ui16 value);
void SwapSimpleHdr(SimpleHdr *hdr);
void SwapPatchHdr(PatchHdr *hdr);
void SwapGameDat(void);

#else

#define SwapLong(a)
#define SwapWord(a)
#define SwapGameDat()
#define SwapSimpleHdr(a)
#define SwapPatchHdr(a)

#endif


#endif // __ENDIANNESS_H__
