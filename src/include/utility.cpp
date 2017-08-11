	
namespace str {
	
	DECLV mlstr __append (dynarr<char, u32>* appendable, ui to_append_count, ...) {
		
		va_list	vl0, vl;
		va_start(vl0, to_append_count);
		va_copy(vl, vl0);
		
		u32 len = 0;
		for (ui i=0; i<to_append_count; ++i) {
			len = safe_add_assert(len, va_arg(vl0, lstr).len);
		}
		va_end(vl0);
		
		char* appended = appendable->grow_by(len);
		char* cur = appended;
		for (ui i=0; i<to_append_count; ++i) {
			lstr to_append = va_arg(vl, lstr);
			cmemcpy(cur, to_append.str, to_append.len);
			cur += to_append.len;
		}
		va_end(vl);
		
		return { appended, len };
	}
	DECLV mlstr __append_term (dynarr<char, u32>* appendable, ui to_append_count, ...) {
		
		va_list	vl0, vl;
		va_start(vl0, to_append_count);
		va_copy(vl, vl0);
		
		u32 len = 1;
		for (ui i=0; i<to_append_count; ++i) {
			len = safe_add_assert(len, va_arg(vl0, lstr).len);
		}
		va_end(vl0);
		
		char* appended = appendable->grow_by(len);
		char* cur = appended;
		for (ui i=0; i<to_append_count; ++i) {
			lstr to_append = va_arg(vl, lstr);
			cmemcpy(cur, to_append.str, to_append.len);
			cur += to_append.len;
		}
		va_end(vl);
		
		appended[len -1] = '\0';
		return { appended, len -1 };
	}
	
	DECLV mlstr __append (Stack* appendable, ui to_append_count, ...) {
		
		va_list	vl0, vl;
		va_start(vl0, to_append_count);
		va_copy(vl, vl0);
		
		u32 len = 0;
		for (ui i=0; i<to_append_count; ++i) {
			len = safe_add_assert(len, va_arg(vl0, lstr).len);
		}
		va_end(vl0);
		
		char* appended = appendable->pushArr<char>(len);
		char* cur = appended;
		for (ui i=0; i<to_append_count; ++i) {
			lstr to_append = va_arg(vl, lstr);
			cmemcpy(cur, to_append.str, to_append.len);
			cur += to_append.len;
		}
		va_end(vl);
		
		return { appended, len };
	}
	DECLV mlstr __append_term (Stack* appendable, ui to_append_count, ...) {
		
		va_list	vl0, vl;
		va_start(vl0, to_append_count);
		va_copy(vl, vl0);
		
		u32 len = 1;
		for (ui i=0; i<to_append_count; ++i) {
			len = safe_add_assert(len, va_arg(vl0, lstr).len);
		}
		va_end(vl0);
		
		char* appended = appendable->pushArr<char>(len);
		char* cur = appended;
		for (ui i=0; i<to_append_count; ++i) {
			lstr to_append = va_arg(vl, lstr);
			cmemcpy(cur, to_append.str, to_append.len);
			cur += to_append.len;
		}
		va_end(vl);
		
		appended[len -1] = '\0';
		return { appended, len -1 };
	}
	
	template <typename APPENDABLE, typename LSTR, typename... LSTRS>
	DECL FORCEINLINE mlstr _append (APPENDABLE* appendable, LSTR to_append, LSTRS... to_appends) {
		static_assert(types::is_same<decltype(to_append), lstr>(), "_append LSTR != lstr");
		return __append(appendable, packlenof<ui, LSTRS...>() +1, to_append, to_appends...);
	}
	template <typename APPENDABLE, typename LSTR, typename... LSTRS>
	DECL FORCEINLINE mlstr _append_term (APPENDABLE* appendable, LSTR to_append, LSTRS... to_appends) {
		static_assert(types::is_same<decltype(to_append), lstr>(), "_append LSTR != lstr");
		return __append_term(appendable, packlenof<ui, LSTRS...>() +1, to_append, to_appends...);
	}
	
	// Can't get implicit constructor call from string literal to lstr with template arguments
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append (APPENDABLE* appendable, lstr a) {
		return _append(appendable, a);
	}
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append_term (APPENDABLE* appendable, lstr a) {
		return _append_term(appendable, a);
	}
	
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append (APPENDABLE* appendable, lstr a, lstr b) {
		return _append(appendable, a, b);
	}
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append_term (APPENDABLE* appendable, lstr a, lstr b) {
		return _append_term(appendable, a, b);
	}
	
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append (APPENDABLE* appendable, lstr a, lstr b, lstr c) {
		return _append(appendable, a, b, c);
	}
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append_term (APPENDABLE* appendable, lstr a, lstr b, lstr c) {
		return _append_term(appendable, a, b, c);
	}
	
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append (APPENDABLE* appendable, lstr a, lstr b, lstr c, lstr d) {
		return _append(appendable, a, b, c, d);
	}
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append_term (APPENDABLE* appendable, lstr a, lstr b, lstr c, lstr d) {
		return _append_term(appendable, a, b, c, d);
	}
	
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append (APPENDABLE* appendable, lstr a, lstr b, lstr c, lstr d, lstr e) {
		return _append(appendable, a, b, c, d, e);
	}
	template <typename APPENDABLE> DECL FORCEINLINE mlstr append_term (APPENDABLE* appendable, lstr a, lstr b, lstr c, lstr d, lstr e) {
		return _append_term(appendable, a, b, c, d, e);
	}
	
}
	
//// Units
namespace units {
	struct Time {
		
		f64			val;
		char const*	unit;
		
		DECLM void set (f64 val_) {
			
			val = val_;
			val_ = fp::ABS(val_);
			
			if (		val_ < micro<f64>(1) ) {
				val *=	1.0 / nano<f64>(1);
				unit =	" ns";
			} else if (	val_ < milli<f64>(1) ) {
				val *=	1.0 / micro<f64>(1);
				unit =	" us";
			} else if (	val_ < 1.0 ) {
				val *=	1.0 / milli<f64>(1);
				unit =	" ms";
			} else if (	val_ < 60.0 ) {
				val *=	1.0 / 1.0;
				unit =	" sec";
			} else {
				val *=	1.0 / 60.0;
				unit =	" min";
			}
		}
		
		DECLM f64 set (u64 diff) { // return float so it can be used in throughput
			auto ftime = (f64)diff * time::QPC::inv_freq;
			set(ftime);
			return ftime;
		}
		DECLM f64 set (u64 begin, u64 end) { // return float so it can be used in throughput
			return set(end -begin);
		}
		
		Time () = default;
		
		Time (f64 diff) {
			set(diff);
		}
		
		Time (u64 diff) {
			set(diff);
		}
		Time (u64 begin, u64 end) {
			set(begin, end);
		}
	};
	struct Bytes {
		
		f64			val;
		char const*	unit;
		
		DECLM f64 set (u64 val_) { // return float so it can be used in throughput
			
			f64 fval = (f64)val_;
			val = fval;
			
			if (		val_ < kibi<u64>(1) ) {
				val *=	1.0 / 1.0;
				unit =	" B";
			} else if (	val_ < mebi<u64>(1) ) {
				val *=	1.0 / kibi<f64>(1);
				unit =	" KiB";
			} else if (	val_ < gibi<u64>(1) ) {
				val *=	1.0 / mebi<f64>(1);
				unit =	" MiB";
			} else if (	val_ < tebi<u64>(1) ) {
				val *=	1.0 / gibi<f64>(1);
				unit =	" GiB";
			} else if (	val_ < pebi<u64>(1) ) {
				val *=	1.0 / tebi<f64>(1);
				unit =	" TiB";
			} else {
				val *=	1.0 / pebi<f64>(1);
				unit =	" PiB";
			}
			
			return fval;
		}
		
		Bytes () = default;
		Bytes (u64 val) {
			set(val);
		}
	};
	struct Throughput {
		
		f64			val;
		char const*	unit;
		
		DECLM void set (f64 val_) {
			
			val = val_;
			val_ = fp::ABS(val_);
			
			if (		val_ < kibi<f64>(1) ) {
				val *=	1.0 / 1.0;
				unit =	" B/s";
			} else if (	val_ < mebi<f64>(1) ) {
				val *=	1.0 / kibi<f64>(1);
				unit =	" KiB/s";
			} else if (	val_ < gibi<f64>(1) ) {
				val *=	1.0 / mebi<f64>(1);
				unit =	" MiB/s";
			} else if (	val_ < tebi<f64>(1) ) {
				val *=	1.0 / gibi<f64>(1);
				unit =	" GiB/s";
			} else if (	val_ < pebi<f64>(1) ) {
				val *=	1.0 / tebi<f64>(1);
				unit =	" TiB/s";
			} else {
				val *=	1.0 / pebi<f64>(1);
				unit =	" PiB/s";
			}
		}
		DECLM void set (f64 bytes, f64 time) {
			set(bytes / time);
		}
		
		Throughput () = default;
		Throughput (f64 val) {
			set(val);
		}
		Throughput (f64 bytes, f64 time) {
			set(bytes, time);
		}
	};
	
	struct Size_Throughput {
		units::Bytes		s;
		units::Throughput	thr;
		
		Size_Throughput (u64 size, u64 time) {
			f64 fsize = s.set(size);
			thr.set(fsize, (f64)time * time::QPC::inv_freq);
		}
	};
	
	struct Size_Time_Throughput {
		units::Bytes		s;
		units::Time			t;
		units::Throughput	thr;
		
		Size_Time_Throughput (u64 size, u64 time) {
			f64 fsize = s.set(size);
			f64 ftime = t.set(time);
			thr.set(fsize, ftime);
		}
	};
}
	
////
	template<typename T, T VAL, uptr EXPONENT>
	struct Pow {
		static constexpr T val = VAL * Pow<T, VAL, EXPONENT -1>::val;
		static_assert(val >= VAL, "");
	};
	template<typename T, T VAL> struct Pow<T, VAL, 0> {
		static constexpr T val = 1;
	};
	
	template<uptr BASE, uptr VAL>
	struct LogPlusOne {
		static constexpr uptr val = 1 +LogPlusOne<BASE, VAL / BASE>::val;
	};
	template<uptr BASE> struct LogPlusOne<BASE, 0> {
		static constexpr uptr val = 0;
	};
	
	template<typename T, uptr BASE, uptr VAL>
	struct _alignUpToPowerOfWrapper {
		static constexpr T val = static_cast<T>(Pow<T, 2, LogPlusOne<BASE, VAL -1>::val>::val);
	};
	template<typename T, uptr BASE> struct _alignUpToPowerOfWrapper<T, BASE, 0> {
		static constexpr T val = 1;
	};
	
	template<typename T, T BASE, T VAL> constexpr FORCEINLINE T				alignUpToPowerOf () {
		return _alignUpToPowerOfWrapper<T, BASE, VAL>::val;
	};
	
	template <uptr LEN>
	struct BitArray {
		static constexpr uptr BITS = alignUpToPowerOf<uptr, 2, clamp(LEN, sizeof(byte)*8, sizeof(uptr)*8)>();
		static constexpr uptr SIZE = BITS / 8;
		typedef typename types::GetUInteger<SIZE>::type	t;
		static constexpr t all_bits = limits::allBits<t>();
		
		t raw[align_up<uptr>(LEN, BITS) / BITS];
		
		bool get (uptr indx) {
			return ((raw[indx / BITS] >> (indx % BITS)) & static_cast<t>(1)) != 0;
		}
		void set (uptr indx, bool val) {
			t mask = (static_cast<t>(1) << (indx % BITS));
			raw[indx / BITS] = (raw[indx / BITS] & ~mask)|(static_cast<t>(val) << (indx % BITS));
		}
		void set (uptr indx) {
			raw[indx / BITS] = raw[indx / BITS] | (static_cast<t>(1) << (indx % BITS));
		}
		void clear (uptr indx) {
			raw[indx / BITS] = raw[indx / BITS] & ~(static_cast<t>(1) << (indx % BITS));
		}
		void toggle (uptr indx) {
			raw[indx / BITS] ^= static_cast<t>(1) << (indx % BITS);
		}
		
		void set_all (bool val) {
			t temp = val ? all_bits : 0;
			for (uptr i=0; i<arrlenof(raw); ++i) {
				raw[i] = temp;
			}
		}
		void set_all () {
			for (uptr i=0; i<arrlenof(raw); ++i) {
				raw[i] = all_bits;
			}
		}
		void clear_all () {
			for (uptr i=0; i<arrlenof(raw); ++i) {
				raw[i] = 0;
			}
		}
		
		bool comp_all (bool val) {
			constexpr t last_mask_shift = (LEN % BITS) == 0 ? 0 : BITS -(LEN % BITS);
			constexpr t last_mask = all_bits >> last_mask_shift;
			
			t temp = val ? all_bits : 0;
			
			bool ret = true;
			for (uptr i=0; i<(arrlenof(raw) -1); ++i) {
				if (raw[i] != temp) {
					ret = false;
				}
			}
			if ( (raw[arrlenof(raw) -1] & last_mask) != (temp & last_mask) ) {
				ret = false;
			}
			return ret;
		}
	};
	
	
	struct Filenames {
		
		struct Filename {
			u32 offs;
			u32 len;
		};
		
		dynarr<Filename>	arr;
		dynarr<char>		str_data;
		
		// lstr becomes invalid if arr or str_data ever do a realloc
		DECLM lstr get_filename (u32 indx) {
			return { &str_data[arr[indx].offs], arr[indx].len };
		}
		
		DECLM void free () {
			arr		.free();
			str_data.free();
		}
		
	};
	
namespace list_of_files_in_n {
	enum flags_e : u32 {
							FILES=				0b000001,
							FOLDERS=			0b000010,
							FILTER_FILES=		0b000100,
							RECURSIVE=			0b001000,
							FILTER_INCL_EXCL=	0b010000,
							NO_BASE_PATH=		0b100000, // Don't include base path in resulting filenames
	};
	DEFINE_ENUM_FLAG_OPS(flags_e, u32)
	
	static constexpr u32	FILTER_INCL=		0b000000;
	static constexpr u32	FILTER_EXCL=		0b010000;
	
}
	
	struct List_Of_Files_In {
		list_of_files_in_n::flags_e		flags;
		ui								filters_len;
		lstr const*						filters;
		lstr							base_path;
		
		DECLM void recurse (Filenames* out, lstr cr path) const {
			using namespace list_of_files_in_n;
			
			WIN32_FIND_DATAA info;
			
			DEFER_POP(&working_stk);
			char* list = working_stk.getTop<char>();
			
			HANDLE search_handle;
			{
				DEFER_POP(&working_stk);
				lstr search_path = str::append_term(&working_stk, base_path, path, "*");
				
				search_handle = FindFirstFileA(search_path.str, &info);
				if (search_handle == INVALID_HANDLE_VALUE) {
					warning("directory_folder_list:: FindFirstFileA() failed, for path of '%'!", search_path);
					return;
				}
			}
			
			ui	count = 0;
			
			for (;;) {
				if (	(info.cFileName[0] == '.' && info.cFileName[1] == '\0') ||
						(info.cFileName[0] == '.' && info.cFileName[1] == '.' && info.cFileName[2] == '\0') ) {
					// Ignore "." and ".." directories
				} else {
					bool is_dir = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
					char* filename = info.cFileName;
					assert(filename[0] != '\0');
					
					working_stk.push<bool>(is_dir);
					str::append_term(&working_stk, lstr::count_cstr(filename));
					++count;
				}
				{
					auto ret = FindNextFile(search_handle, &info);
					if (ret == 0) {
						auto err = GetLastError();
						assert(err == ERROR_NO_MORE_FILES);
						break;
					}
				}
			}
			
			{
				auto ret = FindClose(search_handle);
				assert(ret != 0);
			}
			
			lstr	base_path_ = base_path;
			if (flags & NO_BASE_PATH) {
				base_path_.len = 0;
			}
			
			for (ui i=0; i<count; ++i) {
				bool		is_dir = (bool)*list++;
				char const*	filename_cstr = list;
				
				lstr filename = lstr::count_cstr(filename_cstr);
				
				if (	(!is_dir && (flags & FILES)) ||
						(is_dir && (flags & FOLDERS)) ) {
					
					u32		str_offs = out->str_data.len;
					
					lstr	filepath =	str::append_term(&out->str_data, base_path_, path, filename);
					out->arr.append( {str_offs, filepath.len} );
					
				}
				
				list += filename.len +1;
				
				if (is_dir && (flags & RECURSIVE)) {
					DEFER_POP(&working_stk);
					
					assert(	filename.len > 0 &&
							filename.str[filename.len -1] != '\\' &&
							filename.str[filename.len -1] != '/' );
					
					lstr folder = str::append_term(&working_stk, path, filename, "/");
					recurse(out, folder);
				}
			}
			
		}
		
	};
	
	// base_path needs to end in a slash '/'
	DECL Filenames list_of_files_in (lstr cr base_path, list_of_files_in_n::flags_e flags,
			ui filters_len, lstr const* filters) {
		using namespace list_of_files_in_n;
		
		assert(	(flags & ~(FILES|FOLDERS|RECURSIVE|NO_BASE_PATH)) == 0 &&
				filters_len == 0 && filters == nullptr, "Not implemented");
		
		Filenames ret;
		ret.arr		.alloc(0, 256);
		ret.str_data.alloc(0, 256 * 32);
		
		auto builder = List_Of_Files_In{flags, filters_len, filters, base_path};
		builder.recurse(&ret, lstr(""));
		
		ret.arr		.fit_cap_exact();
		ret.str_data.fit_cap_exact();
		
		return ret;
	}
	
	DECL Filenames list_of_folders_in (lstr cr base_path) {
		using namespace list_of_files_in_n;
		return list_of_files_in(base_path, FOLDERS|NO_BASE_PATH, 0, nullptr);
	}
	DECL Filenames recursive_list_of_files_in (lstr cr base_path) {
		using namespace list_of_files_in_n;
		return list_of_files_in(base_path, FILES|RECURSIVE|NO_BASE_PATH, 0, nullptr);
	}
	