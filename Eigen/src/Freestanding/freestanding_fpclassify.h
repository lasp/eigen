#ifndef EIGEN_FREESTANDING_FPCLASSIFY_H
#define EIGEN_FREESTANDING_FPCLASSIFY_H

#if defined(__GNUC__) && defined(__cplusplus) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)

#include <type_traits>

namespace std {

// Only define if missing
template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type
isfinite(T x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_isfinite(x);
#else
    return !(x != x || x == (1.0 / 0.0) || x == -(1.0 / 0.0));
#endif
}

template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type
isnan(T x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_isnan(x);
#else
    return x != x;
#endif
}

template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type
isinf(T x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_isinf(x);
#else
    return x == (1.0 / 0.0) || x == -(1.0 / 0.0);
#endif
}

} // namespace std

#else

#endif // // __STDC_HOSTED__

#endif // EIGEN_FREESTANDING_FPCLASSIFY_H
