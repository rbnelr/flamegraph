
namespace engine {
    
    /*
        Coordinate system convetions:
        X - right/east,     red, right hand is primary
        Y - forward/north,  green
        Z - up,             blue, least important coordinate, discribes height in maps etc., x & y are like coordinates in a map
    */
    
    #define PI              3.1415926535897932384626433832795f
    #define TWO_PI          6.283185307179586476925286766559f
    #define PId             3.1415926535897932384626433832795
    #define TWO_PId         6.283185307179586476925286766559
    
    #define QNAN            fp::qnan<f32>()
    #define QNANd           fp::qnan<f64>()
    
    #define DEG_TO_RADd     (TWO_PId / 360.0)
    #define DEG_TO_RAD      ((f32)DEG_TO_RADd)
    #define RAD_TO_DEGd     (360.0 / TWO_PId)
    #define RAD_TO_DEG      ((f32)RAD_TO_DEGd)
    
    // Units which always convert to the internal representation (which is radians)
    DECL constexpr  f32 deg (f32 deg) {
        return deg * DEG_TO_RAD;
    }
    DECL constexpr  f64 deg (f64 deg) {
        return deg * DEG_TO_RADd;
    }
    DECL constexpr  f32 rad (f32 rad) {
        return rad;
    }
    DECL constexpr  f64 rad (f64 rad) {
        return rad;
    }
    
    template <typename T>   DECL constexpr tv2<T> deg (tv2<T> vp deg) {     return deg * tv2<T>(T(DEG_TO_RADd)); }
    template <typename T>   DECL constexpr tv3<T> deg (tv3<T> vp deg) {     return deg * tv3<T>(T(DEG_TO_RADd)); }
    template <typename T>   DECL constexpr tv4<T> deg (tv4<T> vp deg) {     return deg * tv4<T>(T(DEG_TO_RADd)); }
    template <typename T>   DECL constexpr tv2<T> rad (tv2<T> vp rad) {     return rad; }
    template <typename T>   DECL constexpr tv3<T> rad (tv3<T> vp rad) {     return rad; }
    template <typename T>   DECL constexpr tv4<T> rad (tv4<T> vp rad) {     return rad; }
    
    // Conversions
    DECL constexpr  f64 to_deg (f64 rad) {
        return rad * RAD_TO_DEGd;
    }
    DECL constexpr  f32 to_deg (f32 rad) {
        return rad * RAD_TO_DEG;
    }
    DECL constexpr  f32 to_rad (f32 deg) {
        return deg * DEG_TO_RAD;
    }
    DECL constexpr  f64 to_rad (f64 deg) {
        return deg * DEG_TO_RADd;
    }
    
    template <typename T>   DECL constexpr tv2<T> to_deg (tv2<T> vp rad) {  return rad * tv2<T>(T(RAD_TO_DEGd)); }
    template <typename T>   DECL constexpr tv3<T> to_deg (tv3<T> vp rad) {  return rad * tv3<T>(T(RAD_TO_DEGd)); }
    template <typename T>   DECL constexpr tv4<T> to_deg (tv4<T> vp rad) {  return rad * tv4<T>(T(RAD_TO_DEGd)); }
    template <typename T>   DECL constexpr tv2<T> to_rad (tv2<T> vp deg) {  return deg * tv2<T>(T(DEG_TO_RADd)); }
    template <typename T>   DECL constexpr tv3<T> to_rad (tv3<T> vp deg) {  return deg * tv3<T>(T(DEG_TO_RADd)); }
    template <typename T>   DECL constexpr tv4<T> to_rad (tv4<T> vp deg) {  return deg * tv4<T>(T(DEG_TO_RADd)); }
    
    DECL f32 mod_pn_180deg (f32 ang) { // mod float into -/+ 180deg range
        return fp::proper_mod(ang +deg(180.0f), deg(360.0f)) -deg(180.0f);
    }
    
    template <typename T>   DECL T to_linear (T srgb) {
        if (srgb <= T(0.0404482362771082)) {
            return srgb * T(1/12.92);
        } else {
            return fp::pow( (srgb +T(0.055)) * T(1/1.055), T(2.4) );
        }
    }
    template <typename T>   DECL T to_srgb (T linear) {
        if (linear <= T(0.00313066844250063)) {
            return linear * T(12.92);
        } else {
            return ( T(1.055) * fp::pow(linear, T(1/2.4)) ) -T(0.055);
        }
    }
    
    template <typename T> DECL constexpr tv3<T> to_linear (tv3<T> srgb) {
        return tv3<T>( to_linear(srgb.x), to_linear(srgb.y), to_linear(srgb.z) );
    }
    template <typename T> DECL constexpr tv3<T> to_srgb (tv3<T> linear) {
        return tv3<T>( to_srgb(linear.x), to_srgb(linear.y), to_srgb(linear.z) );
    }
    
    template <typename T>   DECL constexpr tv3<T> col (T x, T y, T z) {     return tv3<T>(x,y,z); }
    template <typename T>   DECL constexpr tv3<T> srgb (T x, T y, T z) {    return to_linear(tv3<T>(x,y,z) * tv3<T>(T(1.0/255.0))); }
    template <typename T>   DECL constexpr tv3<T> col (T all) {             return col(all,all,all); }
    template <typename T>   DECL constexpr tv3<T> srgb (T all) {            return srgb(all,all,all); }
    
    DECL m3 scale_3 (v3 vp scale) {
        m3 ret = m3::zero();
        ret.row(0, 0, scale.x);
        ret.row(1, 1, scale.y);
        ret.row(2, 2, scale.z);
        return ret;
    }
    DECL m3 rotate_X3 (f32 angle) {
        auto t = fp::sincos(angle);
        m3 ret = m3::row(   1,      0,      0,
                            0,      +t.c,   -t.s,
                            0,      +t.s,   +t.c );
        return ret;
    }
    DECL m3 rotate_Y3 (f32 angle) {
        auto t = fp::sincos(angle);
        m3 ret = m3::row(   +t.c,   0,      +t.s,
                            0,      1,      0,
                            -t.s,   0,      +t.c );
        return ret;
    }
    DECL m3 rotate_Z3 (f32 angle) {
        auto t = fp::sincos(angle);
        m3 ret = m3::row(   +t.c,   -t.s,   0,
                            +t.s,   +t.c,   0,
                            0,      0,      1 );
        return ret;
    }
    
    DECL m4 translate_4 (v3 vp v) {
        m4 ret = m4::ident();
        ret.column(3, v4(v, 1));
        return ret;
    }
    DECL m4 scale_4 (v3 vp scale) {
        m4 ret = m4::zero();
        ret.row(0, 0, scale.x);
        ret.row(1, 1, scale.y);
        ret.row(2, 2, scale.z);
        ret.row(3, 3, 1);
        return ret;
    }
    DECL m4 rotate_X4 (f32 angle) {
        auto t = fp::sincos(angle);
        m4 ret = m4::row(   1,      0,      0,      0,
                            0,      +t.c,   -t.s,   0,
                            0,      +t.s,   +t.c,   0,
                            0,      0,      0,      1 );
        return ret;
    }
    DECL m4 rotate_Y4 (f32 angle) {
        auto t = fp::sincos(angle);
        m4 ret = m4::row(   +t.c,   0,      +t.s,   0,
                            0,      1,      0,      0,
                            -t.s,   0,      +t.c,   0,
                            0,      0,      0,      1 );
        return ret;
    }
    DECL m4 rotate_Z4 (f32 angle) {
        auto t = fp::sincos(angle);
        m4 ret = m4::row(   +t.c,   -t.s,   0,      0,
                            +t.s,   +t.c,   0,      0,
                            0,      0,      1,      0,
                            0,      0,      0,      1 );
        return ret;
    }
    
    DECL hm translate_h (v3 vp v) {
        #if 0
        return hm::row( 1, 0, 0, v.x,
                        0, 1, 0, v.y,
                        0, 0, 1, v.z);
        #else // Better asm
        hm ret;
        ret.arr[0].x = 1;
        ret.arr[0].y = 0;
        ret.arr[0].z = 0;
        ret.arr[1].x = 0;
        ret.arr[1].y = 1;
        ret.arr[1].z = 0;
        ret.arr[2].x = 0;
        ret.arr[2].y = 0;
        ret.arr[2].z = 1;
        ret.arr[3].x = v.x;
        ret.arr[3].y = v.y;
        ret.arr[3].z = v.z;
        return ret;
        #endif
    }
    DECL hm scale_h (v3 vp scale) {
        hm ret = hm::ident();
        ret.row(0, 0, scale.x);
        ret.row(1, 1, scale.y);
        ret.row(2, 2, scale.z);
        return ret;
    }
    DECL hm rotate_x (f32 angle) {
        auto t = fp::sincos(angle);
        return hm::row( 1,      0,      0,      0,
                        0,      +t.c,   -t.s,   0,
                        0,      +t.s,   +t.c,   0 );
    }
    DECL hm rotate_y (f32 angle) {
        auto t = fp::sincos(angle);
        return hm::row( +t.c,   0,      +t.s,   0,
                        0,      1,      0,      0,
                        -t.s,   0,      +t.c,   0 );
    }
    DECL hm rotate_z (f32 angle) {
        auto t = fp::sincos(angle);
        return hm::row( +t.c,   -t.s,   0,      0,
                        +t.s,   +t.c,   0,      0,
                        0,      0,      1,      0 );
    }
    
    DECL quat rotate_quat_axis (v3 vp axis, f32 angle) {
        auto t = fp::sincos(angle / 2.0f);
        return quat( axis * v3(t.s), t.c );
    }
    DECL quat rotate_XQuat (f32 angle) {
        auto t = fp::sincos(angle / 2.0f);
        return quat( v3(t.s, 0, 0), t.c );
    }
    DECL quat rotate_YQuat (f32 angle) {
        auto t = fp::sincos(angle / 2.0f);
        return quat( v3(0, t.s, 0), t.c );
    }
    DECL quat rotate_ZQuat (f32 angle) {
        auto t = fp::sincos(angle / 2.0f);
        return quat( v3(0, 0, t.s), t.c );
    }
    
    constexpr f32 _90deg_arr[5] = { 0, +1, 0, -1, 0 };
    DECL fp::SC _90deg_getSC (si multiple) {
        fp::SC ret;
        
        multiple = multiple & 0b11;
        
        ret.s = _90deg_arr[multiple];
        ret.c = _90deg_arr[multiple +1];
        return ret;
    };
    DECL m3 rotate_X_90deg (si multiple) {
        auto g = _90deg_getSC(multiple);
        m3 ret = m3::row(   1,      0,      0,
                            0,      +g.c,   -g.s,
                            0,      +g.s,   +g.c );
        return ret;
    };
    DECL m3 rotate_Y_90deg (si multiple) {
        auto g = _90deg_getSC(multiple);
        m3 ret = m3::row(   +g.c,   0,      +g.s,
                            0,      1,      0,
                            -g.s,   0,      +g.c );
        return ret;
    };
    DECL m3 rotate_Z_90deg (si multiple) {
        auto g = _90deg_getSC(multiple);
        m3 ret = m3::row(   +g.c,   -g.s,   0,
                            +g.s,   +g.c,   0,
                            0,      0,      1 );
        return ret;
    };
    
//// UBO
#include "ogl_ubo_interface.h"
    
#pragma pack (push, 1)

struct std140_Bar {
    std140::float_      pos_t;
    std140::float_      len_t;
    std140::sint_       depth;
    std140::uint_       type_flags;
    std140::sint_       name_offs;
    std140::sint_       name_len;
    tv3<GLubyte>        col;
    GLubyte             pad_00[1];
};
ASSERT_STD140_ALIGNMENT(std140_Bar, pos_t);
ASSERT_STD140_ALIGNMENT(std140_Bar, len_t);
ASSERT_STD140_ALIGNMENT(std140_Bar, depth);
ASSERT_STD140_ALIGNMENT(std140_Bar, type_flags);
ASSERT_STD140_ALIGNMENT(std140_Bar, name_offs);
ASSERT_STD140_ALIGNMENT(std140_Bar, name_len);
//ASSERT_STD140_ALIGNMENT(std140_Bar, col);
    
struct std140_Global {
    std140::vec2        fscreen_res;
    std140::float_      view_pos_sec_to_pix;
    std140::float_      view_scale_sec_to_pix;
    std140::float_      units_counter_to_sec;
    std140::sint_       bar_base_offs;
    std140::float_      vert_line_pos;
    std140::float_      pad_00[1];
    
    std140::ivec2       pos;
    std140::sint_       str_len;
    std140::float_      str_clip_x;
};
ASSERT_STD140_ALIGNMENT(std140_Global, fscreen_res);
ASSERT_STD140_ALIGNMENT(std140_Global, view_pos_sec_to_pix);
ASSERT_STD140_ALIGNMENT(std140_Global, view_scale_sec_to_pix);
ASSERT_STD140_ALIGNMENT(std140_Global, units_counter_to_sec);
ASSERT_STD140_ALIGNMENT(std140_Global, bar_base_offs);
ASSERT_STD140_ALIGNMENT(std140_Global, vert_line_pos);
ASSERT_STD140_ALIGNMENT(std140_Global, pos);
ASSERT_STD140_ALIGNMENT(std140_Global, str_len);
ASSERT_STD140_ALIGNMENT(std140_Global, str_clip_x);
    
struct std140_View {
    std140::float_      view_pos_sec_to_pix;
    std140::float_      view_scale_sec_to_pix;
};
    
struct std140_String {
    std140::ivec2       pos;
    std140::sint_       str_len;
    std140::float_      str_clip_x;
};
    
#pragma pack (pop)
    
//// VBO
    constexpr GLuint    GLOBAL_UNIFORM_BLOCK_BINDING =      0;
    
    enum vbo_indx_e : u32 {
        VBO_UBO=0,
        VBO_BUF_TEX_BAR_NAME_STR_TBL,
        VBO_BUF_TEX_FREE_TEXT,
        VBO_COUNT
    };
    
//// VAO
    enum : u32 {
        VAO_EMTPY=0,
        VAO_COUNT
    };
    
//// Textures
    enum : u32 {
        SAMPL_TEXT=0,
        SAMPL_COUNT
    };
    enum : u32 {
        TEX_TEXT_DATA=0,
        TEX_GLYPHS,
        TEX_COUNT
    };
    
////
    constexpr GLint     SHAD_BAR_HEIGHT =       36;
    constexpr GLint     SHAD_BAR_OFFS =         38;
    constexpr GLint     SHAD_BAR_BASE_OFFS =    2 * SHAD_BAR_OFFS;
    
    constexpr auto      GLYPH_DIM =             tv2<GLint>(8, 15);
    constexpr u32       SHAD_MAX_STR_LEN =      256; // 256 * 8pix = 2k pix long strings
    
//// Shaders
    constexpr GLint     TEXT_DATA_TEX_UNIT =                    0;
    constexpr GLint     GLYPHS_TEX_UNIT =                       1;
    
    // Shaders
    enum shad_id_e: u32 {
        SHAD_VERT_BARS=0,
        SHAD_FRAG_BARS,
        SHAD_VERT_FULLSCREEN_QUAD,
        //SHAD_FRAG_TEST_SHOW_ALPHA,
        SHAD_FRAG_ACCUM_CLEAR,
        SHAD_VERT_VERT_LINE,
        SHAD_FRAG_VERT_LINE,
        SHAD_VERT_BAR_TEXT,
        SHAD_FRAG_BAR_TEXT,
        SHAD_VERT_BAR_DURATION_TEXT,
        SHAD_FRAG_BAR_DURATION_TEXT,
        SHAD_VERT_FREE_TEXT,
        SHAD_FRAG_FREE_TEXT,
        SHAD_COUNT,
        SHAD_NULL=(u32)-1,
    };
    DEFINE_ENUM_ITER_OPS(shad_id_e, u32)
    
    constexpr GLchar const* GLSL_VERSION_SRC =
R"_SHAD(
    #version 150 // version 3.2
    #extension GL_ARB_explicit_attrib_location : require
)_SHAD";
    
    constexpr GLchar const* SHAD_GLOBAL_UBO =
R"_SHAD(
    layout(std140)          uniform Global {
        vec2            fscreen_res;
        float           view_pos_sec_to_pix;
        float           view_scale_sec_to_pix; // pixels per time-unit
        float           units_counter_to_sec;
        int             bar_base_offs;
        float           vert_line_pos;
        // 
        ivec2           pos; // pixels
        int             str_len;
        float           str_clip_x;
    } g;
    
    const int   BAR_HEIGHT =        36;
    const int   LOW_BAR_HEIGHT =    18;
    const int   BAR_OFFS =          38;
    
    const uint  BT_WAIT =           uint(0x1);
    
    const ivec2 GLYPH_DIM =         ivec2(8, 15);
    const ivec2 GLYPH_TEX_COUNTS =  ivec2(16, 8);
    const ivec2 GLYPH_TEX_DIM =     GLYPH_DIM * GLYPH_TEX_COUNTS;
    
    const vec3  TEXT_FG_COL =       vec3(224, 226, 228) / vec3(255);
)_SHAD";
    
    struct Shader {
        GLenum          type;
        GLchar const*   src;
    };
    
    constexpr Shader SHADERS[] = {
    
/* SHAD_VERT_BARS */ { GL_VERTEX_SHADER, R"_SHAD(
    
    layout(location=0)  in  float           attr_pos_t;
    layout(location=1)  in  float           attr_len_t;
    layout(location=2)  in  int             attr_depth;
    layout(location=3)  in  uint            attr_type_flags;
    //layout(location=4)    in  int             attr_name_offs;
    //layout(location=5)    in  int             attr_name_len;
    layout(location=6)  in  vec3            attr_col;
    
    flat                out vec3            pass_col;
    smooth              out float           interp_alpha;
    
    flat                out float           pass_fbar_height;
    flat                out float           pass_w_px;
    smooth              out vec2            interp_uv_px;
    
    void main () {
        
        float pos_x_counter = attr_pos_t * g.units_counter_to_sec;
        float pos_x_px = (pos_x_counter -g.view_pos_sec_to_pix) * g.view_scale_sec_to_pix;
        
        float len_x_px = attr_len_t * g.units_counter_to_sec * g.view_scale_sec_to_pix;
        
        float bar_pos_x_px = 0.0;
        float alpha = 0.0;
        
        if (len_x_px >= 1.0) {
            switch (gl_VertexID >> 1) {
                case 0:
                    bar_pos_x_px = -0.5;
                    alpha = 0.0;
                    break;
                case 1:
                    bar_pos_x_px = +0.5;
                    alpha = 1.0;
                    break;
                case 2:
                    bar_pos_x_px = len_x_px -0.5;
                    alpha = 1.0;
                    break;
                case 3:
                    bar_pos_x_px = len_x_px +0.5;
                    alpha = 0.0;
                    break;
            }
        } else {
            
            float flat_part_width = 1.0 -len_x_px;
            
            switch (gl_VertexID >> 1) {
                case 0:
                    bar_pos_x_px = (len_x_px / 2.0) -(flat_part_width / 2.0) -len_x_px;
                    alpha = 0.0;
                    break;
                case 1:
                    bar_pos_x_px = (len_x_px / 2.0) -(flat_part_width / 2.0);
                    alpha = len_x_px;
                    break;
                case 2:
                    bar_pos_x_px = (len_x_px / 2.0) +(flat_part_width / 2.0);
                    alpha = len_x_px;
                    break;
                case 3:
                    bar_pos_x_px = (len_x_px / 2.0) +(flat_part_width / 2.0) +len_x_px;
                    alpha = 0.0;
                    break;
            }
        }
        
        int y = (gl_VertexID & 1) ^ 1;
        
        int bar_height = BAR_HEIGHT;
        if ((attr_type_flags & BT_WAIT) != 0u) {
            bar_height = LOW_BAR_HEIGHT;
        }
        
        pass_fbar_height = float(bar_height);
        
        bar_height *= y;
        
        pass_w_px = len_x_px;
        interp_uv_px = vec2(bar_pos_x_px, float(bar_height));
        
        pos_x_px += bar_pos_x_px;
        
        float pos_y_px = float( (attr_depth * BAR_OFFS) +bar_height +g.bar_base_offs );
        
        gl_Position = vec4( ((vec2(pos_x_px, pos_y_px) / g.fscreen_res) * vec2(2)) -vec2(1), 0.0, 1.0 );
        
        pass_col = attr_col;
        interp_alpha = alpha;
        
    }
)_SHAD" },
    
/* SHAD_FRAG_BARS */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    flat                in  vec3            pass_col;
    smooth              in  float           interp_alpha;
    
    flat                in  float           pass_fbar_height;
    flat                in  float           pass_w_px;
    smooth              in  vec2            interp_uv_px;
    
    out vec4            frag_col;
    
    bvec2 lte (vec2 l, vec2 r) {
        return not(greaterThan(l, r));
    }
    
    void main () {
        //frag_col = vec4(pass_col, interp_alpha);
        
        if (    (interp_uv_px.x < 1.2 || interp_uv_px.x > (pass_w_px -1.2)) &&
                (interp_uv_px.y < 1.2 || interp_uv_px.y > (pass_fbar_height -1.2)) ) {
            discard;
        }
        frag_col = vec4(pass_col, interp_alpha);
    }
)_SHAD" },
    
/* SHAD_VERT_FULLSCREEN_QUAD */ { GL_VERTEX_SHADER, R"_SHAD(
    
    void main () {
        
        vec2 pos = vec2(gl_VertexID & 1, (gl_VertexID >> 1) & 1);
        pos *= vec2(2);
        pos -= vec2(1);
        
        gl_Position = vec4(pos, 0.0, 1.0 );
    }
)_SHAD" },
    
#if 0
/* SHAD_FRAG_TEST_SHOW_ALPHA */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    out vec4            frag_col;
    
    void main () {
        
        frag_col = vec4(1.0, 1.0, 1.0, 1.0);
    }
)_SHAD" },
#endif
    
/* SHAD_FRAG_ACCUM_CLEAR */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    uniform vec3        background_col;
    
    out vec4            frag_col;
    
    void main () {
        
        frag_col = vec4(background_col, 1.0);
    }
)_SHAD" },
    
/* SHAD_VERT_VERT_LINE */ { GL_VERTEX_SHADER, R"_SHAD(
    
    smooth              out float           interp_alpha;
    
    void main () {
        
        float pos_x_counter = g.vert_line_pos;
        float pos_x_px = (pos_x_counter -g.view_pos_sec_to_pix) * g.view_scale_sec_to_pix;
        
        float width_x_px = 1.25;
        
        float line_pos_x_px = 0.0;
        float alpha = 0.0;
        
        float width_half_x_px = width_x_px * 0.5;
        
        switch (gl_VertexID >> 1) {
            case 0:
                line_pos_x_px = -width_half_x_px -0.5;
                alpha = 0.0;
                break;
            case 1:
                line_pos_x_px = -width_half_x_px +0.5;
                alpha = 1.0;
                break;
            case 2:
                line_pos_x_px = +width_half_x_px -0.5;
                alpha = 1.0;
                break;
            case 3:
                line_pos_x_px = +width_half_x_px +0.5;
                alpha = 0.0;
                break;
        }
        
        pos_x_px += line_pos_x_px;
        
        int y = (gl_VertexID & 1) ^ 1;
        
        gl_Position = vec4( (vec2(pos_x_px / g.fscreen_res.x, float(y)) * vec2(2)) -vec2(1), 0.0, 1.0 );
        
        interp_alpha = alpha;
    }
)_SHAD" },

/* SHAD_FRAG_VERT_LINE */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    smooth              in  float           interp_alpha;
    
    out vec4            frag_col;
    
    void main () {
        frag_col = vec4(vec3(1.0), interp_alpha * 0.75);
    }
)_SHAD" },
    
/* SHAD_VERT_BAR_TEXT */ { GL_VERTEX_SHADER, R"_SHAD(
    
    layout(location=0)  in  float           attr_pos_t;
    layout(location=1)  in  float           attr_len_t;
    layout(location=2)  in  int             attr_depth;
    //layout(location=3)    in  uint            attr_type_flags;
    layout(location=4)  in  int             attr_name_offs;
    layout(location=5)  in  int             attr_name_len;
    //layout(location=6)    in  vec3            attr_col;
    
    flat                out int             pass_name_offs;
    smooth              out vec2            interp_uv;
    
    const int   TEXT_X_BORD =       2;
    const int   NAME_LOWER_BORD =   3;
    
    void main () {
        float fglyph_x_dim = float(GLYPH_DIM.x);
        
        float pos_x_counter = attr_pos_t * g.units_counter_to_sec;
        float pos_x_px = (pos_x_counter -g.view_pos_sec_to_pix) * g.view_scale_sec_to_pix;
        
        float len_x_px = attr_len_t * g.units_counter_to_sec * g.view_scale_sec_to_pix;
        
        vec2 pos_quad = vec2(gl_VertexID & 1, (gl_VertexID >> 1) & 1);
        
        float end_x_px = pos_x_px +len_x_px;
        
        pos_x_px += float(TEXT_X_BORD);
        end_x_px -= float(TEXT_X_BORD);
        
        pos_x_px = max(pos_x_px, 0.0);
        
        len_x_px = end_x_px -pos_x_px;
        len_x_px = clamp(len_x_px, 0.0, float(attr_name_len) * fglyph_x_dim) * pos_quad.x;
        
        pos_x_px += len_x_px;
        
        interp_uv = vec2(len_x_px / fglyph_x_dim, pos_quad.y);
        
        float pos_y_px = float( (attr_depth * BAR_OFFS) +g.bar_base_offs
                +NAME_LOWER_BORD +(pos_quad.y * GLYPH_DIM.y) );
        
        gl_Position = vec4( ((vec2(pos_x_px, pos_y_px) / g.fscreen_res) * vec2(2)) -vec2(1), 0.0, 1.0 );
        
        pass_name_offs = attr_name_offs;
    }
)_SHAD" },
    
/* SHAD_FRAG_BAR_TEXT */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    flat                in  int             pass_name_offs;
    smooth              in  vec2            interp_uv;
    
                        uniform isamplerBuffer  text_data_tex;
                        uniform sampler2D       glyphs_tex;
                        
    out vec4            frag_col;
    
    void main () {
        
        int char_indx = int(interp_uv.x);
        
        int glyph_code = texelFetch(text_data_tex, pass_name_offs +char_indx).r;
        
        vec2 glyph_uv = vec2(0);
        if (glyph_code < 128) {
            glyph_uv.y = float(glyph_code / 16);
            glyph_uv.x = float(glyph_code % 16);
        }
        glyph_uv.x += fract(interp_uv.x);
        glyph_uv.y += interp_uv.y;
        
        glyph_uv /= vec2(GLYPH_TEX_COUNTS);
        
        frag_col = vec4(TEXT_FG_COL, texture(glyphs_tex, glyph_uv).r);
        //frag_col = vec4(TEXT_FG_COL * texture(glyphs_tex, glyph_uv).r, max(texture(glyphs_tex, glyph_uv).r, 0.25));
    }
)_SHAD" },
    
/* SHAD_VERT_BAR_DURATION_TEXT */ { GL_VERTEX_SHADER, R"_SHAD(
    
    layout(location=0)  in  float           attr_pos_t;
    layout(location=1)  in  float           attr_len_t;
    layout(location=2)  in  int             attr_depth;
    layout(location=3)  in  uint            attr_type_flags;
    //layout(location=4)    in  int             attr_name_offs;
    //layout(location=5)    in  int             attr_name_len;
    //layout(location=6)    in  vec3            attr_col;
    
    flat                out uvec3           pass_chars;
    smooth              out vec2            interp_uv;
    
    const int   TEXT_X_BORD =       2;
    const int   NAME_LOWER_BORD =   3;
    
    void main () {
        
        if ((attr_type_flags & BT_WAIT) != 0u) {
            gl_Position = vec4(0);
        } else {
            
            float fglyph_x_dim = float(GLYPH_DIM.x);
            
            float pos_x_counter = attr_pos_t * g.units_counter_to_sec;
            float pos_x_px = (pos_x_counter -g.view_pos_sec_to_pix) * g.view_scale_sec_to_pix;
            
            uint len = uint(8 +2);
            {
                float val = attr_len_t * g.units_counter_to_sec;
                if (        val < (1.0 / 1000 / 1000) ) {
                    val *= 1000.0 * 1000 * 1000;
                    pass_chars[2] = uint(0x736e); // "ns"
                } else if ( val < (1.0 / 1000) ) {
                    val *= 1000.0 * 1000;
                    pass_chars[2] = uint(0x7375); // "us"
                } else if ( val < 1.0 ) {
                    val *= 1000.0;
                    pass_chars[2] = uint(0x736d); // "ms"
                } else if ( val < 60.0 ) {
                    // do nothing
                    pass_chars[2] = uint(0x636573); // "sec"
                    len += 1u;
                } else {
                    val *= 1.0 / 60;
                    pass_chars[2] = uint(0x6e696d); // "min"
                    len += 1u;
                }
                
                val = round(val * 1000.0) * (1.0 / 1000.0); // round to 3 digits after decimal point
                
                pass_chars[0] =     (uint(val * (1.0 / 1000.0)) %   uint(10));
                pass_chars[0] |=    (uint(val * (1.0 / 100.0)) %    uint(10)) << 8;
                pass_chars[0] |=    (uint(val * (1.0 / 10.0)) %     uint(10)) << 16;
                pass_chars[0] |=    (uint(val) %                    uint(10)) << 24;
                pass_chars[1] =      uint(0x2E); // '.'
                pass_chars[1] |=    (uint(val * 10.0) %             uint(10)) << 8;
                pass_chars[1] |=    (uint(val * 100.0) %            uint(10)) << 16;
                pass_chars[1] |=    (uint(val * 1000.0) %           uint(10)) << 24;
                
                pass_chars += uvec3(0x30303030, 0x30303000, 0);
            }
            
            float len_x_px = attr_len_t * g.units_counter_to_sec * g.view_scale_sec_to_pix;
            
            vec2 pos_quad = vec2(gl_VertexID & 1, (gl_VertexID >> 1) & 1);
            
            float end_x_px = pos_x_px +len_x_px;
            
            pos_x_px += float(TEXT_X_BORD);
            end_x_px -= float(TEXT_X_BORD);
            
            pos_x_px = max(pos_x_px, 0.0);
            
            len_x_px = end_x_px -pos_x_px;
            len_x_px = clamp(len_x_px, 0.0, float(len) * fglyph_x_dim) * pos_quad.x;
            
            pos_x_px += len_x_px;
            
            interp_uv = vec2(len_x_px / fglyph_x_dim, pos_quad.y);
            
            float pos_y_px = float( (attr_depth * BAR_OFFS) +g.bar_base_offs
                    +(BAR_HEIGHT -NAME_LOWER_BORD -GLYPH_DIM.y) +(pos_quad.y * GLYPH_DIM.y) );
            
            gl_Position = vec4( ((vec2(pos_x_px, pos_y_px) / g.fscreen_res) * vec2(2)) -vec2(1), 0.0, 1.0 );
        }
    }
)_SHAD" },
    
/* SHAD_FRAG_BAR_DURATION_TEXT */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    flat                in  uvec3           pass_chars;
    smooth              in  vec2            interp_uv;
    
                        uniform sampler2D       glyphs_tex;
                        
    out vec4            frag_col;
    
    void main () {
        
        int char_indx = int(interp_uv.x);
        
        int glyph_code = int((pass_chars[char_indx / 4] >> ((char_indx % 4) * 8)) & uint(0xff));
        
        vec2 glyph_uv = vec2(0);
        if (glyph_code < 128) {
            glyph_uv.y = float(glyph_code / 16);
            glyph_uv.x = float(glyph_code % 16);
        }
        glyph_uv.x += fract(interp_uv.x);
        glyph_uv.y += interp_uv.y;
        
        glyph_uv /= vec2(GLYPH_TEX_COUNTS);
        
        frag_col = vec4(TEXT_FG_COL, texture(glyphs_tex, glyph_uv).r);
        //frag_col = vec4(TEXT_FG_COL * texture(glyphs_tex, glyph_uv).r, max(texture(glyphs_tex, glyph_uv).r, 0.25));
    }
)_SHAD" },
    
/* SHAD_VERT_FREE_TEXT */ { GL_VERTEX_SHADER, R"_SHAD(
    
    smooth                  out vec2            pass_uv;
    
    void main () {
        vec2 fglyph_dim = vec2(GLYPH_DIM);
        
        vec2 quad_pos = vec2(gl_VertexID & 1, (gl_VertexID >> 1) & 1);
        
        float clipped_len = min(float(g.str_len), g.str_clip_x / fglyph_dim.x);
        
        pass_uv = quad_pos * vec2(clipped_len, 1.0);
        
        vec2 pos = vec2(g.pos);
        pos += pass_uv * fglyph_dim;
        
        gl_Position = vec4( ((pos / g.fscreen_res) * vec2(2)) -vec2(1), 0.0, 1.0);
    }
)_SHAD" },
    
/* SHAD_FRAG_FREE_TEXT */ { GL_FRAGMENT_SHADER, R"_SHAD(
    
    smooth                  in  vec2            pass_uv;
    
                            uniform isamplerBuffer  text_data_tex;
                            uniform sampler2D       glyphs_tex;
                            
                            out vec4            frag_col;
    
    void main () {
        
        int char_indx = int(pass_uv.x);
        
        int glyph_code = texelFetch(text_data_tex, char_indx).r;
        
        vec2 glyph_uv = vec2(0);
        if (glyph_code < 128) {
            glyph_uv.y = float(glyph_code / 16);
            glyph_uv.x = float(glyph_code % 16);
        }
        glyph_uv.x += fract(pass_uv.x);
        glyph_uv.y += pass_uv.y;
        
        glyph_uv /= vec2(GLYPH_TEX_COUNTS);
        
        //frag_col = vec4(glyph_uv, 0.0, 1.0);
        
        //frag_col = vec4(0.0, 1.0, 0.0, 1.0);
        frag_col = vec4(TEXT_FG_COL, texture(glyphs_tex, glyph_uv).r);
    }
    
)_SHAD" },
    
    };
    
    // Programs
    enum prog_id_e: u32 {
        PROG_BARS=0,
        PROG_ACCUM_CLEAR,
        //PROG_TEST_SHOW_ALPHA,
        PROG_VERT_LINE,
        PROG_BAR_TEXT,
        PROG_BAR_DURATION_TEXT,
        PROG_FREE_TEXT,
        PROG_COUNT,
    };
    DEFINE_ENUM_ITER_OPS(prog_id_e, u32)
    
    constexpr ui MAX_SHAD_PER_PROG = 2;
    
    struct Program {
        shad_id_e   shaders[MAX_SHAD_PER_PROG];
    };
    
    constexpr Program PROGRAMS[PROG_COUNT] = {
        /* PROG_BARS */ { { SHAD_VERT_BARS, SHAD_FRAG_BARS } },
        /* PROG_ACCUM_CLEAR */ { { SHAD_VERT_FULLSCREEN_QUAD, SHAD_FRAG_ACCUM_CLEAR } },
        ///* PROG_TEST_SHOW_ALPHA */ { { SHAD_VERT_FULLSCREEN_QUAD, SHAD_FRAG_TEST_SHOW_ALPHA } },
        /* PROG_VERT_LINE */ { { SHAD_VERT_VERT_LINE, SHAD_FRAG_VERT_LINE } },
        /* PROG_BAR_TEXT */ { { SHAD_VERT_BAR_TEXT, SHAD_FRAG_BAR_TEXT } },
        /* PROG_BAR_DURATION_TEXT */ { { SHAD_VERT_BAR_DURATION_TEXT, SHAD_FRAG_BAR_DURATION_TEXT } },
        /* PROG_FREE_TEXT */ { { SHAD_VERT_FREE_TEXT, SHAD_FRAG_FREE_TEXT } },
    };
    
GLuint compile_shader (shad_id_e shad_id) {
    
    auto in_shad = SHADERS[shad_id];
    
    char const* shader_type_str;
    switch(in_shad.type) {
        case GL_VERTEX_SHADER:      shader_type_str = "vertex";     break;
        case GL_GEOMETRY_SHADER:    shader_type_str = "geometry";   break;
        case GL_FRAGMENT_SHADER:    shader_type_str = "fragment";   break;
        default: assert(false);
    }
    
    auto shad = glCreateShader(in_shad.type);
    if (shad == 0) {
        print_err(
                "OpenGL error in % shader compilation (%)!\n"
                "glCreateShader returned 0.", shader_type_str, (u32)shad_id);
        return shad;
    }
    
    {
        GLchar const*   src[3] = { GLSL_VERSION_SRC, SHAD_GLOBAL_UBO, in_shad.src };
        GLint           len[3] = { (GLint)-1, (GLint)-1, (GLint)-1 };
        glShaderSource(shad, 3, src, len);
    }
    
    glCompileShader(shad);
    
    GLint status;
    glGetShaderiv(shad, GL_COMPILE_STATUS, &status);
    
    {
        DEFER_POP(&working_stk);
        
        GLsizei logLen;
        {
            GLint temp = 0;
            glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &temp);
            logLen = safe_cast_assert(GLsizei, temp);
        }
        GLchar* log_str;
        if (logLen <= 1) {
            log_str = "";
        } else {
            {
                DEFER_POP(&working_stk);
                
                GLsizei src_len;
                {
                    GLint temp;
                    glGetShaderiv(shad, GL_SHADER_SOURCE_LENGTH, &temp);
                    assert(temp > 0);
                    src_len = safe_cast_assert(GLsizei, temp);
                }
                
                auto buf = working_stk.pushArr<GLchar>(src_len); // GL_SHADER_SOURCE_LENGTH includes the null terminator
                
                GLsizei written_len;
                glGetShaderSource(shad, src_len, &written_len, buf);
                //assert(written_len <= (src_len -1), "%: % %", shad_id, written_len, src_len);
                assert(written_len <= src_len, "%: % %", shad_id, written_len, src_len); // BUG: on my AMD desktop written_len was actually the length with null terminator at some point
                
                platform::write_whole_file("shader_with_error.tmp", buf, src_len);
            }
            
            log_str = working_stk.pushArr<GLchar>(logLen); // GL_INFO_LOG_LENGTH includes the null terminator
            
            GLsizei writtenLen = 0;
            glGetShaderInfoLog(shad, logLen, &writtenLen, log_str);
            assert(writtenLen == (logLen -1));
        }
        
        if (status != GL_FALSE) {
            if (logLen > 1) {
                print_err("OpenGL % shader compilation log (%):\n>>>\n%\n<<<\n", shader_type_str, (u32)shad_id, log_str);
            }
        } else {
            print_err("OpenGL error in % shader compilation (%)!\n>>>\n%\n<<<\n", shader_type_str, (u32)shad_id, log_str);
            return OGL_ERROR;
        }
    }
    
    return shad;
}
GLuint link_program (prog_id_e prog_id, GLuint const* shaders) {
    
    //print("prog %\n", prog_id);
    
    auto prog = glCreateProgram();
    
    auto* shader_ids = PROGRAMS[prog_id].shaders;
    
    for (ui i=0; i<MAX_SHAD_PER_PROG; ++i) {
        if (shader_ids[i] == SHAD_NULL) {
            break;
        }
        //print("  attach %\n", shaders[ shader_ids[i] ]);
        glAttachShader(prog, shaders[ shader_ids[i] ]);
    }
    
    glLinkProgram(prog);
    
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    
    {
        DEFER_POP(&working_stk);
        
        GLsizei logLen;
        {
            GLint temp = 0;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &temp);
            logLen = safe_cast_assert(GLsizei, temp);
        }
        GLchar* log_str;
        if (logLen <= 1) {
            log_str = "";
        } else {
            {
                DEFER_POP(&working_stk);
                
                char const* buf = working_stk.getTop<GLchar>();
                uptr len = 0;
                
                for (ui i=0; i<MAX_SHAD_PER_PROG; ++i) {
                    if (shader_ids[i] == SHAD_NULL) {
                        break;
                    }
                    auto shad = shaders[ shader_ids[i] ];
                    
                    GLsizei src_len;
                    {
                        GLint temp;
                        glGetShaderiv(shad, GL_SHADER_SOURCE_LENGTH, &temp);
                        assert(temp > 0);
                        src_len = safe_cast_assert(GLsizei, temp);
                    }
                    
                    //len += cstr::append(shader 'file'name);
                    
                    auto buf = working_stk.pushArr<GLchar>(src_len); // GL_SHADER_SOURCE_LENGTH includes the null terminator
                    len += src_len;
                    
                    GLsizei written_len;
                    glGetShaderSource(shad, src_len, &written_len, buf);
                    
                    assert(written_len == (src_len -1));
                }
                
                platform::write_whole_file("shader_with_error.tmp", buf, len);
            }
            
            log_str = working_stk.pushArr<GLchar>(logLen); // GL_INFO_LOG_LENGTH includes the null terminator
            
            GLsizei writtenLen = 0;
            glGetProgramInfoLog(prog, logLen, &writtenLen, log_str);
            assert(writtenLen == (logLen -1));
        }
        
        if (status != GL_FALSE) {
            if (logLen > 1) {
                print_err("OpenGL shader linkage log:\n'%'\n", log_str);
            } else {
                //print_err("OpenGL shader linkage ok, no log.", nl);
            }
        } else {
            print_err("OpenGL error in shader linkage!\n'%'\n", log_str);
        }
    }
    
    for (ui i=0; i<MAX_SHAD_PER_PROG; ++i) {
        if (shader_ids[i] == SHAD_NULL) {
            break;
        }
        //print("  detach %\n", shaders[ shader_ids[i] ]);
        glDetachShader(prog, shaders[ shader_ids[i] ]);
    }
    
    {
        auto block_indx = glGetUniformBlockIndex(prog, "Global");
        if (block_indx == GL_INVALID_INDEX) {
            //print("glGetUniformBlockIndex failed for UBO structure '%' maybe not used not shader?\n",
            //      "Global");
            //assert(false);
        } else {
            glUniformBlockBinding(prog, block_indx, GLOBAL_UNIFORM_BLOCK_BINDING);
        }
    }
    
    return prog;
};
    
void setup_shaders (GLuint* programs) {
    
    GLuint shaders[SHAD_COUNT];
    
    for (auto i=(shad_id_e)0; i<SHAD_COUNT; i=inc(i)) {
        shaders[i] = compile_shader(i);
    }
    
    for (auto i=(prog_id_e)0; i<PROG_COUNT; i=inc(i)) {
        programs[i] = link_program(i, shaders);
    }
    
    {
        auto prog = programs[PROG_FREE_TEXT];
        glUseProgram(prog);
        
        auto unif = glGetUniformLocation(prog, "text_data_tex");
        assert(unif != -1);
        glUniform1i(unif, TEXT_DATA_TEX_UNIT);
        
        unif = glGetUniformLocation(prog, "glyphs_tex");
        assert(unif != -1);
        glUniform1i(unif, GLYPHS_TEX_UNIT);
    }
    {
        auto prog = programs[PROG_BAR_TEXT];
        glUseProgram(prog);
        
        auto unif = glGetUniformLocation(prog, "text_data_tex");
        assert(unif != -1);
        glUniform1i(unif, TEXT_DATA_TEX_UNIT);
        
        unif = glGetUniformLocation(prog, "glyphs_tex");
        assert(unif != -1);
        glUniform1i(unif, GLYPHS_TEX_UNIT);
    }
    {
        auto prog = programs[PROG_BAR_DURATION_TEXT];
        glUseProgram(prog);
        
        auto unif = glGetUniformLocation(prog, "glyphs_tex");
        assert(unif != -1);
        glUniform1i(unif, GLYPHS_TEX_UNIT);
    }
    
    for (auto i=(shad_id_e)0; i<SHAD_COUNT; i=inc(i)) {
        glDeleteShader(shaders[i]);
    }
}
    
namespace bmp {
    constexpr union {
        char    str[2];
        WORD    w;
    } filetype_magic_number_u = { 'B', 'M' };
    
    struct Bmp_Info {
        u32                     width;
        u32                     height;
        ui                      channels;
        u32                     pix_size;
    };
    
    bool parse_bmp_header (HANDLE handle, Bmp_Info* out) {
        
        #define require(cond) do { if (!(comd)) { return false; } } while (0)
        
        constexpr uptr BMP_HEADER_READ_SIZE =
                 sizeof(BITMAPFILEHEADER)
                +MAX( sizeof(BITMAPINFOHEADER), sizeof(BITMAPV4HEADER))
                +(256 * sizeof(RGBQUAD));
        
        DEFER_POP(&working_stk);
        
        BITMAPFILEHEADER*   header;
        uptr                actual_read_size;
        uptr                file_size;
        {
            { // 
                LARGE_INTEGER size;
                auto ret = GetFileSizeEx(handle, &size);
                assert(ret != 0);
                file_size = size.QuadPart;
            }
            
            actual_read_size = MIN(file_size, BMP_HEADER_READ_SIZE);
            auto data = working_stk.pushArr<byte>(actual_read_size);
            
            { // Read file contents onto stack
                DWORD bytesRead;
                auto ret = ReadFile(handle, data, safe_cast_assert(DWORD, actual_read_size), &bytesRead, NULL);
                assert(ret != 0);
                assert(bytesRead == actual_read_size);
            }
            
            header = reinterpret_cast<BITMAPFILEHEADER*>(data);
        }
        
        assert(sizeof(BITMAPFILEHEADER) <= actual_read_size);
        
        assert(header->bfType == filetype_magic_number_u.w);
        assert(header->bfSize == file_size);
        
        BITMAPINFOHEADER* info = reinterpret_cast<BITMAPINFOHEADER*>(header +1);
        assert((sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER)) <= actual_read_size);
        
        assert(info->biSize == sizeof(BITMAPINFOHEADER) || info->biSize == sizeof(BITMAPV4HEADER));
        
        assert((sizeof(BITMAPFILEHEADER) +info->biSize) <= actual_read_size);
        
        assert(info->biWidth > 0);
        assert(info->biHeight > 0);
        assert(info->biPlanes == 1);
        assert(info->biCompression == BI_RGB);
        
        DWORD header_size;
        DWORD payloadByteCount = header->bfSize -header->bfOffBits;
        
        DWORD dw_width = safe_cast_assert(DWORD, info->biWidth);
        DWORD dw_height = safe_cast_assert(DWORD, info->biHeight);
        
        DWORD pix_size;
        
        ui                      channels;
        DWORD                   dw_stride;
        
        switch (info->biBitCount) {
            
            case 8: {
                channels = 1;
                assert(info->biClrUsed == 256);
                
                {
                    DWORD size = 256 * sizeof(RGBQUAD);
                    
                    header_size = sizeof(BITMAPFILEHEADER) +info->biSize +size;
                    
                    assert(header_size <= actual_read_size);
                    
                    auto* bmiColors = reinterpret_cast<RGBQUAD*>(
                            reinterpret_cast<byte*>(header) +sizeof(BITMAPFILEHEADER) +info->biSize );
                    for (u32 i=0; i<256; ++i) {
                        auto col = bmiColors[i];
                        assert(
                            (col.rgbBlue == i) &&
                            (col.rgbGreen == i) &&
                            (col.rgbRed == i) );
                    }
                }
                
                dw_stride = align_up<DWORD>(dw_width * (1 * sizeof(u8)), 4);
                pix_size = dw_height * dw_stride;
            } break;
            
            case 24: {
                channels = 3;
                assert(info->biClrUsed == 0);
                
                header_size = sizeof(BITMAPFILEHEADER) +info->biSize;
                
                dw_stride = align_up<DWORD>(dw_width * (3 * sizeof(u8)), 4);
                pix_size = dw_height * dw_stride;
            } break;
            
            case 32: {
                channels = 4;
                assert(info->biClrUsed == 0);
                
                header_size = sizeof(BITMAPFILEHEADER) +info->biSize;
                
                dw_stride = dw_width * (4 * sizeof(u8));
                pix_size = dw_height * dw_stride;
            } break;
            
            default: assert(false);
        }
        
        assert(((uptr)header->bfOffBits +(uptr)pix_size) <= file_size);
        assert(header->bfOffBits >= header_size);
        
        assert(pix_size <= payloadByteCount);
        
        {
            LONG high = 0;
            auto ret = SetFilePointer(handle, header->bfOffBits, &high, FILE_BEGIN);
            if (ret == INVALID_SET_FILE_POINTER) {
                auto err = GetLastError();
                assert(err == NO_ERROR);
            }
        }
        
        out->width = safe_cast_assert(u32, dw_width);
        out->height = safe_cast_assert(u32, dw_height);
        out->channels = channels;
        out->pix_size = safe_cast_assert(u32, pix_size);
        return true;
    }
}
    
////
    u64                     qpc_dt;
    f32                     dt;
    uptr                    frame_number;
    
    input::State            inp;
    input::Sync_Input       sinp = {}; // init window_res to zero
    
    ////
    constexpr v3 BACKGROUND_COL = v3(41, 49, 52) / v3(255);
    
    GLuint                  VBOs[VBO_COUNT];
    GLuint                  VAOs[VAO_COUNT];
    
    GLuint*                 graph_thread_VBOs;
    GLuint*                 graph_thread_VAOs;
    
    GLuint                  shaders[PROG_COUNT];
    
    GLuint                  samplers[SAMPL_COUNT];
    GLuint                  textures[TEX_COUNT];
    
////
    struct Node {
        u32         parent;
        u32         children;
        u32         next;
        
        u64         begin;
        u64         length;
        lstr        name;
        u32         index;
        u32         flags;
        
        Node* get_parent (Node* arr) {      return parent == -1 ? nullptr : arr +parent; }
        Node* get_children (Node* arr) {    return children == -1 ? nullptr : arr +children; }
        Node* get_next (Node* arr) {        return next == -1 ? nullptr : arr +next; }
        
        void init_root () {
            parent =    -1;
            children =  -1;
            next =      -1;
            
            begin =     0;
            length =    0;
            name =      "<root>";
            index =     0;
            flags =     0;
        }
        
    };
    
    struct Thread {
        dynarr<Node>        tree_storage;
        
        u32                 last_node_added;
        
        f32                 unit_to_sec;
        f32                 sec_to_unit;
        
        void open (cstr name, u32 index, u64 ts) {
            
        }
        void close (cstr name, u32 index, u64 ts) {
            
        }
        void step (cstr name, u32 index, u64 ts) {
            
        }
        
    };
    
    dynarr<Thread>  threads(8);
    
#include "flamegraph_data_file.h"
    
namespace load {
    namespace file = flamegraph_data_file;
    
    DECLD file::File_Header const*  file_header;
    DECLD file::Thread const*       file_threads;
    DECLD char const*               file_thr_str_tbl;
    
    cstr get_thread_name (u32 thread_i) {
        assert(thread_i < file_header->thread_count);
        assert(file_threads[thread_i].name_tbl_offs < file_header->thr_name_str_tbl_size);
        return file_thr_str_tbl +file_threads[thread_i].name_tbl_offs;
    }
    cstr get_chunk_name (file::Chunk const* chunk) {
        return (cstr)(chunk +1);
    }
    
    file::Event const* get_chunk_events (file::Chunk const* chunk) {
        cstr str = get_chunk_name(chunk);
        return (file::Event const*)(str +str::len(str) +1);
    }
    
    cstr get_event_name (file::Event const* event) {
        return (cstr)(event +1);
    }
    
    file::Event const* get_next_event (file::Event const* event) {
        cstr str = get_event_name(event);
        return (file::Event const*)(str +str::len(str) +1);
    }
    
    void load_and_process () {
        Mem_Block   data;
        assert(!platform::read_whole_file_onto(&working_stk, input_filename, 0, &data));
        
        assert(data.size >= sizeof(file::File_Header));
        file_header = (file::File_Header const*)data.ptr;
        
        assert(file_header->id.i == file::FILE_ID.i);
        assert(file_header->file_size == data.size);
        
        print("File_Header: file_size %  total_event_count %  thread_count %  chunks_count %\n",
                    file_header->file_size,
                    file_header->total_event_count,
                    file_header->thread_count,
                    file_header->chunks_count );
        
        file_threads = (file::Thread const*)(file_header +1);
        
        file_thr_str_tbl = (char*)(file_threads +file_header->thread_count);
        
        u32 threads_event_counter = 0;
        
        for (u32 i=0; i<file_header->thread_count; ++i) {
            #if 1
            print("Thread % '%': sec_per_unit %  units_per_sec %  event_count %\n",
                    i, get_thread_name(i),
                    file_threads[i].sec_per_unit, 1.0f / file_threads[i].sec_per_unit,
                    file_threads[i].event_count );
            #endif
            
            threads_event_counter += file_threads[i].event_count;
            
            auto thr = threads.append();
            thr.tree_storage.init(1, 16 * 1024);
            thr.tree_storage[0].init_root();
            thr.last_node_added = 0; // root
            thr.unit_to_sec = file_threads[i].sec_per_unit;
            thr.sec_to_unit = 1.0f / thr.unit_to_sec;
        }
        
        assert(threads_event_counter == file_header->total_event_count);
        
        auto* chunk = (file::Chunk const*)(file_thr_str_tbl +file_header->thr_name_str_tbl_size);
        
        u32 total_events_counter = 0;
        for (u32 chunk_i=0; chunk_i<file_header->chunks_count; ++chunk_i) {
            #if 0
            print("Chunk % '%' %: offs_to_next %  event_count %  ts_begin %  ts_length %\n",
                    chunk_i, get_chunk_name(chunk), chunk->index,
                    chunk->offs_to_next,
                    chunk->event_count,
                    chunk->ts_begin,
                    chunk->ts_length );
            #endif
            
            auto* event = get_chunk_events(chunk);
            
            for (u32 i=0; i<chunk->event_count; ++i) {
                #if 1
                print("  Event % '%' %: % %\n",
                        i, get_event_name(event), event->index,
                        event->thread_indx,
                        event->ts );
                #endif
                
                assert(event->thread_indx < file_header->thread_count);
                auto* thr = &threads[ event->thread_indx ];
                
                auto* code_name = get_event_name(event);
                assert(code_name[0] != '\0' && code_name[1] != '\0');
                
                char source_code_thread =   code_name[0];
                char action =               code_name[1];
                cstr name =                 &code_name[2];
                
                switch (action) {
                    case '{':   thr->open(name, event->ts, event->index);       break;
                    case '}':   thr->close(name, event->ts, event->index);      break;
                    case '|':   thr->step(name, event->ts, event->index);       break;
                    default: assert(false, "action : '%'", action);
                }
                
                event = get_next_event(event);
            }
            
            total_events_counter += chunk->event_count;
            chunk = ptr_add(chunk, chunk->offs_to_next);
        }
        
        assert(total_events_counter == file_header->total_event_count);
        
    }
}
    
    
void process_file () {
    
    load::load_and_process();
    
    #if 0
    { // Upload bars data into VBO
        
        auto* threads = (Thread*)(header +1);
        auto* str_tbl = (char*)(threads +header->thread_count);
        auto* blocks = align_up( (Block*)(str_tbl +header->str_tbl_size), DATA_ALIGN );
        
        graph_file_threads = threads;
        
        bars_count = header->total_block_count;
        
        print("Uploading VBO data for % bars.\n", bars_count);
        
        auto thread_count = header->thread_count;
        
        {
            graph_thread_VBOs = working_stk.pushArr<GLuint>(thread_count);
            graph_thread_VAOs = working_stk.pushArr<GLuint>(thread_count);
            
            auto gl_count = safe_cast_assert(GLsizei, thread_count);
            
            glGenBuffers(gl_count, graph_thread_VBOs);
            glGenVertexArrays(gl_count, graph_thread_VAOs);
        }
        
        constexpr tv3<GLubyte> COLS[8] = {
            tv3<GLubyte>(192,  26,  26),
            tv3<GLubyte>( 26, 192,  26),
            tv3<GLubyte>( 26,  26, 192),
            tv3<GLubyte>(192, 192,  26),
            tv3<GLubyte>( 26, 192, 192),
            tv3<GLubyte>(192,  26, 192),
            tv3<GLubyte>(192,  77, 192),
            tv3<GLubyte>(128, 192,  77),
        };
        
        glBindBuffer(GL_TEXTURE_BUFFER, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
        
        glBufferData(GL_TEXTURE_BUFFER,
                safe_cast_assert(GLsizeiptr, header->str_tbl_size), str_tbl, GL_STREAM_DRAW);
        
        print("Total blocks: %\n", bars_count);
        
        for (u32 thread_i=0; thread_i<thread_count; ++thread_i) {
            
            u32 count = threads[thread_i].block_count;
            
            auto thread_name = str_tbl +threads[thread_i].name_tbl_offs;
            print("Thread % '%' % blocks:\n", thread_i, thread_name, count);
        }
        
        u32 i = 0;
        
        for (u32 thread_i=0; thread_i<thread_count; ++thread_i) {
            
            u32 count = threads[thread_i].block_count;
            
            auto thread_name = str_tbl +threads[thread_i].name_tbl_offs;
            #if 0
            print("Thread % '%' % blocks:\n", thread_i, thread_name, count);
            #endif
            
            gl_bars_vao(graph_thread_VAOs[thread_i], graph_thread_VBOs[thread_i]);
            
            DEFER_POP(&working_stk);
            auto* data = working_stk.pushArr<std140_Bar>(count);
            
            for (u32 j=0; j<count; ++j) {
                
                auto block = blocks[i];
                auto name = str_tbl +block.name_tbl_offs;
                
                #if 0
                print("(%) % %:%:%  % %\n", i, j, block.begin, block.length, block.depth, name, block.type_flags);
                Sleep(0);
                #endif
                
                data[j] = {}; // zero potential padding
                
                auto col = COLS[ j % arrlenof(COLS) ];
                if (block.type_flags & BT_WAIT) {
                    col = tv3<GLubyte>(120, 120, 120);
                }
                
                data[j].pos_t.set( (f32)block.begin );
                data[j].len_t.set( (f32)block.length );
                data[j].depth.set( safe_cast_assert(GLint, block.depth) );
                data[j].type_flags.set(block.type_flags);
                data[j].name_offs.set(safe_cast_assert(GLint, block.name_tbl_offs));
                data[j].name_len.set(safe_cast_assert(GLint, str::len(name)));
                data[j].col = col;
                
                ++i;
            }
            
            auto gl_size = safe_cast_assert(GLsizeiptr, count * sizeof(std140_Bar));
            
            // NOTE: GL_ARRAY_BUFFER bound in gl_bars_vao() call
            glBufferData(GL_ARRAY_BUFFER, gl_size, data, GL_STATIC_DRAW);
        }
        
        assert(i == bars_count, "% %", i, bars_count);
        
        print("done.\n");
    }
    #endif
    
}
    
void init () {
    //THR_ENGINE->init_gl();
    
    { // OpenGL state
        
        glDisable(GL_CULL_FACE);
        
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        
        glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
    
    {
        auto ret = wglSwapIntervalEXT(1);
        assert(ret != FALSE);
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    { // GL buffer setup
        glGenBuffers(VBO_COUNT, VBOs);
        
        glBindBuffer(GL_UNIFORM_BUFFER, VBOs[VBO_UBO]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(std140_Global), NULL, GL_STREAM_DRAW);
        
        glBindBufferRange(GL_UNIFORM_BUFFER, GLOBAL_UNIFORM_BLOCK_BINDING, VBOs[VBO_UBO],
                0, sizeof(std140_Global));
    }
    { // VAO config
        glGenVertexArrays(VAO_COUNT, VAOs);
        
        { // Empty VAO
            glBindVertexArray(VAOs[VAO_EMTPY]);
            
        }
        glBindVertexArray(0);
    }
    // CAUTION: Remember to glBindVertexArray(0) after using this function 
    auto gl_bars_vao = [] (GLuint VAO, GLuint VBO) {
        glBindVertexArray(VAO);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        auto vert_size = safe_cast_assert(GLsizei, sizeof(std140_Bar));
        
        glVertexAttribPointer(  0, 1, GL_FLOAT, GL_FALSE, vert_size,        (void*)offsetof(std140_Bar, pos_t) );
        glVertexAttribPointer(  1, 1, GL_FLOAT, GL_FALSE, vert_size,        (void*)offsetof(std140_Bar, len_t) );
        glVertexAttribIPointer( 2, 1, GL_INT, vert_size,                    (void*)offsetof(std140_Bar, depth) );
        glVertexAttribIPointer( 3, 1, GL_UNSIGNED_INT, vert_size,           (void*)offsetof(std140_Bar, type_flags) );
        glVertexAttribIPointer( 4, 1, GL_INT, vert_size,                    (void*)offsetof(std140_Bar, name_offs) );
        glVertexAttribIPointer( 5, 1, GL_INT, vert_size,                    (void*)offsetof(std140_Bar, name_len) );
        glVertexAttribPointer(  6, 3, GL_UNSIGNED_BYTE, GL_TRUE, vert_size, (void*)offsetof(std140_Bar, col) );
        
        glVertexAttribDivisor(0, 1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    };
    
    { // Shaders
        setup_shaders(shaders);
    }
    
    {
        glUseProgram(shaders[PROG_ACCUM_CLEAR]);
        
        auto unif = glGetUniformLocation(shaders[PROG_ACCUM_CLEAR], "background_col");
        assert(unif != -1);
        glUniform3f(unif, BACKGROUND_COL.x, BACKGROUND_COL.y, BACKGROUND_COL.z);
    }
    
    { // Textures
        {
            GLint align;
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &align);
            assert(align == 4);
        }
        
        glGenSamplers(SAMPL_COUNT, samplers);
        glGenTextures(TEX_COUNT, textures);
        
        {
            glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glSamplerParameteri(samplers[SAMPL_TEXT], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            
            glBindSampler(GLYPHS_TEX_UNIT, samplers[SAMPL_TEXT]);
        }
        
        {
            glActiveTexture(GL_TEXTURE0 +GLYPHS_TEX_UNIT);
            glBindTexture(GL_TEXTURE_2D, textures[TEX_GLYPHS]);
            
            do { // Upload textures
                HANDLE handle;
                {
                    auto ret = CreateFileA(TEXTURES_DIR "8x15.bmp", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_FLAG_SEQUENTIAL_SCAN, NULL);
                    if (ret == INVALID_HANDLE_VALUE) {
                        assert(false, "CreateFileA failed for '%'", input_filename);
                        break;
                    }
                    handle = ret;
                }
                
                bmp::Bmp_Info res;
                if (!bmp::parse_bmp_header(handle, &res)) {
                    break;
                }
                assert(res.width == (8 * 16) && res.height == (15 * 8));
                assert(res.channels == 1);
                
                byte* data;
                {
                    DWORD read_size = safe_cast_assert(DWORD, res.pix_size);
                    data = working_stk.pushArrAlign<16, byte>(read_size);
                    
                    DWORD bytesRead;
                    auto ret = ReadFile(handle, data, read_size, &bytesRead, NULL);
                    assert(ret != 0);
                    assert(bytesRead == read_size);
                }
                
                {
                    GLsizei w = safe_cast_assert(GLsizei, res.width);
                    GLsizei h = safe_cast_assert(GLsizei, res.height);
                    
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                }
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            } while (0);
        }
        
        {
            glActiveTexture(GL_TEXTURE0 +TEXT_DATA_TEX_UNIT);
            glBindTexture(GL_TEXTURE_BUFFER, textures[TEX_TEXT_DATA]);
        }
    }
    
    process_file();
}
    
    f64                     exp_moving_avg;
    constexpr f64           EXP_MOVING_AVG_ALPHA =  1.0 / 8.0;
    
    bool                    dragging_view = false;
    f32                     view_drag_grab_pos;
    f32                     view_pos_sec_to_pix =       0.0f; // sec-space pos of left screen edge
    f32                     view_scale_sec_to_pix =     1000.0f; // pixels per sec
    
    f32                     select_pos =        0.0f; // sec
    f32                     diff_select_pos =   0.0f; // sec
    
GLint get_thread_bars_offs (u32 thread_i) {
    return ((GLint)thread_i * (SHAD_BAR_OFFS * 8)) +SHAD_BAR_BASE_OFFS;
}
    
GLsizei gl_thread_bars_draw_prep (u32 thread_i) {
    
    #if 0
    glBindVertexArray(graph_thread_VAOs[thread_i]);
    
    {
        std140::float_ temp;
        temp.set(graph_file_threads[thread_i].sec_per_unit);
        
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsetof(std140_Global, units_counter_to_sec),
                        sizeof temp, &temp);
    }
    {
        std140::sint_ temp;
        temp.set(get_thread_bars_offs(thread_i));
        glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, bar_base_offs),
                    sizeof temp, &temp);
    }
    
    using namespace flamegraph_data_file;
    auto* threads = (Thread*)(load::file_header +1);
    
    return safe_cast_assert(GLsizei, threads[thread_i].block_count);
    #else
    return 0;
    #endif
}
    
void frame () {
    
    { // Input query
        input::query(&inp, &sinp);
    }
    
    if (sinp.window_res_changed) {
        auto res = sinp.window_res;
        
        {
            std140::vec2 temp;
            temp.set(cast_v2<v2>(res));
            
            glBufferSubData(GL_UNIFORM_BUFFER,
                            offsetof(std140_Global, fscreen_res),
                            sizeof temp, &temp);
        }
        {
            auto temp = safe_cast_assert_v2<tv2<GLsizei>>(res);
            glViewport(0, 0, temp.x, temp.y);
        }
    }
    
    f32 dragged_view_pos;
    {
        f32 mouse_pos_pixel_center = (f32)sinp.mouse_cursor_pos.x +0.5f;
        f32 old_view_scale = view_scale_sec_to_pix; // time-unit-space, but relative to left screen edge
        
        if (sinp.mouse_cursor_in_window) {
            f32 log_space = fp::log(view_scale_sec_to_pix);
            log_space += (f32)inp.mouse_wheel_accum * 0.0625f;
            view_scale_sec_to_pix = fp::exp(log_space);
        }
        
        f32 new_scaled_mouse_pos = mouse_pos_pixel_center / view_scale_sec_to_pix; // time-unit-space, but relative to left screen edge
        
        dragged_view_pos = view_pos_sec_to_pix;
        
        if (!dragging_view) {
            if (inp.rmb_down && sinp.mouse_cursor_in_window) {
                //assert(inp.rmb_count > 0); // button went down
                
                view_drag_grab_pos = new_scaled_mouse_pos;
                dragging_view = true;
            } else {
                f32 old_scaled_mouse_pos = mouse_pos_pixel_center / old_view_scale; // time-unit-space, but relative to left screen edge
                dragged_view_pos -= new_scaled_mouse_pos -old_scaled_mouse_pos; // make scaling centered around mouse position
                view_pos_sec_to_pix = dragged_view_pos;
            }
        } else {
            dragged_view_pos -= new_scaled_mouse_pos -view_drag_grab_pos;
            if (!inp.rmb_down) {
                assert(inp.rmb_count > 0); // button went up
                
                view_pos_sec_to_pix = dragged_view_pos; // apply the dragging
                dragging_view = false;
            }
        }
        
        {
            std140_View temp;
            temp.view_pos_sec_to_pix.set( dragged_view_pos );
            temp.view_scale_sec_to_pix.set( view_scale_sec_to_pix );
            
            glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, view_pos_sec_to_pix),
                    sizeof temp, &temp);
        }
        
        //print("dragged_view_pos: % view_scale_sec_to_pix: %\n", dragged_view_pos, view_scale_sec_to_pix);
    }
    
    {
        if (inp.lmb_down && sinp.mouse_cursor_in_window) {
            auto pos = view_pos_sec_to_pix +((sinp.mouse_cursor_pos.x +0.5f) / view_scale_sec_to_pix); // click pixel center
            if (!inp.ctrl_down) {
                select_pos = pos;
            } else {
                diff_select_pos = pos;
            }
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Instanced bars draw
    glUseProgram(shaders[PROG_BARS]);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    for (u32 thread_i=0; thread_i<load::file_header->thread_count; ++thread_i) {
        
        GLsizei gl_bars_count = gl_thread_bars_draw_prep(thread_i);
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, gl_bars_count);
    }
    
    glBindVertexArray(VAOs[VAO_EMTPY]);
    
    // Bars blend background draw
    glUseProgram(shaders[PROG_ACCUM_CLEAR]);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Bars name text
    glUseProgram(shaders[PROG_BAR_TEXT]);
    for (u32 thread_i=0; thread_i<load::file_header->thread_count; ++thread_i) {
        
        GLsizei gl_bars_count = gl_thread_bars_draw_prep(thread_i);
        
        glBindBuffer(GL_TEXTURE_BUFFER, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
        
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R8I, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gl_bars_count);
    }
    
    // Bars duration text
    glUseProgram(shaders[PROG_BAR_DURATION_TEXT]);
    for (u32 thread_i=0; thread_i<load::file_header->thread_count; ++thread_i) {
        
        GLsizei gl_bars_count = gl_thread_bars_draw_prep(thread_i);
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gl_bars_count);
    }
    
    glUseProgram(shaders[PROG_VERT_LINE]);
    {
        std140::float_ temp = {};
        {
            temp.set(0.0f);
            
            glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, vert_line_pos),
                    sizeof temp, &temp);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        }
        {
            temp.set(select_pos);
            glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, vert_line_pos),
                    sizeof temp, &temp);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        }
        {
            temp.set(diff_select_pos);
            glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, vert_line_pos),
                    sizeof temp, &temp);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        }
    }
    
    // Free text drawing
    glUseProgram(shaders[PROG_FREE_TEXT]);
    glBindBuffer(GL_TEXTURE_BUFFER, VBOs[VBO_BUF_TEX_FREE_TEXT]);
    
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R8I, VBOs[VBO_BUF_TEX_FREE_TEXT]);
    
    auto draw_text_line = [] (tv2<GLint> pos_px, lstr str) {
        
        GLint gl_len = (GLint)str.len;
        {
            std140_String temp = {};
            temp.pos.set(pos_px);
            temp.str_clip_x.set(+fp::inf<f32>());
            temp.str_len.set(gl_len);
            
            glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(std140_Global, pos),
                    sizeof temp, &temp);
        }
        
        glBufferData(GL_TEXTURE_BUFFER, safe_cast_assert(GLsizeiptr, gl_len), NULL, GL_STREAM_DRAW);
        glBufferData(GL_TEXTURE_BUFFER, safe_cast_assert(GLsizeiptr, gl_len), str.str, GL_STREAM_DRAW);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    };
    
    GLint line_height = GLYPH_DIM.y;
    
    auto lowerleft_lines_y = [=] (GLint line) -> GLint {
        return line * line_height;
    };
    auto lowerleft_lines = [=] (GLint line) -> tv2<GLint> {
        return tv2<GLint>(0, lowerleft_lines_y(line));
    };
    auto upperleft_lines_y = [=] (GLint line) -> GLint {
        return safe_cast_assert(GLint, sinp.window_res.y) -((line +1) * line_height);
    };
    auto upperleft_lines = [=] (GLint line) -> tv2<GLint> {
        return tv2<GLint>(0, upperleft_lines_y(line));
    };
    
    { // Thread names and seperator
        
        for (u32 thread_i=0; thread_i<load::file_header->thread_count; ++thread_i) {
            GLint y = get_thread_bars_offs(thread_i) -(1 * SHAD_BAR_OFFS);
            cstr name = load::get_thread_name(thread_i);
            draw_text_line(tv2<GLint>(0, y +8),
                    "----------------------------------------------------------------------"
                    "----------------------------------------------------------------------"
                    "----------------------------------------------------------------------"
                    "----------------------------------------------------------------------"
                    "----------------------------------------------------------------------"
                    "----------------------------------------------------------------------"
                    );
            draw_text_line(tv2<GLint>(0, y +line_height +8), lstr::count_cstr(name));
        }
    }
    
    { // Markers text
        DEFER_POP(&working_stk);
        
        {
            GLint x = (GLint)((0.0f -dragged_view_pos) * view_scale_sec_to_pix) +4;
            
            draw_text_line(tv2<GLint>(x, lowerleft_lines_y(0) +4), "0 ms");
        }
        {
            auto t = units::Time(select_pos);
            
            lstr str = print_working_stk("%%", t.val,t.unit);
            
            GLint x = (GLint)((select_pos -dragged_view_pos) * view_scale_sec_to_pix) +4;
            
            draw_text_line(tv2<GLint>(x, lowerleft_lines_y(1) +4), str);
        }
        {
            auto t = units::Time(diff_select_pos);
            
            lstr str = print_working_stk("%%", t.val,t.unit);
            
            GLint x = (GLint)((diff_select_pos -dragged_view_pos) * view_scale_sec_to_pix);
            x -= (safe_cast_assert(GLint, str.len) * GLYPH_DIM.x) +4;
            
            draw_text_line(tv2<GLint>(x, lowerleft_lines_y(2) +4), str);
        }
        {
            auto diff = select_pos -diff_select_pos;
            
            auto t = units::Time(diff);
            
            lstr str = print_working_stk("%%", t.val,t.unit);
            
            GLint x = (GLint)((diff_select_pos -dragged_view_pos) * view_scale_sec_to_pix) +4;
            
            draw_text_line(tv2<GLint>(x, lowerleft_lines_y(2) +4), str);
        }
    }
    
    { // Graph tool frame timings text draw
        
        if (frame_number == 1) {
            exp_moving_avg = time::cpu_qpc_prev_frame_ms;
        } else if (frame_number > 1) {
            exp_moving_avg = (time::cpu_qpc_prev_frame_ms * EXP_MOVING_AVG_ALPHA)
                    +(exp_moving_avg * (1.0 -EXP_MOVING_AVG_ALPHA));
        }
        
        lstr str = print_working_stk("frame#%,  prev_dt: % ms  roll_avg: %",
                    frame_number, time::cpu_qpc_prev_frame_ms, exp_moving_avg);
        
        draw_text_line(upperleft_lines(0), str);
    }
    
}
    
////
}
