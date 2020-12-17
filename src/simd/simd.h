#if !defined(SIMD_AVX_H)
#define SIMD_AVX_H

#include "./simde/simde/x86/avx.h"
#include "./simde/simde/x86/avx2.h"

#include "simde/x86/avx512bw.h"
#include "simde/x86/avx512cd.h"
#include "simde/x86/avx512dq.h"
#include "simde/x86/avx512f.h"
#include "simde/x86/avx512vl.h"

/*

## Naming Conventions

SIMD has spawned a thicket of architecture specific acronym arithmetic.
With an eye on typographic elegance over camel-cased specifity,
here is an opinionated attempt to normalize & reduce cross-platform overhead.

Most types & APIs are prefixed with a terse representation of:

    1. A SIMD vector type's bitlength (vec_lencode) (v,vv,wv,ww)
    2. containing element's of pow2 bits (elem_bitsize) (8,16,32,64...)

Hereout, "vec" & "elem" are used instead of "vector" & "element" to help disambiguate from adjacent std/libs/concepts.

vec_lencode = encoded vec type bitlength
--------------------------------
  v =  64b "vec"
 vv = 128b "dub-vec"
 wv = 256b "wide-vec"
 ww = 512b "dub-wide-vec"

{ vec_lencode | elem_bitsize } = elem_bitsize x elem_count
------------------------------------------------------------------------
  v8 = 8b x 8      v16 = 16b x 4      v32 = 32b x 2         -             -      -
 vv8 = 8b x 16    vv16 = 16b x 8     vv32 = 32b x 4    vv64 = 64b x 2     -      -
 wv8 = 8b x 32    wv16 = 16b x 16    wv32 = 32b x 8    wv64 = 64b x 4    ...     -
 ww8 = 8b x 64    ww16 = 16b x 32    ww32 = 32b x 16   ww64 = 64b x 8    ...    ...

*/

// AVX512F lacks of operations on bytes, the smallest vector item is a 32-bit word. The limitation forces us to use SWAR techniques.
#define _mm512_set1_epu8(c) simde_mm512_set1_epi32(uint32_t(c) * 0x01010101u)
#define simde_mm512_set1_epu8(c) _mm512_set1_epu8((c))

typedef simde__m128i  vvi_t;
typedef simde__m256i  wvi_t;
typedef simde__m512i  wwi_t;

#define c_vvi_t const vvi_t
#define c_wvi_t const wvi_t
#define c_wwi_t const wwi_t

// mask types

#define   v8_m_t    u8
#define   vv8_m_t   u16
#define   wv8_m_t   u32
#define   ww8_m_t   u64

#define   v16_m_t   u8
#define   vv16_m_t  u8
#define   wv16_m_t  u16
#define   ww16_m_t  u32

#define   vv32_m_t  u8
#define   wv32_m_t  u8
#define   ww32_m_t  u16

#define   wv64_m_t  u8
#define   ww64_m_t  u8

#define c_v8_m_t    const v8_m_t
#define c_vv8_m_t   const vv8_m_t
#define c_wv8_m_t   const wv8_m_t
#define c_ww8_m_t   const ww8_m_t

#define c_v16_m_t   const v16_m_t
#define c_vv16_m_t  const vv16_m_t
#define c_wv16_m_t  const wv16_m_t
#define c_ww16_m_t  const ww16_m_t

#define c_vv32_m_t  const vv32_m_t
#define c_wv32_m_t  const wv32_m_t
#define c_ww32_m_t  const ww32_m_t

#define c_wv64_m_t  const wv64_m_t
#define c_ww64_m_t  const ww64_m_t


// __m256i _mm256_undefined_si256 (void)

#define     wvi_loadu       simde_mm256_loadu_si256
#define     wwi_loadu       simde_mm512_loadu_si512

// __m256i _mm256_maskload_epi32 (int const* mem_addr, __m256i mask)
// __m256i _mm256_maskload_epi64 (__int64 const* mem_addr, __m256i mask)
// __m256i _mm256_loadu2_m128i (__m128i const* hiaddr, __m128i const* loaddr)

#define     wvi8_get        simde_mm256_extract_epi8
#define     wvi16_get       simde_mm256_extract_epi16
#define     wvi32_get       simde_mm256_extract_epi32
#define     wvi64_get       simde_mm256_extract_epi64

#define     wvi8_set        simde_mm256_insert_epi8
#define     wvi16_set       simde_mm256_insert_epi16
#define     wvi32_set       simde_mm256_insert_epi32
#define     wvi64_set       simde_mm256_insert_epi64

// __m256i _mm256_set_epi8
// __m256i _mm256_set_epi16
// __m256i _mm256_set_epi32 (int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0)
// __m256i _mm256_set_epi64x (__int64 e3, __int64 e2, __int64 e1, __int64 e0)
// __m256i _mm256_set_m128i (__m128i hi, __m128i lo)

// reverse order
// __m256i _mm256_setr_epi8
// __m256i _mm256_setr_epi16
// __m256i _mm256_setr_epi32 (int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0)
// __m256i _mm256_setr_epi64x (__int64 e3, __int64 e2, __int64 e1, __int64 e0)
// __m256i _mm256_setr_m128i (__m128i lo, __m128i hi)


// el fill

#define     wvi8_fill       simde_mm256_set1_epi8
#define     wwi8_fill       simde_mm512_set1_epi8
#define     wvi16_fill      simde_mm256_set1_epi16
#define     wwi16_fill      simde_mm512_set1_epi16
#define     wvi32_fill      simde_mm256_set1_epi32
#define     wwi32_fill      simde_mm512_set1_epi32
#define     wvi64_fill      simde_mm256_set1_epi64x
#define     wwi64_fill      simde_mm512_set1_epi64x

// __m512i _mm512_broadcast_i64x4 (__m256i a)

// vpacksswb
// __m256i _mm256_or_si256 (__m256i a, __m256i b)
// __m256i _mm256_packs_epi16 (__m256i a, __m256i b)
// __m256i _mm256_packs_epi32 (__m256i a, __m256i b)
// __m256i _mm256_packus_epi16 (__m256i a, __m256i b)
// __m256i _mm256_packus_epi32 (__m256i a, __m256i b)


// mix(__m256i a, __m256i b, __m256i mask)

#define     wvi8_mix        simde_mm256_blendv_epi8
/* TODO
            128 bit lates???
//          (a, b, int imm8)
#define     wv_mix_i16       simde_mm256_blend_epi16
#define     wv_mix_i32       simde_mm256_blend_epi32
#define     wv_mix_f32       simde_mm256_blend_ps
#define     wv_mix_f64       simde_mm256_blend_pd
*/


// vec extend

#define     wvi16_extend_vvu8 simde_mm256_cvtepu8_epi16
#define     wvi16_extend_vvi8 simde_mm256_cvtepi8_epi16
#define     wvi32_extend_vvu8 simde_mm256_cvtepu8_epi32
#define     wvi32_extend_vvi8 simde_mm256_cvtepi8_epi32
#define     wvi64_extend_vvi8 simde_mm256_cvtepi8_epi64
#define     wvi64_extend_vvu8 simde_mm256_cvtepu8_epi64
#define     wvi32_extend_vvu16 simde_mm256_cvtepu16_epi32
#define     wvi32_extend_vvi16 simde_mm256_cvtepi16_epi32
#define     wvi64_extend_vvu16 simde_mm256_cvtepu16_epi64
#define     wvi64_extend_vvi16 simde_mm256_cvtepi16_epi64
#define     wvi64_extend_vvu32 simde_mm256_cvtepu32_epi64
#define     wvi64_extend_vvi32 simde_mm256_cvtepi32_epi64

#define     wvf64_extend_vvi32 simde_mm256_cvtepi32_pd


// vec logic

#define     wvi_and           simde_mm256_and_si256
#define     wvi_andnot        simde_mm256_andnot_si256
#define     wvi_or            simde_mm256_or_si256
#define     wvi_xor           simde_mm256_xor_si25


// el compare

#define     wvi8_eq         simde_mm256_cmpeq_epi8
// #define     wwi8_eq         simde_mm256_cmpeq_epi8

#define     wvi16_eq        simde_mm256_cmpeq_epi16
#define     wvi32_eq        simde_mm256_cmpeq_epi32
#define     wvi64_eq        simde_mm256_cmpeq_epi64
#define     wvi8_gt         simde_mm256_cmpgt_epi8
#define     wvi16_gt        simde_mm256_cmpgt_epi16
#define     wvi32_gt        simde_mm256_cmpgt_epi32
#define     wvi64_gt        simde_mm256_cmpgt_epi64


// el aritmetic

#define     wvi8_add        simde_mm256_add_epi8
#define     wvi16_add       simde_mm256_add_epi16
#define     wvi32_add       simde_mm256_add_epi32
#define     wvi64_add       simde_mm256_add_epi64
#define     wvu8_adds       simde_mm256_adds_epu8
#define     wvi8_adds       simde_mm256_adds_epi8
#define     wvu16_adds      simde_mm256_adds_epu16
#define     wvi16_adds      simde_mm256_adds_epi16
// #define  wvi32_adds      simde_mm256_adds_epi32
// #define  wvi64_adds      simde_mm256_adds_epi64

#define     wvi8_sub        simde_mm256_sub_epi8
#define     wvi16_sub       simde_mm256_sub_epi16
#define     wvi32_sub       simde_mm256_sub_epi32
#define     wvi64_sub       simde_mm256_sub_epi64
#define     wvu8_subs       simde_mm256_subs_epu8
#define     wvi8_subs       simde_mm256_subs_epi8
#define     wvu16_subs      simde_mm256_subs_epu16
#define     wvi16_subs      simde_mm256_subs_epi16
// #define  wvi32_subs      simde_mm256_subs_epi32
// #define  wvi64_subs      simde_mm256_subs_epi64


// el op

#define     wvu8_max        simde_mm256_max_epu8
#define     wvi8_max        simde_mm256_max_epi8
#define     wvu16_max       simde_mm256_max_epu16
#define     wvi16_max       simde_mm256_max_epi16
#define     wvu32_max       simde_mm256_max_epu32
#define     wvi32_max       simde_mm256_max_epi32
// #define  wvi64_max       simde_mm256_max_epi64
#define     wvf32_max       simde_mm256_max_ps
#define     wvf64_max       simde_mm256_max_pd

#define     wvu8_min        simde_mm256_min_epu8
#define     wvi8_min        simde_mm256_min_epi8
#define     wvu16_min       simde_mm256_min_epu16
#define     wvi16_min       simde_mm256_min_epi16
#define     wvu32_min       simde_mm256_min_epu32
#define     wvi32_min       simde_mm256_min_epi32
// #define  wvi64_min       simde_mm256_min_epi64
#define     wvf32_min       simde_mm256_min_ps
#define     wvf64_min       simde_mm256_min_pd

#define     wvu8_avg        simde_mm256_avg_epu8
#define     wvu16_avg       simde_mm256_avg_epu16


// vec to mask

#define     wvi8_mask       simde_mm256_movemask_epi8
#define     wwi8_mask       simde_mm512_movepi8_mask

#define     wvf32_mask      simde_mm256_movemask_ps
#define     wvf64_mask      simde_mm256_movemask_pd

#define     wvi8_eq_mask(va, vb)    (  (u32)wvi8_mask( (c_wvi_t)wvi8_eq(vb, va) ) )
#define     wvi8_lte_mask(va, vb)   ( ~(u32)wvi8_mask( (c_wvi_t)wvi8_gt(va, vb) ) )

// TODO:    contrib: #define     wwu8_eq_mask(va, vb)    ((u64)simde_mm512_cmpeq_epu8_mask(va, vb))
#define     wwi8_eq_mask(va, vb)    ((u64)simde_mm512_cmpeq_epi8_mask(va, vb))
#define     wwu8_lte_mask(va, vb)   ((u64)simde_mm512_cmple_epu8_mask(va, vb))
#define     wwi8_lte_mask(va, vb)   ((u64)simde_mm512_cmple_epi8_mask(va, vb))

// TODO:    contrib: __mmask64 _mm512_mask_cmpeq_epi8_mask (__mmask64 k1, __m512i a, __m512i b)
//          contrib: #define     wwu8_mask_eq_mask(m, va, vb)    ((u64)(m&simde_mm512_cmpeq_epu8_mask(va, vb)))
#define     wwi8_mask_eq_mask(m, va, vb)    ((u64)(m&simde_mm512_cmpeq_epi8_mask(va, vb)))
#define     wwu8_mask_lte_mask(m, va, vb)   ((u64)(m&simde_mm512_cmple_epu8_mask(va, vb)))
#define     wwi8_mask_lte_mask(m, va, vb)   ((u64)(m&simde_mm512_cmple_epi8_mask(va, vb)))



#define     wvi8_and_mask(va, vb)    (  (u32)wvi8_mask( (c_wvi_t)wvi_and(vb, va) ) )







#endif