
#include "./blockr.h"

typedef struct parser_t* parser_t;
struct parser_t {
    struct blockr_t blockr;
    u32     msk;
}



typedef struct lgp_t* lgp_t;
struct lgp_t {   
    
    
};


void lg_ws_mask(const mi256 chunk) {
    const mi256 tgt = simde_mm256_set1_epi8(' ');
    const mi256 cmp = simde_mm256_cmpeq_epi8(spaces, 
        simde_mm256_max_epu8(spaces, chunk)
    );
    uint32_t mask = simde_mm256_movemask_epi8(cmp);    
    if (mask == 0)
        return void;
}

void lg_parse_obj_open(lgp_t lgp) {

}

#define MASK_SHIFT(mask, byt) (mask >> byt)

void lg_parse_obj(lgp_t lgp) {
    u32 buf = lgp->buf;
    u32 len = lgp->len;
    u32 pos = lgp->pos;
    u32 byt = lgp->byt;
    u32 hits = 0;
    while (pos < len) {                
        const mi256 chunk = simde_mm256_loadu_si256(buf + pos);
        u32 skip_mask = lg_ws_mask(chunk);                
        u32 obj0_mask = lg_obj0_mask(chunk);
        u32 obj1_mask = lg_obj1_mask(chunk);
        u32 spec_mask = lg_obj1_mask(chunk);
        u32 str0_mask = lg_obj0_mask(chunk);
        u32 str1_mask = lg_obj1_mask(chunk);
        u32 lit0_mask = lg_obj0_mask(chunk);
        u32 lit1_mask = lg_obj1_mask(chunk);        

        while (byt < 32) {
            byt = u32_ct1(skip _mask);
            MASK_SHIFT(ws_mask, byt)
            hits = u32_ct1(obj0_mask);
            if (hits) {
                HIT_OPEN()
                byt += hits
                // >
                if (byt == 31)
                    continue
            }
                
            MASK_EAT(ws_mask, byt)            
                
    }
}


void 
parse_block(parser_t p, const u8* block) {
    const mi256 chunk = simde_mm256_loadu_si256((const mi256*)block);


    const mi256 obj_opn = simde_mm256_set1_epi8('{');

    const mi256 spaces = simde_mm256_set1_epi8(' ');    
    

    u32 ws_m = simde_mm256_movemask_epi8( 
        simde_mm256_cmpeq_epi8(spaces, simde_mm256_max_epu8(spaces, chunk))
    );
    // u32 ws_m = ~( simde_mm256_movemask_epi8(simde_mm256_subs_epi8(chunk, spaces)) );
    

    u32 jmp = u32_trailing_1s(ws_m);

    while (ws_m != 0) {
        const u32 bitpos = u32_trailing_0s(mask);

        if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
            return i + bitpos;
        }

        ws_m = u32_clear_lowest_bit(ws_m);
    }




}



int 
parser_stage1(u8* buf, u32 len) {

    // if (unlikely(len > parser.capacity())) { return CAPACITY; }
    if (partial) { len = trim_partial_utf8(buf, len); }

    if (!len) 
        LG_ERR_RETURN(1, "lg_parse_doc : len == 0")    
    

    struct parser_t parser = {
        .blockr = {
            buf  = buf,
            len  = 0,
            step = STEP_SIZE,   
            len_minus_step = len - STEP_SIZE,      
            idx = 0
        }
        .scannr = {

        }
    };    
    blocker_t blockr = &parser.blockr;
    
    MM_READER_DECL(reader, buf, len, 256)

    const mi256 WS = simde_mm256_set1_epi8(' ');    
    const mi256 DQUOTE = simde_mm256_set1_epi8('"');    
    const mi256 FSLASH = simde_mm256_set1_epi8('/');
    const mi256 STAR = simde_mm256_set1_epi8('*');    
    const mi256 LCURLY = simde_mm256_set1_epi8('{');    
    const mi256 LARRAY = simde_mm256_set1_epi8('[');    
    
    while (!mm_reader_done(reader)) {        
        u32 taken_m = 0;
        
        const u8*   block = mm_reader_next(reader);
        const mi256 curr = simde_mm256_loadu_si256((const mi256*)block);
        const mi256 prev = simde_mm256_loadu_si256((const mi256*)(block + 1));
        
        // c-style comments
        const mi256 curr_FSLASH = simde_mm256_cmpeq_epi8(curr, FSLASH);
        u32         curr_FSLASH_m = simde_mm256_movemask_epi8(curr_FSLASH);
        if_UNLIKELY( curr_FSLASH_m != 0 ) {
            // check single-line comments
            // check multi-line comments
            u32 comment_mask = simde_mm256_movemask_epi8(
                simde_mm256_and_si256(
                    curr_FSLASH, 
                    simde_mm256_cmpeq_epi8(prev, block_first)
                )
            );
        }

        
    
    }
    

    
}

