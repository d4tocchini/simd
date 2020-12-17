#if !defined(SIMD_SCAN_H)
#define SIMD_SCAN_H

#include "./_.h"

/*

parse comma separated string via sscanf
    Example program showing how to parse comma seperated strings.
    I've seen many times where people write their own (usually
    flawed) code to parse strings like this.
    #include <stdio.h>

    int main(void) {
        char str1[2];
        char str2[2];
        char str3[2];
        char str4[2];
        char* t="1,2,3,4";
        sscanf(t, "%[^,],%[^,],%[^,],%[^,]", str1, str2, str3, str4);
        printf("str1=[%s], str2=[%s], str3=[%s], str4=[%s]\n", str1, str2, str3, str4);
    }

*/

struct mmscan_t {    
    u64 temp;    
    u64 mask;
    u64 head;
    u64 tail;

    u32 len;
    u32 idx;

    u32 meta;
    // u32 _pad1;    
    // u64 _pad2;
    // u64 _pad3;
};

#define MMSCAN_INIT(LEN)    \
    struct mmscan_t scan = {\
        .temp = 0,          \
        .mask = 0,          \
        .head = 0,          \
        .tail = 0,          \
                            \
        .len = LEN,         \
        .idx = 0,           \
                            \
        .meta = 0,          \        
    };

#define MMSCAN_LOOP()       \
    MMSCAN_CHECK_LAST()     \
    while (!MMSCAN_IS_LAST)

#define MMSCAN_LOAD() \
    c_wvi_t  vec_lo = wvi_loadu( buf + scan.idx ); \
    MMSCAN_STEP() \
    c_wvi_t  vec_hi = (MMSCAN_IS_LAST) ? MM256_0s : wvi_loadu( buf + scan.idx ); \
    MMSCAN_STEP()

#define MMSCAN_PRUNE_STORE(MASK, OUT, POS) \
    POS += mm256_prune(vec_lo, (u32)(MASK), (OUT)+(POS)); \
    POS += mm256_prune(vec_hi, (u32)((MASK)>>32), (OUT)+(POS));         

#define _MMSCAN_FLAG_SET(N, V) scan.meta = (scan.meta & ~(1<<N)) | ((bool)V<<N)
#define MMSCAN_FLAG_SET(N, V) _MMSCAN_FLAG_SET((N), (V))
#define MMSCAN_FLAG_BOOL(N) (bool)(scan.meta & (1 << N))    

#define MMSCAN_EQ_MASK(VEC_TGT) (\
    (u64)wvi8_eq_mask(vec_lo, VEC_TGT) | ((u64)wvi8_eq_mask(vec_hi, VEC_TGT)<<32) \
)
#define MMSCAN_LTE_MASK(VEC_TGT) (\
    (u64)wvi8_lte_mask(vec_lo, VEC_TGT) | ((u64)wvi8_lte_mask(vec_hi, VEC_TGT)<<32) \
)
#define MMSCAN_MASK_EQ_MASK(MASK, VEC_TGT) (MASK & MMSCAN_EQ_MASK(VEC_TGT))
#define MMSCAN_MASK_LTE_MASK(MASK, VEC_TGT) (MASK & MMSCAN_LTE_MASK(VEC_TGT))

#define MMSCAN_CHECK_LAST() \
    scan.meta |= (bool)(scan.idx >= scan.len);

#define MMSCAN_STEP() \
    scan.idx += 32; \
    MMSCAN_CHECK_LAST()

#define MMSCAN_IS_LAST \
    (scan.meta & 1)
    
#define MMSCAN_LAST_MASK \
    -(!MMSCAN_IS_LAST) | ((( (u64)((bool)( scan.len & ((MMSCAN_IS_LAST<<6)-MMSCAN_IS_LAST) )) ) << (scan.len & 63)) - MMSCAN_IS_LAST)

    // #define MMSCAN_FINAL_MASK \
    //     ((( (u64)((bool)(len & 63)) ) << (len & 63)) - 1ULL)
    // #define MMSCAN_FINALIZE_MASK() \
    //     scan.mask &= MMSCAN_FINAL_MASK | (scan.mask * !MMSCAN_IS_LAST);
                                   // | ( (scan.mask >> (MMSCAN_IS_LAST << 5) ) >> (MMSCAN_IS_LAST << 5));


#endif