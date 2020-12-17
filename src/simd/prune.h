#include "./_.h"
#include "./prune.luts.h"

    // Copies to 'output" all bytes corresponding to a 0 in the mask (interpreted as a bitset).
    // Passing a 0 value for mask would be equivalent to writing out every byte to output.
    // Only the first 32 - count_ones(mask) bytes of the result are significant but 32 bytes
    // get written.
    // Design consideration: it seems like a function with the
    // signature simd8<L> compress(uint32_t mask) would be
    // sensible, but the AVX ISA makes this kind of approach difficult.
    
static FORCE_INLINE u32 // => number of bytes moved
mm256_prune( wvi_t vec, u32 mask, u8* output ) {
    if_UNLIKELY( mask == 0 ) {      
        // NOTE: unaligned          
        simde_mm256_storeu_si256((wvi_t *)output, vec);
        return 32;
    }

    u32 nignored = u32_count_1s(mask);  
    // printf("nignored %i\n",nignored);  

    if_UNLIKELY( nignored == 32 ) 
        return 0;    

    // this particular implementation was inspired by work done by @animetosho
    // we do it in four steps, first 8 bytes and then second 8 bytes...
    u8 mask1 = (u8)(mask); // least significant 8 bits
    u8 mask2 = (u8)(mask >> 8); // second least significant 8 bits
    u8 mask3 = (u8)(mask >> 16); // ...
    u8 mask4 = (u8)(mask >> 24); // ...

    // next line just loads the 64-bit values thintable_epi8[mask1] and
    // thintable_epi8[mask2] into a 128-bit register, using only
    // two instructions on most compilers.
    wvi_t shufmask = simde_mm256_set_epi64x(
        thintable_epi8[mask4], thintable_epi8[mask3], 
        thintable_epi8[mask2], thintable_epi8[mask1]
    );
    
    // we increment by 0x08 the second half of the mask and so forth
    shufmask = simde_mm256_add_epi8(
        shufmask, 
        simde_mm256_set_epi32(  0x18181818, 0x18181818, 
                                0x10101010, 0x10101010, 
                                0x08080808, 0x08080808, 
                                0,          0)         );
    // "nearly pruned" ...
    wvi_t pruned = simde_mm256_shuffle_epi8( vec, shufmask );

    // we still need to put the pieces back together.
    // we compute the popcount of the first words:
    int pop1 = BitsSetTable256mul2[mask1];
    int pop3 = BitsSetTable256mul2[mask3];

    // then load the corresponding mask
    // could be done with _mm256_loadu2_m128i but many standard libraries omit this intrinsic.
    wvi_t v256 = simde_mm256_castsi128_si256(
        simde_mm_loadu_si128((c_vvi_t *)(pshufb_combine_table + pop1 * 8))
    );
    wvi_t compactmask = simde_mm256_insertf128_si256(
        v256,
        simde_mm_loadu_si128((c_vvi_t *)(pshufb_combine_table + pop3 * 8)), 
        1
    );
    wvi_t almostthere = simde_mm256_shuffle_epi8(
        pruned, compactmask
    );

    // We just need to write out the result.
    // This is the tricky bit that is hard to do
    // if we want to return a SIMD register, since there
    // is no single-instruction approach to recombine
    // the two 128-bit lanes with an offset.
    vvi_t v128;
    v128 = simde_mm256_castsi256_si128(almostthere);
    simde_mm_storeu_si128( 
        (c_vvi_t *)output, v128
    );
    v128 = simde_mm256_extractf128_si256(
        almostthere, 1
    );
    simde_mm_storeu_si128( 
        (c_vvi_t *)(output + 16 - u16_count_1s(mask)), 
        v128
    );    
    return 32 - nignored;
}


    // Copies to 'output" all bytes corresponding to a 0 in the mask (interpreted as a bitset).
    // Passing a 0 value for mask would be equivalent to writing out every byte to output.
    // Only the first 16 - count_ones(mask) bytes of the result are significant but 16 bytes
    // get written.
    // Design consideration: it seems like a function with the
    // signature simd8<L> compress(uint16_t mask) would be
    // sensible, but the AVX ISA makes this kind of approach difficult.
    
// inline void mm16_compress(uint16_t mask, L * output) const {
//     // this particular implementation was inspired by work done by @animetosho
//     // we do it in two steps, first 8 bytes and then second 8 bytes
//     uint8_t mask1 = uint8_t(mask); // least significant 8 bits
//     uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
//     // next line just loads the 64-bit values thintable_epi8[mask1] and
//     // thintable_epi8[mask2] into a 128-bit register, using only
//     // two instructions on most compilers.
//     uint64x2_t shufmask64 = {thintable_epi8[mask1], thintable_epi8[mask2]};
//     uint8x16_t shufmask = vreinterpretq_u8_u64(shufmask64);
//     // we increment by 0x08 the second half of the mask
// #ifdef SIMDJSON_REGULAR_VISUAL_STUDIO
//     uint8x16_t inc = make_uint8x16_t(0, 0, 0, 0, 0, 0, 0, 0, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08);
// #else
//     uint8x16_t inc = {0, 0, 0, 0, 0, 0, 0, 0, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08};
// #endif
//     shufmask = vaddq_u8(shufmask, inc);
//     // this is the version "nearly pruned"
//     uint8x16_t pruned = vqtbl1q_u8(*this, shufmask);
//     // we still need to put the two halves together.
//     // we compute the popcount of the first half:
//     int pop1 = BitsSetTable256mul2[mask1];
//     // then load the corresponding mask, what it does is to write
//     // only the first pop1 bytes from the first 8 bytes, and then
//     // it fills in with the bytes from the second 8 bytes + some filling
//     // at the end.
//     uint8x16_t compactmask = vld1q_u8((const uint8_t *)(pshufb_combine_table + pop1 * 8));
//     uint8x16_t answer = vqtbl1q_u8(pruned, compactmask);
//     vst1q_u8((uint8_t*) output, answer);
// }

// Copies to 'output" all bytes corresponding to a 0 in the mask (interpreted as a bitset).
// Passing a 0 value for mask would be equivalent to writing out every byte to output.
// Only the first 16 - count_ones(mask) bytes of the result are significant but 16 bytes
// get written.
// Design consideration: it seems like a function with the
// signature simd8<L> compress(uint32_t mask) would be
// sensible, but the AVX ISA makes this kind of approach difficult.

// really_inline void compress(uint16_t mask, L * output) const {
//       // this particular implementation was inspired by work done by @animetosho
//       // we do it in two steps, first 8 bytes and then second 8 bytes
//       uint8_t mask1 = uint8_t(mask); // least significant 8 bits
//       uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
//       // next line just loads the 64-bit values thintable_epi8[mask1] and
//       // thintable_epi8[mask2] into a 128-bit register, using only
//       // two instructions on most compilers.
//       __m128i shufmask =  _mm_set_epi64x(thintable_epi8[mask2], thintable_epi8[mask1]);
//       // we increment by 0x08 the second half of the mask
//       shufmask =
//       _mm_add_epi8(shufmask, _mm_set_epi32(0x08080808, 0x08080808, 0, 0));
//       // this is the version "nearly pruned"
//       __m128i pruned = _mm_shuffle_epi8(*this, shufmask);
//       // we still need to put the two halves together.
//       // we compute the popcount of the first half:
//       int pop1 = BitsSetTable256mul2[mask1];
//       // then load the corresponding mask, what it does is to write
//       // only the first pop1 bytes from the first 8 bytes, and then
//       // it fills in with the bytes from the second 8 bytes + some filling
//       // at the end.
//       __m128i compactmask =
//       _mm_loadu_si128((const __m128i *)(pshufb_combine_table + pop1 * 8));
//       __m128i answer = _mm_shuffle_epi8(pruned, compactmask);
//       _mm_storeu_si128(( __m128i *)(output), answer);
// }

