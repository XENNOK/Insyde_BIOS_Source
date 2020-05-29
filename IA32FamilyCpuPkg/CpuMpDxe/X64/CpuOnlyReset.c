/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for cpu only reset.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuOnlyReset.c

**/

#include "CpuOnlyReset.h"

/**
  Resume entry for CPU Only Reset.

  The assembly function CpuOnlyResetResumeEntryWrapper() will transfer the control to this C function
  after it switches to 64-bit mode.

  @param Context1  Pointer to the CPU Only Reset context buffer.
  @param Context2  Pointer to a context buffer which can be accessed after resuming from the CPU only reset.
**/
VOID
EFIAPI
CpuOnlyResetResumeEntry (
  IN      VOID                      *Context1,
  IN      VOID                      *Context2   OPTIONAL
  );

/**
  Save the context before CPU only reset.

  @return A pointer to the CPU only reset context buffer.
**/
CPU_ONLY_RESET_CONTEXT_BUFFER*
PreCpuOnlyReset (
  VOID
  )
{
  CPU_ONLY_RESET_CONTEXT_BUFFER   *Context;
  CPU_ONLY_RESET_TRAMPOLINE_INFO  *TrampolineInfo;
  CPU_ONLY_RESET_INFO             *Info;
	UINT32                          *Ptr;
	UINT32                          Sum;
	UINT32                          Index;

	//
	// Allocate the CPU context buffer
	//
	Context = AllocatePool (sizeof (CPU_ONLY_RESET_CONTEXT_BUFFER));
  ASSERT (Context != NULL);

	//
	// save the CPU sytem register
	//
	Context->Cr0 = AsmReadCr0();
	Context->Cr3 = AsmReadCr3();
	Context->Cr4 = AsmReadCr4();
	Context->Tr  = AsmReadTr();

	AsmReadGdtr(&Context->GdtDesc);
	AsmReadIdtr(&Context->IdtDesc);

	Context->Ldtr = AsmReadLdtr();
	Context->Cs =	AsmReadCs();
	Context->Ss =	AsmReadSs();
	Context->Ds =	AsmReadDs();
	Context->Es =	AsmReadEs();
	Context->Fs =	AsmReadFs();
	Context->Gs =	AsmReadGs();

  DEBUG_CODE (
    Context->Dr0 =	AsmReadDr0();
    Context->Dr1 =	AsmReadDr1();
    Context->Dr2 =	AsmReadDr2();
    Context->Dr3 =	AsmReadDr3();
    Context->Dr6 =	AsmReadDr6();
    Context->Dr7 =	AsmReadDr7();
  );

  //
  // Prepare the trampoline information at the well know address(say address 0)
  // NOTE: the well know address for trampoline information should be below 4G
  //

  //
  // save the original content at address CPU_ONLY_RESET_TRAMPOLINE_INFO_ADDRESS
  // to context buffer
	//
	CopyMem (
	  (VOID*)Context->CpuOnlyResetTrampolineInfo,
	  (VOID*)(UINTN)CPU_ONLY_RESET_TRAMPOLINE_INFO_BASE_ADDRESS,
	  CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE
	  );
  //
  // init the trampoline memory to zero
  //
  SetMem (
    (VOID*)(UINTN)CPU_ONLY_RESET_TRAMPOLINE_INFO_BASE_ADDRESS,
    CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE,
    0
    );

  //
  // prepare the parameter for calling InternalX86EnablePaging64 to
  // switch to 64-bit code
  //
  TrampolineInfo = AllocatePool (sizeof (CPU_ONLY_RESET_TRAMPOLINE_INFO));
  ASSERT (TrampolineInfo != NULL);
  TrampolineInfo->CpuOnlyResetEntryPoint = (UINTN)CpuOnlyResetResumeEntry;
  TrampolineInfo->NewStack = ((UINT64)(TrampolineInfo->TemporaryStack + CPU_ONLY_RESET_TEMP_STACK_SIZE))& ~0xF; // NOTE: here 16 byte align is necessary
  TrampolineInfo->Context1 = (UINT64)Context;
  TrampolineInfo->Context2 = 0;

  //
  // prepare the temporary gdt, pagetable and cs.
  // ASSUMPTION:
  // The gdt, page table which DXE drivers use and CPU driver itself assumed to
  // be located bellow 4G memory. In this case, we need not implement another
  // set of the GDT, page table and CS. we can just use the one CPU driver is
  // using
  //
  TrampolineInfo->TemporaryCS       = Context->Cs;
  TrampolineInfo->TemporaryGdtLimit = Context->GdtDesc.Limit;
  TrampolineInfo->TemporaryGdtBase  = (UINT32)Context->GdtDesc.Base;
  TrampolineInfo->TemporaryCR3      = (UINT32)Context->Cr3;
	//
	// prepare the cpu only reset information at address zero
	//
  Info = &TrampolineInfo->CpuOnlyResetInfo;

  Info->CpuOnlyResetIndicator = CPU_ONLY_RESET_INDICATOR;
  //
  // The wrapper should locate below 4G. or else we need copy this function to
  // somewhere below 4G. here we assume CPU driver is loaded below 4G
  //
  ASSERT ((UINTN) CpuOnlyResetResumeEntryWrapper < 0xFFFFFFFF );
  Info->EntryPoint            = (UINT32)((UINTN) CpuOnlyResetResumeEntryWrapper & 0xFFFFFFFF);
  Info->NewStack              = (UINT32)(UINTN)&TrampolineInfo->TemporaryGdtLimit ; //NOTE: 4byte aligned is necessary
  //
  // Calculate the checksum of the cpu only reset information table
  //
  Info->Checksum              = 0;
  Sum                         = 0;
  Ptr                         = (UINT32*)Info;
  for (Index = 0; Index < sizeof (CPU_ONLY_RESET_INFO) / sizeof (UINT32); Index++)
  {
  	Sum = Sum + Ptr[Index];
  }
  Info->Checksum  = (UINT32) (0xFFFFFFFF - Sum + 1);
  
  CopyMem (
    (VOID*)(UINTN)CPU_ONLY_RESET_TRAMPOLINE_INFO_BASE_ADDRESS,
    (VOID*)TrampolineInfo,
    CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE
    );
  
  FreePool (TrampolineInfo);

  return  Context;

}

/**
  Clean up after resuming from the CPU only reset.

  @param Context  Pointer to the CPU only reset context buffer.
**/
VOID
PostCpuOnlyReset(
  IN CPU_ONLY_RESET_CONTEXT_BUFFER  *Context
  )
{
	//
  // Restore the original content at address CPU_ONLY_RESET_TRAMPOLINE_INFO_ADDRESS
  // from context buffer
	//
	CopyMem (
	  (VOID*)(UINTN)CPU_ONLY_RESET_TRAMPOLINE_INFO_BASE_ADDRESS,
	  (VOID*)Context->CpuOnlyResetTrampolineInfo,
	  CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE
	  );
	FreePool (Context);
}


/**
  Initiate CPU only reset.

  This function will save CPU context, call SetJump to mark the resume pointer
  and program MCH to trigger CPU only reset.

**/
VOID
InitiateCpuOnlyReset(
  VOID
  )
{
	CPU_ONLY_RESET_CONTEXT_BUFFER   *ContextBuff;
	UINTN                            SetJumpFlag;
	EFI_TPL                          OldTpl;
	//
	// prepare for CPU only reset: allocate  the context buffer,
	// save cpu context, and store CPU only reset information to
	// address zero(which include cpu only reset indicator, resume
	// entry pointer)
	//
	ContextBuff = PreCpuOnlyReset ();
	//
	// Clear the interrupt
 	//
 	OldTpl  = gBS->RaiseTPL (TPL_HIGH_LEVEL);
	//
	// set long jump for resuming after CPU only reset
	//
	SetJumpFlag = SetJump (&ContextBuff->JumpContext);

	//
  // The initial call to SetJump() must always return 0.
  // Subsequent calls to LongJump() cause a non-zero value to be returned by SetJump().
  //
	if (SetJumpFlag == 0) {
	  // Flush all the data in Cache to memory
	  AsmWbinvd ();
	  // program MCH register to trigger CPU only reset
	  CpuOnlyReset ();
	  // never be here
	}
	//
	// Enable interrupt
	//
	gBS->RestoreTPL (OldTpl);
	//
	// resume from the CPU only reset, such as free CPU context buffer
	//
	PostCpuOnlyReset(ContextBuff);

	return;
}

/**
  Resume entry for CPU Only Reset.

  The assembly function CpuOnlyResetResumeEntryWrapper() will transfer the control to this C function
  after it switches to 64-bit mode.

  @param Context1  Pointer to the CPU Only Reset context buffer.
  @param Context2  Pointer to a context buffer which can be accessed after resuming from the CPU only reset.
**/
VOID
EFIAPI
CpuOnlyResetResumeEntry (
  IN      VOID                      *Context1,
  IN      VOID                      *Context2   OPTIONAL
  )
{
  CPU_ONLY_RESET_CONTEXT_BUFFER   *Context;
  UINT16                           TemporaryCS;
  IA32_DESCRIPTOR                  TemporaryGdtr;
  UINT64                           TemporaryCR3;

  Context = (CPU_ONLY_RESET_CONTEXT_BUFFER*)Context1;
  //retore LDTR, CR0, CR4
  AsmWriteCr4 (Context->Cr4);
  AsmWriteCr0 (Context->Cr0);

  AsmWriteIdtr (&Context->IdtDesc);

  // restore DR
  DEBUG_CODE_BEGIN ();
    AsmWriteDr0(Context->Dr0);
    AsmWriteDr1(Context->Dr1);
    AsmWriteDr2(Context->Dr2);
    AsmWriteDr3(Context->Dr3);
    AsmWriteDr6(Context->Dr6);
    AsmWriteDr7(Context->Dr7);
  DEBUG_CODE_END ();
  //
  // Restore GDTR, CS, CR3. These three registers restoration maybe impact
  // the code executing path. however, here we have following assumption:
  //
  // ASSUMPTION:
  // The gdt, page table which DXE drivers use and CPU driver itself assumed to
  // be located bellow 4G memory. In this case, we need not implement another
  // set of the GDT, page table and CS. we can just use the same set of value as
  // CPU driver is using.
  //
  // So we just make sure the value in GDTR, CS, CR3 is equal to the value in
  // context buffer, or else ASSERT
  //
  AsmReadGdtr (&TemporaryGdtr);
  TemporaryCS =	AsmReadCs();
  TemporaryCR3= AsmReadCr3();
  ASSERT (TemporaryGdtr.Base == Context->GdtDesc.Base && TemporaryGdtr.Limit == Context->GdtDesc.Limit);
  ASSERT (TemporaryCS == Context->Cs && TemporaryCR3 == Context->Cr3);
  //
  // there is not library APIs provided for following registers' setting.
  // ES, DS, SS will be ignored in Long mode. And FS, GS, Ldtr and TR is assumed
  // not to use in DXE environment. So we just directly restore it in case some
  // codes in other module have some assumptions for the value in these registers
  //
  AsmWriteLdtr (Context->Ldtr);
 /*
  AsmCpuOnlyResetRestoreMildRegs(
    Context->ES,
    Context->DS,
    Context->SS,
    Context->FS,
    Context->GS,
    Context->TR
  );
  */
  //
  // resume the execution for the SetJump in function InitiateCpuOnlyReset
  //
  LongJump (&Context->JumpContext, 1);
  //
  // never be here
  //
  return;
}
