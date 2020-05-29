/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Platform.h

Abstract:

  Header file for Smm platform driver

Revision History

--*/

#ifndef _SMM_PLATFORM_H_
#define _SMM_PLATFORM_H_

#include <Uefi.h>
#include <Pi/PiSmmCis.h>
#include <ChipsetSetupConfig.h>
#include <PostCode.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Protocol/SetupUtility.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch.h>
#include <Protocol/SmmPowerButtonDispatch.h>
#include <Protocol/SmmIchnDispatch.h>
//[-start-120731-IB10820094-modify]//
#include <Protocol/SmmThunk.h>
//[-end-120731-IB10820094-modify]//
#include <Protocol/SmmIchnDispatchEx.h>
#include <Protocol/GlobalNvsArea.h>

typedef struct _SAVE_RESTORE_MSR_PARAMETER {
  BOOLEAN SaveRestoreFlag;
  UINT8   CpuLogicalID;
} SAVE_RESTORE_MSR_PARAMETER;
typedef enum {
  S0 = 0,
  S5,
  LastState,
  MaxStateAfterG3Type
} STATE_AFTER_G3_TYPE;

extern  UINT16                          mAcpiBaseAddr;
extern  EFI_SMM_SYSTEM_TABLE2           *mSmst;
extern  EFI_GLOBAL_NVS_AREA             *mGlobalNvsArea;
extern  EFI_SMM_THUNK_PROTOCOL          *mSmmThunk;  

#define CPU_NUM                         8

#define FAMILYMODEL_DOTHAN              0x06D0        // Dothan family/model code.
#define FAMILYMODEL_YONAH               0x06E0        // Yonah family/model code.
#define FAMILYMODEL_MEROM               0x06F0        // Merom family/model code
#define FAMILYMODEL_PENRYN              0x0670        // Penryn family/model code
#define FAMILYMODEL_MASK                0x0FF0        // Family/Model Code.

//[-start-121120-IB09890039-remove]//
//#define EFI_OEM_FUNCTION_SW_SMI         0xB2 
//[-end-121120-IB09890039-remove]//

#define RTC_ADDRESS_SECONDS_ALARM       0x01
#define RTC_ADDRESS_MINUTES_ALARM       0x03
#define RTC_ADDRESS_HOURS_ALARM         0x05
#define RTC_ADDRESS_DAY_OF_THE_MONTH    0x07
#define RTC_ADDRESS_MONTH               0x08
#define RTC_ADDRESS_YEAR                0x09
#define RTC_ADDRESS_REGISTER_B          0x0B
#define ALARM_INTERRUPT_ENABLE          0x20
//[-start-120413-IB07360188-add]//
#define S5_WAKE_ON_DAY_OF_MONTH         0x02
//[-end-120413-IB07360188-add]//

#define PM1_EN_HIGH_BYTE                0x03
#define RTC_EVENT_ENABLE                0x04

//
// IO Trap
//

#define PCAT_RTC_ADDRESS_REGISTER       0x70
#define PCAT_RTC_DATA_REGISTER          0x71
#define RTC_ADDRESS_REGISTER_D          0x0D          // Bit 0 to Bit 5 = S5 Wake up day of month


#define PCH_GP_LVL2                     0x38

#define P0_CST_REQ                      0x809         // P0 C-state request port.
#define MSR_IO_BASE_ADDR                0x00E3        // I/O C-state redirection base.
#define   IO_BASE_ADDR_MASK             0xFFFFFFFF
#define   IO_BASE_PMB0_MASK             0x00FF        // PMB0 I/0 base mask.
#define   IO_BASE_PMB1_MASK             0xFF00        // PMB0 I/0 base mask.
#define   IO_CAPT_ADDR_MASK             0x000000FF

extern CHIPSET_CONFIGURATION             *SetupVariable;

#define AHCI_BASE                       (MmPci32 (0,DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SATA,PCI_FUNCTION_NUMBER_PCH_SATA,R_PCH_SATA_AHCI_BAR))                                                                       
#define McAhciAddress(Register)         ((UINTN) AHCI_BASE + (UINTN) (Register))
#define McAhci32Ptr(Register)           ((volatile UINT32*) McAhciAddress (Register))
#define McAhci32(Register)              *McAhci32Ptr (Register)

#define CACHE_LINE_64_BYTE              0x10
#define CACHE_LINE_16_BYTE              0x04
#define CACHE_LINE_SIZE_MAX             CACHE_LINE_64_BYTE
#define CACHE_LINE_SIZE_MIN             CACHE_LINE_16_BYTE

#define NoLVDS                          0x00
#define IntLVDS                         0x01
#define SdvoLVDS                        0x02
#define eDP                             0x03
#define NoHook                          0xFF

//#ifdef MXM_SUPPORT




//
// CRID definition
//
#define CRID_SELECT_KEY_VALUE_PCH       0x1D
#define CRID_SELECT_KEY_VALUE_CPU_ARD   0x00   // to be determine (waiting for correct value)
#define CRID_SELECT_KEY_VALUE_CPU_CFD   0x00   // to be determine (waiting for correct value)

//
// Callback function prototypes
//

//[-start-121109-IB05280008-modify]//
VOID
EFIAPI
ChipsetPowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   *DispatchContext
  );
//[-end-121109-IB05280008-modify]//
VOID
EFIAPI
GpioUnlockCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchExContext
  );

VOID
EFIAPI
S5SleepWakeOnTimeCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

VOID
EFIAPI
S5SleepWakeOnLanCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

VOID
EFIAPI
S5SleepAcLossCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

VOID
EFIAPI
S3SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );


EFI_STATUS
EFIAPI
PlatformEnableAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );

EFI_STATUS
EFIAPI
PlatformDisableAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );

EFI_STATUS
EFIAPI
PlatformRestoreAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );
  
EFI_STATUS
EFIAPI
PlatformRestoreAcpiCallbackDone (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );

VOID
EFIAPI
DummyTco1Callback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT *DispatchContext
  );

VOID
EFIAPI
PmeCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT *DispatchContext
  );

VOID
SetAfterG3On (
  BOOLEAN Enable
  );

VOID
EFIAPI
SmmReadyToBootFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

//
// SMBIOS call back
//
EFI_STATUS
EFIAPI
PnPBiosCallback (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  );

EFI_STATUS
EFIAPI
Int15D042Callback (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  );

//[-start-130809-IB06720232-remove]//
//BOOLEAN
//PowerStateIsAc (
//  VOID
//  );
//[-end-130809-IB06720232-remove]//

//
// Save/Restore Function Prototype
//
EFI_STATUS
SaveRestoreState (
  IN BOOLEAN                        SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreSio (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestorePci (
  IN  BOOLEAN                       SaveRestoreFlag
  );


//[-start-130808-IB06720232-modify]//
//EFI_STATUS
//SaveRestoreKbc (
//  IN  BOOLEAN                       SaveRestoreFlag
//  );
//[-end-130808-IB06720232-modify]//

EFI_STATUS
SaveRestoreAbar (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestorePic (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreLevelEdge (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestorePmIo (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreGpio (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreBSPState (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
EFIAPI
SaveRestoreAPState (
  IN  VOID                          *FlagBuffer
  );

EFI_STATUS
EFIAPI
SaveRestoreMtrr (
  IN  VOID                          *FlagBuffer
  );

EFI_STATUS
EFIAPI
SaveRestoreEMRR (
  IN      VOID                      *FlagBuffer
  );

/**
 Load all microcode updates to memory. Since in S3 resume boot path, CPUs should be
 patched again, these microcode updates are copied to OS reserved memory.

 @param [in]   IsBsp
 @param [in]   MicrocodeAddress
 @param [in]   FastPath

 @retval EFI_SUCCESS            All microcode updates are loaded to memory successfully
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to accomodate all the microcode updates

**/
EFI_STATUS
LoadAllMicrocodeUpdates (
  IN      BOOLEAN                    IsBsp,
  IN      UINT32                     *MicrocodeAddress,
  IN      BOOLEAN                    FastPath
  );


EFI_STATUS
DmaInit ();

EFI_STATUS
APICInit (
  IN CHIPSET_CONFIGURATION           *SetupNVRam
  );
EFI_STATUS
SaveRestoreDmi (
  IN  BOOLEAN                       SaveRestoreFlag
  );

//[-start-130130-IB05280043-remove]//
//EFI_STATUS
//SaveRestoreRcba (
//  IN  BOOLEAN                       SaveRestoreFlag
//  );
//[-end-130130-IB05280043-remove]//

EFI_STATUS
SaveRestoreMch (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreEp (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveRestoreAzalia (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
SaveEDPReg (
  IN  BOOLEAN                       SaveRestoreFlag
  );

//[-start-121205-IB08050187-add]//
EFI_STATUS
SaveRestoreSpiReg (
  IN  BOOLEAN                       SaveRestoreFlag
  );
//[-end-121205-IB08050187-add]//

VOID
S5WakeUpSetting (
  IN UINT16                         AcpiBaseAddr,
  IN CHIPSET_CONFIGURATION           *SetupNVRam
  );

//[-start-121120-IB09890039-remove]//
//EFI_STATUS
//EFIAPI
//OemCallback(
//  IN  EFI_HANDLE                   DispatchHandle,
//  IN  CONST  VOID                  *DispatchContext,
//  IN  OUT  VOID                    *CommBuffer,
//  IN  OUT  UINTN                   *CommBufferSize
//);
//[-end-121120-IB09890039-remove]//


EFI_STATUS
SaveRestoreMsr (
  IN SAVE_RESTORE_MSR_PARAMETER     *SaveRestoreMsrFlag
  );

VOID
SaveCR0 (
  OUT  UINT32                       *Cr0Reg
  );

VOID
ResCR0 (
  IN  UINT32                        Cr0Reg
  );

VOID
EFIAPI
S1SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

VOID
EFIAPI
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

VOID
EFIAPI
S5SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );


UINT8
GetMaxBusNumber (
  VOID
  );

VOID
SetSgVariable (
  IN EFI_GLOBAL_NVS_AREA                *GlobalNvsArea
  );
VOID
OpenHdAudio (
  IN EFI_GLOBAL_NVS_AREA                *GlobalNvsArea
  );
VOID
CloseHdAudio (
  IN EFI_GLOBAL_NVS_AREA                *GlobalNvsArea
  );

UINT32
ConvertBcdTime (
  IN  UINT8 DecimalVal
  );
  
EFI_STATUS
EFIAPI
GetAlarmRtcTime (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  );

//[-start-120508-IB03780437-add]//
VOID
UpdateTbtSxVariable (
  );
//[-end-120508-IB03780437-add]//
#endif
