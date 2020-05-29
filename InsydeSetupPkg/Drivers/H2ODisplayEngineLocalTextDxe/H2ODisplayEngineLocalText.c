/** @file
  Entry point and initial functions for H2O local text display engine driver

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

#include "H2ODisplayEngineLocalText.h"

BOOLEAN                                    mIsInNotifyProcess;

/**
  Add new console to the console list.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleHandle      A pointer to the input console handle instance.

  @retval EFI_SUCCESS            Add new console to console list successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextInitConsole (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  )
{
  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Attach a specific console to this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Attach new console successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.

**/
EFI_STATUS
EFIAPI
LocalTextAttachConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL           *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV          *ConsoleDev
  )
{
  H2O_DISPLAY_ENGINE_PRIVATE_DATA          *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE        *ConDevNode;
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *SimpleTextOut;
  INT32                                    Mode;
  UINTN                                    Columns;
  UINTN                                    Rows;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ConsoleDev->Handle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **) &SimpleTextOut
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private = H2O_DISPLAY_ENGINE_PRIVATE_DATA_FROM_PROTOCOL (This);

  ConDevNode                = (H2O_FORM_BROWSER_CONSOLE_DEV_NODE *) AllocatePool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV_NODE));
  ConDevNode->Signature     = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_SIGNATURE;
  ConDevNode->ConsoleDev    = AllocateCopyPool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV), ConsoleDev);
  ConDevNode->SimpleTextOut = SimpleTextOut;
  ConDevNode->Handle        = ConsoleDev->Handle;
  ConDevNode->ConDevStatus  = CONSOLE_DEVICE_STATUS_INIT;
  InsertTailList (&Private->ConsoleDevListHead, &ConDevNode->Link);

  for (Mode = 0; Mode < SimpleTextOut->Mode->MaxMode; Mode++) {
    SimpleTextOut->QueryMode (SimpleTextOut, Mode, &Columns, &Rows);
    if ((Columns == (UINTN) PcdGet32 (PcdDisplayEngineLocalTextColumn)) &&
        (Rows    == (UINTN) PcdGet32 (PcdDisplayEngineLocalTextRow))) {
      if (SimpleTextOut->Mode->Mode != Mode) {
        SimpleTextOut->SetMode (SimpleTextOut, Mode);
      }
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
  Detach a specific console from this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Detach a console device from the device engine successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_NOT_FOUND          The input device console isn't attached to the display engine.

**/
EFI_STATUS
EFIAPI
LocalTextDetachConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL           *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV          *ConsoleDev
  )
{
  EFI_STATUS                               Status;
  H2O_DISPLAY_ENGINE_PRIVATE_DATA          *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE        *ConDevNode;
  LIST_ENTRY                               *Link;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = H2O_DISPLAY_ENGINE_PRIVATE_DATA_FROM_PROTOCOL (This);

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    return EFI_NOT_FOUND;
  }

  //
  // If call this function during processing notify function, replace fake protocol on this console device.
  // It can make sure that interruped notify function can be executed successfully.
  // After interruped notify function is finished, remove the console device.
  //
  if (mIsInNotifyProcess) {
    Status = DEReplaceFakeConOutDev (Private, ConsoleDev->Handle);
    return Status;
  }

  Link = Private->ConsoleDevListHead.ForwardLink;
  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    if (ConsoleDev->ConsoleId == ConDevNode->ConsoleDev->ConsoleId) {
      //
      // Remove Console Device Node
      //
      RemoveEntryList (&ConDevNode->Link);
      FreePool (ConDevNode->ConsoleDev);
      FreePool (ConDevNode);
      DEBUG ((EFI_D_INFO, "Device is detached\n"));
      return EFI_SUCCESS;
    }
    //
    // Get Next Console
    //
    if (IsNodeAtEnd (&Private->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  }

  return EFI_NOT_FOUND;
}

/**
  Report which console devices are supported by this display engine.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev          A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Report supported console device type successful.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.
  @retval EFI_INVALID_PARAMETER  This is NULL or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextSupportConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL           *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV          *ConsoleDev
  )
{
  EFI_STATUS                               Status;
  EFI_DEVICE_PATH_PROTOCOL                 *DevPath;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *SimpleTextOut;
  INT32                                    Mode;
  UINTN                                    Columns;
  UINTN                                    Rows;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevPath = ConsoleDev->DevicePath;
  if (DevPath->Type == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->HandleProtocol (
                  ConsoleDev->Handle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **) &SimpleTextOut
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Mode = 0; Mode < SimpleTextOut->Mode->MaxMode; Mode++) {
    SimpleTextOut->QueryMode (SimpleTextOut, Mode, &Columns, &Rows);
    if ((Columns == (UINTN) PcdGet32 (PcdDisplayEngineLocalTextColumn)) &&
        (Rows    == (UINTN) PcdGet32 (PcdDisplayEngineLocalTextRow))) {
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

/**
  Add the notification to the notification queue and signal the Notification event.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] Notify             A pointer to the H2O_DISPLAY_ENGINE_EVT instance.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  )
{
  EFI_STATUS                                  Status;

  mIsInNotifyProcess = TRUE;
  Status = DEEventCallback (This, Notify);
  mIsInNotifyProcess = FALSE;

  DERemoveFakeConOutDev (mDEPrivate);

  return Status;
}

/**
  Initizlize private data for local text display engine and install display engine protocol

  @param [in] ImageHandle        The image handle
  @param [in] SystemTable        The system table

  @retval EFI_SUCCESS            Success to initialize private data and install display engine protocol
  @retval EFI_OUT_OF_RESOURCES   Display engine private data allocate fail
  @retval Other                  Fail to initialize display engine private data

**/
EFI_STATUS
EFIAPI
H2ODisplayEngineLocalTextEntryPoint (
  IN     EFI_HANDLE                            ImageHandle,
  IN     EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                               Status;
  EFI_GUID                                 Guid = H2O_DISPLAY_ENGINE_LOCAL_TEXT_GUID;


  mDEPrivate = (H2O_DISPLAY_ENGINE_PRIVATE_DATA*) AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_PRIVATE_DATA));
  if (mDEPrivate == NULL) {
    DEBUG ((EFI_D_INFO, "H2ODisplayEngineLocalTextEntryPoint() mDEPrivate allocate pool fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  mDEPrivate->Signature   = H2O_DISPLAY_ENGINE_SIGNATURE;
  mDEPrivate->ImageHandle = ImageHandle;

  mDEPrivate->DisplayEngine.Size           = (UINT32) sizeof (H2O_DISPLAY_ENGINE_PROTOCOL);
  CopyGuid (&mDEPrivate->DisplayEngine.Id, &Guid);
  mDEPrivate->DisplayEngine.InitConsole    = LocalTextInitConsole;
  mDEPrivate->DisplayEngine.AttachConsole  = LocalTextAttachConsole;
  mDEPrivate->DisplayEngine.DetachConsole  = LocalTextDetachConsole;
  mDEPrivate->DisplayEngine.SupportConsole = LocalTextSupportConsole;
  mDEPrivate->DisplayEngine.Notify         = LocalTextNotify;

  InitializeListHead (&mDEPrivate->ConsoleDevListHead);

  //
  // Locate FBProtocol
  //
  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &mDEPrivate->FBProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[H2ODisplayEngineLib] locate protocol fail in\n"));
    DEBUG ((EFI_D_ERROR, "H2ODisplayEngineLocalTextEntryPoint function\n"));
  }
  ASSERT_EFI_ERROR (Status);

  //
  // Init Display Engine Status
  //
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_INIT;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gH2ODisplayEngineProtocolGuid,
                  &mDEPrivate->DisplayEngine,
                  NULL
                  );

  return Status;
}
