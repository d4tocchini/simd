#include "./hset.h"

om_hset_t
om_hset_alloc(int capacity) {
    //sx_assert(capacity > 0);

    sx_hashtbl* hset = (sx_hashtbl*)sx_malloc(
                    alloc, 
                    sizeof(sx_hashtbl) 
                        + capacity * (sizeof(uint32_t) 
                        + sizeof(int)) 
                        + SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
                    );
    if (!hset) {
        sx_out_of_memory();
        return NULL;
    }

    tbl->keys = (uint32_t*)sx_align_ptr(tbl + 1, 0, SX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT);
    tbl->values = (int*)(tbl->keys + capacity);
    tbl->_bitshift = sx__calc_bitshift(capacity);
    tbl->count = 0;
    tbl->capacity = capacity;
#if SX_CONFIG_HASHTBL_DEBUG
    tbl->_miss_cnt = 0;
    tbl->_probe_cnt = 0;
#endif

    // Reset keys
    sx_memset(tbl->keys, 0x0, sizeof(uint32_t) * capacity);

    return tbl;
}