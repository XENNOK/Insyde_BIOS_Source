//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   RecoveryFlash.c
//;
//; Abstract:
//;
//;   This file include all BDS platform recovery flash functions.
//;

#include "RecoveryFlash.h"
#ifdef SECURE_BOOT_SUPPORT
#include "VariableSupportLib.h"
#endif

#ifdef SECURE_FLASH_SUPPORT
#include "EfiCommonLib.h"
#include EFI_GUID_DEFINITION (ImageAuthentication)
#include EFI_PROTOCOL_DEFINITION (LoadFile)
#include EFI_PROTOCOL_DEFINITION (DevicePath)
#include EFI_GUID_DEFINITION (SecureFlashInfo)

#define SECURE_FLASH_SETUP_MODE_NAME  L"SecureFlashSetupMode"


typedef struct {
  EFI_SIGNATURE_LIST        SignatureListHeader;
  EFI_SIGNATURE_DATA        SignatureData;
} CERTIFICATE_DATA;

typedef struct {
  MEMMAP_DEVICE_PATH            MemDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      EndDevicePath;
} IMAGE_DEVICE_PATH;

EFI_GUID gSignatureOwnerGuid      = { 0x79736E49, 0x6564, 0xBBAA, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 };
EFI_GUID mCertificateFileGuid     = { 0x9F4F421C, 0xCE02, 0x42c1, 0x92, 0xFB, 0xCF, 0x26, 0xC5, 0x2B, 0x95, 0x26 };
extern EFI_HANDLE               mBdsImageHandle;
IMAGE_DEVICE_PATH               ImageDP;
EFI_LOAD_FILE_PROTOCOL          MemMapLoadFile;
EFI_HANDLE                      mMemMapLoadImageHandle;


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
  EFI_SUCCESS                   Certificate variable was successfully set
  EFI_NOT_FOUND                 Certificate data was not found
  EFI_OUT_OF_RESOURCES          Out of memory

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
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_OUT_OF_RESOURCES;
  }

  CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
  CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
  CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                    sizeof (EFI_SIGNATURE_LIST);
  CertData->SignatureData.SignatureOwner = gSignatureOwnerGuid;
  EfiCopyMem (CertData->SignatureData.SignatureData, FileBuffer, FileSize);

  Status = gRT->SetVariable (
                  L"SecureFlashCertData",
                  &gSecureFlashInfoGuid,
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
  EFI_SUCCESS                   Security check of Capsule images is enabled
  others                        Failed to install Capsule security check

--*/
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
  EFI_SUCCESS                   Security check of Capsule images is disabled
  others                        Failed to disable Capsule security check

--*/
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
      *BufferSize = MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1;
      return EFI_BUFFER_TOO_SMALL;
    }

    *BufferSize = MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1;
    EfiCopyMem (Buffer, (VOID *)MemMapFilePath->StartingAddress, *BufferSize);
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
  ImageSize = (UINT32)RecoveryHob->MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;
  ImageEnd = ImageStart + ImageSize - 1;

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
  if (!EFI_ERROR (Status)) {
    Status = gBS->LoadImage (
                    TRUE,
                    mBdsImageHandle,
                    &ImageDP.MemDevicePath.Header,
                    NULL,
                    ImageSize,
                    &CapsuleHandle
                    );

  }
  if (!EFI_ERROR (Status)) {
    Status = gBS->StartImage (CapsuleHandle, NULL, NULL);
  }

  DisableCapsuleSecurityCheck ();

  return Status;

}
#endif    // SECURE_FLASH_SUPPORT

EFI_RESET_SYSTEM                mOriginalResetSystemPtr;

VOID
EFIAPI
ResetSystemDoNothing (
  IN EFI_RESET_TYPE             ResetType,
  IN EFI_STATUS                 ResetStatus,
  IN UINTN                      DataSize,
  IN CHAR16                     *ResetData OPTIONAL
  )
{
  return;
}

VOID
HookResetSystem (
  IN BOOLEAN                    ToHook
  )
{

  if (ToHook) {
    mOriginalResetSystemPtr = gRT->ResetSystem;
    gRT->ResetSystem = ResetSystemDoNothing;
  } else {
    gRT->ResetSystem = mOriginalResetSystemPtr;
  }
}

EFI_INPUT_KEY mKeyList[] = {
                           SCAN_UP,  CHAR_NULL,
                           SCAN_DOWN,CHAR_NULL,
                           SCAN_ESC, CHAR_NULL,
                           SCAN_NULL,CHAR_CARRIAGE_RETURN,
                           SCAN_NULL,CHAR_NULL
                           };

#ifdef SECURE_BOOT_SUPPORT
UINT8 *mAuthVarBackupDataBuffer = NULL;

EFI_STATUS
GetVariableStoreInfo (
  IN OUT EFI_FLASH_SUBAREA_ENTRY     **NvVariableStoreEntry
  )
{
  EFI_STATUS                    Status;
  VOID                          *HobList;
  EFI_FLASH_MAP_ENTRY_DATA      *FlashMapEntryData;


  HobList = NULL;
  FlashMapEntryData = NULL;

  if (NvVariableStoreEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *NvVariableStoreEntry = NULL;
  while (TRUE) {
    Status = GetNextGuidHob (&HobList, &gEfiFlashMapHobGuid, &FlashMapEntryData, NULL);
    if (!EFI_ERROR (Status)) {
      if (FlashMapEntryData->AreaType == EFI_FLASH_AREA_EFI_VARIABLES) {
        *NvVariableStoreEntry = &(FlashMapEntryData->Entries[0]);
        break;
      }
    } else {
      //
      // No more flash hob could be search.
      //
      break;
    }
  }

  if (*NvVariableStoreEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;

}

VOID
RestoreAuthVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_FLASH_SUBAREA_ENTRY       *NvVariableEntry;
  EFI_FIRMWARE_VOLUME_HEADER    *OnboardNvStoreageFvHeader;
  VARIABLE_HEADER               *VariableHeader;
  UINTN                         SkipHeaderSize;
  UINT32                        FlashSize;
  UINT32                        FlashAddress;
  UINTN                         FvHeaderSize;


  if (mAuthVarBackupDataBuffer == NULL) {
    return ;
  }

  NvVariableEntry = NULL;
  Status = GetVariableStoreInfo (&NvVariableEntry);
  OnboardNvStoreageFvHeader = (VOID *)(UINTN)(NvVariableEntry->Base & (~EFI_PAGE_MASK));


  //
  // The current variable region should be empty after done Crisis Recovery Flash.
  // If the variable region with any data, not to restore AuthVariable data to prevent variable region from breaking.
  // But it shall not happen.
  //
  VariableHeader = NULL;
  VariableHeader = (VARIABLE_HEADER *) ((VOID*)(UINTN) (NvVariableEntry->Base + sizeof (VARIABLE_STORE_HEADER)));
  if (IsValidVariableHeader (VariableHeader)) {
    gBS->FreePool (mAuthVarBackupDataBuffer);
    return ;
  }

  //
  // Copy new BIOS firmware volume and variable store header information to mAuthVarBackupDataBuffer.
  //
  SkipHeaderSize = OnboardNvStoreageFvHeader->HeaderLength + sizeof (VARIABLE_STORE_HEADER);
  EfiCommonLibCopyMem (mAuthVarBackupDataBuffer, OnboardNvStoreageFvHeader, SkipHeaderSize);
  FvHeaderSize = (UINTN) OnboardNvStoreageFvHeader->HeaderLength;
  FlashSize = (UINT32)(NvVariableEntry->Length + FvHeaderSize);
  FlashAddress = (UINT32)(UINTN)OnboardNvStoreageFvHeader;
  FlashWrite (mAuthVarBackupDataBuffer, FlashSize, FlashAddress, FLASH_SMI_PORT);

}

VOID
BackupAuthVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *OnboardNvStoreageFvHeader;
  EFI_FLASH_SUBAREA_ENTRY       *NvVariableEntry;
  VARIABLE_HEADER               *VariableHeader;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VariableSize;
  UINTN                         LastVariableOffset;
  UINTN                         SkipHeaderSize;
  UINTN                         FvHeaderSize;


  NvVariableEntry = NULL;
  Status = GetVariableStoreInfo (&NvVariableEntry);

  OnboardNvStoreageFvHeader = (VOID *)(UINTN)(NvVariableEntry->Base & (~EFI_PAGE_MASK));
  FvHeaderSize = (UINTN) OnboardNvStoreageFvHeader->HeaderLength;
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  NvVariableEntry->Length + FvHeaderSize,
                  &mAuthVarBackupDataBuffer
                  );
  gBS->SetMem (mAuthVarBackupDataBuffer, NvVariableEntry->Length + FvHeaderSize, 0xFF);

  LastVariableOffset = 0;
  VariableSize = 0;
  NextVariable = NULL;
  VariableHeader = (VARIABLE_HEADER *) ((VOID*)(UINTN)(NvVariableEntry->Base + sizeof (VARIABLE_STORE_HEADER)));
  SkipHeaderSize = FvHeaderSize + sizeof (VARIABLE_STORE_HEADER);
  while (IsValidVariableHeader (VariableHeader)) {
    NextVariable = GetNextVariablePtr (VariableHeader);
    if (VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      if (((VariableHeader->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) ||
          ((VariableHeader->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)){
        VariableSize = (UINTN) NextVariable - (UINTN) VariableHeader;
        if (LastVariableOffset + VariableSize <= ((EFI_FLASH_SUBAREA_ENTRY*)NvVariableEntry)->Length - 1) {
          EfiCommonLibCopyMem (
            &mAuthVarBackupDataBuffer[LastVariableOffset] + SkipHeaderSize,
            VariableHeader,
            VariableSize
            );
        }
        LastVariableOffset += VariableSize;
      }
    }
    VariableHeader = NextVariable;
  }

}
#endif

UINTN
PrintAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *Fmt,
  ...
  )
/*++

Routine Description:

  Prints a formatted unicode string to the default console, at
  the supplied cursor position

Arguments:

  Column   - The column of  cursor position to print the string at
  Row      - The row of  cursor position to print the string at
  Fmt      - Format string

Returns:

  Length of string printed to the console

--*/
{
  CHAR16      *Buffer;
  UINTN       StrLen;
  VA_LIST     Marker;


  Buffer = EfiLibAllocateZeroPool (0x10000);
  ASSERT(Buffer);
  if (Column != (UINTN) -1) {
    gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  }

  VA_START (Marker, Fmt);
  StrLen = VSPrint (Buffer, 0x10000 , Fmt, Marker);
  VA_END (Marker);

  if (gST->ConOut != NULL) {
    //
    // To be extra safe make sure ConOut has been initialized
    //
    gST->ConOut->OutputString (gST->ConOut, Buffer);
  }
  gBS->FreePool (Buffer);
  return StrLen;

}

EFI_STATUS
RecoveryPopUp (
  IN  UINTN     FlashMode
  )
/*++

Routine Description:

  Display the Recovery flash user interface to user to select

Arguments:

  FlashMode - Input flash mode (DEFAULT_FLASH_DEVICE_TYPE, SPI_FLASH_DEVICE_TYPE, OTHER_FLASH_DEVICE_TYPE)

Returns:

  If success, it will not return. Otherwise, it will return error
  status.

--*/
{
  CHAR16                                *TitleString;
  CHAR16                                *FlashInfoStringArray[5];
  UINTN                                 Index;
  EFI_INPUT_KEY                         Key;
  EFI_OEM_FORM_BROWSER2_PROTOCOL        *OemFormBrowserPtr;
  EFI_STATUS                            Status;


  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowser2ProtocolGuid,
                  NULL,
                  &OemFormBrowserPtr
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Intialize local variable and string for use
  //
  Index = 0;
  TitleString = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_TITLE));
  FlashInfoStringArray[0] = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_YES));
  FlashInfoStringArray[1] = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_RESETSYSTEM));
  FlashInfoStringArray[2] = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_NO));
  FlashInfoStringArray[3] = 0x00;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK));
  while (TRUE) {
    //
    // Print recovery flash message
    //
    OemFormBrowserPtr->OptionIcon (
                         3,
                         FALSE,
                         NULL,
                         &Key,
                         60,
                         TitleString,
                         &Index,
                         (CHAR16 **) (FlashInfoStringArray),
                         0
                         );

      //
      // Process the selection after user pressed "Enter".
      //
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (Index == 0) {
        RecoveryFlash (FlashMode);
      } else if (Index == 1) {
        ResetCommand ();
      } else {
        break;
      }
    }
  }
  gST->ConOut->ClearScreen (gST->ConOut);
  //
  // Free allocated strings
  //
  gBS->FreePool (TitleString);
  gBS->FreePool (FlashInfoStringArray[0]);
  gBS->FreePool (FlashInfoStringArray[1]);
  gBS->FreePool (FlashInfoStringArray[2]);
  gBS->FreePool (FlashInfoStringArray[3]);

  return EFI_SUCCESS;
}

EFI_STATUS
ResetCommand (
  VOID
  )
/*++

Routine Description:

  Send reset request to reset system

Arguments:

Returns:


--*/
{
  FlashComplete (IHISI_REBOOT, FLASH_SMI_PORT);

  return EFI_SUCCESS;
}


VOID
Drawpercentage (
  IN   UINTN        PercentageValue
  )
/*++

Routine Description:

  Update the precentage of recovery flash progress in dialog

Arguments:

  PercentageValue  -  The finished percentage of flash process
Returns:

--*/
{
  UINTN                 Columns;
  UINTN                 ColumnPosition;
  UINTN                 Rows;
  UINTN                 RowPosition;


  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  RowPosition = Columns / 2;
  ColumnPosition = Rows / 2;
  PrintAt (RowPosition, ColumnPosition, L"%d%%", PercentageValue);

}


VOID
DrawDialogBlock (
  VOID
  )
/*++

Routine Description:

  Drawing dialog for showing recovery flash progress

Arguments:

Returns:

--*/
{
  CHAR16                CleanLine[80];
  CHAR16                *StatusString;
  CHAR16                SelectIndexLin[80];
  UINTN                 Color;
  UINTN                 Columns;
  UINTN                 Index;
  UINTN                 Item;
  UINTN                 Rows;
  UINTN                 StrLenth;

  Color    =  0;
  Item     =  1;
  StrLenth = 20;
  //
  //Set the background
  //
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  Color = EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE);
  gST->ConOut->SetAttribute (gST->ConOut, Color);
  StrLenth = StrLenth / 2;
  for (Index = 0; Index < (StrLenth + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
    SelectIndexLin[Index] = 0x20;
  }
  CleanLine[(StrLenth + 2) * 2 + 1] = 0x0000;
  SelectIndexLin[1 + 1] = 0x0000;
  Item = Item / 2;
  for (Index = Rows / 2 - (Item + 3); Index <= Rows / 2 + (Item + 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%s", CleanLine);
  }

  //
  //Next three step will draw a dialog
  //1.draw three row line
  //2.draw two side
  //3.draw four corner
  //

  //
  //This is draw three row line
  //
  for (Index = Columns / 2 - (StrLenth + 1); Index < Columns / 2 + (StrLenth + 2) ; Index++) {
    PrintAt (Index, Rows / 2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows / 2 - (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows / 2 + (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
  }

  //
  //Draw two side
  //
  for (Index = Rows / 2 - (Item + 2); Index < Rows / 2 + (Item + 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (StrLenth + 2), Index, L"%c", BOXDRAW_VERTICAL);
  }

  //
  //This is draw the dialog four corner
  //
  PrintAt (Columns / 2 + (StrLenth + 2) ,Rows / 2 + (Item + 1), L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns / 2 + (StrLenth + 2) ,Rows / 2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (Columns / 2 - (StrLenth + 2), Rows / 2 + (Item + 1), L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns / 2 - (StrLenth + 2), Rows / 2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  //
  //Print the title and flash status percentage
  //
  StatusString = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_STATUS));
  PrintAt (Columns / 2 - (EfiStrLen (StatusString) / 2), Rows / 2 - (Item + 2), L"%s", StatusString);
  gBS->FreePool (StatusString);
}

VOID
RecoveryFlash (
  IN  UINTN     FlashMode
  )
/*++

Routine Description:

  The entry point to doing recovery flash

Arguments:

  FlashMode - Input flash mode (DEFAULT_FLASH_DEVICE_TYPE, SPI_FLASH_DEVICE_TYPE, OTHER_FLASH_DEVICE_TYPE)

Returns:

--*/
{
  UINT8	                    *BufferTmp;
  UINT8                     FlashDevice;
  UINT8	                    *FlashTmp;
  UINT8                     CommTmp;
  UINT8	                    *MapTmp;
  UINT8                     Index;
  CHAR16                    *RebootString;
  UINT32                    FlashAddress;
  UINT32                    FlashSize;
  UINTN                     Columns;
  UINTN                     CompareTemp;
  UINTN                     FlashPrecentage;
  UINTN                     IndexCounter;
  UINTN                     Item;
  UINTN                     PEIBaseTemp[20];
  UINTN                     Rows;
  UINTN                     WriteSize;
  VOID                      *HobList;
  EFI_PEI_HOB_POINTERS      RecoveryHob;
  EFI_STATUS                Status;
  UINTN                     TotalFlashSectors;
  UINTN                     FirmwareSize;


  POSTCODE (BDS_RECOVERY_START_FLASH);
  //
  // Initial local variable
  //
  Item        = 0;
  WriteSize   = 0;
  CommTmp     = 0xFF;
  Index       = 0;
  CompareTemp = 0;
  FlashTmp    = NULL;
  MapTmp      = NULL;
  FlashDevice = (UINT8) FlashMode;

#ifdef SECURE_BOOT_SUPPORT
  BackupAuthVariable ();
#endif

  EfiZeroMem (PEIBaseTemp, 20);
  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // Get the PEI phase .FD file memory base addess Hob ()
  //
  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  RecoveryHob.Raw  = HobList;
  RecoveryHob.Raw  = GetHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  while (RecoveryHob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION &&
        !EfiCompareGuid (&RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.Name,
        &gEfiRecoveryFileAddressGuid)) {
    RecoveryHob.Raw  = GET_NEXT_HOB (RecoveryHob);
    RecoveryHob.Raw  = GetHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  }

  HookResetSystem (TRUE);

#ifdef SECURE_FLASH_SUPPORT
  if ( RecoveryCapsuleIsExecutable (&RecoveryHob) ) {
    ExecuteRecoveryCapsule (&RecoveryHob);
    HookResetSystem (FALSE);
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
#endif

  BufferTmp =(UINT8 *) (UINTN)RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;
  FirmwareSize = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;


  //
  // Detect flash device
  //
  Status = gBS->AllocatePool (EfiBootServicesData, 0x100, &FlashTmp);
  Status = gBS->AllocatePool (EfiBootServicesData, 0x100, &MapTmp);
  gBS->SetMem (FlashTmp, 0x100, 0);
  gBS->SetMem (MapTmp, 0x100, 0);
  CommTmp = FlashPartInfo (FlashTmp, MapTmp, FlashDevice, FLASH_SMI_PORT);
  FlashAddress = 0xFFFFFFFF - (UINT32)FirmwareSize + 1;
  WriteSize = *(UINT16 *)(&(MapTmp[0]));

  if (FLASH_BLOCK_SIZE == WriteSize * SMI_FLASH_UNIT_BYTES) {
    FlashSize = FLASH_BLOCK_SIZE;
  } else {
    FlashSize = FLASH_SECTOR_SIZE;
  }

  //
  // Each block size is 0x1000 or 0x10000 and Call IHISI to flash ROM part
  //
  DrawDialogBlock ();
  TotalFlashSectors = FirmwareSize / FlashSize;
  for (IndexCounter = 0; IndexCounter < TotalFlashSectors; IndexCounter++) {
    CommTmp = FlashWrite (BufferTmp, FlashSize, FlashAddress, FLASH_SMI_PORT);
    FlashAddress += FlashSize;
    BufferTmp += FlashSize;
    FlashPrecentage = (IndexCounter + 1) * 100 / TotalFlashSectors;
    Drawpercentage (FlashPrecentage);
  }

#ifdef SECURE_BOOT_SUPPORT
  RestoreAuthVariable ();
#endif
  HookResetSystem (FALSE);
  
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  RebootString = GetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_REBOOT));
  PrintAt (Columns / 2 - 4, Rows / 2 - Item + 0, L"%s", RebootString);
  gBS->FreePool (RebootString);
  for (IndexCounter = 0; IndexCounter < 10000; IndexCounter++) {
    gBS->Stall (300);
  }
  ResetCommand ();
}
