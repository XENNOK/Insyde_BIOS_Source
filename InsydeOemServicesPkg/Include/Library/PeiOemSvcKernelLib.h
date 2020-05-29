/** @file
  Definition for Oem Services Default Lib.

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_OEM_SVC_KERNEL_LIB_H_
#define _PEI_OEM_SVC_KERNEL_LIB_H_

#include <Uefi.h>
#include <Ppi/VerbTable.h>
#include <Pi/PiBootMode.h>  
#include <Guid/PlatformHardwareSwitch.h>
#include <OemClkGen.h>
#include <Guid/MemoryTypeInformation.h>
#include <Sio/SioCommon.h>

typedef struct _FLASH_ENTRY {
  UINT32        WriteAddress;
  UINT32        WriteSize;
  UINT32        SourceOffset;
} FLASH_ENTRY;


//
// OemSvc function prototypes
//
EFI_STATUS
OemSvcBootModeCreateFv (
  IN  EFI_BOOT_MODE                         BootMode
  );

EFI_STATUS
OemSvcChangeBootMode (
  IN OUT EFI_BOOT_MODE                      *BootMode,
  OUT    BOOLEAN                            *SkipPriorityPolicy
  );

EFI_STATUS
OemSvcChipsetModifyClockGenInfo (
  OUT UINT8                                 *SlaveAddress,
  OUT UINT8                                 *StartOffset,
  OUT CLOCK_GEN_UNION_INFO                  **BigTable,
  OUT UINT16                                *RealPlatformId,
  OUT UINT16                                *SizeInfo
  );

EFI_STATUS
OemSvcGetProtectTable (
  OUT UINTN                                 *Count,
  OUT BOOLEAN                               *UseEcIdle,
  OUT FLASH_ENTRY                           **CrisisProtectTable
  );

EFI_STATUS
OemSvcSetRecoveryRequest (
  VOID		
  );

EFI_STATUS
OemSvcDetectRecoveryRequest (
  IN OUT BOOLEAN                            *IsRecovery
  );

EFI_STATUS
OemSvcGetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  );

EFI_STATUS
OemSvcHookWhenRecoveryFail (
  VOID		
  );

EFI_STATUS
OemSvcInitMemoryDiscoverCallback (
  IN OUT  EFI_BOOT_MODE                     *BootMode
  );

EFI_STATUS
OemSvcInitPlatformStage1 (
  VOID		
  );

EFI_STATUS
OemSvcIsBootWithNoChange (
  IN OUT BOOLEAN                            *IsNoChange
  );

EFI_STATUS
OemSvcInitPlatformStage2 (
  IN VOID                                   *Buffer,
  IN BOOLEAN                                SetupVariableExist
  );

EFI_STATUS
OemSvcModifySetupVarPlatformStage2 (
  IN OUT VOID                               *Buffer,
  IN     BOOLEAN                            SetupVariableExist
  );

EFI_STATUS
OemSvcSetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH              **PlatformHardwareSwitchDptr
  );
EFI_STATUS
OemSvcUpdateMemoryTypeInformation (
  IN OUT EFI_MEMORY_TYPE_INFORMATION           **MemoryTypeInformation,
  IN OUT UINT32                                *MemoryTableSize
  );

EFI_STATUS
OemSvcGetWaitTimerAfterHdaInit (
  OUT UINT16                                *WaitTimerAfterHdaInit
  );

EFI_STATUS
OemSvcBeforeInitSioPei (
  VOID
  );

EFI_STATUS
OemSvcRegSioPei (
  IN UINT8                                SioInstance,
  IN OUT EFI_SIO_TABLE                    **RegTable                          
  );

EFI_STATUS
OemSvcAfterInitSioPei (
  IN UINT8                                SioInstance,
  IN UINT16                                 IndexPort
  );

EFI_STATUS
OemSvcPeiCrisisRecoveryReset (
  );

#endif
