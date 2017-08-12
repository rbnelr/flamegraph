
namespace engine {

/*
	Coordinate system convetions:
	X - right/east,		red, right hand is primary
	Y - forward/north,	green
	Z - up,				blue, least important coordinate, discribes height in maps etc., x & y are like coordinates in a map
*/

#define PI				3.1415926535897932384626433832795f
#define TWO_PI			6.283185307179586476925286766559f
#define PId				3.1415926535897932384626433832795
#define TWO_PId			6.283185307179586476925286766559

#define QNAN			fp::qnan<f32>()
#define QNANd			fp::qnan<f64>()

#define DEG_TO_RADd		(TWO_PId / 360.0)
#define DEG_TO_RAD		((f32)DEG_TO_RADd)
#define RAD_TO_DEGd		(360.0 / TWO_PId)
#define RAD_TO_DEG		((f32)RAD_TO_DEGd)

// Units which always convert to the internal representation (which is radians)
DECL constexpr	f32 deg (f32 deg) {
	return deg * DEG_TO_RAD;
}
DECL constexpr	f64 deg (f64 deg) {
	return deg * DEG_TO_RADd;
}
DECL constexpr	f32 rad (f32 rad) {
	return rad;
}
DECL constexpr	f64 rad (f64 rad) {
	return rad;
}

template <typename T>	DECL constexpr tv2<T> deg (tv2<T> vp deg) {		return deg * tv2<T>(T(DEG_TO_RADd)); }
template <typename T>	DECL constexpr tv3<T> deg (tv3<T> vp deg) {		return deg * tv3<T>(T(DEG_TO_RADd)); }
template <typename T>	DECL constexpr tv4<T> deg (tv4<T> vp deg) {		return deg * tv4<T>(T(DEG_TO_RADd)); }
template <typename T>	DECL constexpr tv2<T> rad (tv2<T> vp rad) {		return rad; }
template <typename T>	DECL constexpr tv3<T> rad (tv3<T> vp rad) {		return rad; }
template <typename T>	DECL constexpr tv4<T> rad (tv4<T> vp rad) {		return rad; }

// Conversions
DECL constexpr	f64 to_deg (f64 rad) {
	return rad * RAD_TO_DEGd;
}
DECL constexpr	f32 to_deg (f32 rad) {
	return rad * RAD_TO_DEG;
}
DECL constexpr	f32 to_rad (f32 deg) {
	return deg * DEG_TO_RAD;
}
DECL constexpr	f64 to_rad (f64 deg) {
	return deg * DEG_TO_RADd;
}

template <typename T>	DECL constexpr tv2<T> to_deg (tv2<T> vp rad) {	return rad * tv2<T>(T(RAD_TO_DEGd)); }
template <typename T>	DECL constexpr tv3<T> to_deg (tv3<T> vp rad) {	return rad * tv3<T>(T(RAD_TO_DEGd)); }
template <typename T>	DECL constexpr tv4<T> to_deg (tv4<T> vp rad) {	return rad * tv4<T>(T(RAD_TO_DEGd)); }
template <typename T>	DECL constexpr tv2<T> to_rad (tv2<T> vp deg) {	return deg * tv2<T>(T(DEG_TO_RADd)); }
template <typename T>	DECL constexpr tv3<T> to_rad (tv3<T> vp deg) {	return deg * tv3<T>(T(DEG_TO_RADd)); }
template <typename T>	DECL constexpr tv4<T> to_rad (tv4<T> vp deg) {	return deg * tv4<T>(T(DEG_TO_RADd)); }

DECL f32 mod_pn_180deg (f32 ang) { // mod float into -/+ 180deg range
	return fp::proper_mod(ang +deg(180.0f), deg(360.0f)) -deg(180.0f);
}

template <typename T>	DECL T to_linear (T srgb) {
	if (srgb <= T(0.0404482362771082)) {
		return srgb * T(1/12.92);
	} else {
		return fp::pow( (srgb +T(0.055)) * T(1/1.055), T(2.4) );
	}
}
template <typename T>	DECL T to_srgb (T linear) {
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

template <typename T>	DECL constexpr tv3<T> col (T x, T y, T z) {		return tv3<T>(x,y,z); }
template <typename T>	DECL constexpr tv3<T> srgb (T x, T y, T z) {	return to_linear(tv3<T>(x,y,z) * tv3<T>(T(1.0/255.0))); }
template <typename T>	DECL constexpr tv3<T> col (T all) {				return col(all,all,all); }
template <typename T>	DECL constexpr tv3<T> srgb (T all) {			return srgb(all,all,all); }

DECL m3 scale_3 (v3 vp scale) {
	m3 ret = m3::zero();
	ret.row(0, 0, scale.x);
	ret.row(1, 1, scale.y);
	ret.row(2, 2, scale.z);
	return ret;
}
DECL m3 rotate_X3 (f32 angle) {
	auto t = fp::sincos(angle);
	m3 ret = m3::row(	1,		0,		0,
						0,		+t.c,	-t.s,
						0,		+t.s,	+t.c );
	return ret;
}
DECL m3 rotate_Y3 (f32 angle) {
	auto t = fp::sincos(angle);
	m3 ret = m3::row(	+t.c,	0,		+t.s,
						0,		1,		0,
						-t.s,	0,		+t.c );
	return ret;
}
DECL m3 rotate_Z3 (f32 angle) {
	auto t = fp::sincos(angle);
	m3 ret = m3::row(	+t.c,	-t.s,	0,
						+t.s,	+t.c,	0,
						0,		0,		1 );
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
	m4 ret = m4::row(	1,		0,		0,		0,
						0,		+t.c,	-t.s,	0,
						0,		+t.s,	+t.c,	0,
						0,		0,		0,		1 );
	return ret;
}
DECL m4 rotate_Y4 (f32 angle) {
	auto t = fp::sincos(angle);
	m4 ret = m4::row(	+t.c,	0,		+t.s,	0,
						0,		1,		0,		0,
						-t.s,	0,		+t.c,	0,
						0,		0,		0,		1 );
	return ret;
}
DECL m4 rotate_Z4 (f32 angle) {
	auto t = fp::sincos(angle);
	m4 ret = m4::row(	+t.c,	-t.s,	0,		0,
						+t.s,	+t.c,	0,		0,
						0,		0,		1,		0,
						0,		0,		0,		1 );
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
	return hm::row( 1,		0,		0,		0,
					0,		+t.c,	-t.s,	0,
					0,		+t.s,	+t.c,	0 );
}
DECL hm rotate_y (f32 angle) {
	auto t = fp::sincos(angle);
	return hm::row( +t.c,	0,		+t.s,	0,
					0,		1,		0,		0,
					-t.s,	0,		+t.c,	0 );
}
DECL hm rotate_z (f32 angle) {
	auto t = fp::sincos(angle);
	return hm::row( +t.c,	-t.s,	0,		0,
					+t.s,	+t.c,	0,		0,
					0,		0,		1,		0 );
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
	m3 ret = m3::row(	1,		0,		0,
						0,		+g.c,	-g.s,
						0,		+g.s,	+g.c );
	return ret;
};
DECL m3 rotate_Y_90deg (si multiple) {
	auto g = _90deg_getSC(multiple);
	m3 ret = m3::row(	+g.c,	0,		+g.s,
						0,		1,		0,
						-g.s,	0,		+g.c );
	return ret;
};
DECL m3 rotate_Z_90deg (si multiple) {
	auto g = _90deg_getSC(multiple);
	m3 ret = m3::row(	+g.c,	-g.s,	0,
						+g.s,	+g.c,	0,
						0,		0,		1 );
	return ret;
};

//// UBO
#include "ogl_ubo_interface.hpp"

#pragma pack (push, 1)

struct std140_Bar {
	std140::float_		pos_t;
	std140::float_		len_t;
	std140::sint_		depth;
	std140::uint_		type_flags;
	std140::sint_		name_offs;
	std140::sint_		name_len;
	tv3<GLubyte>		col;
	GLubyte				pad_00[1];
};
ASSERT_STD140_ALIGNMENT(std140_Bar, pos_t);
ASSERT_STD140_ALIGNMENT(std140_Bar, len_t);
ASSERT_STD140_ALIGNMENT(std140_Bar, depth);
ASSERT_STD140_ALIGNMENT(std140_Bar, type_flags);
ASSERT_STD140_ALIGNMENT(std140_Bar, name_offs);
ASSERT_STD140_ALIGNMENT(std140_Bar, name_len);
//ASSERT_STD140_ALIGNMENT(std140_Bar, col);

struct std140_Global {
	std140::vec2		fscreen_res;
	std140::float_		view_pos_sec_to_pix;
	std140::float_		view_scale_sec_to_pix;
	std140::float_		units_counter_to_sec;
	std140::sint_		bar_base_offs;
	std140::float_		vert_line_pos;
	std140::float_		pad_00[1];
	
	std140::ivec2		pos;
	std140::sint_		str_len;
	std140::float_		str_clip_x;
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
	std140::float_		view_pos_sec_to_pix;
	std140::float_		view_scale_sec_to_pix;
};

struct std140_String {
	std140::ivec2		pos;
	std140::sint_		str_len;
	std140::float_		str_clip_x;
};

#pragma pack (pop)

//// VBO
constexpr GLuint	GLOBAL_UNIFORM_BLOCK_BINDING =		0;

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
constexpr GLint		SHAD_BAR_HEIGHT =		36;
constexpr GLint		SHAD_BAR_OFFS =			38;
constexpr GLint		SHAD_BAR_BASE_OFFS =	2 * SHAD_BAR_OFFS;

constexpr auto		GLYPH_DIM =				tv2<GLint>(8, 15);
constexpr u32		SHAD_MAX_STR_LEN =		256; // 256 * 8pix = 2k pix long strings

//// Shaders
constexpr GLint		TEXT_DATA_TEX_UNIT =					0;
constexpr GLint		GLYPHS_TEX_UNIT =						1;

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
	layout(std140)			uniform Global {
		vec2			fscreen_res;
		float			view_pos_sec_to_pix;
		float			view_scale_sec_to_pix; // pixels per time-unit
		float			units_counter_to_sec;
		int				bar_base_offs;
		float			vert_line_pos;
		// 
		ivec2			pos; // pixels
		int				str_len;
		float			str_clip_x;
	} g;
	
	const int	BAR_HEIGHT =		36;
	const int	LOW_BAR_HEIGHT =	18;
	const int	BAR_OFFS =			38;
	
	const uint	BT_WAIT =			uint(0x1);
	
	const ivec2 GLYPH_DIM =			ivec2(8, 15);
	const ivec2 GLYPH_TEX_COUNTS =	ivec2(16, 8);
	const ivec2 GLYPH_TEX_DIM =		GLYPH_DIM * GLYPH_TEX_COUNTS;
	
	const vec3	TEXT_FG_COL =		vec3(224, 226, 228) / vec3(255);
)_SHAD";

struct Shader {
	GLenum			type;
	GLchar const*	src;
};

constexpr Shader SHADERS[] = {

/* SHAD_VERT_BARS */ { GL_VERTEX_SHADER, R"_SHAD(

layout(location=0)	in	float			attr_pos_t;
layout(location=1)	in	float			attr_len_t;
layout(location=2)	in	int				attr_depth;
layout(location=3)	in	uint			attr_type_flags;
//layout(location=4)	in	int				attr_name_offs;
//layout(location=5)	in	int				attr_name_len;
layout(location=6)	in	vec3			attr_col;

flat				out vec3			pass_col;
smooth				out float			interp_alpha;

flat				out float			pass_fbar_height;
flat				out float			pass_w_px;
smooth				out vec2			interp_uv_px;

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

flat				in	vec3			pass_col;
smooth				in	float			interp_alpha;

flat				in	float			pass_fbar_height;
flat				in	float			pass_w_px;
smooth				in	vec2			interp_uv_px;

out vec4			frag_col;

bvec2 lte (vec2 l, vec2 r) {
	return not(greaterThan(l, r));
}

void main () {
	//frag_col = vec4(pass_col, interp_alpha);
	
	if (	(interp_uv_px.x < 1.2 || interp_uv_px.x > (pass_w_px -1.2)) &&
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

out vec4			frag_col;

void main () {
	
	frag_col = vec4(1.0, 1.0, 1.0, 1.0);
}
)_SHAD" },
#endif

/* SHAD_FRAG_ACCUM_CLEAR */ { GL_FRAGMENT_SHADER, R"_SHAD(

uniform vec3		background_col;

out vec4			frag_col;

void main () {
	
	frag_col = vec4(background_col, 1.0);
}
)_SHAD" },

/* SHAD_VERT_VERT_LINE */ { GL_VERTEX_SHADER, R"_SHAD(

smooth				out float			interp_alpha;

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

smooth				in	float			interp_alpha;

out vec4			frag_col;

void main () {
	frag_col = vec4(vec3(1.0), interp_alpha * 0.75);
}
)_SHAD" },

/* SHAD_VERT_BAR_TEXT */ { GL_VERTEX_SHADER, R"_SHAD(

layout(location=0)	in	float			attr_pos_t;
layout(location=1)	in	float			attr_len_t;
layout(location=2)	in	int				attr_depth;
//layout(location=3)	in	uint			attr_type_flags;
layout(location=4)	in	int				attr_name_offs;
layout(location=5)	in	int				attr_name_len;
//layout(location=6)	in	vec3			attr_col;

flat				out int				pass_name_offs;
smooth				out vec2			interp_uv;

const int	TEXT_X_BORD =		2;
const int	NAME_LOWER_BORD =	3;

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

flat				in	int				pass_name_offs;
smooth				in	vec2			interp_uv;

					uniform isamplerBuffer	text_data_tex;
					uniform sampler2D		glyphs_tex;
					
out vec4			frag_col;

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

layout(location=0)	in	float			attr_pos_t;
layout(location=1)	in	float			attr_len_t;
layout(location=2)	in	int				attr_depth;
layout(location=3)	in	uint			attr_type_flags;
//layout(location=4)	in	int				attr_name_offs;
//layout(location=5)	in	int				attr_name_len;
//layout(location=6)	in	vec3			attr_col;

flat				out uvec3			pass_chars;
smooth				out vec2			interp_uv;

const int	TEXT_X_BORD =		2;
const int	NAME_LOWER_BORD =	3;

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
			if (		val < (1.0 / 1000 / 1000) ) {
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
			
			pass_chars[0] =		(uint(val * (1.0 / 1000.0)) %	uint(10));
			pass_chars[0] |=	(uint(val * (1.0 / 100.0)) %	uint(10)) << 8;
			pass_chars[0] |=	(uint(val * (1.0 / 10.0)) %		uint(10)) << 16;
			pass_chars[0] |=	(uint(val) %					uint(10)) << 24;
			pass_chars[1] =		 uint(0x2E); // '.'
			pass_chars[1] |=	(uint(val * 10.0) %				uint(10)) << 8;
			pass_chars[1] |=	(uint(val * 100.0) %			uint(10)) << 16;
			pass_chars[1] |=	(uint(val * 1000.0) %			uint(10)) << 24;
			
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

flat				in	uvec3			pass_chars;
smooth				in	vec2			interp_uv;

					uniform sampler2D		glyphs_tex;
					
out vec4			frag_col;

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

smooth					out vec2			pass_uv;

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

smooth					in	vec2			pass_uv;

						uniform isamplerBuffer	text_data_tex;
						uniform sampler2D		glyphs_tex;
						
						out vec4			frag_col;

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
	shad_id_e	shaders[MAX_SHAD_PER_PROG];
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
		case GL_VERTEX_SHADER:		shader_type_str = "vertex";		break;
		case GL_GEOMETRY_SHADER:	shader_type_str = "geometry";	break;
		case GL_FRAGMENT_SHADER:	shader_type_str = "fragment";	break;
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
		GLchar const*	src[3] = { GLSL_VERSION_SRC, SHAD_GLOBAL_UBO, in_shad.src };
		GLint			len[3] = { (GLint)-1, (GLint)-1, (GLint)-1 };
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
			//		"Global");
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
		char	str[2];
		WORD	w;
	} filetype_magic_number_u = { 'B', 'M' };
	
	struct Bmp_Info {
		u32						width;
		u32						height;
		ui						channels;
		u32						pix_size;
	};
	
	bool parse_bmp_header (HANDLE handle, Bmp_Info* out) {
		
		#define require(cond) do { if (!(comd)) { return false; } } while (0)
		
		constexpr uptr BMP_HEADER_READ_SIZE =
				 sizeof(BITMAPFILEHEADER)
				+MAX( sizeof(BITMAPINFOHEADER), sizeof(BITMAPV4HEADER))
				+(256 * sizeof(RGBQUAD));
		
		DEFER_POP(&working_stk);
		
		BITMAPFILEHEADER*	header;
		uptr				actual_read_size;
		uptr				file_size;
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
		
		ui						channels;
		DWORD					dw_stride;
		
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
DECLD u64						qpc_dt;
DECLD f32						dt;
DECLD uptr						frame_number;

DECLD input::State				inp;
DECLD input::Sync_Input			sinp = {}; // init window_res to zero

////
#if DEBUG
#define _CAP(c) 0 // to detect cases where dynarr reallocation might trigger a bug
#else
#define _CAP(c) c
#endif

DECLD constexpr v3				BACKGROUND_COL =			v3(41, 49, 52) / v3(255);
DECLD constexpr	u32				THREADS_DYNARR_CAP =		_CAP(8);
DECLD constexpr	u32				LEVELS_DYNARR_CAP =			_CAP(32);
DECLD constexpr	u32				BLOCKS_DYNARR_CAP =			_CAP(16 * 1024); // per thread
DECLD constexpr	u32				UNIQUE_BLOCKS_DYNARR_CAP =	_CAP(256);
DECLD constexpr	u32				BLOCKS_STR_TABLE_CAP =		_CAP(4096);

DECLD constexpr u32				BLOCKS_PER_VBO =			kibi(64); // 128
DECLD constexpr u32				GL_BUF_CAP =				_CAP(64);

DECLD GLuint					VBOs[VBO_COUNT];
DECLD GLuint					VAOs[VAO_COUNT];

DECLD GLuint					shaders[PROG_COUNT];

DECLD GLuint					samplers[SAMPL_COUNT];
DECLD GLuint					textures[TEX_COUNT];

#include "flamegraph_data_file.hpp"
namespace f = flamegraph_data_file;

struct Block {
	u32					parent;
	u32					children;
	u32					next;
	
	u64					begin;
	u64					length;
	block_type_flags_e	flags;
	u32					depth;
	u32					name_offs;
	u32					name_len;
	u32					index;
	
	DECLM lstr get_name ();
};

block_type_flags_e get_flags_and_name (lstr name, lstr* out_remain_name) {
	block_type_flags_e	flags = BT_NONE;
	lstr remain_name = name;
	
	cstr cur = remain_name.str;
	while (*cur != '\0' && *cur != ':') ++cur;
	
	if (*cur == ':') {
		lstr flag = lstr(remain_name.str, ptr_sub(remain_name.str, cur));
		
		remain_name.str = cur +1;
		remain_name.len -= flag.len +1;
		
		if (str::comp(flag, "WAIT")) {
			flags |= BT_WAIT;
		} else {
			warning("unknown flag '%' in event '%'", flag, name);
		}
	}
	
	*out_remain_name = remain_name;
	return flags;
}

DECL void add_str (lstr s, Block* out);

struct Gl_Buf {
	GLuint		VBO;
	GLuint		VAO;
	
	void init () {
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		
		auto vert_size = safe_cast_assert(GLsizei, sizeof(std140_Bar));
		auto gl_size = safe_cast_assert(GLsizeiptr, BLOCKS_PER_VBO * sizeof(std140_Bar));
		
		glBufferData(GL_ARRAY_BUFFER, gl_size, NULL, GL_STREAM_DRAW);
		
		glVertexAttribPointer(	0, 1, GL_FLOAT, GL_FALSE, vert_size,		(void*)offsetof(std140_Bar, pos_t) );
		glVertexAttribPointer(	1, 1, GL_FLOAT, GL_FALSE, vert_size,		(void*)offsetof(std140_Bar, len_t) );
		glVertexAttribIPointer( 2, 1, GL_INT, vert_size,					(void*)offsetof(std140_Bar, depth) );
		glVertexAttribIPointer( 3, 1, GL_UNSIGNED_INT, vert_size,			(void*)offsetof(std140_Bar, type_flags) );
		glVertexAttribIPointer( 4, 1, GL_INT, vert_size,					(void*)offsetof(std140_Bar, name_offs) );
		glVertexAttribIPointer( 5, 1, GL_INT, vert_size,					(void*)offsetof(std140_Bar, name_len) );
		glVertexAttribPointer(	6, 3, GL_UNSIGNED_BYTE, GL_TRUE, vert_size, (void*)offsetof(std140_Bar, col) );
		
		glVertexAttribDivisor(0, 1);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	void free () {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
	
};

struct Thread {
	f32					unit_to_sec;
	f32					sec_to_unit;
	
	lstr				name;
	
	dynarr<Gl_Buf>		gl_bufs;
	
	u32					buffered_count;
	
	dynarr<Block>		blocks;
	dynarr<u32>			prev_stk;
	u32					cur_level;
	
	void init (lstr name_, f32 sec_per_unit) {
		unit_to_sec = sec_per_unit;
		sec_to_unit = 1.0f / unit_to_sec;
		
		name = name_;
		
		gl_bufs.alloc(GL_BUF_CAP);
		buffered_count = 0;
		
		blocks.alloc(BLOCKS_DYNARR_CAP);
		prev_stk.alloc(LEVELS_DYNARR_CAP);
		
		cur_level = -1;
	}
	
	void free () {
		
		for (u32 i=0; i<gl_bufs.len; ++i) {
			gl_bufs[i].free();
		}
		gl_bufs.free();
		
		blocks.free();
		prev_stk.free();
	}
	
	void open (lstr name, u32 index, u64 ts) {
		auto n_i = blocks.len;
		auto* n = &blocks.append();
		
		n->parent =			-1;
		if (cur_level != -1) {
			u32 paren_i = prev_stk[cur_level];
			auto* paren = &blocks[paren_i];
			
			if (paren->children == -1) {
				paren->children = n_i;
			}
			
			n->parent =		paren_i;
		}
		
		n->children =		-1;
		n->next =			-1;
		
		if (prev_stk.len == ++cur_level) {
			prev_stk.append(); // no prev exists
		} else {
			auto prev_i = prev_stk[cur_level];
			auto* prev = &blocks[prev_i];
			
			prev->next =	n_i;
		}
		prev_stk[cur_level] = n_i;
		
		
		n->begin =			ts;
		n->length =			-1;
		n->depth =			cur_level;
		
		n->index =			index;
		
		{
			lstr remain_name;
			n->flags = get_flags_and_name(name, &remain_name);
			
			add_str(remain_name, n);
		}
		
	}
	void close (lstr name, u32 index, u64 ts) {
		assert(cur_level != -1 && prev_stk.len > cur_level); // close() without prior open()
		auto me_i = prev_stk[cur_level];
		auto* me = &blocks[me_i];
		
		assert(	prev_stk.len == cur_level +1 ||
				prev_stk.len == cur_level +2);
		if (prev_stk.len == cur_level +2) {
			prev_stk.pop();
		}
		
		--cur_level;
		
		me->length = ts -me->begin;
		assert(me->next == -1);
		
		{
			lstr remain_name;
			auto flags = get_flags_and_name(name, &remain_name);
			
			assert(str::comp(remain_name, me->get_name()));
			assert(flags == me->flags);
		}
		
		if (index != me->index) {
			warning("NOTE: block open and close for '%' have differing index data (open: % close: %)",
				name, me->index, index);
		}
		
	}
	void step (lstr name, u32 index, u64 ts) {
		auto n_i = blocks.len;
		auto* n = &blocks.append();
		
		assert(cur_level != -1 && prev_stk.len > cur_level); // step() without prior open()
		auto closee_i = prev_stk[cur_level];
		auto* closee = &blocks[closee_i];
		
		assert(	prev_stk.len == cur_level +1 ||
				prev_stk.len == cur_level +2);
		if (prev_stk.len == cur_level +2) {
			prev_stk.pop();
		}
		
		closee->length = ts -closee->begin;
		
		assert(str::comp(name, closee->get_name()));
		
		n->parent =			-1;
		if (cur_level > 0) {
			u32 paren_i = prev_stk[cur_level -1];
			auto* paren = &blocks[paren_i];
			
			assert(paren->children != -1); // step() can never come before open, so parent can never have us as first child
			
			n->parent =		paren_i;
		}
		
		n->children =		-1;
		n->next =			-1;
		
		closee->next =		n_i;
		prev_stk[cur_level] = n_i;
		
		n->begin =			ts;
		n->length =			-1;
		n->flags =			closee->flags;
		n->depth =			cur_level;
		n->name_offs =		closee->name_offs;
		n->name_len =		closee->name_len;
		n->index =			index;
		
	}
	
	#if 0
	template <typename FUNC>
	void _depth_first (FUNC f, Block& n, u32 depth) {
		
		f(n, depth);
		
		u32 c = n.children;
		while (c != 0) {
			auto& node = tree_storage[c];
			_depth_first(f, node, depth +1);
			
			c = node.next;
		}
	}
	
	template <typename FUNC>
	void traverse_depth_first (FUNC f) {
		u32 c = tree_storage[0].children;
		while (c != 0) {
			auto& node = tree_storage[c];
			_depth_first(f, node, 0);
			
			c = node.next;
		}
	}
	#endif
	
};

struct Str {
	u32	offs;
	u32	len;
};

DECLD u32				chunks_count;

DECLD dynarr<Thread>	threads; // zeroed
DECLD dynarr<char>		blocks_str_storage; // zeroed
DECLD dynarr<Str>		blocks_strs; // zeroed

DECLD char*				threads_str_tbl = nullptr;

DECLD u32				gl_str_tbl_size = 0;

DECL void add_str (lstr s, Block* out) {
	for (u32 i=0; i<blocks_strs.len; ++i) {
		lstr c = lstr(blocks_str_storage.arr +blocks_strs[i].offs, blocks_strs[i].len);
		
		if (str::comp(c, s)) {
			out->name_offs = blocks_strs[i].offs;
			out->name_len = blocks_strs[i].len;
			return; // already present
		}
	}
	char* n = blocks_str_storage.grow_by(s.len +1);
	
	assert(s.str[s.len] == '\0');
	cmemcpy(n, s.str, s.len +1);
	
	Str ns;
	ns.offs = safe_cast_assert( u32, ptr_sub(blocks_str_storage.arr, n) );
	ns.len = s.len;
	
	out->name_offs = ns.offs;
	out->name_len = ns.len;
	blocks_strs.append(ns);
}

DECLM lstr Block::get_name () {
	return lstr( blocks_str_storage.arr +name_offs, name_len );
}

#define C cast_v3< tv3<GLubyte> >

DECLD constexpr tv3<GLubyte> COLS[] = {
	{192,  26,  26},
	{ 26, 192,  26},
	{ 26,  26, 192},
	{192, 192,  26},
	{ 26, 192, 192},
	{192,  26, 192},
	{192,  77, 192},
	{128, 192,  77},
	
	C( v3(192,  26,	 26) * v3(0.8f) ),
	C( v3( 26, 192,	 26) * v3(0.8f) ),
	C( v3( 26,  26, 192) * v3(0.8f) ),
	C( v3(192, 192,	 26) * v3(0.8f) ),
	C( v3( 26, 192, 192) * v3(0.8f) ),
	C( v3(192,  26, 192) * v3(0.8f) ),
	C( v3(192,  77, 192) * v3(0.8f) ),
	C( v3(128, 192,	 77) * v3(0.8f) ),
	
	C( v3(192,  65,	 65) * v3(0.8f) ),
	C( v3( 65, 192,	 65) * v3(0.8f) ),
	C( v3( 65,  65, 192) * v3(0.8f) ),
	C( v3(192, 192,	 65) * v3(0.8f) ),
	C( v3( 65, 192, 192) * v3(0.8f) ),
	C( v3(192,  65, 192) * v3(0.8f) ),
	C( v3(192, 154, 192) * v3(0.8f) ),
	C( v3(128, 192,	154) * v3(0.8f) ),
};

#undef C

#include "streaming.hpp"
#include "hash.hpp"

DECLD streaming::Server		stream;

bool streamed () {
	return stream.connected;
}

DECLD u32				chunk_i;
DECLD f::Chunk			chunk;

DECLD f32				frame_view_sync_point;

DECLD u64				bytes_recieved_this_frame;
DECLD u32				chunks_this_frame;
DECLD u32				STREAM_MAX_CHUNKS_PER_FRAME =		32;

void profile_data_init () {
	
	chunk_i = 0;
	
	f::File_Header f_header;
	stream.read(&f_header, sizeof(f_header));
	
	assert(f_header.id.i == f::FILE_ID.i);
	
	u64 remain_header_size = (f_header.thread_count * sizeof(f::Thread)) +f_header.thr_name_str_tbl_size;
	
	assert((remain_header_size +sizeof(f_header)) <= f_header.file_size);
	
	DEFER_POP(&working_stk);
	byte* remain_header = working_stk.pushArr<byte>(remain_header_size);
	
	stream.read(remain_header, remain_header_size);
	
	if (streamed()) {
		assert(f_header.total_event_count == 0);
		assert(f_header.chunks_count == 0);
	}
	
	print("File_Header: file_size %	 total_event_count %  thread_count %  chunks_count %\n",
				f_header.file_size,
				f_header.total_event_count,
				f_header.thread_count,
				f_header.chunks_count );
	
	chunks_count = f_header.chunks_count;
	
	f::Thread*	f_threads = (f::Thread*)(remain_header);
	
	free(threads_str_tbl);
	threads_str_tbl = (char*)malloc(f_header.thr_name_str_tbl_size);
	cmemcpy(threads_str_tbl, f_threads +f_header.thread_count, f_header.thr_name_str_tbl_size);
	
	u32 threads_event_counter = 0;
	
	if (threads.arr) {
		for (u32 i=0; i<threads.len; ++i) {
			threads[i].free();
		}
	}
	threads.clear(0, THREADS_DYNARR_CAP);
	
	for (u32 i=0; i<f_header.thread_count; ++i) {
		assert(f_threads[i].name_tbl_offs < f_header.thr_name_str_tbl_size);
		lstr name = mlstr::count_cstr( threads_str_tbl +f_threads[i].name_tbl_offs );
		
		#if 1
		print("Thread % '%': sec_per_unit %	 units_per_sec %  event_count %\n",
				i, name,
				f_threads[i].sec_per_unit, 1.0f / f_threads[i].sec_per_unit,
				f_threads[i].event_count );
		#endif
		
		threads_event_counter += f_threads[i].event_count;
		
		auto* thr = &threads.append();
		thr->init(name, f_threads[i].sec_per_unit);
	}
	
	assert(threads_event_counter == f_header.total_event_count);
	
	blocks_str_storage.clear(0, THREADS_DYNARR_CAP);
	blocks_strs.clear		(0, BLOCKS_STR_TABLE_CAP);
	
}

void at_init () {
	
	winsock::init();
	
	stream.start();
}

void every_frame () {
	
	bytes_recieved_this_frame = 0;
	
	if (stream.client_pending()) {
		stream.connect_to_client();
		profile_data_init();
	} else {
		if (!stream.connected) {
			return;
		}
	}
	
	for (chunks_this_frame=0;; ++chunks_this_frame) {
		
		if (streamed()) {
			bool avail = stream.poll_read_avail();
			
			if ( !avail || chunks_this_frame == STREAM_MAX_CHUNKS_PER_FRAME ) break;
			++chunks_count;
		} else {
			if (chunk_i == chunks_count) break;
		}
		
		stream.read(&chunk, sizeof(chunk));
		
		DEFER_POP(&working_stk);
		
		assert(chunk.chunk_size > sizeof(chunk));
		assert(chunk.chunk_size <= gibi<u64>(1), "%", chunk.chunk_size);
		u64 size = chunk.chunk_size -sizeof(chunk);
		
		byte* data = working_stk.pushArr<byte>(size);
		stream.read(data, size);
		
		mlstr chunk_name = mlstr::count_cstr( (char*)data );
		
		if (0) {
			print(">>> '%' % size: % event_count: %\n",
					chunk_name, chunk.index, chunk.chunk_size, chunk.event_count);
		}
		
		threads[0].open(chunk_name, chunk.index, chunk.ts_begin); // TODO:
		
		auto* event = (f::Event*)(chunk_name.str +chunk_name.len +1);
		
		for (u32 i=0; i<chunk.event_count; ++i) {
			mlstr code_name = mlstr::count_cstr( (char*)(event +1) );
			
			assert(event->thread_indx < threads.len);
			auto* thr = &threads[ event->thread_indx ];
			
			assert(code_name.str[0] != '\0');
			
			char action =				code_name.str[0];
			lstr name =					lstr(&code_name.str[1], code_name.len -1);
			
			if (event->thread_indx == 1 && str::comp(code_name, "{engine_frame")) {
				frame_view_sync_point = (f32)event->ts * thr->unit_to_sec;
			}
			
			//print(" %  % % %\n", i, code_name, event->index, event->ts);
			
			switch (action) {
				case '{':	thr->open(name,		event->index, event->ts);		break;
				case '}':	thr->close(name,	event->index, event->ts);		break;
				case '|':	thr->step(name,		event->index, event->ts);		break;
				default: assert(false, "action : '%'", action);
			}
			
			event = (f::Event*)(code_name.str +code_name.len +1);
		}
		
		threads[0].close(chunk_name, chunk.index, chunk.ts_begin +chunk.ts_length); // TODO:
		
		//frame_view_sync_point = (f32)chunk.ts_begin * threads[0].unit_to_sec;
		
		++chunk_i;
		
		bytes_recieved_this_frame += chunk.chunk_size;
		
	}
	
	if (blocks_str_storage.len != gl_str_tbl_size) {
		glBindBuffer(GL_TEXTURE_BUFFER, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
		
		glBufferData(GL_TEXTURE_BUFFER,
				safe_cast_assert(GLsizeiptr, blocks_str_storage.len), blocks_str_storage.arr, GL_STREAM_DRAW);
		print("String table size: %\n", blocks_str_storage.len);
		
		gl_str_tbl_size = blocks_str_storage.len;
	}
	
	for (u32 thread_i=0; thread_i<threads.len; ++thread_i) {
		auto& thr = threads[thread_i];
		
		assert(thr.buffered_count <= thr.blocks.len);
		
		while (thr.buffered_count < thr.blocks.len) {
			u32 buf_i = thr.buffered_count / BLOCKS_PER_VBO;
			u32 offs = thr.buffered_count % BLOCKS_PER_VBO;
			
			u32 count = MIN( thr.blocks.len -thr.buffered_count, BLOCKS_PER_VBO -offs );
			
			assert(count > 0);
			if (buf_i == thr.gl_bufs.len) {
				assert(offs == 0);
				
				auto& buf = thr.gl_bufs.append();
				buf.init();
			}
			
			DEFER_POP(&working_stk);
			auto* data = working_stk.pushArr<std140_Bar>(count);
			u32 cur = 0;
			
			for (u32 i=thr.buffered_count; i<(thr.buffered_count +count); ++i) {
				Block& n = thr.blocks[i];
				
				#if 0
				print("%(%) %:%:%	% % %\n", repeat("  ", n.depth), i, n.begin, n.length, n.depth, n.name, n.index, (u32)n.flags);
				//Sleep(1);
				#endif
				
				data[cur] = {}; // zero potential padding
				
				lstr name = n.get_name();
				
				auto col = COLS[ (hash::hash(name) +n.index) % arrlenof(COLS) ];
				if ((n.flags & BT_WAIT) || (thread_i == 0 && n.depth == 0)) { // TODO:
					col = tv3<GLubyte>(120, 120, 120);
				}
				
				data[cur].pos_t.set( (f32)n.begin );
				data[cur].len_t.set( (f32)n.length );
				data[cur].depth.set( safe_cast_assert(GLint, n.depth) );
				data[cur].type_flags.set(n.flags);
				data[cur].name_offs.set(safe_cast_assert(GLint, n.name_offs));
				data[cur].name_len.set(safe_cast_assert(GLint, n.name_len));
				data[cur].col = col;
				
				++cur;
			}
			
			thr.buffered_count += count;
			
			glBindBuffer(GL_ARRAY_BUFFER, thr.gl_bufs[buf_i].VBO);
			glBufferSubData(GL_ARRAY_BUFFER,
					safe_cast_assert(GLintptr,		offs * sizeof(std140_Bar)),
					safe_cast_assert(GLsizeiptr,	count * sizeof(std140_Bar)),
					data);
			
		}
		
	}
	
}

DECLD f64				dt_exp_moving_avg;
DECLD constexpr f64		DT_EXP_MOVING_AVG_ALPHA =	1.0 / 8.0;

DECLD f64				thrp_exp_moving_avg;
DECLD constexpr f64		THRP_EXP_MOVING_AVG_ALPHA =	1.0 / 8.0;

DECLD bool				dragging_view = false;
DECLD f32				view_drag_grab_pos;

struct View_Pos {
	f32					pos_sec_to_pix; // sec-space pos of left screen edge
	f32					scale_sec_to_pix; // pixels per sec
};
DECLD View_Pos			view[2] = {
	{	0.0f,
		1920.0f / 2.0f,
	},
	{	0.0f,
		1920.0f / 0.020f,
	}
};

DECLD f32				select_pos =				0.0f; // sec
DECLD f32				diff_select_pos =			0.0f; // sec

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
		bool vsync_on = 1;
		
		auto ret = wglSwapIntervalEXT(vsync_on ? -1 : 0);
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
	
	at_init();
}

GLint get_thread_bars_offs (u32 thread_i) {
	return ((GLint)thread_i * (SHAD_BAR_OFFS * 8)) +SHAD_BAR_BASE_OFFS;
}

void draw_all_blocks_instanced (GLsizei gl_count) {
	for (u32 thread_i=0; thread_i<threads.len; ++thread_i) {
		auto& thr = threads[thread_i];
		
		{
			std140::float_ temp;
			temp.set(thr.unit_to_sec);
			
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
		
		//for (u32 buf_i=0; buf_i<thr.gl_bufs.len; ++buf_i)
		if (thr.gl_bufs.len > 0) {
			u32 buf_i = thr.gl_bufs.len-1;
			
			glBindVertexArray(thr.gl_bufs[buf_i].VAO);
			
			u32 count;
			if (buf_i == (thr.gl_bufs.len -1)) {
				count = thr.buffered_count % BLOCKS_PER_VBO;
				if (count == 0) count = BLOCKS_PER_VBO;
			} else {
				count = BLOCKS_PER_VBO;
			}
			
			assert(count != 0);
			
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, gl_count, safe_cast_assert(GLsizei, count));
		}
	}
	
}

void frame () {
	
	every_frame();
	
	if (0) { // Artificially slow down to test stuff
		Sleep(333);
	}
	
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
	
	f32 view_pos;
	f32 view_scale;
	{
		auto& v = view[streamed() && chunk_i != 0 ? 1 : 0];
		
		f32 dragged_view_pos;
		{
			f32 mouse_pos_pixel_center = (f32)sinp.mouse_cursor_pos.x +0.5f;
			f32 old_view_scale = v.scale_sec_to_pix; // time-unit-space, but relative to left screen edge
			
			if (sinp.mouse_cursor_in_window) {
				f32 log_space = fp::log(v.scale_sec_to_pix);
				log_space += (f32)inp.mouse_wheel_accum * 0.0625f;
				v.scale_sec_to_pix = fp::exp(log_space);
			}
			view_scale = v.scale_sec_to_pix;
			
			f32 new_scaled_mouse_pos = mouse_pos_pixel_center / v.scale_sec_to_pix; // time-unit-space, but relative to left screen edge
			
			dragged_view_pos = v.pos_sec_to_pix;
			
			if (!dragging_view) {
				if (inp.rmb_down && sinp.mouse_cursor_in_window) {
					//assert(inp.rmb_count > 0); // button went down
					
					view_drag_grab_pos = new_scaled_mouse_pos;
					dragging_view = true;
				} else {
					f32 old_scaled_mouse_pos = mouse_pos_pixel_center / old_view_scale; // time-unit-space, but relative to left screen edge
					dragged_view_pos -= new_scaled_mouse_pos -old_scaled_mouse_pos; // make scaling centered around mouse position
					v.pos_sec_to_pix = dragged_view_pos;
				}
			} else {
				dragged_view_pos -= new_scaled_mouse_pos -view_drag_grab_pos;
				if (!inp.rmb_down) {
					assert(inp.rmb_count > 0); // button went up
					
					v.pos_sec_to_pix = dragged_view_pos; // apply the dragging
					dragging_view = false;
				}
			}
		}
		view_pos = dragged_view_pos;
		
		if (streamed() && chunk_i != 0) {
			view_pos = view_pos +frame_view_sync_point; // TODO:
		}
		
	}
	
	{
		std140_View temp;
		temp.view_pos_sec_to_pix.set( view_pos );
		temp.view_scale_sec_to_pix.set( view_scale );
		
		glBufferSubData(GL_UNIFORM_BUFFER,
				offsetof(std140_Global, view_pos_sec_to_pix),
				sizeof temp, &temp);
	}
	
	{
		if (inp.lmb_down && sinp.mouse_cursor_in_window) {
			auto pos = view_pos +((sinp.mouse_cursor_pos.x +0.5f) / view_scale); // click pixel center
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
	draw_all_blocks_instanced(8);
	
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
	
	glBindBuffer(GL_TEXTURE_BUFFER, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R8I, VBOs[VBO_BUF_TEX_BAR_NAME_STR_TBL]);
	
	draw_all_blocks_instanced(4);
	
	// Bars duration text
	glUseProgram(shaders[PROG_BAR_DURATION_TEXT]);
	draw_all_blocks_instanced(4);
	
	glUseProgram(shaders[PROG_VERT_LINE]);
	{
		std140::float_ temp = {};
		auto draw_vert_line = [&] (f32 pos_sec) {
			temp.set(pos_sec);
			glBufferSubData(GL_UNIFORM_BUFFER,
					offsetof(std140_Global, vert_line_pos),
					sizeof temp, &temp);
			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
		};
		
		draw_vert_line(0);
		draw_vert_line(select_pos);
		draw_vert_line(diff_select_pos);
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
		
		for (u32 i=0; i<threads.len; ++i) {
			DEFER_POP(&working_stk);
			
			GLint y = get_thread_bars_offs(i) -(1 * SHAD_BAR_OFFS);
			draw_text_line(tv2<GLint>(0, y +8),
					"----------------------------------------------------------------------"
					"----------------------------------------------------------------------"
					"----------------------------------------------------------------------"
					"----------------------------------------------------------------------"
					"----------------------------------------------------------------------"
					"----------------------------------------------------------------------"
					);
			draw_text_line(tv2<GLint>(0, y +line_height +8),
				print_working_stk("%    VBOs: %", threads[i].name, threads[i].gl_bufs.len) );
		}
	}
	
	{ // Markers text
		DEFER_POP(&working_stk);
		
		{
			GLint x = (GLint)((0.0f -view_pos) * view_scale) +4;
			
			draw_text_line(tv2<GLint>(x, lowerleft_lines_y(0) +4), "0 ms");
		}
		{
			auto t = units::Time(select_pos);
			
			lstr str = print_working_stk("%%", t.val,t.unit);
			
			GLint x = (GLint)((select_pos -view_pos) * view_scale) +4;
			
			draw_text_line(tv2<GLint>(x, lowerleft_lines_y(1) +4), str);
		}
		{
			auto t = units::Time(diff_select_pos);
			
			lstr str = print_working_stk("%%", t.val,t.unit);
			
			GLint x = (GLint)((diff_select_pos -view_pos) * view_scale);
			x -= (safe_cast_assert(GLint, str.len) * GLYPH_DIM.x) +4;
			
			draw_text_line(tv2<GLint>(x, lowerleft_lines_y(2) +4), str);
		}
		{
			auto diff = select_pos -diff_select_pos;
			
			auto t = units::Time(diff);
			
			lstr str = print_working_stk("%%", t.val,t.unit);
			
			GLint x = (GLint)((diff_select_pos -view_pos) * view_scale) +4;
			
			draw_text_line(tv2<GLint>(x, lowerleft_lines_y(2) +4), str);
		}
	}
	
	{ // Graph tool frame timings text draw
		
		{
			if (frame_number == 1) {
				dt_exp_moving_avg = time::cpu_qpc_prev_frame_ms;
			} else if (frame_number > 1) {
				dt_exp_moving_avg = (time::cpu_qpc_prev_frame_ms * DT_EXP_MOVING_AVG_ALPHA)
						+(dt_exp_moving_avg * (1.0 -DT_EXP_MOVING_AVG_ALPHA));
			}
			
			if (frame_number == 0) {
				thrp_exp_moving_avg = (f64)bytes_recieved_this_frame;
			} else if (frame_number > 0) {
				thrp_exp_moving_avg = ((f64)bytes_recieved_this_frame * THRP_EXP_MOVING_AVG_ALPHA)
						+(thrp_exp_moving_avg * (1.0 -THRP_EXP_MOVING_AVG_ALPHA));
			}
			units::Throughput thrp(thrp_exp_moving_avg);
			
			u64 blocks = 0;
			for (u32 i=0; i<threads.len; ++i) {
				blocks += threads[i].buffered_count;
			}
			units::Bytes vbo_b(blocks * sizeof(std140_Bar));
			
			lstr str = print_working_stk("frame#%, prev_dt: %|% ms  %% |% chunks: % total blocks: % VBO size: %%",
						frame_number, dt_exp_moving_avg, time::cpu_qpc_prev_frame_ms,
						thrp.val,thrp.unit, bytes_recieved_this_frame, chunks_this_frame, blocks, vbo_b.val,vbo_b.unit);
			
			//print(">>> %\n", str);
			
			draw_text_line(upperleft_lines(0), str);
		}
		
		{
			lstr str;
			if (!stream.connected) {
				str = "not connected";
			} else {
				str = print_working_stk("connected to client [%.%.%.%:%]", _IP_PRINT(stream.client_ip));
			}
			draw_text_line(upperleft_lines(1), str);
		}
	}
	
}

////
}
