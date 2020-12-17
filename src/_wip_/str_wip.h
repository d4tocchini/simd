
#define mm256_mask_lte_i8(va, vb) \
    (~simde_mm256_movemask_epi8((const mi256) \
        simde_mm256_cmpgt_epi8( va, vb ) \
    ))
#define mm256_mask_lte_i8(va, vb) \
    simde_mm256_movemask_epi8((const mi256) \
        simde_mm256_cmpeq_epi8( \
            vb, simde_mm256_max_epu8( vb, va ) \
        ) \
    )

#define mm256_mask_eq_i8(va, vb) \
    simde_mm256_movemask_epi8((const mi256) \
        simde_mm256_cmpeq_epi8( vb, va ) \
    ) 

#define mm256_mask_comments(buf) \
        simde_mm256_movemask_epi8((const mi256) \
    )




// 1.1
static inline size_t strdespace256(c_u8* buf, size_t len, u8* out) {                     
    c_mi256*    vec_ptr = (c_mi256*)(buf);    
    c_mm256_8   vec_len = len >> i256_POW_i8;
      u32       vec_i = 0;
      u32       pos = 0;            
    while (vec_i < vec_len) {       
        c_mi256     vec = simde_mm256_loadu_si256( vec_ptr + vec_i++ );                    
                    // const mi256 vec =   *(const mi256 *)(buf + i);   // if aligned                      
        c_mm256_8   msk =  mm256_mask_lte_i8(vec, CH256_SPACES); //mm256_mask_spaces(vec);   
                    pos += mm256_prune(vec, msk, out+pos);                                                    
    }      
    vec_i = vec_len << i256_POW_i8;
    while (vec_i < len) {
        const char c = buf[vec_i++];                                            
        // BRANCHLESS // if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }
    if_LIKELY( pos < len ) 
        out[pos] = 0;
    return pos - 1;
}

// 1.1

static inline size_t 
strdespace256(c_u8* buf, size_t len,volatile  u8* out) {                     
    c_mi256*    vec_ptr = (c_mi256*)(buf);    
    c_mm256_8   vec_len = len >> i256_POW_i8;
      u32       vec_i = 0;
      u32       pos = 0;            
    
    for (int i = 0; i < vec_len; i++) {       
        c_mi256     vec = simde_mm256_loadu_si256( vec_ptr + i );                    
                    // const mi256 vec =   *(const mi256 *)(buf + i);   // if aligned                      
        c_mm256_8   msk =  mm256_mask_lte_i8(vec, CH256_SPACES); //mm256_mask_spaces(vec);   
                    pos += mm256_prune(vec, msk, out+pos);                                                    
    }          

    vec_i = vec_len << i256_POW_i8;    
    for (int i = vec_i; i < len; i++) {   
        const char c = buf[i];                                            
        // BRANCHLESS // if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }
    if_LIKELY( pos < len ) 
        out[pos] = 0;
                        
    return pos - 1;
}


// mp ugh
static inline size_t 
strdespace256(c_u8* buf, size_t len,volatile  u8* out) {                     
    c_mi256*    vec_ptr = (c_mi256*)(buf);    
    c_mm256_8   vec_len = len >> i256_POW_i8;
      u32       vec_i = 0;
      u32       pos = 0;            

      int       i = 0;
      
                        // schedule(dynamic)
#pragma omp parallel    for if (vec_len > 100) \
                        default(none) \
                        shared(CH256_SPACES, vec_ptr, vec_len, out) \
                        private(i) \
                        reduction(+: pos)
        for (i = 0; i < vec_len; i++) {       
            c_mi256     vec = simde_mm256_loadu_si256( vec_ptr + i );                    
                        // const mi256 vec =   *(const mi256 *)(buf + i);   // if aligned                      
            c_mm256_8   msk =  mm256_mask_lte_i8(vec, CH256_SPACES); //mm256_mask_spaces(vec);   
                        pos += mm256_prune(vec, msk, out+pos);                                                    
        }          

    vec_i = vec_len << i256_POW_i8;    
    for (i = vec_i; i < len; i++) {   
        const char c = buf[i];                                            
        // BRANCHLESS // if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }
    if_LIKELY( pos < len ) 
        out[pos] = 0;
                        
    return pos - 1;
}




static inline size_t strdespace256_5_WIP(c_u8* buf, size_t len, u8* out) {                     
  c_mi256*  vec_ptr = (c_mi256*)(buf);    
  c_mm256_8 vec_len = len >> i256_POW_i8;
    u32     vec_i = 0;
    mi256   vec;    

    c_u32   buf_head_off = (u32)buf & ((1<<i256_POW_i8)-1);    
    switch ((bool)buf_head_off) {
        case 0:
            vec = *vec_ptr;
            break;
        default:
            vec = simde_mm256_loadu_si256( vec_ptr );            
    }

  c_mm256_8 msk = mm256_mask_lte_i8(vec, CH256_SPACES); //mm256_mask_spaces(vec);   
    u32     pos = mm256_prune(vec, msk, out);   
    
    
  
      
    while (vec_i < vec_len) {       
                    // printf("\ni %i, pos %i, len %i\n",i,pos,len);                               
        c_mi256     vec = simde_mm256_loadu_si256( vec_ptr + vec_i++ );                    
                    // const mi256 vec =   *(const mi256 *)(buf + i);   // if aligned
                    // _mm256_lddqu_si256                               // perf++ if crossing cache boundary            
        c_mm256_8   msk =  mm256_mask_lte_i8(vec, CH256_SPACES); //mm256_mask_spaces(vec);   
                    pos += mm256_prune(vec, msk, out+pos);                                                    
    }  

    // printf("\ni %i, pos %i, len %i, out=\"%s\"\n",i,pos,len,out);
    vec_i = vec_len << i256_POW_i8;
    while (vec_i < len) {
        const char c = buf[vec_i++];                                            
        // BRANCHLESS : 
        // if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }
    
    // if_LIKELY( pos < len ) 
        out[pos] = 0;

    return pos - 1;
}




// 1.44
static inline size_t strdespace256_4(c_u8* buf, size_t len, u8* out) {                     
    u32         vec_i = 0;
    u32         pos = 0;          
    c_u32       buf_head_off = ((u32)buf & ((1<<i256_POW_i8)-1) );
                len += buf_head_off;    
    c_mi256*    vec_ptr = (c_mi256*)(buf - buf_head_off);    
    c_u32       vec_len = ((len) >> i256_POW_i8)+1 ;
    
    mm512_8     msk =  ((1<<buf_head_off)-1);

    c_u32       remainder_msk = 
                    ~(  (1<< 
                        (len - (vec_len << i256_POW_i8))
                    )-1  ); // remainder_msk =   ~(  (1 << remainder_msk ) - 1  );            
    
    while (vec_i < vec_len) {         
        c_mi256     vec = *(vec_ptr + vec_i++);                                              
                    msk =(u32)(msk 
                        | mm256_mask_lte_i8(vec, CH256_SPACES) 
                        | ((vec_i==vec_len) * remainder_msk)
                    );                                                                                                      
                    pos += mm256_prune(vec, msk, out+pos);         
                    msk = 0;               
    }  
    out[pos] = 0;
    
    return pos - 1;
}

// 1.460
static inline size_t strdespace256_3(c_u8* buf, size_t len, u8* out) {                     
      u32    vec_i = 0;
      u32    pos = 0;      
    if_UNLIKELY(len < 32) 
        goto BRUTE;

    // if (len > 31) {
        c_u32    buf_head_off = ((u32)buf & ((1<<i256_POW_i8)-1) );
        len += buf_head_off;    
        c_mi256* vec_ptr = (c_mi256*)(buf - buf_head_off);    
        c_u32    vec_len = ((len) >> i256_POW_i8)+1 ;

        // if (len > 32) {
            mm512_8   msk =  ((1<<buf_head_off)-1);

            c_u32   remainder_len = (len - (vec_len << i256_POW_i8));            
        
        while (vec_i < vec_len) {         
            c_mi256     vec = *(vec_ptr + vec_i++);                                              
                        msk =(u32)(msk 
                            | mm256_mask_lte_i8(vec, CH256_SPACES) 
                            | ~(  (1 << remainder_len ) - 1  )
                        );                                                                                                      
                        pos += mm256_prune(vec, msk, out+pos);         
                        msk = 0;       
        
        }  
        goto DONE;
    // }

BRUTE:

    while (vec_i < len) {
        const char c = buf[(vec_i++) ];                                            
        // BRANCHLESS : if (c > 32) out[pos++] = c;        
        out[pos] = c;        
        pos+=(c > 32);            
    }

DONE:
    // if_LIKELY( pos < len ) 
        out[pos] = 0;
    
    return pos - 1;
}

// 1.480
static inline size_t strdespace256_2(c_u8* buf, size_t len, u8* out) {                     
      u32    vec_i = 0;
      u32    pos = 0;      
    if_UNLIKELY(len < 32) 
        goto BRUTE;

    
        c_u32    buf_head_off = ((u32)buf & ((1<<i256_POW_i8)-1) );
                 len += buf_head_off;    
        c_mi256* vec_ptr = (c_mi256*)(buf - buf_head_off);    
        c_u32    vec_len = ((len) >> i256_POW_i8) ;

            mm512_8   msk =  ((1<<buf_head_off)-1);
        
        while (vec_i < vec_len) {                   
            c_mi256     vec = *(vec_ptr + vec_i++);                                              
                        msk =(u32)(msk 
                            | mm256_mask_lte_i8(vec, CH256_SPACES)
                        );
                        pos += mm256_prune(vec, msk, out+pos);         
                        msk = 0;       

        
        }  
        
        len -= buf_head_off;
        
        vec_i = ((vec_len - (bool)buf_head_off) << i256_POW_i8) + buf_head_off;        
BRUTE:

    while (vec_i < len) {
        const char c = buf[(vec_i++) ];                                                    
        out[pos] = c;        
        pos+=(c > 32);            
    }

DONE:
    // if_LIKELY( pos < len ) 
        out[pos] = 0;
    
    return pos - 1;
}




// 1.6
static inline size_t strdespace256(c_u8* buf, size_t len, u8* out) {                     
      u32    vec_i = 0;
      u32    pos = 0;      
      u32    vec_len;
    // if (len > 31) {
        
        // load buff_head before first alignment        
        vec_len = ((u32)buf & ((1<<i256_POW_i8)-1) );
        vec_len = vec_len > len ? len : vec_len;
        // puts("----");
        // printf("\nvec_len %u, len %u\n",vec_len, len);
         while (vec_i < len) {
            const char c = buf[(vec_i++) ];                                            
            // BRANCHLESS : if (c > 32) out[pos++] = c;        
            out[pos] = c;        
            pos+=(c > 32);            
        }
        len -= vec_len;    

        //
        vec_i = 0;
        vec_len = ((len) >> i256_POW_i8);
        c_mi256* vec_ptr = (c_mi256*)(buf );    
    // printf("\nvec_len %u\n",vec_len);
        
                
            c_u32   remainder_msk = 
                ~(  (1<< 
                        (len - (vec_len << i256_POW_i8))
                    )-1  );            
            // remainder_msk =   ~(  (1 << remainder_msk ) - 1  );            
        
        while (vec_i < vec_len) {         
            c_mi256     vec = *(vec_ptr + vec_i++);                                              
            c_mm512_8   msk =(mm256_mask_lte_i8(vec, CH256_SPACES) 
                            | ((vec_i==vec_len) * remainder_msk)
                        );                                                                                                      
                        pos += mm256_prune(vec, msk, out+pos);                                 
        }  
        out[pos] = 0;
    
    return pos - 1;
}
