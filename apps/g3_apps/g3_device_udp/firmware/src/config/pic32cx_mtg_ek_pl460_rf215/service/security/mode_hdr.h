/*
---------------------------------------------------------------------------
Copyright (c) 1998-2010, Brian Gladman, Worcester, UK. All rights reserved.

The redistribution and use of this software (with or without changes)
is allowed without the payment of fees or royalties provided that:

  source code distributions include the above copyright notice, this
  list of conditions and the following disclaimer;

  binary distributions include the above copyright notice, this list
  of conditions and the following disclaimer in their documentation.

This software is provided 'as is' with no explicit or implied warranties
in respect of its operation, including, but not limited to, correctness
and fitness for purpose.
---------------------------------------------------------------------------
Issue Date: 07/10/2010

This header file is an INTERNAL file which supports mode implementation
*/

#ifndef _MODE_HDR_H
#define _MODE_HDR_H

#include <string.h>
#include <limits.h>

#include "brg_endian.h"

/*  This define sets the units in which buffers are processed.  This code
    can provide significant speed gains if buffers can be processed in
    32 or 64 bit chunks rather than in bytes.  This define sets the units
    in which buffers will be accessed if possible
*/
#if !defined( UNIT_BITS )
#  if PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN
#    if 0
#      define UNIT_BITS  32
#    elif 1
#      define UNIT_BITS  64
#    endif
#  elif defined( _WIN64 )
#    define UNIT_BITS 64
#  else
#    define UNIT_BITS 32
#  endif
#endif

#if UNIT_BITS == 64 && !defined( NEED_UINT_64T )
#  define NEED_UINT_64T
#endif

#include "brg_types.h"

/*  Use of inlines is preferred but code blocks can also be expanded inline
    using 'defines'.  But the latter approach will typically generate a LOT
    of code and is not recommended.
*/
#if 1 && !defined( USE_INLINING )
#  define USE_INLINING
#endif

#if defined( _MSC_VER )
#  if _MSC_VER >= 1400
#    include <stdlib.h>
#    include <intrin.h>
#    pragma intrinsic(memset)
#    pragma intrinsic(memcpy)
#    define rotl32        _rotl
#    define rotr32        _rotr
#    define rotl64        _rotl64
#    define rotr64        _rotl64
#    define bswap_16(x)   _byteswap_ushort(x)
#    define bswap_32(x)   _byteswap_ulong(x)
#    define bswap_64(x)   _byteswap_uint64(x)
#  else
#    define rotl32 _lrotl
#    define rotr32 _lrotr
#  endif
#endif

#if defined( USE_INLINING )
#  if defined( _MSC_VER )
#    define mh_decl __inline
#  elif defined( __GNUC__ ) || defined( __GNU_LIBRARY__ )
#    define mh_decl static inline
#  else
#    define mh_decl static
#  endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define  UI8_PTR(x)     UPTR_CAST(x,  8)
#define UI16_PTR(x)     UPTR_CAST(x, 16)
#define UI32_PTR(x)     UPTR_CAST(x, 32)
#define UI64_PTR(x)     UPTR_CAST(x, 64)
#define UNIT_PTR(x)     UPTR_CAST(x, UNIT_BITS)

#define  UI8_VAL(x)     UNIT_CAST(x,  8)
#define UI16_VAL(x)     UNIT_CAST(x, 16)
#define UI32_VAL(x)     UNIT_CAST(x, 32)
#define UI64_VAL(x)     UNIT_CAST(x, 64)
#define UNIT_VAL(x)     UNIT_CAST(x, UNIT_BITS)

#define BUF_INC          (UNIT_BITS >> 3)
#define BUF_ADRMASK     ((UNIT_BITS >> 3) - 1)

#define rep2_u2(f,r,x)    f( 0,r,x); f( 1,r,x)
#define rep2_u4(f,r,x)    f( 0,r,x); f( 1,r,x); f( 2,r,x); f( 3,r,x)
#define rep2_u16(f,r,x)   f( 0,r,x); f( 1,r,x); f( 2,r,x); f( 3,r,x); \
                          f( 4,r,x); f( 5,r,x); f( 6,r,x); f( 7,r,x); \
                          f( 8,r,x); f( 9,r,x); f(10,r,x); f(11,r,x); \
                          f(12,r,x); f(13,r,x); f(14,r,x); f(15,r,x)

#define rep2_d2(f,r,x)    f( 1,r,x); f( 0,r,x)
#define rep2_d4(f,r,x)    f( 3,r,x); f( 2,r,x); f( 1,r,x); f( 0,r,x)
#define rep2_d16(f,r,x)   f(15,r,x); f(14,r,x); f(13,r,x); f(12,r,x); \
                          f(11,r,x); f(10,r,x); f( 9,r,x); f( 8,r,x); \
                          f( 7,r,x); f( 6,r,x); f( 5,r,x); f( 4,r,x); \
                          f( 3,r,x); f( 2,r,x); f( 1,r,x); f( 0,r,x)

#define rep3_u2(f,r,x,y,c)  f( 0,r,x,y,c); f( 1,r,x,y,c)
#define rep3_u4(f,r,x,y,c)  f( 0,r,x,y,c); f( 1,r,x,y,c); f( 2,r,x,y,c); f( 3,r,x,y,c)
#define rep3_u16(f,r,x,y,c) f( 0,r,x,y,c); f( 1,r,x,y,c); f( 2,r,x,y,c); f( 3,r,x,y,c); \
                            f( 4,r,x,y,c); f( 5,r,x,y,c); f( 6,r,x,y,c); f( 7,r,x,y,c); \
                            f( 8,r,x,y,c); f( 9,r,x,y,c); f(10,r,x,y,c); f(11,r,x,y,c); \
                            f(12,r,x,y,c); f(13,r,x,y,c); f(14,r,x,y,c); f(15,r,x,y,c)

#define rep3_d2(f,r,x,y,c)  f( 1,r,x,y,c); f( 0,r,x,y,c)
#define rep3_d4(f,r,x,y,c)  f( 3,r,x,y,c); f( 2,r,x,y,c); f( 1,r,x,y,c); f( 0,r,x,y,c)
#define rep3_d16(f,r,x,y,c) f(15,r,x,y,c); f(14,r,x,y,c); f(13,r,x,y,c); f(12,r,x,y,c); \
                            f(11,r,x,y,c); f(10,r,x,y,c); f( 9,r,x,y,c); f( 8,r,x,y,c); \
                            f( 7,r,x,y,c); f( 6,r,x,y,c); f( 5,r,x,y,c); f( 4,r,x,y,c); \
                            f( 3,r,x,y,c); f( 2,r,x,y,c); f( 1,r,x,y,c); f( 0,r,x,y,c)

    /* function pointers might be used for fast XOR operations */

    typedef void (*xor_function)(void* r, const void* p, const void* q);

    /* support for fast aligned buffer move, xor and byte swap operations -
       source and destination buffers for move and xor operations must not
       overlap, those for byte order revesal must either not overlap or
       must be identical
    */
#define f_copy(n,p,q)     p[n] = q[n]
#define f_xor(n,r,p,q,c)  r[n] = c(p[n] ^ q[n])

    mh_decl void xor_block(void *r, const void* p, const void* q)
    {
        rep3_u16(f_xor, UI8_PTR(r), UI8_PTR(p), UI8_PTR(q), UI8_VAL);
    }

    mh_decl void xor_block_aligned(void *r, const void *p, const void *q)
    {
#if UNIT_BITS == 8
        rep3_u16(f_xor, UNIT_PTR(r), UNIT_PTR(p), UNIT_PTR(q), UNIT_VAL);
#elif UNIT_BITS == 32
        rep3_u4(f_xor, UNIT_PTR(r), UNIT_PTR(p), UNIT_PTR(q), UNIT_VAL);
#else
        rep3_u2(f_xor, UNIT_PTR(r), UNIT_PTR(p), UNIT_PTR(q), UNIT_VAL);
#endif
    }

    /* platform byte order to big or little endian order for 16, 32 and 64 bit variables */

#if PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN

#  define uint_16t_to_le(x) (x) = bswap_16((x))
#  define uint_32t_to_le(x) (x) = bswap_32((x))
#  define uint_64t_to_le(x) (x) = bswap_64((x))
#  define uint_16t_to_be(x)
#  define uint_32t_to_be(x)
#  define uint_64t_to_be(x)

#else

#  define uint_16t_to_le(x)
#  define uint_32t_to_le(x)
#  define uint_64t_to_le(x)
#  define uint_16t_to_be(x) (x) = bswap_16((x))
#  define uint_32t_to_be(x) (x) = bswap_32((x))
#  define uint_64t_to_be(x) (x) = bswap_64((x))

#endif

#if defined(__cplusplus)
}
#endif

#endif
