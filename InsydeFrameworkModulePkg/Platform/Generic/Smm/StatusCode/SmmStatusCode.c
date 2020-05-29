//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmStatusCode.c

Abstract:

  Status Code protocol for SMM drivers. Installs the SMMStatusCodeProtocol.

--*/

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "SmmStatusCode.h"
#include "SmmDriverLib.h"

//
// Need to remove this if DXE Core creating correct Loaded Image Handle!
//
// EFI_GUID  mSmmStatusCodeFvImageGuid = {0x994C5A88, 0xFCE2, 0x43E5, 0x9E, 0xA4, 0x2F, 0xAB, 0xCB, 0x15, 0x30, 0x1D};
//
SMM_STATUS_CODE_PRIVATE SmmStatusCodeDev;

EFI_DRIVER_ENTRY_POINT (InitializeSmmStatusCode)

EFI_STATUS
InitializeSmmStatusCode (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initializes the SMM Handler Driver

Arguments:
  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:
  EFI_SUCCESS - If all services discovered.
  Other       - Failure in constructor.

--*/
// GC_TODO:    ImageHandle - add argument and description to function comment
// GC_TODO:    SystemTable - add argument and description to function comment
{
  EFI_STATUS  Status;
  BOOLEAN     InSmm;
  
  InsydeEfiInitializeSmmDriverLib (ImageHandle, SystemTable, &InSmm);

  if (InSmm) {
    SmmStatusCodeDev.SmmStatusCode.ReportStatusCode = SmmReportStatusCode;

    //
    // install the status code protocol
    //
    Status = gBS->InstallProtocolInterface (
                    &SmmStatusCodeDev.Handle,
                    &gEfiSmmStatusCodeProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &SmmStatusCodeDev.SmmStatusCode
                    );

  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmReportStatusCode (
  IN EFI_SMM_STATUS_CODE_PROTOCOL * This,
  IN EFI_STATUS_CODE_TYPE         CodeType,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN EFI_GUID                     * CallerId,
  IN EFI_STATUS_CODE_DATA         * Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  Same as ReportStatusCode service

Returns:

  EFI_SUCCESS     Always returns success.

--*/
// GC_TODO:    This - add argument and description to function comment
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  return PlatformReportStatusCode (CodeType, Value, Instance, CallerId, Data);
}
