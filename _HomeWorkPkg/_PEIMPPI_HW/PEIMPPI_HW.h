/** @file
  

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#ifndef _PEIMPPI_HW_H_
#define _PEIMPPI_HW_H_

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>

extern EFI_GUID gEfiHydraPpiGuid;
extern EFI_GUID gEfiHydraVariGuid;

typedef struct {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Guid;
  UINT8                   *Data;
  UINT16                  Size;
} HOB_LIST;

typedef
EFI_STATUS
(EFIAPI *GET_VARI)(
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINTN                   *Size
);

typedef
EFI_STATUS
(EFIAPI *CREATE_HOB)(
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINT16                   Size
);

typedef
struct {
  GET_VARI GetVari;
  CREATE_HOB CreateHob;
}HYDRA_PPI_FUNC;

EFI_STATUS
GetVari (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINTN                   *Size
);

EFI_STATUS
CreateHob (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINT16                   Size
);


#endif