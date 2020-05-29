/** @file

  Copies the memory related timing and configuration information into the
  Compatible BIOS data (BDAT) table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BdatAccessHandler.h"

#ifdef BDAT_SUPPORT
///
/// Data definitions & structures
///
EFI_GUID                                gMemoryInitHobGuid = EFI_MEMORY_RESTORE_DATA_GUID;

///
/// Bdat Access Handler instance data structure
///
STATIC EFI_BDAT_ACPI_DESCRIPTION_TABLE  mRmtAcpiTable = {
  EFI_BDAT_TABLE_SIGNATURE,                 /// Signature
  sizeof (EFI_BDAT_ACPI_DESCRIPTION_TABLE), /// Length
  0x01,                                     /// Revision [01]
  0,                                        /// Checksum
  ' ',                                      /// OEM ID
  ' ',                                      /// .
  ' ',                                      /// .
  ' ',                                      /// .
  ' ',                                      /// .
  ' ',                                      /// .
  0,                                        /// OEM Table ID
  0,                                        /// OEM Revision [0x00000000]
  0,                                        /// Creator ID
  0,                                        /// Creator Revision
  0,                                        /// System Memory Address Space ID
  0,
  0,
  0,
  EFI_BDAT_ACPI_POINTER,
};
#endif

/**
  Entry point of the Bdat RMT Access Handler.

  @param [in] DxePlatformSaPolicy A pointer to Dxe platform policy
  @param [in] HobList             A pointer to the HOB list
  @param [in] AcpiTable           A pointer to ACPI table

  @retval EFI_SUCCESS:            Driver initialized successfully
  @retval EFI_OUT_OF_RESOURCES:   Could not allocate needed resources
  @exception EFI_UNSUPPORTED:     A needed driver was not located

**/
EFI_STATUS
BdatRmtHandler (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy,
  IN VOID                            *HobList,
  IN EFI_ACPI_TABLE_PROTOCOL         *AcpiTable
  )
{
#ifdef BDAT_SUPPORT
  EFI_STATUS            Status;
  VOID                  *Buffer;
  RmtData               *rmtBdat;
  HOB_SAVE_MEMORY_DATA  *RmtHobData;
  UINTN                 AcpiTableKey;
  UINT64                TempBuffer;
  UINT16                BufferSize;
  UINT32                *ScratchPad;

  Buffer      = NULL;
  rmtBdat     = NULL;
  BufferSize  = 0;
  Status      = EFI_SUCCESS;
  if (!DxePlatformSaPolicy->MemoryConfig->RmtBdatEnable) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get the Compatible BIOS structure PMT BDAT from the HOB.
  ///
  RmtHobData = GetNextGuidHob (&gMemoryInitHobGuid, HobList);
  if (RmtHobData == NULL) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Allocate and clear memory, in 4kb pages
  ///
  BufferSize = sizeof (RmtData);

  Status = (gBS->AllocatePages) (AllocateAnyPages, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (BufferSize), (EFI_PHYSICAL_ADDRESS *) &Buffer);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (Buffer, BufferSize);
  ///
  /// Copy BDAT structure to Reserved memory
  ///
  CopyMem (Buffer, &RmtHobData->MrcData.Rmt, BufferSize);

  TempBuffer = SIGNATURE_64 ('I', 'N', 'T', 'E', 'L', 0, 0, 0);
  ///
  /// RMT ACPI table
  ///
  DEBUG ((DEBUG_INFO, "In RMT ACPI table\n"));
  CopyMem (&mRmtAcpiTable.Header.OemId, &TempBuffer, sizeof (mRmtAcpiTable.Header.OemId));
  mRmtAcpiTable.Header.OemTableId       = SIGNATURE_64 ('H', 'S', 'W', '-', 'L', 'P', 'T', 0);
  mRmtAcpiTable.Header.CreatorId        = CREATOR_ID_INTEL;
  mRmtAcpiTable.Header.CreatorRevision  = CREATOR_REV_INTEL;
  mRmtAcpiTable.Header.OemRevision      = ACPI_BDAT_OEM_REV;
  ///
  /// Copy pointer to RMT ACPI BDAT structure and protocol.
  ///
  mRmtAcpiTable.BdatGas.Address = (EFI_PHYSICAL_ADDRESS) Buffer;
  ///
  /// Install RMT BDAT into RMT ACPI table
  ///
  AcpiTableKey = 0;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &mRmtAcpiTable,
                        sizeof (EFI_BDAT_ACPI_DESCRIPTION_TABLE),
                        &AcpiTableKey
                        );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Write scratchpad register in MCHBAR space with address of the RMT ACPI BDAT structure.
  ///
  ScratchPad  = (UINT32 *) ((UINT64) (RmtHobData->MrcData.SysIn.Inputs.MchBarBaseAddress) + NCDECS_CR_SCRATCHPAD_NCU_2_REG);
  *ScratchPad = (UINT32) ((UINT64) Buffer);

  return Status;
#else
  return EFI_UNSUPPORTED;
#endif
}
