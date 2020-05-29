/** @file
  Boot service DXE ASL update library implementation.
  These functions in this file can be called during DXE and cannot be called during runtime
  or in SMM which should use a RT or SMM library.
  This library uses the ACPI Support protocol.

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

///
/// Include files
///
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AslUpdateLib.h>

///
/// Function implemenations
///
static EFI_ACPI_SUPPORT_PROTOCOL  *mAcpiSupport = NULL;
static EFI_ACPI_TABLE_PROTOCOL    *mAcpiTable   = NULL;

/**
  Initialize the ASL update library state.
  This must be called prior to invoking other library functions.

  @param[in] None

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
InitializeAslUpdateLib (
  VOID
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **) &mAcpiSupport);
  ASSERT_EFI_ERROR (Status);
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTable);
  return EFI_SUCCESS;
}

/**
  This procedure will update two kinds of asl code.
  1:  Operating Region base address and length.
  2:  Resource Consumption structures in device LDRC.

  @param[in] AslSignature         The signature of Operation Region that we want to update.
  @param[in] BaseAddress          Base address of IO trap.
  @param[in] Length               Length of IO address.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
UpdateAslCode (
  IN     UINT32                   AslSignature,
  IN     UINT16                   BaseAddress,
  IN     UINT8                    Length
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_DESCRIPTION_HEADER *Table;
  EFI_ACPI_TABLE_VERSION      Version;
  UINT8                       *CurrPtr;
  UINT8                       *Operation;
  UINT32                      *Signature;
  UINT8                       *DsdtPointer;
  INTN                        Index;
  UINTN                       Handle;
  UINT16                      AslLength;

  ///
  /// Locate table with matching ID
  ///
  Index     = 0;
  AslLength = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) &Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (Table->Signature != EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);

  ///
  /// Fix up the following ASL Code in DSDT:
  /// (1) OperationRegion's IO Base Address and Length.
  /// (2) Resource Consumption in LPC Device.
  ///
  CurrPtr = (UINT8 *) Table;

  ///
  /// Loop through the ASL looking for values that we must fix up.
  ///
  for (DsdtPointer = CurrPtr; DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); DsdtPointer++) {
    ///
    /// Get a pointer to compare for signature
    ///
    Signature = (UINT32 *) DsdtPointer;

    ///
    /// Check if this is the signature we are looking for
    ///
    if ((*Signature) == AslSignature) {
      ///
      /// Conditional match.  For Region Objects, the Operator will always be the
      /// byte immediately before the specific name.  Therefore, subtract 1 to check
      /// the Operator.
      ///
      Operation = DsdtPointer - 1;

      ///
      /// If we have an operation region, update the base address and length
      ///
      if (*Operation == AML_OPREGION_OP) {
        ///
        /// Fixup the Base Address in OperationRegion.
        ///
        *(UINT16 *) (DsdtPointer + 6) = BaseAddress;

        ///
        /// Fixup the Length in OperationRegion.
        ///
        *(DsdtPointer + 9) = Length;
      }

    } else if ((*Signature) == SIGNATURE_32 ('L', 'D', 'R', 'C')) {
      ///
      /// Make sure it's device of LDRC and read the length
      ///
      if (*(DsdtPointer - 2) == AML_DEVICE_OP) {
        AslLength = *(DsdtPointer - 1);
      } else if (*(DsdtPointer - 3) == AML_DEVICE_OP) {
        AslLength = *(UINT16 *) (DsdtPointer - 2);
        AslLength = (AslLength & 0x0F) + ((AslLength & 0x0FF00) >> 4);
      }
      ///
      /// Conditional match.  Search _CSR in Device (LDRC).
      ///
      for (Operation = DsdtPointer; Operation <= DsdtPointer + AslLength; Operation++) {
        ///
        /// Get a pointer to compare for signature
        ///
        Signature = (UINT32 *) Operation;

        ///
        /// Check if this is the signature we are looking for
        ///
        if ((*Signature) == SIGNATURE_32 ('_', 'C', 'R', 'S')) {
          ///
          /// Now look for an empty resource entry, fix the base address and length fields
          ///
          for (Index = 0; *(UINT16 *) (Operation + 9 + 8 * Index) != 0x0079; Index++) {
            if (*(UINT16 *) (Operation + 11 + 8 * Index) == UINT16_BIT_MAGIC_NUMBER) {
              ///
              /// Fixup the Base Address and Length.
              ///
              *(UINT16 *) (Operation + 11 + 8 * Index)  = BaseAddress;
              *(UINT16 *) (Operation + 13 + 8 * Index)  = BaseAddress;
              *(Operation + 16 + 8 * Index)             = Length;
              break;
            }
          }
        }
      }

      DsdtPointer = DsdtPointer + AslLength;
    }
  }
  ///
  /// Update the modified ACPI table
  ///
//[-start-140702-IB05400538-modify]//
//[-start-130424-IB10130046-modify]//
  Status = mAcpiTable->UninstallAcpiTable (mAcpiTable, Handle);

  Handle = 0;
  Status = mAcpiTable->InstallAcpiTable (mAcpiTable, Table, Table->Length, &Handle);
//[-end-130424-IB10130046-modify]//
//[-end-140702-IB05400538-modify]//
  FreePool (Table);

  return EFI_SUCCESS;
}

/**
  This function uses the ACPI support protocol to locate an ACPI table.
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.

  @param[in] Signature            Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in] Table                Updated with a pointer to the table
  @param[in] Handle               AcpiSupport protocol table handle for the table found
  @param[in] Version              The version of the table desired

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
{
  EFI_STATUS              Status;
  INTN                    Index;
  EFI_ACPI_TABLE_VERSION  DesiredVersion;

  DesiredVersion = *Version;
  ///
  /// Locate table with matching ID
  ///
  Index = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) Table, Version, Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while ((*Table)->Signature != Signature || !(*Version & DesiredVersion));

  ///
  /// If we found the table, there will be no error.
  ///
  return Status;
}

/**
  This function uses the ACPI support protocol to locate an ACPI SSDT table.

  @param[in] TableId              Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in] TableIdSize          Length of the TableId to match.  Table ID are 8 bytes long, this function
                                  will consider it a match if the first TableIdSize bytes match
  @param[in] Table                Updated with a pointer to the table
  @param[in] Handle               AcpiSupport protocol table handle for the table found
  @param[in] Version              See AcpiSupport protocol, GetAcpiTable function for use

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableByOemTableId (
  IN      UINT8                         *TableId,
  IN      UINT8                         TableIdSize,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
{
  EFI_STATUS  Status;
  INTN        Index;

  ///
  /// Locate table with matching ID
  ///
  Index = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) Table, Version, Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (CompareMem (&(*Table)->OemTableId, TableId, TableIdSize));

  ///
  /// If we found the table, there will be no error.
  ///
  return Status;
}

/**
  This function calculates and updates an UINT8 checksum.

  @param[in] Buffer               Pointer to buffer to checksum
  @param[in] Size                 Number of bytes to checksum
  @param[in] ChecksumOffset       Offset to place the checksum result in

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
AcpiChecksum (
  IN VOID                         *Buffer,
  IN UINTN                        Size,
  IN UINTN                        ChecksumOffset
  )
{
  UINT8 Sum;
  UINT8 *Ptr;

  Sum = 0;
  ///
  /// Initialize pointer
  ///
  Ptr = Buffer;

  ///
  /// set checksum to 0 first
  ///
  Ptr[ChecksumOffset] = 0;

  ///
  /// add all content of buffer
  ///
  while (Size--) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  ///
  /// set checksum
  ///
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}
