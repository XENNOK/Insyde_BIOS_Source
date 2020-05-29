/** @file
  This file contains an implementation of the function call interfaces
  required by the main TXT PEIM file.  Hopefully, future platform porting
  tasks will be mostly limited to modifying the functions in this file.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
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
  license agreement

**/
#include "TxtPeiLib.h"
#include <Library/TxtLibrary.h>

EFI_GUID mTxtBiosAcmPeiFileGuid    = PEI_BIOS_ACM_FILE_GUID;
EFI_GUID mTxtApStartupPeiFileGuid  = PEI_AP_STARTUP_FILE_GUID;
EFI_GUID mCpuMicrocodeFileGuid     = CPU_MICROCODE_FILE_GUID;
/**
  This routine initializes and collects all PPIs and data required
  by the routines in this file.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] mPS - A pointer to the PEI Service Table

  @exception EFI_UNSUPPORTED   - If any of the required PPIs or data are unavailable
  @retval EFI_SUCCESS       - In all cases not listed above
**/
EFI_STATUS
InitializeTxtPeiLib (
  IN  TXT_PEI_LIB_CONTEXT      *pctx,
  IN  CONST EFI_PEI_SERVICES         **mPS
  )
{
  EFI_STATUS  Status;

  ///
  /// Make sure our pointers start life as NULL pointers
  ///
  pctx->PeiStall    = NULL;
  pctx->BiosAcmBase = NULL;
  pctx->ApStartup   = NULL;
  pctx->McuStart    = NULL;

  ///
  /// Initialize all pointers
  ///
  pctx->PeiServices = mPS;
  pctx->CpuIoPpi    = (**mPS).CpuIo;

  Status = PeiServicesLocatePpi (
                  &gEfiPciCfgPpiInServiceTableGuid,
                  0,
                  NULL,
                  (VOID **) &(pctx->PciCfgPpi)
                  );
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (
                  &gEfiPeiStallPpiGuid,
                  0,
                  NULL,
                  (VOID **) &(pctx->PeiStall)
                  );

  ASSERT_EFI_ERROR (Status);

  ///
  /// Find TxtInfoHob by platfrom code
  ///
  Status = CreateTxtInfoHob (pctx);

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "TXTPEI: TXT Info Hob not found.\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Print out the TxtInfo HOB if TXT_DEBUG_INFO is set
  ///
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtInfoHob passed from platform as:\n"));
  DEBUG ((EFI_D_INFO, "TXTPEI: ChipsetIsTxtCapable = %x\n", pctx->Hob->Data.ChipsetIsTxtCapable));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtMode             = %x\n", pctx->Hob->Data.TxtMode));
  DEBUG ((EFI_D_INFO, "TXTPEI: PmBase              = %x\n", pctx->Hob->Data.PmBase));
  DEBUG ((EFI_D_INFO, "TXTPEI: SinitMemorySize     = %x\n", pctx->Hob->Data.SinitMemorySize));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtHeapMemorySize   = %x\n", pctx->Hob->Data.TxtHeapMemorySize));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtDprMemoryBase    = %x\n", pctx->Hob->Data.TxtDprMemoryBase));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtDprMemorySize    = %x\n", pctx->Hob->Data.TxtDprMemorySize));
  DEBUG ((EFI_D_INFO, "TXTPEI: BiosAcmBase         = %x\n", pctx->Hob->Data.BiosAcmBase));
  DEBUG ((EFI_D_INFO, "TXTPEI: BiosAcmSize         = %x\n", pctx->Hob->Data.BiosAcmSize));
  DEBUG ((EFI_D_INFO, "TXTPEI: McuUpdateDataAddr   = %x\n", pctx->Hob->Data.McuUpdateDataAddr));
  DEBUG ((EFI_D_INFO, "TXTPEI: SinitAcmBase        = %x\n", pctx->Hob->Data.SinitAcmBase));
  DEBUG ((EFI_D_INFO, "TXTPEI: SinitAcmSize        = %x\n", pctx->Hob->Data.SinitAcmSize));
  DEBUG ((EFI_D_INFO, "TXTPEI: TgaSize             = %x\n", pctx->Hob->Data.TgaSize));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtLcpPdBase        = %x\n", pctx->Hob->Data.TxtLcpPdBase));
  DEBUG ((EFI_D_INFO, "TXTPEI: TxtLcpPdSize        = %x\n", pctx->Hob->Data.TxtLcpPdSize));
  DEBUG ((EFI_D_INFO, "TXTPEI: Flags               = %x\n", pctx->Hob->Data.Flags));

  ///
  /// Check if platform specify BIOS ACM addrss by itself, BIOS ACM address must be 4K alignment in FLASH address space
  ///
  if (pctx->Hob->Data.BiosAcmBase != 0) {
    DEBUG ((EFI_D_INFO, "TXTPEI: Customized BIOS ACM location at %x\n", pctx->Hob->Data.BiosAcmBase));
    ///
    /// Check BIOS ACM is 4K alignment or not
    ///
    if ((pctx->Hob->Data.BiosAcmBase & 0xFFF) != 0) {
      DEBUG ((EFI_D_ERROR, "TXTPEI: Customized BIOS ACM is not 4K aligned, force TxtMode=0 and unloaded!!\n"));
      pctx->Hob->Data.TxtMode = 0;
      return EFI_UNSUPPORTED;
    }
    ///
    /// Get BIOS ACM base from TxtInfoHob provided by platform code
    ///
    pctx->BiosAcmBase = (ACM_HEADER *) (UINT32) pctx->Hob->Data.BiosAcmBase;
  } else {
    ///
    /// Get BIOS ACM by seaching PEI firmware volume
    ///
    Status = FindBiosAcmInVolume (pctx, (UINT32 **) &pctx->BiosAcmBase);

    ///
    /// BIOS ACM not found, disable TXT and return EFI_UNLOAD_IMAGE
    ///
    if (((pctx->BiosAcmBase) == 0) || (Status == EFI_NOT_FOUND)) {
      DEBUG ((EFI_D_ERROR, "TXTPEI: BIOS ACM not found, force TxtMode=0 and unloaded!\n"));
      pctx->Hob->Data.TxtMode = 0;
      return EFI_UNSUPPORTED;
    }

    DEBUG ((EFI_D_INFO, "TXTPEI: Found BIOS ACM location at %x\n", pctx->BiosAcmBase));

    ///
    /// Check BIOS ACM is 4K alignment, if not disable TXT and return EFI_UNLOAD_IMAGE
    ///
    if (((UINT32) pctx->BiosAcmBase & 0xFFF) != 0) {
      DEBUG ((EFI_D_ERROR, "TXTPEI: BIOS ACM is not 4K aligned, force TxtMode=0 and unloaded!!\n"));
      pctx->Hob->Data.TxtMode = 0;
      return EFI_UNSUPPORTED;
    }
  }
  ///
  /// Check ACM is matched to chipset or not, if not, disable TXT and return EFI_UNLOAD_IMAGE
  ///
  if (!CheckTxtAcmMatch (pctx, pctx->BiosAcmBase)) {
    DEBUG ((EFI_D_ERROR, "TXTPEI: BIOS ACM is not matched to chipset!! Force TxtMode=0 and unloaded!!\n"));
    pctx->Hob->Data.TxtMode = 0;
    return EFI_UNSUPPORTED;
  }

  pctx->BiosAcmSize = (pctx->BiosAcmBase->Size) << 2;

  Status              = FindApStartupInVolume (pctx, (UINT32 **) &(pctx->ApStartup));

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "TXTPEI: AP Startup code not found.\n"));
    pctx->Hob->Data.TxtMode = 0;
    return EFI_UNSUPPORTED;
  }

  DEBUG ((EFI_D_INFO, "TXTPEI: Found AP init code at %x\n", pctx->ApStartup));

  ///
  /// Check if platform specify MCU addrss by itself
  ///
  if (pctx->Hob->Data.McuUpdateDataAddr != 0) {
    DEBUG ((EFI_D_INFO, "Customized MCU location at %x\n", pctx->Hob->Data.McuUpdateDataAddr));
    pctx->McuStart = (UINT32 *) (UINTN) pctx->Hob->Data.McuUpdateDataAddr;
  } else {
    ///
    /// Find microcode update by searching PEI FV
    ///
    Status = FindMcuInVolume (pctx, (UINT32 **) &(pctx->McuStart));

    if (Status == EFI_NOT_FOUND) {
      DEBUG ((EFI_D_ERROR, "TXTPEI: MCU not found.\n"));
      pctx->Hob->Data.TxtMode = 0;
      return EFI_UNSUPPORTED;
    }

    DEBUG ((EFI_D_INFO, "TXTPEI: Found MCU at %x\n", pctx->McuStart));
  }
  ///
  /// Initialize local APIC
  ///
  if ((((AsmReadMsr64 (MSR_IA32_APIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10)) {
    AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_SVR, 0x1FF);
  } else {
    ///
    /// Determine address of Local APIC
    ///
    pctx->Ia32ApicBase  = ((UINT32) AsmReadMsr64 (MSR_IA32_APIC_BASE)) & BASE_ADDR_MASK;
    *(UINT32 *) (pctx->Ia32ApicBase + APIC_SPURIOUS_VECTOR_REGISTER) = 0x1FF;
  }
  ///
  /// Initialize TxtInfoHob fields
  ///
  pctx->Hob->Data.BiosAcmBase       = (UINTN) pctx->BiosAcmBase;
  pctx->Hob->Data.BiosAcmSize       = (UINTN) pctx->BiosAcmSize;
  pctx->Hob->Data.McuUpdateDataAddr = (UINTN) pctx->McuStart;

  ///
  /// Make sure none of our pointers are still NULL
  ///
  if (!(pctx->PeiStall && pctx->BiosAcmBase && pctx->ApStartup && pctx->McuStart)) {
    pctx->Hob->Data.TxtMode = 0;
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Determines whether or not the current processor is TXT Capable.

  @retval TRUE          - If the current processor supports TXT
  @retval FALSE         - If the current processor does not support TXT
**/
BOOLEAN
IsTxtProcessor (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegs;

  AsmCpuid (1, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);

  return (CpuidRegs.RegEcx & B_CPUID_VERSION_INFO_ECX_SME) ? TRUE : FALSE;
}

/**
  Determines whether or not the current chipset is TXT Capable.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the current chipset supports TXT
  @retval FALSE         - If the current chipset doesn't supports TXT
**/
BOOLEAN
IsTxtChipset (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  BOOLEAN TxtCapable;
  UINT32  Data32;

  TxtCapable  = FALSE;
  Data32      = CheckSmxCapabilities();

  if ((Data32 & BIT0) != 0) {
    TxtCapable = TRUE;
    DEBUG ((EFI_D_INFO, "Platform/PCH - TXT supported\n"));
  } else {
    TxtCapable = FALSE;
    DEBUG ((EFI_D_INFO, "Platform/PCH - TXT not supported!!!\n"));
  }

  return TxtCapable;
}

/**
  Determines whether TXT is enabled by platform setting

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If TXT is enabled by platform setting
  @retval FALSE         - If TXT is disabled by platform setting
**/
BOOLEAN
IsTxtEnabled (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  if (pctx->Hob == 0) {
    return FALSE;
  }

  return (BOOLEAN) (pctx->Hob->Data.TxtMode);
}

/**
  Determines ACM is matched to chipset or not

  @param[in] pctx         - Point to TXT_PEI_LIB_CONTEXT structure
  @param[in] BiosAcmBase  - A pointer to BIOS ACM location

  @retval TRUE          - BIOS ACM is matched to chipset
  @retval FALSE         - BIOS ACM is NOT matched to chipset
**/
BOOLEAN
CheckTxtAcmMatch (
  IN TXT_PEI_LIB_CONTEXT *pctx,
  IN ACM_HEADER          *BiosAcmBase
  )
{
  BOOLEAN ChipsetIsProduction;
  BOOLEAN BiosAcmIsProduction;

  if (BiosAcmBase == NULL) {
    return FALSE;

  }
  ///
  /// Initializing ChipsetIsProduction default value
  ///
  ChipsetIsProduction = (*(UINT32 *) (TXT_PUBLIC_BASE + 0x200) & BIT31) ? TRUE : FALSE;

  ///
  /// Check ACM is production or not
  ///
  BiosAcmIsProduction = (BiosAcmBase->ModuleID & BIT31) ? FALSE : TRUE;

  return ChipsetIsProduction == BiosAcmIsProduction;
}

/**
  Create TXT Info HOB

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - If TXT Info Hob is found
  @retval EFI_NOT_FOUND - If TXT Info Hob is not found
**/
EFI_STATUS
CreateTxtInfoHob (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  EFI_STATUS                  Status;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicy;
  TXT_INFO_HOB                *TxtInfoHob;

  TxtInfoHob = NULL;

  ///
  /// Locate TPM Initialized Ppi to determine TPM is present and initialized properly.
  ///
  Status = PeiServicesLocatePpi (
                  &gPeiCpuPlatformPolicyPpiGuid,
                  0,
                  NULL,
                  (VOID **) &CpuPlatformPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Create hob for storing TXT data
  ///
  Status = (*(pctx->PeiServices))->CreateHob (
                  pctx->PeiServices,
                  EFI_HOB_TYPE_GUID_EXTENSION,
                  sizeof (TXT_INFO_HOB),
                  (VOID **) &TxtInfoHob
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  pctx->Hob                     = TxtInfoHob;
  TxtInfoHob->EfiHobGuidType.Name = gTxtInfoHobGuid;

  ///
  /// Initiate Txt Info Hob
  ///
  ZeroMem (&(TxtInfoHob->Data), sizeof (TXT_INFO_DATA));

  TxtInfoHob->Data.ChipsetIsTxtCapable = IsTxtChipset (pctx);
  if (CpuPlatformPolicy->CpuConfig->Txt == 1) {
    TxtInfoHob->Data.TxtMode = 1;
  }
  TxtInfoHob->Data.PmBase = MmioRead32 (
                  MmPciAddress (0,
                                PCI_BUS_NUMBER_PCH_LPC,
                                PCI_DEVICE_NUMBER_PCH_LPC,
                                PCI_FUNCTION_NUMBER_PCH_LPC,
                                R_PCH_LPC_ACPI_BASE)
                  ) &~1;

  TxtInfoHob->Data.SinitMemorySize    = CpuPlatformPolicy->SecurityConfig->TxtConfig->SinitMemorySize;
  TxtInfoHob->Data.TxtHeapMemorySize  = CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtHeapMemorySize;
  TxtInfoHob->Data.TxtDprMemoryBase   = CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtDprMemoryBase;
  TxtInfoHob->Data.TxtDprMemorySize   = CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtDprMemorySize;
  TxtInfoHob->Data.BiosAcmBase        = CpuPlatformPolicy->SecurityConfig->TxtConfig->BiosAcmBase;
  TxtInfoHob->Data.BiosAcmSize        = CpuPlatformPolicy->SecurityConfig->TxtConfig->BiosAcmSize;
  TxtInfoHob->Data.McuUpdateDataAddr  = CpuPlatformPolicy->SecurityConfig->TxtConfig->McuUpdateDataAddr;
  TxtInfoHob->Data.TgaSize            = CpuPlatformPolicy->SecurityConfig->TxtConfig->TgaSize;
  TxtInfoHob->Data.TxtLcpPdBase       = CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtLcpPdBase;
  TxtInfoHob->Data.TxtLcpPdSize       = CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtLcpPdSize;

  return EFI_SUCCESS;
}

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsEstablishmentBitAsserted (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT8  Access;
  UINT16 TimeOutCount;

  ///
  /// Set TPM.ACCESS polling timeout about 750ms
  ///
  TimeOutCount = TPM_TIME_OUT;
  do {
    ///
    /// Read TPM status register
    ///

      Access = pctx->CpuIoPpi->MemRead8 (
                      pctx->PeiServices,
                      pctx->CpuIoPpi,
                      TPM_STATUS_REG_ADDRESS
                      );

    ///
    /// if TPM.Access == 0xFF, TPM is not present
    ///
    if (Access == 0xFF) {
      return FALSE;
    }
    ///
    /// Check tpmRegValidSts bit before checking establishment bit
    ///
    if (((pctx->PeiStall) != NULL) && ((Access & 0x80) != 0x80)) {
      ///
      /// Delay 1ms
      ///
      pctx->PeiStall->Stall (pctx->PeiServices, pctx->PeiStall, 1000);
    } else {
      ///
      /// tpmRegValidSts set, we can check establishment bit now.
      ///
      break;
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);
  ///
  /// if tpmRegValidSts is not set, TPM is not usable
  ///
  if ((Access & 0x80) != 0x80) {
    DEBUG ((EFI_D_ERROR, "TXTPEI: TPM Valid Status is not set!! TPM.ACCESS=%x\n", Access));
    ASSERT (TRUE);
    EFI_DEADLOOP ();
  }
  ///
  /// The bit we're interested in uses negative logic:
  /// If bit 0 == 1 then return False
  /// Else return True
  ///
  return (Access & 0x1) ? FALSE : TRUE;
}

/**
  Determines whether or not the platform has encountered an error during
  a sleep or power-off state.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TXT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the TXT_WAKE_ERROR bit is unasserted.
**/
BOOLEAN
IsTxtWakeError (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT8 Ests;

  ///
  /// Read TXT.ESTS register
  ///
  Ests = pctx->CpuIoPpi->MemRead8 (
                  pctx->PeiServices,
                  pctx->CpuIoPpi,
                  TXT_PUBLIC_BASE + TXT_ERROR_STATUS_REG_OFF
                  );

  DEBUG ((EFI_D_INFO, "TXTPEI: TXT.ESTS=%x\n", Ests));

  return (Ests & (0x1 << 6)) ? TRUE : FALSE;
}

/**
  Determines whether or not the platform memory has been locked

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If memroy is locked
  @retval FALSE         - If memory is unlocked
**/
BOOLEAN
IsMemoryLocked (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT32 CpuMemLockStatus;

  ///
  /// Check status register for now.
  ///
  CpuMemLockStatus = pctx->CpuIoPpi->MemRead32 (
                  pctx->PeiServices,
                  pctx->CpuIoPpi,
                  TXT_PUBLIC_BASE + TXT_E2STS_REG_OFF
                  );

  DEBUG ((EFI_D_INFO, "TXTPEI: CPU_UNCORE_MEMLOCK_STATUS=%x\n", CpuMemLockStatus));
  ///
  /// if BLOCK_MEM_STS (BIT2) is set to 1, memory is in unlock.
  ///
  return (CpuMemLockStatus & TXT_BLOCK_MEM_STS) ? TRUE : FALSE;
}

/**
  Determines whether or not POISON bit is set in status register

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TXT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the TXT_WAKE_ERROR bit is unasserted.
**/
BOOLEAN
IsTxtResetSet (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT8 Ests;

  ///
  /// Read TXT.ESTS register
  ///
  Ests = pctx->CpuIoPpi->MemRead8 (
                  pctx->PeiServices,
                  pctx->CpuIoPpi,
                  TXT_PUBLIC_BASE + TXT_ERROR_STATUS_REG_OFF
                  );

  return (Ests & (0x1 << 0)) ? TRUE : FALSE;
}

/**
  Determines whether or not SECRETS.STS bit is set in E2STS status register

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the LT.SECRETS.STS bit is asserted.
  @retval FALSE         - If the LT.SECRETS.STS bit is unasserted.
**/
BOOLEAN
IsTxtSecretsSet (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT8 E2sts;

  ///
  /// Read TXT.E2STS register
  ///
  E2sts = pctx->CpuIoPpi->MemRead8 (
                  pctx->PeiServices,
                  pctx->CpuIoPpi,
                  TXT_PUBLIC_BASE + TXT_E2STS_REG_OFF
                  );

  return (E2sts & TXT_SECRETS_STS) ? TRUE : FALSE;
}

/**
  Determines presence of TPM in system

  @param[in] pctx           - Point to TXT_PEI_LIB_CONTEXT structure
  @param[in] TxtPeiCtx      - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS          - If the TPM is present.
  @retval EFI_NOT_FOUND         - If the TPM is not present.
**/
EFI_STATUS
IsTpmPresent (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  EFI_STATUS Status;
  VOID       *TpmInitialize;

  ///
  /// Locate TPM Initialized Ppi to determine TPM is present and initialized properly.
  ///
  Status = PeiServicesLocatePpi (
                  &gPeiTpmInitializedPpiGuid,
                  0,
                  NULL,
                  (VOID **)&TpmInitialize
                  );
  if (EFI_ERROR (Status)) {
    ///
    /// TPM initiated failed
    ///
    pctx->Hob->Data.Flags |= TPM_INIT_FAILED;
  }

  return Status;
}

/**
  Clear Sleep Type register.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - Always
**/
EFI_STATUS
ClearSlpTyp (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT32 Pm1Addr;
  UINT32 Pm1Value;

  ///
  /// Make address for PM1_CNT
  ///
  Pm1Addr = ((UINT32) pctx->Hob->Data.PmBase) + 4;

  ///
  /// Read 32-bits from PM1_CNT
  ///
  Pm1Value = IoRead32 ((UINTN) Pm1Addr);

  ///
  /// Clear SLP_TYP bits 10-12
  ///
  Pm1Value = Pm1Value & 0xffffe3ff;
  IoWrite32 (
          (UINTN) Pm1Addr,
          (UINT32) (Pm1Value)
          );

  return EFI_SUCCESS;
}

/**
  Searches PEI firemare volume (FV_BB) for file containig BIOS ACM.

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] BiosAcBase    - A pointer to pointer to variable to hold found address

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindBiosAcmInVolume (
  IN TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32             **BiosAcBase
  )
{
  EFI_STATUS Status;

  Status = FindModuleInFlash (pctx, &mTxtBiosAcmPeiFileGuid, BiosAcBase);

  ///
  /// If BIOS ACM is not found - don't hang system. Assume that TXT
  /// must be disabled.
  ///
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  ASSERT (((((UINT32) (*BiosAcBase)) & 0xFFF) == 0) ? TRUE : FALSE);

  return EFI_SUCCESS;

}

/**
  Searches PEI firmware volume (FV_BB) for file containig AP Startup code

  @param[in] pctx           - A pointer to an initialized TXT PEI Context data structure
  @param[in] ApStartupBase  - A pointer to pointer to variable to hold address

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindApStartupInVolume (
  IN TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32             **ApStartupBase
  )
{
  EFI_STATUS Status;
  EFI_GUID   PeiBiosApStartupFileGuid;
  BOOLEAN    GoodPlacement;

  PeiBiosApStartupFileGuid  = mTxtApStartupPeiFileGuid;
  Status                    = FindModuleInFlash (pctx, &PeiBiosApStartupFileGuid, ApStartupBase);
  ///
  /// If AP Startup code is not found - don't hang system. Assume that TXT
  /// must be disabled.
  ///
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  ASSERT (((((UINT32) (*ApStartupBase)) & 0xFFF) == 0) ? TRUE : FALSE);

  GoodPlacement = (BOOLEAN) (((UINT32) (*ApStartupBase) >= 0xFFFE0000) && ((UINT32) (*ApStartupBase) <= 0xFFFFF000));

  ASSERT (GoodPlacement);

  return EFI_SUCCESS;
}

/**
  Searches PEI firmware volume (FV_BB) for offset of currently loaded MCU patch

  @param[in] pctx          - A pointer to an initialized TXT PEI Context data structure
  @param[in] McuBase       - A pointer to pointer to variable to hold found offset
  @param[in] address.

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindMcuInVolume (
  IN TXT_PEI_LIB_CONTEXT *pctx,
  OUT UINT32             **McuBase
  )
{
  EFI_STATUS         Status;
  EFI_GUID           MicrocodeGuid;
  MSR_REGISTER       Reg;
  UINT32             PlatformId;
  UINT32             McuRevision;
  EFI_CPUID_REGISTER CpuidRegs;
  MCU                *McuAddr;
  UINT8              *i;
  UINT8              *j;
  UINT8              *b;
  UINT32             c;

  McuAddr       = NULL;
  McuRevision   = 0;
  MicrocodeGuid = mCpuMicrocodeFileGuid;
  Status        = FindModuleInFlash (pctx, &MicrocodeGuid, (UINT32 **) &McuAddr);
  ///
  /// If MCU update is not found - don't hang system. Assume that TXT
  /// must be disabled.
  ///
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  ///
  /// MCU base address has been found. Find exact address of MCU
  /// loaded in BSP
  ///
  Reg.Qword   = AsmReadMsr64 (MSR_IA32_PLATFORM_ID);
  PlatformId  = (Reg.Dwords.High >> (PLATFORM_ID_SHIFT - 32)) & PLATFORM_ID_MASK;

  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0);

  AsmCpuid (1, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);

  Reg.Qword   = AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID);

  McuRevision = Reg.Dwords.High;

  ///
  /// if BSP MCU is not loaded before. Return EFI_NOT_FOUND
  /// This is pre-requisit.
  ///
  if (McuRevision == 0) {
    DEBUG ((EFI_D_ERROR, "TXTPEI: BSP microcode is not loaded!! TXT aborted!\n"));
    return EFI_NOT_FOUND;
  }

  for (i = (UINT8 *) McuAddr; i < ((UINT8 *) McuAddr) + 0x10000; i = i + 0x400) {
    if (((MCU *) i)->revision != McuRevision) {
      continue;
    }

    if (!(((MCU *) i)->procFlags & (UINT32) LShiftU64 (1, PlatformId))) {
      continue;
    }

    if ((((MCU *) i)->signature) == CpuidRegs.RegEax) {
      *McuBase = (UINT32 *) i;
      return EFI_SUCCESS;
    }

    if ((((MCU *) i)->dataSize) == 0) {
      continue;
    }

    if ((((MCU *) i)->dataSize + sizeof (MCU)) >= (((MCU *) i)->totalSize)) {
      continue;
    }
    ///
    /// Extended signature table exists.
    ///
    b = i + (((MCU *) i)->dataSize);
    ///
    /// Base of EST table
    ///
    c = ((EST *) b)->count;
    ///
    /// Count of entries
    ///
    b += sizeof (EST);
    ///
    /// Base of PSS table entries
    ///
    for (j = b; j < b + (c * sizeof (PSS)); j = j + sizeof (PSS)) {
      if ((((PSS *) j)->signature) == CpuidRegs.RegEax) {
        *McuBase = (UINT32 *) i;
        return EFI_SUCCESS;
      }
    }

    continue;
  }

  return EFI_NOT_FOUND;
}

/**
  Searches PEI firmware volume (FV_BB) for the file with specified GUID through pGuid

  @param[in] pctx         - A pointer to an initialized TXT PEI Context data structure
  @param[in] Guid         - A pointer GUID
  @param[in] Module       - A pointer to pointer to variable to hold address

  @retval EFI_SUCCESS   - If address has been found
  @retval EFI ERROR     - If address has not been found
**/
EFI_STATUS
FindModuleInFlash (
  IN TXT_PEI_LIB_CONTEXT *pctx,
  IN EFI_GUID            *Guid,
  OUT UINT32             **Module
  )
{
  EFI_FIRMWARE_VOLUME_HEADER *FvHeader;
  EFI_FFS_FILE_HEADER        *FfsFile;
  EFI_STATUS                 Status;
  UINTN                      Instance;

  Instance = BFV;

  while (TRUE) {
    Status = (*(pctx->PeiServices))->FfsFindNextVolume (pctx->PeiServices, Instance, (VOID **)&FvHeader);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    FfsFile = NULL;
    ///
    /// Start new search in volume
    ///
    while (TRUE) {
      Status = (*(pctx->PeiServices))->FfsFindNextFile (pctx->PeiServices, EFI_FV_FILETYPE_RAW, FvHeader, (VOID **)&FfsFile);
      if (Status == EFI_NOT_FOUND) {
        break;
      }

      if (CompareGuid (&(FfsFile->Name), Guid)) {
        *Module = (UINT32 *) ((UINT8 *) FfsFile + sizeof (EFI_FFS_FILE_HEADER));
        return EFI_SUCCESS;
      }
    }

    Instance += 1;
  }
}

/**
  Initializes values passed to AP

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure
**/
VOID
PrepareApParams (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  UINT32 TxtPublicSpace;

  TxtPublicSpace  = TXT_PUBLIC_BASE;

  *(UINT32 *) (TxtPublicSpace + BIOACM_ADDR)    = (UINT32) (pctx->BiosAcmBase);

  *(UINT32 *) (TxtPublicSpace + MCU_BASE_ADDR)  = (UINT32) (pctx->McuStart);

  *(UINT32 *) (TxtPublicSpace + APINIT_ADDR)    = (UINT32) (pctx->ApStartup);

  *(UINT32 *) (TxtPublicSpace + SEMAPHORE)      = 0;

}

/**
  Returns CPU count

  @retval Number of CPUs
**/
UINT32
GetCpuCount (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegs;
  AsmCpuid (1, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
  return CpuidRegs.RegEbx >> 16;
}

/**
  Invokes the SCLEAN function from the TXT BIOS ACM.
  1. Clearing of sleep type is necessary because SCLEAN destroys memory
  context, so S3 after it is run and system is reset is impossible. We
  do it here since there is no any indication that can sustain reset
  for any other module to do it on our behalf.
  2. APs are initialized before calling of SCLEAN
  3. SCLEAN function is invoked.

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
DoSclean (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  ClearSlpTyp (pctx);

  PrepareApParams (pctx);
#ifdef BOOT_GUARD_SUPPORT_FLAG
#if BOOT_GUARD_SUPPORT_FLAG == 1
  //
  // Disable PBET before send IPI to APs
  //
  StopPbeTimer ();
#endif
#endif

  LaunchBiosAcmSclean ();

  return EFI_SUCCESS;
}

/**
  Unlock memory when security is set and TxT is not enabled

  @param[in] pctx      - A pointer to an initialized TXT PEI Context data structure

  @retval EFI_SUCCESS     - Complete memory unlock
  @exception EFI_UNSUPPORTED - CPU doesn't support TxT.
**/
EFI_STATUS
UnlockMemory (
  IN TXT_PEI_LIB_CONTEXT *pctx
  )
{
  BOOLEAN EstablishmentBitAsserted;

  EstablishmentBitAsserted = IsEstablishmentBitAsserted (pctx);

  ///
  /// Need to read FED40000 before unlocking memory
  ///
  if (!EstablishmentBitAsserted) {
    DEBUG ((EFI_D_INFO, "TXTPEI::Unlock memory\n"));
    AsmWriteMsr64 (0x2e6, 0);
  } else {
    ///
    /// Lunch SCLEAN if wake error bit is set.
    ///
    if (IsTxtWakeError (pctx)) {
      ///
      /// If TXTRESET is set , we must clean TXTRESET bit otherwise SCLEAN
      /// will fail
      ///
      if (IsTxtResetSet (pctx)) {
        DoGlobalReset ();
      }

      if ((pctx->BiosAcmBase == 0) || (pctx->ApStartup == 0) || (pctx->McuStart == 0)) {
        return EFI_UNSUPPORTED;
      }
      ///
      /// Setup and Launch SCLEAN
      ///
      DEBUG ((EFI_D_INFO, "TXTPEI::Entering SCLEAN to unlock memory\n"));
      DoSclean (pctx);

      ///
      /// Reset platform - performed by DoSclean, should not return to execute the following dead looping
      ///
      EFI_DEADLOOP ();
    } else {
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}
