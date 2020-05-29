/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
    CsmInt10Block.c

Abstract:

    Block CSM INT10 after INT19

--*/
//[-start-120712-IB10820081-modify]// 
#include <Protocol/LegacyBios.h>
#include <Protocol/Cpu.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BaseMemoryLib.h>
//[-start-130403-IB09720138-remove]//
//[-start-120725-IB10820090-modify]//
// #include <Protocol/ChipsetLibServices.h>
//[-end-120725-IB10820090-modify]//
//[-end-130403-IB09720138-remove]//
//[-start-130403-IB09720138-add]//
#include <Library/DxeChipsetSvcLib.h>
//[-end-130403-IB09720138-add]//
#include <Guid/EventLegacyBios.h>

extern EFI_GUID  gEfiPerformanceFileGuid;
//[-end-120712-IB10820081-modify]// 

STATIC
VOID
InstallPerfTestOpRom (
  IN EFI_EVENT           Event,
  IN VOID                *Handle
  );

/**

  Register gEfiResetArchProtocolGuid protocol notification to hook gRT reset function.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE pointer

  @retval EFI_SUCCESS           Driver Dispatch success

**/
EFI_STATUS
EFIAPI
CsmInt10BlockInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT                             LegacyBootEvent;  
  EFI_BOOT_MODE                         BootMode;

  //
  // Install this option ROM for fast boot modes only
  //
  BootMode = GetBootModeHob();
  
  switch (BootMode) {

  case BOOT_WITH_MINIMAL_CONFIGURATION:
//[-start-120321-IB06460378-remove]//
//  case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
//[-end-120321-IB06460378-remove]//
    break;

//[-start-120321-IB06460378-add]//
  case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
//[-end-120321-IB06460378-add]//
  case BOOT_ON_FLASH_UPDATE:
  case BOOT_IN_RECOVERY_MODE:
  case BOOT_WITH_FULL_CONFIGURATION:
  case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
  case BOOT_WITH_DEFAULT_SETTINGS:
  default:
    return EFI_SUCCESS;
    break;
  }

    //
    // For UEFI 2.0 and the future use an Event Group
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    InstallPerfTestOpRom,
                    NULL,
                    &gEfiEventLegacyBootGuid,
                    &LegacyBootEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }


  return EFI_SUCCESS;
}

/**

  Install a option ROM to block all CSM INT10 function call

  @param  Event           the event object
  @param  Handle         

  @retval   Does not return if the OEM INT15 reset takes place.
                EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
STATIC
VOID
InstallPerfTestOpRom (
  IN EFI_EVENT           Event,
  IN VOID                *Handle
  )
{
  EFI_STATUS                   Status;
  EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;
  EFI_IA32_REGISTER_SET        Regs;
  VOID                         *Table;
  UINTN                        TableSize;
  UINTN                        TablePtr;

  UINT64                       CurrentTicker;
  UINT64                       TimerPeriod;  
  UINT32                       *Freq;
  EFI_CPU_ARCH_PROTOCOL        *Cpu;
  UINT32                       *EndOfVTS;
  VOID						   *LegacyRegion = NULL;
//[-start-130403-IB09720138-remove]//
//[-start-120725-IB10820090-add]//  
//    CHIPSET_LIB_SERVICES_PROTOCOL  *ChipsetLibServices;
//[-end-120725-IB10820090-add]//
//[-end-130403-IB09720138-remove]//

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);
  if (EFI_ERROR (Status)) {
    return;
  }
//[-start-130403-IB09720138-remove]//
//[-start-120725-IB10820090-add]//  
//   Status = gBS->LocateProtocol (&gChipsetLibServicesProtocolGuid, NULL, &ChipsetLibServices);
//   if (EFI_ERROR (Status)) {
//     return;
//   }
//[-end-120725-IB10820090-add]//  
//[-end-130403-IB09720138-remove]//
  //
  // Get Cpu Frequency
  //
  Cpu->GetTimerValue (Cpu, 0, &(CurrentTicker), &TimerPeriod);
  TimerPeriod   = DivU64x32 (1000000000000, (UINT32) TimerPeriod);
  Freq = (UINT32 *)(UINTN)(0xE0000);
  *Freq = (UINT32)TimerPeriod;

  //
  //INT13 counts for End of VTS 
  //
  EndOfVTS = (UINT32 *)(UINTN)(0xE0010);
  *EndOfVTS = 0x473;

  Status = GetSectionFromFv (
             &gEfiPerformanceFileGuid,
             EFI_SECTION_RAW,
             0,
             &Table,
             &TableSize
             );
  if (EFI_ERROR (Status)) {
    return;
  } 
//[-start-120725-IB10820090-modify]//
//  Status = ChipsetLibServices->LegacyRegionAccessCtrl(0xE0000, 0x20000, LEGACY_REGION_ACCESS_UNLOCK); 
  Status = DxeCsSvcLegacyRegionAccessCtrl (0xE0000, 0x20000, LEGACY_REGION_ACCESS_UNLOCK);
//[-end-120725-IB10820090-modify]//

  //
  // To find the required size of availabe free memory
  //
  Status = LegacyBios->GetLegacyRegion (
                        LegacyBios,
                        TableSize,
                        0x0002,  // 0xE0000 block
                        0x01,
                        &LegacyRegion
                        );

  //
  // To copy the image to legacy memory
  //
  TablePtr =(UINTN)LegacyRegion;                      

  CopyMem((VOID *)TablePtr,
             Table,
             TableSize
             );

  //
  // call into our BIN file for change the INT15
  //
  LegacyBios->FarCall86 (
                LegacyBios,
                (UINT16)(TablePtr >> 4),
                0x03,
                &Regs,
                NULL,
                0
                );
//[-start-130403-IB09720138-modify]//
//[-start-120725-IB10820090-modify]//  
//   Status = ChipsetLibServices->LegacyRegionAccessCtrl(0xE0000, 0x20000, LEGACY_REGION_ACCESS_LOCK);
  Status = DxeCsSvcLegacyRegionAccessCtrl (0xE0000, 0x20000, LEGACY_REGION_ACCESS_LOCK);
//[-end-120725-IB10820090-modify]//
//[-end-130403-IB09720138-modify]//
  return;
}
