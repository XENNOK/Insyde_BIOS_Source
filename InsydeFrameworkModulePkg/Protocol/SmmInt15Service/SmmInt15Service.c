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
//;
//; Module Name:
//; 
//;   SmmInt15Service.c
//; 
//; Abstract:
//; 
//;   This code abstracts SMM Int15 Service Protocol
//;

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (SmmInt15Service)

EFI_GUID  gEfiSmmInt15ServiceProtocolGuid = EFI_SMM_INT15_SERVICE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSmmInt15ServiceProtocolGuid, "SMM Int15 Service Protocol", "SMM Int15 Service Protocol");
