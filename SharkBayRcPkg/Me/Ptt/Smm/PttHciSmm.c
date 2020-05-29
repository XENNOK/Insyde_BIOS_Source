/** @file

  It updates TPM2 items in ACPI table and registers SMI2 callback
  functions for TrEE physical presence, ClearMemory, and sample
  for dTPM StartMethod.

  Caution: This module requires additional review when modified.
  This driver will have external input - variable and ACPINvs data in SMM mode.
  This external input must be validated carefully to avoid security issue.

  PhysicalPresenceCallback() and MemoryClearCallback() will receive untrusted input and do some check.

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
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
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

#include "PttHciSmm.h"
#include <MeAccess.h>

EFI_GUID gTpm2AcpiTableStorageGuid            = TPM2_ACPI_TABLE_STORAGE_GUID;
EFI_GUID gEfiTrEEPhysicalPresenceGuid         = EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID;
EFI_GUID gEfiMemoryOverwriteControlDataGuid   = MEMORY_ONLY_RESET_CONTROL_GUID;
EFI_GUID gEfiSmmVariableProtocolGuid          = EFI_SMM_VARIABLE_PROTOCOL_GUID;
EFI_GUID gMeGlobalNvsAreaProtocolGuid         = ME_GLOBAL_NVS_AREA_PROTOCOL_GUID;
EFI_GUID gMeSsdtAcpiTableStorageGuid          = ME_SSDT_ACPI_TABLE_STORAGE_GUID;

EFI_TPM2_ACPI_TABLE  mTpm2AcpiTemplate = {
  {
    EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE,
    sizeof (mTpm2AcpiTemplate),
    EFI_TPM2_ACPI_TABLE_REVISION,
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  0, // Flags
  (EFI_PHYSICAL_ADDRESS)(UINTN)0xFFFFFFFF, // Control Area
  EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI,
};

EFI_SMM_VARIABLE_PROTOCOL    *mSmmVariable;
TCG_NVS                      *mTcgNvs;
EFI_TPM2_ACPI_CONTROL_AREA   mControlArea; // Smm copy, because we need cache Command & Response address
ME_GLOBAL_NVS_AREA_PROTOCOL  mMeGlobalNvsAreaProtocol;

/**
  Software SMI callback for TPM physical presence which is called from ACPI method.

  Caution: This function may receive untrusted input.
  Variable and ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.

  @retval EFI_SUCCESS             The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
PhysicalPresenceCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
  )
{
  EFI_STATUS                     Status;
  UINTN                          DataSize;
  EFI_TREE_PHYSICAL_PRESENCE     PpData;
  UINT8                          Flags;
  BOOLEAN                        RequestConfirmed;

  //
  // Get the Physical Presence variable
  //
  DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
  Status = mSmmVariable->SmmGetVariable (
                           TREE_PHYSICAL_PRESENCE_VARIABLE,
                           &gEfiTrEEPhysicalPresenceGuid,
                           NULL,
                           &DataSize,
                           &PpData
                           );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "[TPM2] PP callback, Parameter = %x\n", mTcgNvs->PhysicalPresence.Parameter));
  if (mTcgNvs->PhysicalPresence.Parameter == ACPI_FUNCTION_RETURN_REQUEST_RESPONSE_TO_OS) {
    mTcgNvs->PhysicalPresence.LastRequest = PpData.LastPPRequest;
    mTcgNvs->PhysicalPresence.Response    = PpData.PPResponse;
  } else if ((mTcgNvs->PhysicalPresence.Parameter == ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS)
          || (mTcgNvs->PhysicalPresence.Parameter == ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS_2)) {
    if (PpData.PPRequest != mTcgNvs->PhysicalPresence.Request) {
      PpData.PPRequest = (UINT8) mTcgNvs->PhysicalPresence.Request;
      DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
      Status = mSmmVariable->SmmSetVariable (
                               TREE_PHYSICAL_PRESENCE_VARIABLE,
                               &gEfiTrEEPhysicalPresenceGuid,
                               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                               DataSize,
                               &PpData
                               );
    }

    if (EFI_ERROR (Status)) {
      mTcgNvs->PhysicalPresence.ReturnCode = PP_SUBMIT_REQUEST_GENERAL_FAILURE;
      return EFI_SUCCESS;
    }
    mTcgNvs->PhysicalPresence.ReturnCode = PP_SUBMIT_REQUEST_SUCCESS;
  } else if (mTcgNvs->PhysicalPresence.Parameter == ACPI_FUNCTION_GET_USER_CONFIRMATION_STATUS_FOR_REQUEST) {
    Flags = PpData.Flags;
    RequestConfirmed = FALSE;

    switch (mTcgNvs->PhysicalPresence.Request) {

      case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR:
      case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2:
      case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3:
      case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4:
        if ((Flags & TREE_FLAG_NO_PPI_CLEAR) != 0) {
          RequestConfirmed = TRUE;
        }
        break;

      case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE:
        RequestConfirmed = TRUE;
        break;

      case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE:
        break;

      default:
        if (mTcgNvs->PhysicalPresence.Request <= TREE_PHYSICAL_PRESENCE_NO_ACTION_MAX) {
          RequestConfirmed = TRUE;
        }
        break;
    }

    if (RequestConfirmed) {
      mTcgNvs->PhysicalPresence.ReturnCode = PP_REQUEST_ALLOWED_AND_PPUSER_NOT_REQUIRED;
    } else {
      mTcgNvs->PhysicalPresence.ReturnCode = PP_REQUEST_ALLOWED_AND_PPUSER_REQUIRED;
    }
  }

  return EFI_SUCCESS;
}

/**
  Software SMI callback for MemoryClear which is called from ACPI method.

  Caution: This function may receive untrusted input.
  Variable and ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.

  @retval EFI_SUCCESS             The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
MemoryClearCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
  )
{
  EFI_STATUS                     Status;
  UINTN                          DataSize;
  UINT8                          MorControl;

  mTcgNvs->MemoryClear.ReturnCode = MOR_REQUEST_SUCCESS;
  if (mTcgNvs->MemoryClear.Parameter == ACPI_FUNCTION_DSM_MEMORY_CLEAR_INTERFACE) {
    MorControl = (UINT8) mTcgNvs->MemoryClear.Request;
  } else if (mTcgNvs->MemoryClear.Parameter == ACPI_FUNCTION_PTS_CLEAR_MOR_BIT) {
    DataSize = sizeof (UINT8);
    Status = mSmmVariable->SmmGetVariable (
                             MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             NULL,
                             &DataSize,
                             &MorControl
                             );
    if (EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }

    if (MOR_CLEAR_MEMORY_VALUE (MorControl) == 0x0) {
      return EFI_SUCCESS;
    }
    MorControl &= ~MOR_CLEAR_MEMORY_BIT_MASK;
  }

  DataSize = sizeof (UINT8);
  Status = mSmmVariable->SmmSetVariable (
                           MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                           &gEfiMemoryOverwriteControlDataGuid,
                           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                           DataSize,
                           &MorControl
                           );
  if (EFI_ERROR (Status)) {
    mTcgNvs->MemoryClear.ReturnCode = MOR_REQUEST_GENERAL_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Find the operation region in TCG ACPI table by given Name and Size,
  and initialize it if the region is found.

  @param[in, out] Table          The TPM item in ACPI table.
  @param[in]      Name           The name string to find in TPM table.
  @param[in]      Size           The size of the region to find.

  @return                        The allocated address for the found region.
**/
VOID *
AssignOpRegion (
  EFI_ACPI_DESCRIPTION_HEADER    *Table,
  UINT32                         Name,
  UINT16                         Size
  )
{
  EFI_STATUS                     Status;
  AML_OP_REGION_32_8             *OpRegion;
  EFI_PHYSICAL_ADDRESS           MemoryAddress;

  MemoryAddress = 0xFFFFFFFF;

  //
  // Patch some pointers for the ASL code before loading the SSDT.
  //
  for (OpRegion  = (AML_OP_REGION_32_8 *) (Table + 1);
       OpRegion <= (AML_OP_REGION_32_8 *) ((UINT8 *) Table + Table->Length);
       OpRegion  = (AML_OP_REGION_32_8 *) ((UINT8 *) OpRegion + 1)) {
    if ((OpRegion->OpRegionOp  == AML_EXT_REGION_OP) &&
        (OpRegion->NameString  == Name) &&
        (OpRegion->DWordPrefix == AML_DWORD_PREFIX) &&
        (OpRegion->BytePrefix  == AML_BYTE_PREFIX)) {

      Status = (gBS->AllocatePages)(AllocateMaxAddress, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES (Size), &MemoryAddress);
      ASSERT_EFI_ERROR (Status);
      ZeroMem ((VOID *)(UINTN)MemoryAddress, Size);
      OpRegion->RegionOffset = (UINT32) (UINTN) MemoryAddress;
      OpRegion->RegionLen    = (UINT8) Size;
      break;
    }
  }

  return (VOID *) (UINTN) MemoryAddress;
}

VOID *
GetTpm2AcpiTableFromFv (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  EFI_FV_FILETYPE                 FileType;
  UINT32                          FvStatus;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINTN                           Size;
  UINTN                           Index;
  INTN                            Instance;
  EFI_ACPI_COMMON_HEADER          *CurrentTable;
  EFI_FIRMWARE_VOLUME_PROTOCOL    *FwVol;

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

  FwVol = NULL;
  ///
  /// Looking for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gTpm2AcpiTableStorageGuid,
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
    } else {
      FwVol = NULL;
    }
  }

  if ((Index == NumberOfHandles) || (FwVol == NULL)) {
    return NULL;
  }

  Instance      = 0;
  CurrentTable  = NULL;

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gTpm2AcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the Signature ID to modify the table
      ///
      if ((((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Signature == SIGNATURE_32 ('S', 'S', 'D', 'T')) &&
          (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('T', 'p', 'm', '2', 'T', 'a', 'b', 'l'))) {
        //
        // Find it.
        //
        break;
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  (gBS->FreePool) (HandleBuffer);

  return CurrentTable;
}

/**
  Initialize and publish TPM items in ACPI table.

  @retval   EFI_SUCCESS     The TCG ACPI table is published successfully.
  @retval   Others          The TCG ACPI table is not published.
**/
EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                          TableKey;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;
  UINTN                          TableSize;

  Table = GetTpm2AcpiTableFromFv ();
  ASSERT (Table != NULL);

  if (Table == NULL) {
     return EFI_NOT_FOUND;
  }

  TableSize = Table->Length;

  ASSERT (Table->OemTableId == SIGNATURE_64 ('T', 'p', 'm', '2', 'T', 'a', 'b', 'l'));
  mTcgNvs = AssignOpRegion (Table, SIGNATURE_32 ('T', 'N', 'V', 'S'), (UINT16) sizeof (TCG_NVS));
  ASSERT (mTcgNvs != NULL);

  //
  // Publish the TPM ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  TableKey = 0;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        Table,
                        TableSize,
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
PublishFtpmAcpiTable (
  VOID
  )
/**
  Publish TPM2 ACPI table

  @retval   EFI_SUCCESS     The TPM2 ACPI table is published successfully.
  @retval   Others          The TPM2 ACPI table is not published.
**/
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                          TableKey;
  EFI_TPM2_ACPI_CONTROL_AREA     *ControlArea;
  ME_DATA_HOB                    *MeDataHob;
  EFI_GUID                       gMeDataHobGuid   = ME_DATA_HOB_GUID;

  ///
  /// Construct ACPI table
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  MeDataHob       = NULL;
  MeDataHob = GetFirstGuidHob (&gMeDataHobGuid);
  if (MeDataHob != NULL) {
    (UINTN)mTpm2AcpiTemplate.AddressOfControlArea = MeDataHob->FtpmBufferAddress;
  } else {
    (UINTN) mTpm2AcpiTemplate.AddressOfControlArea = 0xFED70000;
  }

  ControlArea = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN)mTpm2AcpiTemplate.AddressOfControlArea;
  ZeroMem (ControlArea, sizeof(*ControlArea));
  ControlArea->CommandSize  = 0xF80;
  ControlArea->ResponseSize = 0xF80;
  ControlArea->Command      = (UINTN)mTpm2AcpiTemplate.AddressOfControlArea + 0x80;
  ControlArea->Response     = (UINTN)mTpm2AcpiTemplate.AddressOfControlArea + 0x80;
  CopyMem (&mControlArea, ControlArea, sizeof(mControlArea));

  DEBUG ((EFI_D_INFO, "Ftpm Windows Buffer Control Area Address = %x\n", mTpm2AcpiTemplate.AddressOfControlArea));
  DEBUG ((EFI_D_INFO, "Ftpm Windows Command/Response Buffer Address = %x\n", ControlArea->Command));

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &mTpm2AcpiTemplate,
                        sizeof(mTpm2AcpiTemplate),
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

BOOLEAN
IsFtpmEnabled (
  VOID
  )
{
  UINT32 IsPttEnabled;
  UINT32 IsPttReady;
  UINT32 MeFwSts4;

  IsPttEnabled = ( B_PTT_HCI_STS_ENABLED & MmioRead32((UINTN)(R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_STS)) );
  ///
  /// Read ME FWSTS4 to check if TPM_DISCONNECT_ALL BIT[12] is set
  ///
  MeFwSts4 = HeciPciRead32(R_ME_HFS_4); 

  ///
  /// Read Ready Bit to identify if PTT is ready for accepting OS Commands
  ///
  IsPttReady = (B_PTT_HCI_STS_READY & MmioRead32((UINTN)(R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_STS)));

  if ( (IsPttEnabled == 0) || (MeFwSts4 & BIT12) || (IsPttReady == 0) ) {
    return FALSE;
  }

  return TRUE;
}

EFI_STATUS
InitializeMeSsdtAcpiTables (
IN EFI_HANDLE                  ImageHandle
  )
/**
@brief
  Initialize MEFW SSDT ACPI tables

  @retval EFI_SUCCESS    ACPI tables are initialized successfully
  @retval EFI_NOT_FOUND  ACPI tables not found
**/
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
  EFI_ACPI_DESCRIPTION_HEADER   *MeAcpiTable;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  Status = (gBS->AllocatePool) (EfiReservedMemoryType, sizeof (ME_GLOBAL_NVS_AREA), (VOID **) &mMeGlobalNvsAreaProtocol.Area);
  ASSERT_EFI_ERROR (Status);
  ZeroMem ((VOID *) mMeGlobalNvsAreaProtocol.Area, sizeof (ME_GLOBAL_NVS_AREA));

  FwVol       = NULL;
  MeAcpiTable = NULL;

  ///
  /// Locate ACPI Table protocol
  ///
  DEBUG ((EFI_D_INFO, "Init ME SSDT table\n"));
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Fail to locate EfiAcpiTableProtocol.\n"));
    return EFI_NOT_FOUND;
  }

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
                    (VOID **)&FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gMeSsdtAcpiTableStorageGuid,
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
    DEBUG ((EFI_D_INFO, "ME Global NVS table not found\n"));
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
                      &gMeSsdtAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table ID to modify the table
      ///
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('M', 'e', 'S', 's', 'd', 't', ' ', 0)) {
        MeAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        ///
        /// Locate the SSDT package
        ///
        CurrPtr = (UINT8 *) MeAcpiTable;
        EndPtr  = CurrPtr + MeAcpiTable->Length;

        for (; CurrPtr <= EndPtr; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 3);
          if (*Signature == SIGNATURE_32 ('M', 'E', 'N', 'V')) {
            ASSERT_EFI_ERROR (*(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) == 0xFFFF0000);
            ASSERT_EFI_ERROR (*(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
            ///
            /// ME Global NVS Area address
            ///

            *(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) = (UINT32) (UINTN) mMeGlobalNvsAreaProtocol.Area;
            ///
            /// ME Global NVS Area size
            ///
            *(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) =
            sizeof (ME_GLOBAL_NVS_AREA);

            AcpiTableKey = 0;
            Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  MeAcpiTable,
                                  MeAcpiTable->Length,
                                  &AcpiTableKey
                                  );
            ASSERT_EFI_ERROR (Status);

            Status = gBS->InstallMultipleProtocolInterfaces (
              &ImageHandle,
              &gMeGlobalNvsAreaProtocolGuid,
              &mMeGlobalNvsAreaProtocol,
              NULL
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
  The driver's entry point.

  It install callbacks for TPM physical presence and MemoryClear, and locate
  SMM variable to be used in the callback function.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
InitializeFtpmSmm (
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH_PROTOCOL   *SwDispatch;
  EFI_SMM_SW_DISPATCH_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;
  ME_GLOBAL_NVS_AREA_PROTOCOL    *MeGlobalNvsAreaProtocol;
  ME_GLOBAL_NVS_AREA             *MeGlobalNvsArea;

  if (!IsFtpmEnabled ()) {
    DEBUG ((EFI_D_ERROR, "InitializeFtpmSmm - FTPM not enabled\n"));
    return EFI_SUCCESS;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_SUCCESS;
  }

if (GetCpuFamily() == EnumCpuHswUlt) {
    Status = PublishAcpiTable ();
    ASSERT_EFI_ERROR (Status);

    //
    // Get the Sw dispatch protocol and register SMI callback functions.
    //
    Status = gBS->LocateProtocol (&gEfiSmmSwDispatchProtocolGuid, NULL, (VOID**)&SwDispatch);
    ASSERT_EFI_ERROR (Status);
    SwContext.SwSmiInputValue = EFI_TPM2_PP_SW_SMI;
    Status = SwDispatch->Register (SwDispatch, PhysicalPresenceCallback, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mTcgNvs->PhysicalPresence.SoftwareSmi = (UINT8) SwContext.SwSmiInputValue;

    SwContext.SwSmiInputValue = EFI_TPM2_MOR_SW_SMI;
    Status = SwDispatch->Register (SwDispatch, MemoryClearCallback, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mTcgNvs->MemoryClear.SoftwareSmi = (UINT8) SwContext.SwSmiInputValue;

    ///
    /// Locate SmmVariableProtocol.
    ///
    Status = gBS->LocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&mSmmVariable);
    ASSERT_EFI_ERROR (Status);

    Status = InitializeMeSsdtAcpiTables(ImageHandle);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Publish TPM2 ACPI table
    ///
    Status = PublishFtpmAcpiTable();
    ASSERT_EFI_ERROR (Status);

    ///
    /// Locate Global NVS and update PTT Buffer Address
    ///
    Status = gBS->LocateProtocol (&gMeGlobalNvsAreaProtocolGuid, NULL, (VOID **) &MeGlobalNvsAreaProtocol);
    ASSERT_EFI_ERROR (Status);

    MeGlobalNvsArea             = MeGlobalNvsAreaProtocol->Area;
    MeGlobalNvsArea->PTTAddress = mTpm2AcpiTemplate.AddressOfControlArea;
}

  return EFI_SUCCESS;
}

