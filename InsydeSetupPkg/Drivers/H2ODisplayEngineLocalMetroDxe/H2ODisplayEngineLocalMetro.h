/** @file
 Function definition for H2O local Metro display engine

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

#ifndef _H2O_DISPLAY_ENGINE_LOCAL_METRO_H_
#define _H2O_DISPLAY_ENGINE_LOCAL_METRO_H_


#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Guid/H2ODisplayEngineType.h>
#include <Protocol/H2ODisplayEngine.h>
#include <Protocol/SetupMouse.h>
#include <Library/Win32Lib.h>
#include <Library/LayoutLib.h>
#include <Library/DxeOemSvcKernelLib.h>

#define H2O_DISPLAY_ENGINE_SIGNATURE SIGNATURE_32 ('H', 'D', 'E', 'S')


#define H2O_FORM_BROWSER_CONSOLE_DEV_NODE_SIGNATURE SIGNATURE_32 ('F', 'B', 'C', 'D')

typedef struct _H2O_FORM_BROWSER_CONSOLE_DEV_NODE {
  UINT32                                       Signature;
  H2O_FORM_BROWSER_CONSOLE_DEV                 *ConsoleDev;
  LIST_ENTRY                                   Link;
  EFI_HANDLE                                   Handle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL              *SimpleTextOut;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                 *GraphicsOut;
} H2O_FORM_BROWSER_CONSOLE_DEV_NODE;

#define H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK(a) CR (a, H2O_FORM_BROWSER_CONSOLE_DEV_NODE, Link, H2O_FORM_BROWSER_CONSOLE_DEV_NODE_SIGNATURE)



typedef struct _H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA {

  UINT32                                       Signature;
  EFI_HANDLE                                   ImageHandle;

  H2O_DISPLAY_ENGINE_PROTOCOL                  DisplayEngine;
  H2O_FORM_BROWSER_PROTOCOL                    *FBProtocol;

  LIST_ENTRY                                   ConsoleDevListHead;

} H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA;

#define H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA_FROM_PROTOCOL(a) CR (a, H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA, DisplayEngine, H2O_DISPLAY_ENGINE_SIGNATURE)

LRESULT
CALLBACK
DisplayEngineProc (
  HWND HWnd,
  UINT message,
  WPARAM WParam,
  LPARAM LParam
  );

EFI_STATUS
EFIAPI
LocalMetroNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  );

#define DISPLAY_ENGINE_CLASS_NAME  L"DisplayEngine"

EFI_STATUS
InitializeGUI (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput
  );

extern H2O_FORM_BROWSER_PROTOCOL       *gFB;
extern H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA      *mMetroPrivate;


#define H2O_METRO_DE_TIMER_PERIODIC_TIME            10000 // The number of 100ns units. Current setting: 1ms
#define H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER        0

EFI_STATUS
RegisterTimerEvent (
  IN INT32                                 TimerId,
  IN UINT64                                TriggerTime
  );

#endif

