/** @file

  Event Log Viewer DXE implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <EventLogViewer.h>
#include <EventLogViewerCommonFunc.h>

EVENT_LOG_VIEWER_PROTOCOL           gUtility;
UINTN                               gColumn;
UINTN                               gTopRow;
UINTN                               gBottomRow;
UINTN                               gMaxItemPerPage;
UINTN                               ItemNum;
UINTN                               SelectedItem;
UINTN                               SelectedMenu;
VIEW_EVENT_LOG_MENU                 *MenuList;
EVENT_LOG_PROTOCOL                  *gEventHandler;
BOOLEAN                             gShowUtilVer; 
CHAR16                              *gUtilityVersion = L"Version 0.00.03";

/**
 Unload function of this driver

 @param[in]         ImageHandle    - Handle of this image            
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ViewEventLogDriverUnload ( 
  IN EFI_HANDLE  ImageHandle 
  )
{
  EFI_STATUS Status;

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gH2OEventLogViewerProtocolGuid, 
                  &gUtility,
                  NULL
                  );

  return Status;
}

/**
 Function of draw frame.          
 
 @retval EFI Status                  
*/
VOID
EFIAPI
DrawFrame (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
{
  UINTN                  Index;
  CHAR16                 *ViewEventTitle = L"H2O Event Log Viewer";
  DsplayHotkeyInfo1 (LocalScreen);
  ClearLines (
    LocalScreen.LeftColumn, 
    LocalScreen.RightColumn, 
    LocalScreen.TopRow, 
    LocalScreen.TopRow, 
    VE_TITLE_TEXT);
  //
  // Show "H2O Event Log Utility" Title
  //
  SetCurPos ((LocalScreen.RightColumn - GetStringWidth(ViewEventTitle)/2)/2, 0);
  Print (L"%s", ViewEventTitle);
  ClearLines (LocalScreen.LeftColumn, 
              LocalScreen.RightColumn, 
              LocalScreen.TopRow + 1, 
              LocalScreen.TopRow + 1, 
              VE_SUBTITLE_TEXT);

  SetColor (VE_BODER_LINE_COLOR);
  
  SetCurPos (LocalScreen.LeftColumn, gTopRow - 1);
  Print (L"%c", BOXDRAW_DOWN_RIGHT);
  
  SetCurPos (LocalScreen.LeftColumn, gBottomRow + 1);
  Print (L"%c", BOXDRAW_UP_RIGHT);
  
  SetCurPos (LocalScreen.RightColumn - 1, gTopRow - 1);
  Print (L"%c", BOXDRAW_DOWN_LEFT);
  
  SetCurPos (LocalScreen.RightColumn - 1, gBottomRow + 1);
  Print (L"%c", BOXDRAW_UP_LEFT);

  for (Index = LocalScreen.LeftColumn + 1; Index < (LocalScreen.RightColumn - 1) ; Index++) {
    SetCurPos (Index, gTopRow - 1);
    Print (L"%c", BOXDRAW_HORIZONTAL);
    
    SetCurPos (Index, gBottomRow + 1);
    Print (L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = gTopRow; Index < gBottomRow + 1; Index++) {
    SetCurPos (LocalScreen.LeftColumn, Index);
    Print (L"%c", BOXDRAW_VERTICAL);
    
    SetCurPos (LocalScreen.RightColumn - 1, Index);
    Print (L"%c", BOXDRAW_VERTICAL);
  }

  ClearLines (
    gColumn,
    LocalScreen.RightColumn - 1,
    gTopRow,
    gBottomRow,
    VE_NORMAL_MENU_TEXT);
  
  return;
}

/**
 Init Function.          
 
 @retval EFI Status                  
*/
VOID
EFIAPI
InitMenu (
  VOID
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  UINT8                                 Index;
  UINTN                                 StorageNum;
  EVENT_STORAGE_INFO                    *StorageName;

  Status = gBS->LocateProtocol (&gH2OEventLogProtocolGuid, NULL, (VOID **)&gEventHandler);
  if (EFI_ERROR(Status)) {
    return;
  }

  Status = gEventHandler->GetStorageName (&StorageName, &StorageNum);
  if (EFI_ERROR(Status)) {
    return;
  }

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof(VIEW_EVENT_LOG_MENU) * StorageNum * 2, (VOID **)&MenuList);
  if (EFI_ERROR(Status)) {
    return;
  }

  ItemNum = StorageNum * 2;

  for (Index = 0; Index < StorageNum; Index++) {
    //
    // for View XXX(StorageName) Event Log.
    //
    MenuList[Index].Id            = Index;
    MenuList[Index].StorageString = StorageName[Index].EventStorageName;
    CatViewEventLogString (StorageName[Index].EventStorageName, &MenuList[Index].ShowString);
    
    //
    // for Clear XXX(StorageName) Event Log.
    //
    MenuList[Index + StorageNum].Id            = Index + StorageNum;
    MenuList[Index + StorageNum].StorageString = StorageName[Index].EventStorageName;
    CatClearEventLogString(StorageName[Index].EventStorageName, &MenuList[Index + StorageNum].ShowString);
 
  }

}

/**
 Print function.         
 
 @retval EFI Status                  
*/
VOID
EFIAPI
PrintMenu (
  VOID
  )
{ 
  UINT8     Index, Index2;

  Index = 0;
  Index2 = 0;
  
  for (Index = 0; Index < ItemNum; Index++){  
    if (Index2 == SelectedItem) {
      SetColor (VE_SELECTED_MENU_TEXT);
      SelectedMenu = Index;
    } else {
      SetColor (VE_NORMAL_MENU_TEXT);
    }
    SetCurPos (gColumn, gTopRow + Index2);
    Print (L"%s", MenuList[Index].ShowString);
    Index2++;
  }

  //Restore color setting
  SetColor (VE_NORMAL_MENU_TEXT);
}

/**
 Get the Event String.    

 Param[in]  EventInfo     The information of the event to translate string message.
 Param[out] EventString   String returned for the event.
 Param[out] StringSize    Size of EventString.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetEventString (
  IN     VOID                           *EventInfo,
  IN OUT CHAR16                         **EventString,
  IN OUT UINTN                          *StringSize
  )
{
  EFI_STATUS                              Status;
  UINTN                                   HandleCount;
  EFI_HANDLE                              *HandleBuffer;
  UINTN                                   Index;
  H2O_EVENT_LOG_STRING_PROTOCOL           *EventLogString;
  H2O_EVENT_LOG_STRING_OVERWRITE_PROTOCOL *EventLogStringOverwrite;

  Status = gBS->LocateProtocol (
                            &gH2OEventLogStringOverwriteProtocolGuid,
                            NULL,
                            (VOID **)&EventLogStringOverwrite
                            );
  if (!EFI_ERROR(Status)) {
    //
    // There exist RAS String Overwrite protocol. So, chech it firstly.
    //
    Status = EventLogStringOverwrite->EventLogStringOverwriteHandler (EventInfo, EventString, StringSize);
    if (!EFI_ERROR(Status)) {
      //
      // We Got it.
      //
      return EFI_SUCCESS;
    }
  }
  
  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                                ByProtocol,
                                &gH2OEventLogStringProtocolGuid,
                                NULL,
                                &HandleCount,
                                &HandleBuffer
                                );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    //
    // Double check which device connected on PCI(1D|0). Is keyboard??
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2OEventLogStringProtocolGuid,
                    (VOID **)&EventLogString
                    );  
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = EventLogString->EventLogStringHandler (EventInfo, EventString, StringSize);

    if (!EFI_ERROR (Status)) {
      //
      // We Got It
      //
      return Status;
    }
  }

  gBS->FreePool (HandleBuffer);

  return EFI_NOT_FOUND;
}

/**
 Execure function of Event Log Viewer.

 @param[in]         This                          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ExecuteEventLogViewer (
  IN EVENT_LOG_VIEWER_PROTOCOL            *This
  )
{
  EFI_INPUT_KEY                             Key;
  UINTN                                     Row, Column;
  UINTN                                     Index;
  EFI_SCREEN_DESCRIPTOR                     LocalScreen;
  
  gST->ConOut->QueryMode (
                gST->ConOut,
                gST->ConOut->Mode->Mode,
                &LocalScreen.RightColumn,
                &LocalScreen.BottomRow
                );
  LocalScreen.LeftColumn = 0;
  LocalScreen.TopRow = 0;
  
  Column            = LocalScreen.LeftColumn + 1;
  Row               = LocalScreen.TopRow + 3;
  gColumn           = LocalScreen.LeftColumn + 1;
  gTopRow           = LocalScreen.TopRow + 3;
  gBottomRow        = LocalScreen.BottomRow - 4;
  gMaxItemPerPage   = gBottomRow - gTopRow;
  SelectedItem      = 0;
  SelectedMenu      = 0;
  
  EnCursor(FALSE);
  
  InitMenu ();

  //
  // Exit if no Event Log Storages exist
  //
  if (ItemNum == 0) {
    DisplayPopupMessage (L"No Event Log Storage Exist!!", VeDlgOk, LocalScreen);
    return EFI_NOT_FOUND;
  }
  
  DrawFrame (LocalScreen);
  
  ClearLines (
    gColumn,
    LocalScreen.RightColumn - 1,
    gTopRow,
    gBottomRow,
    VE_NORMAL_MENU_TEXT);
  
  do {
    PrintMenu ();  
    
    SetCurPos (Column, Row);
    
    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);  

    switch (Key.UnicodeChar) {
      
    default:
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        if (MenuList[SelectedMenu].Id < (ItemNum / 2)) {
          //
          // Process View Storage Event Log function.
          //
          ViewStorageEventLog (MenuList[SelectedMenu].Id, LocalScreen);
          
          ClearLines (
            gColumn,
            LocalScreen.RightColumn - 1,
            gTopRow,
            gBottomRow,
            VE_NORMAL_MENU_TEXT);

          DsplayHotkeyInfo1 (LocalScreen);
        } else {
          ClearStorageEventLog(MenuList[SelectedMenu].Id - (ItemNum / 2), LocalScreen);
        } 
        
        SetColor (VE_NORMAL_MENU_TEXT);
        break;
      }
      
    case 0:
      switch (Key.ScanCode) {
        
      case SCAN_UP:
        if(Row <= gTopRow)
          Row = gTopRow + ItemNum - 1;
        else
          Row--;

        SelectedItem = Row - gTopRow;
        break;
   
      case SCAN_DOWN:
        if(Row >= (gTopRow + ItemNum - 1)) 
          Row = gTopRow;
        else
          Row++;
  
        SelectedItem = Row -gTopRow;
        break;
        
      case SCAN_F1:
        if (gShowUtilVer == TRUE) {
          gShowUtilVer = FALSE;
        } else {
          gShowUtilVer = TRUE;
        }
        DsplayHotkeyInfo1 (LocalScreen);
        break;
      case SCAN_ESC:
        return EFI_SUCCESS;
        
      default:
        break;
      }
    }
  }while (TRUE);
}


/**
 Entry point of this driver. Install Event Log Viewer protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogViewerDxeEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  ) 
{
  EFI_STATUS                   Status;
  VOID                        *Interface;
  EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;

  gEventHandler = NULL;
  gShowUtilVer  = FALSE;
  
  //
  // Check if the custom-define protocol is installed or not
  //
  Status = gBS->LocateProtocol (
                  &gH2OEventLogViewerProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
                  
  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  // 
  // Retrieve the Loaded Image Protocol from image handle 
  // 
  Status = gBS->OpenProtocol (
                          ImageHandle,
                          &gEfiLoadedImageProtocolGuid,
                          (VOID **)&LoadedImage,
                          ImageHandle,
                          ImageHandle,
                          EFI_OPEN_PROTOCOL_GET_PROTOCOL
                          ); 
  if (EFI_ERROR (Status)) { 
    return Status; 
  }
  
  LoadedImage->Unload = ViewEventLogDriverUnload;
  gUtility.ExecuteEventLogViewer = ExecuteEventLogViewer;

  //
  // Install custom protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gH2OEventLogViewerProtocolGuid,
                  &gUtility,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  return EFI_SUCCESS ;
}

