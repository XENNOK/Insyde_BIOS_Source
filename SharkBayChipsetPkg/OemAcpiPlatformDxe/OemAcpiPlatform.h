/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OEM_ACPI_PLATFORM_H_
#define _OEM_ACPI_PLATFORM_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Acpi.h>
#include <ChipsetSetupConfig.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/AcpiSupport.h>

//[-start-121219-IB10820205-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121219-IB10820205-add]//
//[-start-130912-IB12360024-add]//
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
//[-end-130912-IB12360024-add]//

//[-start-120525-IB03600485-add]//
//[-end-120525-IB03600485-add]//
extern EFI_ACPI_SUPPORT_PROTOCOL                 *mAcpiSupport;
//[-Start-120712-IB10820084-modify]//
EFI_STATUS
OemUpdateOemTableID (
  VOID
  );
//[-end-120712-IB10820084-modify]//
#endif
