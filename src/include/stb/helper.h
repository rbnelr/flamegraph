
namespace stb {
	#define STB_IMAGE_IMPLEMENTATION 1
	#define STB_IMAGE_STATIC 1
	#define STBI_NO_STDIO 1
	#define STBI_ASSERT(cond)		assert(cond)
	#define STBI_MALLOC(sz)			malloc(sz)
	#define STBI_REALLOC(p, newsz)	realloc(p, newsz)
	#define STBI_FREE(p)			free(p)
	
	//#define STBI_NO_LINEAR 1
	
	
	#define STBI_ONLY_JPEG	1
	#define STBI_ONLY_HDR	1
	
	#define STBI__VERTICALLY_FLIP_ON_LOAD_INIT 1
	
	
	#pragma push_macro("cr")
	#undef cr
	
	#include "stb/stb_image_modified.h"
	
	#pragma pop_macro("cr")
}
