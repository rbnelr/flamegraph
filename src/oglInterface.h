
#include "gl/gl.h"

#include "gl/glext.h" // Have to include glext.h before glcorearb.h (or only glext.h) ? else some extension stuff does not get included because of GL_VERSION_?_? 'include' guards
//#include "glcorearb_no_windows_h_bloat.h"
#include "wglext_h_helper.h"

//#include "gl/glcorearb.h"

STATIC_ASSERT(sizeof(GLuint) ==     sizeof(u32));   // gl.h:        typedef unsigned int GLuint;
STATIC_ASSERT(sizeof(GLint) ==      sizeof(s32));   // gl.h:        typedef int GLint;
STATIC_ASSERT(sizeof(GLsizei) ==    sizeof(s32));   // gl.h:        typedef int GLsizei;
STATIC_ASSERT(sizeof(GLchar) ==     sizeof(char));  // glcorearb.h: typedef char GLchar;
STATIC_ASSERT(sizeof(GLfloat) ==    sizeof(f32));   // glcorearb.h: typedef char GLchar;

DECLA void APIENTRY gl_debug_proc (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar* message, const GLvoid* userParam) {
    
    if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) {
        return;
    }
    
    switch (id) {
        //case 131154: // Pixel transfer sync with rendering warning
        case 131185: // Buffer detailed info (where the memory lives which is supposed to depend on the usage hint)
        
        //case 1282: // Wierd error on notebook when trying to do texture streaming
        case 131222: // warning with unused shadow samplers ? (Program undefined behavior warning: Sampler object 0 is bound to non-depth texture 0, yet it is used with a program that uses a shadow sampler . This is undefined behavior.), This might just be unused shadow samplers, which should not be a problem
        case 131218: // performance warning, because of shader recompiling based on some 'key'
            return; 
    }
    
    char const* src_str = "<unknown>";
    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB:               src_str = "GL_DEBUG_SOURCE_API_ARB";                break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     src_str = "GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB";      break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   src_str = "GL_DEBUG_SOURCE_SHADER_COMPILER_ARB";    break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:       src_str = "GL_DEBUG_SOURCE_THIRD_PARTY_ARB";        break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:       src_str = "GL_DEBUG_SOURCE_APPLICATION_ARB";        break;
        case GL_DEBUG_SOURCE_OTHER_ARB:             src_str = "GL_DEBUG_SOURCE_OTHER_ARB";              break;
    }
    
    char const* type_str = "<unknown>";
    switch (source) {
        case GL_DEBUG_TYPE_ERROR_ARB:               type_str = "GL_DEBUG_TYPE_ERROR_ARB";               break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  type_str = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB";  break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:         type_str = "GL_DEBUG_TYPE_PORTABILITY_ARB";         break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:         type_str = "GL_DEBUG_TYPE_PERFORMANCE_ARB";         break;
        case GL_DEBUG_TYPE_OTHER_ARB:               type_str = "GL_DEBUG_TYPE_OTHER_ARB";               break;
    }
    
    char const* severity_str = "<unknown>";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:            severity_str = "GL_DEBUG_SEVERITY_HIGH_ARB";        break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:          severity_str = "GL_DEBUG_SEVERITY_MEDIUM_ARB";      break;
        case GL_DEBUG_SEVERITY_LOW_ARB:             severity_str = "GL_DEBUG_SEVERITY_LOW_ARB";         break;
    }
    
    warning("gl_dbg_msg: severity: % src: % type: % id: %  %\n",
            severity_str, src_str, type_str, id, message);
    
    //DBGBREAK;
}

#if DBG_VM_DEBUG
DECL void unavalibleOGLFunc () {
    //assert(false);
}
#else
DECL void unavalibleOGLFunc () {
    assert(false, "Null OpenGL function called!");
}
#endif

DECLD constexpr uptr FUNC_COUNT =
    #include "oglInterface_funcCount.h"
;

#include "oglInterface_funcNames.h"
DECLD constexpr char const* FUNC_NAMES[FUNC_COUNT] = {
    #define X(name, fp_t) STRINGIFY(name),
    OGL_FUNC_NAMES
    #undef X
};
static_assert(arrlenof(FUNC_NAMES) == FUNC_COUNT, "");

struct OGL_Iface {
    
    union {
        struct {
            #define X(name, fp_t) fp_t name;
            OGL_FUNC_NAMES
            #undef X
        };
        void const* ptrs[FUNC_COUNT];
    };
    #undef OGL_FUNC_NAMES
    
    #if MODULE == MODULE_EXE
    void load_all () {
        for (uptr i=0; i<FUNC_COUNT; ++i) {
            auto str = FUNC_NAMES[i];
            
            auto ret = wglGetProcAddress(str);
            
            if (ret == nullptr) {
                warning("wglGetProcAddress returned null for '%'!\n", str);
                #if 0
                assert(false);
                #endif
            }
            
            if (ret == nullptr) {
                ptrs[i] = reinterpret_cast<void const*>(unavalibleOGLFunc);
            } else {
                ptrs[i] = reinterpret_cast<void const*>(ret);
            }
        }
    }
    #endif
};
DECLD OGL_Iface gl_tbl;

DECL GLenum _getOpenGLError () {
    #if !DBG_VM_DEBUG
    GLenum err;
    ui maxCount = 16;
    while (maxCount--) {
        err = glGetError();
        if (err != GL_NO_ERROR) {
            break;
        }
    }
    return err;
    #else
    return GL_NO_ERROR;
    #endif
}
DECL err_e _catchOpenGLErrors (char const** outErrString) {
    GLenum err = _getOpenGLError();
    if (err != GL_NO_ERROR) {
        char const* errStr;
        switch (err) {
            case GL_INVALID_ENUM:                       errStr = "GL_INVALID_ENUM";                 break;
            case GL_INVALID_VALUE:                      errStr = "GL_INVALID_VALUE";                    break;
            case GL_INVALID_OPERATION:                  errStr = "GL_INVALID_OPERATION";                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:      errStr = "GL_INVALID_FRAMEBUFFER_OPERATION";    break;
            case GL_OUT_OF_MEMORY:                      errStr = "GL_OUT_OF_MEMORY";                    break;
            case GL_STACK_UNDERFLOW:                    errStr = "GL_STACK_UNDERFLOW";              break;
            case GL_STACK_OVERFLOW:                     errStr = "GL_STACK_OVERFLOW";               break;
            default:                                    errStr = "unknown error";                       break;
        }
        *outErrString = errStr;
        return OGL_ERROR;
    }
    return OK;
}

DECL void _catchOpenGLErrorsAssert (char const* fileStr, ui lineN) {
    char const* errStr;
    auto err = _catchOpenGLErrors(&errStr);
    if (err != OK) {
        warning("OpenGL error! % at %:%!", errStr, fileStr, lineN);
    }
}

#define catchOpenGLErrorsAssert() _catchOpenGLErrorsAssert(DBG_SRC_POS_INFO)

DECL err_e catchOpenGLErrors () {
    GLenum err = _getOpenGLError();
    if (err != GL_NO_ERROR) {
        return OGL_ERROR;
    }
    return OK;
}

#define CALL_OGL_FUNC_TBL(name, ...) gl_tbl.name(__VA_ARGS__)
//#define CALL_OGL_FUNC(name, ...) name(__VA_ARGS__)

#include "oglInterface_funcWrappers.h"
