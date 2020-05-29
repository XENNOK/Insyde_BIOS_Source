/** @file
  PCH SPI SMM Driver implements the SPI Host Controller Compatibility Interface.

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
  Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
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
  PCH SPI SMM Driver implements the SPI Host Controller Compatibility Interface.

**/
#include <PchSpi.h>

//
// Global variables
//
EFI_SMM_BASE_PROTOCOL *mSmmBase;
EFI_SMM_SYSTEM_TABLE  *mSmst;
SPI_INSTANCE          *mSpiInstance;

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
  EFI_STATUS  Status;
  BOOLEAN     InSmm;

  ///
  /// Locate SMM Base Protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  ASSERT_EFI_ERROR (Status);

  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    mSmmBase->InSmm (mSmmBase, &InSmm);
  }
  if (!InSmm) {
    return EFI_SUCCESS;
  }

  ///
  /// Initialize our module variables
  ///
  Status = mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate pool for SPI protocol instance
  ///
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData, /// MemoryType don't care
                    sizeof (SPI_INSTANCE),
                    (VOID **) &mSpiInstance
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mSpiInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) mSpiInstance, sizeof (SPI_INSTANCE));
  ///
  /// Initialize the SPI protocol instance
  ///
  Status = SpiProtocolConstructor (mSpiInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install the SMM EFI_SPI_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(mSpiInstance->Handle),
                  &gEfiSmmSpiProtocolGuid,
                  &(mSpiInstance->SpiProtocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    mSmst->SmmFreePool (mSpiInstance);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  This function is a a hook for Spi Smm phase specific initialization

  @param[in] None

  @retval None
**/
VOID
EFIAPI
SpiPhaseInit (
  VOID
  )
{
  UINTN               Index;
  static CONST UINT32 SpiRegister[] = {
    R_PCH_SPI_SSFS,
    R_PCH_SPI_PREOP,
    R_PCH_SPI_OPMENU,
    R_PCH_SPI_OPMENU + 4,
    R_PCH_SPI_VSCC0,
    R_PCH_SPI_VSCC1
  };

  ///
  /// Save SPI Registers for S3 resume usage
  ///
  for (Index = 0; Index < sizeof (SpiRegister) / sizeof (UINT32); Index++) {
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (mSpiInstance->PchRootComplexBar + SpiRegister[Index]),
      1,
      (VOID *) (UINTN) (mSpiInstance->PchRootComplexBar + SpiRegister[Index])
      );
  }
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
  UINT8     Data8;
  UINT32    Data32;

  ///
  /// Set BIOSWE bit (B0:D31:F0 Offset DCh [0]) = 1b
  ///
  PciOr8 (
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    0,
    R_PCH_LPC_BIOS_CNTL),
    (UINT8) (B_PCH_LPC_BIOS_CNTL_BIOSWE)
    );
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 3.7 BIOS Region SMM Protection Enabling
  /// If the following steps are implemented:
  ///  - Set the SMM_BWP bit (B0:D31:F0 Offset DCh [5]) = 1b
  ///  - Follow the 1st recommendation in section 3.6
  /// the BIOS Region can only be updated by following the steps bellow:
  ///  - Once all threads enter SMM
  ///  - Read memory location FED30880h OR with 00000001h, place the result in EAX,
  ///    and write data to lower 32 bits of MSR 1FEh (sample code available)
  ///  - Set BIOSWE bit (B0:D31:F0 Offset DCh [0]) = 1b
  ///  - Modify BIOS Region
  ///  - Clear BIOSWE bit (B0:D31:F0 Offset DCh [0]) = 0b
  ///  - Read memory location FED30880h AND with FFFFFFFEh, place the result in EAX,
  ///    and write data to lower 32 bits of MSR 1FEh (sample code available)
  ///
  Data8 = PciRead8 (
            PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            0,
            R_PCH_LPC_BIOS_CNTL)
            );
  ///
  /// Check if SMM_BWP bit is set
  ///
  if ((Data8 & B_PCH_LPC_BIOS_CNTL_SMM_BWP) == B_PCH_LPC_BIOS_CNTL_SMM_BWP) {
    ///
    /// Read memory location FED30880h OR with 00000001h, place the result in EAX,
    /// and write data to lower 32 bits of MSR 1FEh (sample code available)
    ///
    Data32 = MmioRead32 ((UINTN) (0xFED30880)) | (UINT32) (BIT0);
    AsmWriteMsr32 (0x1FE, Data32);
  }
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
  UINT8     Data8;
  UINT32    Data32;

  ///
  /// Clear BIOSWE bit (B0:D31:F0 Offset DCh [0]) = 0b
  ///
  PciAnd8 (
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    0,
    R_PCH_LPC_BIOS_CNTL),
    (UINT8) (~B_PCH_LPC_BIOS_CNTL_BIOSWE)
    );

  Data8 = PciRead8 (
            PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            0,
            R_PCH_LPC_BIOS_CNTL)
            );
  ///
  /// Check if SMM_BWP bit is set
  ///
  if ((Data8 & B_PCH_LPC_BIOS_CNTL_SMM_BWP) == B_PCH_LPC_BIOS_CNTL_SMM_BWP) {
    ///
    /// Read memory location FED30880h AND with FFFFFFFEh, place the result in EAX,
    /// and write data to lower 32 bits of MSR 1FEh (sample code available)
    ///
    Data32 = MmioRead32 ((UINTN) (0xFED30880)) & (UINT32) (~BIT0);
    AsmWriteMsr32 (0x1FE, Data32);
  }
}
