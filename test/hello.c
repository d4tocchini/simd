#include "simd/str.h"
#include "./test.h"

#define TESTS_ITERATIONS 1
// #define TESTS_O_QUIET 1
// #define TESTS_ITERATIONS 1000

/*

~0
 rm -f ./hello; gcc -w -g -std=gnu2x -march=native -O3 -pipe -DSIMDE_ENABLE_OPENMP -fopenmp -fopenmp-simd -ftree-vectorize -I../src -I../src/simde -o hello hello.c; time ./hello
//  -mfpmath=sse -msse2 ? 
*/

// https://stackoverflow.com/questions/30838959/parallelizing-matrix-multiplication-through-threading-and-simd
// https://en.wikipedia.org/wiki/Loop_nest_optimization
// #define MAX_THREADS 4
// #include <omp.h>


#define log_char_int(x) printf("\"%s\"=%i ",x,(uint8_t)*x);


#include <sys/mman.h>



#define mmfree(ptr, len) munmap(ptr, len)
#define mmalloc(size) \
  mmap(NULL, size, \
    PROT_READ|PROT_WRITE, \
    MAP_ANON|MAP_PRIVATE|MAP_HUGETLB, \
    -1, 0)
// https://scc.ustc.edu.cn/zlsc/chinagrid/intel/compiler_c/main_cls/index.htm#GUID-FC3721DD-38C0-4237-9709-D974A50E3CBE.htm
// MAP_HASSEMAPHORE Notify the kernel that the region may contain sema- phores and that special handling may be necessary.
// MAP_SHARED MAP_NOCACHE MAP_FILE MAP_FIXED MAP_HASSEMAPHORE
// MAP_SHARED || MAP_PRIVATE (Modifications are private (copy-on-write).)
// VM_FLAGS_PURGABLE   to create Mach purgable (i.e. volatile) memory.


#define SPACES_8 "        "
#define SPACES_16 SPACES_8 SPACES_8
#define SPACES_32 SPACES_16 SPACES_16
#define SPACES_64 SPACES_32 SPACES_32
#define SPACES_128 SPACES_64 SPACES_64
#define SPACES_256 SPACES_128 SPACES_128
#define SPACES_512 SPACES_256 SPACES_256

void str_suite() {
    
    SUITE("bits",{
        TEST(u64_cmp_num, {
            u64 x = (u64)1 << 33;
            u64 y =      1 << 33;
            EXPECT( x == 0, 0 )
            EXPECT( y == 0, 1 )
        })
    })
    SUITE("str.h",{
        char out[1024];

        TEST(str_find, {
            EXPECT( str_find("xxfooyy", "foo"), 2 )
            EXPECT( str_find("1234567890  XX  ", "XX"), 12 )
            EXPECT( str_find("1234567890123456789012345678901234567890  XX  ", "XX"), 42 )
            EXPECT( str_find("1234567890123456789012345678901234567890  XX  ", "123"), 0 )            
            
            TEST_DONE // WIP...
            EXPECT( str_find("1234567890  X  ", "X"), 12 )
            EXPECT( str_find("1234567890123456789012345678901234567890  X  ", "X"), 42 )
        })

        TEST(str_despace, {                    
                        //   (3233868229).toString(2)
                           //10100111000101
            char str[] =    "   x y  z                    . . . abc   12 3 ";
            // // write output
            str_despace(str, out);                
            PASS( strcmp(out, out), out )
            PASS( strcmp(out, "xyz...abc123"), out )        
            // overwrite input
            str_despace(str, str);                
            PASS( strcmp(str, "xyz...abc123"), str )
            
            str_despace(" x ", out);                            
            PASS( strcmp(out, "x"), out )        
                        
            str_despace(" y                                      ", out);            
            PASS( strcmp(out, "y"), out )        

            str_despace(
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "    x     " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                "          " "          " "          " "          " "          " "          " "          " "          " "          " "          "
                , out);                            
            PASS( strcmp(out, "x"), out )        
            
            str_despace(" ", out);                
            PASS( strcmp(out, ""), "EMPTY_1" )       
                    //  1234567890123456789012345678901234567890
            // out[0] = 0;
            str_despace("                                        ", out);                
            PASS( strcmp(out, ""), out )        
        })

        TEST(str_minify__sline_comments, {            
            {
                char out[1024];
                char str[] = 
                    "// l:0                     \n"
                    "       // l:1 comment              \n"
                    "                                   // l:2\n"
                    "x=1;// l:3\n"
                    "                                   \n"
                    "                                   \n"
                    "y=2;//\n"                                        
                ;
                str_minify(str,out);
                PASS( strcmp(out, "x=1;y=2;"), out)
                str_minify(str,str);
                PASS( strcmp(str, "x=1;y=2;"), str)
            }
            for (int i = 1; i < 509; i++)
            {                
                char str[] = SPACES_512;                
                str[i-1] = 'x';
                str[i] = '/';
                str[i+1] = '/';
                str[511] = '\n';
                str_minify(str,str);
                PASS( strcmp(str, "x"), str)
                // printf("%i\n",i);
            }
            for (int i = 1; i < 510; i++)
            {                
                char str[] = SPACES_512;                
                str[i-1] = 'x';
                str[i] = '/';
                str[i+1] = '/';                
                str_minify(str,str);
                PASS( strcmp(str, "x"), str)
                // printf("%i\n",i);                
            }            
            for (int i = 1; i < 1020; i++)
            {                
                char str[] = SPACES_512 SPACES_512;                
                str[i-1] = 'x';
                str[i] = '/';
                str[i+1] = '/';
                str[i+2] = '/';
                str[i+3] = 0;
                str_minify(str,str);
                PASS( strcmp(str, "x"), str)
                // printf("%i\n",i);
            }
            for (int i = 1; i < 1020; i++)
            {                
                char str[] = SPACES_512 SPACES_512;                                
                str[i] = '/';
                str[i+1] = '/';
                str[i+2] = 'x';
                str[i+3] = 0;
                str_minify(str,str);
                PASS( strcmp(str, ""), str)
                // printf("%i\n",i);
            }                        
        })
        TEST(str_minify__dquote_strings, {            
            {
                char out[1024];
                char str[] = 
                    "\" x \"                     \n"
                    "       // l:1 comment              \n"
                    "                                   // l:2\n"
                    "\" y \"// l:3\n"
                    "                                   \n"
                    "                                   \n"
                    "\" z \""                                        
                    "                                   \n"
                    "\" \\\" \""
                    "                                   \n"
                    // "\" \\\\\" \""
                ;
                str_minify(str,out);
                PASS( strcmp(out, 
                    "\" x \""
                    "\" y \""
                    "\" z \"" 
                    "\" \\\" \"" 
                    // "\" \\\\\" \""
                ), out)
                // str_minify(str,str);
                // PASS( strcmp(str, " x y z "), str)
            }
            // {
            //     char str[] = " {
            //         [W, '',
            //             [N,['foo','bar0'],{}]
            //             [N,['foo','bar1'],{"name":"bob"}]
            //             [E,["foo"],["foo","bar0"],["foo","bar1"]],
            //             [Q, [ ["n",{"type":"user"}] ],
            //                 [ {"color":"red"} ]
            //             ]
            //         ]
            //         [R, "", {
            //             "fof":[
            //                 [["n"],["-"],["n",{"type":"foo"}]],
            //                 [["type","val"],null,["name"]]
            //             ]
            //             "me":[,""[[n],[e],[n]],[, ,{}]]
            //         }]
            //     }"
            // }
            {                
                char str[] = 
                    SPACES_512 "\"" SPACES_512 "\"" SPACES_512;
                str_minify(str,str);
                PASS( strcmp(str, "\"" SPACES_512 "\""), str)                
            }
            {                
                char str[] = 
                    "// \"" SPACES_512 "\"" SPACES_512;
                str_minify(str,str);
                PASS( strcmp(str, ""), str)                
            }
            {                
                char str[] = 
                    "\"//" SPACES_512 "\"" SPACES_512;
                str_minify(str,str);
                PASS( strcmp(str, "\"//" SPACES_512 "\""), str)                
            }
            for (int i = 0; i < 508; i++) {                
                char str[] = SPACES_512;                                
                str[i] = '"';
                str[i+1] = 'x';
                str[i+2] = '"';
                str[i+3] = 0;
                str_minify(str,str);
                PASS( strcmp(str, "\"x\""), str)
                // printf("%i\n",i);
            }       
            for (int i = 0; i < 508; i++) {                
                char str[] = SPACES_512;                                
                str[i] = '"';
                str[i+1] = '"';
                str[i+2] = '/';
                str[i+3] = '/';
                str_minify(str,str);
                PASS( strcmp(str, "\"\""), ":empty-str|comment")
                // printf("%i\n",i);
            }       
            for (int i = 0; i < 508; i++) {                
                char str[] = SPACES_512 ;                                
                str[i] = '"';
                str[i+1] = '\\';
                str[i+2] = '"';
                str[i+3] = '"';
                str_minify(str,str);
                // printf("%i\n",i);
                PASS( strcmp(str, "\"\\\"\""), str)
                
            }       
            for (int i = 0; i < 508; i++) {                
                char str[] = SPACES_512 ;                                
                str[i] = '"';
                str[i+1] = '\\';
                str[i+2] = '\\';
                str[i+3] = '"';
                str_minify(str,str);
                // printf("%i\n",i);                                
                // printf("\n%i %i\n",strlen("\"""\\\\""\""),strlen(str));
                PASS( strcmp(str, "\"" "\\\\" "\""), str)
                
            }       
            // TODO:...
            // for (int i = 0; i < 500; i++) {                
            //     char str[] = SPACES_512 ;                                
            //     str[i] = '"';
            //     str[i+1] = '\\';
            //     str[i+2] = '\\';
            //     str[i+3] = '\\';
            //     str[i+4] = '"';
            //     str[i+5] = '"';
            //     str_minify(str,str);
            //     // printf("%i\n",i);                                
            //     printf("\n%i %i\n",strlen("\"" "\\\\\\\"" "\""),strlen(str));
            //     PASS( strcmp(str, "\"" "\\\\\\\"" "\""), str)                
            // }       
            for (int i = 0; i < 1020; i++) {                
                char str[] = SPACES_512 SPACES_512;                                
                str[i] = '"';
                str[i+1] = 'x';
                str[i+2] = ' ';                
                str[i+3] = '"';                
                str_minify(str,str);
                PASS( strcmp(str, "\"x \""), str)
                // printf("%i\n",i);
            }       
        })

        // TEST(str_ltrim, {        
        //     char str[] = "   hello world x y z     ";
        //     str_ltrim(str, strlen(str));                
        //     PASS( strcmp(str, "hello world x y z     "), str )        
        // })

    })
}

int main(int argc, char *argv[]) {
    // char* src = argv[1];
    // char* tgt = argv[2];
    // printf("str_find %s ? %s => %i\n", src, tgt, str_find(src, tgt));

    #if defined(__AVX2__)
        puts("__AVX2__");
    #endif
    // printf("%ull",&_mm_clmulepi64_si128);

    #if defined(MAX_THREADS)
    omp_set_num_threads(MAX_THREADS);
    #endif

    int i = 0;
    while (i++ < TESTS_ITERATIONS)
        str_suite();        
}