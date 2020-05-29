/** @file
  This Dxe driver will initialize RapidStart Persistent Data Variable and also verify RapidStart Store valid or not

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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/RcFviDxeLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/RapidStartDxeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/RapidStartCommonLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/RcFviDxeLib.h>
#include <Library/GfxDisplayLibDxe.h>

#include <Protocol/RapidStartPlatformPolicy.h>
#include <Protocol/RapidStartInfo.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/RapidStartGlobalNvsArea.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/DxeGfxProtocol.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/DiskIo.h>

#include <Guid/RapidStartAcpiTableStorage.h>
#include <Guid/AcpiVariable.h>
#include <Guid/RapidStartPersistentData.h>

#include <RapidStartData.h>
#include <RapidStartAhciReg.h>
#include <Pi/PiFirmwareVolume.h>
#include <ProcessorBind.h>
#include <IndustryStandard/Acpi10.h>
#include <SaRegs.h>

//EFI_GUID                              gEfiAcpiVariableGuid = EFI_ACPI_VARIABLE_GUID;
//[-start-130315-IB06690220-add]//
EFI_GUID                                gAcpiVariableSetGuid = EFI_ACPI_VARIABLE_GUID;
//[-end-130315-IB06690220-add]//

RAPID_START_GLOBAL_NVS_AREA_PROTOCOL  mRapidStartGlobalNvsAreaProtocol;
RAPID_START_PERSISTENT_DATA           *mRapidStartData;
RAPID_START_PLATFORM_POLICY_PROTOCOL  *mRapidStartPolicy;
BOOLEAN                               mForceNewStoreUid = FALSE;
BOOLEAN                               SecondTimeSearchRSTPartition;

/**
  Search for RapidStart Store and verify if valid. Install ACPI table only when RapidStart can be performed.

  @param[in] Event    - The event that triggered this notification function
  @param[in] Context  - Pointer to the notification functions context
**/

VOID
EFIAPI
SearchRapidStartPartition (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
;

FVI_ELEMENT_AND_FUNCTION              mRapidStartFviElementsData[] = {
  {
    DEFAULT_FVI_ELEMENT_DATA(RAPID_START),
    NULL
  }
};

FVI_DATA_HUB_CALLBACK_CONTEXT         mRapidStartFviVersionData = {
  MISC_SUBCLASS_FVI_HEADER_ENTRY(RAPID_START),
  mRapidStartFviElementsData,
};

UINTN                                 mRapidStartFviElements = sizeof (mRapidStartFviElementsData) / sizeof (FVI_ELEMENT_AND_FUNCTION);

/**
  Installs RapidStart info protocols.

  @param[in] ImageHandle - Image handle for this driver image

  @retval EFI_OUT_OF_RESOURCES - Not enough resource to install RapidStart Info Protocol
  @retval EFI_SUCCESS - RapidStart Info Protocol installed successfully
**/
STATIC
EFI_STATUS
RapidStartInstallInfoProtocol (
  IN EFI_HANDLE       ImageHandle
  )
{
  RAPID_START_INFO_PROTOCOL *pRapidStartInfoProtocol;
  EFI_STATUS                Status;

  ///
  /// Install RAPID_START_INFO_PROTOCOL
  ///
  pRapidStartInfoProtocol = AllocateZeroPool (sizeof (RAPID_START_INFO_PROTOCOL));
  if (pRapidStartInfoProtocol == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  pRapidStartInfoProtocol->Revision = RAPID_START_INFO_PROTOCOL_REVISION_1;
  ///
  /// RCVersion[32:0] is the release number.
  /// For example:
  /// RapidStart Framework 0.5.0 should be 00 05 00 00 (0x00050000)
  ///
  pRapidStartInfoProtocol->RCVersion = RAPID_START_RC_VERSION;
  DEBUG ((EFI_D_INFO, "RapidStart Reference Code Revision = %X\n", pRapidStartInfoProtocol->RCVersion));
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gRapidStartInfoProtocolGuid,
                  pRapidStartInfoProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Allocates RapidStart memory ranges.

  @param[in] ImageHandle - Image handle for this driver image
  @param[in,out] RapidStartData    - Pointer to RapidStart persistent data
**/
STATIC
VOID
RapidStartAllocateMemory (
  IN EFI_HANDLE                       ImageHandle,
  IN OUT  RAPID_START_PERSISTENT_DATA *RapidStartData
  )
{
  EFI_PHYSICAL_ADDRESS  Addr;
  EFI_STATUS            Status;
  UINT64                MemMax;
  RAPID_START_MEM_DATA  *RapidStartMemData;
  EFI_STATUS            GfxStatus;
  DXE_GFX_PROTOCOL      *DxeGfxProtocol;
  UINT64                *DssAddress;
  UINT64                DssSize;

  DssAddress = NULL;
  DssSize = 0;

  if (RapidStartData->Tohm > MEM_EQU_4GB) {
    MemMax = RapidStartData->Tohm;
  } else {
    MemMax = RapidStartData->Tolm;
  }

  RapidStartData->ZeroBitmapSize = ALIGN ((UINT32) (RShiftU64 (NUMBER_OF_PAGES (MemMax), 3) + 8), EFI_PAGE_SIZE);
  RapidStartData->RapidStartMemSize = ALIGN (
                                        sizeof (RAPID_START_MEM_DATA) + AHCI_MEM_MAX_SIZE + RapidStartData->ZeroBitmapSize,
                                        EFI_PAGE_SIZE
                                        );
#ifdef RAPID_START_WHOLE_MEMORY_CHECK
  RapidStartData->Crc32RecordSize = ALIGN ((UINT32) DivU64x32 (MemMax, (MEMORY_CRC32_GRANULARITY / 4)), EFI_PAGE_SIZE);
  RapidStartData->RapidStartMemSize += RapidStartData->Crc32RecordSize;
#else
  RapidStartData->Crc32RecordSize = 0;
#endif

  if (mRapidStartPolicy->DisplaySaveRestore == 1) {
    GfxStatus = gBS->LocateProtocol (&gDxeGfxProtocolGuid, NULL, (VOID **)&DxeGfxProtocol);
    if (!EFI_ERROR (GfxStatus)) {
      Status = DxeGfxProtocol->AllocateDssBuffer ((UINT8)mRapidStartPolicy->DisplayType + 1);
      Status = DxeGfxProtocol->GetDssAddress (&DssAddress, &DssSize);
    }
  }

  RapidStartData->DssAddress = (UINT64)(DssAddress);
  RapidStartData->DssSize = DssSize;
  RapidStartData->DisplayType = (UINT8)(mRapidStartPolicy->DisplayType);
  DEBUG ((EFI_D_INFO, "RapidStart Display DssAddress = %x, DssSize = %x\n", DssAddress, DssSize));

  RapidStartData->HybridHardDisk = (UINT8)(mRapidStartPolicy->HybridHardDisk);
  ///
  /// Allocate RapidStart reserved memory
  ///
  Addr = (UINT64) AllocateReservedPages (EFI_SIZE_TO_PAGES (RapidStartData->RapidStartMemSize));
  ASSERT (Addr != 0);
  ASSERT (RShiftU64 (Addr, 32) == 0);

  RapidStartData->RapidStartMem = (UINT32) Addr;
  DEBUG ((EFI_D_ERROR, "RapidStartMem = %X\n", RapidStartData->RapidStartMem));
  DEBUG ((EFI_D_ERROR, "RapidStartMemSize = %X\n", RapidStartData->RapidStartMemSize));
  DEBUG ((EFI_D_ERROR, "ZeroBitmapSize = %X\n", RapidStartData->ZeroBitmapSize));
  DEBUG ((EFI_D_ERROR, "Crc32RecordSize = %X\n", RapidStartData->Crc32RecordSize));
  ///
  /// Initialize private memory data
  ///
  RapidStartMemData                 = RAPID_START_MEM_DATA_PTR (RapidStartData);
  RapidStartMemData->EntryCanceled  = 0;
  ///
  /// Allocate RapidStart AHCI MMIO
  ///
  Addr = 0xFFFFFFFF;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_SATA_AHCI_BAR_ALIGNMENT,  // 2^11: 2K Alignment
                  V_PCH_SATA_AHCI_BAR_LENGTH,     // 2K Length
                  &Addr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  RapidStartData->MmioSpace = (UINT32) Addr;
  ///
  /// Allocate Wlan MMIO
  ///
  Addr = 0xFFFFFFFF;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  15,                             // 2^15: 32K Alignment
                  0x8000,                         // 32K Length
                  &Addr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  RapidStartData->WlanMmioSpace = (UINT32) Addr;

}

/**
  Initialize RapidStart ACPI tables

  @retval EFI_SUCCESS    RapidStart ACPI tables are initialized successfully
  @retval EFI_NOT_FOUND  RapidStart ACPI tables not found
**/
STATIC
EFI_STATUS
InitializeRapidStartAcpiTables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         i;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         AcpiTableKey;
  UINT8                         *CurrPtr;
  UINT8                         *EndPtr;
  UINT32                        *Signature;
  EFI_ACPI_DESCRIPTION_HEADER   *RapidStartAcpiTable;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  FwVol               = NULL;
  RapidStartAcpiTable = NULL;

  ///
  /// Locate ACPI support protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locate protocol.
  /// There is little chance we can't find an FV protocol
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
  /// Looking for FV with ACPI storage file
  ///
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gRapidStartAcpiTableStorageGuid,
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
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gRapidStartAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table ID to modify the table
      ///
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('I', 'f', 'f', 's', 'A', 's', 'l', 0)) {
        RapidStartAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        ///
        /// Locate the SSDT package
        ///
        CurrPtr = (UINT8 *) RapidStartAcpiTable;
        EndPtr  = CurrPtr + RapidStartAcpiTable->Length;

        for (; CurrPtr <= EndPtr; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 3);
          if (*Signature == SIGNATURE_32 ('F', 'F', 'S', 'N')) {
            ASSERT_EFI_ERROR (*(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) == 0xFFFF0000);
            ASSERT_EFI_ERROR (*(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
            ///
            /// RapidStart Global NVS Area address
            ///
            *(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) = (UINT32) (UINTN) mRapidStartGlobalNvsAreaProtocol.Area;
            ///
            /// RapidStart Global NVS Area size
            ///
            *(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) =
            sizeof (RAPID_START_GLOBAL_NVS_AREA);

            AcpiTableKey = 0;
            Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  RapidStartAcpiTable,
                                  RapidStartAcpiTable->Length,
                                  &AcpiTableKey
                                  );
            ASSERT_EFI_ERROR (Status);
            return EFI_SUCCESS;
          }
        }
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;

}

/**
  Update RapidStart Persistent Data if necessary.

  @param[in] RapidStartData    - Pointer to RapidStart persistent data

  @retval EFI_SUCCESS - RapidStart Persistent Data updated successfully
**/
STATIC
EFI_STATUS
RapidStartUpdatePersistentData (
  IN      RAPID_START_PERSISTENT_DATA *RapidStartData
  )
{
  RAPID_START_PERSISTENT_DATA RapidStartDataOrg;
  UINTN                       RapidStartDataSize;
  EFI_STATUS                  Status;

  RapidStartDataSize = sizeof (RAPID_START_PERSISTENT_DATA);
  Status = gRT->GetVariable (
                  RAPID_START_PERSISTENT_DATA_NAME,
                  &gRapidStartPersistentDataGuid,
                  NULL,
                  &RapidStartDataSize,
                  &RapidStartDataOrg
                  );
  ///
  /// RapidStartData variable not present or RapidStart Store change detected in previous RapidStart Resume.
  /// Generate new RapidStart Store UID
  ///
  if (EFI_ERROR (Status) ||
      mForceNewStoreUid ||
      (RapidStartData->StoreSectors != RapidStartDataOrg.StoreSectors) ||
      (RapidStartData->StoreLbaAddr != RapidStartDataOrg.StoreLbaAddr) ||
      (RapidStartData->StoreSataPort != RapidStartDataOrg.StoreSataPort)
      ) {
    DEBUG ((EFI_D_ERROR, "Generate RapidStart Store UID.\n"));
    RapidStartData->RapidStartStoreUid = GenerateRapidStartStoreUid ();
  } else {
    RapidStartData->RapidStartStoreUid = RapidStartDataOrg.RapidStartStoreUid;
  }
  ///
  /// Check for changes in RapidStartData
  ///
  if ((Status == EFI_SUCCESS) &&
      (CompareMem (&RapidStartDataOrg, RapidStartData, sizeof (RAPID_START_PERSISTENT_DATA)) == 0)
      ) {
    return EFI_SUCCESS;
  }

  Status = gRT->SetVariable (
                  RAPID_START_PERSISTENT_DATA_NAME,
                  &gRapidStartPersistentDataGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (RAPID_START_PERSISTENT_DATA),
                  RapidStartData
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Installs Search RapidStart Partition handler by DiskIoProtocol.
**/
VOID
RegisterForSearchRapidStartPartitionAgain (
  VOID
  )
{
  EFI_EVENT   Event;
  EFI_STATUS  Status;
  VOID        *Registration;

  //
  // Set this flag to TRUE and we will not search RapidStart partition again.
  //
  SecondTimeSearchRSTPartition = TRUE;
  //
  // Create a callback event to detect RapidStart Partition
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SearchRapidStartPartition,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Register DISK_IO_PROTOCOL notify function
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiDiskIoProtocolGuid,
                  Event,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Search for RapidStart Store and verify if valid. Install ACPI table only when RapidStart can be performed.

  @param[in] Event    - The event that triggered this notification function
  @param[in] Context  - Pointer to the notification functions context
**/

VOID
EFIAPI
SearchRapidStartPartition (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS  Status;
  BOOLEAN     DisableRapidStart;
  BOOLEAN     CheckSize;
  UINT32      ThresholdSize;

  DEBUG ((EFI_D_INFO, "SearchRapidStartPartition Start()\n"));

  DisableRapidStart = FALSE;
  CheckSize         = FALSE;
  ThresholdSize     = MEM_TO_SECT (mRapidStartData->TotalMem);

  ///
  /// RapidStart has been enabled, check if RapidStart store valid.
  ///
  Status = SearchRapidStartStore (
            mRapidStartPolicy,
            &(mRapidStartData->StoreSectors),
            &(mRapidStartData->StoreLbaAddr),
            &(mRapidStartData->StoreSataPort)
            );
  mRapidStartData->ActivePageThresholdSizeInSector = 0;

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "RapidStart parition not found\n"));
    
    if (SecondTimeSearchRSTPartition == FALSE) {
      RegisterForSearchRapidStartPartitionAgain ();
      DEBUG ((EFI_D_ERROR, " - Will search RapidStart parition again\n"));
      gBS->CloseEvent (Event);
      return;
    }

    DisableRapidStart = TRUE;
  } else if (mRapidStartPolicy->ActivePageThresholdSupport == 0) {
    CheckSize = TRUE;
  } else {
    DEBUG ((EFI_D_ERROR, "RapidStart Partition size less than memory size is supported\n"));
    if (mRapidStartPolicy->ActivePageThresholdSize == 0) {
      DEBUG ((EFI_D_ERROR, "RapidStart Partition size check in Auto Mode\n"));
    } else {
      ///
      /// ActivePageThreshold size in MB
      ///
      ThresholdSize = (UINT32)MultU64x32 (mRapidStartPolicy->ActivePageThresholdSize, MEM_TO_SECT (MEM_EQU_1MB));
      mRapidStartData->ActivePageThresholdSizeInSector = ThresholdSize;
      CheckSize     = TRUE;
    }
  }

  if (CheckSize) {
    if (ThresholdSize > mRapidStartData->StoreSectors) {
      DEBUG ((EFI_D_ERROR, "RapidStart partition too small\n"));
      DisableRapidStart = TRUE;
    }
  }

  if (DisableRapidStart) {
    DEBUG ((EFI_D_ERROR, "Disabling RapidStart!\n"));
    ///
    /// Disable RapidStart
    ///
    mRapidStartPolicy->EnableRapidStart = 0;
    ///
    /// Delete RapidStart variable
    ///
    Status = gRT->SetVariable (
                    RAPID_START_PERSISTENT_DATA_NAME,
                    &gRapidStartPersistentDataGuid,
                    0,
                    0,
                    NULL
                    );
    ///
    /// Disable FFS relevant SMI handlers.
    ///
    mRapidStartGlobalNvsAreaProtocol.Area->EventsAvailable  = 0;
    mRapidStartGlobalNvsAreaProtocol.Area->EventsEnabled    = 0;
  } else {
    ///
    /// Enabling RapidStart
    /// install RapidStart ACPI Table.
    ///
    DEBUG ((EFI_D_INFO, "Enabling RapidStart\n"));
    DEBUG ((EFI_D_ERROR, "RapidStart Store:\n"));
    DEBUG ((EFI_D_ERROR, "  Port         : %X\n", mRapidStartData->StoreSataPort));
    DEBUG ((EFI_D_ERROR, "  LbaAddr      : %X\n", mRapidStartData->StoreLbaAddr));
    DEBUG ((EFI_D_ERROR, "  Size         : %X Sectors\n", mRapidStartData->StoreSectors));
    DEBUG ((EFI_D_ERROR, "Threshold Size : %X Sectors\n", mRapidStartData->ActivePageThresholdSizeInSector));

    Status = InitializeRapidStartAcpiTables ();
    ASSERT_EFI_ERROR (Status);
#ifndef RAPID_START_ON_MEMORY_INSTALLED
    RapidStartUpdatePersistentData (mRapidStartData);
#endif
  }

  gBS->CloseEvent (Event);
}

/**
  Installs Search RapidStart Partition handler.
**/
VOID
RegisterForSearchRapidStartPartition (
  VOID
  )
{
  EFI_EVENT   Event;
  EFI_STATUS  Status;

  SecondTimeSearchRSTPartition = FALSE;

  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            SearchRapidStartPartition,
            NULL,
            &Event
            );
  ASSERT_EFI_ERROR (Status);
}

#ifdef RAPID_START_ON_MEMORY_INSTALLED
/**
  Updates pointers necessary for S3 resume to RapidStart Flash variable.
  This has to be postponed till ReadyToBoot as framework may update
  this pointers late in a boot flow.

  @param[in] Event    - The event that triggered this notification function
  @param[in] Context  - Pointer to the notification functions context
**/

VOID
EFIAPI
RapidStartOnExitBootServices (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  UINT64            AcpiVariableSet64;
  ACPI_VARIABLE_SET *AcpiVariableSet;
  UINTN             VarSize;
  EFI_STATUS        Status;

  DEBUG ((EFI_D_INFO, "RapidStartOnExitBootServices()\n"));

  if (mRapidStartPolicy->EnableRapidStart) {
    VarSize = sizeof (AcpiVariableSet64);
    Status = gRT->GetVariable (
                    ACPI_GLOBAL_VARIABLE,
//[-start-130315-IB06690220-modify]//
//                    &gEfiAcpiVariableGuid,
                    &gAcpiVariableSetGuid,
//[-end-130315-IB06690220-modify]//
                    NULL,
                    &VarSize,
                    &AcpiVariableSet64
                    );
    ASSERT_EFI_ERROR (Status);

    AcpiVariableSet                         = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;

    mRapidStartData->AcpiReservedMemoryBase = AcpiVariableSet->AcpiReservedMemoryBase;
    mRapidStartData->AcpiReservedMemorySize = AcpiVariableSet->AcpiReservedMemorySize;
#ifdef RAPID_START_USE_OLD_ACPI_VARIABLE_SET
    mRapidStartData->SystemMemoryLengthBelow4GB = AcpiVariableSet->SystemMemoryLength;
    mRapidStartData->SystemMemoryLengthAbove4GB = 0;
#else
    mRapidStartData->SystemMemoryLengthBelow4GB = AcpiVariableSet->SystemMemoryLengthBelow4GB;
    mRapidStartData->SystemMemoryLengthAbove4GB = AcpiVariableSet->SystemMemoryLengthAbove4GB;
#endif
    DEBUG ((EFI_D_INFO, "AcpiReservedMemoryBase = %0lx\n", mRapidStartData->AcpiReservedMemoryBase));
    DEBUG ((EFI_D_INFO, "AcpiReservedMemorySize = %0x\n", mRapidStartData->AcpiReservedMemorySize));
    DEBUG ((EFI_D_INFO, "SystemMemoryLengthBelow4GB = %0lx\n", mRapidStartData->SystemMemoryLengthBelow4GB));
    DEBUG ((EFI_D_INFO, "SystemMemoryLengthAbove4GB = %0lx\n", mRapidStartData->SystemMemoryLengthAbove4GB));
    RapidStartUpdatePersistentData (mRapidStartData);
  }

  gBS->CloseEvent (Event);
}

/**
  Installs ExitBootService handler.
**/
VOID
RegisterForExitBootService (
  VOID
  )
{
  EFI_EVENT   Event;
  EFI_STATUS  Status;

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  RapidStartOnExitBootServices,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventLegacyBootEx (
            TPL_CALLBACK,
            RapidStartOnExitBootServices,
            NULL,
            &Event
            );
  ASSERT_EFI_ERROR (Status);
}
#endif /* RAPID_START_ON_MEMORY_INSTALLED */

/**
  Initial RC Firmware Version Info
**/
VOID
InitRcFvi (
  VOID
  )
{
  InitFviDataHubCbContext (
    mRapidStartPolicy->FviSmbiosType,
    (UINT8) mRapidStartFviElements,
    &mRapidStartFviVersionData
    );

  CreateRcFviDatahub (&mRapidStartFviVersionData);
}

/**
  RapidStart DXE driver entry point function

  @param[in] ImageHandle - Image handle for this driver image
  @param[in] SystemTable - Pointer to the EFI System Table

  @retval EFI_OUT_OF_RESOURCES - no enough memory resource when installing reference code information protocol
  @retval EFI_SUCCESS          - function completed successfully
**/
EFI_STATUS
EFIAPI
RapidStartDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  RAPID_START_GLOBAL_NVS_AREA *RapidStartGlobalNvs;
  UINT16                      SaDeviceId;
  EFI_STATUS                  Status;
  UINTN                       MchBase;
  UINT8                       RapidStartFlag;

  DEBUG ((EFI_D_INFO, "RapidStartDxeEntryPoint()\n"));

  Status = RapidStartInstallInfoProtocol (ImageHandle);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  ///
  /// Locate RapidStart platform policy.
  ///
  Status = gBS->LocateProtocol (
                  &gRapidStartPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID **) &mRapidStartPolicy
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No RapidStart Platform Policy Protocol available"));
    ASSERT_EFI_ERROR (Status);
  }
  ///
  /// Initial RC Firmware Version Info
  ///
  InitRcFvi ();

  if ((!EFI_ERROR (Status)) && (mRapidStartPolicy->EnableRapidStart)) {

    Status = RapidStartGetFlag (&RapidStartFlag);
    if (!EFI_ERROR (Status) && (RapidStartFlag & RAPID_START_FLAG_STORE_CHANGE)) {
      mForceNewStoreUid = TRUE;
    }

    RapidStartSetFlag (0);

    Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (RAPID_START_GLOBAL_NVS_AREA), (VOID **) &mRapidStartGlobalNvsAreaProtocol.Area);
    ASSERT_EFI_ERROR (Status);
    ZeroMem ((VOID *) mRapidStartGlobalNvsAreaProtocol.Area, sizeof (RAPID_START_GLOBAL_NVS_AREA));

    Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (RAPID_START_PERSISTENT_DATA), &mRapidStartGlobalNvsAreaProtocol.RapidStartData);
    ASSERT_EFI_ERROR (Status);
    ZeroMem ((VOID *) mRapidStartGlobalNvsAreaProtocol.RapidStartData, sizeof (RAPID_START_PERSISTENT_DATA));

    mRapidStartData = mRapidStartGlobalNvsAreaProtocol.RapidStartData;

    ///
    /// Get Tolm and Tohm
    ///
    MchBase               = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);
    mRapidStartData->Tolm = MmioRead32 (MchBase + R_SA_BDSM) & B_SA_BDSM_BDSM_MASK;
    mRapidStartData->Tohm = MmioRead64 (MchBase + R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
    ///
    /// Calculate total size of memory to be stored
    ///
    mRapidStartData->TotalMem = mRapidStartData->Tolm;
    if (mRapidStartData->Tohm > MEM_EQU_4GB) {
      mRapidStartData->TotalMem += mRapidStartData->Tohm - MEM_EQU_4GB;
    }

    if (mRapidStartPolicy->EntryOnS3RtcWake == 0 && mRapidStartPolicy->EntryOnS3CritBattWake == 0) {
      DEBUG ((EFI_D_WARN, "RapidStart both S3 wakeup timer and critical battery wake event are disabled\n"));
    }

    RapidStartGlobalNvs                   = mRapidStartGlobalNvsAreaProtocol.Area;
    RapidStartGlobalNvs->EventsAvailable  = RAPID_START_ACPI_RTC_WAKE;

    if (mRapidStartPolicy->S3WakeTimerMin <= RAPID_START_WAKE_TIMER_MAX) {
      RapidStartGlobalNvs->WakeTimerMin = mRapidStartPolicy->S3WakeTimerMin;
    } else {
      DEBUG ((EFI_D_WARN, "RapidStart S3 wakeup timer out of range - resetting to default\n"));
      RapidStartGlobalNvs->WakeTimerMin = RAPID_START_WAKE_TIMER_DEFAULT;
    }

    if (mRapidStartPolicy->EntryOnS3RtcWake) {
      RapidStartGlobalNvs->EventsEnabled |= RAPID_START_ACPI_RTC_WAKE;
    }

    SaDeviceId = MmioRead16 (
                  MmPciAddress (
                  0,
                  SA_MC_BUS,
                  SA_MC_DEV,
                  SA_MC_FUN,
                  R_SA_MC_DEVICE_ID)
                  );

    if (IS_SA_DEVICE_ID_MOBILE (SaDeviceId)) {
      RapidStartGlobalNvs->EventsAvailable |= RAPID_START_ACPI_BATT_WAKE;
      if ((mRapidStartPolicy->EntryOnS3CritBattWake)) {
        RapidStartGlobalNvs->EventsEnabled |= RAPID_START_ACPI_BATT_WAKE;
      }
    }

    mRapidStartData->RapidStartGlobalNvsPtr = (UINT32) (UINTN) RapidStartGlobalNvs;

    mRapidStartData->SataPortConfiguration = MmioRead8 (
                                              MmPciAddress (
                                              0,
                                              DEFAULT_PCI_BUS_NUMBER_PCH,
                                              PCI_DEVICE_NUMBER_PCH_SATA,
                                              PCI_FUNCTION_NUMBER_PCH_SATA,
                                              R_PCH_SATA_PCS)
                                              );

    RapidStartAllocateMemory (ImageHandle, mRapidStartData);

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gRapidStartGlobalNvsAreaProtocolGuid,
                    &mRapidStartGlobalNvsAreaProtocol,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    RegisterForSearchRapidStartPartition ();
#ifdef RAPID_START_ON_MEMORY_INSTALLED
    RegisterForExitBootService ();
#endif
    RegisterForRapidStartDisplayScreenCallback();
    return EFI_SUCCESS;
  }
  ///
  /// Delete RapidStart variable
  ///
  Status = gRT->SetVariable (
                  RAPID_START_PERSISTENT_DATA_NAME,
                  &gRapidStartPersistentDataGuid,
                  0,
                  0,
                  NULL
                  );
  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
