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

#define MAX_BBS_ENTRIES 0x100

/**
  Attempt to legacy boot the BootOption. If the EFI contexted has been
  compromised this function will not return.

  @param  This                   The protocol instance pointer.
  @param  BbsDevicePath          The EFI Device Path from BootXXXX variable.
  @param  BbsTable               The Internal BBS table.
  @param  LoadOptionSize         The size of LoadOption in size.
  @param  LoadOption             The LoadOption from BootXXXX variable
  @param  EfiToLegacy16BootTable A pointer to BootTable structure

  @retval EFI_SUCCESS           Ready to boot.

**/
EFI_STATUS
EFIAPI
PrepareToBoot (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  BBS_BBS_DEVICE_PATH                         *BbsDevicePath,
  IN  VOID                                        *BbsTable,
  IN  UINT32                                      LoadOptionsSize,
  IN  VOID                                        *LoadOptions,
  IN  EFI_TO_COMPATIBILITY16_BOOT_TABLE           *EfiToLegacy16BootTable
  )  
{
  UINT16                          DevicePathType;
  UINT16                          Index;
  UINT16                          Priority;
  BBS_TABLE                       *RegionalBbsTable;
  DEVICE_PRODUCER_DATA_HEADER     *SuperIoPtr;
  EFI_STATUS                      Status;
  UINTN                           SlpStringLength;
  UINTN                           SlpStringAddress;

  Priority = 0;

  //
  // Set how Gate A20 is gated by hardware
  //
  SuperIoPtr = &EfiToLegacy16BootTable->SioData;
  SuperIoPtr->Flags.A20Kybd   = 1;
  SuperIoPtr->Flags.A20Port90 = 1;
  SuperIoPtr->MousePresent    = 1;

  RegionalBbsTable = (BBS_TABLE *) BbsTable;

  //
  // There are 2 cases that must be covered.
  // Case 1: Booting to a legacy OS - BbsDevicePath is non-NULL.
  // Case 2: Booting to an EFI aware OS - BbsDevicePath is NULL.
  //         We need to perform the PrepareToBoot function to assign
  //         drive numbers to HDD devices to allow the shell or EFI
  //         to access them.
  //
  if (BbsDevicePath != NULL) {
    DevicePathType = BbsDevicePath->DeviceType;
  } else {
    DevicePathType = BBS_HARDDISK;
  }
  //
  // Skip the boot devices where priority is set by BDS and set the next one
  //
  for (Index = 0; Index < MAX_BBS_ENTRIES; Index++) {
    if ((RegionalBbsTable[Index].BootPriority != BBS_UNPRIORITIZED_ENTRY) &&
        (RegionalBbsTable[Index].BootPriority != BBS_IGNORE_ENTRY) &&
        (RegionalBbsTable[Index].BootPriority != BBS_LOWEST_PRIORITY) &&
        (Priority <= RegionalBbsTable[Index].BootPriority)
        ) {
      Priority = (UINT16) (RegionalBbsTable[Index].BootPriority + 1);
    }
  }

  switch (DevicePathType) {
    
  case BBS_FLOPPY:
  case BBS_HARDDISK:
  case BBS_CDROM:
  case BBS_EMBED_NETWORK:
    for ( Index = 0; Index < MAX_BBS_ENTRIES; Index ++) {
      if ((RegionalBbsTable[Index].BootPriority == BBS_UNPRIORITIZED_ENTRY) &&
          (RegionalBbsTable[Index].DeviceType == DevicePathType)
          ) {
        RegionalBbsTable[Index].BootPriority = Priority;
        ++ Priority;
      }
    }
    break;

  case BBS_BEV_DEVICE:
    for ( Index = 0; Index < MAX_BBS_ENTRIES; Index ++) {
      if ((RegionalBbsTable[Index].BootPriority == BBS_UNPRIORITIZED_ENTRY) &&
          (RegionalBbsTable[Index].Class == 01) &&
          (RegionalBbsTable[Index].SubClass == 01)
          ) {
        RegionalBbsTable[Index].BootPriority = Priority;
        ++ Priority;
        }
      }
      break;

  case BBS_USB:
  case BBS_PCMCIA:
  case BBS_UNKNOWN:
  default:
    break;
  }
  //
  // Set priority for rest of devices
  //
  for (Index = 0; Index < MAX_BBS_ENTRIES; Index ++) {
    if (RegionalBbsTable[Index].BootPriority == BBS_UNPRIORITIZED_ENTRY) {
      RegionalBbsTable[Index].BootPriority = Priority;
      ++ Priority;
    }
  }

  //
  // OemServices
  //
  Status = OemSvcInstallLegacyBiosOemSlp (
             &SlpStringLength,
             &SlpStringAddress
             );

  return  EFI_SUCCESS;
}

