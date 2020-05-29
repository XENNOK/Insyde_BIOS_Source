/** @file
  Memory Thermal Initialization Driver.
   
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.
**/

#include "MemoryThermal.h"

EFI_STATUS
MemoryThermalEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                         Status;
  UINTN                              BufferSize;
  CHIPSET_CONFIGURATION              *SetupNvData;
  EFI_GUID                           GuidId = SYSTEM_CONFIGURATION_GUID;
  UINT16                             Data16;
  UINT32                             Data32;
  
  Status = EFI_SUCCESS;
  BufferSize = 0;
  Data16 = 0;
  Data32 = 0;
  SetupNvData = NULL;
  
//[-start-130709-IB05160465-modify]//
  BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  SetupNvData = AllocatePool (BufferSize);
  
  Status = gRT->GetVariable (
             L"Setup",
             &GuidId,
             NULL,
             &BufferSize,
             ( VOID * )(SetupNvData)
             );
  ASSERT_EFI_ERROR ( Status );
  
  Data32 = (BIT0 << PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_OFF);

  if (SetupNvData->ExternThermalStatus) {
    Data32 |= (BIT0 << PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_OFF);
  }
  if (SetupNvData->ClosedLoopThermalManage) {
    Data32 |= (BIT0 << PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_OFF);
  }
  if (SetupNvData->OpenLoopThermalManage) {
    Data32 |= (BIT0 << PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_OFF);
  }  
  McMmio32Or (PCU_CR_DDR_PTM_CTL_PCU_REG, Data32);  

  Data16 = (SetupNvData->WarmThresholdCh0Dimm0 << PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_OFF) |
           (SetupNvData->WarmThresholdCh0Dimm1 << PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_OFF);
  McMmio16AndThenOr(PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_REG, 0,Data16);
  
  Data16 = (SetupNvData->WarmThresholdCh1Dimm0 << PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_OFF) | 
           (SetupNvData->WarmThresholdCh1Dimm1 << PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_OFF);
  McMmio16AndThenOr(PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_REG, 0,Data16);
  

  Data16 = (SetupNvData->HotThresholdCh0Dimm0 << PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_OFF) | 
           (SetupNvData->HotThresholdCh0Dimm1 << PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_OFF);
  McMmio16AndThenOr(PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_REG, 0,Data16);
  
  Data16 = (SetupNvData->HotThresholdCh1Dimm0 << PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_OFF) | 
           (SetupNvData->HotThresholdCh1Dimm1 << PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_OFF);
  McMmio16AndThenOr (PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_REG, 0, Data16);

  Data16 = (SetupNvData->WarmBudgetCh0Dimm0 << PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_OFF) | 
           (SetupNvData->WarmBudgetCh0Dimm1 << PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_OFF);
  McMmio16AndThenOr (PCU_CR_DDR_WARM_BUDGET_CH0_PCU_REG, 0, Data16);

  Data16 = (SetupNvData->WarmBudgetCh1Dimm0 << PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_OFF) | 
           (SetupNvData->WarmBudgetCh1Dimm1 << PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_OFF);
  McMmio16AndThenOr (PCU_CR_DDR_WARM_BUDGET_CH1_PCU_REG, 0, Data16);

  Data16 = (SetupNvData->HotBudgetCh0Dimm0 << PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_OFF) | 
           (SetupNvData->HotBudgetCh0Dimm1 << PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_OFF);
  McMmio16AndThenOr (PCU_CR_DDR_HOT_BUDGET_CH0_PCU_REG, 0, Data16);
 
  Data16 = (SetupNvData->HotBudgetCh1Dimm0 << PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_OFF) | 
           (SetupNvData->HotBudgetCh1Dimm1 << PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_OFF);
  McMmio16AndThenOr (PCU_CR_DDR_HOT_BUDGET_CH1_PCU_REG, 0, Data16);
  
  Data32 = (SetupNvData->RaplPL1Power & PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_MSK) | 
            (SetupNvData->RaplPL1Enable << PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_OFF) |
            (SetupNvData->RaplPL1WindowX << PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_OFF) |
            (SetupNvData->RaplPL1WindowY << PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_OFF);         
  McMmio32Or (PCU_CR_DDR_RAPL_LIMIT_PCU_REG, Data32);

  Data32 = (SetupNvData->RaplPL2Power & PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_MSK) | 
            (SetupNvData->RaplPL2Enable << (PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_OFF - 32)) |
            (SetupNvData->RaplPL2WindowX << (PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_OFF - 32)) |
            (SetupNvData->RaplPL2WindowY << (PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_OFF - 32));
          
  McMmio32Or (PCU_CR_DDR_RAPL_LIMIT_PCU_REG + 4, Data32);

  Data32 = McMmio32(PCU_CR_DDR_RAPL_LIMIT_PCU_REG + 4);
  Data32 |= (SetupNvData->RaplPLLock << (PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_OFF - 32));
  McMmio32Or (PCU_CR_DDR_RAPL_LIMIT_PCU_REG + 4, Data32);

  gBS->FreePool (SetupNvData); 
  return EFI_SUCCESS;

}
