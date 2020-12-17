
## Reference

* Intel intrinsics
    * https://software.intel.com/sites/landingpage/IntrinsicsGuide
* Neon intrinsics 
    * https://developer.arm.com/documentation/dui0491/g/using-neon-support/summary-of-neon-intrinsics

## Installation

cd dev
git clone --recursive https://github.com/Wunkolo/Intriman.git

## Build

cd Intriman
mkdir build
cd build
cmake ..
make
./Intriman data-latest.xml

rm -rf ../../docs
mv ./docs ../../


The process of switching to SIMDe is quite trivial:

 1. Include the relevant header from SIMDe.  In this case, `sse4.1.h`.
 2. Rename relevant types and functions.  `__m128i` becomes `simde__m128i`, and `_mm_*` functions become `simde_mm_*`.
 3. Change compiler flags from `-msse4.1` to `-march=native`.

`_may_i_use_cpu_feature`
https://scc.ustc.edu.cn/zlsc/chinagrid/intel/compiler_c/main_cls/index.htm#GUID-4284FE01-6BA7-4459-8973-2DF2DB889B4B.htm


gcc -mavx2 -dM -E - < /dev/null | egrep "SSE|AVX" | sort

x86intrin.h: x86 instructions
mmintrin.h: MMX (Pentium MMX!)
mm3dnow.h: 3dnow! (K6-2) (deprecated)
xmmintrin.h: SSE + MMX (Pentium 3, Athlon XP)
emmintrin.h: SSE2 + SSE + MMX (Pentiuem 4, Ahtlon 64)
pmmintrin.h: SSE3 + SSE2 + SSE + MMX (Pentium 4 Prescott, Ahtlon 64 San Diego)
tmmintrin.h: SSSE3 + SSE3 + SSE2 + SSE + MMX (Core 2, Bulldozer)
popcntintrin.h: POPCNT (Core i7, Phenom subset of SSE4.2 and SSE4A)
ammintrin.h: SSE4A + SSE3 + SSE2 + SSE + MMX (Phenom)
smmintrin.h: SSE4_1 + SSSE3 + SSE3 + SSE2 + SSE + MMX (Core i7, Bulldozer)
nmmintrin.h: SSE4_2 + SSE4_1 + SSSE3 + SSE3 + SSE2 + SSE + MMX (Core i7, Bulldozer)
wmmintrin.h: AES (Core i7 Westmere, Bulldozer)
immintrin.h: AVX, SSE4_2 + SSE4_1 + SSSE3 + SSE3 + SSE2 + SSE + MMX (Core i7 Sandy Bridge, Bulldozer)

    - http://www.g-truc.net/post-0359.html

## sort

https://github.com/WojciechMula/simd-sort

https://www.geeksforgeeks.org/quicksort-better-mergesort/



# Runtime Dispatch

https://stackoverflow.com/questions/44479069/generate-code-for-multiple-simd-architectures

`target_clones("sse4.1,avx")`
https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-target_005fclones-function-attribute



# Parser

## SIMDJSON

https://www.infoq.com/presentations/simdjson-parser/

1. Read all content
2. Check is valid JSON
3. Check Unicode Encoding
4. Parse Numbers
5. Build DOM

### Trick #1: avoid hard-to-predict branches

* And... Reduce the number of misprediction by doing more work per iteration:

while loop to write random odd vals w/ simple `if` => from 3 to 15 cycles per val!
branchless:

```c
    while (howmany != 0) {
        val = random();
        out[index] = val;
        index += (val & 1); // write only odd
        homany--;
    }
```

### Trick #2: use wide "words", not byte-by-byte
### Trick #3: avoid malloc, duh, use a "tape"
    https://github.com/simdjson/simdjson/blob/master/doc/tape.md
### Trick #4: measure perf

### Example #1: Validate UTF8 
https://github.com/lemire/fastvalidate-utf-8/blob/master/include/simdutf8check.h

verify all bytes <= 244. "saturated subtraction" is non-zero only if x > 244

`_mm256_subs_epu8(curr_bytes, 244)`

### "packed strings" `_mm_cmpestrm`

https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=3503,701,5153,1012,1016,789,5944,5938,5903,3897,223,5230,4856,5796,1141,3789,5878,731,707,834,834,832&text=packed%2525252525252525252520strings



## AVX2 vs SSE4

When both AVX2 and SSE4 are available, the decision whether to use AVX2 is non-obvious. AVX2 vectors are twice as wide, but require a higher power license (integer multiplications count as 'heavy' instructions) and can thus reduce the clock frequency of the core or entire socket(!) on Haswell systems. This partially explains the observed 1.25x (not 2x) speedup over SSE4. Moreover, it is inadvisable to only sporadically use AVX2 instructions because there is also a ~56K cycle warmup period during which AVX2 operations are slower, and Haswell can even stall during this period. Thus, we recommend avoiding AVX2 for infrequent hashing if the rest of the application is also not using AVX2. For any input larger than 1 MiB, it is probably worthwhile to enable AVX2.




# C

The best way to init auto variables to zero
    http://www.pixelbeat.org/programming/gcc/auto_init.html

# C++

reducing c++ runtime size overhead using supc++
    http://www.pixelbeat.org/programming/gcc/supc++/

# ASM

One can trigger gdb breakpoints from code by inserting:
    
    asm("int $0x3\n");