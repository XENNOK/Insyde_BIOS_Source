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

  EndOfBdsBootSelection.c

Abstract:

  This protocol will be installed on the end of BdsBootSelection

--*/

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION(EndOfBdsBootSelection)

EFI_GUID gEfiEndOfBdsBootSelectionProtocolGuid = EFI_END_OF_BDS_BOOT_SELECTION_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiEndOfBdsBootSelectionProtocolGuid, "EndOfBdsBootSelection", "End Of BdsBootSelection Protocol");
