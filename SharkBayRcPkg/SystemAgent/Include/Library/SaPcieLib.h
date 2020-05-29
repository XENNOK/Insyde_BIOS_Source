/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SaPcieLib.h

@brief
  Defines and prototypes for the library module

**/
#ifndef _SA_PCIE_LIB_H_
#define _SA_PCIE_LIB_H_

//#include <EfiScriptLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/SaPlatformPolicy.h>

#define MAX_SUPPORTED_ROOT_BRIDGE_NUMBER  3
#define MAX_SUPPORTED_DEVICE_NUMBER       192
#define L0_SET                            BIT0
#define L1_SET                            BIT1
#define MAX_PAM_REG_COUNT                 7
#define PAM_REG_BASE                      0x80
#define CRID_DATA                         0x69
#define CRID_LOCK                         0x17

typedef struct {
  UINT64                   Address;
  S3_BOOT_SCRIPT_LIB_WIDTH Width;
  UINT32                   Value;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

EFI_STATUS
EnumerateAllPcieDevices (
  VOID
  )
/**

  Enumerate all end point devices connected to root bridge ports and record their MMIO base address

  @param[in] None

  @exception EFI_UNSUPPORTED      PCIe capability structure not found
  @retval    EFI_SUCCESS          All done successfully

**/
;

VOID
SaPcieConfigBeforeOpRom (
  VOID
  )
/**

  Sets Common Clock, TCx-VC0 mapping, and Max Payload for PCIe

  @param[in] None

  @retval None

**/
;

VOID
SaAspm (
  VOID
  )
/**

  This function does all SA ASPM initialization

  @param[in] None

  @retval None

**/
;

VOID
EnableExtendedTag (
  VOID
  )
/**

  This function checks PEG end point device for extended tag capability and enables them if they are.

  @param[in] None

  @retval None

**/
;

VOID
SaSaveRestoreChipset (
  IN     BOOLEAN                       IsSaving,
  IN     BOOT_SCRIPT_PCI_REGISTER_SAVE PciRegistersSaveTable[],
  IN     UINTN                         PciRegistersSaveTableSize,
  IN OUT UINTN                         *PciRegistersSaveBuffer
)
/**

  This function saves/restores Chipset registers

  @param[in] IsSaving                  - TRUE for saving and FALSE for restoring
  @param[in] PciRegistersSaveTable[]   - The register table that has to be saved/restored
  @param[in] PciRegistersSaveTableSize - Size of above table
  @param[in] PciRegistersSaveBuffer    - A saving/restoring buffer for those register settings.

  @retval None

**/
;

VOID
SaSaveRestorePlatform (
  IN BOOLEAN        IsSaving
)
/**

  This function saves/restores platform relative registers

  @param[in] IsSaving                  - TRUE for saving and FALSE for restoring

  @retval None

**/
;

VOID
SaSecurityLock (
  VOID
)
/**

  This function does SA security lock

  @param[in] None

  @retval None

**/
;

VOID
SaS3Resume (
  VOID
)
/**

  This function handles SA S3 resume

  @param[in] None

  @retval None

**/
;

VOID
SaS3ResumeCallback (
  VOID
)
/**

  Wrapper function for all SA S3 resume tasks which can be a callback function.

  @param[in] None

  @retval None

**/
;

VOID
SaPcieConfigAfterOpRom (
  VOID
)
/**

  Wrapper function for all SA ASPM tasks and extended tag which can be a callback function.

  @param[in] None

  @retval None

**/
;

VOID
SaPcieEnumCallback (
  VOID
)
/**

  Wrapper function for all SA enumeration tasks which can be a callback function.

  @param[in] None

  @retval None

**/
;

VOID
SaPcieInitPolicy (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
)
/**

  This function will initialize all required platform policy into global veriables so no need to locate policy protocol during runtime.

  @param[in] None

  @retval None

**/
;

EFI_STATUS
SaScriptMemWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  UINTN                             Count,
  IN  VOID                              *Buffer
)
/**

  Wrapper for boot script with opcode EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE

  @param[in] Width   - The width of the memory operations.
  @param[in] Address - The base address of the memory operations.
  @param[in] Count   - The number of memory operations to perform.
  @param[in] Buffer  - The source buffer from which to write the data.

  @retval EFI_NOT_FOUND - BootScriptSave Protocol not exist.
  @retval EFI_STATUS - BootScriptSave Protocol exist, always returns EFI_SUCCESS

**/
;

EFI_STATUS
SaScriptMemReadWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  VOID                              *Data,
  IN  VOID                              *DataMask
)
/**

  Wrapper for boot script with opcode EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE

  @param[in] Width   - The width of the memory operations.
  @param[in] Address - The base address of the memory operations.
  @param[in] Data    - A pointer to the data to be OR-ed.
  @param[in] DataMask  - A pointer to the data mask to be AND-ed with the data read from the register.

  @retval EFI_NOT_FOUND - BootScriptSave Protocol not exist.
  @retval EFI_STATUS - BootScriptSave Protocol exist, always returns EFI_SUCCESS

**/
;

EFI_STATUS
SaScriptMemPoll (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  VOID                              *BitMask,
  IN  VOID                              *BitValue,
  IN  UINTN                             Duration,
  IN  UINTN                             LoopTimes
  )
/**

  Wrapper for boot script for Polling one memory mapping register

  @param[in] Width     - The width of the memory operations.
  @param[in] Address   - The base address of the memory operations.
  @param[in] BitMask   - A pointer to the bit mask to be AND-ed with the data read from the register.
  @param[in] BitValue  - A pointer to the data value after to be Masked.
  @param[in] Duration  - Duration in microseconds of the stall.
  @param[in] LoopTimes - The times of the register polling.

  @retval EFI_SUCCESS - The operation was executed successfully

**/
;

#endif
