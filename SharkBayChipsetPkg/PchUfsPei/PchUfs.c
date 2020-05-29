/** @file
  Implementation of PchUfsPei module for Crisis Recovery

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PchUfs.h"

EFI_STATUS
GetUfsMemBase (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_UFS_CONTROLLER_PPI         *This,
  IN     UINT8                          UfsControllerId,
     OUT UINT32                         *ControllerType,
     OUT EFI_PHYSICAL_ADDRESS           *MemBase
  );

EFI_STATUS
EFIAPI
UfsEndOfPeiPpiNotifyCallback (
  IN     CONST EFI_PEI_SERVICES          **PeiServices,
  IN     CONST EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN     VOID                           *Ppi
  );

EFI_STATUS
EnableUfsController (
  IN     CONST EFI_PEI_SERVICES   **PeiServices,
  IN     PEI_UFS_DEVICE          *PeiPchUfsDev,
  IN     UINT8                    UfsControllerId
  );

//
// Globals
//
PEI_UFS_CONTROLLER_PPI mUfsControllerPpi = { GetUfsMemBase };

EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiUfsControllerPpiGuid,
  NULL
};

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  UfsEndOfPeiPpiNotifyCallback
};

UINT32  mRootBridge;
UINT32  mPciAddress;

/**
  Install PeiUfsControllerPpi for recovery.
 
  @param[in] FileHandle                 Handle of the file being invoked.
  @param[in] PeiServices                Describes the list of possible PEI Services.
 
  @retval EFI_SUCCESS                    PeiUfsControllerPpi is successfully installed
  @retval EFI_OUT_OF_RESOURCES           Cannot allocate resources
  @retval Others                         Fail to install PeiUfsControllerPpi

**/
EFI_STATUS
InitializePchUfs (
  IN     EFI_PEI_FILE_HANDLE        FileHandle,
  IN     CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  PEI_UFS_DEVICE          *PeiPchUfsDev;

  //
  // Get PciCfgPpi at first.
  //
  PciCfgPpi = (**PeiServices).PciCfg;

  PeiPchUfsDev = (PEI_UFS_DEVICE *)AllocatePages (1);
  if (PeiPchUfsDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (PeiPchUfsDev, sizeof (PEI_UFS_DEVICE));

  PeiPchUfsDev->Signature            = PEI_UFS_SIGNATURE;
  PeiPchUfsDev->UfsControllerPpi     = mUfsControllerPpi;
  PeiPchUfsDev->PpiList              = mPpiList;
  PeiPchUfsDev->PpiList.Ppi          = &PeiPchUfsDev->UfsControllerPpi;
  PeiPchUfsDev->PciCfgPpi            = PciCfgPpi;
  PeiPchUfsDev->TotalUfsControllers  = 1;

  //
  // Assign resources and enable UFS controllers
  //
  PeiPchUfsDev->MemBase = UFS_MEM_BASE_ADDRESS;
  Status = EnableUfsController (PeiServices, PeiPchUfsDev, 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Install UFS Controller PPI
  //
  Status = (**PeiServices).InstallPpi (
                             PeiServices,
                             &PeiPchUfsDev->PpiList
                             );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // Install notification in order to reset the UFS
  //
  Status = (**PeiServices).NotifyPpi (
                             PeiServices,
                             &mNotifyList
                             );
  return Status;
}

/**
  Get memory mapped base address of UFS
 
  @param [in]      PeiServices              Pointer to the PEI Services Table.
  @param [in]      This                     Pointer to PEI_UFS_CONTROLLER_PPI
  @param [in]      UfsControllerId          UFS controller ID
  @param [in, out] UfsMemBaseAddr           The result UFS memory base address
 
  @retval EFI_SUCCESS                       UFS memory base address is successfully retrieved
  @retval EFI_INVALID_PARAMETER             Invalid input used.
**/
EFI_STATUS
GetUfsMemBase (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_UFS_CONTROLLER_PPI         *This,
  IN     UINT8                          UfsControllerId,
     OUT UINT32                         *ControllerType,
     OUT EFI_PHYSICAL_ADDRESS           *MemBase
  )
{
  PEI_UFS_DEVICE                     *PeiPchUfsDev;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (ControllerType == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (MemBase == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PeiPchUfsDev = PEI_UFS_DEVICE_FROM_THIS (This);

  if (UfsControllerId >= PeiPchUfsDev->TotalUfsControllers) {
    return EFI_INVALID_PARAMETER;
  }
  
  *ControllerType = PEI_UFS_CONTROLLER;
  *MemBase  = PeiPchUfsDev->MemBase;

  return EFI_SUCCESS;
}

/**
  Enable UFS controller
 
  @param [in]   PeiServices              Pointer to the PEI Services Table.
  @param [in]   PeiPchUfsDev             Pointer the PEI_UFS_DEVICE structure
  @param [in]   UfsControllerId          The UFS Controller ID
 
  @retval EFI_SUCCESS                    UFS controller is succesfully enabled
  @retval EFI_INVALID_PARAMETER          Invalid UfsControllerId is given

**/
EFI_STATUS
EnableUfsController (
  IN     CONST EFI_PEI_SERVICES   **PeiServices,
  IN     PEI_UFS_DEVICE          *PeiPchUfsDev,
  IN     UINT8                    UfsControllerId
  )
{
  UINTN                     Index;
  EFI_PEI_PCI_CFG2_PPI      *PciCfgPpi;
  UINT32                    Bridge;
  UINT32                    Address;
  UINT32                    Register;
  UINT8                     ClassCode;
  UINT8                     SubClassCode;

  if (UfsControllerId >= PeiPchUfsDev->TotalUfsControllers) {
    return EFI_INVALID_PARAMETER;
  }
  
  PciCfgPpi = PeiPchUfsDev->PciCfgPpi;
  //
  // Discover UFS through PCIE bridge
  //
  for (Index = 0; Index < ICH_NUMBER_OF_PCIE_BRIDGES; Index ++) {
    //
    // Setup appropriate value to PCIE bridge
    //
    Bridge   = ICH_PCIE_BRIDGE_ADDRESS + (Index << 8);
    Register = (UFS_BUS_NUMBER << 8) + (UFS_BUS_NUMBER << 16);
    //
    // Assign bus number to PCIE bridge
    //
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Bridge | ROOT_BRIDGE_BUS_REGISTER,
                 &Register
                 );
    //
    // Discover SDHC
    //
    Address  = (UFS_BUS_NUMBER << 24);
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
      // UFS is not found, clear bus number to PCIE bridge
      //
      goto Error;
    }
    //
    // Check the class code 
    //
    ClassCode    = 0;
    SubClassCode = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint8,
                 (Address | PCI_CLASSCODE_OFFSET + 2),
                 &ClassCode
                 );
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint8,
                 (Address | PCI_CLASSCODE_OFFSET + 1),
                 &SubClassCode
                 );
    if (ClassCode != PCI_CLASS_MEMORY_CONTROLLER || SubClassCode != PCI_CLASS_MEMORY_FLASH) {
      //
      // Not UFS, clear bus number to PCIE bridge
      //
      goto Error;
    }
    mRootBridge = Bridge;
    mPciAddress = Address;

    //
    // Assign address range for root bridge
    //
    Register = ((UFS_MEM_BASE_ADDRESS + 0x00100000) & 0xfff00000) + ((UFS_MEM_BASE_ADDRESS & 0xfff00000) >> 16);
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Bridge | ROOT_BRIDGE_ADDRESS_REGISTER,
                 &Register
                 );
    //
    // Assign address prefetchable range for root bridge
    //
    Register = ((UFS_MEM_BASE_ADDRESS + 0x00200000) & 0xfff00000) + (((UFS_MEM_BASE_ADDRESS + 0x00100000) & 0xfff00000) >> 16);
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Bridge | ROOT_BRIDGE_ADDRESS_REGISTER + 4,
                 &Register
                 );
    //
    // Assign base address 1 register to UFS
    //
    Register = UFS_MEM_BASE_ADDRESS;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Address | BASE_ADDRESS_REGISTER_1,
                 &Register
                 );

    //
    // Assign base address 1 register to UFS
    //
    Register = UFS_MEM_BASE_ADDRESS + 0x10000;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Address | BASE_ADDRESS_REGISTER_0,
                 &Register
                 );
    //
    // Enable root bridge
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Bridge | COMMAND_REGISTER,
                 &Register
                 );
    Register |= 0x06;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Bridge | COMMAND_REGISTER,
                 &Register
                 );
    //
    // Enable UFS
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Address | COMMAND_REGISTER,
                 &Register
                 );
    Register |= 0x06;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Address | COMMAND_REGISTER,
                 &Register
                 );
    
    return EFI_SUCCESS;
    
Error:
    Register = 0;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Bridge | ROOT_BRIDGE_BUS_REGISTER,
                 &Register
                 );
  }

  return EFI_NOT_FOUND;
}

/**
  Register notify ppi to reset the UFS controller.
 
  @param[in]   PeiServices               Pointer to the PEI Services Table
  @param[in]   NotifyDescriptor          Pointer to the notify descriptor

  @retval EFI_SUCCESS                    Reset UFS controller successfully

**/
EFI_STATUS
EFIAPI
UfsEndOfPeiPpiNotifyCallback (
  IN     CONST EFI_PEI_SERVICES               **PeiServices,
  IN     CONST EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN     VOID                                 *Ppi
  )
{
  EFI_PEI_PCI_CFG2_PPI        *PciCfgPpi;
  UINT32                      Register;

  //
  // Get PciCfgPpi at first.
  //
  PciCfgPpi = (**PeiServices).PciCfg;
  
  //
  // Disable UFS
  //
  Register = 0;
  PciCfgPpi->Read (
               PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               (mPciAddress | PCI_COMMAND_OFFSET),
               &Register
               );
  
  Register &= ~0x06;
  PciCfgPpi->Write (
               PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               (mPciAddress | PCI_COMMAND_OFFSET),
               &Register
               );
  
  //
  // Clear bus number for root bridge
  //
  Register = 0;
  PciCfgPpi->Write (
               PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint32,
               (mRootBridge | ROOT_BRIDGE_BUS_REGISTER),
               &Register
               );
  
  //
  // Clear address range for root bridge
  //
  PciCfgPpi->Write (
               PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint32,
               (mRootBridge | ROOT_BRIDGE_ADDRESS_REGISTER),
               &Register
               );
  
  return EFI_SUCCESS;
}
