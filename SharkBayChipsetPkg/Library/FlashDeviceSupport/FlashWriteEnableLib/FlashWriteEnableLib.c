/** @file
  Library classes for enabling/disabling flash write access

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/FdSupportLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <PchRegs.h>
#include <CpuRegs.h>
//[-start-130305-IB05160418-add]//
#include <Protocol/Pfat.h>
//[-end-130305-IB05160418-add]//

#define MSR_CACHE_IA32_MTRR_DEF_TYPE          0x2FF
#define MSR_CACHE_MTRR_VALID                  0x800

#define PCI_ADDRESS_INDEX       0xcf8
#define PCI_DATA_INDEX          0xcfc
#define PCI_ACCRESS_ENABLE_BIT  0x80000000


#define PciCfgAddr(bus, dev, func) \
    ((UINT32) ((((UINT32) bus) << 16) + (((UINT32) dev) << 11) + (((UINT32) func) << 8)))

//[-start-130305-IB05160418-add]//
extern PFAT_PROTOCOL                  *mPfatProtocol;
//[-end-130305-IB05160418-add]//

//
// Hard-code procedure follows "BIOS Region SMM Protection Enabling" section in PCH BIOS Spec.
//
UINT32 mBiosWriteEnableMmio = 0xFED30880;

//[-start-140107-IB10920075-add]//
#define OCW1_DATA_PROT          0x21
#define OCW1_IRQ0_MASK_ENABLE   0xFE
#define OCW1_IRQ0_MASK_DISABLE  0x01

VOID
EnableIRQ0 (
  UINT8 OCW1
  )
{
  IoWrite8 (OCW1_DATA_PROT, OCW1);
}

VOID
DisableIRQ0 (
  UINT8 OCW1
  )
{
  OCW1 = OCW1 | OCW1_IRQ0_MASK_DISABLE;
  IoWrite8 (OCW1_DATA_PROT, OCW1);
}
//[-end-140107-IB10920075-add]//


UINT8
PciReadConfig8 (
  IN  UINT32 Register
  )
{

  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoWrite32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  return IoRead8(DataInxPort);

}

VOID
PciWriteConfig8 (
  IN  UINT32 Register,
  IN  UINT8 Val
  )
{
  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoWrite32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  IoWrite8(DataInxPort, (UINT8)Val);

}


EFI_STATUS
EnableFvbWrites (
  BOOLEAN EnableWrites
  )
{
  UINT8                                 Buffer8;
  STATIC BOOLEAN                        DisableCache = FALSE;
  UINT64                                MsrValue;
  UINT32                                Buffer32;
//[-start-140107-IB10920075-add]//
  STATIC UINT8                          OCW1 = 0;
//[-end-140107-IB10920075-add]//


//[-start-130305-IB05160418-modify]//
////[-start-120815-IB05300312-modify]//
//  MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
//  if ((MsrValue & (B_MSR_PLAT_FRMW_PROT_CTRL_EN | B_MSR_PLAT_FRMW_PROT_CTRL_LK)) == (B_MSR_PLAT_FRMW_PROT_CTRL_EN | B_MSR_PLAT_FRMW_PROT_CTRL_LK)) {
//    return EFI_SUCCESS;
//  }
////[-end-120815-IB05300312-modify]//

  if (mPfatProtocol != NULL) {
    return EFI_SUCCESS;
  }
//[-end-130305-IB05160418-modify]//
//[-start-140107-IB10920075-add]//
  if (!DisableCache) {
    OCW1 = IoRead8 (OCW1_DATA_PROT);
  }
//[-end-140107-IB10920075-add]//

  Buffer8 = PciReadConfig8 (PciCfgAddr(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC) +
                            R_PCH_LPC_BIOS_CNTL
                           );
  if (EnableWrites) {
    Buffer8 = (UINT8) (Buffer8 | B_PCH_LPC_BIOS_CNTL_BIOSWE);

    if (!DisableCache) {
//[-start-140107-IB10920075-add]//
      DisableIRQ0 (OCW1);      
//[-end-140107-IB10920075-add]//      
      AsmDisableCache();
      MsrValue = AsmReadMsr64 (MSR_CACHE_IA32_MTRR_DEF_TYPE);
      MsrValue = MsrValue & ~MSR_CACHE_MTRR_VALID;
      AsmWriteMsr64 (MSR_CACHE_IA32_MTRR_DEF_TYPE, MsrValue);
      DisableCache = TRUE;
    }
  } else {
    Buffer8 = (UINT8) (Buffer8 & (~B_PCH_LPC_BIOS_CNTL_BIOSWE));
    if (DisableCache) {
      AsmEnableCache();
      MsrValue = AsmReadMsr64 (MSR_CACHE_IA32_MTRR_DEF_TYPE);
      MsrValue = MsrValue  | MSR_CACHE_MTRR_VALID;
      AsmWriteMsr64 (MSR_CACHE_IA32_MTRR_DEF_TYPE, MsrValue);
//[-start-140107-IB10920075-add]//
      EnableIRQ0 (OCW1);
//[-end-140107-IB10920075-add]//        
      DisableCache = FALSE;
    }
  }

  PciWriteConfig8 (PciCfgAddr(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC) +
                   R_PCH_LPC_BIOS_CNTL,
                   Buffer8
                  );

  Buffer8 = PciReadConfig8 (PciCfgAddr(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC) +
                            R_PCH_LPC_BIOS_CNTL
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
      Buffer32 = MmioRead32 (mBiosWriteEnableMmio) | (UINT32) (BIT0);
    } else {
      Buffer32 = MmioRead32 (mBiosWriteEnableMmio) & (UINT32) (~BIT0);
    }
    AsmWriteMsr64(0x1FE, (UINT64)Buffer32);
  }

  return EFI_SUCCESS;

}


/**
  Enable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully enabled
  @return Others                Failed to enable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteEnable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  return EnableFvbWrites(TRUE);
}

/**
  Disable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully disabled
  @return Others                Failed to disable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteDisable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  return EnableFvbWrites (FALSE);
}


EFI_STATUS
EFIAPI
FlashWriteEnableLibInit (
  VOID
  )
{
  IrsiAddVirtualPointer ((VOID **)&mBiosWriteEnableMmio);
  return EFI_SUCCESS;
}

