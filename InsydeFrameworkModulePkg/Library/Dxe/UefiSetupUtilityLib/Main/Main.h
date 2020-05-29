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
//;   Main.h
//;
//; Abstract:
//;
//;   Header file for Main menu
//;

#ifndef _MAIN_CALLBACK_H_
#define _MAIN_CALLBACK_H_

#include "SetupUtilityLibCommon.h"
#include "SystemInformation.h"

#include EFI_PROTOCOL_DEFINITION (OemServices)

#define EFI_MAIN_MENU_SIGNATURE EFI_SIGNATURE_32('M','a','i','M')
#define EFI_MAIN_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_MAIN_MENU_SIGNATURE)

#endif