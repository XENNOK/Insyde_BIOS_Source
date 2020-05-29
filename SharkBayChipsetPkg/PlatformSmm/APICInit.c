/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Platform.h>
#include <Library/BaseLib.h>
//[-start-121214-IB10820196-add]//
#include <CpuRegs.h>
#include <Register/IoApic.h>
//[-end-121214-IB10820196-add]//

//[-start-121214-IB10820196-remove]//
//define EFI_MSR_IA32_APIC_BASE          0x1B
//#define EFI_APIC_GLOBAL_ENABLE          0x800
//#define APIC_INDEX_REG                  0xfec00000
//#define APIC_DATA_REG                   0xfec00010
//[-end-121214-IB10820196-remove]//
/**
 This function initializes the APIC for S3 resume.

 @param [in]   SetupNVRam       Data from NVRam.

 @retval EFI_SUCCESS            Returned if Apic registers are set accordingly.
 @retval EFI_UNSUPPORTED        Apic mode unsupported.

**/
EFI_STATUS
APICInit (
  IN CHIPSET_CONFIGURATION                  *SetupNVRam
  )
{
  UINT64                        TempQword;
  UINTN                         *ApicAddr;
  UINTN                         Buff;

  if (SetupNVRam->IoApicMode == 0x0) {
    return EFI_UNSUPPORTED;
  }

  TempQword =AsmReadMsr64(MSR_IA32_APIC_BASE);
  AsmWriteMsr64(MSR_IA32_APIC_BASE, TempQword | APIC_GLOBAL_ENABLE);

  //Set APIC direct register (Boot configuration)
  ApicAddr = (UINTN *)(UINTN)(PcdGet32 (PcdIoApicBaseAddress) + IOAPIC_INDEX_OFFSET);
  Buff = 3;
  *ApicAddr = Buff;
  ApicAddr = (UINTN *)(UINTN)(PcdGet32 (PcdIoApicBaseAddress) + IOAPIC_DATA_OFFSET);
  Buff = *ApicAddr;
  Buff |= 1;
  *ApicAddr = Buff;
  return EFI_SUCCESS;
}
