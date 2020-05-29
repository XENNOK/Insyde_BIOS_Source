/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcEnableFdWrites().
 The function PeiCsSvcEnableFdWrites() use chipset services to enable/disable 
 flash device write access.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
//[-start-130808-IB06720232-modify]//
//#include <Library/EcLib.h>
#include <Library/BaseOemSvcKernelLib.h>
//[-end-130808-IB06720232-modify]//
#include <Library/PciCf8Lib.h>
#include <Protocol/SmmFwBlockService.h>
#include <PchAccess.h>
#include <CpuRegs.h>
//[-start-130204-IB10820229-remove]//
//#define CACHE_DISABLE 1     // Switch
//[-end-130204-IB10820229-remove]//

UINT8  mFlashMode = SMM_FW_DEFAULT_MODE;

/**
 Platform specific function to enable/disable flash device write access.

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function returns successfully
*/
EFI_STATUS
EFIAPI
EnableFdWrites (
  IN  BOOLEAN           EnableWrites
  )
{
  UINT8                                 Buffer8;
  STATIC BOOLEAN                        DisableCache = FALSE;
  UINT64                                MsrValue;
  UINT32                                Buffer32;

//[-start-121119-IB10820169-add]//
//[-start-120815-IB05300312-modify]//
  MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
  if ((MsrValue & (B_MSR_PLAT_FRMW_PROT_CTRL_EN | B_MSR_PLAT_FRMW_PROT_CTRL_LK)) == (B_MSR_PLAT_FRMW_PROT_CTRL_EN | B_MSR_PLAT_FRMW_PROT_CTRL_LK)) {
    return EFI_SUCCESS;
  }
//[-end-120815-IB05300312-modify]//
//[-end-121119-IB10820169-add]//

//[-start-130808-IB06720232-modify]//
  if (FeaturePcdGet (PcdEcSharedFlashSupported)) {
    if (FeaturePcdGet (PcdEcIdlePerWriteBlockSupported)) {
      if (mFlashMode != SMM_FW_FLASH_MODE) {
        OemSvcEcWait (EnableWrites);
      } else {
        OemSvcEcIdle (EnableWrites);
      }
    } else {
      if (mFlashMode != SMM_FW_FLASH_MODE) {
        OemSvcEcWait (EnableWrites);
      }
    }
  }
//[-end-130808-IB06720232-modify]//

  //
  // Get BIOS enable bit by PCI reading in SMM
  //
  Buffer8 = PciCf8Read8 (PCI_CF8_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 
                           R_PCH_LPC_BIOS_CNTL)
                          );
//[-start-130204-IB10820229-modify]//
  if (EnableWrites) {
    Buffer8 = (UINT8) (Buffer8 | B_PCH_LPC_BIOS_CNTL_BIOSWE);
    if (FeaturePcdGet (PcdDisableCacheSupportInEnableFdWrites)) {
      if (!DisableCache) {
        AsmDisableCache ();
        AsmWbinvd ();
        MsrValue = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
        MsrValue = MsrValue & ~B_CACHE_MTRR_VALID;
        AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, MsrValue);
        DisableCache = TRUE;
      }
    }
  } else {
    Buffer8 = (UINT8) (Buffer8 & (~B_PCH_LPC_BIOS_CNTL_BIOSWE));
    if (FeaturePcdGet (PcdDisableCacheSupportInEnableFdWrites)) {
      if (DisableCache) {
        AsmEnableCache();
        MsrValue = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
        MsrValue = MsrValue | B_CACHE_MTRR_VALID;
        AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, MsrValue);
        DisableCache = FALSE;
      }
    }
  }
//[-end-130204-IB10820229-modify]//

  PciCf8Write8 (PCI_CF8_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_BIOS_CNTL),
                  Buffer8
                  );
  
  Buffer8 = PciCf8Read8 (PCI_CF8_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 
                            R_PCH_LPC_BIOS_CNTL)
                           );

  //
  // The SMM_BWP bit and BLE bit are set in ExitBootServices event and LegacyBoot event 
  // to make that BIOS access right is protected in SMM mode. 
  // After set these two bits, below procedure should be executed to lock/unlock BIOS region access.
  //
  if (((Buffer8 & B_PCH_LPC_BIOS_CNTL_BLE) == B_PCH_LPC_BIOS_CNTL_BLE) &&
      ((Buffer8 & B_PCH_LPC_BIOS_CNTL_SMM_BWP) == B_PCH_LPC_BIOS_CNTL_SMM_BWP)) {
    //
    // Below hard-code procedure follows "BIOS Region SMM Protection Enabling" section in PCH BIOS Spec.
    //
    if (EnableWrites) {
      Buffer32 = MmioRead32 ((UINTN) (0xFED30880)) | (UINT32) (BIT0);
    } else {
      Buffer32 = MmioRead32 ((UINTN) (0xFED30880)) & (UINT32) (~BIT0);
    }
    AsmWriteMsr32 (0x1FE, Buffer32);
  }

  return EFI_SUCCESS;
}
