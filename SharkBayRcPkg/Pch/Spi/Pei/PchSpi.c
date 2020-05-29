/** @file
  PCH SPI PEIM implements the SPI Host Controller Compatibility Interface.

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
  PCH SPI PEIM implements the SPI Host Controller Compatibility Interface.

**/
#include <PchSpi.h>

/**
  Installs PCH SPI PPI

  @param[in] FfsHeader            Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
InstallPchSpi (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  )
{
  EFI_STATUS        Status;
  PEI_SPI_INSTANCE  *PeiSpiInstance;
  SPI_INSTANCE      *SpiInstance;

  DEBUG ((EFI_D_INFO, "InstallPchSpi() Start\n"));

  PeiSpiInstance = (PEI_SPI_INSTANCE *) AllocateZeroPool (sizeof (PEI_SPI_INSTANCE));
  if (NULL == PeiSpiInstance) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiInstance = &(PeiSpiInstance->SpiInstance);
  SpiProtocolConstructor (SpiInstance);

  PeiSpiInstance->PpiDescriptor.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiSpiInstance->PpiDescriptor.Guid  = &gPeiSpiPpiGuid;
  PeiSpiInstance->PpiDescriptor.Ppi   = &(SpiInstance->SpiProtocol);

  ///
  /// Install the SPI PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, &PeiSpiInstance->PpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "SPI PPI Installed\n"));

  DEBUG ((EFI_D_INFO, "InstallPchSpi() End\n"));

  return Status;
}

/**
  This function is a a hook for Spi Pei phase specific initialization

  @param[in] None

  @retval None
**/
VOID
EFIAPI
SpiPhaseInit (
  VOID
  )
{
  return;
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
    0,
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
    0,
    R_PCH_LPC_BIOS_CNTL),
    (UINT8) (~B_PCH_LPC_BIOS_CNTL_BIOSWE)
    );
}
