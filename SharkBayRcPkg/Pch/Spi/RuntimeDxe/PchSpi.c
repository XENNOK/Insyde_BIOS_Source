/** @file
  PCH SPI Runtime Driver implements the SPI Host Controller Compatibility Interface.

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
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSpi.c

@brief
  PCH SPI Runtime Driver implements the SPI Host Controller Compatibility Interface.

**/
#include <PchSpi.h>

///
/// Global variables
///
SPI_INSTANCE        *mSpiInstance;
static CONST UINT32 mSpiRegister[] = {
  R_PCH_SPI_SSFS,
  R_PCH_SPI_PREOP,
  R_PCH_SPI_OPMENU,
  R_PCH_SPI_OPMENU + 4,
  R_PCH_SPI_VSCC0,
  R_PCH_SPI_VSCC1
};

//
// Function implementations
//

/**
  Fixup internal data pointers so that the services can be called in virtual mode.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.

  @retval None.
**/
VOID
PchSpiVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mSpiInstance->PchRootComplexBar));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mSpiInstance->SpiProtocol.Init));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mSpiInstance->SpiProtocol.Execute));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mSpiInstance->SpiDescriptor));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mSpiInstance));
}

/**
  Entry point for the SPI host controller driver.
  
  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
InstallPchSpi (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS                      Status;
  UINT64                          BaseAddress;
  UINT64                          Length;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdMemorySpaceDescriptor;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR LpcMemorySpaceDescriptor;
  UINT64                          Attributes;

  DEBUG ((EFI_D_INFO, "InstallPchSpi() Start\n"));

  Status = PciLibConstructor ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate Runtime memory for the SPI protocol instance.
  ///
  mSpiInstance = AllocateRuntimeZeroPool (sizeof (SPI_INSTANCE));
  if (mSpiInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the SPI protocol instance
  ///
  Status = SpiProtocolConstructor (mSpiInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install the EFI_SPI_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(mSpiInstance->Handle),
                  &gEfiSpiProtocolGuid,
                  &(mSpiInstance->SpiProtocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    FreePool (mSpiInstance);
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Set RCBA space in GCD to be RUNTIME so that the range will be supported in
  /// virtual address mode in EFI aware OS runtime.
  /// It will assert if RCBA Memory Space is not allocated
  /// The caller is responsible for the existence and allocation of the RCBA Memory Spaces
  ///
  BaseAddress = (EFI_PHYSICAL_ADDRESS) (mSpiInstance->PchRootComplexBar);
  Length      = 0x4000;

  Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &GcdMemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes = GcdMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  ///  LPC memory space
  ///
  BaseAddress = MmPciAddress(0,
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_LPC,
                             PCI_FUNCTION_NUMBER_PCH_LPC,
                             0
                             );
  Length  = 4096;

  Status  = gDS->GetMemorySpaceDescriptor (BaseAddress, &LpcMemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes = LpcMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);

  Status = PciLibRegisterMemory (
            PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            0),
            (UINTN) Length
            );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "InstallPchSpi() End\n"));

  return EFI_SUCCESS;
}

/**
  Save SPI VSCC0/VSCC1 register into S3 resume script table.

  @param[in] Event                The event that triggered this notification function  
  @param[in] ParentImageHandle    Pointer to the notification functions context
**/
VOID
EFIAPI
VsccS3SaveRestore (
  VOID
  )
{
  UINTN                           Index;

  for (Index = 0; Index < sizeof (mSpiRegister) / sizeof (UINT32); Index++) {
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (mSpiInstance->PchRootComplexBar + mSpiRegister[Index]),
      1,
      (VOID *) (UINTN) (mSpiInstance->PchRootComplexBar + mSpiRegister[Index])
      );
  }

  return;
}

/**
  This function is a hook for Spi Dxe phase specific initialization

  @param[in] None

  @retval None
**/
VOID
EFIAPI
SpiPhaseInit (
  VOID
  )
{
  VsccS3SaveRestore ();
}

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @param[in] None

  @retval None
**/
VOID
EFIAPI
DisableBiosWriteProtect (
  VOID
  )
{
  ///
  /// Enable the access to the BIOS space for both read and write cycles
  ///
  PciOr8 (
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    PCI_FUNCTION_NUMBER_PCH_LPC,
    R_PCH_LPC_BIOS_CNTL),
    (UINT8) (B_PCH_LPC_BIOS_CNTL_BIOSWE)
    );
}

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @param[in] None

  @retval None
**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  )
{
  ///
  /// Disable the access to the BIOS space for write cycles
  ///
  PciAnd8 (
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    PCI_FUNCTION_NUMBER_PCH_LPC,
    R_PCH_LPC_BIOS_CNTL),
    (UINT8) (~B_PCH_LPC_BIOS_CNTL_BIOSWE)
    );
}
