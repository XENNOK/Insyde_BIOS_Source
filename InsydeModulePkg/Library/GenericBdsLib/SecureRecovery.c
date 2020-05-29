/** @file
  Routines for handling capsule update security

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
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
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadFile.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/EfiSystemResourceTable.h>
#include <SecureFlash.h>

typedef struct {
  EFI_SIGNATURE_LIST        SignatureListHeader;
  EFI_SIGNATURE_DATA        SignatureData;
} CERTIFICATE_DATA;

typedef struct {
  MEMMAP_DEVICE_PATH            MemDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      EndDevicePath;
} IMAGE_DEVICE_PATH;

EFI_GUID gSignatureOwnerGuid    = { 0x79736E49, 0x6564, 0xBBAA, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 };

IMAGE_DEVICE_PATH             ImageDP;
EFI_LOAD_FILE_PROTOCOL        MemMapLoadFile;
EFI_HANDLE                    mMemMapLoadImageHandle;

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
                    (VOID **) &Fv
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

  CertData = NULL;
  FileBuffer = NULL;

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

  Status = LoadCertToVariable ();
  if(EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
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
  ASSERT_EFI_ERROR (Status);

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
  ASSERT_EFI_ERROR (Status);

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
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
MemMapLoadFileFunction (
  IN EFI_LOAD_FILE_PROTOCOL     *This,
  IN EFI_DEVICE_PATH_PROTOCOL   *FilePath,
  IN BOOLEAN                    BootPolicy,
  IN OUT UINTN                  *BufferSize,
  IN VOID                       *Buffer OPTIONAL
  )
{
  MEMMAP_DEVICE_PATH            *MemMapFilePath;

  //
  // The FilePath is pointed to EndDevicePath, modify it to the right address(MEMMAP_DEVICE_PATH).
  //
  MemMapFilePath = (MEMMAP_DEVICE_PATH *)((UINTN)FilePath - sizeof (MEMMAP_DEVICE_PATH));

  if ((MemMapFilePath->Header.Type == HARDWARE_DEVICE_PATH) && (MemMapFilePath->Header.SubType == HW_MEMMAP_DP)) {
    if (*BufferSize < (MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1)) {
      *BufferSize = (UINTN) (MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
      return EFI_BUFFER_TOO_SMALL;
    }

    *BufferSize = (UINTN) (MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
    CopyMem (Buffer, (VOID *) (UINTN) MemMapFilePath->StartingAddress, *BufferSize);
  }

  return EFI_SUCCESS;
}




BOOLEAN
RecoveryCapsuleIsExecutable (
  IN EFI_PEI_HOB_POINTERS      *RecoveryHob
  )
{
  CHAR8    *Buffer;

  if (RecoveryHob == NULL) {
    return FALSE;
  }
  Buffer =(CHAR8 *) (UINTN)RecoveryHob->MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;

  return (Buffer[0] == 'M') && (Buffer[1] == 'Z');
}

EFI_STATUS
ExecuteRecoveryCapsule (
  IN EFI_PEI_HOB_POINTERS      *RecoveryHob
  )
{
  EFI_PHYSICAL_ADDRESS          ImageStart;
  EFI_PHYSICAL_ADDRESS          ImageEnd;
  EFI_HANDLE                    CapsuleHandle;
  EFI_STATUS                    Status;
  UINT32                        ImageSize;


  if (RecoveryHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  CapsuleHandle = NULL;
  ImageStart = RecoveryHob->MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;
  ImageSize  = (UINT32)RecoveryHob->MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;
  ImageEnd   = ImageStart + ImageSize - 1;

  ImageDP.MemDevicePath.Header.Type = HARDWARE_DEVICE_PATH;
  ImageDP.MemDevicePath.Header.SubType = HW_MEMMAP_DP;
  ImageDP.MemDevicePath.Header.Length[0] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH));
  ImageDP.MemDevicePath.Header.Length[1] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8);
  ImageDP.MemDevicePath.MemoryType = EfiBootServicesCode;
  ImageDP.MemDevicePath.StartingAddress = ImageStart;
  ImageDP.MemDevicePath.EndingAddress = ImageEnd;

  ImageDP.EndDevicePath.Type = END_DEVICE_PATH_TYPE;
  ImageDP.EndDevicePath.SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
  ImageDP.EndDevicePath.Length[0] = END_DEVICE_PATH_LENGTH;
  ImageDP.EndDevicePath.Length[1] = 0;

  MemMapLoadFile.LoadFile = MemMapLoadFileFunction;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mMemMapLoadImageHandle,
                  &gEfiDevicePathProtocolGuid,
                  &ImageDP.MemDevicePath,
                  &gEfiLoadFileProtocolGuid,
                  &MemMapLoadFile,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EnableCapsuleSecurityCheck ();
  if (!EFI_ERROR(Status)) {
    Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    &ImageDP.MemDevicePath.Header,
                    NULL,
                    ImageSize,
                    &CapsuleHandle
                    );
  }

  if (!EFI_ERROR(Status)) {
    Status = gBS->StartImage (CapsuleHandle, NULL, NULL);
  }

  DisableCapsuleSecurityCheck ();

  return Status;

}

