/** @file
  MP Support driver

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

#include "MpService.h"
#include <Library/BaseMemoryLib.h>

extern EFI_CPU_MICROCODE_HEADER **mMicrocodePointerBuffer;

ACPI_CPU_DATA                   *mAcpiCpuData;
MP_SYSTEM_DATA                  *mMPSystemData;

///
/// Function declarations
///
/**
  Initializes MP support in the system.

  @retval EFI_SUCCESS          - Multiple processors are initialized successfully.
  @retval EFI_OUT_OF_RESOURCES - No enough resoruces (such as out of memory).
**/
EFI_STATUS
InitializeMpSupport (
  VOID
  )
{
  EFI_STATUS            Status;
  MP_CPU_RESERVED_DATA  *MpCpuReservedData;

  ///
  /// Allocate memory for MP CPU related data below 4G
  ///
  Status = AllocateReservedMemoryBelow4G (
            sizeof (MP_CPU_RESERVED_DATA),
            (VOID **) &MpCpuReservedData
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (MpCpuReservedData, sizeof (MP_CPU_RESERVED_DATA));

  mMPSystemData = &(MpCpuReservedData->MPSystemData);
  mAcpiCpuData  = &(MpCpuReservedData->AcpiCpuData);

  ///
  /// Copy microcode to allocated memory
  ///
  CopyMem (
    MpCpuReservedData->MicrocodePointerBuffer,
    mMicrocodePointerBuffer,
    sizeof (EFI_CPU_MICROCODE_HEADER *) * (NUMBER_OF_MICROCODE_UPDATE + 1)
    );

  ///
  /// Initialize ACPI_CPU_DATA data
  ///
  mAcpiCpuData->CpuPrivateData          = (EFI_PHYSICAL_ADDRESS) (UINTN) (&(mMPSystemData->S3DataPointer));
  mAcpiCpuData->S3BootPath              = FALSE;
  mAcpiCpuData->MicrocodePointerBuffer  = (EFI_PHYSICAL_ADDRESS) MpCpuReservedData->MicrocodePointerBuffer;
  mAcpiCpuData->GdtrProfile             = (EFI_PHYSICAL_ADDRESS) & (MpCpuReservedData->GdtrProfile);
  mAcpiCpuData->IdtrProfile             = (EFI_PHYSICAL_ADDRESS) & (MpCpuReservedData->IdtrProfile);

  ///
  /// Initialize MP services
  ///
  InitializeMpServices ();

  return EFI_SUCCESS;
}
