/** @file
  PeimA_20 H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEIM_A_20_H_
#define _PEIM_A_20_H_

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

#include <Ppi/ReadOnlyVariable2.h>

#define VARIABLE_GUID                                                             \
  {                                                                               \
    0x10000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }

#define HOB_GUID                                                                  \
  {                                                                               \
    0x20000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }

typedef struct _EFI_PEIM_PPI EFI_PEIM_PPI;

EFI_STATUS
PeimDo (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *PEIM_DO)(
  VOID
);

typedef struct _EFI_PEIM_PPI {
  PEIM_DO  PeimDo;
};

static EFI_PEIM_PPI gPeimPpi;
extern EFI_PEI_SERVICES **gPeiServices;
extern EFI_GUID gPeimPpiGuid;

static EFI_PEI_PPI_DESCRIPTOR EfiPeimDescriptorPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeimPpiGuid,
  &gPeimPpi
};

#endif