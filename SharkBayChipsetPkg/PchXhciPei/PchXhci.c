/** @file
  Implementation of PchXhciPei module for Crisis Recovery

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
//[-start-121116-IB10820168-modify]//
#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <IndustryStandard/Pci.h>
//
// Driver Produced PPI Prototypes
//
#include <Ppi/UsbController.h>

//
// Driver Consumed PPI Prototypes
//
#include <Ppi/PciCfg2.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/PchUsbPolicy.h>

#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>


#define XHCI_BUS_NUMBER                 0x02

#define CLASSCODE_REGISTER              0x08
#define XHCI_BASE_ADDRESS_REGISTER       0x10

#define XHCI_CLASSCODE                  0x0C033000

#define NUMBER_OF_PCIE_BRIDGES          1
#define PCIE_BRIDGE_ADDRESS             0x1C0600

#define ROOT_BRIDGE_BUS_REGISTER        0x18
#define ROOT_BRIDGE_ADDRESS_REGISTER    0x20

#define PEI_XHCI_SIGNATURE              SIGNATURE_32 ('X', 'H', 'C', 'I')

//
// The MdePkg/Include/Ppi/UsbController.h does not define PEI_XHCI_CONTROLLER
// It is defined here and is to be removed if it is defined in UsbController.h
//
#define PEI_XHCI_CONTROLLER             0x04

typedef struct {
  UINTN                         Signature;
  PEI_USB_CONTROLLER_PPI        UsbControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR        PpiList;
  EFI_PEI_NOTIFY_DESCRIPTOR     NotifyList;
  EFI_PEI_PCI_CFG2_PPI          *PciCfgPpi;
  UINTN                         TotalUsbControllers;
  UINTN                         MemBase;
  UINTN                         RootBridge;
  UINTN                         PciAddress;
} PEI_XHCI_DEVICE;

#define PEI_XHCI_DEVICE_FROM_THIS(a)        CR(a, PEI_XHCI_DEVICE, UsbControllerPpi, PEI_XHCI_SIGNATURE)
#define PEI_XHCI_DEVICE_FROM_NOTIFY_DESC(a) CR(a, PEI_XHCI_DEVICE, NotifyList, PEI_XHCI_SIGNATURE)
//[-end-121116-IB10820168-modify]//

EFI_STATUS
EFIAPI
GetXhciController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_CONTROLLER_PPI         *This,
  IN UINT8                          UsbControllerId,
  OUT UINTN                         *ControllerType,
  OUT UINTN                         *BaseAddress
  );

EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi
  );

//
// Globals
//
PEI_USB_CONTROLLER_PPI mUsbControllerPpi = { GetXhciController };

EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiUsbControllerPpiGuid,
  NULL
};

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiPpiNotifyCallback
};

//
// Helper function
//
EFI_STATUS
EnableXhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN PEI_XHCI_DEVICE          *PeiPchXhciDev,
  IN UINT8                    UsbControllerId
  );

/**

 @param [in]   FfsHeader
 @param [in]   PeiServices


**/
EFI_STATUS
EFIAPI
InitializePchXhci (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  PEI_XHCI_DEVICE         *PeiPchXhciDev;
//[-start-121116-IB10820168-add]//
  PCH_USB_POLICY_PPI      *UsbPolicyPpi;

  //
  // Locate UsbPolicy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchUsbPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&UsbPolicyPpi
             );
 if (EFI_ERROR(Status)) {
    return Status;
  }
  if (UsbPolicyPpi->UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return EFI_UNSUPPORTED;
  }
  if (UsbPolicyPpi->UsbConfig->Usb30Settings.PreBootSupport != PCH_DEVICE_ENABLE) {
    return EFI_UNSUPPORTED;
  }
//[-end-121116-IB10820168-add]//
  //
  // Get PciCfgPpi at first.
  //
  PciCfgPpi = (**PeiServices).PciCfg;

  PeiPchXhciDev = (PEI_XHCI_DEVICE *)AllocatePages (1);
  ASSERT (PeiPchXhciDev != NULL);
  ZeroMem (PeiPchXhciDev, sizeof(PeiPchXhciDev));

  PeiPchXhciDev->UsbControllerPpi     = mUsbControllerPpi;
  PeiPchXhciDev->PpiList              = mPpiList;
  PeiPchXhciDev->PpiList.Ppi          = &PeiPchXhciDev->UsbControllerPpi;
  PeiPchXhciDev->PciCfgPpi            = PciCfgPpi;
  PeiPchXhciDev->NotifyList           = mNotifyList;
  PeiPchXhciDev->TotalUsbControllers  = 1;

  //
  // Assign resources and enable XHCI controllers
  //
//[-start-121116-IB10820168-modify]//
  PeiPchXhciDev->MemBase = PcdGet32 ( PcdXhciMemBaseAddress );
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "XHCI_MEM_BASE_ADDRESS :%x\n",PeiPchXhciDev->MemBase));
  Status = EnableXhciController (PeiServices, PeiPchXhciDev, 0);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "EnableXhciController :%r\n",Status));
//[-end-121116-IB10820168-modify]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Install USB Controller PPI
  //
//[-start-121116-IB10820168-modify]//
  Status = PeiServicesInstallPpi (
             &PeiPchXhciDev->PpiList
             );
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "InstallPpi :%r\n",Status));
//[-end-121116-IB10820168-modify]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Install notification in order to reset the XHCI
  //
//[-start-121116-IB10820168-modify]//
  Status = PeiServicesNotifyPpi (
             &PeiPchXhciDev->NotifyList
             );
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "End PeimInitializePchXhci !\n"));
//[-end-121116-IB10820168-modify]//
  return Status;
}

/**
 Retrieve XHCI controller information

 @param [in]   PeiServices              Pointer to the PEI Services Table.
 @param [in]   This                     Pointer to PEI_AHCI_CONTROLLER_PPI
 @param [in]   UsbControllerId          USB Controller ID
 @param [out]   ControllerType          Result USB controller type
 @param [out]   BaseAddress             Result XHCI base address

 @retval EFI_INVALID_PARAMETER          Invalid AhciControllerId is given
 @retval EFI_SUCCESS                    XHCI controller information is retrieved successfully

**/
EFI_STATUS
EFIAPI
GetXhciController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_CONTROLLER_PPI         *This,
  IN UINT8                          UsbControllerId,
  OUT UINTN                         *ControllerType,
  OUT UINTN                         *BaseAddress
  )
{
  PEI_XHCI_DEVICE         *PeiPchXhciDev;

  PeiPchXhciDev = PEI_XHCI_DEVICE_FROM_THIS (This);

  if (UsbControllerId >= PeiPchXhciDev->TotalUsbControllers) {
    return EFI_INVALID_PARAMETER;
  }
  *ControllerType = PEI_XHCI_CONTROLLER;
  *BaseAddress = PeiPchXhciDev->MemBase;
  return EFI_SUCCESS;
}

/**
 GC_TODO: Add function description

 @param [in]   PeiServices      GC_TODO: add argument description
 @param [in]   PeiPchXhciDev    GC_TODO: add argument description
 @param [in]   UsbControllerId  GC_TODO: add argument description

 @retval EFI_INVALID_PARAMETER  GC_TODO: Add description for return value
 @retval EFI_SUCCESS            GC_TODO: Add description for return value

**/
EFI_STATUS
EnableXhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN PEI_XHCI_DEVICE          *PeiPchXhciDev,
  IN UINT8                    UsbControllerId
  )
{
  UINTN                     Index;
  EFI_PEI_PCI_CFG2_PPI      *PciCfgPpi;
  UINT32                    Bridge;
  UINT32                    Address;
  UINT32                    Register;

  if (UsbControllerId >= PeiPchXhciDev->TotalUsbControllers) {
    return EFI_INVALID_PARAMETER;
  }
  PciCfgPpi = PeiPchXhciDev->PciCfgPpi;
  //
  // Discover XHCI through PCIE bridge
  //
  for (Index = 0; Index < NUMBER_OF_PCIE_BRIDGES; Index ++) {
    //
    // Setup appropriate value to PCIE bridge
    //
    Bridge   = PCIE_BRIDGE_ADDRESS + (Index << 8);
    Register = (XHCI_BUS_NUMBER << 8) + (XHCI_BUS_NUMBER << 16);
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
    // Discover XHCI
    //
    Address  = (XHCI_BUS_NUMBER << 24);
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
      // XHCI not found, clear bus number to PCIE bridge
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
                 Address | CLASSCODE_REGISTER,
                 &Register
                 );
    Register &= 0xffffff00;
    if (Register != XHCI_CLASSCODE) {
      //
      // Not XHCI, clear bus number to PCIE bridge
      //
      goto error;
    }
    PeiPchXhciDev->RootBridge = Bridge;
    PeiPchXhciDev->PciAddress = Address;
    //
    // Assign address range for root bridge
    //
//[-start-121116-IB10820168-modify]//
    Register = ((PcdGet32 ( PcdXhciMemBaseAddress ) + 0x00100000) & 0xfff00000) + ((PcdGet32 ( PcdXhciMemBaseAddress ) & 0xfff00000) >> 16);
//[-end-121116-IB10820168-modify]//
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
//[-start-121116-IB10820168-modify]//
    Register = ((PcdGet32 ( PcdXhciMemBaseAddress ) + 0x00200000) & 0xfff00000) + (((PcdGet32 ( PcdXhciMemBaseAddress ) + 0x00100000) & 0xfff00000) >> 16);
//[-end-121116-IB10820168-modify]//
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Bridge | (ROOT_BRIDGE_ADDRESS_REGISTER + 4),
                 &Register
                 );
    //
    // Assign base address register to XHCI
    //
//[-start-121116-IB10820168-modify]//
    Register = PcdGet32 ( PcdXhciMemBaseAddress );
//[-end-121116-IB10820168-modify]//
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint32,
                 Address | XHCI_BASE_ADDRESS_REGISTER,
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
                 Bridge | PCI_COMMAND_OFFSET,
                 &Register
                 );
    Register |= 0x06;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Bridge | PCI_COMMAND_OFFSET,
                 &Register
                 );
    //
    // Enable XHCI
    //
    Register = 0;
    PciCfgPpi->Read (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Address | PCI_COMMAND_OFFSET,
                 &Register
                 );
    Register |= 0x06;
    PciCfgPpi->Write (
                 PeiServices,
                 PciCfgPpi,
                 EfiPeiPciCfgWidthUint16,
                 Address | PCI_COMMAND_OFFSET,
                 &Register
                 );
    return EFI_SUCCESS;
error:
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
 Register notify ppi to reset the XHCI.


 @param[in]   PeiServices               Pointer to the PEI Services Table.
 @param[in]   NotifyDescriptor          Pointer to the notify descriptor

 @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_PEI_PCI_CFG2_PPI        *PciCfgPpi;
  UINT8                       *BaseAddress;
  UINT32                      Register;
  PEI_XHCI_DEVICE             *PeiPchXhciDev;
  
  PeiPchXhciDev = PEI_XHCI_DEVICE_FROM_NOTIFY_DESC (NotifyDescriptor);

  //
  // Reset the HC
  //
//[-start-121116-IB10820168-modify]//
  BaseAddress = (UINT8*)PcdGet32 ( PcdXhciMemBaseAddress );
//[-end-121116-IB10820168-modify]//
  BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);
  //
  // Halt HC first
  //
  Register = *(UINT32*)(UINTN)BaseAddress;
  Register &= ~0x01;
  *(UINT32*)(UINTN)BaseAddress = Register;

  MicroSecondDelay (1);
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
  // Disable XHCI
  //
  Register = 0;
  PciCfgPpi->Read (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               PeiPchXhciDev->PciAddress | PCI_COMMAND_OFFSET,
               &Register
               );
  Register &= ~0x06;
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               PeiPchXhciDev->PciAddress | PCI_COMMAND_OFFSET,
               &Register
               );
  //
  // Disable root bridge
  //
  Register = 0;
  PciCfgPpi->Read (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               PeiPchXhciDev->RootBridge | PCI_COMMAND_OFFSET,
               &Register
               );
  Register &= ~0x06;
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint16,
               PeiPchXhciDev->RootBridge | PCI_COMMAND_OFFSET,
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
               PeiPchXhciDev->RootBridge | ROOT_BRIDGE_BUS_REGISTER,
               &Register
               );
  //
  // Clear address range for root bridge
  //
  PciCfgPpi->Write (
               (CONST EFI_PEI_SERVICES **)PeiServices,
               PciCfgPpi,
               EfiPeiPciCfgWidthUint32,
               PeiPchXhciDev->RootBridge | ROOT_BRIDGE_ADDRESS_REGISTER,
               &Register
               );
  return EFI_SUCCESS;
}