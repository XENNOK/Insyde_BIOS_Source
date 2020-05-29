/** @file
  BdsDxe

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

/**
  Head file for BDS Architectural Protocol implementation

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BDS_MODULE_H_
#define _BDS_MODULE_H_

#include <FrameworkDxe.h>
#include <IndustryStandard/PeImage.h>
#include <IndustryStandard/SmBios.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/LegacyDevOrder.h>
#include <Guid/FileInfo.h>
#include <Guid/DataHubRecords.h>
#include <Guid/DebugMask.h>
#include <Guid/Performance.h>
#include <Guid/EventGroup.h>

#include <Protocol/DevicePath.h>
#include <Protocol/Bds.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DataHub.h>
#include <Protocol/SkipScanRemovableDev.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/MemoryErrorEvent.h>
#include <Protocol/Smbios.h>
#include <Protocol/EndOfDisableQuietBoot.h>
#include <Protocol/VariableLock.h>
#include <Protocol/Timer.h>
#include <Protocol/StartOfBdsDiagnostics.h>


#include <Library/UefiDriverEntryPoint.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/CapsuleLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/CmosLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/PlatformBdsLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/VariableLib.h>

#include <Library/DxeOemSvcKernelLib.h>
#include <PostCode.h>
#include "PerformanceTokens.h"

//
// Prototypes
//

/**

  Install Boot Device Selection Protocol

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  BDS has finished initializing.
                        Return the dispatcher and recall BDS.Entry
  @retval  Other        Return status from AllocatePool() or gBS->InstallProtocolInterface

**/
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  );

/**

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.

  @param This            Protocol Instance structure.

**/
VOID
EFIAPI
BdsEntry (
  IN  EFI_BDS_ARCH_PROTOCOL *This
  );

/**
  Perform the memory test base on the memory test intensive level,
  and update the memory resource.

  @param  Level         The memory test intensive level.

  @retval EFI_STATUS    Success test all the system memory and update
                        the memory resource

**/
EFI_STATUS
EFIAPI
BdsMemoryTest (
  IN EXTENDMEM_COVERAGE_LEVEL Level,
  IN UINTN                    BaseLine
  );

/**

  This routine is called to see if there are any capsules we need to process.
  If the boot mode is not UPDATE, then we do nothing. Otherwise find the
  capsule HOBS and produce firmware volumes for them via the DXE service.
  Then call the dispatcher to dispatch drivers from them. Finally, check
  the status of the updates.

  This function should be called by BDS in case we need to do some
  sort of processing even if there is no capsule to process. We
  need to do this if an earlier update went away and we need to
  clear the capsule variable so on the next reset PEI does not see it and
  think there is a capsule available.

  @param BootMode                 the current boot mode

  @retval EFI_INVALID_PARAMETER   boot mode is not correct for an update
  @retval EFI_SUCCESS             There is no error when processing capsule

**/
EFI_STATUS
EFIAPI
BdsProcessCapsules (
  EFI_BOOT_MODE BootMode
  );

#endif
