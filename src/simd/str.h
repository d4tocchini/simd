#if !defined(SIMD_STR_H)
#define SIMD_STR_H

#include <string.h>
#include "./_.h"
#include "./simd.h"
#include "./prune.h"
#include "./scan.h"

/*

How to correctly use string buffers
    http://www.pixelbeat.org/programming/gcc/string_buffers.html


print format
    int types
    http://www.pixelbeat.org/programming/gcc/int_types/

    http://www.pixelbeat.org/programming/gcc/format_specs.html

    format	                                output
    printf("%08X",32_bit_var);	            0000ABCD
    printf("%lu",32_bit_var);	            43981
    printf("%'d",32_bit_var);	            43,981 // *
    printf("%10s","string");	            string
    printf("%*s",10,"string");	            string
    printf("%-10s","string");	            string    
    printf("%-10.10s","truncateiftoolong");	truncateif

    * Note for the POSIX %'d format, one must have already set your locale as in the following example:

    #include <locale.h>
    #include <stdio.h>

    int main(void)
    {
        setlocale(LC_ALL,"");
        // Note I is a GNU extension, and ' is a POSIX extension 
        printf("%'Id\n",1234);
    }

    //

    $ ./numtest
    1,234
    $ LANG=fa_IR.utf8 ./numtest
    ۱٬۲۳۴

*/

// char * string_repeat( int n, const char * s ) {
//   size_t slen = strlen(s);
//   char * dest = malloc(n*slen+1);
 
//   int i; char * p;
//   for ( i=0, p = dest; i < n; ++i, p += slen ) {
//     memcpy(p, s, slen);
//   }
//   *p = '\0';
//   return dest;
// }

// int strset(const char* src, int c, size_t n) {}




#define MMSTR_POW   i256_POW_i8

#define str_find(src, tgt) utf_find(src, strlen(src), tgt, strlen(tgt))

static 
size_t utf_find(const char* s, size_t n, const char* needle, size_t k) {

    const wvi_t first = wvi8_fill(needle[0]);
    const wvi_t last  = wvi8_fill(needle[k - 1]);

    for (size_t i = 0; i < n; i += 32) {

        const wvi_t block_first = wvi_loadu((const wvi_t*)(s + i));
        const wvi_t block_last  = wvi_loadu((const wvi_t*)(s + i + k - 1));

        const wvi_t eq_first = wvi8_eq(first, block_first);
        const wvi_t eq_last  = wvi8_eq(last, block_last);

        u32 mask = wvi8_and_mask(eq_first, eq_last);

        while (mask != 0) {

            const u32 bitpos = u32_ctz(mask);

            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
                return i + bitpos;
            }

            mask = b_clear_leftmost_set(mask);
        }
    }

    return -1;
}

/*

strdespace

- http://0x80.pl/notesen/2019-01-05-avx512vbmi-remove-spaces.html
- https://lemire.me/blog/2017/01/20/how-quickly-can-you-remove-spaces-from-a-string/
- https://github.com/WojciechMula/toys/blob/master/avx512-remove-spaces/aqrit.cpp

*/

// #define CHAR_ARR32(C) { \
//     C,C,C,C,  C,C,C,C,  \
//     C,C,C,C,  C,C,C,C,  \
//     C,C,C,C,  C,C,C,C,  \
//     C,C,C,C,  C,C,C,C   \
// }
// static const char const spaces_32[32] = CHAR_ARR32(' ');
// static const char const fslash_32[32] = CHAR_ARR32('/');

// // const wvi_t spaces = wvi8_fill(' ');      

static const wvi_t MM256_0s = /* 0*/ { 0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000 };
// __m256i _mm256_setzero_si256 (void)

static c_wvi_t WV_CHAR_NL     = /*\n 22*/ { 0x0a0a0a0a0a0a0a0a,0x0a0a0a0a0a0a0a0a,0x0a0a0a0a0a0a0a0a,0x0a0a0a0a0a0a0a0a };
static c_wvi_t WV_CHAR_SPACES = /*   32*/ { 0x2020202020202020,0x2020202020202020,0x2020202020202020,0x2020202020202020 };
static c_wvi_t WV_CHAR_DQUOTE = /* " 34*/ { 0x2222222222222222,0x2222222222222222,0x2222222222222222,0x2222222222222222 };
static c_wvi_t WV_CHAR_SQUOTE = /* ' 39*/ { 0x2727272727272727,0x2727272727272727,0x2727272727272727,0x2727272727272727 };
static c_wvi_t WV_CHAR_FSLASH = /* / 47*/ { 0x2f2f2f2f2f2f2f2f,0x2f2f2f2f2f2f2f2f,0x2f2f2f2f2f2f2f2f,0x2f2f2f2f2f2f2f2f };
static c_wvi_t WV_CHAR_A      = /* A 65*/ { 0x4141414141414141,0x4141414141414141,0x4141414141414141,0x4141414141414141 };
static c_wvi_t WV_CHAR_LBRACK = /* [ 91*/ { 0x5b5b5b5b5b5b5b5b,0x5b5b5b5b5b5b5b5b,0x5b5b5b5b5b5b5b5b,0x5b5b5b5b5b5b5b5b };
static c_wvi_t WV_CHAR_BSLASH = /* \ 92*/ { 0x5c5c5c5c5c5c5c5c,0x5c5c5c5c5c5c5c5c,0x5c5c5c5c5c5c5c5c,0x5c5c5c5c5c5c5c5c };
static c_wvi_t WV_CHAR_a      = /* a 97*/ { 0x6161616161616161,0x6161616161616161,0x6161616161616161,0x6161616161616161 };
static c_wvi_t WV_CHAR_LCURLY = /* { 123*/{ 0x7b7b7b7b7b7b7b7b,0x7b7b7b7b7b7b7b7b,0x7b7b7b7b7b7b7b7b,0x7b7b7b7b7b7b7b7b };

// #define WV_CHAR_SPACES *(const wvi_t *)(spaces_32)

static inline size_t 
utf_minify(
    c_u8* restrict  buf, 
               u32  len, 
      u8* restrict  out
) {                    
    u32 out_pos = 0;

    // c_wvi_t WV_CHAR_BSLASH = wvi8_fill('\\');
    // c_wvi_t WV_CHAR_FSLASH = wvi8_fill('/');
    // c_wvi_t WV_CHAR_NL     = wvi8_fill('\n');
    // c_wvi_t WV_CHAR_SPACES = wvi8_fill(' ');
    // c_wvi_t WV_CHAR_DQUOTE = wvi8_fill('"');

    #define FLAG_IS_LAST_STEP 0
    #define FLAG_OPEN_DQUOTE_N 1
    #define FLAG_OPEN_FLASH_N 2
    #define FLAG_OPEN_BLASH_N 3
    #define FLAG_OPEN_C1_N 4

    MMSCAN_INIT(len)        
    MMSCAN_LOOP() {
        scan.mask = 0;

    // load char data        
        MMSCAN_LOAD()

    // char key masks        
        u64 BOUNDS_m = MMSCAN_LAST_MASK;                
        u64 BSLASH_m = MMSCAN_MASK_EQ_MASK(  BOUNDS_m, WV_CHAR_BSLASH );
        u64 FSLASH_m = MMSCAN_MASK_EQ_MASK(  BOUNDS_m, WV_CHAR_FSLASH );
        u64 NL_m     = MMSCAN_MASK_EQ_MASK(  BOUNDS_m, WV_CHAR_NL     );
        u64 WS_m     = MMSCAN_MASK_LTE_MASK( BOUNDS_m, WV_CHAR_SPACES );
        u64 DQUOTE_m = MMSCAN_MASK_EQ_MASK(  BOUNDS_m, WV_CHAR_DQUOTE );
        
    // handle hanging BSLASH '\\'        
        // TODO: generically handle more than two slashes...  
        // TODO: collapse consecutive ones to higest bit: 1 if odd, 0 if even
        BSLASH_m &= ~(BSLASH_m
                   & ( (BSLASH_m<<1) | MMSCAN_FLAG_BOOL(FLAG_OPEN_BLASH_N) ) );        
        DQUOTE_m &= ~( (BSLASH_m<<1) | MMSCAN_FLAG_BOOL(FLAG_OPEN_BLASH_N) );
        MMSCAN_FLAG_SET(FLAG_OPEN_BLASH_N, BSLASH_m >> 63);
        
    // double quote strings                 
        scan.head = DQUOTE_m;         
        scan.temp = MMSCAN_FLAG_BOOL(FLAG_OPEN_DQUOTE_N);
        //               
        _mm_prefetch(&scan, 1);
        if (scan.head) {
            printf("  dquotes %i \n",b64_count_1s(scan.head));
            LOGV(b64, scan.head);
        }
        scan.tail = -scan.temp;                
        while (scan.head) {        
            scan.temp ^= 1;            
            scan.tail ^= ~((1ULL<<u64_ctz(scan.head))-1ULL) ; // scan.tail &= scan.temp;                        
            scan.head = b64_clear_lo1(scan.head);
        }
        scan.mask = scan.tail;
        // handle hanging head                
        MMSCAN_FLAG_SET( FLAG_OPEN_DQUOTE_N, scan.temp & 1 );            
            // scan.temp = (~((1ULL<<u64_ctz(scan.head))-1ULL));            
        scan.mask = ~scan.tail;
    
    // mask out strings        
        FSLASH_m    &= scan.mask;
        NL_m        &= scan.mask;
        WS_m        &= scan.mask;
        scan.mask = WS_m;        

    // single line comments
        // ^"//" 
        scan.head = (FSLASH_m & (FSLASH_m>>1));        
        // handle hanging FLASH
        scan.head |= FSLASH_m & MMSCAN_FLAG_BOOL(FLAG_OPEN_FLASH_N);
        out_pos -= scan.head & MMSCAN_FLAG_BOOL(FLAG_OPEN_FLASH_N);
        MMSCAN_FLAG_SET(FLAG_OPEN_FLASH_N, 1&((FSLASH_m >> 63) ^ (FSLASH_m >> 62)));
        // handle hanging head
        scan.head |= MMSCAN_FLAG_BOOL(FLAG_OPEN_C1_N);
        // $"\n"                       
        scan.tail = NL_m;        
        //           
        _mm_prefetch(&scan, 1);
        while (scan.tail && scan.head) {            
            scan.temp = ~((1ULL<<u64_ctz(scan.head))-1ULL); 
            scan.tail &= scan.temp;
            scan.mask |= scan.temp & ((1ULL<<(u64_ctz(scan.tail)))-1ULL);                    
            scan.head = b64_clear_lo1(scan.head);
            scan.tail = b64_clear_lo1(scan.tail);
        }
        // handle hanging head                
        MMSCAN_FLAG_SET(FLAG_OPEN_C1_N, (bool)scan.head);
        scan.mask |= (-(bool)scan.head) & (~((1ULL<<u64_ctz(scan.head))-1ULL));
            // if (head_prev = (bool)head) mask |= ~((1<<u32_ctz(head))-1);                
    
    // MMSCAN_FINALIZE_MASK()
        MMSCAN_PRUNE_STORE(scan.mask, out, out_pos)                                         
    }                          
    // LOGV(u, scan.pos);    
    return out_pos;
}

// static inline size_t 
// utf_minify_ww(
//     c_u8* restrict  buf, 
//                u32  len, 
//       u8* restrict  out
// ) {                                  

//     struct mmscan_t scan = {        
//         .temp = 0,                        
//         .mask = 0,
//         .head = 0,
//         .tail = 0,

//         .len = len, //((len) >> 5) + (bool)(len & 31),                
//         .idx = 0,

//         .meta = 0,        
//         .pos = 0,          
//     };

//     #define FLAG_IS_LAST_STEP 0
//     #define FLAG_OPEN_DQUOTE_N 1
//     #define FLAG_OPEN_FLASH_N 2
//     #define FLAG_OPEN_BLASH_N 3
//     #define FLAG_OPEN_C1_N 4

//     c_wwi_t WW_BSLASH = wwi8_fill('\\');
//     c_wwi_t WW_FSLASH = wwi8_fill('/');
//     c_wwi_t WW_NL     = wwi8_fill('\n');
//     c_wwi_t WW_SPACES = wwi8_fill(' ');
//     c_wwi_t WW_DQUOTE = wwi8_fill('"');
        
//     MMSCAN_CHECK_LAST()     
//     while (!MMSCAN_IS_LAST) {
//         scan.mask = 0;
//     // load char data
//         c_wwi_t  vec = wwi_loadu( buf + scan.idx );
//         scan.idx += 64;
//         MMSCAN_CHECK_LAST()
//         // MMSCAN_STEP()           
        
//     // char key masks        
//         u64 BOUNDS_m = MMSCAN_LAST_MASK;                
//         u64 BSLASH_m = wwi8_mask_eq_mask(BOUNDS_m, vec, WW_BSLASH);
//         u64 FSLASH_m = wwi8_mask_eq_mask(BOUNDS_m, vec, WW_FSLASH);        
//         u64 NL_m     = wwi8_mask_eq_mask(BOUNDS_m, vec, WW_NL);
//         u64 DQUOTE_m = wwi8_mask_eq_mask(BOUNDS_m, vec, WW_DQUOTE);        
//         u64 WS_m     = wwu8_mask_lte_mask(BOUNDS_m, vec, WW_SPACES);                                        
        
        
//     // handle hanging BSLASH '\\'        
//         // TODO: generically handle more than two slashes...  
//         // TODO: collapse consecutive ones to higest bit: 1 if odd, 0 if even
//         BSLASH_m &= ~(BSLASH_m
//                    & ( (BSLASH_m<<1) | MMSCAN_FLAG_BOOL(FLAG_OPEN_BLASH_N) ) );        
//         DQUOTE_m &= ~( (BSLASH_m<<1) | MMSCAN_FLAG_BOOL(FLAG_OPEN_BLASH_N) );
//         MMSCAN_FLAG_SET(FLAG_OPEN_BLASH_N, BSLASH_m >> 63);
        
//     // double quote strings                 
//         scan.head = DQUOTE_m;         
//         scan.temp = MMSCAN_FLAG_BOOL(FLAG_OPEN_DQUOTE_N);
//         //               
//         _mm_prefetch(&scan, 1);
//         scan.tail = -scan.temp;                
//         while (scan.head) {        
//             scan.temp ^= 1;            
//             scan.tail ^= ~((1ULL<<u64_ctz(scan.head))-1ULL) ; // scan.tail &= scan.temp;                        
//             scan.head = b64_clear_lo1(scan.head);
//         }
//         scan.mask = scan.tail;
//         // handle hanging head                
//         MMSCAN_FLAG_SET( FLAG_OPEN_DQUOTE_N, scan.temp & 1 );            
//             // scan.temp = (~((1ULL<<u64_ctz(scan.head))-1ULL));            
//         scan.mask = ~scan.tail;
    
//     // mask out strings
        
//         FSLASH_m    &= scan.mask;
//         NL_m        &= scan.mask;
//         WS_m        &= scan.mask;
//         scan.mask = WS_m;        

//     // single line comments
//         // ^"//" 
//         scan.head = (FSLASH_m & (FSLASH_m>>1));        
//         // handle hanging FLASH
//         scan.head |= FSLASH_m & MMSCAN_FLAG_BOOL(FLAG_OPEN_FLASH_N);
//         scan.pos -= scan.head & MMSCAN_FLAG_BOOL(FLAG_OPEN_FLASH_N);
//         MMSCAN_FLAG_SET(FLAG_OPEN_FLASH_N, 1&((FSLASH_m >> 63) ^ (FSLASH_m >> 62)));
//         // handle hanging head
//         scan.head |= MMSCAN_FLAG_BOOL(FLAG_OPEN_C1_N);
//         // $"\n"                       
//         scan.tail = NL_m;        
//         //           
//         _mm_prefetch(&scan, 1);
//         while (scan.tail && scan.head) {            
//             scan.temp = ~((1ULL<<u64_ctz(scan.head))-1ULL); 
//             scan.tail &= scan.temp;
//             scan.mask |= scan.temp & ((1ULL<<(u64_ctz(scan.tail)))-1ULL);                    
//             scan.head = b64_clear_lo1(scan.head);
//             scan.tail = b64_clear_lo1(scan.tail);
//         }
//         // handle hanging head                
//         MMSCAN_FLAG_SET(FLAG_OPEN_C1_N, (bool)scan.head);
//         scan.mask |= (-(bool)scan.head) & (~((1ULL<<u64_ctz(scan.head))-1ULL));
//             // if (head_prev = (bool)head) mask |= ~((1<<u32_ctz(head))-1);                
    
//     // MMSCAN_FINALIZE_MASK()
//         c_wvi_t* wv = (c_wvi_t*)&vec;
//         scan.pos += mm256_prune(wv[0], (u32)scan.mask, out+scan.pos);
//         scan.pos += mm256_prune(wv[1], (u32)(scan.mask>>32), out+scan.pos);                                                               
//         // LOGV(b64, scan.mask);
//     }                          
//     // LOGV(u, scan.pos);    
//     return scan.pos;
// }

static inline size_t 
str_minify(c_u8* restrict buf, u8* restrict out) {               
    size_t pos = utf_minify(buf, strlen(buf), out);
    out[pos] = 0;
    return pos;
}

static inline size_t 
utf_despace(
    c_u8* restrict  buf, 
            size_t  len, 
      u8* restrict  out
) {                     
    c_wvi_t*    vec_ptr = (c_wvi_t*)(buf);    
    c_wv8_m_t   vec_len = len >> MMSTR_POW;
      u32       vec_i = 0;
      u32       pos = 0;            
    
    for (int i = 0; i < vec_len; i++) {       
        c_wvi_t     vec = wvi_loadu( vec_ptr + i );                    
                    // const wvi_t vec =   *(const wvi_t *)(buf + i);   // if aligned                      
        c_wv8_m_t   msk =  wvi8_lte_mask(vec, WV_CHAR_SPACES); //mm256_mask_spaces(vec);   
                    pos += mm256_prune(vec, msk, out+pos);                                                    
    }          

    vec_i = vec_len << MMSTR_POW;    
    for (int i = vec_i; i < len; i++) {   
        const char c = buf[i];                                            
        // BRANCHLESS // if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }            
                        
    return pos;
}


static inline size_t 
str_despace(c_u8* restrict buf, u8* restrict out) {               
    size_t pos = utf_despace(buf, strlen(buf), out);
    out[pos] = 0;
    return pos;
}


// static inline size_t strdespace512(c_u8* buf, size_t len, u8* out) {        
//     c_mi512*    vec_ptr = (c_mi512*)(buf);    
//     c_mm512_8   vec_len = len >> i512_POW_i8;
//       u32       vec_i = 0;
//       u32       pos = 0;      
      
//     while (vec_i < vec_len) {       
//                     // printf("\ni %i, pos %i, len %i\n",i,pos,len);
//         c_mi512     vec = simde_mm512_loadu_si512( vec_ptr + vec_i++ );                    
//                     // const wvi_t vec =   *(const wvi_t *)(buf + i);   // if aligned
//                     // _mm256_lddqu_si256                               // perf++ if crossing cache boundary            
//         c_mm512_8   msk =  mm512_mask_lte_i8(vec, WV_CHAR_SPACES); //mm256_mask_spaces(vec);   
//                     pos += mm512_prune(vec, msk, out+pos);                                                    
//     }  

//     // printf("\ni %i, pos %i, len %i, out=\"%s\"\n",i,pos,len,out);
//     vec_i = vec_len << i512_POW_i8;
//     while (vec_i < len) {
//         const char c = buf[vec_i++];                                            
//         // BRANCHLESS : 
//         // if (c > 32) out[pos++] = c;        
//         out[pos] = c;        
//         pos+=(c > 32);            
//     }
    
//     if_LIKELY( pos < len ) 
//         out[pos] = 0;
    
//     return pos - 1;
// }


#endif