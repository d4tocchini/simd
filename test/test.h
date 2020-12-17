/*

(legacy) Providing compile time assertions
    http://www.pixelbeat.org/programming/gcc/static_assert.html

How the GNU coreutils are tested
    http://www.pixelbeat.org/docs/coreutils-testing.html

gnu automake tests
    https://www.gnu.org/software/automake/manual/html_node/Tests.html#Tests

*/


#define LN_RESET "\033[2K\033[1G"
#define LN_TRUNC "\033[0K"
#define CUR_L "\033[1D"
#define CUR_L2 "\033[2D"



// '\033[#D': Move cursor left by # characters.
// '\033[2K': Clear current line (but don't move cursor).
// '\033[1K': Clear line to the left of current position.
// '\033[0K': Clear line to the right of current position.
// '\033[1G': Move cursor to beginning of line.
// '\033[#;#f': Move cursor to specific position. First # is row number, second # is column number.
#define printindents(n) printf("%*s", ((n)<<2), "")

// #define printftab(n, fmt, ...) printf("%*s" fmt, ((n)<<2), __VA_ARGS__)

// #if defined(OM_TEST_H)
// #define OM_TEST_H

// #if defined(__GNUC__)

#define _SUITE_BEFORE_      \
    puts("");               \
    printindents(_t_depth); \
    printf("%s ", _suite_name );   \
    puts("");               

#define _SUITE_AFTER_       \
    puts("");              

// printindents(_t_depth);  \
// printf("%s %i/%i",_t_count - _t_fails, _t_count );   \
// puts("");               \

#define _TEST_BEFORE_       \
    printindents(_t_depth); \
    printf("%s ", _t_name );      
#define _TEST_DID_PASS_       \
    puts("✓");      

#if defined(TESTS_O_QUIET)
#   define _SUITE_BEFORE_
#   define _SUITE_AFTER_
#   define _TEST_BEFORE_
#   define _TEST_DID_PASS_
#endif

#define SUITE(name,SRC) {   \
    int _t_count = 0;       \
    int _t_depth = 1;       \
    int _t_fails = 0;       \
    const char _suite_name[] = #name; \
    _SUITE_BEFORE_          \
    SRC                     \
    _SUITE_AFTER_           \
}



#define TEST(name, SRC) {               \
    _t_count++;                         \
	_t_depth++;                         \
    const char  _t_name[] = #name;      \
    int   _t_errno = 0;                 \
    void* _t_done   = &&name##_t_done;  \
    _TEST_BEFORE_                       \
	SRC                                 \
name##_t_done :                         \
    if (_t_errno) {                     \
        _t_fails++;                     \
    }                                   \
    else {                               \
        _TEST_DID_PASS_                 \
    }                                   \
    _t_depth--;                         \
}

#define TEST_DONE goto * _t_done;

#define _PASS(SRC, ERRNO, msg)                 \
    _t_errno = ERRNO;                   \
    if (_t_errno) {                     \
        printf("❌ " SRC " : %s\n", msg); \
        TEST_DONE                       \
    } 

#define PASS(x, msg) _PASS( #x, (x), (msg));

#define _EXPECT(SRC, val, expected)              \
    _t_errno = val != expected;                   \
    if (_t_errno) {                     \
        printf("❌ " SRC " => %i expected %i", val, expected); \
        TEST_DONE                       \
    } 
#define EXPECT(actual, expected) _EXPECT(#actual, (actual), (expected))

// #define PASS_STREQ(val, exp)     

// #endif


// #define T__(name) { \
// 	_t_depth++; \
//         printf(" %s %i \n", name, _t_depth); 

// #define __T \
// 	_t_depth--; \
// }

// #define SUITE(name,src) { \
//         int _t_depth =  0; \
// 	src \
// }

// #define TEST(name, src) T__(name) \
// 	src \
// __T