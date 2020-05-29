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

#include "OpenSaveFile.h"
#include "FileHandleLib.h"
#include "FileSelectUILib.h"
#include "FileConOut.h"
#include "FileTextOutHook.h"
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SetupUtility.h>
//	int                                                     FILE_UI_LX = 0;
//	int                                                     FILE_UI_LY = 0;
//	int                                                     WINDOW_LIMIT_HIGH = 25;
//	int                                                     WINDOW_LIMIT_WIDTH = 79;
//	#define                                                 WINDOW_LIMIT 79
//	#define FILE_UI_RX                                      (FILE_UI_LX + 48)
//	#define FILE_UI_RY                                      (FILE_UI_LY + 15)
//	#define FILE_NAME_LX                                    (FILE_UI_LX + 14)
//	#define FILE_NAME_RX                                    (FILE_UI_LX + 14 + 31)
//	#define FILE_NAME_Y                                     (FILE_UI_LY)
//	#define FILE_PATH_X                                     (FILE_UI_LX + 3)
//	#define FILE_PATH_Y                                     (FILE_UI_LY + 1)
//	#define DIR_X                                           (FILE_UI_LX + 3)
//	#define DIR_Y                                           (FILE_UI_LY + 1)
//	#define FILE_ITEM_LX                                    (FILE_UI_LX + 3)
//	#define FILE_ITEM_LY                                    (FILE_UI_LY + 4)
//	#define FILE_ITEM_RX                                    (FILE_UI_RX - 3)
//	#define FILE_ITEM_RY                                    (FILE_UI_RY - 2)
//	#define OK_BOX_X                                        (FILE_UI_LX + 4)
//	#define OK_BOX_Y                                        (FILE_UI_RY)
//	#define CANCEL_BOX_X                                    (FILE_UI_RX - 13)
//	#define CANCEL_BOX_Y                                    (FILE_UI_RY)
//
//
//	#define FILE_NAME_SIZE                                  (FILE_NAME_RX - FILE_NAME_LX + 1)
//	#define FILE_ITEM_SIZE                                  (FILE_ITEM_RY - FILE_ITEM_LY + 1)

#define MediaFloppy         0x01
#define MediaHardDisk       0x02
#define MediaCDRom          0x04
#define MediaUnknown        0x08
#define MediaTypeAll        (MediaFloppy | MediaHardDisk | MediaCDRom | MediaUnknown)


//[-start-140402-IB10300106-add]//
//#define DEFAULT_FILE_NAME   L"FlashUpdate.efi"
//[-end-140402-IB10300106-add]//

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                 *mFileSystemProtocol;

CHAR16  gStartUpBatch[1] = {0xfeff};
CHAR16  gStringBatch[199] = {0};

//[-start-140402-IB10300106-add]//
SIM_KEY_STATE KeyState       = STATE_SEARCH_DEVICE;
INT32         mCurrentDevice = 0;
BOOLEAN       mAutoLoad      = TRUE;
//[-end-140402-IB10300106-add]//

BOOLEAN
IsRemoveableDevice (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  );

EFI_STATUS
GetDiskFsNumber (
  UINTN                HandleIndex,
//  EFI_HANDLE           DiskHandle,
  UINT16               *FsNumber
  );

BOOLEAN
IsFiltered (
  EFI_FILE_INFO          **FileInfo
  );

//[-start-140402-IB10300106-add]//
EFI_STATUS
SearchDefaultFile (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      CHAR16                                        *FilePath,
  IN OUT  CHAR16                                        *FileName,
  OUT     EFI_FILE                                      **OperationFileHandle
  );

EFI_INPUT_KEY
SimKey(
  INT32             SelectNo, 
  INT32             EndNo, 
  FILE_HANDLE_NODE  *CurrentNode
  );
//[-end-140402-IB10300106-add]//

MODE_OF_FILE_UI                                   Mode;
BOOLEAN                                           Refresh;
INT32                                             PageStartNo;
UINTN                                             FileDepth;
VOID                                              *TextBuffer;
VOID                                              *GraphicBuffer;

  //
  // Columns --->
  // Rows
  // |
  // |
  // ¡õ
  //
int                                               WindowLeftColumn = 0;
int                                               WindowLeftRow = 0;

#define  MAX_NUMBER(a,b)                                ((a) > (b) ? (a) : (b))

BOOLEAN
ValidCharacter (
  IN      CHAR16                                        Character
  )
{
  if (((Character >= '0' && Character <= '9') ||
       (Character >= 'a' && Character <= 'z') ||
       (Character >= 'A' && Character <= 'Z') ||
        Character == '(' || Character == ')'  ||
        Character == '[' || Character == ']'  ||
        Character == '{' || Character == '}'  ||
        Character == '_' || Character == '-'  || Character == '.')) {
     return TRUE;
  } else {
    return FALSE;
  }
}


static
CHAR16
FileUIToLower (
  CHAR16  a
  )
{
  if ('A' <= a && a <= 'Z') {
    return (CHAR16) (a + 0x20);
  } else {
    return a;
  }
}

INTN
FileUIStriCmp (
  IN CHAR16 *s1,
  IN CHAR16 *s2
  )
{
  while (*s1) {
    if (FileUIToLower(*s1) != FileUIToLower(*s2)) {
      break;
    }

    s1 += 1;
    s2 += 1;
  }

  return FileUIToLower(*s1) - FileUIToLower(*s2);
}


EFI_STATUS
FileUIWaitForSingleEvent (
  EFI_EVENT  Event,
  UINT64     Timeout OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];

  if (Timeout != 0) {
    //
    // Create a timer event
    //
    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    if (!EFI_ERROR (Status)) {
      //
      // Set the timer event
      //
      gBS->SetTimer (
             TimerEvent,
             TimerRelative,
             Timeout
             );

      //
      // Wait for the original event or the timer
      //
      WaitList[0] = Event;
      WaitList[1] = TimerEvent;
      Status      = gBS->WaitForEvent (2, WaitList, &Index);
      gBS->CloseEvent (TimerEvent);

      //
      // If the timer expired, change the return to timed out
      //
      if (!EFI_ERROR (Status) && Index == 1) {
        Status = EFI_TIMEOUT;
      }
    }
  } else {
    //
    // No timeout... just wait on the event
    //
    Status = gBS->WaitForEvent (1, &Event, &Index);
  }

  return Status;
}


EFI_INPUT_KEY
WaitForAKey (
  IN UINT64                                             Timeout OPTIONAL
  )
{
  EFI_INPUT_KEY                              Key;

  FileUIWaitForSingleEvent (gST->ConIn->WaitForKey, Timeout);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

  return Key;
}


VOID
FileUISetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
/*++

Routine Description:

  Set Buffer to Value for Size bytes.

Arguments:

  Buffer  - Memory to set.

  Size    - Number of bytes to set

  Value   - Value of the set operation.

Returns:

  None

--*/
{
  CHAR16  *Ptr;

  Ptr = Buffer;
  while (Size--) {
    *(Ptr++) = Value;
  }
}


VOID
FileUIClearScreen (
  int lx,
  int ly,
  int rx,
  int ry,
  int Color
  )
{
  CHAR16 Buffer[90];
  int iRow = 0;
  int Len;

  gBS->SetMem (Buffer, sizeof (Buffer), 0);
  if (lx <= 0 || ly <= 0 || rx < lx || ry < ly)
    return;

  Len = rx-lx+1;
//	  if (Len > WINDOW_LIMIT_WIDTH || (lx+Len-1) > WINDOW_LIMIT_WIDTH)
//	    Len = WINDOW_LIMIT_WIDTH - lx + 1;

  FileUISetUnicodeMem (Buffer, Len, L' ');
  gST->ConOut->SetAttribute (gST->ConOut, Color);
  gST->ConOut->Mode->Attribute = gST->ConOut->Mode->Attribute & 0x7f;
  gST->ConOut->SetAttribute (gST->ConOut, gST->ConOut->Mode->Attribute);

  iRow = ly-1;
  do  {
    gST->ConOut->SetCursorPosition (gST->ConOut, lx-1, iRow);
    gST->ConOut->OutputString (gST->ConOut, Buffer);
    iRow++;
  } while (iRow < ry);
}


VOID
FileUIDisplayString (
  int x,
  int y,
  CHAR16 *String,
  UINTN Len,
  int Color)
{
  CHAR16 *Text;

  if (Len <= 0 || x < 1 || y < 1 || String == NULL)
    return;

  Text = (CHAR16*) AllocateZeroPool (StrSize(String));
//[-start-140624-IB05080432-add]//
  if (Text == NULL)
    return;
//[-end-140624-IB05080432-add]//
  StrCpy (Text, String);

  gST->ConOut->SetAttribute (gST->ConOut, Color);

  //
  // Set to narrow (not wide) characters.  Set attributes and strip it and print it
  //
  gST->ConOut->Mode->Attribute = gST->ConOut->Mode->Attribute & 0x7f;
  gST->ConOut->SetAttribute (gST->ConOut, gST->ConOut->Mode->Attribute);

  gST->ConOut->SetCursorPosition (gST->ConOut, x-1, y-1);
  gST->ConOut->OutputString (gST->ConOut, Text);
  FreePool (Text);
}


VOID
FileUIDisplayErrorMsg (
  CHAR16 *Message
  )
{
  int lx, ly, rx;
  int MaxStrLen;

  UINTN                                         Columns;
  UINTN                                         Rows;

  ly = WindowLeftRow + 3;



  //
  //<--------- Columns -------->
  // ¡ô
  // |
  // |
  // Rows
  // |
  // |
  // ¡õ
  //
  FcoQueryMode (&Columns, &Rows);


  MaxStrLen = (int)StrLen (Message) + 1;

  if (MaxStrLen < 28) {
    MaxStrLen = 29;
  }

  lx = (int)((Columns - MaxStrLen) / 2) - 1;
  rx = lx + MaxStrLen;

  FileUIClearScreen (lx, ly, rx, ly, FILE_UI_TITLE_COLOR);
  FileUIClearScreen (lx, ly+1, rx, ly+3, FILE_UI_ERROR_BODY_COLOR);
  FileUIDisplayString (lx+(MaxStrLen-7)/2, ly, L" Error ", 7, FILE_UI_TITLE_COLOR);
  FileUIDisplayString (lx+1, ly+1, Message, StrLen (Message), FILE_UI_ERROR_BODY_COLOR);
  FileUIDisplayString (lx+1, ly+3, L"Press any key to continue...", 28, FILE_UI_ERROR_BODY_COLOR);

  WaitForAKey (0);

  FileUIClearScreen (lx, ly, rx, ly+3, FILE_UI_BODY_COLOR);

}


BOOLEAN
FileUICheckBox (
  CHAR16 *Message
  )
{
  int lx, ly, rx;
  int MaxStrLen;
  EFI_INPUT_KEY                                         Key = {0};
  BOOLEAN                                               IsOK = TRUE;

  ly = 11;
  MaxStrLen = (int)StrLen (Message) + 1;

  if (MaxStrLen < 28) {
    MaxStrLen = 29;
  }

  lx = ((80 - MaxStrLen) / 2) - 1;
  rx = lx + MaxStrLen;

  FileUIClearScreen (lx, ly, rx, ly, FILE_UI_TITLE_COLOR);
  FileUIClearScreen (lx, ly+1, rx, ly+4, FILE_UI_WARNING_BODY_COLOR);
  FileUIDisplayString (lx+(MaxStrLen-7)/2, ly, L" Warning ", 9, FILE_UI_TITLE_COLOR);
  FileUIDisplayString (lx+1, ly+1, Message, StrLen (Message), FILE_UI_WARNING_BODY_COLOR);

  FileUIDisplayString (lx+1, ly+3, L"[  OK  ]", (int)8, FILE_UI_TITLE_COLOR);
  FileUIDisplayString (rx-10, ly+3, L"  Cancel  ", (int)10, FILE_UI_TITLE_COLOR);

  while (TRUE) {
    Key = WaitForAKey (0);

    switch (Key.ScanCode) {

    case SCAN_LEFT:
      FileUIDisplayString (lx+1, ly+3, L"[  OK  ]", (int)8, FILE_UI_TITLE_COLOR);
      FileUIDisplayString (rx-10, ly+3, L"  Cancel  ", (int)10, FILE_UI_TITLE_COLOR);
      IsOK = TRUE;
      break;

    case SCAN_RIGHT:
      FileUIDisplayString (lx+1, ly+3, L"   OK   ", (int)8, FILE_UI_TITLE_COLOR);
      FileUIDisplayString (rx-10, ly+3, L"[ Cancel ]", (int)10, FILE_UI_TITLE_COLOR);
      IsOK = FALSE;
      break;

    default:
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        FileUIClearScreen (lx, ly, rx, ly+4, FILE_UI_BODY_COLOR);
        return IsOK;
      }
      break;
    }
  }

}


EFI_STATUS
IsFileExists (
  IN CONST CHAR16                                       *Name,
  IN       EFI_LIST_ENTRY                               *DirListHead,
  IN       EFI_LIST_ENTRY                               *FileListHead,
  OUT      BOOLEAN                                      *IsDir
  )
{
	EFI_LIST_ENTRY                                        *FLink = NULL;
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;

  if (Name == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // search Dirlist
  //
  FLink = DirListHead->ForwardLink;
  while (FLink != DirListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
    if (StrnCmp (CurrentNode->Name, Name, MAX_NUMBER(StrLen(CurrentNode->Name), StrLen(Name))) == 0) {
      *IsDir = TRUE;
      return EFI_SUCCESS;
    }
    FLink = FLink->ForwardLink;
  }

  //
  // search Filelist
  //
  FLink = FileListHead->ForwardLink;
  while (FLink != FileListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);

    if (StrnCmp (CurrentNode->Name, Name, MAX_NUMBER(StrLen(CurrentNode->Name), StrLen(Name))) == 0) {
      *IsDir = FALSE;
      return EFI_SUCCESS;
    }
    FLink = FLink->ForwardLink;
  }

  return EFI_NOT_FOUND;
}


UINT32
GetEndNo (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead
  )
{
  UINT32                                                EndNo = 0;
  FILE_HANDLE_NODE                                       *EndNode = NULL;


  //
  // file always behind at directory.
  // so check the number of end node of file first.
  //
  if (!IsListEmpty (FileListHead)) {
    EndNode = _CR (FileListHead->BackLink, FILE_HANDLE_NODE, FilePtr);
    EndNo = EndNode->No;

  } else if (!IsListEmpty (DirListHead)) {
    EndNode = _CR (DirListHead->BackLink, FILE_HANDLE_NODE, FilePtr);
    EndNo = EndNode->No;
  }

  return EndNo;
}


FILE_HANDLE_NODE *
GetFileNode (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      UINT32                                        No,
  OUT     BOOLEAN                                       *IsDir
  )
{
  FILE_HANDLE_NODE                                       *CurrentNode = NULL;
  EFI_LIST_ENTRY                                        *FLink = NULL;

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    return NULL;
  }

  //
  // search Dirlist
  //
  FLink = DirListHead->ForwardLink;
  while (FLink != DirListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);

    if (CurrentNode->No == (INT32)No) {
      *IsDir = TRUE;
      return CurrentNode;
    }
    FLink = FLink->ForwardLink;
  }

  //
  // search Filelist
  //
  FLink = FileListHead->ForwardLink;
  while (FLink != FileListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);

    if (CurrentNode->No == (INT32)No) {
      *IsDir = FALSE;
      return CurrentNode;
    }
    FLink = FLink->ForwardLink;
  }

  return NULL;
}


FILE_HANDLE_NODE *
SearchListByCharacter (
  IN      EFI_LIST_ENTRY                                *ListHead,
  IN      CHAR16                                        Character
  )
{
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;
  EFI_LIST_ENTRY                                        *FLink = NULL;

  FLink = ListHead->ForwardLink;
  while (FLink != ListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
    if (FileUIToLower(CurrentNode->Name[0]) == FileUIToLower(Character)) {
      return CurrentNode;
    }
    FLink = FLink->ForwardLink;
  }

  return NULL;
}


EFI_STATUS
SearchNextNodeByCharacter (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      CHAR16                                        Character,
  IN      FILE_HANDLE_NODE                              **SearchNode
  )
{
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;
  EFI_LIST_ENTRY                                        *FLink = NULL;

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    return EFI_NOT_FOUND;
  }

  if (*SearchNode == NULL) {
    //
    // search Dirlist
    //
    CurrentNode = SearchListByCharacter (DirListHead, Character);
    if (CurrentNode != NULL) {
      *SearchNode = CurrentNode;
      return EFI_SUCCESS;
    }

    //
    // search Filelist
    //
    CurrentNode = SearchListByCharacter (FileListHead, Character);
    if (CurrentNode != NULL) {
      *SearchNode = CurrentNode;
      return EFI_SUCCESS;
    }
  } else {

    FLink = (*SearchNode)->FilePtr.ForwardLink;
    while (FLink != &((*SearchNode)->FilePtr)) {

      //
      //Transform search file operation from folder list to file list or from file list to folder list.
      //
      if (FLink == DirListHead) {
        CurrentNode = SearchListByCharacter (FileListHead, Character);
        if (CurrentNode != NULL) {
          *SearchNode = CurrentNode;
          return EFI_SUCCESS;
        } else {
          FLink = FLink->ForwardLink;
        }
      } else if (FLink == FileListHead) {
        CurrentNode = SearchListByCharacter (DirListHead, Character);
        if (CurrentNode != NULL) {
          *SearchNode = CurrentNode;
          return EFI_SUCCESS;
        } else {
          FLink = FLink->ForwardLink;
        }
      }

      CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
      if (FileUIToLower(CurrentNode->Name[0]) == FileUIToLower(Character)) {
        *SearchNode = CurrentNode;
        return EFI_SUCCESS;
      }
      FLink = FLink->ForwardLink;
    }

  }

  return EFI_NOT_FOUND;
}


VOID
UpdateFileFrame (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      UINT32                                        StartNo
  )
{
  CHAR16                                                buf[80] = {0};
  UINT32                                                EndNo = 0;
  UINT32                                                NoIndex = 0;
  UINT32                                                Count = 0;
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;
  BOOLEAN                                               IsDir = FALSE;

  FileUIClearScreen ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET), (WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET), FILE_UI_BODY_COLOR);

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    //
    // clean file frame
    //
    return;
  }

  EndNo = GetEndNo (DirListHead, FileListHead);

  for (NoIndex = StartNo, Count = 0; NoIndex <= EndNo && Count < (UINT32)FILE_ITEM_SIZE; NoIndex++) {

    CurrentNode = GetFileNode (DirListHead, FileListHead, NoIndex, &IsDir);
    if (CurrentNode == NULL) {
      continue;
    }
    if (IsDir) {
      UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
    } else {
      UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
    }
    FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + Count, buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
    Count++;
  }
}


VOID
UpdateFilePath (
  IN      CHAR16                                        *FilePath
  )
{
  CHAR16                                                buf[80] = {0};
  UINT8                                                 Index = 0;

  FileUIDisplayString ((WindowLeftColumn + FILE_PATH_WIDTH_OFFSET), (WindowLeftRow + FILE_PATH_HEIGHT_OFFSET), L"                                              ", 46, FILE_UI_BODY_COLOR);
  if (StrLen (FilePath) >= 32) {

    for (Index = 0; FilePath[Index]; Index++) {
      buf[Index] = FilePath[Index];
      if (buf[Index] == L':') {
        break;
      }
    }

    buf[Index] = L'.';
    buf[Index + 1] = L'.';
    buf[Index + 2] = L'.';
    buf[Index + 3] = L'\0';
    FileUIDisplayString ((WindowLeftColumn + FILE_PATH_WIDTH_OFFSET), (WindowLeftRow + FILE_PATH_HEIGHT_OFFSET), buf, (int)(StrLen (buf)), FILE_UI_BODY_COLOR);
    return;
  }

  FileUIDisplayString ((WindowLeftColumn + FILE_PATH_WIDTH_OFFSET), (WindowLeftRow + FILE_PATH_HEIGHT_OFFSET), FilePath, (int)(StrLen (FilePath)), FILE_UI_BODY_COLOR);

}


VOID
UpdateScrollBar (
  IN      INT32                                         NumberItems,
  IN      INT32                                         PageBaseOffset,
  IN      INT32                                         PageSize
  )
{
  INT32                                                 NumberPages = 0;
  INT32                                                 Row = 0;
  INT32                                                 Index = 0;
  INT8                                                  BarIndex = 0;
  INT8                                                  BarSize = 0;
  CHAR16                                                buf[80] = {0};
//[-start-140516-IB10300111-add]//
  CHAR16                                                ArrowString[4]= {0};
//[-end-140516-IB10300111-add]//

  NumberPages = (NumberItems / PageSize);
  if ((NumberItems % PageSize) != 0) {
    NumberPages++;
  }

  if (NumberPages <= 1) {
    //
    // 2.Draw two side
    //
    for (Index = (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET); Index <= (WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET); Index++) {
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_VERTICAL);
      FileUIDisplayString (((WindowLeftColumn + FILE_UI_WIDTH) - 2), Index, buf, (int)1, FILE_UI_BODY_COLOR);
    }
    FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) - 1), L"    ", (int)4, FILE_UI_BODY_COLOR);
    FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET) + 1), L"    ", (int)4, FILE_UI_BODY_COLOR);
    UnicodeSPrint (buf, 80, L"%c%c", BOXDRAW_HORIZONTAL ,BOXDRAW_DOWN_LEFT);
    FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) - 1), buf, (int)2, FILE_UI_BODY_COLOR);
    UnicodeSPrint (buf, 80, L"%c%c",BOXDRAW_HORIZONTAL ,BOXDRAW_UP_LEFT);
    FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET) + 1), buf, (int)2, FILE_UI_BODY_COLOR);

    return;
  }

  //
  // calculate BarSize
  //

  BarSize = (INT8)(PageSize - ((NumberItems - PageSize + 2) * PageSize) / NumberItems);

  Row = (PageBaseOffset * PageSize)/ NumberItems;


  //
  // 2.Draw right side
  //

//[-start-140516-IB10300111-modify]//
  //
  // L" ¡ô "
  //
  ArrowString[0] = L' ';
  ArrowString[1] = 0x2191;
  ArrowString[2] = L' ';
  ArrowString[3] = L'\0';
  FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) - 1), ArrowString, (int)(StrLen(ArrowString)), FILE_UI_SCROLL_BAR_COLOR);

  //
  // L" ¡õ "
  //
  ArrowString[0] = L' ';
  ArrowString[1] = 0x2193;
  ArrowString[2] = L' ';
  ArrowString[3] = L'\0';
  FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET), ((WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET) + 1), ArrowString, (int)(StrLen(ArrowString)), FILE_UI_SCROLL_BAR_COLOR);
//[-end-140516-IB10300111-modify]//
  for (Index = (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET); Index <= (WindowLeftRow + FILE_ITEM_R_HEIGHT_OFFSET);) {
    if (Row == Index - (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET)) {
      for (BarIndex = 0; BarIndex < BarSize; BarIndex++) {
        FileUIDisplayString (((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET) + 1),Index, L" ", (int)1, FILE_UI_SCROLL_BAR_COLOR);
        Index++;
      }
    } else {
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_VERTICAL);
      FileUIDisplayString (((WindowLeftColumn + FILE_ITEM_R_WIDTH_OFFSET) + 1), Index, buf, (int)1, FILE_UI_BODY_COLOR);
      Index++;
    }

  }

}


VOID
RecoveryItem (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      INT32                                         SelectNo
  )
{
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;
  BOOLEAN                                               IsDir = FALSE;
  CHAR16                                                buf[80] = {0};


  CurrentNode = GetFileNode (DirListHead, FileListHead, SelectNo, &IsDir);
//[-start-140624-IB05080432-add]//
  if (CurrentNode == NULL) {
    return;
  }
//[-end-140624-IB05080432-add]//
  if (IsDir) {
    UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
  } else {
    UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
  }

  FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_BODY_COLOR);
  FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
}


VOID
RemoveFileList (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead
  )
{

  FILE_HANDLE_NODE                                      *FileNode = NULL;

  //
  //Remove node until the list is empty (directory)
  //
	while (!IsListEmpty(DirListHead)) {
    FileNode = _CR (DirListHead->ForwardLink, FILE_HANDLE_NODE, FilePtr);
    RemoveEntryList (&(FileNode->FilePtr));
    //
    //free space
    //
    gBS->FreePool ((VOID*)FileNode->Name);
    gBS->FreePool ((VOID*)FileNode->Info);
    gBS->FreePool ((VOID*)FileNode);
  }

  //
  //Remove node until the list is empty (file)
  //
	while (!IsListEmpty(FileListHead)) {
    FileNode = _CR (FileListHead->ForwardLink, FILE_HANDLE_NODE, FilePtr);
    RemoveEntryList (&(FileNode->FilePtr));
    //
    //free space
    //
    gBS->FreePool ((VOID*)FileNode->Name);
    gBS->FreePool ((VOID*)FileNode->Info);
    gBS->FreePool ((VOID*)FileNode);
  }
}


//[-start-140624-IB05080432-modify]//
EFI_STATUS
SetNoAndPage (
  OUT     EFI_LIST_ENTRY                                *DirListHead,
  OUT     EFI_LIST_ENTRY                                *FileListHead
  )
{
  INT32                                                 No = 1;
  EFI_LIST_ENTRY                                        *FLink = NULL;
  FILE_HANDLE_NODE                                      *CurrentNode = NULL;

  //
  // directory
  //
  FLink = DirListHead;
  FLink = FLink->ForwardLink;
  while (FLink != DirListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
    CurrentNode->No = No;
    FLink = FLink->ForwardLink;
    No++;
  }

  //
  // file
  //
  FLink = FileListHead;
  FLink = FLink->ForwardLink;
  while (FLink != FileListHead) {
    CurrentNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
    CurrentNode->No = No;
    FLink = FLink->ForwardLink;
    No++;
  }

  if (FileDepth > 0) {
    CurrentNode = AllocateZeroPool (sizeof(FILE_HANDLE_NODE));
    if (CurrentNode == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CurrentNode->Name = L"..";
    CurrentNode->No = 0;
    InsertHeadList (DirListHead, &(CurrentNode->FilePtr));
  }
  return EFI_SUCCESS;
}
//[-end-140624-IB05080432-modify]//



EFI_STATUS
GenRootFileHandleList (
  EFI_LIST_ENTRY                                  *ListHead
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                           HandleCount;
  UINTN                           Index;
  EFI_HANDLE                      ParentHandle;
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;
  FILE_HANDLE_NODE               *NodePtr = NULL;
  CHAR16                         *NamePtr = NULL;
  UINT32                          NodeNum = 1;
  CHAR16                          TempName[80] = {0};
  EFI_FILE                       *FileHandle;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  UINT16                         FsNumber = 0;

  Status = gBS->LocateHandleBuffer (
                            ByProtocol,
                            &gEfiSimpleFileSystemProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer
                          );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Handle that with Blockio protocol is recordable device
  //
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Dump Root FileHandle List: \n"));
  for (Index=0; Index < HandleCount; Index++) {
    Status = FsuGetParentHandle (HandleBuffer[Index], &ParentHandle);
    if (EFI_ERROR(Status)){
      continue;
    }

    Status = gBS->HandleProtocol (ParentHandle, &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR(Status)){
      continue;
    }

    if (IsRemoveableDevice(DevicePath) == FALSE) {
      continue;
    }

    Status = gBS->HandleProtocol (ParentHandle, &gEfiBlockIoProtocolGuid, (VOID **)&BlkIo);
    if (EFI_ERROR(Status)){
      continue;
    } else {
      if (BlkIo->Media->ReadOnly){
        continue;
      } else {
        NodePtr = AllocateZeroPool (sizeof(FILE_HANDLE_NODE));
        if (NodePtr == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        NodePtr->Handle = HandleBuffer[Index];
        ZeroMem(TempName, 80);
        GetDiskFsNumber (Index, &FsNumber);
        UnicodeSPrint (TempName + StrLen(TempName), 80 - StrSize(TempName), L"fs%d", FsNumber);

        FREE(NamePtr);
        NodePtr->Name = AllocateZeroPool (StrSize(TempName));
        if (NodePtr->Name == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        StrCpy (NodePtr->Name, TempName);
        Status = FsuGetFileHandleByHandle(NodePtr->Handle, &FileHandle);
        if (Status == EFI_SUCCESS) {
          NodePtr->Info = FsuFileHandleGetInfo(FileHandle);
        } else {
          NodePtr->Info = NULL;
        }
        FileHandle->Close(FileHandle);

        NodePtr->No = NodeNum;
        NodeNum++;
        InsertTailList (ListHead, &(NodePtr->FilePtr));
        DEBUG (\
          (EFI_D_INFO | EFI_D_ERROR, "%d \t %s \n",\
          NodePtr->No,\
          NodePtr->Name)\
          );
      }
    }
  }

  //
  //It should always success,
  //because After IsExistRecordableDevice() and it exist recordable device
  //
  if (IsListEmpty(ListHead)) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }

}

VOID
GetRootHandleFromList (
  IN      FILE_HANDLE_NODE                              *CurrentNode,
  IN      EFI_LIST_ENTRY                                *ListHead,
  OUT     EFI_HANDLE                                    *Handle
  )
{

  FILE_HANDLE_NODE                                      *FileNode = NULL;
  EFI_LIST_ENTRY                                        *FLink;

  FLink = ListHead->ForwardLink;
  while (FLink != ListHead) {
    FileNode = _CR (FLink, FILE_HANDLE_NODE, FilePtr);
    if (StrnCmp (CurrentNode->Name, FileNode->Name, MAX_NUMBER(StrLen(CurrentNode->Name), StrLen(FileNode->Name))) == 0) {
      *Handle = FileNode->Handle;
      return;
    }
    FLink = FLink->ForwardLink;
  }

  *Handle = NULL;
}

BOOLEAN
IsFileSttributeVaild (
  UINT64                                          Attribute
  )
{
  if (Attribute == 0) {
    return FALSE;
  }

  if (Attribute & (~EFI_FILE_VALID_ATTR)) {
    return FALSE;
  }

  return TRUE;
}

//[-start-140624-IB05080432-modify]//
EFI_STATUS
FillListByDirFileHandle (
  IN      EFI_FILE                                      *OperationFileHandle,
  OUT     EFI_LIST_ENTRY                                *DirListHead,
  OUT     EFI_LIST_ENTRY                                *FileListHead
  )
{
  BOOLEAN                                         NoFile;
  EFI_STATUS                                      Status;
  EFI_FILE_INFO                                   *FileInfo;
  FILE_HANDLE_NODE                                *NodePtr;

  NoFile = FALSE;


  for ( Status = FsuFileHandleFindFirstFile(OperationFileHandle, &FileInfo, &NoFile)
      ; !EFI_ERROR(Status) && !NoFile
      ; Status = FsuFileHandleFindNextFile(OperationFileHandle, FileInfo, &NoFile)
     ){
    if (FileInfo != NULL) {
      if (IsFileSttributeVaild(FileInfo->Attribute)) {
        //
        //filter the folder name with ".." and "."
        //
        if ((StrCmp (FileInfo->FileName , L"..") != 0) && (StrCmp (FileInfo->FileName , L".") != 0)) {
          //
          // filter un-wanted file
          //
          if (IsFiltered (&FileInfo)) {
            continue;
          }
          NodePtr = AllocateZeroPool (sizeof(FILE_HANDLE_NODE));
          if (NodePtr == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          NodePtr->Name = AllocateZeroPool (StrSize(FileInfo->FileName));
          if (NodePtr->Name == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          StrCpy (NodePtr->Name, FileInfo->FileName);
          NodePtr->Info = FileInfo;
          if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0) {
            InsertTailList (DirListHead, &(NodePtr->FilePtr));
          } else {
            InsertTailList (FileListHead, &(NodePtr->FilePtr));
          }
        }
      }
    }
  }
  return EFI_SUCCESS;
}
//[-end-140624-IB05080432-modify]//

EFI_STATUS
GenFileList (
  IN      CHAR16                                        *FilePath,
  IN      FILE_HANDLE_NODE                              *CurrentNode,
  OUT     EFI_LIST_ENTRY                                *DirListHead,
  OUT     EFI_LIST_ENTRY                                *FileListHead,
  OUT     EFI_FILE                                      **OperationFileHandle
  )
{
  EFI_STATUS                                            Status = EFI_SUCCESS;

//	  CHAR16                                                FullNameArg[200] = {0};
  EFI_HANDLE                                            RootHandle;
  BOOLEAN                                               IsReturnMode;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                       *FileSystemProtocol;
  EFI_FILE                                              *NewHandle;

//[-start-140624-IB05080432-add]//
  if (CurrentNode == NULL) {
    return EFI_NOT_FOUND;
  }
//[-end-140624-IB05080432-add]//

  IsReturnMode = (StrnCmp (CurrentNode->Name, L"..", MAX_NUMBER(StrLen(CurrentNode->Name), StrLen(L".."))) == 0);

  if (IsReturnMode) {
    FileDepth--;
  } else {
    FileDepth++;
  }

  if (FileDepth > 1) {
    Status = (*OperationFileHandle)->Open(*OperationFileHandle, &NewHandle, CurrentNode->Name, (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE) , EFI_FILE_DIRECTORY);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    (*OperationFileHandle)->Close ((*OperationFileHandle));
    *OperationFileHandle = NewHandle;
//[-start-140624-IB05080432-modify]//
    Status = FillListByDirFileHandle(*OperationFileHandle, DirListHead, FileListHead);
    if (EFI_ERROR(Status)) {
      return Status;
    }
//[-end-140624-IB05080432-modify]//
  } else if (FileDepth == 1) {
    if (IsReturnMode) {
      Status = (*OperationFileHandle)->Open(*OperationFileHandle, &NewHandle, CurrentNode->Name, (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE) , EFI_FILE_DIRECTORY);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      (*OperationFileHandle)->Close ((*OperationFileHandle));
      *OperationFileHandle = NewHandle;
//[-start-140624-IB05080432-modify]//
      Status = FillListByDirFileHandle(*OperationFileHandle, DirListHead, FileListHead);
      if (EFI_ERROR(Status)) {
        return Status;
      }
//[-end-140624-IB05080432-modify]//
    } else {
      GetRootHandleFromList(CurrentNode, DirListHead, &RootHandle);
      if (RootHandle == NULL) {
        return EFI_ABORTED;
      }
      gBS->HandleProtocol (RootHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystemProtocol);
      //
      //Opens the root directory on a volume
      //
      Status = FileSystemProtocol->OpenVolume (
                                     FileSystemProtocol,
                                     OperationFileHandle
                                     );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      mFileSystemProtocol = FileSystemProtocol;

      RemoveFileList (DirListHead, FileListHead);
//[-start-140624-IB05080432-modify]//
      Status = FillListByDirFileHandle(*OperationFileHandle, DirListHead, FileListHead);    
      if (EFI_ERROR(Status)) {
        return Status;
      }
//[-end-140624-IB05080432-modify]//
    }
  } else {
    GenRootFileHandleList(DirListHead);
  }

//[-start-140624-IB05080432-modify]//
  Status = SetNoAndPage (DirListHead, FileListHead);
  if (EFI_ERROR(Status)) {
    return Status;
  }
//[-end-140624-IB05080432-modify]//
  return EFI_SUCCESS;
}


EFI_STATUS
ReGenFileList (
  IN      CHAR16                                        *FilePath,
  IN      FILE_HANDLE_NODE                              *CurrentNode,
  OUT     EFI_LIST_ENTRY                                *DirListHead,
  OUT     EFI_LIST_ENTRY                                *FileListHead,
  OUT     EFI_FILE                                      **OperationFileHandle
  )
{
  if (FileDepth > 0) {
    RemoveFileList (DirListHead, FileListHead);
  }
  return GenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle);
}


EFI_STATUS
InputFileName (
  IN OUT  CHAR16                                        *FileName
  )
{
  EFI_INPUT_KEY                                         Key = {0};
  UINT8                                                 Index = 0;
  UINT8                                                 StringLen = 0;

  StringLen = (UINT8)StrLen (FileName);

  gST->ConOut->SetCursorPosition (gST->ConOut, (WindowLeftColumn + FILE_NAME_L_OFFSET) + StringLen - 1, WindowLeftRow - 1);

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  Index = StringLen;

  while (TRUE) {
    gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_CURSOR_COLOR);

    //
    //wait for inputting a key
    //
    Key = WaitForAKey (0);

    if ((Index < FILE_NAME_SIZE) && ValidCharacter (Key.UnicodeChar)) {

      gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_BODY_COLOR);
	    Print (L"%c", Key.UnicodeChar);
      FileName[Index] = Key.UnicodeChar;
      Index++;

    } else if ((Key.UnicodeChar == CHAR_BACKSPACE) && (Index != 0)) {
      gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_BODY_COLOR);
      Print (L"\b \b");
      FileName[Index - 1] = '\0';
      Index--;

    } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      FileName [Index] = '\0';
      return EFI_SUCCESS;

    } else if (Key.ScanCode == SCAN_ESC) {
      FileName [Index] = '\0';
      return EFI_ABORTED;

    } else if (Key.UnicodeChar == CHAR_TAB || Key.ScanCode == ALT_F || Key.ScanCode == SCAN_DOWN) {
      FileName [Index] = '\0';
      Mode = Mode_SelectFile;
      return EFI_NOT_READY;
    }

  }

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
}


EFI_STATUS
SelectFile (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      CHAR16                                        *FilePath,
  IN OUT  CHAR16                                        *FileName,
  OUT     EFI_FILE                                      **OperationFileHandle
  )
{
  EFI_STATUS                                      Status = EFI_SUCCESS;
  INT32                                           SelectNo = 0;
  INT32                                           LastSelectNo = 0xffffffff;
  FILE_HANDLE_NODE                                *SearchNode = NULL;
  INT32                                           StartNo = 0;
  INT32                                           EndNo = 0;
  BOOLEAN                                         IsNeedRefreshFileFrame = FALSE;
  BOOLEAN                                         RefreshDir = TRUE;
  BOOLEAN                                         OnePage = TRUE;
  FILE_HANDLE_NODE                                *CurrentNode = NULL;
  BOOLEAN                                         IsDir = FALSE;
  INT32                                           RelativeIndex = 0;
  EFI_INPUT_KEY                                   Key = {0};
  CHAR16                                          buf[80] = {0};
  CHAR16                                          TempFilePath[200] = {0};
  INT32                                           Index = 0;
//
  UINTN                                           StringLen;
  INT32                                           StartIndex;

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    Mode = Mode_OK;
    return EFI_NOT_READY;
  }

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  while (TRUE) {

    if (RefreshDir) {
      SelectNo = 0;
	    PageStartNo = 0;
      LastSelectNo = 0xffffffff;
      UpdateFilePath (FilePath);

      StartNo = (FileDepth == 0)? 1 : 0;
      EndNo = GetEndNo (DirListHead, FileListHead);
      OnePage = (EndNo < (StartNo + FILE_ITEM_SIZE)) ? TRUE : FALSE;
      if ((FileDepth == 0) && SelectNo == 0) {
        SelectNo = 1;
        if (PageStartNo == 0) {
          PageStartNo = 1;
        }
      }
    }

    if (IsNeedRefreshFileFrame) {
      gST->ConOut->EnableCursor (gST->ConOut, FALSE);
      UpdateScrollBar ((EndNo - StartNo + 1), (PageStartNo - StartNo), FILE_ITEM_SIZE);
      UpdateFileFrame (DirListHead, FileListHead, PageStartNo);
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    }

    if ((SelectNo != LastSelectNo) || IsNeedRefreshFileFrame) {

      CurrentNode = GetFileNode (DirListHead, FileListHead, SelectNo, &IsDir);
//[-start-140624-IB05080432-modify]//
      if (CurrentNode != NULL) {
        if (IsDir) {
          UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
        } else {
          UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
        }

        //
        // SelectNo == 0 means [..], that is not a normal directory
        //
        if (SelectNo == 0) {
          IsDir = FALSE;
        }

        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_SELECT_ITEM_COLOR);
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_SELECT_ITEM_COLOR);
        LastSelectNo = SelectNo;
      }
//[-end-140624-IB05080432-modify]//

    }

    RefreshDir = FALSE;
    IsNeedRefreshFileFrame = FALSE;
    gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_SELECT_ITEM_COLOR);
    gST->ConOut->SetCursorPosition (gST->ConOut, (WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET) - 1, (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo) - 1);

    Key = WaitForAKey (0);

    switch (Key.ScanCode) {

    case SCAN_ESC:
      return EFI_ABORTED;
      break;

    case ALT_N:
      Mode = Mode_FileName;
      return EFI_NOT_READY;
      break;

    case SCAN_UP:
      if (SelectNo > PageStartNo) {
        RecoveryItem (DirListHead, FileListHead, SelectNo);
        SelectNo--;
      } else if (SelectNo > StartNo) {
        SelectNo--;
        PageStartNo = SelectNo;
        IsNeedRefreshFileFrame = TRUE;
      }
      break;

    case SCAN_DOWN:
      if (!OnePage) {
        if (SelectNo < (PageStartNo + FILE_ITEM_SIZE - 1)) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo++;
        } else if (SelectNo < EndNo) {
          SelectNo++;
          PageStartNo++;
          IsNeedRefreshFileFrame = TRUE;
        }
      } else if (SelectNo < EndNo) {
        RecoveryItem (DirListHead, FileListHead, SelectNo);
        SelectNo++;
      }
      break;

    case SCAN_PAGE_UP:
      if (PageStartNo == StartNo) {
        if (SelectNo != StartNo) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo = PageStartNo;
        }

      } else if ((PageStartNo - FILE_ITEM_SIZE) >= StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = PageStartNo - FILE_ITEM_SIZE;
        SelectNo = PageStartNo + RelativeIndex;
        IsNeedRefreshFileFrame = TRUE;
      } else if ((PageStartNo - FILE_ITEM_SIZE) < StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = StartNo;
        SelectNo = PageStartNo + RelativeIndex;
        IsNeedRefreshFileFrame = TRUE;
      }
      break;

    case SCAN_PAGE_DOWN:
      if (!OnePage) {
        if ((PageStartNo + FILE_ITEM_SIZE - 1) == EndNo) {
          if (SelectNo != EndNo) {
            CurrentNode = GetFileNode (DirListHead, FileListHead, SelectNo, &IsDir);
//[-start-140624-IB05080432-modify]//
            if (CurrentNode != NULL) {
              if (IsDir) {
                UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
              } else {
                UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
              }
              FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_BODY_COLOR);
              FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
  
              SelectNo = EndNo;
            }
//[-end-140624-IB05080432-modify]//
          }

        } else if ((PageStartNo + FILE_ITEM_SIZE) <= (EndNo - FILE_ITEM_SIZE + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = PageStartNo + FILE_ITEM_SIZE;
          SelectNo = PageStartNo + RelativeIndex;
          IsNeedRefreshFileFrame = TRUE;
        } else if ((PageStartNo + FILE_ITEM_SIZE) > (EndNo - FILE_ITEM_SIZE + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = (EndNo - FILE_ITEM_SIZE + 1);
          SelectNo = PageStartNo + RelativeIndex;
          IsNeedRefreshFileFrame = TRUE;
        }
      } else {
        //
        // Only one page
        //
        if (SelectNo != EndNo) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo = EndNo;
        }
      }
      break;

    default:

      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        if (IsDir) {
          StrCpy (TempFilePath, FilePath);
          StringLen = StrLen(FilePath);
          if ((FilePath[StringLen - 1] != L'\\') && StringLen > 0) {
            StrCat(FilePath, L"\\");
          }
//[-start-140624-IB05080432-modify]//
          if (CurrentNode != NULL) {
            StrCat(FilePath, CurrentNode->Name);
          }
//[-end-140624-IB05080432-modify]//

          if (FileDepth == 0) {
            StrCat(FilePath, L":\\");
          }
          Status = ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle);
          if ((Status != EFI_SUCCESS) && (Status != EFI_NOT_FOUND)) {
            UnicodeSPrint (buf, 80, L"Get file info failed! Status: 0x%x", Status);
            FileUIDisplayErrorMsg (buf);
            StrCpy (FilePath, TempFilePath);
            ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle);

          }

//[-start-140624-IB05080432-modify]//
//          if (FileDepth == 1) {
          if ((FileDepth == 1) && (StrLen(FilePath) > 0)) {
//[-end-140624-IB05080432-modify]//
            StrCpy (gStringBatch, FilePath);
            gStringBatch[StrLen(FilePath) -1] = '\n';
          }

          IsNeedRefreshFileFrame = TRUE;
          RefreshDir = TRUE;

        } else if (SelectNo == 0 && (FileDepth != 0)) {
          //
          // return up level
          //
          for (Index = (INT32)(StrLen(FilePath) - 1), StartIndex = Index; Index >= 0; Index--) {

            if (FilePath[Index] == L'\\') {
              if (FilePath[Index - 1] != L':') {
                FilePath[Index] = L'\0';
              } else if (Index == StartIndex) {
                gBS->SetMem (FilePath, StrSize(FilePath), 0);
              }
              break;
            }
            FilePath[Index] = L'\0';
          }

          if (EFI_SUCCESS != ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle)) {
            FileUIDisplayErrorMsg (L"Get file info failed!");
            return EFI_ABORTED;
          }

          IsNeedRefreshFileFrame = TRUE;
          RefreshDir = TRUE;

        } else {
          //
          // confirm to save exist file
          //
//[-start-140624-IB05080432-modify]//
          if (CurrentNode != NULL) {
            StrCpy (FileName, CurrentNode->Name);
          }
//[-end-140624-IB05080432-modify]//
          StrCat (gStringBatch, L"cd ");
          StrCat (gStringBatch, FilePath);
          StrCat (gStringBatch, L"\n");
          StrCat (gStringBatch, FileName);
          return EFI_SUCCESS;
        }
      } else if (Key.UnicodeChar == CHAR_TAB) {
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_BODY_COLOR);
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
        Mode = Mode_OK;
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        return EFI_NOT_READY;
      } else if (ValidCharacter (Key.UnicodeChar)) {
        if (SearchNode != NULL) {
          if (FileUIToLower(SearchNode->Name[0]) != FileUIToLower(Key.UnicodeChar)) {
            SearchNode = NULL;
          }
        }

        if (EFI_SUCCESS == SearchNextNodeByCharacter (DirListHead, FileListHead, Key.UnicodeChar, &SearchNode)) {
          if ((SearchNode->No < PageStartNo) || (SearchNode->No > (PageStartNo + FILE_ITEM_SIZE - 1))) {
            PageStartNo = SearchNode->No;
            if (PageStartNo >= (EndNo - FILE_ITEM_SIZE + 1)) {
              PageStartNo = (EndNo - FILE_ITEM_SIZE + 1);
            }
            IsNeedRefreshFileFrame = TRUE;
          } else {
            if (SelectNo != SearchNode->No) {
              RecoveryItem (DirListHead, FileListHead, SelectNo);
            }
          }
          SelectNo = SearchNode->No;

        }
      }
      break;
    }

    if (!ValidCharacter (Key.UnicodeChar)) {
      SearchNode = NULL;
    }
  }

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  return EFI_NOT_READY;
}


EFI_STATUS
CheckOKBox (
  VOID
  )
{
  EFI_INPUT_KEY                                         Key = {0};

  FileUIDisplayString ((WindowLeftColumn + OK_BOX_WIDTH_OFFSET), (WindowLeftRow + OK_BOX_HEIGHT_OFFSET), L"[  OK  ]", (int)8, FILE_UI_TITLE_COLOR);
  FileUIDisplayString ((WindowLeftColumn + CANCEL_BOX_WIDTH_OFFSET), (WindowLeftRow + CANCEL_BOX_HEIGHT_OFFSET), L"  Cancel  ", (int)10, FILE_UI_TITLE_COLOR);

//	  gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_CURSOR_COLOR);

//	  gST->ConOut->SetCursorPosition (gST->ConOut, OK_BOX_X + 3 - 1, OK_BOX_Y - 1);

//	  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  while (TRUE) {

    Key = WaitForAKey (0);

    switch (Key.ScanCode) {

    case SCAN_ESC:
      return EFI_ABORTED;
      break;

    case ALT_N:
      Mode = Mode_FileName;
      return EFI_NOT_READY;
      break;

    case SCAN_RIGHT:
      Mode = Mode_Cancel;
      return EFI_NOT_READY;
      break;

    case ALT_F:
    case SCAN_UP:
      Mode = Mode_SelectFile;
      return EFI_NOT_READY;
      break;

    default:
      if (Key.UnicodeChar == CHAR_TAB) {
        Mode = Mode_Cancel;
        return EFI_NOT_READY;
      } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        return EFI_SUCCESS;
      }
    }
  }

}


EFI_STATUS
CheckCancelBox (
  VOID
  )
{
  EFI_INPUT_KEY                                         Key = {0};

  FileUIDisplayString ((WindowLeftColumn + OK_BOX_WIDTH_OFFSET), (WindowLeftRow + OK_BOX_HEIGHT_OFFSET), L"   OK   ", (int)8, FILE_UI_TITLE_COLOR);
  FileUIDisplayString ((WindowLeftColumn + CANCEL_BOX_WIDTH_OFFSET), (WindowLeftRow + CANCEL_BOX_HEIGHT_OFFSET), L"[ Cancel ]", (int)10, FILE_UI_TITLE_COLOR);

//	  gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_CURSOR_COLOR);

//	  gST->ConOut->SetCursorPosition (gST->ConOut, CANCEL_BOX_X + 2 - 1, CANCEL_BOX_Y - 1);

//	  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  while (TRUE) {

    Key = WaitForAKey (0);

    switch (Key.ScanCode) {

    case SCAN_ESC:
      return EFI_ABORTED;
      break;

    case ALT_N:
      Mode = Mode_FileName;
      return EFI_NOT_READY;
      break;

    case SCAN_LEFT:
      Mode = Mode_OK;
      return EFI_NOT_READY;
      break;

    case ALT_F:
    case SCAN_UP:
      Mode = Mode_SelectFile;
      return EFI_NOT_READY;
      break;

    default:
      if (Key.UnicodeChar == CHAR_TAB) {
        Mode = Mode_FileName;
        return EFI_NOT_READY;
      } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        return EFI_ABORTED;
      }
    }
  }

}


//	VOID
//	printlist (
//	  IN     EFI_LIST_ENTRY                                 *DirListHead,
//	  IN     EFI_LIST_ENTRY                                 *FileListHead
//	  )
//	{
//	  EFI_LIST_ENTRY                                        *FLink = NULL;
//	  SHELL_FILE_NODE                                       *CurrentNode = NULL;
//
//	  Print (L"printlist\n");
//
//
//	  //
//	  // directory
//	  //
//	  if (IsListEmpty (DirListHead)) {
//	    Print (L"DirListHead is empty!\n");
//	  } else {
//	    FLink = DirListHead;
//	    FLink = FLink->ForwardLink;
//	    do {
//	      CurrentNode = _CR (FLink, SHELL_FILE_NODE, FilePtr);
//	      Print (L"No: %03d, Dirname: %s\n", CurrentNode->No, CurrentNode->ShellFileInfo->FileName);
//	      FLink = FLink->ForwardLink;
//	    } while (FLink != DirListHead);
//	  }
//
//	  //
//	  // file
//	  //
//	  if (IsListEmpty (FileListHead)) {
//	    Print (L"FileListHead is empty!\n");
//	  } else {
//	    FLink = FileListHead;
//	    FLink = FLink->ForwardLink;
//	    do {
//	      CurrentNode = _CR (FLink, SHELL_FILE_NODE, FilePtr);
//	      Print (L"No: %03d, FileName: %s\n", CurrentNode->No, CurrentNode->ShellFileInfo->FileName);
//	      FLink = FLink->ForwardLink;
//	    } while (FLink != FileListHead);
//	  }
//
//	}

BOOLEAN
IsExistRecordableDevice (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                           HandleCount;
  UINTN                           Index;
  EFI_HANDLE                      ParentHandle;
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;

  Status = gBS->LocateHandleBuffer (
                            ByProtocol,
                            &gEfiSimpleFileSystemProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer
                          );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  //
  // Handle that with Blockio protocol is recordable device
  //
  for (Index=0; Index < HandleCount; Index++) {
    Status = FsuGetParentHandle (HandleBuffer[Index], &ParentHandle);
    if (EFI_ERROR(Status)){
      continue;
    }
    Status = gBS->HandleProtocol (ParentHandle, &gEfiBlockIoProtocolGuid, (VOID **)&BlkIo);
    if (EFI_ERROR(Status)){
      continue;
    } else {
      if (BlkIo->Media->ReadOnly){
        continue;
      } else {
        return TRUE;
      }
    }
  }
  return FALSE;
}



VOID
DoLsWithHandle (
  EFI_HANDLE                                      Handle
)
{
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL       *FileSystemProtocol;
  EFI_FILE                              *File = NULL;
  EFI_STATUS                             Status;
  BOOLEAN                   NoFile;
  EFI_FILE_INFO             *FileInfo;
  BOOLEAN                   IsFirst = TRUE;
  CHAR16                    *FileName = NULL;
  CHAR16                    *TempName;
  EFI_FILE                              *RootHandle = NULL;
  EFI_FILE                              *NewHandle;

  Status = gBS->HandleProtocol (Handle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystemProtocol);

  if (EFI_ERROR (Status)) {
    return;
  }
  //
  //Opens the root directory on a volume
  //
  Status = FileSystemProtocol->OpenVolume (
                                 FileSystemProtocol,
                                 &File
                                 );
  if (EFI_ERROR (Status)) {
    return;
  }
  RootHandle = File;
  FileName = AllocatePool (sizeof(CHAR16));
//[-start-140624-IB05080432-add]//
  if (FileName == NULL) {
    return;
  }
//[-end-140624-IB05080432-add]//
  FileName[0] = L'\0';
  do {

    NoFile            = FALSE;
    IsFirst           = TRUE;
    for ( Status = FsuFileHandleFindFirstFile(File, &FileInfo, &NoFile)
        ; !EFI_ERROR(Status) && !NoFile
        ; Status = FsuFileHandleFindNextFile(File, FileInfo, &NoFile)
       ){

//[-start-140624-IB05080432-modify]//
      if (FileInfo != NULL) {
        if (IsFileSttributeVaild(FileInfo->Attribute)) {
          Print (L"Attribute:0x%x Size:0x%x %s %s           (at 0x%x)\n", FileInfo->Attribute, FileInfo->Size,((FileInfo->Attribute & EFI_FILE_DIRECTORY) == 0) ? L"      " : L"<DIR> ", FileInfo->FileName, FileInfo);
          if (IsFirst && ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0) && (StrCmp (FileInfo->FileName , L"..") != 0)) {
            TempName =  AllocateZeroPool ((StrSize(FileInfo->FileName) + StrLen(FileName) + 1) * sizeof(CHAR16));
            if (TempName != NULL) {
              StrCpy (TempName, FileName);
              StrCat (TempName, L"\\");
              StrCat (TempName, FileInfo->FileName);
//	            FileName =  AllocateZeroPool ((StrSize(FileInfo->FileName) + StrLen(FileName)) * sizeof(CHAR16));
//	            StrCpy (FileName, FileInfo->FileName);
              FREE (FileName);
              FileName = TempName;
              IsFirst = FALSE;
            }
          }
        }
      }
//[-end-140624-IB05080432-modify]//
    }

//[-start-140624-IB05080432-modify]//
    if (FileInfo != NULL) {
      FreePool (FileInfo);
    }
//[-end-140624-IB05080432-modify]//
    if (!IsFirst) {
      Print (L"\n\n Open   \"%s\"\n", FileName);
      Status = File->Open(File, &NewHandle, FileName, (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE) , EFI_FILE_DIRECTORY);
//	      Status = File->Open(File, &File, FileName, (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE) , EFI_FILE_DIRECTORY);
      if (EFI_ERROR (Status)) {
        Print (L" Open failed\n");
      }
//	      Status = File->Close(File);
//	      FreePool (FileName);
      File = NewHandle;
    }
  } while ((Status == EFI_SUCCESS) && !IsFirst);
}

VOID
GetWindowsLoaction (
  VOID
  )
{
  UINTN                                         Columns;
  UINTN                                         Rows;

  //
  //<--------- Columns -------->
  // ¡ô
  // |
  // |
  // Rows
  // |
  // |
  // ¡õ
  //
  FcoQueryMode (&Columns, &Rows);

  WindowLeftColumn = (((int)Columns/2) - (FILE_UI_WIDTH/2));
  WindowLeftRow = (((int)Rows/2) - (FILE_UI_HEIGHT/2));

}

VOID
BackupBackground (
  VOID
  )
{
  UINTN       TextBufferSize;
  UINTN       GraphicBufferSize;
  UINT16      GraphicHeight;

  //
  // Calculate buffer size about text and griphics
  //
  TextBufferSize = (FILE_UI_WIDTH + 1) * (FILE_UI_HEIGHT + 2) * (CHAR_SIZE + ATTR_SIZE);
  GraphicHeight = (((FILE_UI_HEIGHT + 2) * 192)/190) + 1;
  GraphicBufferSize = ((FILE_UI_WIDTH + 1) * GLYPH_WIDTH) * (GraphicHeight * GLYPH_HEIGHT) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

  TextBuffer = AllocateZeroPool (TextBufferSize);
  GraphicBuffer = AllocateZeroPool (GraphicBufferSize);

  if (TextBuffer != NULL) {
    TextGetBackground (WindowLeftColumn, WindowLeftRow - 2, (FILE_UI_WIDTH + 1), (FILE_UI_HEIGHT + 2), TextBuffer);
  }

  if (GraphicBuffer != NULL) {
    GraphicsGetBackground (WindowLeftColumn - 1, WindowLeftRow - 2, (FILE_UI_WIDTH + 1), (FILE_UI_HEIGHT + 2), GraphicBuffer);
  }
}

VOID
RecoveryBackground (
  VOID
)
{

  if (TextBuffer != NULL) {
    TextPutBackground (WindowLeftColumn, WindowLeftRow - 2, (FILE_UI_WIDTH + 1), (FILE_UI_HEIGHT + 2), TextBuffer);
    FreePool (TextBuffer);
    TextBuffer = NULL;
  }

  if (GraphicBuffer != NULL) {
    GraphicsPutBackground (WindowLeftColumn - 1, WindowLeftRow - 2, (FILE_UI_WIDTH + 1), (FILE_UI_HEIGHT + 2), GraphicBuffer);
    FreePool (GraphicBuffer);
    GraphicBuffer = NULL;
  }

}

EFI_STATUS
SaveFileUi (
  IN      CHAR16                                        *TitleName,
  IN      UINT32                                        Type,
  OUT     EFI_FILE                                      **Handle
  )
/*++

Routine Description:

  Save File ui interface

Arguments:


Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  CHAR16                                                buf[80] = {0};
  CHAR16                                                FileName[80] = {0};
  EFI_SIMPLE_TEXT_OUTPUT_MODE                           OldTextSetting = {0};
  UINT32                                                Index = 0;
  CHAR16                                                FilePath[200] = {0};
  EFI_LIST_ENTRY                                        DirListHead = {0};
  EFI_LIST_ENTRY                                        FileListHead = {0};
  BOOLEAN                                               IsDir = FALSE;

  EFI_FILE                                              *OperationFileHandle = NULL;
  EFI_FILE                                              *TempFilePtr = NULL;
#if 0
  EFI_FILE                                              *File = NULL;
  EFI_FILE                                              *RootFileHandle = NULL;

  CHAR16                                                FileData[200] = {0};
  UINTN                                                 DataLen;
#endif

  PageStartNo = 0;
  Mode = Mode_FileName;
  Refresh = TRUE;
  TextBuffer = NULL;
  GraphicBuffer = NULL;

  //
  //Get the left up corner location of windows.
  //
  // (WindowLeftColumn and WindowLeftRow)
  //
  GetWindowsLoaction();

  //
  //for recovery to before screen
  //
  BackupBackground();

  //
  // back up TextSetting
  //
  CopyMem (&OldTextSetting, gST->ConOut->Mode, sizeof (EFI_SIMPLE_TEXT_OUTPUT_MODE));

  if (!IsExistRecordableDevice()) {
    FileUIDisplayErrorMsg (L"No Recordable device !");
    Status = EFI_NOT_FOUND;
    goto SaveFileUiEXIT;
  }

  InitializeListHead (&DirListHead);
  InitializeListHead (&FileListHead);

  Status = GenRootFileHandleList (&DirListHead);
  if (EFI_ERROR(Status)) {
    Status = EFI_NOT_READY;
    goto SaveFileUiEXIT;
  }

  FileDepth = 0;
  Refresh = TRUE;
  
//[-start-140402-IB10300106-modify]//
  if (mAutoLoad) {
    Mode = Mode_LoadDefualtFile;
    mAutoLoad = FALSE;
  }
  else {
    Mode = Mode_FileName;
  }
//[-end-140402-IB10300106-modify]//

  while (TRUE) {

    if (Refresh) {
      FileUIClearScreen (WindowLeftColumn, WindowLeftRow - 1, (WindowLeftColumn + FILE_UI_WIDTH), (WindowLeftRow + FILE_UI_HEIGHT) , FILE_UI_TITLE_COLOR);
      FileUIClearScreen (WindowLeftColumn, WindowLeftRow, (WindowLeftColumn + FILE_UI_WIDTH), (WindowLeftRow + FILE_UI_HEIGHT), FILE_UI_BODY_COLOR);
      FileUIDisplayString (
        (int)(WindowLeftColumn + ((FILE_UI_WIDTH + 1 - StrLen (TitleName)) / 2)),
        WindowLeftRow-1,
        TitleName,
        (int)(StrLen (TitleName)),
        FILE_UI_TITLE_COLOR
        );

      FileUIDisplayString (WindowLeftColumn, WindowLeftRow, L"  File Name: [                                ]  ", (int)49, FILE_UI_BODY_COLOR);

      //
      // display Filepath
      //
      UpdateFilePath (FilePath);

      FileUIDisplayString (WindowLeftColumn, WindowLeftRow+2, L"   Existing Files:", (int)18, FILE_UI_BODY_COLOR);


      //
      //Next three step will draw a dialog
      //1.draw two row line
      //2.draw two side
      //3.draw four corner
      //

      //
      // 1.This is draw two row line
      //
      for (Index = (UINT32)(WindowLeftColumn + 3); Index <= (UINT32)((WindowLeftColumn + FILE_UI_WIDTH) - 3); Index++) {
        UnicodeSPrint (buf, 80, L"%c", BOXDRAW_HORIZONTAL);
        FileUIDisplayString (Index, (WindowLeftRow + 3), buf, (int)1, FILE_UI_BODY_COLOR);
        FileUIDisplayString (Index, ((WindowLeftRow + FILE_UI_HEIGHT) - 1), buf, (int)1, FILE_UI_BODY_COLOR);
      }

      //
      // 2.Draw two side
      //
      for (Index = (UINT32)(WindowLeftRow + 4); Index <= (UINT32)((WindowLeftRow + FILE_UI_HEIGHT) - 2); Index++) {
        UnicodeSPrint (buf, 80, L"%c", BOXDRAW_VERTICAL);
        FileUIDisplayString ((WindowLeftColumn + 2), Index, buf, (int)1, FILE_UI_BODY_COLOR);
        FileUIDisplayString (((WindowLeftColumn + FILE_UI_WIDTH) - 2), Index, buf, (int)1, FILE_UI_BODY_COLOR);
      }

      //
      // 3.This is draw the dialog four corner
      //
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_DOWN_RIGHT);
      FileUIDisplayString ((WindowLeftColumn + 2), (WindowLeftRow + 3), buf, (int)1, FILE_UI_BODY_COLOR);
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_UP_RIGHT);
      FileUIDisplayString ((WindowLeftColumn + 2), ((WindowLeftRow + FILE_UI_HEIGHT) - 1), buf, (int)1, FILE_UI_BODY_COLOR);
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_DOWN_LEFT);
      FileUIDisplayString (((WindowLeftColumn + FILE_UI_WIDTH) - 2), (WindowLeftRow + 3), buf, (int)1, FILE_UI_BODY_COLOR);
      UnicodeSPrint (buf, 80, L"%c", BOXDRAW_UP_LEFT);
      FileUIDisplayString (((WindowLeftColumn + FILE_UI_WIDTH) - 2), ((WindowLeftRow + FILE_UI_HEIGHT) - 1), buf, (int)1, FILE_UI_BODY_COLOR);

      UpdateScrollBar ((GetEndNo(&DirListHead, &FileListHead) + 1), PageStartNo, FILE_ITEM_SIZE);
      UpdateFileFrame (&DirListHead, &FileListHead, PageStartNo);

    }


    gST->ConOut->EnableCursor (gST->ConOut, FALSE);

    while (TRUE) {

      if (Mode != Mode_Cancel && Mode != Mode_OK) {
        FileUIDisplayString ((WindowLeftColumn + OK_BOX_WIDTH_OFFSET), (WindowLeftRow + OK_BOX_HEIGHT_OFFSET), L"   OK   ", (int)8, FILE_UI_TITLE_COLOR);
        FileUIDisplayString ((WindowLeftColumn + CANCEL_BOX_WIDTH_OFFSET), (WindowLeftRow + CANCEL_BOX_HEIGHT_OFFSET), L"  Cancel  ", (int)10, FILE_UI_TITLE_COLOR);
      }

      switch (Mode) {

      case Mode_FileName:
        Status = InputFileName (FileName);
        break;

      case Mode_SelectFile:
 	      Status = SelectFile (&DirListHead, &FileListHead, FilePath, FileName, &OperationFileHandle);
        break;

//[-start-140402-IB10300106-add]//
      case Mode_LoadDefualtFile:
      Status = SearchDefaultFile (&DirListHead, &FileListHead, FilePath, FileName, &OperationFileHandle);
      Mode = Mode_FileName;
      break;
//[-end-140402-IB10300106-add]//

      case Mode_OK:
        Status = CheckOKBox ();
        break;

      case Mode_Cancel:
        Status = CheckCancelBox ();
        break;
      }


      if (Status == EFI_SUCCESS) {
        if (StrLen (FileName) == 0) {
          FileUIDisplayErrorMsg (L"You must specify a filename");
          Refresh = TRUE;
          Status = EFI_NOT_READY;
          break;
        } else if (FileDepth > 0){
          //
          // check file is exist in list or not
          //
          Status = IsFileExists (FileName, &DirListHead, &FileListHead, &IsDir);
          if (Status == EFI_INVALID_PARAMETER) {
            SetMem (FileName, 80, 0);
            Status = EFI_NOT_READY;
            break;
          } else if ((Status == EFI_SUCCESS) && !IsDir && (Type == Type_Save)) {
            if (FileUICheckBox (L"Replace existing file?") == FALSE) {
              SetMem (FileName, 80, 0);
              Refresh = TRUE;
              Status = EFI_NOT_READY;
              break;
            } else {
              //
              //delete the file than "creat" it, then return file handle of it.
              //
              OperationFileHandle->Open (OperationFileHandle, &TempFilePtr, FileName,  (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE), EFI_FILE_ARCHIVE);
              TempFilePtr->Delete (TempFilePtr);
              OperationFileHandle->Open (OperationFileHandle, Handle, FileName,  (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE ), EFI_FILE_ARCHIVE);
              OperationFileHandle->Close (OperationFileHandle);
            }
          } else if ((Status != EFI_SUCCESS) && !IsDir && (Type == Type_Save)) {
            //
            // Creat it, then return file handle of it.
            //
            OperationFileHandle->Open (OperationFileHandle, Handle, FileName,  (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE ), EFI_FILE_ARCHIVE);
            OperationFileHandle->Close (OperationFileHandle);
          } else if ((Status != EFI_SUCCESS) && !IsDir && (Type == Type_Load)) {
            FileUIDisplayErrorMsg (L"File isn't exist");
            SetMem (FileName, 80, 0);
            Refresh = TRUE;
            Status = EFI_NOT_READY;
            break;
          } else if ((Status == EFI_SUCCESS) && !IsDir && (Type == Type_Load)) {
            //
            //use Read attribute to get the file handle, then return file handle of it.
            //
            OperationFileHandle->Open (OperationFileHandle, Handle, FileName, EFI_FILE_MODE_READ, EFI_FILE_ARCHIVE);
            OperationFileHandle->Close (OperationFileHandle);
            {
              gRT->SetVariable (
                     L"BiosUpdateFilePath",
                     &gEfiGenericVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     StrSize (FilePath),
                     (VOID*)FilePath
                     );

              gRT->SetVariable (
                     L"BiosUpdateFileName",
                     &gEfiGenericVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     StrSize (FileName),
                     (VOID*)FileName
                     );
            }
#if 0
            Status = mFileSystemProtocol->OpenVolume (
                                           mFileSystemProtocol,
                                           &RootFileHandle
                                           );

            if (!EFI_ERROR(Status)) {
              Status = RootFileHandle->Open (RootFileHandle, &File, L"StartUp.nsh", (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE), 0);

              if (!EFI_ERROR(Status)) {
                File->Delete (File);
              }
              Status = RootFileHandle->Open (RootFileHandle, &File, L"StartUp.nsh", (EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE ), 0);

              //gen file head
              CopyMem(FileData,gStartUpBatch,2);
              StrCpy (&FileData[1],gStringBatch);

              DataLen = 200;
              File->Write (File, &DataLen, FileData);
              File->Close (File);
            }
#endif
          } else if ((Status == EFI_SUCCESS) && IsDir) {
            FileUIDisplayErrorMsg (L"It exists folder with same name");
            SetMem (FileName, 80, 0);
            Refresh = TRUE;
            Status = EFI_NOT_READY;
            break;
          }
          Status = EFI_SUCCESS;
          goto SaveFileUiEXIT;
        } else {
          FileUIDisplayErrorMsg (L"Must specify a stroage device");
          SetMem (FileName, 80, 0);
          Refresh = TRUE;
          Status = EFI_NOT_READY;
          break;
        }
      }

      if (Status == EFI_ABORTED) {
        goto SaveFileUiEXIT;
      }
    }
  }


SaveFileUiEXIT:


  //
  // Recovery TextSetting
  //
  CopyMem (gST->ConOut->Mode, &OldTextSetting, sizeof (EFI_SIMPLE_TEXT_OUTPUT_MODE));
  if (FileDepth > 0) {
    RemoveFileList (&DirListHead, &FileListHead);
  }
  RecoveryBackground();
  return Status;
}



BOOLEAN
IsRemoveableDevice (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
/*++

Routine Description:

  Judge if DevicePath is describing a removable device, such as USB or SCSI

Arguments:

  DevicePath  - Device path instance for the device

Returns:

  TRUE  - if the device is a removable device
  FALSE - if the device is not a removable device
--*/
{
  if (NULL == DevicePath) {
    return FALSE;
  }

  while (!IsDevicePathEndType (DevicePath)) {
    if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) {
      switch (DevicePathSubType (DevicePath)) {
      case MSG_USB_DP:
      case MSG_SCSI_DP:
        return TRUE;

      default:
        return FALSE;
      }
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  return FALSE;
}

UINTN
GetDeviceMediaType (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH  *Acpi;

  //
  //  Parse the device path:
  //  Devicepath sub type                 mediatype
  //    MEDIA_HANRDDRIVE_DP      ->       Hard Disk
  //    MEDIA_CDROM_DP           ->       CD Rom
  //    Acpi.HID = 0X0604        ->       Floppy
  //
  if (NULL == DevicePath) {
    return MediaUnknown;
  }

  while (!IsDevicePathEndType (DevicePath)) {
    if (DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) {
      switch (DevicePathSubType (DevicePath)) {
      case MEDIA_HARDDRIVE_DP:
        return MediaHardDisk;

      case MEDIA_CDROM_DP:
        return MediaCDRom;
      }
    }

    if (DevicePathType (DevicePath) == ACPI_DEVICE_PATH) {
      Acpi = (ACPI_HID_DEVICE_PATH *) DevicePath;
      if (EISA_ID_TO_NUM (Acpi->HID) == 0x0604) {
        return MediaFloppy;
      }
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  return MediaUnknown;
}


EFI_STATUS
GetDiskFsNumber (
  UINTN                HandleIndex,
  //EFI_HANDLE           DiskHandle,
  UINT16               *FsNumber
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       NoHandles;
  EFI_HANDLE                  *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINT16                      Count;
  UINT16                      Count1;
  EFI_HANDLE                 ParentHandle;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &NoHandles, &HandleBuffer);
  if (EFI_ERROR(Status))
    return Status;

  //
  //  The sort order is:
  //    floppy
  //    hard disk
  //    cd rom
  //    unknown device
  //    removeable floppy
  //    removeable hard disk
  //    removeable cd rom
  //    removeable unknown device
  //
  //

  //
  // count non-removeable device
  //
  for (Count = 0, Index = 0; Index < NoHandles; Index++) {

    Status = FsuGetParentHandle (HandleBuffer[Index], &ParentHandle);
    if (EFI_ERROR(Status)){
      continue;
    }

    Status = gBS->HandleProtocol (ParentHandle, &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);

    if (IsRemoveableDevice(DevicePath) == FALSE) {
      Count++;
    }
  }

  //
  // Count removeable
  //
  for (Count1 = 0, Index = 0; Index < NoHandles; Index++) {

    Status = FsuGetParentHandle (HandleBuffer[Index], &ParentHandle);
    if (EFI_ERROR(Status)){
      continue;
    }

    Status = gBS->HandleProtocol (ParentHandle, &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);

    if (IsRemoveableDevice(DevicePath) == TRUE) {
      if (Index != HandleIndex) {
        Count1++;
      }
      else {
        break;
      }

    }
  }

  *FsNumber = Count + Count1;
  return EFI_SUCCESS;
}

BOOLEAN
IsFiltered (
  EFI_FILE_INFO          **FileInfo
  )
{
  UINTN                  NameSize;
  CHAR16                 *NamePtr;

  if ((*FileInfo)->Attribute == EFI_FILE_DIRECTORY) {
    //never filter Dirs
    return FALSE;
  }
  NamePtr = (*FileInfo)->FileName;
  NameSize = StrLen ((*FileInfo)->FileName);
  if (StrnCmp ((CHAR16*)(NamePtr + NameSize - 4), L".", 1) == 0) {
    if ((StrnCmp ((CHAR16*)(NamePtr + NameSize - 3), L"e", 1) == 0) || (StrnCmp ((CHAR16*)(NamePtr + NameSize - 3), L"E", 1) == 0)) {
      if ((StrnCmp ((CHAR16*)(NamePtr + NameSize - 2), L"f", 1) == 0) || (StrnCmp ((CHAR16*)(NamePtr + NameSize - 2), L"F", 1) == 0)) {
        if ((StrnCmp ((CHAR16*)(NamePtr + NameSize - 1), L"i", 1) == 0) || (StrnCmp ((CHAR16*)(NamePtr + NameSize - 1), L"I", 1) == 0)) {
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

//[-start-140402-IB10300106-add]//
EFI_STATUS
SearchDefaultFile (
  IN      EFI_LIST_ENTRY                                *DirListHead,
  IN      EFI_LIST_ENTRY                                *FileListHead,
  IN      CHAR16                                        *FilePath,
  IN OUT  CHAR16                                        *FileName,
  OUT     EFI_FILE                                      **OperationFileHandle
  )
{
  EFI_STATUS                                      Status = EFI_SUCCESS;
  INT32                                           SelectNo = 0;
  INT32                                           LastSelectNo = 0xffffffff;
  FILE_HANDLE_NODE                                *SearchNode = NULL;
  INT32                                           StartNo = 0;
  INT32                                           EndNo = 0;
  BOOLEAN                                         IsNeedRefreshFileFrame = FALSE;
  BOOLEAN                                         RefreshDir = TRUE;
  BOOLEAN                                         OnePage = TRUE;
  FILE_HANDLE_NODE                                *CurrentNode = NULL;
  BOOLEAN                                         IsDir = FALSE;
  INT32                                           RelativeIndex = 0;
  EFI_INPUT_KEY                                   Key = {0};
  CHAR16                                          buf[80] = {0};
  CHAR16                                          TempFilePath[200] = {0};
  INT32                                           Index = 0;
  UINTN                                           StringLen;
  INT32                                           StartIndex;

  if (IsListEmpty(DirListHead) && IsListEmpty(FileListHead)) {
    Mode = Mode_OK;
    return EFI_NOT_READY;
  }

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  while (TRUE) {

    if (RefreshDir) {
      SelectNo = 0;
	    PageStartNo = 0;
      LastSelectNo = 0xffffffff;
      UpdateFilePath (FilePath);

      StartNo = (FileDepth == 0)? 1 : 0;
      EndNo = GetEndNo (DirListHead, FileListHead);
      OnePage = (EndNo < (StartNo + FILE_ITEM_SIZE)) ? TRUE : FALSE;
      if ((FileDepth == 0) && SelectNo == 0) {
        SelectNo = 1;
        if (PageStartNo == 0) {
          PageStartNo = 1;
        }
      }
    }

    if (IsNeedRefreshFileFrame) {
      gST->ConOut->EnableCursor (gST->ConOut, FALSE);
      UpdateScrollBar ((EndNo - StartNo + 1), (PageStartNo - StartNo), FILE_ITEM_SIZE);
      UpdateFileFrame (DirListHead, FileListHead, PageStartNo);
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    }

//[-start-140624-IB05080432-modify]//
    if ((SelectNo != LastSelectNo) || IsNeedRefreshFileFrame) {

      CurrentNode = GetFileNode (DirListHead, FileListHead, SelectNo, &IsDir);
      if (CurrentNode != NULL) {
        if (IsDir) {
          UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
        } else {
          UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
        }

        //
        // SelectNo == 0 means [..], that is not a normal directory
        //
        if (SelectNo == 0) {
          IsDir = FALSE;
        }

        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_SELECT_ITEM_COLOR);
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_SELECT_ITEM_COLOR);
        LastSelectNo = SelectNo;
      }

    }
//[-end-140624-IB05080432-modify]//

    RefreshDir = FALSE;
    IsNeedRefreshFileFrame = FALSE;
    gST->ConOut->SetAttribute (gST->ConOut, FILE_UI_SELECT_ITEM_COLOR);
    gST->ConOut->SetCursorPosition (gST->ConOut, (WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET) - 1, (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo) - 1);

    Key = SimKey (SelectNo, EndNo, CurrentNode);

    if ( KeyState == STATE_FINISH_SIM_KEY) {
        return EFI_ABORTED;
    }

    switch (Key.ScanCode) {

    case SCAN_ESC:
      return EFI_ABORTED;
      break;

    case ALT_N:
      Mode = Mode_FileName;
      return EFI_NOT_READY;
      break;

    case SCAN_UP:
      if (SelectNo > PageStartNo) {
        RecoveryItem (DirListHead, FileListHead, SelectNo);
        SelectNo--;
      } else if (SelectNo > StartNo) {
        SelectNo--;
        PageStartNo = SelectNo;
        IsNeedRefreshFileFrame = TRUE;
      }
      break;

    case SCAN_DOWN:
      if (!OnePage) {
        if (SelectNo < (PageStartNo + FILE_ITEM_SIZE - 1)) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo++;
        } else if (SelectNo < EndNo) {
          SelectNo++;
          PageStartNo++;
          IsNeedRefreshFileFrame = TRUE;
        }
      } else if (SelectNo < EndNo) {
        RecoveryItem (DirListHead, FileListHead, SelectNo);
        SelectNo++;
      }
      break;

    case SCAN_PAGE_UP:
      if (PageStartNo == StartNo) {
        if (SelectNo != StartNo) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo = PageStartNo;
        }

      } else if ((PageStartNo - FILE_ITEM_SIZE) >= StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = PageStartNo - FILE_ITEM_SIZE;
        SelectNo = PageStartNo + RelativeIndex;
        IsNeedRefreshFileFrame = TRUE;
      } else if ((PageStartNo - FILE_ITEM_SIZE) < StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = StartNo;
        SelectNo = PageStartNo + RelativeIndex;
        IsNeedRefreshFileFrame = TRUE;
      }
      break;

    case SCAN_PAGE_DOWN:
      if (!OnePage) {
        if ((PageStartNo + FILE_ITEM_SIZE - 1) == EndNo) {
//[-start-140624-IB05080432-modify]//
          if (SelectNo != EndNo) {
            CurrentNode = GetFileNode (DirListHead, FileListHead, SelectNo, &IsDir);
            if (CurrentNode != NULL) {
              if (IsDir) {
                UnicodeSPrint (buf, 80, L"[%s]", CurrentNode->Name);
              } else {
                UnicodeSPrint (buf, 80, L"%s", CurrentNode->Name);
              }
              FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_BODY_COLOR);
              FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
  
              SelectNo = EndNo;
            }
          }
//[-end-140624-IB05080432-modify]//

        } else if ((PageStartNo + FILE_ITEM_SIZE) <= (EndNo - FILE_ITEM_SIZE + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = PageStartNo + FILE_ITEM_SIZE;
          SelectNo = PageStartNo + RelativeIndex;
          IsNeedRefreshFileFrame = TRUE;
        } else if ((PageStartNo + FILE_ITEM_SIZE) > (EndNo - FILE_ITEM_SIZE + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = (EndNo - FILE_ITEM_SIZE + 1);
          SelectNo = PageStartNo + RelativeIndex;
          IsNeedRefreshFileFrame = TRUE;
        }
      } else {
        //
        // Only one page
        //
        if (SelectNo != EndNo) {
          RecoveryItem (DirListHead, FileListHead, SelectNo);
          SelectNo = EndNo;
        }
      }
      break;

    default:

      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        if (IsDir) {
          StrCpy (TempFilePath, FilePath);
          StringLen = StrLen(FilePath);
          if ((FilePath[StringLen - 1] != L'\\') && StringLen > 0) {
            StrCat(FilePath, L"\\");
          }
//[-start-140624-IB05080432-modify]//
          if (CurrentNode != NULL) {
            StrCat(FilePath, CurrentNode->Name);
          }
//[-end-140624-IB05080432-modify]//

          if (FileDepth == 0) {
            StrCat(FilePath, L":\\");
          }
          Status = ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle);
          if ((Status != EFI_SUCCESS) && (Status != EFI_NOT_FOUND)) {
            UnicodeSPrint (buf, 80, L"Get file info failed! Status: 0x%x", Status);
            FileUIDisplayErrorMsg (buf);
            StrCpy (FilePath, TempFilePath);
            ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle);

          }

//[-start-140624-IB05080432-modify]//
//          if (FileDepth == 1) {
          if ((FileDepth == 1) && (StrLen(FilePath) > 0)) {
//[-end-140624-IB05080432-modify]//
            StrCpy (gStringBatch, FilePath);
            gStringBatch[StrLen(FilePath) -1] = '\n';
          }

          IsNeedRefreshFileFrame = TRUE;
          RefreshDir = TRUE;

        } else if (SelectNo == 0 && (FileDepth != 0)) {
          //
          // return up level
          //
          for (Index = (INT32)(StrLen(FilePath) - 1), StartIndex = Index; Index >= 0; Index--) {

            if (FilePath[Index] == L'\\') {
              if (FilePath[Index - 1] != L':') {
                FilePath[Index] = L'\0';
              } else if (Index == StartIndex) {
                gBS->SetMem (FilePath, StrSize(FilePath), 0);
              }
              break;
            }
            FilePath[Index] = L'\0';
          }

          if (EFI_SUCCESS != ReGenFileList (FilePath, CurrentNode, DirListHead, FileListHead, OperationFileHandle)) {
            FileUIDisplayErrorMsg (L"Get file info failed!");
            return EFI_ABORTED;
          }

          IsNeedRefreshFileFrame = TRUE;
          RefreshDir = TRUE;

        } else {
          //
          // confirm to save exist file
          //
//[-start-140624-IB05080432-modify]//
          if (CurrentNode != NULL) {
            StrCpy (FileName, CurrentNode->Name);
          }
//[-end-140624-IB05080432-modify]//
          StrCat (gStringBatch, L"cd ");
          StrCat (gStringBatch, FilePath);
          StrCat (gStringBatch, L"\n");
          StrCat (gStringBatch, FileName);
          return EFI_SUCCESS;
        }
      } else if (Key.UnicodeChar == CHAR_TAB) {
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), L"                                           ", 43, FILE_UI_BODY_COLOR);
        FileUIDisplayString ((WindowLeftColumn + FILE_ITEM_L_WIDTH_OFFSET), (WindowLeftRow + FILE_ITEM_L_HEIGHT_OFFSET) + (SelectNo - PageStartNo), buf, (int)(StrLen(buf)), FILE_UI_BODY_COLOR);
        Mode = Mode_OK;
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        return EFI_NOT_READY;
      } else if (ValidCharacter (Key.UnicodeChar)) {
        if (SearchNode != NULL) {
          if (FileUIToLower(SearchNode->Name[0]) != FileUIToLower(Key.UnicodeChar)) {
            SearchNode = NULL;
          }
        }

        if (EFI_SUCCESS == SearchNextNodeByCharacter (DirListHead, FileListHead, Key.UnicodeChar, &SearchNode)) {
          if ((SearchNode->No < PageStartNo) || (SearchNode->No > (PageStartNo + FILE_ITEM_SIZE - 1))) {
            PageStartNo = SearchNode->No;
            if (PageStartNo >= (EndNo - FILE_ITEM_SIZE + 1)) {
              PageStartNo = (EndNo - FILE_ITEM_SIZE + 1);
            }
            IsNeedRefreshFileFrame = TRUE;
          } else {
            if (SelectNo != SearchNode->No) {
              RecoveryItem (DirListHead, FileListHead, SelectNo);
            }
          }
          SelectNo = SearchNode->No;

        }
      }
      break;
    }

    if (!ValidCharacter (Key.UnicodeChar)) {
      SearchNode = NULL;
    }
  }

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  return EFI_NOT_READY;
}


EFI_INPUT_KEY
SimKey(
  INT32             SelectNo, 
  INT32             EndNo, 
  FILE_HANDLE_NODE  *CurrentNode
  )
{

  EFI_INPUT_KEY Key;

  Key.ScanCode = 0;
  Key.UnicodeChar = 0;

  switch (KeyState) {
    case STATE_SEARCH_DEVICE:
      if (SelectNo > mCurrentDevice) {
        KeyState = STATE_SLELECT_DEVICE;
        mCurrentDevice = SelectNo;
      }
      else {
        
      if (SelectNo >= EndNo) {
        KeyState = STATE_FINISH_SIM_KEY;
        break;
      }
        
        Key.ScanCode = SCAN_DOWN;
      }
      break;
    case STATE_SLELECT_DEVICE:
      Key.ScanCode = 0;
      Key.UnicodeChar = CHAR_CARRIAGE_RETURN;

     KeyState = STATE_SEARCH_FILE;
      break;
    case STATE_SEARCH_FILE:

//[-start-140624-IB05080432-modify]//
      if (CurrentNode != NULL) {
        if (StrnCmp (CurrentNode->Name, PcdGetPtr(PcdHotKeyFlashFileName), MAX_NUMBER(StrLen(CurrentNode->Name), StrLen(PcdGetPtr(PcdHotKeyFlashFileName)))) == 0) {
          KeyState = STATE_SELECT_FILE;
          break;
        }
      }
//[-end-140624-IB05080432-modify]//
      
      if (SelectNo >= EndNo) {
        KeyState = STATE_SEARCH_FILE_NOT_FOUND;
        break;
      }
      
      Key.ScanCode = SCAN_DOWN;
      break;
      
    case STATE_SELECT_FILE:
      Key.ScanCode = 0;
      Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
      KeyState = STATE_SEARCH_FILE_COMPLETE;
      break;
      
    case STATE_SEARCH_FILE_COMPLETE:
    case STATE_FINISH_SIM_KEY:
      break;

    case STATE_SEARCH_FILE_NOT_FOUND:

      if (SelectNo == 0) {
        KeyState = STATE_RETURN_UP_LEVEL;
        break;
      }
      Key.ScanCode = SCAN_UP;
      break;
    case STATE_RETURN_UP_LEVEL:
      Key.ScanCode = 0;
      Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
      
      KeyState = STATE_SEARCH_DEVICE;
      break;
  }

  return Key;
}
//[-end-140402-IB10300106-add]//

