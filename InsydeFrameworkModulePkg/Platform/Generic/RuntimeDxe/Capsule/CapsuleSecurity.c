//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//;   CapsuleSecurity.c
//; 
//; Abstract:
//; 
//;    Routines for handling capsule update security
//;

#include "CapsuleService.h"
#include EFI_GUID_DEFINITION (ImageAuthentication)
#define SECURE_FLASH_SETUP_MODE_NAME  L"SecureFlashSetupMode"
typedef struct {
  EFI_SIGNATURE_LIST        SignatureListHeader;
  EFI_SIGNATURE_DATA        SignatureData;
} CERTIFICATE_DATA;

EFI_GUID gSignatureOwnerGuid           = { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }};
EFI_GUID mCertificateFileGuid     = { 0x9F4F421C, 0xCE02, 0x42c1, 0x92, 0xFB, 0xCF, 0x26, 0xC5, 0x2B, 0x95, 0x26 };
EFI_GUID mSecureFlashInfoGuid     = { 0x382AF2BB, 0xFFFF, 0xABCD, 0xAA, 0xEE, 0xCC, 0xE0, 0x99, 0x33, 0x88, 0x77 };


EFI_STATUS
GetCertificateData (
  IN EFI_GUID                   *NameGuid,
  IN OUT VOID                   **Buffer,
  IN OUT UINTN                  *Size
  )
/*++

Routine Description:
  Get the certificate from FV

Arguments:
  NameGuid                      The file guid of the certificate
  Buffer                        returned the address of the certificate
  Size                          the size of the certificate

Returns:
  EFI_SUCCESS                   found a certificate
  EFI_NOT_FOUND                 did not find one
  EFI_LOAD_ERROR                there is no FV protocol

--*/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
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
                    &gEfiFirmwareVolumeProtocolGuid,
                    &Fv
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


EFI_STATUS
LoadCertToVariable (
 VOID
  )
/*++

Routine Description:
  Load the certificate data to "SecureFlashCertData" variable
  The certificate is used when the Capsule image is loaded via gBS->LoadImage()

Arguments:
  None

Returns:
  EFI_SUCCESS           - Certificate variable was successfully set
  EFI_NOT_FOUND         - Certificate data was not found
  EFI_OUT_OF_RESOURCES  - Out of memory

--*/
{
  EFI_STATUS                  Status;
  UINT8                       *FileBuffer;
  UINTN                       FileSize;
  CERTIFICATE_DATA            *CertData;

  Status = GetCertificateData (&mCertificateFileGuid, &FileBuffer, &FileSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  Status = gBS->AllocatePool(
                  EfiBootServicesData,
                  sizeof (CERTIFICATE_DATA) + FileSize,
                  (VOID **)&CertData
                  );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return EFI_OUT_OF_RESOURCES;
  }

  CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
  CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
  CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                    sizeof (EFI_SIGNATURE_LIST);
  CertData->SignatureData.SignatureOwner = gSignatureOwnerGuid;
  EfiCopyMem (CertData->SignatureData.SignatureData, FileBuffer, FileSize);

  Status = EfiSetVariable (
                  L"SecureFlashCertData",
                  &mSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  CertData->SignatureListHeader.SignatureListSize,
                  CertData
                  );
  gBS->FreePool (CertData);

  return Status;
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
/*++

Routine Description:
  Enable security check of Capsule images

Arguments:
  None

Returns:
  EFI_SUCCESS           - Security check of Capsule images is enabled
  Others                - Failed to install Capsule security check

--*/
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
  Status = EfiSetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &mSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (SetupMode),
                    &SetupMode
                    );
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS
DisableCapsuleSecurityCheck (
  VOID
  )
/*++

Routine Description:
  Disable security check of Capsule images

Arguments:
  None

Returns:
  EFI_SUCCESS           - Security check of Capsule images is disabled
  Others                - Failed to disable Capsule security check

--*/
{
   EFI_STATUS                  Status;

  //
  // Clear ceritificate data variable
  //
  Status = EfiSetVariable (
                  L"SecureFlashCertData",
                  &mSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  0,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  //
  // Clear SecureFlashSetupMode variable
  //
  Status = EfiSetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &mSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );
  ASSERT_EFI_ERROR(Status);

  return Status;
}
