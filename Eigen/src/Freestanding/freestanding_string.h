#pragma once
#include <string.h>

// Move some of the string.h functions into standard namespace to be more cstring-like for Eigen.
namespace std {

    inline void* memcpy(void* destination, const void* source, size_t num) {
        return ::memcpy(destination, source, num);
    }

    inline void* memmove(void* destination, const void* source, size_t num) {
        return ::memmove(destination, source, num);
    }

    // Minimal std::string stub for freestanding mode.
    // Eigen's Constants.h uses std::string for debugName() methods which are
    // not called at runtime in freestanding builds. This satisfies the compiler
    // without pulling in the full <string> header (unavailable in freestanding).
    class string {
        const char* str_;
    public:
        string() : str_("") {}
        string(const char* s) : str_(s) {}
        const char* c_str() const { return str_; }
    };

} // namespace std
