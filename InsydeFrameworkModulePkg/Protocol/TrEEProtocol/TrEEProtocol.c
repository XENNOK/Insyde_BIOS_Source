//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include <Tiano.h>
#include EFI_PROTOCOL_DEFINITION (TrEEProtocol)

EFI_GUID gEfiTrEEProtocolGuid = EFI_TREE_PROTOCOL_GUID;
EFI_GUID gEfiTrEEServiceBindingProtocolGuid = EFI_TREE_SERVICE_BINDING_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiTrEEProtocolGuid, "EfiTrEEProtocol", "Efi TrEE Protocol");
