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
#include <IndustryStandard/Tpm20.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Tpm2Interface.h>
#include <Protocol/SmmBase2.h>
#include <Guid/TpmInstance.h>

TPM2_INTERFACE_PROTOCOL  *mTpm2Interface = NULL;

/**
  Return Tpm2 interface based on current phase.

  @retval TPM2_INTERFACE_PROTOCOL  A pointer to Tpm2 interface protocol
**/
TPM2_INTERFACE_PROTOCOL*
RetrieveTpm2Interface (
  VOID
  )
{
  EFI_STATUS                              Status;
  BOOLEAN                                 InSmm;
  EFI_SMM_BASE2_PROTOCOL                  *SmmBase;
  EFI_SMM_SYSTEM_TABLE2                   *Smst;

  InSmm = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (!InSmm) {
    //
    // In boot service
    //
    Status = gBS->LocateProtocol (
                    &gTpm2InterfaceGuid,
                    NULL,
                    (VOID **) &mTpm2Interface
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Tpm2Interface Error!\n"));
      mTpm2Interface = NULL;
    }
  } else {
    //
    // In Smm
    //
    SmmBase->GetSmstLocation (SmmBase, &Smst);

    Status = Smst->SmmLocateProtocol (
                      &gTpm2InterfaceGuid,
                      NULL,
                      (VOID **) &mTpm2Interface
                      );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Tpm2Interface Error!\n"));
      mTpm2Interface = NULL;
    }
  }

  return mTpm2Interface;
}

/**
  This service enables the sending of commands to the TPM2.

  @param[in]      InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small. 
  @retval EFI_INVALID_PARAMETER  TPM2_INTERFACE_PROTOCOL not found.   
**/
EFI_STATUS
EFIAPI
ProtocolBasedTpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  TPM2_INTERFACE_PROTOCOL                 *Tpm2Interface;

  Tpm2Interface = RetrieveTpm2Interface ();
  if (Tpm2Interface == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return Tpm2Interface->SummitCommand (
                          Tpm2Interface,
                          InputParameterBlock,
                          InputParameterBlockSize,
                          OutputParameterBlock,
                          OutputParameterBlockSize
                          );

}

/**
  This service requests use TPM2.

  @retval EFI_SUCCESS            Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND          TPM2 not found.
  @retval EFI_DEVICE_ERROR       Unexpected device behavior.
  @retval EFI_INVALID_PARAMETER  TPM2_INTERFACE_PROTOCOL not found.   
**/
EFI_STATUS
EFIAPI
ProtocolBasedTpm2RequestUseTpm (
  VOID
  )
{
  TPM2_INTERFACE_PROTOCOL                 *Tpm2Interface;

  Tpm2Interface = RetrieveTpm2Interface ();
  if (Tpm2Interface == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return Tpm2Interface->RequestUse (Tpm2Interface);
}
