/** @file
  Setup Utility Browser Protocol definition

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

#ifndef _SETUP_UTILITY_BROWSER_PROTOCOL_H_
#define _SETUP_UTILITY_BROWSER_PROTOCOL_H_

//
// Device Manager Setup Protocol GUID
//
#define EFI_SETUP_UTILITY_BROWSER_PROTOCOL_GUID \
{ 0x392744da, 0xdf68, 0x4c3b, 0x96, 0x6b, 0xf2, 0xf, 0xf, 0x47, 0xbc, 0x23 }

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_SETUP_UTILITY_BROWSER_PROTOCOL EFI_SETUP_UTILITY_BROWSER_PROTOCOL;


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiSetupUtilityBrowserProtocolGuid;

//
// Direction Enumeration
//
typedef enum {
  NoChange,
  Left,
  Right,
  Jump,
} MOVE_TYPE;

//
// Internal menu structure
//
typedef struct {
  VOID   *Page;
  UINT16  MenuTitle;
  CHAR16 *String;
} MENU_ITEM;

#define MAX_ITEMS 10

#define GET_SETUP_CONFIG          0x4756
#define SET_SETUP_CONFIG          0x5356

//
// Mouse behavior definition
//
#define  SETUP_MOUSE_PAGE_UP_DOWN       0x00
#define  SETUP_MOUSE_LINE_UP_DOWN       0x01

//
// Protocol Data Structures
//
struct _EFI_SETUP_UTILITY_BROWSER_PROTOCOL {
  BOOLEAN                                   AtRoot;             // Is the DeviceManager at the root of the tree
  BOOLEAN                                   Finished;           // Has the user hit save and exit?
  BOOLEAN                                   Changed;            // Has any settings been changed?
  BOOLEAN                                   Firstin;            // First time entry SendFrom
  BOOLEAN                                   JumpToFirstOption;  // Jump menu option to first one
  EFI_GUID                                  *Guid;              // GUID of setup variable used in NvRAM
  UINTN                                     Size;               // Size of SCBuffer
  UINT8                                     *SCBuffer;          // Pointer to the SYSTEM_CONFIGURATION of SetupUility
  UINT8                                     *MyIfrNVData;       // Pointer of NvRamMap in OemSetupBrowser engine
  UINTN                                     PreviousMenuEntry;  // Saved previous menu item
  BOOLEAN                                   UseMenus;           // Are the following structures used
  MOVE_TYPE                                 Direction;          // Direction of the menu change
  UINT8                                     CurRoot;            // Index of menu item currently selected
  UINT8                                     MenuItemCount;      // Number of valid menu items
  BOOLEAN                                   ScreenLastCharIsSet;// Flag to check if screen last cahr is set or not.
  MENU_ITEM                                 MenuList[MAX_ITEMS];// Menu item list.
  BOOLEAN                                   SetupMouseFlag;     //Flag of the SetupMouse has initialized or not. to avoid the positiion of curse reset.    
  UINT8                                     SetupMouseBehavior; // SetupMouse Behavior: Page-Down/Page-Up or Line-Down/Line/Up
  UINT8                                     *SetupMouse;                                                                
  UINT8                                     Reload;
};

#endif
