/*-- Determining compiler/platform/architecture --*/
	#define _COMP_GCC			1
	#define _COMP_LLVM			2
	#define _COMP_MSVC			3
	// Determining the compiler
	#if !defined _COMP
		#if _MSC_VER && !__INTEL_COMPILER && !__clang__
			#define _COMP _COMP_MSVC
		#elif __GNUC__ && !__clang__
			#define _COMP _COMP_GCC
		#elif __clang__
			#define _COMP _COMP_LLVM
		#else
			#warning Cannot determine compiler!.
		#endif
	#endif
	
	#define ARCH_X64			1
	#define ARCH_ARM_CORTEX_M4	2
	#define ARCH_ARM_V6_HF		3
	
	#define PLATF_GENERIC_WIN	1
	#define PLATF_GENERIC_UNIX	2
	#define PLATF_NONE			3
	
/*-- Compiler specifics --*/
	#if _COMP == _COMP_MSVC
		
		#define PRAGMA_MSG(msg) __pragma(message(__FILE__ "(" TO_STR(__LINE__) "): " msg))
		
		#define DLLEXPORT						__declspec(dllexport)
		#define DLLIMPORT						__declspec(dllimport)
		#define NORETURN						__declspec(noreturn)
		#define FORCEINLINE						__forceinline
		// RETURNS_TWICE no equivalent?
		#define NOINLINE						__declspec(noinline)
		#define DEPRECATED						__declspec(deprecated)
		#define CHECK_RESULT					_Check_return_ // Does not actually work, preprocesses to nothing even if put after #include "windows.h", not sure how you are supposed to use this
		#define DBGBREAK						__debugbreak()
		#define USED							
		#define SECTION(name)					
		#define UNREACHABLE						__assume(0)
		#define LIKELY(cond)					!!(cond)
		#define UNLIKELY(cond)					!!(cond)
		#define ASSUME(cond)					__assume(cond)
		#define ASSUME_ALIGNED(ptr, align)		(ptr)
		#define ALIGNED(a)						__declspec(align(a)) // This is 
		//#define BUILTIN_STRLEN(cstr)			strlen(cstr)
		#define BUILTIN_F32_INF					((float)(1e+300 * 1e+300))
		#define BUILTIN_F64_INF					(1e+300 * 1e+300)
		#define BUILTIN_F32_QNAN				__builtin_nanf("0")
		#define BUILTIN_F64_QNAN				__builtin_nan("0")
		//#define offsetof(s,m)					((uptr)&reinterpret_cast<byte const volatile&>((((s*)0)->m)))
		//#define offsetof(s,m)					((uptr)&(byte const volatile&)((((s*)0)->m)))
		
		#define VECTORCALL						// What is the vectorcall tag on msvc? does not matter, vectorcall is default because of /Gv option
		
		#define WARNINGS_PUSH					__pragma( warning(push) )
		#define WARNINGS_POP					__pragma( warning(pop) )
		#define WARNING_DISABLE(code)			__pragma( warning( disable : code ) )
		
	#elif _COMP == _COMP_LLVM
		#define DLLEXPORT						__attribute__((dllexport))
		#define DLLIMPORT						__attribute__((dllimport))
		#define NORETURN						__attribute__((noreturn))
		#define FORCEINLINE						__attribute__((always_inline)) inline
		#define NOINLINE						__attribute__((noinline))
		#define DEPRECATED						__attribute__((deprecated))
		#define CHECK_RESULT					__attribute__((warn_unused_result))
		//#define RETURNS_TWICE					__attribute__((returns_twice))
		#define UNREACHABLE						do { __builtin_unreachable(); } while(0)
		#define LIKELY(cond)					__builtin_expect(!!(cond), 1)
		#define UNLIKELY(cond)					__builtin_expect(!!(cond), 0)
		#define ASSUME(cond)					__builtin_assume(cond)
		#define ASSUME_ALIGNED(ptr, align)		__builtin_assume_aligned((ptr), (align))
		#define SECTION(name)					__attribute__((section(name)))
		#define USED							__attribute__((used))
		#define UNUSED							__attribute__((unused))
		#define ALIGNED(a)						__attribute__((aligned(a)))
		//#define BUILTIN_STRLEN(cstr)			__builtin_strlen((cstr))
		#define BUILTIN_F32_INF					(__builtin_inff())
		#define BUILTIN_F64_INF					(__builtin_inf())
		#define BUILTIN_F32_QNAN				__builtin_nan("0")
		#define BUILTIN_F64_QNAN				__builtin_nan("0")
		
		#define VECTORCALL						__attribute__((vectorcall))
		
		#define DBGBREAK						do { asm volatile ("int3"); } while(0)
	#elif _COMP == _COMP_GCC
		#define PRAGMA_MSG(msg) _Pragma( STRINGIFY(message(__FILE__ "(" TO_STR(__LINE__) "): " msg)) )
		
		#define DLLEXPORT						__attribute__((dllexport))
		#define DLLIMPORT						__attribute__((dllimport))
		#define NORETURN						__attribute__((noreturn))
		#define PURE							__attribute__((pure))
		#define FORCEINLINE						__attribute__((always_inline)) inline
		#define NOINLINE						__attribute__((noinline))
		#define DEPRECATED						__attribute__((deprecated))
		#define CHECK_RESULT					__attribute__((warn_unused_result))
		//#define RETURNS_TWICE					__attribute__((returns_twice))
		#define UNREACHABLE						do { __builtin_unreachable(); } while(0)
		#define LIKELY(cond)					__builtin_expect(!!(cond), 1)
		#define UNLIKELY(cond)					__builtin_expect(!!(cond), 0)
		#define ASSUME(cond)					do { if (UNLIKELY(!(cond))) { UNREACHABLE; } } while(0)
		#define ASSUME_ALIGNED(ptr, align)		__builtin_assume_aligned((ptr), (align))
		#define SECTION(name)					__attribute__((section(name)))
		#define USED							__attribute__((used))
		#define UNUSED							__attribute__((unused))
		#define ALIGNED(a)						__attribute__((aligned(a)))
		//#define BUILTIN_STRLEN(cstr)			__builtin_strlen((cstr))
		#define BUILTIN_F32_INF					(__builtin_inff())
		#define BUILTIN_F64_INF					(__builtin_inf())
		#define BUILTIN_F32_QNAN				__builtin_nan("0")
		#define BUILTIN_F64_QNAN				__builtin_nan("0")
		//#if _ARCH == ARCH_ARM_CORTEX_M4 || _ARCH == ARCH_ARM_V6_HF
		//	#define offsetof(struct_type, member) __builtin_offsetof(struct_type, member)
		//#endif
		
		#if _PLATF == PLATF_GENERIC_WIN
			#define DBGBREAK					do { __debugbreak(); } while(0)
		#elif _PLATF == PLATF_GENERIC_UNIX
			#if _ARCH == ARCH_ARM_V6_HF
				#define DBGBREAK				do { asm volatile ("bkpt #0"); } while(0)
			#endif
		#endif
		
		#define VECTORCALL						//__attribute__((vectorcall))
		
		//#define TYPENAME typename // for 'need 'typename' before' error which MSVC does not need
		
		#define WARNINGS_PUSH					_Pragma("GCC diagnostic push")
		#define WARNINGS_POP					_Pragma("GCC diagnostic pop")
		// Can't disable all warnings in gcc, supposedly works in clang
		//#define WARNING_DISABLE_ALL				_Pragma( STRINGIFY(diagnostic ignored "-Wall") );
	#endif
	
	#define DBGBREAK_IF_DEBUGGER_PRESENT		if (IsDebuggerPresent()) { DBGBREAK; }
	