
// https://github.com/simdjson/simdjson/blob/master/src/generic/stage1/buf_block_reader.h


typedef blockr_t* blockr_t;
struct  blockr_t {
    const u8*   buf; 
    const u32   len;
    const u32   step;    
    const u32   len_minus_step;      
          u32   idx;    
};

inline bool 
blockr_has_full(blockr_t self) {
    return (self->idx < self->len_minus_step);
}

inline const u8*
blockr_block(blockr_t self) {
    return &self->buf[self->idx];
}

inline u32 
blockr_remainder(blockr_t self, u8* dst) {
    memset(dst, 0x20, self->step); // memset STEP_SIZE because it's more efficient to write out 8 or 16 bytes at once.
    u32 r = self->len - self->idx;
    memcpy(dst, self->buf + self->idx, r);
    return r;
}

inline void
blockr_advance(blockr_t self) {
    self->idx += self->step;    
}
