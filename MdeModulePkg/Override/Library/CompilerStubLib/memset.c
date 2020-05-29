/** @file
  memset() function implementation for compiler stub

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>

#ifndef __GNUC__

/**
  Wrapper function for intrinsic memset()

  @param  Dest      The memory to set.
  @param  Count     The number of bytes to set.
  @param  Char      The value with which to fill Length bytes of Buffer.

  @return Dest

*/
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