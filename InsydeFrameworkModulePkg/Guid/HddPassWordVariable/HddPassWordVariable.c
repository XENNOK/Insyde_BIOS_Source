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
//; Abstract:
//;
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (HddPasswordVariable)

EFI_GUID  gSaveHddPasswordGuid     = HDD_PASSWORD_TABLE_GUID;
EFI_GUID  gSaveHddPasswordInfoGuid = HDD_PASSWORD_INFO_TABLE_GUID;

EFI_GUID_STRING (&gSaveHddPasswordGuid, "Save Hdd Password Variable", "Save Hdd Password GUID");
EFI_GUID_STRING (&gSaveHddPasswordInfoGuid, "Save Hdd Password Info Variable", "Save Hdd Password Info GUID");
