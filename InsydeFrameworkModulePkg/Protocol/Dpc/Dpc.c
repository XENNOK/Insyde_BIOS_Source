//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
//  Abstract:
//
//  EFI Deferred Procedure Call Protocol

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (Dpc)

EFI_GUID gEfiDpcProtocolGuid = EFI_DPC_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiDpcProtocolGuid, "EFI Deferred Procedure Call Protocol", "EFI Deferred Procedure Call Protocol");
