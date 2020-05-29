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

/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/** 
  C funtions in SEC

Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


**/

#include "EmuSecMain.h"

EFI_STATUS
EFIAPI
EmuSecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  );

EFI_PEI_TEMPORARY_RAM_SUPPORT_PPI gSecTemporaryRamSupportPpi = { SecTemporaryRamSupport };

EFI_SEC_PLATFORM_INFORMATION_PPI  mSecPlatformInformationPpi = { EmuSecPlatformInformation };

EFI_PEI_PPI_DESCRIPTOR            mPeiSecPlatformInformationPpi[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEmuPeiPpiGuid,
    NULL                          // It will be replaced with &mEmuPeiData in runtime. 
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI),
    &gEfiTemporaryRamSupportPpiGuid,
    &gSecTemporaryRamSupportPpi
  }
    ,
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiSecPlatformInformationPpiGuid,
    &mSecPlatformInformationPpi
  }
};

FAST_RECOVERY_DXE_TO_PEI_DATA *mEmuPeiData;

//
// These are IDT entries pointing to 10:FFFFFFE4h.
//
UINT64  mIdtEntryTemplate = 0xffff8e000010ffe4ULL;

VOID
EFIAPI
SecStartupPhase2(
  IN VOID                     *Context
  );

EFI_STATUS 
EmuSecMaskitself (
  IN EFI_FFS_FILE_HEADER   *FfsHeader,
  IN UINT8                 ErasePolarity
);

/**

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.


  @param SizeOfRam           Size of the temporary memory available for use.
  @param TempRamBase         Base address of tempory ram
  @param BootFirmwareVolume  Base address of the Boot Firmware Volume.
  
**/
EFI_STATUS
EFIAPI
SecStartup (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  IA32_DESCRIPTOR             IdtDescriptor;
  SEC_IDT_TABLE               IdtTableInStack;
  UINT32                      Index;
  EFI_SEC_PEI_HAND_OFF        SecCoreData;
  UINTN                       Counter;
  UINT32                      PeiStackSize;

  //
  // Check if it is being despatched as normal PEIM.
  //
  if ((VOID *)FileHandle != (VOID *)PeiServices) {
    return EFI_SUCCESS;
  }

  //
  // PostCode = 0x09, Setup BIOS ROM cache
  //
  //POST_CODE (SEC_GO_TO_SECSTARTUP); 

  mEmuPeiData = (FAST_RECOVERY_DXE_TO_PEI_DATA *)FileHandle; 
  //
  // Mask itself to prevent Pei dispatcher dispatch it. 
  //
  EmuSecMaskitself(
        (EFI_FFS_FILE_HEADER *)((UINTN)mEmuPeiData->EmuSecFfsAddress),
        (((((EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)mEmuPeiData->EmuPeiFv)->Attributes) & EFI_FVB2_ERASE_POLARITY) != 0 ? 1 : 0) 
        );
        

  //
  // Change EmuPeiMark Ppi to DxeToPeiData for we can not decide it in compile
  // time. 
  //
  Counter = 0;
  do {
    
    if (CompareGuid(mPeiSecPlatformInformationPpi[Counter].Guid, &gEmuPeiPpiGuid)) {
      mPeiSecPlatformInformationPpi[Counter].Ppi = (VOID *)mEmuPeiData; 
    }
    Counter ++;
    
  } while ((mPeiSecPlatformInformationPpi[Counter].Flags & EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST) == 0);


  // |-------------------|---->
  // |Idt Table          |
  // |-------------------|
  // |PeiService Pointer |    PeiStackSize
  // |-------------------|
  // |                   |
  // |      Stack        |
  // |-------------------|---->
  // |                   |
  // |                   |
  // |      Heap         |    PeiTemporayRamSize
  // |                   |
  // |                   |
  // |-------------------|---->  TempRamBase

  IdtTableInStack.PeiService = 0;
  for (Index = 0; Index < SEC_IDT_ENTRY_COUNT; Index ++) {
    CopyMem ((VOID*)&IdtTableInStack.IdtTable[Index], (VOID*)&mIdtEntryTemplate, sizeof (UINT64));
  }

  IdtDescriptor.Base  = (UINTN) &IdtTableInStack.IdtTable;
  IdtDescriptor.Limit = (UINT16)(sizeof (IdtTableInStack.IdtTable) - 1);

  AsmWriteIdtr (&IdtDescriptor);
  
  if (PcdGet32(PcdPeiTemporaryRamStackSizeWhenRecovery) == 0) {
    PeiStackSize = PcdGet32(PcdTemporaryRamSizeWhenRecovery) >> 1;
  } else {
    PeiStackSize = PcdGet32(PcdPeiTemporaryRamStackSizeWhenRecovery);
  }

  SecCoreData.DataSize               = (UINT16) sizeof (EFI_SEC_PEI_HAND_OFF);
  SecCoreData.BootFirmwareVolumeBase = (VOID *)(UINTN)mEmuPeiData->EmuPeiFv;
  SecCoreData.BootFirmwareVolumeSize = (UINTN)(((EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)mEmuPeiData->EmuPeiFv)->FvLength);
  SecCoreData.TemporaryRamBase       = (VOID*)(UINTN) mEmuPeiData->StackData;
  SecCoreData.TemporaryRamSize       = PcdGet32(PcdTemporaryRamSizeWhenRecovery);
  SecCoreData.PeiTemporaryRamBase    = SecCoreData.TemporaryRamBase;
  SecCoreData.PeiTemporaryRamSize    = PcdGet32(PcdTemporaryRamSizeWhenRecovery) - PeiStackSize;  
  SecCoreData.StackBase = (VOID*)(UINTN)(mEmuPeiData->StackData + SecCoreData.PeiTemporaryRamSize);
  SecCoreData.StackSize = PeiStackSize;

  //
  // Initialize Debug Agent to support source level debug in SEC/PEI phases before memory ready.
  //
  InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, &SecCoreData, SecStartupPhase2);
  return EFI_SUCCESS;

}

/**
  Caller provided function to be invoked at the end of InitializeDebugAgent().

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.

  @param[in] Context    The first input parameter of InitializeDebugAgent().

**/
VOID
EFIAPI
SecStartupPhase2(
  IN VOID                     *Context
  )
{
  EFI_SEC_PEI_HAND_OFF        *SecCoreData;
  EFI_PEI_PPI_DESCRIPTOR      *PpiList;

  SecCoreData = (EFI_SEC_PEI_HAND_OFF *) Context;
  PpiList = &mPeiSecPlatformInformationPpi[0];
  
  //
  // Transfer the control to the PEI core
  // PostCode = 0x0A, Enter Boot Firmware Volume
  //
  //POST_CODE (SEC_GO_TO_PEICORE);

  //
  // Transfer the control to the PEI core
  //
  (*(EFI_PEI_CORE_ENTRY_POINT)(UINTN)mEmuPeiData->EmuPeiEntry)(SecCoreData, PpiList);

  //
  // Should not come here.
  //
  return ;
}

/**
  This service of the TEMPORARY_RAM_SUPPORT_PPI that migrates temporary RAM into
  permanent memory.

  @param PeiServices            Pointer to the PEI Services Table.
  @param TemporaryMemoryBase    Source Address in temporary memory from which the SEC or PEIM will copy the
                                Temporary RAM contents.
  @param PermanentMemoryBase    Destination Address in permanent memory into which the SEC or PEIM will copy the
                                Temporary RAM contents.
  @param CopySize               Amount of memory to migrate from temporary to permanent memory.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_INVALID_PARAMETER PermanentMemoryBase + CopySize > TemporaryMemoryBase when
                                TemporaryMemoryBase > PermanentMemoryBase.

**/
EFI_STATUS
EFIAPI
SecTemporaryRamSupport (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
  )
{
  IA32_DESCRIPTOR   IdtDescriptor;
  VOID*             OldHeap;
  VOID*             NewHeap;
  VOID*             OldStack;
  VOID*             NewStack;
  DEBUG_AGENT_CONTEXT_POSTMEM_SEC  DebugAgentContext;
  BOOLEAN           OldStatus;
  UINTN             PeiStackSize;

  PeiStackSize = (UINTN)PcdGet32 (PcdPeiTemporaryRamStackSizeWhenRecovery);
  if (PeiStackSize == 0) {
    PeiStackSize = (CopySize >> 1);
  }

  ASSERT (PeiStackSize < CopySize);

  //
  // |-------------------|---->
  // |      Stack        |    PeiStackSize
  // |-------------------|---->
  // |      Heap         |    PeiTemporayRamSize
  // |-------------------|---->  TempRamBase
  //
  // |-------------------|---->
  // |      Heap         |    PeiTemporayRamSize
  // |-------------------|---->
  // |      Stack        |    PeiStackSize
  // |-------------------|---->  PermanentMemoryBase
  //

  OldHeap = (VOID*)(UINTN)TemporaryMemoryBase;
  NewHeap = (VOID*)((UINTN)PermanentMemoryBase + PeiStackSize);
  
  OldStack = (VOID*)((UINTN)TemporaryMemoryBase + CopySize - PeiStackSize);
  NewStack = (VOID*)(UINTN)PermanentMemoryBase;

  DebugAgentContext.HeapMigrateOffset = (UINTN)NewHeap - (UINTN)OldHeap;
  DebugAgentContext.StackMigrateOffset = (UINTN)NewStack - (UINTN)OldStack;
  
  OldStatus = SaveAndSetDebugTimerInterrupt (FALSE);
  //
  // Initialize Debug Agent to support source level debug in PEI phase after memory ready.
  // It will build HOB and fix up the pointer in IDT table.
  //
  InitializeDebugAgent (DEBUG_AGENT_INIT_POSTMEM_SEC, (VOID *) &DebugAgentContext, NULL);

  //
  // Migrate Heap
  //
  CopyMem (NewHeap, OldHeap, CopySize - PeiStackSize);

  //
  // Migrate Stack
  //
  CopyMem (NewStack, OldStack, PeiStackSize);


  //
  // We need *not* fix the return address because currently,
  // The PeiCore is executed in flash.
  //

  //
  // Rebase IDT table in permanent memory
  //
  AsmReadIdtr (&IdtDescriptor);
  IdtDescriptor.Base = IdtDescriptor.Base - (UINTN)OldStack + (UINTN)NewStack;

  AsmWriteIdtr (&IdtDescriptor);


  //
  // Program MTRR
  //

  //
  // SecSwitchStack function must be invoked after the memory migration
  // immediatly, also we need fixup the stack change caused by new call into
  // permenent memory.
  //
  SecSwitchStack (
    (UINT32) (UINTN) OldStack,
    (UINT32) (UINTN) NewStack
    );

  SaveAndSetDebugTimerInterrupt (OldStatus);
  
  return EFI_SUCCESS;
}

/*++

Routine Description:

  Change EmuSec Ffs entry to PEI un-dispatchable by setting Ffs state as
  deleted. Then, PEIM dispatched will not treat this module as a valid entry and
  try to dispatch it.
  

Arguments:

  FfsHeader     - Point to Emu PEI Ffs entry.
  ErasePolarity - Firmware Volume erase polarity 
  
  
Returns:

  EFI_SUCCESS   - Operation is successful complete.

--*/
EFI_STATUS 
EmuSecMaskitself (
  IN EFI_FFS_FILE_HEADER   *FfsHeader,
  IN UINT8                 ErasePolarity
)
{

  FfsHeader->State =  EFI_FILE_DELETED | 
                      EFI_FILE_MARKED_FOR_UPDATE | 
                      EFI_FILE_DATA_VALID | 
                      EFI_FILE_HEADER_VALID | 
                      EFI_FILE_HEADER_CONSTRUCTION;

  if (ErasePolarity != 0) {
    FfsHeader->State = ~FfsHeader->State; 
  }

  
  return EFI_SUCCESS;

}



/*++

Routine Description:

  Implementation of the PlatformInformation service in
  EFI_SEC_PLATFORM_INFORMATION_PPI.
  This function conveys state information out of the SEC phase into PEI.

Arguments:

  PeiServices               - Pointer to the PEI Services Table.
  StructureSize             - Pointer to the variable describing size of the 
                              input buffer.
  PlatformInformationRecord - Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

Returns:

  EFI_SUCCESS               - The data was successfully returned.
  EFI_BUFFER_TOO_SMALL      - The buffer was too small.

--*/
EFI_STATUS
EFIAPI
EmuSecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  )
{
  PlatformInformationRecord = NULL;
  *StructureSize = 0;
  return EFI_NOT_FOUND;
//  UINTN                           SecInformationSize;
//  VOID                            *SecInformation;
//  VOID                            *HobList;
//
//  HobList = (VOID *)(UINTN)mEmuPeiData->OriginalHobList; 
//
//  SecInformation = GetNextGuidHob (
//                            &gEfiHtBistHobGuid,
//                            HobList
//                            );
//
//  if (SecInformation == NULL) {
//    SecInformationSize = 0;
//  }
//
//  SecInformationSize = ((EFI_PEI_HOB_POINTERS *)SecInformation)->Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);
//
//  if ((*StructureSize) < (UINT64)SecInformationSize) {
//    *StructureSize = SecInformationSize;
//    return EFI_BUFFER_TOO_SMALL;
//  }
//
//  CopyMem (
//        PlatformInformationRecord, 
//        SecInformation, 
//        SecInformationSize
//        );
//  
//  *StructureSize = SecInformationSize;
//  
//  return EFI_SUCCESS;
}

