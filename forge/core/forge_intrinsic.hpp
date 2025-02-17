#pragma once

// to ensure rp malloc overrides all new and delete operators
#include <rpmalloc/rpnew.h>

// defs that are intrinsic to the byte forge engine and will automatically be included in the build system

typedef unsigned char u8;
typedef signed char i8;
typedef signed short int i16;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef signed int i32;
typedef float f32;
typedef double f64;

#if __WORDSIZE == 64
typedef signed long int i64;
typedef unsigned long int u64;
#else
__extension__ typedef signed long long int i64;
__extension__ typedef unsigned long long int u64;
#endif

// the following are macro definitions for the reflection system that i will implement soon and will be used by the header parser tool
#define CLASS(...)
#define ENUM(...)
#define PROPERTY(...)
#define FUNCTION(...)