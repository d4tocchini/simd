#if !defined(SIMD_TYPES_H)
#define SIMD_TYPES_H

#define __STDC_FORMAT_MACROS
#include <stdint.h>
#include <inttypes.h>
// printf("blabla %" PRIi64 " blabla\n", var);
#include <stdbool.h>

/*

* Integer overflow considerations in C http://www.pixelbeat.org/programming/gcc/integer_overflow.html


Beware of signed/unsigned integer comparisons:

    `-Wsign-compare` is a very useful option to enable so as to pin point possible issues in signed comparison.
    https://www.airs.com/blog/archives/327
        There are good reasons to use signed types: they don’t have odd behaviour around zero, so you can write i < limit - 1 without worrying about the case limit == 0. There are good reasons to use unsigned types for things like the number of elements in a container: you get the full range of sizes, rather than limiting yourself to only the positive half. In particular, the C++ standard containers use unsigned types as their size. Combining these two rules gets you in trouble with portable code. The only reasonable answer I can see for portable code is to use -Wsign-compare and work around the many false positive warnings.

    https://git.savannah.gnu.org/gitweb/?p=coreutils.git;a=commit;h=e2dbcee4
        * src/truncate.c: Explicitly convert from off_t to intmax_t
        when printing numbers as they may be different types.
        Also don't mix size_t and off_t types in operations as
        the latter will be promoted to unsigned when these types
        are the same size.

*/

/*
aligned types & variables

        int x ALIGNED_16 = 0;
        struct foo { int x[2] ALIGNED_16; };

    NOTE: the effectiveness of aligned attributes may be limited by inherent limitations in your linker. On many systems, the linker is only able to arrange for variables to be aligned up to a certain maximum alignment. (For some linkers, the maximum supported alignment may be very very small.) If your linker is only able to align variables up to a maximum of 8 byte alignment, then specifying aligned(16) in an __attribute__ will still only provide you with 8 byte alignment. See your linker documentation for further information.
    NOTE: `aligned` attribute can only increase alignment; but you can decrease with `packed`
    REF: https://books.google.com/books?id=KJORYTHOxbEC&pg=PA143&lpg=PA143&dq=%22_mm_malloc%22+performance&source=bl&ots=y8E9ircmZy&sig=ACfU3U23aRBzJ0FxbyBEP-_iKo2vcHpfBg&hl=en&sa=X&ved=2ahUKEwj-_IbFwqvqAhUlJTQIHeKpChMQ6AEwBHoECAoQAQ#v=onepage&q=%22_mm_malloc%22%20performance&f=false
*/
#define ALIGNED_MAX __attribute__ ((aligned)) 
    // Sets the alignment to largest ever used for any data type on target machine you are compiling for. 
    // Can often make copy operations more efficient, b/c compiler can use whatever instructions copy the biggest chunks of memory when performing copies to or from the variables or fields that you have aligned this way.
#define ALIGNED_64  __attribute__ ((aligned (64)))
#define ALIGNED_32  __attribute__ ((aligned (32)))
#define ALIGNED_16  __attribute__ ((aligned (16)))
#define ALIGNED_8   __attribute__ ((aligned (8)))
#define ALIGNED_b512 ALIGNED_64
#define ALIGNED_b256 ALIGNED_32
#define ALIGNED_b128 ALIGNED_16
#define ALIGNED_b64  ALIGNED_8 

/*
__builtin_assume_aligned
https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
*/

// #define I64     LL
// #define U64     ULL


// #define BITSPLAT_BYTE(v, BIT) (v>>(BIT-1))&1,(v>>(BIT-2))&1,(v>>(BIT-3))&1,(v>>(BIT-4))&1, (v>>(BIT-5))&1,(v>>(BIT-6))&1,(v>>(BIT-7))&1,(v>>(BIT-8))&1
// #define BITSPLAT8(v)  BITSPLAT_BYTE(v,8)
// #define BITSPLAT16(v) BITSPLAT_BYTE(v,16), BITSPLAT_BYTE(v,8)
// #define BITSPLAT32(v) BITSPLAT_BYTE(v,32), BITSPLAT_BYTE(v,24), BITSPLAT_BYTE(v,16), BITSPLAT_BYTE(v,8)

// #define iF     "%i"
// #define uF     "%u"
// #define xF     "%x"
// #define xF     "%x"

// #define b8F     "%i%i%i%i""%i%i%i%i"
// #define b16F    b8F "_" b8F
// #define b32F    b16F "_" b16F

// #define _b8F_(v) BITSPLAT8(v)
// #define _b16F_(v) BITSPLAT16(v)
// #define _b32F_(v) BITSPLAT32(v)

#define   uch   unsigned char
#define   ch    char
#define c_uch   const uch
#define c_ch    const ch


// ints

#define   u8  uint8_t
#define   i8   int8_t
#define   u16 uint16_t
#define   i16  int16_t
#define   u32 uint32_t
#define   i32  int32_t
#define   u64 uint64_t
#define   i64  int64_t
#define c_u8    const u8
#define c_i8    const i8
#define c_u16   const u16
#define c_i16   const i16
#define c_u32   const u32
#define c_i32   const i32
#define c_u64   const u64
#define c_i64   const i64


// mi256_8
// simde_mm256_movemask_epi8
// simde_mm512_movepi8_mask


#define   i8_POW_i8   0
#define  i16_POW_i8   1
#define  i32_POW_i8   2
#define  i64_POW_i8   3
#define i128_POW_i8   4
#define i256_POW_i8   5
#define i512_POW_i8   6
#define  i16_POW_i16  0
#define  i32_POW_i16  1
#define  i64_POW_i16  2
#define i128_POW_i16  3
#define i256_POW_i16  4
#define i512_POW_i16  5
#define  i32_POW_i32  0
#define  i64_POW_i32  1
#define i128_POW_i32  2
#define i256_POW_i32  3
#define i512_POW_i32  4
#define  i64_POW_i64  0
#define i128_POW_i64  1
#define i256_POW_i64  2
#define i512_POW_i64  3
#define i128_POW_i128 0
#define i256_POW_i128 1
#define i512_POW_i128 2

#define LOG2_2     1
#define LOG2_4     2
#define LOG2_8     3
#define LOG2_16    4
#define LOG2_32    5
#define LOG2_64    6
#define LOG2_128   7
#define LOG2_256   8
#define LOG2_512   9
#define LOG2_1024 10
#define LOG2_2048 11

// #define floor256(x) (x >> 5)
// #define floor512(x) (x >> 9)

// typedef struct mi512 mi512;
// struct mi512 {
//     mi256 lo;
//     mi256 hi;
// };
// typedef struct c_mi512 c_mi512;
// struct c_mi512 {
//     c_mi256 lo;
//     c_mi256 hi;
// };

// mi512 mm512_loadu(c_mi512 vec) {
    
// }

// #if defined(__AVX2__)
//     puts("__AVX2__");
// #endif

#endif
