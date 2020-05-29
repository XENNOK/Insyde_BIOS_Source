/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <ThunderboltSmm.h>

EFI_GLOBAL_NVS_AREA            *mGlobalNvsArea;
EFI_SMM_SYSTEM_TABLE2          *mSmst;
EFI_SMM_VARIABLE_PROTOCOL      *mSmmVariable;

//
// Entry Point
//
EFI_STATUS
ThunderboltSmmEntryPoint (
  IN  EFI_HANDLE                ImageHandle,
  IN  EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_SMM_BASE2_PROTOCOL        *SmmBase2;
  BOOLEAN                       InSmm;
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  CHIPSET_CONFIGURATION          *SetupConfiguration;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
  EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
  EFI_HANDLE                    SwHandle;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase2);
  ASSERT_EFI_ERROR (Status);

  SmmBase2->InSmm (SmmBase2, &InSmm);
  if (!InSmm) {
    return EFI_SUCCESS;
  }

  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);

  SetupConfiguration = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);
//[-start-130709-IB05400426-add]//
  if (SetupConfiguration->TbtDevice == 0x00) {
    //
    // Thunderbolt disabled.
    //
    return EFI_SUCCESS;
  }
//[-end-130709-IB05400426-add]//

//[-start-120727-IB03780455-add]//
  SysConfig              = *SetupConfiguration;
  mReserveMemoryPerSlot  = SysConfig.ReserveMemoryPerSlot <<= 4;
  mReservePMemoryPerSlot = SysConfig.ReservePMemoryPerSlot <<= 4;
  mReserveIOPerSlot      = SysConfig.ReserveIOPerSlot <<= 2;
//[-end-120727-IB03780455-add]//

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsArea);
  ASSERT_EFI_ERROR (Status);

  mGlobalNvsArea = GlobalNvsArea->Area;

  Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch2);
  ASSERT_EFI_ERROR (Status);

  Status = mSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID **)&mSmmVariable);
  ASSERT_EFI_ERROR (Status);

  SwContext.SwSmiInputValue = THUNDERBOLT_SW_SMI;
  Status = SwDispatch2->Register (
                          SwDispatch2,
                          ThunderboltCallback,
                          &SwContext,
                          &SwHandle
                          );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

//[-start-130130-IB08620156-add]//
/**
  Returns the bit position of the highest bit set in a 32-bit value. Equivalent
  to log2(x).

  This function computes the bit position of the highest bit set in the 32-bit
  value specified by Operand. If Operand is zero, then -1 is returned.
  Otherwise, a value between 0 and 31 is returned.

  @param  Operand The 32-bit operand to evaluate.

  @return Position of the highest bit set in Operand if found.
  @retval -1  Operand is zero.

**/
INTN
EFIAPI
TbtHighBitSet32 (
  IN      UINT32                    Operand
  )
{
  INTN                              BitIndex;

  for (BitIndex = -1; Operand != 0; BitIndex++, Operand >>= 1);
  return BitIndex;
}
//[-end-130130-IB08620156-add]//

//[-start-130123-IB04770265-add]//
BOOLEAN
BitScanForward(
  UINT32  *Index,
  UINT32  Mask
) 
{
   INTN Postion;
   
//[-start-130130-IB08620156-modify]//
   Postion = TbtHighBitSet32 (Mask);
//[-end-130130-IB08620156-modify]//
   if (Postion == -1 ) {
      *Index = 0;
      return FALSE;
   } else {
     *Index = (UINT32) Postion;
     return TRUE;
   }
}
//[-end-130123-IB04770265-add]//
