	
	#include <intrin.h>
	//#include <xmmintrin.h>
	//#include <emmintrin.h>
	//#include <immintrin.h>
	//#include <tmmintrin.h>
		
namespace simd {
	
	// _mm_set_ps has it arguments like this: _mm_set_ps(w,z,y,x), to avoid confusion I do: SET_PS(x,y,z,w)
	
	enum : s8 {
		X=0, Y=1, Z=2, W=3,
		_x=0, _y=1, _z=2, _w=3
	};
	
	#define _SHUF(a, b, c, d) (d << 6)|(c << 4)|(b << 2)|(a << 0)
	
	#define SET_PS(a, b, c, d)						_mm_set_ps(d, c, b, a)
	#define SET_EPI32(a, b, c, d)					_mm_set_epi32(d, c, b, a)
	#define SHUF_PS(lower, upper, a, b, c, d)		_mm_shuffle_ps(lower, upper, _SHUF(a,b,c,d))
	#define SHUF_EPI32(val, a, b, c, d)				_mm_shuffle_epi32(val, _SHUF(a,b,c,d))
	
	FORCEINLINE __m128i get_epi8_shuf_mask (si a, si b, si c, si d, si e, si f, si g, si h,
											si i, si j, si k, si l, si m, si n, si o, si p) {
		return _mm_set_epi8(p,o,n,m, l,k,j,i, h,g,f,e, d,c,b,a);
	}
	
	FORCEINLINE __m128 madd (__m128 a, __m128 b, __m128 c) {
		return _mm_add_ps( _mm_mul_ps(a, b), c );
		//return _mm_fmadd_ps( a, b, c );
	}
	FORCEINLINE __m128 sel (__m128 a, __m128 b, __m128 mask) {
		return _mm_or_ps( _mm_andnot_ps(mask, a), _mm_and_ps(mask, b) );
	}
	
//////////
	
	//constexpr u32 _sse_sign_bit[4] = {		0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	//constexpr u32 _sse_not_sign_bit[4] = {	0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	
	#if 0 // These generate actual mov int->sse instructions
	DECL FORCEINLINE __m128 sign_bit_ss () {		return _mm_castsi128_ps( _mm_cvtsi32_si128(0x80000000) ); }
	DECL FORCEINLINE __m128 not_sign_bit_ss () {	return _mm_castsi128_ps( _mm_cvtsi32_si128(0x7fffffff) ); };
	
	DECL FORCEINLINE __m128d sign_bit_sd () {		return _mm_castsi128_pd( _mm_cvtsi64_si128(0x8000000000000000) ); }
	DECL FORCEINLINE __m128d not_sign_bit_sd () {	return _mm_castsi128_pd( _mm_cvtsi64_si128(0x7fffffffffffffff) ); };
	
	#elif 0 // These generate mov constant_mem->sse mov instructions, which is the same as the
			//	ps&pd functions below, so just use those even if you only need ss|sd, since this will save some constant data space
	DECL FORCEINLINE __m128 sign_bit_ss () {		return _mm_castsi128_ps( _mm_set_epi32(0x80000000,0,0,0) ); }
	DECL FORCEINLINE __m128 not_sign_bit_ss () {	return _mm_castsi128_ps( _mm_set_epi32(0x7fffffff,0,0,0) ); };
	
	DECL FORCEINLINE __m128d sign_bit_sd () {		return _mm_castsi128_pd( _mm_set_epi64x(0x8000000000000000,0) ); }
	DECL FORCEINLINE __m128d not_sign_bit_sd () {	return _mm_castsi128_pd( _mm_set_epi64x(0x7fffffffffffffff,0) ); };
	
	#endif
	
	DECL FORCEINLINE __m128 sign_bit_ps () {		return _mm_castsi128_ps( _mm_set1_epi32(0x80000000) ); }
	DECL FORCEINLINE __m128 not_sign_bit_ps () {	return _mm_castsi128_ps( _mm_set1_epi32(0x7fffffff) ); };
	
	DECL FORCEINLINE __m128d sign_bit_pd () {		return _mm_castsi128_pd( _mm_set1_epi64x(0x8000000000000000) ); }
	DECL FORCEINLINE __m128d not_sign_bit_pd () {	return _mm_castsi128_pd( _mm_set1_epi64x(0x7fffffffffffffff) ); };
	
	// unfortunately _mm_set_ss zero's the upper values even though we don't care about those
	#define TOs(f)		_mm_set_ss(f)
	#define TOd(f)		_mm_set_sd(f)
	#define FROMs(sse)	_mm_cvtss_f32(sse)
	#define FROMd(sse)	_mm_cvtsd_f64(sse)
	
	
	FORCEINLINE DECL __m128 ABS (__m128 f) {
		return _mm_and_ps(f, not_sign_bit_ps());
	}
	FORCEINLINE DECL __m128d ABS (__m128d f) {
		return _mm_and_pd(f, not_sign_bit_pd());
	}
	FORCEINLINE DECL f32 ABS (f32 f) {
		return FROMs( ABS( TOs(f) ) );
		//u32 i = reint_flt_as_int(f);
		//i &= 0x7fffffff;
		//return reint_int_as_flt(i);
	}
	FORCEINLINE DECL f64 ABS (f64 f) {
		return FROMd( ABS( _mm_set_sd(f) ) );
		//u64 i = reint_flt_as_int(f);
		//i &= 0x7fffffffffffffffull;
		//return reint_int_as_flt(i);
	}
	
	
	FORCEINLINE DECL __m128 MIN (__m128 a, __m128 b) {
		return _mm_min_ps(a, b);
	}
	FORCEINLINE DECL __m128 MAX (__m128 a, __m128 b) {
		return _mm_max_ps(a, b);
	}
	
	FORCEINLINE DECL f32 MIN (f32 a, f32 b) {
		return FROMs( _mm_min_ps(TOs(a), TOs(b)) );
	}
	FORCEINLINE DECL f32 MAX (f32 a, f32 b) {
		return FROMs( _mm_max_ps(TOs(a), TOs(b)) );
	}
	
	FORCEINLINE DECL __m128 clamp (__m128 val, __m128 min, __m128 max) {
		return MIN(MAX(val, min), max);
	}
	
	FORCEINLINE DECL f32 clamp (f32 val, f32 min, f32 max) {
		return FROMs( clamp(TOs(val), TOs(min), TOs(max)) );
	}
	
	////
	FORCEINLINE DECL __m128 sqrt (__m128 val) {
		return _mm_sqrt_ps(val);
	}
	FORCEINLINE DECL f32 sqrt (f32 val) {
		return FROMs( _mm_sqrt_ss( TOs(val) ) );
	}
	FORCEINLINE DECL f64 sqrt (f64 val) {
		// Both of these add extra zero'ing and mov instructions to zero the upper double, which is unfortunate
		//return _mm_cvtsd_f64( _mm_sqrt_pd( _mm_set_sd(val) ) ); // This verion should result in the same amout of mov/zero overhead, but the sqrtpd is slower on some processors (Broadwell (20/7) vs (20/13)) although it's faster on Knights Landing
		return FROMd( _mm_sqrt_sd( _mm_setzero_pd(), _mm_set_sd(val) ) );
	}
	
	#if 0
	DECL f32 abs_get_sign (f32 f, f32* out_sign) {
		u32 i = reint_flt_as_int(f);
		u32 s = i & 0x80000000;
		u32 a = i & 0x7fffffff;
		*out_sign = reint_int_as_flt(s);
		return reint_int_as_flt(a);
	}
	#endif
	
	DECL __m128 get_sign (__m128 f) {
		return _mm_and_ps(f, sign_bit_ps());
	}
	DECL __m128d get_sign (__m128d f) {
		return _mm_and_pd(f, sign_bit_pd());
	}
	DECL f32 get_sign (f32 f) {
		return FROMs( get_sign( TOs(f) ) );
	}
	DECL f64 get_sign (f64 f) {
		return FROMd( get_sign( TOd(f) ) );
	}
	
	DECL __m128 set_sign (__m128 f, __m128 sign) {
		f = _mm_and_ps(f, not_sign_bit_ps());
		return _mm_or_ps(f, sign);
	}
	DECL __m128d set_sign (__m128d f, __m128d sign) {
		f = _mm_and_pd(f, not_sign_bit_pd());
		return _mm_or_pd(f, sign);
	}
	DECL f32 set_sign (f32 f, f32 sign) {
		return FROMs( set_sign( TOs(f), TOs(sign) ) );
	}
	DECL f64 set_sign (f64 f, f64 sign) {
		return FROMd( set_sign( TOd(f), TOd(sign) ) );
	}
	
	DECL __m128 or_sign (__m128 f, __m128 sign) {
		return _mm_or_ps(f, sign);
	}
	DECL __m128d or_sign (__m128d f, __m128d sign) {
		return _mm_or_pd(f, sign);
	}
	DECL f32 or_sign (f32 f, f32 sign) {
		return FROMs( or_sign( TOs(f), TOs(sign) ) );
	}
	DECL f64 or_sign (f64 f, f64 sign) {
		return FROMd( or_sign( TOd(f), TOd(sign) ) );
	}
	
	#undef TOs
	#undef TOd
	#undef FROMs
	#undef FROMd
	
}
	