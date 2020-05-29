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
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    EfiServerMgmtRtLib.h

Abstract:

  Server Management Driver Lib

Revision History

--*/

#ifndef _EFI_SERVER_MEMT_LIB_H_
#define _EFI_SERVER_MEMT_LIB_H_

#include "EfiServerManagement.h"
#include EFI_PROTOCOL_DEFINITION (GenericElog)

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
/*++

Routine Description:

  Function is used to append a Src1 and Src2 together.

Arguments:

  Src1  - A pointer to a device path data structure.
  Src2  - A pointer to a device path data structure.

Returns:

  A pointer to the new device path is returned.
  NULL is returned if space for the new device path could not be allocated from pool.
  It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

--*/
;

VOID
EFIAPI
EfiGetFullDriverPath (
  IN  EFI_HANDLE                  ImageHandle,
  IN  EFI_SYSTEM_TABLE            *SystemTable,
  OUT EFI_DEVICE_PATH_PROTOCOL    **CompleteFilePath
  )
/*++

Routine Description:

  Function is used to get the full device path for this driver.

Arguments:

  ImageHandle        - The loaded image handle of this driver.
  SystemTable        - The pointer of system table.
  CompleteFilePath   - The pointer of returned full file path

Returns:

  none

--*/
;

//
// Com Library Initializer.
//
EFI_STATUS
EfiInitializeComLayer (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

//
// IPMI COM Runtime Library functions.
//
EFI_STATUS
EfiSmLibGetComControllerInfo (
  IN  UINT16                            InstanceNumber,
  OUT UINT32                            *NumInterfaces,
  OUT EFI_SM_CTRL_INFO                  *ControllerInfo,
  OUT EFI_SM_COM_ADDRESS                *ControllerAddress
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  InstanceNumber    - GC_TODO: add argument description
  NumInterfaces     - GC_TODO: add argument description
  ControllerInfo    - GC_TODO: add argument description
  ControllerAddress - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibSendComData (
  IN  UINT16                            Instance,
  IN  EFI_SM_PROTOCOL_TYPE              ComProtocol,
  IN  EFI_SM_COM_ADDRESS                *SourceAddress,
  IN  EFI_SM_COM_ADDRESS                *TargetAddress,
  IN  UINT8                             *Data,
  IN  UINT32                            *Size,
  IN  UINT32                            *ReceiveKey
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Instance      - GC_TODO: add argument description
  ComProtocol   - GC_TODO: add argument description
  SourceAddress - GC_TODO: add argument description
  TargetAddress - GC_TODO: add argument description
  Data          - GC_TODO: add argument description
  Size          - GC_TODO: add argument description
  ReceiveKey    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibReceiveComData (
  IN UINT32                             SendKey,
  IN UINT8                              *Data,
  IN OUT  UINT32                        *Size,
  IN EFI_SM_CALLBACK                    *SignalData,
  IN BOOLEAN                            Blocking,
  IN OUT BOOLEAN                        *ReleaseKey
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SendKey     - GC_TODO: add argument description
  Data        - GC_TODO: add argument description
  Size        - GC_TODO: add argument description
  SignalData  - GC_TODO: add argument description
  Blocking    - GC_TODO: add argument description
  ReleaseKey  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
// ELOG Library Initializer.
//
EFI_STATUS
EfiInitializeGenericElog (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmSetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ElogData    - GC_TODO: add argument description
  DataType    - GC_TODO: add argument description
  AlertEvent  - GC_TODO: add argument description
  DataSize    - GC_TODO: add argument description
  RecordId    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmGetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ElogData  - GC_TODO: add argument description
  DataType  - GC_TODO: add argument description
  DataSize  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmEraseEventlogData (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  DataType  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmActivateEventLog (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  DataType    - GC_TODO: add argument description
  EnableElog  - GC_TODO: add argument description
  ElogStatus  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
// SENSOR Library Initializer.
//
EFI_STATUS
EfiInitializeGenericSensor (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibReadSensorInfo (
  IN EFI_SM_SENSOR_TYPE       SensorType,
  IN UINTN                    EventType,
  OUT UINTN                   *SensorNumber,
  OUT UINTN                   *SensorCount
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SensorType    - GC_TODO: add argument description
  EventType     - GC_TODO: add argument description
  SensorNumber  - GC_TODO: add argument description
  SensorCount   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibReadSensorStatus (
  IN UINTN                  SensorNumber,
  IN UINTN                  SensorInstance,
  IN  UINT64                StateBitMask,
  OUT UINT64                *StateBitStatus
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SensorNumber    - GC_TODO: add argument description
  SensorInstance  - GC_TODO: add argument description
  StateBitMask    - GC_TODO: add argument description
  StateBitStatus  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibRearmSensor (
  IN UINTN                  SensorNumber,
  IN UINTN                  SensorInstance,
  IN  UINT64                RearmBitMask,
  OUT UINT64                *RearmBitStatus
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SensorNumber    - GC_TODO: add argument description
  SensorInstance  - GC_TODO: add argument description
  RearmBitMask    - GC_TODO: add argument description
  RearmBitStatus  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSmLibReadSensorData (
  IN UINTN                                SensorNumber,
  IN UINTN                                SensorInstance,
  OUT EFI_SENSOR_THRESHOLD_STRUCT         *SensorThreshold,
  OUT EFI_SENSOR_READING_STRUCT           *SensorReading,
  OUT UINT8                               *SensorDataUnit
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SensorNumber    - GC_TODO: add argument description
  SensorInstance  - GC_TODO: add argument description
  SensorThreshold - GC_TODO: add argument description
  SensorReading   - GC_TODO: add argument description
  SensorDataUnit  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
