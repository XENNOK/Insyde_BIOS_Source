/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//[-start-121102-IB10820148-modify]//
#include <Uefi.h>
#include <PchRegs.h>
#include <Ppi/PciCfg.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/AhciController.h>
#include <Ppi/Stall.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
//[-start-121108-IB11410019-modify]//
#include <IndustryStandard/Pci22.h>
//[-end-121108-IB11410019-modify]//

#define AHCI_BUS_NUMBER                    0x00
//[-start-121108-IB11410019-modify]//
#define AHCI_CLASSC                        ((PCI_CLASS_MASS_STORAGE << 8) | V_PCH_SATA_SUB_CLASS_CODE_AHCI)
//[-end-121108-IB11410019-modify]//
#define AHCI_MEM_BASE_ADDRESS              PcdGet32 ( PcdAhciMemBaseAddress )
#define ICH_PCIE_BRIDGE_ADDRESS            PcdGet32 ( PcdIchPcieBridgeAddress )
#define PEI_AHCI_SIGNATURE                 SIGNATURE_32 ('P', 'A', 'H', 'C')

typedef struct _PEI_AHCI_DEVICE {
  UINTN                    Signature;
  PEI_AHCI_CONTROLLER_PPI  AhciControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR   PpiList;
  EFI_PEI_PCI_CFG2_PPI     *PciCfgPpi;
  UINTN                    TotalAhciControllers;
  EFI_PHYSICAL_ADDRESS     MemBase;
} PEI_AHCI_DEVICE;

#define PEI_AHCI_DEVICE_FROM_THIS(a) \
  CR(a, PEI_AHCI_DEVICE, AhciControllerPpi, PEI_AHCI_SIGNATURE)
//[-end-121102-IB10820148-modify]//

//
// PPI interface function
//
STATIC
EFI_STATUS
EFIAPI
GetAhciMemBase (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_PPI        *This,
  IN UINT8                          AhciControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS       *AhciMemBaseAddr
  );

STATIC
EFI_STATUS
EFIAPI
AhciEndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

//
// Globals
//
STATIC PEI_AHCI_CONTROLLER_PPI mAhciControllerPpi = { GetAhciMemBase };

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiAhciControllerPpiGuid,
  NULL
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  AhciEndOfPeiPpiNotifyCallback
};

UINT32 mRootBridge;
UINT32 mPciAddress;

//
// Helper function
//
STATIC
EFI_STATUS
EnableAhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN PEI_AHCI_DEVICE          *PeiPchAhciDev,
  IN UINT8                    AhciControllerId
  );

/**

 PEIM Entry Point

**/
EFI_STATUS
PeimInitializePchAhci (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  EFI_PHYSICAL_ADDRESS    AllocateAddress;
  PEI_AHCI_DEVICE         *PeiPchAhciDev;

  //
  // Get PciCfgPpi at first.
  //
  PciCfgPpi = (**PeiServices).PciCfg;

//[-start-121102-IB10820148-modify]//
  Status = PeiServicesAllocatePages (
             EfiBootServicesData,
             1,
             &AllocateAddress
             );
//[-end-121102-IB10820148-modify]//

  ASSERT_EFI_ERROR (Status);

  PeiPchAhciDev = (PEI_AHCI_DEVICE *)((UINTN)AllocateAddress);
  ZeroMem (PeiPchAhciDev, sizeof(PeiPchAhciDev));

  PeiPchAhciDev->Signature            = PEI_AHCI_SIGNATURE;
  PeiPchAhciDev->AhciControllerPpi    = mAhciControllerPpi;
  PeiPchAhciDev->PpiList              = mPpiList;
  PeiPchAhciDev->PpiList.Ppi          = &PeiPchAhciDev->AhciControllerPpi;
  PeiPchAhciDev->PciCfgPpi            = PciCfgPpi;
  PeiPchAhciDev->TotalAhciControllers = 1;

  //
  // Assign resources and enable AHCI controllers
  //
  PeiPchAhciDev->MemBase = AHCI_MEM_BASE_ADDRESS;
  Status = EnableAhciController (PeiServices, PeiPchAhciDev, 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Install AHCI Controller PPI
  //
//[-start-121102-IB10820148-modify]//
  Status = PeiServicesInstallPpi (
             &PeiPchAhciDev->PpiList
             );
//[-end-121102-IB10820148-modify]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Install notification in order to reset the AHCI
  //
//[-start-121102-IB10820148-modify]//
  Status = PeiServicesNotifyPpi (
             &mNotifyList
             );
//[-end-121102-IB10820148-modify]//
  return Status;
}

//
// PPI interface implementation
//
/**

 GC_TODO: Add function description

 @param   PeiServices             GC_TODO: add argument description
 @param   This                    GC_TODO: add argument description
 @param   AhciControllerId        GC_TODO: add argument description
 @param   BaseAddress             GC_TODO: add argument description

 @retval  EFI_INVALID_PARAMETER   GC_TODO: Add description for return value
 @retval  EFI_SUCCESS             GC_TODO: Add description for return value

**/
STATIC
EFI_STATUS
EFIAPI
GetAhciMemBase (
  IN     EFI_PEI_SERVICES           **PeiServices,
  IN     PEI_AHCI_CONTROLLER_PPI    *This,
  IN     UINT8                      AhciControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS       *AhciMemBaseAddr
  )
{
  PEI_AHCI_DEVICE         *PeiPchAhciDev;

  PeiPchAhciDev = PEI_AHCI_DEVICE_FROM_THIS (This);

  if (AhciControllerId >= PeiPchAhciDev->TotalAhciControllers) {
    return EFI_INVALID_PARAMETER;
  }
  *AhciMemBaseAddr = PeiPchAhciDev->MemBase;
  return EFI_SUCCESS;
}

/**

 GC_TODO: Add function description

 @param   PeiServices             GC_TODO: add argument description
 @param   PeiPchAhciDev           GC_TODO: add argument description
 @param   AhciControllerId        GC_TODO: add argument description

 @retval  EFI_INVALID_PARAMETER   GC_TODO: Add description for return value
 @retval  EFI_SUCCESS             GC_TODO: Add description for return value

**/
STATIC
EFI_STATUS
EnableAhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN PEI_AHCI_DEVICE          *PeiPchAhciDev,
  IN UINT8                    AhciControllerId
  )
{
  UINTN                     Index;
  EFI_PEI_PCI_CFG2_PPI      *PciCfgPpi;
  UINT32                    Bridge;
  UINT32                    Address;
  UINT32                    Register;
  UINT16                    WordReg;
  UINTN                     RegIndex;
//[-start-121102-IB10820148-modify]//
  UINT16                    ICH_NUMBER_OF_PCIE_BRIDGES;

  ICH_NUMBER_OF_PCIE_BRIDGES = LPTH_PCIE_MAX_ROOT_PORTS;
  if ( FeaturePcdGet ( PcdUltFlag ) ) {
    ICH_NUMBER_OF_PCIE_BRIDGES = LPTLP_PCIE_MAX_ROOT_PORTS;
  }
//[-end-121102-IB10820148-modify]//
  if (AhciControllerId >= PeiPchAhciDev->TotalAhciControllers) {
    return EFI_INVALID_PARAMETER;
  }
  PciCfgPpi = PeiPchAhciDev->PciCfgPpi;
  //
  // Discover AHCI through PCIE bridge
  //
  for (Index = 0; Index < ICH_NUMBER_OF_PCIE_BRIDGES; Index ++) {
    //
    // Setup appropriate value to PCIE bridge
    //
    Bridge   = ICH_PCIE_BRIDGE_ADDRESS + (Index << 8);
    Register = (AHCI_BUS_NUMBER << 8) + (AHCI_BUS_NUMBER << 16);
    //
    // Assign bus number to PCIE bridge
    //
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
                 (Bridge | R_PCH_PCIE_BNUM),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
    //
    // Discover AHCI
    //
//[-start-121102-IB10820148-modify]//
    Address  = (AHCI_BUS_NUMBER << 24) + (PCI_DEVICE_NUMBER_PCH_SATA << 16) + (PCI_FUNCTION_NUMBER_PCH_SATA << 8);
//[-end-121102-IB10820148-modify]//
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Address,
                 &Register
                 );
    if (Register == 0xffff) {
      //
      // AHCI is not found, clear bus number to PCIE bridge
      //
      goto error;
    }
    //
    // Check the class code 
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
                 (Address | R_PCH_SATA_SUB_CLASS_CODE),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
//[-start-121108-IB11410019-modify]//
    Register &= 0x0000ffff;
//[-end-121108-IB11410019-modify]//
    if (Register != AHCI_CLASSC) {
      //
      // Not AHCI, clear bus number to PCIE bridge
      //
      goto error;
    }
    mRootBridge = Bridge;
    mPciAddress = Address;
    //
    // Set Map to AHCI
    //
    Register = 0;
    Register |= V_PCH_SATA_MAP_SMS_AHCI;
    Register |= B_PCH_SATA_PORT_TO_CONTROLLER_CFG;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_MAP),
                 &Register
                 );
    //
    // Set PCS
    // default is enable all
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_PCS),
                 &Register
                 );
//[-start-120628-IB06460410-modify]//
    for (RegIndex = 0; RegIndex < LPTH_AHCI_MAX_PORTS ; RegIndex++) {
//[-end-120628-IB06460410-modify]//
      Register |= (B_PCH_SATA_PCS_PORT0_EN << RegIndex);
    }
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_PCS),
                 &Register
                 );
    WordReg = 0;
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_PCS),
                 &WordReg
                 );
//[-start-120628-IB06460410-modify]//
    for (RegIndex = 0; RegIndex < LPTH_AHCI_MAX_PORTS; RegIndex++) {
//[-end-120628-IB06460410-modify]//
      //
      // Check R_PCH_SATA_PCS if port is present so as to enable SATA port
      //
      if ((WordReg & (BIT0 << (8 + RegIndex)))) {
       Register |= (BIT0 << RegIndex);
      }
    }
    WordReg &= (UINT16)(~(B_PCH_SATA_PCS_PORT5_EN |
                          B_PCH_SATA_PCS_PORT4_EN |
                          B_PCH_SATA_PCS_PORT3_EN |
                          B_PCH_SATA_PCS_PORT2_EN |
                          B_PCH_SATA_PCS_PORT1_EN |
                          B_PCH_SATA_PCS_PORT0_EN 
                         )
                       );
    WordReg |= (UINT16) Register;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_PCS),
                 &WordReg
                 );
    //
    // Assign address range for root bridge
    //
    Register = ((AHCI_MEM_BASE_ADDRESS + 0x00100000) & 0xfff00000) + ((AHCI_MEM_BASE_ADDRESS & 0xfff00000) >> 16);
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
                 (Bridge | R_PCH_PCIE_MBL),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
    //
    // Assign address prefetchable range for root bridge
    //
    Register = ((AHCI_MEM_BASE_ADDRESS + 0x00200000) & 0xfff00000) + (((AHCI_MEM_BASE_ADDRESS + 0x00100000) & 0xfff00000) >> 16);
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
                 (Bridge | (R_PCH_PCIE_MBL + 4)),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
    //
    // Assign base address register to AHCI
    //
    Register = AHCI_MEM_BASE_ADDRESS;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 (Address | R_PCH_SATA_AHCI_BAR),
                 &Register
                 );   
    //
    // Enable AHCI
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
//[-start-121102-IB10820148-modify]//
                 (Address | R_PCH_SATA_COMMAND),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
    Register |= (B_PCH_SATA_COMMAND_BME | B_PCH_SATA_COMMAND_MSE | B_PCH_SATA_COMMAND_IOSE);
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
//[-start-121102-IB10820148-modify]//
                 (Address | R_PCH_SATA_COMMAND),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );   
    //
    // Assign ABAR PI register
    //
    WordReg = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 (Address | R_PCH_SATA_PCS),
                 &WordReg
                 );
    Register = 0;
    Register = *(volatile UINT32*) (AHCI_MEM_BASE_ADDRESS + R_PCH_SATA_AHCI_PI);
//[-start-120628-IB06460410-modify]//
    Register &= (UINT32)(~B_PCH_H_SATA_PORT_MASK);
//[-end-120628-IB06460410-modify]//
    Register |= (UINT32) WordReg;
    *(volatile UINT32*) (AHCI_MEM_BASE_ADDRESS + R_PCH_SATA_AHCI_PI) = Register;
    
    //
    // Assign ABAR CAP register
    //
//[-start-121102-IB10820148-modify]//
    Register = PcdGet32 ( PcdAbarCapDefault );
//[-end-121102-IB10820148-modify]//
    *(volatile UINT32*) (AHCI_MEM_BASE_ADDRESS + R_PCH_SATA_AHCI_CAP) = Register;
        
    return EFI_SUCCESS;
error:
    Register = 0;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
                 (Bridge | R_PCH_PCIE_BNUM),
//[-end-121102-IB10820148-modify]//
                 &Register
                 );
  }

  return EFI_NOT_FOUND;
}

/**

 Register notify ppi to reset the AHCI.

 CAUTION!!! This function may NOT be called due to the 
 MAX_PPI_DESCRIPTORS(in PeiCore.h) exceed 64 and caused
 AHCI malfunction in DXE phase. For this situation, put
 the AHCI HC reset code into DXE chipset init driver

 @param   PeiServices     General purpose services available to every PEIM.

 @retval  EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
AhciEndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_STALL_PPI           *StallPpi;
  EFI_PEI_PCI_CFG2_PPI        *PciCfgPpi;
  UINT8                       *BaseAddress;
  UINT32                      Register;
    
//[-start-121102-IB10820148-modify]//
  Status = PeiServicesLocatePpi (
             &gEfiPeiStallPpiGuid,
             0,
             NULL,
             (VOID **)&StallPpi
             );
//[-end-121102-IB10820148-modify]//
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Reset the HC
  //
  BaseAddress = (UINT8*)AHCI_MEM_BASE_ADDRESS;
  BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);
  //
  // Halt HC first
  //
  Register = *(UINT32*)(UINTN)BaseAddress;
  Register &= ~0x01;
  *(UINT32*)(UINTN)BaseAddress = Register;
  //
  // Stall 1 millisecond
  //
  StallPpi->Stall (
              (CONST EFI_PEI_SERVICES **)PeiServices,
              StallPpi,
              1000
              );
  //
  // HCReset
  //
  Register = *(UINT32*)(UINTN)BaseAddress;
  Register |= 0x02;
  *(UINT32*)(UINTN)BaseAddress = Register;
  //
  // Get PciCfgPpi at first.
  //
  PciCfgPpi = (**PeiServices).PciCfg;
  //
  // Disable AHCI
  //
  Register = 0;
  PciCfgPpi->Read (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
//[-start-121102-IB10820148-modify]//
               (mPciAddress | R_PCH_PCIE_PCICMD),
//[-end-121102-IB10820148-modify]//
               &Register
               );
  Register &= ~0x06;
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
//[-start-121102-IB10820148-modify]//
               (mPciAddress | R_PCH_PCIE_PCICMD),
//[-end-121102-IB10820148-modify]//
               &Register
               );
  //
  // Clear bus number for root bridge
  //
  Register = 0;
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
               (mRootBridge | R_PCH_PCIE_BNUM),
//[-end-121102-IB10820148-modify]//
               &Register
               );
  //
  // Clear address range for root bridge
  //
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint32,
//[-start-121102-IB10820148-modify]//
               (mRootBridge | R_PCH_PCIE_MBL),
//[-end-121102-IB10820148-modify]//
               &Register
               );
  return EFI_SUCCESS;
}
