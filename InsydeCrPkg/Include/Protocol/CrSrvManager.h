/** @file
  Console Redirection Service Manager Protocol definition.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CR_SRV_MANAGER_PROTOCOL_H_
#define _CR_SRV_MANAGER_PROTOCOL_H_

#include <Protocol/TerminalEscCode.h>
#include <Protocol/SimpleTextOut.h>

#define CR_SRV_MANAGER_PROTOCOL_GUID \
  { \
    0xb188d5e9, 0x44c9, 0x41f7, 0xae, 0xc6, 0x60, 0xf3, 0x6d, 0x12, 0x1a, 0x34 \
  }

typedef struct _CR_SRV_MANAGER_PROTOCOL CR_SRV_MANAGER_PROTOCOL;


//
// CrSrvManager definition
//
typedef 
EFI_STATUS
(EFIAPI *CR_SRV_MANAGER_ENTRY) (
  IN EFI_HANDLE  NotifiedTerminal
  );

typedef 
EFI_STATUS
(EFIAPI *CR_SRV_NOTIFY_FUNCTION) (
  IN EFI_HANDLE  NotifiedTerminal
  );

typedef
EFI_STATUS
(EFIAPI *CR_SRV_REGISTER_NOTIFY) (
  IN  CR_SRV_MANAGER_PROTOCOL            *This,
  IN  CHAR16                            *ServiceName,
  IN  CR_SRV_NOTIFY_FUNCTION            CrSrvNotifyFunc,
  OUT EFI_HANDLE                        *NotifyHandle
  );

typedef 
VOID
(EFIAPI *CR_SRV_BDS_CALLBACK) (VOID);

//
// CrConOutSrv definition
//
typedef struct {
  INT16   X;
  INT16   Y;
  UINT16  Width;
  UINT16  Height;
} RECT;

typedef struct {
  RECT                             Area;
  RECT                             ClientArea;
  BOOLEAN                          Border;
  BOOLEAN                          IsShow;
  UINT16                           CursorX;
  UINT16                           CursorY;
  CHAR16                           *Title;
  VOID                             *TextBuffer;
} CR_SRV_WINDOW; 

typedef
VOID
(EFIAPI *CR_SRV_WIN_SETCONOUT) (
  IN  CR_SRV_WINDOW   *This,
  IN  BOOLEAN         CRedirection
  );

typedef
EFI_STATUS
(EFIAPI *CR_SRV_WIN_CREATE) (
  INT16             X,
  INT16             Y,
  UINT16            CWidth,
  UINT16            CHeight,
  BOOLEAN           Border,
  CHAR16            *Title,
  CR_SRV_WINDOW     **AWin
  );

typedef
EFI_STATUS
(EFIAPI *CR_SRV_WIN_DESTROY) (
  CR_SRV_WINDOW  *Win
  );

typedef
EFI_STATUS
(EFIAPI *CR_SRV_WIN_SHOW) (
  CR_SRV_WINDOW  *Win
  );

typedef
VOID
(EFIAPI *CR_SRV_WIN_CLEARLINE) (
  CR_SRV_WINDOW  *Win,
  UINTN          Attrib,
  UINT16         LineNumber
  );

typedef
VOID
(EFIAPI *CR_SRV_WIN_PRINTCHAR) (
  CR_SRV_WINDOW   *Win,
  CHAR16          Ch
  );

typedef
EFI_STATUS
(EFIAPI *CR_SRV_WIN_PRINTAT) (
  CR_SRV_WINDOW   *Win,
  UINT16          X,
  UINT16          Y,
  CHAR16          *Format,
  ...  
  );

typedef
VOID
(EFIAPI *CR_SRV_WIN_TEXTOUT) (
  CR_SRV_WINDOW     *Win,
  INT16              X,
  INT16              Y,
  CHAR16             *Str
  );


typedef
VOID 
(EFIAPI *CR_SRV_WIN_GETCURSOR) (
  CR_SRV_WINDOW    *Win,
  UINT16           *CursorX,
  UINT16           *CursorY
  );

typedef
VOID
(EFIAPI *CR_SRV_WIN_SETCURSOR) (
  CR_SRV_WINDOW    *Win,
  UINT16           CursorX,
  UINT16           CursorY
  );


typedef struct _CR_SRV_MANAGER_PROTOCOL {
  // Service manager
  CR_SRV_REGISTER_NOTIFY    CrSrvRegNotify;
  CR_SRV_MANAGER_ENTRY      CrSrvManagerEntry;
  CR_SRV_BDS_CALLBACK       CrSrvBdsCallback;

  // Window services
  CR_SRV_WIN_CREATE         CrSrvWinCreate;
  CR_SRV_WIN_DESTROY        CrSrvWinDestroy;
  CR_SRV_WIN_SHOW           CrSrvWinShow;
  CR_SRV_WIN_CLEARLINE      CrSrvWinClearLine;
  CR_SRV_WIN_PRINTCHAR      CrSrvWinPrintChar;
  CR_SRV_WIN_PRINTAT        CrSrvWinPrintAt;
  CR_SRV_WIN_TEXTOUT        CrSrvWinTextOut;
  CR_SRV_WIN_GETCURSOR      CrSrvWinGetCursor;
  CR_SRV_WIN_SETCURSOR      CrSrvWinSetCursor;
  CR_SRV_WIN_SETCONOUT      CrSrvWinSetConOut;
} CR_SRV_MANAGER_PROTOCOL;

extern EFI_GUID gCrSrvManagerProtocolGuid;

#endif // _CR_SRV_MANAGER_PROTOCOL_H_

