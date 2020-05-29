/** @file
  Hot plug support for H2O form browser

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

#include "FBHotPlug.h"
#include <Guid/ReturnFromImage.h>

EFI_EVENT                                  mReadyToBootEvent;
BOOLEAN                                    mTriggerByFB;
EFI_HANDLE                                 *mFBHotPlugAddConsoleDevList;
UINT32                                     mFBHotPlugAddConsoleDevCount;
EFI_HANDLE                                 *mFBHotPlugRemoveConsoleDevList;
UINT32                                     mFBHotPlugRemoveConsoleDevCount;

EFI_GUID                                   *mFBHotPlugGuidList[] = {
  &gEfiConsoleInDeviceGuid,
  &gEfiConsoleOutDeviceGuid,
  //
  // SetupMouse will produce AbsolutePointer and SimplePointer
  //
//&gEfiAbsolutePointerProtocolGuid,
//&gEfiSimplePointerProtocolGuid
  };

EFI_DRIVER_BINDING_PROTOCOL                mFBHotPlugDriverBinding = {
  FBHotPlugDriverBindingSupported,
  FBHotPlugDriverBindingStart,
  FBHotPlugDriverBindingStop,
  0x9,
  NULL,
  NULL
  };

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  mFBHotPlugComponentName = {
  FBHotPlugComponentNameGetDriverName,
  FBHotPlugComponentNameGetControllerName,
  "eng"
  };

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL mFBHotPlugComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)     FBHotPlugComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) FBHotPlugComponentNameGetControllerName,
  "en"
  };

GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mFBHotPlugDriverNameTable[] = {
  {
    "eng;en",
    L"H2O Form Browser Driver"
  },
  {
    NULL,
    NULL
  }
  };

EFI_SIMPLE_TEXT_INPUT_PROTOCOL             mFakeSimpleInput = {
  FakeSimpleTextInReset,
  FakeSimpleTextInReadKeyStroke,
  NULL
  };

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL          mFakeSimpleInputEx = {
  FakeSimpleTextInExReset,
  FakeSimpleTextInExReadKeyStrokeEx,
  NULL,
  FakeSimpleTextInExSetState,
  FakeSimpleTextInExRegisterKeyNotify,
  FakeSimpleTextInExUnregisterKeyNotify
  };

EFI_SIMPLE_POINTER_MODE                    mFakeSimplePointerMode;
EFI_SIMPLE_POINTER_PROTOCOL                mFakeSimplePointer = {
  FakeSimplePointerReset,
  FakeSimplePointerGetState,
  NULL,
  &mFakeSimplePointerMode
  };

EFI_ABSOLUTE_POINTER_MODE                  mAbsolutePointerMode;
EFI_ABSOLUTE_POINTER_PROTOCOL              mAbsolutePointer = {
  FakeAbsolutePointerReset,
  FakeAbsolutePointerGetState,
  NULL,
  &mAbsolutePointerMode
  };

EFI_STATUS
EFIAPI
FakeSimpleTextInReset (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL        *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL       *This,
  OUT EFI_INPUT_KEY                        *Key
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInExReset (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInExReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  OUT EFI_KEY_DATA                         *KeyData
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInExSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_KEY_TOGGLE_STATE                  *KeyToggleState
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInExRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  IN  EFI_KEY_DATA                         *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
  OUT EFI_HANDLE                           *NotifyHandle
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimpleTextInExUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_HANDLE                            NotificationHandle
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimplePointerReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeSimplePointerGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN OUT EFI_SIMPLE_POINTER_STATE          *State
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeAbsolutePointerReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL         *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeAbsolutePointerGetState (
  IN     EFI_ABSOLUTE_POINTER_PROTOCOL     *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE        *State
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a Unicode string.
  If the driver specified by This has a user readable name in the language specified by Language,
  then a pointer to the driver name is returned in DriverName, and EFI_SUCCESS is returned.
  If the driver specified by This does not support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param[in]  This              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Language          A pointer to a Null-terminated ASCII string array indicating the language.
                                This is the language of the driver name that the caller is requesting,
                                and it must match one of the languages specified in SupportedLanguages.
                                The number of languages supported by a driver is up to the driver writer.
                                Language is specified in RFC 4646 or ISO 639-2 language code format.
  @param[out] DriverName        A pointer to the Unicode string to return. This Unicode string is the name of the
                                driver specified by This in the language specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was returned in DriverName.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER DriverName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the language specified by Language.

**/
EFI_STATUS
EFIAPI
FBHotPlugComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL          *This,
  IN  CHAR8                                *Language,
  OUT CHAR16                               **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mFBHotPlugDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &mFBHotPlugComponentName)
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by ControllerHandle and
  ChildHandle in the form of a Unicode string. If the driver specified by This has a user readable name in the
  language specified by Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently managing the controller specified
  by ControllerHandle and ChildHandle, then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param[in]  This              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  ControllerHandle  The handle of a controller that the driver specified by This is managing.
                                This handle specifies the controller whose name is to be returned.
  @param[in]  ChildHandle       The handle of the child controller to retrieve the name of.This is an optional
                                parameter that may be NULL. It will be NULL for device drivers. It will also be NULL
                                for a bus drivers that wish to retrieve the name of the bus controller.
                                It will not be NULL for a bus driver that wishes to retrieve the name of a child controller.
  @param[in]  Language          A pointer to a Null-terminated ASCII string array indicating the language.
                                This is the language of the driver name that the caller is requesting, and it must
                                match one of the languages specified in SupportedLanguages. The number of languages
                                supported by a driver is up to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.
  @param[out] ControllerName    A pointer to the Unicode string to return. This Unicode string is the name of the
                                controller specified by ControllerHandle and ChildHandle in the language specified by
                                Language from the point of view of the driver specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in the language specified by Language for
                                the driver specified by This was returned in DriverName.
  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER ControllerName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This is not currently managing the controller specified by
                                ControllerHandle and ChildHandle.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the language specified by Language.

**/
EFI_STATUS
EFIAPI
FBHotPlugComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL          *This,
  IN  EFI_HANDLE                           ControllerHandle,
  IN  EFI_HANDLE                           ChildHandle OPTIONAL,
  IN  CHAR8                                *Language,
  OUT CHAR16                               **ControllerName
  )
{
  return EFI_UNSUPPORTED;
}

BOOLEAN
IsConsoleDevPresent (
  IN EFI_HANDLE                            Handle
  )
{
  UINT32                                   Index;

  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    if (mConsoleDevList[Index].Handle == Handle) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Add console device into target console device list

  @param[in]      ConsoleHandle       Console device handle
  @param[in, out] ConsoleDevList      Pointer to output console device list
  @param[in, out] ConsoleDevCount     Pointer to the number of output console device list

  @retval EFI_SUCCESS                 Add console device successfully
  @retval EFI_INVALID_PARAMETER       ConsoleDevList or ConsoleDevCount is NULL
  @retval EFI_OUT_OF_RESOURCES        Allocate pool fail

**/
EFI_STATUS
AddConsoleIntoList (
  IN     EFI_HANDLE                        ConsoleHandle,
  IN OUT EFI_HANDLE                        **ConsoleDevList,
  IN OUT UINT32                            *ConsoleDevCount
  )
{
  EFI_HANDLE                               *NewList;

  if (ConsoleDevList == NULL || ConsoleDevCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NewList = AllocatePool ((*ConsoleDevCount + 1) * sizeof (EFI_HANDLE));
  if (NewList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (*ConsoleDevList != NULL) {
    CopyMem (NewList, *ConsoleDevList, *ConsoleDevCount * sizeof (EFI_HANDLE));
    FreePool (*ConsoleDevList);
  }
  NewList[*ConsoleDevCount] = ConsoleHandle;

  *ConsoleDevList = NewList;
  *ConsoleDevCount += 1;

  return EFI_SUCCESS;
}

/**
  Attach console output device on first supported display engine

  @param[in] ConsoleDev         Pointer to console device

  @retval EFI_SUCCESS                This driver is removed ControllerHandle
  @retval EFI_INVALID_PARAMETER      ConsoleDev is NULL
  @retval EFI_NOT_FOUND              There is no support display engine for this console device

**/
STATIC
EFI_STATUS
AttachToDE (
  IN H2O_FORM_BROWSER_CONSOLE_DEV          *ConsoleDev
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Index;
  H2O_DISPLAY_ENGINE_PROTOCOL              *H2ODisplayEngine;

  if (ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < mFBPrivate.EngineListCount; Index++) {
    H2ODisplayEngine = mFBPrivate.EngineList[Index];

    Status = H2ODisplayEngine->SupportConsole (H2ODisplayEngine, ConsoleDev);
    if (!EFI_ERROR (Status)) {
      ConsoleDev->ConsoleId = (H2O_CONSOLE_ID) (Index + 1);
      CopyGuid (&ConsoleDev->DisplayEngine, &H2ODisplayEngine->Id);
      Status = H2ODisplayEngine->AttachConsole (H2ODisplayEngine, ConsoleDev);
      if (!EFI_ERROR (Status)) {
        break;;
      }
    }
  }

  if (Index == mFBPrivate.EngineListCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Remove console device in form browser.
  For console output device, it will be detach from display engine.
  For non console input device, console device data will be deleted form browser private data
  For console input device, input protocol pointer will be pointed to fake protocol instance.
  It can make sure that inturrupted process can still go until the process is finished.

  @retval EFI_SUCCESS                Stop console device successfully
  @retval EFI_NOT_FOUND              Can not find the console device data by ControllerHandle

**/
STATIC
EFI_STATUS
RemoveConsoleDev (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  UINT32                                   Index;
  H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev;
  H2O_DISPLAY_ENGINE_PROTOCOL              *H2ODisplayEngine;

  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    if (mConsoleDevList[Index].Handle == ControllerHandle) {
      break;
    }
  }
  if (Index == mConsoleDevListCount) {
    return EFI_NOT_FOUND;
  }

  ConsoleDev = &mConsoleDevList[Index];

  if (IsConOutDeviceType (ConsoleDev->DeviceType)) {
    for (Index = 0; Index < mFBPrivate.EngineListCount; Index++) {
      H2ODisplayEngine = mFBPrivate.EngineList[Index];

      if (CompareGuid (&H2ODisplayEngine->Id, &ConsoleDev->DisplayEngine)) {
        H2ODisplayEngine->DetachConsole (H2ODisplayEngine, ConsoleDev);
        break;
      }
    }
  }

  if (!IsConInDeviceType (ConsoleDev->DeviceType)) {
    return FBRemoveConsoleDevice (&mFBPrivate, ControllerHandle);
  }

  AddConsoleIntoList (ControllerHandle, &mFBHotPlugRemoveConsoleDevList, &mFBHotPlugRemoveConsoleDevCount);

  //
  // Replace the protocol pointer to fake protocol instance. Form browser will remove device from private data
  // after the interrupted process is finished.
  //
  for (Index = 0; Index < mInputEventListCount; Index++) {
    if (mInputEventDescList[Index].Handle == ControllerHandle) {
      switch (mInputEventDescList[Index].DeviceType) {

      case H2O_FORM_BROWSER_CONSOLE_STI:
        mInputEventDescList[Index].Protocol = &mFakeSimpleInput;
        break;

      case H2O_FORM_BROWSER_CONSOLE_STI2:
        mInputEventDescList[Index].Protocol = &mFakeSimpleInputEx;
        break;

      case H2O_FORM_BROWSER_CONSOLE_SP:
        mInputEventDescList[Index].Protocol = &mFakeSimplePointer;
        break;

      case H2O_FORM_BROWSER_CONSOLE_AP:
        mInputEventDescList[Index].Protocol = &mAbsolutePointer;
        break;

      default:
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Test to see if console device could be supported on the Controller.

  @param[in] This                  Driver Binding protocol instance pointer.
  @param[in] ControllerHandle      Handle of device to test.
  @param[in] RemainingDevicePath   Optional parameter use to pick a specific child device to start.

  @retval EFI_SUCCESS         This driver supports this device.
  @retval EFI_UNSUPPORTED     This driver does not support this device.

**/
EFI_STATUS
EFIAPI
FBHotPlugDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL              *RemainingDevicePath
  )
{
  EFI_STATUS                               Status;
  VOID                                     *Instance;
  UINT32                                   Index;
  UINT32                                   Count;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Count = sizeof (mFBHotPlugGuidList) / sizeof (EFI_GUID *);
  for (Index = 0; Index < Count; Index++) {
    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    mFBHotPlugGuidList[Index],
                    &Instance,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_BY_DRIVER | EFI_OPEN_PROTOCOL_EXCLUSIVE
                    );
    if (!EFI_ERROR (Status)) {
      gBS->CloseProtocol (
             ControllerHandle,
             mFBHotPlugGuidList[Index],
             This->DriverBindingHandle,
             ControllerHandle
             );
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

/**
  Start hot plug functinoality on device handle.

  @param[in] This                 Driver Binding protocol instance pointer.
  @param[in] ControllerHandle     Handle of device to bind driver to.
  @param[in] RemainingDevicePath  Optional parameter use to pick a specific child device to start.

  @retval EFI_SUCCESS          Hot plug functinoality is added to ControllerHandle.
  @retval EFI_UNSUPPORTED      Hot plug functinoality does not support this device.

**/
EFI_STATUS
EFIAPI
FBHotPlugDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL              *RemainingDevicePath
  )
{
  EFI_STATUS                               Status;
  BOOLEAN                                  OpenSuccess;
  VOID                                     *Instance;
  UINT32                                   Index;
  UINT32                                   Count;

  OpenSuccess = FALSE;
  Count = sizeof (mFBHotPlugGuidList) / sizeof (EFI_GUID *);
  for (Index = 0; Index < Count; Index++) {
    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    mFBHotPlugGuidList[Index],
                    &Instance,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_BY_DRIVER | EFI_OPEN_PROTOCOL_EXCLUSIVE
                    );
    if (!EFI_ERROR (Status)) {
      OpenSuccess = TRUE;
    }
  }
  if (!OpenSuccess) {
    return EFI_UNSUPPORTED;
  }

  FBConsplitterAddDevice (ControllerHandle);

  if (!mTriggerByFB) {
    //
    // Add device handle in list. Form browser will start to install device into private data
    // after current interrupted event is finished.
    //
    AddConsoleIntoList (ControllerHandle, &mFBHotPlugAddConsoleDevList, &mFBHotPlugAddConsoleDevCount);
  }

  return EFI_SUCCESS;
}

/**
  Stop hot plug functionality on ControllerHandle by closing console devcice GUID.

  @param[in] This               Driver Binding protocol instance pointer.
  @param[in] ControllerHandle   Handle of device to stop driver on
  @param[in] NumberOfChildren   Number of Handles in ChildHandleBuffer.
                                If number of children is zero stop the entire bus driver.
  @param[in] ChildHandleBuffer  List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
FBHotPlugDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN UINTN                                 NumberOfChildren,
  IN EFI_HANDLE                            *ChildHandleBuffer
  )
{
  EFI_STATUS                               Status;
  BOOLEAN                                  CloseSuccess;
  UINT32                                   Index;
  UINT32                                   Count;

  FBConsplitterDeleteDevice (ControllerHandle);

  if (!mTriggerByFB) {
    RemoveConsoleDev (ControllerHandle);
  }

  CloseSuccess = FALSE;
  Count = sizeof (mFBHotPlugGuidList) / sizeof (EFI_GUID *);
  for (Index = 0; Index < Count; Index++) {
    Status = gBS->CloseProtocol (
                    ControllerHandle,
                    mFBHotPlugGuidList[Index],
                    This->DriverBindingHandle,
                    ControllerHandle
                    );
    if (!EFI_ERROR (Status)) {
      CloseSuccess = TRUE;
    }
  }
  if (!CloseSuccess) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
FBBootFromImageReturnNotifyFun (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  gBS->CloseEvent (Event);

  FBInitConsoles (&mFBPrivate);
}

VOID
FBHotPlugEvtFunc (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  EFI_EVENT                                ImageReturnEvent;
  VOID                                     *Registration;

  gBS->CloseEvent (Event);

  FBDetachConsoles (&mFBPrivate);

  Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, FBBootFromImageReturnNotifyFun, NULL, &ImageReturnEvent);
  if (!EFI_ERROR (Status)) {
    gBS->RegisterProtocolNotify (&gReturnFromImageGuid, ImageReturnEvent, &Registration);
  }
}

/**
  Initialize hot plug functionality

  @retval EFI_SUCCESS       Initialize hot plug functionality successfully
  @retval other             Install driver binding protocol fail

**/
EFI_STATUS
FBHotPlugInit (
  VOID
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Index;
  UINTN                                    NumberOfHandle;
  EFI_HANDLE                               *HandleBuffer;

  FBConsplitterInit ();

  Status = EfiLibInstallDriverBindingComponentName2 (
             gImageHandle,
             gST,
             &mFBHotPlugDriverBinding,
             gImageHandle,
             &mFBHotPlugComponentName,
             &mFBHotPlugComponentName2
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mTriggerByFB = TRUE;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  &NumberOfHandle,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NumberOfHandle; Index++) {
      if (IsConsoleDeviceType (HandleBuffer[Index])) {
        gBS->ConnectController (HandleBuffer[Index], NULL, NULL, FALSE);
      }
    }
  }
  FBFreePool ((VOID **) &HandleBuffer);
  mTriggerByFB = FALSE;

  FBConsplitterStart ();

  mReadyToBootEvent = NULL;
  EfiCreateEventReadyToBootEx (TPL_CALLBACK, FBHotPlugEvtFunc, NULL, &mReadyToBootEvent);

  return Status;
}

/**
  Close hot plug functionality

  @retval EFI_SUCCESS       Close hot plug functionality successfully
  @retval EFI_UNSUPPORTED   No need to shutdown hot plug functionality
**/
EFI_STATUS
FBHotPlugShutdown (
  VOID
  )
{
  UINT32                                   Index;

  if (mFBHotPlugDriverBinding.DriverBindingHandle == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  if (mReadyToBootEvent != NULL) {
    gBS->CloseEvent (mReadyToBootEvent);
    mReadyToBootEvent = NULL;
  }

  //
  // Undo console devices from mFBHotPlugDriverBinding protocol
  //
  mTriggerByFB = TRUE;
  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    gBS->DisconnectController (mConsoleDevList[Index].Handle, mFBHotPlugDriverBinding.DriverBindingHandle, NULL);
  }
  mTriggerByFB = FALSE;

  gBS->UninstallMultipleProtocolInterfaces (
         gImageHandle,
         &gEfiDriverBindingProtocolGuid,
         &mFBHotPlugDriverBinding,
         &gEfiComponentNameProtocolGuid,
         &mFBHotPlugComponentName,
         &gEfiComponentName2ProtocolGuid,
         &mFBHotPlugComponentName2,
         NULL
         );
  mFBHotPlugDriverBinding.DriverBindingHandle = NULL;
  mFBHotPlugDriverBinding.ImageHandle         = NULL;

  FBConsplitterShutdown ();
  //
  // Bind console devices on original driver binding protocol (ConSplitter), because
  // mFBHotPlugDriverBinding protocol use EFI_OPEN_PROTOCOL_EXCLUSIVE open mode which
  // make original driver binding protocol is disconnected
  //
  gST->ConOut->ClearScreen (gST->ConOut);
  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    gBS->ConnectController (mConsoleDevList[Index].Handle, NULL, NULL, FALSE);
  }
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  gST->ConOut->ClearScreen (gST->ConOut);

  return EFI_SUCCESS;
}

/**
  Process hot plug functionality for new console devices.
  It will add devices in private data and attach on display engine for console output device

  @retval EFI_SUCCESS                Process hot plug functionality successfully

**/
EFI_STATUS
FBHotPlugEventFunc (
  VOID
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Index;
  H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev;
  EFI_TPL                                  OriginalTpl;
  BOOLEAN                                  Refreshed;

  if ((mFBHotPlugAddConsoleDevCount    == 0 || mFBHotPlugAddConsoleDevList    == NULL) &&
      (mFBHotPlugRemoveConsoleDevCount == 0 || mFBHotPlugRemoveConsoleDevList == NULL)) {
    return EFI_NOT_FOUND;
  }

  OriginalTpl = gBS->RaiseTPL (TPL_CALLBACK);

  //
  // Process plug-in device
  //
  Refreshed = FALSE;

  if (mFBHotPlugAddConsoleDevList != NULL) {
    for (Index = 0; Index < mFBHotPlugAddConsoleDevCount; Index++) {
      if (IsConsoleDevPresent (mFBHotPlugAddConsoleDevList[Index])) {
        continue;
      }

      ConsoleDev = FBAddConsoleDevice (&mFBPrivate, mFBHotPlugAddConsoleDevList[Index]);
      if (ConsoleDev == NULL) {
        continue;
      }

      if (IsConOutDeviceType (ConsoleDev->DeviceType)) {
        Status = AttachToDE (ConsoleDev);
        if (EFI_ERROR (Status)) {
          continue;
        }

        if (!Refreshed) {
          FBRepaint (&mFBPrivate);
          Refreshed = TRUE;
        }
      }
    }
  }
  mFBHotPlugAddConsoleDevCount = 0;
  FBFreePool ((VOID **) &mFBHotPlugAddConsoleDevList);

  //
  // Process removed device
  //
  if (mFBHotPlugRemoveConsoleDevList != NULL) {
    for (Index = 0; Index < mFBHotPlugRemoveConsoleDevCount; Index++) {
      FBRemoveConsoleDevice (&mFBPrivate, mFBHotPlugRemoveConsoleDevList[Index]);
    }
  }
  mFBHotPlugRemoveConsoleDevCount = 0;
  FBFreePool ((VOID **) &mFBHotPlugRemoveConsoleDevList);

  gBS->RestoreTPL (OriginalTpl);

  return EFI_SUCCESS;
}

