/** @file
  Secure Flash DXE driver.

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

#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Framework/Hob.h>
#include <Protocol/LoadFile.h>
#include <Protocol/FirmwareVolume.h>
#include <Guid/ImageAuthentication.h>
#include <Protocol/ConsoleControl.h>
#include <SecureFlash.h>
#include <Guid/UsbEnumeration.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Protocol/LoadedImage.h>
#include <Ppi/FirmwareAuthentication.h>
#include <Library/MemoryAllocationLib.h>
#include "TianoDecompress.h"

#define SECURE_FLASH_SIGNATURE        SIGNATURE_32 ('$', 'S', 'E', 'C')

EFI_GUID mSecureFlashErrorMsgGuid = { 0x2719F233, 0xDD1A, 0xABCD, { 0xA9, 0xDC, 0xAA, 0xE0, 0x7B, 0x67, 0x88, 0xF2 }};
EFI_GUID mSignatureOwnerGuid      = { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }};

typedef struct {
  EFI_SIGNATURE_LIST            SignatureListHeader;
  EFI_SIGNATURE_DATA            SignatureData;
} CERTIFICATE_DATA;

typedef struct {
  MEMMAP_DEVICE_PATH            MemDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      EndDevicePath;
} IMAGE_DEVICE_PATH;

EFI_EVENT                       mSecureFlashEvent;
EFI_HANDLE                      mImageHandle;
EFI_HANDLE                      mMemMapLoadImageHandle;
EFI_STATUS                      mErrorStatus;
EFI_RESET_SYSTEM                mOriginalResetSystemPtr;

UINT8
EFIAPI
SecureFlashReadyToBootSmi (
  IN UINT32	                Command,
  IN UINT16                     SmiPort
  );

VOID
EFIAPI
SecureFlashReadyToBootEvent (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  SecureFlashReadyToBootSmi (SECURE_FLASH_SIGNATURE, PcdGet16 (PcdSoftwareSmiPort));
  gBS->CloseEvent (mSecureFlashEvent);
}

/**
  Use Console Control to turn on GOP/UGA based Simple Text Out consoles. The GOP/UGA
  Simple Text Out screens will now be synced up with all non GOP/UGA output devices

  @param None

  @retval EFI_SUCCESS            GOP/UGA devices are back in text mode and synced up.
  @retval EFI_UNSUPPORTED        Logo not found

**/
EFI_STATUS
DisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID**)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  return ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
}

/**
 Find the next Capsule volume HOB

 @param [in, out] HobStart      start of HOBs
 @param [out]  BaseAddress      returned base address of capsule volume
 @param [out]  Length           length of capsule volume pointed to by BaseAddress

 @retval EFI_SUCCESS            one found
 @retval EFI_NOT_FOUND          did not find one

**/
EFI_STATUS
GetNextCapsuleVolumeHob (
  IN OUT VOID                   **HobStart,
  OUT EFI_PHYSICAL_ADDRESS      *BaseAddress,
  OUT UINT64                    *Length
  )
{
  EFI_PEI_HOB_POINTERS          Hob;

  Hob.Raw = *HobStart;
  if (END_OF_HOB_LIST (Hob)) {
    return EFI_NOT_FOUND;
  }

  Hob.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, *HobStart);
  if (Hob.Header->HobType != EFI_HOB_TYPE_UEFI_CAPSULE) {
    return EFI_NOT_FOUND;
  }

  *BaseAddress = Hob.Capsule->BaseAddress;
  *Length      = Hob.Capsule->Length;
  *HobStart    = GET_NEXT_HOB (Hob);

  return EFI_SUCCESS;
}

/**
 Get the certificate from FV

 @param [in]   NameGuid         The file guid of the certificate
 @param [in, out] Buffer        returned the address of the certificate
 @param [in, out] Size          the size of the certificate

 @retval EFI_SUCCESS            found a certificate
 @retval EFI_NOT_FOUND          did not find one
 @retval EFI_LOAD_ERROR         there is no FV protocol

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

EFI_STATUS
LoadCertToVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         *FileBuffer;
  UINTN                         FileSize;
  CERTIFICATE_DATA              *CertData;
  UINTN                         DataSize;

  FileBuffer = NULL;
  Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&FileBuffer, &FileSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  CertData = NULL;
  CertData = AllocatePool (sizeof (CERTIFICATE_DATA) + FileSize);
  if (CertData == NULL) {
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Make sure there is no SecureFlashCertData variable.
  // if the variable SecureFlashCertData exists, delete it before setting.
  //
  DataSize = 0;
  Status = gRT->GetVariable (
                  L"SecureFlashCertData",
                  &gSecureFlashInfoGuid,
                  NULL,
                  &DataSize,
                  CertData
                  );
  if (Status != EFI_NOT_FOUND) {
    Status = gRT->SetVariable (
                    L"SecureFlashCertData",
                    &gSecureFlashInfoGuid,
                    0,
                    0,
                    NULL
                    );
  }

  CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
  CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
  CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                    sizeof (EFI_SIGNATURE_LIST);
  CertData->SignatureData.SignatureOwner = mSignatureOwnerGuid;
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
      *BufferSize = (UINTN)(MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
      return EFI_BUFFER_TOO_SMALL;
    }

    *BufferSize = (UINTN)(MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
    CopyMem (Buffer, (VOID *)(UINTN)MemMapFilePath->StartingAddress, *BufferSize);
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
SecureFlashResetSystem (
  IN EFI_RESET_TYPE             ResetType,
  IN EFI_STATUS                 ResetStatus,
  IN UINTN                      DataSize,
  IN VOID                       *ResetData OPTIONAL
  )
{
  return;
}

VOID
SecureFlashHookResetSystem (
  IN BOOLEAN                    ToHook
  )
{

  if (ToHook) {
    mOriginalResetSystemPtr = gRT->ResetSystem;
    gRT->ResetSystem = SecureFlashResetSystem;
  } else {
    gRT->ResetSystem = mOriginalResetSystemPtr;
  }
}

VOID
SecureFlashEndingProcess (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  UINT8                         *Instance;
  
  //
  // Install USB enumeration protocol to make sure USB keyboard is initialized.
  //
  gBS->RestoreTPL (TPL_APPLICATION);
  Status = gBS->LocateProtocol (&gEfiUsbEnumerationGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    Handle = 0;
    gBS->InstallProtocolInterface (
           &Handle,
           &gEfiUsbEnumerationGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
  gBS->RaiseTPL (TPL_NOTIFY);
      
  gRT->SetVariable (
         SECURE_FLASH_SETUP_MODE_NAME,
         &gSecureFlashInfoGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         0,
         NULL
         );
  gRT->SetVariable (
         SECURE_FLASH_INFORMATION_NAME,
         &gSecureFlashInfoGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         NULL
         );
  
  //
  // To trigger error message driver if it is error-loaded or it updates failed.
  //
  gBS->InstallProtocolInterface (&mImageHandle, &mSecureFlashErrorMsgGuid, EFI_NATIVE_INTERFACE, &mErrorStatus);

}

VOID
EFIAPI
LoadIsFlashImageCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Cotext
  )
{
  EFI_STATUS                    Status;
  UINTN                         Size;
  EFI_HANDLE                    IsFlashImageHandle;
  IMAGE_DEVICE_PATH             ImageDP;
  EFI_LOAD_FILE_PROTOCOL        MemMapLoadFile;
  IMAGE_INFO                    ImageInfo;
  UINT8                         SetupMode;

  COMPRESS_DATA_HEADER          *CompressDataHeader;
  COMPRESS_TABLE_HEADER         *CompressTableHeader;
  UINT8                         *ImagegBuffer;
  UINT8                         *CompressDataPtr;
  UINTN                         Index;
  SCRATCH_DATA                  *Scratch;


  ImagegBuffer = NULL;
  Scratch = NULL;
  
  gBS->CloseEvent (Event);
  mErrorStatus = EFI_SUCCESS;
  
  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );

  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    if (ImageInfo.Compressed) {
      Size = sizeof (SCRATCH_DATA);
      Scratch = AllocatePool (Size);
      if (Scratch == NULL) {
        mErrorStatus = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      //
      // Decompress the image.
      //
      CompressTableHeader = (COMPRESS_TABLE_HEADER *)(UINTN)ImageInfo.ImageAddress;
      ImageInfo.ImageSize = CompressTableHeader->TotalImageSize;
      ImagegBuffer = AllocatePool (ImageInfo.ImageSize);
      if (ImagegBuffer == NULL) {
        mErrorStatus = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      CompressDataPtr = (UINT8 *)(UINTN)(ImageInfo.ImageAddress + sizeof (COMPRESS_TABLE_HEADER));
      ImageInfo.ImageAddress = (UINTN)ImagegBuffer;
  

      for (Index = 0; Index < CompressTableHeader->NumOfBlock; Index++) {
        CompressDataHeader = (COMPRESS_DATA_HEADER *)(UINTN)CompressDataPtr;
        CompressDataPtr += sizeof (COMPRESS_DATA_HEADER);
        Status = Decompress (CompressDataPtr, ImagegBuffer, Scratch);
        if (EFI_ERROR (Status)) {
          mErrorStatus = EFI_INVALID_PARAMETER;
          goto Done;
        }
        //
        // Go to next compressed data
        //
        CompressDataPtr += CompressDataHeader->CompressedSize;
        ImagegBuffer += Scratch->mOutBuf;
      }
    }

    //
    // Set the infomation needed so it will verify the image.
    //
    ImageDP.MemDevicePath.Header.Type = HARDWARE_DEVICE_PATH;
    ImageDP.MemDevicePath.Header.SubType = HW_MEMMAP_DP;
    ImageDP.MemDevicePath.Header.Length[0] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH));
    ImageDP.MemDevicePath.Header.Length[1] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8);
    ImageDP.MemDevicePath.MemoryType = EfiReservedMemoryType;
    ImageDP.MemDevicePath.StartingAddress = ImageInfo.ImageAddress;
    ImageDP.MemDevicePath.EndingAddress = ImageInfo.ImageAddress + ImageInfo.ImageSize - 1;

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

    //
    // Clear this variable so it won't hang if it fails or user doesn't flash the NV region.
    //
    Status = gRT->SetVariable (
                    SECURE_FLASH_INFORMATION_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );

    //
    // Set this variable to trigger the verification process.
    //
    SetupMode = USER_MODE;
    Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (SetupMode),
                    &SetupMode
                    );

    DisableQuietBoot ();
    LoadCertToVariable ();

    //
    // In order to notify USB keyboard initial, current TPL must lower than TPL_CALLBACK.
    // Lower the TPL to let other ReadyToBootEvents will be triggered earlier.
    // So, those events won't affect flash process.
    //
    gBS->RestoreTPL (TPL_APPLICATION);
    Status = gBS->LoadImage (
                    TRUE,
                    mImageHandle,
                    &ImageDP.MemDevicePath.Header,
                    NULL,
                    ImageInfo.ImageSize,
                    &IsFlashImageHandle
                    );

    //
    // Clear this variable to disable the verification process.
    //
    gRT->SetVariable (
           SECURE_FLASH_SETUP_MODE_NAME,
           &gSecureFlashInfoGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           0,
           NULL
           );

    if (EFI_ERROR (Status)) {
      mErrorStatus = EFI_ACCESS_DENIED;
    } else {
      //
      // Hook resetsystem function so it won't reboot when user press CTRL+ALT+DEL.
      //
      SecureFlashHookResetSystem (TRUE);
      Status = gBS->StartImage (IsFlashImageHandle, &Size, NULL);
      if (EFI_ERROR (Status)) {
        mErrorStatus = EFI_ABORTED;
      }
      SecureFlashHookResetSystem (FALSE);
    }
    gBS->RaiseTPL (TPL_NOTIFY);
  }


Done:
        
  if (Scratch != NULL){
    Status = gBS->FreePool (Scratch);
  }
  if (ImagegBuffer != NULL){
    Status = gBS->FreePool (ImagegBuffer);
  }
  
  SecureFlashEndingProcess ();
  
}

/**
 This driver receives reserved memory information form Hob that is created by
 PEIM SecureFlashPei and set to the variable "SecureFlashInfo".
 The driver will install a ReadyToBoot event depending on whether system
 will update firmware this time or not.

 @param [in]   ImageHandle      The image handle.
 @param [in]   SystemTable      The system table.

 @retval EFI_SUCCESS            The driver is loaded.

**/
EFI_STATUS
EFIAPI
SecureFlashDxeEntry (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     ReadyToBootEvent;
  EFI_HOB_HANDOFF_INFO_TABLE    *HobList;
  EFI_PHYSICAL_ADDRESS          BaseAddress;
  UINT64                        BaseSize;
  IMAGE_INFO                    ImageInfo;
  UINTN                         Size;
  EFI_PHYSICAL_ADDRESS          BaseAddressCopy;

  BaseAddressCopy = 0;
  mImageHandle = ImageHandle;

  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );

  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    Status = EfiCreateEventReadyToBootEx (
               TPL_NOTIFY,
               LoadIsFlashImageCallback,
               NULL,
               &ReadyToBootEvent
               );
  } else {
    ImageInfo.FlashMode = FALSE;
    //
    // Clear this variable to disable the verification process.
    //
    Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );
    //
    // This event is used for modifying the module variable mInPOST in SMM.
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               SecureFlashReadyToBootEvent,
               NULL,
               &mSecureFlashEvent
               );
  }

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  Status = GetNextCapsuleVolumeHob ((VOID **)&HobList, &BaseAddress, &BaseSize);
  //
  // Due to PEI core not allowed to allocate EfiReservedMemoryType memory, we allocate
  // EfiACPIMemoryNVS under PEI and then change the memory type to EfiReservedMemoryType
  // in order to preventing legacy E820 table from produce ASSERT error
  //
  if (ImageInfo.FlashMode) {
    //
    // Allocate intermediate memory to backup flash image in case the content of 
    // preserved memory destroyed by gBS->FreePages when DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED set
    //
    Status = gBS->AllocatePages (
                    AllocateAnyPages,
                    EfiBootServicesData,
                    (UINTN)((BaseSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE),
                    &BaseAddressCopy
                    );
    if (!EFI_ERROR (Status)) {
      CopyMem ((VOID*)(UINTN)BaseAddressCopy, (VOID*)(UINTN)BaseAddress, (UINTN)BaseSize);
    }
  }
  Size = PcdGet32 (PcdReservedMemorySizeForSecureFlash);
  Status = gBS->FreePages (
                  BaseAddress,
                  (UINTN)(Size / EFI_PAGE_SIZE)
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->AllocatePages (
                    AllocateAddress,
                    EfiReservedMemoryType,
                    (UINTN)(Size / EFI_PAGE_SIZE),
                    &BaseAddress
                    );
  }
  if (ImageInfo.FlashMode && BaseAddressCopy) {
    CopyMem ((VOID*)(UINTN)BaseAddress, (VOID*)(UINTN)BaseAddressCopy, (UINTN)BaseSize);
    gBS->FreePages (
           BaseAddressCopy, 
           (UINTN)((BaseSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE)
           );
  }
  ImageInfo.ImageAddress = BaseAddress;
  ImageInfo.ImageSize = (UINT32)BaseSize;

  Status = gRT->SetVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (IMAGE_INFO),
                  &ImageInfo
                  );

  return EFI_SUCCESS;
}

