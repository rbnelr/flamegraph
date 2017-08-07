// This includes the actual windows.h and tries to contain some of it's #defines

#pragma push_macro("interface")
#pragma push_macro("min")
#pragma push_macro("MIN")
#pragma push_macro("max")
#pragma push_macro("MAX")
#pragma push_macro("FORCEINLINE")
#pragma push_macro("PURE")
#pragma push_macro("OUT")
#pragma push_macro("far")
#pragma push_macro("near")
#pragma push_macro("pascal")

#define noexcept

#define UINT _UINT

// Without WIN32_LEAN_AND_MEAN: ogl.cpp preprocessed:	4,149,355 bytes; 299056 lines
// With WIN32_LEAN_AND_MEAN: ogl.cpp preprocessed:		2,846,833 bytes; 194215 lines
#define WIN32_LEAN_AND_MEAN 1

#if _COMP == _COMP_MSVC
	WARNINGS_PUSH
	
	WARNING_DISABLE(4005) // macro redefinition
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
//#include <iphlpapi.h>

#include "windows.h"

#if _COMP == _COMP_MSVC
	WARNINGS_POP
#endif

#undef UINT

#pragma pop_macro("interface")
#pragma pop_macro("min")
#pragma pop_macro("MIN")
#pragma pop_macro("max")
#pragma pop_macro("MAX")
#pragma pop_macro("FORCEINLINE")
#pragma pop_macro("PURE")
#pragma pop_macro("OUT")
#pragma pop_macro("far")
#pragma pop_macro("near")
#pragma pop_macro("pascal")

#undef noexcept

#undef SHORT
#undef INT
#undef LONG
#undef USHORT
#undef UINT
#undef ULONG

typedef short			WIN_H_SHORT;
typedef int				WIN_H_INT;
typedef long			WIN_H_LONG;
typedef unsigned short	WIN_H_USHORT;
typedef unsigned int	WIN_H_UINT;
typedef unsigned long	WIN_H_ULONG;
