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
    void* alloca(std::size_t);  // declared here, no standard header guarantees it in freestanding mode
}

namespace std {
    using ::malloc;
    using ::free;
    using ::realloc;
    using ::calloc;
    using ::alloca;
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
