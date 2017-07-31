#pragma once

/*
	Things that exist to deal with c/c++ -es types goes here
*/

#include "stdint.h"

// 
typedef int						si;
typedef unsigned int			ui;

// 
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned long			ulong;
typedef unsigned long long		ullong;
typedef signed char				schar;
typedef signed short			sshort;
typedef signed long				slong;
typedef signed long long		sllong;

// explicit size integers
typedef uint8_t					u8;
typedef uint16_t				u16;
typedef uint32_t				u32;
typedef uint64_t				u64;
typedef int8_t					s8;
typedef int16_t					s16;
typedef int32_t					s32;
typedef int64_t					s64;

// explicit size floating point
typedef float					f32;
typedef double					f64;

static_assert(sizeof(f32) == 4, "sizeof(f32) not 4");
static_assert(sizeof(f64) == 8, "sizeof(f64) not 8");

// byte and char type
static_assert(sizeof(char) == 1, "sizeof(char) not 1");
typedef char					byte; // Use when dealing with raw memory to be a little more explicit about usage than just using a char or uchar pointer

// pointer size typedefs
static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "sizeof(void*) not 4 or 8");
static_assert(sizeof(uintptr_t) == sizeof(void*), "sizeof(uintptr_t) not sizeof(void*)");
static_assert(sizeof(intptr_t) == sizeof(void*), "sizeof(intptr_t) not sizeof(void*)");
typedef uintptr_t				uptr; // Use when dealing with size in bytes or offsets or for calculating pointers
typedef intptr_t				sptr;

static_assert(sizeof(size_t) == sizeof(uptr), "sizeof(size_t) not sizeof(uptr)");

// wide char
typedef wchar_t					wchar;

//#ifndef NULL
//	#define NULL 0
//#endif

// const reference macro for brevity
#define cr const &

typedef char const*				cstr;
