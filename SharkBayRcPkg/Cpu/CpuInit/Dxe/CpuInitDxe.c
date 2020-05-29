	/** @file
  Cpu driver, which initializes CPU and implements CPU Architecture
  Protocol as defined in Framework specification.


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
#include "ProcessorData.h"
#include "CacheData.h"
#include "Exception.h"
#include <PfatDefinitions.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/ProcessorProducer.h>
#include <Protocol/SmmBase.h>
#include <Protocol/ExitPmAuth.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HiiLib.h>
#include <Library/RcFviDxeLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BootGuardLibrary.h>
#include <Library/BootGuardRevocationLib.h>
#include <Uefi/UefiInternalFormRepresentation.h>



///
///
#define SAMPLE_TICK_COUNT 1000
///
///


extern UINT64                     mValidMtrrAddressMask;
extern UINT64                     mValidMtrrBitsMask;
extern EFI_CPU_MICROCODE_HEADER   **mMicrocodePointerBuffer;
extern UINT8                      CpuInitDxeStrings[];

extern FVI_ELEMENT_AND_FUNCTION       mCpuFviElementsData[];
extern FVI_DATA_HUB_CALLBACK_CONTEXT  mCpuFviVersionData;
extern UINTN                          mCpuFviElements;

EFI_HANDLE                        mDriverHandle;
EFI_HII_DATABASE_PROTOCOL         *mHiiDatabase;
EFI_SMM_BASE_PROTOCOL             *mSmmBaseProtocol = NULL;
VOID                              *mSmmBaseRegistration;
EFI_METRONOME_ARCH_PROTOCOL       *mMetronome;
DXE_CPU_PLATFORM_POLICY_PROTOCOL  *mPlatformCpu = NULL;
EFI_HII_HANDLE                    mStringHandle;
BOOLEAN                           mIsFlushingGCD = TRUE;
UINT8                             mSmmbaseSwSmiNumber;
BOOLEAN                           mVariableMtrrChanged;
BOOLEAN                           mFixedMtrrChanged;
UINT64                            mCpuFrequency = 0;

EFI_EVENT                 gReadyToBootEvent;
EFI_CPU_INTERRUPT_HANDLER mExternalVectorTable[0x100];
BOOLEAN                   mInterruptState = FALSE;

///
/// The Cpu Architectural Protocol that this Driver produces
///
EFI_CPU_ARCH_PROTOCOL             gCpu = {
  FlushCpuDataCache,
  EnableInterrupt,
  DisableInterrupt,
  CpuGetInterruptState,
  Init,
  RegisterInterruptHandler,
  GetTimerValue,
  SetMemoryAttributes,
  1,  /// NumberOfTimers
  4,  /// DmaBufferAlignment
};

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  UINT32                         Reserved;
  UINT64                         UniqueId;
} HII_VENDOR_DEVICE_PATH_NODE;
#pragma pack()

typedef struct {
  HII_VENDOR_DEVICE_PATH_NODE    Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_TEMP_DEVICE_PATH;

//
// Hii vendor device path template
//
HII_TEMP_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
        (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
      },
      EFI_IFR_TIANO_GUID,
    },
    0,
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

/**

  Decide if the CPU is executing in SMM mode

  @param[in] None

  @retval TRUE  - The CPU is executing in SMM mode
  @retval FALSE - The CPU is not executing in SMM mode

**/
BOOLEAN
ExecutionInSmm (
  VOID
  )
{
  EFI_STATUS  Status;
  BOOLEAN     InSmm;

  if (mSmmBaseProtocol == NULL) {
    return FALSE;
  }

  Status = mSmmBaseProtocol->InSmm (mSmmBaseProtocol, &InSmm);
  ASSERT_EFI_ERROR (Status);
  return InSmm;
}

/**

  Flush CPU data cache. If the instruction cache is fully coherent
  with all DMA operations then function can just return EFI_SUCCESS.

  @param[in] This                - Protocol instance structure
  @param[in] Start               - Physical address to start flushing from.
  @param[in] Length              - Number of bytes to flush. Round up to chipset 
      granularity.
  @param[in] FlushType           - Specifies the type of flush operation to perform.

  @retval EFI_SUCCESS           - If cache was flushed
  @exception EFI_UNSUPPORTED       - If flush type is not supported.
  @retval EFI_DEVICE_ERROR      - If requested range could not be flushed.

**/
EFI_STATUS
EFIAPI
FlushCpuDataCache (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_PHYSICAL_ADDRESS      Start,
  IN UINT64                    Length,
  IN EFI_CPU_FLUSH_TYPE        FlushType
  )
{
  if (FlushType == EfiCpuFlushTypeWriteBackInvalidate) {
    AsmWbinvd ();
    return EFI_SUCCESS;
  } else if (FlushType == EfiCpuFlushTypeInvalidate) {
    AsmInvd ();
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**

  Enables CPU interrupts.

  @param[in] This                - Protocol instance structure

  @retval EFI_SUCCESS           - If interrupts were enabled in the CPU
  @retval EFI_DEVICE_ERROR      - If interrupts could not be enabled on the CPU.

**/
EFI_STATUS
EFIAPI
EnableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL          *This
  )
{
  if (!ExecutionInSmm ()) {
    CpuEnableInterrupt ();
  }

  mInterruptState = TRUE;
  return EFI_SUCCESS;
}

/**

  Disables CPU interrupts.

  @param[in] This                - Protocol instance structure

  @retval EFI_SUCCESS           - If interrupts were disabled in the CPU.
  @retval EFI_DEVICE_ERROR      - If interrupts could not be disabled on the CPU.

**/
EFI_STATUS
EFIAPI
DisableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL     *This
  )
{
  CpuDisableInterrupt ();

  mInterruptState = FALSE;
  return EFI_SUCCESS;
}

/**

  Return the state of interrupts.

  @param[in] This                - Protocol instance structure
  @param[in] State               - Pointer to the CPU's current interrupt state

  @retval EFI_SUCCESS           - If interrupts were disabled in the CPU.
  @retval EFI_INVALID_PARAMETER - State is NULL.

**/
EFI_STATUS
EFIAPI
CpuGetInterruptState (
  IN  EFI_CPU_ARCH_PROTOCOL     *This,
  OUT BOOLEAN                   *State
  )
{
  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *State = mInterruptState;
  return EFI_SUCCESS;
}

/**

  Generates an INIT to the CPU

  @param[in] This                - Protocol instance structure
  @param[in] InitType            - Type of CPU INIT to perform

  @retval EFI_SUCCESS           - If CPU INIT occurred. This value should never be
        seen.
  @retval EFI_DEVICE_ERROR      - If CPU INIT failed.
  @exception EFI_UNSUPPORTED       - Requested type of CPU INIT not supported.

**/
EFI_STATUS
EFIAPI
Init (
  IN EFI_CPU_ARCH_PROTOCOL      *This,
  IN EFI_CPU_INIT_TYPE          InitType
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Registers a function to be called from the CPU interrupt handler.

  @param[in] This                - Protocol instance structure
  @param[in] InterruptType       - Defines which interrupt to hook. IA-32 valid range
                         is 0x00 through 0xFF
  @param[in] InterruptHandler    - A pointer to a function of type
      EFI_CPU_INTERRUPT_HANDLER that is called when a
      processor interrupt occurs. A null pointer
      is an error condition.

  @retval EFI_SUCCESS           - If handler installed or uninstalled.
  @retval EFI_ALREADY_STARTED   - InterruptHandler is not NULL, and a handler for 
        InterruptType was previously installed
  @retval EFI_INVALID_PARAMETER - InterruptHandler is NULL, and a handler for 
        InterruptType was not previously installed.
  @exception EFI_UNSUPPORTED       - The interrupt specified by InterruptType is not
        supported.

**/
EFI_STATUS
EFIAPI
RegisterInterruptHandler (
  IN EFI_CPU_ARCH_PROTOCOL         *This,
  IN EFI_EXCEPTION_TYPE            InterruptType,
  IN EFI_CPU_INTERRUPT_HANDLER     InterruptHandler
  )
{
  if (InterruptType < 0 || InterruptType > 0xff) {
    return EFI_UNSUPPORTED;
  }

  if (InterruptHandler == NULL && mExternalVectorTable[InterruptType] == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (InterruptHandler != NULL && mExternalVectorTable[InterruptType] != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mExternalVectorTable[InterruptType] = InterruptHandler;
  return EFI_SUCCESS;
}

/**

  Returns a timer value from one of the CPU's internal timers. There is no
  inherent time interval between ticks but is a function of the CPU frequency.

  @param[in] This                - Protocol instance structure.
  @param[in] TimerIndex          - Specifies which CPU timer is requested.
  @param[in] TimerValue          - Pointer to the returned timer value.
  @param[in] TimerPeriod         - A pointer to the amount of time that passes in femtoseconds (10-15) for each
                        increment of TimerValue. If TimerValue does not increment at a predictable
                        rate, then 0 is returned. The amount of time that has passed between two calls to
                        GetTimerValue() can be calculated with the formula 
                        (TimerValue2 - TimerValue1) * TimerPeriod. This parameter is optional and may be NULL.

  @retval EFI_SUCCESS           - If the CPU timer count was returned.
  @exception EFI_UNSUPPORTED       - If the CPU does not have any readable timers.
  @retval EFI_DEVICE_ERROR      - If an error occurred while reading the timer.
  @retval EFI_INVALID_PARAMETER - TimerIndex is not valid or TimerValue is NULL.

**/
EFI_STATUS
EFIAPI
GetTimerValue (
  IN  EFI_CPU_ARCH_PROTOCOL     * This,
  IN  UINT32                    TimerIndex,
  OUT UINT64                    *TimerValue,
  OUT UINT64                    *TimerPeriod OPTIONAL
  )
{
  UINT64  Actual;

  if (TimerValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (TimerIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }

  *TimerValue = AsmReadTsc ();

  if (TimerPeriod != NULL) {
    GetActualFrequency (mMetronome, &Actual);
    *TimerPeriod = DivU64x32 (1000000000, (UINT32) Actual);
  }

  return EFI_SUCCESS;
}

/**

  Set memory cacheability attributes for given range of memeory

  @param[in] This                - Protocol instance structure
  @param[in] BaseAddress         - Specifies the start address of the memory range
  @param[in] Length              - Specifies the length of the memory range
  @param[in] Attributes          - The memory cacheability for the memory range

  @retval EFI_SUCCESS           - If the cacheability of that memory range is set successfully
  @exception EFI_UNSUPPORTED       - If the desired operation cannot be done
  @retval EFI_INVALID_PARAMETER - The input parameter is not correct, such as Length = 0

**/
EFI_STATUS
EFIAPI
SetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN UINT64                    Length,
  IN UINT64                    Attributes
  )
{
  EFI_STATUS                Status;
  UINT64                    TempQword;
  UINT32                    MsrNum;
  UINTN                     MtrrNumber;
  BOOLEAN                   Positive;
  BOOLEAN                   OverLap;
#define SKIP_ALIGN_CHECK  0
#if SKIP_ALIGN_CHECK
  UINT32                    Remainder;
#endif
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *MpService;
  EFI_STATUS                Status1;
  UINT32                    VariableMtrrLimit;

  mFixedMtrrChanged     = FALSE;
  mVariableMtrrChanged  = FALSE;

  VariableMtrrLimit     = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  if (mIsFlushingGCD) {
    return EFI_SUCCESS;
  }

  TempQword = 0;

  ///
  /// Check for invalid parameter
  ///
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BaseAddress &~mValidMtrrAddressMask) != 0 || (Length &~mValidMtrrAddressMask) != 0) {
    return EFI_UNSUPPORTED;
  }

  switch (Attributes) {
  case EFI_MEMORY_UC:
    Attributes = CACHE_UNCACHEABLE;
    break;

  case EFI_MEMORY_WC:
    Attributes = CACHE_WRITECOMBINING;
    break;

  case EFI_MEMORY_WT:
    Attributes = CACHE_WRITETHROUGH;
    break;

  case EFI_MEMORY_WP:
    Attributes = CACHE_WRITEPROTECTED;
    break;

  case EFI_MEMORY_WB:
    Attributes = CACHE_WRITEBACK;
    break;

  default:
    return EFI_UNSUPPORTED;
  }
  ///
  /// Check if Fixed MTRR
  ///
  Status = EFI_SUCCESS;
  while ((BaseAddress < (1 << 20)) && (Length > 0) && Status == EFI_SUCCESS) {
    Status = CalculateFixedMtrr (Attributes, &BaseAddress, &Length);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (mFixedMtrrChanged) {
    ProgramFixedMtrr ();
  }

  if (Length == 0) {
    ///
    /// Just Fixed MTRR. NO need to go through Variable MTRR
    ///
    goto Done;
  }

  ///
  /// since mem below 1m will be override by fixed mtrr, we can set it to 0 to save mtrr.
  ///
  if (BaseAddress == 0x100000) {
    BaseAddress = 0;
    Length += 0x100000;
  }

  ///
  /// Check memory base address alignment
  ///
#if SKIP_ALIGN_CHECK
  DivU64x32Remainder (BaseAddress, (UINT32) Power2MaxMemory (LShiftU64 (Length, 1)), &Remainder);
  if (Remainder != 0) {
    DivU64x32Remainder (BaseAddress, (UINT32) Power2MaxMemory (Length), &Remainder);
    if (Remainder != 0) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }
  }
#endif

  ///
  /// Check overlap
  ///
  GetMemoryAttribute ();
  OverLap = CheckMemoryAttributeOverlap (BaseAddress, BaseAddress + Length - 1);
  if (OverLap) {
    Status = CombineMemoryAttribute (Attributes, &BaseAddress, &Length);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    if (Length == 0) {
      ///
      /// combine successfully
      ///
      Status = EFI_SUCCESS;
      goto Done;
    }
  } else {
    if (Attributes == CACHE_UNCACHEABLE) {
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  ///
  /// Program Variable MTRRs
  ///
  if (mUsedMtrr >= VariableMtrrLimit) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  ///
  /// Find first unused MTRR
  ///
  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {
    if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) == 0) {
      break;
    }
  }

  TempQword = Length;
  if (TempQword == Power2MaxMemory (TempQword)) {
    ProgramVariableMtrr (
      MsrNum,
      BaseAddress,
      Length,
      Attributes
      );
  } else {
    GetDirection (TempQword, &MtrrNumber, &Positive);
    if ((mUsedMtrr + MtrrNumber) > VariableMtrrLimit) {
      goto Done;
    }
    if (!Positive) {
      Length = Power2MaxMemory (LShiftU64 (TempQword, 1));
      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        Attributes
        );
      BaseAddress += TempQword;
      TempQword   = Length - TempQword;
      Attributes  = CACHE_UNCACHEABLE;
    }
    do {
      ///
      /// Find unused MTRR
      ///
      for (; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {
        if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) == 0) {
          break;
        }
      }
      Length = Power2MaxMemory (TempQword);
      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        Attributes
        );
      BaseAddress += Length;
      TempQword -= Length;
    } while (TempQword);
  }

Done:
  Status1 = gBS->LocateProtocol (
                  &gFrameworkEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );

  if (!EFI_ERROR (Status1)) {
    if (mVariableMtrrChanged || mFixedMtrrChanged) {
      ///
      /// PERF_START (NULL, L"CacheSync", NULL, 0);
      ///
      ReadMtrrRegisters ();
      Status1 = MpService->StartupAllAPs (
                            MpService,
                            MpMtrrSynchUp,
                            FALSE,
                            NULL,
                            0,
                            NULL,
                            NULL
                            );
      ///
      /// PERF_END (NULL, L"CacheSync", NULL, 0);
      ///
    }
  }

  return Status;
}

/**
  Initialize the SmmBase pointer when SmmBase protocol get installed

  @param[in] Event   - Event whose notification function is being invoked.
  @param[in] Context - Pointer to the notification functions context, which is implementation dependent.
**/
VOID
EFIAPI
InitializeSmmBasePtr (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBaseProtocol);
  if (EFI_ERROR (Status)) {
    mSmmBaseProtocol = NULL;
  }
}

/**
 The HII driver handle passed in for HiiDatabase.NewPackageList() requires
 that there should be DevicePath Protocol installed on it.
 This routine create a virtual Driver Handle by installing a vendor device
 path on it, so as to use it to invoke HiiDatabase.NewPackageList().

 @param [in]     DriverHandle         Handle to be returned

 @retval EFI_SUCCESS            Handle destroy success.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  )
{
  EFI_STATUS                   Status;
  HII_VENDOR_DEVICE_PATH_NODE  *VendorDevicePath;

  VendorDevicePath = AllocateCopyPool (sizeof (HII_TEMP_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
  if (VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use memory address as unique ID to distinguish from different device paths
  //
  VendorDevicePath->UniqueId = (UINT64) ((UINTN) VendorDevicePath);

  *DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  VendorDevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**

  Call back function to publish Hii data

  @param[in] Event   - The triggered event.
  @param[in] Context - Context for this event.


**/
VOID
EFIAPI
HiiCallback (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
  EFI_STATUS                  Status;

  ///
  /// Initialize strings to HII database
  ///

  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&mHiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  ///
  /// Create driver handle used by HII database
  ///
  Status = CreateHiiDriverHandle (&mDriverHandle);
  if (EFI_ERROR (Status)) {
    return;
  }

  ///
  /// Publish our HII data
  ///
  mStringHandle = HiiAddPackages (
                &gProcessorProducerGuid,
                mDriverHandle,
                CpuInitDxeStrings,
                NULL
                );

  ASSERT (mStringHandle != NULL);
  
}

/**
  Create SMBIOS Table type - FviSmbiosType, when ExitPmAuth event is signaled

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
  **/
VOID
InitializeFviDataCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS Status;
  VOID       *ProtocolPointer;
  UINT32     uCodeRevision;

  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  gBS->CloseEvent (Event);

  if (mPlatformCpu->CpuConfig->FviReport) {
    InitFviDataHubCbContext (
      mPlatformCpu->CpuConfig->FviSmbiosType,
      (UINT8) mCpuFviElements,
      &mCpuFviVersionData
      );

    uCodeRevision = GetCpuUcodeRevision ();
    mCpuFviElementsData[UCODE_VER].Element.Version.MajorVersion = (UINT8) ((uCodeRevision & 0xFF000000) >> 24);
    mCpuFviElementsData[UCODE_VER].Element.Version.MinorVersion = (UINT8) ((uCodeRevision & 0x00FF0000) >> 16);
    mCpuFviElementsData[UCODE_VER].Element.Version.Revision     = (UINT8) ((uCodeRevision & 0x0000FF00) >> 8);
    mCpuFviElementsData[UCODE_VER].Element.Version.BuildNum     = (UINT16) (uCodeRevision & 0x000000FF);

    CreateRcFviDatahub (&mCpuFviVersionData);
  }
  return;
}

/**
  Initialize the state information for the CPU Architectural Protocol

  @param[in] ImageHandle - Image handle of the loaded driver
  @param[in] SystemTable - Pointer to the System Table

  @retval EFI_SUCCESS           - thread can be successfully created
  @retval EFI_OUT_OF_RESOURCES  - cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      - cannot create the thread
**/
EFI_STATUS
EFIAPI
InitializeCpu (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_HANDLE NewHandle1;
  VOID       *Registration;

  ///
  /// Initialize the Global Descriptor Table
  ///
  InitializeSelectors ();

  ///
  /// Setup Cache attributes and Interrupt Tables
  ///
  PrepareMemory ();

  ///
  /// Initialize Exception Handlers
  ///
  InitializeException (&gCpu);

  ///
  /// Install CPU Architectural Protocol
  ///
  NewHandle1  = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle1,
                  &gEfiCpuArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gCpu
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Refresh memory space attributes according to MTRRs
  ///
  Status          = RefreshGcdMemoryAttributes ();
  mIsFlushingGCD  = FALSE;
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Locate DxeCpuPlatformPolicy protocol instance and assign it to a global variable
  ///
  Status = gBS->LocateProtocol (&gDxeCpuPlatformPolicyProtocolGuid, NULL, (VOID **) &mPlatformCpu);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR,"Failed to locate DxeCpuPlatformPolicy Protocol\n"));
    return Status;
  }

  //
  // Dump Cpu platform policy
  //
  CpuDxePolicyDump();

  ///
  /// Initialize the global SmmBase SWSMI number
  ///
  mSmmbaseSwSmiNumber = mPlatformCpu->CpuConfig->SmmbaseSwSmiNumber;

  ///
  /// Load the microcode if needed
  ///
  Status  = LoadAllMicrocodeUpdates ();

  Status  = gBS->LocateProtocol (&gEfiMetronomeArchProtocolGuid, NULL, (VOID **) &mMetronome);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiCreateProtocolNotifyEvent (
    &gEfiHiiDatabaseProtocolGuid,
    TPL_CALLBACK,
    HiiCallback,
    NULL,
    &Registration
    );

  ///
  /// Create an SmmBase protocol call back event to initialize
  /// Smm Base pointer, during SMM mode
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiSmmBaseProtocolGuid,
    TPL_CALLBACK,
    InitializeSmmBasePtr,
    NULL,
    &mSmmBaseRegistration
    );

  ///
  /// Initialize MP Support if necessary
  ///
  Status = InitializeMpSupport ();
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR,"Failed to initialize MPs\n"));
  }

  ///
  /// Create an ExitPmAuth protocol call back event.
  ///
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    InitializeFviDataCallback,
    NULL,
    &Registration
    );

  ///
  /// Create an ExitPmAuth protocol call back event for PFAT.
  ///
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    InitializePfatToolsIntCallback,
    NULL,
    &Registration
    );

  ///
  /// Verify if Boot Guard is supported
  ///
  if (IsBootGuardSupported()){
    ///
    /// Identify if Revocation is requested by Boot Guard ACM
    ///
    if (AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & BIT7) {
    ///
    /// Create an ExitPmAuth protocol call back event if one or more of Boot Guard modules are revoked.
    ///
    EfiCreateProtocolNotifyEvent (
      &gExitPmAuthProtocolGuid,
      TPL_CALLBACK,
      BootGuardRevocationCallback,
      NULL,
      &Registration
      );
    }
  }

  return EFI_SUCCESS;
}

/**
  Returns the actual CPU core frequency in MHz.

  @param[in] Metronome           - Metronome protocol
  @param[in] Frequency           - Pointer to the CPU core frequency

  @retval EFI_SUCCESS            - If the frequency is returned successfully
  @retval EFI_INVALID_PARAMETER  - If the input parameter is wrong
**/
EFI_STATUS
GetActualFrequency (
  IN EFI_METRONOME_ARCH_PROTOCOL *Metronome,
  OUT UINT64                     *Frequency
  )
{
  UINT64     BeginValue;
  UINT64     EndValue;
  UINT64     TotalValue;
  UINT32     TickCount;
  BOOLEAN    InterruptState;
  EFI_STATUS Status;

  if (Metronome == NULL || Frequency == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mCpuFrequency == 0) {
    ///
    /// In order to calculate the actual CPU frequency, we keep track of the CPU Tsc value (which
    /// increases by 1 for every cycle) for a know period of time. The Metronome is not accurate
    /// for the 1st tick, so I choose to wait for 1000 ticks, thus the error can be control to be
    /// lower than 1%.
    ///
    TickCount = SAMPLE_TICK_COUNT;
    CpuGetInterruptState (&gCpu, &InterruptState);
    if (InterruptState) {
      DisableInterrupt (&gCpu);
    }
    ///
    /// In DxeCis-0.91 specs.
    /// Metronome->WaitForTick is possible for interrupt processing,
    /// or exception processing to interrupt the execution of the WaitForTick() function.
    /// Depending on the hardware source for the ticks, it is possible for a tick to be missed.
    /// This function cannot guarantee that ticks will not be missed.
    ///
    while (TRUE) {
      BeginValue  = AsmReadTsc ();
      Status      = Metronome->WaitForTick (Metronome, TickCount);
      EndValue    = AsmReadTsc ();
      if (!EFI_ERROR (Status)) {
        TotalValue = EndValue - BeginValue;
        break;
      }
    }

    if (InterruptState) {
      EnableInterrupt (&gCpu);
    }

    mCpuFrequency = MultU64x32 (TotalValue, 10);
    mCpuFrequency = DivU64x32 (mCpuFrequency, Metronome->TickPeriod * TickCount);
  }

  *Frequency = mCpuFrequency;

  return EFI_SUCCESS;
}

/**
  Dump RC CPU and PPM platform policies
**/
VOID
CpuDxePolicyDump (
  VOID
  )
{
#ifdef EFI_DEBUG
  CPU_CONFIG        *CpuConfig;
  POWER_MGMT_CONFIG *PowerMgmtConfig;
  SECURITY_CONFIG   *SecurityConfig;

  CpuConfig = mPlatformCpu->CpuConfig;
  PowerMgmtConfig = mPlatformCpu->PowerMgmtConfig;
  SecurityConfig = mPlatformCpu->SecurityConfig;
  //
  // Dump Cpu Platform policy
  //
  DEBUG ((EFI_D_INFO, "\n------------------------ DXE CpuPlatformPolicy Dump Begin -----------------\n"));
  //
  // Cpu config
  //
  DEBUG ((EFI_D_INFO, " CPU:: HtState : 0x%X \n", CpuConfig->HtState));
  DEBUG ((EFI_D_INFO, " CPU:: LimitCpuidMaximumValue : 0x%X\n", CpuConfig->LimitCpuidMaximumValue));
  DEBUG ((EFI_D_INFO, " CPU:: ExecuteDisableBit : 0x%X\n", CpuConfig->ExecuteDisableBit));
  DEBUG ((EFI_D_INFO, " CPU:: VmxEnable : 0x%X\n", CpuConfig->VmxEnable));
  DEBUG ((EFI_D_INFO, " CPU:: SmxEnable : 0x%X\n", CpuConfig->SmxEnable));
  DEBUG ((EFI_D_INFO, " CPU:: MachineCheckEnable : 0x%X\n", CpuConfig->MachineCheckEnable));
  DEBUG ((EFI_D_INFO, " CPU:: MonitorMwaitEnable : 0x%X\n", CpuConfig->MonitorMwaitEnable));
  DEBUG ((EFI_D_INFO, " CPU:: XapicEnable : 0x%X\n", CpuConfig->XapicEnable));
  DEBUG ((EFI_D_INFO, " CPU:: IsColdReset : 0x%X\n", CpuConfig->IsColdReset));
  DEBUG ((EFI_D_INFO, " CPU:: MlcStreamerPrefetcher : 0x%X\n", CpuConfig->MlcStreamerPrefetcher));
  DEBUG ((EFI_D_INFO, " CPU:: EnableDts : 0x%X\n", CpuConfig->EnableDts));
  DEBUG ((EFI_D_INFO, " CPU:: FviReport : 0x%X\n", CpuConfig->FviReport));
  DEBUG ((EFI_D_INFO, " CPU:: AesEnable : 0x%X\n", CpuConfig->AesEnable));
  DEBUG ((EFI_D_INFO, " CPU:: DebugInterfaceEnable : 0x%X\n", CpuConfig->DebugInterfaceEnable));
  DEBUG ((EFI_D_INFO, " CPU:: DebugInterfaceLockEnable : 0x%X\n", CpuConfig->DebugInterfaceLockEnable));
  DEBUG ((EFI_D_INFO, " CPU:: ApIdleManner : 0x%X\n", CpuConfig->ApIdleManner));
  DEBUG ((EFI_D_INFO, " CPU:: ApHandoffManner : 0x%X\n", CpuConfig->ApHandoffManner));
  DEBUG ((EFI_D_INFO, " CPU:: BspSelection : 0x%X\n", CpuConfig->BspSelection));
  DEBUG ((EFI_D_INFO, " CPU:: SmmbaseSwSmiNumber : 0x%X\n", CpuConfig->SmmbaseSwSmiNumber));
  DEBUG ((EFI_D_INFO, " CPU:: FviSmbiosType; : 0x%X\n", CpuConfig->FviSmbiosType));
  //
  // SECURITY_CONFIG : TXT_FUNCTION_CONFIG
  //
  DEBUG ((EFI_D_INFO, " CPU:: SECURITY_CONFIG:TXT_FUNCTION_CONFIG : ResetAux : 0x%X\n", SecurityConfig->TxtFunctionConfig->ResetAux));
  DEBUG ((EFI_D_INFO, "\n------------------------ DXE CpuPlatformPolicy Dump End -----------------\n"));
#endif
}

/**
  Initialize the global DataHub pointer

  @param[in] DataHub   - Pointer to the DataHub protocol as output

  @retval EFI_SUCCESS  - If the DataHub pointer is initialized successfully
**/
EFI_STATUS
InitializeDataHubPtr (
  OUT EFI_DATA_HUB_PROTOCOL **DataHub
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **) DataHub);
  return Status;
}

/**
  Drop into SMM to register IOTRAP for pfat tools interface

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
InitializePfatToolsIntCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS           Status;
  VOID                 *ProtocolPointer;
  EFI_PHYSICAL_ADDRESS BaseAddress;
  PFAT_HOB             *PfatHobPtr;
  EFI_GUID             PfatHobGuid = PFAT_HOB_GUID;

  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  PfatHobPtr = GetFirstGuidHob (&PfatHobGuid);
  if (PfatHobPtr != NULL) {
    BaseAddress = (EFI_PHYSICAL_ADDRESS) PfatHobPtr->PfatToolsIntIoTrapAdd;
    ///
    /// IOTRAP TO SMM
    ///
    IoRead8 (BaseAddress);
  }
  return;
}

/**
  Register callback function for Boot Guard revocation flow.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
BootGuardRevocationCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS  Status;
  VOID        *ProtocolPointer;

  //
  // Check whether this is real ExitPmAuth notification, or just a SignalEvent
  //
  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_ERROR (Status)) {
    return;
  }

  BootGuardOemRevocationHook();
  
  //
  // Closed the event to avoid call twice
  //
  gBS->CloseEvent (Event);

  return;
}
