/* 
 * $Id: common.h 239 2008-02-04 19:20:46Z alexios $
 * 
 * common.h - Interface to common.c
 * 
 * Copyright (C) 2002-2004 Alexios Chouchoulas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
 * 
 */

#ifndef _COMMON_H
#define _COMMON_H


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <stdlib.h>
#include "../config.h"


#define abstract = NULL


#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#ifdef __cplusplus
typedef signed int    int32_t;
typedef signed short  int16_t;
typedef signed char   int8_t;
#endif // __cplusplus
#endif // HAVE_STDINT_H

typedef int bool_t;



#ifndef SWIG


#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif // max

#ifndef between
#define between(x,a,b) (((x)>=(a)) && ((x)<=(b)))
#endif // between




#ifndef HAVE_SNPRINTF
#warning "This system lacks snprintf. Will simulate using sprintf (bletch)."
#define snprintf(s,n,f...) sprintf(s,##f)
#endif


#define get_libdir(fname) "../data/"fname

#define FILE_COPYING "COPYING"
#define FILE_ABOUT   "about.rtxt"



#endif // SWIG


#ifdef SWIG
#define SWIG_ARRAY(x) x##_array
#define CHAR_SWIG_ARRAY(x) char
#else
#define SWIG_ARRAY(x) x
#endif //



void random_init (void);

uint32_t write_magic (FILE *fp, const char * magic);

uint32_t check_magic (FILE *fp, const char * magic);

/*
#define bytes_between(first,last) \
(uint32_t) (((char *)(last)) - (char*)(first));
*/


extern int debugging_assertions_enabled;

extern void __my_assert_fail (const char *, const char *, unsigned int, const char *);

#ifdef NDEBUG
#include <assert.h>
#ifdef assert
#undef assert
#endif // assert
#define assert(expr) \
  (__ASSERT_VOID_CAST ((expr) ? 0 :					      \
		       (__my_assert_fail (__STRING(expr)"\r", __FILE__, __LINE__,    \
				       __FUNCTION__), 0)))
#endif // NDEBUG



#ifndef SWIG
#ifdef __cplusplus
};
#endif // __cplusplus
#endif // SWIG

#endif /* _COMMON_H */
