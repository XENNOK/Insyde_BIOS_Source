/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  UEFI Component Name and Name2 protocol for Isa serial driver.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Serial.h"

#define SERIAL_PORT_NAME      L"ISA Serial Port "
#define SERIAL_PORT_NAME_LEN  (sizeof (SERIAL_PORT_NAME) / sizeof (CHAR16))
//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gIsaSerialComponentName = {
  IsaSerialComponentNameGetDriverName,
  IsaSerialComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH_ISO639
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gIsaSerialComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) IsaSerialComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) IsaSerialComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH_RFC4646
};


GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mIsaSerialDriverNameTable[] = {
  { LANGUAGE_CODE_ENGLISH_ISO639,  L"ISA Serial Driver" },
  { LANGUAGE_CODE_ENGLISH_RFC4646, L"ISA Serial Driver" },
  { NULL ,                         NULL              }
};

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
IsaSerialComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mIsaSerialDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gIsaSerialComponentName)
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
IsaSerialComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
{
  EFI_STATUS              Status;
  EFI_SERIAL_IO_PROTOCOL  *SerialIo;
  SERIAL_DEV              *SerialDevice;

  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiTestManagedDevice (
             ControllerHandle,
             gSerialControllerDriver.DriverBindingHandle,
             &gEfiIsaIoProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Serial I/O Protocol on Controller
  //
  Status = gBS->OpenProtocol (
                  ChildHandle,
                  &gEfiSerialIoProtocolGuid,
                  (VOID **) &SerialIo,
                  gSerialControllerDriver.DriverBindingHandle,
                  ChildHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Serial Controller's Device structure
  //
  SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           SerialDevice->ControllerNameTable,
           ControllerName,
           (BOOLEAN)(This == &gIsaSerialComponentName)
           );
}

/**
  Add the ISO639-2 and RFC4646 component name both for the Serial IO device

  @param SerialDevice     A pointer to the SERIAL_DEV instance.

  @param IsaIo            A pointer to the EFI_ISA_IO_PROTOCOL instance.

**/
VOID
AddName (
  IN  SERIAL_DEV                               *SerialDevice,
  IN  EFI_ISA_IO_PROTOCOL                      *IsaIo
  )
{
  CHAR16  SerialPortName[SERIAL_PORT_NAME_LEN + 1];
  CHAR16  StrBuff[2];

  StrCpy (SerialPortName, SERIAL_PORT_NAME);
  UnicodeValueToString (StrBuff, 0, (INT64) IsaIo->ResourceList->Device.UID, 0);
  StrBuff[1] = 0;
  StrCat (SerialPortName, StrBuff);

  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_ISO639,
    gIsaSerialComponentName.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    SerialPortName,
    TRUE
    );
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_RFC4646,
    gIsaSerialComponentName.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    SerialPortName,
    FALSE
    );

}
