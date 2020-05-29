/** @file
  CrHookDxe driver for memory allocation module 

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
  
#include "CrBdaMemManager.h"

#include <PiDxe.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>

#define  MAX_ALLOC_ADDR               0x10000
#define  DEFAULT_CR_MEM_SIZE_OF_PAGE  1

UINTN    mCrMemStart = 0;

/**

  Initialize Console Redirection memory manager.

  @retval EFI_SUCCESS Console Redirection memory manager Initialize success
  @retval others           Console Redirection memory manager Initialize fail
 
**/
EFI_STATUS
InitCrEfiMemManager (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    MemoryBuf;
  CR_EFI_MEM_MANAGER      *CrEfiMemoryMgr;

  //
  // Allocate a page space as default CREfiMemory space
  //
  MemoryBuf = MAX_ALLOC_ADDR;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  DEFAULT_CR_MEM_SIZE_OF_PAGE,
                  &MemoryBuf
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem ((UINT8 *)(UINTN)MemoryBuf, (DEFAULT_CR_MEM_SIZE_OF_PAGE << 12));
  mCrMemStart = (UINT32)MemoryBuf;
  CrEfiMemoryMgr = (CR_EFI_MEM_MANAGER *) mCrMemStart;

  //
  // Fill CR Efi memory space Info
  //
  CrEfiMemoryMgr->Signature    = SIGNATURE('$','C','E','I');
  CrEfiMemoryMgr->EndOfMemory  = (UINT16)(DEFAULT_CR_MEM_SIZE_OF_PAGE << 12);
  CrEfiMemoryMgr->UsedOfMemory = (UINT16)sizeof(CR_EFI_MEM_MANAGER);

  return EFI_SUCCESS;

}

/**

  Allocate a memory space from Console Redirection Memory Manager.

  @param  Size         Size of memory to allocate.

  @retval  Return allocated address.

**/
VOID*
CrEfiMemAlloc (
  IN UINTN     Size
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    MemoryBuf;
  CR_EFI_MEM_MANAGER      *CrEfiMemoryMgr;
  UINTN                   RemainMemorySize;
  UINTN                   NeededSizeOfPages;
  UINTN                   OrgSizeOfPages;
  VOID                    *Memory;

  if (Size == 0) {
    return NULL;
  }
  
  if (mCrMemStart == 0) {
    Status = InitCrEfiMemManager ();
    if (EFI_ERROR (Status)) {
      return NULL;
    }
  }


  CrEfiMemoryMgr = (CR_EFI_MEM_MANAGER *)(mCrMemStart);
  RemainMemorySize = CrEfiMemoryMgr->EndOfMemory - CrEfiMemoryMgr->UsedOfMemory;

  if (Size > RemainMemorySize) {
    OrgSizeOfPages = CrEfiMemoryMgr->EndOfMemory >> 12;
    //
    // Calculate needed Memory size in Pages
    //
    NeededSizeOfPages = (Size - RemainMemorySize + 4095) >> 12;

    //
    // Rellocate CREfiMemory space below 64K
    //
    MemoryBuf = MAX_ALLOC_ADDR;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiRuntimeServicesData,
                    OrgSizeOfPages + NeededSizeOfPages,
                    &MemoryBuf
                    );
    if (EFI_ERROR (Status)) {
      return NULL;
    }

    ZeroMem ((UINT8 *)(UINTN)MemoryBuf, ((OrgSizeOfPages + NeededSizeOfPages) << 12));
    //
    // Copy old location data to new location
    //
    CopyMem ((UINT8 *)(UINTN)MemoryBuf, (UINT8 *)mCrMemStart, CrEfiMemoryMgr->UsedOfMemory);
    //
    // Rellocate memory space complete and free old memory space
    //
    FreeCrEfiMemorySpace ();
    //
    // Update new location to mCrMemStart
    //
    mCrMemStart = (UINT32)MemoryBuf;
    CrEfiMemoryMgr = (CR_EFI_MEM_MANAGER *)(mCrMemStart);

    CrEfiMemoryMgr->EndOfMemory = (UINT16)((OrgSizeOfPages + NeededSizeOfPages) << 12);
  }

  Memory = (void *)(mCrMemStart + CrEfiMemoryMgr->UsedOfMemory);
  CrEfiMemoryMgr->UsedOfMemory = (UINT16)(CrEfiMemoryMgr->UsedOfMemory + Size);

  return Memory;
}

/**

  Frees the memory space that allocate for Console Redirection Memory Manager.

  @retval EFI_SUCCESS         Free memory success
  @retval EFI_UNSUPPORTED Free memory fail
  @retval others                   Free memory fail

**/
EFI_STATUS
FreeCrEfiMemorySpace (
  VOID
  )
{
  EFI_STATUS           Status;
  CR_EFI_MEM_MANAGER   *CrEfiMemoryMgr;

  if (mCrMemStart == 0) {
    return EFI_UNSUPPORTED;
  }

  CrEfiMemoryMgr = (CR_EFI_MEM_MANAGER *)(mCrMemStart);

  Status = gBS->FreePages ((EFI_PHYSICAL_ADDRESS)mCrMemStart, (CrEfiMemoryMgr->EndOfMemory >> 12));

  return Status;
}
