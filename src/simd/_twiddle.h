#if !defined(SIMD_TWIDDLE_H)
#define SIMD_TWIDDLE_H


/*

Articles
* Advanced bit manipulation-fu http://realtimecollisiondetection.net/blog/?p=78
* Branchless selections https://realtimecollisiondetection.net/blog/?p=90
* multiplication tricks https://realtimecollisiondetection.net/blog/?p=21
* old-school division trick https://realtimecollisiondetection.net/blog/?p=22
* abs, max, min https://realtimecollisiondetection.net/blog/?p=49

Books
* Hacker's Delight https://www.amazon.com/exec/obidos/tg/detail/-/0201914654?tag=realtimecolli-20
* Reading list http://realtimecollisiondetection.net/books/list/

Branchless selections

    rt = rc ? rb : ra

    rt = (rb & rc) | (ra & ~rc)

    rt = ((ra ^ rb) & rc) ^ ra      

Advanced bit manipulation-fu

          x         00101100	original value
         ~x         11010011	complement of x
         -x         11010100	= ~x+1, negation of x (this you MUST know about 2’s-complement arithmetic!)
     x & -x	        00000100	extract lowest bit set
     x | -x	        11111100	mask for bits above (AND including) lowest bit set
     x ^ -x	        11111000	mask for bits above (NOT including) lowest bit set
     x & (x – 1)	00101000	strip off lowest bit set
     x | (x – 1)	00101111	fill in all bits below lowest bit set
     x ^ (x – 1)	00000111	= ~x ^ -x, mask for bits below (AND including) lowest bit set
    ~x & (x – 1)	00000011	= ~(x | -x) = (x & -x) – 1, mask for bits below (NOT including) lowest bit set
     x | (x + 1)    00101101	fills in lowest zero bit
     x / (x & -x)	00001011	shift number right so lowest bit set ends up at bit 0
     
*/


// #define b_ternary(a,b,c) (((a ^ b) & c) ^ a)
// branchless selection https://realtimecollisiondetection.net/blog/?p=90

#define b_clear_leftmost_set(x) (x & (x - 1))

/*
https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
https://stackoverflow.com/questions/671815/what-is-the-fastest-most-efficient-way-to-find-the-highest-set-bit-msb-in-an-i

https://opensource.apple.com/source/clang/clang-800.0.38/src/tools/clang/lib/Headers/


_mm_prefetch


#define _mm256_set1_bool8(x) _mm256_set1_epi8( uint8_t(-(!!x) )

__m256i all_ones(void) { return _mm256_set1_epi64x(-1); }
*/

// https://github.com/fredrikwidlund/hash-function-benchmark/blob/1e1daf30d61105e52c3eca9052188c4b3da149fe/support/clhash.c
// static inline mi128 mm128_lshift(mi128 vec, int n) {    
//     mi128 u64shift =  simde_mm_slli_epi64(vec, n);
//     mi128 topbits =  simde_mm_slli_si128(
//         simde_mm_srli_epi64(vec, 64 - n), 
//         sizeof(u64)
//     );
//     return simde_mm_or_si128(u64shift, topbits);
// }
// // computes a << 2
// static inline __m128i leftshift2(__m128i a) {
//     const int x = 2;
//     __m128i u64shift =  _mm_slli_epi64(a,x);
//     __m128i topbits =  _mm_slli_si128(_mm_srli_epi64(a,64 - x),sizeof(uint64_t));
//     return _mm_or_si128(u64shift, topbits);
// }

#define b64_lo1s(n) ((1ULL << n) - 1)
#define b32_lo1s(n) ((1    << n) - 1)

#define b64_clear_lo1 _blsr_u64
#define b32_clear_lo1 _blsr_u32

#define u64_clear_lowest_bit _blsr_u64
#define u32_clear_lowest_bit _blsr_u32


// count 1s    
#define b64_count_1s _mm_popcnt_u64 
//                  _popcnt64
#define u32_count_1s _mm_popcnt_u32
#define u16_count_1s(x) u32_count_1s((x & 0xFFFF))
#define  u8_count_1s(x) u32_count_1s((x & 0xFF))
// int popcount256(const uint64_t* u){ 
//     return _mm_popcnt_u64(u[0]);
//          + _mm_popcnt_u64(u[1]);
//          + _mm_popcnt_u64(u[2]);
//          + _mm_popcnt_u64(u[3]);
// }

// clear lowest


// trailing zeros
                
#define u64_ctz(x) __builtin_ctzll(x)
#define u32_ctz(x) __builtin_ctz(x)
#define u16_ctz(x) (x ? u32_ctz((x)) : 16)
#define  u8_ctz(x) (x ? u32_ctz((x)) : 8)

#define u64_trailing_0s(x) u64_ctz((x))
#define u32_trailing_0s(x) u32_ctz((x))
#define u16_trailing_0s(x) u16_ctz((x))
#define  u8_trailing_0s(x)  u8_ctz((x))

/* 
leading zeroes
        * __builtin_clzl(0) is undefined, so we need to special-case for it.
*/
                // __builtin_clzl
#define u64_clz(x) __builtin_clzll(x)
#define u32_clz(x) __builtin_clz(x)
#define u16_clz(x) (u32_clz((x)) - 16)
#define  u8_clz(x) (u32_clz((x)) - 24)

#define u64_leading_0s(x) u64_clz((x))
#define u32_leading_0s(x) u32_clz((x))
#define u16_leading_0s(x) u16_clz((x))
#define  u8_leading_0s(x)  u8_clz((x))

// leading ones
#define b64_cl1(x) (  u64_clz( ~x ) )
#define u32_cl1(x) ( (u32_clz((x ^ ((1<<32)-1)) )) )
#define u16_cl1(x) ( (u32_clz((x ^ 65535))) - 16 )
#define  u8_cl1(x) ( (u32_clz((x ^ 255) )) - 24 )

#define u32_leading_1s(x) u32_cl1((x))
#define u16_leading_1s(x) u16_cl1((x))
#define  u8_leading_1s(x)  u8_cl1((x))

// trailing ones
// #define u64_ct1(x) ( (u64_ctz( (x ^ ((1LL<<64LL)-1LL)) )) )
#define u32_ct1(x) ( (u32_ctz((x ^ ((1<<32)-1)) )) )
#define u16_ct1(x) ( (u32_ctz((x ^ 65535) ^ 65536)) )
#define  u8_ct1(x) ( (u32_ctz((x ^ 255) ^256 )) )

#define u32_trailing_1s(x) u32_ct1((x))
#define u16_trailing_1s(x) u16_ct1((x))
#define  u8_trailing_1s(x)  u8_ct1((x))



// __builtin_ctz takes arguments of type unsigned int, which is 32-bits on most platforms.


// TODO: If long is 64 bits, you can use __builtin_ctzl which takes unsigned long. Or you can use __builtin_ctzll which takes unsigned long long - In this case you should use 1ULL << i instead of 1UL << i.

/*

    * https://jameshfisher.com/2018/03/30/round-up-power-2/

*/
#define b64_round_up_pow(x) ( x==1?1: 1 << (64 - u64_clz(x - 1)) )
#define b32_round_up_pow(x) ( x==1?1: 1 << (32 - u32_clz(x - 1)) )

#define b_round_multof_ispow2(x, N) ((x + N - 1) & ~(N - 1))
// rounding up to the nearest multiple of N (where N is a power of 2) 
// https://fgiesen.wordpress.com/2016/10/26/rounding-up-to-the-nearest-int-k-mod-n/


#endif