	
	#include <stdarg.h>
	
namespace print_n {
	enum print_type_e : char {
		TERMINATOR='\0', OTHER='?',
		CSTR='s', LSTR='S',
		CSTR_ESC='s'|(char)0x80,	LSTR_ESC='S'|(char)0x80, // No complex flags for now
		CHAR='c', BOOL='b',
		REPS='r',
		S32='i', S64='I', U32='u', U64='U',
		F32='f', F64='d',
		HEX='H', BIN='B',
		PTR='p',
	};
	
	
	struct CStr_Escaped {
		cstr	str;
	};
	struct LStr_Escaped {
		lstr	str;
	};
	
	// No sizes for now (a s16 with val -1 should print 0xffff not 0xffffffffffffffff for ex.)
	struct Hex {
		u64		val;
	};
	struct Bin {
		u64		val;
	};
	
	struct Rep_S {
		cstr	str;
		uptr	count;
	};
	
	DECL FORCEINLINE CStr_Escaped escaped (cstr s) {
		return {s};
	}
	DECL FORCEINLINE CStr_Escaped escaped (char* s) {
		return {s};
	}
	DECL FORCEINLINE LStr_Escaped escaped (lstr s) {
		return {s};
	}
	DECL FORCEINLINE LStr_Escaped escaped (mlstr s) {
		return {lstr(s)};
	}
	
	
	DECL FORCEINLINE Hex hex (uchar i) {		return {i}; }
	DECL FORCEINLINE Hex hex (ushort i) {		return {i}; }
	DECL FORCEINLINE Hex hex (ui i) {			return {i}; }
	DECL FORCEINLINE Hex hex (ulong i) {		return {i}; }
	DECL FORCEINLINE Hex hex (ullong i) {		return {i}; }
	DECL FORCEINLINE Hex hex (schar i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Hex hex (sshort i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Hex hex (si i) {			return {(u64)(s64)i}; }
	DECL FORCEINLINE Hex hex (slong i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Hex hex (sllong i) {		return {(u64)(s64)i}; }
	
	DECL FORCEINLINE Bin bin (uchar i) {		return {i}; }
	DECL FORCEINLINE Bin bin (ushort i) {		return {i}; }
	DECL FORCEINLINE Bin bin (ui i) {			return {i}; }
	DECL FORCEINLINE Bin bin (ulong i) {		return {i}; }
	DECL FORCEINLINE Bin bin (ullong i) {		return {i}; }
	DECL FORCEINLINE Bin bin (schar i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Bin bin (sshort i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Bin bin (si i) {			return {(u64)(s64)i}; }
	DECL FORCEINLINE Bin bin (slong i) {		return {(u64)(s64)i}; }
	DECL FORCEINLINE Bin bin (sllong i) {		return {(u64)(s64)i}; }
	
	DECL FORCEINLINE Rep_S repeat (cstr str, uptr count) {	return {str, count}; }
	
	#if _PLATF == PLATF_GENERIC_WIN
	typedef HANDLE fileh_t;
	#elif _PLATF == PLATF_GENERIC_UNIX
	static_assert(false, "reimplement");
	#endif
	
	struct Base_Printer;
	typedef DECLFP void (*printstr_fp) (Base_Printer* this_, lstr cr str);
	
	enum putval_type_e : u32 {
		DYNARR=0,
		STK,
		TMP_STK,
	};
	struct Base_Printer {
		union {
			dynarr<char>*	arr;
			Stack*			stk;
			uptr			generic_putval;
		};
		putval_type_e		putval_type;
		
		printstr_fp			printstr;
		
		constexpr Base_Printer (): arr{nullptr}, putval_type{(putval_type_e)0}, printstr{nullptr} {}
		constexpr Base_Printer (dynarr<char>* a, putval_type_e b, printstr_fp d):
				arr{a}, putval_type{b}, printstr{d} {}
		constexpr Base_Printer (Stack* a, putval_type_e b, printstr_fp d):
				stk{a}, putval_type{b}, printstr{d} {}
		
	};
	
	struct Printer : public Base_Printer {
		
		constexpr Printer (): Base_Printer{} {}
		constexpr Printer (Stack* a, putval_type_e b, u32 c, printstr_fp d):
				Base_Printer{a,b,d} {}
		constexpr Printer (dynarr<char>* a, putval_type_e b, printstr_fp d):
				Base_Printer{a,b,d} {}
		
		template <typename... Ts>
		FORCEINLINE void operator() (char const* str, Ts... args);
		
	};
	struct Printer_Stk : public Base_Printer {
		
		constexpr Printer_Stk (): Base_Printer{} {}
		constexpr Printer_Stk (Stack* a, putval_type_e b, printstr_fp d):
				Base_Printer{a,b,d} {}
		
		template <typename... Ts>
		FORCEINLINE lstr operator() (char const* str, Ts... args);
		
	};
	struct Printer_File : public Base_Printer {
		fileh_t fhandle;
		
		Printer_File () {}
		Printer_File (fileh_t h, Stack* a, putval_type_e b, printstr_fp d):
				Base_Printer{a,b,d}, fhandle{h} {}
		
		template <typename... Ts>
		FORCEINLINE void operator() (char const* str, Ts... args);
		
		template <typename... Ts>
		FORCEINLINE void operator() (u32 indent, char const* str, Ts... args);
		
	};
	
	Printer			make_printer_stdout ();
	Printer			make_printer_stderr ();
	Printer_Stk		make_printer_stk (Stack* stk, printstr_fp printstr=nullptr);
	Printer			make_printer_dynarr (dynarr<char>* arr, printstr_fp printstr=nullptr);
	Printer_File	make_printer_file (fileh_t fhandle);
	
}
	
DECLD thread_local print_n::Printer		print =				print_n::make_printer_stdout();	// print to stdout and full.log
DECLD thread_local print_n::Printer		print_err =			print_n::make_printer_stderr();	// print to stderr (with red color if possible) and err.log and full.log
DECLD thread_local print_n::Printer_Stk	print_working_stk =	print_n::make_printer_stk(&working_stk);
	
	