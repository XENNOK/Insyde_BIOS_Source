/** @file
  TianoCompress SMM Driver

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
#include <Library/SmmServicesTableLib.h>
#include <Protocol/TianoCompress.h>

EFI_STATUS
EFIAPI
TianoCompress (
  IN      UINT8   *SrcBuffer,
  IN      UINT32  SrcSize,
  IN      UINT8   *DstBuffer,
  IN OUT  UINT32  *DstSize
  );

EFI_STATUS
InitMemorySpace (
  VOID
  );

VOID
FreeMemorySpace (
  VOID
  );


TIANO_COMPRESS_PROTOCOL mTianoCompress = {
  TianoCompress
};


/**
  The driver's entry point.


  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
TianoCompressEntry (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_HANDLE   Handle;
  EFI_STATUS   Status;

  //
  // Initialize the memory for tianocompress.
  // If there is one memory space cannot be allocated, free memory.
  //
  Status = InitMemorySpace ();
  if (EFI_ERROR (Status)) {
    FreeMemorySpace ();
    return Status;
  }

  Handle = NULL;
  return gSmst->SmmInstallProtocolInterface (
                  &Handle,
                  &gTianoCompressProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTianoCompress
                  );
}
