/** @file
  Header file for I2C Mouse driver function prototype and data structures

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

#ifndef _I2C_MOUSE_H
#define _I2C_MOUSE_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HidDescriptorLib.h>
#include <Protocol/I2cIo.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AdapterInformation.h>
#include <I2cHidDeviceInfo.h>

#define POINTER_CLASS           ((HID_UP_GENDESK << 8) | HID_GD_POINTER)
#define MOUSE_CLASS             ((HID_UP_GENDESK << 8) | HID_GD_MOUSE)

#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"
#define IMAGE_DRIVER_NAME               L"I2C Mouse Driver"
#define CONTROLLER_DRIVER_NAME          L"Generic I2C Mouse"
#define I2C_MOUSE_DEV_SIGNATURE SIGNATURE_32 ('I', '2', 'C', 'M')

typedef struct {
  UINTN                         Signature;
  EFI_SIMPLE_POINTER_PROTOCOL   SimplePointerProtocol;
  EFI_SIMPLE_POINTER_STATE      State;
  EFI_SIMPLE_POINTER_MODE       Mode;
  EFI_UNICODE_STRING_TABLE      *ControllerNameTable;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  EFI_EVENT                     OnCompleteEvent;
  EFI_STATUS                    TransferResult;
  H2O_I2C_HID_DEVICE            I2cHid;
  HID_DESCRIPTOR                HidDescriptor;
  REPORT_FIELD_INFO             ReportFieldInfo;
  UINT8                         ReportId;
  UINT8                         *ReportData;
  UINT8                         NumberOfButtons;
  INT32                         XLogicMax;
  INT32                         XLogicMin;
  INT32                         YLogicMax;
  INT32                         YLogicMin;
  BOOLEAN                       StateChanged;
} I2C_MOUSE_DEV;

#define I2C_MOUSE_DEV_FROM_SIMPLE_POINTER_PROTOCOL(a) \
    CR(a, I2C_MOUSE_DEV, SimplePointerProtocol, I2C_MOUSE_DEV_SIGNATURE)

extern EFI_DRIVER_BINDING_PROTOCOL      gI2cMouseDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gI2cMouseComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gI2cMouseComponentName2;

#endif
