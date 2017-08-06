	
/*-- Preprocessor tricks --*/
	// STRINGIFY([definition name]) expands to [definition name] as string literal
	#define STRINGIFY(x) #x
	// TO_STR([definition name]) expands to definition as string literal
	#define TO_STR(x) STRINGIFY(x)
	// Concatenate tokens ex: CONCAT(func, __COUNTER__) -> func0, func1, func2, ...
	#define _CONCAT(a, b) a##b
	#define CONCAT(a, b) _CONCAT(a, b)
	
	// Allows putting comma into macro arguments
	//	without being intepreted as macro argument delimiter
	#define _COMMA ,
	
	#define STATIC_ASSERT(cond) static_assert((cond), TO_STR(cond)) // This does not work for some reason???
	
//// 
	#include "comp_arch_platf_specifics.hpp"
	
	#define DECLD									static				// declare global data
	#define DECL									static VECTORCALL	// declare global function
	#define DECLFP									VECTORCALL			// declare function pointer
	#define DECLV									static				// declare variadic function
	#define DECLA									static				// declare API (other calling conv) function
	#define DECLM									VECTORCALL			// declare method
	#define DECLT									VECTORCALL			// declare template
	
//// typedefs for basic types
	#include "types.hpp"
	#include "type_utils.hpp"
	
////
	#include "debug.hpp"
	
	enum err_e {
		OK=0,
		FILE_OPEN_FAIL,
		FILE_READ_FAIL,
		FILE_WRITE_FAIL,
		FILE_HEADER_ERR,
		OGL_ERROR,
	};
	
//// 
	#include "simd.hpp"
	#include "floating_point.hpp"
	
////
	#include "utility.hpp"
	
////
	//#define DBG_MEMORY DEBUG
	#define DBG_MEMORY 0
	
	#define ARRAYS_BOUNDS_ASSERT DEBUG
	
	DECLD constexpr byte DBG_MEM_UNALLOCATED_BYTE =			(byte)0xcc;
	DECLD constexpr byte DBG_MEM_UNINITIALIZED_BYTE =		(byte)0xdd;
	DECLD constexpr byte DBG_MEM_ALIGN_PADDING_BYTE =		(byte)0xaa;
	DECLD constexpr byte DBG_MEM_FREED_BYTE =				(byte)0xee;
	
	#include <cstdlib>
	
	#include "stack.hpp"
	#include "dynarr.hpp"
	
	DECLD thread_local Stack	working_stk =		makeStack(0, mebi<uptr>(32));
	
//// windows header #include
	#include "windows_h_helper.hpp"
	
////
	#include "print.hpp"
	using print_n::print_type_e;
	using print_n::escaped;
	using print_n::hex;
	using print_n::bin;
	using print_n::repeat;
	
////
	#include "windows_utility.hpp"
	
////
	#include "sync.hpp"
	
////
	#include "time.hpp"
	
////
	#include "utility.cpp"
	
////
	#include "stack.cpp"
	#include "dynarr.cpp"
	
////
	#include "print.cpp"
	
////
	#include "debug.cpp"
	
////
	#include "vector.hpp"
	
	typedef tv2<u32>	v2u32;
	
namespace print_n {
	
	enum extra_print_type_e : char {
		V2='y',
		V3='v',
		V4='V',
		M2='n',
		M3='m',
		M4='M',
		HM='H'|(char)0x80,
		QUAT='q',
	};
	
	template<> struct ForEach<v2> {
		static constexpr print_type_e VAL =					(print_type_e)V2;
		static FORCEINLINE v2 do_ (v2 v) {					return v; }
	};
	template<> struct ForEach<v3> {
		static constexpr print_type_e VAL =					(print_type_e)V3;
		static FORCEINLINE v3 do_ (v3 v) {					return v; }
	};
	template<> struct ForEach<v4> {
		static constexpr print_type_e VAL =					(print_type_e)V4;
		static FORCEINLINE v4 do_ (v4 v) {					return v; }
	};
	template<> struct ForEach<m2> {
		static constexpr print_type_e VAL =					(print_type_e)M2;
		static FORCEINLINE m2 do_ (m2 m) {					return m; }
	};
	template<> struct ForEach<m3> {
		static constexpr print_type_e VAL =					(print_type_e)M3;
		static FORCEINLINE m3 do_ (m3 m) {					return m; }
	};
	template<> struct ForEach<m4> {
		static constexpr print_type_e VAL =					(print_type_e)M4;
		static FORCEINLINE m4 do_ (m4 m) {					return m; }
	};
	template<> struct ForEach<hm> {
		static constexpr print_type_e VAL =					(print_type_e)HM;
		static FORCEINLINE hm do_ (hm m) {					return m; }
	};
	template<> struct ForEach<quat> {
		static constexpr print_type_e VAL =					(print_type_e)QUAT;
		static FORCEINLINE quat do_ (quat v) {				return v; }
	};
	
	DECL FORCEINLINE va_list _printCustomTypeToBuffer (Base_Printer* this_, print_type_e type, va_list args) {
		
		switch ((extra_print_type_e)type) {
			case V2:	vec_print::print_vec(this_, va_arg(args, v2));		break;
			case V3:	vec_print::print_vec(this_, va_arg(args, v3));		break;
			case V4:	vec_print::print_vec(this_, va_arg(args, v4));		break;
			case M2:	vec_print::print_mat(this_, va_arg(args, m2));		break;
			case M3:	vec_print::print_mat(this_, va_arg(args, m3));		break;
			case M4:	vec_print::print_mat(this_, va_arg(args, m4));		break;
			case HM:	vec_print::print_mat(this_, va_arg(args, hm));		break;
			case QUAT:	vec_print::print_quat(this_,	va_arg(args, quat));	break;
			default: return nullptr;
		}
		
		return args;
	}
}
	