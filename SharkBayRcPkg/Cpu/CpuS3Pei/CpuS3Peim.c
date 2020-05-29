/** @file
  Cpu driver running on S3 boot path

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

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "MpCommon.h"

#include <Ppi/Stall.h>
#include <Guid/SmramCpuDataHeader.h>
#include <Ppi/CpuPlatformPolicy.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/DebugLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/BootGuardLibrary.h>
#include <Ppi/SmmAccess.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FrameworkMpService.h>
//[-start-130426-IB10930028-add]//
#include <PchRegs.h>
#include <PchRegs/PchRegsLpc.h>
#include <PciAccess.h>
#include <Library/IoLib.h>
//[-end-130426-IB10930028-add]//

#define PEI_ACPI_CPU_DATA_HOB_GUID \
  { 0x7682bbef, 0xb0b6, 0x4939, 0xae, 0x66, 0x1b, 0x3d, 0xf2, 0xf6, 0xaa, 0xf3};

EFI_GUID gPeiAcpiCpuDataGuid = PEI_ACPI_CPU_DATA_HOB_GUID;
typedef VOID (*S3_AP_PROCEDURE)(
  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  UINT64               *MtrrValues
  );

/**
  This function handles SA S3 resume task

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
static
EFI_STATUS
CpuS3ResumeAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  );

EFI_GUID                         gEfiPeiEndOfPeiPhasePpiGuid = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpuS3ResumeNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiEndOfPeiPhasePpiGuid,
  CpuS3ResumeAtEndOfPei
};

//[-start-130426-IB10930028-add]//
/**
  Enable GBL_SMI_EN and APMC_EN before executing S3 resume script table

  Note: Some routine save SMI command to script table, so GBL_SMI_EN and APMC_EN
           must be enable before executing the table

  @param          None
  @retval           None
**/
VOID
EnableSMI (
  VOID
  )
{
  UINT32                                        PmBase;  
  UINT32                                        OutputPort;
  UINT32                                        OutputData;

  PmBase = MmioRead32 (
             MmPciAddress (
             0,
             DEFAULT_PCI_BUS_NUMBER_PCH,
             PCI_DEVICE_NUMBER_PCH_LPC,
             PCI_FUNCTION_NUMBER_PCH_LPC,
             R_PCH_LPC_ACPI_BASE)
             ) & B_PCH_LPC_ACPI_BASE_BAR;
  
  ///
  /// Enable the APMC SMI
  ///
  OutputPort  = PmBase + R_PCH_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData  = (B_PCH_SMI_EN_APMC | B_PCH_SMI_EN_GBL_SMI);
  IoWrite32 ((UINTN) OutputPort, (UINT32) (OutputData));
}
//[-end-130426-IB10930028-add]//


/**
  Get APIC ID of processor

  @retval APIC ID of processor
**/
UINT32
GetApicID (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;

  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  return (UINT32) (CpuidRegisters.RegEbx >> 24);
}

/**
  Send interrupt to CPU

  @param[in] BroadcastMode - interrupt broadcast mode
  @param[in] ApicID        - APIC ID for sending interrupt
  @param[in] VectorNumber  - Vector number
  @param[in] DeliveryMode  - Interrupt delivery mode
  @param[in] TriggerMode   - Interrupt trigger mode
  @param[in] Assert        - Interrupt pin polarity
  @param[in] PeiServices   - Indirect reference to the PEI Services Table
  @param[in] PeiStall      - EFI_PEI_STALL_PPI to stall for some interval

  @retval EFI_INVALID_PARAMETER - input parameter not correct
  @retval EFI_NOT_READY         - there was a pending interrupt
  @retval EFI_SUCCESS           - interrupt sent successfully
**/
EFI_STATUS
SendInterrupt (
  IN UINT32            BroadcastMode,
  IN UINT32            ApicID,
  IN UINT32            VectorNumber,
  IN UINT32            DeliveryMode,
  IN UINT32            TriggerMode,
  IN BOOLEAN           Assert,
  IN EFI_PEI_SERVICES  **PeiServices,
  IN EFI_PEI_STALL_PPI *PeiStall
  )
{
  UINT64               ApicBaseReg;
  EFI_PHYSICAL_ADDRESS ApicBase;
  UINT32               ICRLow;
  UINT32               ICRHigh;

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

  switch (BroadcastMode) {
    case BROADCAST_MODE_SPECIFY_CPU:
      ICRHigh = ApicID << 24;
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
  ApicBase    = ApicBaseReg & 0xffffff000ULL;

  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_HIGH_OFFSET) = ICRHigh;
  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET)  = ICRLow;

  PeiStall->Stall (
          (CONST EFI_PEI_SERVICES **)PeiServices,
          PeiStall,
          10 * STALL_ONE_MICRO_SECOND
          );

  ICRLow = *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);
  if (ICRLow & 0x1000) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}

/**
  Programs XAPIC registers.

  @param[in] BSP             - Is this BSP?
**/
VOID
ProgramXApic (
  BOOLEAN BSP
  )
{
  UINT64               ApicBaseReg;
  EFI_PHYSICAL_ADDRESS ApicBase;
  volatile UINT32      *EntryAddress;
  UINT32               EntryValue;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000ULL;

  ///
  /// Program the Spurious Vectore entry
  ///
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFFFD0F;
  EntryValue |= 0x10F;
  *EntryAddress = EntryValue;

  ///
  /// Program the LINT1 vector entry as extINT
  ///
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;

  if (BSP) {
    EntryValue &= 0xFFFE00FF;
    EntryValue |= 0x700;
  } else {
    EntryValue |= 0x10000;
  }

  *EntryAddress = EntryValue;

  ///
  /// Program the LINT1 vector entry as NMI
  ///
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFE00FF;
  if (BSP) {
    EntryValue |= 0x400;
  } else {
    EntryValue |= 0x10400;
  }

  *EntryAddress = EntryValue;

}

/**
  Restore all MSR settings

  @param[in] ScriptTable - contain the MSR settings that will be restored
**/
VOID
InitializeFeatures (
  IN MP_CPU_S3_SCRIPT_DATA *ScriptTable
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  UINT32             ApicId;
  UINT8              SkipMsr;
  ///
  /// Restore all the MSRs for processor
  ///
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  ApicId = (CpuidRegisters.RegEbx >> 24);

  while (ScriptTable->MsrIndex != 0) {
    if (ApicId == ScriptTable->ApicId) {
      SkipMsr = 0;
      if ((ScriptTable->MsrIndex == MSR_PMG_CST_CONFIG) && (AsmReadMsr64 (MSR_PMG_CST_CONFIG) & B_CST_CONTROL_LOCK)) {
        SkipMsr = 1;
      }
      if (SkipMsr == 0) {
        AsmWriteMsr64 (ScriptTable->MsrIndex, ScriptTable->MsrValue);
      }
    }
    ScriptTable++;
  }
}

/**
  Restore all MSR settings with debug message output

  @param[in] PeiServices - General purpose services available to every PEIM.
  @param[in] ScriptTable - Script table contain all MSR settings that will be restored
**/
VOID
InitializeFeaturesLog (
  IN EFI_PEI_SERVICES      **PeiServices,
  IN MP_CPU_S3_SCRIPT_DATA *ScriptTable
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  UINT32             ApicId;
  BOOLEAN            SkipMsr;

  ///
  /// Restore all the MSRs for processor
  ///
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  ApicId = (CpuidRegisters.RegEbx >> 24);

  while (ScriptTable->MsrIndex != 0) {
    if (ApicId == ScriptTable->ApicId) {
      DEBUG ((EFI_D_INFO, "MSR Index - %x, MSR value - %x\n", ScriptTable->MsrIndex, ScriptTable->MsrValue));
      SkipMsr = FALSE;
      if ((ScriptTable->MsrIndex == MSR_PMG_CST_CONFIG) && (AsmReadMsr64 (MSR_PMG_CST_CONFIG) & B_CST_CONTROL_LOCK)) {
        SkipMsr = TRUE;
      }
      if (ScriptTable->MsrIndex == MSR_IA32_DEBUG_INTERFACE) {
        /* Access to MSR_IA32_DEBUG_INTERFACE is supported on
  HSW B0, HSWULT B0 and CRW B0 Stepping
  HSW stepping >= C0, HSWULT Stepping >= C0 and CRW stepping >= C0 stepping, if CPUID (EAX=1): ECX[11] = 1
        */
        switch (CpuidRegisters.RegEax) {
          case (EnumCpuHsw + EnumHswA0):
            DEBUG ((EFI_D_INFO,"MSR_IA32_DEBUG_INTERFACE is not supported on Ax CPU stepping\n"));
            break;
          case (EnumCpuHsw    + EnumHswB0):
          case (EnumCpuHswUlt + EnumHswUltB0):
          case (EnumCpuCrw    + EnumCrwB0):
            if ((AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE) & B_DEBUG_INTERFACE_LOCK)) {
  SkipMsr = TRUE;
            }
            break;
          default:
            if (CpuidRegisters.RegEcx & BIT11) {
              if ((AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE) & B_DEBUG_INTERFACE_LOCK)) {
                SkipMsr = TRUE;
              }
            }
            break;
        }
      }
      if (!SkipMsr) {
        AsmWriteMsr64 (ScriptTable->MsrIndex, ScriptTable->MsrValue);
      }
    }
    ScriptTable++;
  }
}

/**
  Initialize prefetcher settings

  @param[in] MlcStreamerprefecterEnabled - Enable/Disable MLC streamer prefetcher
  @param[in] MlcSpatialPrefetcherEnabled - Enable/Disable MLC spatial prefetcher
//[-start-130906-IB08620308-add]//
  @param[in] DcuStreamerPrefetcher       - Enable/Disable DCU Streamer Prefetcher
  @param[in] DcuIPPrefetcher             - Enable/Disable DCU IP Prefetcher
//[-end-130906-IB08620308-add]//
**/
VOID
ProcessorsPrefetcherInitialization (
  IN UINTN MlcStreamerprefecterEnabled,
  IN UINTN MlcSpatialPrefetcherEnabled
//[-start-130906-IB08620308-add]//
  ,
  IN UINTN DcuStreamerPrefetcher,
  IN UINTN DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
  )
{
  UINT64 MsrValue;
  MsrValue = AsmReadMsr64 (MISC_FEATURE_CONTROL);

  if (MlcStreamerprefecterEnabled == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_STRP;
  }

  if (MlcSpatialPrefetcherEnabled == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_SPAP;
  }

//[-start-130906-IB08620308-add]//
  if (DcuStreamerPrefetcher == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_DCU_STRP;
  }

  if (DcuIPPrefetcher == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_DCU_IPP;
  }
//[-end-130906-IB08620308-add]//

//[-start-130906-IB08620308-modify]//
  if ((MsrValue & (B_MISC_FEATURE_CONTROL_MLC_STRP | 
                   B_MISC_FEATURE_CONTROL_MLC_SPAP | 
                   B_MISC_FEATURE_CONTROL_DCU_STRP | 
                   B_MISC_FEATURE_CONTROL_DCU_IPP)) != 0) {
    AsmWriteMsr64 (MISC_FEATURE_CONTROL, MsrValue);
  }
//[-end-130906-IB08620308-modify]//

  return;
}

/**
  AP initialization

  @param[in] ExchangeInfo  - Pointer to the exchange info buffer for output.
  @param[in] MtrrValues    - buffer contains MTRR settings
**/
VOID
MPRendezvousProcedure (
  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  UINT64               *MtrrValues
  )
{
  UINT32                 FailedRevision;
  ACPI_CPU_DATA          *AcpiCpuData;
  EFI_PEI_SERVICES       **PeiServices;
  EFI_PEI_STALL_PPI      *PeiStall;
  MP_CPU_S3_DATA_POINTER *CpuS3DataPtr;
//[-start-120924-IB06460446-add]//
  UINT32                  CpreApicID;
//[-end-120924-IB06460446-add]//

  ///
  /// Switch AP speed to BSP speed
  ///
  AsmWriteMsr64 (MSR_IA32_PERF_CTRL, ExchangeInfo->CpuPerfCtrlValue);

  AcpiCpuData  = (ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress);
  PeiServices   = ExchangeInfo->PeiServices;
  PeiStall      = ExchangeInfo->PeiStall;

  ProgramXApic (FALSE);

  InitializeMicrocode (
          ExchangeInfo,
          (EFI_CPU_MICROCODE_HEADER **) (UINTN) ExchangeInfo->MicrocodePointer,
          &FailedRevision
          );

//[-start-120924-IB06460446-modify]//
  CpreApicID = GetApicID ();
  if ((CpreApicID == 0x2) || (CpreApicID == 0x4) || (CpreApicID == 0x6)) {
    //
    // We only need to update MSR 0x1A4 for primary thread, because its scopes is Core.
    //
    ProcessorsPrefetcherInitialization (
      ExchangeInfo->CpuPlatformPolicyPpi->CpuConfig->MlcStreamerPrefetcher,
      ExchangeInfo->CpuPlatformPolicyPpi->CpuConfig->MlcSpatialPrefetcher
//[-start-130906-IB08620308-add]//
      ,
      ExchangeInfo->CpuPlatformPolicyPpi->CpuConfig->DcuStreamerPrefetcher,
      ExchangeInfo->CpuPlatformPolicyPpi->CpuConfig->DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
      );
  }
//[-end-120924-IB06460446-modify]//

  ///
  /// wait till all CPUs done the Microcode Load
  ///
  while (ExchangeInfo->McuLoadCount < AcpiCpuData->NumberOfCpus) {
    CpuPause ();
  }

//[-start-120924-IB06460446-modify]//
  if ((CpreApicID == 0x2) || (CpreApicID == 0x4) || (CpreApicID == 0x6)) {
    //
    // We only need to update MTRRs for primary thread, because their scopes are Core.
    //
    MpMtrrSynchUp (MtrrValues);
  }
//[-end-120924-IB06460446-modify]//

  InitializeFeatures (ExchangeInfo->S3BootScriptTable);

  InterlockedIncrement (&(ExchangeInfo->FinishedCount));

  ///
  /// Sempahore check loop executed in memory
  ///
  (*ExchangeInfo->SemaphoreCheck)(&ExchangeInfo->FinishedCount);

  InterlockedIncrement (&(ExchangeInfo->WakeupCount));

  ///
  /// Restore the MTRR programmed before OS boot
  ///
  /// MpMtrrSynchUp (MtrrValues);
  ///
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  SetMtrrRegisters (PeiServices, (EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);

  while (ExchangeInfo->WakeupCount < AcpiCpuData->NumberOfCpus - 1) {
    CpuPause ();
  }

  InterlockedIncrement (&(ExchangeInfo->FinishedCount));

}

/**
  Wake up all the application processors

  @param[in] PeiServices   - Indirect reference to the PEI Services Table
  @param[in] PeiStall      - EFI_PEI_STALL_PPI to stall for some interval
  @param[in] AcpiCpuData   - pointer to ACPI_CPU_DATA structure
  @param[in] MtrrValues    - pointer to a buffer which stored MTRR settings

  @retval EFI_SUCCESS - APs are successfully waked up
**/
EFI_STATUS
WakeUpAPs (
  IN EFI_PEI_SERVICES  **PeiServices,
  IN EFI_PEI_STALL_PPI *PeiStall,
  ACPI_CPU_DATA        *AcpiCpuData,
  UINT64               *MtrrValues,
  S3_AP_PROCEDURE      Function
  )
{
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  MP_CPU_S3_DATA_POINTER *CpuS3DataPtr;
  UINTN                  CpuPpiAddrDelta;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPolicyPpi;

  WakeUpBuffer = AcpiCpuData->WakeUpBuffer;
  CopyMem ((VOID *) (UINTN) WakeUpBuffer, AsmGetAddressMap (), 0x1000 - 0x200);

  ExchangeInfo                        = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));

  ExchangeInfo->Lock                  = 0;
  ExchangeInfo->StackStart            = (UINTN) AcpiCpuData->StackAddress;
  ExchangeInfo->StackSize             = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction            = (UINT32) Function;
  ExchangeInfo->BufferStart           = (UINT32) WakeUpBuffer;
  ExchangeInfo->PmodeOffset           = (UINT32) (AsmGetPmodeOffset ());
  ExchangeInfo->SemaphoreCheck        = (VOID (*)(UINT32 *))(AsmGetSemaphoreCheckOffset () + (UINT32) WakeUpBuffer);
  ExchangeInfo->AcpiCpuDataAddress    = (UINT32) AcpiCpuData;
  ExchangeInfo->MtrrValuesAddress     = (UINT32) MtrrValues;
  ExchangeInfo->FinishedCount         = (UINT32) 0;
  ExchangeInfo->SerializeLock         = (UINT32) 0;
  ExchangeInfo->MicrocodePointer      = (UINT32) (UINTN) AcpiCpuData->MicrocodePointerBuffer;
  ExchangeInfo->StartState            = (UINT32) 0;

  CpuS3DataPtr                        = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  ExchangeInfo->S3BootScriptTable     = (MP_CPU_S3_SCRIPT_DATA *) (UINTN) CpuS3DataPtr->S3BootScriptTable;
  ExchangeInfo->VirtualWireMode       = CpuS3DataPtr->VirtualWireMode;
  ExchangeInfo->PeiServices           = PeiServices;
  ExchangeInfo->PeiStall              = PeiStall;
  ExchangeInfo->CpuPerfCtrlValue      = AsmReadMsr64 (MSR_IA32_PERF_CTRL);

  ExchangeInfo->CpuPlatformPolicyPpi  = NULL;
  PeiServicesLocatePpi (
          &gPeiCpuPlatformPolicyPpiGuid,
          0,
          NULL,
          (VOID **) &(ExchangeInfo->CpuPlatformPolicyPpi)
          );

  //
  // Calculate delta from cache address to memory address
  //  
  CpuPolicyPpi = ExchangeInfo->CpuPlatformPolicyPpi;
  CpuPpiAddrDelta = CpuPolicyPpi->CpuPlatformPpiPtr - (UINTN)(CpuPolicyPpi);                                               
  //
  // Calculate new address in memory for each pointer in PEI_CPU_PLATFORM_POLICY_PPI
  //  
  CpuPolicyPpi->CpuConfig                       = (CPU_CONFIG_PPI *)((UINTN)CpuPolicyPpi->CpuConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->PowerMgmtConfig                 = (POWER_MGMT_CONFIG_PPI *)((UINTN)CpuPolicyPpi->PowerMgmtConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->SecurityConfig                  = (SECURITY_CONFIG_PPI *)((UINTN)CpuPolicyPpi->SecurityConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->SecurityConfig->PfatConfig      = (PFAT_CONFIG *)((UINTN)CpuPolicyPpi->SecurityConfig->PfatConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->SecurityConfig->TxtConfig       = (TXT_CONFIG *)((UINTN)CpuPolicyPpi->SecurityConfig->TxtConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->SecurityConfig->BootGuardConfig = (BOOT_GUARD_CONFIG *)((UINTN)CpuPolicyPpi->SecurityConfig->BootGuardConfig - CpuPpiAddrDelta);
  CpuPolicyPpi->OverclockingConfig              = (OVERCLOCKING_CONFIG_PPI *)((UINTN)CpuPolicyPpi->OverclockingConfig - CpuPpiAddrDelta);
  
  CopyMem (
          (VOID *) (UINTN) &ExchangeInfo->GdtrProfile,
          (VOID *) (UINTN) AcpiCpuData->GdtrProfile,
          sizeof (PSEUDO_DESCRIPTOR)
          );
  CopyMem (
          (VOID *) (UINTN) &ExchangeInfo->IdtrProfile,
          (VOID *) (UINTN) AcpiCpuData->IdtrProfile,
          sizeof (PSEUDO_DESCRIPTOR)
          );

  DEBUG ((EFI_D_INFO, "Cpu S3 Bootscript at %08X\n", (UINT32) ExchangeInfo->S3BootScriptTable));

#if defined (BOOT_GUARD_SUPPORT_FLAG) && (BOOT_GUARD_SUPPORT_FLAG == 1)
  //
  // Disable PBET before send IPI to APs
  //
  StopPbeTimer ();
#endif

  ///
  /// Don't touch MPCPU private data
  /// Here we use ExchangeInfo instead
  ///
  ///
  /// Send INIT IPI - SIPI to all APs
  ///
  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          0,
          DELIVERY_MODE_INIT,
          TRIGGER_MODE_EDGE,
          TRUE,
          PeiServices,
          PeiStall
          );

  PeiStall->Stall (
          (CONST EFI_PEI_SERVICES **)PeiServices,
          PeiStall,
//[-start-130802-IB05160477-modify]//
          3 * STALL_ONE_MILLI_SECOND ///< 10ms
//[-end-130802-IB05160477-modify]//
          );

  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          (UINT32) RShiftU64 (WakeUpBuffer,
                              12),
          DELIVERY_MODE_SIPI,
          TRIGGER_MODE_EDGE,
          TRUE,
          PeiServices,
          PeiStall
          );

  PeiStall->Stall (
          (CONST EFI_PEI_SERVICES **)PeiServices,
          PeiStall,
          200 * STALL_ONE_MICRO_SECOND ///< 200us
          );

  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          (UINT32) RShiftU64 (WakeUpBuffer,
                              12),
          DELIVERY_MODE_SIPI,
          TRIGGER_MODE_EDGE,
          TRUE,
          PeiServices,
          PeiStall
          );

  PeiStall->Stall (
          (CONST EFI_PEI_SERVICES **)PeiServices,
          PeiStall,
          200 * STALL_ONE_MICRO_SECOND ///< 200us
          );

  return EFI_SUCCESS;
}

/**
  This routine is used to search SMRAM and get SmramCpuData point.

  @param[in] PeiServices  - PEI services global pointer
  @param[in] SmmAccessPpi - SmmAccess PPI instance

  @retval SmramCpuData - The pointer of CPU information in SMRAM.
**/
STATIC
SMRAM_CPU_DATA *
GetSmmCpuData (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN PEI_SMM_ACCESS_PPI *SmmAccessPpi
  )
{
  EFI_SMRAM_DESCRIPTOR *SmramRanges;
  UINTN                SmramRangeCount;
  UINTN                Size;
  EFI_STATUS           Status;
  UINT32               Address;
  SMRAM_CPU_DATA       *SmramCpuData;

        ///
        /// Get all SMRAM range
        ///
  Size    = 0;
  Status  = SmmAccessPpi->GetCapabilities (PeiServices, SmmAccessPpi, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  Status = PeiServicesAllocatePool (
                        Size,
                        (VOID **) &SmramRanges
                        );
  ASSERT_EFI_ERROR (Status);

  Status = SmmAccessPpi->GetCapabilities (PeiServices, SmmAccessPpi, &Size, SmramRanges);
  ASSERT_EFI_ERROR (Status);

  Size /= sizeof (*SmramRanges);
  SmramRangeCount = Size;

        ///
        ///  BUGBUG: We assume TSEG is the last range of SMRAM in SmramRanges
        ///
  SmramRanges += SmramRangeCount - 1;

  DEBUG ((EFI_D_INFO, "TsegBase - %x\n", SmramRanges->CpuStart));
  DEBUG ((EFI_D_INFO, "TsegTop  - %x\n", SmramRanges->CpuStart + SmramRanges->PhysicalSize));

        ///
        /// Search SMRAM on page alignment for the SMMNVS signature
        ///
        for (Address = (UINT32) (SmramRanges->CpuStart + SmramRanges->PhysicalSize - EFI_PAGE_SIZE);
             Address >= (UINT32) SmramRanges->CpuStart;
             Address -= EFI_PAGE_SIZE
             ) {
                SmramCpuData = (SMRAM_CPU_DATA *) (UINTN) Address;
                if (CompareGuid (&SmramCpuData->HeaderGuid, &gSmramCpuDataHeaderGuid)) {
                        ///
                        /// Find it
                        ///
                        return SmramCpuData;
    }
  }

  ASSERT (FALSE);

        return NULL;
}

/**
  This routine is restore the CPU information from SMRAM to original reserved memory region.

  @param[in] PeiServices  - PEI services global pointer

  @retval AcpiCpuData - The pointer of CPU information in reserved memory.
**/
ACPI_CPU_DATA *
RestoreSmramCpuData (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  PEI_SMM_ACCESS_PPI       *SmmAccessPpi;
  SMRAM_CPU_DATA           *SmramCpuData;
  EFI_STATUS               Status;
  ACPI_CPU_DATA            *AcpiCpuData;
  MP_CPU_S3_DATA_POINTER   *CpuS3DataPtr;
  PSEUDO_DESCRIPTOR        *Idtr;
  PSEUDO_DESCRIPTOR        *Gdtr;
  UINTN                    MicrocodeSize;
  EFI_CPU_MICROCODE_HEADER **Microcode;
  EFI_CPU_MICROCODE_HEADER *LockBoxMicrocode;
  UINTN                    Index;

  Status = PeiServicesLocatePpi (
                        &gPeiSmmAccessPpiGuid,
                        0,
                        NULL,
                        (VOID **) &SmmAccessPpi
                        );
  ASSERT_EFI_ERROR (Status);

        ///
        /// Open all SMM regions
        ///
  Index = 0;
        do {
                Status = SmmAccessPpi->Open (PeiServices, SmmAccessPpi, Index);
                Index++;
  } while (!EFI_ERROR (Status));

  SmramCpuData = GetSmmCpuData (PeiServices, SmmAccessPpi);
  DEBUG ((EFI_D_INFO, "CpuS3 SmramCpuData                         - 0x%x \n", SmramCpuData));
  DEBUG ((EFI_D_INFO, "SmramCpuData->GdtrProfileSize              - %x\n", SmramCpuData->GdtrProfileSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->GdtSize                      - %x\n", SmramCpuData->GdtSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->IdtrProfileSize              - %x\n", SmramCpuData->IdtrProfileSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->IdtSize                      - %x\n", SmramCpuData->IdtSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->CpuPrivateDataSize           - %x\n", SmramCpuData->CpuPrivateDataSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->S3BootScriptTableSize        - %x\n", SmramCpuData->S3BootScriptTableSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->S3BspMtrrTableSize           - %x\n", SmramCpuData->S3BspMtrrTableSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->MicrocodePointerBufferSize   - %x\n", SmramCpuData->MicrocodePointerBufferSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->MicrocodeDataBufferSize      - %x\n", SmramCpuData->MicrocodeDataBufferSize));
  DEBUG ((EFI_D_INFO, "SmramCpuData->GdtrProfileOffset            - %x\n", SmramCpuData->GdtrProfileOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->GdtOffset                    - %x\n", SmramCpuData->GdtOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->IdtrProfileOffset            - %x\n", SmramCpuData->IdtrProfileOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->IdtOffset                    - %x\n", SmramCpuData->IdtOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->CpuPrivateDataOffset         - %x\n", SmramCpuData->CpuPrivateDataOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->S3BootScriptTableOffset      - %x\n", SmramCpuData->S3BootScriptTableOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->S3BspMtrrTableOffset         - %x\n", SmramCpuData->S3BspMtrrTableOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->MicrocodePointerBufferOffset - %x\n", SmramCpuData->MicrocodePointerBufferOffset));
  DEBUG ((EFI_D_INFO, "SmramCpuData->MicrocodeDataBufferOffset    - %x\n", SmramCpuData->MicrocodeDataBufferOffset));

        ///
        /// Start restore data to NVS
        ///
  AcpiCpuData = (ACPI_CPU_DATA *) (UINTN) SmramCpuData->AcpiCpuPointer;
  CopyMem (AcpiCpuData, &SmramCpuData->AcpiCpuData, sizeof (ACPI_CPU_DATA));

  CopyMem (
                (VOID *) (UINTN) AcpiCpuData->GdtrProfile,
                (UINT8 *) SmramCpuData + SmramCpuData->GdtrProfileOffset,
                SmramCpuData->GdtrProfileSize
                );
  Gdtr = (PSEUDO_DESCRIPTOR *) (UINTN) AcpiCpuData->GdtrProfile;
  CopyMem (
                (VOID *) (UINTN) Gdtr->Base,
                (UINT8 *) SmramCpuData + SmramCpuData->GdtOffset,
                SmramCpuData->GdtSize
                );
  CopyMem (
                (VOID *) (UINTN) AcpiCpuData->IdtrProfile,
                (UINT8 *) SmramCpuData + SmramCpuData->IdtrProfileOffset,
                SmramCpuData->IdtrProfileSize
                );
  Idtr = (PSEUDO_DESCRIPTOR *) (UINTN) AcpiCpuData->IdtrProfile;
  CopyMem (
                (VOID *) (UINTN) Idtr->Base,
                (UINT8 *) SmramCpuData + SmramCpuData->IdtOffset,
                SmramCpuData->IdtSize
                );

  CopyMem (
                (VOID *) (UINTN) AcpiCpuData->CpuPrivateData,
                (UINT8 *) SmramCpuData + SmramCpuData->CpuPrivateDataOffset,
                SmramCpuData->CpuPrivateDataSize
                );
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  CopyMem (
                (VOID *) (UINTN) CpuS3DataPtr->S3BootScriptTable,
                (UINT8 *) SmramCpuData + SmramCpuData->S3BootScriptTableOffset,
                SmramCpuData->S3BootScriptTableSize
                );
  CopyMem (
                (VOID *) (UINTN) CpuS3DataPtr->S3BspMtrrTable,
                (UINT8 *) SmramCpuData + SmramCpuData->S3BspMtrrTableOffset,
                SmramCpuData->S3BspMtrrTableSize
                );

  CopyMem (
                (VOID *) (UINTN) AcpiCpuData->MicrocodePointerBuffer,
                (UINT8 *) SmramCpuData + SmramCpuData->MicrocodePointerBufferOffset,
                SmramCpuData->MicrocodePointerBufferSize
                );
        ///
        /// Restore Microcode Data
        ///
  Microcode         = (VOID *) (UINTN) AcpiCpuData->MicrocodePointerBuffer;
  LockBoxMicrocode  = (EFI_CPU_MICROCODE_HEADER *) ((UINT8 *) SmramCpuData + SmramCpuData->MicrocodeDataBufferOffset);
        if (Microcode != NULL) {
                Index         = 0;
                MicrocodeSize = 0;
                while (Microcode[Index] != NULL) {
                        if (LockBoxMicrocode->DataSize == 0) {
                                MicrocodeSize = 2048;
      } else {
                                MicrocodeSize = LockBoxMicrocode->TotalSize;
      }

                        CopyMem (Microcode[Index], LockBoxMicrocode, MicrocodeSize);
                        LockBoxMicrocode = (EFI_CPU_MICROCODE_HEADER *) ((UINT8 *) LockBoxMicrocode + MicrocodeSize);
                        Index++;
    }
  }
        ///
        /// Close all SMM regions
        ///
  Index = 0;
        do {
                Status = SmmAccessPpi->Close (PeiServices, SmmAccessPpi, Index);
                Index++;
  } while (!EFI_ERROR (Status));

        return AcpiCpuData;
}

/**
  Initialize multiple processors.

  @param[in] FfsHeader     - Pointer to an alleged FFS file.
  @param[in] PeiServices   - Indirect reference to the PEI Services Table

  @retval EFI_SUCCESS - Multiple processors are intialized successfully
**/
EFI_STATUS
InitializeCpu (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS             Status;
  EFI_PEI_STALL_PPI      *PeiStall;
  ACPI_CPU_DATA          *AcpiCpuData;
  EFI_BOOT_MODE          BootMode;
  UINT64                 *MtrrValues;
  MP_CPU_S3_DATA_POINTER *CpuS3DataPtr;
  UINTN                  VariableMtrrNumber;
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  UINT32                 FailedRevision;
  VOID                   *Hob;

  DEBUG ((EFI_D_INFO, "Cpu S3 dispatch\n"));

  Status = PeiServicesGetBootMode (&BootMode);
        if (EFI_ERROR (Status)) {
                ///
                /// If not in S3 boot path. do nothing
                ///
                return EFI_SUCCESS;
  }

        if (BootMode != BOOT_ON_S3_RESUME) {
                return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "Cpu S3 Entrypoint\n"));

        ///
        /// Restore ACPI Nvs data from SMRAM
        ///
  AcpiCpuData = RestoreSmramCpuData ((EFI_PEI_SERVICES **)PeiServices);
  DEBUG ((EFI_D_INFO, "CpuS3 RestoreSmramCpuData - 0x%x \n", AcpiCpuData));

  AcpiCpuData->S3BootPath = TRUE;

  Status = PeiServicesLocatePpi (
                        &gEfiPeiStallPpiGuid,
                        0,
                        NULL,
                        (VOID **) &PeiStall
                        );
  ASSERT_EFI_ERROR (Status);

  CpuS3DataPtr        = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;

  VariableMtrrNumber  = (UINTN) ((UINT64) AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT) * 2;
  Status = PeiServicesAllocatePool (
                        (FixedMtrrNumber + MtrrDefTypeNumber + VariableMtrrNumber) * sizeof (UINT64),
                        (VOID **) &MtrrValues
                        );
  ASSERT_EFI_ERROR (Status);
  ReadMtrrRegisters ((EFI_PEI_SERVICES **)PeiServices, MtrrValues);

  WakeUpBuffer                = AcpiCpuData->WakeUpBuffer;
  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));
  ExchangeInfo->WakeupCount   = (UINT32) 0;
  ExchangeInfo->FinishedCount = (UINT32) 0;

        ///
        /// Restore AP configuration
        ///
  WakeUpAPs ((EFI_PEI_SERVICES **)PeiServices, PeiStall, AcpiCpuData, MtrrValues, MPRendezvousProcedure);

        ///
        /// Program XApic register
        ///
  ProgramXApic (
                TRUE
                );

  InitializeMicrocode (
                ExchangeInfo,
                (EFI_CPU_MICROCODE_HEADER **) (UINTN) AcpiCpuData->MicrocodePointerBuffer,
                &FailedRevision
                );

        ///
        /// Restore features for BSP
        ///
  InitializeFeaturesLog (
    (EFI_PEI_SERVICES **)PeiServices,
                (MP_CPU_S3_SCRIPT_DATA *) CpuS3DataPtr->S3BootScriptTable
                );

        ///
        /// Save acpi cpu data into one hob, it will be used by a callback when End of Pei Signal installed.
        ///
  Hob = BuildGuidDataHob (
                        &gPeiAcpiCpuDataGuid,
                        (VOID *)(UINTN)AcpiCpuData,
                        (UINTN) sizeof (ACPI_CPU_DATA)
                        );
  ASSERT (Hob != NULL);

  DEBUG ((EFI_D_INFO, "Install CPU S3 Notify callback\n"));
  Status = PeiServicesNotifyPpi (&mCpuS3ResumeNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Wait for all APs to complete
  ///
  while (ExchangeInfo->FinishedCount < AcpiCpuData->NumberOfCpus - 1) {
          CpuPause ();
  }

  //[-start-130426-IB10930028-add]//
  //
  //  Enable SMI for S3 scipt table
  //
  EnableSMI ();  
  //[-end-130426-IB10930028-add]//

  return EFI_SUCCESS;
  
}

/**
  This function handles CPU S3 resume task

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
STATIC
EFI_STATUS
CpuS3ResumeAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{

  EFI_STATUS             Status;
  EFI_PEI_STALL_PPI      *PeiStall;
  ACPI_CPU_DATA          *AcpiCpuData;
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_S3_DATA_POINTER *CpuS3DataPtr;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  VOID                   *Hob;

  DEBUG ((EFI_D_INFO, "Cpu S3 callback Entry\n"));

        ///
        /// Find the saved acpi cpu data from HOB.
        ///
  AcpiCpuData = NULL;
  Hob = GetFirstGuidHob (&gPeiAcpiCpuDataGuid);
        if (Hob != NULL) {
                AcpiCpuData = (ACPI_CPU_DATA *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));
                ASSERT (AcpiCpuData != NULL);
                if (AcpiCpuData == NULL) {
                        return EFI_UNSUPPORTED;
    }
  } else {
                return EFI_UNSUPPORTED;
  }

  WakeUpBuffer                = AcpiCpuData->WakeUpBuffer;
  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));
        ///
        /// Have APs to continue the task - Restore S3BspMtrrTable
        ///
  ExchangeInfo->WakeupCount   = (UINT32) 0;
  ExchangeInfo->FinishedCount = (UINT32) 0;

  PeiStall = NULL;
  Status = PeiServicesLocatePpi (
                        &gEfiPeiStallPpiGuid,
                        0,
                        NULL,
                        (VOID **) &PeiStall
                        );
  ASSERT_EFI_ERROR (Status);

        ///
        /// Set MTRR to the final values
        /// Do not do it too early so as to avoid performance penalty
        ///
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;

#ifdef EFI_DEBUG
  ShowMtrrRegisters (PeiServices, (EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);
#endif
  SetMtrrRegisters (PeiServices, (EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);

  PeiStall->Stall (
                (CONST EFI_PEI_SERVICES **)PeiServices,
                PeiStall,
                1 * STALL_ONE_MILLI_SECOND ///< 1ms
                );

        while (ExchangeInfo->FinishedCount < AcpiCpuData->NumberOfCpus - 1) {
                CpuPause ();
  }

        return EFI_SUCCESS;

}
