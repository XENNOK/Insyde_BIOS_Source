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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  EfiVpdData.c

Abstract:

  GUID for use conveying VPD Data to DXE

--*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (EfiVpdData)

//
// Firmware ID
//
EFI_GUID  gFirmwareIdGuid   = FIRMWARE_ID_GUID;

CHAR16    gFirmwareIdName[] = L"FirmwareId";

//
// GUID for Firmware ID
//
EFI_GUID_STRING (&gFirmwareIdGuid, "FirmwareId", "FirmwareId");

//
// DMI data
//
EFI_GUID  gDmiDataGuid    = DMI_DATA_GUID;

CHAR16    gDmiDataName[]  = DMI_DATA_NAME;

//
// GUID for DMI data
//
EFI_GUID_STRING (&gDmiDataGuid, ASCII_DMI_DATA_NAME, ASCII_DMI_DATA_NAME);

//
// UUID data
//
EFI_GUID  gUuidVariableGuid   = UUID_VARIABLE_GUID;

CHAR16    gUuidVariableName[] = UUID_VARIABLE_NAME;

//
// GUID for UUID
//
EFI_GUID_STRING (&gUuidVariableGuid, ASCII_UUID_VARIABLE_GUID, ASCII_UUID_VARIABLE_GUID);
