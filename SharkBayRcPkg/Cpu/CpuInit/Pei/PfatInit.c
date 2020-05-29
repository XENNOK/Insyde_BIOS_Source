/** @file

  PFAT EarlyPost initializations.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
    This software and associated documentation (if any) is furnished
    under a license and may only be used or copied in accordance
    with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be
    reproduced, stored in a retrieval system, or transmitted in any
    form or by any means without the express written consent of
    Intel Corporation.

*/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "CpuInitPeim.h"
#include "PfatInit.h"
#include <CpuAccess.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiKscLib.h>
#include <Ppi/Stall.h>
#include <Ppi/CpuIo.h>

#ifdef PFAT_EC_FLAG
/**
  Gets CPU's random number generator.

  @param[out] UINT32  - Random value
**/
UINT32
RandomNumber (
        void
  )
{
  UINT32 Random = 0;

  ///
  /// Assembly instruction to read CPU's random number generator
  /// Instruction is only available 100k cycles after reset
  ///
#ifdef __GNUC__
  asm( "rdrand %0" : "=r" (Random) );   // BugBug: no retry loop
#else
  __asm {
tryAgain:
    ;    rdrand eax
    ;    db 0Fh, 0C7h, 0F0h

    _emit 0x0F
    _emit 0xC7
    _emit 0xF0

    mov Random, eax
    jnc tryAgain; CF will be set is valid number was generated
  }
#endif

  return (Random);
}
#endif //PFAT_EC_FLAG

/**
  Perform the platform spefific initializations.

  @param[in] PeiServices           - Indirect reference to the PEI Services Table.
  @param[in] CpuPlatformPolicyPpi  - Platform Policy PPI
**/
VOID
PfatInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  EFI_STATUS  Status;
  UINT64      MsrValue;
  BOOLEAN     ResetRequired;
  PFAT_HOB    *PfatHobPtr;
  EFI_GUID    PfatHobGuid = PFAT_HOB_GUID;
  UINT8       i;
  PFAT_CONFIG *PfatConfig;
#ifdef PFAT_EC_FLAG
  volatile UINT32 EphemeralAuthValue;
  UINT8           EcStatus;
  EFI_PEI_STALL_PPI   *StallPpi;
  EFI_PEI_CPU_IO_PPI  *CpuIo;
#endif  //PFAT_EC_FLAG

  DEBUG ((EFI_D_INFO, "PfatInit\n"));

  ResetRequired = FALSE;
  MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
  if (!(MsrValue & B_MSR_PLATFORM_INFO_PFAT_AVAIL)) {
    DEBUG ((EFI_D_INFO, "PFAT Feature is not supported\n"));
    return;
  }

  if (CpuPlatformPolicyPpi->CpuConfig->Pfat) {
    DEBUG ((EFI_D_INFO, "PFAT Module is Enable\n"));
    PfatConfig = CpuPlatformPolicyPpi->SecurityConfig->PfatConfig;
    ///
    /// Read PFAT Control Register
    ///
    MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
    if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_LK) {
      if (!(MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) {
        ///
        /// Reset required as the PFAT CTRL MSR is locked and needs to be toggled
        ///
        ResetRequired = TRUE;
      }
    } else {
#ifdef PFAT_EC_FLAG
      if ((PfatConfig->Ppdt.PlatAttr & EC_PRESENT) && (PfatConfig->Ppdt.PlatAttr & EC_PFAT_PROTECTED)) {
        DEBUG ((EFI_D_INFO, "EC is Present and EC FW supports PFAT\n"));
        CpuIo  = (**PeiServices).CpuIo;
        Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **)&StallPpi);
        ASSERT_EFI_ERROR (Status);
        Status = SendKscCommand ((CONST EFI_PEI_SERVICES **)PeiServices, CpuIo, StallPpi, PfatConfig->EcCmdProvisionEav);
        if (Status == EFI_SUCCESS) {
          EphemeralAuthValue = RandomNumber ();
          for (i = 0; (i < (sizeof (EphemeralAuthValue))); i++) {
            Status = SendKscData ((CONST EFI_PEI_SERVICES **)PeiServices, CpuIo, StallPpi, (UINT8) ((EphemeralAuthValue >> (i * 8)) & 0xFF));
          }
          Status = ReceiveKscData ((CONST EFI_PEI_SERVICES **)PeiServices, CpuIo, StallPpi, &EcStatus);
          if (EcStatus != 0) {
            ResetRequired = TRUE;
          }
          AsmWriteMsr32 (MSR_PLAT_FRMW_PROT_PASSWD, EphemeralAuthValue);
          EphemeralAuthValue = 0;
          Status = SendKscCommand ((CONST EFI_PEI_SERVICES **)PeiServices, CpuIo, StallPpi, PfatConfig->EcCmdLock);
        }
      }
#endif                  //PFAT_EC_FLAG
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_0, PfatConfig->PpdtHash[0]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_1, PfatConfig->PpdtHash[1]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_2, PfatConfig->PpdtHash[2]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_3, PfatConfig->PpdtHash[3]);
      MsrValue |= (B_MSR_PLAT_FRMW_PROT_CTRL_LK | B_MSR_PLAT_FRMW_PROT_CTRL_EN);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_CTRL, MsrValue);
    }
    ///
    /// Create PFAT HOB
    ///
    if (!ResetRequired) {
      Status = (*PeiServices)->CreateHob ((CONST EFI_PEI_SERVICES **)PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (PFAT_HOB), (VOID **) &PfatHobPtr);
      PfatHobPtr->EfiHobGuidType.Name = PfatHobGuid;
      CopyMem (&PfatHobPtr->Ppdt, &PfatConfig->Ppdt, PfatConfig->Ppdt.PpdtSize);
      CopyMem (&PfatHobPtr->PupHeader, &PfatConfig->PupHeader, sizeof (PUP_HEADER));
      CopyMem (&PfatHobPtr->PfatLog, &PfatConfig->PfatLog, sizeof (PFAT_LOG));
      PfatHobPtr->NumSpiComponents = PfatConfig->NumSpiComponents;
      for (i = 0; i < PfatConfig->NumSpiComponents; i++) {
        PfatHobPtr->ComponentSize[i] = PfatConfig->ComponentSize[i];
      }
    }
  } else {
    DEBUG ((EFI_D_INFO, "PFAT Module is Disabled\n"));
    MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
    if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_LK) {
      if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_EN) {
        ///
        /// Reset required as the PFAT CTRL MSR is locked and needs to be toggled
        ///
        ResetRequired = TRUE;
      }
    } else {
      MsrValue &= ~B_MSR_PLAT_FRMW_PROT_CTRL_EN;
      MsrValue |= B_MSR_PLAT_FRMW_PROT_CTRL_LK;
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_CTRL, MsrValue);
    }
  }

  if (ResetRequired) {
    ///
    /// Perform Cold Reset
    ///
    DEBUG ((EFI_D_INFO, "Reset Required. Performing Cold Reset to unlock PFAT CONTROL MSR\n"));
    PerformWarmORColdReset (PeiServices, COLDRESET);
  }

  return;
}
