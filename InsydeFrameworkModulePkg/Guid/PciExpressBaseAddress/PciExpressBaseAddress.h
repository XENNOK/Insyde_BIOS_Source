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
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    PciExpressBaseAddress.h

Abstract:

   GUIDs used for PciExpress Base Address

--*/

#ifndef _EFI_PCI_EXPRESS_BASE_ADDRESS_H_
#define _EFI_PCI_EXPRESS_BASE_ADDRESS_H_

#define EFI_PCI_EXPRESS_BASE_ADDRESS_GUID \
  { \
    0x3677d529, 0x326f, 0x4603, 0xa9, 0x26, 0xea, 0xac, 0xe0, 0x1d, 0xcb, 0xb0 \
  }

//
// Following structure defines PCI Express Base Address information.
// This information is platform specific, and built into hob in PEI phase.
// It can be consumed by PEI PCI driver and DXE PCI driver.
//
#pragma pack(1)
typedef struct _EFI_PCI_EXPRESS_BASE_ADDRESS_INFORMATION {
  UINT32       HostBridgeNumber;
  UINT32       RootBridgeNumber;
  UINT64       PciExpressBaseAddress;
} EFI_PCI_EXPRESS_BASE_ADDRESS_INFORMATION;
#pragma pack()

extern EFI_GUID gEfiPciExpressBaseAddressGuid;

#endif
