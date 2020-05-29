/** @file
  Routines for displaying virtual keyboard in bitlocker

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
#include <PiDxe.h>
#include <Guid/ReturnFromImage.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SetupMouse.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>

#define TICKS_PER_MS            10000U

typedef struct _BITLOCKER_PRIVATE {
  EFI_GRAPHICS_OUTPUT_PROTOCOL        *GraphicsOutput;
  EFI_SIMPLE_POINTER_PROTOCOL         *SimplePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL       *AbsolutePointer;
  EFI_SETUP_MOUSE_PROTOCOL            *SetupMouse;

  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *SimpleTextInputEx;
  EFI_INPUT_READ_KEY_EX               OrgReadKeyStrokeEx;

  EFI_EVENT                           CheckReadKeyEvent;
  EFI_EVENT                           DisableCheckReadKeyEvent;

  UINTN                               NoReadKeyCount;

  BOOLEAN                             FirstIn;
  BOOLEAN                             FirstDisplayKB;
  BOOLEAN                             ReadKeyFlag;
  BOOLEAN                             SetupMouseIsStart;
} BITLOCKER_PRIVATE;

STATIC BITLOCKER_PRIVATE *mBitlocker = NULL;

#define RESOURCE_SECTION_RESOURCE_TYPE_VERSION               16
#define VS_FIXEDFILEINFO_SIGNATURE                           0xFEEF04BD
#define MICROSOFT_OS_VERSION_WINDOWS_8                       0x00060002

typedef struct {
  UINT32             Signature;
  UINT32             StrucVersion;
  UINT32             FileVersionMS;
  UINT32             FileVersionLS;
  UINT32             ProductVersionMS;
  UINT32             ProductVersionLS;
  UINT32             FileFlagsMask;
  UINT32             FileFlags;
  UINT32             FileOS;
  UINT32             FileType;
  UINT32             FileSubtype;
  UINT32             FileDateMS;
  UINT32             FileDateLS;
} VS_FIXEDFILEINFO;

/**

 Get product version from VS_FIXEDFILEINFO of resoruce section (.rsrc) in image.

 @param [in]   ResourceDir      Pointer of image resource directory
 @param [in]   ResourceData     Pointer of image resource data
 @param [in]   ResourceDataSize Size of image resource data
 @param [out]  VersionMS        Product version
 @param [out]  VersionLS        Product version

 @retval EFI_SUCCESS            Find product version success
 @retval EFI_INVALID_PARAMETER  Invalid parameter
 @retval EFI_NOT_FOUND          The product version isn't found

**/
EFI_STATUS
GetWindowsOsVer (
  IN  EFI_IMAGE_DATA_DIRECTORY          *ResourceDir,
  IN  UINT8                             *ResourceData,
  IN  UINT32                            ResourceDataSize,
  OUT UINT32                            *VersionMS,
  OUT UINT32                            *VersionLS
  )
{
  EFI_IMAGE_RESOURCE_DIRECTORY          *VersionDir;
  EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY    *VersionDirEntry;
  EFI_IMAGE_RESOURCE_DIRECTORY          *Dir;
  EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY    *DirEntry;
  EFI_IMAGE_RESOURCE_DATA_ENTRY         *DataEntry;
  UINT16                                Index;
  UINT16                                Count;
  UINT32                                Offset;
  UINT8                                 *Ptr;
  UINT8                                 *VersionInfo;
  UINT16                                VersionInfoSize;
  UINT16                                FixedFileInfoSize;
  UINT16                                Type;
  CHAR16                                *KeyStr;
  UINT16                                MaxPaddingCount;
  VS_FIXEDFILEINFO                      *FixedFileInfo;

  if (ResourceData == NULL || ResourceDataSize == 0 || VersionMS == NULL || VersionLS == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get RT_VERSION directory
  //
  Dir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) ResourceData;
  DirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) Dir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));

  Count = Dir->NumberOfNamedEntries + Dir->NumberOfIdEntries;
  for (Index = 0; Index < Count; Index++) {
    if (DirEntry[Index].u1.Id == RESOURCE_SECTION_RESOURCE_TYPE_VERSION &&
        DirEntry[Index].u2.s.DataIsDirectory) {
      break;
    }
  }
  if (Index == Count) {
    return EFI_NOT_FOUND;
  }
  VersionDir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) (ResourceData + DirEntry[Index].u2.s.OffsetToDirectory);
  VersionDirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) VersionDir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));

  //
  // Get version info data entry
  //
  Count = VersionDir->NumberOfNamedEntries + VersionDir->NumberOfIdEntries;
  for (Index = 0; Index < Count; Index++) {
    if (!VersionDirEntry[Index].u2.s.DataIsDirectory) {
      continue;
    }
    Dir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) (ResourceData + VersionDirEntry[Index].u2.s.OffsetToDirectory);
    DirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) Dir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));

    if (DirEntry[Index].u2.s.DataIsDirectory) {
      continue;
    }

    DataEntry = (EFI_IMAGE_RESOURCE_DATA_ENTRY *) (ResourceData + DirEntry[Index].u2.OffsetToData);
    if (DataEntry->OffsetToData < ResourceDir->VirtualAddress) {
      continue;
    }

    //
    // Parsing VS_VERSIONINFO which is defined in MSDN
    //
    Offset = DataEntry->OffsetToData - ResourceDir->VirtualAddress;
    VersionInfo = ResourceData + Offset;
    Ptr = VersionInfo;
    VersionInfoSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);
    FixedFileInfoSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);
    Type = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);
    KeyStr = (CHAR16 *) Ptr;
    if (StrCmp (KeyStr, L"VS_VERSION_INFO") != 0) {
      continue;
    }
    Ptr += StrSize (KeyStr);

    MaxPaddingCount = 3;
    while (*Ptr == 0 && MaxPaddingCount > 0) {
      Ptr++;
      MaxPaddingCount--;
    }
    if (MaxPaddingCount == 0) {
      continue;
    }
    FixedFileInfo = (VS_FIXEDFILEINFO *) Ptr;

    if (FixedFileInfo->Signature != VS_FIXEDFILEINFO_SIGNATURE) {
      continue;
    }

    *VersionMS = FixedFileInfo->ProductVersionMS;
    *VersionLS = FixedFileInfo->ProductVersionLS;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
BdsLibGetImageHeader (
  IN  EFI_HANDLE                            Device,
  IN  CHAR16                                *FileName,
  OUT EFI_IMAGE_DOS_HEADER                  *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  );

/**

 Get resoruce section (.rsrc) of image.

 @param [in]   Device           Handle of SimpleFileSystem
 @param [in]   FileName         Target file name
 @param [in]   ResourceDataDir  Resource data directory
 @param [out]  ResourceData     Resource data
 @param [out]  ResourceDataSize Size of data size

 @retval EFI_SUCCESS            The resource section is found
 @retval EFI_NOT_FOUND          The resource section isn't found

**/
EFI_STATUS
GetResourceSectionInfo (
  IN EFI_HANDLE                 Device,
  IN CHAR16                     *FileName,
  OUT EFI_IMAGE_DATA_DIRECTORY  *ResourceDataDir,
  OUT UINT8                     **ResourceData,
  OUT UINT32                    *ResourceDataSize
  )
{
  EFI_STATUS                            Status;
  EFI_IMAGE_DOS_HEADER                  DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  UINT16                                Magic;
  EFI_IMAGE_DATA_DIRECTORY             *DataDir;
  UINT16                               NumOfSections;
  EFI_IMAGE_SECTION_HEADER             *SectionHdrPtr;

  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;
  UINT8                            *Buffer;
  UINTN                            BufferSize;
  UINT16                           Index;

  Hdr.Union = &HdrData;
  Status = BdsLibGetImageHeader (Device, FileName, &DosHeader, Hdr);
  if (EFI_ERROR (Status) || !EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)) {
    return Status;
  }

  if (Hdr.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    Magic = Hdr.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    DataDir       = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_RESOURCE];
    NumOfSections = Hdr.Pe32->FileHeader.NumberOfSections;
  } else {
    DataDir       = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_RESOURCE];
    NumOfSections = Hdr.Pe32Plus->FileHeader.NumberOfSections;
  }

  if (DataDir->Size == 0) {
    return EFI_NOT_FOUND;
  }

  Buffer   = NULL;
  Root     = NULL;
  ThisFile = NULL;
  Status = gBS->HandleProtocol (Device, &gEfiSimpleFileSystemProtocolGuid, (VOID *) &Volume);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (Volume, &Root);
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }

  Status = Root->Open (Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Move to section header
  //
  Status = ThisFile->SetPosition (ThisFile, DosHeader.e_lfanew + sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION));
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  BufferSize = NumOfSections * sizeof (EFI_IMAGE_SECTION_HEADER);
  Buffer     = AllocatePool (BufferSize);
  if (Buffer == NULL) {
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }
  Status = ThisFile->Read (ThisFile, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  SectionHdrPtr = (EFI_IMAGE_SECTION_HEADER *) Buffer;

  //
  // Get the pointer of resource section header
  //
  for (Index = 0; Index < NumOfSections; Index++) {
    if (AsciiStrnCmp ((CHAR8 *)SectionHdrPtr, ".rsrc", 5) == 0) {
      break;
    }
    SectionHdrPtr++;
  }
  if (Index == NumOfSections) {
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  //
  // Move to resource section data
  //
  Status = ThisFile->SetPosition (ThisFile, SectionHdrPtr->PointerToRawData);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  gBS->FreePool (Buffer);
  BufferSize = DataDir->Size;
  Buffer     = AllocatePool (BufferSize);
  Status = ThisFile->Read (ThisFile, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (ResourceDataDir != NULL) {
    CopyMem (ResourceDataDir, DataDir, sizeof (EFI_IMAGE_DATA_DIRECTORY));
  }
  if (ResourceData != NULL) {
    *ResourceData = Buffer;
  }
  if (ResourceDataSize != NULL) {
    *ResourceDataSize = DataDir->Size;
  }

Done:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }

  return Status;
}

/**

 Check the boot device path whether is Windows.

 @param [in]   DeivcePath       Device path of boot option

 @return                        Whether is Windows boot option

**/
BOOLEAN
IsWindowsOS (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *WorkingDevicePath;
  EFI_HANDLE                    Device;
  FILEPATH_DEVICE_PATH          *FilePath;
  EFI_IMAGE_DATA_DIRECTORY      ResourceDataDir;
  UINT8                         *ResourceData;
  UINT32                        ResourceDataSize;
  UINT32                        VersionMS;
  UINT32                        VersionLS;


  WorkingDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Device
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (DevicePathType (WorkingDevicePath) != MEDIA_DEVICE_PATH ||
      DevicePathSubType (WorkingDevicePath) != MEDIA_FILEPATH_DP) {
    return FALSE;
  }

  FilePath = (FILEPATH_DEVICE_PATH *) WorkingDevicePath;
  Status = GetResourceSectionInfo (Device, FilePath->PathName, &ResourceDataDir, &ResourceData, &ResourceDataSize);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = GetWindowsOsVer (&ResourceDataDir, ResourceData, ResourceDataSize, &VersionMS, &VersionLS);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}

/**

 Close check read key event

 @param [in] Event              Event
 @param [in] Context            Passed parameter to event handler

**/
VOID
EFIAPI
DisableCheckReadKeyHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  BITLOCKER_PRIVATE             *Private;

  Private = (BITLOCKER_PRIVATE *) Context;

  gBS->CloseEvent (Private->DisableCheckReadKeyEvent);
  Private->DisableCheckReadKeyEvent = NULL;

  if (Private->CheckReadKeyEvent != NULL) {
    gBS->CloseEvent (Private->CheckReadKeyEvent);
    Private->CheckReadKeyEvent = NULL;
  }
}


/**

 Close setup mouse if no read key at a period time

 @param [in] Event              Event
 @param [in] Context            Passed parameter to event handler

**/
VOID
EFIAPI
CheckReadKeyHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  BITLOCKER_PRIVATE             *Private;

  Private = (BITLOCKER_PRIVATE *) Context;

  if (!Private->ReadKeyFlag) {
    Private->NoReadKeyCount++;
  } else {
    Private->NoReadKeyCount = 0;
  }

  Private->ReadKeyFlag = FALSE;

  if (Private->NoReadKeyCount > 2) {
    gBS->CloseEvent (Private->CheckReadKeyEvent);
    Private->CheckReadKeyEvent = NULL;

    if (Private->DisableCheckReadKeyEvent != NULL) {
      gBS->CloseEvent (Private->DisableCheckReadKeyEvent);
      Private->DisableCheckReadKeyEvent = NULL;
    }

    Private->SetupMouseIsStart = FALSE;
    Private->SetupMouse->Close (Private->SetupMouse);
  }
}

/**

 Monitor readkeystroke function to create timer event of bitlocker.

 @param [in]  This              Protocol instance pointer of SimpleTextInputEx.
 @param [out] KeyData           EFI key data.

 @retval                        Origianl ReadKeyStrokeEx function return status

**/
EFI_STATUS
EFIAPI
BitlockerKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  EFI_STATUS                    Status;
  EFI_SIMPLE_POINTER_STATE      SimplePointerState;
  EFI_ABSOLUTE_POINTER_STATE    AbsolutePointerState;
  KEYBOARD_ATTRIBUTES           KeyboardAttrs;
  BITLOCKER_PRIVATE             *Private;

  Private = (BITLOCKER_PRIVATE *) mBitlocker;


  if (Private->FirstIn) {
    Private->FirstIn = FALSE;
    Private->SimplePointer->Reset (Private->SimplePointer, TRUE);
    Private->AbsolutePointer->Reset (Private->AbsolutePointer, TRUE);
  }

  if (!Private->SetupMouseIsStart) {
    Status = Private->SimplePointer->GetState(Private->SimplePointer, &SimplePointerState);
    if (EFI_ERROR (Status)) {
      Status = Private->AbsolutePointer->GetState(Private->AbsolutePointer, &AbsolutePointerState);
    }

    if (!EFI_ERROR (Status)) {
      Private->SetupMouseIsStart = TRUE;
    }
  }

  if (Private->SetupMouseIsStart) {
    Private->SetupMouse->Start (Private->SetupMouse);
    Private->SetupMouse->GetKeyboardAttributes (Private->SetupMouse, &KeyboardAttrs);
    if (!KeyboardAttrs.IsStart) {
      if (Private->FirstDisplayKB) {
        Private->FirstDisplayKB = FALSE;
        Private->SetupMouse->StartKeyboard (Private->SetupMouse, (UINTN) 10000, (UINTN) 10000); // right-bottom
      } else {
        Private->SetupMouse->StartKeyboard (Private->SetupMouse, (UINTN) KeyboardAttrs.X, (UINTN) KeyboardAttrs.Y);
      }
    }
  }

  Private->ReadKeyFlag = TRUE;
  Status = Private->OrgReadKeyStrokeEx (This, KeyData);
  if (EFI_ERROR (Status) || !Private->SetupMouseIsStart) {
    return Status;
  }

  if (Private->DisableCheckReadKeyEvent == NULL &&
      Private->CheckReadKeyEvent == NULL &&
      (KeyData->Key.ScanCode == SCAN_ESC || KeyData->Key.UnicodeChar == CHAR_CARRIAGE_RETURN)) {

    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    DisableCheckReadKeyHandler,
                    Private,
                    &Private->DisableCheckReadKeyEvent
                    );
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR (Status)) {
      Status = gBS->SetTimer(Private->DisableCheckReadKeyEvent, TimerRelative, 500 * TICKS_PER_MS);
    }

    Private->NoReadKeyCount = 0;
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    CheckReadKeyHandler,
                    Private,
                    &Private->CheckReadKeyEvent
                    );
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR (Status)) {
      Status = gBS->SetTimer(Private->CheckReadKeyEvent, TimerPeriodic, 50 * TICKS_PER_MS);
    }
  }

  return EFI_SUCCESS;
}

/**

 Disable Bitlocker handler when receive return from image event

 @param [in] Event              Event
 @param [in] Context            Passed parameter to event handler

**/
VOID
EFIAPI
DisableBitlockerHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  BITLOCKER_PRIVATE             *Private;

  Private = (BITLOCKER_PRIVATE *) Context;

  gBS->CloseEvent (Event);
  if (Private->CheckReadKeyEvent != NULL) {
    gBS->CloseEvent (Private->CheckReadKeyEvent);
    Private->CheckReadKeyEvent = NULL;
  }

  if (Private->DisableCheckReadKeyEvent != NULL) {
    gBS->CloseEvent (Private->DisableCheckReadKeyEvent);
    Private->DisableCheckReadKeyEvent = NULL;
  }

  Private->SetupMouseIsStart = FALSE;
  Private->SetupMouse->Close (Private->SetupMouse);
  Private->SimpleTextInputEx->ReadKeyStrokeEx = Private->OrgReadKeyStrokeEx;
}


/**

  Bitlocker virtual keybaord support

  @param [in] DevicePath        Device path of boot option

  @retval EFI_SUCCESS           Success
  @retval EFI_UNSUPPORTED       It isn't Windows boot option.

**/
EFI_STATUS
EFIAPI
BitlockerVirtualKeyboardSupport (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     Event;
  VOID                          *Registration;
  BITLOCKER_PRIVATE             *Private;

  if (!IsWindowsOS (DevicePath)) {
    return EFI_UNSUPPORTED;
  }

  if (mBitlocker == NULL) {
    mBitlocker = AllocateZeroPool (sizeof (BITLOCKER_PRIVATE));
    if (mBitlocker == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    ZeroMem (mBitlocker, sizeof (BITLOCKER_PRIVATE));
  }

  Private = mBitlocker;

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &Private->GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // SimplePointer, Absolute should be install by ConSplitter
  //
  Status = gBS->HandleProtocol (
                  gST->ConsoleInHandle,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **) &Private->SimplePointer
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Status = gBS->HandleProtocol (
                  gST->ConsoleInHandle,
                  &gEfiAbsolutePointerProtocolGuid,
                  (VOID **) &Private->AbsolutePointer
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gSetupMouseProtocolGuid,
                  NULL,
                  (VOID **) &Private->SetupMouse
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleInHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID **) &Private->SimpleTextInputEx
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  DisableBitlockerHandler,
                  Private,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gReturnFromImageGuid,
                  Event,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (Event);
    return Status;
  }

  Private->FirstIn        = TRUE;
  Private->FirstDisplayKB = TRUE;

  //
  // hook ReadKeyStrokeEx of SimpleTextInEx
  //
  Private->OrgReadKeyStrokeEx = Private->SimpleTextInputEx->ReadKeyStrokeEx;
  Private->SimpleTextInputEx->ReadKeyStrokeEx = BitlockerKeyboardReadKeyStrokeEx;

  return EFI_SUCCESS;
}

