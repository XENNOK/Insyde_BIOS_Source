//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; Module Name:
//;
//;   NetworkLocker.c
//;
//; Abstract:
//;
//;   When user enable PxeToLan, install one protocol to indicate Native driver load or not
//;

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (NetworkLocker)

EFI_GUID gNetworkLockerProtocolGuid = NETWORK_LOCKER_PROTOCOL_GUID;
