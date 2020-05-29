/** @file
  Memory Management Header for USB

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

#ifndef _MEM_MNG_H
#define _MEM_MNG_H

typedef struct {
  EFI_PHYSICAL_ADDRESS          Address;
  VOID                          *AdjustedAddress;
  UINTN                         Length;
  UINTN                         Direction;
} MEMORY_MAPPING;

typedef struct {
  UINT64                        DxeWorkingMemoryStart;
  UINT64                        DxeWorkingMemoryLength;
  UINT64                        SmmWorkingMemoryStart;
  UINT64                        SmmWorkingMemoryLength;
} USB_WORKING_REGION;

//
// Allocate Pool
//
EFI_STATUS
EFIAPI
AllocateBuffer (
  IN  UINTN                             AllocSize,
  IN  UINTN                             Alignment,
  OUT VOID                              **Pool
  );
//
// Free Pool
//
EFI_STATUS
EFIAPI
FreeBuffer (
  IN UINTN                              AllocSize,
  IN VOID                               *Pool
  );
//
// Memory Map
//
EFI_STATUS
EFIAPI
MemoryMapping (
  IN  UINTN                             Direction,
  IN  EFI_PHYSICAL_ADDRESS              Addr,
  IN  UINTN                             Length,
  OUT VOID                              **AdjustedAddr,
  OUT VOID                              **Handle
  );
//
// Memory Unmap
//
EFI_STATUS
EFIAPI
MemoryUnmapping (
  IN VOID                               *Handle
  );
//
// Initial Memory Management
//
EFI_STATUS
EFIAPI
InitialMemoryManagement (
  IN UINTN                              Type
  );

#endif
