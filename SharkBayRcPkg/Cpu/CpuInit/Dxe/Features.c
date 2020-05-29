/** @file
  CPU feature control module

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "CpuInitDxe.h"
#include "Features.h"
#include "MachineCheck.h"
#include "MpCommon.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <Protocol/CpuInfo.h>

extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;

UINT8 mFeatureLock;
UINTN mCommonFeatures;
UINTN mSetupFeatures;

static UINT8              mLock;
static LEAST_FEATURE_PROC mLeastFeatureProcessor;

/**
  Write 64bits MSR with script

  @param[in] Index - MSR index that will be written
  @param[in] Value - value written to MSR
**/
VOID
AsmWriteMsr64WithScript (
  IN UINT32 Index,
  IN UINT64 Value
  )
{
  AsmWriteMsr64 (Index, Value);
  WriteMsr64ToScript (Index, Value);
}

/**
  Write 64bits MSR to script

  @param[in] Index - MSR index that will be written
  @param[in] Value - value written to MSR
**/
VOID
WriteMsr64ToScript (
  IN UINT32 Index,
  IN UINT64 Value
  )
{
  UINTN TableIndex;

  ASSERT (mMPSystemData != NULL);

  ///
  /// Save it into script
  ///
  AsmAcquireMPLock (&(mMPSystemData->S3BootScriptLock));
  TableIndex = mMPSystemData->S3BootScriptCount++;
  AsmReleaseMPLock (&(mMPSystemData->S3BootScriptLock));

  ASSERT (TableIndex < MAX_CPU_S3_TABLE_SIZE - 1);
  mMPSystemData->S3BootScriptTable[TableIndex].ApicId   = GetApicID (NULL, NULL);
  mMPSystemData->S3BootScriptTable[TableIndex].MsrIndex = Index;
  mMPSystemData->S3BootScriptTable[TableIndex].MsrValue = Value;
}

/**
  Provide access to the CPU misc enables MSR

  @param[in] Enable  - Enable or Disable Misc Features
  @param[in] BitMask - The register bit offset of MSR MSR_IA32_MISC_ENABLE
**/
VOID
CpuMiscEnable (
  BOOLEAN Enable,
  UINT64  BitMask
  )
{
  UINT64 MsrValue;

  MsrValue = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  if (Enable) {
    MsrValue |= BitMask;
  } else {
    MsrValue &= ~BitMask;
  }

  AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, MsrValue);
}

/**
  Calculate how many bits are one from given number

  @param[in] Value - number that will be calculated bits

  @retval Number of bits
**/
UINT32
GetBitsNumberOfOne (
  UINT32 Value
  )
{
  UINT32 Result;

  Result = 0;
  while (Value) {
    if (Value & 1) {
      Result++;
    }
    Value >>= 1;
  }
  return Result;
}

///
/// DCA contains processor code and chipset code
/// CPU driver has the following assumption on the initialization flow
/// 1. Chipset pre-initialization should detect DCA support per chipset capability after CpuPlatformPolicy
/// 2. If not support, it should update CpuPlatformPolicy DCA to disable state
/// 3. If support, it should enable the DCA related registers
/// 4. CPU initialization for DCA (CPU may change CpuPlatformPolicy DCA states per CPU capability)
/// 5. Normal chipset driver (IOH) should look at CpuPlatformPolicy DCA policy again in PCI enumeratoin
/// 6. Chipset enable or disable DCA according to CpuPlatformPolicy DCA state
///
/**
  Detect DCA supported or not

  @retval DCA_SUPPORT if supported or 0 if not supported
**/
UINTN
IsDcaSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  BOOLEAN            Support;

  Support = 0;
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if ((CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_DCA) != 0) {
    ///
    /// Execute Disable Bit feature is not supported on this processor.
    ///
    Support = DCA_SUPPORT;
  }
  return Support;
}

/**
  Detect HT supported or not

  @retval HT_SUPPORT if supported or 0 if not supported
**/
UINTN
IsHTSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  UINTN              Support;

  Support = 0;

  AsmCpuidEx (
          CPUID_CORE_TOPOLOGY,
          0,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if ((CpuidRegisters.RegEbx & 0x00FF) == 2) {
    Support = HT_SUPPORT;
  }
  return Support;

}

/**
  Detect if AES supported or not

  @retval AES_SUPPORT if supported or 0 if not supported
**/
UINTN
IsAesSupported (
  VOID
  )
{

  EFI_CPUID_REGISTER CpuidRegisters;
  UINTN              Support;

  Support = 0;

  AsmCpuid (
    CPUID_VERSION_INFO, 
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  
  if ((CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_AES) != 0) {
    Support = AES_SUPPORT;
  }
  return Support;
}

/**
  Detect if XD supported or not

  @retval XD_SUPPORT if supported or 0 if not supported
**/
UINTN
IsXdSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  BOOLEAN            Support;

  Support = 0;
  AsmCpuid (
          CPUID_EXTENDED_FUNCTION,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if (CpuidRegisters.RegEax > CPUID_EXTENDED_FUNCTION) {
    AsmCpuid (
            CPUID_EXTENDED_CPU_SIG,
            &CpuidRegisters.RegEax,
            &CpuidRegisters.RegEbx,
            &CpuidRegisters.RegEcx,
            &CpuidRegisters.RegEdx
            );
    if ((CpuidRegisters.RegEdx & B_CPUID_VERSION_INFO_EDX_XD) != 0) {
      ///
      /// Execute Disable Bit feature is not supported on this processor.
      ///
      Support = XD_SUPPORT;
    }
  }
  return Support;
}

/**
  Enable XD if supported or disable it if not supported

  @param[in] Support  - bitmap that indicate XD supported or not
**/
VOID
EnableDisableXd (
  IN UINTN Support
  )
{
  BOOLEAN XdSupport;

  if ((mCommonFeatures & XD_SUPPORT) == 0) {
    return;
  }
  XdSupport = (BOOLEAN)((Support & XD_SUPPORT) == XD_SUPPORT);
  ///
  /// MSR MISC_ENABLE[34] has negative logic: 0 - XD Enabled, 1 - XD Disabled
  ///
  CpuMiscEnable (!XdSupport, B_MSR_IA32_MISC_ENABLE_XD);
}

/**
  Check on the processor if VMX/TXT is supported.

  @retval VMX_SUPPORT and TXT_SUPPORT if supported or 0 if not supported
**/
UINTN
IsVmxSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuIdRegister;
  UINTN              Support;

  Support = 0;

  ///
  /// Get CPUID to check if the processor supports Vanderpool Technology.
  ///
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuIdRegister.RegEax,
          &CpuIdRegister.RegEbx,
          &CpuIdRegister.RegEcx,
          &CpuIdRegister.RegEdx
          );
  if ((CpuIdRegister.RegEcx & B_CPUID_VERSION_INFO_ECX_VME) != 0) {
    ///
    /// VT is supported.
    ///
    Support |= VMX_SUPPORT;
  }
  if ((CpuIdRegister.RegEcx & B_CPUID_VERSION_INFO_ECX_SME) != 0) {
    ///
    /// TXT is supported.
    ///
    Support |= TXT_SUPPORT;
  }
  return Support;
}

/**
  Enable VMX/TXT on the processor.

  @param[in] Support  - To enable or disable VMX/TXT feature.
**/
VOID
EnableDisableVmx (
  IN UINTN Support
  )
{
  UINT64 Ia32FeatCtrl;
  UINT64 NewFeatCtrl;

  Ia32FeatCtrl = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  Ia32FeatCtrl &= ~((UINT64) OPTION_FEATURE_RESERVED_MASK);

  NewFeatCtrl = Ia32FeatCtrl;

  ///
  /// If Vmx is Disabled, Enable it.
  ///
  if (mCommonFeatures & VMX_SUPPORT) {
    if (Support & VMX_SUPPORT) {
      NewFeatCtrl |= B_MSR_IA32_FEATURE_CONTROL_EVT;
    } else {
      NewFeatCtrl &= ~B_MSR_IA32_FEATURE_CONTROL_EVT;
    }
  }
  if (mCommonFeatures & TXT_SUPPORT) {
    if (Support & TXT_SUPPORT) {
      ///
      /// MSR Lock will be done in later.
      ///
      NewFeatCtrl |= (B_MSR_IA32_FEATURE_CONTROL_SLFE | B_MSR_IA32_FEATURE_CONTROL_SGE);
      if (mCommonFeatures & VMX_SUPPORT) {
        ///
        /// Bit [1] can only be set if CPU is both VMX and TXT capable
        ///
        NewFeatCtrl |= B_MSR_IA32_FEATURE_CONTROL_ELT;
      }
    } else {
      NewFeatCtrl &= ~(B_MSR_IA32_FEATURE_CONTROL_ELT | B_MSR_IA32_FEATURE_CONTROL_SLFE | B_MSR_IA32_FEATURE_CONTROL_SGE);
    }
  }

  ///
  /// Check the Feature Lock Bit.
  /// If it is already set, which indicates we are executing POST
  /// due to a warm RESET (i.e., PWRGOOD was not de-asserted).
  ///
  if ((Ia32FeatCtrl & B_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
    AsmWriteMsr64WithScript (MSR_IA32_FEATURE_CONTROL, NewFeatCtrl);
  } else {
    ///
    /// if Lock bit is set
    ///
    NewFeatCtrl &= ~(B_MSR_IA32_FEATURE_CONTROL_LOCK);
    WriteMsr64ToScript (MSR_IA32_FEATURE_CONTROL, NewFeatCtrl);
  }
}

/**
  Enable / Disable AES on the processor.

  @param[in] Support  - To enable or disable AES feature.
**/
VOID
EnableDisableAes (
  IN UINTN Support
  )
{
  UINT64 MsrValue;

  if (!(mCommonFeatures & AES_SUPPORT) || (IsSecondaryThread ())) {
    return;
  }

  ///
  /// The processor was manufacted with AES-NI feature
  ///
  MsrValue = AsmReadMsr64 (MSR_IA32_FEATURE_CONFIG);

  ///
  /// Check the Feature Lock Bit.
  /// If it is already set, which indicates we are executing POST
  /// due to a warm RESET (i.e., PWRGOOD was not de-asserted).
  ///
  if ((MsrValue & B_IA32_FEATURE_CONFIG_LOCK) == 0) {
    if (Support & AES_SUPPORT) {
      ///
      /// Enabled AES, writes of 00b, 01b pr 10b to the MSR will result in AES enable.
      /// Should lock this MSR always, so write 01b to the MSR.
      ///
      MsrValue &= ~B_IA32_FEATURE_CONFIG_AES_DIS;
      MsrValue |= B_IA32_FEATURE_CONFIG_LOCK;
    } else {
      ///
      /// To disable AES, system BIOS must write 11b to this MSR.
      ///
      MsrValue |= (B_IA32_FEATURE_CONFIG_AES_DIS + B_IA32_FEATURE_CONFIG_LOCK);
    }
    AsmWriteMsr64WithScript (MSR_IA32_FEATURE_CONFIG, MsrValue);
  }
  return;
}

/**
  Check on the processor if Debug Interface is supported.

  @retval DEBUG_SUPPORT and DEBUG_LOCK_SUPPORT if supported or 0 if not supported
**/
UINTN
IsDebugInterfaceSupported (
  VOID
  )
{
  UINTN              Support;
  EFI_CPUID_REGISTER CpuIdRegister;

  Support = 0;

  ///
  /// Access to MSR_IA32_DEBUG_INTERFACE is supported on:
  /// HSW B0, HSWULT B0 and CRW B0 Stepping
  /// HSW stepping >= C0, HSWULT Stepping >= C0 and CRW stepping >= C0 stepping, if CPUID (EAX=1): ECX[11] = 1
  ///
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuIdRegister.RegEax,
          &CpuIdRegister.RegEbx,
          &CpuIdRegister.RegEcx,
          &CpuIdRegister.RegEdx
          );
  switch (CpuIdRegister.RegEax) {
    case (EnumCpuHsw + EnumHswA0):
      DEBUG ((EFI_D_INFO,"MSR_IA32_DEBUG_INTERFACE is not supported on Ax CPU stepping\n"));
      break;
    case (EnumCpuHsw    + EnumHswB0):
    case (EnumCpuHswUlt + EnumHswUltB0):
    case (EnumCpuCrw    + EnumCrwB0):
      Support |= DEBUG_SUPPORT;
      Support |= DEBUG_LOCK_SUPPORT;
      break;
    default:
      if (CpuIdRegister.RegEcx & BIT11) {
        Support |= DEBUG_SUPPORT;
        Support |= DEBUG_LOCK_SUPPORT;
      }
      break;
  }
  return Support;
}

/**
  Enable/Disable Debug Interfaces in the processor.

  @param[in] Support  - To enable or disable Debug Interface feature.
**/
VOID
EnableDisableDebugInterface (
  IN UINTN Support
  )
{
  UINT64  Ia32DebugInterface;
  BOOLEAN IsBsp;

  ///
  /// IA32_DEBUG_INTERFACE_MSR scope is "Package", program on BSP only
  ///
  IsBsp = (AsmReadMsr64 (MSR_IA32_APIC_BASE) & B_MSR_IA32_APIC_BASE_BSP) ? TRUE : FALSE;
  if (!(mCommonFeatures & DEBUG_SUPPORT) || (IsBsp == FALSE)) {
    return;
  }

  ///
  /// Check if the processor supports debug interface
  ///
  if (IsDebugInterfaceSupported()) {
    Ia32DebugInterface = AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE);
    if (!(Ia32DebugInterface &  B_DEBUG_INTERFACE_LOCK)) {
      if (Support & DEBUG_SUPPORT) {
        ///
        /// Enable Debug Interface (MSR 0xC80.Bit0 = 1)
        ///
        Ia32DebugInterface |= B_DEBUG_INTERFACE_ENABLE;
        DEBUG ((EFI_D_ERROR, "Enable MSR_IA32_DEBUG_INTERFACE\n"));
      } else {
        ///
        /// Disable Debug Interface (MSR 0xC80.Bit0 = 0)
        ///
        Ia32DebugInterface &= ~B_DEBUG_INTERFACE_ENABLE;
        DEBUG ((EFI_D_ERROR, "Disable MSR_IA32_DEBUG_INTERFACE\n"));
      }
      if (Support & DEBUG_LOCK_SUPPORT) {
        Ia32DebugInterface &= ~B_DEBUG_INTERFACE_LOCK;
      }
      DEBUG ((EFI_D_ERROR, "Set MSR_IA32_DEBUG_INTERFACE to %x\n",Ia32DebugInterface));
      AsmWriteMsr64WithScript (MSR_IA32_DEBUG_INTERFACE, Ia32DebugInterface);
    }
  }
  return;
}

/**
  Lock VMX/TXT feature bits on the processor.
  Set "CFG Lock" (MSR 0E2h Bit[15]

  @param[in] LockFeatureEnable - TRUE to lock these feature bits and FALSE to not lock
**/
VOID
LockFeatureBit (
  IN BOOLEAN LockFeatureEnable
  )
{
  UINT64 Ia32FeatCtrl;
  UINT32 MsrValue;

  if (!LockFeatureEnable) {
    return;
  }

  ///
  /// MSR 3Ah for VMX/TXT Lock
  ///
  Ia32FeatCtrl = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  Ia32FeatCtrl &= ~((UINT64) OPTION_FEATURE_RESERVED_MASK);

  if ((Ia32FeatCtrl & B_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
    ///
    /// Set Feature Lock bits.
    ///
    Ia32FeatCtrl |= B_MSR_IA32_FEATURE_CONTROL_LOCK;
    AsmWriteMsr64WithScript (MSR_IA32_FEATURE_CONTROL, Ia32FeatCtrl);
  } else {
    WriteMsr64ToScript (MSR_IA32_FEATURE_CONTROL, Ia32FeatCtrl);
  }

  MsrValue = GetCsrDesiredCores ();
  if ((MsrValue & BIT16) == 0) {
    ///
    /// Set Lock
    ///
    SetLockCsrDesiredCores ();
  }
  return;
}

/**
  Detect if X2APIC supported or not

  @retval XAPIC_SUPPORT if supported or 0 if not supported
**/
UINTN
IsXapicSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;
  UINTN              Support;
  UINT64             MsrValue;

  Support   = 0;

  MsrValue  = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if ((CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_XAPIC) != 0) {
    if (MsrValue & B_MSR_IA32_APIC_BASE_G_XAPIC) {
      ///
      /// XAPIC Mode feature is supported on this processor.
      ///
      Support = XAPIC_SUPPORT;
    }
  }
  return Support;
}

/**
  Enable / Disable X2APIC on the processor.

  @param[in] Support  - To enable or disable X2APIC feature.
**/
VOID
EnableDisableXAPIC (
  IN UINTN Support
  )
{
  UINT64 MsrValue;

  if (!(mCommonFeatures & XAPIC_SUPPORT)) {
    return;
  }
  MsrValue = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  if (Support & XAPIC_SUPPORT) {
    MsrValue |= B_MSR_IA32_APIC_BASE_M_XAPIC;
  } else {
    MsrValue &= ~B_MSR_IA32_APIC_BASE_G_XAPIC;
    MsrValue &= ~B_MSR_IA32_APIC_BASE_M_XAPIC;
    AsmWriteMsr64 (MSR_IA32_APIC_BASE, MsrValue);
    MsrValue |= B_MSR_IA32_APIC_BASE_G_XAPIC;
  }
  AsmWriteMsr64 (MSR_IA32_APIC_BASE, MsrValue);
}

/**
  Initialize other processor functions (TPR messaging, floating point)
**/
VOID
InitializeMiscProcessorFunctions (
  VOID
  )
{
  UINT64 MsrValue;

  ///
  /// Enable TPR Update messages,if supported (see section 2.5)
  ///
  MsrValue = AsmReadMsr64 (PIC_THREAD_CONTROL);
  MsrValue &= (~B_PIC_THREAD_CONTROL_TPR_DIS);
  AsmWriteMsr64WithScript (PIC_THREAD_CONTROL, MsrValue);

  ///
  /// Enable the Save Floating Point feature on every logical processors in the
  /// platform when available. The BIOS must verify the SMM SAVE CONTROL capability
  /// bit is set to 1 in PLATFORM_INFO MSR CEh [16] (see Section 2.16) before setting
  /// the SMM_SAVE_CONTROL MSR 3Eh [0] to 1.
  ///
  MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
  if ((MsrValue & B_PLATFORM_INFO_SMM_SAVE_CONTROL) != 0) {
    MsrValue = AsmReadMsr64 (MSR_IA32_SMM_SAVE_CONTROL);
    MsrValue |= B_MSR_IA32_SMM_SAVE_CONTROL_SFPPE;
    AsmWriteMsr64WithScript (MSR_IA32_SMM_SAVE_CONTROL, MsrValue);
  }

}

/**
  Create feature control structure which will be used to program each feature on each core.

  @param[in] MPSystemData - MP_SYSTEM_DATA global variable that contains platform policy protocol settings of each features.
**/
VOID
InitializeFeaturePerSetup (
  IN MP_SYSTEM_DATA *MPSystemData
  )
{
  mFeatureLock    = VacantFlag;
  mCommonFeatures = (UINTN) -1;
  mSetupFeatures  = (UINTN) -1;

  if (!MPSystemData->VmxEnable) {
    mSetupFeatures &= ~VMX_SUPPORT;
  }
  if (!MPSystemData->TxtEnable) {
    mSetupFeatures &= ~TXT_SUPPORT;
  }
  if (!MPSystemData->ExecuteDisableBit) {
    mSetupFeatures &= ~XD_SUPPORT;
  }
  if (!MPSystemData->XapicEnable) {
    mSetupFeatures &= ~XAPIC_SUPPORT;
  }
  if (!MPSystemData->AesEnable) {
    mSetupFeatures &= ~AES_SUPPORT;
  }
  if (!MPSystemData->DebugInterfaceEnable) {
    mSetupFeatures &= ~DEBUG_SUPPORT;
  }
  if (!MPSystemData->DebugInterfaceLockEnable) {
    mSetupFeatures &= ~DEBUG_LOCK_SUPPORT;
  }
}

/**
  Detect each processor feature and log all supported features

  @param[in] MpServices  - EFI_MP_SERVICES_PROTOCOL
**/
VOID
CollectProcessorFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  )

{
  UINTN Support;

  Support = 0;
  Support |= IsXdSupported ();
  Support |= IsVmxSupported ();
  Support |= IsDcaSupported ();
  Support |= IsAesSupported ();
  Support |= IsXapicSupported ();
  Support |= IsHTSupported ();
  Support |= IsDebugInterfaceSupported ();

  AsmAcquireMPLock (&mFeatureLock);
  mCommonFeatures &= Support;
  AsmReleaseMPLock (&mFeatureLock);

  return;
}

/**
  Program all processor features basing on desired settings

  @param[in] MpServices  - FRAMEWORK_EFI_MP_SERVICES_PROTOCOL
**/
VOID
ProgramProcessorFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  )
{
  UINTN   Supported;
  BOOLEAN IsBsp;

  Supported = mCommonFeatures & mSetupFeatures;

  ///
  /// Configure features such as Xd, Vmx, Smx, XAPIC, AES, DebugInterface
  ///
  EnableDisableXd (Supported);
  EnableDisableVmx (Supported);
  EnableDisableXAPIC (Supported);
  EnableDisableAes (Supported);
  EnableDisableDebugInterface (Supported);

  ///
  /// Lock Proceesor Features
  /// Make sure all CPU Features configuration are set before lock
  ///
  LockFeatureBit (1);

  ///
  /// Programe XApic register
  ///
  IsBsp = (AsmReadMsr64 (MSR_IA32_APIC_BASE) & B_MSR_IA32_APIC_BASE_BSP) ? TRUE : FALSE;
  ProgramXApic (IsBsp);

  ///
  /// Initialize MonitorMWait register
  ///
  CpuMiscEnable (mMPSystemData->MonitorMwaitEnable, B_MSR_IA32_MISC_ENABLE_MONITOR);

  ///
  /// Initialize Machine Check registers
  ///
  InitializeMachineCheckRegisters (NULL, mMPSystemData->MachineCheckEnable);

  //
  // Misc functions
  //
  InitializeMiscProcessorFunctions();

  return;
}

/**
  Program CPUID Limit before booting to OS

  @param[in] MpServices  - MP Services Protocol entry
**/
VOID
ProgramCpuidLimit (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  )
{
  UINT64 MsrValue;

  ///
  /// Move Limit CPUID Maxval configuration here to not impact the BOOT
  /// After setting this, no code can execute CPUID function > 3.
  ///
  CpuMiscEnable (mMPSystemData->LimitCpuidMaximumValue, B_MSR_IA32_MISC_ENABLE_CPUID_MAX);

  ///
  /// Finally record the MISC MSR into CPU S3 script table
  /// to avoid access for multiple times
  ///
  MsrValue = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  AsmWriteMsr64WithScript (MSR_IA32_MISC_ENABLE, MsrValue);

  return;
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
InitializeProcessorsPrefetcher (
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
  Get processor feature

  @param[in] Features  - pointer to a buffer which stores feature information
**/
VOID
GetProcessorFeatures (
  IN UINT32 *Features
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
  Features[0] = CpuidRegisters.RegEcx;
  Features[1] = CpuidRegisters.RegEdx;
  AsmCpuid (
          CPUID_EXTENDED_CPU_SIG,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  Features[2] = CpuidRegisters.RegEax;
  Features[3] = CpuidRegisters.RegEbx;
  Features[4] = CpuidRegisters.RegEcx;
  Features[5] = CpuidRegisters.RegEdx;

  return;
}

/**
  Update some processor info into LEAST_FEATURE_PROC data structure.

  @param[in] Index                 - indicate which processor calling this routine
  @param[in] LeastFeatureProcessor - the data structure that will be updated
**/
VOID
UpdateProcessorInfo (
  IN UINTN                 Index,
  IN LEAST_FEATURE_PROC    *LeastFeatureProcessor
  )
{
  UINT16             FamilyId;
  UINT8              Model;
  UINT8              SteppingId;
  EFI_CPUID_REGISTER Register;
  UINT8              TempFamilyId;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &Register.RegEax,
    &Register.RegEbx,
    &Register.RegEcx,
    &Register.RegEdx
    );

  SteppingId = (UINT8) (Register.RegEax & 0xF);
  TempFamilyId = (UINT8) ((Register.RegEax >> 8) & 0xF);
  Model = (UINT8) ((Register.RegEax >> 4) & 0xF);
  
  if (TempFamilyId == 0x6 || TempFamilyId == 0xF) {
    Model = (UINT8) (Model  | ((Register.RegEax >> 12) & 0xF0));
  }

  FamilyId = TempFamilyId;
  if (TempFamilyId == 0xF) {
    FamilyId = (UINT8 ) (FamilyId + (UINT16) ((Register.RegEax >> 20) & 0xFF));
  }

  LeastFeatureProcessor->Index    = Index;
  LeastFeatureProcessor->ApicId   = GetApicID (NULL, NULL);
  LeastFeatureProcessor->Version  = EfiMakeCpuVersion (FamilyId, Model, SteppingId);
}

/**
  Get processor feature delta

  @param[in] FeaturesInput   - Supported features for input processor
  @param[in] CommonFeatures  - Supported features for processor (subset of FeaturesInput)

  @retval The least of processor features
**/
UINT32
GetProcessorFeatureDelta (
  IN UINT32 *FeaturesInput,
  IN UINT32 *CommonFeatures
  )
{
  UINT32 Delta;
  UINTN  Index;

  ///
  /// CommonFeatures is the subset of FeaturesInput
  ///
  Delta = 0;
  for (Index = 0; Index < MAX_FEATURE_NUM; Index++) {
    Delta += GetBitsNumberOfOne (FeaturesInput[Index] - CommonFeatures[Index]);
  }

  return 0;
}

/**

  Find out the common features supported by all core/threads

  @param[in] MpServices  - FRAMEWORK_EFI_MP_SERVICES_PROTOCOL


**/
VOID
GetProcessorCommonFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  )
{
  UINTN  Index;
  UINT32 Features[MAX_FEATURE_NUM];

  GetProcessorFeatures (Features);

  AsmAcquireMPLock (&mLock);
  for (Index = 0; Index < MAX_FEATURE_NUM; Index++) {
    mLeastFeatureProcessor.Features[Index] &= Features[Index];
  }
  AsmReleaseMPLock (&mLock);
  return;
}

/**
  Get the processor data with least features

  @param[in] MpServices  - FRAMEWORK_EFI_MP_SERVICES_PROTOCOL
**/
VOID
GetProcessorWithLeastFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  )
{
  EFI_STATUS         Status;
  UINTN              CurrentProcessor;
  LEAST_FEATURE_PROC LeastFeatureProcessor;

  Status = MpServices->WhoAmI (
                  MpServices,
                  &CurrentProcessor
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  GetProcessorFeatures (LeastFeatureProcessor.Features);
  LeastFeatureProcessor.FeatureDelta = GetProcessorFeatureDelta (
                  LeastFeatureProcessor.Features,
                  mLeastFeatureProcessor.Features
                  );

  AsmAcquireMPLock (&mLock);
  if (LeastFeatureProcessor.FeatureDelta < mLeastFeatureProcessor.FeatureDelta) {
    mLeastFeatureProcessor.FeatureDelta = LeastFeatureProcessor.FeatureDelta;
    UpdateProcessorInfo (CurrentProcessor, &mLeastFeatureProcessor);
  } else if (LeastFeatureProcessor.FeatureDelta == mLeastFeatureProcessor.FeatureDelta) {
    UpdateProcessorInfo (CurrentProcessor, &LeastFeatureProcessor);
    if (LeastFeatureProcessor.Version < mLeastFeatureProcessor.Version) {
      UpdateProcessorInfo (CurrentProcessor, &mLeastFeatureProcessor);
    } else if (LeastFeatureProcessor.Version == mLeastFeatureProcessor.Version) {
      if (LeastFeatureProcessor.ApicId < mLeastFeatureProcessor.ApicId) {
        UpdateProcessorInfo (CurrentProcessor, &mLeastFeatureProcessor);
      }
    }
  }

  AsmReleaseMPLock (&mLock);

  return;
}

/**
  Switch BSP to the processor which has least features

  @param[in] MpServices  - EFI_MP_SERVICES_PROTOCOL

  @retval EFI_STATUS  - status code returned from each sub-routines
**/
EFI_STATUS
SwitchToLowestFeatureProcess (
  IN FRAMEWORK_EFI_MP_SERVICES_PROTOCOL               *MpServices
  )
{
  EFI_STATUS Status;
  UINTN      CurrentProcessor;
  UINTN      NewBsp;
  UINT32     Features[MAX_FEATURE_NUM];

  Status = MpServices->WhoAmI (
                  MpServices,
                  &CurrentProcessor
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ReportStatusCode (
          EFI_PROGRESS_CODE,
          EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_BSP_SELECT
          );

  ///
  /// Take current BSP as the least feature
  ///
  UpdateProcessorInfo (CurrentProcessor, &mLeastFeatureProcessor);
  GetProcessorFeatures (mLeastFeatureProcessor.Features);
  CopyMem (Features, mLeastFeatureProcessor.Features, sizeof (Features));
  Status = MpServices->StartupAllAPs (
                  MpServices,
                  GetProcessorCommonFeature,
                  FALSE,
                  NULL,
                  100000,
                  (VOID *) MpServices,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Go through all processors to find the processor with least features
  ///
  mLeastFeatureProcessor.FeatureDelta = GetProcessorFeatureDelta (Features, mLeastFeatureProcessor.Features);
  Status = MpServices->StartupAllAPs (
                  MpServices,
                  GetProcessorWithLeastFeature,
                  FALSE,
                  NULL,
                  100000,
                  (VOID *) MpServices,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Switch BSP according
  ///
  if (mPlatformCpu->CpuConfig->BspSelection == 16) {
    ///
    /// Enable least feature SBSP selection
    ///
    NewBsp = mLeastFeatureProcessor.Index;
  } else {
    ///
    /// Do not change the current BSP
    /// made by SEC
    ///
    NewBsp = CurrentProcessor;
  }

  if (NewBsp != CurrentProcessor) {

    DEBUG ((EFI_D_INFO, "Switch BSP from %d ==> %d\n", CurrentProcessor, NewBsp));
    Status = MpServices->SwitchBSP (
                    MpServices,
                    NewBsp,
                    TRUE
                    );
  }

  return Status;
}
