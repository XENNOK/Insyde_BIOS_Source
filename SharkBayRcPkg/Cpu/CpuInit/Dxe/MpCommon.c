/** @file
  Code which support multi-processor

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

#include "MpCommon.h"
#include "CpuInitDxe.h"
#include "Features.h"
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmControl.h>
#include <Protocol/ExitPmAuth.h>


///
/// #define CPU_USE_MWAIT_HANDOFF 1
///
 
/*
#ifndef EFI_NO_MEMORY_TEST
EFI_GENERIC_MEMORY_TEST_PROTOCOL  *mGenMemoryTest;
#endif
*/
extern MP_SYSTEM_DATA                   *mMPSystemData;

extern EFI_PHYSICAL_ADDRESS             mOriginalBuffer;
extern EFI_PHYSICAL_ADDRESS             mBackupBuffer;
extern EFI_METRONOME_ARCH_PROTOCOL      *mMetronome;
extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;
volatile UINTN                          mSwitchToLegacyRegionCount = 0;

EFI_PHYSICAL_ADDRESS mLegacyRegion;

/**
  Check if X2APIC is enabled

  @retval TRUE if enabled
  @retval FALSE if not enabled
**/
BOOLEAN
IsXapicEnabled (
  VOID
  )
{
  UINT64 MsrValue;

  MsrValue = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  if (MsrValue & B_MSR_IA32_APIC_BASE_G_XAPIC) {
    if (MsrValue & B_MSR_IA32_APIC_BASE_M_XAPIC) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}

/**
  Function to get APIC register from MSR or MMIO

  @param[in] XapicEnabled    - x2APIC enabled or not
  @param[in] MsrIndex        - MSR index of APIC register
  @param[in] MemoryMappedIo  - MMIO address for APIC register

  @retval The value of APIC register
**/
UINT64
ReadApicMsrOrMemory (
  BOOLEAN XapicEnabled,
  UINT32  MsrIndex,
  UINT64  MemoryMappedIo
  )
{
  UINT64 Value;

  if (XapicEnabled) {
    Value = AsmReadMsr64 (MsrIndex);
  } else {
    Value = (UINT64) *(volatile UINT32 *) (UINTN) MemoryMappedIo;
  }

  return Value;
}

/**
  Function to write APIC register by MSR or MMIO

  @param[in] XapicEnabled    - x2APIC enabled or not
  @param[in] MsrIndex        - MSR index of APIC register
  @param[in] MemoryMappedIo  - MMIO address for APIC register
  @param[in] Value           - Value that will be written to APIC register
**/
VOID
WriteApicMsrOrMemory (
  BOOLEAN XapicEnabled,
  UINT32  MsrIndex,
  UINT64  MemoryMappedIo,
  UINT64  Value
  )
{
  if (XapicEnabled) {
    AsmWriteMsr64 (MsrIndex, Value);
  } else {
    if (MsrIndex == EXT_XAPIC_ICR) {
      *(volatile UINT32 *) (UINTN) (MemoryMappedIo - APIC_REGISTER_ICR_LOW_OFFSET + APIC_REGISTER_ICR_HIGH_OFFSET) = (UINT32) (Value >> 32);
    }
    *(volatile UINT32 *) (UINTN) MemoryMappedIo = (UINT32) Value;
  }
}

/**
  Send interrupt to CPU

  @param[in] BroadcastMode       - Interrupt broadcast mode
  @param[in] ApicID              - APIC ID for sending interrupt
  @param[in] VectorNumber        - Vector number
  @param[in] DeliveryMode        - Interrupt delivery mode
  @param[in] TriggerMode         - Interrupt trigger mode
  @param[in] Assert              - Interrupt pin polarity

  @retval EFI_INVALID_PARAMETER  - Input parameter not correct
  @retval EFI_NOT_READY          - There was a pending interrupt
  @retval EFI_SUCCESS            - Interrupt sent successfully
**/
EFI_STATUS
SendInterrupt (
  IN UINT32  BroadcastMode,
  IN UINT32  ApicID,
  IN UINT32  VectorNumber,
  IN UINT32  DeliveryMode,
  IN UINT32  TriggerMode,
  IN BOOLEAN Assert
  )
{
  UINT64               ApicBaseReg;
  EFI_PHYSICAL_ADDRESS ApicBase;
  UINT32               ICRLow;
  UINT32               ICRHigh;
  BOOLEAN              XapicEnabled;

  ///
  /// Initialze ICR high dword, since P6 family processor needs
  /// the destination field to be 0x0F when it is a broadcast
  ///
  ICRHigh = 0x0f000000;
  ICRLow  = VectorNumber | (DeliveryMode << 8);

  if (TriggerMode == TRIGGER_MODE_LEVEL) {
    ICRLow |= 0x8000;
  }

  if (Assert) {
    ICRLow |= 0x4000;
  }

  XapicEnabled = IsXapicEnabled ();

  switch (BroadcastMode) {
    case BROADCAST_MODE_SPECIFY_CPU:
      if (XapicEnabled) {
        ICRHigh = (UINT32) ApicID;
      } else {
        ICRHigh = ApicID << 24;
      }
      break;

    case BROADCAST_MODE_ALL_INCLUDING_SELF:
      ICRLow |= 0x80000;
      break;

    case BROADCAST_MODE_ALL_EXCLUDING_SELF:
      ICRLow |= 0xC0000;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000;

  ///
  /// According Nehalem BWG, if Extended XAPIC Mode is enabled,
  /// legacy xAPIC is no longer working.
  /// So, previous MMIO offset must be transferred to MSR offset R/W.
  /// ----------------------------------------------------------------
  /// MMIO Offset     MSR Offset     Register Name
  /// ----------------------------------------------------------------
  ///  300h-310h        830h         Interrupt Command Register [63:0]
  ///                   831h         [Reserved]
  /// ----------------------------------------------------------------
  ///
  WriteApicMsrOrMemory (
          XapicEnabled,
          EXT_XAPIC_ICR,
          ApicBase + APIC_REGISTER_ICR_LOW_OFFSET,
          (((UINT64) ICRHigh << 32) | (UINT64) ICRLow)
          );

  gBS->Stall (10);

  ICRLow = (UINT32) ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_ICR, ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);

  if (ICRLow & BIT12) {
    return EFI_NOT_READY;
  }

  gBS->Stall (100);

  return EFI_SUCCESS;
}

/**
  Check number of cores in the package.

  @retval Number of cores in the package.
**/
UINT8
GetCoreNumber (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  AsmCpuidEx (
          4,
          0,
          &Cpuid.RegEax,
          NULL,
          NULL,
          NULL
          );
  return (UINT8)(RShiftU64 (Cpuid.RegEax, 26) & 0x3f) + 1;
}

/**
  Get APIC ID of processor

  @param[in] ApicBase    - APIC base
  @param[in] ApicVersion - APIC version

  @retval APIC ID of processor
**/
UINT32
GetApicID (
  OUT EFI_PHYSICAL_ADDRESS *ApicBase OPTIONAL,
  OUT UINT32 *ApicVersion            OPTIONAL
  )
{
  UINT64  ApicBaseReg;
  UINT32  ApicID;
  UINT32  LocalApicVersion;
  UINT64  LocalApicBase;
  UINTN   MsrValue;
  BOOLEAN XapicEnabled;

  XapicEnabled = IsXapicEnabled ();

  if (XapicEnabled) {
    ///
    /// According Nehalem BWG, if Extended XAPIC Mode
    /// is enabled, legacy xAPIC is no longer working.
    /// So, previous MMIO offset must be transfered
    /// to MSR offset R/W.
    /// MMIO Offset     MSR Offset     Register Name
    ///  020h             802h         EXT_XAPIC_LOGICAL_APIC_ID
    ///  030h             803h         EXT_XAPIC_VERSION
    ///
    MsrValue      = (UINTN) AsmReadMsr64 (EXT_XAPIC_VERSION);
    *ApicVersion  = (UINT32) (MsrValue & 0xff);
    *ApicBase     = 0;

    MsrValue      = (UINTN) AsmReadMsr64 (EXT_XAPIC_LOGICAL_APIC_ID);
    ApicID        = (UINT32) MsrValue;
    return ApicID;
  }

  ApicBaseReg   = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  LocalApicBase = ApicBaseReg & 0xffffff000;
  if (ApicBase) {
    *ApicBase = LocalApicBase;
  }

  ///
  /// if Apic is not enabled yet, enable it here
  ///
  if ((ApicBaseReg & 0x800) == 0) {
    ApicBaseReg |= 0x800;
    AsmWriteMsr64 (MSR_IA32_APIC_BASE, ApicBaseReg);
  }

  if (ApicVersion) {
    LocalApicVersion  = *(volatile UINT32 *) (UINTN) (LocalApicBase + APIC_REGISTER_APIC_VERSION_OFFSET);
    *ApicVersion      = LocalApicVersion & 0xff;
  }

  ApicID = *(volatile UINT32 *) (UINTN) (LocalApicBase + APIC_REGISTER_LOCAL_ID_OFFSET);
  return (ApicID >> 24) & 0x0ff;
}

/**
  Programs Local APIC registers.

  @param[in] BSP  - Is this BSP?
**/
VOID
ProgramXApic (
  BOOLEAN BSP
  )
{
  UINT64               ApicBaseReg;
  EFI_PHYSICAL_ADDRESS ApicBase;
  UINT64               EntryValue;
  BOOLEAN              XapicEnabled;

  ApicBaseReg   = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase      = ApicBaseReg & 0xffffff000;

  XapicEnabled  = IsXapicEnabled ();

  ///
  /// Program the Spurious Vector entry if XAPIC is enabled
  ///
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_SVR, ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
  EntryValue &= 0xFFFFFD0F;
  EntryValue |= 0x10F;
  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_SVR, ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET, EntryValue);

  ///
  /// Double check if it is BSP
  ///
  if (!BSP) {
    CpuDisableInterrupt ();
  }

  ///
  /// Program the LINT0 vector entry as EntInt
  ///
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT0, ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET);
  if (BSP) {
    EntryValue &= 0xFFFE00FF;
    EntryValue |= 0x700;
  } else {
    EntryValue |= 0x10000;
    ///
    /// set bit 16 as mask for LINT0
    ///
  }

  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT0, ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET, EntryValue);

  ///
  /// Program the LINT1 vector entry as NMI
  ///
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT1, ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET);
  EntryValue &= 0xFFFE00FF;
  if (BSP) {
    EntryValue |= 0x400;
  } else {
    EntryValue |= 0x10400;
  }

  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT1, ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET, EntryValue);

}

/**
  Allocate a temporary memory under 1MB for MP Init to perform INIT-SIPI.
  This buffer also provides memory for stack/data for MP running

  @param[in] WakeUpBuffer  - Return buffer location

  @retval EFI_SUCCESS if ok to get a memory under 1MB for MP running.
**/
EFI_STATUS
AllocateWakeUpBuffer (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;
//[-start-120430-IB10540002-modify]//
  for (*WakeUpBuffer = 0x6F000; *WakeUpBuffer >= MP_CPU_WAKEUP_BUFFER_SIZE; *WakeUpBuffer -= 0x1000) {
//[-end-120430-IB10540002-modify]//
     Status = (gBS->AllocatePages)(AllocateAddress, EfiReservedMemoryType, 1, WakeUpBuffer);

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}

/**
  Allocate Reserved Memory

  @param[in] Size       - Memory Size
  @param[in] Alignment  - Alignment size
  @param[in] Pointer    - return memory location

  @retval EFI_SUCCESS   - Allocate a reserved memory successfully
**/
EFI_STATUS
AllocateAlignedReservedMemory (
  IN UINTN Size,
  IN UINTN Alignment,
  OUT VOID **Pointer
  )
{
  EFI_STATUS Status;
  UINTN      PointerValue;

  Status = AllocateReservedMemoryBelow4G (
                  Size + Alignment - 1,
                  Pointer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PointerValue  = (UINTN) *Pointer;
  PointerValue  = (PointerValue + Alignment - 1) / Alignment * Alignment;

  *Pointer      = (VOID *) PointerValue;
  return EFI_SUCCESS;
}

/**
  Fill in the CPU location information

  @param[in] Location  - CPU location information

  @retval EFI_SUCCESS  - always return success
**/
EFI_STATUS
FillInCpuLocation (
  IN CPU_PHYSICAL_LOCATION *Location
  )
{
  UINT32             ApicId;
  EFI_CPUID_REGISTER RegsInfo;
  UINT32             LevelType;
  UINT32             LevelBits;
  UINT8              Shift;
  UINT8              Bits;
  UINT32             Mask;
  BOOLEAN            HyperThreadingEnabled;

  AsmCpuid (CPUID_VERSION_INFO, &RegsInfo.RegEax, &RegsInfo.RegEbx, &RegsInfo.RegEcx, &RegsInfo.RegEdx);
  ApicId = (RegsInfo.RegEbx >> 24);

  AsmCpuid (CPUID_SIGNATURE, &RegsInfo.RegEax, &RegsInfo.RegEbx, &RegsInfo.RegEcx, &RegsInfo.RegEdx);
  if (RegsInfo.RegEax >= CPUID_CORE_TOPOLOGY) {
    LevelBits = 0;
    LevelType = 0;
    do {
      AsmCpuidEx (
              CPUID_CORE_TOPOLOGY,
              LevelType,
              &RegsInfo.RegEax,
              &RegsInfo.RegEbx,
              &RegsInfo.RegEcx,
              &RegsInfo.RegEdx
              );
      LevelType = ((RegsInfo.RegEcx >> 8) & 0xFF);
      switch (LevelType) {
        case 1:
          ///
          /// Thread
          ///
          Location->Thread  = ApicId & ((1 << (RegsInfo.RegEax & 0x0F)) - 1);
          LevelBits         = RegsInfo.RegEax & 0x0F;
          break;

        case 2:
          ///
          /// Core
          ///
          Location->Core  = ApicId >> LevelBits;
          LevelBits       = RegsInfo.RegEax & 0x0F;
          break;

        default:
          ///
          /// End of Level
          ///
          Location->Die     = 0;
          Location->Package = ApicId >> LevelBits;
          break;
      }
    } while (!(RegsInfo.RegEax == 0 && RegsInfo.RegEbx == 0));
  } else {

    AsmCpuid (CPUID_VERSION_INFO, &RegsInfo.RegEax, &RegsInfo.RegEbx, &RegsInfo.RegEcx, &RegsInfo.RegEdx);
    Bits  = 0;
    Shift = (UINT8) ((RegsInfo.RegEbx >> 16) & 0xFF);

    Mask  = Shift - 1;
    while (Shift > 1) {
      Shift >>= 1;
      Bits++;
    }

    HyperThreadingEnabled = FALSE;
    AsmCpuidEx (CPUID_CACHE_PARAMS, 0, &RegsInfo.RegEax, &RegsInfo.RegEbx, &RegsInfo.RegEcx, &RegsInfo.RegEdx);
    if (Mask > (RegsInfo.RegEax >> 26)) {
      HyperThreadingEnabled = TRUE;
    }

    Location->Package = (ApicId >> Bits);
    Location->Die     = 0;
    if (HyperThreadingEnabled) {
      Location->Core    = (ApicId & Mask) >> 1;
      Location->Thread  = (ApicId & Mask) & 1;
    } else {
      Location->Core    = (ApicId & Mask);
      Location->Thread  = 0;
    }
  }

  return EFI_SUCCESS;
}

/**
  Fill in CPU relevant information into data hub

  @param[in] CpuNumber          - CPU number
  @param[in] CpuDataforDatahub  - pointer to data hub that will be updated

  @retval EFI_SUCCESS           - always return success
**/
EFI_STATUS
FillinDataforDataHub (
  IN UINTN                 CpuNumber,
  OUT CPU_DATA_FOR_DATAHUB *CpuDataforDatahub
  )
{
  EFI_CPUID_REGISTER  *CpuIdPtr;
  
  ZeroMem (CpuDataforDatahub, sizeof (*CpuDataforDatahub));

  ///
  /// Read Cpu Frequency from MSR instead
  ///
  CpuDataforDatahub->IntendCoreFrequency =
//[-start-110630-IB03600406-modify]//
    (INTERL_BASE_CLOCK * (((UINT32)AsmReadMsr64 (MSR_IA32_PERF_STS) >> N_EFI_PLATFORM_P_STATE_RATIO) & B_EFI_PLATFORM_P_STATE_RATIO_MASK)) ;
//[-end-110630-IB03600406-modify]//

  GetProcessorVersion (&CpuDataforDatahub->Version);
  CpuDataforDatahub->Manufacturer = GetProcessorManufacturer ();

  CpuIdPtr = (EFI_CPUID_REGISTER *) &CpuDataforDatahub->CpuidData;
  AsmCpuid ( 
    CPUID_VERSION_INFO, 
    &CpuIdPtr->RegEax,
    &CpuIdPtr->RegEbx,
    &CpuIdPtr->RegEcx,
    &CpuIdPtr->RegEdx
    );

  CpuDataforDatahub->Family   = GetProcessorFamily ();
  CpuDataforDatahub->Voltage  = GetProcessorVoltage ();
  CpuDataforDatahub->ApicID = GetApicID (
                  &CpuDataforDatahub->ApicBase,
                  &CpuDataforDatahub->ApicVersion
                  );

  CpuDataforDatahub->MicrocodeRevision = GetCpuUcodeRevision ();
  AsmCpuid ( 
    CPUID_CACHE_INFO, 
    &CpuDataforDatahub->CacheInformation[0].RegEax,
    &CpuDataforDatahub->CacheInformation[0].RegEbx,
    &CpuDataforDatahub->CacheInformation[0].RegEcx,
    &CpuDataforDatahub->CacheInformation[0].RegEdx
    );

  ///
  /// Status field will be updated later, after calling CpuPlatformPolicy protocol to override
  ///
  CpuDataforDatahub->Status = GetProcessorStatus (CpuNumber);;

  FillInCpuLocation (&CpuDataforDatahub->Location);

  ///
  /// Health field will be filled in else where
  ///
  return EFI_SUCCESS;
}

/**
  Allocate EfiReservedMemoryType below 4G memory address.

  @param[in] Size      - Size of memory to allocate.
  @param[in] Buffer    - Allocated address for output.

  @retval EFI_SUCCESS  - Memory successfully allocated.
  @retval Other        - Other errors occur.
**/
EFI_STATUS
AllocateReservedMemoryBelow4G (
  IN UINTN Size,
  OUT VOID **Buffer
  )
{
  UINTN                Pages;
  EFI_PHYSICAL_ADDRESS Address;
  EFI_STATUS           Status;

  Pages   = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = (gBS->AllocatePages)(AllocateMaxAddress, EfiReservedMemoryType, Pages, &Address);

  *Buffer = (VOID *) (UINTN) Address;

  return Status;
}

/**
  This function is invoked when SMM_BASE protocol is installed, then we
  allocate SMRAM and save all information there.

  @param[in] Event   - The triggered event.
  @param[in] Context - Context for this event.
**/
VOID
EFIAPI
InitializeSmramDataContent (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_SMM_BASE_PROTOCOL    *SmmBase;
  SMRAM_CPU_DATA           SmramCpuDataTemplate;
  UINTN                    LockBoxSize;
  UINT8                    *LockBoxData;
  PSEUDO_DESCRIPTOR        *Idtr;
  PSEUDO_DESCRIPTOR        *Gdtr;
  UINTN                    MicrocodeSize;
  EFI_CPU_MICROCODE_HEADER **Microcode;
  UINT8                    *LockBoxMicrocode;
  UINTN                    Index;
  EFI_STATUS               Status;
  EFI_SMM_CONTROL_PROTOCOL *SmmControl;
  UINT8                    *SmramCpuData;
  UINTN                    VarSize;
  SMRAM_CPU_DATA_ADDRESS   SmramCpuDataAddr;
  UINTN                    ArgBufferSize;
  UINT8                    ArgBuffer;
  EFI_SMM_CONTROL_REGISTER SmiRegister;

  DEBUG ((EFI_D_INFO, "InitializeSmramDataContent\n"));

  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &SmmBase);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiSmmControlProtocolGuid, NULL, (VOID **) &SmmControl);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Init
  ///
  CopyMem (&SmramCpuDataTemplate.HeaderGuid, &gSmramCpuDataHeaderGuid, sizeof (EFI_GUID));
  SmramCpuDataTemplate.AcpiCpuPointer = (EFI_PHYSICAL_ADDRESS) (UINTN) mAcpiCpuData;
  CopyMem (&SmramCpuDataTemplate.AcpiCpuData, mAcpiCpuData, sizeof (ACPI_CPU_DATA));

  ///
  /// Calculate size
  ///
  SmramCpuDataTemplate.GdtrProfileSize        = sizeof (PSEUDO_DESCRIPTOR);
  Gdtr = (PSEUDO_DESCRIPTOR *) (UINTN) mAcpiCpuData->GdtrProfile;
  SmramCpuDataTemplate.GdtSize = Gdtr->Limit + 1;
  SmramCpuDataTemplate.IdtrProfileSize = sizeof (PSEUDO_DESCRIPTOR);
  Idtr = (PSEUDO_DESCRIPTOR *) (UINTN) mAcpiCpuData->GdtrProfile;
  SmramCpuDataTemplate.IdtSize = Idtr->Limit + 1;
  SmramCpuDataTemplate.CpuPrivateDataSize = sizeof (MP_CPU_S3_DATA_POINTER);
  SmramCpuDataTemplate.S3BootScriptTableSize = sizeof (mMPSystemData->S3BootScriptTable);
  SmramCpuDataTemplate.S3BspMtrrTableSize = sizeof (mMPSystemData->S3BspMtrrTable);
  ///
  /// Record best match for each CPU Microcode and NULL for end
  ///
  SmramCpuDataTemplate.MicrocodePointerBufferSize = sizeof (UINT32) * (mAcpiCpuData->NumberOfCpus + 1);
  ///
  /// Calculate Microcode DataSize
  ///
  SmramCpuDataTemplate.MicrocodeDataBufferSize  = 0;
  Microcode = (VOID *) (UINTN) mAcpiCpuData->MicrocodePointerBuffer;
  if (Microcode != NULL) {
    Index         = 0;
    MicrocodeSize = 0;
    while (Microcode[Index] != NULL) {
      if (Microcode[Index]->DataSize == 0) {
        MicrocodeSize = 2048;
      } else {
        MicrocodeSize = Microcode[Index]->TotalSize;
      }

      SmramCpuDataTemplate.MicrocodeDataBufferSize += (UINT32) MicrocodeSize;
      Index++;
    }
  }

  SmramCpuDataTemplate.GdtrProfileOffset    = sizeof (SMRAM_CPU_DATA);
  SmramCpuDataTemplate.GdtOffset            = SmramCpuDataTemplate.GdtrProfileOffset + SmramCpuDataTemplate.GdtrProfileSize;
  SmramCpuDataTemplate.IdtrProfileOffset    = SmramCpuDataTemplate.GdtOffset + SmramCpuDataTemplate.GdtSize;
  SmramCpuDataTemplate.IdtOffset            = SmramCpuDataTemplate.IdtrProfileOffset + SmramCpuDataTemplate.IdtrProfileSize;
  SmramCpuDataTemplate.CpuPrivateDataOffset = SmramCpuDataTemplate.IdtOffset + SmramCpuDataTemplate.IdtSize;
  SmramCpuDataTemplate.S3BootScriptTableOffset = SmramCpuDataTemplate.CpuPrivateDataOffset + SmramCpuDataTemplate.CpuPrivateDataSize;
  SmramCpuDataTemplate.S3BspMtrrTableOffset = SmramCpuDataTemplate.S3BootScriptTableOffset + SmramCpuDataTemplate.S3BootScriptTableSize;
  SmramCpuDataTemplate.MicrocodePointerBufferOffset = SmramCpuDataTemplate.S3BspMtrrTableOffset + SmramCpuDataTemplate.S3BspMtrrTableSize;
  SmramCpuDataTemplate.MicrocodeDataBufferOffset = SmramCpuDataTemplate.MicrocodePointerBufferOffset + SmramCpuDataTemplate.MicrocodePointerBufferSize;

  LockBoxSize = sizeof (SMRAM_CPU_DATA) +
          SmramCpuDataTemplate.GdtrProfileSize +
          SmramCpuDataTemplate.GdtSize +
          SmramCpuDataTemplate.IdtrProfileSize +
          SmramCpuDataTemplate.IdtSize +
          SmramCpuDataTemplate.CpuPrivateDataSize +
          SmramCpuDataTemplate.S3BootScriptTableSize +
          SmramCpuDataTemplate.S3BspMtrrTableSize +
          SmramCpuDataTemplate.MicrocodePointerBufferSize +
          SmramCpuDataTemplate.MicrocodeDataBufferSize;

  DEBUG ((EFI_D_INFO, "LockBoxSize                               - %x\n", LockBoxSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.GdtrProfileSize              - %x\n", SmramCpuDataTemplate.GdtrProfileSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.GdtSize                      - %x\n", SmramCpuDataTemplate.GdtSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.IdtrProfileSize              - %x\n", SmramCpuDataTemplate.IdtrProfileSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.IdtSize                      - %x\n", SmramCpuDataTemplate.IdtSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.CpuPrivateDataSize           - %x\n", SmramCpuDataTemplate.CpuPrivateDataSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.S3BootScriptTableSize        - %x\n", SmramCpuDataTemplate.S3BootScriptTableSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.S3BspMtrrTableSize           - %x\n", SmramCpuDataTemplate.S3BspMtrrTableSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.MicrocodePointerBufferSize   - %x\n", SmramCpuDataTemplate.MicrocodePointerBufferSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.MicrocodeDataBufferSize      - %x\n", SmramCpuDataTemplate.MicrocodeDataBufferSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData.GdtrProfileOffset            - %x\n", SmramCpuDataTemplate.GdtrProfileOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.GdtOffset                    - %x\n", SmramCpuDataTemplate.GdtOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.IdtrProfileOffset            - %x\n", SmramCpuDataTemplate.IdtrProfileOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.IdtOffset                    - %x\n", SmramCpuDataTemplate.IdtOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.CpuPrivateDataOffset         - %x\n", SmramCpuDataTemplate.CpuPrivateDataOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.S3BootScriptTableOffset      - %x\n", SmramCpuDataTemplate.S3BootScriptTableOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.S3BspMtrrTableOffset         - %x\n", SmramCpuDataTemplate.S3BspMtrrTableOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.MicrocodePointerBufferOffset - %x\n", SmramCpuDataTemplate.MicrocodePointerBufferOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData.MicrocodeDataBufferOffset    - %x\n", SmramCpuDataTemplate.MicrocodeDataBufferOffset));

  ///
  /// Allocate Normal Memory
  ///
  Status = (gBS->AllocatePool)(EfiBootServicesData, LockBoxSize, &SmramCpuData);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate SMRAM
  ///
  Status = SmmBase->SmmAllocatePool (
                  SmmBase,
                  EfiRuntimeServicesData,
                  LockBoxSize + EFI_PAGE_SIZE,
                  (VOID **)&LockBoxData
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Let it page aligned
  ///
  LockBoxData = (UINT8 *) (((UINTN) LockBoxData + EFI_PAGE_SIZE - 1) &~(EFI_PAGE_SIZE - 1));
  DEBUG ((EFI_D_INFO, "CPU SMRAM NVS Data - %x\n", LockBoxData));

  ///
  /// Copy data buffer
  ///
  CopyMem (SmramCpuData, &SmramCpuDataTemplate, sizeof (SmramCpuDataTemplate));

  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.GdtrProfileOffset,
          (VOID *) (UINTN) mAcpiCpuData->GdtrProfile,
          SmramCpuDataTemplate.GdtrProfileSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.GdtOffset,
          (VOID *) (UINTN) Gdtr->Base,
          SmramCpuDataTemplate.GdtSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.IdtrProfileOffset,
          (VOID *) (UINTN) mAcpiCpuData->IdtrProfile,
          SmramCpuDataTemplate.IdtrProfileSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.IdtOffset,
          (VOID *) (UINTN) Idtr->Base,
          SmramCpuDataTemplate.IdtSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.CpuPrivateDataOffset,
          (VOID *) (UINTN) mAcpiCpuData->CpuPrivateData,
          SmramCpuDataTemplate.CpuPrivateDataSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.S3BootScriptTableOffset,
          (VOID *) (UINTN) mMPSystemData->S3DataPointer.S3BootScriptTable,
          SmramCpuDataTemplate.S3BootScriptTableSize
          );
  CopyMem (
          SmramCpuData + SmramCpuDataTemplate.S3BspMtrrTableOffset,
          (VOID *) (UINTN) mMPSystemData->S3DataPointer.S3BspMtrrTable,
          SmramCpuDataTemplate.S3BspMtrrTableSize
          );
  Microcode = (VOID *) (UINTN) mAcpiCpuData->MicrocodePointerBuffer;
  if (Microcode != NULL) {
    ///
    /// Copy Microcode Pointer Buffer
    ///
    CopyMem (
            SmramCpuData + SmramCpuDataTemplate.MicrocodePointerBufferOffset,
            Microcode,
            SmramCpuDataTemplate.MicrocodePointerBufferSize
            );
    ///
    /// Copy Microcode Data
    ///
    Index             = 0;
    MicrocodeSize     = 0;
    LockBoxMicrocode  = SmramCpuData + SmramCpuDataTemplate.MicrocodeDataBufferOffset;
    while (Microcode[Index] != NULL) {
      if (Microcode[Index]->DataSize == 0) {
        MicrocodeSize = 2048;
      } else {
        MicrocodeSize = Microcode[Index]->TotalSize;
      }

      CopyMem (LockBoxMicrocode, Microcode[Index], MicrocodeSize);
      LockBoxMicrocode += MicrocodeSize;
      Index++;
    }
  }

  ///
  /// Copy to SMRAM
  ///
  ///
  /// We have to use SMI to copy SMRAM, because we can not access SMRAM after SMRR enabled.
  /// SMM_ACCESS.Open () takes no effect.
  ///
  VarSize                       = sizeof (SmramCpuDataAddr);
  SmramCpuDataAddr.LockBoxData  = (UINT64) (UINTN) LockBoxData;
  SmramCpuDataAddr.SmramCpuData = (UINT64) (UINTN) SmramCpuData;
  SmramCpuDataAddr.LockBoxSize  = (UINT64) LockBoxSize;

  Status = gRT->SetVariable (
                  SMRAM_CPU_DATA_VARIABLE,
                  &gSmramCpuDataVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VarSize,
                  &SmramCpuDataAddr
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Fill SMI data port
  ///
  Status = SmmControl->GetRegisterInfo (SmmControl, &SmiRegister);
  ASSERT_EFI_ERROR (Status);
  IoWrite8 (SmiRegister.SmiDataRegister, SMM_FROM_CPU_DRIVER_SAVE_INFO);

  ///
  /// Trigger SMI
  ///
  ArgBufferSize = sizeof (ArgBuffer);
  ArgBuffer     = mSmmbaseSwSmiNumber;
  Status        = SmmControl->Trigger (SmmControl, (INT8 *) &ArgBuffer, &ArgBufferSize, FALSE, 0);
  Status        = SmmControl->Clear (SmmControl, 0);
  return;
}
//[-start-120321-IB02700366-add]// 
  /*++
  
  Routine Description:
  
    Check whether EBDA is present or not
  
  Arguments:
  
    None
  
  Returns:
  
    TRUE  - Class 1 or Class 2 BIOS
    FALSE - Class 3 BIOS	
  
  --*/ 
BOOLEAN
IsEBDAPresent (
  )
{
  if ((BDA (0x13) == 0xFFFF) || (BDA (0x13) == 0)) {
    return FALSE;
  }
  //
  // BDA offset 0x13(40:13) * 0x400 is equal EBDA address
  //
  return ((BDA (0x13) * 0x400) == (BDA (0x0e) << 4));
}
//[-end-120321-IB02700366-add]// 

//[-start-120321-IB02700366-modify]// 
/**
  
    This function is invoked when LegacyBios protocol is installed, we must
    allocate reserved memory under 1M for AP.
  
    @param[in] Event   - The triggered event.
    @param[in] Context - Context for this event.


  **/
VOID
EFIAPI
ReAllocateEbdaMemoryForAP (
//  IN EFI_EVENT  Event,
//  IN VOID       *Context
  )
//[-end-120321-IB02700366-modify]//  
{
//[-start-120321-IB02700366-modify]// 
//  EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios;
  EFI_PHYSICAL_ADDRESS        EbdaOld; 
  EFI_PHYSICAL_ADDRESS        EbdaNew; 
  UINTN                       EbdaSize;
  EFI_STATUS                  Status;

//  //
//  // Check whether this is real LegacyBios notification
//  //
//  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
//  if (EFI_ERROR (Status)) {
//    return ;
//  }
//[-end-120321-IB02700366-modify]//
  ///
  /// PLEASE NOTE:
  /// For legacy implementation, we have reserved 0x9F000 to 0x9FFFF for S3 usage in CSM,
  /// Please don't need to allocate it again
  /// This range will be used for MpS3 driver and S3Resume driver on S3 boot path
  /// The base needs to be aligned to 4K to satisfy the AP vector requirement
  /// The original implementation requires 8K from legacy memory form E/F segment,
  /// which needs lock/unlock and makes lots of code chipset dependent on S3 boot path
  /// Here we just use normal low memory to eliminate the dependency
  /// In this case, EBDA will start from 0x9F000 - sizeof (EBDA) in CSM definition
  /// CSM EBDA base and memory size in BDA area needs to be consistent with this
  ///
  ///
  /// Get EDBA address/length and turn it into the S3 reserved address
  /// The length of this range is limited so we need to keep the real mode code small
  ///
  EbdaOld                     = (EFI_PHYSICAL_ADDRESS) (*(UINT16 *) (UINTN) 0x40E) << 4;;
  EbdaSize                    = (UINTN) (*((UINT8 *) (UINTN) EbdaOld));
  mLegacyRegion               = EbdaOld + (EbdaSize << 10);
  mLegacyRegion               = (mLegacyRegion - 0x1000) & 0xFFFFF000;
  EbdaNew                     = mLegacyRegion - (EbdaSize << 10);
  (*(UINT16 *) (UINTN) 0x40E) = (UINT16) (EbdaNew >> 4);
  CopyMem ((VOID *) (UINTN) EbdaNew, (VOID *) (UINTN) EbdaOld, EbdaSize << 10);

  ///
  /// Update 40:13 with the new size of available base memory
  ///
  *(UINT16 *) (UINTN) 0x413 = (*(UINT16 *) (UINTN) 0x413) - (UINT16) (((EbdaOld - EbdaNew) >> 10));

  ///
  /// Free the Wake-up buffer and re-declare it as Reserved Memory
  ///
  DEBUG ((EFI_D_INFO, "Legacy region freed before re-allocation:  %X\n", mLegacyRegion));
  Status  = (gBS->FreePages) (mLegacyRegion, 1);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "Allocate and reserve the 4K buffer for Legacy Region\n"));
  Status = (gBS->AllocatePages)(AllocateAddress, EfiReservedMemoryType, 1, &mLegacyRegion);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "mLegacyRegion CSM - %x\n", mLegacyRegion));
}

/**
  This function is invoked when LegacyBios protocol is installed, we must
  allocate reserved memory under 1M for AP.

  @param[in] Event    - The triggered event.
  @param[in] Context  - Context for this event.
**/
VOID
EFIAPI
ReAllocateMemoryForAP (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
//[-start-120321-IB02700366-modify]//  
//  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_STATUS                Status;
  EFI_PHYSICAL_ADDRESS      LegacyRegion;
  MP_CPU_EXCHANGE_INFO      *ExchangeInfo;
  MONITOR_MWAIT_DATA        *MonitorAddr;
  UINTN                     Index;
  UINT64                    MaxCstate;
  UINT64                   CStateLimit;
  UINT32                   SubStates;
  EFI_CPUID_REGISTER       MwaitInfo;
  BOOLEAN                     Class3BIOS;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *MpService;

  VOID                      *ExitPmAuth;

  ///
  /// Check whether this is real ExitPmAuth notification
  ///
  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ExitPmAuth);
  if (EFI_ERROR (Status)) {
    return ;
  }


  if (IsEBDAPresent ()) {
    Class3BIOS = FALSE;
  } else {
    Class3BIOS = TRUE;
  }

  while (ApRunning ()) {
    CpuPause ();
  }
  ///
  /// Re-load microcode patch here!!!
  ///
  ReLoadMicrocodeBeforeBoot ();

  if (!Class3BIOS) {
    //
	// Allocate memory from EBDA
	//
    ReAllocateEbdaMemoryForAP ();
    LegacyRegion = mLegacyRegion;
    DEBUG ((EFI_D_INFO, "Using LegacyRegion CSM - %x\n", LegacyRegion));
  } else {
    ///
    /// The BackBuffer is 4k. Allocate 0x2000 bytes from below 640K memory to ensure 4k aligned spaces of 0x1000 bytes,
    /// since Alignment argument does not work.
    ///
    LegacyRegion = 0x9FFFF;
//[-start-120430-IB10540002-modify]//
    Status = (gBS->AllocatePages) (AllocateMaxAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (MP_CPU_WAKEUP_BUFFER_SIZE), &LegacyRegion);
//[-end-120430-IB10540002-modify]//
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "LegacyRegion NonCSM - %x\n", LegacyRegion));
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  ///
  /// This address should be less than A seg.
  /// And it should be aligned to 4K
  ///
  ASSERT (!((UINTN) LegacyRegion & 0x0FFF) && ((UINTN) LegacyRegion < 0xA0000));

  mAcpiCpuData->WakeUpBuffer  = (EFI_PHYSICAL_ADDRESS) LegacyRegion;
  mAcpiCpuData->WakeUpBuffer  = (mAcpiCpuData->WakeUpBuffer + 0x0fff) & 0x0fffff000;

  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mBackupBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  ExchangeInfo->BufferStart   = (UINT32) mAcpiCpuData->WakeUpBuffer;
  CopyMem (
          (VOID *) (UINTN) mAcpiCpuData->WakeUpBuffer,
          (VOID *) (UINTN) mBackupBuffer,
          EFI_PAGE_SIZE
          );
  RedirectFarJump ();

    CopyMem (
      (VOID *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET),
      (VOID *) (UINTN) (mBackupBuffer + MP_CPU_EXCHANGE_INFO_OFFSET),
      sizeof (MP_CPU_EXCHANGE_INFO)
      );

  ///
  /// Set all APs to deepest C-State before ready to boot for better power saving,
  /// if boot to DOS/EFI_SHARE or any operating system that running only single thread.
  ///
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  if (mPlatformCpu->CpuConfig->ApHandoffManner != WakeUpApPerHltLoop) {
    ///
    /// Based on HSW BWG 17.2.7, BIOS should use CPUID.(EAX=5) Monitor/Mwait Leaf and also check MSR E2h[3:0] Package C-state limit to determine
    /// if the processor supports MONITOR/MWAIT extensions for various Haswell specific C-states and sub C-states.
    ///
    CStateLimit = AsmReadMsr64 (MSR_PMG_CST_CONFIG) & B_PACKAGE_C_STATE_LIMIT;
    AsmCpuid (5, &MwaitInfo.RegEax, &MwaitInfo.RegEbx, &MwaitInfo.RegEcx, &MwaitInfo.RegEdx);
    MaxCstate = 0;
    SubStates = 0;
    if (MwaitInfo.RegEcx & BIT0) {
      switch (CStateLimit) {
        case V_CSTATE_LIMIT_C10:
          SubStates = (MwaitInfo.RegEdx & (BIT31 | BIT30 | BIT29 | BIT28)) >> 28;
          MaxCstate = 0x60 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C9:
          SubStates = (MwaitInfo.RegEdx & (BIT27 | BIT26 | BIT25 | BIT24)) >> 24;
          MaxCstate = 0x50 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C8:
          SubStates = (MwaitInfo.RegEdx & (BIT23 | BIT22 | BIT21 | BIT20)) >> 20;
          MaxCstate = 0x40 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C7S:
          SubStates = (MwaitInfo.RegEdx & (BIT19 | BIT18 | BIT17 | BIT16)) >> 16;
          MaxCstate = 0x30 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C7:
          SubStates = (MwaitInfo.RegEdx & (BIT19 | BIT18 | BIT17 | BIT16)) >> 16;
          MaxCstate = 0x30 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C6:
          SubStates = (MwaitInfo.RegEdx & (BIT15 | BIT14 | BIT13 | BIT12)) >> 12;
          MaxCstate = 0x20 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C3:
          SubStates = (MwaitInfo.RegEdx & (BIT11 | BIT10 | BIT9 | BIT8)) >> 8;
          MaxCstate = 0x10 | (SubStates - 1);
          break;

        case V_CSTATE_LIMIT_C1:
          SubStates = (MwaitInfo.RegEdx & (BIT7 | BIT6 | BIT5 | BIT4)) >> 4;
          MaxCstate = 0x00 | (SubStates - 1);
          break;

        default:
          break;
      }
    }

    ///
    /// Use WakeUpApPerMwaitLoop32 if CR4 paging table entities are not allocated as RESERVED MEMORY TYPE in 64-bits mode.
    ///
    ExchangeInfo->WakeUpApManner  = WakeUpApPerMwaitLoop32;
    for (Index = 0; Index < mMPSystemData->NumberOfCpus; Index++) {
      MonitorAddr = (MONITOR_MWAIT_DATA *) ((UINT8 *) ExchangeInfo->StackStart + (Index + 1) * ExchangeInfo->StackSize - MONITOR_FILTER_SIZE);
      MonitorAddr->WakeUpApVectorChangeFlag = TRUE;
      MonitorAddr->MwaitTargetCstate        = MaxCstate;
    }
  } else {
    ExchangeInfo->WakeUpApManner = WakeUpApPerHltLoop;
  }

  ///
  /// Locate MpServices protocol
  ///
  Status = gBS->LocateProtocol (
                  &gFrameworkEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Move Limit CPUID Maxval configuration here to not impact the BOOT
  /// After setting this, no code can execute CPUID function > 3.
  ///
  ProgramCpuidLimit (MpService);
  Status = MpService->StartupAllAPs (
                  MpService,
                  ProgramCpuidLimit,
                  FALSE,
                  NULL,
                  0,
                  MpService,
                  NULL
                  );

  ///
  /// Invoke the InitializeSmram directly, since it is in ExitPmAuth event.
  ///
  InitializeSmramDataContent (NULL, NULL);
  
  gBS->CloseEvent (Event);
//[-end-120321-IB02700366-modify]//    
}

/**
  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, reset AP's wakeup buffer address,
  preparing for S3 usage.

  @param[in] Event    - The triggered event.
  @param[in] Context  - Context for this event.
**/
VOID
ResetAPs (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  return;
}
