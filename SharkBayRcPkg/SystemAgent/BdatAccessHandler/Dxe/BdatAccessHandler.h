/** @file

  This code makes the BIOS Data structure available via standard ACPI mechanisms.

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

#ifndef _BDAT_ACCESS_HANDLER_H_
#define _BDAT_ACCESS_HANDLER_H_

#include "BdatRmtHandler.h"
///===============================================
///  MdePkg/Include/
///===============================================
#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/AcpiTable.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SyatemAgent/
///===============================================
#include <Protocol/SaPlatformPolicy.h>

///
/// Ensure proper structure formats
///
#pragma pack(push, 1)
///
/// BIOS Data ACPI structure
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  BdatGas;
} EFI_BDAT_ACPI_DESCRIPTION_TABLE;


/**

  Entry point of the Bdat RMT Access Handler.

  @param[in] DxePlatformSaPolicy   : A pointer to Dxe platform policy
  @param[in] HobList               : A pointer to the HOB list
  @param[in] AcpiTable             : A pointer to ACPI table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @exception EFI_UNSUPPORTED:       A needed driver was not located
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
BdatRmtHandler (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy,
  IN VOID                            *HobList,
  IN EFI_ACPI_TABLE_PROTOCOL         *AcpiTable
  );

///
/// This is copied from Include\Acpi.h
///
#define CREATOR_ID_INTEL  0x4C544E49  /// "LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

#pragma pack(pop)

#endif /// _BDAT_ACCESS_HANDLER_H_
