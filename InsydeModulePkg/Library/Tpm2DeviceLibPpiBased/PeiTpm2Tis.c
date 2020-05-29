/** @file
  This library abstract how to access TPM2 hardware device.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/Tpm2InterfacePei.h>
#include <IndustryStandard/Tpm20.h>

/**
  This service enables the sending of commands to the TPM2.

  @param[in]      InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small. 
  @retval EFI_INVALID_PARAMETER  PeiServices not found.   
**/
EFI_STATUS
EFIAPI
PpiBasedTpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_PPI_DESCRIPTOR            *Tpm2InterfaceDescriptor;
  PEI_TPM2_INTERFACE_PPI            *Tpm2Interface;
  EFI_PEI_SERVICES                  **PeiServices;

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
  if (PeiServices == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = (**PeiServices).LocatePpi (
                           (CONST EFI_PEI_SERVICES **)PeiServices,
                           &gPeiTpm2InterfacePpiGuid,
                           0,
                           &Tpm2InterfaceDescriptor,
                           (VOID **)&Tpm2Interface
                           );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2Interface Error!\n"));
    return Status;
  }

  return Tpm2Interface->SummitCommand (
                           PeiServices,
                           Tpm2Interface,
                           InputParameterBlock,
                           InputParameterBlockSize,
                           OutputParameterBlock,
                           OutputParameterBlockSize
                           );
}

/**
  This service requests use TPM2.

  @retval EFI_SUCCESS      Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND    TPM2 not found.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
PpiBasedTpm2RequestUseTpm (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_PPI_DESCRIPTOR            *Tpm2InterfaceDescriptor;
  PEI_TPM2_INTERFACE_PPI            *Tpm2Interface;
  EFI_PEI_SERVICES                  **PeiServices;

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
  if (PeiServices == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = (**PeiServices).LocatePpi (
                           (CONST EFI_PEI_SERVICES **)PeiServices,
                           &gPeiTpm2InterfacePpiGuid,
                           0,
                           &Tpm2InterfaceDescriptor,
                           (VOID **)&Tpm2Interface
                           );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2Interface Error!\n"));
    return Status;
  }

  return Tpm2Interface->RequestUse (PeiServices, Tpm2Interface);
}
