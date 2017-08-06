
#include <cmath>

namespace fp {
	
	////
	DECL f32 sin (f32 ang) {
		return ::sinf(ang);
	}
	DECL f64 sin (f64 ang) {
		return ::sin(ang);
	}
	
	DECL f32 cos (f32 ang) {
		return ::cosf(ang);
	}
	DECL f64 cos (f64 ang) {
		return ::cos(ang);
	}
	
	DECL f32 tan (f32 ang) {
		return ::tanf(ang);
	}
	DECL f64 tan (f64 ang) {
		return ::tan(ang);
	}
	
	////
	DECL f32 asin (f32 val) {
		return ::asinf(val);
	}
	DECL f64 asin (f64 val) {
		return ::asin(val);
	}
	
	DECL f32 acos (f32 val) {
		return ::acosf(val);
	}
	DECL f64 acos (f64 val) {
		return ::acos(val);
	}
	
	DECL f32 atan (f32 val) {
		return ::atanf(val);
	}
	DECL f64 atan (f64 val) {
		return ::atan(val);
	}
	
	DECL f32 atan2 (f32 y, f32 x) {
		return ::atan2f(y, x);
	}
	DECL f64 atan2 (f64 y, f64 x) {
		return ::atan2(y, x);
	}
	
	////
	DECL f32 sqrt (f32 val) {
		return ::sqrtf(val);
	}
	DECL f64 sqrt (f64 val) {
		return ::sqrt(val);
	}
	
	DECL f32 mod (f32 val, f32 denom) {
		return ::fmodf(val, denom);
	}
	
	DECL f32 pow (f32 val, f32 exp) {
		return ::powf(val, exp);
	}
	DECL f64 pow (f64 val, f64 exp) {
		return ::pow(val, exp); // This generates this error on msvc: 'unresolved external symbol __vdecl_pow2 referenced'
										//	no idea where this is coming from or how to fix it
	}
	
	DECL f32 exp (f32 exponent) {
		return ::expf(exponent);
	}
	
	DECL f32 log (f32 val) {
		return ::logf(val);
	}
	
	DECL f64 ldexp (f64 val, int exp) {
		return ::ldexp(val, exp);
	}
	
	DECL f32 round (f32 val) {
		return ::roundf(val);
	}
	DECL f64 round (f64 val) {
		return ::round(val);
	}
	DECL s32 iround (f32 val) {
		return ::lroundf(val);
	}
	DECL s64 iround (f64 val) {
		return ::llround(val);
	}
	
	DECL f32 floor (f32 val) {
		return ::floorf(val);
	}
	
	DECL f32 ceil (f32 val) {
		return ::ceilf(val);
	}
	
	DECL f32 trunc (f32 val) {
		return ::truncf(val);
	}
	
	DECL f32 ABS (f32 val) {
		return ::fabsf(val);
	}
	DECL f64 ABS (f64 val) {
		return ::fabs(val);
	}
	
	using simd::MIN;
	using simd::MAX;
	//using simd::ABS;
	using simd::clamp;
	using simd::set_sign;
	using simd::get_sign;
	using simd::or_sign;
	
	DECL constexpr f32 lerp (f32 start, f32 end, f32 alpha) {
		return (start * (1.0f -alpha)) +(end * alpha);
	}
	
	template <typename T> DECL FORCEINLINE T inf ();
	template<> FORCEINLINE constexpr f32 inf<f32> () {
		return BUILTIN_F32_INF;
	}
	template<> FORCEINLINE constexpr f64 inf<f64> () {
		return BUILTIN_F32_INF;
	}
	
	template <typename T> DECL FORCEINLINE T qnan ();
	template<> FORCEINLINE constexpr f32 qnan<f32> () {
		return BUILTIN_F32_QNAN;
	}
	template<> FORCEINLINE constexpr f64 qnan<f64> () {
		return BUILTIN_F32_QNAN;
	}
	
	template <typename CAST_T, typename T> DECL FORCEINLINE CAST_T cast_flt (T i) {
		STATIC_ASSERT(types::isFloatingPoint<CAST_T>());
		STATIC_ASSERT(types::isInteger<T>() || types::isFloatingPoint<CAST_T>());
		return static_cast<CAST_T>(i);
	}
	
	template <typename CAST_T, typename T> DECL FORCEINLINE CAST_T cast_trunc (T f) {
		STATIC_ASSERT(types::isInteger<CAST_T>());
		STATIC_ASSERT(types::isFloatingPoint<T>());
		return static_cast<CAST_T>(f);
	}
	
	struct SC {
		f32 s;
		f32 c;
	};
	DECL SC sincos (f32 ang) {
		return { sin(ang), cos(ang) };
	}
	
	struct SCd {
		f64 s;
		f64 c;
	};
	DECL SCd sincos (f64 ang) {
		return { sin(ang), cos(ang) };
	}
	
	DECL f32 proper_mod (f32 val, f32 denom) {
		f32 m = ::fmodf(val, denom);
		if (denom >= 0.0f) {
			if (m < 0.0f) {
				m += denom;
			}
		} else {
			if (m > 0.0f) {
				m -= denom;
			}
		}
		return m;
	}
	
	#if 0
	template <typename T>
	DECL T strtof (char const* str, char** endptr);
	
	template<> f64 cstrtof<f64> (char const* str, char** endptr) {
		#if _COMP == _COMP_GCC
		while ((*str >= '0' && *str <= '9') || *str == '.' || *str == '+' || *str == '-') { ++str; }
		*endptr = (char*)str;
		return 0.0;
		#else
		return ::strtod(str, endptr);
		#endif
	}
	template<> f32 cstrtof<f32> (char const* str, char** endptr) {
		#if _COMP == _COMP_GCC
		while ((*str >= '0' && *str <= '9') || *str == '.' || *str == '+' || *str == '-') { ++str; }
		*endptr = (char*)str;
		return 0.0f;
		#else
		return static_cast<f32>(strtof<f64>(str, endptr));
		#endif
	}
	#endif
	
}
