//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Capsule.c

Abstract:

  Capsule update PEIM.

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "EfiCapsule.h"
#include "Capsule.h"

#include EFI_GUID_DEFINITION (Capsule)
#include EFI_PPI_DEFINITION (Variable)
#include EFI_PPI_DEFINITION (Capsule)

//
// We want to avoid using memory at 0 for coalescing, so set a
// min address.
//
#define MIN_COALESCE_ADDR (1024 * 1024)

//
// This capsule PEIM puts its private data at the start of the
// coalesced capsule. Here's the structure definition.
//
#define EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE EFI_SIGNATURE_32 ('C', 'a', 'p', 'D')

typedef struct {
  UINT32  Signature;
  UINTN   CapsuleSize;
} EFI_CAPSULE_PEIM_PRIVATE_DATA;

//
// Entry point function prototype
//
EFI_STATUS
EFIAPI
CapsuleMain (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  );

//
// Local function prototypes
//
STATIC
UINT8                               *
FindFreeMem (
  EFI_CAPSULE_BLOCK_DESCRIPTOR      *BlockList,
  UINT8                             *MemBase,
  UINTN                             MemSize,
  UINTN                             DataSize
  );

STATIC
EFI_STATUS
GetCapsuleInfo (
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR  *Desc,
  IN OUT UINTN                     *NumDescriptors OPTIONAL,
  IN OUT UINTN                     *CapsuleSize OPTIONAL
  );

STATIC
BOOLEAN
IsOverlapped (
  UINT8     *Buff1,
  UINTN     Size1,
  UINT8     *Buff2,
  UINTN     Size2
  );

STATIC
EFI_STATUS
ProcessCapsules (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR       *BlockList,
  IN EFI_PEI_HOB_POINTERS               HobList,
  IN OUT VOID                           **NewCapsuleBase OPTIONAL
  );

STATIC
EFI_CAPSULE_BLOCK_DESCRIPTOR             *
RelocateBlockDescriptors (
  IN EFI_PEI_SERVICES                    **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR        *BlockList,
  IN UINT8                               *MemBase,
  IN UINTN                               MemSize
  );

STATIC
EFI_STATUS
GetCapsuleDescriptors (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN OUT EFI_CAPSULE_BLOCK_DESCRIPTOR   **BlockList OPTIONAL
  );

STATIC
EFI_STATUS
ValidateCapsuleIntegrity (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR       *BlockList
  );

STATIC
EFI_STATUS
BuildHobCv (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN EFI_PHYSICAL_ADDRESS        BaseAddress,
  IN UINT64                      Length
  );

//
// Define what we need for the capsule PPI
//
STATIC
EFI_STATUS
EFIAPI
CapsuleCoalesce (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN OUT VOID                        **MemoryBase,
  IN OUT UINTN                       *MemSize
  );

STATIC
EFI_STATUS
EFIAPI
CreateState (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN VOID                             *CapsuleBase,
  IN UINTN                            CapsuleSize
  );

STATIC
EFI_STATUS
EFIAPI
CheckCapsuleUpdate (
  IN EFI_PEI_SERVICES           **PeiServices
  );

STATIC
VOID
CapsuleTestPatternPreCoalesce (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR *Desc
  );

STATIC const PEI_CAPSULE_PPI        mCapsulePpi = {
  CapsuleCoalesce,
  CheckCapsuleUpdate,
  CreateState
};

STATIC const EFI_PEI_PPI_DESCRIPTOR mPpiListCapsule = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiCapsulePpiGuid,
  (PEI_CAPSULE_PPI *) &mCapsulePpi
};

//
// Flags for the PrintHex() debug function
//
#define PRINT_HEX_SPACE   0x0001
#define PRINT_HEX_NEWLINE 0x0002

//
// Debug functions and data
//
DEBUG_CODE (
  static const CHAR8 HexDigits[] = "0123456789ABCDEF";

  STATIC VOID PrintHex (IN EFI_PEI_SERVICES **PeiServices, IN UINT32 Num, IN UINT32 PrintFlags);
) // end DEBUG_CODE
STATIC
BOOLEAN
CapsuleTestPattern (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN VOID                             *CapsuleBase
  );

//
// Main entry point
//
EFI_PEIM_ENTRY_POINT (CapsuleMain)

EFI_STATUS
EFIAPI
CapsuleMain (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:
  Entry point function for the PEIM

Arguments:
  FfsHeader   - standard PEIM entry point arg
  PeiServices - pointer to PEI services table

Returns:
  EFI_SUCCESS if we installed our PPI

--*/
{
  EFI_STATUS  Status;

  //
  // Just produce our PPI
  //
  Status = (**PeiServices).InstallPpi (
                            PeiServices,
                            (EFI_PEI_PPI_DESCRIPTOR *) &mPpiListCapsule
                            );
  return Status;
}

EFI_STATUS
EFIAPI
CapsuleCoalesce (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN OUT VOID                        **MemoryBase,
  IN OUT UINTN                       *MemorySize
  )
/*++

Routine Description:

  Capsule PPI service to coalesce a fragmented capsule in memory.

Arguments:

  PeiServices - General purpose services available to every PEIM.
  MemoryBase  - Pointer to the base of a block of memory that we can walk
                all over while trying to coalesce our buffers.
                On output, this variable will hold the base address of
                a coalesced capsule.
  MemorySize  - Size of the memory region pointed to by MemoryBase.
                On output, this variable will contain the size of the
                coalesced capsule.

Returns:

  EFI_NOT_FOUND   if we can't determine the boot mode
                  if the boot mode is not flash-update
                  if we could not find the capsule descriptors

  EFI_BUFFER_TOO_SMALL
                  if we could not coalesce the capsule in the memory
                  region provided to us

  EFI_SUCCESS     if there's no capsule, or if we processed the
                  capsule successfully.

--*/
{
  EFI_STATUS                    Status;
  EFI_BOOT_MODE                 BootMode;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockList;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *CurrentBlockDesc;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockDesc;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  PrivateDataDesc[2];
  UINTN                         CapsuleSize;
  UINTN                         DescriptorsSize;
  UINT8                         *FreeMemBase;
  UINT8                         *DestPtr;
  UINT8                         *RelocPtr;
  UINTN                         FreeMemSize;
  UINTN                         NumDescriptors;
  VOID                          *NewCapsuleBase;
  EFI_CAPSULE_PEIM_PRIVATE_DATA PrivateData;
  //
  // Someone should have already ascertained the boot mode. If it's not
  // capsule update, then return normally.
  //
  Status = (**PeiServices).GetBootMode (PeiServices, &BootMode);
  if ((Status != EFI_SUCCESS) || (BootMode != BOOT_ON_FLASH_UPDATE)) {
    return EFI_NOT_FOUND;
  }
  //
  // Find out if we actually have a capsule.
  //
  Status = GetCapsuleDescriptors (PeiServices, &BlockList);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

#ifdef EFI_DEBUG
  CapsuleTestPatternPreCoalesce (PeiServices, BlockList);
#endif
  //
  // Get the size of our descriptors and the capsule size. GetCapsuleInfo()
  // returns the number of descriptors that actually point to data, so add
  // one for a terminator. Do that below.
  //
  GetCapsuleInfo (BlockList, &NumDescriptors, &CapsuleSize);
  if ((CapsuleSize == 0) || (NumDescriptors == 0)) {
    return EFI_NOT_FOUND;
  }
  //
  // Initialize our local copy of private data. When we're done, we'll create a
  // descriptor for it as well so that it can be put into free memory without
  // trashing anything.
  //
  PrivateData.Signature     = EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE;
  PrivateData.CapsuleSize   = CapsuleSize;
  PrivateDataDesc[0].Data   = (EFI_PHYSICAL_ADDRESS) (UINTN) &PrivateData;
  PrivateDataDesc[0].Length = sizeof (EFI_CAPSULE_PEIM_PRIVATE_DATA);
  PrivateDataDesc[1].Data   = (EFI_PHYSICAL_ADDRESS) (UINTN) BlockList;
  PrivateDataDesc[1].Length = 0;
  NumDescriptors += 3;
  CapsuleSize += sizeof (EFI_CAPSULE_PEIM_PRIVATE_DATA);
  BlockList       = PrivateDataDesc;

  DescriptorsSize = NumDescriptors * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
  //
  // Don't go below some min address. If the base is below it,
  // then move it up and adjust the size accordingly.
  //
  if (*MemoryBase < (VOID *) MIN_COALESCE_ADDR) {
    *MemorySize = *MemorySize - ((UINTN) MIN_COALESCE_ADDR - (UINTN) MemoryBase);
    *MemoryBase = (VOID *) MIN_COALESCE_ADDR;
  }

  if (*MemorySize <= CapsuleSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  FreeMemBase = *MemoryBase;
  FreeMemSize = *MemorySize;
  //
  // Relocate all the block descriptors to low memory to make further
  // processing easier.
  //
  BlockList = RelocateBlockDescriptors (PeiServices, BlockList, FreeMemBase, FreeMemSize);
  if (BlockList == NULL) {
    //
    // Not enough room to relocate the descriptors
    //
    return EFI_BUFFER_TOO_SMALL;
  }
  //
  // Take the top of memory for the capsule. Naturally align.
  //
  DestPtr         = FreeMemBase + FreeMemSize - CapsuleSize;
  DestPtr         = (UINT8 *) ((UINTN) DestPtr &~ (UINTN) (sizeof (UINTN) - 1));
  FreeMemBase     = (UINT8 *) BlockList + DescriptorsSize;
  FreeMemSize     = FreeMemSize - DescriptorsSize - CapsuleSize;
  NewCapsuleBase  = (VOID *) DestPtr;
  //
  // PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule coalesce to "));
  // PrintHex (PeiServices, (UINT32)(UINTN)DestPtr, PRINT_HEX_NEWLINE);
  //
  // Move all the blocks to the top (high) of memory.
  // Relocate all the obstructing blocks. Note that the block descriptors
  // were coalesced when they were relocated, so we can just ++ the pointer.
  //
  CurrentBlockDesc = BlockList;
  while (CurrentBlockDesc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
    //
    // See if any of the remaining capsule blocks are in the way
    //
    TempBlockDesc = CurrentBlockDesc;
    while (TempBlockDesc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
      //
      // Is this block in the way of where we want to copy the current
      // descriptor to?
      //
      if (IsOverlapped (
            (UINT8 *) DestPtr,
            (UINTN) CurrentBlockDesc->Length,
            (UINT8 *) (UINTN) TempBlockDesc->Data,
            (UINTN) TempBlockDesc->Length
            )) {
        //
        // Relocate the block
        //
        RelocPtr = FindFreeMem (BlockList, FreeMemBase, FreeMemSize, (UINTN) TempBlockDesc->Length);
        if (RelocPtr == NULL) {
          return EFI_BUFFER_TOO_SMALL;
        }
        (**PeiServices).CopyMem ((VOID *) RelocPtr, (VOID *) (UINTN) TempBlockDesc->Data, (UINTN) TempBlockDesc->Length);
        DEBUG_CODE (
          PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule reloc data block from/to/size "));
          PrintHex (PeiServices, (UINT32) (UINTN) TempBlockDesc->Data, PRINT_HEX_SPACE);
          PrintHex (PeiServices, (UINT32) (UINTN) RelocPtr, PRINT_HEX_SPACE);
          PrintHex (PeiServices, (UINT32) (UINTN) TempBlockDesc->Length, PRINT_HEX_NEWLINE);
        )

        TempBlockDesc->Data = (EFI_PHYSICAL_ADDRESS) (UINTN) RelocPtr;
      }
      //
      // Next descriptor
      //
      TempBlockDesc++;
    }
    //
    // Ok, we made it through. Copy the block.
    //
    (**PeiServices).CopyMem ((VOID *) DestPtr, (VOID *) (UINTN) CurrentBlockDesc->Data, (UINTN) CurrentBlockDesc->Length);
    DestPtr += CurrentBlockDesc->Length;
    DEBUG_CODE (
      PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule coalesce block from/to/size   "));
      PrintHex (PeiServices, (UINT32) (UINTN) CurrentBlockDesc->Data, PRINT_HEX_SPACE);
      PrintHex (PeiServices, (UINT32) (UINTN) DestPtr, PRINT_HEX_SPACE);
      PrintHex (PeiServices, (UINT32) (UINTN) CurrentBlockDesc->Length, PRINT_HEX_NEWLINE);
    )

    CurrentBlockDesc++;
  }
  //
  // We return the base of memory we want reserved, and the size.
  // The memory peim should handle it appropriately from there.
  //
  *MemoryBase = (VOID *) NewCapsuleBase;
  *MemorySize = CapsuleSize;
  return Status;
}

STATIC
EFI_STATUS
GetCapsuleDescriptors (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN OUT EFI_CAPSULE_BLOCK_DESCRIPTOR   **BlockList OPTIONAL
  )
/*++

Routine Description:

  Checks for the presence of capsule descriptors.

Arguments:

  PeiServices   - General purpose services available to every PEIM.
  BlockList     - Pointer to where to return the start of the capsule descriptor list.

Returns:

  EFI_SUCCESS     a valid capsule is present
  EFI_NOT_FOUND   if a valid capsule is not present

NOTES:
  This function may be called just to determine the boot mode. If that
  is the case, then don't integrity-check the capsule because memory
  is not yet available, so we'd think our capsule is corrupted.

--*/
{
  EFI_STATUS                  Status;
  PEI_READ_ONLY_VARIABLE_PPI  *PPIVariableServices;
  UINTN                       Size;
  UINTN                       CapsuleDataPtr;
  UINT64                      CapsuleDataPtr64;

  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiReadOnlyVariablePpiGuid,
                            0,
                            NULL,
                            (VOID **) &PPIVariableServices
                            );
  if (Status != EFI_SUCCESS) {
    //
    // Should never happen since we have a dependency expression based on variable services
    //
    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "GetCapsuleDescriptors() -- variable services not available\n"));
    return EFI_NOT_FOUND;
  } else {
    Size = sizeof (CapsuleDataPtr64);
    Status = PPIVariableServices->PeiGetVariable (
                                    PeiServices,
                                    EFI_CAPSULE_VARIABLE_NAME,
                                    &gEfiCapsuleVendorGuid,
                                    NULL,
                                    &Size,
                                    (VOID *) &CapsuleDataPtr64
                                    );
    CapsuleDataPtr = (UINTN) CapsuleDataPtr64;
    if (Status == EFI_SUCCESS) {
      //
      // We have a chicken/egg situation where the memory init code needs to
      // know the boot mode prior to initializing memory. For this case, our
      // validate function will fail. We can detect if this is the case if blocklist
      // pointer is null. In that case, return success since we know that the
      // variable is set.
      //
      if (BlockList == NULL) {
        return EFI_SUCCESS;
      }
      //
      // Test integrity of descriptors.
      //
      // PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule variable = "));
      // PrintHex (PeiServices, (UINT32)CapsuleResetData.CapsuleDescriptors);
      //
      Status = ValidateCapsuleIntegrity (
                PeiServices,
                (EFI_CAPSULE_BLOCK_DESCRIPTOR *) CapsuleDataPtr
                );
      if (Status != EFI_SUCCESS) {
        return EFI_NOT_FOUND;
      }

      //
      // Return the base of the block descriptors
      //
      *BlockList = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) CapsuleDataPtr;
      if (((*BlockList)->Data == 0) && ((*BlockList)->Length == 0)) {
        return EFI_NOT_FOUND;
      }
    } else {
      //
      // PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule -- capsule variable not set\n"));
      //
      Status = EFI_NOT_FOUND;
    }
  }

  return Status;
}

STATIC
EFI_STATUS
ValidateCapsuleIntegrity (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR   *BlockList
  )
/*++

Routine Description:
  Check the integrity of the capsule descriptors.

Arguments:
  PeiServices - PEI services table
  BlockList   - pointer to the capsule descriptors

Returns:
  EFI_SUCCESS          - descriptors all ok
  EFI_VOLUME_CORRUPTED - fault detected in the capsule descriptors

--*/
{
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *Ptr;
  UINT32                        Checksum;
  UINT32                        Count;
  //
  // Go through the list to look for inconsistencies. Check for:
  //   * misaligned block descriptors.
  //   * Data + Length < Data (wrap)
  //   * Block descriptor signature
  //   * Block descriptor checksum
  //
  Ptr = BlockList;
  while (1) {
    //
    // Make sure the descriptor is aligned in memory
    //
    if ((UINTN) Ptr & 0x03) {
      PEI_DEBUG ((PeiServices, EFI_D_ERROR, "ValidateCapsuleIntegrity() failed alignment check\n"));
      goto FailDone;
    }
    //
    // Check the checksum
    //
    Checksum = 0;
    for (Count = 0; Count < sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR) / sizeof (UINT32); Count++) {
      Checksum += ((UINT32 *) Ptr)[Count];
    }

    if (Checksum != 0) {
      PEI_DEBUG ((PeiServices, EFI_D_ERROR, "ValidateCapsuleIntegrity() failed descriptor checksum\n"));
      goto FailDone;
    }
    //
    // If data pointer is NULL then we're done
    //
    if (Ptr->Data == (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
      return EFI_SUCCESS;
    }

    if (Ptr->Length == 0) {
      //
      // Descriptor points to another list of block descriptors somewhere
      // else.
      //
      Ptr = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) Ptr->Data;
    } else {
      if ((UINTN) Ptr->Data > (UINTN) Ptr->Data + Ptr->Length) {
        PEI_DEBUG ((PeiServices, EFI_D_ERROR, "ValidateCapsuleIntegrity failed data wrap ptr/size\n"));
        goto FailDone;
      }

      Ptr++;
    }
  }

  return EFI_SUCCESS;
FailDone:
  //
  // Report the error
  //
  PEI_REPORT_STATUS_CODE (
    PeiServices,
    EFI_ERROR_CODE | EFI_ERROR_MAJOR, // CodeType
    EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_EC_INVALID_CAPSULE_DESCRIPTOR,
    0,                                // Instance
    &gEfiCapsuleVendorGuid,           // CallerId
    NULL                              // Optional data
    );

  return EFI_VOLUME_CORRUPTED;
}

STATIC
BOOLEAN
IsOverlapped (
  UINT8     *Buff1,
  UINTN     Size1,
  UINT8     *Buff2,
  UINTN     Size2
  )
/*++

Routine Description:
  Determine if two buffers overlap in memory.

Arguments:
  Buff1   - pointer to first buffer
  Size1   - size of Buff1
  Buff2   - pointer to second buffer
  Size2   - size of Buff2

Returns:

  TRUE    - if buffers overlap in memory
  FALSE   - if they don't overlap

--*/
{
  //
  // If buff1's end is less than the start of buff2, then it's ok.
  // Also, if buff1's start is beyond buff2's end, then it's ok.
  //
  if (((Buff1 + Size1) <= Buff2) || (Buff1 >= (Buff2 + Size2))) {
    return FALSE;
  }

  return TRUE;
}

STATIC
UINT8 *
FindFreeMem (
  EFI_CAPSULE_BLOCK_DESCRIPTOR      *BlockList,
  UINT8                             *MemBase,
  UINTN                             MemSize,
  UINTN                             DataSize
  )
/*++

Routine Description:
  Given a pointer to the capsule block list, info on the available system
  memory, and the size of a buffer, find a free block of memory where a
  buffer of the given size can be copied to safely.

Arguments:
  BlockList   - pointer to head of capsule block descriptors
  MemBase     - pointer to the base of memory in which we want to find free
                space
  MemSize     - the size of the block of memory pointed to by MemBase
  DataSize    - how big a free block we want to find

Returns:
  Returns a pointer to a memory block of at least DataSize that lies somewhere
  between MemBase and (MemBase + MemSize). The memory pointed to does not
  contain any of the capsule block descriptors or capsule blocks pointed to
  by the BlockList.

--*/
{
  UINTN                         Size;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *CurrDesc;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempDesc;
  UINT8                         *MemEnd;
  BOOLEAN                       Failed;

  //
  // Need at least enough to copy the data to at the end of the buffer, so
  // say the end is less the data size for easy comparisons here.
  //
  MemEnd    = MemBase + MemSize - DataSize;
  CurrDesc  = BlockList;
  //
  // Go through all the descriptor blocks and see if any obstruct the range
  //
  while (CurrDesc != NULL) {
    //
    // Get the size of this block list and see if it's in the way
    //
    Failed    = FALSE;
    TempDesc  = CurrDesc;
    Size      = sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
    while (TempDesc->Length != 0) {
      Size += sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
      TempDesc++;
    }

    if (IsOverlapped (MemBase, DataSize, (UINT8 *) CurrDesc, Size)) {
      //
      // Set our new base to the end of this block list and start all over
      //
      MemBase   = (UINT8 *) CurrDesc + Size;
      CurrDesc  = BlockList;
      if (MemBase > MemEnd) {
        return NULL;
      }

      Failed = TRUE;
    }
    //
    // Now go through all the blocks and make sure none are in the way
    //
    while ((CurrDesc->Length != 0) && (!Failed)) {
      if (IsOverlapped (MemBase, DataSize, (UINT8 *) (UINTN) CurrDesc->Data, (UINTN) CurrDesc->Length)) {
        //
        // Set our new base to the end of this block and start all over
        //
        Failed    = TRUE;
        MemBase   = (UINT8 *) ((UINTN) CurrDesc->Data) + CurrDesc->Length;
        CurrDesc  = BlockList;
        if (MemBase > MemEnd) {
          return NULL;
        }
      }

      CurrDesc++;
    }
    //
    // Normal continuation -- jump to next block descriptor list
    //
    if (!Failed) {
      CurrDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) CurrDesc->Data;
    }
  }

  return MemBase;
}

STATIC
EFI_STATUS
EFIAPI
CheckCapsuleUpdate (
  IN EFI_PEI_SERVICES           **PeiServices
  )
/*++

Routine Description:
  Determine if we're in capsule update boot mode.

Arguments:
  PeiServices - PEI services table

Returns:
  EFI_SUCCESS   - if we have a capsule available
  EFI_NOT_FOUND - no capsule detected

--*/
{
  EFI_STATUS  Status;
  Status = GetCapsuleDescriptors (PeiServices, NULL);
  return Status;
}

STATIC
EFI_STATUS
GetCapsuleInfo (
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR *Desc,
  IN OUT UINTN                    *NumDescriptors OPTIONAL,
  IN OUT UINTN                    *CapsuleSize OPTIONAL
  )
/*++

Routine Description:
  Given a pointer to a capsule block descriptor, traverse the list to figure
  out how many legitimate descriptors there are, and how big the capsule it
  refers to is.

Arguments:
  Desc            - pointer to the capsule block descriptors
  NumDescriptors  - optional pointer to where to return the number of descriptors
  CapsuleSize     - optional pointer to where to return the capsule size

Returns:
  EFI_NOT_FOUND   - no descriptors containing data in the list
  EFI_SUCCESS     - return data is valid

--*/
{
  UINTN Count;
  UINTN Size;

  Count = 0;
  Size  = 0;
  while (Desc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
    if (Desc->Length == 0) {
      //
      // Descriptor points to another list of block descriptors somewhere
      //
      Desc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) Desc->Data;
    } else {
      Size += (UINTN) Desc->Length;
      Count++;
      Desc++;
    }
  }
  //
  // If no descriptors, then fail
  //
  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  if (NumDescriptors != NULL) {
    *NumDescriptors = Count;
  }

  if (CapsuleSize != NULL) {
    *CapsuleSize = Size;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_CAPSULE_BLOCK_DESCRIPTOR *
RelocateBlockDescriptors (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR       *BlockList,
  IN UINT8                              *MemBase,
  IN UINTN                              MemSize
  )
/*++

Routine Description:
  The capsule block descriptors may be fragmented and spread all over memory.
  To simplify the coalescing of capsule blocks, first coalesce all the
  capsule block descriptors low in memory.

Arguments:
  PeiService  - pointer to PEI services table
  BlockList   - pointer to the capsule block descriptors
  MemBase     - base of system memory in which we can work
  MemSize     - size of the system memory pointed to by MemBase

Returns:
  NULL    could not relocate the descriptors
  Pointer to the base of the successfully-relocated block descriptors.

Notes:
  The descriptors passed in can be fragmented throughout memory. Here
  we relocate them in memory to turn them into a contiguous (null
  terminated) array.

--*/
// GC_TODO:    PeiServices - add argument and description to function comment
{
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *NewBlockList;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *CurrBlockDescHead;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockDesc;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *PrevBlockDescTail;
  UINTN                         NumDescriptors;
  UINTN                         BufferSize;
  UINT8                         *RelocBuffer;
  UINTN                         BlockListSize;
  //
  // Get the info on the blocks and descriptors. Since we're going to move
  // the descriptors low in memory, adjust the base/size values accordingly here.
  // GetCapsuleInfo() returns the number of legit descriptors, so add one for
  // a terminator.
  //
  if (GetCapsuleInfo (BlockList, &NumDescriptors, NULL) != EFI_SUCCESS) {
    return NULL;
  }

  NumDescriptors++;
  BufferSize    = NumDescriptors * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
  NewBlockList  = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) MemBase;
  if (MemSize < BufferSize) {
    return NULL;
  }

  MemSize -= BufferSize;
  MemBase += BufferSize;
  //
  // Go through all the blocks and make sure none are in the way
  //
  TempBlockDesc = BlockList;
  while (TempBlockDesc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
    if (TempBlockDesc->Length == 0) {
      //
      // Next block of descriptors
      //
      TempBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) TempBlockDesc->Data;
    } else {
      //
      // If the capsule data pointed to by this descriptor is in the way,
      // move it.
      //
      if (IsOverlapped (
            (UINT8 *) NewBlockList,
            BufferSize,
            (UINT8 *) (UINTN) TempBlockDesc->Data,
            (UINTN) TempBlockDesc->Length
            )) {
        //
        // Relocate the block
        //
        RelocBuffer = FindFreeMem (BlockList, MemBase, MemSize, (UINTN) TempBlockDesc->Length);
        if (RelocBuffer == NULL) {
          return NULL;
        }
        (**PeiServices).CopyMem ((VOID *) RelocBuffer, (VOID *) (UINTN) TempBlockDesc->Data, (UINTN) TempBlockDesc->Length);
        TempBlockDesc->Data = (EFI_PHYSICAL_ADDRESS) (UINTN) RelocBuffer;
        DEBUG_CODE (
          PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule relocate descriptors from/to/size "));
          PrintHex (PeiServices, (UINT32) (UINTN) TempBlockDesc->Data, PRINT_HEX_SPACE);
          PrintHex (PeiServices, (UINT32) (UINTN) RelocBuffer, PRINT_HEX_SPACE);
          PrintHex (PeiServices, (UINT32) (UINTN) TempBlockDesc->Length, PRINT_HEX_NEWLINE);
        )
      }
    }

    TempBlockDesc++;
  }
  //
  // Now go through all the block descriptors to make sure that they're not
  // in the memory region we want to copy them to.
  //
  CurrBlockDescHead = BlockList;
  PrevBlockDescTail = NULL;
  while ((CurrBlockDescHead != NULL) && (CurrBlockDescHead->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL)) {
    //
    // Get the size of this list then see if it overlaps our low region
    //
    TempBlockDesc = CurrBlockDescHead;
    BlockListSize = sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
    while (TempBlockDesc->Length != 0) {
      BlockListSize += sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
      TempBlockDesc++;
    }

    if (IsOverlapped (
          (UINT8 *) NewBlockList,
          BufferSize,
          (UINT8 *) CurrBlockDescHead,
          BlockListSize
          )) {
      //
      // Overlaps, so move it out of the way
      //
      RelocBuffer = FindFreeMem (BlockList, MemBase, MemSize, BlockListSize);
      if (RelocBuffer == NULL) {
        return NULL;
      }
      (**PeiServices).CopyMem ((VOID *) RelocBuffer, (VOID *) CurrBlockDescHead, BlockListSize);
      DEBUG_CODE (
        PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule reloc descriptor block #2\n"));
      )
      //
      // Point the previous block's next point to this copied version. If
      // the tail pointer is null, then this is the first descriptor block.
      //
      if (PrevBlockDescTail == NULL) {
        BlockList = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) RelocBuffer;
      } else {
        PrevBlockDescTail->Data = (EFI_PHYSICAL_ADDRESS) (UINTN) RelocBuffer;
      }
    }
    //
    // Save our new tail and jump to the next block list
    //
    PrevBlockDescTail = TempBlockDesc;
    CurrBlockDescHead = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) TempBlockDesc->Data;
  }
  //
  // Cleared out low memory. Now copy the descriptors down there.
  //
  TempBlockDesc     = BlockList;
  CurrBlockDescHead = NewBlockList;
  while ((TempBlockDesc != NULL) && (TempBlockDesc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL)) {
    if (TempBlockDesc->Length != 0) {
      CurrBlockDescHead->Data   = TempBlockDesc->Data;
      CurrBlockDescHead->Length = TempBlockDesc->Length;
      CurrBlockDescHead++;
      TempBlockDesc++;
    } else {
      TempBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) TempBlockDesc->Data;
    }
  }
  //
  // Null terminate
  //
  CurrBlockDescHead->Data   = (EFI_PHYSICAL_ADDRESS) (UINTN) NULL;
  CurrBlockDescHead->Length = 0;
  return NewBlockList;
}

STATIC
EFI_STATUS
EFIAPI
CreateState (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN VOID                             *CapsuleBase,
  IN UINTN                            CapsuleSize
  )
/*++

Routine Description:
  Capsule PPI service that gets called after memory is available. The
  capsule coalesce function, which must be called first, returns a base
  address and size, which can be anything actually. Once the memory init
  PEIM has discovered memory, then it should call this function and pass in
  the base address and size returned by the coalesce function. Then this
  function can create a capsule HOB and return.

Arguments:
  PeiServices   - standard pei services pointer
  CapsuleBase   - address returned by the capsule coalesce function. Most
                  likely this will actually be a pointer to private data.
  CapsuleSize   - value returned by the capsule coalesce function.

Returns:
  EFI_VOLUME_CORRUPTED  - CapsuleBase does not appear to point to a
                          coalesced capsule
  EFI_SUCCESS if all goes well.

Notes:
  This function makes the assumption that it won't be called unless we're
  actually doing a capsule update.

--*/
{
  //
  //  EFI_HOB_CAPSULE_VOLUME          HobData;
  //
  EFI_STATUS                    Status;
  EFI_CAPSULE_PEIM_PRIVATE_DATA *PrivateData;
  UINTN                         NumPages;
  UINTN                         Size;
  EFI_PHYSICAL_ADDRESS          NewBuffer;

  PrivateData = (EFI_CAPSULE_PEIM_PRIVATE_DATA *) CapsuleBase;
  if (PrivateData->Signature != EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }
  //
  // Allocate the memory so that it gets preserved into DXE
  //
  Size      = (UINTN) PrivateData->CapsuleSize;
  NumPages  = EFI_SIZE_TO_PAGES (Size);
  Status = (**PeiServices).AllocatePages (
                            PeiServices,
                            EfiBootServicesData,
                            NumPages,
                            &NewBuffer
                            );

  if (Status != EFI_SUCCESS) {
    return Status;
  }
  //
  // Copy to our new buffer and build a capsule HOB for DXE
  //
  (**PeiServices).CopyMem ((VOID *) (UINTN) NewBuffer, (VOID *) (UINTN) (PrivateData + 1), Size);
  DEBUG_CODE (
    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule copy\n"));
  )
  //
  // Check for test data pattern. If it is the test pattern, then we'll
  // test it ans still create the HOB so that it can be used to verify
  // that capsules don't get corrupted all the way into BDS. BDS will
  // still try to turn it into a firmware volume, but will think it's
  // corrupted so nothing will happen.
  //
  CapsuleTestPattern (PeiServices, (VOID *) (UINTN) NewBuffer);
  Status = BuildHobCv (PeiServices, NewBuffer, PrivateData->CapsuleSize);
  return Status;
}

STATIC
EFI_STATUS
BuildHobCv (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN EFI_PHYSICAL_ADDRESS        BaseAddress,
  IN UINT64                      Length
  )
/*++

Routine Description:
  Builds a Capsule Volume HOB

Arguments:
  PeiServices - The PEI core services table.

  BaseAddress - The base address of the Capsule Volume

  Length      - The size of the Capsule Volume in bytes

Returns:
  EFI_SUCCESS   all went well
  Return value from CreateHob() otherwise.

--*/
{
  EFI_STATUS              Status;
  EFI_HOB_CAPSULE_VOLUME  *Hob;

  Status = (*PeiServices)->CreateHob (
                            PeiServices,
                            EFI_HOB_TYPE_CV,
                            sizeof (EFI_HOB_CAPSULE_VOLUME),
                            &Hob
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hob->BaseAddress  = BaseAddress;
  Hob->Length       = Length;
  return EFI_SUCCESS;
}

STATIC
BOOLEAN
CapsuleTestPattern (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN VOID                             *CapsuleBase
  )
/*++

Routine Description:
  The CapsuleApp has a test-mode where it will put a fixed pattern in
  memory and initiate a capsule update. This function will look at a
  capsule and determine if it's a test pattern. If it is, then it
  will verify it and emit an error message if corruption is detected.

Arguments:
  PeiServices   - standard pei services pointer
  CapsuleBase   - base address of coalesced capsule, which is preceeded
                  by private data. Very implementation specific.

Returns:
  TRUE    if we're in capsule-test mode
  FALSE   otherwise

Notes:
  Ideally we could wrap this in a DEBUG_CODE macro, but it does
  not compile that way. Therefore #ifdef as much as possible to
  minimize code size.

--*/
{
#ifdef EFI_DEBUG
  UINT32  *TestPtr;

  UINT32  TestCounter;

  UINT32  TestSize;
#endif
  BOOLEAN RetValue;

  RetValue = FALSE;
  //
  // Look at the capsule data and determine if it's a test pattern. If it
  // is, then test it now.
  //
#ifdef EFI_DEBUG
  TestPtr = (UINT32 *) CapsuleBase;
  if (*TestPtr == 0x54534554) {
    RetValue = TRUE;
    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern mode activated...\n"));
    TestSize = TestPtr[1] / sizeof (UINT32);
    TestPtr += 2;
    TestCounter = 0;
    while (TestSize > 0) {
      if (*TestPtr != TestCounter) {
        PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern mode FAILED: BaseAddr/FailAddr "));
        PrintHex (PeiServices, (UINT32) (UINTN) (EFI_CAPSULE_PEIM_PRIVATE_DATA *) CapsuleBase, PRINT_HEX_SPACE);
        PrintHex (PeiServices, (UINT32) (UINTN) TestPtr, PRINT_HEX_NEWLINE);
        return TRUE;
      }

      TestPtr++;
      TestCounter++;
      TestSize--;
    }

    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern mode SUCCESS\n"));
  }
#endif
  return RetValue;
}

#ifdef EFI_DEBUG
STATIC
VOID
CapsuleTestPatternPreCoalesce (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR *Desc
  )
/*++

Routine Description:
  Try to verify the integrity of a capsule test pattern before the
  capsule gets coalesced. This can be useful in narrowing down
  where capsule data corruption occurs.

Arguments:
  PeiServices - PEI services table
  Desc        - pointer to capsule descriptors

Returns:
  NA

Notes:
  The test pattern mode fills in memory with a counting UINT32 value.
  If the capsule is not divided up in a multiple of 4-byte blocks, then
  things get messy doing the check. Therefore there are some cases
  here where we just give up and skip the pre-coalesce check.

--*/
{
  UINT32  *TestPtr;

  UINT32  TestCounter;

  UINT32  TestSize;
  //
  // Find first data descriptor
  //
  while ((Desc->Length == 0) && (Desc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL)) {
    Desc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) Desc->Data;
  }

  if (Desc->Data == 0) {
    return ;
  }
  //
  // First one better be long enough to at least hold the test signature
  //
  if (Desc->Length < sizeof (UINT32)) {
    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern pre-coalesce punted #1\n"));
    return ;
  }

  TestPtr = (UINT32 *) (UINTN) Desc->Data;
  if (*TestPtr != 0x54534554) {
    return ;
  }

  TestCounter = 0;
  TestSize    = (UINTN) Desc->Length - 2 * sizeof (UINT32);
  //
  // Skip over the signature and the size fields in the pattern data header
  //
  TestPtr += 2;
  while (1) {
    if (TestSize & 0x03) {
      PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern pre-coalesce punted #2\n"));
      return ;
    }

    while (TestSize > 0) {
      if (*TestPtr != TestCounter) {
        PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Capsule test pattern pre-coalesce failed data corruption check\n"));
        return ;
      }

      TestSize -= sizeof (UINT32);
      TestCounter++;
      TestPtr++;
    }

    Desc++;
    while ((Desc->Length == 0) && (Desc->Data != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL)) {
      Desc = (EFI_CAPSULE_BLOCK_DESCRIPTOR *) (UINTN) Desc->Data;
    }

    if (Desc->Data == (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
      return ;
    }

    TestSize = (UINTN) Desc->Length;
  }
}

STATIC
VOID
PrintHex (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN UINT32                           Num,
  IN UINT32                           PrintFlags
  )
/*++

Routine Description:
  Print a HEX number to the standard output

Arguments:
  PeiServices - pointer to the PEI services table
  Num         - the value to print
  PrintFlags  - modifiers on how to print the value

Returns:
  NA

--*/
{
  UINT32  Mask;
  INT32   Nibble;
  CHAR8   HexString[12];

  Mask    = 0xF0000000;
  Nibble  = 7;
  while (Nibble >= 0) {
    HexString[7 - Nibble] = HexDigits[(Num & Mask) >> 28];
    Num <<= 4;
    Nibble--;
  }

  Nibble = 8;
  if (PrintFlags & PRINT_HEX_SPACE) {
    HexString[Nibble++] = ' ';
  }

  if (PrintFlags & PRINT_HEX_NEWLINE) {
    HexString[Nibble++] = '\n';
  }

  HexString[Nibble] = 0;
  PEI_DEBUG ((PeiServices, EFI_D_ERROR, HexString));
}
#endif // #ifdef EFI_DEBUG
