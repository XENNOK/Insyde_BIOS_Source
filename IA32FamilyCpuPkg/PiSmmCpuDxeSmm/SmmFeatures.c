/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  The CPU specific programming for PiSmmCpuDxeSmm module, such as SMRR, EMRR, IED.
  Currently below CPUs are supported.

  0x00000F00 // Pentium4

  0x000006F0 // Conroe
  0x00010660 // Conroe-L
  0x00010670 // Wolfdale
  0x000106D0 // Dunnington
  0x000106C0 // Silverthorne
  0x00020660 // Tunnel Creek

  0x000106A0 // Nehalem
  0x000106E0 // Lynnfield
  0x000106F0 // Havendale
  0x000206E0 // Nehalem-EX
  0x00020650 // Clarkdale
  0x000206C0 // Westmere
  0x000206F0 // Westmere-EX
  0x000206A0 // SandyBridge
  0x000206D0 // SandyBridge-EP
  0x000306A0 // Ivybridge Client
  0x000306E0 // Ivybridge Server

  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Base.h>

#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/LocalApicLib.h>

#include "PiSmmCpuDxeSmm.h"
#include "SmmFeatures.h"

//
// The CPUID mapping for Pentium4
//
CPUID_MAPPING  mPentium4Map[] = {
  {CPUID_SIGNATURE_PENTIUM, CPUID_MASK_NO_STEPPING_MODEL},      // Pentium
  };

//
// The CLASS for Pentium4
//
CPU_SMM_CLASS mPentium4Class = {
  CpuPentium4,
  sizeof(mPentium4Map)/sizeof(mPentium4Map[0]),
  mPentium4Map,
  };

//
// The CPUID mapping for Core2
//
CPUID_MAPPING  mCore2Map[] = {
  {CPUID_SIGNATURE_CONROE, CPUID_MASK_NO_STEPPING},             // Conroe
  {CPUID_SIGNATURE_CONROE_L, CPUID_MASK_NO_STEPPING},           // Conroe-L
  {CPUID_SIGNATURE_WOLFDALE, CPUID_MASK_NO_STEPPING},           // Wolfdale
  {CPUID_SIGNATURE_DUNNINGTON, CPUID_MASK_NO_STEPPING},         // Dunnington
  {CPUID_SIGNATURE_SILVERTHORNE, CPUID_MASK_NO_STEPPING},       // Silverthorne
  {CPUID_SIGNATURE_TUNNELCREEK, CPUID_MASK_NO_STEPPING},        // Tunnel Creek
  };

//
// The CPUID mapping for Atom
//
CPUID_MAPPING  mAtomMap[] = {
  {CPUID_SIGNATURE_SILVERTHORNE, CPUID_MASK_NO_STEPPING},       // Silverthorne
  {CPUID_SIGNATURE_TUNNELCREEK, CPUID_MASK_NO_STEPPING},        // Tunnel Creek
  };

//
// The CLASS for Core2
//
CPU_SMM_CLASS mCore2Class = {
  CpuCore2,
  sizeof(mCore2Map)/sizeof(mCore2Map[0]),
  mCore2Map,
  };

//
// The CPUID mapping for Nehalem
//
CPUID_MAPPING  mNehalemMap[] = {
  {CPUID_SIGNATURE_NEHALEM, CPUID_MASK_NO_STEPPING},            // Nehalem
  {CPUID_SIGNATURE_LYNNFIELD, CPUID_MASK_NO_STEPPING},          // Lynnfield
  {CPUID_SIGNATURE_HAVENDALE, CPUID_MASK_NO_STEPPING},          // Havendale
  {CPUID_SIGNATURE_NEHALEM_EX, CPUID_MASK_NO_STEPPING},         // Nehalem-EX
  {CPUID_SIGNATURE_CLARKDALE, CPUID_MASK_NO_STEPPING},          // Clarkdale
  {CPUID_SIGNATURE_WESTMERE, CPUID_MASK_NO_STEPPING},           // Westmere
  {CPUID_SIGNATURE_WESTMERE_EX, CPUID_MASK_NO_STEPPING},        // Westmere-EX
  {CPUID_SIGNATURE_SANDYBRIDGE, CPUID_MASK_NO_STEPPING},        // SandyBridge
  {CPUID_SIGNATURE_SANDYBRIDGE_EP, CPUID_MASK_NO_STEPPING},     // SandyBridge-EP
  {CPUID_SIGNATURE_IVYBRIDGE_CLIENT, CPUID_MASK_NO_STEPPING},   // Ivybridge Client
  {CPUID_SIGNATURE_IVYBRIDGE_SERVER, CPUID_MASK_NO_STEPPING},   // Ivybridge Server
  {CPUID_SIGNATURE_HASWELL, CPUID_MASK_NO_STEPPING},  
  {CPUID_SIGNATURE_HASWELL_ULT, CPUID_MASK_NO_STEPPING},
  {CPUID_SIGNATURE_CRYSTALLWELL, CPUID_MASK_NO_STEPPING},
  {CPUID_SIGNATURE_BROADWELL_ULT, CPUID_MASK_NO_STEPPING},
  {CPUID_SIGNATURE_BRYSTALWELL, CPUID_MASK_NO_STEPPING},
  };

//
// The CPUID mapping for NehalemEx
//
CPUID_MAPPING  mNehalemExMap[] = {
  {CPUID_SIGNATURE_NEHALEM_EX, CPUID_MASK_NO_STEPPING},         // Nehalem-EX
  {CPUID_SIGNATURE_WESTMERE_EX, CPUID_MASK_NO_STEPPING},        // Westmere-EX
  };

//
// The CLASS for Nehalem
//
CPU_SMM_CLASS mNehalemClass = {
  CpuNehalem,
  sizeof(mNehalemMap)/sizeof(mNehalemMap[0]),
  mNehalemMap,
  };

//
// This table defines supported CPU class
//
CPU_SMM_CLASS *mCpuClasstable[] = {
  &mPentium4Class,
  &mCore2Class,
  &mNehalemClass,
  };

////////
// Below section is common definition
////////

CPU_SMM_FEATURE_CONTEXT  mFeatureContext;
CPU_SMM_CLASS            *mThisCpu;

/**
  Return if SMRR is supported

  @retval TRUE  SMRR is supported.
  @retval FALSE SMRR is not supported.

**/
BOOLEAN
IsSmrrSupported (
  VOID
  )
{
  UINT64                            MtrrCap;

  MtrrCap = AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP);
  if ((MtrrCap & IA32_MTRR_SMRR_SUPPORT_BIT) == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Return if EMRR is supported

  @retval TRUE  EMRR is supported.
  @retval FALSE EMRR is not supported.

**/
BOOLEAN
IsEmrrSupported (
  VOID
  )
{
  UINT64                            MtrrCap;

  MtrrCap = AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP);
  if ((MtrrCap & IA32_MTRR_EMRR_SUPPORT_BIT) == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

////////
// Below section is definition for CpuPentium4
////////

/**
  Programming IED.

  @param  IedBase            The base address of IED. -1 when IED is disabled.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
Pentium4RelocateIedBase (
  IN UINT32                IedBase,
  IN BOOLEAN               IsBsp
  )
{
  UINT32 IedSize;
  UINT32 StrSize;
  UINT32 *IedOffset;

  IedSize = PcdGet32 (PcdCpuIEDRamSize);
  IedOffset = (UINT32 *)(UINTN)(SMM_DEFAULT_SMBASE + SMM_PENTIUM4_IEDBASE_OFFSET);
  if (*IedOffset != SMM_PENTIUM4_DEFAULT_IEDBASE) {
    return ;
  }
  *IedOffset = IedBase;

  if ((IedBase != (UINT32)-1) && IsBsp) {
    // 48 byte header
    ZeroMem ((UINT8 *)(UINTN)IedBase, 48);
    // signature
    StrSize = sizeof(IED_STRING) - 1;
    CopyMem ((UINT8 *)(UINTN)IedBase, IED_STRING, StrSize);
    // IED size
    *(UINT32 *)(UINTN)(IedBase + 10) = IedSize;
  }

  return ;
}

////////
// Below section is definition for CpuCore2
////////

/**
  Return if SMRR is enabled

  @retval TRUE  SMRR is enabled.
  @retval FALSE SMRR is not enabled.

**/
BOOLEAN
Core2IsSmrrEnabled (
  VOID
  )
{
  UINT64                            SmrrMtrr;

  SmrrMtrr = AsmReadMsr64(EFI_MSR_IA32_FEATURE_CONTROL);
  if ((SmrrMtrr & IA32_SMRR_ENABLE_BIT) != 0)	{
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Initialize SMRR in SMM relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
**/
VOID
Core2InitSmrr (
  IN UINT32                SmrrBase,
  IN UINT32                SmrrSize
  )
{
  AsmWriteMsr64 (EFI_MSR_CORE2_SMRR_PHYS_BASE, SmrrBase);
  AsmWriteMsr64 (EFI_MSR_CORE2_SMRR_PHYS_MASK, (~(SmrrSize - 1) & EFI_MSR_SMRR_MASK) | EFI_MSR_SMRR_PHYS_MASK_VALID);
}

/**
  Disable SMRR register when SmmInit replace non-SMM MTRRs.
**/
VOID
Core2DisableSmrr (
  VOID
  )
{
  AsmWriteMsr64(EFI_MSR_CORE2_SMRR_PHYS_MASK, AsmReadMsr64(EFI_MSR_CORE2_SMRR_PHYS_MASK) & ~EFI_MSR_SMRR_PHYS_MASK_VALID);
}

/**
  Enable SMRR register when SmmInit restores non-SMM MTRRs.
**/
VOID
Core2EnableSmrr (
  VOID
  )
{
  AsmWriteMsr64(EFI_MSR_CORE2_SMRR_PHYS_MASK, AsmReadMsr64(EFI_MSR_CORE2_SMRR_PHYS_MASK) | EFI_MSR_SMRR_PHYS_MASK_VALID);
}

/**
  Check if it is Atom processor.

  @retval TRUE  It is Atom.
  @retval FALSE It is not Atom.
**/
BOOLEAN
IsAtom (
  VOID
  )
{
  UINT32 RegEax;
  UINTN  Index;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  for (Index = 0; Index < sizeof(mAtomMap)/sizeof(mAtomMap[0]); Index++) {
    if ((RegEax & mAtomMap[Index].Mask) == (mAtomMap[Index].Signature & mAtomMap[Index].Mask)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Programming IED.

  @param  IedBase            The base address of IED. -1 when IED is disabled.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
Core2RelocateIedBase (
  IN UINT32                IedBase,
  IN BOOLEAN               IsBsp
  )
{
  UINT32 IedSize;
  UINT32 StrSize;
  UINT32 *IedOffset;

  if (IsAtom ()) {
    // No IED for Atom
    return ;
  }

  IedSize = PcdGet32 (PcdCpuIEDRamSize);
  IedOffset = (UINT32 *)(UINTN)(SMM_DEFAULT_SMBASE + SMM_CORE2_IEDBASE_OFFSET);
  if (*IedOffset != SMM_CORE2_DEFAULT_IEDBASE) {
    return ;
  }
  *IedOffset = IedBase;

  if ((IedBase != (UINT32)-1) && IsBsp) {
    // 48 byte header
    ZeroMem ((UINT8 *)(UINTN)IedBase, 48);
    // signature
    StrSize = sizeof(IED_STRING) - 1;
    CopyMem ((UINT8 *)(UINTN)IedBase, IED_STRING, StrSize);
    // IED size
    *(UINT32 *)(UINTN)(IedBase + 10) = IedSize;
  }

  return ;
}

////////
// Below section is definition for CpuNehalem
////////

/**
  Initialize EMRR in SMM relocate.

  @param  IedBase           IEDRAM base address.
  
**/
VOID
NehalemInitEmrr (
  IN UINT32          IedBase
  )
{
  UINT32                ApicId;
  UINT32                IntraPackageIdBits;
  UINT64                McSegBase;
  UINT64                McSegMask;
  UINT8                 PhysicalAddressBits;
  UINT64                ValidMtrrAddressMask;
  UINT32                RegEax;
  UINTN                 PciExBarSize;
  UINTN                 BusNumber;

  //
  // Note that Nehalem-EX and Westmere-EX does not support EMRR.
  //
  ApicId = GetInitialApicId ();

  AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, 1, &IntraPackageIdBits, NULL, NULL, NULL);
  IntraPackageIdBits &= 0x1f;
  
  AsmCpuid (EFI_CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
  PhysicalAddressBits = 36;
  if (RegEax >= EFI_CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (EFI_CPUID_VIR_PHY_ADDRESS_SIZE, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  }
  ValidMtrrAddressMask = (LShiftU64 (1, PhysicalAddressBits) - 1) & 0xfffffffffffff000ull;

  if ((ApicId >> IntraPackageIdBits) != 0) {
    //
    // IEDBASE + 3MB for socket 1
    //
    McSegBase = IedBase + 0x300000;
  } else {
    //
    // IEDBASE + 2MB for socket 0
    //
    McSegBase = IedBase + 0x200000;
  }
  McSegBase &= ValidMtrrAddressMask; 

  McSegMask = (UINT64)(~(UINT64)(0x200000 - 1)); 
  McSegMask &= ValidMtrrAddressMask; 

  //
  // SAD_MCSEG_BASE and SAD_MCSEG_MASK programming described in NHM BWG 0.70.
  // SAD_MCSEG_BASE and SAD_MCSEG_MASK programming is NOT specified in SNB and IVB BWG.
  // 0x206a0: SandyBridge
  // 0x206d0: SandyBridge-EP
  // 0x306A0: Ivybridge Client
  // 0x306E0: Ivybridge Server
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  if (((RegEax & 0x0FFF0FF0) != 0x000206A0) && 
      ((RegEax & 0x0FFF0FF0) != 0x000206D0) &&
      ((RegEax & 0x0FFF0FF0) != 0x000306A0) &&
      ((RegEax & 0x0FFF0FF0) != 0x000306E0) &&   
      ((RegEax & 0x0FFF0FF0) != CPUID_SIGNATURE_HASWELL_ULT) &&       
      ((RegEax & 0x0FFF0FF0) != CPUID_SIGNATURE_CRYSTALLWELL) &&  
      ((RegEax & 0x0FFF0FF0) != CPUID_SIGNATURE_BROADWELL_ULT) &&       
      ((RegEax & 0x0FFF0FF0) != CPUID_SIGNATURE_BRYSTALWELL) &&       
      ((RegEax & 0x0FFF0FF0) != CPUID_SIGNATURE_HASWELL)){  

    //
    // Determine PCI bus number of this processor
    //

    PciExBarSize = (((UINTN)AsmReadMsr64 (EFI_MSR_NEHALEM_PCIEXBAR)) & (BIT3 | BIT2 | BIT1)) >> 1;

    switch (PciExBarSize) {
      default:
      case 0x00:
        BusNumber = 0xFF;
        break;
      case 0x07:
        BusNumber = 0x7F;
        break;
      case 0x06:
        BusNumber = 0x3F;
        break;
    }

    BusNumber -= (ApicId >> IntraPackageIdBits);

    if (!(PciRead32 (NEHALEM_SAD_MCSEG_MASK (BusNumber)) & NEHALEM_SAD_MCSEG_MASK_LOCK)) {
      PciWrite32 (NEHALEM_SAD_MCSEG_BASE (BusNumber), (UINT32)McSegBase);
      PciWrite32 (NEHALEM_SAD_MCSEG_BASE (BusNumber) + 4, (UINT32)RShiftU64 (McSegBase, 32));

      PciWrite32 (NEHALEM_SAD_MCSEG_MASK (BusNumber) + 4, (UINT32)RShiftU64 (McSegMask, 32));    
      PciWrite32 (
        NEHALEM_SAD_MCSEG_MASK (BusNumber),
        ((UINT32)McSegMask) | NEHALEM_SAD_MCSEG_MASK_LOCK | NEHALEM_SAD_MCSEG_MASK_ENABLE
        );
    }
  }

  if (!(AsmReadMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_MASK) & EFI_MSR_EMRR_PHYS_MASK_LOCK)) {
    AsmWriteMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_BASE, McSegBase | CACHE_WRITE_BACK);
    AsmWriteMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_MASK, McSegMask | EFI_MSR_EMRR_PHYS_MASK_LOCK);
  }
}

/**
  Initialize SMRR in SMM relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
**/
VOID
NehalemInitSmrr (
  IN UINT32                SmrrBase,
  IN UINT32                SmrrSize
  )
{
  AsmWriteMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_BASE, SmrrBase | CACHE_WRITE_BACK);
  AsmWriteMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_MASK, (~(SmrrSize - 1) & EFI_MSR_SMRR_MASK)); // Valid bit will be set in ConfigSmrr() at first SMI
}

/**
  Configure SMRR register at each SMM entry.
**/
VOID
NehalemConfigSmrr (
  VOID
  )
{
  UINT64 SmrrMask;

  SmrrMask = AsmReadMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_MASK);
  if ((SmrrMask & EFI_MSR_SMRR_PHYS_MASK_VALID) == 0) {
    AsmWriteMsr64(EFI_MSR_NEHALEM_SMRR_PHYS_MASK, SmrrMask | EFI_MSR_SMRR_PHYS_MASK_VALID);
  }
}

/**
  Check if it is Nehalem-EX processor.

  @retval TRUE  It is Nehalem-EX.
  @retval FALSE It is not Nehalem-EX.
**/
BOOLEAN
IsNehalemEx (
  VOID
  )
{
  UINT32 RegEax;
  UINTN  Index;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  for (Index = 0; Index < sizeof(mNehalemExMap)/sizeof(mNehalemExMap[0]); Index++) {
    if ((RegEax & mNehalemExMap[Index].Mask) == (mNehalemExMap[Index].Signature & mNehalemExMap[Index].Mask)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Programming IED.

  @param  IedBase            The base address IED. -1 when IED is disabled.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
NehalemRelocateIedBase (
  IN UINT32                IedBase,
  IN BOOLEAN               IsBsp
  )
{
  UINT32 IedSize;
  UINT32 StrSize;
  UINT32 *IedOffset;
  UINT32 ScratchPad8;

  IedSize = PcdGet32 (PcdCpuIEDRamSize);
  //
  // A minimum of 4MB IDERAM is required for Nehalem processors.
  //
  ASSERT (IedSize >= 0x400000);

  IedOffset = (UINT32 *)(UINTN)(SMM_DEFAULT_SMBASE + SMM_NEHALEM_IEDBASE_OFFSET);
  *IedOffset = IedBase;

  if ((IedBase != (UINT32)-1) && IsBsp) {
    // 48 byte header
    ZeroMem ((UINT8 *)(UINTN)IedBase, 48);
    // signature
    StrSize = sizeof(IED_STRING) - 1;
    CopyMem ((UINT8 *)(UINTN)IedBase, IED_STRING, StrSize);
    // IED size
    *(UINT32 *)(UINTN)(IedBase + 10) = IedSize;

    // Set IED trace
    if (IsNehalemEx ()) {
      ScratchPad8 = *(UINT32 *)(UINTN)NEHALEM_EX_UU_CR_U_PCSR_FW_SCRATCH_8;
      if (ScratchPad8 == 0) {
        *(UINT64 *)(UINTN)(IedBase + 16) = (UINT64)-1;
      } else {
        // IED Trace Ptr = UU_CR_U_PCSR_FW_SCRATCH_8 * 1MB
        *(UINT64 *)(UINTN)(IedBase + 16) = LShiftU64 (ScratchPad8, 20);
      }
    }

    // patch
    ZeroMem ((UINT8 *)(UINTN)IedBase + 0x100000, 32 * 0x1024);
  }
}

/**
  Enable the Save Floating Point Pointers feature on every logical processor.
  Refer to SNB/IVB BWG "SMM Handler Considerations".

**/
VOID
EnableSmmSaveControl (
  VOID
)
{
  //
  // MSR_PLATFORM_INFO[16] = 1 indicates the Save Floating Point Pointers feature exists.
  //
  if ((AsmReadMsr64 (MSR_PLATFORM_INFO) & PLATFORM_INFO_SMM_SAVE_CONTROL) != 0) {
    AsmMsrOr64 (EFI_MSR_SMM_SAVE_CONTROL, SAVE_FLOATING_POINT_POINTERS);
  }
}

////////
// Below section is definition for the supported class
////////

/**
  This function will return current CPU_SMM_CLASS accroding to CPUID mapping.

  @return The point to current CPU_SMM_CLASS

**/
CPU_SMM_CLASS *
GetCpuFamily (
  VOID
  )
{
  UINT32         ClassIndex;
  UINT32         Index;
  UINT32         Count;
  CPUID_MAPPING  *CpuMapping;
  UINT32         RegEax;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  for (ClassIndex = 0; ClassIndex < sizeof(mCpuClasstable)/sizeof(mCpuClasstable[0]); ClassIndex++) {
    CpuMapping = mCpuClasstable[ClassIndex]->MappingTable;
    Count = mCpuClasstable[ClassIndex]->MappingCount;
    for (Index = 0; Index < Count; Index++) {
      if ((CpuMapping[Index].Signature & CpuMapping[Index].Mask) == (RegEax & CpuMapping[Index].Mask)) {
        return mCpuClasstable[ClassIndex];
      }
    }
  }

  // Not found!!! Should not happen
  ASSERT (FALSE);
  return NULL;
}

////////
// Below section is external function
////////

/**
  Disable SMRR register when SmmInit set SMM MTRRs.
**/
VOID
DisableSmrr (
  VOID
  )
{
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuCore2:
    if (mFeatureContext.SmrrEnabled) {
      Core2DisableSmrr ();
    }
    return ;
  case CpuPentium4:
  case CpuNehalem:
  default:
    return ;
  }
}

/**
  Enable SMRR register when SmmInit restore non SMM MTRRs.
**/
VOID
ReenableSmrr (
  VOID
  )
{
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuCore2:
    if (mFeatureContext.SmrrEnabled) {
      Core2EnableSmrr ();
    }
    return ;
  case CpuPentium4:
  case CpuNehalem:
  default:
    return ;
  }
}

/**
  Return if it is needed to configure MTRR to set TSEG cacheability.

  @retval  TRUE  - we need configure MTRR
  @retval  FALSE - we do not need configure MTRR
**/
BOOLEAN
NeedConfigureMtrrs (
  VOID
  )
{
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuNehalem:
    return FALSE;
  case CpuPentium4:
  case CpuCore2:
  default:
    return TRUE;
  }
}

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.

**/
VOID
SmmRendezvousEntry (
  IN UINTN  CpuIndex
  )
{
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuNehalem:
    //
    // Configure SMRR/EMRR register at each SMM entry.
    //
    if (mFeatureContext.SmrrEnabled) {
      NehalemConfigSmrr ();
    }
    //
    // T-state fix
    // NHM & SNB BWG: T-state throttling should be disabled while executing the SMM handler.
    //
    gSmmCpuPrivate->TstateMsr[CpuIndex] = AsmReadMsr64 (EFI_MSR_IA32_CLOCK_MODULATION);
    AsmWriteMsr64 (EFI_MSR_IA32_CLOCK_MODULATION, 0);
    return ;
  case CpuPentium4:
  case CpuCore2:
  default:
    return ;
  }
}

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousExit (
  IN UINTN  CpuIndex
  )
{
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuNehalem:
    //
    // T-state fix
    // NHM & SNB BWG: T-state throttling should be disabled while executing the SMM handler.
    //
    AsmWriteMsr64 (EFI_MSR_IA32_CLOCK_MODULATION, gSmmCpuPrivate->TstateMsr[CpuIndex]);
    return ;
  case CpuPentium4:
  case CpuCore2:
  default:
    return ;
  }
}

/**
  Initialize SMRR context in SMM Init.
**/
VOID
InitializeSmmMtrrManager (
  VOID
  )
{
  mThisCpu = GetCpuFamily ();
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuCore2:
    if (!IsSmrrSupported ()) {
      return ;
    }
    mFeatureContext.SmrrEnabled = Core2IsSmrrEnabled ();
    return ;
  case CpuNehalem:
    if (!IsSmrrSupported ()) {
      return ;
    }
    mFeatureContext.SmrrEnabled = TRUE;
    mFeatureContext.EmrrSupported = IsEmrrSupported ();
    return ;
  case CpuPentium4:
  default:
    return ;
  }
}

/**
  Initialize SMRR/EMRR/IED register in SMM Relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
  @param  IEDBase            The base address IED.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
SmmInitiFeatures (
  IN UINT32  SmrrBase,
  IN UINT32  SmrrSize,
  IN UINT32  IEDBase,
  IN BOOLEAN IsBsp
  )
{
  mThisCpu = GetCpuFamily ();
  ASSERT (mThisCpu != NULL);

  switch (mThisCpu->Family) {
  case CpuPentium4:
    Pentium4RelocateIedBase (IEDBase, IsBsp);
    return ;
  case CpuCore2:
    Core2RelocateIedBase (IEDBase, IsBsp);
    if (!IsSmrrSupported ()) {
      return ;
    }
    if (!Core2IsSmrrEnabled ()) {
      return ;
    }
    Core2InitSmrr (SmrrBase, SmrrSize);
    return ;
  case CpuNehalem:
    NehalemRelocateIedBase (IEDBase, IsBsp);
    if (!IsSmrrSupported ()) {
      return ;
    }
    NehalemInitSmrr (SmrrBase, SmrrSize);
    if (IsEmrrSupported ()) {
      NehalemInitEmrr (IEDBase);
    }
    EnableSmmSaveControl ();
    return ;
  default:
    return ;
  }
}
