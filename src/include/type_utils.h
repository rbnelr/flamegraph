
namespace types {
	template <typename T> DECLT constexpr bool isUnsigned () {				return false; }
	template<> DECLT constexpr bool isUnsigned<uchar> () {					return true; }
	template<> DECLT constexpr bool isUnsigned<ushort> () {					return true; }
	template<> DECLT constexpr bool isUnsigned<ui> () {						return true; }
	template<> DECLT constexpr bool isUnsigned<ulong> () {					return true; }
	template<> DECLT constexpr bool isUnsigned<ullong> () {					return true; }
	
	template <typename T> DECLT constexpr bool isSigned () {				return false; }
	template<> DECLT constexpr bool isSigned<schar> () {					return true; }
	template<> DECLT constexpr bool isSigned<sshort> () {					return true; }
	template<> DECLT constexpr bool isSigned<si> () {						return true; }
	template<> DECLT constexpr bool isSigned<slong> () {					return true; }
	template<> DECLT constexpr bool isSigned<sllong> () {					return true; }
	
	
	template <bool SIGNED, ui SIZE> struct GetInteger;
	template<> struct GetInteger<false, 1> {	typedef u8 type; };
	template<> struct GetInteger<false, 2> {	typedef u16 type; };
	template<> struct GetInteger<false, 4> {	typedef u32 type; };
	template<> struct GetInteger<false, 8> {	typedef u64 type; };
	template<> struct GetInteger<true, 1> {		typedef s8 type; };
	template<> struct GetInteger<true, 2> {		typedef s16 type; };
	template<> struct GetInteger<true, 4> {		typedef s32 type; };
	template<> struct GetInteger<true, 8> {		typedef s64 type; };
	
	template <ui SIZE> struct GetUInteger { typedef typename GetInteger<false, SIZE>::type type; };
	template <ui SIZE> struct GetSInteger { typedef typename GetInteger<true, SIZE>::type type; };
	
	template <typename T>
	struct NormalizeIntType {
		typedef typename GetInteger<isSigned<T>(), sizeof(T)>::type type;
	};
	
	template <typename T> DECL constexpr
	typename NormalizeIntType<T>::type normalize_int_type (T val) {
		return val;
	}
	
	template <typename T1, typename T2>		struct _Is_Same {
		static constexpr bool value = false;
	};
	template <typename T>					struct _Is_Same<T, T> {
		static constexpr bool value = true;
	};
	
	template <typename T1, typename T2>		constexpr bool is_same () {
		return _Is_Same<T1, T2>::value;
	}
	
}

namespace limits {
	template <typename T> DECLT constexpr T min ();
	template <typename T> DECLT constexpr T max ();
	template <typename T> DECLT constexpr T allBits ();
	
	template <typename T, bool S, ui B> struct _Min {};
	template <typename T> struct _Min<T, false, 1> {		static constexpr T val = 0u; };
	template <typename T> struct _Min<T, false, 2> {		static constexpr T val = 0u; };
	template <typename T> struct _Min<T, false, 4> {		static constexpr T val = 0ul; };
	template <typename T> struct _Min<T, false, 8> {		static constexpr T val = 0ull; };
	template <typename T> struct _Min<T, true, 1> {			static constexpr T val = 0x80; };
	template <typename T> struct _Min<T, true, 2> {			static constexpr T val = 0x8000; };
	template <typename T> struct _Min<T, true, 4> {			static constexpr T val = 0x80000000l; };
	template <typename T> struct _Min<T, true, 8> {			static constexpr T val = 0x8000000000000000ll; };
	
	template <typename T, bool S, ui B> struct _Max {};
	template <typename T> struct _Max<T, false, 1> {		static constexpr T val = 0xFFu; };
	template <typename T> struct _Max<T, false, 2> {		static constexpr T val = 0xFFFFu; };
	template <typename T> struct _Max<T, false, 4> {		static constexpr T val = 0xFFFFFFFFul; };
	template <typename T> struct _Max<T, false, 8> {		static constexpr T val = 0xFFFFFFFFFFFFFFFFull; };
	template <typename T> struct _Max<T, true, 1> {			static constexpr T val = 0x7F; };
	template <typename T> struct _Max<T, true, 2> {			static constexpr T val = 0x7FFF; };
	template <typename T> struct _Max<T, true, 4> {			static constexpr T val = 0x7FFFFFFFl; };
	template <typename T> struct _Max<T, true, 8> {			static constexpr T val = 0x7FFFFFFFFFFFFFFFll; };
	
	template <typename T, bool S, ui B> struct _All_Bits;
	template <typename T> struct _All_Bits<T, false, 1> {	static constexpr T val = 0xFFu; };
	template <typename T> struct _All_Bits<T, false, 2> {	static constexpr T val = 0xFFFFu; };
	template <typename T> struct _All_Bits<T, false, 4> {	static constexpr T val = 0xFFFFFFFFul; };
	template <typename T> struct _All_Bits<T, false, 8> {	static constexpr T val = 0xFFFFFFFFFFFFFFFFull; };
	template <typename T> struct _All_Bits<T, true, 1> {	static constexpr T val = -1; };
	template <typename T> struct _All_Bits<T, true, 2> {	static constexpr T val = -1; };
	template <typename T> struct _All_Bits<T, true, 4> {	static constexpr T val = -1l; };
	template <typename T> struct _All_Bits<T, true, 8> {	static constexpr T val = -1ll; };
	
	template <typename T> DECLT constexpr T min () {		return _Min< T, types::isSigned<T>(), sizeof(T) >::val; };
	template <typename T> DECLT constexpr T max () {		return _Max< T, types::isSigned<T>(), sizeof(T) >::val; };
	template <typename T> DECLT constexpr T allBits () {	return _All_Bits< T, types::isSigned<T>(), sizeof(T) >::val; };
}

union FI32_u {
	f32		f;
	u32		i;
	constexpr FI32_u(f32 f): f{f} {}
	constexpr FI32_u(u32 i): i{i} {}
};
union FI64_u {
	f64		f;
	u64		i;
	constexpr FI64_u(f64 f): f{f} {}
	constexpr FI64_u(u64 i): i{i} {}
};

DECL FORCEINLINE constexpr f32 reint_int_as_flt (u32 i) {
	return FI32_u(i).f;
}
DECL FORCEINLINE constexpr f64 reint_int_as_flt (u64 i) {
	return FI64_u(i).f;
}
DECL FORCEINLINE constexpr u32 reint_flt_as_int (f32 f) {
	return FI32_u(f).i;
}
DECL FORCEINLINE constexpr u64 reint_flt_as_int (f64 f) {
	return FI64_u(f).i;
}
