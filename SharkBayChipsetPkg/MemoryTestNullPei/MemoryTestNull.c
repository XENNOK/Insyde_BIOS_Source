/** @file

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

//[-start-120627-IB10820071-modify]//
#include <Uefi.h>
#include <PiPei.h>
//[-start-121102-IB10820149-modify]//
#include <Library/PeiServicesLib.h>

typedef struct _PEI_BASE_MEMORY_TEST_PPI PEI_BASE_MEMORY_TEST_PPI;

typedef enum {
  Ignore,
  Quick,
  Sparse,
  Extensive
} PEI_MEMORY_TEST_OP;

typedef
EFI_STATUS
(EFIAPI *PEI_BASE_MEMORY_TEST) (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           * This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               * ErrorAddress
  );

struct _PEI_BASE_MEMORY_TEST_PPI {
  PEI_BASE_MEMORY_TEST  BaseMemoryTest;
};

extern EFI_GUID gPeiBaseMemoryTestPpiGuid;
//[-end-121102-IB10820149-modify]//  

EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN      EFI_PEI_SERVICES                **PeiServices,
  IN      PEI_BASE_MEMORY_TEST_PPI        *This,
  IN      EFI_PHYSICAL_ADDRESS            BeginAddress,
  IN      UINT64                          MemoryLength,
  IN      PEI_MEMORY_TEST_OP              Operation,
     OUT  EFI_PHYSICAL_ADDRESS            *ErrorAddress
  );
//[-end-120627-IB10820071-modify]//  

static PEI_BASE_MEMORY_TEST_PPI        mPeiBaseMemoryTestPpi = { BaseMemoryTest };

static EFI_PEI_PPI_DESCRIPTOR        PpiListPeiBaseMemoryTest = {
  ( EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST ),
  &gPeiBaseMemoryTestPpiGuid,
  &mPeiBaseMemoryTestPpi
};

EFI_STATUS
EFIAPI
MemoryTestNullEntry (
  IN       EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES		      **PeiServices
  )
{
  EFI_STATUS        Status;

//[-start-121102-IB10820149-modify]//
  Status = PeiServicesInstallPpi ( &PpiListPeiBaseMemoryTest );
//[-end-121102-IB10820149-modify]//

  return Status;
}

EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN      EFI_PEI_SERVICES                **PeiServices,
  IN      PEI_BASE_MEMORY_TEST_PPI        *This,
  IN      EFI_PHYSICAL_ADDRESS            BeginAddress,
  IN      UINT64                          MemoryLength,
  IN      PEI_MEMORY_TEST_OP              Operation,
     OUT  EFI_PHYSICAL_ADDRESS            *ErrorAddress
  )
{
  return EFI_SUCCESS;
}
