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
//;   Exit.h
//;
//; Abstract:
//;
//;   Header file for Exit menu
//;

#ifndef _EXIT_CALLBACK_H_
#define _EXIT_CALLBACK_H_

#include "SetupUtilityLibCommon.h"
#include "ExitFunc.h"
#define EFI_EXIT_MENU_SIGNATURE EFI_SIGNATURE_32('E','x','i','M')
#define EFI_EXIT_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_EXIT_MENU_SIGNATURE)
#define CUSTOM_BOOT_ORDER_SIZE              256

typedef struct {
  UINT16                                    BootOption;
  UINT16                                    DeviceNameLength;
  UINT8                                     FindFlag;
  CHAR16                                    *DeviceName;
} BOOT_ORDER_OPTION_HEAD;

#endif