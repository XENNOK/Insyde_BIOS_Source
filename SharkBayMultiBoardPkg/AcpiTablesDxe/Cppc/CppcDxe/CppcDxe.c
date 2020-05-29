/** @file
  This DXE driver configures and supports Collaborative Processor Performance Control (CPPC).

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
#include "CppcDxe.h"

ACPI_PLATFORM_POLICY_PROTOCOL              *mAcpiPlatformPolicyProtocol;

//extern EFI_RUNTIME_SERVICES     *gRT;
//extern EFI_BOOT_SERVICES        *gBS;

EFI_PHYSICAL_ADDRESS            mCppcBaseAddressMem = 0;

///
/// Driver entry point
///
//EFI_DRIVER_ENTRY_POINT (InitializeCppc)

EFI_STATUS
EFIAPI
InitializeCppc (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
/**

@brief

  Cppc DXE support.

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS - The driver installed/initialized correctly.

**/
{
  EFI_STATUS                  Status;
  VOID                        *MemoryPointer;
  UINT64                      MsrValue;

  ///
  /// Locate platform configuration information and copy it to a global variable.
  ///
  Status = gBS->LocateProtocol (
                  &gAcpiPlatformPolicyProtocolGuid, 
                  NULL, 
                  (VOID **) &mAcpiPlatformPolicyProtocol
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "CPPC: No Platform Policy Protocol available.\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  } else {
    DEBUG ((EFI_D_ERROR, "CPPC: Platform Policy Protocol is loaded.\n"));
  }

  ///
  /// Enable or disable CPPC depending on platform policy.
  ///
  if (mAcpiPlatformPolicyProtocol->EnableCppc == 0) {
    DEBUG ((EFI_D_ERROR, "CPPC is disabled by platform policy. \n"));
    return EFI_SUCCESS;
  }

  ///
  /// Check if Haswell traditional, Haswell ULT or Crystal Well CPU.
  ///
  if(!(IsCpuSupported())){
    DEBUG ((EFI_D_ERROR, "CPPC not supported in this CPU. \n"));
    return EFI_SUCCESS;
  }

  ///
  /// Check if Mobile CPU.
  ///
  if(!(IS_SA_DEVICE_ID_MOBILE (McD0PciCfg16 (R_SA_MC_DEVICE_ID)))) {
    DEBUG ((EFI_D_ERROR, "CPPC not supported in this CPU. (not mobile) \n"));
    return EFI_SUCCESS;
  }

  ///
  /// Check if processor supports EE Policy capability.
  ///
  MsrValue = AsmReadMsr64 (EFI_MSR_POWER_CTL);
  if (((UINT32)MsrValue & BIT18) != BIT18) {
    DEBUG ((EFI_D_ERROR, "CPPC not supported in this CPU. (no Energy Efficient Policy capability) \n"));
    return EFI_SUCCESS;
  }

  ///
  /// Allocate 4kb of runtime memory.
  ///
  Status  = (gBS->AllocatePool) (EfiACPIMemoryNVS, CPPC_SIZE, &MemoryPointer);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "CPPC: No Memory Buffer available.\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  } else {
    mCppcBaseAddressMem = (EFI_PHYSICAL_ADDRESS) MemoryPointer;
    ZeroMem ((VOID *)(UINTN)mCppcBaseAddressMem, CPPC_SIZE);
    DEBUG((EFI_D_ERROR, "CPPC: PCCA memory buffer address: %x\n", mCppcBaseAddressMem));
  }

  ///
  /// Add the Platform Communications Channel Table signature.
  ///
  CopyMem ((VOID *)(UINTN)mCppcBaseAddressMem, "PCC" , 3);

  ///
  /// Load the SSDT Table for the CPPC ACPI objects.
  ///
  LoadAcpiTables ();

  ///
  /// Load the ACPI 5.0 PCC Table.
  ///
  LoadAcpiTablesPcct ();

  ///
  /// Load the ACPI 5.0 _CPC objects.
  ///
  LoadAcpiTablesCpc ();

  return EFI_SUCCESS;
}

VOID
LoadAcpiTables(
  VOID
  )
/**
@brief
  This procedure loads the CPPC SSDT table.

  @param[in] None

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;
  NAME_LAYOUT                   *NamePtr;
  UINT8                         *CurrPtr;
  UINT32                        *Signature;
  UINT8                         UpdateCounter;

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gCppcAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gCppcAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('C', 'p', 'p', 'c', 'T', 'a', 'b', 'l'):
        ///
        /// This is Cppc SSDT. Cppc should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((EFI_D_ERROR, "CPPC: Found Cppc SSDT signature.\n"));

        UpdateCounter = 3; // Number of objects to initialize.
        for (CurrPtr = (UINT8 *) TableHeader; ((CurrPtr <= ((UINT8 *) TableHeader + TableHeader->Length))) && UpdateCounter !=0; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 1);
          ///
          /// Patch PCCA with the address of the CPPC shared memory buffer.
          ///
          if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('P', 'C', 'C', 'A'))) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            if (mCppcBaseAddressMem != 0){
              NamePtr->Value  = (UINT32) mCppcBaseAddressMem;
            }
            UpdateCounter--;
            DEBUG((EFI_D_ERROR, "CPPC: PCCA pointer updated in SSDT: %x\n", mCppcBaseAddressMem));
          }
          ///
          /// Patch PCCS with the size of the CPPC shared memory buffer.
          ///
          if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('P', 'C', 'C', 'S'))) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            NamePtr->Value  = (UINT32) CPPC_SIZE;
            UpdateCounter--;
            DEBUG((EFI_D_ERROR, "CPPC: PCCS length updated in SSDT: %x\n", CPPC_SIZE));
          }
          ///
          /// Patch PENB with the CPPC Enable/Disable Flag.
          ///
          if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('P', 'E', 'N', 'B'))) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            NamePtr->Value  = (UINT32) mAcpiPlatformPolicyProtocol->EnableCppc;
            UpdateCounter--;
            DEBUG((EFI_D_ERROR, "CPPC: PENB updated in SSDT: %x\n", (UINT32) mAcpiPlatformPolicyProtocol->EnableCppc));
          }
        }
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
}

VOID
LoadAcpiTablesPcct(
  VOID
  )
/**
@brief
  This procedure loads the ACPI 5.0 PCCT table.

  @param[in] None

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;
  EFI_ACPI_5_0_PLATFORM_COMMUNICATION_CHANNEL_TABLE_HEADER *PcctHeaderPtr;
  EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC  *PcctPtr;
  UINT8                         *CurrPtr;
  UINT32                        *Signature;
  UINT8                         UpdateCounter;

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gCppcAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gCppcAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('P', 'c', 'c', 't', 'T', 'a', 'b', 'l'):
        ///
        /// This is PCCT. Cppc should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((EFI_D_ERROR, "PCCT: Found PCCT signature.\n"));

        UpdateCounter = 1; /// Number of objects to initialize.
        for (CurrPtr = (UINT8 *) TableHeader; ((CurrPtr <= ((UINT8 *) TableHeader + TableHeader->Length))) && UpdateCounter !=0; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 1);
          ///
          /// Patch the EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC structure.
          ///
          if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('P', 'C', 'C', 'T'))) {
            CurrPtr -= sizeof (EFI_ACPI_DESCRIPTION_HEADER); // backup the current pointer in order to use the PCCT header structure
            PcctHeaderPtr = (EFI_ACPI_5_0_PLATFORM_COMMUNICATION_CHANNEL_TABLE_HEADER *) CurrPtr;
            PcctHeaderPtr->Flags  = (UINT32) mAcpiPlatformPolicyProtocol->EnableCppcPlatformSCI;
            PcctHeaderPtr->Reserved = 0;

            CurrPtr += sizeof (EFI_ACPI_5_0_PLATFORM_COMMUNICATION_CHANNEL_TABLE_HEADER);
            PcctPtr = (EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC *) CurrPtr;
            PcctPtr->Type = 0x0;
            PcctPtr->Length = 62;
            PcctPtr->BaseAddress = mCppcBaseAddressMem; /// Shared memory address
            PcctPtr->AddressLength = CPPC_SIZE;         /// sizeof(Shared memory) in bytes

            PcctPtr->DoorbellRegister.AddressSpaceId = 1;      /// IO address type
            PcctPtr->DoorbellRegister.RegisterBitWidth = 8;    /// byte
            PcctPtr->DoorbellRegister.RegisterBitOffset = 0x0; /// offset=0
            PcctPtr->DoorbellRegister.AccessSize = 1;          /// byte access
            PcctPtr->DoorbellRegister.Address = MmioRead16 (MmPciExpressAddress (0,0, PCI_DEV_NUM_ICH_LPC, 0, R_ACPI_BAR)) &~BIT0;
            PcctPtr->DoorbellRegister.Address += R_ACPI_GPE_CNTL; /// PMBASE + GPE_CNTL register offset
            PcctPtr->DoorbellPreserve = 0xFD; /// preserve all bits except bit 1
            PcctPtr->DoorbellWrite = 2;       /// write bit 1 = 1 to trigger GPE
            PcctPtr->NominalLatency = 5000;   /// 5ms
            PcctPtr->MaximumPeriodicAccessRate = 0x0;
            PcctPtr->MinimumRequestTurnaroundTime = 0x0;

            UpdateCounter--;
            DEBUG((EFI_D_ERROR, "PCCT: Patch the EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC structure.\n"));
          }
        }
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
}

VOID
LoadAcpiTablesCpc(
  VOID
  )
/**
@brief
  Load ACPI SSDT tables.

  @param[in] None

  @retval None

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gCppcAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gCppcAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('C', 'p', 'c', '_', 'T', 'a', 'b', 'l'):
        ///
        /// This is the _CPC SSDT. Cppc should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((EFI_D_ERROR, "CPPC: Found _CPC SSDT signature.\n"));
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
}

EFI_STATUS
LocateSupportProtocol (
  IN  EFI_GUID                *Protocol,
  OUT VOID                    **Instance,
  IN  BOOLEAN                 Type
  )
/**
  @brief
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param[in] The protocol to find.
  @param[in] Return pointer to the first instance of the protocol
  @param[in] TRUE if the desired protocol is a FV protocol

  @retval EFI_SUCCESS - The function completed successfully.
  @retval EFI_NOT_FOUND - The protocol could not be located.
  @retval EFI_OUT_OF_RESOURCES - There are not enough resources to find the protocol.
**/
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       NumberOfHandles;
  EFI_FV_FILETYPE             FileType;
  UINT32                      FvStatus;
  EFI_FV_FILE_ATTRIBUTES      Attributes;
  UINTN                       Size;
  UINTN                       i;

  ///
  /// Locate protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {

    ///
    /// Defined errors at this time are not found and out of resources.
    ///
    return Status;
  }

  ///
  /// Looking for FV with ACPI storage file
  ///
  for (i = 0; i < NumberOfHandles; i++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    Protocol,
                    Instance
                    );
    ASSERT_EFI_ERROR (Status);

    if (!Type) {

      ///
      /// Not looking for the FV protocol, so find the first instance of the
      /// protocol.  There should not be any errors because our handle buffer
      /// should always contain at least one or LocateHandleBuffer would have
      /// returned not found.
      ///
      break;
    }

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = ((EFI_FIRMWARE_VOLUME_PROTOCOL *) (*Instance))->ReadFile (
                                                              *Instance,
                                                              &gCppcAcpiTableStorageGuid,
                                                              NULL,
                                                              &Size,
                                                              &FileType,
                                                              &Attributes,
                                                              &FvStatus
                                                              );

    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  return Status;
}
