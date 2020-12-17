

// source: https://gist.github.com/aqrit/6e73ca6ff52f72a2b121d584745f89f3
/*
* various methods to strip whitespace from text
* (aka. despace, leftpack, copy_if)
*
* 'whitespace' is considered the following bytes:
* 0x09 - tab
* 0x20 - space
* 0x0A - line feed
* 0x0D - carriage return
*/
/*
// 3x faster on unpredicatble data vs naive version
char* despacer_bitmap (const char* src, char* dst, size_t count)
{
    const uint64_t bitmap = UINT64_C(0xFFFFFFFEFFFFC1FF);
    if (count != 0) {
        do {
            uint64_t c = *(const unsigned char*)src;
            src++;
            *dst = (char)c;
            dst += ((bitmap >> c) & 1) | ((c + 0xC0) >> 8);
        } while (--count);
    }
    return dst;
}


// no lookup tables
// probably needs improvment...
char* despace_avx2_vpermd(const char* src_ptr, char* dst_ptr, size_t length )
{
	uint8_t* src = (uint8_t*)src_ptr;
	uint8_t* dst = (uint8_t*)dst_ptr;

	const  mi256 mask_20  = _mm256_set1_epi8( 0x20 );
	const  mi256 mask_70  = _mm256_set1_epi8( 0x70 );
	const  mi256 lut_cntrl = _mm256_setr_epi8(
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
		//
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00
	);

	const  mi256 permutation_mask = _mm256_set1_epi64x( 0x0020100884828180 );
	const  mi256 invert_mask = _mm256_set1_epi64x( 0x0020100880808080 ); 
	const  mi256 zero = _mm256_setzero_si256();
	const  mi256 fixup = _mm256_set_epi32(
		0x08080808, 0x0F0F0F0F, 0x00000000, 0x07070707,
		0x08080808, 0x0F0F0F0F, 0x00000000, 0x07070707
	);
	const  mi256 lut = _mm256_set_epi32(
		0x04050607, // 0x03020100', 0x000000'07
		0x04050704, // 0x030200'00, 0x0000'0704
		0x04060705, // 0x030100'00, 0x0000'0705
		0x04070504, // 0x0300'0000, 0x00'070504
		0x05060706, // 0x020100'00, 0x0000'0706
		0x05070604, // 0x0200'0000, 0x00'070604
		0x06070605, // 0x0100'0000, 0x00'070605
		0x07060504  // 0x00'000000, 0x'07060504
	);

	// hi bits are ignored by pshufb, used to reject movement of low qword bytes
	const  mi256 shuffle_a = _mm256_set_epi8(
		0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x07, 0x16, 0x25, 0x34, 0x43, 0x52, 0x61, 0x70,
		0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x07, 0x16, 0x25, 0x34, 0x43, 0x52, 0x61, 0x70
	);

	// broadcast 0x08 then blendd...
	const  mi256 shuffle_b = _mm256_set_epi32(
		0x08080808, 0x08080808, 0x00000000, 0x00000000,
		0x08080808, 0x08080808, 0x00000000, 0x00000000
	);

	for( uint8_t* end = &src[(length & ~31)]; src != end; src += 32){
		 mi256 r0,r1,r2,r3,r4;
		unsigned int s0,s1;

		r0 = _mm256_loadu_si256(( mi256 *)src); // asrc

		r1 = _mm256_adds_epu8(mask_70, r0);
		r2 = _mm256_cmpeq_epi8(mask_20, r0);

		r1 = _mm256_shuffle_epi8(lut_cntrl, r1);

		r1 = _mm256_or_si256(r1, r2); // bytemask of spaces

		r2 = _mm256_sad_epu8(zero, r1);
		s0 = _mm256_movemask_epi8(r1);
		r1 = _mm256_andnot_si256(r1, permutation_mask);

		r1 = _mm256_sad_epu8(r1, invert_mask); // index_bitmap[0:5], low32_spaces_count[7:15]

		r2 = _mm256_shuffle_epi8(r2, zero);

		r2 = _mm256_sub_epi8(shuffle_a, r2); // add space cnt of low qword
		s0 = ~s0;

		r3 = _mm256_slli_epi64(r1, 29); // move top part of index_bitmap to high dword
		r4 = _mm256_srli_epi64(r1, 7); // number of spaces in low dword 

		r4 = _mm256_shuffle_epi8(r4, shuffle_b);
		r1 = _mm256_or_si256(r1, r3);

		r1 = _mm256_permutevar8x32_epi32(lut, r1);
		s1 = _mm_popcnt_u32(s0);
		r4 = _mm256_add_epi8(r4, shuffle_a);
		s0 = s0 & 0xFFFF; // isolate low oword

		r2 = _mm256_shuffle_epi8(r4, r2);
		s0 = _mm_popcnt_u32(s0);

		r2 = _mm256_max_epu8(r2, r4); // pin low qword bytes

		r1 = _mm256_xor_si256(r1, fixup);

		r1 = _mm256_shuffle_epi8(r1, r2); // complete shuffle mask

		r0 = _mm256_shuffle_epi8(r0, r1); // despace!

		_mm_storeu_si128((__m128i*)dst, _mm256_castsi256_si128(r0));
		_mm_storeu_si128((__m128i*)&dst[s0], _mm256_extracti128_si256(r0,1));
		dst += s1;
	}
	return despacer_bitmap((const char*)src, (char*)dst, length & 31);
}
*/





/*

static inline 
mi128 cleanm128(mi128 x, mi128 spaces, mi128 newline,
                                mi128 carriage, int *mask16) {
  mi128 xspaces = simde_mm_cmpeq_epi8(x, spaces);
  mi128 xnewline = simde_mm_cmpeq_epi8(x, newline);
  mi128 xcarriage = simde_mm_cmpeq_epi8(x, carriage);
  mi128 anywhite = simde_mm_or_si128(_mm_or_si128(xspaces, xnewline), xcarriage);
  *mask16 = simde_mm_movemask_epi8(anywhite);
  return simde_mm_shuffle_epi8(
      x, simde_mm_loadu_si128((const mi128 *)despace_mask16 + (*mask16 & 0x7fff)));
}

static inline 
size_t despace_sse4_branchless_u4(char *buf, size_t len) {
  size_t pos = 0;
  mi128 spaces = simde_mm_set1_epi8(' ');
  mi128 newline = simde_mm_set1_epi8('\n');
  mi128 carriage = simde_mm_set1_epi8('\r');
  size_t i = 0;
  for (; i + 64 - 1 < len; i += 64) {
    mi128 x1 = simde_mm_loadu_si128((const mi128 *)(buf + i));
    mi128 x2 = simde_mm_loadu_si128((const mi128 *)(buf + i + 16));
    mi128 x3 = simde_mm_loadu_si128((const mi128 *)(buf + i + 32));
    mi128 x4 = simde_mm_loadu_si128((const mi128 *)(buf + i + 48));

    int mask16;
    x1 = cleanm128(x1, spaces, newline, carriage, &mask16);
    simde_mm_storeu_si128((mi128 *)(buf + pos), x1);
    pos += 16 - simde_mm_popcnt_u32(mask16);

    x2 = cleanm128(x2, spaces, newline, carriage, &mask16);
    simde_mm_storeu_si128((mi128 *)(buf + pos), x2);
    pos += 16 - simde_mm_popcnt_u32(mask16);

    x3 = cleanm128(x3, spaces, newline, carriage, &mask16);
    simde_mm_storeu_si128((mi128 *)(buf + pos), x3);
    pos += 16 - simde_mm_popcnt_u32(mask16);

    x4 = cleanm128(x4, spaces, newline, carriage, &mask16);
    simde_mm_storeu_si128((mi128 *)(buf + pos), x4);
    pos += 16 - simde_mm_popcnt_u32(mask16);
  }
  for (; i + 16 - 1 < len; i += 16) {
    mi128 x = simde_mm_loadu_si128((const mi128 *)(buf + i));
    int mask16;
    x = cleanm128(x, spaces, newline, carriage, &mask16);
    simde_mm_storeu_si128((mi128 *)(buf + pos), x);
    pos += 16 - simde_mm_popcnt_u32(mask16);
  }
  for (; i < len; i++) {
    char c = buf[i];
    if (c == '\r' || c == '\n' || c == ' ') {
      continue;
    }
    buf[pos++] = c;
  }
  return pos;
}
*/





/* http://0x80.pl/articles/simd-strfind.html#generic-avx512f

TODO:
#define __AVX512BW__ 1
#define __AVX512CD__ 1
#define __AVX512DQ__ 1
#define __AVX512F__ 1

AVX512F lacks of operations on bytes, the smallest vector item is a 32-bit word. The limitation forces us to use SWAR techniques.

1. Using AVX512F instructions we compare two vectors, like in SWAR version, i.e. two xors joined with bitwise or. There is only one difference, a single ternary logic instruction expresses one xor and bitwise or.
2. Using AVX512F instructions we locate which 32-bit elements contain any zero byte.
3. Then for such 32-bit element check four substrings for equality.

Unlike the SWAR procedure, where we need a precise mask for zero bytes, an AVX512F procedure requires just information "a word has zero byte". A simpler algorithm, described in Bit Twiddling Hacks is used; below is its C++ implementation.

size_t strfind(const char* string, size_t n, const char* needle, size_t k) {

    assert(n > 0);
    assert(k > 0);

    const __m512i first = simde_mm512_set1_epu8(needle[0]);
    const __m512i last  = simde_mm512_set1_epu8(needle[k - 1]);

    char* haystack = const_cast<char*>(string);
    char* end      = haystack + n;

    for (; haystack < end; haystack += 64) {

        const __m512i block_first = simde_mm512_loadu_si512(haystack + 0);
        const __m512i block_last  = simde_mm512_loadu_si512(haystack + k - 1);

        const __m512i first_zeros = simde_mm512_xor_si512(block_first, first);
        // zeros = first_zeros | (block_last ^ last)
        const __m512i zeros = simde_mm512_ternarylogic_epi32(first_zeros, block_last, last, 0xf6);

        uint32_t mask = zero_byte_mask(zeros);
        while (mask) {

            const uint64_t p = __builtin_ctz(mask);

            if (memcmp(haystack + 4*p + 0, needle, k) == 0) {
                return (haystack - string) + 4*p + 0;
            }

            if (memcmp(haystack + 4*p + 1, needle, k) == 0) {
                return (haystack - string) + 4*p + 1;
            }

            if (memcmp(haystack + 4*p + 2, needle, k) == 0) {
                return (haystack - string) + 4*p + 2;
            }

            if (memcmp(haystack + 4*p + 3, needle, k) == 0) {
                return (haystack - string) + 4*p + 3;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}

*/





static inline size_t despace_sse4_branchless(char *buf, size_t len) {
    const mi128 spaces = simde_mm_set1_epi8(' ');  
    size_t pos = 0;  
    size_t i = 0;  
    for (; i + 15 < len; i += 16) {
        mi128 x = simde_mm_loadu_si128((const mi128 *)(buf + i));
        mi128 anywhite = simde_mm_cmpeq_epi8(spaces, simde_mm_max_epu8(spaces, x));
        uint32_t mask16 = simde_mm_movemask_epi8(anywhite);    
        
        printf("mask\t%u,\tppc %u,\tclz %u,\tctz %u,\tcl1 %u,\tct1 %u,\n",
                mask16,_mm_popcnt_u32(mask16),u16_clz(mask16),u16_ctz(mask16),u16_cl1(mask16),u16_ct1(mask16));
        
        uint8_t popcnt = _mm_popcnt_u32(mask16);
        if (popcnt < 16) {
            x = simde_mm_shuffle_epi8(x, *((mi128 *)despace_mask16 + (mask16 & 0x7fff)));
            simde_mm_storeu_si128((mi128 *)(buf + pos), x);
        }
        pos += 16 - popcnt;
    }

    while (i < len) {
        const char c = buf[i++];
        if (c <= 32)
            continue;
        buf[pos++] = c;        
    }
    
    if (pos < len) 
        buf[pos] = 0;

    //   while (pos < len) 
    //       buf[pos++] = ' ';      

    return pos;
}


static inline size_t despace_avx_branchless(char *buf, size_t len) {
  size_t pos = 0;
  
//   mi256 spaces = simde_mm256_broadcastb_epi8(*(mi128 *)" "); // ??
    mi256 spaces = simde_mm256_set1_epi8(33);
//   size_t shift = 0;
  
  size_t i = 0;
  for (; i + 31 < len; i += 32) {
    mi256 x = simde_mm256_loadu_si256((const mi256 *)(buf + i));
    
    // mi256 anywhite = simde_mm256_cmpeq_epi8(spaces, simde_mm256_max_epu8(spaces, x));
    // uint32_t mask = simde_mm256_movemask_epi8(anywhite);
    
    mi256 anywhite = simde_mm256_cmpgt_epi8(spaces, x);
    uint32_t mask = simde_mm256_movemask_epi8(anywhite);    
    mi128 shuf[2] = {
        *((mi128 *)despace_mask16 + (mask & 0x7fff)),
        *((mi128 *)despace_mask16 + ((mask>>16) & 0x7fff))
    };    

    x = simde_mm256_shuffle_epi8(x, *((mi256 *)shuf) );
    simde_mm256_store_si256((mi256 *)(buf + pos), x);
    size_t delta = _mm_popcnt_u32(mask & ((1 << 16)-1)) + _mm_popcnt_u32(mask >> 16);
    
    // shift += delta;
    pos += 32 - delta;
  }

  for (; i < len; i++) {
    const char c = buf[i];
    buf[pos] = c;
    pos += ((unsigned char)c > 32 ? 1 : 0);
  }
  return pos;

}