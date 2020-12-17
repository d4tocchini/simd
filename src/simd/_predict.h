#if !defined(SIMD_PREDICT_H)
#define SIMD_PREDICT_H

/*

GCC recognizes `__builtin_expect`:

```c
    long __builtin_expect(long EXP, long C);
```

"What every Programmer should know about Memory" (p. 57) contains an in-depth explanation
https://akkadia.org/drepper/cpumemory.pdf

If the programmer makes use of these macros and then
uses the -freorder-blocks optimization option gcc
will reorder blocks...


https://kristerw.blogspot.com/2017/02/branch-prediction.html

GCC has a number of such predictors, for example:

    * Branch ending with returning a constant is probably not taken.
    * Branch from comparison using != is probably taken, == is probably not taken.
    * Branch to a basic block calling a cold function is probably not taken.

*/

#if !defined(likely)
#   define likely(expr)     __builtin_expect(!!(expr), 1)
#   define unlikely(expr)   __builtin_expect(!!(expr), 0)
#endif

#define if_LIKELY(x) if (likely((x)))
#define if_UNLIKELY(x) if (unlikely((x)))

// TODO: __assume_aligned


#endif