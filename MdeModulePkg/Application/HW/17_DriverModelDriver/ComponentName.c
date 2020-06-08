/** @file
  ComponentName C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DriverModelDriver_17.h"

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gPciAudioComponentName = {
  PciAudioComponentNameGetDriverName,
  PciAudioComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gPciAudioComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) PciAudioComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) PciAudioComponentNameGetControllerName,
  "en"
};


GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mPciAudioDriverNameTable[] = {
  { "eng;en", L"Pci Audio Driver" },
  { NULL , NULL }
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mPciAudioControllerNameTable[] = {
  { "eng;en", L"Pci Audio Driver" },
  { NULL , NULL }
};

EFI_STATUS
EFIAPI
PciAudioComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mPciAudioDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gPciAudioComponentName)
           );
}

EFI_STATUS
EFIAPI
PciAudioComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
{
  EFI_STATUS            Status;
  PCI_AUDIO_DEV         *PciAudioDev;
  PCI_AUDIO_PROTOCOL    *PciAudioProtocol;

  //
  // Get the device context
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciAudioProtocolGuid,
                  (VOID **) &PciAudioProtocol,
                  gPciAudioDriverBinding.DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    Print(L"%r\n", Status);
    return Status;
  }

  PciAudioDev = PCI_AUDIO_DEV_FROM_AUDIO_PROTOCOL(PciAudioProtocol);

  return LookupUnicodeString2 (
                           Language,
                           This->SupportedLanguages,
                           mPciAudioControllerNameTable,
                           ControllerName,
                           (BOOLEAN)(This == &gPciAudioComponentName)
                           );
}