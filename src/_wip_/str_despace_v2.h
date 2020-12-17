




#define str_despace despace_sse4_branchless

// despace_avx_tableless


#define _LOG_LUT_ROW(x) printf("    0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", *(x+0), *(x+1),*(x+2), *(x+3),*(x+4), *(x+5),*(x+6), *(x+7),*(x+8), *(x+9),*(x+10), *(x+11),*(x+12), *(x+13),*(x+14), *(x+15));
#define LOG_LUT_ROW(x) _LOG_LUT_ROW((x))

// const mi128 i128_spaces = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}; 
// const mi128 mi128_zeroes = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

// UGH 
// shuffle is fucked in avx2 w/ 256: https://stackoverflow.com/questions/46582438/unexpected-mm256-shuffle-epi-with-256i-vectors/46602049#46602049

// 

mi256 get_mask4(int m) {
    puts("get_mask3");
    const  mi256 vshift_count = simde_mm256_set_epi32(24, 25, 26, 27, 28, 29, 30, 31);
     mi256 bcast = simde_mm256_set1_epi32(m);
     mi256 shifted = simde_mm256_sllv_epi32(bcast, vshift_count);  // high bit of each element = corresponding bit of the mask
    return shifted;
    // use _mm256_and and _mm256_cmpeq if you need all bits set.
    //return _mm256_srai_epi32(shifted, 31);             // broadcast the sign bit to the whole element
}
// mi256 get_mask3(const uint32_t mask) {
//     puts("get_mask3_4");
//     // https://stackoverflow.com/questions/21622212/how-to-perform-the-inverse-of-mm256-movemask-epi8-vpmovmskb
//     // https://stackoverflow.com/questions/36488675/is-there-an-inverse-instruction-to-the-movemask-instruction-in-intel-avx2/36491672#36491672
//     mi256 vmask = (simde_mm256_set1_epi32(mask));
//     const mi256 shuffle = (simde_mm256_setr_epi64x(0x0000000000000000, 0x0101010101010101, 0x0202020202020202, 0x0303030303030303));
//     vmask = simde_mm256_shuffle_epi8(vmask, shuffle);
//     const  mi256 bit_mask = (simde_mm256_set1_epi64x(0x7fbfdfeff7fbfdfe));
//     vmask = simde_mm256_or_si256(vmask, bit_mask);
//     return mmstr_eq(vmask, simde_mm256_set1_epi64x(-1));
// }

static inline size_t despace_avx_tableless(char *buf, size_t len) {
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

    // uint32_t mask = simde_mm256_cmpge_epi8_mask(spaces, x);
        
    x = simde_mm256_shuffle_epi8(x, get_mask4(mask & 0x7fff));
    simde_mm256_store_si256((mi256 *)(buf + pos), x);
    size_t delta = _mm_popcnt_u32(mask);
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





    // puts("");
    // printf("sizeof mi128 = %iB %ib\n",sizeof(mi128),sizeof(mi128)*8);
    // printf("sizeof despace_mask16 = %iB %ib\n",sizeof(despace_mask16),sizeof(despace_mask16)*8);
    // mi128* block0 = (const mi128 *)(despace_mask16);
    // mi128* block1 = (const mi128 *)(despace_mask16 +4 );
    // printf("ptr: block1 - block0 = %llu : %llu %llu\n",(uint64_t)block1 - (uint64_t)block0,(uint64_t)block1,(uint64_t)block0);

    // #define _LOG_LUT_ROW(x) printf(\
    // "    0x%x, 0x%x, 0x%x, 0x%x,  0x%x, 0x%x, 0x%x, 0x%x,  0x%x, 0x%x, 0x%x, 0x%x,  0x%x, 0x%x, 0x%x, 0x%x,\n",\
    //     *(x+0),*(x+1),*(x+2),*(x+3),    *(x+4),*(x+5),*(x+6),*(x+7),    *(x+8),*(x+9),*(x+10),*(x+11),      *(x+12),*(x+13),*(x+14),*(x+15));
    // #define LOG_LUT_ROW(x) _LOG_LUT_ROW((x))
    // u8* tbl = (u8*)despace_mask16;

    // printf("\n sizeof ==== %u \n\n",(u64)((mi128*)tbl + 1) - (u64)(tbl));

    // LOG_LUT_ROW(tbl+0);             LOG_LUT_ROW(tbl+(16 & 0x7fff));
    // LOG_LUT_ROW(tbl+(32 & 0x7fff)); LOG_LUT_ROW(tbl+(48 & 0x7fff));
    // puts("");
    // LOG_LUT_ROW(tbl+(64 & 0x7fff)); LOG_LUT_ROW(tbl+(80 & 0x7fff));
    // LOG_LUT_ROW(tbl+(96 & 0x7fff)); LOG_LUT_ROW(tbl+(112 & 0x7fff));

    // LOG_LUT_ROW(tbl+(128 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(256 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(512 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(1024 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(2048 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(4096 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(8192 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(16384 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(32768 & 0x7fff));    
    // LOG_LUT_ROW(tbl+(65536 & 0x7fff));    