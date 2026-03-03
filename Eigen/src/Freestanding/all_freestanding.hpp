#ifndef ALL_FREESTANDING_HPP
#define ALL_FREESTANDING_HPP

// GCC 15 introduced -Wtemplate-body which checks template definitions even
// when they are never instantiated. In freestanding mode the standard library
// is intentionally incomplete, so many Eigen template paths reference types
// and functions that do not exist (std::complex, half, bfloat16,
// Eigen::internal::random, etc.). These paths are never used at runtime in
// freestanding builds. Suppress the diagnostic globally for freestanding
// compilations since this header is -include'd before all translation units.
#if defined(__GNUC__) && (__GNUC__ >= 15)
#pragma GCC diagnostic ignored "-Wtemplate-body"
#endif

#include "freestanding_fpclassify.h"
#include "freestanding_math.h"
#include "freestanding_string.h"
#include "portable_stdlib.h"

#endif //ALL_FREESTANDING_HPP
