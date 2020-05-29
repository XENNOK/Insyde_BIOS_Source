//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  NonVolatileVariable.c

Abstract:

  This protocol will be installed when the non-volatile service initialized successfully.

--*/

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION(NonVolatileVariable)

EFI_GUID gEfiNonVolatileVariableProtocolGuid = EFI_NON_VOLATILE_VARIABLE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiNonVolatileVariableProtocolGuid, "NonVolatileVariable", "Non-volatile Variable Protocol");
