//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FWH_LPC.h"

EFI_STATUS
ReadDevice (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      Count
  )
/*++

Routine Description:

  Read from the flash device with given address and size

Arguments:

  Dest                          Destination Offset
  Src                           Source Offset
  Count                         The size for programming

Returns:

  EFI status

--*/
{

  EfiCopyMem (Dest, Src, Count);
  return EFI_SUCCESS;
}



