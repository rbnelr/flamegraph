    
namespace debug {
    
#if DEBUG
    
    DECL void _assert_failed (cstr cond_str, cstr file, u32 line) {
        print_err("Assertion failed! At %:%.\n  '%'\n", file,line, cond_str);
        
        DBGBREAK;
        UNREACHABLE;
    }
    
    DECLV void _2_assert_failed (cstr cond_str, cstr file, u32 line, cstr msg_format,
            print_n::print_type_e const* msg_types, ...) {
        DEFER_POP(&working_stk);
        
        va_list vl;
        va_start(vl, msg_types);
        
        lstr msg;
        vl = print_n::_print_base_ret_vl(&print_working_stk, msg_format, msg_types, vl, &msg);
        
        va_end(vl);
        
        
        print_err("Assertion failed! At %:%.\n  '%'\n  '%'\n", file,line, cond_str, msg);
        
        DBGBREAK;
        UNREACHABLE;
    }
    
    struct _Wrapper {
        cstr cond_str;
        cstr file;
        u32 line;
        
        template <typename... Ts>
        FORCEINLINE void operator() (print_n::Base_Printer* this_, char const* format, print_n::print_type_e const* type_arr, Ts... args) {
            _2_assert_failed(cond_str, file, line, format, type_arr, args...);
        }
    };
    
    template <typename... MSG_ARGS>
    DECL void _assert_failed (cstr cond_str, cstr file, u32 line, cstr msg_format, MSG_ARGS... msg_args) {
        _Wrapper w{cond_str, file, line};
        print_n::_print_wrapper(w, nullptr, msg_format, msg_args...);
    }
    
#endif
    
    // for dynamicly disabling breaking while in debugger
    volatile bool _break_on_warning = true;
    
    DECLV void _warning (cstr format, print_n::print_type_e const* types, ...) {
        DEFER_POP(&working_stk);
        
        va_list vl;
        va_start(vl, types);
        
        lstr msg;
        vl = print_n::_print_base_ret_vl(&print_working_stk, format, types, vl, &msg);
        
        va_end(vl);
        
        
        print_err("[WARNING]  %\n", msg);
        
        if (_break_on_warning) {
            DBGBREAK_IF_DEBUGGER_PRESENT;
        }
    }
    
    struct _Warning_Wrapper {
        template <typename... Ts>
        FORCEINLINE void operator() (print_n::Base_Printer* this_, char const* format, print_n::print_type_e const* type_arr, Ts... args) {
            _warning(format, type_arr, args...);
        }
    };
    
    template<typename... Ts>
    DECL FORCEINLINE void warning (char const* format, Ts... args) {
        _Warning_Wrapper w{};
        print_n::_print_wrapper(w, nullptr, format, args...);
    }
    
}
