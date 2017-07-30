
namespace debug {
    
#if DEBUG
    
    DECL void _assert_failed (cstr cond_str, cstr file, u32 line);
    
    template <typename... MSG_Ts>
    DECL FORCEINLINE void _assert_failed (cstr cond_str, cstr file, u32 line, cstr msg_format, MSG_Ts... msg_args);
    
    #define DBG_SRC_POS_INFO __FILE__, __LINE__
    
    #define assert(cond, ...) if (!(cond)) debug::_assert_failed(STRINGIFY(cond), DBG_SRC_POS_INFO, __VA_ARGS__)
    
#else
    
    #define DBG_SRC_POS_INFO nullptr, 0
    
    #define assert(cond, ...) ((void)(cond))
    
#endif
    
    template<typename... Ts>
    DECL FORCEINLINE void warning (char const* format, Ts... args);
    
}
using debug::warning;
