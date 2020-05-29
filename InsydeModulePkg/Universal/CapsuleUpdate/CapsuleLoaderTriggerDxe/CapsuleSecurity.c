/** @file
  Routines for handling capsule update security

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

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/EfiSystemResourceTable.h>
#include <SecureFlash.h>

typedef struct {
  EFI_SIGNATURE_LIST        SignatureListHeader;
  EFI_SIGNATURE_DATA        SignatureData;
} CERTIFICATE_DATA;

EFI_GUID gSignatureOwnerGuid           = { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }};


/**
  Get the certificate from firmware volume

  @param NameGuid               Pointer to the file GUID of the certificate
  @param Buffer                 Returned the address of the certificate
  @param Size                   Pointer to the size of the certificate

  @retval EFI_SUCCESS           The certificate was successfully retrieved
  @retval EFI_NOT_FOUND         Failed to find the certificate
  @retval EFI_LOAD_ERROR        Firmware Volume Protocol error
**/
EFI_STATUS
GetCertificateData (
  IN EFI_GUID                   *NameGuid,
  IN OUT VOID                   **Buffer,
  IN OUT UINTN                  *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );

    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }

    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadSection (
                   Fv,
                   NameGuid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   Size,
                   &AuthenticationStatus
                   );

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Load the certificate data to "SecureFlashCertData" variable
  The certificate is used when the Capsule image is loaded via gBS->LoadImage()

  @param  None

  @retval EFI_SUCCESS          Certificate variable was successfully set
  @retval EFI_NOT_FOUND        Certificate data was not found
  @retval EFI_OUT_OF_RESOURCES Out of memory
**/
EFI_STATUS
LoadCertToVariable (
 VOID
  )
{
  EFI_STATUS                  Status;
  UINT8                       *FileBuffer;
  UINTN                       FileSize;
  CERTIFICATE_DATA            *CertData;

  Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&FileBuffer, &FileSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  CertData = AllocatePool (sizeof (CERTIFICATE_DATA) + FileSize);
  if (CertData == NULL) {
    ASSERT (CertData != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
  CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
  CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                    sizeof (EFI_SIGNATURE_LIST);
  CertData->SignatureData.SignatureOwner = gSignatureOwnerGuid;
  CopyMem (CertData->SignatureData.SignatureData, FileBuffer, FileSize);

  Status = gRT->SetVariable (
                  L"SecureFlashCertData",
                  &gSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  CertData->SignatureListHeader.SignatureListSize,
                  CertData
                  );

  return EFI_SUCCESS;
}

/**
  Enable security check of Capsule images

  @param  None

  @retval EFI_SUCCESS          Security check of Capsule images is enabled
  @return others               Failed to install Capsule security check
**/
EFI_STATUS
EnableCapsuleSecurityCheck (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT8                       SetupMode;

  Status = LoadCertToVariable();
  if(EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  //
  // Set SecureFlashSetupMode variable to trigger image verification process.
  //
  SetupMode = USER_MODE;
  Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (SetupMode),
                    &SetupMode
                    );
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**

  Enable security check of Capsule images

  @param  None

  @retval EFI_SUCCESS          Security check of Capsule images is disabled
  @return others               Failed to disable Capsule security check

**/
EFI_STATUS
DisableCapsuleSecurityCheck (
  VOID
  )
{
   EFI_STATUS                  Status;

  //
  // Clear ceritificate data variable
  //
  Status = gRT->SetVariable (
                  L"SecureFlashCertData",
                  &gSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  0,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  //
  // Clear SecureFlashSetupMode variable
  //
  Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );
  ASSERT_EFI_ERROR(Status);

  return Status;
}
