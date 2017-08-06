	
////
	// SI prefixed (don't use these for bytes!)
	template <typename T> DECL constexpr FORCEINLINE T milli (T val) {	return val /		static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T micro (T val) {	return milli(val) /	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T nano (T val) {	return micro(val) /	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T pico (T val) {	return nano(val) /	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T femto (T val) {	return pico(val) /	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T atto (T val) {	return femto(val) /	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T kilo (T val) {	return val *		static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T mega (T val) {	return kilo(val) *	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T giga (T val) {	return mega(val) *	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T tera (T val) {	return giga(val) *	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T peta (T val) {	return tera(val) *	static_cast<T>(1000); }
	template <typename T> DECL constexpr FORCEINLINE T exa (T val) {	return peta(val) *	static_cast<T>(1000); }
	// 
	template <typename T> DECL constexpr FORCEINLINE T kibi (T val) {	return val *		static_cast<T>(1024); }
	template <typename T> DECL constexpr FORCEINLINE T mebi (T val) {	return kibi(val) *	static_cast<T>(1024); }
	template <typename T> DECL constexpr FORCEINLINE T gibi (T val) {	return mebi(val) *	static_cast<T>(1024); }
	template <typename T> DECL constexpr FORCEINLINE T tebi (T val) {	return gibi(val) *	static_cast<T>(1024); }
	template <typename T> DECL constexpr FORCEINLINE T pebi (T val) {	return tebi(val) *	static_cast<T>(1024); }
	
////
	template <typename T> DECL constexpr FORCEINLINE T					SQUARED (T x) {
		return x*x;
	}
	template <typename T> DECL constexpr FORCEINLINE T					CUBED (T x) {
		return x*x*x;
	}
	
	template <typename T> DECL constexpr FORCEINLINE T					MIN (T a, T b) {
		return a <= b ? a : b;
	}
	template <typename T> DECL constexpr FORCEINLINE T					MAX (T a, T b) {
		return a >= b ? a : b;
	}
	
	template <typename T, typename... Ts> DECL constexpr FORCEINLINE T	MIN (T a, T b, Ts... args) {
		return MIN( MIN(a, b), args... );
	}
	template <typename T, typename... Ts> DECL constexpr FORCEINLINE T	MAX (T a, T b, Ts... args) {
		return MAX( MAX(a, b), args... );
	}
	
	template <typename T> DECL constexpr FORCEINLINE T	clamp (T val, T min, T max) {
		return MIN( MAX(val, min), max );
	}
	
////
	template <typename RET_T=uptr, typename T, uptr N> DECL constexpr FORCEINLINE RET_T
																		arrlenof (T (& arr)[N]) {
		static_assert(N <= limits::max<RET_T>(), "arrlenof():: out of range!");
		return static_cast<RET_T>(N);
	}
	template <typename RET_T=uptr, typename T, uptr N> DECL constexpr FORCEINLINE RET_T
																		arrsizeof (T (& arr)[N]) {
		static_assert(sizeof(arr) <= limits::max<RET_T>(), "arrlenof():: out of range!");
		return static_cast<RET_T>(sizeof(arr));
	}
	
	template <typename RET_T, typename... Ts> DECL constexpr FORCEINLINE RET_T	packlenof () {
		static_assert(sizeof...(Ts) <= limits::max<RET_T>(), "packlenof():: out of range!");
		return static_cast<RET_T>(sizeof...(Ts));
	}
	
////
	template <typename T> DECL constexpr FORCEINLINE T						align_down (T i, T a) {
		return i -(i % a);
	}
	template <typename T> DECL constexpr FORCEINLINE T						align_up (T i, T a) {
		return (i % a) == 0 ? i : i +(a -(i % a));
	}
	template <typename T, typename TA> DECL constexpr FORCEINLINE T*		align_down (T* i, TA a) {
		return (T*)( (uptr)i -((uptr)i % a) );
	}
	template <typename T, typename TA> DECL constexpr FORCEINLINE T*		align_up (T* i, TA a) {
		return ((uptr)i % a) == 0 ? i :(T*)((uptr)i +(a -((uptr)i % a)) );
	}
	
	template <typename T> DECL constexpr FORCEINLINE bool					is_aligned (T i, T a) {
		return (i % a) == 0;
	}
	template <typename T, typename TA> DECL constexpr FORCEINLINE bool		is_aligned (T const* i, TA a) {
		return ((uptr)i % a) == 0;
	}
	
////
	DECL u64 aligndown_power_of_two (u64 val) { // 0 -> 0   (2^64)-1 -> 2^53
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		val |= val >> 32;
		return val -(val >> 1);
	}
	DECL u64 alignup_power_of_two (u64 val) { // 0 -> 0   (2^64)-1 -> 0 [overflow]
		--val;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		val |= val >> 32;
		return ++val;
	}
	DECL s64 aligndown_power_of_two (s64 val) {
		return (s64)aligndown_power_of_two((u64)val);
	}
	DECL s64 alignup_power_of_two (s64 val) {
		return (s64)alignup_power_of_two((u64)val);
	}
	
	DECL u32 aligndown_power_of_two (u32 val) {
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		return val -(val >> 1);
	}
	DECL u32 alignup_power_of_two (u32 val) {
		--val;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		return ++val;
	}
	DECL s32 aligndown_power_of_two (s32 val) {
		return (s32)aligndown_power_of_two((u32)val);
	}
	DECL s32 alignup_power_of_two (s32 val) {
		return (s32)alignup_power_of_two((u32)val);
	}
	
////
	template <typename T>
	DECL constexpr uptr ptr_sub (T const* a, T const* b) {
		return (uptr)b -(uptr)a;
	}
	template <typename T, typename OFFS_T>
	DECL constexpr T* ptr_add (T* a, OFFS_T b) {
		return (T*)((uptr)a +b);
	}
	
////
	#include "string.h"
	
	DECL void* cmemset (void* ptr, u8 val, uptr size) {
		return memset(ptr, val, size);
	}
	DECL void* cmemcpy (void* dest, void const* src, uptr size) {
		return memcpy(dest, (void*)src, size);
	}
	DECL void* cmemmove (void* dest, void const* src, uptr size) {
		return memmove(dest, src, size);
	}
	DECL bool cmemcmp (void const* l, void const* r, uptr size) {
		return memcmp(l, r, size) == 0;
	}
	
////
namespace safe {
	
	
	template <typename T_CAST, typename T>	DECLT constexpr bool _safe_cast (T val);
	
	#define SAFE_CAST(t_cast, t) template<> DECLT constexpr bool _safe_cast<t_cast, t> (t val)
	
	DECLD constexpr u64 MAX_S64 =		0x8000000000000000ull;
	DECLD constexpr u64 MAX_U32 =		       0x100000000ull;
	DECLD constexpr u32 MAX_S32 =		        0x80000000ul;
	
	SAFE_CAST(s64, u64) { return val < MAX_S64; }
	SAFE_CAST(u64, u64) { return true; }
	SAFE_CAST(u32, u64) { return val < MAX_U32; }
	SAFE_CAST(s32, u64) { return val < MAX_S32; }
	SAFE_CAST(u32, s64) { return val >= 0; }
	
	SAFE_CAST(s64, u32) { return true; }
	SAFE_CAST(u32, u32) { return true; }
	SAFE_CAST(s32, u32) { return val < MAX_S32; }
	SAFE_CAST(s64, s32) { return true; }
	SAFE_CAST(s32, s32) { return true; }
	SAFE_CAST(u32, s32) { return val >= 0; }
	
	SAFE_CAST(u32, u16) { return true; }
	SAFE_CAST(s32, u16) { return true; }
	SAFE_CAST(s32, s16) { return true; }
	
	#undef SAFE_CAST
	
	#define safe_cast(t, val) _safe_cast<typename types::NormalizeIntType<t>::type>(types::normalize_int_type(val))
	
	template <typename T_CAST, typename T>
	DECL T_CAST _safe_cast_assert (T val) {
		assert(safe_cast(T_CAST, val), "_safe_cast_assert:: out of range !(% >= % >= %)", limits::min<T_CAST>(), val, limits::max<T_CAST>());
		return static_cast<T_CAST>(val);
	}
	
	#define safe_cast_assert(t, val) _safe_cast_assert<t>(val)
	
	
	DECL constexpr bool safe_add (u32 l, uptr r) {
		return (uptr)(limits::max<u32>() -l) >= r;
	}
	DECL constexpr bool safe_add (u64 l, uptr r) {
		return (uptr)(limits::max<u64>() -l) >= r;
	}
	
	DECL constexpr bool safe_add (u32 l, uptr a, uptr b) {
		return safe_add(a, b) && (uptr)(limits::max<u32>() -l) >= (a +b);
	}
	DECL constexpr bool safe_add (u64 l, uptr a, uptr b) {
		return safe_add(a, b) && (uptr)(limits::max<u64>() -l) >= (a +b);
	}
	
	DECL u32 safe_add_assert (u32 l, uptr r) {
		assert(safe_add(l, r), "safe_add_assert:: addition overflow (% +% > %)", l, r, limits::max<u32>());
		return l +(u32)r;
	}
	DECL u64 safe_add_assert (u64 l, uptr r) {
		assert(safe_add(l, r), "safe_add_assert:: addition overflow (% +% > %)", l, r, limits::max<u64>());
		return l +(u64)r;
	}
	
	template <typename T_CAST, typename T>
	DECL constexpr T_CAST static_safe_cast_assert (T VAL) {
		static_assert(safe_cast(T_CAST, VAL), "static_safe_cast_assert:: out of range");
		//static_assert(safe_cast(T_CAST, VAL), "static_safe_cast_assert:: out of range");
		return static_cast<T_CAST>(VAL);
	}
	
	#define offsetof_t(t, s, m)		(t)(offsetof(s, m))
	#define sizeof_t(t, val)		(t)(sizeof(val))
	
	//#define offsetof_t(t, s, m)		static_safe_cast_assert<t>(offsetof(s, m))
	//#define sizeof_t(t, val)		static_safe_cast_assert<t>(sizeof(val))
	
}
using namespace safe;
	
////
namespace str {
	DECL u32 len (char const* str) {
		char const* cur = str;
		while (*cur != '\0') { ++cur; }
		auto len = ptr_sub(str, cur);
		assert(safe_cast(u32, len));
		return (u32)len;
	}
}
	
//// lstr
	template <u32 N>
	DECL constexpr FORCEINLINE u32 lit_cstrlen (char const (&lit)[N]) {
		STATIC_ASSERT(N > 0);
		return N -1;
	}

	struct mlstr {
		char*		str;
		u32			len;
		
		constexpr mlstr (char* str, u32 len):	str{str}, len{len} {}
		
		constexpr static mlstr null () { return {nullptr, 0}; }
		
		static mlstr count_cstr (char* str) {
			return mlstr(str, str::len(str));
		}
		
		constexpr operator bool () const { // Check if null
			return str != nullptr;
		}
	};
	struct lstr {
		char const*	str;
		u32			len;
		
		lstr () {}
		constexpr lstr (char const* str, u32 len):	str{str}, len{len} {}
		constexpr lstr (mlstr s):					str{s.str}, len{s.len} {}
		
		constexpr static lstr null () { return {nullptr, 0}; }
		
		template <u32 LEN>
		constexpr lstr (char const (& lit)[LEN]):	str{lit}, len{LEN -1} {
			STATIC_ASSERT(LEN > 0);
		}
		
		static lstr count_cstr (char const* str) {
			return lstr(str, str::len(str));
		}
		
		constexpr operator bool () const { // Check if null
			return str != nullptr;
		}
	};
	
////
namespace str {
	DECL bool comp (char const* a, char const* b) {
		for (;;) {
			if (*a != *b) {
				return false;
			}
			if (*a == '\0') {
				return true;
			}
			++a; ++b;
		}
	}
	
	// Supports strings with length 0, even though no actual characters are compared, two strings of length 0 are considered equal
	DECL bool comp (lstr cr a, lstr cr b) {
		char const*	a_ = a.str;
		char const*	b_ = b.str;
		
		if (a.len != b.len) {
			return false;
		}
		
		char const* end = a_ +a.len;
		while (a_ != end) {
			if (*a_++ != *b_++) {
				return false;
			}
		}
		return true;
	}
	
	#if 0
	DECL bool comp (char const* a, char const* b, uptr* out_len) {
		char const* cur = a;
		for (;;) {
			if (*cur != *b) {
				return false;
			}
			if (*cur == '\0') {
				*out_len = getPtrOffset(a, cur);
				return true;
			}
			++cur; ++b;
		}
	}
	#endif
	
	DECL bool comp_up_to_count (char const* a, char const* b, uptr len) {
		char const* end = a +len;
		while (a != end) {
			if (*a++ != *b) {
				return false;
			}
			if (*b++ == '\0') {
				return true; // NOTE: if strings are both null terminated and shorter than len and equal we return true
			}
		}
		return true;
	}
	
	// for c=='/'
	// "blah/dir/file.ext" ->	"/file.ext"
	// "file.ext" ->			"/file.ext"
	// "blah/" ->				"/"
	// "" ->					null
	DECL lstr find_tail_incl (lstr path, char c) {
		cstr ret = nullptr;
		cstr cur = path.str;
		cstr end = cur +path.len;
		while (cur != end) {
			if (*cur == c) {
				ret = cur;
			}
			++cur;
		}
		return ret ? lstr(ret, (u32)ptr_sub(ret, end)): lstr(nullptr,0);
	}
	// for c=='/'
	// "blah/dir/file.ext" ->	"file.ext"
	// "file.ext" ->			"file.ext"
	// "blah/" ->				""
	// "" ->					""
	DECL lstr find_tail_excl (lstr path, char c) {
		cstr ret = nullptr;
		cstr cur = path.str;
		cstr end = cur +path.len;
		while (cur != end) {
			if (*cur == c) {
				ret = cur +1;
			}
			++cur;
		}
		return ret ? lstr(ret, (u32)ptr_sub(ret, end)): lstr(nullptr,0);
	}
	
}
	
////
namespace path {
	// "dir\file.ext" -> "file.ext"
	// "file.ext" -> "file.ext"
	DECL lstr find_filename (lstr path) {
		return str::find_tail_excl(path, '\\');
	}
	
	// "blah.ext" ->	".ext"
	// "blah." ->		""
	// "blah" ->		null
	DECL lstr find_ext (lstr path) {
		return str::find_tail_incl(path, '.');
	}
}
	

namespace parse_n {
	
	////////
	
	#define require(cond) if (!(cond)) { return 0; }
	
	#define WHITESPACE_C_CASES		' ':	case '\t'
	#define NEWLINE_C_CASES			'\r':	case '\n'
	
	#define DEC_DIGIT_CASES \
										'0': case '1': case '2': case '3': case '4': \
									case '5': case '6': case '7': case '8': case '9'
	
	#define BIN_DIGIT_CASES			'0': case '1'
	#define HEX_LOWER_CASES			'a': case 'b': case 'c': case 'd': case 'e': case 'f'
	#define HEX_UPPER_CASES			'A': case 'B': case 'C': case 'D': case 'E': case 'F'
	#define HEX_DIGIT_CASES			DEC_DIGIT_CASES: case HEX_LOWER_CASES: case HEX_UPPER_CASES
	
	DECL bool is_whitespace_c (char c) {
		switch (c) {
			case WHITESPACE_C_CASES:	return true;
			default:					return false;
		}
	}
	DECL bool is_newline_c (char c) {
		switch (c) {
			case NEWLINE_C_CASES:	return true;
			default:				return false;
		}
	}
	DECL bool is_dec_digit_c (char c) {
		switch (c) {
			case DEC_DIGIT_CASES:	return true;
			default:				return false;
		}
	}
	DECL bool is_bin_digit_c (char c) {
		switch (c) {
			case BIN_DIGIT_CASES:	return true;
			default:				return false;
		}
	}
	DECL bool is_hex_lower_c (char c) {
		switch (c) {
			case HEX_LOWER_CASES:	return true;
			default:				return false;
		}
	}
	DECL bool is_hex_upper_c (char c) {
		switch (c) {
			case HEX_UPPER_CASES:	return true;
			default:				return false;
		}
	}
	DECL bool is_hex_digit_c (char c) {
		switch (c) {
			case HEX_DIGIT_CASES:	return true;
			default:				return false;
		}
	}
	
	DECL bool is_identifier_start_c (char c) {
		return (c >= 'a' && c <= 'z') || c == '_' || (c >= 'A' && c <= 'Z');
	}
	DECL bool is_identifier_c (char c) {
		return is_identifier_start_c(c) || is_dec_digit_c(c);
	}
	
	DECL char* whitespace (char const* cur) {
		require(*cur == ' ' || *cur == '\t');
		
		do {
			++cur;
		} while (*cur == ' ' || *cur == '\t');
		
		return (char*)cur;
	}
	DECL char* newline (char const* cur) {
		char c = *cur;
		require(*cur == '\r' || *cur == '\n');
		
		++cur;
		if (*cur != c && (*cur == '\r' || *cur == '\n')) {
			++cur;
		}
		
		return (char*)cur;
	}
	DECL char* newline (char const* cur, u32* line_num, char** line_begin) {
		require(cur = newline(cur));
		
		++(*line_num);
		*line_begin = (char*)cur;
		
		return (char*)cur;
	}
	
	#undef require
	
}
	
	template <typename FUNC>
	struct AtScopeExit {
		FUNC	f;
		void operator= (AtScopeExit &) = delete;
		
		FORCEINLINE AtScopeExit (FUNC f): f(f) {}
		FORCEINLINE ~AtScopeExit () { f(); }
	};
	
	struct DeferTask {};
	
	template<typename FUNC>
	DECL AtScopeExit<FUNC> operator+(DeferTask, FUNC f) {
		return AtScopeExit<FUNC>(f);
	}
	
	#define _defer(counter) auto CONCAT(func, counter) = DeferTask() +[&] () 
	#define defer _defer(__COUNTER__)
	// use like: defer { lambda code };
	
	// defer with capture-by-value lambda
	#define _defer_by_val(counter) auto CONCAT(func, counter) = DeferTask() +[=] () 
	#define defer_by_val _defer_by_val(__COUNTER__)
	
	#undef DEFINE_ENUM_FLAG_OPS
	#define DEFINE_ENUM_FLAG_OPS(TYPE, UNDERLYING_TYPE) \
		DECL constexpr FORCEINLINE TYPE& operator|= (TYPE& l, TYPE r) { \
			return l = (TYPE)((UNDERLYING_TYPE)l | (UNDERLYING_TYPE)r); \
		} \
		DECL constexpr FORCEINLINE TYPE& operator&= (TYPE& l, TYPE r) { \
			return l = (TYPE)((UNDERLYING_TYPE)l & (UNDERLYING_TYPE)r); \
		} \
		DECL constexpr FORCEINLINE TYPE operator| (TYPE l, TYPE r) { \
			return (TYPE)((UNDERLYING_TYPE)l | (UNDERLYING_TYPE)r); \
		} \
		DECL constexpr FORCEINLINE TYPE operator& (TYPE l, TYPE r) { \
			return (TYPE)((UNDERLYING_TYPE)l & (UNDERLYING_TYPE)r); \
		} \
		DECL constexpr FORCEINLINE TYPE operator~ (TYPE e) { \
			return (TYPE)(~(UNDERLYING_TYPE)e); \
		}
	
	#define DEFINE_ENUM_ITER_OPS(TYPE, UNDERLYING_TYPE) \
		DECL constexpr FORCEINLINE TYPE& operator++ (TYPE& val) { \
			return val = (TYPE)((UNDERLYING_TYPE)val +1); \
		} \
		DECL constexpr FORCEINLINE DEPRECATED TYPE inc (TYPE val) { \
			return (TYPE)((UNDERLYING_TYPE)val +1); \
		}
	
////
	DECLD constexpr char hex_digits[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	
	DECL u32 print_escaped_ascii_len (char c, char* buf) { // Buf must be at least 4 chars large
		buf[0] = '\\';
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';
		
		u32 len;
		if (c >= 0x20 && c <= 0x7e) { // ' ' - '~' (All printable characters)
			buf[0] = c;
			len = 1;
		} else {
			switch (c) {
				case '\0':						// NUL		Null
				{ // 0x00
					buf[1] = '0';
					len = 2;
				} break;
				
				case '\a':						// BEL		Bell
				case '\b':						// BS		Backspace
				case '\t':						// HT		Horizontal Tab
				case '\n':						// LF		Line Feed
				case '\v':						// VT		Vertical Tab
				case '\f':						// FF		Form Feed
				case '\r':						// CR		Carriage Return
				{ // 0x07 - 0x0d
					static constexpr char arr[7] = { 'a','b','t','n','v','f','r' };
					buf[1] = arr[c -7];
					len = 2;
				} break;
				
				case '\x01':					// SOH		Start of Heading
				case '\x02':					// STX		Start of Text
				case '\x03':					// ETX		End of Text
				case '\x04':					// EOT		End of Transmission
				case '\x05':					// ENQ		Enquiry
				case '\x06':					// ACK		Acknowledgment
				
				case '\x0e':					// SO		Shift Out
				case '\x0f':					// SI		Shift In
				case '\x10':					// DLE		Data Link Escape
				case '\x11':					// DC1		Device Control 1 (XON)
				case '\x12':					// DC2		Device Control 2
				case '\x13':					// DC3		Device Control 3 (XOFF)
				case '\x14':					// DC4		Device Control 4
				case '\x15':					// NAK		Negative Acknowledgment
				case '\x16':					// SYN		Synchronous Idle
				case '\x17':					// ETB		End of Transmission Block
				case '\x18':					// CAN		Cancel
				case '\x19':					// EM		End of Medium
				case '\x1a':					// SUB		Substitute
				case '\x1b':					// ESC		Escape
				case '\x1c':					// FS		File Separator
				case '\x1d':					// GS		Group Separator
				case '\x1e':					// RS		Record Separator
				case '\x1f':					// US		Unit Separator
				case '\x7f':					// DEL		Delete
				default:
				{ // 0x01 - 0x06  and  0x0e - 0x1f  and  0x7f - 0xff
					buf[1] = 'x';
					buf[2] = hex_digits[(c >> 4) & 0x0f];
					buf[3] = hex_digits[(c >> 0) & 0x0f];
					len = 4;
				} break;
			}
		}
		return len;
	}
	DECL char* print_escaped_ascii_term (char c, char* buf) { // Buf must be at least 5 chars large
		buf[4] = '\0';
		print_escaped_ascii_len(c, buf);
		return buf;
	}
	