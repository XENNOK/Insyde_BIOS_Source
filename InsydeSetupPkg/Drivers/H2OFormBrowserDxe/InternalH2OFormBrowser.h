/** @file
 Internal include file for H2O Formbrowser

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

#ifndef _INTERNALH2O_FORM_BROWSER_H_
#define _INTERNALH2O_FORM_BROWSER_H_
#include <Uefi.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/H2OFormBrowser.h>
#include <Protocol/H2ODisplayEngine.h>
#include <Protocol/FormBrowser2.h>
#include <Guid/H2ODisplayEngineType.h>
#include "Setup.h"
#include "Timer.h"
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/SetupMouse.h>
#include <Protocol/DriverBinding.h>
#include <Guid/ConsoleInDevice.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/UsbEnumeration.h>
#include "FBDialog.h"
#include "FBHotPlug.h"
#include <Protocol/H2OKeyDesc.h>
#include <Guid/BdsHii.h>
#include "FBHotPlug.h"

extern UI_MENU_SELECTION  *gCurrentSelection;

typedef struct _UI_MENU_SELECTION                 UI_MENU_SELECTION;
typedef struct _H2O_INPUT_EVENT_DESCRIPTION H2O_INPUT_EVENT_DESCRIPTION;
typedef struct _FORM_BROWSER_STATEMENT            FORM_BROWSER_STATEMENT;

typedef struct _STATEMENT_REFRESH_ENTRY {
  LIST_ENTRY                  Link;
  FORM_BROWSER_STATEMENT      *Statement;  //Refresh statement
  H2O_PAGE_ID                 PageId;      //Refresh page ID
} STATEMENT_REFRESH_ENTRY;


#define H2O_FORM_BROWSER_SIGNATURE SIGNATURE_32 ('F', 'B', 'P', 'D')
typedef struct _H2O_FORM_BROWSER_PRIVATE_DATA {
  UINT32                                       Signature;
  EFI_HANDLE                                   ImageHandle;
  H2O_FORM_BROWSER_PROTOCOL                    FB;
  EFI_FORM_BROWSER2_PROTOCOL                   FormBrowser2;
  EFI_FORM_BROWSER_EXTENSION_PROTOCOL          FormBrowserEx;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL       FormBrowserEx2;


  H2O_DISPLAY_ENGINE_PROTOCOL                  **EngineList;
  UINT32                                        EngineListCount;

  H2O_DISPLAY_ENGINE_PROTOCOL                  *ActivatedEngine;

  LIST_ENTRY                                   TimerList;
  LIST_ENTRY                                   RefreshList;

  //
  // Setup Mouse
  //
  EFI_SETUP_MOUSE_PROTOCOL                     *SetupMouse;


  //
  // edk2 private data
  //
  LIST_ENTRY                                   FormSetList;
  EFI_HANDLE                                   PackageNotifyHandle;

  //
  // H2O FormBrowser data
  //
  BOOLEAN                                      ConsoleInitComplete;
  UINT32                                       PageCount;
  H2O_PAGE_ID                                  *PageIdList;

  //
  // For H2O Setup
  //
  EFI_HII_HANDLE                              *HiiHandleList;
  UINT32                                       HiiHandleCount;
  UINT32                                       HiiHandleIndex;

  SETUP_MENU_INFO                             *SetupMenuInfo;
  UINT32                                       NumberOfSetupMenus;

  HOT_KEY_INFO                                *HotKeyInfo;
  UINT32                                       NumberOfHotKeys;
} H2O_FORM_BROWSER_PRIVATE_DATA;

extern H2O_FORM_BROWSER_PRIVATE_DATA mFBPrivate;
extern EFI_GUID                      mScuFormSetGuid;
extern EFI_GUID                      mSecureBootMgrFormSetGuid;

#define H2O_FORM_BROWSER_DATA_FROM_PROTOCOL(a) CR (a, H2O_FORM_BROWSER_PRIVATE_DATA, FB, H2O_FORM_BROWSER_SIGNATURE)
#define FORM_BROWSER2_DATA_FROM_PROTOCOL(a)    CR (a, H2O_FORM_BROWSER_PRIVATE_DATA, FormBrowser2, H2O_FORM_BROWSER_SIGNATURE)

#define H2O_DISPLAY_ENGINE_EVENT_NODE_SIGNATURE SIGNATURE_32 ('D', 'E', 'E', 'N')

typedef struct _H2O_DISPLAY_ENGINE_EVENT_NODE {
  UINT32                                       Signature;   ///<Particular statement signature. This signature must be 'D', 'E', 'E', 'N'
  LIST_ENTRY                                   Link;        ///<Event node list entry
  H2O_DISPLAY_ENGINE_EVT                       *Event;      ///<H2O display engine event
} H2O_DISPLAY_ENGINE_EVENT_NODE;

#define H2O_DISPLAY_ENGINE_EVENT_NODE_FROM_LINK(a) CR (a, H2O_DISPLAY_ENGINE_EVENT_NODE, Link, H2O_DISPLAY_ENGINE_EVENT_NODE_SIGNATURE)


typedef struct _H2O_INPUT_EVENT_DESCRIPTION {
  EFI_DEVICE_PATH                              *DevicePath;  //Input event device path
  VOID                                         *Protocol;    //Input event protocol
  H2O_CONSOLE_DEVICE_TYPE                      DeviceType;   //Input event device type
  H2O_CONSOLE_ID                               ConsoleId;    //Input event console id
  EFI_HANDLE                                   Handle;       //Input event handle
} H2O_INPUT_CHECK_EVENT_DESCRIPTION;

typedef struct _CONSOLE_TYPE_MAP {
  EFI_GUID                    *Guid;
  H2O_CONSOLE_DEVICE_TYPE     Type;
} CONSOLE_TYPE_MAP;

extern CONSOLE_TYPE_MAP                             mConsoleMap[];
extern H2O_FORM_BROWSER_CONSOLE_DEV                 *mConsoleDevList;
extern UINT32                                       mConsoleDevListCount;
extern EFI_EVENT                                    *mInputEventList;
extern H2O_INPUT_EVENT_DESCRIPTION                  *mInputEventDescList;
extern UINT32                                       mInputEventListCount;
extern EFI_UNICODE_COLLATION_PROTOCOL               *mUnicodeCollation;
extern EFI_USER_MANAGER_PROTOCOL                    *mUserManager;
EFI_STATUS
QueueEvent (
  H2O_DISPLAY_ENGINE_EVT        *Event
  );

BOOLEAN
FBKillTimer (
  IN  INT32                    Target,
  IN  UINT32                   TimerId
  );

//
// Protocol definitions
//

EFI_STATUS
FBInitPageInfo (
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  );

EFI_STATUS
FBRepaint (
  IN H2O_FORM_BROWSER_PRIVATE_DATA            *Private
  );

FORM_BROWSER_FORM *
FBPageIdToForm (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN H2O_PAGE_ID                                 PageId
  );

//
// FBConsole.c
//
EFI_STATUS
FBSetActivedEngine (
  H2O_FORM_BROWSER_PRIVATE_DATA          *Private,
  H2O_CONSOLE_ID                         ConsoleId
  );

EFI_STATUS
EFIAPI
FBInitConsoles (
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  );

EFI_STATUS
EFIAPI
FBDetachConsoles (
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  );

EFI_STATUS
FBFreeConsoleDevList (
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  );

VOID
EFIAPI
FBIsConsoleInit (
 BOOLEAN                             *ConsoleInitComplete,
 H2O_FORM_BROWSER_PRIVATE_DATA       **Private
 );

VOID
GetConsoleInfo (
 H2O_FORM_BROWSER_CONSOLE_DEV **ConsoleDevList,
 UINT32                        *ConsoleDevListCount
 );

BOOLEAN
IsConsoleDeviceType (
  IN EFI_HANDLE                            Handle
  );

BOOLEAN
IsConInDeviceType (
  IN H2O_CONSOLE_DEVICE_TYPE               DeviceType
  );

BOOLEAN
IsConOutDeviceType (
  IN H2O_CONSOLE_DEVICE_TYPE               DeviceType
  );

H2O_FORM_BROWSER_CONSOLE_DEV *
EFIAPI
FBAddConsoleDevice (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN EFI_HANDLE                                  Handle
  );

EFI_STATUS
EFIAPI
FBRemoveConsoleDevice (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN EFI_HANDLE                                  Handle
  );

H2O_CONSOLE_DEVICE_TYPE
FBGetDeviceType (
  EFI_HANDLE              Handle
  );

//
// FBEvent.c
//
EFI_STATUS
FBBroadcastEvent (
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  );

EFI_STATUS
FBTimer (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  );

VOID
CheckTimerEvent (
  VOID
  );

CHAR16 *
EventToStr (
  IN CONST H2O_EVT_TYPE EvtType
  );

BOOLEAN
GetNextEvent (
  H2O_DISPLAY_ENGINE_EVT        **Event
  );

EFI_STATUS
DestroyPageInfo (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  );

EFI_STATUS
FBInitSMInfo (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  );

EFI_STATUS
DestroySetupMenuInfo (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  );

EFI_STATUS
DestroyHotKeyInfo (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  );

EFI_STATUS
DestroyFormSetList (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  );

VOID
DestroyEventQueue (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  );


EFI_STATUS
EFIAPI
FormUpdateNotify (
  IN UINT8                              PackageType,
  IN CONST EFI_GUID                     *PackageGuid,
  IN CONST EFI_HII_PACKAGE_HEADER       *Package,
  IN EFI_HII_HANDLE                     Handle,
  IN EFI_HII_DATABASE_NOTIFY_TYPE       NotifyType
  );

EFI_STATUS
FBKeyPress (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  );

//
// Statement.c
//
VOID
InitH2OStatement (
  IN     FORM_BROWSER_FORMSET      *FormSet,
  IN     FORM_BROWSER_FORM         *Form,
  IN     EXPRESS_RESULT               ExpressResult,
  IN     H2O_FORM_BROWSER_P        *Page,
  IN OUT FORM_BROWSER_STATEMENT    *Statement
  );

/**
  Initialize Statement header members.

  @param  OpCodeData             Pointer of the raw OpCode data.
  @param  FormSet                Pointer of the current FormSet.
  @param  Form                   Pointer of the current Form.

  @return The Statement.

**/
FORM_BROWSER_STATEMENT *
CreateStatement (
  IN UINT8                        *OpCodeData,
  IN OUT FORM_BROWSER_FORMSET     *FormSet,
  IN OUT FORM_BROWSER_FORM        *Form
  )
  ;

/**
  Free resources of a Statement.

  @param  FormSet                Pointer of the FormSet
  @param  Statement              Pointer of the Statement

**/
VOID
DestroyStatement (
  IN     FORM_BROWSER_FORMSET    *FormSet,
  IN OUT FORM_BROWSER_STATEMENT  *Statement
  )
  ;

//
// Page.c
//
VOID
InitPage (
  IN     H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN     FORM_BROWSER_FORMSET                        *FormSet,
  IN     FORM_BROWSER_FORM                           *Form,
  IN OUT H2O_FORM_BROWSER_P                          *Page
  );
//
// Value.c
//
EFI_STATUS
CreateValueAsBoolean (
  IN EFI_HII_VALUE *TargetHiiValue,
  IN BOOLEAN        ValueBoolean
  );

EFI_STATUS
CreateValueAsUint64 (
  IN EFI_HII_VALUE *TargetHiiValue,
  IN UINT64         ValueUint64
  );

EFI_STATUS
CreateValueAsString (
  IN EFI_HII_VALUE  *TargetHiiValue,
  IN UINT16          BufferLen,
  IN UINT8          *Buffer
  );
VOID
CopyHiiValue (
  IN EFI_HII_VALUE         *SourceHiiValue,
  IN EFI_HII_VALUE         *DestinationHiiValue
  );
VOID
CopyValue (
  IN EFI_HII_VALUE         *DestinationValue,
  IN EFI_HII_VALUE         *SourceValue
  );

EFI_STATUS
SetValueBool(
  IN EFI_HII_VALUE   *SourceHiiValue,
  IN BOOLEAN          ValueBoolean
  );

EFI_STATUS
DumpValue(
IN EFI_HII_VALUE     *DumpHiiValue
 );

//
// Option.c
//
VOID
CopyOption (
  IN     FORM_BROWSER_FORM         *Form,
  IN OUT FORM_BROWSER_STATEMENT    *Statement
  );
//
// H2ODialog.c
//
 EFI_STATUS
 InstallH2ODialogProtocol (
   EFI_HANDLE  ImageHandle
   );

/**
  Free up the resource allocated for all strings required
  by Setup Browser.

**/
VOID
FreeBrowserStrings (
  VOID
  );

//
// Uefi.c
//
VOID
FBFreePool (
  IN VOID **Buffer
  );

//
// Ifrparse.c
//
/**
  Free resources of a Expression.

  @param  Expression                Pointer of the Expression

**/
VOID
DestroyExpression (
  IN FORM_EXPRESSION   *Expression
  )
  ;

VOID
DestroyH2OStatement (
  IN OUT FORM_BROWSER_STATEMENT *Statement
  );

EFI_STATUS
ProcessUserInput (
  IN USER_INPUT               *UserInput,
  IN BOOLEAN                  ChangeHighlight
  );

EFI_STATUS
ProcessAction (
  IN UINT32        Action,
  IN UINT16        DefaultId
  );

BOOLEAN
IsSelectable (
  IN FORM_BROWSER_STATEMENT        *Statement,
  IN UINT32                        DisplayAttribute
  );

#endif

