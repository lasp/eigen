#pragma once
#include <string.h>

// Move some of the string.h functions into standard namespace to be more cstring-like for Eigen.
namespace std {

    inline void* memcpy(void* destination, const void* source, size_t num) {
        return ::memcpy(destination, source, num);
    }

} // namespace std
