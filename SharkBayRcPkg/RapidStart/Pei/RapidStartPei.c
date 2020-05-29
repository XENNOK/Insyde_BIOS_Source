/** @file
  This Peim driver will do RapidStart Entry or RapidStart Exit transition

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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


#include <RapidStartAhci.h>
#include <RapidStartData.h>
#include <SaAccess.h>

#include <Library/RapidStartPeiLib.h>
#include <Library/DebugLib.h>
#include <Library/PchPlatformLib/PchPlatformLibrary.h>
#include <Library/PeiServicesLib.h>
#include <Library/RapidStartCommonLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/GfxDisplayLibPei.h>
#include <Library/PerformanceLib.h>

#include <Ppi/RapidStart.h>
#include <Ppi/Heci.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/PeiGfxPpi.h>
#include <Ppi/SaPlatformPolicy.h>

#include <Protocol/RapidStartGlobalNvsArea.h>

#include <Guid/AcpiVariable.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/RapidStartTransition.h>

#define PAM_COUNT         7

#define ME_FID_TIMEOUT    10000

#define MAX_MEMORY_RANGES   (MAX_GFX_MEMORY_RANGES + 0x08)
#define MAX_SMRAM_RANGES  2
#define DPR_RANGES        1

#ifndef MAX
#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)         (((a) <= (b)) ? (a) : (b))
#endif
#define ARRAY_SIZE(a)     (sizeof (a) / sizeof (*(a)))
#define POOL_SIZE(a)      (((((EFI_HOB_GENERIC_HEADER*)a - 1)->HobLength) - sizeof(EFI_HOB_GENERIC_HEADER)) / sizeof (*(a)))
#define VARIABLE_SIZE(a)  (ARRAY_SIZE(a) > 0) ? ARRAY_SIZE(a) : POOL_SIZE(a)

#define END_RANGES        ((UINT64) (INT64) (-1))
#define RANGE_LENGTH(r)   ((r)->End - (r)->Start)

//EFI_GUID  gEfiAcpiVariableGuid              = EFI_ACPI_VARIABLE_GUID;
//[-start-130315-IB06690220-add]//
EFI_GUID  gAcpiVariableSetGuid              = EFI_ACPI_VARIABLE_GUID;
//[-end-130315-IB06690220-add]//

typedef struct {
  UINT64  Start;
  UINT64  End;
} MEMORY_RANGE;

#define RAPID_START_INSTANCE_SIGNATURE  SIGNATURE_32 ('i', 'R', 'S', 'T')

typedef struct _RAPID_START_INSTANCE {
  RAPID_START_PPI             Ppi;
  EFI_PEI_PPI_DESCRIPTOR      PpiDesc;
  UINT32                      Signature;
  BOOLEAN                     Enabled;
  RAPID_START_PERSISTENT_DATA Data;
  RAPID_START_TRANSITION      Transition;
  RAPID_START_GLOBAL_NVS_AREA *RapidStartGlobalNvs;
  AHCI_CONTEXT                Ahci;
  CHAR8                       HddPassword[ATA_PASSWORD_LEN];
  EFI_STATUS                  PwdStatus;
  BOOLEAN                     FreezeLock;
#ifndef RAPID_START_NO_SMRAM_INTEGRITY_CHECK
  UINT8                       SmRamHash[RAPID_START_SECURE_HASH_LENGTH];
#endif
  UINT64                      NotUsedLba;
  UINT32                      IedSize;
//[-start-120628-IB05330351-add]//
  UINT16                      PasswordType;
//[-end-120628-IB05330351-add]//
} RAPID_START_INSTANCE;

#define RAPID_START_INSTANCE_FROM_THIS(a) CR (a, RAPID_START_INSTANCE, Ppi, RAPID_START_INSTANCE_SIGNATURE)

EFI_STATUS
RapidStartClearAndEnablePmeEvent (
  IN    BOOLEAN EnablePme
  );

STATIC
EFI_STATUS
RapidStartExitAtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

STATIC
EFI_STATUS
RapidStartRecoveryAtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

STATIC
EFI_STATUS
RapidStartS4AtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

STATIC
EFI_STATUS
RapidStartMemoryInstalled (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

STATIC
EFI_STATUS
InstallRapidStartPpi (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mRapidStartExitNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  RapidStartExitAtEndOfPei
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mRapidStartRecoveryNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  RapidStartRecoveryAtEndOfPei
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mRapidStartS4NotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  RapidStartS4AtEndOfPei
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mRapidStartTransitionPpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gRapidStartTransitionPpiGuid,
  RapidStartMemoryInstalled
};

#ifdef ME_SUPPORT_FLAG
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mInstallRapidStartPpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiHeciPpiGuid,
  InstallRapidStartPpi
};
#endif

STATIC
EFI_STATUS
RapidStartDoTransition (
  IN      RAPID_START_PPI      *This
  );

#if !defined(RAPID_START_NO_SMRAM_INTEGRITY_CHECK) && defined(BUILD_WITH_GLUELIB)
#undef SetMem
/**
  SetMem alternative in case GlueLib defines SetMem as a macro.
  Required by CryptoLib.

  @param[out] Buffer  - Pointer to the memory buffer
  @param[in] Length  - The length for setting memory
  @param[in] Value   - The value that will be set to memory buffer
**/
VOID *
SetMem (
  OUT VOID *Buffer,
  IN UINTN Length,
  IN UINT8 Value
  )
{
  return GlueSetMem (Buffer, Length, Value);
}

#undef CopyMem
/**
  Copy Length bytes from Source to Destination.

  @param[out] DestinationBuffer - Target of copy
  @param[in] SourceBuffer      - Place to copy from
  @param[in] Length            - Number of bytes to copy

  @retval Status code
**/
VOID *
EFIAPI
CopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  return GlueCopyMem (DestinationBuffer, SourceBuffer, Length);
}
#endif

#ifndef EFI_DEBUG
#define PrintRanges(r)
#else
/**
  Show memory range information by debug macro

  @param[in] ranges  - Memory range information structure
**/
VOID
PrintRanges (
  IN MEMORY_RANGE *ranges
  )
{
  while (ranges->Start != END_RANGES) {
    DEBUG ((EFI_D_INFO, "%08lx - %08lx\n", ranges->Start, ranges->End));
    ++ranges;
  }
}
#endif

/**
  Sort memory ranges.

  @param[in][out] MemoryRanges - Memory Ranges
  @param[in] MemoryRangesSize  - Number of entries in Memory Ranges

  @retval Status code

--*/
EFI_STATUS
SortRanges (
  IN OUT       MEMORY_RANGE   *MemoryRanges,
  IN           UINTN          MemoryRangesSize
  )
{
  UINT32        Count, Index;
  MEMORY_RANGE  TempRange;
  MEMORY_RANGE  *Range;

  Range = MemoryRanges;

  if (MemoryRangesSize == 0) {
    while (Range->Start != END_RANGES) {
      MemoryRangesSize++;
      Range++;
    }
  }

  for (Count = 1; Count < MemoryRangesSize; Count++) {
   for (Index = 0; Index < MemoryRangesSize - Count; Index++) {
       if (MemoryRanges[Index].Start > MemoryRanges[Index + 1].Start) {
         TempRange.Start               = MemoryRanges[Index].Start;
         TempRange.End                 = MemoryRanges[Index].End;
         MemoryRanges[Index].Start     = MemoryRanges[Index + 1].Start;
         MemoryRanges[Index].End       = MemoryRanges[Index + 1].End;
         MemoryRanges[Index + 1].Start = TempRange.Start;
         MemoryRanges[Index + 1].End   = TempRange.End;
       }
     }
   }

  return EFI_SUCCESS;
}


/**
  Subtracts set of memory ranges from other set of memory ranges.
  Resulting set will contain ranges covering InputRanges with SubRanges excluded.

  Both InputRanges and SubRanges must be provided in ascending order
  by Start address. Output set is also sorted.

  All sets end with a range which Start address is END_RANGES.

  @param[out] OutputRanges      - Output buffer
  @param[in] OutputRangesSize  - Number of entries in output buffer
  @param[in] InputRanges       - Input ranges to substract from
  @param[in] SubRanges         - Ranges to be excluded from InputRanges

  @retval EFI_SUCCESS          - Success
  @retval EFI_BUFFER_TOO_SMALL - Not enough space in output buffer
**/
STATIC
EFI_STATUS
SubtractRanges (
  OUT       MEMORY_RANGE   *OutputRanges,
  IN        UINTN          OutputRangesSize,
  IN        MEMORY_RANGE   *InputRanges,
  IN        MEMORY_RANGE   *SubRanges
  )
{
  MEMORY_RANGE  *SubRange;
  MEMORY_RANGE  Current;
  UINTN         OutIndex;

  ASSERT (OutputRanges != InputRanges);
  ASSERT (OutputRanges != SubRanges);

  DEBUG ((EFI_D_INFO, "Sub ranges Before Sorting:\n"));
  PrintRanges (SubRanges);
  SortRanges (SubRanges, 0);
  DEBUG ((EFI_D_INFO, "Sub ranges After Sorting:\n"));
  PrintRanges (SubRanges);

  OutIndex = 0;
  while (InputRanges->Start != END_RANGES) {
    Current.Start = InputRanges->Start;
    Current.End   = InputRanges->End;
    SubRange      = SubRanges;
    while (SubRange->Start != END_RANGES) {
      if ((Current.Start < SubRange->End) && (Current.End > SubRange->Start)) {
        if (Current.Start < SubRange->Start) {
          if (OutIndex + 1 >= OutputRangesSize) {
            return EFI_BUFFER_TOO_SMALL;
          }

          OutputRanges[OutIndex].Start  = Current.Start;
          OutputRanges[OutIndex].End    = MIN (Current.End, SubRange->Start);
          ++OutIndex;
        }

        if (Current.End > SubRange->End) {
          Current.Start = SubRange->End;
        } else {
          Current.Start = END_RANGES;
        }
      }

      ++SubRange;
    }

    if (Current.Start != END_RANGES) {
      if (OutIndex + 1 >= OutputRangesSize) {
        return EFI_BUFFER_TOO_SMALL;
      }

      OutputRanges[OutIndex].Start  = Current.Start;
      OutputRanges[OutIndex].End    = Current.End;
      ++OutIndex;
    }

    ++InputRanges;
  }

  ASSERT (OutIndex < OutputRangesSize);
  OutputRanges[OutIndex].Start = END_RANGES;
  return EFI_SUCCESS;
}

/**
  Returns total length of ranges in the set Ranges.

  @param[in] Ranges       - Array of memory ranges.

  @retval The size of memory range
**/
STATIC
UINT64
SumRanges (
  IN MEMORY_RANGE *Ranges
  )
{
  UINT64  Size;
  Size = 0;
  while (Ranges->Start != END_RANGES) {
    Size += RANGE_LENGTH (Ranges);
    ++Ranges;
  }

  return Size;
}


/**
  This routing is executed to clear all pending wake events and then enable wake events.

  @param[in] EnablePme - set to TRUE to enable PME event

  @retval EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
RapidStartClearAndEnablePmeEvent (
  IN    BOOLEAN EnablePme
  )
{
  RAPID_START_PPI             *This;
  EFI_STATUS                  Status;
  RAPID_START_INSTANCE        *Instance;
  RAPID_START_PERSISTENT_DATA *RapidStartData;
  RAPID_START_MEM_DATA        *RapidStartMemData;
  PCH_SERIES                  PchSeries;
  UINT32                      PmBase;

  PchSeries = GetPchSeries();
  PmBase = MmioRead32 (
            MmPciAddress (
            0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  Status = PeiServicesLocatePpi (&gRapidStartPpiGuid, 0, NULL, (VOID **) &This);
  ASSERT_EFI_ERROR (Status);
  Instance          = RAPID_START_INSTANCE_FROM_THIS (This);
  RapidStartData    = &Instance->Data;
  RapidStartMemData = RAPID_START_MEM_DATA_PTR (RapidStartData);
  DEBUG ((EFI_D_ERROR, "RapidStart: RapidStartMemData @ %X\n", RapidStartMemData));

  ///
  /// Clear all pending wake events
  ///
  RapidStartClearOemPmeEvent ();
  if (PchSeries == PchLp) {
    IoWrite32 (PmBase + R_PCH_ACPI_GPE0_STS_127_96, ~0u);
  } else if (PchSeries == PchH) {
    IoWrite32 (PmBase + R_PCH_ACPI_GPE0a_STS, ~0u);
    IoWrite32 (PmBase + R_PCH_ACPI_GPE0b_STS, ~0u);
  }
  IoWrite16 (
    PmBase + R_PCH_ACPI_PM1_STS,
    B_PCH_ACPI_PM1_STS_WAK | B_PCH_ACPI_PM1_STS_RTC | B_PCH_ACPI_PM1_STS_PWRBTN
    );

  ///
  /// Enable wake events
  ///
  if (EnablePme == TRUE) {
    if (PchSeries == PchLp) {
      IoOr32 (PmBase + R_PCH_ACPI_GPE0_EN_127_96, RapidStartMemData->GPE0);
    } else if (PchSeries == PchH) {
      IoOr32 (PmBase + R_PCH_ACPI_GPE0a_EN, RapidStartMemData->GPE0a);
      IoOr32 (PmBase + R_PCH_ACPI_GPE0b_EN, RapidStartMemData->GPE0b);
    }
  }

  return EFI_SUCCESS;
}

/**
  RapidStart End of PEI handler.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS - Ppi callback function executed successfully
**/
STATIC
EFI_STATUS
RapidStartS4AtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  UINT32                      PmBase;
  EFI_STATUS                  Status;
  RAPID_START_PPI             *This;
  RAPID_START_INSTANCE        *Instance;
  RAPID_START_PERSISTENT_DATA *RapidStartData;
//[-start-120809-IB03780457-add]//
  RAPID_START_MEM_DATA        *RapidStartMemData;
//[-end-120809-IB03780457-add]//

  DEBUG ((EFI_D_INFO, "RapidStartS4AtEndOfPei ()\n"));

  PmBase = MmioRead32 (
            MmPciAddress (
            0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  Status = PeiServicesLocatePpi (&gRapidStartPpiGuid, 0, NULL, (VOID **) &This);
  ASSERT_EFI_ERROR (Status);
  Instance          = RAPID_START_INSTANCE_FROM_THIS (This);
  RapidStartData    = &Instance->Data;
//[-start-120809-IB03780457-add]//
  RapidStartMemData = RAPID_START_MEM_DATA_PTR (RapidStartData);
  DEBUG ((EFI_D_ERROR, "RapidStart: RapidStartMemData @ %X\n", RapidStartMemData));
//[-end-120809-IB03780457-add]//
//[-start-120606-IB04770233-add]//
  DEBUG ((EFI_D_ERROR, "RapidStart: RapidStartMemData->GPE0a = %X\n", RapidStartMemData->GPE0a));
  DEBUG ((EFI_D_ERROR, "RapidStart: RapidStartMemData->GPE0b = %X\n", RapidStartMemData->GPE0b));

  //
  // Re-aim OS wake timer if enabled
  //
  if (RapidStartMemData->OsWakeTimeEnabled) {
    DEBUG (
      (EFI_D_INFO,
      "Setting RTC alarm to OS provided value %02d-%02d:%02d:%02d\n",
      RapidStartMemData->OsWakeTime.Date,
      RapidStartMemData->OsWakeTime.Hour,
      RapidStartMemData->OsWakeTime.Minute,
      RapidStartMemData->OsWakeTime.Second)
      );

    RtcSetAlarm (&RapidStartMemData->OsWakeTime);

    ///
    /// Clear RTC PM1 status
    ///
    IoWrite16 (PmBase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

    //
    // set RTC_EN bit in PM1_EN to wake up from the alarm
    //
    IoWrite16 (
      PmBase + R_PCH_ACPI_PM1_EN,
      (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
      );
  }
//[-end-120606-IB04770233-add]//

  RapidStartWANetDetect ((CONST EFI_PEI_SERVICES **)PeiServices, RapidStartData->WlanMmioSpace);
  RapidStartClearAndEnablePmeEvent (TRUE);

  DEBUG ((EFI_D_ERROR, "RapidStart: Switching to S4\n"));
  IoAndThenOr32 (
    PmBase + R_PCH_ACPI_PM1_CNT,
    (UINT32)~B_PCH_ACPI_PM1_CNT_SLP_TYP,
    (UINT32) (V_PCH_ACPI_PM1_CNT_S4 | B_PCH_ACPI_PM1_CNT_SLP_EN)
    );

  CpuDeadLoop ();
  return EFI_SUCCESS;
}

/**
  RapidStart End of PEI handler.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS - Ppi callback function executed successfully
**/
STATIC
EFI_STATUS
RapidStartRecoveryAtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  UINT32  PmBase;

  DEBUG ((EFI_D_INFO, "RapidStartRecoveryAtEndOfPei ()\n"));

  RapidStartClearAndEnablePmeEvent (TRUE);

  PmBase = MmioRead32 (
            MmPciAddress (
            0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  DEBUG ((EFI_D_ERROR, "RapidStart: Switching state back to S3\n"));
  IoAndThenOr32 (
    PmBase + R_PCH_ACPI_PM1_CNT,
    (UINT32)~B_PCH_ACPI_PM1_CNT_SLP_TYP,
    (UINT32) (V_PCH_ACPI_PM1_CNT_S3 | B_PCH_ACPI_PM1_CNT_SLP_EN)
    );

  CpuDeadLoop ();
  return EFI_SUCCESS;
}

/**
  Executes necessary RapidStart late initialization after Boot Script done.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS   - RapidStart initialization successful
**/
STATIC
EFI_STATUS
RapidStartExitAtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  DEBUG ((EFI_D_INFO, "RapidStartExitAtEndOfPei ()\n"));
  return RapidStartEnableAcpi ((CONST EFI_PEI_SERVICES **)PeiServices);
}

/**
  PEI_PERMANENT_MEMORY_INSTALLED_PPI handler.
  RapidStart transition is triggered here in case RapidStart is postoponed untill memory available in PEI.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
RapidStartMemoryInstalled (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  RAPID_START_PPI *RapidStartPpi;
  EFI_STATUS      Status;

  DEBUG ((EFI_D_INFO, "RapidStartMemoryInstalled ()\n"));
  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gRapidStartPpiGuid, 0, NULL, (VOID **) &RapidStartPpi);
  ASSERT_EFI_ERROR (Status);

  RapidStartDoTransition (RapidStartPpi);

  return EFI_SUCCESS;
}

#ifdef EFI_DEBUG
/**
  Dump MCH relevant registers
**/
STATIC
VOID
DumpMcRegisters (
  VOID
  )
{
  UINTN MchBase;
  MchBase = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);

  DEBUG ((EFI_D_INFO, "TOM      : %lx\n", MmioRead64 (MchBase + R_SA_TOM)));
  DEBUG ((EFI_D_INFO, "TSEGMB   : %x\n", MmioRead32 (MchBase + R_SA_TSEGMB)));
  DEBUG ((EFI_D_INFO, "TOLUD    : %x\n", MmioRead32 (MchBase + R_SA_TOLUD)));
  DEBUG ((EFI_D_INFO, "TOUUD    : %lx\n", MmioRead64 (MchBase + R_SA_TOUUD)));
  DEBUG ((EFI_D_INFO, "BDSM     : %x\n", MmioRead32 (MchBase + R_SA_BDSM)));
  DEBUG ((EFI_D_INFO, "BGSM     : %x\n", MmioRead32 (MchBase + R_SA_BGSM)));
  DEBUG ((EFI_D_INFO, "DPR      : %x\n", MmioRead32 (MchBase + R_SA_DPR)));
  DEBUG ((EFI_D_INFO, "MESEG_B  : %lx\n", MmioRead64 (MchBase + R_SA_MESEG_BASE)));
  DEBUG ((EFI_D_INFO, "MESEG_M  : %lx\n", MmioRead64 (MchBase + R_SA_MESEG_MASK)));
  DEBUG ((EFI_D_INFO, "REMAPBASE: %lx\n", MmioRead64 (MchBase + R_SA_REMAPBASE)));
  DEBUG ((EFI_D_INFO, "REMAPLIM : %lx\n", MmioRead64 (MchBase + R_SA_REMAPLIMIT)));
  DEBUG ((EFI_D_INFO, "SMRAMC   : %02x\n", MmioRead8 (MchBase + R_SA_SMRAMC)));
  DEBUG ((EFI_D_INFO, "GGC      : %04x\n", MmioRead16 (MchBase + R_SA_GGC)));
}

/**
  Show Hexadecimal number by debug macro

  @param[in] Ptr    - Pointer to buffer of number that will be showed
  @param[in] Length - The length of the number buffer
**/
VOID
PrintHex (
  IN     UINT8   *Ptr,
  IN     UINTN   Length
  )
{
  while (Length--) {
    DEBUG ((EFI_D_INFO, "%02x", *Ptr++));
  }

  DEBUG ((EFI_D_INFO, "\n"));
}
#endif

/**
  Retrieves SMRAM areas ffrom SMRAM HOB, and places the ranges found in
  SmRamRanges array.

  @param[out] SmRamRanges       - Output buffer
  @param[in] SmRamRangesSize   - Number of entries in output buffer

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND
  @retval EFI_BUFFER_TOO_SMALL
**/
STATIC
EFI_STATUS
GetSmRamRanges (
  OUT     MEMORY_RANGE *SmRamRanges,
  IN      UINTN        SmRamRangesSize,
  IN      UINT32       IedSize
  )
{
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *SmmDescBlock;
  UINTN                           Index;
  VOID                            *Hob;

  Hob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (Hob == NULL) {
    DEBUG ((EFI_D_ERROR, "SMRAM HOB not found!\n"));
    ASSERT (0);
    return EFI_NOT_FOUND;
  }

  SmmDescBlock = (VOID *) ((UINT8 *) Hob + sizeof (EFI_HOB_GUID_TYPE));

  DEBUG ((EFI_D_INFO, "Found %d SMM ranges\n", SmmDescBlock->NumberOfSmmReservedRegions));

  ASSERT (SmRamRangesSize > SmmDescBlock->NumberOfSmmReservedRegions);
  if (SmRamRangesSize <= SmmDescBlock->NumberOfSmmReservedRegions) {
    return EFI_BUFFER_TOO_SMALL;
  }

  for (Index = 0; Index < SmmDescBlock->NumberOfSmmReservedRegions; ++Index) {
    SmRamRanges[Index].Start  = SmmDescBlock->Descriptor[Index].PhysicalStart;
    if (SmRamRanges[Index].Start > 0x100000) {
      //
      // This is TSEG SMRAM range.
      // IED region is also part of SMRAM but not reported in SMRAM Hob so here include IED for Rapid Start saving/restoring.
      //
      DEBUG ((EFI_D_INFO, "Ied size %X\n", IedSize));
      SmRamRanges[Index].End    = SmRamRanges[Index].Start + SmmDescBlock->Descriptor[Index].PhysicalSize + IedSize;
    }
  }

  SmRamRanges[Index].Start = END_RANGES;
  return EFI_SUCCESS;
}

/**
  Retrieves DPR areas from SA register, and places the ranges found in
  DprRanges array.

  @param[out] DprRanges - Output buffer

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND
**/
STATIC
EFI_STATUS
GetDprRanges (
  OUT MEMORY_RANGE *DprRanges  
  )
{
  EFI_STATUS Status;
  UINTN      Index;
  UINTN      MchBase;
  UINT32     DprData;
  UINT32     DprSize;
  
  Status = EFI_NOT_FOUND;
  Index  = 0;

  MchBase = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);
  DprData = MmioRead32 (MchBase + R_SA_DPR);
  DprSize = (DprData & V_DPR_DPRSIZE_MASK) << (N_SA_DPR_TOPOFDPR_OFFSET - N_DPR_DPRSIZE_OFFSET);

  ///
  /// Check whether DPR protection and DPR is enabled or not.
  ///
  if ((DprData & (B_SA_DPR_PRS_MASK + B_SA_DPR_EPM_MASK)) == (B_SA_DPR_PRS_MASK + B_SA_DPR_EPM_MASK)) {
    DprRanges[Index].Start = (DprData & B_SA_DPR_TOPOFDPR_MASK) - DprSize;
    DprRanges[Index].End   = DprData & B_SA_DPR_TOPOFDPR_MASK;
    DEBUG ((EFI_D_INFO, "DPR Start = %08lx, End = %08lx\n", DprRanges[Index].Start, DprRanges[Index].End));

    Index ++;
    Status = EFI_SUCCESS;
  }

  DprRanges[Index].Start = END_RANGES;
  return Status;
}

/**
  Constructs memory ranges to be saved/restored by RapidStart.
  Reserved areas are excluded.

  @param[out] OutMemoryRanges       - Output buffer
  @param[in] OutMemoryRangesSize   - Number of entries in output buffer
  @param[in] RapidStartData        - A data buffer stored RapidStart internal non-volatile information.

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
BuildMemoryRanges (
  OUT    MEMORY_RANGE                 *OutMemoryRanges,
  IN     UINTN                        OutMemoryRangesSize,
  IN     RAPID_START_PERSISTENT_DATA  *RapidStartData
  )
{
  MEMORY_RANGE  MainRanges[4];
  MEMORY_RANGE  ResvRanges[3];
  EFI_STATUS    Status;
  UINTN         Index;

  ///
  /// Always exclude Legacy SMRAM ranges (A0000~BFFFF) since this range can not perform DMA.
  /// If Legacy SMRAM is used it will be handled separately.
  ///
  Index                   = 0;
  MainRanges[Index].Start = 0;
  MainRanges[Index].End   = LEGACY_SMRAM_BASE;
  ++Index;
  MainRanges[Index].Start = LEGACY_SMRAM_BASE + LEGACY_SMRAM_SIZE;
  MainRanges[Index].End   = RapidStartData->Tolm;
  ++Index;
  if (RapidStartData->Tohm > MEM_EQU_4GB) {
    MainRanges[Index].Start = MEM_EQU_4GB;
    MainRanges[Index].End   = RapidStartData->Tohm;
    ++Index;
  }

  ASSERT (Index < VARIABLE_SIZE (MainRanges));
  MainRanges[Index].Start = END_RANGES;

  Index                   = 0;
  ResvRanges[Index].Start = RapidStartData->RapidStartMem;
  ResvRanges[Index].End   = RapidStartData->RapidStartMem + RapidStartData->RapidStartMemSize;
  ++Index;
#ifdef RAPID_START_ON_MEMORY_INSTALLED
  ResvRanges[Index].Start = RapidStartData->AcpiReservedMemoryBase;
  ResvRanges[Index].End   = RapidStartData->AcpiReservedMemoryBase + RapidStartData->AcpiReservedMemorySize;
  ++Index;
#endif
  ASSERT (Index < VARIABLE_SIZE (ResvRanges));
  ResvRanges[Index].Start = END_RANGES;

  ///
  /// Exclude reserved ranges
  ///
  Status = SubtractRanges (
            OutMemoryRanges,
            OutMemoryRangesSize,
            MainRanges,
            ResvRanges
            );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

#ifdef RAPID_START_NO_SMRAM_INTEGRITY_CHECK
#define HASHING_CONTEXT                typedef enum { EMPTY }
#define HashingInit(c, r)               EFI_SUCCESS
#define HashingProcessChunk(c)          TRUE
#define HashingCompleteAndVerify(c, t)  EFI_SUCCESS
#else

typedef struct {
  MEMORY_RANGE  *Range;
  UINT8         *Ptr;
  VOID          *HashState;
} HASHING_CONTEXT;

#define HASHING_CHUNK (4 * EFI_PAGE_SIZE)

/**
  Initializes secure hash algorithm.

  @param[in] Context  - Hashing context
  @param[in] Ranges   - Memory ranges to be included in resulting hash

  @retval EFI_SUCCESS         Hashing process completed
  @retval ERROR               Return the error code if PeiServicesAllocatePool () failed
**/
STATIC
EFI_STATUS
HashingInit (
  HASHING_CONTEXT *Context,
  MEMORY_RANGE    *Ranges
  )
{
  UINTN       HashStatetSize;
  EFI_STATUS  Status;

  Context->Range  = Ranges;
  Context->Ptr    = (VOID *) (UINTN) Ranges->Start;

  HashStatetSize  = IfsSecureHashGetContextSize ();
  ASSERT (HashStatetSize != 0);

  Status = PeiServicesAllocatePool (HashStatetSize, (VOID **)&Context->HashState);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  RapidStartSecureHashInit (Context->HashState);
  return EFI_SUCCESS;
}

/**
  Processes next chunk of memory.

  @param[in] Context  - Hashing context

  @retval TRUE if hashing complete, FALSE othervise.
**/
STATIC
BOOLEAN
HashingProcessChunk (
  HASHING_CONTEXT *Context
  )
{
  UINTN Size;
  if (Context->Range->Start == END_RANGES) {
    return TRUE;
  }

  DEBUG ((EFI_D_INFO, "#"));
  Size = (UINTN) Context->Range->End - (UINTN) Context->Ptr;
  if (Size > HASHING_CHUNK) {
    Size = HASHING_CHUNK;
  }

  RapidStartSecureHashUpdate (Context->HashState, (VOID *) Context->Ptr, Size);
  Context->Ptr += Size;
  if ((UINTN) Context->Ptr == (UINTN) Context->Range->End) {
    ++Context->Range;
    Context->Ptr = (VOID *) (UINTN) Context->Range->Start;
  }

  return FALSE;
}

/**
  Finishes hashing and saves/verifies resulting value depending on RapidStart transition.

  @param[in] Context    - Hashing context
  @param[in] Instance   - RapidStart instance

  @retval EFI_SECURITY_VIOLATION - SMRAM content has changed.
  @retval EFI_SUCCESS            - SMRAM content no change.
**/
STATIC
EFI_STATUS
HashingCompleteAndVerify (
  HASHING_CONTEXT        *Context,
  RAPID_START_INSTANCE   *Instance
  )
{
  UINT8       Hash[RAPID_START_SECURE_HASH_LENGTH];
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "HashingCompleteAndVerify...\n"));

  Status = EFI_SUCCESS;
  while (!HashingProcessChunk (Context)) {
  }

  RapidStartSecureHashFinal (Context->HashState, Hash);

#ifdef EFI_DEBUG
  PrintHex (Hash, RAPID_START_SECURE_HASH_LENGTH);
#endif

  if (Instance->Transition == RapidStartExit) {
    if (CompareMem (Hash, Instance->SmRamHash, RAPID_START_SECURE_HASH_LENGTH) != 0) {
      DEBUG ((EFI_D_ERROR, "Error: SMRAM content has changed!\n"));
      Status = EFI_SECURITY_VIOLATION;
    }

    ZeroMem (Instance->SmRamHash, RAPID_START_SECURE_HASH_LENGTH);
  } else {
    Status = RapidStartSaveSecureHash (Hash);
    ASSERT_EFI_ERROR (Status);
  }

  ZeroMem (Hash, RAPID_START_SECURE_HASH_LENGTH);
  ZeroMem (Context->HashState, IfsSecureHashGetContextSize ());

  return Status;
}

#endif

#ifdef ME_SUPPORT_FLAG
/**
  Check ME status and wait for ME init done

  @retval EFI_SUCCESS - ME Init done
  @retval EFI_TIMEOUT - ME is not ready after timeout occurred
  @retval EFI_DEVICE_ERROR - Error reported by ME F/W
**/
STATIC
EFI_STATUS
WaitMeInitDone (
  VOID
  )
{
  EFI_PEI_SERVICES  **PeiServices;
  PEI_HECI_PPI      *HeciPpi;
  EFI_STATUS        Status;
  UINT32            MeStatus;
  UINT32            MeMode;
  UINTN             Timeout;

  DEBUG ((EFI_D_INFO, "WaitMeInitDone()\n"));

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            &gEfiPeiHeciPpiGuid,
                            0,
                            NULL,
                            (VOID **) &HeciPpi
                            );
  ASSERT_EFI_ERROR (Status);

  Status = HeciPpi->GetMeMode ((CONST EFI_PEI_SERVICES **)PeiServices, &MeMode);
  ASSERT_EFI_ERROR (Status);
  ///
  /// If ME is in ME_DEBUG mode, return success.
  ///
  if (MeMode == ME_MODE_DEBUG) {
    return EFI_SUCCESS;
  }

  if (MeMode != ME_MODE_NORMAL) {
    return EFI_DEVICE_ERROR;
  }

  Timeout = 0;
  while (Timeout < ME_FID_TIMEOUT) {
    Status = HeciPpi->GetMeStatus ((CONST EFI_PEI_SERVICES **)PeiServices, &MeStatus);
    ASSERT_EFI_ERROR (Status);

    if (ME_STATUS_IS_ME_FW_INIT_COMPLETE (MeStatus)) {
      return EFI_SUCCESS;;
    }

    PchPmTimerStall (1000);
    Timeout++;
  }

  DEBUG ((EFI_D_ERROR, "ME init timeout!\n"));
  return EFI_TIMEOUT;
}
#endif // ME_SUPPORT_FLAG

/**
  Calls Func for each port in PortMap.

  @param[in] Ahci     - SATA controller information structure
  @param[in] Func     - Pointer to function to be called with Ahci and port number passed as arguments
  @param[in] PortMap  - Bitmap of ports for wich Func is to be called
**/
STATIC
VOID
ForEachAhciPort (
  IN     AHCI_CONTEXT *Ahci,
  IN     EFI_STATUS (*Func) (AHCI_CONTEXT*, UINTN),
  IN     UINT32 PortMap
  )
{
  UINTN       Port;

  for (Port = 0; Port <= 31; ++Port) {
    if (PortMap & (1u << Port)) {
      Func (Ahci, Port);
    }
  }
  return;
}

/**
  Issues STANDBY_IMMEDIATE command to given SATA port.
  Port is initialized first and disabled afterwards.

  @param Ahci     - SATA controller information structure
  @param Port     - Port number to issue command to

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - AHCI port initialization failed or Command failed
**/
STATIC
EFI_STATUS
StandbyAhciPort (
  IN     AHCI_CONTEXT  *Ahci,
  IN     UINTN         Port
  )
{
  AHCI_CONTEXT AhciTmp;
  EFI_STATUS   Status;
  DEBUG ((EFI_D_INFO, "StandbyAhciPort(%d)\n", Port));

  AhciTmp = *Ahci;
  AhciTmp.Port = Port;
  Status = AhciPortInit (&AhciTmp);
  if (Status == EFI_SUCCESS) {
    Status = AhciSimpleCommand (&AhciTmp, ATA_CMD_STANDBY_IMMEDIATE);
  }
  AhciPortDone (&AhciTmp);
  return Status;
}

/**
  Initialize Ahci port for RapidStart accessing SSD in early stage.

  @param[in] Instance  - Instance contains required data and function for RapidStart
**/
STATIC
VOID
RapidStartInitAhci (
  IN     RAPID_START_INSTANCE *Instance
  )
{
  AHCI_CONTEXT                *Ahci;
  RAPID_START_PERSISTENT_DATA *RapidStartData;

  DEBUG ((EFI_D_INFO, "InitAhci\n"));

  Ahci            = &Instance->Ahci;
  RapidStartData  = &Instance->Data;

  Ahci->Abar      = (UINT32) RapidStartData->MmioSpace;
  Ahci->PortBase  = (UINT32) RapidStartData->RapidStartMem;
  Ahci->PortSize = RapidStartData->RapidStartMemSize -
    sizeof (RAPID_START_MEM_DATA) -
    RapidStartData->ZeroBitmapSize -
    RapidStartData->Crc32RecordSize;
  Ahci->Port = RapidStartData->StoreSataPort;

  DEBUG ((EFI_D_INFO, "Ahci.Abar = %08x\n", Ahci->Abar));
  DEBUG ((EFI_D_INFO, "Ahci.Port = %d\n", Ahci->Port));
  DEBUG ((EFI_D_INFO, "Ahci.PortBase = %08x\n", Ahci->PortBase));
  DEBUG ((EFI_D_INFO, "Ahci.PortSize = %08x\n", Ahci->PortSize));

  ///
  /// Restore SATA ports configuration
  ///
  MmioWrite8 (
    MmPciAddress (
    0,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SATA,
    PCI_FUNCTION_NUMBER_PCH_SATA,
    R_PCH_SATA_PCS),
    RapidStartData->SataPortConfiguration
    );

  AhciInit (Ahci);
  AhciSpinUp (Ahci);
}

#define RETURN_ON_ERROR(expr) \
  do { \
    EFI_STATUS  _status; \
    _status = (expr); \
    if (EFI_ERROR (_status)) { \
      return _status; \
    } \
  } while (0)

/**
  Transfer SMRAM ranges to/from RapidStart partition.

  Legacy SMRAM 0xA0000-0xC0000 is handled indirectly.

  @param[in] Ahci        - AHCI context
  @param[in,out] AtaCmd - ATA command structure
  @param[in] Transition  - RapidStart transition
  @param[in] SmRamRanges - Array of SMRAM ranges

  @retval EFI_STATUS  - the error code returned by AhciPostCommand () if failed.
  @retval EFI_SUCCESS - Hasing SMRAM process completed.
**/
STATIC
EFI_STATUS
RapidStartHandleSmRam (
  IN     AHCI_CONTEXT            *Ahci,
  IN OUT ATA_COMMAND             *AtaCmd,
  IN     RAPID_START_TRANSITION  Transition,
  IN     MEMORY_RANGE            *SmRamRanges
  )
{
  MEMORY_RANGE  *Range;
  UINTN         CmdMask;

  DEBUG ((EFI_D_INFO, "SMRAM handling...\n"));
  PrintRanges (SmRamRanges);

  if (Transition == RapidStartEntry) {
    ///
    /// Ensure original LEGACY_SMRAM_BUFFER data has been stored into RapidStart Store before we modifying it.
    ///
    RETURN_ON_ERROR (AhciWaitAllComplete (Ahci));
    CopyMem ((VOID *) LEGACY_SMRAM_BUFFER, (VOID *) LEGACY_SMRAM_BASE, (UINTN) LEGACY_SMRAM_SIZE);
  }

  CmdMask = 0;
  Range   = SmRamRanges;
  while (Range->Start != END_RANGES) {
    AtaCmd->SectorCount = MEM_TO_SECT (RANGE_LENGTH (Range));
    if (Range->Start == LEGACY_SMRAM_BASE) {
      ASSERT (Range->End == LEGACY_SMRAM_BASE + LEGACY_SMRAM_SIZE);
      AtaCmd->MemAddr = LEGACY_SMRAM_BUFFER;
      RETURN_ON_ERROR (AhciPostCommand (Ahci, AtaCmd, &CmdMask));
    } else {
      AtaCmd->MemAddr = Range->Start;
      CmdMask         = ~CmdMask;
      RETURN_ON_ERROR (AhciPostCommand (Ahci, AtaCmd, &CmdMask));
      CmdMask = ~CmdMask;
    }

    ++Range;
  }

  AhciWaitComplete (Ahci, CmdMask);
  if (Transition == RapidStartExit) {
    CopyMem ((VOID *) LEGACY_SMRAM_BASE, (VOID *) LEGACY_SMRAM_BUFFER, (UINTN) LEGACY_SMRAM_SIZE);
  }

  return EFI_SUCCESS;
}

/**
  Performs AHCI commands required to save or restore memory content.

  @param[in] Instance    - RapidStart instance
  @param[in] MainRanges  - Ranges of memory to be transferred
  @param[in] SmRamRanges - SMRAM ranges, to by handled by RapidStartHandleSmRam

  @retval EFI_SUCCESS          - RapidStart transition succeeded
  @retval EFI_TIMEOUT          - AHCI timeout occured
  @retval EFI_DEVICE_ERROR     - AHCI device not ready
  @retval EFI_NOT_STARTED      - Transition cancellation happened, abort the transition.
  @retval EFI_NO_MEDIA         - No RapidStart Store available or RapidStart store has been changed. Abort RapidStart transition.
  @retval EFI_BAD_BUFFER_SIZE  - Memory configuration changed. Abort RapidStart transition.
**/
STATIC
EFI_STATUS
RapidStartMemoryDiskTransfer (
  IN     RAPID_START_INSTANCE  *Instance,
  IN     MEMORY_RANGE          *MainRanges,
  IN     MEMORY_RANGE          *SmRamRanges
  )
{
  RAPID_START_PERSISTENT_DATA *RapidStartData;
  AHCI_CONTEXT                *Ahci;
  RAPID_START_TRANSITION      Transition;
  ATA_COMMAND                 AtaCmd;
  MEMORY_RANGE                *MemRanges;
  MEMORY_RANGE                *Range;
  EFI_STATUS                  Status;
  UINT32                      *ZeroPageBitMap;
  UINT32                      SectorsLeft;
  HASHING_CONTEXT             Hashing;
  BOOLEAN                     HashingDone;
  UINT64                      DataCount;
  UINT32                      *SmramBuffer;
  UINT32                      ZeroBitmapForSmramBuffer;
  UINT64                      LbaForData;
  UINT64                      SmRamBufferLba;
  EFI_STATUS                  GfxDisplayStatus;
  PEI_GFX_PPI                 *PeiGfxPpi;
  GFX_RESTORE_MEM_TABLE       *GfxRestoreMemTable;
  GFX_RESTORE_MEM_TABLE       *FbRestoreMemTable;
  UINT32                      Index, GfxIndex, MemIndex;
  MEMORY_RANGE                *GfxResvRanges;
  MEMORY_RANGE                *TempMemRanges;
  UINT64                      PageAlignedDssAddr;
  UINT64                      PageAlignedDssSize;

  MemRanges = (MEMORY_RANGE *) AllocatePool (sizeof(MEMORY_RANGE) * (MAX_MEMORY_RANGES + 1));
  if (MemRanges == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for MemRanges! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  GfxResvRanges = (MEMORY_RANGE *) AllocatePool (sizeof(MEMORY_RANGE) * (MAX_GFX_MEMORY_RANGES + 1));
  if (GfxResvRanges == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for GfxResvRanges! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  TempMemRanges = (MEMORY_RANGE *) AllocatePool (sizeof(MEMORY_RANGE) * (MAX_MEMORY_RANGES + 1));
  if (GfxResvRanges == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for TempMemRanges! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  RapidStartData  = &Instance->Data;
  Ahci            = &Instance->Ahci;
  Transition      = Instance->Transition;

  ///
  /// Exclude SMRAM ranges as they are handled separetely
  ///
  Status = SubtractRanges (
            MemRanges,
            VARIABLE_SIZE (MemRanges),
            MainRanges,
            SmRamRanges
            );
  ASSERT_EFI_ERROR (Status);

  AtaCmd.Auxiliary.Data = 0;
  if (Transition == RapidStartEntry) {
    if (RapidStartData->HybridHardDisk == 1) {
      Status = AhciHybridHardDiskSupport (Ahci);
      if (Status == EFI_SUCCESS) {
        if (Ahci->TotalRemainingCacheCapacityInSector < RapidStartData->StoreSectors) {
          return EFI_BAD_BUFFER_SIZE;
        }
        AtaCmd.Auxiliary.r.HybridPriority  = Ahci->HybridInfo.HybridPriority;
        AtaCmd.Auxiliary.r.HybridInfoValid = 1;
      } else if (Status != EFI_UNSUPPORTED) {
          return Status;
      }
    }

    Ahci->PollCancellation  = TRUE;
    AtaCmd.Command          = ATA_CMD_WRITE_DMA_EXT;
    AtaCmd.Direction        = AHCI_DIR_HOST2DEV;
  } else {
    Ahci->PollCancellation  = FALSE;
    AtaCmd.Command          = ATA_CMD_READ_DMA_EXT;
    AtaCmd.Direction        = AHCI_DIR_DEV2HOST;
    if (RapidStartData->HybridHardDisk == 1) {
      if (((Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_SUPPORT] & B_ATA_ID_DEV_HYBRID_FEATURE_SUPPORT) &
           (Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_ENABLE] & B_ATA_ID_DEV_HYBRID_FEATURE_ENABLE))
         ) {
        AtaCmd.Auxiliary.r.HybridPriority  = 0;
        AtaCmd.Auxiliary.r.HybridInfoValid = 1;
      }
    }
  }

  AtaCmd.Feature = 0;

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
  ///
  /// Save CRC32 for memory below 4GB
  ///
  if (Transition == RapidStartEntry) {
    SaveOrCompareCrc32 (FALSE, 0, RapidStartData->Tolm, RapidStartData);
  }
#endif
  ///
  /// Head sector store the Zero Page Bitmap, so save/restore it first
  ///
  ZeroPageBitMap = RAPID_START_ZERO_PAGE_BITMAP_PTR (RapidStartData);
  ///
  /// For Entry, store RapidStart Store UID in the head of Zero Page Bitmap.
  ///
  if (Transition == RapidStartEntry) {
    *(UINT64 *) (ZeroPageBitMap) = RapidStartData->RapidStartStoreUid;
  }

  AtaCmd.MemAddr      = (UINTN) ZeroPageBitMap;
  AtaCmd.Lba          = RapidStartData->StoreLbaAddr;
  AtaCmd.SectorCount  = MEM_TO_SECT (RapidStartData->ZeroBitmapSize + RapidStartData->Crc32RecordSize);
  RETURN_ON_ERROR (AhciPostCommand (Ahci, &AtaCmd, NULL));

  LbaForData = RapidStartData->StoreLbaAddr + MEM_TO_SECT (RapidStartData->ZeroBitmapSize + RapidStartData->Crc32RecordSize);
  GfxDisplayStatus = EFI_UNSUPPORTED;
  if (RapidStartData->DssAddress != (UINT64)NULL) {
    PageAlignedDssAddr  = RapidStartData->DssAddress & ~EFI_PAGE_MASK;
    PageAlignedDssSize  = (EFI_SIZE_TO_PAGES(PageAlignedDssAddr) < EFI_SIZE_TO_PAGES(RapidStartData->DssAddress) ) ? EFI_PAGE_SIZE : 0;
    PageAlignedDssSize  += RapidStartData->DssSize;
    AtaCmd.MemAddr      = (UINTN) PageAlignedDssAddr;
    AtaCmd.Lba          = LbaForData;
    AtaCmd.SectorCount  = MEM_TO_SECT (PageAlignedDssSize);
    RETURN_ON_ERROR (AhciPostCommand (Ahci, &AtaCmd, NULL));
    LbaForData += MEM_TO_SECT (PageAlignedDssSize);

    GfxIndex                      = 0;
    GfxResvRanges[GfxIndex].Start = PageAlignedDssAddr;
    GfxResvRanges[GfxIndex].End   = PageAlignedDssAddr + PageAlignedDssSize;
    ++GfxIndex;

    //
    // save/restore GTT, GMM... ets Gfx memory related Range
    //
    GfxDisplayStatus = PeiServicesLocatePpi (&gPeiGfxPpiGuid, 0, NULL, (VOID **)&PeiGfxPpi);
    ASSERT_EFI_ERROR (GfxDisplayStatus);
    if (!EFI_ERROR (GfxDisplayStatus)) {
      if (Transition == RapidStartExit) {
        RETURN_ON_ERROR (AhciWaitAllComplete (Ahci));
      }
      GfxDisplayStatus = PeiGfxPpi->GetRestoreMemTable (GetPeiServicesTablePointer (), &GfxRestoreMemTable);
      DEBUG ((EFI_D_ERROR, "Gfx GetRestoreMemTable Status = %r, NumberofEntry = %x\n", GfxDisplayStatus, GfxRestoreMemTable->NumOfEntry));
      if (!EFI_ERROR (GfxDisplayStatus) && (GfxRestoreMemTable->NumOfEntry <= GFX_RESTORE_RANGE)) {
        for (Index = 0; Index < GfxRestoreMemTable->NumOfEntry; Index++) {
          DEBUG ((EFI_D_ERROR, "Gfx related memory Ranges [%x] - BaseAddress is %lx, Size is %lx\n", Index, GfxRestoreMemTable->MemRangeEntry[Index].BaseAddress, GfxRestoreMemTable->MemRangeEntry[Index].Size));

          GfxResvRanges[GfxIndex].Start = GfxRestoreMemTable->MemRangeEntry[Index].BaseAddress;
          GfxResvRanges[GfxIndex].End   = GfxRestoreMemTable->MemRangeEntry[Index].BaseAddress + GfxRestoreMemTable->MemRangeEntry[Index].Size;

          MemIndex = 0;
          while (MemRanges[MemIndex].Start != END_RANGES) {
            if (GfxResvRanges[GfxIndex].Start >= MemRanges[MemIndex].Start && GfxResvRanges[GfxIndex].End <= MemRanges[MemIndex].End) {
              DEBUG ((EFI_D_ERROR, "Gfx related memory Ranges [%x] - BaseAddress is %lx, Size is %lx has been save(restore).\n", Index, GfxRestoreMemTable->MemRangeEntry[Index].BaseAddress, GfxRestoreMemTable->MemRangeEntry[Index].Size));

              AtaCmd.MemAddr     = (UINTN) GfxRestoreMemTable->MemRangeEntry[Index].BaseAddress;
              AtaCmd.Lba         = LbaForData;
              AtaCmd.SectorCount = MEM_TO_SECT (GfxRestoreMemTable->MemRangeEntry[Index].Size);
              RETURN_ON_ERROR (AhciPostCommand (Ahci, &AtaCmd, NULL));
              LbaForData += MEM_TO_SECT (GfxRestoreMemTable->MemRangeEntry[Index].Size);
              ++GfxIndex;
              break;
            }
            ++MemIndex;
          }
        }
      }
      //
      // save/restore Gfx Frame Buffer Ranges
      //
      if (RapidStartData->DisplayType == 1 && !EFI_ERROR (GfxDisplayStatus)) {
        if (Transition == RapidStartExit) {
          RETURN_ON_ERROR (AhciWaitAllComplete (Ahci));
        }
        GfxDisplayStatus = PeiGfxPpi->GetRestoreFbRange (GetPeiServicesTablePointer (), &FbRestoreMemTable);
        DEBUG ((EFI_D_ERROR, "Gfx GetRestoreFbRange Status = %r, NumberofEntry = %x\n", GfxDisplayStatus, FbRestoreMemTable->NumOfEntry));
        if (!EFI_ERROR (GfxDisplayStatus) && (FbRestoreMemTable->NumOfEntry <= FB_RESTORE_RANGE)) {
          for (Index = 0; Index < FbRestoreMemTable->NumOfEntry; Index++) {
            DEBUG ((EFI_D_ERROR, "Gfx FB memory Ranges [%x] - BaseAddress is %lx, Size is %lx\n", Index, FbRestoreMemTable->MemRangeEntry[Index].BaseAddress, FbRestoreMemTable->MemRangeEntry[Index].Size));

            GfxResvRanges[GfxIndex].Start = FbRestoreMemTable->MemRangeEntry[Index].BaseAddress;
            GfxResvRanges[GfxIndex].End   = FbRestoreMemTable->MemRangeEntry[Index].BaseAddress + FbRestoreMemTable->MemRangeEntry[Index].Size;

            MemIndex = 0;
            while (MemRanges[MemIndex].Start != END_RANGES) {
              if (GfxResvRanges[GfxIndex].Start >= MemRanges[MemIndex].Start && GfxResvRanges[GfxIndex].End <= MemRanges[MemIndex].End) {
                DEBUG ((EFI_D_ERROR, "Gfx FB memory Ranges [%x] - BaseAddress is %lx, Size is %lx has been save(restore).\n", Index, FbRestoreMemTable->MemRangeEntry[Index].BaseAddress, FbRestoreMemTable->MemRangeEntry[Index].Size));

                AtaCmd.MemAddr     = (UINTN) FbRestoreMemTable->MemRangeEntry[Index].BaseAddress;
                AtaCmd.Lba         = LbaForData;
                AtaCmd.SectorCount = MEM_TO_SECT (FbRestoreMemTable->MemRangeEntry[Index].Size);
                RETURN_ON_ERROR (AhciPostCommand (Ahci, &AtaCmd, NULL));
                LbaForData += MEM_TO_SECT (FbRestoreMemTable->MemRangeEntry[Index].Size);
                ++GfxIndex;
                break;
              }
              ++MemIndex;
            }
          }
        }
      }
      ASSERT (GfxIndex < VARIABLE_SIZE (GfxResvRanges));
      GfxResvRanges[GfxIndex].Start = END_RANGES;

      //
      // Copy MemRanges to TempMemRanges
      //
      GfxDisplayStatus = SubtractRanges (
                          TempMemRanges,
                          VARIABLE_SIZE (TempMemRanges),
                          MainRanges,
                          SmRamRanges
                          );
      ASSERT_EFI_ERROR (GfxDisplayStatus);

      //
      // Exclude Gfx related memory ranges as they are handled separetely
      //
      GfxDisplayStatus = SubtractRanges (
                          MemRanges,
                          VARIABLE_SIZE (MemRanges),
                          TempMemRanges,
                          GfxResvRanges
                          );
      ASSERT_EFI_ERROR (GfxDisplayStatus);

    }
  }

  if (RapidStartData->DssAddress != (UINT64)NULL) {
    if (!EFI_ERROR (GfxDisplayStatus)) {
      switch(Transition) {
        case RapidStartEntry:
          DEBUG ((EFI_D_INFO, "Calling GfxDisplay in RapidStart entry flow...\n"));
//        GfxDisplayStatus = RapidStartGfxDisplayScreen();
          break;
        case RapidStartExit:
          DEBUG ((EFI_D_INFO, "Calling GfxDisplay in RapidStart exit flow...\n"));
          if (RapidStartUnattendedWake() == FALSE) {
            GfxDisplayStatus = RapidStartGfxDisplayScreen();
          }
          break;
        default:
          break;
      }
    }
  }

  if (Transition == RapidStartExit) {
    AtaCmd.Lba = LbaForData;
    if ((RapidStartHandleSmRam (Ahci, &AtaCmd, Transition, SmRamRanges) == EFI_DEVICE_ERROR) ||
        (AhciWaitAllComplete (Ahci) == EFI_DEVICE_ERROR)
        ) {
      DEBUG (
        (
        EFI_D_ERROR,
        "\nEFI_DEVICE_ERROR in first DMA command, probably RapidStart Store changed. Aborted RapidStart resume.\n"
        )
        );
      RapidStartSetFlag (RAPID_START_FLAG_STORE_CHANGE);
      return EFI_NO_MEDIA;
    }

    if (*(UINT64 *) (ZeroPageBitMap) != RapidStartData->RapidStartStoreUid) {
      DEBUG ((EFI_D_ERROR, "\nRapidStart Store UID Mismatched! Aborted RapidStart resume.\n"));
      RapidStartSetFlag (RAPID_START_FLAG_STORE_CHANGE);
      return EFI_NO_MEDIA;
    }
  }

  ZeroPageBitMap += 2;
  DEBUG ((EFI_D_INFO, "Transfer ranges:\n"));
  PrintRanges (MemRanges);

  RETURN_ON_ERROR (HashingInit (&Hashing, SmRamRanges));

  DataCount       = 0;

  HashingDone     = FALSE;
  AtaCmd.Lba      = LbaForData + MEM_TO_SECT (SumRanges (SmRamRanges));
  Range           = MemRanges;
  SmRamBufferLba  = AtaCmd.Lba;
  ///
  /// Log the last Lba which not in used.
  ///
  Instance->NotUsedLba = AtaCmd.Lba;
  PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2510);
  while (Range->Start != END_RANGES) {
    AtaCmd.MemAddr  = Range->Start;
    SectorsLeft     = MEM_TO_SECT (RANGE_LENGTH (Range));
    while (SectorsLeft > 0) {
      if (HashingDone || AhciHasFreeCmdSlot (Ahci)) {
        if (SectorsLeft > AHCI_MAX_SECTORS) {
          AtaCmd.SectorCount = AHCI_MAX_SECTORS;
        } else {
          AtaCmd.SectorCount = SectorsLeft;
        }

        SectorsLeft -= AtaCmd.SectorCount;
        RETURN_ON_ERROR (AhciPostCommandWithZeroFilter (Ahci, &AtaCmd, NULL, ZeroPageBitMap, &SmRamBufferLba));
        DataCount += AtaCmd.SectorCount << SECTOR_SHIFT;
      } else {
        if (Ahci->PollCancellation && RapidStartShouldCancelEntry ()) {
          return EFI_NOT_STARTED;
        }

        HashingDone = HashingProcessChunk (&Hashing);
      }
      ///
      /// Log the last Lba which not in used.
      ///
      Instance->NotUsedLba = AtaCmd.Lba;
    }

    ++Range;
  }

  RETURN_ON_ERROR (HashingCompleteAndVerify (&Hashing, Instance));
  PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2511);

  if (Transition == RapidStartEntry) {
    Ahci->PollCancellation  = FALSE;
    AtaCmd.Lba              = LbaForData;
    RETURN_ON_ERROR (RapidStartHandleSmRam (Ahci, &AtaCmd, Transition, SmRamRanges));
    ///
    /// Restore LEGACY_SMRAM_BUFFER
    ///
    AtaCmd.Command      = ATA_CMD_READ_DMA_EXT;
    AtaCmd.Direction    = AHCI_DIR_DEV2HOST;
    AtaCmd.MemAddr      = LEGACY_SMRAM_BUFFER;
    AtaCmd.SectorCount  = MEM_TO_SECT (LEGACY_SMRAM_SIZE);
    AtaCmd.Lba          = SmRamBufferLba;
    RETURN_ON_ERROR (AhciPostCommandWithZeroFilter (Ahci, &AtaCmd, NULL, ZeroPageBitMap, NULL));
  }
  ///
  /// If LEGACY_SMRAM_BUFFER contained zero pages, they have to be zero-ed
  ///
  ZeroBitmapForSmramBuffer  = *(ZeroPageBitMap + (LEGACY_SMRAM_BUFFER / EFI_PAGE_SIZE / 8 / 4));
  SmramBuffer               = (UINT32 *) LEGACY_SMRAM_BUFFER;
  if (ZeroBitmapForSmramBuffer != 0) {
    while (SmramBuffer < (UINT32 *) (LEGACY_SMRAM_BUFFER + LEGACY_SMRAM_SIZE)) {
      if (ZeroBitmapForSmramBuffer & 1) {
        ZeroMem (SmramBuffer, EFI_PAGE_SIZE);
      }

      ZeroBitmapForSmramBuffer >>= 1;
      SmramBuffer += (EFI_PAGE_SIZE / 4);
    }
  }

  PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2520);
  Status = AhciWaitAllComplete (Ahci);
  PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2521);

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
  ///
  /// Verify CRC32 for memory below 4GB
  ///
  if (Transition == RapidStartExit) {
    SaveOrCompareCrc32 (TRUE, 0, RapidStartData->Tolm, RapidStartData);
  }
#endif

  return Status;
}

/**
  Save/Restore memory content to/from RapidStart Store

  @param[in] Instance    - RapidStart instance

  @retval EFI_SUCCESS          - RapidStart transition succeeded
  @retval EFI_BAD_BUFFER_SIZE  - Memory configuration changed. Abort RapidStart transition.
  @retval EFI_BUFFER_TOO_SMALL - RapidStart Store size is too small to store memory content
  @retval EFI_TIMEOUT          - AHCI timeout occured
  @retval EFI_DEVICE_ERROR     - AHCI device not ready
**/
STATIC
EFI_STATUS
RapidStartTransitionInternal (
  IN     RAPID_START_INSTANCE         *Instance
  )
{
  RAPID_START_PERSISTENT_DATA *RapidStartData;
  AHCI_CONTEXT                *Ahci;
  UINTN                       MchBase;
  UINT8                       OldSmramControl;
  UINT32                      OldTseg;
  UINT8                       OldPam[PAM_COUNT];
  MEMORY_RANGE                *MemRanges;
  MEMORY_RANGE                SmRamRanges[MAX_SMRAM_RANGES + 1];
  UINTN                       Index;
  EFI_STATUS                  Status;
  UINT32                      OldDpr;

  MemRanges = (MEMORY_RANGE *) AllocatePool (sizeof(MEMORY_RANGE) * (MAX_MEMORY_RANGES + 1));
  if (MemRanges == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for MemRanges! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  RapidStartData  = &Instance->Data;
  Ahci            = &Instance->Ahci;

#ifdef EFI_DEBUG
  DumpMcRegisters ();
#endif
  DEBUG ((EFI_D_INFO, "Mem  : %x\n", (UINT32) RapidStartData->RapidStartMem));
  DEBUG ((EFI_D_INFO, "Io   : %x\n", (UINT32) RapidStartData->MmioSpace));
  DEBUG (
    (EFI_D_INFO,
    "Store: port=%d start=%lx size=%x\n",
    RapidStartData->StoreSataPort,
    RapidStartData->StoreLbaAddr,
    RapidStartData->StoreSectors)
    );

  MchBase = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);
  if (RapidStartData->Tolm != (MmioRead32 (MchBase + R_SA_BDSM) & B_SA_BDSM_BDSM_MASK)) {
    DEBUG ((EFI_D_ERROR, "Memory Tolm changed\n"));
    return EFI_BAD_BUFFER_SIZE;
  }

  if (RapidStartData->Tohm != (MmioRead64 (MchBase + R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK)) {
    DEBUG ((EFI_D_ERROR, "Memory Tohm changed\n"));
    return EFI_BAD_BUFFER_SIZE;
  }

  DEBUG ((EFI_D_INFO, "Total Memory: %lx\n", RapidStartData->TotalMem));

  Status = GetSmRamRanges (SmRamRanges, VARIABLE_SIZE (SmRamRanges), Instance->IedSize);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SMRAM ranges:\n"));
  PrintRanges (SmRamRanges);

  Status = BuildMemoryRanges (
            MemRanges,
            VARIABLE_SIZE (MemRanges),
            RapidStartData
            );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "Memory ranges:\n"));
  PrintRanges (MemRanges);

  ///
  /// Save and open SMRAM
  ///
  OldSmramControl = MmioRead8 (MchBase + R_SA_SMRAMC);
  MmioWrite8 (MchBase + R_SA_SMRAMC, B_SA_SMRAMC_D_OPEN_MASK | B_SA_SMRAMC_G_SMRAME_MASK);

  ///
  /// Open PAM regions
  ///
  for (Index = 0; Index < PAM_COUNT; ++Index) {
    OldPam[Index] = MmioRead8 (MchBase + R_SA_PAM0 + Index);
    MmioWrite8 (MchBase + R_SA_PAM0 + Index, B_SA_PAM1_HIENABLE_MASK | B_SA_PAM1_LOENABLE_MASK);
  }
  ///
  /// Disable TSEG and DPR
  ///
  OldTseg = MmioRead32 (MchBase + R_SA_TSEGMB);
  MmioWrite32 (MchBase + R_SA_TSEGMB, RapidStartData->Tolm);
  OldDpr = MmioRead32 (MchBase + R_SA_DPR);
  MmioAnd32 (MchBase + R_SA_DPR, (UINT32) ~B_SA_DPR_EPM_MASK);

  PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2500);
  Status = AhciPortInit (Ahci);
  PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2501);

  //
  // Unlock device if password locked
  //
  PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2530);
  if (Status == EFI_SUCCESS) {
    Status = AhciGetLockStatus (Ahci);
    if (Status == EFI_ACCESS_DENIED) {
      if (Instance->Transition == RapidStartEntry) {
        Instance->PwdStatus = RapidStartGetDriveUnlockPassword (
                                GetPeiServicesTablePointer (),
                                Instance->Transition,
                                Instance->Data.StoreSataPort,
                                (UINT8 *) Instance->HddPassword,
//[-start-120628-IB05330351-modify]//
                                &Instance->FreezeLock,
                                &Instance->PasswordType
//[-end-120628-IB05330351-modify]//
                                );
      }

      if (Instance->PwdStatus == EFI_SUCCESS) {
//[-start-111024-IB04770172-modify]//
        Status = AhciSecurityUnlock (Ahci, Instance->HddPassword, Instance->PasswordType);
//[-end-111024-IB04770172-modify]//
        if (Instance->FreezeLock) {
          AhciSimpleCommand (Ahci, ATA_CMD_SECURITY_FREEZE_LOCK);
        }
      } else {
        DEBUG ((EFI_D_ERROR, "RapidStart: No drive password provided!"));
      }
    }
  }
  ZeroMem (Instance->HddPassword, ATA_PASSWORD_LEN);
  PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2531);

  if (Status == EFI_SUCCESS) {
    //
    // DPR will be disable temporary, so we won't exculde this region.
    // To save/restore DPR region as normal memory does.
    //
    
    Instance->NotUsedLba  = RapidStartData->StoreLbaAddr;

    PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2540);
    Status                = RapidStartMemoryDiskTransfer (Instance, MemRanges, SmRamRanges);
    PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2541);

    if ((Instance->Transition == RapidStartEntry) && (Status == EFI_SUCCESS)) {
      AhciSimpleCommand (Ahci, ATA_CMD_FLUSH_CACHE);
      AhciSimpleCommand (Ahci, ATA_CMD_STANDBY_IMMEDIATE);
      AhciPortDone (Ahci);
      ForEachAhciPort (Ahci, StandbyAhciPort, AhciGetPresentPorts () & ~(1u << Ahci->Port));
    } else {
      ///
      /// 1. In RapidStart Entry, if user has canceled RapidStart Entry flow, TRIM RapidStart Store.
      /// 2. In RapidStart Resume, only TRIM when Resume successfully, SMRAM content changed.
      ///
#ifdef RAPID_START_TRIM_ON_RESUME
      if (((Instance->Transition == RapidStartEntry) && (Status == EFI_NOT_STARTED)) ||
          ((Instance->Transition == RapidStartExit) && ((Status == EFI_SUCCESS) || (Status == EFI_SECURITY_VIOLATION)))
          ) {
        DEBUG ((EFI_D_INFO, "\nNot used LBA in RapidStart store = %lX\n", Instance->NotUsedLba));
        ASSERT (Instance->NotUsedLba > RapidStartData->StoreLbaAddr);
        Ahci->PollCancellation = FALSE;

        PERF_START_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2550);
        AhciTrimRange (
          Ahci,
          RapidStartData->StoreLbaAddr,
          (UINT32) (Instance->NotUsedLba - RapidStartData->StoreLbaAddr)
          );
        PERF_END_EX (NULL, (CHAR8 *)L"EventRec", NULL, AsmReadTsc (), 0x2551);
      }
#endif
    }
    AhciPortDone (Ahci);
  }

  AhciDone (Ahci);

  //
  // Restore TSEG
  //
  MmioWrite32 (MchBase + R_SA_TSEGMB, OldTseg);
  MmioWrite32 (MchBase + R_SA_DPR, OldDpr);

  //
  // Restore PAM regions
  //
  for (Index = 0; Index < PAM_COUNT; ++Index) {
    MmioWrite8 (MchBase + R_SA_PAM0 + Index, OldPam[Index]);
  }
  //
  // Restore SMRAMC
  //
  MmioWrite8 (MchBase + R_SA_SMRAMC, OldSmramControl);

  return Status;
}

/**
  Restores Memory pointers required by MemoryInit to install S3 reserved area on S3 resume.

  @param[in] This       - RapidStart PPI
**/
STATIC
VOID
RapidStartRestoreS3Pointers (
  IN      RAPID_START_PPI *This
  )
{
  RAPID_START_INSTANCE            *Instance;
  RAPID_START_PERSISTENT_DATA     *RapidStartData;
  UINT64                          AcpiVariableSet64;
  ACPI_VARIABLE_SET               *AcpiVariableSet;
  EFI_PEI_READ_ONLY_VARIABLE_PPI  *ReadOnlyVariable;
  UINTN                           VarSize;
  EFI_STATUS                      Status;

  DEBUG ((EFI_D_INFO, "RapidStartRestoreS3Pointers\n"));

  Instance        = RAPID_START_INSTANCE_FROM_THIS (This);
  RapidStartData  = &Instance->Data;

  Status          = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariablePpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (AcpiVariableSet64);
  Status = ReadOnlyVariable->PeiGetVariable (
                              (EFI_PEI_SERVICES **)GetPeiServicesTablePointer (),
                              ACPI_GLOBAL_VARIABLE,
//[-start-130315-IB06690220-modify]//
//                              &gEfiAcpiVariableGuid,
                              &gAcpiVariableSetGuid,
//[-end-130315-IB06690220-modify]//
                              NULL,
                              &VarSize,
                              &AcpiVariableSet64
                              );

  AcpiVariableSet = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;

  ASSERT (!EFI_ERROR (Status) && AcpiVariableSet != NULL);
  if (EFI_ERROR (Status) || AcpiVariableSet == NULL) {
    return;
  }

#ifdef RAPID_START_USE_OLD_ACPI_VARIABLE_SET
  AcpiVariableSet->SystemMemoryLength = RapidStartData->SystemMemoryLengthBelow4GB;
#else
  AcpiVariableSet->SystemMemoryLengthBelow4GB = RapidStartData->SystemMemoryLengthBelow4GB;
  AcpiVariableSet->SystemMemoryLengthAbove4GB = RapidStartData->SystemMemoryLengthAbove4GB;
#endif
  AcpiVariableSet->AcpiReservedMemoryBase = RapidStartData->AcpiReservedMemoryBase;
  AcpiVariableSet->AcpiReservedMemorySize = RapidStartData->AcpiReservedMemorySize;

  DEBUG ((EFI_D_INFO, "SystemMemoryLengthBelow4GB %lx\n", RapidStartData->SystemMemoryLengthBelow4GB));
  DEBUG ((EFI_D_INFO, "SystemMemoryLengthAbove4GB %lx\n", RapidStartData->SystemMemoryLengthAbove4GB));
  DEBUG ((EFI_D_INFO, "AcpiReservedMemoryBase %lx\n", RapidStartData->AcpiReservedMemoryBase));
  DEBUG ((EFI_D_INFO, "AcpiReservedMemorySize %x\n", RapidStartData->AcpiReservedMemorySize));

}

/**
  Check if RapidStart support is enabled.

  @param[in] This - Pointer to RapidStart PPI

  @retval TRUE if RapidStart support is enabled FALSE otherwise
**/
BOOLEAN
RapidStartIsEnabled (
  IN      RAPID_START_PPI      *This
  )
{
  RAPID_START_INSTANCE  *Instance;
  Instance = RAPID_START_INSTANCE_FROM_THIS (This);
  return Instance->Enabled;
}

/**
  Detremines RapidStart transition.

  @param[in] This - Pointer to RapidStart PPI

  @retval RapidStart_TRANSITION - The RapidStart transition being performed
**/
RAPID_START_TRANSITION
RapidStartGetMode (
  IN      RAPID_START_PPI      *This
  )
{
  RAPID_START_INSTANCE  *Instance;
  DEBUG ((EFI_D_INFO, "RapidStartGetMode()\n"));
  Instance = RAPID_START_INSTANCE_FROM_THIS (This);
  ASSERT (Instance->Transition != RapidStartTransitionMax);
  return Instance->Transition;
}

/**
  Performs actual RapidStart transition.

  @param[in] This - RapidStart PPI

  @retval EFI_ABORTED  - RapidStart Entry or Resume failed
  @retval EFI_SUCCESS  - RapidStart Entry/Resume successfully
**/
STATIC
EFI_STATUS
RapidStartDoTransition (
  IN      RAPID_START_PPI      *This
  )
{
  RAPID_START_INSTANCE        *Instance;
  RAPID_START_PERSISTENT_DATA *RapidStartData;
  RAPID_START_MEM_DATA        *RapidStartMemData;
  CONST EFI_PEI_SERVICES      **PeiServices;
  EFI_STATUS                  Status;
  EFI_STATUS                  MeStatus;
  UINT32                      PmBase;

  ///
  /// 1857 = Intel Rapid Start Technology
  ///
  IoWrite16 (0x80, 0x1857);

  DEBUG ((EFI_D_INFO, "RapidStartDoTransition()\n"));

  PmBase = MmioRead32 (
            MmPciAddress (
            0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  PeiServices       = (CONST EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
  Instance          = RAPID_START_INSTANCE_FROM_THIS (This);
  RapidStartData    = &Instance->Data;

  RapidStartMemData = RAPID_START_MEM_DATA_PTR (RapidStartData);

  Status = RapidStartBeforeTransition (
            GetPeiServicesTablePointer (),
            Instance->Transition,
            Instance->Data.StoreSataPort
            );
  if (EFI_ERROR (Status)) {
    Instance->Transition = RapidStartNone;
  }

  switch (Instance->Transition) {

  case RapidStartEntry:
    DEBUG ((EFI_D_ERROR, "RapidStart ENTRY\n"));

    ///
    /// Clear all GPE events before SIM enabling
    ///
    RapidStartClearAndEnablePmeEvent (FALSE);

    RapidStartMemData->EntryCanceled  = 0;

    Status = RapidStartTransitionInternal (Instance);

    MeStatus = EFI_SUCCESS;
#ifdef ME_SUPPORT_FLAG
    MeStatus = WaitMeInitDone ();
#endif
//[-start-120606-IB04770233-remove]//
//     if (RapidStartMemData->OsWakeTimeEnabled && (Status != EFI_NOT_STARTED)) {
//       DEBUG (
//         (EFI_D_INFO,
//         "Setting RTC alarm to OS provided value %02d-%02d:%02d:%02d\n",
//         RapidStartMemData->OsWakeTime.Date,
//         RapidStartMemData->OsWakeTime.Hour,
//         RapidStartMemData->OsWakeTime.Minute,
//         RapidStartMemData->OsWakeTime.Second)
//         );
// 
//       RtcSetAlarm (&RapidStartMemData->OsWakeTime);
// 
//       ///
//       /// set RTC_EN bit in PM1_EN to wake up from the alarm
//       ///
//       IoWrite16 (
//         PmBase + R_PCH_ACPI_PM1_EN,
//         (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
//         );
//     }
//[-end-120606-IB04770233-remove]//
    if (EFI_ERROR (Status) || EFI_ERROR (MeStatus)) {
      if (Status == EFI_NOT_STARTED) {
        ///
        /// RapidStart Entry flow has been canceled, do S3 resume.
        ///
        DEBUG ((EFI_D_ERROR, "User canceled RapidStart!\n"));
        RapidStartMemData->EntryCanceled = 1;
      } else {
        DEBUG ((EFI_D_ERROR, "RapidStart entry failed!\n"));
        Instance->RapidStartGlobalNvs->EventsEnabled    = 0;
        Instance->RapidStartGlobalNvs->EventsAvailable  = 0;
      }

      Status = PeiServicesNotifyPpi (&mRapidStartRecoveryNotifyDesc);
      ASSERT_EFI_ERROR (Status);
      RapidStartAfterTransition (PeiServices, RapidStartEntry, Status, RapidStartData->StoreSataPort);
      break;
    }

    RapidStartSetFlag (RAPID_START_FLAG_ENTRY_DONE);

    RapidStartAfterTransition (PeiServices, RapidStartEntry, Status, RapidStartData->StoreSataPort);

    Status = PeiServicesNotifyPpi (&mRapidStartS4NotifyDesc);
    ASSERT_EFI_ERROR (Status);
    break;

  case RapidStartExit:
    DEBUG ((EFI_D_ERROR, "RapidStart EXIT\n"));

    Status = PeiServicesNotifyPpi (&mRapidStartExitNotifyDesc);
    ASSERT_EFI_ERROR (Status);

    Status    = RapidStartTransitionInternal (Instance);

    MeStatus  = EFI_SUCCESS;

    if (EFI_ERROR (Status) || EFI_ERROR (MeStatus)) {
      DEBUG ((EFI_D_ERROR, "RapidStart exit failed!\n"));
      if (EFI_ERROR (Status)) {
        RapidStartAfterTransition (PeiServices, RapidStartExit, Status, RapidStartData->StoreSataPort);
      }

      if (EFI_ERROR (MeStatus)) {
        RapidStartAfterTransition (PeiServices, RapidStartExit, MeStatus, RapidStartData->StoreSataPort);
      }

      PeiServicesResetSystem ();
      CpuDeadLoop ();
    }
    ///
    /// Set DISB
    ///
    MmioOr16 (
      MmPciAddress (
      0,
      DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_LPC,
      PCI_FUNCTION_NUMBER_PCH_LPC,
      R_PCH_LPC_GEN_PMCON_2),
      B_PCH_LPC_GEN_PMCON_DRAM_INIT
      );

    RapidStartAfterTransition (PeiServices, RapidStartExit, EFI_SUCCESS, RapidStartData->StoreSataPort);
    break;

  case RapidStartNone:
    ///
    /// Cancelled
    ///
    Status = EFI_ABORTED;
    break;

  default:
    ASSERT (0);
  }

  IoWrite16 (0x80, 0);

  return Status;
}

/**
  Executes or schedules RapidStart transition if appropriate.

  @param[in] This        - Pointer to RapidStart PPI
  @param[out] BootMode   - Boot mode that may be overridden

  @retval EFI_SUCCESS   - RapidStart transition performed/scheduled successfully
  @retval EFI_ABORTED   - RapidStart transition aborted
**/
STATIC
EFI_STATUS
RapidStartTransitionEntryPoint (
  IN      RAPID_START_PPI      *This,
  OUT     EFI_BOOT_MODE        *BootMode
  )
{
  EFI_STATUS              Status;
  RAPID_START_TRANSITION  Transition;

  DEBUG ((EFI_D_INFO, "RapidStartTransitionEntryPoint\n"));

  Transition = RapidStartGetMode (This);
  Status = EFI_SUCCESS;

  if (Transition != RapidStartNone) {
#ifdef RAPID_START_ON_MEMORY_INSTALLED
    RapidStartRestoreS3Pointers (This);
    Status = PeiServicesNotifyPpi (&mRapidStartTransitionPpiNotifyDesc);
    ASSERT_EFI_ERROR (Status);
#else
    Status = RapidStartDoTransition (This);
#endif
    if (Transition == RapidStartExit && BootMode != NULL) {
      *BootMode = BOOT_ON_S3_RESUME;
      ///
      /// Update Boot Mode
      ///
      Status = PeiServicesSetBootMode (BOOT_ON_S3_RESUME);
      ASSERT_EFI_ERROR (Status);
    }
  }

  return Status;
}

/**
  Checks power button override status.

  @retval TRUE if power button override has taken place, FALSE otherwise.
**/
STATIC
BOOLEAN
IsPowerButtonOverride (
  VOID
  )
{
  UINT32  PmBase;
  PmBase = MmioRead32 (
            MmPciAddress (
            0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  return (IoRead16 (PmBase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PRBTNOR) != 0;
}

/**
  Retrieves RapidStart non-volatile flag and data,
  initializes RapidStart instance,
  handles RapidStart entry conditions and prerequisites.

  @param[in,out] Instance   - RapidStart instance
**/
STATIC
VOID
RapidStartInitInstance (
  IN OUT RAPID_START_INSTANCE   *Instance
  )
{
  EFI_PEI_READ_ONLY_VARIABLE_PPI  *ReadOnlyVariable;
  UINTN                           Size;
  EFI_STATUS                      Status;
  BOOLEAN                         RapidStartFlag;
  UINT8                           EventsEnabled;
  UINT32                          PmBase;
  EFI_BOOT_MODE                   BootMode;
  SA_PLATFORM_POLICY_PPI          *SaPlatformPolicyPpi;

  Instance->Signature   = RAPID_START_INSTANCE_SIGNATURE;
  Instance->Transition  = RapidStartNone;
  Instance->Enabled     = FALSE;
  Instance->FreezeLock  = TRUE;
  Instance->PwdStatus   = EFI_NOT_FOUND;

  Status                = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariablePpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
  ASSERT_EFI_ERROR (Status);

  Size = sizeof (RAPID_START_PERSISTENT_DATA);
  Status = ReadOnlyVariable->PeiGetVariable (
                              (EFI_PEI_SERVICES **)GetPeiServicesTablePointer (),
                              RAPID_START_PERSISTENT_DATA_NAME,
                              &gRapidStartPersistentDataGuid,
                              NULL,
                              &Size,
                              &Instance->Data
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "RapidStart disabled (No RapidStart variable found)\n"));
    return ;
  }

  Instance->Enabled             = TRUE;
  Instance->RapidStartGlobalNvs = (RAPID_START_GLOBAL_NVS_AREA *) Instance->Data.RapidStartGlobalNvsPtr;
  DEBUG ((EFI_D_INFO, "RapidStartGlobalNVS: %x\n", Instance->RapidStartGlobalNvs));

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // Get IED size through the SaPlatformPolicy PPI
  //
  SaPlatformPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSaPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SaPlatformPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);
  if ((SaPlatformPolicyPpi != NULL) && (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_9)) {
    Instance->IedSize = SaPlatformPolicyPpi->PlatformData->IedSize;
  } else {
    Instance->IedSize = 0x400000;
  }

  if (BootMode == BOOT_ON_S3_RESUME) {
    DEBUG ((EFI_D_INFO, "S3 resume\n"));

    PmBase = MmioRead32 (
              MmPciAddress (
              0,
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_ACPI_BASE)
              ) & B_PCH_LPC_ACPI_BASE_BAR;

    Status = RapidStartGetConfig (&EventsEnabled);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Check RTC alarm wake event
    ///
    if (IoRead16 (PmBase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_RTC) {
      ///
      /// Clear RTC Alarm Flag
      ///
      RtcRead (R_PCH_RTC_REGC);
      if (EventsEnabled & RAPID_START_ACPI_RTC_WAKE) {
        DEBUG ((EFI_D_INFO, "RapidStart on RTC wake\n"));
        Instance->Transition = RapidStartEntry;
      } else {
        DEBUG ((EFI_D_INFO, "Boot from RapidStart cancellation or this is waking by OS timer\n"));
      }
    }
    ///
    /// Check for critical battery wake event
    ///
    else if ((EventsEnabled & RAPID_START_ACPI_BATT_WAKE) && RapidStartCheckCriticalBatteryWakeupEvent ()) {
      DEBUG ((EFI_D_INFO, "RapidStart on critical battery wake from EC.\n"));
      RapidStartInitializeCriticalBatteryWakeupEvent (FALSE);
      RapidStartClearAllKscWakeStatus ();
      Instance->Transition = RapidStartEntry;
    }

    return ;
  }
  ///
  /// Check RapidStart non-volatile flag
  ///
  Status = RapidStartGetFlag (&RapidStartFlag);
  if (EFI_ERROR (Status) || ((RapidStartFlag & RAPID_START_FLAG_ENTRY_DONE) == 0)) {
    DEBUG ((EFI_D_INFO, "No RapidStart flag set\n"));
    return ;
  }
  ///
  /// RapidStart exit path (Cold boot assumed)
  /// For security reasones need to clear SMRAM hash and drive password from non-volatile
  /// memory first.
  ///
#ifndef RAPID_START_NO_SMRAM_INTEGRITY_CHECK
  Status = RapidStartRestoreAndClearSecureHash (Instance->SmRamHash);
#endif

  Instance->PwdStatus = RapidStartGetDriveUnlockPassword (
                          GetPeiServicesTablePointer (),
                          RapidStartExit,
                          Instance->Data.StoreSataPort,
                          (UINT8 *) Instance->HddPassword,
//[-start-120628-IB05330351-modify]//
                          &Instance->FreezeLock,
                          &Instance->PasswordType
//[-end-120628-IB05330351-modify]//
                          );

  ///
  /// This is either RapidStart Resume flow or non-RapidStart transition
  /// Clear RapidStart flag in the start point.
  ///
  RapidStartSetFlag (0);

  if (EFI_ERROR (Status)) {
    return ;
  }

  if (IsPowerButtonOverride ()) {
    DEBUG ((EFI_D_INFO, "PWRBTN override\n"));
    return ;
  }

  DEBUG ((EFI_D_INFO, "RapidStart exit\n"));
  Instance->Transition = RapidStartExit;
}

/**
  Install RapiStartPpi.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
InstallRapidStartPpi (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  RAPID_START_INSTANCE  *Instance;
  EFI_STATUS            Status;

  Instance = AllocateZeroPool (sizeof (RAPID_START_INSTANCE));
  ASSERT (Instance != NULL);
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Instance->Ppi.Revision              = RAPID_START_PPI_REVISION_1;
  Instance->Ppi.GetMode               = RapidStartGetMode;
  Instance->Ppi.TransitionEntryPoint  = RapidStartTransitionEntryPoint;
  Instance->Ppi.IsEnabled             = RapidStartIsEnabled;

  Instance->PpiDesc.Flags             = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  Instance->PpiDesc.Guid              = &gRapidStartPpiGuid;
  Instance->PpiDesc.Ppi               = &Instance->Ppi;

  RapidStartInitInstance (Instance);

  if (Instance->Transition != RapidStartNone) {
    RapidStartInitAhci (Instance);
  }
  if (Instance->Transition == RapidStartEntry) {
    ForEachAhciPort (
      &Instance->Ahci,
      AhciSpinUpPort,
      AhciGetEnabledPorts () & ~(1u << Instance->Ahci.Port)
      );
  }

  Status = PeiServicesInstallPpi (&Instance->PpiDesc);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;

}

/**
  RapidStart PEIM entry point driver

  @param[in] FfsHeader   - FFS file header pointer of this driver
  @param[in] PeiServices - Pointer to PEI service table

  @retval EFI_SUCCESS - RapidStart PPI installed successfully
  @retval EFI_OUT_OF_RESOURCES - Error in allocate required resource.
**/
EFI_STATUS
EFIAPI
RapidStartPeiEntryPoint (
  IN        EFI_PEI_FILE_HANDLE  FileHandle,
  IN  CONST EFI_PEI_SERVICES     **PeiServices
  )
{
#ifdef ME_SUPPORT_FLAG
  PEI_HECI_PPI                *HeciPpi;
  EFI_STATUS                  Status;
#endif

  DEBUG ((EFI_D_INFO, "RapidStart: RapidStartPeiEntryPoint\n"));

#ifdef ME_SUPPORT_FLAG
  ///
  /// Install RapiStart Ppi when Heci Ppi is ready.
  ///

  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gEfiPeiHeciPpiGuid,
                            0,
                            NULL,
                            (VOID **) &HeciPpi
                            );
  if (EFI_ERROR(Status)) {
    Status = PeiServicesNotifyPpi (&mInstallRapidStartPpiNotifyDesc);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  }
#endif

  return InstallRapidStartPpi ((EFI_PEI_SERVICES **)PeiServices, NULL, NULL);
}
