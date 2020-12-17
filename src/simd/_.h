#if !defined(SIMD_COMMON_H)
#define SIMD_COMMON_H

#include <stdio.h>
#include "./_types.h"
#include "./_twiddle.h"
#include "./_predict.h"

// function attributes

#define FORCE_INLINE inline __attribute__((always_inline))

/* memory

Articles
    * https://lwn.net/Articles/255364/
        from: https://lwn.net/Archives/GuestIndex/#Drepper_Ulrich
    https://events.prace-ri.eu/event/718/attachments/693/993/ato_handout.pdf
prefetch 
    https://stackoverflow.com/questions/48994494/how-to-properly-use-prefetch-instructions

*/

// format / splat

#define bN(v, N) ( ( v>>((N)-1) ) & 1 )

#define BITSPLAT_BYTE(v, BIT) bN(v,BIT),bN(v,BIT-1),bN(v,BIT-2),bN(v,BIT-3), bN(v,BIT-4),bN(v,BIT-5),bN(v,BIT-6),bN(v,BIT-7)
#define BITSPLAT8(v)  BITSPLAT_BYTE(v,8)
#define BITSPLAT16(v) BITSPLAT_BYTE(v,16), BITSPLAT_BYTE(v,8)
#define BITSPLAT32(v) BITSPLAT_BYTE(v,32), BITSPLAT_BYTE(v,24), BITSPLAT_BYTE(v,16), BITSPLAT_BYTE(v,8)
#define BITSPLAT64(v) BITSPLAT_BYTE(v,32+32), BITSPLAT_BYTE(v,32+24), BITSPLAT_BYTE(v,32+16), BITSPLAT_BYTE(v,32+8), BITSPLAT_BYTE(v,32), BITSPLAT_BYTE(v,24), BITSPLAT_BYTE(v,16), BITSPLAT_BYTE(v,8)

#define iF     "%i"
#define uF     "%u"
#define oF     "%o"
#define xF     "%x"
#define iF_ARGS(v) (v)
#define uF_ARGS(v) (v)
#define oF_ARGS(v) (v)
#define xF_ARGS(v) (v)

#define i8F    iF

#define b8F     "%i%i%i%i%i%i%i%i"
#define b16F    b8F "_" b8F
#define b32F    b16F "_" b16F
#define b64F    b32F "__" b32F
#define b8F_ARGS(v) BITSPLAT8(v)
#define b16F_ARGS(v) BITSPLAT16(v)
#define b32F_ARGS(v) BITSPLAT32(v)
#define b64F_ARGS(v) BITSPLAT64(v)

#define F_ARGS(FMT, v) FMT##F_ARGS((v))

#define F__(x) (x)
#define LOGV(FMT, VAR) printf("" #VAR " = " FMT##F "\n", F_ARGS(FMT, VAR))
// #define LOGV(FMT, VAR) printf(":" #VAR "=%" #FMT "\n", VAR)


#endif