/** @file
  Main routines for the Rng interpreter.
  Includes the initialization and main interpreter routines.

;******************************************************************************
;* Copyright (c) 2012-2014 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "RngDxe.h"

//
// The handle and instance of the Random Number Generator Protocol.
//
//
// The protocol instance produced by this driver.
//
EFI_RNG_PROTOCOL gRngProtocol = {
  GetInfo,
  GetRNG
};

CRYPTO_SERVICES_PROTOCOL *mCryptoServiceProtocol;



/**
  Retrieves a list of the supported RNG algorithms.

  Note: We are using the TPM protocol for the RNG functionality
  and we only return the type EFI_RNG_ALGORITHM_RAW and a size of EFI_GUID.

  @param[in]      This                  The instance of the EFI_RNG_PROTOCOL.
  @param[in,out]  RNGAlgorithmListSize  The size of the RNG algorithm list.
  @param[out]     RNGAlgorithmList      The list of the support RNG alogorithms.

  @retval EFI_SUCCESS                   RNG algorithm list returned successfully.
  @retval Others                        Failed to retrieve RNG alogorithm list.

**/
EFI_STATUS
EFIAPI
GetInfo (
  IN EFI_RNG_PROTOCOL             *This,
  IN OUT UINTN                    *RNGAlgorithmListSize,
  OUT EFI_RNG_ALGORITHM           *RNGAlgorithmList
  )
{

  if ((This == NULL) || (RNGAlgorithmListSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (*RNGAlgorithmListSize < sizeof (EFI_GUID)) {
    *RNGAlgorithmListSize = sizeof (EFI_GUID);
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // TODO: Add support to populate the RNGAlgorithmList with valid
  // entries based on the hardware and software capabilites.
  // Currently we are only using the hardware RNG so we have hardcoded
  // the return results.
  //
  *RNGAlgorithmListSize = sizeof (EFI_GUID);
  if (RNGAlgorithmList != NULL) {
    CopyMem (RNGAlgorithmList, &gEfiRngAlgorithmRaw, sizeof (EFI_GUID));
  }
  return EFI_SUCCESS;
}


/**
  Retrieves a random number of a given size using TPM protocol to access the hardware RNG.

  @param[in]   This             The instance of the RNG protocol.
  @param[in]   RNGAlgorithm     The RNG algorthm to use to generate the random number.
  @param[in]   RNGValueLength   The length of the random number to return in bytes.
  @param[out]  RNGValue         The random number returned.

  @retval EFI_SUCCESS           RNG request completed successfully.
  @retval Others                Failed to generate a random number.

**/
EFI_STATUS
EFIAPI
GetRNG (
  IN EFI_RNG_PROTOCOL            *This,
  IN EFI_RNG_ALGORITHM           *RNGAlgorithm, OPTIONAL
  IN UINTN                       RNGValueLength,
  OUT UINT8                      *RNGValue
  )
{

  if ((RNGValueLength == 0) || (RNGValue == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((RNGAlgorithm != NULL) &&
      (CompareMem(RNGAlgorithm, &gEfiRngAlgorithmRaw, sizeof(EFI_GUID)) != 0)) {
    return EFI_UNSUPPORTED;
  }

  if (!mCryptoServiceProtocol->RandomSeed (NULL, (UINT32) RNGValueLength)) {
    return EFI_DEVICE_ERROR;
  }

  if (!mCryptoServiceProtocol->RandomBytes (RNGValue, (UINT32) RNGValueLength)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  The entry point for Rng driver which installs the driver binding and component name
  protocol on its ImageHandle.

  @param[in]  ImageHandle       The image handle of the driver.
  @param[in]  SystemTable       The system table.

  @retval EFI_SUCCESS           If the driver binding and component name protocols
                                are successfully.
  @retval Others                Failed to install the protocols.

**/
EFI_STATUS
EFIAPI
RngDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )

{
  EFI_STATUS                  Status;
  Status = gBS->LocateProtocol (
                &gCryptoServicesProtocolGuid,
                NULL,
                (VOID **)&mCryptoServiceProtocol
                );
  ASSERT_EFI_ERROR (Status);
  //
  // Install the Random Number Generator Architctural Protocol onto a new handle
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiRngProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gRngProtocol
                  );
  if (Status != EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  return Status;
}


/**
  Unload function that is registered in the LoadImage protocol.
  It uninstalls Rng protocols produced by user.

  @param  ImageHandle

  @retval EFI_SUCCESS     Unload Rng protocol successfully.

**/
EFI_STATUS
EFIAPI
RngDriverUnload (
  IN EFI_HANDLE             ImageHandle
  )
{
  EFI_STATUS                  Status;
  EFI_STATUS                  LocateHandleStatus;
  UINTN                       HandleSize;

  HandleSize = sizeof (ImageHandle);
  //
  //  Locate the handle by GUID
  //
  LocateHandleStatus = gBS->LocateHandle (ByProtocol,
                                          &gEfiRngProtocolGuid,
                                          NULL,
                                          &HandleSize,
                                          &ImageHandle);

  Status = gBS->UninstallProtocolInterface (ImageHandle,
                                            &gEfiRngProtocolGuid,
                                            &gRngProtocol);

  return Status;
}
