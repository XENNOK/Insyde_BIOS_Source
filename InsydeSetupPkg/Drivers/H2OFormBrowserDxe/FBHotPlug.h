/** @file
  Header file for hot plug support

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

#ifndef _FB_HOT_PLUG_H_
#define _FB_HOT_PLUG_H_

#include "InternalH2OFormBrowser.h"
#include "FBConSplitter.h"

EFI_STATUS
EFIAPI
FBHotPlugDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL              *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
FBHotPlugDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL              *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
FBHotPlugDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN UINTN                                 NumberOfChildren,
  IN EFI_HANDLE                            *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
FBHotPlugComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL          *This,
  IN  CHAR8                                *Language,
  OUT CHAR16                               **DriverName
  );

EFI_STATUS
EFIAPI
FBHotPlugComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL          *This,
  IN  EFI_HANDLE                           ControllerHandle,
  IN  EFI_HANDLE                           ChildHandle OPTIONAL,
  IN  CHAR8                                *Language,
  OUT CHAR16                               **ControllerName
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInReset (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL        *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL       *This,
  OUT EFI_INPUT_KEY                        *Key
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInExReset (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInExReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  OUT EFI_KEY_DATA                         *KeyData
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInExSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_KEY_TOGGLE_STATE                  *KeyToggleState
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInExRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  IN  EFI_KEY_DATA                         *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
  OUT EFI_HANDLE                           *NotifyHandle
  );

EFI_STATUS
EFIAPI
FakeSimpleTextInExUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_HANDLE                            NotificationHandle
  );

EFI_STATUS
EFIAPI
FakeSimplePointerReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FakeSimplePointerGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN OUT EFI_SIMPLE_POINTER_STATE          *State
  );

EFI_STATUS
EFIAPI
FakeAbsolutePointerReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL         *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FakeAbsolutePointerGetState (
  IN     EFI_ABSOLUTE_POINTER_PROTOCOL     *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE        *State
  );

EFI_STATUS
FBHotPlugInit (
  VOID
  );

EFI_STATUS
FBHotPlugShutdown (
  VOID
  );

EFI_STATUS
FBHotPlugEventFunc (
  VOID
  );

#endif
