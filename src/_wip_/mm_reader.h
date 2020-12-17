
#define MM_READER_MAX_STEP 512;
#define MM_READER_PAD_CHAR 0x20;

typedef mm_reader_t* mm_reader_t;
struct  mm_reader_t {
    const u8*   buf;     
    const u32   len;
    const u32   step;        
          u32   idx; 
    const u32   pad;
    const u8    last[MM_READER_MAX_STEP];   
};

#define MM_READER_DECL(NAME, buf, len, step) \
    struct mm_reader_t NAME = { \
        .buf = buf,             \
        .len = len,             \
        .step = step,           \
        .idx = 0,               \
        .pad = len % step       \
    };

inline bool 
mm_reader_done(mm_reader_t r) {
    return (r->idx < (r->len + (bool)r->pad));
}

inline const u8*
mm_reader_next(mm_reader_t r) {
    if_LIKELY( r->idx < r->len ) {
        const u32 idx = r->idx;
        r->idx += r->step;
        return &(r->buf[idx]);
    }
    else {
        memset(&r->last, MM_READER_PAD_CHAR, r->step); // memset STEP_SIZE because it's more efficient to write out 8 or 16 bytes at once.        
        memcpy(&r->last, r->buf + r->idx, r->len - r->idx);
        return &r->last;
    }    
}
