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
//;   DefaultUpdateProtocol.c
//;
//; Abstract:
//;
//;   EFI Default Update protocol
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (VariableDefaultUpdateProtocol)

EFI_GUID gEfiVariableDefaultUpdateProtocolGuid = EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiVariableDefaultUpdateProtocolGuid, "Variable Default Update Protocol", "EFI Variable Default Update Protocol");

