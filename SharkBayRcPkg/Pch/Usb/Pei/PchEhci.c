/** @file
  Pch Ehci PPI Init

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchEhci.c

@brief
  Pch Ehci PPI Init

**/
#include <PchEhci.h>
#include <Library/PchPlatformLib.h>

EFI_GUID  mPeiEhciControllerPpiGuid = PEI_USB_CONTROLLER_PPI_GUID;

///
/// PPI interface function
///
STATIC
EFI_STATUS
EFIAPI
GetEhciController (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  PEI_USB_CONTROLLER_PPI          *This,
  IN  UINT8                           UsbControllerId,
  OUT UINTN                           *ControllerType,
  OUT UINTN                           *BaseAddress
  );

///
/// Globals variable
///
STATIC PEI_USB_CONTROLLER_PPI   mEhciControllerPpi = { GetEhciController };

STATIC EFI_PEI_PPI_DESCRIPTOR   mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &mPeiEhciControllerPpiGuid,
  NULL
};

///
/// Helper function
///
STATIC
EFI_STATUS
EnableEhciController (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN PCH_EHCI_DEVICE          *PeiPchEhciDev,
  IN UINT8                    UsbControllerId
  );

/**
  Initialize PCH EHCI PEIM

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] UsbPolicyPpi         PCH Usb Policy PPI

  @retval EFI_SUCCESS             The PCH EHCI PEIM is initialized successfully
  @retval EFI_INVALID_PARAMETER   UsbControllerId is out of range
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
InitForEHCI (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN PCH_USB_POLICY_PPI         *UsbPolicyPpi
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  PCH_EHCI_DEVICE       *PeiPchEhciDev;
  EFI_BOOT_MODE         BootMode;

  DEBUG ((EFI_D_INFO, "InitForEHCI() Start\n"));

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

  ///
  /// We do not export this in S3 boot path, because it is only for recovery.
  ///
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  PeiPchEhciDev = (PCH_EHCI_DEVICE *) AllocatePool (sizeof (PCH_EHCI_DEVICE));
  if (PeiPchEhciDev == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for PeiPchEhciDev! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

//[-start-130325-IB10130038-add]//
  PeiPchEhciDev->Signature            = PEI_PCH_EHCI_SIGNATURE;
//[-end-130325-IB10130038-add]//
  PeiPchEhciDev->EhciControllerPpi    = mEhciControllerPpi;
  PeiPchEhciDev->PpiList              = mPpiList;
  PeiPchEhciDev->PpiList.Ppi          = &PeiPchEhciDev->EhciControllerPpi;

//[-start-120710-IB05330355-modify]//
  PeiPchEhciDev->TotalEhciControllers = GetPchEhciMaxControllerNum();
//[-end-120710-IB05330355-modify]//

  ///
  /// Assign resources and enable EHCI controllers
  ///
  if (UsbPolicyPpi->EhciMemLength < (EHCI_MEMORY_SPACE * PeiPchEhciDev->TotalEhciControllers)) {
    DEBUG ((EFI_D_ERROR, "The EhciMemLength got from UsbPolicyPpi is less than the required (%0x) !\n", (EHCI_MEMORY_SPACE * PeiPchEhciDev->TotalEhciControllers)));
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < PeiPchEhciDev->TotalEhciControllers; Index++) {
    PeiPchEhciDev->MemBase[Index] = UsbPolicyPpi->EhciMemBaseAddr + EHCI_MEMORY_SPACE * Index;
    Status                        = EnableEhciController (PeiServices, PeiPchEhciDev, (UINT8) Index);
    ASSERT_EFI_ERROR (Status);
  }
  ///
  /// Install USB Controller PPI
  ///
  Status = PeiServicesInstallPpi (&PeiPchEhciDev->PpiList);

  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "InitForEHCI() End\n"));

  return Status;

}

///
/// PPI interface implementation
///

/**
  Get EHCI controller information

  @param[in] PeiServices          General PEI services
  @param[in] This                 Pointer to the PEI_EHCI_CONTROLLER_PPI
  @param[in] UsbControllerId      The USB controller number
  @param[out] ControllerType      Output: USB controller type
  @param[out] BaseAddress         Output: EHCI controller memory base address

  @retval EFI_INVALID_PARAMETER   UsbControllerId is out of range
  @retval EFI_SUCCESS             Function completes successfully
**/
STATIC
EFI_STATUS
EFIAPI
GetEhciController (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  PEI_USB_CONTROLLER_PPI          *This,
  IN  UINT8                           UsbControllerId,
  OUT UINTN                           *ControllerType,
  OUT UINTN                           *BaseAddress
  )
{
  PCH_EHCI_DEVICE *PeiPchEhciDev;

  PeiPchEhciDev = PCH_EHCI_DEVICE_FROM_THIS (This);

  if (UsbControllerId >= PeiPchEhciDev->TotalEhciControllers) {
    return EFI_INVALID_PARAMETER;
  }

  *ControllerType = PEI_EHCI_CONTROLLER;

  *BaseAddress    = PeiPchEhciDev->MemBase[UsbControllerId];

  return EFI_SUCCESS;
}

/**
  Enable the EHCI controller

  @param[in] PeiServices          The general PEI services
  @param[in] PeiPchEhciDev        The EHCI device
  @param[in] UsbControllerId      The USB Controller number

  @retval EFI_INVALID_PARAMETER   UsbControllerId is out of range
  @retval EFI_SUCCESS             The function completes successfully
**/
STATIC
EFI_STATUS
EnableEhciController (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN PCH_EHCI_DEVICE          *PeiPchEhciDev,
  IN UINT8                    UsbControllerId
  )
{
  UINTN       BaseAddress;
  UINTN       EhciAddress;
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  if (UsbControllerId >= PeiPchEhciDev->TotalEhciControllers) {
    return EFI_INVALID_PARAMETER;
  }

  BaseAddress = PeiPchEhciDev->MemBase[UsbControllerId];

  EhciAddress = EFI_UNSUPPORTED;
  if (PchSeries == PchH) {
    EhciAddress = PCH_H_PCIE_EHCI_ADDR (UsbControllerId);
  } else if (PchSeries == PchLp) {
    EhciAddress = PCH_LP_PCIE_EHCI_ADDR (UsbControllerId);
  }

  if (EhciAddress == EFI_UNSUPPORTED) {
    return EFI_INVALID_PARAMETER;
  }
  
  ///
  /// Assign base address register
  ///
  MmioWrite32 ((EhciAddress + R_PCH_EHCI_MEM_BASE), BaseAddress);

  ///
  /// Enable PCH EHCI register
  ///
  MmioOr16 (
    (EhciAddress + R_PCH_EHCI_COMMAND_REGISTER),
    (UINT16) (B_PCH_EHCI_COMMAND_BME | B_PCH_EHCI_COMMAND_MSE)
    );

  return EFI_SUCCESS;
}
