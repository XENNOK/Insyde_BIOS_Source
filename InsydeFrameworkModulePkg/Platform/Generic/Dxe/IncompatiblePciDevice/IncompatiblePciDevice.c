//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IncompatiblePciDevice.c

Abstract:

  The source file for incompatible PCI device support driver

Revision History:

--*/

#include "IncompatiblePciDevice.h"
#include "IncompatiblePciDeviceList.h"

//
// Handle onto which the Incompatible PCI Device List is installed
//
EFI_HANDLE                                    gIncompatiblePciDeviceSupportHandle = NULL;

//
// The Incompatible PCI Device Support Protocol instance produced by this driver
//
EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT_PROTOCOL  gIncompatiblePciDeviceSupport = { PCheckDevice };

EFI_DRIVER_ENTRY_POINT (IncompatiblePciDeviceDriverEntryPoint)

EFI_STATUS
IncompatiblePciDeviceDriverEntryPoint (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

  Routine Description:
    The entry point for an Socket Service Driver simply allocates space for
    an  EFI_DRIVER_BINDING_PROTOCOL protocol instance, initializes its contents,
    and attaches this protocol instance to the image handle of the Socket
    Service Driver.

  Arguments:
    ImageHandle   EFI_HANDLE: The firmware allocated handle for the EFI
                  Socket Service Driver image.
    SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  Returns:
    EFI_SUCCESS:            The incompatible PCI device support driver was initialized.
    EFI_ALREADY_STARTED:    The driver has already been initialized.
    EFI_INVALID_PARAMETER:  One of the parameters has an invalid value.
    EFI_OUT_OF_RESOURCES:   The request could not be completed due to a lack of
                            resources.

--*/
{
  EfiInitializeDriverLib (ImageHandle, SystemTable);

  return gBS->InstallMultipleProtocolInterfaces (
                &gIncompatiblePciDeviceSupportHandle,
                &gEfiIncompatiblePciDeviceSupportProtocolGuid,
                &gIncompatiblePciDeviceSupport,
                NULL
                );
}

EFI_STATUS
PCheckDevice (
  IN  EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT_PROTOCOL  *This,
  IN  UINTN                                         VendorId,
  IN  UINTN                                         DeviceId,
  IN  UINTN                                         Revision,
  IN  UINTN                                         SubVendorId, OPTIONAL
  IN  UINTN                                         SubDeviceId, OPTIONAL
  OUT VOID                                          *Configuration
  )
/*++

Routine Description:

  This routine returns a list of ACPI resource descriptors details
  the special configuration for an incompatible device specified
  by a given vendor ID and device ID.

Arguments:

  This            The pointer to the instance of the EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT protocol.
  VendorID        An unique ID to specify the vendor of device.
  DeviceID        An unique ID to specify the PCI device
  Revision        Revision number for the PCI device
  SubVendorId     Used to specify sub-system vendor Id
  SubDeviceId     Used to specify sub-system device Id
  Configuration   A list of ACPI resource descriptor details the configuration requirement.

Returns:

  EFI_INVALID_PARAMETER
  EFI_NOT_FOUND
  EFI_UNSUPPORTED
--*/
// GC_TODO:    VendorId - add argument and description to function comment
// GC_TODO:    DeviceId - add argument and description to function comment
// GC_TODO:    EFI_ABORTED - add return value to function comment
// GC_TODO:    EFI_OUT_OF_RESOURCES - add return value to function comment
// GC_TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_STATUS                        Status;
  UINT64                            Tag;
  UINT64                            *ListPtr;
  UINT64                            *TempListPtr;
  EFI_PCI_DEVICE_HEADER_INFO        *Header;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *AcpiPtr;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *OldAcpiPtr;
  EFI_PCI_RESOUCE_DESCRIPTOR        *Dsc;
  EFI_ACPI_END_TAG_DESCRIPTOR       *PtrEnd;
  UINTN                             Index;

  //
  // Validate the parameters
  //
  if (!Configuration) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Initialize the return value to NULL
  //
  * (VOID **) Configuration = NULL;

  ListPtr                   = IncompatiblePciDeviceList;
  while (*ListPtr != LIST_END_TAG) {

    Tag = *ListPtr;

    switch (Tag) {
    case DEVICE_INF_TAG:
      Header  = (EFI_PCI_DEVICE_HEADER_INFO *) (ListPtr + 1);
      ListPtr = ListPtr + 1 + sizeof (EFI_PCI_DEVICE_HEADER_INFO) / sizeof (UINT64);
      //
      // See if the Header matches the parameters passed in
      //
      if (Header->VendorId != DEVICE_ID_NOCARE) {
        if (Header->VendorId != VendorId) {
          continue;
        }
      }

      if (Header->DeviceId != DEVICE_ID_NOCARE) {
        if (DeviceId != Header->DeviceId) {
          continue;
        }
      }

      if (Header->Revision != DEVICE_ID_NOCARE) {
        if (Revision != Header->Revision) {
          continue;
        }
      }

      if (Header->SubVendorId != DEVICE_ID_NOCARE) {
        if (SubVendorId != Header->SubVendorId) {
          continue;
        }
      }

      if (Header->SubDeviceId != DEVICE_ID_NOCARE) {
        if (SubDeviceId != Header->SubDeviceId) {
          continue;
        }
      }
      //
      // Matched an item, so construct the ACPI descriptor for the resource.
      //
      //
      // Count the resource items so that to allocate space
      //
      for (Index = 0, TempListPtr = ListPtr; *TempListPtr == DEVICE_RES_TAG; Index++) {
        TempListPtr = TempListPtr + 1 + ((sizeof (EFI_PCI_RESOUCE_DESCRIPTOR)) / sizeof (UINT64));
      }
      //
      // If there is at least one type of resource request,
      // allocate a acpi resource node
      //
      if (Index == 0) {
        return EFI_ABORTED;
      }

      Status = gBS->AllocatePool (
                      EfiBootServicesData,
                      sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) * Index + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR),
                      &AcpiPtr
                      );
      if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
      }

      OldAcpiPtr = AcpiPtr;
      gBS->SetMem (
            AcpiPtr,
            sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) * Index + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR),
            0
            );

      //
      //   Fill the EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR structure
      //   according to the EFI_PCI_RESOUCE_DESCRIPTOR structure
      //
      for (; *ListPtr == DEVICE_RES_TAG;) {

        Dsc = (EFI_PCI_RESOUCE_DESCRIPTOR *) (ListPtr + 1);

        AcpiPtr->Desc = ACPI_ADDRESS_SPACE_DESCRIPTOR;
        AcpiPtr->Len = sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
        AcpiPtr->ResType = (UINT8) Dsc->ResType;
        AcpiPtr->GenFlag = (UINT8) Dsc->GenFlag;
        AcpiPtr->SpecificFlag = (UINT8) Dsc->SpecificFlag;
        AcpiPtr->AddrSpaceGranularity = Dsc->AddrSpaceGranularity;;
        AcpiPtr->AddrRangeMin = Dsc->AddrRangeMin;
        AcpiPtr->AddrRangeMax = Dsc->AddrRangeMax;
        AcpiPtr->AddrTranslationOffset = Dsc->AddrTranslationOffset;
        AcpiPtr->AddrLen = Dsc->AddrLen;

        ListPtr = ListPtr + 1 + ((sizeof (EFI_PCI_RESOUCE_DESCRIPTOR)) / sizeof (UINT64));
        AcpiPtr++;
      }
      //
      // put the checksum
      //
      PtrEnd                    = (EFI_ACPI_END_TAG_DESCRIPTOR *) (AcpiPtr);
      PtrEnd->Desc              = ACPI_END_TAG_DESCRIPTOR;
      PtrEnd->Checksum          = 0;

      *(VOID **) Configuration  = OldAcpiPtr;

      return EFI_SUCCESS;

    case DEVICE_RES_TAG:
      //
      // Adjust the pointer to the next PCI resource descriptor item
      //
      ListPtr = ListPtr + 1 + ((sizeof (EFI_PCI_RESOUCE_DESCRIPTOR)) / sizeof (UINT64));
      break;

    default:
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_UNSUPPORTED;
}
