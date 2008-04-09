#ifndef __tydefs_h__
#define __tydefs_h__

  /*
   *  +--------------------------------------------------------------+
   *  |                     Paul Scherrer Institut                   |
   *  |                       Computing Division                     |
   *  |                                                              |
   *  | This software may be used freely by non-profit organizations.|
   *  | It may be copied provided that the name of P.S.I. and of the |
   *  | author is included. Neither P.S.I. nor the author assume any |
   *  | responsibility for the use of this software outside of P.S.I.|
   *  +--------------------------------------------------------------+
   *
   *  Project  . . . . . . . . . . :  Musr support software
   *  Component/Facility . . . . . :  Define basic data types and functions
   *  File Name  . . . . . . . . . :  tydefs.h
   *  Title  . . . . . . . . . . . :  
   *  Abstract . . . . . . . . . . :
   *
   *
   *  Author . . . . . . . . . . . :  RA84
   *  Date of creation . . . . . . :  March 1998
   *
   *  Date        Name        Modification
   *  -----------------------------------------------------------------
   *  04-MAY-1998 RA84        Header indluded
   *  22-MAY-2002 RA95        && !defined(OS_OSF1) added for TRU64
   *  17-NOV-2005 RA36
   */

/* --------------------------------------------------------------------- */

/* --- OpenVMS (DECC or VAXC),(AXP or VAX) --- */
#ifdef _WIN32
#define INT64_SUPPORT
#endif

#if ((defined(__DECC) || defined(__VAXC)) && !defined(unix) && !defined(OS_OSF1))
#if defined (__ALPHA)

#define MODEFS_CC_SYS "Compiled for VAXC or DECC OpenVMS ALPHA"

typedef                 int      Int16;
typedef  unsigned       int     UInt16;  
typedef            long int      Int32;    
typedef  unsigned  long int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#else  /* !defined( __ALPHA) */

#define MODEFS_CC_SYS "Compiled for VAXC or DECC OpenVMS VAX"

typedef           short int      Int16;
typedef  unsigned short int     UInt16;  
typedef                 int      Int32;    
typedef  unsigned       int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#endif /* #else __ALPHA */

#define VMS_SUCCESS   1
#define VMS_ERROR     2


/* --- DEC UNIX or OFS/1 (AXP or else) --- */

#elif defined (__osf__)

#if defined (__alpha)
#define MODEFS_CC_SYS "Compiled for (DEC) OSF/1 or UNIX Alpha"
typedef           short int      Int16;
typedef  unsigned short int     UInt16;  
typedef                 int      Int32;    
typedef  unsigned       int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#else /* !defined ( __alpha) */

#define MODEFS_CC_SYS "Compiled for (DEC) OSF/1 or UNIX NON Alpha"

typedef                 int      Int16;
typedef  unsigned       int     UInt16;  
typedef            long int      Int32;    
typedef  unsigned  long int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#endif /* #else __alpha */


/* --- other operating system --- */

#else /* other operating system */

//#ifdef _WIN32
#if (defined(_WIN32) || defined(__linux__))
#define MODEFS_CC_SYS "Compiled for Microsoft Windows 32-bit or Linux operating system"

typedef                 short    Int16;
typedef  unsigned       short   UInt16;  
typedef                 int      Int32;    
typedef  unsigned       int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INT64_SUPPORT
#define INT64_SUPPORTED
typedef            __int64       Int64;    
typedef  unsigned  __int64      UInt64;   
typedef                 Int64 *   Int64Ptr;    
typedef                UInt64 *  UInt64Ptr;   
#endif

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#else

#define MODEFS_CC_SYS "Compiled for other (#else) operating system"

typedef                 int      Int16;
typedef  unsigned       int     UInt16;  
typedef            long int      Int32;    
typedef  unsigned  long int     UInt32;   

typedef                 Int16 *   Int16Ptr;
typedef                UInt16 *  UInt16Ptr;  
typedef                 Int32 *   Int32Ptr;    
typedef                UInt32 *  UInt32Ptr;   

#ifdef INT64_SUPPORT
#define INT64_SUPPORTED
//typedef            long int      Int64;    
//typedef  unsigned  long int     UInt64;   
typedef                 Int64 *   Int64Ptr;    
typedef                UInt64 *  UInt64Ptr;   
#endif

#ifdef INCLUDE_CHARDEFS
typedef                 char    Char;     
typedef                 char  * CharPtr;
typedef                 char  * Str;
#endif

#ifdef INCLUDE_BOOLDEF
typedef  unsigned       char    BoolEnum;
#endif

#endif

#endif

/* --------------------------------------------------------------------- */
/* not operating system specific (does not have to be portable) */

typedef                 float   Float;
typedef                 double  Double;
typedef                 float   Float32;
typedef                 double  Float64;

#ifdef FLOAT128_SUPPORT
#define FLOAT128_SUPPORTED
typedef                 double  Float128;
#endif

/* --------------------------------------------------------------------- */

#define MAXUINT16    0xffff
#define MAXUINT32    0xffffffff
#ifdef INT64_SUPPORT
#define MAXUINT64    0xffffffffffffffff
#endif

#define MAXINT16     0x7fff
#define MININT16     (-MAXINT16)
#define MAXINT32     0x7fffffff
#define MININT32     (-MAXINT32)
#ifdef INT64_SUPPORT
#define MAXINT64     0x7fffffffffffffff
#define MININT64     (-MAXINT64)
#endif

/* --------------------------------------------------------------------- */

#define BOOL_TRUE    1
#define BOOL_FALSE   0

#define ERRORRET  -1
#define MOSUCCESS  0
#define MOERROR    1

/* --------------------------------------------------------------------- */

#define MOMESSAGEMAX  255

/* --------------------------------------------------------------------- */

#define SPRINTF sprintf
#define PRINTF  printf
#define SSCANF  sscanf
#define STRLEN  strlen
#define STRCMP  strcmp
#define STRCPY  strcpy
#define STRCAT  strcat
#define STRNCPY strncpy
#define STRNCMP strncmp
#define ATOL    atol
#define MEMCPY  memcpy
#define MALLOCF malloc
#define FREEF   free
#define FOPENR  fopen
#define FCLOSER fclose
#define FWRITER fwrite

/* --------------------------------------------------------------------- */

#endif  /* __modefs_h__ */
