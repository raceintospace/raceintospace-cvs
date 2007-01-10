/*****************************************************************************/
/* C prototype header file for UC.OBJ.	Provides ability to decompress audio */
/* data compressed using either ACOMP or BCOMP utilities. 									 */
/*****************************************************************************/
// ABX file format:
//
// Bytes 0-1: int  TotalFrames;     Total number of ACOMP frames in file.
// Bytes 2-5: long int TotalSize;   Total size of original source file.
// Bytes 6-7: unsigned int bufsize; Frame buffer size used to compress in.
// Bytes 8-9: unsigned int freq;    Playback frequency of audio file.
//     ....   ABH HEADERS[TotalFrames] Array of headers indicating all
//				    audio frame data.

typedef struct
{
  long int	  fileaddress;	  // Address in file of this audio section.
  ui16 fsize;		  // compressed file size.
  ui16 usize;		  // uncompressed file size.
} ABH;

unsigned int	UnCompressAudio(unsigned char *source,unsigned char *dest);
// UnCompressAudio will decompress data which was compressed using ACOMP
// into the destination address provided.  UnCompressAudio returns the
// total size, in bytes, of the uncompressed audio data.

unsigned int	GetFreq(unsigned char *sound);
// GetFreq will report the playback frequency of a particular ACOMP data
// file.
