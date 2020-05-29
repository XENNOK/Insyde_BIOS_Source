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
//;   DebugComm.c
//; 

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (DebugComm)


EFI_GUID gEfiDebugCommProtocolGuid = EFI_DEBUG_COMM_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiDebugCommProtocolGuid, "DebugComm Protocol", "EFI 1.1 DebugComm Protocol");
