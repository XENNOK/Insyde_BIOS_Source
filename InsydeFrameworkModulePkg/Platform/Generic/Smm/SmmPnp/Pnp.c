//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "Pnp.h"
#include "PnpDmi.h"
#include "SmmPnp.h"

EFI_PHYSICAL_ADDRESS       gSMBIOSTableEntryAddress = 0;

PNP_REDIRECTED_ENTRY_POINT mPnpDmiFunctions[] = {
  Pnp0x50,
  Pnp0x51,
  Pnp0x52,
  UnsupportedPnpFunction, //Pnp0x53,
  Pnp0x54,
  Pnp0x55,
  Pnp0x56,
  Pnp0x57
};
EFI_SMM_CPU_SAVE_STATE            *SmmCpuSaveLocalState = NULL;

VOID
PnpRedirectedEntryPoint (
  IN OUT EFI_DWORD_REGS                 *RegBufferPtr
  )
/*++

Routine Description:

  PnP support.

Arguments:

  RegBufferPtr - Pointer to input register values.


Returns:

  None

--*/
{
  PNP_GENERIC_ENTRY_FRAME               *Frame;
  INT16                                 ReturnStatus;
  UINT32                                CR3Value;
  UINT32                                CR4Value;

  CR3Value = (UINT32)SmmCpuSaveLocalState->EBX;
  CR4Value = (UINT32)SmmCpuSaveLocalState->ECX;

  Frame = (PNP_GENERIC_ENTRY_FRAME*)(UINTN)LogicalToPhysicalAddress((UINT32)RegBufferPtr->ESP,
                                                                     CR3Value,
                                                                     CR4Value,
                                                                     (UINTN)RegBufferPtr->SS);
  if ((Frame->Function & ~0x7) == 0x50) {
    ReturnStatus = mPnpDmiFunctions[Frame->Function & 0x7](Frame);
  } else {
    ReturnStatus = PNP_FUNCTION_NOT_SUPPORTED;
  }

  *((INT16*)&(RegBufferPtr->EAX)) = ReturnStatus;
  return;
}

INT16
UnsupportedPnpFunction (
  PNP_GENERIC_ENTRY_FRAME               *Frame
  )
/*++

Routine Description:

  default function returning that the call was to an unsupported function.

Arguments:

  Frame - unused

Returns:

  None

--*/
{
  return PNP_FUNCTION_NOT_SUPPORTED;
}

VOID *
PnpFarToLinear (
  IN PNP_FAR_PTR                        Ptr
  )
/*++

Routine Description:

  Does a quick conversion from a 16-bit C far pointer to a linear
  address.  Unfortunately, this can only support far pointers from
  16-bit real mode.

Arguments:

  far - 16-bit far pointer

Returns:

  None

--*/
{
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINTN                                 SegementSelector;
  //
  //  Call by new way that AP trigger software SMI directly when the signature is "$ISB'
  //
  if ((UINT32)SmmCpuSaveLocalState->EBX == EFI_SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    return (VOID *)(UINTN)(UINT32)(Ptr.Offset | Ptr.Segment << 16);    
  }  

  CR3Value = (UINT32)SmmCpuSaveLocalState->EBX;
  CR4Value = (UINT32)SmmCpuSaveLocalState->ECX;
  SegementSelector = (UINTN)Ptr.Segment;
  return (VOID*)(UINTN)LogicalToPhysicalAddress ((UINT32)Ptr.Offset, CR3Value, CR4Value, SegementSelector);
}

VOID
PnPBiosCallback (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT       *DispatchContext
  )
{
  EFI_DWORD_REGS                        *RegBufferPtr;
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINTN                                 SegementSelector;
  UINTN                                 Index;
  PNP_GENERIC_ENTRY_FRAME               *Frame;
  INT16                                 ReturnStatus;
  VOID                                  *TempAddress;

  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    //
    // Find out which CPU triggered PnP SMI
    //
    SmmCpuSaveLocalState = (EFI_SMM_CPU_SAVE_STATE *)(&mSmst->CpuSaveState[Index]);
    if ((SmmCpuSaveLocalState->EAX & 0xff) == SMM_PnP_BIOS_CALL &&
        (SmmCpuSaveLocalState->EDX & 0xffff) == SW_SMI_PORT) {
      //
      // Cpu found!
      //
      break;
    }
  }
  if (Index == mSmst->NumberOfCpus) {
    //
    // Error out due to CPU not found
    //
    return;
  }

  if (gSMBIOSTableEntryAddress == 0) {
    EfiLibGetSystemConfigurationTable (&gEfiSmbiosTableGuid, &TempAddress);
    gSMBIOSTableEntryAddress = (EFI_PHYSICAL_ADDRESS)TempAddress;
    return;
  }
  
  if ((UINT32)SmmCpuSaveLocalState->EBX == EFI_SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    //
    // Call by new way that AP trigger software SMI directly when the signature is "$ISB'
    //
    Frame = (PNP_GENERIC_ENTRY_FRAME*)(UINTN)SmmCpuSaveLocalState->ESI;
    
    if ((Frame->Function & ~0x7) == 0x50) {
      ReturnStatus = mPnpDmiFunctions[Frame->Function & 0x7](Frame);
    } else {
      ReturnStatus = PNP_FUNCTION_NOT_SUPPORTED;
    }
  
    SmmCpuSaveLocalState->EAX = ReturnStatus; 

  } else {
    //
    // Call by csm16
    //
    CR3Value = (UINT32)SmmCpuSaveLocalState->EBX;
    CR4Value = (UINT32)SmmCpuSaveLocalState->ECX;
    SegementSelector = (UINTN)SmmCpuSaveLocalState->SS;
    RegBufferPtr = (EFI_DWORD_REGS*)(UINTN)LogicalToPhysicalAddress((UINT32)SmmCpuSaveLocalState->ESI,
                                                                     CR3Value,
                                                                     CR4Value,
                                                                     SegementSelector);
    PnpRedirectedEntryPoint (RegBufferPtr);
  }  
}

UINT32
LogicalToPhysicalAddress (
  IN UINT32  LinearAddress,
  IN UINT32 CR3Value,
  IN UINT32 CR4Value,
  IN UINTN  SegementSelector
  )
{
  UINT32  *TmpPtr;
  UINT32  Buffer32;

  if ((CR3Value & 0x00000001) == 0) {

    return ((UINT32)SegementSelector << 4 ) + LinearAddress;
  }

  //
  // 32bit Mode SMI: transfer to physical address
  //  IA-32 Intel Architecture Software Developer's Manual
  //  Volume 3 - Chapter 3.7
  // there is 2 necessay condition:
  //  1.Base address in Segement Selector(GDT) must be 0
  //      if not 0, the Linear address need add the base address first
  //      Check Chapter 3.4 "Logical and Linear Addresses"
  if (SegementSelector != 0x10) {
    //
    // Base Address in SegementSelector 0x10 is set to 0 in SEC Phase.
    // Other will need to be read GDT...not impelement yet.Just return.
    //
    LinearAddress+= (UINT32)SmmCpuSaveLocalState->EDI;
  }

  //
  //  2.Only support 4MByte Pages now for XP-SP2
  //      4KByte pages isn't verified.
  //      if need support 4KByte Pages for Win2K...etc,
  //      check Chapter 3.7.1
  //

  // Use CR4 Bit5 to check 2MBytes Page for XD enable

  if (CR4Value & 0x20) {
    Buffer32 = CR3Value & 0xFFFFFFE0;

    //
    // Page Directory Pointer Table Entry
    //
    TmpPtr = (UINT32*)(UINTN)(Buffer32 + ((LinearAddress & 0xC0000000)>>27));
    Buffer32 = *TmpPtr;

    //
    // 2MByte Page - Page Directory Entry
    //
    TmpPtr = (UINT32*)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x3FE00000)>>18));
    Buffer32 = *TmpPtr;

    if ((Buffer32 & 0x80) == 0) {
      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32*)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x001FF000)>>9));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 2MByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFE00000) + (LinearAddress & 0x001FFFFF);
    }
  } else {

    //
    // Get Page-Directory from CR3
    //
    Buffer32 = CR3Value & 0xFFFFF000;

    //
    // Page Directory Entry
    //
    TmpPtr = (UINT32*)(UINTN)(Buffer32 + ((LinearAddress & 0xFFC00000)>>20));
    Buffer32 = *TmpPtr;

    //
    // Check 4KByte/4MByte Pages
    //
    if ((Buffer32 & 0x80) == 0) {

      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32*)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x003FF000)>>10));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 4MByte Pages - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFC00000) + (LinearAddress & 0x003FFFFF);
    }
  }

  return Buffer32;
}
