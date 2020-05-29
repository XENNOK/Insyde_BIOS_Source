/** @file
  FtwLiteOnFtwThunk.c

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FtwLiteOnFtwThunk.h"

EFI_STATUS
FtwLiteWrite (
  IN EFI_FTW_LITE_PROTOCOL             *This,
  IN EFI_HANDLE                        FvbHandle,
  IN EFI_LBA                           Lba,
  IN UINTN                             Offset,
  IN UINTN                             *NumBytes,
  IN VOID                              *Buffer
  )
;

EFI_FTW_LITE_PROTOCOL mFtwLite = {
  FtwLiteWrite
};

EFI_STATUS
FtwLiteWrite (
  IN EFI_FTW_LITE_PROTOCOL             *This,
  IN EFI_HANDLE                        FvbHandle,
  IN EFI_LBA                           Lba,
  IN UINTN                             Offset,
  IN UINTN                             *NumBytes,
  IN VOID                              *Buffer
  )
{
  EFI_STATUS Status;
  EFI_FAULT_TOLERANT_WRITE_PROTOCOL *Ftw;
  
  Status = gBS->LocateProtocol(&gEfiFaultTolerantWriteProtocolGuid, NULL, &Ftw);
  ASSERT_EFI_ERROR(Status);
  
  return Ftw->Write(Ftw, Lba, Offset, *NumBytes, NULL, FvbHandle, Buffer);
}


/**
 GC_TODO: Add function description

 @param [in]   ImageHandle      GC_TODO: add argument description
 @param [in]   SystemTable      GC_TODO: add argument description

 @retval EFI_SUCCESS            GC_TODO: Add description for return value
 @retval EFI_SUCCESS            GC_TODO: Add description for return value

**/
EFI_STATUS
FtwLiteInitialize (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_HANDLE   Handle = NULL;
  
  return gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiFaultTolerantWriteLiteProtocolGuid,
                  &mFtwLite,
                  NULL
                  );
}
