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
//;   Security.h
//;
//; Abstract:
//;
//;   Header file for Security menu
//;

#ifndef _SECURITY_CALLBACK_H_
#define _SECURITY_CALLBACK_H_

#include "SetupUtilityLibCommon.h"
#include "password.h"

#define EFI_SECURITY_MENU_SIGNATURE EFI_SIGNATURE_32('S','e','c','M')
#define EFI_SECURITY_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_SECURITY_MENU_SIGNATURE)

#endif