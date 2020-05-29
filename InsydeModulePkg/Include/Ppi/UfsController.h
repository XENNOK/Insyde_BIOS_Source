/** @file
  UFS Controller PPI Header
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_UFS_CONTROLLER_PPI_H_
#define _PEI_UFS_CONTROLLER_PPI_H_

#define PEI_UFS_CONTROLLER_PPI_GUID \
  { \
    0xe364bbf1, 0xb6b7, 0x47c9, 0x8f, 0x7e, 0x31, 0xe7, 0xd3, 0x66, 0x95, 0x5f \
  }

#define PEI_UFS_CONTROLLER         0x01

///
/// Forward declaration for the PEI_UFS_CONTROLLER_PPI.
///
typedef struct _PEI_UFS_CONTROLLER_PPI  PEI_UFS_CONTROLLER_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_GET_UFS_CONTROLLER) (
  IN  EFI_PEI_SERVICES               **PeiServices,
  IN  PEI_UFS_CONTROLLER_PPI         *This,
  IN  UINT8                          UfsControllerId,
  OUT UINT32                         *ControllerType,
  OUT EFI_PHYSICAL_ADDRESS           *MemBase
  );

struct _PEI_UFS_CONTROLLER_PPI {
  PEI_GET_UFS_CONTROLLER  GetUfsController;
};

extern EFI_GUID gPeiUfsControllerPpiGuid;

#endif
