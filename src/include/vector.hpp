	
// TODO: maybe make vp const& on non-vectorcall systems
#define vp const
#define mp const&
	
#define V_BY_VAL 1
#define M_BY_VAL 0

#define DECL_	DECL
#define DECLT_	DECLT
#define DECLM_	DECLM

#define SSE_OPTIMIZE 0

////// Vector
	
	#define COMPOUND(VT, OP) \
			template <typename T>	DECL_ constexpr VT&		operator OP##= (VT& l, VT vp r) { \
				return l = BINARY_(VT, OP); \
			}
			
	#define UNARY(VT, OP) \
			template <typename T>	DECL_ constexpr VT		operator OP (VT vp v) { \
				return UNARY_(VT, OP); \
			}
			
	#define BINARY(VT, OP) \
			template <typename T>	DECL_ constexpr VT		operator OP (VT vp l, VT vp r) { \
				return BINARY_(VT, OP); \
			}
			
	#define COMP(VT) \
			template <typename T>	DECL_ constexpr bool	identical (VT vp l, VT vp r) { \
				return COMP_(VT, ==, &&); \
			}
	#define COMP_TO_BV(VT, V, OP) \
			template <typename T>	DECL_ constexpr V<bool> operator OP (VT vp l, VT vp r) { \
				return BINARY_(V<bool>, OP); \
			}
	#define COMP_BV_BINARY(V, OP) \
									DECL_ constexpr V<bool> operator OP (V<bool> vp l, V<bool> vp r) { \
				return BINARY_(V<bool>, OP); \
			}
	#define COMP_BV_UNARY(V, OP) \
									DECL_ constexpr V<bool> operator OP (V<bool> vp v) { \
				return UNARY_(V<bool>, OP); \
			}
	#define COMP_BV_COMBINE(V, OP_NAME, AC) \
									DECLM_ constexpr bool	OP_NAME (V<bool> vp v) { \
				return COMP_BV_COMBINE_(V<bool>, AC); \
			} \
			template<>				DECLT_	constexpr bool	V<bool>::OP_NAME () const { \
				return vec::OP_NAME(*this); \
			}
	
	#define OPS(VT, V) \
			COMPOUND(VT, +) \
			COMPOUND(VT, -) \
			COMPOUND(VT, *) \
			COMPOUND(VT, /) \
			UNARY(VT, +) \
			UNARY(VT, -) \
			BINARY(VT, +) \
			BINARY(VT, -) \
			BINARY(VT, *) \
			BINARY(VT, /) \
			COMP(VT) \
			COMP_TO_BV(VT, V, <) \
			COMP_TO_BV(VT, V, >) \
			COMP_TO_BV(VT, V, <=) \
			COMP_TO_BV(VT, V, >=) \
			COMP_BV_BINARY(V, &&) \
			COMP_BV_BINARY(V, ||) \
			COMP_BV_UNARY(V, !)
	
	#define STRUCT_OPS(VT, V) \
			COMP_BV_COMBINE(V, all, &&) \
			COMP_BV_COMBINE(V, any, ||)
	
	#define SWIZZLE_2(V2) \
			constexpr V2 xy () const { \
				return *this; \
			} \
			V2& xy (V2 vp r) { \
				return *this = r; \
			}
	#define SWIZZLE_3(V2, V3) \
			constexpr V2 xy () const { \
				return V2(x, y); \
			} \
			V3& xy (V2 vp r) { \
				x = r.x; \
				y = r.y; \
				return *this; \
			} \
			constexpr V3 xyz () const { \
				return *this; \
			} \
			V3& xyz (V3 vp r) { \
				return *this = r; \
			}
	#define SWIZZLE_4(V2, V3, V4) \
			constexpr V2 xy () const { \
				return V2(x, y); \
			} \
			V4& xy (V2 vp r) { \
				x = r.x; \
				y = r.y; \
				return *this; \
			} \
			constexpr V3 xyz () const { \
				return V3(x, y, z); \
			} \
			V4& xyz (V3 vp r) { \
				x = r.x; \
				y = r.y; \
				z = r.z; \
				return *this; \
			} \
			constexpr V4 xyzw () const { \
				return *this; \
			} \
			V4& xyzw (V4 vp r) { \
				return *this = r; \
			} \
			 \
			constexpr V2 ww () const { \
				return V2(w); \
			} \
			constexpr V3 www () const { \
				return V3(w); \
			}
	
namespace vec {
	
	template <typename T>
	struct tv2 {
		typedef T		t;
		
		T				x;
		T				y;
		
		constexpr tv2 () = default;
		
		constexpr explicit tv2 (T all):		x{all}, y{all} {}
		constexpr tv2 (T x_, T y_):			x{x_},	y{y_} {}
		
		constexpr T operator[] (uptr indx) const {
			return *(&x +indx);
		}
		T& operator[] (uptr indx) {
			return *(&x +indx);
		}
		
		DECLM_ constexpr bool all () const;
		DECLM_ constexpr bool any () const;
		
		SWIZZLE_2(tv2)
	};
	
	#undef COMP_BV_COMBINE_
	#define COMP_BV_COMBINE_(VT, AC)		v.x AC			v.y
	
	STRUCT_OPS(tv2<T>, tv2)
	
	template <typename T>
	struct tv3 {
		typedef T		t;
		
		T				x;
		T				y;
		T				z;
		
		constexpr tv3 () = default;
		
		constexpr explicit tv3 (T all):					x{all}, y{all}, z{all} {}
		constexpr tv3 (T x_, T y_, T z_):				x{x_},	y{y_},	z{z_} {}
		
		constexpr explicit tv3 (tv2<T> vp v, T z_):		x{v.x}, y{v.y}, z{z_} {}
		
		constexpr T operator[] (uptr indx) const {
			return *(&x +indx);
		}
		T& operator[] (uptr indx) {
			return *(&x +indx);
		}
		
		DECLM_ constexpr bool all () const;
		DECLM_ constexpr bool any () const;
		
		SWIZZLE_3(tv2<T>, tv3)
	};
	
	#undef COMP_BV_COMBINE_
	#define COMP_BV_COMBINE_(VT, AC)		v.x AC			v.y AC			v.z
	
	STRUCT_OPS(tv3<T>, tv3)
	
	template <typename T>
	struct tv4 {
		typedef T		t;
		
		T				x;
		T				y;
		T				z;
		T				w;
		
		constexpr tv4 () = default;
		
		constexpr explicit tv4 (T all):						x{all}, y{all}, z{all}, w{all} {}
		constexpr tv4 (T x_, T y_, T z_, T w_):				x{x_},	y{y_},	z{z_},	w{w_} {}
		
		constexpr explicit tv4 (tv2<T> vp v, T z_, T w_):	x{v.x}, y{v.y}, z{z_},	w{w_} {}
		constexpr explicit tv4 (tv3<T> vp v, T w_):			x{v.x}, y{v.y}, z{v.z}, w{w_} {}
		
		constexpr T operator[] (uptr indx) const {
			return *(&x +indx);
		}
		T& operator[] (uptr indx) {
			return *(&x +indx);
		}
		
		DECLM_ constexpr bool all () const;
		DECLM_ constexpr bool any () const;
		
		SWIZZLE_4(tv2<T>, tv3<T>, tv4)
	};
	
	#undef COMP_BV_COMBINE_
	#define COMP_BV_COMBINE_(VT, AC)		v.x AC			v.y AC			v.z AC			v.w
	
	STRUCT_OPS(tv4<T>, tv4)
	
	#undef COMP_BV_COMBINE_
	
	
	template <typename T>
	struct thv {
		typedef T		t;
		
		T				x;
		T				y;
		T				z;
		// Implicit w = 1
		
		constexpr thv () = default;
		
		constexpr explicit thv (T all):				x{all}, y{all}, z{all} {}
		constexpr thv (T x_, T y_, T z_):			x{x_},	y{y_},	z{z_} {}
		
		constexpr thv (tv3<T> vp v):				x{v.x}, y{v.y}, z{v.z} {}
		
		constexpr T operator[] (uptr indx) const {
			return *(&x +indx);
		}
		T& operator[] (uptr indx) {
			return *(&x +indx);
		}
		
		constexpr tv2<T> xy () const {
			return tv2<T>(x, y);
		}
		thv& xy (tv2<T> vp r) {
			x = r.x;
			y = r.y;
			return *this;
		}
		constexpr tv3<T> xyz () const {
			return tv3<T>(x, y, z);
		}
		thv& xyz (tv3<T> vp r) {
			x = r.x;
			y = r.y;
			z = r.z;
			return *this;
		}
		constexpr tv4<T> xyzw () const {
			return tv4<T>(x, y, z, 1);
		}
	};
}
	
using vec::tv2;
using vec::tv3;
using vec::tv4;
using vec::thv;
	
typedef vec::tv2<f32> v2;
typedef vec::tv3<f32> v3;
typedef vec::tv4<f32> v4;
typedef vec::thv<f32> hv;

using vec::any;
using vec::all;
	
namespace vec_op {
	
	#undef BINARY_
	#undef UNARY_
	#undef COMP_
	#define BINARY_(VT, OP)				VT( l.x OP r.x,		l.y OP r.y )
	#define UNARY_(VT, OP)				VT( OP v.x,			OP v.y )
	#define COMP_(VT, OP, AC)				l.x OP r.x AC	l.y OP r.y
	
	OPS(vec::tv2<T>, vec::tv2)
	
	#undef BINARY_
	#undef UNARY_
	#undef COMP_
	#define BINARY_(VT, OP)				VT( l.x OP r.x,		l.y OP r.y,		l.z OP r.z )
	#define UNARY_(VT, OP)				VT( OP v.x,			OP v.y,			OP v.z )
	#define COMP_(VT, OP, AC)				l.x OP r.x AC	l.y OP r.y AC	l.z OP r.z
	
	OPS(tv3<T>, tv3)
	
	#undef BINARY_
	#undef UNARY_
	#undef COMP_
	#define BINARY_(VT, OP)				VT( l.x OP r.x,		l.y OP r.y,		l.z OP r.z,		l.w OP r.w )
	#define UNARY_(VT, OP)				VT( OP v.x,			OP v.y,			OP v.z,			OP v.w )
	#define COMP_(VT, OP, AC)				l.x OP r.x AC	l.y OP r.y AC	l.z OP r.z AC	l.x OP r.z
	
	OPS(tv4<T>, tv4)
	
	#undef BINARY_
	#undef UNARY_
	#undef COMP_
	
	#define DOT_2(l, r)		(l.x * r.x) +(l.y * r.y)
	#define DOT_3(l, r)		((l.x * r.x) +(l.y * r.y)) +(l.z * r.z)
	#define DOT_4(l, r)		((l.x * r.x) +(l.y * r.y)) +((l.z * r.z) +(l.w * r.w))
	#define LEN_SQR_2(v)	(v.x * v.x) +(v.y * v.y)
	#define LEN_SQR_3(v)	((v.x * v.x) +(v.y * v.y)) +(v.z * v.z)
	#define LEN_SQR_4(v)	((v.x * v.x) +(v.y * v.y)) +((v.z * v.z) +(v.w * v.w))
	
	template <typename T>	DECL_ T dot (tv2<T> vp l, tv2<T> vp r) {	return DOT_2(l, r); }
	template <typename T>	DECL_ T dot (tv3<T> vp l, tv3<T> vp r) {	return DOT_3(l, r); }
	template <typename T>	DECL_ T dot (tv4<T> vp l, tv4<T> vp r) {	return DOT_4(l, r); }
	
	// project p onto v
	template <typename T>	DECL_ tv2<T> dot_onto (tv2<T> vp v, tv2<T> vp p) {	return tv2<T>(DOT_2(v, p)) * v; }
	template <typename T>	DECL_ tv3<T> dot_onto (tv3<T> vp v, tv3<T> vp p) {	return tv3<T>(DOT_3(v, p)) * v; }
	template <typename T>	DECL_ tv4<T> dot_onto (tv4<T> vp v, tv4<T> vp p) {	return tv4<T>(DOT_4(v, p)) * v; }
	
	template <typename T>	DECL_ T length_sqr (tv2<T> vp v) {			return LEN_SQR_2(v); }
	template <typename T>	DECL_ T length_sqr (tv3<T> vp v) {			return LEN_SQR_3(v); }
	template <typename T>	DECL_ T length_sqr (tv4<T> vp v) {			return LEN_SQR_4(v); }
	
	template <typename T>	DECL_ T length (tv2<T> vp v) {				return fp::sqrt(length_sqr(v)); }
	template <typename T>	DECL_ T length (tv3<T> vp v) {				return fp::sqrt(length_sqr(v)); }
	template <typename T>	DECL_ T length (tv4<T> vp v) {				return fp::sqrt(length_sqr(v)); }
	
	template <typename T>	DECL_ tv2<T> normalize (tv2<T> vp v) {		return tv2<T>( v / tv2<T>(length(v)) ); }
	template <typename T>	DECL_ tv3<T> normalize (tv3<T> vp v) {		return tv3<T>( v / tv3<T>(length(v)) ); }
	template <typename T>	DECL_ tv4<T> normalize (tv4<T> vp v) {		return tv4<T>( v / tv4<T>(length(v)) ); }
	
	template <typename T>	DECL_ tv3<T> cross (tv3<T> vp l, tv3<T> vp r) {
		
		T yz = l.y * r.z;
		T zy = l.z * r.y;
		T zx = l.z * r.x;
		T xz = l.x * r.z;
		T xy = l.x * r.y;
		T yx = l.y * r.x;
		
		tv3<T> ret;
		ret.x = yz -zy;
		ret.y = zx -xz;
		ret.z = xy -yx;
		return ret;
	}
	
	template <typename T>	DECL_ T distance_sqr (tv2<T> vp a, tv2<T> vp b) {	return length_sqr(a -b); }
	template <typename T>	DECL_ T distance_sqr (tv3<T> vp a, tv3<T> vp b) {	return length_sqr(a -b); }
	template <typename T>	DECL_ T distance_sqr (tv4<T> vp a, tv4<T> vp b) {	return length_sqr(a -b); }
	
	template <typename T>	DECL_ T distance (tv2<T> vp a, tv2<T> vp b) {		return length(a -b); }
	template <typename T>	DECL_ T distance (tv3<T> vp a, tv3<T> vp b) {		return length(a -b); }
	template <typename T>	DECL_ T distance (tv4<T> vp a, tv4<T> vp b) {		return length(a -b); }
}
using vec_op::operator += ;
using vec_op::operator -= ;
using vec_op::operator *= ;
using vec_op::operator /= ;
using vec_op::operator +  ;
using vec_op::operator -  ;
using vec_op::operator *  ;
using vec_op::operator /  ;
using vec_op::identical	  ;
using vec_op::operator <  ;
using vec_op::operator >  ;
using vec_op::operator <= ;
using vec_op::operator >= ;
using vec_op::operator && ;
using vec_op::operator || ;
using vec_op::operator !  ;

using vec_op::dot;
using vec_op::dot_onto;
using vec_op::length_sqr;
using vec_op::length;
using vec_op::normalize;
using vec_op::cross;
using vec_op::distance;

namespace vec_util {
	// Asserted cast
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v2 (T x, T y) {
		return T_CAST(	safe_cast_assert(typename T_CAST::t, x),
						safe_cast_assert(typename T_CAST::t, y) );
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v3 (T x, T y, T z) {
		return T_CAST(	safe_cast_assert(typename T_CAST::t, x),
						safe_cast_assert(typename T_CAST::t, y),
						safe_cast_assert(typename T_CAST::t, z) );
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v4 (T x, T y, T z, T w) {
		return T_CAST(	safe_cast_assert(typename T_CAST::t, x),
						safe_cast_assert(typename T_CAST::t, y),
						safe_cast_assert(typename T_CAST::t, z),
						safe_cast_assert(typename T_CAST::t, w) );
	}
	
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v2 (T vp v) {
		return safe_cast_assert_v2<T_CAST>(v.x, v.y);
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v3 (T vp v) {
		return safe_cast_assert_v3<T_CAST>(v.x, v.y, v.z);
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST safe_cast_assert_v4 (T vp v) {
		return safe_cast_assert_v4<T_CAST>(v.x, v.y, v.z, v.w);
	}
	
	// Unasserted cast
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v2 (T x, T y) {
		return T_CAST(	(typename T_CAST::t)x,
						(typename T_CAST::t)y );
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v3 (T x, T y, T z) {
		return T_CAST(	(typename T_CAST::t)x,
						(typename T_CAST::t)y,
						(typename T_CAST::t)z );
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v4 (T x, T y, T z, T w) {
		return T_CAST(	(typename T_CAST::t)x,
						(typename T_CAST::t)y,
						(typename T_CAST::t)z,
						(typename T_CAST::t)w );
	}
	
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v2 (T vp v) {
		return cast_v2<T_CAST>(v.x, v.y);
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v3 (T vp v) {
		return cast_v3<T_CAST>(v.x, v.y, v.z);
	}
	template<typename T_CAST, typename T>	DECL_ constexpr T_CAST cast_v4 (T vp v) {
		return cast_v4<T_CAST>(v.x, v.y, v.z, v.w);
	}
	
	//// Vectorized math functions
	template <typename T> DECL_ constexpr tv2<T> MAX_v (tv2<T> vp l, tv2<T> vp r) {
		return tv2<T>(	MAX(l.x, r.x), MAX(l.y, r.y) );
	}
	template <typename T> DECL_ constexpr tv3<T> MAX_v (tv3<T> vp l, tv3<T> vp r) {
		return tv3<T>(	MAX(l.x, r.x), MAX(l.y, r.y), MAX(l.z, r.z) );
	}
	template <typename T> DECL_ constexpr tv4<T> MAX_v (tv4<T> vp l, tv4<T> vp r) {
		return tv4<T>(	MAX(l.x, r.x), MAX(l.y, r.y), MAX(l.z, r.z), MAX(l.w, r.w) );
	}
	
	template <typename T> DECL_ constexpr tv2<T> MIN_v (tv2<T> vp l, tv2<T> vp r) {
		return tv2<T>(	MIN(l.x, r.x), MIN(l.y, r.y) );
	}
	template <typename T> DECL_ constexpr tv3<T> MIN_v (tv3<T> vp l, tv3<T> vp r) {
		return tv3<T>(	MIN(l.x, r.x), MIN(l.y, r.y), MIN(l.z, r.z) );
	}
	template <typename T> DECL_ constexpr tv4<T> MIN_v (tv4<T> vp l, tv4<T> vp r) {
		return tv4<T>(	MIN(l.x, r.x), MIN(l.y, r.y), MIN(l.z, r.z), MIN(l.w, r.w) );
	}
	
	template <typename T> DECL_ tv2<T>		normalize_or_zero (tv2<T> vp v) {
		tv2<T> len = tv2<T>(length(v));
		return len.x == T(0) ? len : v / len;
	}
	template <typename T> DECL_ tv3<T>		normalize_or_zero (tv3<T> vp v) {
		tv3<T> len = tv3<T>(length(v));
		return len.x == T(0) ? len : v / len;
	}
	template <typename T> DECL_ tv4<T>		normalize_or_zero (tv4<T> vp v) {
		tv2<T> len = tv4<T>(length(v));
		return len.x == T(0) ? len : v / len;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv2<T>* v) {
		tv2<T> len = tv2<T>(length(*v));
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv3<T>* v) {
		tv3<T> len = tv3<T>(length(*v));
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv4<T>* v) {
		tv4<T> len = tv4<T>(length(*v));
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv2<T>* v, T* out_len) {
		tv2<T> len = tv2<T>(length(*v));
		*out_len = len.x;
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv3<T>* v, T* out_len) {
		tv3<T> len = tv3<T>(length(*v));
		*out_len = len.x;
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	template <typename T> DECL_ bool		normalize_or_zero (tv4<T>* v, T* out_len) {
		tv4<T> len = tv4<T>(length(*v));
		*out_len = len.x;
		bool zero = len.x == 0.0f;
		*v = zero ? len : *v / len;
		return !zero;
	}
	
	template <typename T> DECL_ constexpr FORCEINLINE tv2<T>	clamp_v (tv2<T> val, tv2<T> min, tv2<T> max) {
		return tv2<T>( clamp(val.x,min.x,max.x), clamp(val.y,min.y,max.y) );
	}
	template <typename T> DECL_ constexpr FORCEINLINE tv3<T>	clamp_v (tv3<T> val, tv3<T> min, tv3<T> max) {
		return tv3<T>( clamp(val.x,min.x,max.x), clamp(val.y,min.y,max.y), clamp(val.z,min.z,max.z) );
	}
	template <typename T> DECL_ constexpr FORCEINLINE tv4<T>	clamp_v (tv4<T> val, tv4<T> min, tv4<T> max) {
		return tv4<T>( clamp(val.x,min.x,max.x), clamp(val.y,min.y,max.y), clamp(val.z,min.z,max.z), clamp(val.w,min.w,max.w) );
	}
	
	template <typename T> DECL_ constexpr FORCEINLINE tv2<T>	lerp_v (tv2<T> alpha, tv2<T> start, tv2<T> end) {
		return (start * (tv2<T>(1) -alpha)) +(end * alpha);
	}
	template <typename T> DECL_ constexpr FORCEINLINE tv3<T>	lerp_v (tv3<T> alpha, tv3<T> start, tv3<T> end) {
		return (start * (tv3<T>(1) -alpha)) +(end * alpha);
	}
	template <typename T> DECL_ constexpr FORCEINLINE tv2<T>	lerp_v (tv4<T> alpha, tv4<T> start, tv4<T> end) {
		return (start * (tv4<T>(1) -alpha)) +(end * alpha);
	}
}

namespace fp {
	template <typename T> DECL_ constexpr tv2<T> pow_v (tv2<T> vp val, tv2<T> vp exp) {
		return tv2<T>(	pow(val.x, exp.x), pow(val.y, exp.y) );
	}
	template <typename T> DECL_ constexpr tv3<T> pow_v (tv3<T> vp val, tv3<T> vp exp) {
		return tv3<T>(	pow(val.x, exp.x), pow(val.y, exp.y), pow(val.z, exp.z) );
	}
	template <typename T> DECL_ constexpr tv4<T> pow_v (tv4<T> vp val, tv4<T> vp exp) {
		return tv4<T>(	pow(val.x, exp.x), pow(val.y, exp.y), pow(val.z, exp.z), pow(val.w, exp.w) );
	}
}

using vec_util::safe_cast_assert_v2;
using vec_util::safe_cast_assert_v3;
using vec_util::safe_cast_assert_v4;
using vec_util::cast_v2;
using vec_util::cast_v3;
using vec_util::cast_v4;

using vec_util::MAX_v;
using vec_util::MIN_v;
using vec_util::normalize_or_zero;
using vec_util::clamp_v;
using vec_util::lerp_v;
using fp::pow_v;



////// Matrix
namespace mat {
	
	template <typename T>
	struct tm2 {
		typedef T		t;
		typedef tv2<T>	v_t;
		
		v_t				arr[2];
		
		constexpr tm2 () = default;
		
	private:
		constexpr FORCEINLINE tm2 (v_t a, v_t b): arr{a, b} {}; // This does unnecessary copying on msvc
		constexpr FORCEINLINE tm2 (
				T ax,T ay,		T bx,T by ): arr{
				v_t(ax, ay),	v_t(bx, by) } {}; // This avoids unnecessary copying on msvc
		
	public:
		
		static constexpr CHECK_RESULT tm2 zero () {
			return tm2(v_t(0), v_t(0));
		}
		static constexpr CHECK_RESULT tm2 ident () {
			return tm2( v_t(1, 0),
						v_t(0, 1) );
		}
		static constexpr CHECK_RESULT tm2 all (T val) {
			return tm2(v_t(val), v_t(val));
		}
		
		// Set all values at once in column/row order
		static constexpr CHECK_RESULT tm2 column (	v_t vp a, v_t vp b ) {
			return tm2(a, b);
		}
		static constexpr CHECK_RESULT tm2 row (		v_t vp a, v_t vp b ) {
			return tm2( v_t(a.x, b.x),
						v_t(a.y, b.y) );
		}
		static constexpr CHECK_RESULT tm2 column (	T _0x, T _1x,
													T _0y, T _1y ) {
			return tm2( _0x,_1x,	_0y,_1y );
		}
		static constexpr CHECK_RESULT tm2 row (		T _0x, T _0y,
													T _1x, T _1y ) {
			return tm2( _0x,_1x,	_0y,_1y );
		}
		
		// Get/Set column/row
		constexpr v_t column (uptr indx) const {
			return arr[indx];
		}
		constexpr v_t row (uptr indx) const {
			return v_t(arr[0][indx], arr[1][indx]);
		}
		void column (uptr indx, v_t vp r) {
			arr[indx] = r;
		}
		void row (uptr indx, v_t vp r) {
			arr[0][indx] = r.x;
			arr[1][indx] = r.y;
		}
		
		// Get/Set single value with column/row indexing
		constexpr T column (uptr x, uptr y) const {
			return arr[x][y];
		}
		constexpr T row (uptr x, uptr y) const {
			return arr[y][x];
		}
		void column (uptr x, uptr y, T r) {
			arr[x][y] = r;
		}
		void row (uptr x, uptr y, T r) {
			arr[y][x] = r;
		}
		
		constexpr tm2 m2 () const {
			return *this;
		}
		tm2& m2 (tm2 mp r) {
			return *this = r;
		}
		
	};
	
	template <typename T>
	struct tm3 {
		typedef T		t;
		typedef tv3<T>	v_t;
		
		v_t				arr[3];
		
		constexpr tm3 () = default;
		
	private:
		constexpr FORCEINLINE tm3 (v_t a, v_t b, v_t c): arr{a, b, c} {}; // This does unnecessary copying on msvc
		constexpr FORCEINLINE tm3 (
				T ax,T ay,T az,		T bx,T by,T bz,		T cx,T cy,T cz ): arr{
				v_t(ax, ay, az),	v_t(bx, by, bz),	v_t(cx, cy, cz) } {}; // This avoids unnecessary copying on msvc
		
	public:
		
		static constexpr CHECK_RESULT tm3 zero () {
			return tm3(v_t(0), v_t(0), v_t(0));
		}
		static constexpr CHECK_RESULT tm3 ident () {
			return tm3( v_t(1, 0, 0),
						v_t(0, 1, 0),
						v_t(0, 0, 1) );
		}
		static constexpr CHECK_RESULT tm3 all (T val) {
			return tm3(v_t(val), v_t(val), v_t(val));
		}
		
		// Set all values at once in column/row order
		static constexpr CHECK_RESULT tm3 column (	v_t vp a, v_t vp b, v_t vp c ) {
			return tm3(a, b, c);
		}
		static constexpr CHECK_RESULT tm3 row (		v_t vp a, v_t vp b, v_t vp c ) {
			return tm3( v_t(a.x, b.x, c.x),
						v_t(a.y, b.y, c.y),
						v_t(a.z, b.z, c.z) );
		}
		static constexpr CHECK_RESULT tm3 column (	T _0x, T _1x, T _2x,
													T _0y, T _1y, T _2y,
													T _0z, T _1z, T _2z ) {
			return tm3( _0x,_1x,_2x,	_0y,_1y,_2y,	_0z,_1z,_2z );
		}
		static constexpr CHECK_RESULT tm3 row (		T _0x, T _0y, T _0z,
													T _1x, T _1y, T _1z,
													T _2x, T _2y, T _2z ) {
			return tm3( _0x,_1x,_2x,	_0y,_1y,_2y,	_0z,_1z,_2z );
		}
		
		// Get/Set column/row
		constexpr v_t column (uptr indx) const {
			return arr[indx];
		}
		constexpr v_t row (uptr indx) const {
			return v_t(arr[0][indx], arr[1][indx], arr[2][indx]);
		}
		void column (uptr indx, v_t vp r) {
			arr[indx] = r;
		}
		void row (uptr indx, v_t vp r) {
			arr[0][indx] = r.x;
			arr[1][indx] = r.y;
			arr[2][indx] = r.z;
		}
		
		// Get/Set single value with column/row indexing
		constexpr T column (uptr x, uptr y) const {
			return arr[x][y];
		}
		constexpr T row (uptr x, uptr y) const {
			return arr[y][x];
		}
		void column (uptr x, uptr y, T r) {
			arr[x][y] = r;
		}
		void row (uptr x, uptr y, T r) {
			arr[y][x] = r;
		}
		
		constexpr tm2<T> m2 () const {
			return tm2<T>::column(	arr[0].xy(),
									arr[1].xy() );
		}
		tm3& m2 (tm2<T> mp r) {
			arr[0].xy(r.arr[0]);
			arr[1].xy(r.arr[1]);
			return *this;
		}
		constexpr tm3 m3 () const {
			return *this;
		}
		tm3& m3 (tm3 mp r) {
			return *this = r;
		}
		
	};
	
	template <typename T>
	struct tm4 {
		typedef T		t;
		typedef tv4<T>	v_t;
		
		v_t				arr[4];
		
		constexpr tm4 () = default;
		
	private:
		constexpr FORCEINLINE tm4 (v_t a, v_t b, v_t c, v_t d): arr{a, b, c, d} {}; // This does unnecessary copying on msvc
		constexpr FORCEINLINE tm4 (
				T ax,T ay,T az, T aw,	T bx,T by,T bz, T bw,	T cx,T cy,T cz, T cw,	T dx,T dy,T dz, T dw ): arr{
				v_t(ax, ay, az, aw),	v_t(bx, by, bz, bw),	v_t(cx, cy, cz, cw),	v_t(dx, dy, dz, dw) } {}; // This avoids unnecessary copying on msvc
		
	public:
		
		static constexpr CHECK_RESULT tm4 zero () {
			return tm4(v_t(0), v_t(0), v_t(0), v_t(0));
		}
		static constexpr CHECK_RESULT tm4 ident () {
			return tm4( v_t(1, 0, 0, 0),
						v_t(0, 1, 0, 0),
						v_t(0, 0, 1, 0),
						v_t(0, 0, 0, 1) );
		}
		static constexpr CHECK_RESULT tm4 all (T val) {
			return tm4(v_t(val), v_t(val), v_t(val), v_t(val));
		}
		
		// Set all values at once in column/row order
		static constexpr CHECK_RESULT tm4 column (	v_t vp a, v_t vp b, v_t vp c, v_t vp d ) {
			return tm4(a, b, c, d);
		}
		static constexpr CHECK_RESULT tm4 row (		v_t vp a, v_t vp b, v_t vp c, v_t vp d ) {
			return tm4( v_t(a.x, b.x, c.x, d.x),
						v_t(a.y, b.y, c.y, d.y),
						v_t(a.z, b.z, c.z, d.z),
						v_t(a.w, b.w, c.w, d.w) );
		}
		static constexpr CHECK_RESULT tm4 column (	T _0x, T _1x, T _2x, T _3x,
													T _0y, T _1y, T _2y, T _3y,
													T _0z, T _1z, T _2z, T _3z,
													T _0w, T _1w, T _2w, T _3w ) {
			return tm4( _0x,_1x,_2x,_3x,	_0y,_1y,_2y,_3y,	_0z,_1z,_2z,_3z,	_0w,_1w,_2w,_3w );
		}
		static constexpr CHECK_RESULT tm4 row (		T _0x, T _0y, T _0z, T _0w,
													T _1x, T _1y, T _1z, T _1w,
													T _2x, T _2y, T _2z, T _2w,
													T _3x, T _3y, T _3z, T _3w ) {
			return tm4( _0x,_1x,_2x,_3x,	_0y,_1y,_2y,_3y,	_0z,_1z,_2z,_3z,	_0w,_1w,_2w,_3w );
		}
		
		// Get/Set column/row
		constexpr v_t column (uptr indx) const {
			return arr[indx];
		}
		constexpr v_t row (uptr indx) const {
			return v_t(arr[0][indx], arr[1][indx], arr[2][indx], arr[3][indx]);
		}
		void column (uptr indx, v_t vp r) {
			arr[indx] = r;
		}
		void row (uptr indx, v_t vp r) {
			arr[0][indx] = r.x;
			arr[1][indx] = r.y;
			arr[2][indx] = r.z;
			arr[3][indx] = r.w;
		}
		
		// Get/Set single value with column/row indexing
		constexpr T column (uptr x, uptr y) const {
			return arr[x][y];
		}
		constexpr T row (uptr x, uptr y) const {
			return arr[y][x];
		}
		void column (uptr x, uptr y, T r) {
			arr[x][y] = r;
		}
		void row (uptr x, uptr y, T r) {
			arr[y][x] = r;
		}
		
		constexpr tm2<T> m2 () const {
			return tm2<T>::column(	arr[0].xy(),
									arr[1].xy() );
		}
		tm4& m2 (tm2<T> mp r) {
			arr[0].xy(r.arr[0]);
			arr[1].xy(r.arr[1]);
			return *this;
		}
		constexpr tm3<T> m3 () const {
			return tm3<T>::column(	arr[0].xyz(),
									arr[1].xyz(),
									arr[2].xyz() );
		}
		tm4& m3 (tm3<T> mp r) {
			arr[0].xyz(r.arr[0]);
			arr[1].xyz(r.arr[1]);
			arr[2].xyz(r.arr[3]);
			return *this;
		}
		constexpr tm4 m4 () const {
			return *this;
		}
		tm4& m4 (tm4 mp r) {
			return *this = r;
		}
		
	};
	
	template <typename T>
	struct thm {
		typedef T		t;
		typedef tv4<T>	rv_t;
		typedef thv<T>	hv_t;
		
		hv_t			arr[4];
		
		constexpr thm () = default;
		
	private:
		constexpr FORCEINLINE thm (hv_t a, hv_t b, hv_t c, hv_t d): arr{a, b, c, d} {}; // This does unnecessary copying on msvc
		constexpr FORCEINLINE thm (
				T ax,T ay,T az,		T bx,T by,T bz,		T cx,T cy,T cz,		T dx,T dy,T dz ): arr{
				hv_t(ax, ay, az),	hv_t(bx, by, bz),	hv_t(cx, cy, cz),	hv_t(dx, dy, dz) } {}; // This avoids unnecessary copying on msvc
		
	public:
		
		static constexpr CHECK_RESULT thm ident () {
			return thm( 1, 0, 0,
						0, 1, 0,
						0, 0, 1,
						0, 0, 0 );
		}
		
		// Set all values at once in column/row order
		static constexpr CHECK_RESULT thm column (	hv_t vp a, hv_t vp b, hv_t vp c, hv_t vp d ) {
			return thm(a, b, c, d);
		}
		static constexpr CHECK_RESULT thm row (		rv_t vp a, rv_t vp b, rv_t vp c ) {
			return thm( hv_t(a.x, b.x, c.x),
						hv_t(a.y, b.y, c.y),
						hv_t(a.z, b.z, c.z),
						hv_t(a.w, b.w, c.w) );
		}
		static constexpr CHECK_RESULT thm column (	T _0x, T _1x, T _2x,
													T _0y, T _1y, T _2y,
													T _0z, T _1z, T _2z,
													T _0w, T _1w, T _2w ) {
			return thm( _0x,_1x,_2x,	_0y,_1y,_2y,	_0z,_1z,_2z,	_0w,_1w,_2w );
		}
		static constexpr CHECK_RESULT thm row (		T _0x, T _0y, T _0z, T _0w,
													T _1x, T _1y, T _1z, T _1w,
													T _2x, T _2y, T _2z, T _2w ) {
			return thm( _0x,_1x,_2x,	_0y,_1y,_2y,	_0z,_1z,_2z,	_0w,_1w,_2w );
		}
		
		// Get/Set column/row
		constexpr rv_t column (uptr indx) const {
			return rv_t(arr[indx].xyz(), indx == 3 ? 1.0f : 0.0f);
		}
		#if 0
		constexpr rv_t row (uptr indx) const {
			return rv_t(arr[0][indx], arr[1][indx], arr[2][indx], arr[3][indx]);
		}
		#endif
		void column (uptr indx, hv_t vp r) {
			arr[indx] = r;
		}
		void row (uptr indx, rv_t vp r) {
			arr[0][indx] = r.x;
			arr[1][indx] = r.y;
			arr[2][indx] = r.z;
		}
		
		// Get/Set single value with column/row indexing
		constexpr T column (uptr x, uptr y) const {
			return arr[x][y];
		}
		constexpr T row (uptr x, uptr y) const {
			return arr[y][x];
		}
		void column (uptr x, uptr y, T r) {
			arr[x][y] = r;
		}
		void row (uptr x, uptr y, T r) {
			arr[y][x] = r;
		}
		
		constexpr tm2<T> m2 () const {
			return tm2<T>::column(	arr[0].xy(),
									arr[1].xy() );
		}
		thm& m2 (tm2<T> mp r) {
			arr[0].xy(r.arr[0]);
			arr[1].xy(r.arr[1]);
			return *this;
		}
		constexpr tm3<T> m3 () const {
			return tm3<T>::column(	arr[0].xyz(),
									arr[1].xyz(),
									arr[2].xyz() );
		}
		thm& m3 (tm3<T> mp r) {
			arr[0].xyz(r.arr[0]);
			arr[1].xyz(r.arr[1]);
			arr[2].xyz(r.arr[2]);
			return *this;
		}
		constexpr tm4<T> m4 () const {
			return tm4<T>::column(	tv4<T>( arr[0].xyz(), 0 ),
									tv4<T>( arr[1].xyz(), 0 ),
									tv4<T>( arr[2].xyz(), 0 ),
									tv4<T>( arr[3].xyz(), 1 ) );
		}
		
		#if 0
		thm& translate (tv3<T> vp transl);
		thm& scale (tv3<T> vp scale);
		thm& rotation_x (f32 angle);
		thm& rotation_y (f32 angle);
		thm& rotation_z (f32 angle);
		#endif
		
	};
}
	
using mat::tm2;
using mat::tm3;
using mat::tm4;
using mat::thm;
	
typedef mat::tm2<f32> m2;
typedef mat::tm3<f32> m3;
typedef mat::tm4<f32> m4;
typedef mat::thm<f32> hm;
	
namespace mat_op {
	
	#if M_BY_VAL
	#else
	template <typename T>	DECL_ tm2<T>& compute_transpose_m2 (tm2<T>* ret, tm2<T> mp m) {
		ret->arr[0].x = m.arr[0].x;
		ret->arr[0].y = m.arr[1].x;
		ret->arr[1].x = m.arr[0].y;
		ret->arr[1].y = m.arr[1].y;
		return *ret;
	}
	template <typename T>	DECL_ tm3<T>& compute_transpose_m3 (tm3<T>* ret, tm3<T> mp m) {
		ret->arr[0].x = m.arr[0].x;
		ret->arr[0].y = m.arr[1].x;
		ret->arr[0].z = m.arr[2].x;
		ret->arr[1].x = m.arr[0].y;
		ret->arr[1].y = m.arr[1].y;
		ret->arr[1].z = m.arr[2].y;
		ret->arr[2].x = m.arr[0].z;
		ret->arr[2].y = m.arr[1].z;
		ret->arr[2].z = m.arr[2].z;
		return *ret;
	}
	template <typename T>	DECL_ tm4<T>& compute_transpose_m4 (tm4<T>* ret, tm4<T> mp m) {
		ret->arr[0].x = m.arr[0].x;
		ret->arr[0].y = m.arr[1].x;
		ret->arr[0].z = m.arr[2].x;
		ret->arr[0].w = m.arr[3].x;
		ret->arr[1].x = m.arr[0].y;
		ret->arr[1].y = m.arr[1].y;
		ret->arr[1].z = m.arr[2].y;
		ret->arr[1].w = m.arr[3].y;
		ret->arr[2].x = m.arr[0].z;
		ret->arr[2].y = m.arr[1].z;
		ret->arr[2].z = m.arr[2].z;
		ret->arr[2].w = m.arr[3].z;
		ret->arr[3].x = m.arr[0].w;
		ret->arr[3].y = m.arr[1].w;
		ret->arr[3].z = m.arr[2].w;
		ret->arr[3].w = m.arr[3].w;
		return *ret;
	}
	
	template <typename T>	DECL_ tm3<T>& compute_transpose_m3 (tm3<T>* m) {
		T ay = m->arr[0].y;
		T bx = m->arr[1].x;
		m->arr[0].y = bx;
		m->arr[1].x = ay;
		
		T cx = m->arr[2].x;
		T az = m->arr[0].z;
		m->arr[0].z = cx;
		m->arr[2].x = az;
		
		T cy = m->arr[2].y;
		T bz = m->arr[1].z;
		m->arr[1].z = cy;
		m->arr[2].y = bz;
		return *m;
	}
	
	#if 0
	template <typename T>	DECL FORCEINLINE tm2<T>& transpose (tm2<T>* m) {
		return tm2<T>::column( m.row(0), m.row(1) );
	}
	#endif
	template <typename T>	DECL FORCEINLINE tm3<T>& transpose (tm3<T>* m) {
		return compute_transpose_m3(m);
	}
	#if 0
	template <typename T>	DECL FORCEINLINE tm4<T>& transpose (tm4<T>* m) {
		return tm4<T>::column( m.row(0), m.row(1), m.row(2), m.row(3) );
	}
	#endif
	template <typename T>	DECL FORCEINLINE tm2<T> transpose (tm2<T> mp m) {
		tm2<T> ret;
		return compute_transpose_m2(&ret, m);
	}
	template <typename T>	DECL FORCEINLINE tm3<T> transpose (tm3<T> mp m) {
		tm3<T> ret = m;
		return compute_transpose_m3(&ret, m);
	}
	template <typename T>	DECL FORCEINLINE tm4<T> transpose (tm4<T> mp m) {
		tm4<T> ret;
		return compute_transpose_m4(&ret, m);
	}
	#endif
	
	#if V_BY_VAL
	template <typename T>
	DECL_ tv2<T> compute_mul_m2_v2 (tv2<T> vp v, tm2<T> mp m) {
		tv2<T> ret;
		ret.x = (m.arr[0].x * v.x) +(m.arr[1].x * v.y);
		ret.y = (m.arr[0].y * v.x) +(m.arr[1].y * v.y);
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tv2<T> operator* (tm2<T> mp m, tv2<T> vp v) {
		return compute_mul_m2_v2(v, m);
	}
	
	
	template <typename T>
	DECL_ tv3<T> compute_mul_m3_v3 (tv3<T> vp v, tm3<T> mp m) {
		tv3<T> ret;
		ret.x = ((m.arr[0].x * v.x) +(m.arr[1].x * v.y)) +(m.arr[2].x * v.z);
		ret.y = ((m.arr[0].y * v.x) +(m.arr[1].y * v.y)) +(m.arr[2].y * v.z);
		ret.z = ((m.arr[0].z * v.x) +(m.arr[1].z * v.y)) +(m.arr[2].z * v.z);
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tv3<T> operator* (tm3<T> mp m, tv3<T> vp v) {
		return compute_mul_m3_v3(v, m);
	}
	#else
	#endif
	
	template <typename T>
	DECL_ tv4<T> compute_mul_m4_v4 (tm4<T> mp m, tv4<T> vp v) {
		tv4<T> ret;
		ret.x = ((m.arr[0].x * v.x) +(m.arr[1].x * v.y)) +((m.arr[2].x * v.z) +(m.arr[3].x * v.w));
		ret.y = ((m.arr[0].y * v.x) +(m.arr[1].y * v.y)) +((m.arr[2].y * v.z) +(m.arr[3].y * v.w));
		ret.z = ((m.arr[0].z * v.x) +(m.arr[1].z * v.y)) +((m.arr[2].z * v.z) +(m.arr[3].z * v.w));
		ret.w = ((m.arr[0].w * v.x) +(m.arr[1].w * v.y)) +((m.arr[2].w * v.z) +(m.arr[3].w * v.w));
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tm4<T>& operator*= (tm4<T>& l, tm4<T> mp r) {
		return l = compute_mul_m4_m4(l, r); // TODO: inplace version of compute_mul_m4_m4?
	}
	
	#if M_BY_VAL
	#else
	template <typename T>
	DECL_ tm2<T>& compute_mul_m2_m2 (tm2<T>* ret, tm2<T> const* l, tm2<T> mp r) {
		
		T l0x=l->arr[0].x;
		ret->arr[0].x = (l->arr[0].x * r.arr[0].x) +(l->arr[1].x * r.arr[0].y);
		ret->arr[1].x = (		 l0x * r.arr[1].x) +(l->arr[1].x * r.arr[1].y);
		
		T l0y=l->arr[0].y;
		ret->arr[0].y = (l->arr[0].y * r.arr[0].x) +(l->arr[1].y * r.arr[0].y);
		ret->arr[1].y = (		 l0y * r.arr[1].x) +(l->arr[1].y * r.arr[1].y);
		
		return *ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tm2<T>& operator*= (tm2<T>& l, tm2<T> mp r) {
		return compute_mul_m2_m2(&l, &l, r);
	}
	template <typename T>
	DECL FORCEINLINE tm2<T> operator* (tm2<T> mp l, tm2<T> mp r) {
		tm2<T> ret;
		compute_mul_m2_m2(&ret, &l, r);
		return ret;
	}
	
	template <typename T>
	DECL_ tm3<T>& compute_mul_m3_m3 (tm3<T>* ret, tm3<T> const* l, tm3<T> mp r) {
		
		T l0x=l->arr[0].x, l1x=l->arr[1].x;
		ret->arr[0].x = ((l->arr[0].x * r.arr[0].x) +(l->arr[1].x * r.arr[0].y)) +(l->arr[2].x * r.arr[0].z);
		ret->arr[1].x = ((		  l0x * r.arr[1].x) +(l->arr[1].x * r.arr[1].y)) +(l->arr[2].x * r.arr[1].z);
		ret->arr[2].x = ((		  l0x * r.arr[2].x) +(		  l1x * r.arr[2].y)) +(l->arr[2].x * r.arr[2].z);
		
		T l0y=l->arr[0].y, l1y=l->arr[1].y;
		ret->arr[0].y = ((l->arr[0].y * r.arr[0].x) +(l->arr[1].y * r.arr[0].y)) +(l->arr[2].y * r.arr[0].z);
		ret->arr[1].y = ((		  l0y * r.arr[1].x) +(l->arr[1].y * r.arr[1].y)) +(l->arr[2].y * r.arr[1].z);
		ret->arr[2].y = ((		  l0y * r.arr[2].x) +(		  l1y * r.arr[2].y)) +(l->arr[2].y * r.arr[2].z);
		
		T l0z=l->arr[0].z, l1z=l->arr[1].z;
		ret->arr[0].z = ((l->arr[0].z * r.arr[0].x) +(l->arr[1].z * r.arr[0].y)) +(l->arr[2].z * r.arr[0].z);
		ret->arr[1].z = ((		  l0z * r.arr[1].x) +(l->arr[1].z * r.arr[1].y)) +(l->arr[2].z * r.arr[1].z);
		ret->arr[2].z = ((		  l0z * r.arr[2].x) +(		  l1z * r.arr[2].y)) +(l->arr[2].z * r.arr[2].z);
		
		return *ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tm3<T>& operator*= (tm3<T>& l, tm3<T> mp r) {
		return compute_mul_m3_m3(&l, &l, r);
	}
	template <typename T>
	DECL FORCEINLINE tm3<T> operator* (tm3<T> mp l, tm3<T> mp r) {
		tm3<T> ret;
		compute_mul_m3_m3(&ret, &l, r);
		return ret;
	}
	#endif
	
	
	template <typename T>
	DECL_ tm4<T> compute_mul_m4_m4 (tm4<T> mp l, tm4<T> mp r) {
		tm4<T> ret;
		ret.arr[0] = compute_mul_m4_v4(l, r.arr[0]);
		ret.arr[1] = compute_mul_m4_v4(l, r.arr[1]);
		ret.arr[2] = compute_mul_m4_v4(l, r.arr[2]);
		ret.arr[3] = compute_mul_m4_v4(l, r.arr[3]);
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tv4<T> operator* (tm4<T> mp m, tv4<T> vp v) {
		return compute_mul_m4_v4(m, v);
	}
	template <typename T>
	DECL FORCEINLINE tm4<T> operator* (tm4<T> mp l, tm4<T> mp r) {
		return compute_mul_m4_m4(l, r);
	}
	
	template <typename T>
	DECL_ T compute_determinant_m3 (tm3<T> mp m) {
		return	 ( m.arr[0].x * ( (m.arr[1].y * m.arr[2].z) -(m.arr[2].y * m.arr[1].z) ) )
				-( m.arr[1].x * ( (m.arr[0].y * m.arr[2].z) -(m.arr[2].y * m.arr[0].z) ) ) // -(a -b) -> (b -a)
				+( m.arr[2].x * ( (m.arr[0].y * m.arr[1].z) -(m.arr[1].y * m.arr[0].z) ) );
	}
	
	template <typename T>
	DECL FORCEINLINE T determinant (tm3<T> mp m) {
		return compute_determinant_m3(m);
	}
	
	#if M_BY_VAL
	#else
	template <typename T>
	DECL_ tm3<T>& compute_inverse_m3 (tm3<T>* ret, tm3<T> const* m) {
		
		T p_1y2z_2y1z = (m->arr[1].y * m->arr[2].z) -(m->arr[2].y * m->arr[1].z);
		T n_0y2z_2y0z = (m->arr[2].y * m->arr[0].z) -(m->arr[0].y * m->arr[2].z); // -(a -b) -> (b -a)
		T p_0y1z_1y0z = (m->arr[0].y * m->arr[1].z) -(m->arr[1].y * m->arr[0].z);
		
		T inv_det;
		{
			T det = m->arr[0].x * p_1y2z_2y1z;
			det +=	m->arr[1].x * n_0y2z_2y0z;
			det +=	m->arr[2].x * p_0y1z_1y0z;
			
			inv_det = T(1) / det;
		}
		
		T m1x=m->arr[1].x, m1y=m->arr[1].y;
		ret->arr[1].x = ((m->arr[2].x * m->arr[1].z) -(m->arr[1].x * m->arr[2].z)) * inv_det; // -(a -b) -> (b -a)
		ret->arr[1].y = ((m->arr[0].x * m->arr[2].z) -(m->arr[2].x * m->arr[0].z)) * inv_det;
		ret->arr[1].z = ((		  m1x * m->arr[0].z) -(m->arr[0].x * m->arr[1].z)) * inv_det; // -(a -b) -> (b -a)
		
		T m2x=m->arr[2].x;
		ret->arr[2].x = ((		  m1x * m->arr[2].y) -(m->arr[2].x *		 m1y)) * inv_det;
		ret->arr[2].y = ((		  m2x * m->arr[0].y) -(m->arr[0].x * m->arr[2].y)) * inv_det; // -(a -b) -> (b -a)
		ret->arr[2].z = ((m->arr[0].x *			m1y) -(		   m1x * m->arr[0].y)) * inv_det;
		
		ret->arr[0].x = (p_1y2z_2y1z * inv_det);
		ret->arr[0].y = (n_0y2z_2y0z * inv_det);
		ret->arr[0].z = (p_0y1z_1y0z * inv_det);
		return *ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tm3<T>& inverse (tm3<T>* m) {
		return compute_inverse_m3(&m, &m);
	}
	template <typename T>
	DECL FORCEINLINE tm3<T> inverse (tm3<T> mp m) {
		tm3<T> ret;
		compute_inverse_m3(&ret, &m);
		return ret;
	}
	#endif
	
	template <typename T>
	DECL_ tm3<T> compute_euler_angle_zxz_m3 (tv3<T> vp zxz) {
		// This function does (i think):
		//	 a = rotate_z(zxz[0]);
		//	 b = rotate_x(zxz[1]);
		//	 c = rotate_z(zxz[2]);
		//	 ret = a * b * c;
		tm3<T> ret;
		// 12 mul 4 add 1 neg 6 regs spilled, don't know if this can be reduced further
		
		// Why is the order of a b c messed up here?
		auto b = fp::sincos(zxz.y);
		auto c = fp::sincos(zxz.z);
		auto a = fp::sincos(zxz.x);
		
		ret.arr[2].z =	b.c;
		ret.arr[0].z =	b.s * c.s;
		ret.arr[1].z =	b.s * c.c;
		
		ret.arr[2].x =	a.s * b.s;
		
		T paspcc = a.s * c.c;
		
		T nac = -a.c;
		ret.arr[2].y =	nac * b.s;
		
		T nacpcs = nac * c.s;
		
		ret.arr[0].y = (   paspcc) -(b.c * nacpcs);
		ret.arr[1].x = nacpcs -(paspcc * b.c);
		
		T pacpcc = a.c * c.c;
		T paspcs = a.s * c.s;
		ret.arr[0].x = (pacpcc) -(b.c * paspcs);
		ret.arr[1].y = (pacpcc * b.c) -paspcs;
		
		return ret;
	}
	
	template <typename T>
	DECL_ tm3<T> compute_euler_angle_zxy_m3 (tv3<T> vp zxy) {
		// This function does:
		//	 a = rotate_z(zxy[0]);
		//	 b = rotate_x(zxy[1]);
		//	 c = rotate_y(zxy[2]);
		//	 ret = a * b * c;
		tm3<T> ret;
		
		auto a = fp::sincos(zxy[0]);
		auto b = fp::sincos(zxy[1]);
		auto c = fp::sincos(zxy[2]);
		
		// 12 mul 4 add 1 neg 6 reg spilled, don't know if this can be reduced further
		
		T nbc = -b.c;
		
		ret.arr[0].z = nbc * c.s;
		ret.arr[1].x = a.s * nbc;
		
		ret.arr[1].y = a.c * b.c;
		ret.arr[2].z = b.c * c.c;
		
		ret.arr[1].z = b.s;
		
		T accc = a.c * c.c;
		T ascs = a.s * c.s;
		ret.arr[0].x = accc -(ascs * b.s);
		ret.arr[2].y = ascs -(accc * b.s);
		
		T ascc = a.s * c.c;
		T accs = a.c * c.s;
		ret.arr[0].y = ascc +(accs * b.s);
		ret.arr[2].x = accs +(ascc * b.s);
		
		return ret;
	}
	
	// This is m4_v4 mul optimized for m4 with fourth_row=0,0,0,1 and v4 with w=1
	//	so v can only be a position vector
	#if V_BY_VAL
	template <typename T>
	DECL_ thv<T> compute_mul_hm_hv (thv<T> vp v, thm<T> mp m) {
		thv<T> ret;
		ret.x = ((m.arr[0].x * v.x) +(m.arr[1].x * v.y)) +((m.arr[2].x * v.z) +m.arr[3].x);
		ret.y = ((m.arr[0].y * v.x) +(m.arr[1].y * v.y)) +((m.arr[2].y * v.z) +m.arr[3].y);
		ret.z = ((m.arr[0].z * v.x) +(m.arr[1].z * v.y)) +((m.arr[2].z * v.z) +m.arr[3].z);
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE thv<T> operator* (thm<T> mp m, thv<T> vp v) {
		return compute_mul_hm_hv(v, m);
	}
	#else
	#endif
	
	#if M_BY_VAL
	#else
	template <typename T>
	DECL_ thm<T>& compute_mul_hm_hm (thm<T>* ret, thm<T> const* l, thm<T> mp r) {
		
		ret->arr[3].x = ((l->arr[0].x * r.arr[3].x) +(l->arr[1].x * r.arr[3].y)) +((l->arr[2].x * r.arr[3].z) +l->arr[3].x);
		ret->arr[3].y = ((l->arr[0].y * r.arr[3].x) +(l->arr[1].y * r.arr[3].y)) +((l->arr[2].y * r.arr[3].z) +l->arr[3].y);
		ret->arr[3].z = ((l->arr[0].z * r.arr[3].x) +(l->arr[1].z * r.arr[3].y)) +((l->arr[2].z * r.arr[3].z) +l->arr[3].z);
		
		T l0x=l->arr[0].x, l1x=l->arr[1].x;
		ret->arr[0].x = ((l->arr[0].x * r.arr[0].x) +(l->arr[1].x * r.arr[0].y)) +(l->arr[2].x * r.arr[0].z);
		ret->arr[1].x = ((		  l0x * r.arr[1].x) +(l->arr[1].x * r.arr[1].y)) +(l->arr[2].x * r.arr[1].z);
		ret->arr[2].x = ((		  l0x * r.arr[2].x) +(		  l1x * r.arr[2].y)) +(l->arr[2].x * r.arr[2].z);
		
		T l0y=l->arr[0].y, l1y=l->arr[1].y;
		ret->arr[0].y = ((l->arr[0].y * r.arr[0].x) +(l->arr[1].y * r.arr[0].y)) +(l->arr[2].y * r.arr[0].z);
		ret->arr[1].y = ((		  l0y * r.arr[1].x) +(l->arr[1].y * r.arr[1].y)) +(l->arr[2].y * r.arr[1].z);
		ret->arr[2].y = ((		  l0y * r.arr[2].x) +(		  l1y * r.arr[2].y)) +(l->arr[2].y * r.arr[2].z);
		
		T l0z=l->arr[0].z, l1z=l->arr[1].z;
		ret->arr[0].z = ((l->arr[0].z * r.arr[0].x) +(l->arr[1].z * r.arr[0].y)) +(l->arr[2].z * r.arr[0].z);
		ret->arr[1].z = ((		  l0z * r.arr[1].x) +(l->arr[1].z * r.arr[1].y)) +(l->arr[2].z * r.arr[1].z);
		ret->arr[2].z = ((		  l0z * r.arr[2].x) +(		  l1z * r.arr[2].y)) +(l->arr[2].z * r.arr[2].z);
		return *ret;
	}
	
	template <typename T>
	DECL FORCEINLINE thm<T>& operator*= (thm<T>& l, thm<T> mp r) {
		return compute_mul_hm_hm(&l, &l, r);
	}
	template <typename T>
	DECL FORCEINLINE thm<T> operator* (thm<T> mp l, thm<T> mp r) {
		thm<T> ret;
		compute_mul_hm_hm(&ret, &l, r);
		return ret;
	}
	#endif
	
#if 0
	template <typename T>
	DECL_ void compute_mul_hm_translate (thm<T>* l, tv3<T> vp transl) {
		l->arr[3].x += ((l->arr[0].x * transl.x) +(l->arr[1].x * transl.y)) +(l->arr[2].x * transl.z);
		l->arr[3].y += ((l->arr[0].y * transl.x) +(l->arr[1].y * transl.y)) +(l->arr[2].y * transl.z);
		l->arr[3].z += ((l->arr[0].z * transl.x) +(l->arr[1].z * transl.y)) +(l->arr[2].z * transl.z);
	}
	
	template <typename T>
	DECL_ void compute_mul_hm_scale (thm<T>* l, tv3<T> vp scale) {
		l->arr[0].x *= scale.x;
		l->arr[0].y *= scale.x;
		l->arr[0].z *= scale.x;
		
		l->arr[1].x *= scale.y;
		l->arr[1].y *= scale.y;
		l->arr[1].z *= scale.y;
		
		l->arr[2].x *= scale.z;
		l->arr[2].y *= scale.z;
		l->arr[2].z *= scale.z;
	}
	
	template <typename T>
	DECL_ void compute_mul_hm_rotate_z (thm<T>* l, T angle) {
		auto t = fp::sincos(angle);
		
		thv<T> l0 = l->arr[0];
		thv<T> l1 = l->arr[1];
		
		l->arr[0].x = (l0.x *  t.c) +(l1.x *  t.s);
		l->arr[0].y = (l0.y *  t.c) +(l1.y *  t.s);
		l->arr[0].z = (l0.z *  t.c) +(l1.z *  t.s);
		
		l->arr[1].x = (l0.x * -t.s) +(l1.x *  t.c);
		l->arr[1].y = (l0.y * -t.s) +(l1.y *  t.c);
		l->arr[1].z = (l0.z * -t.s) +(l1.z *  t.c);
	}
	
	template <typename T>
	DECL_ void compute_mul_hm_rotate (thm<T>* l, tm3<T> vp rot) {
		
		thm<T> l_cpy = *l;
		
		compute_mul_hm_hv(&l->arr[0], l_cpy, thv<T>(rot.arr[0]));
		compute_mul_hm_hv(&l->arr[1], l_cpy, thv<T>(rot.arr[1]));
		compute_mul_hm_hv(&l->arr[2], l_cpy, thv<T>(rot.arr[2]));
	}
#endif
	
#if SSE_OPTIMIZE
	using namespace sse;
	
	template<>
	DECLT_ tv3<f32> compute_mul_m3_v3<f32> (tv3<f32> vp v, tm3<f32> mp m) {
		STATIC_ASSERT(!M_BY_VAL);
		STATIC_ASSERT(V_BY_VAL);
		
		__m128	ldm0 = _mm_loadu_ps(&m.arr[0].x);
		__m128	ldm1 = _mm_loadu_ps(&m.arr[1].x);
		__m128	ldm2 = _mm_loadu_ps(&m.arr[2].x -1); // to prevent reading over the end
				ldm2 = _mm_shuffle_ps(ldm2, ldm2, _SHUF(Y,Z,W,X));
		
		__m128	vx = _mm_set_ps1(v.x);
		__m128	vy = _mm_set_ps1(v.y);
		__m128	vz = _mm_set_ps1(v.z);
		
		__m128	mul0 = _mm_mul_ps(ldm0, vx);
		__m128	mul1 = _mm_mul_ps(ldm1, vy);
		__m128	mul2 = _mm_mul_ps(ldm2, vz);
		
		__m128	add0 = _mm_add_ps(mul0, mul1);
		__m128	add1 = _mm_add_ps(add0, mul2);
		
		tv3<f32> ret;
		ret.x = _mm_cvtss_f32(add1);
		ret.y = _mm_cvtss_f32( _mm_shuffle_ps(add1, add1, Y) );
		ret.z = _mm_cvtss_f32( _mm_shuffle_ps(add1, add1, Z) );
		return ret;
	}
	
	template<>
	DECLT_ tm3<f32>& compute_mul_m3_m3<f32> (tm3<f32>* ret, tm3<f32> const* l, tm3<f32> mp r) {
		STATIC_ASSERT(!M_BY_VAL);
		
		__m128	lm0 = _mm_loadu_ps(&l->arr[0].x);
		__m128	lm1 = _mm_loadu_ps(&l->arr[1].x);
		__m128	lm2 = _mm_loadu_ps(&l->arr[2].x -1); // to prevent reading over the end
				lm2 = _mm_shuffle_ps(lm2, lm2, _SHUF(Y,Z,W,X));
		
		
		__m128	vx = _mm_set_ps1(r.arr[0].x);
		__m128	vy = _mm_set_ps1(r.arr[0].y);
		__m128	vz = _mm_set_ps1(r.arr[0].z);
		
		__m128	mul0 = _mm_mul_ps(lm0, vx);
		__m128	mul1 = _mm_mul_ps(lm1, vy);
		__m128	mul2 = _mm_mul_ps(lm2, vz);
		
		__m128	add0 = _mm_add_ps(mul0, mul1);
		__m128	add1 = _mm_add_ps(add0, mul2);
		
		_mm_storeu_ps(&ret->arr[0].x, add1); // overwrites ret[1].x
		
		
				vx = _mm_set_ps1(r.arr[1].x);
				vy = _mm_set_ps1(r.arr[1].y);
				vz = _mm_set_ps1(r.arr[1].z);
		
				mul0 = _mm_mul_ps(lm0, vx);
				mul1 = _mm_mul_ps(lm1, vy);
				mul2 = _mm_mul_ps(lm2, vz);
		
				add0 = _mm_add_ps(mul0, mul1);
				add1 = _mm_add_ps(add0, mul2);
		
		_mm_storeu_ps(&ret->arr[1].x, add1); // overwrites ret[2].x
		
		
				vx = _mm_set_ps1(r.arr[2].x);
				vy = _mm_set_ps1(r.arr[2].y);
				vz = _mm_set_ps1(r.arr[2].z);
		
				mul0 = _mm_mul_ps(lm0, vx);
				mul1 = _mm_mul_ps(lm1, vy);
				mul2 = _mm_mul_ps(lm2, vz);
		
				add0 = _mm_add_ps(mul0, mul1);
				add1 = _mm_add_ps(add0, mul2);
		
		_mm_storel_pd((double*)&ret->arr[2].x, _mm_castps_pd(add1)); // to prevent writing over the end
		ret->arr[2].z = _mm_cvtss_f32( _mm_shuffle_ps(add1, add1, Z) );
		
		return *ret;
	}
	
	template<>
	DECLT_ thv<f32> compute_mul_hm_hv<f32> (thv<f32> v, thm<f32> mp m) {
		STATIC_ASSERT(!M_BY_VAL);
		STATIC_ASSERT(V_BY_VAL);
		
		__m128	ldm0 = _mm_loadu_ps(&m.arr[0].x);
		__m128	ldm1 = _mm_loadu_ps(&m.arr[1].x);
		__m128	ldm2 = _mm_loadu_ps(&m.arr[2].x);
		__m128	ldm3 = _mm_loadu_ps(&m.arr[3].x -1); // to prevent reading over the end
				ldm3 = _mm_shuffle_ps(ldm3, ldm3, _SHUF(Y,Z,W,X));
		
		__m128	vx = _mm_set_ps1(v.x);
		__m128	vy = _mm_set_ps1(v.y);
		__m128	vz = _mm_set_ps1(v.z);
		
		__m128	mul0 = _mm_mul_ps(ldm0, vx);
		__m128	mul1 = _mm_mul_ps(ldm1, vy);
		__m128	mul2 = _mm_mul_ps(ldm2, vz);
		
		__m128	add0 = _mm_add_ps(mul0, mul1);
		__m128	add1 = _mm_add_ps(mul2, ldm3);
		__m128	add2 = _mm_add_ps(add0, add1);
		
		thv<f32> ret;
		ret.x = _mm_cvtss_f32(add2);
		ret.y = _mm_cvtss_f32( _mm_shuffle_ps(add2, add2, Y) );
		ret.z = _mm_cvtss_f32( _mm_shuffle_ps(add2, add2, Z) );
		return ret;
	}
	
	template<>
	DECLT_ thm<f32>& compute_mul_hm_hm<f32> (thm<f32>* ret, thm<f32> const* l, thm<f32> mp r) {
		STATIC_ASSERT(!M_BY_VAL);
		
		__m128	lm0 = _mm_loadu_ps(&l->arr[0].x);
		__m128	lm1 = _mm_loadu_ps(&l->arr[1].x);
		__m128	lm2 = _mm_loadu_ps(&l->arr[2].x);
		__m128	lm3 = _mm_loadu_ps(&l->arr[3].x -1); // to prevent reading over the end
				lm3 = _mm_shuffle_ps(lm3, lm3, _SHUF(Y,Z,W,X));
		
		
		__m128	vx = _mm_set_ps1(r.arr[0].x);
		__m128	vy = _mm_set_ps1(r.arr[0].y);
		__m128	vz = _mm_set_ps1(r.arr[0].z);
		
		__m128	mul0 = _mm_mul_ps(lm0, vx);
		__m128	mul1 = _mm_mul_ps(lm1, vy);
		__m128	mul2 = _mm_mul_ps(lm2, vz);
		
		__m128	add0 = _mm_add_ps(mul0, mul1);
		__m128	add1 = _mm_add_ps(add0, mul2);
		
		_mm_storeu_ps(&ret->arr[0].x, add1); // overwrites ret[1].x
		
		
				vx = _mm_set_ps1(r.arr[1].x);
				vy = _mm_set_ps1(r.arr[1].y);
				vz = _mm_set_ps1(r.arr[1].z);
		
				mul0 = _mm_mul_ps(lm0, vx);
				mul1 = _mm_mul_ps(lm1, vy);
				mul2 = _mm_mul_ps(lm2, vz);
		
				add0 = _mm_add_ps(mul0, mul1);
				add1 = _mm_add_ps(add0, mul2);
		
		_mm_storeu_ps(&ret->arr[1].x, add1); // overwrites ret[2].x
		
		
				vx = _mm_set_ps1(r.arr[2].x);
				vy = _mm_set_ps1(r.arr[2].y);
				vz = _mm_set_ps1(r.arr[2].z);
		
				mul0 = _mm_mul_ps(lm0, vx);
				mul1 = _mm_mul_ps(lm1, vy);
				mul2 = _mm_mul_ps(lm2, vz);
		
				add0 = _mm_add_ps(mul0, mul1);
				add1 = _mm_add_ps(add0, mul2);
		
		_mm_storeu_ps(&ret->arr[2].x, add1); // overwrites ret[3].x
		
		
				vx = _mm_set_ps1(r.arr[3].x);
				vy = _mm_set_ps1(r.arr[3].y);
				vz = _mm_set_ps1(r.arr[3].z);
		
				mul0 = _mm_mul_ps(lm0, vx);
				mul1 = _mm_mul_ps(lm1, vy);
				mul2 = _mm_mul_ps(lm2, vz);
		
				add0 = _mm_add_ps(mul0, mul1);
				add1 = _mm_add_ps(mul2, lm3);
		__m128	add2 = _mm_add_ps(add0, add1);
		
		_mm_storel_pd((double*)&ret->arr[3].x, _mm_castps_pd(add2)); // to prevent writing over the end
		ret->arr[3].z = _mm_cvtss_f32( _mm_shuffle_ps(add2, add2, Z) );
		
		return *ret;
	}
#endif
	
}

using mat_op::operator *= ;
using mat_op::operator *  ;
using mat_op::transpose;
using mat_op::determinant;
using mat_op::inverse;

#if 0
namespace mat {
	template <typename T> FORCEINLINE thm<T>& thm<T>::translate (tv3<T> vp transl) {
		mat_op::compute_mul_hm_translate(this, transl);
		return *this;
	}
	template <typename T> FORCEINLINE thm<T>& thm<T>::scale (tv3<T> vp scale) {
		mat_op::compute_mul_hm_scale(this, scale);
		return *this;
	}
	template <typename T> FORCEINLINE thm<T>& thm<T>::rotation_x (f32 angle) {
		auto t = fp::sincos(angle);
		tm3<T> temp = tm3<T>::row(	1,		0,		0,
									0,		+t.c,	-t.s,
									0,		+t.s,	+t.c );
		mat_op::compute_mul_hm_rotate(this, temp);
		return *this;
	}
	template <typename T> FORCEINLINE thm<T>& thm<T>::rotation_y (f32 angle) {
		auto t = fp::sincos(angle);
		tm3<T> temp = tm3<T>::row(	+t.c,	0,		+t.s,
									0,		1,		0,
									-t.s,	0,		+t.c );
		mat_op::compute_mul_hm_rotate(this, temp);
		return *this;
	}
	template <typename T> FORCEINLINE thm<T>& thm<T>::rotation_z (f32 angle) {
		auto t = fp::sincos(angle);
		tm3<T> temp = tm3<T>::row(	+t.c,	-t.s,	0,
									+t.s,	+t.c,	0,
									0,		0,		1 );
		mat_op::compute_mul_hm_rotate(this, temp);
		return *this;
	}
}
#endif
	
namespace quat_ {
	
	template <typename T>
	struct tquat {
		typedef T		t;
		
		T				x;
		T				y;
		T				z;
		T				w;
		
		constexpr tquat () = default;
		
		static constexpr tquat ident () {
			return tquat(tv3<T>(0), 1);
		}
		
		constexpr tquat (tv3<T> vp v, T w_):	x{v.x}, y{v.y}, z{v.z}, w{w_} {}
		
		constexpr tv4<T> xyzw () const {
			return tv4<T>(x, y, z, w); 
		}
	};
}
	
using quat_::tquat;
	
typedef quat_::tquat<f32> quat;
	
namespace quat_op {
	
	#if V_BY_VAL
	template <typename T>
	DECL_ tv3<T> compute_mul_q_v (tv3<T> vp v, tquat<T> vp q) {
		
		#if 0
		tv3<T> qv = tv3<T>(q.x, q.y, q.z);
		tv3<T> uv = vec_op::cross(qv, v);
		tv3<T> uuv = vec_op::cross(qv, uv);
		
		return v +::operator*( (::operator*(uv, tv3<T>(q.w)) +uuv), tv3<T>(2) );
		#else
		T yz = q.y * v.z;
		T zy = q.z * v.y;
		T zx = q.z * v.x;
		T xz = q.x * v.z;
		T xy = q.x * v.y;
		T yx = q.y * v.x;
		
		T ux = yz -zy;
		T uy = zx -xz;
		T uz = xy -yx;
		
		yz = q.y * uz;
		zy = q.z * uy;
		zx = q.z * ux;
		xz = q.x * uz;
		xy = q.x * uy;
		yx = q.y * ux;
		
		T uux = yz -zy;
		T uuy = zx -xz;
		T uuz = xy -yx;
		
		ux *= q.w;
		uy *= q.w;
		uz *= q.w;
		
		uux += ux;
		uuy += uy;
		uuz += uz;
		
		uux *= T(2);
		uuy *= T(2);
		uuz *= T(2);
		
		uux += v.x;
		uuy += v.y;
		uuz += v.z;
		
		return tv3<T>(uux, uuy, uuz);
		#endif
	}
	
	template <typename T>
	DECL_ tv3<T> operator* (tquat<T> vp q, tv3<T> vp v) {
		return compute_mul_q_v(v, q);
	}
	#else
	#endif
	
	#if V_BY_VAL
	template <typename T>
	DECL_ tquat<T> compute_mul_q_q (tquat<T> vp l, tquat<T> vp r) {
		tquat<T> ret;
		ret.x = (l.w * r.x) +(l.x * r.w) +(l.y * r.z) -(l.z * r.y);
		ret.y = (l.w * r.y) +(l.y * r.w) +(l.z * r.x) -(l.x * r.z);
		ret.z = (l.w * r.z) +(l.z * r.w) +(l.x * r.y) -(l.y * r.x);
		ret.w = (l.w * r.w) -(l.x * r.x) -(l.y * r.y) -(l.z * r.z);
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tquat<T>& operator*= (tquat<T>& l, tquat<T> vp r) {
		return l = compute_mul_q_q(l, r);
	}
	template <typename T>
	DECL FORCEINLINE tquat<T> operator* (tquat<T> vp l, tquat<T> vp r) {
		return compute_mul_q_q(l, r);
	}
	#else
	#endif
	
	#if V_BY_VAL
	template <typename T>
	DECL_ tquat<T> compute_inverse_q (tquat<T> vp q) {
		
		// NOTE: glm divided the conjugate by the len_sqr to get the inverse,
		//		  but quaternions are supposed to be always unit length, so there seems to be no point in doing this in here
		
		#if 0
		T conjugate_x = -q.x;
		T conjugate_y = -q.y;
		T conjugate_z = -q.z;
		T conjugate_w = q.w;
		
		T len_sqr =	 ((q.x * q.x) +(q.y * q.y))
					+((q.z * q.z) +(q.w * q.w));
		T len_sqr_inv = T(1) / len_sqr;
		
		tquat<T> ret;
		ret.x = conjugate_x * len_sqr_inv;
		ret.y = conjugate_y * len_sqr_inv;
		ret.z = conjugate_z * len_sqr_inv;
		ret.w = conjugate_w * len_sqr_inv;
		return ret;
		#else
		return tquat<T>(-tv3<T>(q.x, q.y, q.z), q.w);
		#endif
	}
	
	template <typename T>
	DECL FORCEINLINE tquat<T>& inverse (tquat<T>* q) {
		return q = compute_inverse_q(q);
	}
	template <typename T>
	DECL FORCEINLINE tquat<T> inverse (tquat<T> vp q) {
		return compute_inverse_q(q);
	}
	#else
	#endif
	
	template <typename T>
	DECL_ tm3<T> compute_convert_to_q_m3 (tquat<T> vp q) {
		
		T yy = q.y * q.y;
		T zz = q.z * q.z;
		T xy = q.x * q.y;
		T wz = q.w * q.z;
		T xz = q.x * q.z;
		T wy = q.w * q.y;
		T xx = q.x * q.x;
		T yz = q.y * q.z;
		T wx = q.w * q.x;
		
		tm3<T> ret;
		
		ret.arr[0].x = T(1) -( T(2) * (yy +zz) );
		ret.arr[0].y =		 ( T(2) * (xy +wz) );
		ret.arr[0].z =		 ( T(2) * (xz -wy) );
		
		ret.arr[1].x =		 ( T(2) * (xy -wz) );
		ret.arr[1].y = T(1) -( T(2) * (xx +zz) );
		ret.arr[1].z =		 ( T(2) * (yz +wx) );
		
		ret.arr[2].x =		 ( T(2) * (xz +wy) );
		ret.arr[2].y =		 ( T(2) * (yz -wx) );
		ret.arr[2].z = T(1) -( T(2) * (xx +yy) );
		return ret;
	}
	
	template <typename T>
	DECL FORCEINLINE tm3<T> conv_to_m3 (tquat<T> vp q) {
		return compute_convert_to_q_m3(q);
	}
	
	template <typename T>
	DECL_ tquat<T> compute_euler_angle_zxz_q (tv3<T> vp zxz) {
		
		tv3<T> ang = zxz / tv3<T>(2);
		
		auto a = fp::sincos(ang.x);
		auto b = fp::sincos(ang.y);
		auto c = fp::sincos(ang.z);
		
		quat ret;
		{
			T asbs = a.s * b.s;
			T acbs = a.c * b.s;
			T asbc = a.s * b.c;
			T acbc = a.c * b.c;
			
			ret.x = (asbs * c.s) +(acbs * c.c);
			ret.y = (asbs * c.c) -(acbs * c.s);
			ret.z = (acbc * c.s) +(asbc * c.c);
			ret.w = (acbc * c.c) -(asbc * c.s);
		}
		return ret;
	}
	
}
	
using quat_op::operator *= ;
using quat_op::operator *  ;
using quat_op::inverse;
using quat_op::conv_to_m3;
	
////// Print functions
namespace vec_print {
	
	using namespace print_n;
	
	DECL_ void print_vec (Base_Printer* this_, v2 vp v) {
		_print_wrapper(_recursive_print, this_, "v2(%, %)", v.x, v.y);
	}
	DECL_ void print_vec (Base_Printer* this_, v3 vp v) {
		_print_wrapper(_recursive_print, this_, "v3(%, %, %)", v.x, v.y, v.z);
	}
	DECL_ void print_vec (Base_Printer* this_, v4 vp v) {
		_print_wrapper(_recursive_print, this_, "v4(%, %, %, %)", v.x, v.y, v.z, v.w);
	}
	
	DECL_ void print_mat (Base_Printer* this_, m2 vp m) {
		_print_wrapper(_recursive_print, this_, "m2(%, %,\n"
												"	%, %)",
												m.row(0,0), m.row(0,1),
												m.row(1,0), m.row(1,1) );
	}
	DECL_ void print_mat (Base_Printer* this_, m3 mp m) {
		_print_wrapper(_recursive_print, this_, "m3(%, %, %,\n"
												"	%, %, %,\n"
												"	%, %, %)",
												m.row(0,0), m.row(0,1), m.row(0,2),
												m.row(1,0), m.row(1,1), m.row(1,2),
												m.row(2,0), m.row(2,1), m.row(2,2) );
	}
	DECL_ void print_mat (Base_Printer* this_, m4 mp m) {
		_print_wrapper(_recursive_print,this_,	"m4(%, %, %, %,\n"
												"	%, %, %, %,\n"
												"	%, %, %, %,\n"
												"	%, %, %, %)",
												m.row(0,0), m.row(0,1), m.row(0,2), m.row(0,3),
												m.row(1,0), m.row(1,1), m.row(1,2), m.row(1,3),
												m.row(2,0), m.row(2,1), m.row(2,2), m.row(2,3),
												m.row(3,0), m.row(3,1), m.row(3,2), m.row(3,3) );
	}
	DECL_ void print_mat (Base_Printer* this_, hm mp m) {
		_print_wrapper(_recursive_print, this_, "hm(%, %, %, %,\n"
												"	%, %, %, %,\n"
												"	%, %, %, %)",
												m.row(0,0), m.row(0,1), m.row(0,2), m.row(0,3),
												m.row(1,0), m.row(1,1), m.row(1,2), m.row(1,3),
												m.row(2,0), m.row(2,1), m.row(2,2), m.row(2,3) );
	}
	
	DECL_ void print_quat (Base_Printer* this_, quat vp q) {
		_print_wrapper(_recursive_print, this_, "quat(%, %, %,	%)", q.x, q.y, q.z, q.w);
	}
	
}
	
namespace sse_op {
	using namespace simd;
	
	struct S64V4 {
		__m128i xy;
		__m128i zw;
	};
	struct DV4 {
		__m128d xy;
		__m128d zw;
	};
	struct S64M4 {
		S64V4	column[4];
	};
	struct DM4 {
		DV4		column[4];
	};
	
	DECL FORCEINLINE DV4 load (tv4<f64> const* p) {
		return { _mm_loadu_pd(&p->x), _mm_loadu_pd(&p->z) };
	}
	DECL FORCEINLINE DM4 load (tm4<f64> const* p) {
		return {{ load(&p->arr[0]), load(&p->arr[1]), load(&p->arr[2]), load(&p->arr[3]) }};
	}
	DECL FORCEINLINE void store (tv4<f64>* p, DV4 v) {
		_mm_storeu_pd(&p->x, v.xy);
		_mm_storeu_pd(&p->z, v.zw);
	}
	DECL FORCEINLINE void store (tm4<f64>* p, DM4 v) {
		store(&p->arr[0], v.column[0]);
		store(&p->arr[1], v.column[1]);
		store(&p->arr[2], v.column[2]);
		store(&p->arr[3], v.column[3]);
	}
	
	template <typename T>
	DECLT_ FORCEINLINE T cast (DV4 v);
	template <typename T>
	DECLT_ FORCEINLINE T cast (S64V4 v);
	
	template<> DECLT_ FORCEINLINE S64V4 cast<S64V4> (DV4 v) {
		return { _mm_castpd_si128(v.xy), _mm_castpd_si128(v.zw) };
	}
	template<> DECLT_ FORCEINLINE DV4 cast<DV4> (S64V4 v) {
		return { _mm_castsi128_pd(v.xy), _mm_castsi128_pd(v.zw) };
	}
	
	DECL_ DV4 neg (DV4 v) {
		return { _mm_xor_pd(v.xy, sign_bit_pd()), _mm_xor_pd(v.zw, sign_bit_pd()) };
	}
	DECL_ S64V4 neg (S64V4 v) {
		return { _mm_sub_epi64(_mm_setzero_si128(), v.xy), _mm_sub_epi64(_mm_setzero_si128(), v.zw) };
	}
	
	DECL_ DV4 mul (DV4 l, DV4 r) {
		return { _mm_mul_pd(l.xy, r.xy), _mm_mul_pd(l.zw, r.zw) };
	}
	
	DECL_ DV4 div (DV4 l, DV4 r) {
		return { _mm_div_pd(l.xy, r.xy), _mm_div_pd(l.zw, r.zw) };
	}
	
	DECL_ DV4 add (DV4 l, DV4 r) {
		return { _mm_add_pd(l.xy, r.xy), _mm_add_pd(l.zw, r.zw) };
	}
	DECL_ S64V4 add (S64V4 l, S64V4 r) {
		return { _mm_add_epi64(l.xy, r.xy), _mm_add_epi64(l.zw, r.zw) };
	}
	
	DECL_ DV4 sub (DV4 l, DV4 r) {
		return { _mm_sub_pd(l.xy, r.xy), _mm_sub_pd(l.zw, r.zw) };
	}
	DECL_ S64V4 sub (S64V4 l, S64V4 r) {
		return { _mm_sub_epi64(l.xy, r.xy), _mm_sub_epi64(l.zw, r.zw) };
	}
	
	DECL_ DV4 normalize (DV4 v) {
		
		__m128d sqr_xy =	_mm_mul_pd(v.xy, v.xy);			// x*x y*y
		__m128d sqr_zw =	_mm_mul_pd(v.zw, v.zw);			// z*z w*w
		__m128d len0 =		_mm_add_pd(sqr_xy, sqr_zw);		// (x*x)+(z*z) (y*y)+(w*w)
		__m128d len1 =		_mm_shuffle_pd(len0, len0, 1);	// (y*y)+(w*w) _
		__m128d len =		_mm_add_pd(len0, len1);			// ((x*x)+(z*z))+((y*y)+(w*w)) _
				len =		_mm_sqrt_pd(len);				// ||v||
		
		// Could check for zero here with _mm_comieq_sd(len, _mm_setzero_pd())
		
				len =		_mm_shuffle_pd(len, len, 0);	// ||v|| ||v||
		
				v.xy =		_mm_div_sd(v.xy, len);
				v.zw =		_mm_div_sd(v.zw, len);
		
		return v;
	}
	
}
	
#undef DECL_
#undef DECLT_
#undef DECLM_
	