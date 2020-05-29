/** @file

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

#include "Efikey.h"

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
UsbKeyboardComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  );

EFI_STATUS
EFIAPI
UsbKeyboardComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle        OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  );

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gUsbKeyboardComponentName = {
  UsbKeyboardComponentNameGetDriverName,
  UsbKeyboardComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH_ISO639
};
//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gUsbKeyboardComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) UsbKeyboardComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) UsbKeyboardComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH_RFC4646
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mUsbKeyboardDriverNameTable[] = {
  { LANGUAGE_CODE_ENGLISH_ISO639,  IMAGE_DRIVER_NAME },
  { LANGUAGE_CODE_ENGLISH_RFC4646, IMAGE_DRIVER_NAME },
  { NULL ,                         NULL              }
};

/**

  Retrieves a Unicode string that is the user readable name of the EFI Driver.

  @param  This                  A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param  Language              A pointer to a three character ISO 639-2 language identifier.
                                This is the language of the driver name that that the caller 
                                is requesting, and it must match one of the languages specified
                                in SupportedLanguages.  The number of languages supported by a 
                                driver is up to the driver writer.
  @param  DriverName            A pointer to the Unicode string to return.  This Unicode string
                                is the name of the driver specified by This in the language 
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by This
                                and the language specified by Language was returned 
                                in DriverName.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER DriverName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the 
                                language specified by Language.

**/
EFI_STATUS
EFIAPI
UsbKeyboardComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mUsbKeyboardDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gUsbKeyboardComponentName)
           );
}

/**

  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by an EFI Driver.

  @param  This                  A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param  ControllerHandle      The handle of a controller that the driver specified by 
                                This is managing.  This handle specifies the controller 
                                whose name is to be returned.
  @param  ChildHandle           The handle of the child controller to retrieve the name 
                                of.  This is an optional parameter that may be NULL.  It 
                                will be NULL for device drivers.  It will also be NULL 
                                for a bus drivers that wish to retrieve the name of the 
                                bus controller.  It will not be NULL for a bus driver 
                                that wishes to retrieve the name of a child controller.
  @param  Language              A pointer to a three character ISO 639-2 language 
                                identifier.  This is the language of the controller name 
                                that that the caller is requesting, and it must match one
                                of the languages specified in SupportedLanguages.  The 
                                number of languages supported by a driver is up to the 
                                driver writer.
  @param  ControllerName        A pointer to the Unicode string to return.  This Unicode
                                string is the name of the controller specified by 
                                ControllerHandle and ChildHandle in the language specified
                                by Language from the point of view of the driver specified
                                by This. 

  @retval EFI_SUCCESS           The Unicode string for the user readable name in the 
                                language specified by Language for the driver 
                                specified by This was returned in DriverName.
  @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER ControllerName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This is not currently managing 
                                the controller specified by ControllerHandle and 
                                ChildHandle.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the 
                                language specified by Language.

**/
EFI_STATUS
EFIAPI
UsbKeyboardComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle        OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  )
{
  EFI_STATUS                     Status;
  USB_KB_DEV                     *UsbKbDev;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *SimpleTxtIn;
  EFI_USB_IO_PROTOCOL            *UsbIoProtocol;
  //
  // This is a device driver, so ChildHandle must be NULL.
  //
  if (ChildHandle != NULL) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Check Controller's handle
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **) &UsbIoProtocol,
                  gUsbKeyboardDriverBinding.DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (!EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           ControllerHandle,
           &gEfiUsbIoProtocolGuid,
           gUsbKeyboardDriverBinding.DriverBindingHandle,
           ControllerHandle
           );

    return EFI_UNSUPPORTED;
  }

  if (Status != EFI_ALREADY_STARTED) {
    return EFI_UNSUPPORTED;
  }
  //
  // Get the device context
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID **) &SimpleTxtIn,
                  gUsbKeyboardDriverBinding.DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  UsbKbDev = USB_KB_DEV_FROM_THIS (SimpleTxtIn);

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           UsbKbDev->ControllerNameTable,
           ControllerName,
           (BOOLEAN)(This == &gUsbKeyboardComponentName)
           );
}
