/** @file
  The EFI Legacy BIOS Patform Protocol member function

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosPlatformDxe.h"

#include <SmiTable.h>
#include <Library/PcdLib.h>
#include <Protocol/SdPassThru.h>

#define SMM_ENTRY_NUM     6

//
// Continue the define of DMI_PNP_50_57 in LegacyBios.h
//
#define TCG_MOR_SMM_FUN    0x0004
#define ATA_LEGACY_SMM_FUN 0x0008
#define SD_LEGACY_SMM_FUN  0x0010

/**
  Load and initialize the Legacy BIOS SMM handler.

  @param  This                   The protocol instance pointer.
  @param  EfiToLegacy16BootTable A pointer to Legacy16 boot table.

  @retval EFI_SUCCESS           SMM code loaded.
  @retval EFI_DEVICE_ERROR      SMM code failed to load

**/
EFI_STATUS
EFIAPI
SmmInit (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  VOID                                        *EfiToLegacy16BootTable
  )
{
  EFI_STATUS                            Status;
  SMM_TABLE_PORT16                      *SoftIntData;
  SMM_ENTRY_PORT16                      *SmmEntry;
  UINTN                                 TableSize;
  SD_PASS_THRU_PROTOCOL                 *SdPassThru;
  UINT16                                SoftwareSmiPort;

  TableSize = sizeof (SMM_TABLE_PORT16) + sizeof(SMM_ENTRY_PORT16) * SMM_ENTRY_NUM;
  Status    = gBS->AllocatePool (EfiBootServicesData, TableSize, (VOID **)&SoftIntData);
  ASSERT_EFI_ERROR (Status);

  ZeroMem (SoftIntData, TableSize);
  ((EFI_TO_COMPATIBILITY16_BOOT_TABLE *) EfiToLegacy16BootTable)->SmmTable  = (UINT32)(UINTN)SoftIntData;

  SoftIntData->NumSmmEntries = 0;

  SmmEntry = &SoftIntData->SmmEntry;

  SoftwareSmiPort = PcdGet16 (PcdSoftwareSmiPort);

  //
  // This entry will be port16 (address 0x00b2) and data8 (data 0x44).
  //
  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = INT15_D042;
  SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
  SmmEntry->SmmPort                       = SoftwareSmiPort;
  SmmEntry->SmmData                       = INT15_0D042H;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;

  //
  // Skip over the port16 data8 entry we just entered.
  //
  // This entry will be port16 (address 0x00b2) and data8 (data 0x47).
  //
  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = DMI_PNP_50_57;
  SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
  SmmEntry->SmmPort                       = SoftwareSmiPort;
  SmmEntry->SmmData                       = SMM_PnP_BIOS_CALL;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;


  //
  // Assign USB SMI Port/date..INFO .
  //
  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = GET_USB_BOOT_INFO;
  SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
  SmmEntry->SmmPort                       = SoftwareSmiPort;
  SmmEntry->SmmData                       = USB_TX_SMI;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;

  //
  // This entry will be port16 (address 0x00b2) and data8 (data 0x48).
  //
  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = TCG_MOR_SMM_FUN;
  SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
  SmmEntry->SmmPort                       = SoftwareSmiPort;
  SmmEntry->SmmData                       = SMM_TCG_MOR_CALL;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;
  //
  // Assign ATA Legacy SMI Port/date..INFO .
  //
  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = ATA_LEGACY_SMM_FUN;
  SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
  SmmEntry->SmmPort                       = SoftwareSmiPort;
  SmmEntry->SmmData                       = ATA_LEGACY_SMI;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;

  Status = gBS->LocateProtocol (
                  &gSdPassThruProtocolGuid,
                  NULL,
                  (VOID **)&SdPassThru
                  );
  if (!EFI_ERROR (Status)) {
    SmmEntry->SmmAttributes.Type            = 0;
    SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
    SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
    SmmEntry->SmmFunction.Function          = SD_LEGACY_SMM_FUN;
    SmmEntry->SmmFunction.Owner             = STANDARD_OWNER;
    SmmEntry->SmmPort                       = SoftwareSmiPort;
    SmmEntry->SmmData                       = SD_LEGACY_SMI;
    SmmEntry ++;
    SoftIntData->NumSmmEntries ++;
  }

  ASSERT(SoftIntData->NumSmmEntries <= SMM_ENTRY_NUM);

  return EFI_SUCCESS;
}

