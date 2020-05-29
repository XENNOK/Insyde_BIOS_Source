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
/*++

Module Name:

  SystemHealth.h

Abstract:

  This code abstracts SystemHealth Protocol

--*/

#ifndef _EFI_SYSTEM_HEALTH_PROTOCOL_
#define _EFI_SYSTEM_HEALTH_PROTOCOL_

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (SystemHealth)

#define EFI_SYSTEM_HEALTH_PROTOCOL_GUID \
  { \
    0x5aaeff02, 0x305e, 0x4d57, 0xae, 0x49, 0x25, 0xcc, 0x04, 0x66, 0xbb, 0xce \
  }

EFI_FORWARD_DECLARATION (EFI_SYSTEM_HEALTH_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SYSTEM_VOLTAGE) (
  OUT UINTN       *VoltageDataArray,
  OUT UINTN       *DataArrayNumber
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SYSTEM_TEMPERATURE) (
  OUT UINTN       *TemperatureDataArray,
  OUT UINTN       *DataArrayNumber
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SYSTEM_FAN_SPEED) (
  OUT UINTN       *FanSpeedDataArray,
  OUT UINTN       *DataArrayNumber
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_SYSTEM_FAN_SPEED) (
  IN UINTN        *FanSpeedDataArray,
  IN UINTN        *DataArrayNumber
  );

//
// SystemHealth PROTOCOL
//
typedef struct _EFI_SYSTEM_HEALTH_PROTOCOL {
  EFI_GET_SYSTEM_VOLTAGE              GetSystemVoltage;
  EFI_GET_SYSTEM_TEMPERATURE          GetSystemTemperature;
  EFI_GET_SYSTEM_FAN_SPEED            GetSystemFanSpeed;
  EFI_SET_SYSTEM_FAN_SPEED            SetSystemFanSpeed;
} EFI_SYSTEM_HEALTH_PROTOCOL;

extern EFI_GUID gEfiSystemHealthProtocolGuid;

#endif
