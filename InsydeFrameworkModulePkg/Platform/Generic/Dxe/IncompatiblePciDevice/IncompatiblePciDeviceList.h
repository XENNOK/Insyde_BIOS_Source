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

    IncompatiblePciDeviceList.h

Abstract:

  The hardcode of incompatible PCI device list

Revision History:

--*/

#ifndef _EFI_INCOMPATIBLE_PCI_DEVICE_LIST_H
#define _EFI_INCOMPATIBLE_PCI_DEVICE_LIST_H

#define PCI_DEVICE_ID(VendorId, DeviceId, Revision, SubVendorId, SubDeviceId) \
    VendorId, DeviceId, Revision, SubVendorId, SubDeviceId

#define PCI_BAR_TYPE_IO   ACPI_ADDRESS_SPACE_TYPE_IO
#define PCI_BAR_TYPE_MEM  ACPI_ADDRESS_SPACE_TYPE_MEM

#define DEVICE_INF_TAG    0xFFF2
#define DEVICE_RES_TAG    0xFFF1
#define LIST_END_TAG      0x0000

//
// Create hardcode of the incompatible PCI device list
//
UINT64 IncompatiblePciDeviceList[] = {
  //
  // DEVICE_INF_TAG,
  // PCI_DEVICE_ID (VendorID, DeviceID, Revision, SubVendorId, SubDeviceId),
  // DEVICE_RES_TAG,
  // ResType,  GFlag , SFlag,   Granularity,  RangeMin,
  // RangeMax, Offset, AddrLen
  //
  //
  // Device Adaptec 9004
  //
  DEVICE_INF_TAG,
  PCI_DEVICE_ID(0x9004, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE),
  DEVICE_RES_TAG,
  PCI_BAR_TYPE_IO,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_BAR_EVEN_ALIGN,
  PCI_BAR_ALL,
  PCI_BAR_NOCHANGE,
  //
  // Device Adaptec 9005
  //
  DEVICE_INF_TAG,
  PCI_DEVICE_ID(0x9005, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE),
  DEVICE_RES_TAG,
  PCI_BAR_TYPE_IO,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_BAR_EVEN_ALIGN,
  PCI_BAR_ALL,
  PCI_BAR_NOCHANGE,
  //
  // Device QLogic  1007
  //
  DEVICE_INF_TAG,
  PCI_DEVICE_ID(0x1077, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE),
  DEVICE_RES_TAG,
  PCI_BAR_TYPE_IO,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_BAR_EVEN_ALIGN,
  PCI_BAR_ALL,
  PCI_BAR_NOCHANGE,
  //
  // Device Agilent 103C
  //
  DEVICE_INF_TAG,
  PCI_DEVICE_ID(0x103C, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE),
  DEVICE_RES_TAG,
  PCI_BAR_TYPE_IO,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_BAR_EVEN_ALIGN,
  PCI_BAR_ALL,
  PCI_BAR_NOCHANGE,
  //
  // Device Agilent 15BC
  //
  DEVICE_INF_TAG,
  PCI_DEVICE_ID(0x15BC, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE, DEVICE_ID_NOCARE),
  DEVICE_RES_TAG,
  PCI_BAR_TYPE_IO,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_ACPI_UNUSED,
  PCI_BAR_EVEN_ALIGN,
  PCI_BAR_ALL,
  PCI_BAR_NOCHANGE,
  //
  // The end of the list
  //
  LIST_END_TAG
};

#endif
