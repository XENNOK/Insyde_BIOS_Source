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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

 PciEx.h

Abstract:

  PciEx PPI as defined in EFI 2.0

  Used to access PCI Express Extended registers in PEI

--*/

#ifndef _PEI_PCI_EX_H_
#define _PEI_PCI_EX_H_

typedef enum {
  PeiPciExWidthUint8  = 0,
  PeiPciExWidthUint16 = 1,
  PeiPciExWidthUint32 = 2,
  PeiPciExWidthUint64 = 3,
  PeiPciExWidthMaximum
} PEI_PCI_EX_PPI_WIDTH;

#define PEI_PCI_EX_PPI_GUID \
  { \
    0x2881c9a5, 0xbba1, 0x41a9, 0x96, 0xaa, 0x10, 0x41, 0x5c, 0x80, 0xd6, 0xf6 \
  }

EFI_FORWARD_DECLARATION (PEI_PCI_EX_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_PCI_EX_PPI_IO) (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN PEI_PCI_EX_PPI         * This,
  IN UINT32                 HecBase,
  IN UINT8                  Bus,
  IN UINT8                  Device,
  IN UINT16                 Function,
  IN UINT16                 ExtendedRegister,
  IN PEI_PCI_EX_PPI_WIDTH   Width,
  IN OUT VOID               *Buffer
  );

typedef struct _PEI_PCI_EX_PPI {
  PEI_PCI_EX_PPI_IO Read;
  PEI_PCI_EX_PPI_IO Write;
} PEI_PCI_EX_PPI;

extern EFI_GUID gPeiPciExPpiGuid;

#endif
