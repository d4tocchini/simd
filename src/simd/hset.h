#if !defined(_OM_HASH_SET_H_)
#define _OM_HASH_SET_H_
/*


ctl     |  slots
16         16 


hset_alloc


*/


typedef struct hset_t*   hset_t;
        struct hset_t {
            u8*     ctl;
            i64*    slots;            
            i32     flags;        
            i32     len;
            i32     cap;
            i32     pow;
#if                                                 SX_CONFIG_HASHTBL_DEBUG
            i32  _miss_cnt;
            i32  _probe_cnt;
#endif
        };


#define hset_ctl_NIL 0b10000000 // empty, never set
#define hset_ctl_DEL 0b11111110 // empty, deleted
// #define hset_ctl_SEN 0b11111111 // 
// #define hset_ctl_FULL_MASK 0b10000000

// OM_API 
hset_t   hset_alloc(i32 cap); // allocates hset, capacity  rounded to pow of 2
    // SX_API sx_hashtbl* sx_hashtbl_create(const sx_alloc* alloc, int capacity);




void hset(hset_t hset, i32 cap, u32* keys, i32* values);

void hset_free(hset_t  hset); //, const sx_alloc* alloc);


hset_t hset_alloc(i32 cap) {

}

bool hset_merge_unsafe(hset_t dst, hset_t src) {
    // for (int i = 0, c = src->capacity; i < c; i++) {
    //     if (tbl->keys[i] > 0)
    //         sx_hashtbl_add(new_tbl, tbl->keys[i], tbl->values[i]);
    // }
}

bool hset_grow(hset_t* hset, i32 n) {
    
    i32 pow = hset->pow + n;
    i32 cap = 1 << pow;
    i32 blocks = 1 << (pow - 4);
    u8* ctl = malloc(cap);
    u8* slots = malloc(cap << 3);
    
    HSET_MERGE_UNSAGE( tmp, src )


}

#define hset_H1(self, hash) hash % hset_BLOCK_LEN(self)


i32 hset_find(hash, val) {
    // sx_assert(tbl->count < tbl->capacity);
    
    h1 = hset_H1(hash);
    h2 = hset_H2(hash)    
    u32 pos = hset_SLOT(self, hash);
    ctl_t ctl = self->ctl;
    while (true) {
        ctl[ pos << 4 ]
        kctl_t kctl = _ctl_mask(ctl, h2)
        _ctl_mask_ITERATE(kctl, pos,    IDX,
            if_LIKELY(
                val === slots[ IDX ]
            ) return IDX;            
        )
        if_UNLIKELY( 
            hset_IS_EMPTY() 
        ) return -1;        
        pos = (pos + 1) % hset_BLOCK_LEN(self);
    }


    h2 = hset_H2(hash);
}

i32 hset_add(self, hash, val) {
    
    hset_SCAN( IDX,
        
    )
    
}


#endif