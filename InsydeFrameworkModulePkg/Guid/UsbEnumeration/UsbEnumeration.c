//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    UsbEnumeration.c

Abstract:

    Guid used to trigger event to do USB connection.

--*/

#include "Tiano.h"

#include EFI_GUID_DEFINITION (UsbEnumeration)

EFI_GUID gEfiUsbEnumerationGuid = EFI_USB_ENUMERATION_GUID;

EFI_GUID_STRING (&gEfiUsbEnumerationGuid, "EFI USB Enumeration Guid", "EFI USB Enumeration Guid");

