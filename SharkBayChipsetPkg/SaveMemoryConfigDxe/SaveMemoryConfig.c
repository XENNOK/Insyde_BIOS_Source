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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <MemInfoHob.h>
//#ifdef DDR3LV_SUPPORT
//[-start-121214-IB10820195-remove]//
//#include <PlatformBaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <Guid/DDR3LVoltageHob.h>
//#endif

#include <Protocol/SaveMemoryConfigDone.h>
#include <Guid/MrcS3RestoreVariable.h> 

EFI_STATUS
EFIAPI
SaveMemoryConfigEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 Handle;
  EFI_GUID                   MemRestoreDataGuid = EFI_MEMORY_RESTORE_DATA_GUID;
  UINTN                      S3DataPtrSize;
  VOID                       *S3DataPtr;
  EFI_HOB_GUID_TYPE          *GuidHob;
  VOID                       *MemRestoreDataPtr;
  HOB_SAVE_MEMORY_DATA       *HOBSaveMemoryData;
//#ifdef DDR3LV_SUPPORT
  DDR3L_VOLTAGE_SETTING      *DDR3LVoltageDate;
//[-start-121019-IB05330384-remove]//
//  UINT32                     GPIO31_0;
//  UINT32                     GPIO63_32;
//  UINT32                     GPIO95_64;
//[-end-121019-IB05330384-remove]//
  
  DDR3LVoltageDate  = NULL;
//#endif  
  Handle            = NULL;
  S3DataPtrSize     = 0;
  S3DataPtr         = NULL;
  GuidHob           = NULL;
  MemRestoreDataPtr = NULL;
  HOBSaveMemoryData = NULL;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSave Memory Config Entry\n"));

  //
  // Search for the Memory Restore Data GUID HOB.
  //
  GuidHob = NULL;
  GuidHob = GetFirstGuidHob (&MemRestoreDataGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob != NULL) {
    MemRestoreDataPtr = (UINT8 *)(UINTN)(GET_GUID_HOB_DATA (GuidHob));
  }
  HOBSaveMemoryData = BASE_CR ( MemRestoreDataPtr, HOB_SAVE_MEMORY_DATA, MrcData );

  S3DataPtrSize = sizeof (SysSave);
  S3DataPtr     = (VOID *) &(HOBSaveMemoryData->MrcData.SysSave);
  Status = gRT->SetVariable (
                  L"MrcS3RestoreVariable",
                  &gMrcS3RestoreVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  S3DataPtrSize,
                  S3DataPtr
                  );
//#ifdef DDR3LV_SUPPORT
    //
    //  Keep GPIO Setting for Warm boot 
    //  1. Save GPIO date to Hob
    //  2. Save to variable in DEX phase.
    //  3. Before Memoryinit doen, system will check GPIO setting for DDR3L
    // 
    //  This is step 2.
    //
  if (FeaturePcdGet(PcdDdr3lvSupported)) {    
    GuidHob = GetFirstGuidHob (&gDDR3LVoltageHobGuid);
    if (GuidHob != NULL) {
      DDR3LVoltageDate = (DDR3L_VOLTAGE_SETTING *) ((UINT8 *)GuidHob + (sizeof(EFI_HOB_GUID_TYPE)));

//[-start-121019-IB05330384-remove]//
//      GPIO31_0 = IoRead32((UINT16)(PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_LVL));
//      GPIO63_32 = IoRead32((UINT16)(PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_LVL2));
//      GPIO95_64 = IoRead32((UINT16)(PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_LVL3));
//      DDR3LVoltageDate->GPIOPingSetting[0] = GPIO31_0 & DDR3LVoltageDate->GPIOPingSelect[0];
//      DDR3LVoltageDate->GPIOPingSetting[1] = GPIO63_32 & DDR3LVoltageDate->GPIOPingSelect[1];
//      DDR3LVoltageDate->GPIOPingSetting[2] = GPIO95_64 & DDR3LVoltageDate->GPIOPingSelect[2];
//[-end-121019-IB05330384-remove]//
      Status = gRT->SetVariable (
               L"DDR3LVoltageVariable",
               &gDDR3LVoltageVariableGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               sizeof(DDR3L_VOLTAGE_SETTING),
               DDR3LVoltageDate
               );
    }
  }
//#endif  
  //
  // Set DISB
  //
  PchLpcPciCfg8AndThenOr (R_PCH_LPC_GEN_PMCON_2 , 0, B_PCH_LPC_GEN_PMCON_DRAM_INIT);

  //
  // Install Save Memory Config Done Protocol.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gSaveMemoryConfigDoneGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
