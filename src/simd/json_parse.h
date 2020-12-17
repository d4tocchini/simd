#if !defined(_SIMD_JSON_PARSE_H_)
#define _SIMD_JSON_PARSE_H_

#include "./_.h"
#include "./scan.h"



json_parse(
    c_u8* restrict  buf, 
               u32  len
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

static inline size_t 
json_parse_str(c_u8* restrict buf, u8* restrict out) {               
    size_t pos = json_parse_buf(buf, strlen(buf), out);
    out[pos] = 0;
    return pos;
}


/*

    @node_t User {
        
    }

    N:User, {
        ref = "me"                
        type = "user"
        val = 
        name = "D4"
        avatar = ""                
    })

    

    @edge fri

    n(me)->e(friend)->n()


    n(user/me) {
        name
        e(friend) {
            name
        }  
    }

    //
    
    "user/me": {
        "name": "d4"
        "friend": [
            {"name": "bob"},
            {"name": },
        ]
    }




    

    

json_t r;
json_parse(r, `
    {        
        "nodes" [
            ["foo/val"]
            ["foo/val"]
        ],
        "edges" [
            ["foo", "foo/bar-0", "foo/bar-1"]
        ]
    }
`);

json_parse(r, `
    [
        [ "N(foo=bar-0)", {} ]
        [ "N(foo=bar-1)", {} ]
        [ "E(foo)"]
        
        "edges" [
            ["foo","foo=bar-0","foo=bar-1", {}]
        ]
        "chains" [
            [""]
        ]
    }
`);

json_parse(r, `
    {
        [W, "",
            [N,["foo","bar0"],{}]
            [N,["foo","bar1"],{"name":"bob"}]
            [E,["foo"],["foo","bar0"],["foo","bar1"]],
            [Q, [ ["n",{"type":"user"}] ],
                [ {"color":"red"} ]
            ]
        ]
        [R, "", {
            "fof":[
                [["n"],["-"],["n",{"type":"foo"}]],
                [["type","val"],null,["name"]]
            ]
            "me":[,""[[n],[e],[n]],[, ,{}]]
        }]
    }
`);
==>
{
    "data": {
        "fof": [
            {"type": "foo", "val":"bar"},
            {"name"}
        ]
    }
}

om`
    GRAPH_W("xx/xx",{
        NODE("foo","bar0",{
            SET("name",)
        })
        NODE("foo","bar1")

    })
    GRAPH_R("xx/xx",{    
        QUERY("n()-n(type='foo')",{
            n(0, {
                
            })
        })
    })
`

*/

static inline size_t 
mmjson_minify(
    c_u8* restrict  buf, 
               u32  len, 
      u8* restrict  out
) {                                      
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

    struct mmscan_t scan = {        
        .temp = 0,                        
        .mask = 0,
        .head = 0,
        .tail = 0,

        .len = len, //((len) >> 5) + (bool)(len & 31),                
        .idx = 0,

        .meta = 0,        
        .pos = 0,          
    };
        
    MMSCAN_CHECK_LAST()     
    while (!MMSCAN_IS_LAST) {
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
        scan.pos -= scan.head & MMSCAN_FLAG_BOOL(FLAG_OPEN_FLASH_N);
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
        MMSCAN_PRUNE_STORE()                                         
    }                          
    // LOGV(u, scan.pos);    
    return scan.pos;
}


#endif