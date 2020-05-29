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

  IncompatiblePciDevice.h

Abstract:

  The header file for incompatible PCI device driver

Revision History:

--*/

#ifndef _EFI_INCOMPATIBLE_PCI_DEVICE_H
#define _EFI_INCOMPATIBLE_PCI_DEVICE_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "pci22.h"
#include "acpi.h"

//
// Produceed protocol
//
#include EFI_PROTOCOL_DEFINITION (IncompatiblePciDeviceSupport)

typedef struct {
  UINT64  VendorId;
  UINT64  DeviceId;
  UINT64  Revision;
  UINT64  SubVendorId;
  UINT64  SubDeviceId;
} EFI_PCI_DEVICE_HEADER_INFO;

typedef struct {
  UINT64  ResType;
  UINT64  GenFlag;
  UINT64  SpecificFlag;
  UINT64  AddrSpaceGranularity;
  UINT64  AddrRangeMin;
  UINT64  AddrRangeMax;
  UINT64  AddrTranslationOffset;
  UINT64  AddrLen;
} EFI_PCI_RESOUCE_DESCRIPTOR;

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

  GC_TODO: Add function description

Arguments:

  This          - GC_TODO: add argument description
  VendorId      - GC_TODO: add argument description
  DeviceId      - GC_TODO: add argument description
  Revision      - GC_TODO: add argument description
  SubVendorId   - GC_TODO: add argument description
  SubDeviceId   - GC_TODO: add argument description
  Configuration - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
