/** @file

   Chipset SMM Thunk Driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "ChipsetSmmThunkDriver.h"

#define EFI_APIC_GLOBAL_ENABLE                0x800
#define APIC_REGISTER_TPR_OFFSET              0x000000080
#define APIC_REGISTER_TIMER_VECTOR_OFFSET     0x000000320

#ifdef SMM_INT10_ENABLE

  SMM_THUNK_PRIVATE_DATA    mPrivateData = {
    EFI_SMM_THUNK_SIGNATURE,
    NULL,
    {
      SmmFarCall86,
      SmmInt10,
      SmmInt86
    },
    0,
    0,
    0
  };

#else
  SMM_THUNK_PRIVATE_DATA    mPrivateData = {
    EFI_SMM_THUNK_SIGNATURE,
    NULL,
    {
      SmmFarCall86,
      NULL,
      SmmInt86
    },
    0,
    0,
    0
  };
#endif

UINT32                    ApicTPR;
UINT32                    ApicLVT[6];
UINT32                    SavedSmramReg;
BOOLEAN                   SkipSoftEnDisAPIC = FALSE;              
//[-start-140721-IB05580359-add]//
IA32_REGISTER_SET         mRegister;
//[-end-140721-IB05580359-add]//

/**

  Initializes the SMM Thunk Driver
  
 @param[in]         ImageHandle   Handle for the image of this driver
 @param[in]         SystemTable    Pointer to the EFI System Table
 
 @retval EFI_SUCCESS    SMM thunk driver has been initialized successfully                    
 @retval Other               SMM thunk driver init failed
*/
EFI_STATUS
EFIAPI
InitializeChipsetSmmThunkProtocol (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   InSmm;
  EFI_SMM_SYSTEM_TABLE2     *Smst2;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase2;

  SmmBase2 = NULL;
  Smst2 = NULL;

  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase2
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (InSmm) {
    Status = SmmBase2->GetSmstLocation (SmmBase2, &Smst2);
    ASSERT_EFI_ERROR (Status);    
    if (EFI_ERROR(Status)){
      return Status;
    }

    //
    // Allocate buffer for ThunkLib code below 1MB
    //
    mPrivateData.RealModeBuffer = 0x100000;
    Status = Smst2->SmmAllocatePages (
                      AllocateMaxAddress,
                      EfiRuntimeServicesData,
                      EFI_SIZE_TO_PAGES(LOW_STACK_SIZE),
                      &mPrivateData.RealModeBuffer
                      );
    if (!EFI_ERROR (Status)) {
      //
      // Get the real mode thunk implementation the address and size of the real
      // mode buffer needed.
      //
      mPrivateData.BufferSize = EFI_PAGE_SIZE;
      AsmThunk16SetProperties (
        &mPrivateData.ThunkContext,
        (VOID*)(UINTN)mPrivateData.RealModeBuffer,
        (UINT32)mPrivateData.BufferSize
        );
      mPrivateData.ThunkContext.RealModeBuffer = &(mPrivateData.RealModeBuffer);
      mPrivateData.ThunkContext.RealModeBufferSize = mPrivateData.BufferSize;
      AsmPrepareThunk16 (&mPrivateData.ThunkContext);
      
    } else {
      //
      // The SmmThunk functionality force off due to out of A/B segment memory
      // The SmmInt10 is still usable because it is no need to using A/B segment
      //    

      //
      // Use EBDA to replace A/B segment when system isn't support A/B segment 
      //
      mPrivateData.RealModeBuffer = GetBufferfromEBDA (LOW_STACK_SIZE);

      if (mPrivateData.RealModeBuffer) {
        //
        // Get the real mode thunk implementation the address and size of the real
        // mode buffer needed.
        //
        mPrivateData.BufferSize = LOW_STACK_SIZE;
        AsmThunk16SetProperties (
          &mPrivateData.ThunkContext,
          (VOID*)(UINTN)mPrivateData.RealModeBuffer,
          (UINT32)mPrivateData.BufferSize
          );
      }
    }
    Status = gBS->InstallProtocolInterface (
                    &mPrivateData.Handle,
                    &gEfiSmmThunkProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mPrivateData.SmmThunk
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

/**

 Convert EFI_IA32_REGISTER_SET to IA32_REGISTER_SET

 @param[in]         Reg         Register contexted passed into (and returned) from thunk to 
                                        16-bit mode
 @param[out]       Register   Register context will be used in Thunk16Lib
 
 
 @retval None
 
*/
VOID
ConvertRegister (
  IN  EFI_IA32_REGISTER_SET           *Regs,
  OUT IA32_REGISTER_SET               *Register
  )
{
  Register->E.EDI = Regs->E.EDI;
  Register->E.ESI = Regs->E.ESI;
  Register->E.EBP = Regs->E.EBP;
  Register->E.ESP = Regs->E.ESP;
  Register->E.EBX = Regs->E.EBX;
  Register->E.EDX = Regs->E.EDX;
  Register->E.ECX = Regs->E.ECX;
  Register->E.EAX = Regs->E.EAX;
  Register->E.DS  = Regs->E.DS;
  Register->E.ES  = Regs->E.ES;
  Register->E.FS  = Regs->E.FS;
  Register->E.GS  = Regs->E.GS;
  Register->E.CS  = Regs->E.CS;
  Register->E.SS  = Regs->E.SS;

  CopyMem (&Register->E.EFLAGS, &Regs->E.EFlags, sizeof(UINT32));

  return ;
}

/**

  Thunk to 16-bit real mode and call Segment:Offset. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument

 @param[in]         This          Protocol instance pointer. 
 @param[in]         Segment    Segemnt of 16-bit mode call     
 @param[in]         Offset       Offset of 16-bit mdoe call  
 @param[in]         Reg          Register contexted passed into (and returned) from thunk to
                                          16-bit mode
 @param[in]         Stack        Caller allocated stack used to pass arguments  
 @param[in]         StackSize   Size of Stack in bytes
 
 @retval FALSE      Thunk completed, and there were no BIOS errors in the target code.
                           See Regs for status.
 @retval TRUE       There was a BIOS erro in the target code.
 
*/
BOOLEAN
EFIAPI
SmmFarCall86 (
  IN EFI_SMM_THUNK_PROTOCOL           *This,
  IN  UINT16                          Segment,
  IN  UINT16                          Offset,
  IN  EFI_IA32_REGISTER_SET           *Regs OPTIONAL,
  IN  VOID                            *Stack OPTIONAL,
  IN  UINTN                           StackSize
  )
{
#ifdef SMM_INT10_ENABLE
  UINT32              ThunkFlag;
  IA32_REGISTER_SET   Register;
  THUNK_CONTEXT       ThunkContext;
  EFI_STATUS          Status;
  UINTN               *SaveBuffer;

  //
  // Transfer the EFI_IA32_REGISTER_SET register set to
  // IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
    ConvertRegister (Regs, &Register);
  }

  //
  // Set the flag that talk to thunk code
  // this far call will return by iret instruction
  //
  ThunkFlag = 0;

  //
  // Fill the far call address
  //
  Register.E.CS  = Segment;
  Register.E.EIP = Offset;

  if (Segment == 0xC000) {
    //
    // Allocate buffer for saving (0x80000) data
    //
    Status = mSmst->SmmAllocatePool (
             EfiRuntimeServicesData,
             0x1000,
             (VOID **)&SaveBuffer
             );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }

    //
    // Save Data
    //
    CopyMem (SaveBuffer, (VOID*)((UINTN) 0x80000), 0x1000);

    //
    // Prepare environment
    //
    AsmThunk16SetProperties (
      &ThunkContext,
      (VOID*)((UINTN) 0x80000),
      0x1000
      );

    //
    // Disable A0000-BFFFF SMRAM attribute
    //
    DisableSMRAMCAttribute ();

    DisableAPIC ();

    AsmThunk16FarCall86 (&ThunkContext, &Register, (UINT32)ThunkFlag);

    EnableAPIC ();
    //
    // Restore A0000-BFFFF SMRAM attribute
    //
    RestoreSMRAMCAttribute ();
    
    //
    // Destroy environment
    //
    //AsmThunk16Destroy (&ThunkContext);
    ThunkContext->RealModeBuffer = 0;
    
    //
    // Restore Data
    //
    CopyMem ((VOID*)((UINTN) 0x80000), SaveBuffer, 0x1000);

    Status = mSmst->SmmFreePool (SaveBuffer);

  } else {

    if (mPrivateData.RealModeBuffer) {
      AsmThunk16FarCall86 (&mPrivateData.ThunkContext, &Register, (UINT32)ThunkFlag);
    } else {
      //
      // The SmmThunk functionality force off due to out of A/B segment memory
      //
      return FALSE;
    }
  }

  //
  // Transfer the IA32_REGISTER_SET register set to
  // EFI_IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
    ConvertRegisterBack (Regs, &Register);
  }

#else
  UINT32             ThunkFlag;
//[-start-140721-IB05580359-remove]//
//  IA32_REGISTER_SET  Register;
//[-end-140721-IB05580359-remove]//

  if (!mPrivateData.RealModeBuffer) {
    //
    // The SmmThunk functionality force off due to out of A/B segment memory
    //
    return FALSE;
  }
  //
  // Transfer the EFI_IA32_REGISTER_SET register set to
  // IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
//[-start-140721-IB05580359-modify]//
    ConvertRegister (Regs, &mRegister);
//[-end-140721-IB05580359-modify]//
  }

  //
  // Set the flag that talk to thunk code
  // this far call will return by iret instruction
  //
  ThunkFlag = THUNK_INTERRUPT;

  //
  // Fill the far call address
  //
//[-start-140721-IB05580359-modify]//
  mRegister.E.CS  = Segment;
  mRegister.E.Eip = Offset;
//[-end-140721-IB05580359-modify]//

  DisableAPIC ();

//[-start-140721-IB05580359-modify]//
  AsmThunk16FarCall86 (&mPrivateData.ThunkContext, &mRegister, (UINT32)ThunkFlag);
//[-end-140721-IB05580359-modify]//

  EnableAPIC ();
#endif

  return TRUE;
}

/**

  Thunk to 16-bit real mode and call Segment:Offset. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument

 @param[out]       Regs         Register contexted passed into (and returned) from thunk to
                                         16-bit mode
 @param[in]         Register    Register context will be used in Thunk16Lib   
 
 @retval None
 
*/
VOID
ConvertRegisterBack (
  OUT EFI_IA32_REGISTER_SET           *Regs,
  IN  IA32_REGISTER_SET               *Register
  )
{
  Regs->E.EDI = Register->E.EDI;
  Regs->E.ESI = Register->E.ESI;
  Regs->E.EBP = Register->E.EBP;
  Regs->E.ESP = Register->E.ESP;
  Regs->E.EBX = Register->E.EBX;
  Regs->E.EDX = Register->E.EDX;
  Regs->E.ECX = Register->E.ECX;
  Regs->E.EAX = Register->E.EAX;
  Regs->E.DS  = Register->E.DS;
  Regs->E.ES  = Register->E.ES;
  Regs->E.FS  = Register->E.FS;
  Regs->E.GS  = Register->E.GS;
  Regs->E.CS  = Register->E.CS;
  Regs->E.SS  = Register->E.SS;

  CopyMem (&Regs->E.EFlags, &Register->E.EFLAGS, sizeof(UINT32));

  return ;
}

#ifdef SMM_INT10_ENABLE

EFI_STATUS
DisableSMRAMCAttribute (
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  SMM_PCI_IO_ADDRESS                    Address;
  UINT32                                TempSMRAMC;

  Address.Register = 0x88;
  Address.Function = 0;
  Address.Device   = 0;
  Address.Bus      = 0;
  Address.ExtendedRegister = 0;

  SmmPciCfgRead (
    SmmPciWidthUint8,
    &Address,
    &TempSMRAMC
    );
 
  SavedSmramReg = TempSMRAMC;
  
  TempSMRAMC = TempSMRAMC | 0x20;
  
  SmmPciCfgWrite (
    SmmPciWidthUint8,
    &Address,
    &TempSMRAMC
    );

  return EFI_SUCCESS;
}

EFI_STATUS
RestoreSMRAMCAttribute (
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{

  SMM_PCI_IO_ADDRESS                    Address;
  
  Address.Register = 0x88;
  Address.Function = 0;
  Address.Device   = 0;
  Address.Bus      = 0;
  Address.ExtendedRegister = 0;

  SmmPciCfgWrite (
    SmmPciWidthUint8,
    &Address,
    &SavedSmramReg
    );

  return EFI_SUCCESS;
}

/**

  Thunk to 16-bit Int10 real mode. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument

 @param[in]         This             Protocol instance pointer.
 @param[in, out]  Regs            Register contexted passed into (and returned) from thunk to 16-
                                            bit mode
 
 @retval FALSE    Thunk completed, and there were no BIOS errors in the target code.
                         See Regs for status.                        
 @retval TRUE     There was a BIOS error in the target code.
*/
BOOLEAN
EFIAPI
SmmInt10(
   IN EFI_SMM_THUNK_PROTOCOL    *This,
   IN OUT EFI_IA32_REGISTER_SET *Regs
   )
{

  THUNK_CONTEXT             ThunkContext;
  IA32_REGISTER_SET         Register;
  EFI_STATUS                Status;
  UINTN                     *SaveBuffer;

  //
  // Allocate buffer for saving (0x80000) data
  //
  Status = mSmst->SmmAllocatePool (
             EfiRuntimeServicesData,
             0x1000,
             (VOID **)&SaveBuffer
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
    }

  //
  // Save Data
  //
  CopyMem (SaveBuffer, (VOID*)((UINTN) 0x80000), 0x1000);

  //
  // Prepare environment
  //
  AsmThunk16SetProperties (
    &ThunkContext,
    (VOID*)((UINTN) 0x80000),
    0x1000
    );
  

  //
  // Transfer the EFI_IA32_REGISTER_SET register set to
  // IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
    ConvertRegister (Regs, &Register);
  }

  //
  // Disable A0000-BFFFF SMRAM attribute
  //
  DisableSMRAMCAttribute ();

  DisableAPIC ();

  AsmThunk16Int86 (
    &ThunkContext,
    0x10,
    &Register,
    (UINT32)0x0
    );

  EnableAPIC ();
  //
  // Restore A0000-BFFFF SMRAM attribute
  //
  RestoreSMRAMCAttribute ();

  //
  // Destroy environment
  //
  //AsmThunk16Destroy (&ThunkContext);
  ThunkContext->RealModeBuffer = 0;

  //
  // Restore Data
  //
  CopyMem ((VOID*)((UINTN) 0x80000), SaveBuffer, 0x1000);

 
  //
  // Transfer the IA32_REGISTER_SET register set to
  // EFI_IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
    ConvertRegisterBack (Regs, &Register);
  }
  
  Status = mSmst->SmmFreePool (SaveBuffer);
  
  return TRUE;
}
#endif

/**

  Thunk to 16-bit Int10 real mode. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument
  
 @param[in]         This             Protocol instance pointer.
 @param[in]         IntNumber    Int vector number
 @param[in, out]  Regs            Register contexted passed into (and returned) from thunk to 16-
                                            bit mode
 
 @retval FALSE    Thunk completed, and there were no BIOS errors in the target code.
                         See Regs for status.                         
 @retval TRUE     There was a BIOS error in the target code.
*/
BOOLEAN
EFIAPI
SmmInt86(
   IN EFI_SMM_THUNK_PROTOCOL    *This,
   IN UINT8                     IntNumber,
   IN OUT EFI_IA32_REGISTER_SET *Regs
   )
{
  UINT32             ThunkFlag;
//[-start-140721-IB05580359-remove]//
//  IA32_REGISTER_SET  Register;
//[-end-140721-IB05580359-remove]//

  if (!mPrivateData.RealModeBuffer) {
    //
    // The SmmThunk functionality force off due to out of A/B segment memory
    //
    return FALSE;
  }
  //
  // Transfer the EFI_IA32_REGISTER_SET register set to
  // IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
//[-start-140721-IB05580359-modify]//
    ConvertRegister (Regs, &mRegister);
//[-end-140721-IB05580359-modify]//
  }

  //
  // Set the flag that talk to thunk code
  // this far call will return by iret instruction
  //
  ThunkFlag = THUNK_INTERRUPT;

  DisableAPIC ();
  //
  // Fill the far call address
  //
//[-start-140721-IB05580359-modify]//
  AsmThunk16Int86 (
    &mPrivateData.ThunkContext,
    IntNumber,
    &mRegister,
    ThunkFlag
    );
//[-end-140721-IB05580359-modify]//

  EnableAPIC ();
  //
  // Transfer the IA32_REGISTER_SET register set to
  // EFI_IA32_REGISTER_SET register set
  //
  if (Regs != NULL) {
//[-start-140721-IB05580359-modify]//
    ConvertRegisterBack (Regs, &mRegister);
//[-end-140721-IB05580359-modify]//
  }
  
  return TRUE;
}

//
// Set Complier Optimize off or else the MMIO access code will be optimized from dword access to byte access
// and caused some MMIO registers access error
//
#ifndef __GNUC__
#pragma optimize( "", off )
#endif

/**

  Disable APIC
  
 @param None
 
 @retval None
 
*/
STATIC
VOID
DisableAPIC (
  )
{
  UINT64 ApicBaseReg;
  UINT8  *ApicBase;
  UINTN  Index;
  UINTN  Offset;

  ApicBaseReg = AsmReadMsr64(MSR_IA32_APIC_BASE);
  if (ApicBaseReg & EFI_APIC_GLOBAL_ENABLE) {
    ApicBase = (UINT8*)(UINTN)(ApicBaseReg & 0xffffff000);
    //
    // Backup the whole LVTs due to software disable APIC will set the mask value of LVTs
    //
    for (Index = 0, Offset = APIC_REGISTER_TIMER_VECTOR_OFFSET; Index < 6; Index ++, Offset += 0x10) {
      ApicLVT[Index] = *(UINT32*)(UINTN)(ApicBase + Offset);
    }
    //
    // Backup the TPR
    //
    ApicTPR = *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET);
    //
    // Set the TPR to 0xff to block whole queuing interrupts(in the IRR)
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET) = 0xff;
    //
    // Stall 10ms to waiting for signal stable
    //
    Stall(10 * 1000);
    //
    // Software disable APIC
    //
    if (!SkipSoftEnDisAPIC){
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET) &= ~0x100;
    }
  }
}

/**

  Enable APIC
  
 @param None
 
 @retval None
*/
STATIC
VOID
EnableAPIC (
  )
{
  UINT64 ApicBaseReg;
  UINT8  *ApicBase;
  UINTN  Index;
  UINTN  Offset;

  ApicBaseReg = AsmReadMsr64(MSR_IA32_APIC_BASE);
  if (ApicBaseReg & EFI_APIC_GLOBAL_ENABLE) {
    ApicBase = (UINT8*)(UINTN)(ApicBaseReg & 0xffffff000);
    //
    // Software enable APIC
    //
  if (!SkipSoftEnDisAPIC){
     *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET) |= 0x100;
    }
    //
    // Restore whole LVTs
    //
    for (Index = 0, Offset = APIC_REGISTER_TIMER_VECTOR_OFFSET; Index < 6; Index ++, Offset += 0x10) {
      *(UINT32*)(UINTN)(ApicBase + Offset) = ApicLVT[Index];
    }
    //
    // Restore the TPR
    //
    *(UINT32*)(UINTN)(ApicBase + APIC_REGISTER_TPR_OFFSET) = ApicTPR;
  }
}


EFI_PHYSICAL_ADDRESS
GetBufferfromEBDA(
  IN    UINTN                    LegacyRegionSize
  )
 {
      
  UINTN                             EbdaAddr = 0;
  UINTN                             EbdaAddrNew = 0;
  UINTN                             EbdaSize = 0;
  EFI_PHYSICAL_ADDRESS              RealModeBuffer = 0;

  EbdaAddr = BDA(0x0E) << 4;
  EbdaSize = EBDA(0) << 10;
  
  if ((EbdaAddr + EbdaSize) & 0x03ff) {
    //
    // Alignment :LegacyRegionSize = LegacyRegionSize + (Top EBDA address - Top EBDA address(alignment))
    //
    LegacyRegionSize = LegacyRegionSize + ((EbdaAddr + EbdaSize) - ((EbdaAddr + EbdaSize) & 0xfffff800));         
  }
  
  BDA(0x13) = BDA(0x13) - ((UINT16)(LegacyRegionSize >> 10));
  BDA(0x0E) = BDA(0x0E) - ((UINT16)(LegacyRegionSize >> 4));
  
  EbdaAddrNew = BDA(0x0E) << 4;
  
  CopyMem ((VOID *) (UINTN) EbdaAddrNew,(VOID *) (UINTN) EbdaAddr,EbdaSize);
  
  // 
  // Get Realmode buffer from EBDA
  //
  RealModeBuffer = (EFI_PHYSICAL_ADDRESS)(EbdaAddrNew + EbdaSize);
 
  return RealModeBuffer;
}
