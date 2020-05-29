/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains a standard c library
--*/
/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Portions copyright (c) 1999, 2000
 * Intel Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *
 *    This product includes software developed by the University of
 *    California, Berkeley, Intel Corporation, and its contributors.
 *
 * 4. Neither the name of University, Intel Corporation, or their respective
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS, INTEL CORPORATION AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS,
 * INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "StdCLibSupport.h"

//
// setjmp.h
//
void longjmp(jmp_buf env, int val)
{
  LongJump(env, (UINTN)((val == 0) ? 1 : val));
}

//
// ctype.h
//
int tolower (int c)
{
  if (('A' <= (c)) && ((c) <= 'Z')) {
    return ((c - ('A' - 'a')));
  }
  return (c);
}

int
isdigit(int c)
{
  return ((((c) >= '0') && ((c) <= '9')));
}

int
isalpha(int c)
{
  return (((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')));
}

int
isupper(int c)
{
  return ((c<='Z' && c>='A'));
}


int
isascii(int c)
{
  /* c is a simulated unsigned integer */
  return ((c <= 127));
}


int
isspace(int c) {
  return ((c<=' ' && (c==' ' || (c<=13 && c>=9))));
}


//
// stdlib.h
//
int
abs(int j)
{
  return(j < 0 ? -j : j);
}

void *
calloc(size_t NMemb, size_t MembSize )
{
  return AllocateZeroPool (NMemb * MembSize);
}

void
free (void *buf) {
  ASSERT (buf != NULL);
  FreePool (buf);
}

void *
malloc(
  size_t Size
  )
{
    return AllocatePool ((UINTN) Size);
}

long int
labs (long int i)
{
 return i < 0 ? -i : i;
}


//
// Duplicated from EDKII BaseSortLib for qsort() wrapper
//
typedef
INTN
(*SORT_COMPARE)(
  IN  VOID  *Buffer1,
  IN  VOID  *Buffer2
  );

STATIC
VOID
QuickSortWorker (
  IN OUT    VOID          *BufferToSort,
  IN CONST  UINTN         Count,
  IN CONST  UINTN         ElementSize,
  IN        SORT_COMPARE  CompareFunction,
  IN        VOID          *Buffer
  )
{
  VOID        *Pivot;
  UINTN       LoopCount;
  UINTN       NextSwapLocation;

  ASSERT(BufferToSort    != NULL);
  ASSERT(CompareFunction != NULL);
  ASSERT(Buffer          != NULL);

  if (Count < 2 || ElementSize  < 1) {
    return;
  }

  NextSwapLocation = 0;

  //
  // Pick a pivot (we choose last element)
  //
  Pivot = ((UINT8 *)BufferToSort + ((Count - 1) * ElementSize));

  //
  // Now get the pivot such that all on "left" are below it
  // and everything "right" are above it
  //
  for (LoopCount = 0; LoopCount < Count - 1;  LoopCount++)
  {
    //
    // If the element is less than the pivot
    //
    if (CompareFunction ((VOID *)((UINT8 *)BufferToSort + ((LoopCount) * ElementSize)), Pivot) <= 0) {
      //
      // Swap
      //
      CopyMem (Buffer, (UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), ElementSize);
      CopyMem ((UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), (UINT8 *)BufferToSort + ((LoopCount) * ElementSize), ElementSize);
      CopyMem ((UINT8 *)BufferToSort + ((LoopCount) * ElementSize), Buffer, ElementSize);

      //
      // Increment NextSwapLocation
      //
      NextSwapLocation++;
    }
  }
  //
  // Swap pivot to it's final position (NextSwapLocaiton)
  //
  CopyMem (Buffer, Pivot, ElementSize);
  CopyMem (Pivot, (UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), ElementSize);
  CopyMem ((UINT8 *)BufferToSort + (NextSwapLocation * ElementSize), Buffer, ElementSize);

  //
  // Now recurse on 2 paritial lists.  Neither of these will have the 'pivot' element.
  // IE list is sorted left half, pivot element, sorted right half...
  //
  QuickSortWorker (
    BufferToSort,
    NextSwapLocation,
    ElementSize,
    CompareFunction,
    Buffer
    );

  QuickSortWorker (
    (UINT8 *)BufferToSort + (NextSwapLocation + 1) * ElementSize,
    Count - NextSwapLocation - 1,
    ElementSize,
    CompareFunction,
    Buffer
    );

  return;
}


/* Performs a quick sort */
void qsort (void *base, size_t num, size_t width, int (*compare)(const void *, const void *))
{
  VOID  *Buffer;

  ASSERT (base    != NULL);
  ASSERT (compare != NULL);

  Buffer = AllocatePool (width);

  ASSERT (Buffer != NULL);

  //
  // Re-use PerformQuickSort() function Implementation in EDKII BaseSortLib.
  //
  QuickSortWorker (base, (UINTN)num, (UINTN)width, (SORT_COMPARE)compare, Buffer);

  FreePool (Buffer);
  return;
}

void *
realloc(
  void   *OldPtr,
  size_t NewSize
  )
{
  void *NewPtr;

  if (OldPtr == NULL) {
    return AllocatePool (NewSize);
  } else if (NewSize == 0) {
    FreePool (OldPtr);
  } else {
    NewPtr = AllocatePool (NewSize);
    if (NewPtr != NULL) {
      CopyMem (NewPtr, OldPtr, NewSize);
	  FreePool (OldPtr);
      return NewPtr;
    }
  }

  return NULL;
}

#ifdef __GNUC__
#ifndef MDE_CPU_ARM
VOID *
memcpy (
  OUT VOID        *Dest,
  IN  const VOID  *Src,
  IN  UINTN       Count
  )
{
  volatile UINT8  *Ptr;
  const    UINT8  *Source;

  for (Ptr = Dest, Source = Src; Count > 0; Count--, Source++, Ptr++) {
    *Ptr = *Source;
  }

  return Dest;
}

VOID *
memset (
  OUT VOID    *Dest,
  IN  int     Char,
  IN  UINTN   Count
  )
{
  volatile UINT8  *Ptr;

  for (Ptr = Dest; Count > 0; Count--, Ptr++) {
    *Ptr = (UINT8) Char;
  }

  return Dest;
}
#endif
#endif

void *
memmove(void *dst0, const void *src0, size_t size0)
{
  char *dst = (char *)dst0;
  char *src = (char *)src0;
  INTN size = size0;
  if (size <= 0)
    return dst;
  if (src >= dst+size || dst >= src+size)
    return memcpy(dst, src, size);
  if (src > dst) {
    while (--size >= 0)
      *dst++ = *src++;
  }
  else if (src < dst) {
    src += size;
    dst += size;
    while (--size >= 0)
      *--dst = *--src;
  }
  return dst0;
}

char *
strcat(register char *s, register const char *append)
{
  char *save = s;

  for (; *s; ++s);
  while ((*s++ = *append++) != 0);
  return(save);
}

int
strcmp(register const char *s1,  register const char *s2)
{
  while (*s1 == *s2++)
    if (*s1++ == 0)
      return (0);
  return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}

char *
strcpy(register char *to, register const char * from)
{
  char *save = to;

  for (; (*to = *from) != 0; ++from, ++to);
  return(save);
}

size_t
strlen(const char *str)
{
  register const char *s;

  for (s = str; *s; ++s);
  return (size_t)(s - str);
}

int
strncmp(register const char *s1, register const char *s2, register size_t n)
{

  if (n == 0)
    return (0);
  do {
    if (*s1 != *s2++)
      return (*(const unsigned char *)s1 -
        *(const unsigned char *)(s2 - 1));
    if (*s1++ == 0)
      break;
  } while (--n != 0);
  return (0);
}

char *
strncpy(char *dst, const char *src, register size_t n)
{
  if (n != 0) {
    register char *d = dst;
    register const char *s = src;

    do {
      if ((*d++ = *s++) == 0) {
        /* NUL pad the remaining n-1 bytes */
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}

/*
 * Find the first occurrence of find in s.
 */
char *
strstr(register const char *s, register const char *find)
{
  register char c, sc;
  register size_t len;

  if ((c = *find++) != 0) {
    len = strlen(find);
    do {
      do {
        if ((sc = *s++) == 0)
          return (NULL);
      } while (sc != c);
    } while (strncmp(s, find, len) != 0);
    s--;
  }
  return ((char *)s);
}

int
strcmpi(register const char *s1, register const char *s2)
{
  while (tolower(*s1) == tolower(*s2++))
    if (*s1++ == 0)
      return (0);
  return (tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)(s2 - 1)));
}

char *
strdup(const char *str)
{
  if (str != NULL) {
    register char *copy = malloc(strlen(str) + 1);
    if (copy != NULL)
      return strcpy(copy, str);
  }
  return NULL;
}

//
// wchar.c
//
unsigned short *
wcsdup(unsigned short *str)
{
  if (str != NULL) {
    register unsigned short *copy = malloc(wcslen(str) + 1);
    if (copy != NULL) {
      return wcscpy(copy, str);
    }
  }
  return NULL;
}

unsigned long
strtoul(char *nptr, char **endptr, register int base)
{
  register const char *s = nptr;
  register unsigned long acc;
  register unsigned char c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * See strtol for comments as to the logic used.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
  cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (!isascii(c))
      break;
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
//  errno = ERANGE;
  } else if (neg)
    acc = (unsigned long)(-(long)acc);
  if (endptr != 0)
    *endptr = (char *)(any ? s - 1 : nptr);
  return (acc);
}


long
strtol(const char *nptr, char **endptr, register int base)
{
  register const char *s = nptr;
  register unsigned long acc;
  register unsigned char c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? LONG_MIN+1 : LONG_MAX; /* XXX EFI port */
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (!isascii(c))
      break;
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
//  errno = ERANGE;
  } else if (neg)
    acc = (unsigned long)(-(long)acc); /* XXX cast for EFI port */
  if (endptr != 0)
    *endptr = (char *)(any ? s - 1 : nptr);
  return (acc);
}




long wcstol(unsigned short  *nptr, unsigned short  **endptr , int base )
{
  register const unsigned short *s = nptr;
  register unsigned long acc;
  register unsigned short c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? LONG_MIN+1 : LONG_MAX; /* XXX EFI port */
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (c > 128) // ascii check
      break;
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
//  errno = ERANGE;
  } else if (neg)
    acc = (unsigned long)(-(long)acc); /* XXX cast for EFI port */
  if (endptr != 0)
    *endptr = (unsigned short *)(any ? s - 1 : nptr);
  return (acc);
}


unsigned long wcstoul(unsigned short *nptr, unsigned short **endptr , unsigned int base )
{
  register const unsigned short *s = nptr;
  register unsigned long acc;
  register unsigned short c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * See strtol for comments as to the logic used.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
  cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (c > 127) // ascii check
      break;
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
//  errno = ERANGE;
  } else if (neg)
    acc = (unsigned long)(-(long)acc);
  if (endptr != 0)
    *endptr = (unsigned short *)(any ? s - 1 : nptr);
  return (acc);
}


int wcscasecmp (wchar_t *s1, wchar_t *s2)
{
  register const wchar_t
      *us1 = (const wchar_t *)s1,
      *us2 = (const wchar_t *)s2;

  while (tolower(*us1) == tolower(*us2++))
    if (*us1++ == '\0')
      return (0);
  return (tolower(*us1) - tolower(*--us2));
}

wchar_t*
wcscpy(register wchar_t  *to, register const wchar_t  *from)
{
  unsigned short  *save = to;

  for (; (*to = *from) != 0; ++from, ++to);
  return(save);
}

size_t
wcslen(const wchar_t *str)
{
  register const wchar_t *s;

  for (s = str; *s; ++s);
  return (size_t)(s - str);
}

wchar_t *
wcscat(register wchar_t *s, register const wchar_t * append)
{
  wchar_t *save = s;

  for (; *s; ++s);
  while ((*s++ = *append++) != 0);
  return(save);
}

int
wcscmp(register const wchar_t *s1, register const wchar_t *s2)
{
  while (*s1 == *s2++)
    if (*s1++ == 0)
      return (0);
  return (*(const wchar_t *)s1 - *(const wchar_t *)(s2 - 1));
}

wchar_t *
wcsncpy(wchar_t *dst, const wchar_t * src, register size_t n)
{
  if (n != 0) {
    register wchar_t *d = dst;
    register const wchar_t *s = src;

    do {
      if ((*d++ = *s++) == 0) {
        /* NUL pad the remaining n-1 wide characters */
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}

