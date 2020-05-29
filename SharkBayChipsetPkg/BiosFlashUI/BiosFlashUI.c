/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/BvdtLib.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FileSelectUI.h>
#include <Protocol/BiosFlashUI.h>

#define UI_FOCUS                   EFI_TEXT_ATTR(EFI_BLACK, EFI_LIGHTGRAY)
#define UI_UNFOCUS                 EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE)
#define CMD_FILENAME_LENGTH        30  // length of the file name field
#define CMD_PARAM_LENGTH           20  // length of the cmd line parameter field
#define UI_MAX_STRING_WIDTH        40
#define UI_NUM_OF_LINES            15

//
// Static strings
//
STATIC CONST CHAR16                TitleString[]           = L"BIOS Flash Update";
STATIC CONST CHAR16                BIOSUpdateInfoString[]  = L"BIOS Upgrade Information";
STATIC CONST CHAR16                SysBIOSInfoString[]     = L"System BIOS Information";
STATIC CONST CHAR16                BiosUpdateFileSring[]   = L"BIOS Update File:";
STATIC CONST CHAR16                SystemString[]          = L"System:   ";
STATIC CONST CHAR16                RevisionString[]        = L"Revision: ";
STATIC CONST CHAR16                VendorString[]          = L"Vendor:   ";
STATIC CONST CHAR16                PowerStatus[]           = L"Power Status: ";
STATIC CONST CHAR16                CancelButton[]          = L"Cancel";
STATIC CONST CHAR16                BeginFlashButton[]      = L"Start Flash Upgrade";
STATIC CONST CHAR16                FileSelectButton[]      = L"...";

UINT8                              *mBufferPtr = NULL;
UINTN                              mBufferSize;
UINTN                              mLeftMargin = 0;
UINTN                              mCurrentSystemInfoRow = 0;
UINTN                              mBiosUpdateInfoRow = 0;
UINTN                              mUpdateFileNameRow = 0;
UINTN                              mPowerStatusRow = 0;
CHAR16                             *mCmdLineParameter;
UINTN                              mCmdLineIndex;
BOOLEAN                            mAutoLoad = TRUE;

extern EFI_GUID gEfiGenericVariableGuid;

typedef enum {
  State_FileSelect,
  State_CmdLine,
  State_UpgradeBios,
  State_Cancel,
} BIOS_UPDATE_UI_STATE;


EFI_STATUS
GetBiosUpdatePackage (
  OUT VOID                          **FileBuffer,
  OUT UINTN                         *FileSize
  );

EFI_STATUS
FindBiosUpdatePackage (
  IN OUT VOID                          **FileBuffer,
  IN OUT UINTN                         *FileSize
  );

EFI_STATUS
BiosFlashFrontPage (
  IN OUT VOID                       **BufferPtr,
  IN OUT UINTN                      *BufferSize,
  IN OUT CHAR16                     **CommandString
  );

EFI_STATUS
DrawFrontPageFrame (
  VOID
  );

EFI_STATUS
UpdateSystemBIOSInfo (
  VOID
  );

EFI_STATUS
UpdatePowerStatus (
  VOID
  );

EFI_STATUS
UpdateBIOSUpgradeInfo (
  VOID
  );

EFI_STATUS
InvalidFile (
  );

EFI_STATUS
FocusUpgradeBios (
  BOOLEAN           Focus
  );

EFI_STATUS
FocusCancel (
  BOOLEAN           Focus
  );

EFI_STATUS
FocusCmdLine (
  BOOLEAN           Focus
  );

EFI_STATUS
FocusFileSelect (
  BOOLEAN           Focus
  );

EFI_STATUS
CmdLineInputHandle (
  IN OUT EFI_INPUT_KEY                   *EventKey
  );

EFI_STATUS
ConfirmBiosUpgrade (
  IN OUT EFI_INPUT_KEY                   *EventKey
  );

UINTN
PrintAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *Fmt,
  ...
  );

/**
  According to device path to open file .
  @param  DeviceHandle  Handle of the current directory
  @param  DevicePath    Pointer to EFI_DEVICE_PATH_PROTOCOL
  @param  OpenMode      The mode to open the file. The only valid combinations that the
                        file may be opened with are: Read, Read/Write, or Create/Read/
                        Write. See ¡§Related Definitions¡¨ below.
  @param  Attributes    Only valid for EFI_FILE_MODE_CREATE, in which case these
                        are the attribute bits for the newly created file. See ¡§Related
                        Definitions¡¨ below.
  @param  NewHandle     A pointer to the location to return the opened handle for the new
                        file. See the type EFI_FILE_PROTOCOL description.

  @retval EFI_SUCCESS            The file was opened.
  @retval EFI_NOT_FOUND          The specified file could not be found on the device.
  @retval EFI_NO_MEDIA           The device has no medium.
  @retval EFI_MEDIA_CHANGED      The device has a different medium in it or the medium is no
                                 longer supported.
  @retval EFI_DEVICE_ERROR       The device reported an error.
  @retval EFI_VOLUME_CORRUPTED   The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED    An attempt was made to create a file, or open a file for write
                                 when the media is write-protected.
  @retval EFI_ACCESS_DENIED      The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES   Not enough resources were available to open the file.
  @retval EFI_VOLUME_FULL        The volume is full.

**/
EFI_STATUS
BdsLibOpenFile (
  IN  EFI_FILE_HANDLE            DeviceHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  UINT64                     OpenMode,
  IN  UINT64                     Attributes,
  OUT EFI_FILE_HANDLE            *NewHandle
  )
{
  EFI_STATUS                        Status;
  FILEPATH_DEVICE_PATH              *FilePathNode;
  EFI_FILE_HANDLE                   FileHandle;
  EFI_FILE_HANDLE                   LastHandle;
  FILEPATH_DEVICE_PATH              *OriginalFilePathNode;

  FilePathNode = (FILEPATH_DEVICE_PATH *) DevicePath;
  *NewHandle   = NULL;
  FileHandle   = DeviceHandle;
  Status       = EFI_SUCCESS;

  //
  // Duplicate the device path to avoid the access to unaligned device path node.
  // Because the device path consists of one or more FILE PATH MEDIA DEVICE PATH
  // nodes, It assures the fields in device path nodes are 2 byte aligned.
  //
  FilePathNode = (FILEPATH_DEVICE_PATH *) DuplicateDevicePath (
                                            (EFI_DEVICE_PATH_PROTOCOL *)(UINTN)FilePathNode
                                            );
  if (FilePathNode == NULL) {
    FileHandle->Close (FileHandle);
    Status = EFI_OUT_OF_RESOURCES;
  } else {
    OriginalFilePathNode = FilePathNode;
    //
    // Parse each MEDIA_FILEPATH_DP node. There may be more than one, since the
    //  directory information and filename can be seperate. The goal is to inch
    //  our way down each device path node and close the previous node
    //
    while (!IsDevicePathEnd (&FilePathNode->Header)) {
      if (DevicePathType (&FilePathNode->Header) != MEDIA_DEVICE_PATH ||
          DevicePathSubType (&FilePathNode->Header) != MEDIA_FILEPATH_DP) {
        Status = EFI_UNSUPPORTED;
      }

      if (EFI_ERROR (Status)) {
        //
        // Exit loop on Error
        //
        break;
      }

      LastHandle = FileHandle;
      FileHandle = NULL;
      Status = LastHandle->Open (
                             LastHandle,
                             &FileHandle,
                             FilePathNode->PathName,
                             OpenMode,
                             Attributes
                             );
      //
      // Close the previous node
      //
      if (LastHandle != DeviceHandle) {
        LastHandle->Close (LastHandle);
      }
      FilePathNode = (FILEPATH_DEVICE_PATH *) NextDevicePathNode (&FilePathNode->Header);
    }
    //
    // Free the allocated memory pool
    //
    gBS->FreePool (OriginalFilePathNode);
  }

  if (Status == EFI_SUCCESS) {
    *NewHandle = FileHandle;
  }

  return Status;
}

EFI_STATUS
FindBiosUpdatePackage (
  IN OUT VOID                          **FileBuffer,
  IN OUT UINTN                         *FileSize
  )
{
  UINT8                                *TempPtr;
  UINTN                                Index;
  EFI_IMAGE_DOS_HEADER                 *DosHdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION      *PeHdr;
  UINTN                                NewFileSize;
  UINT16                               Subsystem;
  BOOLEAN                              EfiPackage;

  TempPtr = *FileBuffer;
  Index = 0;
  EfiPackage = TRUE;
  
  //SignatureLength = AsciiStrLen (ShellPackageSignature);

  while (Index < *FileSize) {
    if (EfiPackage) {
      DosHdr = (EFI_IMAGE_DOS_HEADER*)TempPtr;
      if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
        PeHdr = (EFI_IMAGE_OPTIONAL_HEADER_UNION*) (TempPtr + DosHdr->e_lfanew);
        if (PeHdr->Pe32.Signature == EFI_IMAGE_NT_SIGNATURE) {
          //
          // Determine PE type and read subsytem
          //
          if (PeHdr->Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            Subsystem = PeHdr->Pe32.OptionalHeader.Subsystem;
          } else if (PeHdr->Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
            Subsystem = PeHdr->Pe32Plus.OptionalHeader.Subsystem;
          } else {
            goto DoScanFile;
          }

          if (Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
            *FileBuffer = TempPtr;
            NewFileSize = *FileSize - Index;
            *FileSize = NewFileSize;
            return EFI_SUCCESS;
          }
        }
      }
    }
DoScanFile:
    TempPtr++;
    Index++;
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
BiosFlashFrontPage (
  IN OUT VOID                       **BufferPtr,
  IN OUT UINTN                      *BufferSize,
  IN OUT CHAR16                     **CommandString
  )
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  FILE_SELECT_UI_PROTOCOL            *FileSelectProtocol = NULL;
  EFI_INPUT_KEY                      Key;
  BIOS_UPDATE_UI_STATE               UiState;
  BOOLEAN                            LoopEnable;
  BOOLEAN                            Repaint;

  Status = gBS->LocateProtocol (&gFileSelectUIProtocolGuid, NULL, (VOID**)&FileSelectProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->ClearScreen (gST->ConOut);

  UiState = State_FileSelect;
  Key.UnicodeChar = 0;
  Key.ScanCode = 0;
  LoopEnable = TRUE;
  Repaint = TRUE;
  mCmdLineIndex = 0;
  mCmdLineParameter = AllocateZeroPool ((CMD_PARAM_LENGTH+1) * sizeof(CHAR16));
  mBufferPtr = NULL;
  mAutoLoad = TRUE;
  
  while (LoopEnable) {
    if (Repaint) {
      DrawFrontPageFrame ();
      UpdateSystemBIOSInfo ();
      UpdatePowerStatus ();
      UpdateBIOSUpgradeInfo ();
      FocusFileSelect (FALSE);
      FocusCmdLine (FALSE);
      FocusUpgradeBios (FALSE);
      FocusCancel (FALSE);
      switch (UiState) {
        case State_FileSelect:
          FocusFileSelect (TRUE);
          break;
        case State_CmdLine:
          FocusCmdLine (TRUE);
          break;
        case State_UpgradeBios:
          FocusUpgradeBios (TRUE);
          break;
        case State_Cancel:
          FocusCancel (TRUE);
          break;
      }
      Repaint = FALSE;
    }

    do {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    } while (Status != EFI_SUCCESS);

    switch (UiState) {
      case State_CmdLine:
        if (Key.UnicodeChar == CHAR_TAB) {
          UiState = State_FileSelect;
          FocusCmdLine (FALSE);
          FocusFileSelect (TRUE);
        } else if (Key.ScanCode == SCAN_ESC) {
          UiState = State_Cancel;
          FocusCmdLine (FALSE);
          FocusCancel (TRUE);
        } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          if (mBufferPtr != NULL) {
            UiState = State_UpgradeBios;
            FocusCmdLine (FALSE);
            FocusUpgradeBios (TRUE);
          } else {
            UiState = State_FileSelect;
            FocusCmdLine (FALSE);
            FocusFileSelect (TRUE);
          }
        } else {
          Status = CmdLineInputHandle (&Key);
        }
        break;
      case State_FileSelect:
        if (Key.UnicodeChar == CHAR_TAB) {
          if (mBufferPtr != NULL) {
            UiState = State_UpgradeBios;
            FocusFileSelect (FALSE);
            FocusUpgradeBios (TRUE);
          } else {
            UiState = State_Cancel;
            FocusFileSelect (FALSE);
            FocusCancel (TRUE);
          }
        } else if (Key.ScanCode == SCAN_ESC) {
          UiState = State_Cancel;
          FocusFileSelect (FALSE);
          FocusCancel (TRUE);
//[-start-140402-IB10300106-modify]//
        } else if ((Key.UnicodeChar == CHAR_CARRIAGE_RETURN) || mAutoLoad) {
//[-end-140402-IB10300106-modify]//
          Status = FileSelectProtocol->LoadFileOperation((VOID **)&mBufferPtr, &mBufferSize);
          if (!EFI_ERROR(Status)) {
            Status = FindBiosUpdatePackage ((VOID **)&mBufferPtr, &mBufferSize);
            if (EFI_ERROR(Status)) {
              gBS->FreePool ((VOID *)mBufferPtr);
              mBufferPtr = NULL;
              InvalidFile ();
              gRT->SetVariable (
                     L"BiosUpdateFileName",
                     &gEfiGenericVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     0,
                     NULL
                     );
              gRT->SetVariable (
                     L"BiosUpdateFilePath",
                     &gEfiGenericVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     0,
                     NULL
                     );

              Repaint = TRUE;
            } else {
              UiState = State_CmdLine;
              Repaint = TRUE;
              
//[-start-140402-IB10300106-add]//
              if (mAutoLoad) {
                UiState = State_UpgradeBios;
                FocusFileSelect (FALSE);
                FocusUpgradeBios (TRUE);
              }
//[-end-140402-IB10300106-add]//
            }
            ZeroMem (mCmdLineParameter, (CMD_PARAM_LENGTH+1) * sizeof(CHAR16));
            mCmdLineIndex = 0;
            FocusCmdLine (FALSE);
          } else {
            Repaint = TRUE;
          }
//[-start-140402-IB10300106-add]//
          mAutoLoad = FALSE;
//[-end-140402-IB10300106-add]//
        }
        break;
      case State_UpgradeBios:
        if (Key.UnicodeChar == CHAR_TAB) {
          UiState = State_Cancel;
          FocusUpgradeBios (FALSE);
          FocusCancel (TRUE);
        } else if (Key.ScanCode == SCAN_ESC) {
          UiState = State_Cancel;
          FocusUpgradeBios (FALSE);
          FocusCancel (TRUE);
        } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          Status = ConfirmBiosUpgrade (&Key);
          if (!EFI_ERROR(Status)) {
            LoopEnable = FALSE;
            *BufferPtr     = mBufferPtr;
            *BufferSize    = mBufferSize;
            *CommandString = mCmdLineParameter;
            return EFI_SUCCESS;
          } else {
            Repaint = TRUE;
          }
        }
        break;
      case State_Cancel:
        if (Key.UnicodeChar == CHAR_TAB) {
          UiState = State_CmdLine;
          FocusCancel (FALSE);
          FocusCmdLine (TRUE);
        } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          LoopEnable = FALSE;
        }
        break;
    }
  }

  if (mBufferPtr != NULL) {
    gBS->FreePool (mBufferPtr);
    mBufferPtr = NULL;
  }
  if (mCmdLineParameter != NULL) {
    gBS->FreePool (mCmdLineParameter);
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
DrawFrontPageFrame (
  VOID
  )
{
//   EFI_STATUS                                  Status;
  UINTN                                       Columns;
  UINTN                                       Rows;
  INT32                                       BackupAttribute;
  UINTN                                       Index;
  UINTN                                       Item;
  CHAR16                                      *CleanLine;

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);

  BackupAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));

  CleanLine = AllocateZeroPool (((UI_MAX_STRING_WIDTH + 2) * 2 + 2) * sizeof (CHAR16));
  if (CleanLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < (UI_MAX_STRING_WIDTH + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
  }

  Item = UI_NUM_OF_LINES;
  Item = Item / 2;
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 - (Item + 3) + 3 + UI_NUM_OF_LINES; Index++) {
    PrintAt (Columns / 2 - (UI_MAX_STRING_WIDTH + 2), Index, L"%s", CleanLine);
  }

  for (Index = Columns/2 - (UI_MAX_STRING_WIDTH + 1); Index < Columns/2 + (UI_MAX_STRING_WIDTH + 2) ; Index++) {
    PrintAt (Index, Rows/2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 3) + 3 + UI_NUM_OF_LINES, L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = Rows/2 - (Item + 2); Index < Rows/2 - (Item + 3) + 3 + UI_NUM_OF_LINES; Index++) {
    PrintAt (Columns / 2 - (UI_MAX_STRING_WIDTH + 2), Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (UI_MAX_STRING_WIDTH + 2), Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (Columns/2 + (UI_MAX_STRING_WIDTH + 2) ,Rows/2 - (Item + 3) + 3 + UI_NUM_OF_LINES, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns/2 + (UI_MAX_STRING_WIDTH + 2) ,Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);

  PrintAt (Columns/2 - (UI_MAX_STRING_WIDTH + 2), Rows/2 - (Item + 3) + 3 + UI_NUM_OF_LINES, L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns/2 - (UI_MAX_STRING_WIDTH + 2), Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  //
  // Print Title
  //
  PrintAt (Columns/2 - (StrLen(TitleString)/2), Rows/2 - (Item + 2), L"%s", TitleString);

  //
  // Print BIOS Update info section
  //
  Index = Rows/2 - (Item);
  mLeftMargin = Columns/2 - UI_MAX_STRING_WIDTH;
  PrintAt (mLeftMargin, Index, L"%s", BIOSUpdateInfoString);

  //
  // Print File select entry
  //
  Index++;
  mUpdateFileNameRow = Index;
  PrintAt (mLeftMargin, Index, L"%s", BiosUpdateFileSring);

  //
  // Print new BIOS Info
  //
  Index++;
  mBiosUpdateInfoRow = Index;
  PrintAt (mLeftMargin, Index, L"%s", SystemString);
  Index++;
  PrintAt (mLeftMargin, Index, L"%s", RevisionString);
  Index++;
  PrintAt (mLeftMargin, Index, L"%s", VendorString);

  //
  // Print System BIOS Info section
  //
  Index+=2;
  PrintAt (mLeftMargin, Index, L"%s", SysBIOSInfoString);

  //
  // Print current BIOS Info
  //
  Index++;
  mCurrentSystemInfoRow = Index;
  PrintAt (mLeftMargin, Index, L"%s", SystemString);
  Index++;
  PrintAt (mLeftMargin, Index, L"%s", RevisionString);
  Index++;
  PrintAt (mLeftMargin, Index, L"%s", VendorString);

  //
  // Print Power Status
  //
  Index+=2;
  mPowerStatusRow = Index;
  PrintAt (mLeftMargin, Index, L"%s", PowerStatus);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  gBS->FreePool (CleanLine);
  return EFI_SUCCESS;

}

EFI_STATUS
UpdateSystemBIOSInfo (
  VOID
  )
{
  INT32                                       BackupAttribute;
  CHAR16                                      CurrentSystem[BVDT_MAX_STR_SIZE];
  CHAR16                                      CurrentRevision[BVDT_MAX_STR_SIZE];
  UINTN                                       StrSize;
  CHAR16                                      *CurrentVendor = L" ";
  UINTN                                       LeftIdent;
  EFI_STATUS                                  Status;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));

  ZeroMem (CurrentSystem,   (BVDT_MAX_STR_SIZE));
  ZeroMem (CurrentRevision, (BVDT_MAX_STR_SIZE));

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, CurrentRevision);
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, CurrentSystem);

  LeftIdent = mLeftMargin + StrLen (SystemString);
  PrintAt (LeftIdent, mCurrentSystemInfoRow, L"%s", CurrentSystem);
  PrintAt (LeftIdent, mCurrentSystemInfoRow+1, L"%s", CurrentRevision);
  PrintAt (LeftIdent, mCurrentSystemInfoRow+2, L"%s", CurrentVendor);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);

  return EFI_SUCCESS;
}

EFI_STATUS
UpdatePowerStatus (
  VOID
  )
{
  return EFI_SUCCESS;

}

EFI_STATUS
UpdateBIOSUpgradeInfo (
  VOID
  )
{
  INT32                                       BackupAttribute;
  CHAR16                                      CurrentSystem[BVDT_MAX_STR_SIZE];
  CHAR16                                      CurrentRevision[BVDT_MAX_STR_SIZE];
  CHAR16                                      *CurrentVendor = L" ";
  UINTN                                       LeftIdent;
  UINTN                                       Index;
  BOOLEAN                                     BvdtFound;

  if (mBufferPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  BvdtFound = FALSE;
  for (Index = 0; Index < mBufferSize; Index++) {
     if (AsciiStrnCmp ((CHAR8*)(mBufferPtr + Index), "$BVDT$", 6) == 0) {
       BvdtFound = TRUE;
       break;
     }
  }

  if (!BvdtFound) {
    return EFI_NOT_FOUND;
  }
  AsciiStrToUnicodeStr ((CHAR8*)(mBufferPtr + Index + BIOS_VERSION_OFFSET), CurrentRevision);
  AsciiStrToUnicodeStr ((CHAR8*)(mBufferPtr + Index + PRODUCT_NAME_OFFSET), CurrentSystem);
  BackupAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));

  LeftIdent = mLeftMargin + StrLen (SystemString);
  PrintAt (LeftIdent, mBiosUpdateInfoRow, L"%s", CurrentSystem);
  PrintAt (LeftIdent, mBiosUpdateInfoRow+1, L"%s", CurrentRevision);
  PrintAt (LeftIdent, mBiosUpdateInfoRow+2, L"%s", CurrentVendor);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);

  return EFI_SUCCESS;
}

EFI_STATUS
InvalidFile (
  )
{
  INT32                                       BackupAttribute;
  EFI_STATUS                                  Status;
  UINTN                                       Columns;
  UINTN                                       Rows;
  UINTN                                       Index;
  UINTN                                       Item;
  CHAR16                                      *CleanLine;
  CHAR16                                      *InvalidFileString = L"Invalid File!";
  UINTN                                       InvalidFileStringLength;
  UINTN                                       NumberOfLines = 3;
  EFI_INPUT_KEY                               Key;

  InvalidFileStringLength = StrLen (InvalidFileString);
  CleanLine = AllocateZeroPool (((InvalidFileStringLength + 2) * 2 + 2) * sizeof (CHAR16));
  if (CleanLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < (InvalidFileStringLength + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
  }

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  BackupAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_MAGENTA));

  Item = NumberOfLines;
  Item = Item / 2;
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 - (Item + 3) + 2 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (InvalidFileStringLength + 2), Index, L"%s", CleanLine);
  }

  for (Index = Columns/2 - (InvalidFileStringLength + 1); Index < Columns/2 + (InvalidFileStringLength + 2) ; Index++) {
    PrintAt (Index, Rows/2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = Rows/2 - (Item + 2); Index < Rows/2 - (Item + 3) + 2 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (InvalidFileStringLength + 2), Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (InvalidFileStringLength + 2), Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (Columns/2 + (InvalidFileStringLength + 2) ,Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns/2 + (InvalidFileStringLength + 2) ,Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);

  PrintAt (Columns/2 - (InvalidFileStringLength + 2), Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns/2 - (InvalidFileStringLength + 2), Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  PrintAt (Columns/2 - (InvalidFileStringLength/2), Rows/2 - (Item + 3) + 2, L"%s", InvalidFileString);

  do {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  } while (Status != EFI_SUCCESS);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  gBS->FreePool (CleanLine);
  return EFI_SUCCESS;
}

EFI_STATUS
FocusUpgradeBios (
  BOOLEAN           Focus
  )
{
  INT32                                       BackupAttribute;
  BOOLEAN                                     GrayOut = TRUE;
  INT32                                       Attribute;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (mBufferPtr != NULL) {
    GrayOut = FALSE;
  }

  if (Focus) {
    Attribute = UI_FOCUS;
  } else {
    Attribute = UI_UNFOCUS;
    if (GrayOut) {
      Attribute = (EFI_TEXT_ATTR(EFI_DARKGRAY, EFI_BLUE));
    }
  }
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  PrintAt (mLeftMargin+4, mPowerStatusRow+2, L"[%s]", BeginFlashButton);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  return EFI_SUCCESS;
}

EFI_STATUS
FocusCancel (
  BOOLEAN           Focus
  )
{
  INT32                                       BackupAttribute;
  INT32                                       Attribute;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (Focus) {
    Attribute = UI_FOCUS;
  } else {
    Attribute = UI_UNFOCUS;
  }
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  PrintAt (mLeftMargin + 4 + StrLen(BeginFlashButton) + 10, mPowerStatusRow+2, L"[%s]", CancelButton);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  return EFI_SUCCESS;
}

EFI_STATUS
FocusCmdLine (
  BOOLEAN           Focus
  )
{
  INT32                                       BackupAttribute;
  INT32                                       Attribute;
  EFI_STATUS                                  Status;
  CHAR16                                      *CleanLine;
  UINTN                                       Index;
  UINTN                                       VariableSize;
  CHAR16                                      *FileNameString;
  CHAR16                                      *FilePathString;
  CHAR16                                      *CmdLineString;

  CmdLineString = AllocateZeroPool ((CMD_FILENAME_LENGTH + CMD_PARAM_LENGTH + 2) * sizeof (CHAR16));
  if (CmdLineString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CleanLine = AllocateZeroPool ((CMD_FILENAME_LENGTH + CMD_PARAM_LENGTH + 2) * sizeof (CHAR16));
  if (CleanLine == NULL) {
    gBS->FreePool (CmdLineString);
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < CMD_FILENAME_LENGTH + CMD_PARAM_LENGTH + 1; Index++) {
    CleanLine[Index] = 0x20;
  }

  FileNameString = NULL;
  VariableSize = 0;
  Status = gRT->GetVariable (
                  L"BiosUpdateFileName",
                  &gEfiGenericVariableGuid,
                  NULL,
                  &VariableSize,
                  FileNameString
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FileNameString = AllocatePool (VariableSize);
//[-start-140625-IB0508_klockwork-add]//
    if (FileNameString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
//[-end-140625-IB0508_klockwork-add]//
    Status = gRT->GetVariable (
                    L"BiosUpdateFileName",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VariableSize,
                    FileNameString
                    );
    if (EFI_ERROR(Status)) {
      gBS->FreePool (FileNameString);
      FileNameString = NULL;
    }
  }

  FilePathString = NULL;
  VariableSize = 0;
  Status = gRT->GetVariable (
                  L"BiosUpdateFilePath",
                  &gEfiGenericVariableGuid,
                  NULL,
                  &VariableSize,
                  FilePathString
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FilePathString = AllocatePool (VariableSize);
//[-start-140626-IB0508_klockwork-add]//
    if (FilePathString== NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
//[-end-140626-IB0508_klockwork-add]//
    Status = gRT->GetVariable (
                    L"BiosUpdateFilePath",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VariableSize,
                    FilePathString
                    );
    if (EFI_ERROR(Status)) {
      gBS->FreePool (FilePathString);
      FilePathString = NULL;
    }
  }

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (Focus) {
    Attribute = UI_FOCUS;
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  } else {
    Attribute = UI_UNFOCUS;
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  }
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);

  PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + 1, mUpdateFileNameRow, L"%s", CleanLine);
  if (FileNameString != NULL){
//[-start-140701-IB05080434-modify]//
    if (StrLen(FileNameString) >= CMD_FILENAME_LENGTH) {
      StrnCat (CmdLineString, FileNameString, CMD_FILENAME_LENGTH);
    } else {
      if (FilePathString != NULL){
        if (StrLen (FileNameString) + StrLen (FilePathString) > CMD_FILENAME_LENGTH) {
          StrnCat (CmdLineString, FilePathString, StrLen(FilePathString) - StrLen(FileNameString) - 1);
          StrCat (CmdLineString, L"\\");
          StrCat (CmdLineString, FileNameString);
        } else {
          StrCat (CmdLineString, FilePathString);
          StrCat (CmdLineString, FileNameString);
        }
      }
    }
//[-end-140701-IB05080434-modify]//
    PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + 1, mUpdateFileNameRow, L"%s", CmdLineString);
  }
  PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + StrLen(CmdLineString) + 2, mUpdateFileNameRow, L"%s", mCmdLineParameter);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  gBS->FreePool (CmdLineString);
  gBS->FreePool (FileNameString);
  gBS->FreePool (FilePathString);
  return EFI_SUCCESS;
}

EFI_STATUS
FocusFileSelect (
  BOOLEAN           Focus
  )
{
  INT32                                       BackupAttribute;
  INT32                                       Attribute;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (Focus) {
    Attribute = UI_FOCUS;
  } else {
    Attribute = UI_UNFOCUS;
  }
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);

  PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + CMD_FILENAME_LENGTH + CMD_PARAM_LENGTH + 3, mUpdateFileNameRow, L"[%s]", FileSelectButton);

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  return EFI_SUCCESS;
}

EFI_STATUS
CmdLineInputHandle (
  IN OUT EFI_INPUT_KEY                   *EventKey
  )
{
  INT32                                       BackupAttribute;
  EFI_STATUS                                  Status;
  UINTN                                       VariableSize;
  CHAR16                                      *FileNameString;
  CHAR16                                      *FilePathString;
  CHAR16                                      *CmdLineString;

  if (mBufferPtr == NULL) {
    return EFI_SUCCESS;
  }

  if ((EventKey->UnicodeChar == L'/') ||
      (EventKey->UnicodeChar == L'-') ||
      ((EventKey->UnicodeChar >= L'a') && (EventKey->UnicodeChar <= L'z')) ||
      ((EventKey->UnicodeChar >= L'A') && (EventKey->UnicodeChar <= L'Z')) ||
      (EventKey->UnicodeChar == CHAR_BACKSPACE) ||
      (EventKey->UnicodeChar == L' ')) {
    CmdLineString = AllocateZeroPool ((CMD_FILENAME_LENGTH + CMD_PARAM_LENGTH + 2) * sizeof (CHAR16));
    if (CmdLineString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    FileNameString = NULL;
    VariableSize = 0;
    Status = gRT->GetVariable (
                    L"BiosUpdateFileName",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VariableSize,
                    FileNameString
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      FileNameString = AllocateZeroPool (VariableSize);
//[-start-140625-IB0508_klockwork-add]//
      if (FileNameString == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
//[-end-140625-IB0508_klockwork-add]//
      Status = gRT->GetVariable (
                      L"BiosUpdateFileName",
                      &gEfiGenericVariableGuid,
                      NULL,
                      &VariableSize,
                      FileNameString
                      );
      if (EFI_ERROR(Status)) {
        gBS->FreePool (FileNameString);
        FileNameString = NULL;
        return Status;
      }
    }
    FilePathString = NULL;
    VariableSize = 0;
    Status = gRT->GetVariable (
                    L"BiosUpdateFilePath",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VariableSize,
                    FilePathString
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      FilePathString = AllocateZeroPool (VariableSize);
//[-start-140626-IB0508_klockwork-add]//
      if (FilePathString == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
//[-end-140626-IB0508_klockwork-add]//
      Status = gRT->GetVariable (
                      L"BiosUpdateFilePath",
                      &gEfiGenericVariableGuid,
                      NULL,
                      &VariableSize,
                      FilePathString
                      );
      if (EFI_ERROR(Status)) {
        gBS->FreePool (FilePathString);
        FilePathString = NULL;
        return Status;
      }
    }

    BackupAttribute = gST->ConOut->Mode->Attribute;
    gST->ConOut->SetAttribute (gST->ConOut, UI_FOCUS);
    if (FileNameString != NULL) {
//[-start-140701-IB05080434-modify]//
      if (StrLen(FileNameString) >= CMD_FILENAME_LENGTH) {
        StrnCat (CmdLineString, FileNameString, CMD_FILENAME_LENGTH);
      } else {
        if (FilePathString != NULL){
          if (StrLen (FileNameString) + StrLen (FilePathString) > CMD_FILENAME_LENGTH) {
            StrnCat (CmdLineString, FilePathString, StrLen(FilePathString) - StrLen(FileNameString) - 1);
            StrCat (CmdLineString, L"\\");
            StrCat (CmdLineString, FileNameString);
          } else {
            StrCat (CmdLineString, FilePathString);
            StrCat (CmdLineString, FileNameString);
          }
        }
      }
//[-end-140701-IB05080434-modify]//
    }

    if (EventKey->UnicodeChar == CHAR_BACKSPACE) {
      if (mCmdLineIndex > 0) {
        mCmdLineIndex--;
        PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + StrLen(CmdLineString) + 2 + mCmdLineIndex, mUpdateFileNameRow, L" ");
        mCmdLineParameter[mCmdLineIndex] = 0;
      }
    } else if (mCmdLineIndex < CMD_PARAM_LENGTH) {
      mCmdLineParameter[mCmdLineIndex] = EventKey->UnicodeChar;
      PrintAt (mLeftMargin+StrLen(BiosUpdateFileSring) + StrLen(CmdLineString) + 2, mUpdateFileNameRow, L"%s", mCmdLineParameter);
      mCmdLineIndex++;
    }

    gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
    gBS->FreePool (FileNameString);
    gBS->FreePool (FilePathString);

  }
  return EFI_SUCCESS;
}

EFI_STATUS
ConfirmBiosUpgrade (
  IN OUT EFI_INPUT_KEY                   *EventKey
  )
{
  INT32                                       BackupAttribute;
  EFI_STATUS                                  Status;
  UINTN                                       Columns;
  UINTN                                       Rows;
  UINTN                                       Index;
  UINTN                                       Item;
  CHAR16                                      *CleanLine;
  CHAR16                                      *WarningString = L"Warning!";
  CHAR16                                      *Description1 = L"During the update procedure, your system will restart.";
  CHAR16                                      *Description2 = L"Do not interrupt this procedure once it begins.";
  CHAR16                                      *Description3 = L"Do not disconnect the AC power source.";
  CHAR16                                      *Description4 = L"Interruption of the BIOS/firmware update procedure will";
  CHAR16                                      *Description5 = L"likely render your system unusable.";
  CHAR16                                      *Description6 = L"Do you want do proceed? (Y)es/(N)o";
  UINTN                                       DialogLength = 28;
  UINTN                                       NumberOfLines = 10;
  EFI_INPUT_KEY                               Key;

  CleanLine = AllocateZeroPool (((DialogLength + 2) * 2 + 2) * sizeof (CHAR16));
  if (CleanLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < (DialogLength + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
  }

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  BackupAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_MAGENTA));

  Item = NumberOfLines;
  Item = Item / 2;
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 - (Item + 3) + 2 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (DialogLength + 2), Index, L"%s", CleanLine);
  }

  for (Index = Columns/2 - (DialogLength + 1); Index < Columns/2 + (DialogLength + 2) ; Index++) {
    PrintAt (Index, Rows/2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = Rows/2 - (Item + 2); Index < Rows/2 - (Item + 3) + 2 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (DialogLength + 2), Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (DialogLength + 2), Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (Columns/2 + (DialogLength + 2) ,Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns/2 + (DialogLength + 2) ,Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);

  PrintAt (Columns/2 - (DialogLength + 2), Rows/2 - (Item + 3) + 2 + NumberOfLines, L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns/2 - (DialogLength + 2), Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  PrintAt (Columns/2 - (StrLen(WarningString)/2), Rows/2 - (Item + 3) + 2, L"%s", WarningString);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 4, L"%s", Description1);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 5, L"%s", Description2);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 6, L"%s", Description3);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 7, L"%s", Description4);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 8, L"%s", Description5);
  PrintAt (Columns/2 - (DialogLength), Rows/2 - (Item + 3) + 10, L"%s", Description6);

CheckKey:
  do {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  } while (Status != EFI_SUCCESS);

  if ((Key.UnicodeChar == L'Y') || (Key.UnicodeChar == L'y')) {
    Status = EFI_SUCCESS;
  } else if ((Key.UnicodeChar == L'N') || (Key.UnicodeChar == L'n')) {
    Status = EFI_NOT_READY;
  } else {
    goto CheckKey;
  }

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  gBS->FreePool (CleanLine);
  return Status;

}


EFI_STATUS
UnloadBiosFlashUI (
  IN EFI_HANDLE     ImageHandle
  )
{

  gBS->UninstallProtocolInterface (
          ImageHandle,
          &gBiosFlashUIProtocolGuid,
          NULL
          );

  return EFI_SUCCESS;
}

EFI_STATUS
LocateDriverIndex (
  EFI_GUID        *ProtocolGuid
  )
{
  EFI_STATUS      Status;
  UINTN           HandleCount;
  EFI_HANDLE      *HandleBuffer;
  UINTN           HIndex;
  UINTN           PIndex;
  UINTN           ProtocolCount;
  EFI_GUID        **ProtocolBuffer;
  VOID            *Interface;

  Status = gBS->LocateProtocol (ProtocolGuid, NULL, &Interface);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateHandleBuffer (
                AllHandles,
                NULL,
                NULL,
                &HandleCount,
                &HandleBuffer
                );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HIndex = HandleCount; HIndex > 0; HIndex--) {
    
    Status = gBS->ProtocolsPerHandle (HandleBuffer[HIndex-1], &ProtocolBuffer, &ProtocolCount);
    
    for (PIndex = 0; PIndex < ProtocolCount; PIndex++) {
      if(CompareGuid(ProtocolBuffer[PIndex], ProtocolGuid) == TRUE) {
        gBS->FreePool (ProtocolBuffer);
        gBS->FreePool (HandleBuffer);
        return EFI_SUCCESS;
      }
    }
    
    gBS->FreePool (ProtocolBuffer);
  }
  
  gBS->FreePool (HandleBuffer);
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
BiosFlashUIEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS             Status;
  //EFI_HANDLE             Handle;

  EFI_LOADED_IMAGE_PROTOCOL           *LoadedImage;
  BIOS_FLASH_UI_PROTOCOL              *BiosFlashUIProtocol;

  Status = LocateDriverIndex (&gBiosFlashUIProtocolGuid);

  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  // Register unload function
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (Status == EFI_SUCCESS) {
    LoadedImage->Unload = UnloadBiosFlashUI;
  }
  BiosFlashUIProtocol = (BIOS_FLASH_UI_PROTOCOL*)AllocatePool (sizeof (BIOS_FLASH_UI_PROTOCOL));
//[-start-140625-IB0508_klockwork-add]//
  if (BiosFlashUIProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB0508_klockwork-add]//
  BiosFlashUIProtocol->DrawBiosFlashUI = BiosFlashFrontPage;

  //
  // Install protocol
  //
  //Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gBiosFlashUIProtocolGuid,
                  BiosFlashUIProtocol,
                  NULL
                  );

  return Status;
}


