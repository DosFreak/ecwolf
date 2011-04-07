// Misc stuff to transition from ZDoom to Wolf

#ifndef __ZDOOM_SUPPORT__
#define __ZDOOM_SUPPORT__

#include "m_crc32.h"

inline unsigned int MakeKey(const char *s, size_t len) { return CalcCRC32((const BYTE*)s, len); }
inline unsigned int MakeKey(const char *s) { return MakeKey(s, strlen(s)); }

#define countof(x) (sizeof(x)/sizeof(x[0]))
#ifndef __BIG_ENDIAN__
#define MAKE_ID(a,b,c,d)	((DWORD)((a)|((b)<<8)|((c)<<16)|((d)<<24)))
#else
#define MAKE_ID(a,b,c,d)	((DWORD)((d)|((c)<<8)|((b)<<16)|((a)<<24)))
#endif

#define MAXWIDTH 2560
#define M_Realloc realloc
#define M_Malloc malloc
#define M_Free free
#define Printf printf
#define I_Error Quit
#define I_FatalError(a) printf(a);exit(0);

#define FLOAT2FIXED(x) (fixed_t((x)*FRACUNIT))
#define MulScale16(x,y) (SDWORD((SQWORD(x)*SQWORD(y))>>16))

#endif /* __ZDOOM_SUPPORT__ */