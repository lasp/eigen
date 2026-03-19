#ifndef EIGEN_FREESTANDING_MATH_H
#define EIGEN_FREESTANDING_MATH_H

// Detect GCC freestanding mode
#if defined(__GNUC__) && defined(__cplusplus)
#  if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 0
//#    pragma message "Eigen in freestanding mode - using C math header only"
#    define _GLIBCXX_INCLUDE_NEXT_C_HEADERS
extern "C" {
#    include <math.h>
}
    // Move some functions into standard namespace to be more cmath-like for Eigen.
    namespace std {

        using ::sqrt;
        using ::log;
        using ::exp;
        using ::pow;

        inline double abs(double x) { return ::fabs(x); }
        inline float abs(float x) { return ::fabsf(x); }

        inline float rintf( float arg ) {return ::rintf(arg);}
        inline double rint( double arg ) {return ::rint(arg);}
        inline long double rintl( long double arg ) {return ::rintl(arg);}

        inline float roundf( float arg ) {return ::roundf(arg);}
        inline double round( double arg ) {return ::round(arg);}
        inline long double roundl( long double arg ) {return ::roundl(arg);}

        inline float expm1(float x) { return ::expm1f(x); }
        inline double expm1(double x) { return ::expm1(x); }
        inline long double expm1(long double x) { return ::expm1l(x); }

        inline float log1p(float x) { return ::log1pf(x); }
        inline double log1p(double x) { return ::log1p(x); }
        inline long double log1p(long double x) { return ::log1pl(x); }

        inline float acosh(float x) { return ::acoshf(x); }
        inline double acosh(double x) { return ::acosh(x); }
        inline long double acosh(long double x) { return ::acoshl(x); }

        inline float asinh(float x) { return ::asinhf(x); }
        inline double asinh(double x) { return ::asinh(x); }
        inline long double asinh(long double x) { return ::asinhl(x); }

        inline float atanh(float x) { return ::atanhf(x); }
        inline double atanh(double x) { return ::atanh(x); }
        inline long double atanh(long double x) { return ::atanhl(x); }

        inline double log10(double x) { return ::log10(x); }
        inline float log10(float x) { return ::log10f(x); }

        inline double floor(double x) { return ::floor(x); }
        inline float floor(float x) { return ::floorf(x); }

        inline double ceil(double x) { return ::ceil(x); }
        inline float ceil(float x) { return ::ceilf(x); }

        inline double frexp(double x, int* exp) { return ::frexp(x, exp); }
        inline float frexp(float x, int* exp) { return ::frexpf(x, exp); }

        inline double ldexp(double x, int exp) { return ::ldexp(x, exp); }
        inline float ldexp(float x, int exp) { return ::ldexpf(x, exp); }

        inline double tan(double x) { return ::tan(x); }
        inline float tan(float x) { return tanf(x); }

        inline double sin(double x) { return ::sin(x); }
        inline float sin(float x) { return sinf(x); }

        inline double cos(double x) { return ::cos(x); }
        inline float cos(float x) { return cosf(x); }

        inline double tanh(double x) { return ::tanh(x); }
        inline float tanh(float x) { return tanhf(x); }

        inline double sinh(double x) { return ::sinh(x); }
        inline float sinh(float x) { return sinhf(x); }

        inline double cosh(double x) { return ::cosh(x); }
        inline float cosh(float x) { return coshf(x); }

        inline double atan(double x) { return ::atan(x); }
        inline float atan(float x) { return atanf(x); }

        inline double atan2(double x, double y) { return ::atan2(x,y); }
        inline float atan2(float x, float y) { return atan2f(x,y); }

        inline double asin(double x) { return ::asin(x); }
        inline float asin(float x) { return asinf(x); }

        inline double acos(double x) { return ::acos(x); }
        inline float acos(float x) { return acosf(x); }

        inline double fmod(double numer, double denom) { return ::fmod(numer, denom); }
        inline float fmod(float numer, float denom) { return fmodf(numer, denom); }

        // conj() stubs for freestanding mode.
        // Eigen's BDCSVD.h uses "using std::conj;" in a template body that GCC 15
        // checks even when not instantiated (-Wtemplate-body). For real scalar
        // types (the only types used in freestanding builds), conj is identity.
        inline float conj(float x) { return x; }
        inline double conj(double x) { return x; }
        inline long double conj(long double x) { return x; }
    }
#    undef _GLIBCXX_INCLUDE_NEXT_C_HEADERS
#  else
//#    pragma message "Eigen in hosted mode - using standard <cmath>"
#    include <cmath>
#  endif
#else
    // Non-GCC
    static_assert(false, "Freestanding Eigen compilation is only supported with GCC (and variants).");
#endif

#endif // EIGEN_FREESTANDING_MATH_H
