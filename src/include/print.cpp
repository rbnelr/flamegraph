	
#include "stdio.h"
	
namespace print_n {
	
	DECLD lstr indent_str = "  ";
	
	HANDLE error_log_file;
	HANDLE full_log_file;
	
	#if _PLATF == PLATF_GENERIC_WIN
	HANDLE	stdout_handle;
	HANDLE	stderr_handle;
	
	DECLD constexpr lstr newlineSequence = "\r\n";
	
	#define S32_PRINTF_CODE				"I32d"
	#define S64_PRINTF_CODE				"I64d"
	#define U32_PRINTF_CODE				"I32u"
	#define U64_PRINTF_CODE				"I64u"
	
	DECLM WORD get_console_color (HANDLE h) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		
		auto ret = GetConsoleScreenBufferInfo(h, &info);
		assert(ret != 0);
		
		return info.wAttributes;
	}
	
	DECLM void set_console_color (HANDLE h, WORD col) {
		auto ret = SetConsoleTextAttribute(h, col);
		assert(ret != 0);
	}
	DECLM void set_console_error_color (HANDLE h) {
		set_console_color(h, FOREGROUND_RED|FOREGROUND_INTENSITY);
	}
	DECLM void set_console_profile_color (HANDLE h) {
		set_console_color(h, FOREGROUND_RED|FOREGROUND_GREEN);
	}
	
	#elif _PLATF == PLATF_GENERIC_UNIX
	DECLD constexpr lstr newlineSequence = "\n";
	
	#define S32_PRINTF_CODE				"d"
	#define S64_PRINTF_CODE				"lld"
	#define U32_PRINTF_CODE				"u"
	#define U64_PRINTF_CODE				"llu"
	
	DECL int platformGetStdStream (const char * stdDevice) {
		auto ret = open(stdDevice, O_RDONLY);
		assert_never_print(ret != -1);
		return ret;
	}
	
	#endif
	
	DECL void print_stdout (Base_Printer* this_, lstr cr str) {
		
		#if _PLATF == PLATF_GENERIC_WIN
		
		{
			DWORD numWritten;
			auto ret = WriteFile(stdout_handle, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		if (full_log_file != INVALID_HANDLE_VALUE) {
			DWORD numWritten;
			auto ret = WriteFile(full_log_file, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		OutputDebugString(str.str);
		
		#elif _PLATF == PLATF_GENERIC_UNIX
		printf("%.*s", str.len, str.str);
		#endif
		
	}
	DECL void print_stderr (Base_Printer* this_, lstr cr str) {
		
		#if _PLATF == PLATF_GENERIC_WIN
		auto old_col = get_console_color(stderr_handle);
		set_console_error_color(stderr_handle);
		
		{
			DWORD numWritten;
			auto ret = WriteFile(stderr_handle, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		if (error_log_file != INVALID_HANDLE_VALUE) {
			DWORD numWritten;
			auto ret = WriteFile(error_log_file, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		if (full_log_file != INVALID_HANDLE_VALUE) {
			DWORD numWritten;
			auto ret = WriteFile(full_log_file, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		OutputDebugString(str.str); // TODO: not correctly null terminated at the moment
		
		set_console_color(stderr_handle, old_col);
		
		#elif _PLATF == PLATF_GENERIC_UNIX
		printf("%.*s", str.len, str.str);
		#endif
		
	}
	DECL void print_file (Base_Printer* this_, lstr cr str) {
		auto prin = (Printer_File*)this_;
		
		#if _PLATF == PLATF_GENERIC_WIN
		{
			DWORD numWritten;
			auto ret = WriteFile(prin->fhandle, str.str, (DWORD)str.len, &numWritten, NULL);
			if (ret == 0 || numWritten != (DWORD)str.len) {
				DBGBREAK_IF_DEBUGGER_PRESENT;
			}
		}
		#elif _PLATF == PLATF_GENERIC_UNIX
		static_assert(false, "reimplement");
		#endif
	}
	
	Printer			make_printer_stdout () {
		return { &working_stk, TMP_STK, 0, print_stdout };
	}
	Printer			make_printer_stderr () {
		return { &working_stk, TMP_STK, 0, print_stderr };
	}
	Printer_Stk		make_printer_stk (Stack* stk, printstr_fp printstr) {
		return { stk, STK, printstr };
	}
	Printer			make_printer_dynarr (dynarr<char>* arr, printstr_fp printstr) {
		return { arr, DYNARR, printstr };
	}
	Printer_File	make_printer_file (fileh_t fhandle) {
		return { fhandle, &working_stk, TMP_STK, print_file };
	}
	
	#if _PLATF == PLATF_GENERIC_WIN
	DECL void init () {
		stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (stdout_handle == INVALID_HANDLE_VALUE && stdout_handle == NULL) {
			DBGBREAK_IF_DEBUGGER_PRESENT;
		}
		
		stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
		if (stderr_handle == INVALID_HANDLE_VALUE && stderr_handle == NULL) {
			DBGBREAK_IF_DEBUGGER_PRESENT;
		}
		
		{
			DEFER_POP(&working_stk);
			
			lstr exe_path =			win32::get_exe_path(&working_stk);
			lstr exe_filename =		win32::find_exe_filename(exe_path);
			lstr pc_name =			win32::get_pc_name(&working_stk);
			
			SYSTEMTIME loc, utc;
			
			GetLocalTime(&loc);
			GetSystemTime(&utc);
			
			cstr error_filename = print_working_stk("logs/%.%.%-%.%.%_%_%.error.log\\0",
				loc.wYear, loc.wMonth, loc.wDay,
				loc.wHour, loc.wMinute, loc.wSecond,
				exe_filename, pc_name ).str;
			
			cstr full_filename = print_working_stk("logs/%.%.%-%.%.%_%_%.full.log\\0",
				loc.wYear, loc.wMonth, loc.wDay,
				loc.wHour, loc.wMinute, loc.wSecond,
				exe_filename, pc_name ).str;
				
			lstr loc_ms_str = print_working_stk("%.%.%-%.%.%.%",
				loc.wYear, loc.wMonth, loc.wDay,
				loc.wHour, loc.wMinute, loc.wSecond, loc.wMilliseconds );
				
			lstr utc_ms_str = print_working_stk("%.%.%-%.%.%.%",
				utc.wYear, utc.wMonth, utc.wDay,
				utc.wHour, utc.wMinute, utc.wSecond, utc.wMilliseconds );
			
			win32::overwrite_file_wr(error_filename, &error_log_file);
			win32::overwrite_file_wr(full_filename, &full_log_file);
			
			if (error_log_file != INVALID_HANDLE_VALUE) {
				auto print_file = make_printer_file(error_log_file);
				print_file(" -----------------------------------------------------------------------------------------------\n");
				print_file("| Error log file (whatever was printed with conerr)\n");
				print_file("| Run at % local time (% utc time)\n", loc_ms_str, utc_ms_str);
				print_file("| Run on '%' at '%'\n", pc_name, exe_path);
				print_file(" -----------------------------------------------------------------------------------------------\n");
			}
			
			if (full_log_file != INVALID_HANDLE_VALUE) {
				auto print_file = make_printer_file(full_log_file);
				print_file(" -----------------------------------------------------------------------------------------------\n");
				print_file("| Full log file (whatever was printed with conout or conerr)\n");
				print_file("| Run at % local time (% utc time)\n", loc_ms_str, utc_ms_str);
				print_file("| Run on '%' at '%'\n", pc_name, exe_path);
				print_file(" -----------------------------------------------------------------------------------------------\n");
			}
			
		}
		
	}
	#elif _PLATF == PLATF_GENERIC_UNIX
	
	#endif
	
	////
	template <typename T>
	struct ForEach {
		static constexpr print_type_e VAL =								OTHER;
		static FORCEINLINE char do_ (T cr arg) {						return '\0'; } 
	};
	
	template<typename T> struct ForEach<T*> {
		static constexpr print_type_e VAL =								PTR;
		static FORCEINLINE uptr do_ (T* p) {							return reinterpret_cast<uptr>(p); }
	};
	
	template<> struct ForEach<char*> {
		static constexpr print_type_e VAL =								CSTR;
		static FORCEINLINE char const* do_ (char* cstr) {				return cstr; }
	};
	template<> struct ForEach<const char*> {
		static constexpr print_type_e VAL =								CSTR;
		static FORCEINLINE const char* do_ (const char* cstr) {			return cstr; }
	};
	template<> struct ForEach<CStr_Escaped> {
		static constexpr print_type_e VAL =								CSTR_ESC;
		static FORCEINLINE CStr_Escaped do_ (CStr_Escaped cstr) {		return cstr; }
	};
	
	template<> struct ForEach<lstr> {
		static constexpr print_type_e VAL =								LSTR;
		static FORCEINLINE lstr do_ (lstr str) {						return str; }
	};
	template<> struct ForEach<mlstr> {
		static constexpr print_type_e VAL =								LSTR;
		static FORCEINLINE lstr do_ (mlstr str) {						return lstr(str); }
	};
	template<> struct ForEach<LStr_Escaped> {
		static constexpr print_type_e VAL =								LSTR_ESC;
		static FORCEINLINE LStr_Escaped do_ (LStr_Escaped lstr) {		return lstr; }
	};
	
	template<> struct ForEach<bool> {
		static constexpr print_type_e VAL =								BOOL;
		static FORCEINLINE bool do_ (bool b) {							return b; }
	};
	
	template<> struct ForEach<char> {
		static constexpr print_type_e VAL =								CHAR;
		static FORCEINLINE char do_ (char c) {							return c; }
	};
	template<> struct ForEach<Rep_S> {
		static constexpr print_type_e VAL =								REPS;
		static FORCEINLINE Rep_S do_ (Rep_S reps) {						return reps; }
	};
	
	#define INT(SIGN, T) \
			template<> struct ForEach<T> { \
				static constexpr print_type_e VAL =						sizeof(T) == 8 ? SIGN##64 : SIGN##32; \
				typedef types::Get##SIGN##Integer<MAX<uptr>(sizeof(T), 4)>::type ret_t; \
				static FORCEINLINE ret_t do_ (T i) {					return i; } \
			};
	INT(S, schar)
	INT(S, sshort)
	INT(S, si)
	INT(S, slong)
	INT(S, sllong)
	
	INT(U, uchar)
	INT(U, ushort)
	INT(U, ui)
	INT(U, ulong)
	INT(U, ullong)
	#undef INT
	
	template<> struct ForEach<Hex> {
		static constexpr print_type_e VAL =								HEX;
		static FORCEINLINE u64 do_ (Hex h) {							return h.val; }
	};
	template<> struct ForEach<Bin> {
		static constexpr print_type_e VAL =								BIN;
		static FORCEINLINE u64 do_ (Bin b) {							return b.val; }
	};
	
	template<> struct ForEach<f32> {
		static constexpr print_type_e VAL =								F32;
		static FORCEINLINE u32 do_ (f32 f) {							return reint_flt_as_int(f); } // To prevent the automatic f32->f64 conversion that happens normally when passing f32 to variadic functions
	};
	template<> struct ForEach<f64> {
		static constexpr print_type_e VAL =								F64;
		static FORCEINLINE f64 do_ (f64 f) {							return f; }
	};
	
	
	template <typename... Ts>
	struct _rdataTypeString {
		static constexpr uptr		N = sizeof...(Ts);
		static print_type_e const	arr[N +1];
	};
	
	template <typename... Ts>
	print_type_e const	_rdataTypeString<Ts...>::arr[N +1] = {
		ForEach<Ts>::VAL..., TERMINATOR
	};
	
	template <uptr N>
	struct _Print_1;
	
	template <uptr N, typename T>
	struct _Print_2 {
		template <typename LAMB, typename... Ts>
		static DECLM FORCEINLINE void do_ (LAMB& func, Base_Printer* this_, cstr str, print_type_e const * type_arr, T arg, Ts... args) {
			_Print_1<N>::do_(func, this_, str, type_arr, args..., arg);
		}
	};
	template<uptr N> struct _Print_2<N, CStr_Escaped> {
		template <typename LAMB, typename... Ts>
		static DECLM FORCEINLINE void do_ (LAMB& func, Base_Printer* this_, cstr str, print_type_e const * type_arr, CStr_Escaped arg, Ts... args) {
			_Print_1<N>::do_(func, this_, str, type_arr, args..., arg.str);
		}
	};
	template<uptr N> struct _Print_2<N, LStr_Escaped> {
		template <typename LAMB, typename... Ts>
		static DECLM FORCEINLINE void do_ (LAMB& func, Base_Printer* this_, cstr str, print_type_e const * type_arr, LStr_Escaped arg, Ts... args) {
			_Print_1<N>::do_(func, this_, str, type_arr, args..., arg.str);
		}
	};
	
	template <uptr N>
	struct _Print_1 {
		template <typename LAMB, typename T, typename... Ts>
		static DECLM FORCEINLINE void do_ (LAMB& func, Base_Printer* this_, cstr str, print_type_e const * type_arr, T arg, Ts... args) {
			_Print_2<N -1, T>::do_(func, this_, str, type_arr, arg, args...);
		}
	};
	template<> struct _Print_1<0> {
		template <typename LAMB, typename... Ts>
		static DECLM FORCEINLINE void do_ (LAMB& func, Base_Printer* this_, cstr str, print_type_e const * type_arr, Ts... args) {
			func(this_, str, type_arr, args...);
		}
	};
	
	template <typename LAMB, typename... Ts>
	DECL FORCEINLINE void _print_wrapper (LAMB& func, Base_Printer* this_, cstr str, Ts... args) {
		_Print_1<sizeof...(Ts)>::do_( func, this_, str, _rdataTypeString<Ts...>::arr, ForEach<Ts>::do_(args)... );
	}
	
	DECL va_list _print_core (Base_Printer* this_, char const* str, print_type_e const* type_arr, va_list args);
	
	DECLV void _recursive_print (Base_Printer* this_, char const* str, print_type_e const* type_arr, ...) {
		
		va_list vl;
		va_start(vl, type_arr);
		
		vl = _print_core(this_, str, type_arr, vl);
		
		va_end(vl);
	}
	
	DECL FORCEINLINE va_list _printCustomTypeToBuffer (Base_Printer* this_, print_type_e type, va_list args);
	
	DECL va_list _print_core (Base_Printer* this_, char const* format, print_type_e const* type_arr, va_list args) {
		
		uptr cur_type = 0;
		
		#define GET_VAL(type) va_arg(args, type)
		
		u32		arr_str;
		char*	stk_str;
		switch (this_->putval_type) {
			case DYNARR:
				arr_str = this_->arr->len;
				break;
			case STK:
			case TMP_STK:
				stk_str = this_->stk->getTop<char>();
				break;
			default: assert(false);
		}
		
		Base_Printer	sub_printer;
		sub_printer.printstr = nullptr; // we call printstr with the full printed string, so no printstr on the sub strings
		sub_printer.generic_putval = this_->generic_putval; // still print on either stack or dynarr, depending on where we are supposed to print to
		switch (this_->putval_type) {
			case DYNARR:
				sub_printer.putval_type = DYNARR;
				break;
			case STK:
			case TMP_STK:
			default:
				sub_printer.putval_type = STK; // even if we are suppsed to use the stack as a temp storage, we will still need the sub prints to stay until we pop the entire print in one go ourselves
				break;
		}
		
		auto PUTC = [&] (char c) {
			switch (this_->putval_type) {
				case DYNARR:
					this_->arr->append(c);
					break;
				case STK:
				case TMP_STK:
				default: // already asserted, shut up compiler warning
					this_->stk->push(c);
					break;
			}
		};
		auto PUTSTR = [&] (lstr cr s) {
			switch (this_->putval_type) {
				case DYNARR:
					this_->arr->append_n(s.len, s.str);
					break;
				case STK:
				case TMP_STK:
				default: // already asserted, shut up compiler warning
					cmemcpy(this_->stk->pushArr<char>(s.len), s.str, s.len);
					break;
			}
		};
		auto PUTSTR_TERM = [&] (cstr c) {
			PUTSTR(lstr::count_cstr(c));
		};
		
		auto EXPAND_BUF = [&] (u32 len) -> char* {
			switch (this_->putval_type) {
				case DYNARR:
					return this_->arr->append_n(len);
				case STK:
				case TMP_STK:
				default: // already asserted, shut up compiler warning
					return this_->stk->pushArr<char>(len);
			}
		};
		auto FIT_BUF = [&] (char* ptr, u32 len) {
			switch (this_->putval_type) {
				case DYNARR:
					this_->arr->grow_to((u32)ptr_sub(this_->arr->arr, ptr) +len);
					break;
				case STK:
				case TMP_STK:
				default: // already asserted, shut up compiler warning
					this_->stk->pop(ptr +len);
					break;
			}
		};
		
		#define PRINTF(format, val) \
				{ \
					u32 len = 512; \
					char* ptr = EXPAND_BUF(len); \
					auto ret = snprintf(ptr, len, format, val); \
					if (ret < 0 || (u32)ret >= len) { \
						DBGBREAK_IF_DEBUGGER_PRESENT; \
					} \
					FIT_BUF(ptr, ret); \
				}
		
		ui err = 0;
		
		while (*format != '\0') {
			
			switch (*format) {
				case '\\': {
					++format;
					if (!(*format == '%' || *format == '\\' || *format == '0')) { // escaped '%' or '\\' or '\0'
						DBGBREAK_IF_DEBUGGER_PRESENT;
					}
					switch (*format) {
						case '%':
						case '\\': {
							PUTC(*format++);
						} break;
						case '0': {
							PUTC('\0');
							++format;
						} break;
					}
					continue;
				}
				case '%': {
					++format;
					break; // format value
				}
				case '\n': {
					if (0) {
						PUTSTR(newlineSequence);
					} else {
						PUTC(*format++);
					}
					continue;
				}
				default: { // normal char in format
					PUTC(*format++);
					continue;
				}
			}
			
			
			
			print_type_e type = type_arr[cur_type++];
			
			switch (type) {
				
				case CSTR: {
					cstr str = GET_VAL(cstr);
					PUTSTR_TERM(str);
				} break;
				
				case REPS: {
					Rep_S rep = GET_VAL(Rep_S);
					for (uptr i=0; i<rep.count; ++i) {
						PUTSTR_TERM(rep.str);
					}
				} break;
				
				case CSTR_ESC: {
					cstr str = GET_VAL(cstr);
					char buf[4];
					while (str[0] != '\0') {
						u32 len = print_escaped_ascii_len(*str++, buf);
						PUTSTR(lstr(buf, len));
					}
				} break;
				
				case LSTR: {
					lstr str = GET_VAL(lstr);
					PUTSTR(str);
				} break;
				
				case LSTR_ESC: {
					lstr str = GET_VAL(lstr);
					cstr str_end = str.str +str.len;
					char buf[4];
					while (str.str != str_end) {
						u32 len = print_escaped_ascii_len(*str.str++, buf);
						PUTSTR(lstr(buf, len));
					}
				} break;
				
				case BOOL: {
					int b = GET_VAL(int);
					PUTSTR(b != 0 ? lstr("true") : lstr("false"));
				} break;
				
				case CHAR: {
					char c = static_cast<char>(GET_VAL(int));
					PUTC(c);
				} break;
				
				case S32: {
					s32 i = GET_VAL(s32);
					cstr format = "%+1" S32_PRINTF_CODE;
					PRINTF(format, i)
				} break;
				
				case S64: {
					s64 i = GET_VAL(s64);
					cstr format = "%+1" S64_PRINTF_CODE;
					PRINTF(format, i)
				} break;
				
				case U32: {
					u32 i = GET_VAL(u32);
					cstr format = "%1" U32_PRINTF_CODE;
					PRINTF(format, i)
				} break;
				
				case U64: {
					u64 i = GET_VAL(u64);
					cstr format = "%1" U64_PRINTF_CODE;
					PRINTF(format, i)
				} break;
				
				{
					char buf[64];
					
					case PTR:
					case HEX: {
						u64 u = GET_VAL(u64);
						for (ui i=0; i<16; ++i) {
							u64 indx = (15 -i) * 4;
							indx = u >> indx;
							indx &= 0b1111;
							buf[i] = hex_digits[indx];
						}
						PUTSTR("0x");
						PUTSTR(lstr(buf, 16));
					} break;
					case BIN: {
						u64 u = GET_VAL(u64);
						for (ui i=0; i<64; ++i) {
							u64 indx = (63 -i);
							indx = u >> indx;
							indx &= 1;
							buf[i] = hex_digits[indx];
						}
						PUTSTR("0b");
						PUTSTR(lstr(buf, 64));
					} break;
				}
				
				case F32: {
					f32 i = reint_int_as_flt(GET_VAL(u32));
					cstr format = "%+.2f";
					PRINTF(format, i)
				} break;
				case F64: {
					f64 i = GET_VAL(f64);
					cstr format = "%+.4f";
					PRINTF(format, i)
				} break;
				
				case TERMINATOR: {
					
					--cur_type; // Unget type so we don't read off the end of the array
					
					PUTC('%');
					
					err |= 0b10;
					
				} break;
				
				case OTHER:
				default: {
					auto res = _printCustomTypeToBuffer(&sub_printer, type, args);
					if (res) {
						args = res;
					} else {
						
						PUTSTR("<unhandled_type>");
						
						err |= 0b1;
						goto end_l;
						
						#if 0
						char c = GET_VAL(char);
						assert_never_print(c == '\0');
						
						_recursivePrintWrapper wrap;
						wrap.cur = cur;
						wrap.end = end;
						wrap.ind = ind;
						cur = _print(wrap, "<other_type>");
						#endif
					}
				}
			}
		} end_l:;
		
		if (this_->printstr) {
			lstr str;
			switch (this_->putval_type) {
				case DYNARR:
					str = lstr( &(*this_->arr)[arr_str], this_->arr->len -arr_str );
					break;
				case STK:
				case TMP_STK:
				default:
					str = lstr( stk_str, safe_cast_assert(u32, ptr_sub(stk_str, this_->stk->getTop<char>())) );
					break;
			}
			
			PUTC('\0');
			
			this_->printstr(this_, str);
			
			switch (this_->putval_type) {
				case DYNARR:
					this_->arr->grow_to( this_->arr->len -1 );
					break;
				case STK:
				case TMP_STK:
				default:
					this_->stk->pop( this_->stk->top -1 );
					break;
			}
		}
		
		switch (this_->putval_type) {
			case DYNARR:
			case STK:
				break;
			case TMP_STK:
			default:
				this_->stk->pop(stk_str);
				break;
		}
		
		if (type_arr[cur_type] != TERMINATOR) {
			err |= 0b100;
		}
		
		if (err) {
			if (err & 0b1) {
				print_err("\nTrying to print unhandled type (everything after the unhandled type will not be printed)!\n");
			}
			if (err & 0b10) {
				print_err("Not enough values for the amount of %% in print!\n");
			} else if (err & 0b100) {
				print_err("Not enough %% for the amount of values in print!\n");
			}
			
			DBGBREAK_IF_DEBUGGER_PRESENT;
		}
		
		return args;
		
		#undef GET_VAL
		#undef PUTC
		#undef _PRINTF
	}
	
	DECLV va_list _print_base_ret_vl (Base_Printer* this_, cstr str, print_type_e const * type_arr, va_list vl, lstr* out) {
		assert(this_->putval_type == STK);
		lstr ret;
		ret.str = this_->stk->getTop<char>();
		
		vl = _print_core(this_, str, type_arr, vl);
		
		ret.len = (u32)ptr_sub(ret.str, this_->stk->getTop<char>());
		*out = ret;
		return vl;
	}
	
	DECLV void _print_base (Base_Printer* this_, cstr str, print_type_e const * type_arr, ...) {
		va_list vl;
		va_start(vl, type_arr);
		
		vl = _print_core(this_, str, type_arr, vl);
		
		va_end(vl);
	}
	
	DECLV lstr _print_base_ret (Base_Printer* this_, cstr str, print_type_e const * type_arr, ...) {
		assert(this_->putval_type == STK);
		lstr ret;
		ret.str = this_->stk->getTop<char>();
		
		va_list vl;
		va_start(vl, type_arr);
		
		vl = _print_core(this_, str, type_arr, vl);
		
		va_end(vl);
		
		ret.len = (u32)ptr_sub(ret.str, this_->stk->getTop<char>());
		return ret;
	}
	struct _print_base_ret_Wrapper {
		lstr ret;
		template <typename... Ts>
		DECLM FORCEINLINE void operator() (Base_Printer* this_, cstr str, print_type_e const * type_arr, Ts... args) {
			ret = _print_base_ret(this_, str, type_arr, args...);
		}
	};
	
	//
	template <typename... Ts>
	FORCEINLINE void Printer::operator() (char const* str, Ts... args) {
		_print_wrapper(_print_base, this, str, args...);
	}
	
	//
	template <typename... Ts>
	FORCEINLINE lstr Printer_Stk::operator() (char const* str, Ts... args) {
		_print_base_ret_Wrapper func;
		_print_wrapper(func, this, str, args...);
		return func.ret;
	}
	
	//
	template <typename... Ts>
	FORCEINLINE void Printer_File::operator() (char const* str, Ts... args) {
		_print_wrapper(_print_base, this, str, args...);
	}
	
	
}
using print_n::make_printer_stk;
using print_n::make_printer_dynarr;
using print_n::make_printer_file;
