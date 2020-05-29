/** @file
  Header file for I2C Touch Panel driver function prototype and data structures

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _I2C_TOUCH_PANEL_H_
#define _I2C_TOUCH_PANEL_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HidDescriptorLib.h>
#include <Protocol/I2cIo.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/AdapterInformation.h>
#include <I2cHidDeviceInfo.h>

#define TOUCH_PANEL_CLASS               0x0d00

#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"
#define IMAGE_DRIVER_NAME               L"I2C Touch Panel Driver"
#define CONTROLLER_DRIVER_NAME          L"Generic I2C Touch Panel"
#define I2C_TOUCH_PANEL_DEV_SIGNATURE   SIGNATURE_32 ('I', '2', 'C', 'T')

typedef struct {
  UINTN                         Signature;
  EFI_ABSOLUTE_POINTER_PROTOCOL AbsolutePointerProtocol;
  EFI_ABSOLUTE_POINTER_STATE    State;
  EFI_ABSOLUTE_POINTER_MODE     Mode;
  EFI_UNICODE_STRING_TABLE      *ControllerNameTable;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  EFI_EVENT                     OnCompleteEvent;
  EFI_STATUS                    TransferResult;
  H2O_I2C_HID_DEVICE            I2cHid;
  HID_DESCRIPTOR                HidDescriptor;
  REPORT_FIELD_INFO             ReportFieldInfo;
  UINT8                         ReportId;
  UINT8                         *ReportData;
  BOOLEAN                       StateChanged;

} I2C_TOUCH_PANEL_DEV;

#define I2C_TOUCH_PANEL_DEV_FROM_ABSOLUTE_POINTER_PROTOCOL(a) \
    CR(a, I2C_TOUCH_PANEL_DEV, AbsolutePointerProtocol, I2C_TOUCH_PANEL_DEV_SIGNATURE)

extern EFI_DRIVER_BINDING_PROTOCOL      gI2cTouchPanelDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gI2cTouchPanelComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gI2cTouchPanelComponentName2;

#endif
