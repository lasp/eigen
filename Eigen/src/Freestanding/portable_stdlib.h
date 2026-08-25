#ifndef PORTABLE_STDLIB_HPP
#define PORTABLE_STDLIB_HPP

#include <cstddef>  // for std::size_t

#if defined(__GNUC__) && defined(__cplusplus) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)

// Freestanding Mode
extern "C" {
    void* malloc(std::size_t);
    void  free(void*);
    void* realloc(void*, std::size_t);
    void* calloc(std::size_t, std::size_t);
}

// alloca must be a MACRO, not a declaration: Eigen selects its stack-allocation
// path with `#if ... || (defined alloca)` (Core/util/Memory.h), which a function
// declaration never satisfies. Left as a declaration, EIGEN_ALLOCA stays
// undefined and every Eigen stack temporary falls back to the heap -- and any
// call would be an undefined symbol, since freestanding has no libc alloca.
// __builtin_alloca is emitted inline by GCC on every target, RISC-V included.
#define alloca __builtin_alloca

namespace std {
    using ::malloc;
    using ::free;
    using ::realloc;
    using ::calloc;
}

#else

// Hosted Mode
#include <cstdlib>  // for std::malloc, etc.
#include <alloca.h>  // alloca on POSIX-compatible systems. This doesn't work with MSVC

namespace std {
    using ::alloca;  // alloca is not part of std, so pull it in explicitly
}

#endif // __STDC_HOSTED__

#endif // PORTABLE_STDLIB_HPP
