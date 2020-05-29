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


#include "PciSerial.h"

#define PCI_SERIAL_PORT_NAME      L"PCI Serial Port # "
#define PCI_SERIAL_PORT_NAME_LEN  (sizeof (PCI_SERIAL_PORT_NAME) / sizeof (CHAR16))


//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL gPciSerialComponentName = {
  PciSerialComponentNameGetDriverName,
  PciSerialComponentNameGetControllerName,
  "eng"
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gPciSerialComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) PciSerialComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) PciSerialComponentNameGetControllerName,
  "en"
};

static EFI_UNICODE_STRING_TABLE mPciSerialDriverNameTable[] = {
  {
    "eng;en",
    L"PCI Serial Driver"
  },
  {
    NULL,
    NULL
  }
};

/**

  Retrieves a Unicode string that is the user readable name of the EFI Driver.

  @param[in]  This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Language   - A pointer to a three character ISO 639-2 language identifier.
                           This is the language of the driver name that that the caller 
                           is requesting, and it must match one of the languages specified
                           in SupportedLanguages.  The number of languages supported by a 
                           driver is up to the driver writer.
  @param[in] DriverName  - A pointer to the Unicode string to return.  This Unicode string
                           is the name of the driver specified by This in the language 
                           specified by Language.

  @retval   EFI_SUCCESS  - The Unicode string for the Driver specified by This
                           and the language specified by Language was returned 
                           in DriverName.
  @retval   EFI_INVALID_PARAMETER - Language is NULL.
  @retval   EFI_INVALID_PARAMETER - DriverName is NULL.
  @retval   EFI_UNSUPPORTED       - The driver specified by This does not support the 
                                    language specified by Language.
**/
EFI_STATUS
EFIAPI
PciSerialComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mPciSerialDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gPciSerialComponentName)
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by an EFI Driver.

  @param[in]  This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  ControllerHandle - The handle of a controller that the driver specified by    
                                 This is managing.  This handle specifies the controller 
                                 whose name is to be returned.
  @param[in]  ChildHandle      - The handle of the child controller to retrieve the name 
                                 of.  This is an optional parameter that may be NULL.  It 
                                 will be NULL for device drivers.  It will also be NULL 
                                 for a bus drivers that wish to retrieve the name of the 
                                 bus controller.  It will not be NULL for a bus driver 
                                 that wishes to retrieve the name of a child controller.
  @param[in]  Language         - A pointer to a three character ISO 639-2 language 
                                 identifier.  This is the language of the controller name 
                                 that that the caller is requesting, and it must match one
                                 of the languages specified in SupportedLanguages.  The 
                                 number of languages supported by a driver is up to the 
                                 driver writer.
  @param[in]  ControllerName   - A pointer to the Unicode string to return.  This Unicode
                                 string is the name of the controller specified by 
                                 ControllerHandle and ChildHandle in the language 
                                 specified by Language from the point of view of the 
                                 driver specified by This. 
  
  @retval   EFI_SUCCESS           - The Unicode string for the user readable name in the 
                                    language specified by Language for the driver 
                                    specified by This was returned in DriverName.
  @retval   EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
  @retval   EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                                    EFI_HANDLE.
  @retval   EFI_INVALID_PARAMETER - Language is NULL.
  @retval   EFI_INVALID_PARAMETER - ControllerName is NULL.
  @retval   EFI_UNSUPPORTED       - The driver specified by This is not currently 
                                    managing the controller specified by 
                                    ControllerHandle and ChildHandle.
  @retval   EFI_UNSUPPORTED       - The driver specified by This does not support the 
                                    language specified by Language.
         
**/
EFI_STATUS
EFIAPI
PciSerialComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  )
{
  EFI_STATUS                  Status;
  EFI_SERIAL_IO_PROTOCOL      *SerialIo;
  SERIAL_DEV                  *SerialDevice;

  //
  // This is a device driver, so ChildHandle must be NULL.
  //
  if (ChildHandle != NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get the Block I/O Protocol on Controller
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle, 
                  &gEfiSerialIoProtocolGuid, 
                  (VOID **)&SerialIo,
                  gPciSerialControllerDriver.DriverBindingHandle,  
                  ControllerHandle, 
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the Serial Controller's Device structure
  //
  SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

  return  LookupUnicodeString2 (
           Language, 
           This->SupportedLanguages,
           SerialDevice->ControllerNameTable, 
           ControllerName,
           (BOOLEAN)(This == &gPciSerialComponentName)
           );
}

/**
  Add the ISO639-2 and RFC4646 component name both for the Serial IO device

  @param SerialDevice     A pointer to the SERIAL_DEV instance.

  @param PciIo            A pointer to the EFI_PCI_IO_PROTOCOL instance.

**/
VOID
AddName (
  IN  SERIAL_DEV                               *SerialDevice,
  IN  EFI_PCI_IO_PROTOCOL                      *PciIo
  )
{
  CHAR16  SerialPortName[PCI_SERIAL_PORT_NAME_LEN];

  StrCpy (SerialPortName, PCI_SERIAL_PORT_NAME);

  SerialPortName[PCI_SERIAL_PORT_NAME_LEN -2 ] = (CHAR16)(L'0');

  AddUnicodeString2 (
    "eng",
    gPciSerialComponentName.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    (CHAR16 *) SerialPortName,
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gPciSerialComponentName2.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    (CHAR16 *) SerialPortName,
    FALSE
    );

}

