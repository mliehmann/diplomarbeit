/* per-machine configuration. this file is automatically generated. */

#ifndef _ODE_CONFIG_H_
#define _ODE_CONFIG_H_

/* shared lib definitions */
#ifndef SHAREDLIBIMPORT
#define SHAREDLIBIMPORT
#endif
#ifndef SHAREDLIBEXPORT
#define SHAREDLIBEXPORT
#endif

/* standard system headers */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* is this a pentium on a gcc-based platform? */
/* #define PENTIUM 1 -- not a pentium */

/* integer types (we assume int >= 32 bits) */
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

/* an integer type that we can safely cast a pointer to and
 * from without loss of bits.
 */
typedef unsigned int intP;

/* select the base floating point type */
#define dSINGLE 1

/* the floating point infinity */
#define DINFINITY_DECL SHAREDLIBEXPORT union dInfBytes dInfinityValue = {{0,0,0x80,0x7f}};
#if defined SHARED_CONFIG_H_INCLUDED_FROM_DEFINING_FILE
  #define GLOBAL_SHAREDLIB_SPEC SHAREDLIBEXPORT
#else 
  #define GLOBAL_SHAREDLIB_SPEC SHAREDLIBIMPORT
#endif
union dInfBytes { unsigned char c[4]; float f; };
extern GLOBAL_SHAREDLIB_SPEC union dInfBytes dInfinityValue;
#define dInfinity (dInfinityValue.f)
#undef GLOBAL_SHAREDLIB_SPEC

/* ODE feature configuration */

#ifdef __cplusplus
}
#endif
#endif