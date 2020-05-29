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

#include "UsbMassImpl.h"

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
UsbMassStorageGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  );

EFI_STATUS
EFIAPI
UsbMassStorageGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle        OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  );

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL gUsbMassStorageComponentName = {
  UsbMassStorageGetDriverName,
  UsbMassStorageGetControllerName,
  LANGUAGE_CODE_ENGLISH_ISO639
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gUsbMassStorageComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) UsbMassStorageGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) UsbMassStorageGetControllerName,
  LANGUAGE_CODE_ENGLISH_RFC4646
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mUsbMassStorageDriverNameTable[] = {
  { LANGUAGE_CODE_ENGLISH_ISO639,  IMAGE_DRIVER_NAME },
  { LANGUAGE_CODE_ENGLISH_RFC4646, IMAGE_DRIVER_NAME },
  {NULL,  NULL}
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
UsbMassStorageGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mUsbMassStorageDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gUsbMassStorageComponentName)
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

  @retval EFI_UNSUPPORTED       The driver specified by This does not support the 
                                language specified by Language.

**/
EFI_STATUS
EFIAPI
UsbMassStorageGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle        OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  )
{
  EFI_STATUS            Status;
  USB_MASS_DEVICE       *UsbMass;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  //
  // Get the device context
  //
  if (!ChildHandle) {
    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlockIo,
                    gUsbMassDriverBinding.DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
  } else {
    Status = gBS->OpenProtocol (
                    ChildHandle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlockIo,
                    gUsbMassDriverBinding.DriverBindingHandle,
                    ChildHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UsbMass = USB_MASS_DEVICE_FROM_BLOCKIO (BlockIo);

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           UsbMass->DeviceNameTable,
           ControllerName,
           (BOOLEAN)(This == &gUsbMassStorageComponentName)
           );
}
