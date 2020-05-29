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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
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

  RtPlatformStatusCode.c

Abstract:

  Contains D845GRG specific implementations required to use status codes.

--*/

//
// Statements that include other files.
//
#include "Tiano.h"
#include "EfiCommonLib.h"
#include "EfiRuntimeLib.h"
#include "EfiStatusCode.h"
#include "RtPort80StatusCodeLib.h"
#include "BsDataHubStatusCodeLib.h"
#include "BsSerialStatusCodeLib.h"

//
// Lib globals used to for serial config defaults.
//
DEBUG_CODE (
  UINT16 gComBase = 0x3f8;
  UINTN gBps      = 115200;
  UINT8 gData     = 8;
  UINT8 gStop     = 1;
  UINT8 gParity   = 0;
  UINT8 gBreakSet = 0;
)

#define CONFIG_PORT0  0x2E
#define INDEX_PORT0   0x2E
#define DATA_PORT0    0x2F

//
// Function implementations
//
EFI_RUNTIMESERVICE
EFI_STATUS
EFIAPI
RtPlatformReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  Same as ReportStatusCode service

Returns:

  EFI_SUCCESS     Always returns success.

--*/
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  //
  // Report runtime capable status codes first.  These are higher priority then
  // BS listeners.
  //
  RtPort80ReportStatusCode (CodeType, Value, Instance, CallerId, Data);

  if (EfiAtRuntime ()) {
    //
    // For now all we do is post code at runtime
    //
    return EFI_SUCCESS;
  }
  //
  // Report to BS listeners.
  //
  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
  {
    BsSerialReportStatusCode (CodeType, Value, Instance, CallerId, Data);
  }
  );

  BsDataHubReportStatusCode (CodeType, Value, Instance, CallerId, Data);

  return EFI_SUCCESS;
}

EFI_BOOTSERVICE
VOID
EFIAPI
RtPlatformInitializeStatusCode (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

Routine Description:

  Initialize the status code listeners.  In pre-memory pass, all are
  initialized.  In post-memory pass, only Memory listener is initialized.

Arguments:

  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:

  None

--*/
// GC_TODO:    ImageHandle - add argument and description to function comment
// GC_TODO:    SystemTable - add argument and description to function comment
{
  BsDataHubInitializeStatusCode (ImageHandle, SystemTable);

  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
  {
    UINT16 ConfigPort;
    UINT16 IndexPort;
    UINT16 DataPort;

    //
    // hard code for sio init
    //
    ConfigPort  = CONFIG_PORT0;
    DataPort    = DATA_PORT0;
    IndexPort   = INDEX_PORT0;

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.ConfigPort, 0x55);
    //
    IoWrite8 (ConfigPort, 0x55);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, REG_POWER_CONTROL);
    //
    IoWrite8 (IndexPort, 0x22);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x39);
    //
    IoWrite8 (DataPort, 0x19);

    //
    // FloppyFuncGroup->FdcWritePort ( SioPort.IndexPort, REG_LOGICAL_DEVICE ) ;
    //
    IoWrite8 (IndexPort, 0x07);

    //
    // FloppyFuncGroup->FdcWritePort ( SioPort.DataPort, SIO_COM1 ) ;
    //
    IoWrite8 (DataPort, 0x03);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, BASE_ADDRESS_LOW);
    //
    IoWrite8 (IndexPort, 0x61);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0xf8);
    //
    IoWrite8 (DataPort, 0xf8);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, BASE_ADDRESS_HIGH);
    //
    IoWrite8 (IndexPort, 0x60);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x03);
    //
    IoWrite8 (DataPort, 0x03);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, PRIMARY_INTERRUPT_SELECT);
    //
    IoWrite8 (IndexPort, 0x70);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x04);
    //
    IoWrite8 (DataPort, 0x04);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, ACTIVATE);
    //
    IoWrite8 (IndexPort, 0x30);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 1);
    //
    IoWrite8 (DataPort, 0x01);

    //
    // FloppyFuncGroup->FdcWritePort(SioPort.ConfigPort, 0xaa);
    //
    IoWrite8 (ConfigPort, 0xAA);

    //
    // Initialize additional debug status code listeners.
    //
    BsSerialInitializeStatusCode (ImageHandle, SystemTable);
  }
  );
}
