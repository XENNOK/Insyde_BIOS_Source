//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Password.h
//;
//; Abstract:
//;
//;   Header file for Password
//;

#ifndef _SETUP_PASSWORD_H_
#define _SETUP_PASSWORD_H_

#include "SetupUtilityLibCommon.h"
#include "EfiPrintLib.h"
//#include "Print.h"
#include "OemGraphicsLib.h"
#include "EfiHobLib.h"

#include EFI_PROTOCOL_DEFINITION (SetupMouse)
#include EFI_PROTOCOL_DEFINITION (CpuIo)
#include EFI_GUID_DEFINITION (Hob)
#include EFI_PROTOCOL_DEFINITION (HddPasswordService)
#include EFI_PROTOCOL_DEFINITION (SysPasswordService)

#define MIN_PASSWORD_LENGTH       1
#define SETUP_PASSWORD            0x01
#define EBDA(a)                   (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))

#define EBDA_HDD_LOCKED_FLAG      0x3d9
#define HDD_UNLOCKED              5
#define HDD_FROZEN                21
#define HDD_LOCKED                7

#define bit(a)   (1 << (a))
#define HDD_SECURITY_SUPPORT bit(0)
#define HDD_SECURITY_ENABLE  bit(1)
#define HDD_SECURITY_LOCK    bit(2)
#define HDD_SECURITY_FROZEN  bit(3)
#define HDD_SECURITY_EXPIRED bit(4)


typedef struct _PSW_HDD_INFO {
  CHAR16      InfoString[50];
} PSW_HDD_INFO;

typedef enum {
  BeforeHotKey     = 1,
  ScuFirst,
  ScuReEnter
} SYSTEM_PASSWORD_UNLOCK_STATUS;

//
// The following functions are internal use in security page
//
EFI_STATUS
UpdateSupervisorTags (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  );

EFI_STATUS
UpdateUserTags (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  );

EFI_STATUS
SetAllHarddiskPswd (
  IN  EFI_FORM_CALLBACK_PROTOCOL        *FormCallback,
  IN  BOOLEAN                           MasterOrUser,
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_IFR_DATA_ARRAY                *Data,
  OUT EFI_HII_CALLBACK_PACKET           **Packet,
  OUT BOOLEAN                           *PState,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINTN                             NumOfHddPswd
  );

EFI_STATUS
UpdateHddPasswordLabel (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINTN                             NumOfHddPswd
  );

#endif
