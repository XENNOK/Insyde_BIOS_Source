/** @file
  Thermal Monitor initialization.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "Thermal.h"
#include "Features.h"
#include <Library/BaseLib.h>

/**

  Initialize thermal monitor.


  @param[in] None

  @retval None
  
**/
VOID
InitializeThermalMonitor (
  VOID
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;
  UINT64              Data;

  ///
  /// Check the TM2 feature flag.
  ///
  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  if ((CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_TM2) != 0) {
    ///
    /// Enable TM2 if it is supported.
    ///
    CpuMiscEnable (TRUE, B_MSR_IA32_MISC_ENABLE_TME);

    ///
    /// Lock TM interrupt
    ///
    Data = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
    Data |= B_MSR_MISC_PWR_MGMT_LTMI;
    AsmWriteMsr64 (MSR_MISC_PWR_MGMT, Data);

    ///
    /// Enable Thermal Interrupt
    ///
    Data = AsmReadMsr64 (IA32_THERM_INTERRUPT);
    Data |= B_IA32_THERM_INTERRUPT_VIE;
    AsmWriteMsr64 (IA32_THERM_INTERRUPT, Data);
  }
}
