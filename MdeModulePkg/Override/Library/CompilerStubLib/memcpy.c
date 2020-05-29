/** @file
  memcpy() function implementation for compiler stub

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
  Wrapper function for intrinsic memcpy()

  @param  Dest                The pointer to the destination buffer of the memory copy.
  @param  Src                 The pointer to the source buffer of the memory copy.
  @param  Count               The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @return Dest
*/
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

#endif