/** @file
  Header file for Password

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_PASSWORD_H_
#define _SETUP_PASSWORD_H_

#include "SetupUtilityLibCommon.h"
#include <Uefi/UefiSpec.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>

#include <Protocol/SetupMouse.h>
#include <Protocol/CpuIo.h>
#include <Guid/HobList.h>
#include <Protocol/HddPasswordService.h>
#include <Protocol/SysPasswordService.h>
#include <Guid/HddPasswordVariable.h>
#include <Library/PcdLib.h>

#define MIN_PASSWORD_LENGTH       1
#define SETUP_PASSWORD            0x01
#define EBDA(a)                   (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))

#define EBDA_HDD_LOCKED_FLAG      0x3d9
#define HDD_UNLOCKED              5
#define HDD_FROZEN                21
#define HDD_LOCKED                7
#define BROWSER_STATE_VALIDATE_PASSWORD  0
#define BROWSER_STATE_SET_PASSWORD       1

#define bit(a)   (1 << (a))
#define HDD_SECURITY_SUPPORT bit(0)
#define HDD_SECURITY_ENABLE  bit(1)
#define HDD_SECURITY_LOCK    bit(2)
#define HDD_SECURITY_FROZEN  bit(3)
#define HDD_SECURITY_EXPIRED bit(4)
#define ATAPI_DEVICE_BIT     bit(15)

#define INIT_ITEM_MAPPING(item,value) (item==0?(item=value):(value=value))

typedef struct _PSW_HDD_INFO {
  CHAR16      InfoString[50];
} PSW_HDD_INFO;

//
// The following functions are internal use in security page
//
EFI_STATUS
UpdateSupervisorTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  );

EFI_STATUS
UpdateUserTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  );

EFI_STATUS
SetAllHarddiskPswd (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *FormCallback,
  IN  BOOLEAN                                MasterOrUser,
  IN  EFI_HII_HANDLE                         HiiHandle,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT BOOLEAN                                *PState,
  IN  HDD_PASSWORD_INFO_DATA                 *HddPasswordDataInfo,
  IN  UINTN                                  NumOfHddPswd
  );

EFI_STATUS
UpdateHddPasswordLabel (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINTN                             NumOfHddPswd
  );

UINTN
SysPasswordMaxNumber (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  );

#endif
