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
//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  The CPU specific programming for PiSmmCpuDxeSmm module.

  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef __SMM_FEATURES_H__
#define __SMM_FEATURES_H__

////////
// Below definition is from IA32 SDM
////////
#define EFI_CPUID_VERSION_INFO                 0x1
#define EFI_CPUID_CORE_TOPOLOGY                0x0B
#define EFI_CPUID_EXTENDED_FUNCTION            0x80000000
#define EFI_CPUID_VIR_PHY_ADDRESS_SIZE         0x80000008

#define EFI_MSR_IA32_MTRR_CAP                  0xFE
#define  IA32_MTRR_SMRR_SUPPORT_BIT            BIT11
#define  IA32_MTRR_EMRR_SUPPORT_BIT            BIT12

#define EFI_MSR_IA32_FEATURE_CONTROL           0x3A
#define  IA32_SMRR_ENABLE_BIT                  BIT3

#define  EFI_MSR_SMRR_PHYS_MASK_VALID          BIT11
#define  EFI_MSR_SMRR_MASK                     0xFFFFF000
#define  EFI_MSR_EMRR_PHYS_MASK_LOCK           BIT10
#define  EFI_MSR_EMRR_MASK                     0xFFFFF000

#define EFI_MSR_CORE2_SMRR_PHYS_BASE           0xA0
#define EFI_MSR_CORE2_SMRR_PHYS_MASK           0xA1

#define EFI_MSR_NEHALEM_SMRR_PHYS_BASE         0x1F2
#define EFI_MSR_NEHALEM_SMRR_PHYS_MASK         0x1F3

#define CACHE_WRITE_BACK                       6
#define SMM_DEFAULT_SMBASE                     0x30000

////////
// Below definition is from CPU BWG
////////
#define EFI_MSR_NEHALEM_EMRR_PHYS_BASE         0x1F4
#define EFI_MSR_NEHALEM_EMRR_PHYS_MASK         0x1F5

#define EFI_MSR_NEHALEM_PCIEXBAR               0x300

#define EFI_MSR_SMM_SAVE_CONTROL               0x3e
#define SAVE_FLOATING_POINT_POINTERS           BIT0

#define PLATFORM_INFO_SMM_SAVE_CONTROL         BIT16

#define SMM_PENTIUM4_IEDBASE_OFFSET            0xFF04
#define SMM_CORE2_IEDBASE_OFFSET               0xFEEC
#define SMM_NEHALEM_IEDBASE_OFFSET             0xFEEC
#define SMM_PENTIUM4_DEFAULT_IEDBASE           0x30000
#define SMM_CORE2_DEFAULT_IEDBASE              0x30000
#define SMM_NEHALEM_DEFAULT_IEDBASE            0x50000
#define IED_STRING                             "INTEL RSVD"

#define NEHALEM_EX_UU_CR_U_PCSR_FW_SCRATCH_8   0xFEB204A0

#define NEHALEM_SAD_MCSEG_BASE(Bus)            PCI_LIB_ADDRESS (Bus, 0, 1, 0x60)
#define NEHALEM_SAD_MCSEG_MASK(Bus)            PCI_LIB_ADDRESS (Bus, 0, 1, 0x68)

#define NEHALEM_SAD_MCSEG_MASK_LOCK            BIT10
#define NEHALEM_SAD_MCSEG_MASK_ENABLE          BIT11

////////
// Below section is definition for CPU SMM Feature context
////////

//
// Structure to describe CPU identification mapping
// if ((CPUID_EAX(1) & Mask) == (Signature & Mask)), it means matched.
//
typedef struct {
  UINT32  Signature;
  UINT32  Mask;
} CPUID_MAPPING;

//
// CPU SMM familiy
//
typedef enum {
  CpuPentium4,
  CpuCore2,
  CpuNehalem,
  CpuSmmFamilyMax
} CPU_SMM_FAMILY;

//
// Structure to describe CPU SMM class
//
typedef struct {
  CPU_SMM_FAMILY    Family;
  UINT32            MappingCount;
  CPUID_MAPPING     *MappingTable;
} CPU_SMM_CLASS;

//
// Structure to describe CPU_SMM_FEATURE_CONTEXT
//
typedef struct {
  BOOLEAN          SmrrEnabled;
  BOOLEAN          EmrrSupported;
} CPU_SMM_FEATURE_CONTEXT;

//
// Pentium4 CPUID signatures
//
#define CPUID_SIGNATURE_PENTIUM                 0x00000F00

//
// Core2 CPUID signatures
//
#define CPUID_SIGNATURE_CONROE                  0x000006F0
#define CPUID_SIGNATURE_CONROE_L                0x00010660
#define CPUID_SIGNATURE_WOLFDALE                0x00010670
#define CPUID_SIGNATURE_DUNNINGTON              0x000106D0
#define CPUID_SIGNATURE_SILVERTHORNE            0x000106C0
#define CPUID_SIGNATURE_TUNNELCREEK             0x00020660

//
// Nehalem CPUID signatures
//
#define CPUID_SIGNATURE_NEHALEM                 0x000106A0
#define CPUID_SIGNATURE_LYNNFIELD               0x000106E0
#define CPUID_SIGNATURE_HAVENDALE               0x000106F0
#define CPUID_SIGNATURE_NEHALEM_EX              0x000206E0
#define CPUID_SIGNATURE_CLARKDALE               0x00020650
#define CPUID_SIGNATURE_WESTMERE                0x000206C0
#define CPUID_SIGNATURE_WESTMERE_EX             0x000206F0
#define CPUID_SIGNATURE_SANDYBRIDGE             0x000206A0
#define CPUID_SIGNATURE_SANDYBRIDGE_EP          0x000206D0
#define CPUID_SIGNATURE_IVYBRIDGE_CLIENT        0x000306A0
#define CPUID_SIGNATURE_IVYBRIDGE_SERVER        0x000306E0
#define CPUID_SIGNATURE_HASWELL                 0x000306C0
#define CPUID_SIGNATURE_HASWELL_ULT             0x00040650
#define CPUID_SIGNATURE_CRYSTALLWELL            0x00040660
#define CPUID_SIGNATURE_BROADWELL_ULT           0x000306D0
#define CPUID_SIGNATURE_BRYSTALWELL             0x00040670

//
// CPUID masks
//
#define CPUID_MASK_NO_STEPPING                  0x0FFF0FF0
#define CPUID_MASK_NO_STEPPING_MODEL            0x0FFF0F00

/**
  Disable SMRR register when SmmInit set SMM MTRRs.
**/
VOID
DisableSmrr (
  VOID
  );

/**
  Enable SMRR register when SmmInit restore non SMM MTRRs.
**/
VOID
ReenableSmrr (
  VOID
  );

/**
  Return if it is needed to configure MTRR to set TSEG cacheability.

  @retval  TRUE  - we need configure MTRR
  @retval  FALSE - we do not need configure MTRR
**/
BOOLEAN
NeedConfigureMtrrs (
  VOID
  );

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousEntry (
  IN UINTN  CpuIndex
  );

/**
  Processor specific hook point at each SMM exit.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousExit (
  IN UINTN  CpuIndex
  );

/**
  Initialize SMRR context in SMM Init.
**/
VOID
InitializeSmmMtrrManager (
  VOID
  );

/**
  Initialize SMRR/EMRR/IED register in SMM Relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
  @param  IedBase            The base address IED. -1 when IED is disabled.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
SmmInitiFeatures (
  IN UINT32  SmrrBase,
  IN UINT32  SmrrSize,
  IN UINT32  IedBase,
  IN BOOLEAN IsBsp
  );

#endif
