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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
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

  Contains simulator codenamed SoftSdv specific implementations required to use
  status codes.

--*/

//
// Statements that include other files.
//
#include "Tiano.h"
#include "EfiCommonLib.h"
#include "EfiRuntimeLib.h"
#include "EfiStatusCode.h"
#include "RtMemoryStatusCodeLib.h"
#include "BsDataHubStatusCodeLib.h"
#include "BsSerialStatusCodeLib.h"

//
// Platform definitions
//
#define CONFIG_PORT0              0x02E
#define INDEX_PORT0               0x02E
#define DATA_PORT0                0x02F

#define SIO_COM1                  0x4
#define SIO_COM1_POWERON          0x10

#define REG_LOGICAL_DEVICE        0x07

#define POWER_CONTROL             0x22
#define ACTIVATE                  0x30
#define BASE_ADDRESS_HIGH         0x60
#define BASE_ADDRESS_LOW          0x61
#define PRIMARY_INTERRUPT_SELECT  0x70

//
// Lib globals used to for serial config defaults.
//
DEBUG_CODE (
  UINT16 gComBase = 0x2f8;
  UINTN gBps      = 115200;
  UINT8 gData     = 8;
  UINT8 gStop     = 1;
  UINT8 gParity   = 0;
  UINT8 gBreakSet = 0;
)
//
// Function implementations
//
EFI_STATUS
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

  CoseType  - Indicates the type of status code being reported.
  Value     - Describes the current status of a hardware or software entity.
  Instance  - The enumeration of a hardware or software entity within the system.
  CallerId  - This parameter may be used to identify the caller.
  Data      - This optional parameter may be used to pass additional data.

Returns:

  EFI_SUCCESS     Always returns success.

--*/
// GC_TODO:    CodeType - add argument and description to function comment
{
  //
  // Report runtime capable status codes first.  These are higher priority then
  // BS listeners.
  //
  RtMemoryReportStatusCode (CodeType, Value, Instance, CallerId, Data);

  if (EfiAtRuntime ()) {
    //
    // For now all we do is post code at runtime
    //
    return EFI_SUCCESS;
  }
  //
  // Report to BS listeners.
  //
  BsDataHubReportStatusCode (CodeType, Value, Instance, CallerId, Data);

  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
  {
    BsSerialReportStatusCode (CodeType, Value, Instance, CallerId, Data);
  }
  );

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

  ImageHandle   - Handle for the image of this driver
  SystemTable   - Pointer to the EFI System Table

Returns:

  None

--*/
{
  RtMemoryInitializeStatusCode (ImageHandle, SystemTable);
  BsDataHubInitializeStatusCode (ImageHandle, SystemTable);

  //
  // Play any prior status codes to the data hub.
  //
  PlaybackStatusCodes (BsDataHubReportStatusCode);

  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
  {
    UINT16 IndexPort;
    UINT16 ConfigPort;
    UINT16 DataPort;
    UINT8 PowerControl;

    //
    // hard code for sio init
    //
    ConfigPort  = CONFIG_PORT0;
    DataPort    = DATA_PORT0;
    IndexPort   = INDEX_PORT0;

    //
    // Start Configuration Mode
    //
    IoWrite8 (ConfigPort, 0x55);

    IoWrite8 (IndexPort, POWER_CONTROL);
    PowerControl = IoRead8 (DataPort);
    IoWrite8 (DataPort, PowerControl | SIO_COM1_POWERON);

    IoWrite8 (IndexPort, REG_LOGICAL_DEVICE);

    IoWrite8 (DataPort, SIO_COM1);

    IoWrite8 (IndexPort, BASE_ADDRESS_LOW);
    IoWrite8 (DataPort, 0xf8);

    IoWrite8 (IndexPort, BASE_ADDRESS_HIGH);
    IoWrite8 (DataPort, 0x03);

    IoWrite8 (IndexPort, PRIMARY_INTERRUPT_SELECT);
    IoWrite8 (DataPort, 0x04);

    IoWrite8 (IndexPort, ACTIVATE);
    IoWrite8 (DataPort, 0x01);

    //
    // End of Configuration Mode
    //
    IoWrite8 (ConfigPort, 0xAA);

    //
    // Initialize status code listener.
    //
    BsSerialInitializeStatusCode (ImageHandle, SystemTable);
  }
  );
}
