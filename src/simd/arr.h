

// A common requirement is to get the length of an array at compile time
// Also note that C99 supports variable length arrays.
#define arr_lengthof(x) (sizeof(x)/sizeof(x[0]))
