#if defined(_SIMD_TAPE_H_)
#define _SIMD_TAPE_H_

/*

the layout of the tape is in "document order": elements are stored as they are encountered in the JSON document.

tape_slab
    * 512-bit 64-Byte reprensenting 7 elements
        el-close-bits       |    el-types       |   el-vals        
        (1-Byte, 7x1-bit)   (7x1-Byte)          (7x8-Byte)
                            1 2 3 4 5 6 7       val1 val2 val3 val4 val5 val6 val7
    * 64x(1-byte el-type), 8x[8x(8-byte)]


STRUCTURAL
    ROOT
    ARRAY 
    OBJECT
STRING
NUMERIC
    I64
    U64
    F64
RESERVED KEYWORDS
    NULL
    TRUE
    FALSE


Tape API inspired by 

# SIMDJSON Tape Structure
https://github.com/simdjson/simdjson/blob/master/doc/tape.md

most elements written as 
    `('c' << 56) + x`
    * `'c'` is some ASCII character determining the type of the element 
    * `x`  is a 56-bit value called the payload. The payload is normally interpreted as an unsigned 56-bit integer

Performance consideration: 
    * We believe accessing the tape in regular units of 64 bits is more important for performance than saving memory.

Simple nodes are represented with one 64-bit tape element:
    * `null` is `('n' << 56)`
    * `true` is `('t' << 56)`
    * `false` is `('f' << 56)`

Numberic nodes are represented as two 64-bit tape elements:
    * signed 64-bit int     : `(u64 [2]){ ('l' << 56), val }`
    * unsigned 64-bit int   : `(u64 [2]){ ('u' << 56), val }`
    * floats                : `(u64 [2]){ ('d' << 56), val }`  value literally in standard IEEE 754 notation
    
Root nodes:
    * `('r' << 56) + x` where x is the location on the tape of the last root element.
    * The last 64-bit tape element contains the value `('r' << 56)`
    * Hint: We can read the first tape element to determine the length of the tape.

String nodes:
    * prefix the string data itself by a 32-bit header to be interpreted as the strlen (32-bit int). 
        actual string data starts at an offset of 4 bytes.
    * We store string values using UTF-8 encoding with null termination on a separate tape. 
        A string value is represented on the main tape as the 64-bit tape element ('"' << 56) + x 
            where the payload x is the location on the string tape of the null-terminated string.

Array nodes (two 64-bit tape elements):
    * `('[' << 56) + x` where the payload x is 1 + the index of the second 64-bit tape element on the tape.
    * `(']' << 56) + x` where the payload x contains the index of the first 64-bit tape element on the tape.
    * All the content of the array is located between these two tape elements, including arrays and objects.
    * Performance consideration: We can skip the content of an array entirely by accessing the first 64-bit tape element, reading the payload and moving to the corresponding index on the tape.

Object noes (two 64-bit tape elements)
    * In-between these two tape elements, we alternate between key (which must be strings) and values. A value could be an object or an array.
    * All the content of the object is located between these two tape elements, including arrays and objects.
    * Performance consideration: We can skip the content of an object entirely by accessing the first 64-bit tape element, reading the payload and moving to the corresponding index on the tape.
*/


#include "./_.h"


/**
 * The possible types in the tape.
 */
// enum tape_type {
//   ROOT = 'r', // 0
//   START_ARRAY = '[',
//   START_OBJECT = '{',
//   END_ARRAY = ']',
//   END_OBJECT = '}',
//   STRING = '"',
//   INT64 = 'l',
//   UINT64 = 'u',
//   DOUBLE = 'd',
//   TRUE_VALUE = 't',
//   FALSE_VALUE = 'f',
//   NULL_VALUE = 'n'
// }; // enum class tape_type


struct tape_slab_t {    
    union {
        u64 meta;
        struct {
            u8 head;
            u8 types[7];
        };
    }
    u64 vals[7];    
}
struct tape_t {
    tape_slab_t * slabs;
    u32 slab_len;
    u32 el_len;
    u32 slab_idx;
    u32 slab_pos;
}

void
tape_push_root(tape_t tape, f64 val) {
    tape_slab_t slab = tape.slabs[tape.slab_idx];
    slab.types[tape.slab_pos] = ;
    static_assert(sizeof(val) == 64, "Type is not 64 bits!");
    memcpy(&slab.vals[tape.slab_pos++], &val, sizeof(val));        
}

void
tape_push_i64(tape_t tape, i64 val) {
    tape_slab_t slab = tape.slabs[tape.slab_idx];
    slab.types[tape.slab_pos] = ;
    slab.vals[tape.slab_pos++] = val;    
}

void
tape_push_u64(tape_t tape, u64 val) {
    tape_slab_t slab = tape.slabs[tape.slab_idx];
    slab.types[tape.slab_pos] = ;
    slab.vals[tape.slab_pos++] = val;    
}

void
tape_push_f64(tape_t tape, f64 val) {
    tape_slab_t slab = tape.slabs[tape.slab_idx];
    slab.types[tape.slab_pos] = ;
    static_assert(sizeof(val) == 64, "Type is not 64 bits!");
    memcpy(&slab.vals[tape.slab_pos++], &val, sizeof(val));        
}




#endif