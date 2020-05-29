/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _HARDWARE_MONITOR_H_
#define _HARDWARE_MONITOR_H_

#include <SetupUtility.h>
#include <Library/HiiExLib.h>

typedef struct {
  UINT16                                StringID;
  STRING_REF                            TokenString1;
  STRING_REF                            TokenString2;
} HARDWARE_MONITOR_STRING;


EFI_STATUS
UpdateHardwareMonitorFanSpeed (
  IN  EFI_HII_HANDLE   HiiHandle,
  IN  UINTN            Keyvalue
  );

EFI_STATUS
UpdateHardwareMonitorTemperature (
  IN EFI_HII_HANDLE   HiiHandle,
  IN UINTN            KeyValue
  );

EFI_STATUS
UpdateHardwareMonitorVoltage (
  IN EFI_HII_HANDLE   HiiHandle,
  IN UINTN            KeyValue
  );

EFI_STATUS
InitHardwareMonitorMenu (
  IN EFI_HII_HANDLE   HiiHandle,
  IN EFI_GUID         *Protocol,     
  IN UINTN            FormLabel
  );

#endif
