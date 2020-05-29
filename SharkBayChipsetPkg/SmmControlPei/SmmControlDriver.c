/** @file
Implementation of SmmControlPei module
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

/*++
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmControlDriver.c
  
Abstract: 

  This is the driver that publishes the SMM Control Ppi.

--*/
#include "SmmControlDriver.h"

EFI_GUID  mPeiSmmControlPpiGuid = PEI_SMM_CONTROL_PPI_GUID;

STATIC PEI_SMM_CONTROL_PPI      mSmmControlPpi = {
  PeiActivate,
  PeiDeactivate
};

STATIC EFI_PEI_PPI_DESCRIPTOR   mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &mPeiSmmControlPpiGuid,
  &mSmmControlPpi
};


/**
 This is the constructor for the SMM Control ppi

 @param [in]   FfsHeader        FfsHeader.
 @param [in]   PeiServices      General purpose services available to every PEIM.

 @return Results of the installation of the SMM Control Ppi

**/
EFI_STATUS
EFIAPI
SmmControlPeiDriverEntryInit (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status      = (**PeiServices).InstallPpi (PeiServices, &mPpiList);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
 Trigger the software SMI

 @param [in]   Data             The value to be set on the software SMI data port

 @retval EFI_SUCCESS            Function completes successfully

**/
EFI_STATUS
EFIAPI
SmmTrigger (
  IN UINT8   Data
  )
{
  UINT32  OutputData;
  UINT32  OutputPort;
  UINT32  PmBase;

//  PmBase = MmioRead32 (
//              PchPciDeviceMmBase (DEFAULT_PCI_BUS_NUMBER_PCH,
//              PCI_DEVICE_NUMBER_PCH_LPC,
//              PCI_FUNCTION_NUMBER_PCH_LPC) + R_PCH_LPC_ACPI_BASE
//              ) & B_PCH_LPC_ACPI_BASE_BAR;

//[-start-120508-IB10820048-modify]//
  PmBase = MmioRead32 (
              MmPciAddress (0,DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_ACPI_BASE)
              ) & B_PCH_LPC_ACPI_BASE_BAR;
//[-end-120508-IB10820048-modify]//  
  
  
  //
  // Enable the APMC SMI
  //
  OutputPort  = PmBase + R_PCH_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= (B_PCH_SMI_EN_APMC | B_PCH_SMI_EN_GBL_SMI);
  DEBUG (
    (EFI_D_INFO,
    "The SMI Control Port at address %x will be written to %x.\n",
    OutputPort,
    OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  OutputPort  = R_PCH_APM_CNT;
  OutputData  = Data;

  //
  // Generate the APMC SMI
  //
  IoWrite8 (
    (UINTN) OutputPort,
    (UINT8) (OutputData)
    );

  return EFI_SUCCESS;
}

/**
 Clear the SMI status

 @param None

 @retval EFI_SUCCESS            The function completes successfully
 @retval EFI_DEVICE_ERROR       Something error occurred

**/
EFI_STATUS
EFIAPI
SmmClear (
  VOID
  )
{
  UINT32      OutputData;
  UINT32      OutputPort;
  UINT32      PmBase;

//  PmBase = MmioRead32 (
//              PchPciDeviceMmBase (DEFAULT_PCI_BUS_NUMBER_PCH,
//              PCI_DEVICE_NUMBER_PCH_LPC,
//              PCI_FUNCTION_NUMBER_PCH_LPC) + R_PCH_LPC_ACPI_BASE
//              ) & B_PCH_LPC_ACPI_BASE_BAR;

//[-start-120508-IB10820048-modify]//
  PmBase = MmioRead32 (
              MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_ACPI_BASE)
              ) & B_PCH_LPC_ACPI_BASE_BAR;
//[-end-120508-IB10820048-modify]//  
  
  //
  // Clear the Power Button Override Status Bit, it gates EOS from being set.
  //
  OutputPort  = PmBase + R_PCH_ACPI_PM1_STS;
  OutputData  = B_PCH_ACPI_PM1_STS_PRBTNOR;
  DEBUG (
    (EFI_D_INFO,
    "The PM1 Status Port at address %x will be written to %x.\n",
    OutputPort,
    OutputData)
    );
  IoWrite16 (
    (UINTN) OutputPort,
    (UINT16) (OutputData)
    );

  //
  // Clear the APM SMI Status Bit
  //
  OutputPort  = PmBase + R_PCH_SMI_STS;
  OutputData  = B_PCH_SMI_STS_APM;
  DEBUG (
    (EFI_D_INFO,
    "The SMI Status Port at address %x will be written to %x.\n",
    OutputPort,
    OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  //
  // Set the EOS Bit
  //
  OutputPort  = PmBase + R_PCH_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= B_PCH_SMI_EN_EOS;
  DEBUG (
    (EFI_D_INFO,
    "The SMI Control Port at address %x will be written to %x.\n",
    OutputPort,
    OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  //
  // If the EOS bit did not get set, then we've got a problem.
  //
  DEBUG_CODE (
    OutputData = IoRead32 ((UINTN) OutputPort);
    if ((OutputData & B_PCH_SMI_EN_EOS) != B_PCH_SMI_EN_EOS) {
    DEBUG ((EFI_D_ERROR, "Bugger, EOS did not get set!\n"));
    return EFI_DEVICE_ERROR;
  }
  );

  return EFI_SUCCESS;
}

/**
 This routine generates an SMI

 @param [in]   PeiServices
 @param [in]   This             The EFI SMM Control ppi instance
 @param [in, out] ArgumentBuffer  The buffer of argument
 @param [in, out] ArgumentBufferSize  The size of the argument buffer
 @param [in]   Periodic         Periodic or not
 @param [in]   ActivationInterval  Interval of periodic SMI

 @return EFI Status describing the result of the operation
 @retval EFI_INVALID_PARAMETER  Some parameter value passed is not supported

**/
EFI_STATUS
EFIAPI
PeiActivate (
  IN       EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_SMM_CONTROL_PPI       *This,
  IN OUT  INT8                       *ArgumentBuffer OPTIONAL,
  IN OUT  UINTN                      *ArgumentBufferSize OPTIONAL,
  IN      BOOLEAN                    Periodic OPTIONAL,
  IN      UINTN                      ActivationInterval OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       Data;

  if (Periodic) {
    DEBUG ((EFI_D_WARN, "Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (ArgumentBuffer == NULL) {
    Data = 0xFF;
  } else {
    if (ArgumentBufferSize == NULL || *ArgumentBufferSize != 1) {
      return EFI_INVALID_PARAMETER;
    }

    Data = *ArgumentBuffer;
  }
  //
  // Clear any pending the APM SMI
  //
  Status = SmmClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SmmTrigger (Data);
}

/**
 This routine clears an SMI

 @param [in]   PeiServices
 @param [in]   This             The EFI SMM Control ppi instance
 @param [in]   Periodic         Periodic or not

 @return EFI Status describing the result of the operation
 @retval EFI_INVALID_PARAMETER  Some parameter value passed is not supported

**/
EFI_STATUS
EFIAPI
PeiDeactivate (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN PEI_SMM_CONTROL_PPI          *This,
  IN  BOOLEAN                     Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return SmmClear ();
}

