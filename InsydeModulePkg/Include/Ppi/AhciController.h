/** @file
  AHCI Controller PPI Header
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

#ifndef _PEI_AHCI_CONTROLLER_PPI_H_
#define _PEI_AHCI_CONTROLLER_PPI_H_

#define PEI_AHCI_CONTROLLER_PPI_GUID \
  { \
    0xa8d49d7d, 0x06b2, 0x4bf4, 0x89, 0x22, 0x46, 0x14, 0x9f, 0x23, 0x5e, 0x7a \
  }

///
/// Forward declaration for the PEI_USB3_HOST_CONTROLLER_PPI.
///
typedef struct _PEI_AHCI_CONTROLLER_PPI  PEI_AHCI_CONTROLLER_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_GET_AHCI_MEM_BASE) (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_AHCI_CONTROLLER_PPI        *This,
  IN     UINT8                          AhciControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS           *AhciMemBaseAddr
  );

struct _PEI_AHCI_CONTROLLER_PPI {
  PEI_GET_AHCI_MEM_BASE             GetAhciMemBase;
};

extern EFI_GUID gPeiAhciControllerPpiGuid;

#endif
