//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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

Module Name:

  AhciController.h

Abstract:

  Ahci Controller PPI

--*/

#ifndef _PEI_AHCI_CONTROLLER_PPI_H_
#define _PEI_AHCI_CONTROLLER_PPI_H_

#define PEI_AHCI_CONTROLLER_PPI_GUID \
  { \
    0xa8d49d7d, 0x06b2, 0x4bf4, 0x89, 0x22, 0x46, 0x14, 0x9f, 0x23, 0x5e, 0x7a \
  }

EFI_FORWARD_DECLARATION (PEI_AHCI_CONTROLLER_PPI);

typedef
UINT32
(EFIAPI *PEI_GET_AHCI_MEM_BASE) (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_AHCI_CONTROLLER_PPI        *This,
  IN     UINT8                          AhciControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS           *AhciMemBaseAddr
  );

typedef struct _PEI_AHCI_CONTROLLER_PPI {
  PEI_GET_AHCI_MEM_BASE             GetAhciMemBase;
} PEI_AHCI_CONTROLLER_PPI;

extern EFI_GUID gPeiAhciControllerPpiGuid;

#endif
