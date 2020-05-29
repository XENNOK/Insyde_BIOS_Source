#ifndef _STD_C_LIB_SUPPORT_H
#define _STD_C_LIB_SUPPORT_H

#include "Tiano.h"


#define va_list   VA_LIST
#define va_arg    VA_ARG
#define va_start  VA_START
#define va_end    VA_END

#define CHAR_BIT     8   /* number of bits in a char */
#define INT_MAX      2147483647       /* max value for an int */
#define INT_MIN      (-2147483647-1)  /* min value for an int */
#define UINT_MAX     0xffffffff  /* max value for an unsigned int */
#define ULONG_MAX    0xffffffff  /* max value for an unsigned long */
#define SHRT_MAX     32767/* max value for a short */
#define LONG_MAX     2147483647  /* max value for a long */
#define LONG_MIN     (-2147483647-1) /* min value for a long */


#define EOF          (-1)

typedef unsigned int  u_int32_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned short wchar_t;
typedef UINTN size_t;

#ifdef EFI32

typedef struct {
    u_int32_t ebx;
    u_int32_t esi;
    u_int32_t edi;
    u_int32_t ebp;
    u_int32_t esp;
    u_int32_t eip;
    u_int32_t fpucw;
} jmp_buf[1];
#endif


#ifdef EFIX64

typedef struct {
    u_int64_t rbx;
    u_int64_t rbp;
    u_int64_t r12;
    u_int64_t r13;
    u_int64_t r14;
    u_int64_t r15;
    u_int64_t rsp;
    u_int64_t rip;
    u_int64_t rdi;
    u_int64_t rsi;
} jmp_buf[1];

#endif

//
// ctype.h
//
int tolower(int c);
int isdigit(int c);
int isalpha(int c);
int isupper(int c);
int isscii(int c);
int isspace(int c);

//
// setjmp.h
//
int  setjmp( jmp_buf env );
void longjmp ( jmp_buf env, int val );

//
// stdlib.h
//
int  abs(register int i);
void *calloc (size_t, size_t);
//void exit(int status);
void  free (void *);
//long labs(long j);
void *malloc (size_t);
void qsort (void *base, size_t num, size_t width, int (*compare)(const void *, const void *));
void *realloc (void *, size_t);

//
// string.h
//
void *memchr (const void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void *memcpy (void *, const void *, size_t);
void *memmove (void *, const void *, size_t size);
void *memset (void *, int, size_t size);
char *strcat (char *, const char *);
int  strcmp (const char *, const char *);
char *strcpy (char *, const char *);
size_t  strlen (const char *);
int strncmp (const char *, const char *, size_t);
char *strncpy (char *, const char *, size_t);
char *strrchr (const char *, int);
char *strstr (const char *, const char *);
int   strcmpi ( const char * str1, const char * str2 );
char *strdup(const char *s1);

//
// wchar.h
//
wchar_t * wcsdup(wchar_t *str);
unsigned long wcstoul(wchar_t *nptr, wchar_t **endptr , unsigned int base );
long wcstol(wchar_t  *nptr, wchar_t  **endptr , int base );
int wcscasecmp (wchar_t *s1, wchar_t *s2);
wchar_t *wcscpy (wchar_t *to, const wchar_t *from);
size_t wcslen(const wchar_t *str);
wchar_t *wcscat(wchar_t *s, const wchar_t *append);
int wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcsncpy(wchar_t *dst, const wchar_t * src,size_t n);

#if EFI64 || EFIX64
#define ptrdiff_t __int64     /* ptr1 - ptr2 */
#else
#define ptrdiff_t int         /* ptr1 - ptr2 */
#endif

#define assert(exp) ASSERT(exp)
#define offsetof(type, member) ( (int) & ((type*)0) -> member)
#define swprintf SPrint

#endif