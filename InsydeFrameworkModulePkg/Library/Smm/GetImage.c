//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  GetImage.c

Abstract:

  Image data extraction support for common use.

--*/

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "EfiImageFormat.h"

EFI_STATUS
GetImage (
  IN  EFI_GUID           *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size
  )
/*++

  Routine Description:

  Arguments:

  Returns:

--*/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; ++Index) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID**)&Fv
                    );

    if (EFI_ERROR (Status)) {
      gBS->FreePool(HandleBuffer);
      return Status;
    }

    //
    // Read desired section content in NameGuid file
    //
    *Buffer     = NULL;
    *Size       = 0;
    Status      = Fv->ReadSection (
                        Fv,
                        NameGuid,
                        SectionType,
                        0,
                        Buffer,
                        Size,
                        &AuthenticationStatus
                        );

    if (EFI_ERROR (Status) && (SectionType != EFI_SECTION_RAW)) {

      //
      // Try reading raw file, since the desired section does not exist
      //
      *Buffer = NULL;
      *Size   = 0;
      Status  = Fv->ReadFile (
                      Fv,
                      NameGuid,
                      Buffer,
                      Size,
                      &FileType,
                      &Attributes,
                      &AuthenticationStatus
                      );
    }

    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  gBS->FreePool(HandleBuffer);

  //
  // Not found image
  //
  if (Index == HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

