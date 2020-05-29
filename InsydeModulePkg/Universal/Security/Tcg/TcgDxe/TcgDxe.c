/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This module implements TCG EFI Protocol.

Copyright (c) 2005 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <IndustryStandard/Tpm12.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/PeImage.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/TcpaAcpi.h>

#include <Guid/GlobalVariable.h>
#include <Guid/SmBios.h>
#include <Guid/HobList.h>
#include <Guid/TcgEventHob.h>
#include <Guid/EventGroup.h>
#include <Guid/EventExitBootServiceFailed.h>
#include <Guid/TpmInstance.h>

#include <Protocol/DevicePath.h>
#include <Protocol/TcgService.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/TrEEMeasureOsLoaderAuthority.h>
#include <Protocol/TrEEProtocol.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/TpmCommLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>

#include <TpmComm.h>
#include <PostCode.h>
#include <TpmPolicy.h>

#define  EFI_TCG_LOG_AREA_SIZE        0x10000

typedef UINT32  EFI_TPM_EVENT_LOG_INFORMATION_HEADER;

#pragma pack (push, 1)
typedef struct _EFI_TPM_EVENT_LOG_INFORMATION_TABLE {
  EFI_TPM_EVENT_LOG_INFORMATION_HEADER   Header;
  UINT32                                 StartAddress;
  UINT32                                 EventLogSize;
  UINT32                                 LastEventAddress;
  UINT16                                 LeftSize;
  UINT16                                 Entries;
} EFI_TPM_EVENT_LOG_INFORMATION_TABLE;
#pragma pack (pop)

#define TCG_DXE_DATA_FROM_THIS(this)  \
  BASE_CR (this, TCG_DXE_DATA, TcgProtocol)

typedef struct _TCG_DXE_DATA {
  EFI_TCG_PROTOCOL                  TcgProtocol;
  TCG_EFI_BOOT_SERVICE_CAPABILITY   BsCap;
  EFI_PHYSICAL_ADDRESS              LogAreaStartAddress;
  UINTN                             MaxLogAreaSize;
  UINTN                             EventLogSize;
  UINT8                             *LastEvent;
  TIS_TPM_HANDLE                    TpmHandle;
} TCG_DXE_DATA;

typedef struct {
  CHAR16                                  *VariableName;
  EFI_GUID                                *VendorGuid;
} VARIABLE_TYPE;

EFI_TCG_CLIENT_ACPI_TABLE           mTcgClientAcpiTemplate = {
  {
    EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE,
    sizeof (mTcgClientAcpiTemplate),
    0x02                      //Revision
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  0,                          // 0 for PC Client Platform Class
  0,                          // Log Area Max Length
  (EFI_PHYSICAL_ADDRESS) (SIZE_4GB - 1)  // Log Area Start Address
};

//
// The following EFI_TCG_SERVER_ACPI_TABLE default setting is just one example,
// the TPM device connectes to LPC, and also defined the ACPI _UID as 0xFF,
// this _UID can be changed and should match with the _UID setting of the TPM
// ACPI device object
//
EFI_TCG_SERVER_ACPI_TABLE           mTcgServerAcpiTemplate = {
  {
    EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE,
    sizeof (mTcgServerAcpiTemplate),
    0x02                      //Revision
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  1,                          // 1 for Server Platform Class
  0,                          // Reserved
  0,                          // Log Area Max Length
  (EFI_PHYSICAL_ADDRESS) (SIZE_4GB - 1), // Log Area Start Address
  0x0100,                     // TCG Specification revision 1.0
  2,                          // Device Flags
  0,                          // Interrupt Flags
  0,                          // GPE
  {0},                        // Reserved 3 bytes
  0,                          // Global System Interrupt
  {
    EFI_ACPI_3_0_SYSTEM_MEMORY,
    0,
    0,
    EFI_ACPI_3_0_BYTE,
    TPM_BASE_ADDRESS          // Base Address
  },
  0,                          // Reserved
  {0},                        // Configuration Address
  0xFF,                       // ACPI _UID value of the device, can be changed for different platforms
  0,                          // ACPI _UID value of the device, can be changed for different platforms
  0,                          // ACPI _UID value of the device, can be changed for different platforms
  0                           // ACPI _UID value of the device, can be changed for different platforms
};

UINTN  mBootAttempts  = 0;
CHAR16 mBootVarName[] = L"BootOrder";

VARIABLE_TYPE mVariableType[] = {
  {EFI_SECURE_BOOT_MODE_NAME,    &gEfiGlobalVariableGuid},        // 1. SecureBoot Variable
  {EFI_PLATFORM_KEY_NAME,        &gEfiGlobalVariableGuid},        // 2. PK Variable
  {EFI_KEY_EXCHANGE_KEY_NAME,    &gEfiGlobalVariableGuid},        // 3. KEK Variable
  {EFI_IMAGE_SECURITY_DATABASE,  &gEfiImageSecurityDatabaseGuid}, // 4. EFI_IMAGE_SECURITY_DATABASE_GUID / EFI_IMAGE_SECURITY_DATABASE(db)
  {EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid}, // 5. EFI_IMAGE_SECURITY_DATABASE_GUID / EFI_IMAGE_SECURITY_DATABASE1(dbx)
};

BOOLEAN mTcgEventLogging = TRUE;

/**
  This service provides EFI protocol capability information, state information
  about the TPM, and Event Log state information.

  @param[in]  This               Indicates the calling context
  @param[out] ProtocolCapability The callee allocates memory for a TCG_BOOT_SERVICE_CAPABILITY
                                 structure and fills in the fields with the EFI protocol
                                 capability information and the current TPM state information.
  @param[out] TCGFeatureFlags    This is a pointer to the feature flags. No feature
                                 flags are currently defined so this parameter
                                 MUST be set to 0. However, in the future,
                                 feature flags may be defined that, for example,
                                 enable hash algorithm agility.
  @param[out] EventLogLocation   This is a pointer to the address of the event log in memory.
  @param[out] EventLogLastEntry  If the Event Log contains more than one entry,
                                 this is a pointer to the address of the start of
                                 the last entry in the event log in memory.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_INVALID_PARAMETER  ProtocolCapability does not match TCG capability.

**/
EFI_STATUS
EFIAPI
TcgDxeStatusCheck (
  IN      EFI_TCG_PROTOCOL                 *This,
  OUT     TCG_EFI_BOOT_SERVICE_CAPABILITY  *ProtocolCapability,
  OUT     UINT32                           *TCGFeatureFlags,
  OUT     EFI_PHYSICAL_ADDRESS             *EventLogLocation,
  OUT     EFI_PHYSICAL_ADDRESS             *EventLogLastEntry
  )
{
  TCG_DXE_DATA                      *TcgData;

  TcgData = TCG_DXE_DATA_FROM_THIS (This);

  if (ProtocolCapability != NULL) {
    *ProtocolCapability = TcgData->BsCap;
  }

  if (TCGFeatureFlags != NULL) {
    *TCGFeatureFlags = 0;
  }

  if (EventLogLocation != NULL) {
    *EventLogLocation = TcgData->LogAreaStartAddress;
  }

  if (EventLogLastEntry != NULL) {
    if (TcgData->BsCap.TPMDeactivatedFlag) {
      *EventLogLastEntry = (EFI_PHYSICAL_ADDRESS)(UINTN)0;
    } else {
      *EventLogLastEntry = (EFI_PHYSICAL_ADDRESS)(UINTN)TcgData->LastEvent;
    }
  }

  return EFI_SUCCESS;
}

/**
  This service abstracts the capability to do a hash operation on a data buffer.

  @param[in]      This             Indicates the calling context
  @param[in]      HashData         Pointer to the data buffer to be hashed
  @param[in]      HashDataLen      Length of the data buffer to be hashed
  @param[in]      AlgorithmId      Identification of the Algorithm to use for the hashing operation
  @param[in, out] HashedDataLen    Resultant length of the hashed data
  @param[in, out] HashedDataResult Resultant buffer of the hashed data

  @retval EFI_SUCCESS              Operation completed successfully.
  @retval EFI_INVALID_PARAMETER    HashDataLen is NULL.
  @retval EFI_INVALID_PARAMETER    HashDataLenResult is NULL.
  @retval EFI_OUT_OF_RESOURCES     Cannot allocate buffer of size *HashedDataLen.
  @retval EFI_UNSUPPORTED          AlgorithmId not supported.
  @retval EFI_BUFFER_TOO_SMALL     *HashedDataLen < sizeof (TCG_DIGEST).

**/
EFI_STATUS
EFIAPI
TcgDxeHashAll (
  IN      EFI_TCG_PROTOCOL          *This,
  IN      UINT8                     *HashData,
  IN      UINT64                    HashDataLen,
  IN      TCG_ALGORITHM_ID          AlgorithmId,
  IN OUT  UINT64                    *HashedDataLen,
  IN OUT  UINT8                     **HashedDataResult
  )
{
  if (HashedDataLen == NULL || HashedDataResult == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (AlgorithmId) {
    case TPM_ALG_SHA:
      if (*HashedDataLen == 0) {
        *HashedDataLen    = sizeof (TPM_DIGEST);
        *HashedDataResult = AllocatePool ((UINTN) *HashedDataLen);
        if (*HashedDataResult == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
      }

      if (*HashedDataLen < sizeof (TPM_DIGEST)) {
        *HashedDataLen = sizeof (TPM_DIGEST);
        return EFI_BUFFER_TOO_SMALL;
      }
      *HashedDataLen = sizeof (TPM_DIGEST);

      if (*HashedDataResult == NULL) {
        *HashedDataResult = AllocatePool ((UINTN) *HashedDataLen);
      }

      return TpmCommHashAll (
               HashData,
               (UINTN) HashDataLen,
               (TPM_DIGEST*)*HashedDataResult
               );
    default:
      return EFI_UNSUPPORTED;
  }
}

/**
  Add a new entry to the Event Log.

  @param[in] TcgData       TCG_DXE_DATA structure.
  @param[in] NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in] NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           The new event log entry was added.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.

**/
EFI_STATUS
EFIAPI
TcgDxeLogEventI (
  IN      TCG_DXE_DATA              *TcgData,
  IN      TCG_PCR_EVENT_HDR         *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  if (!mTcgEventLogging) {
   return EFI_SUCCESS;
  }

  TcgData->LastEvent = (UINT8*)(UINTN)TcgData->LogAreaStartAddress;
  
  return TpmCommLogEvent (
           &TcgData->LastEvent,
           &TcgData->EventLogSize,
           (UINTN)TcgData->MaxLogAreaSize,
           NewEventHdr,
           NewEventData
           );
}

/**
  This service abstracts the capability to add an entry to the Event Log.

  @param[in]      This           Indicates the calling context
  @param[in]      TCGLogData     Pointer to the start of the data buffer containing
                                 the TCG_PCR_EVENT data structure. All fields in
                                 this structure are properly filled by the caller.
  @param[in, out] EventNumber    The event number of the event just logged
  @param[in]      Flags          Indicate additional flags. Only one flag has been
                                 defined at this time, which is 0x01 and means the
                                 extend operation should not be performed. All
                                 other bits are reserved.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES   Insufficient memory in the event log to complete this action.

**/
EFI_STATUS
EFIAPI
TcgDxeLogEvent (
  IN      EFI_TCG_PROTOCOL          *This,
  IN      TCG_PCR_EVENT             *TCGLogData,
  IN OUT  UINT32                    *EventNumber,
  IN      UINT32                    Flags
  )
{
  TCG_DXE_DATA  *TcgData;

  if (TCGLogData == NULL){
    return EFI_INVALID_PARAMETER;
  }

  TcgData = TCG_DXE_DATA_FROM_THIS (This);

  if (TcgData->BsCap.TPMDeactivatedFlag) {
    return EFI_DEVICE_ERROR;
  }
  return TcgDxeLogEventI (
           TcgData,
           (TCG_PCR_EVENT_HDR*)TCGLogData,
           TCGLogData->Event
           );
}

/**
  This service is a proxy for commands to the TPM.

  @param[in]  This                        Indicates the calling context
  @param[in]  TpmInputParameterBlockSize  Size of the TPM input parameter block
  @param[in]  TpmInputParameterBlock      Pointer to the TPM input parameter block
  @param[in]  TpmOutputParameterBlockSize Size of the TPM output parameter block
  @param[in]  TpmOutputParameterBlock     Pointer to the TPM output parameter block

  @retval     EFI_SUCCESS                 Operation completed successfully.
  @retval     EFI_INVALID_PARAMETER       Invalid ordinal.
  @retval     EFI_UNSUPPORTED             Current Task Priority Level  >= EFI_TPL_CALLBACK.
  @retval     EFI_TIMEOUT                 The TIS timed-out.

**/
EFI_STATUS
EFIAPI
TcgDxePassThroughToTpm (
  IN      EFI_TCG_PROTOCOL          *This,
  IN      UINT32                    TpmInputParameterBlockSize,
  IN      UINT8                     *TpmInputParameterBlock,
  IN      UINT32                    TpmOutputParameterBlockSize,
  IN      UINT8                     *TpmOutputParameterBlock
  )
{
  TCG_DXE_DATA                      *TcgData;

  if (TpmInputParameterBlock == NULL ||
      TpmOutputParameterBlock == NULL ||
      TpmInputParameterBlockSize == 0 ||
      TpmOutputParameterBlockSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  TcgData = TCG_DXE_DATA_FROM_THIS (This);

  return TisPcExecute (
           TcgData->TpmHandle,
           "%r%/%r",
           TpmInputParameterBlock,
           (UINTN) TpmInputParameterBlockSize,
           TpmOutputParameterBlock,
           (UINTN) TpmOutputParameterBlockSize
           );
}

/**
  Do a hash operation on a data buffer, extend a specific TPM PCR with the hash result,
  and add an entry to the Event Log.

  @param[in]      TcgData       TCG_DXE_DATA structure.
  @param[in]      HashData      Physical address of the start of the data buffer
                                to be hashed, extended, and logged.
  @param[in]      HashDataLen   The length, in bytes, of the buffer referenced by HashData
  @param[in, out] NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in]      NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcgDxeHashLogExtendEventI (
  IN      TCG_DXE_DATA              *TcgData,
  IN      UINT8                     *HashData,
  IN      UINT64                    HashDataLen,
  IN OUT  TCG_PCR_EVENT_HDR         *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  EFI_STATUS                        Status;

  if (HashData == NULL && HashDataLen > 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (HashDataLen > 0 || HashData != NULL) {
    Status = TpmCommHashAll (
               HashData,
               (UINTN) HashDataLen,
               &NewEventHdr->Digest
               );
    ASSERT_EFI_ERROR (Status);
  }

  Status = TpmCommExtend (
             TcgData->TpmHandle,
             &NewEventHdr->Digest,
             NewEventHdr->PCRIndex,
             NULL
             );
  if (!EFI_ERROR (Status)) {
    Status = TcgDxeLogEventI (TcgData, NewEventHdr, NewEventData);
  }

  return Status;
}

/**
  This service abstracts the capability to do a hash operation on a data buffer,
  extend a specific TPM PCR with the hash result, and add an entry to the Event Log

  @param[in]      This               Indicates the calling context
  @param[in]      HashData           Physical address of the start of the data buffer
                                     to be hashed, extended, and logged.
  @param[in]      HashDataLen        The length, in bytes, of the buffer referenced by HashData
  @param[in]      AlgorithmId        Identification of the Algorithm to use for the hashing operation
  @param[in, out] TCGLogData         The physical address of the start of the data
                                     buffer containing the TCG_PCR_EVENT data structure.
  @param[in, out] EventNumber        The event number of the event just logged.
  @param[out]     EventLogLastEntry  Physical address of the first byte of the entry
                                     just placed in the Event Log. If the Event Log was
                                     empty when this function was called then this physical
                                     address will be the same as the physical address of
                                     the start of the Event Log.

  @retval EFI_SUCCESS                Operation completed successfully.
  @retval EFI_UNSUPPORTED            AlgorithmId != TPM_ALG_SHA.
  @retval EFI_UNSUPPORTED            Current TPL >= EFI_TPL_CALLBACK.
  @retval EFI_DEVICE_ERROR           The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcgDxeHashLogExtendEvent (
  IN      EFI_TCG_PROTOCOL          *This,
  IN      EFI_PHYSICAL_ADDRESS      HashData,
  IN      UINT64                    HashDataLen,
  IN      TPM_ALGORITHM_ID          AlgorithmId,
  IN OUT  TCG_PCR_EVENT             *TCGLogData,
  IN OUT  UINT32                    *EventNumber,
     OUT  EFI_PHYSICAL_ADDRESS      *EventLogLastEntry
  )
{
  TCG_DXE_DATA  *TcgData;
  EFI_STATUS    Status;

  if (TCGLogData == NULL || EventLogLastEntry == NULL){
    return EFI_INVALID_PARAMETER;
  }

  TcgData = TCG_DXE_DATA_FROM_THIS (This);

  if (TcgData->BsCap.TPMDeactivatedFlag) {
    return EFI_DEVICE_ERROR;
  }

  if (AlgorithmId != TPM_ALG_SHA) {
    return EFI_UNSUPPORTED;
  }

  Status = TcgDxeHashLogExtendEventI (
             TcgData,
             (UINT8 *) (UINTN) HashData,
             HashDataLen,
             (TCG_PCR_EVENT_HDR*)TCGLogData,
             TCGLogData->Event
             );

  if (!EFI_ERROR(Status)){
    *EventLogLastEntry = (EFI_PHYSICAL_ADDRESS)(UINTN) TcgData->LastEvent;
  }

  return Status;
}

TCG_DXE_DATA                 mTcgDxeData = {
  {
    TcgDxeStatusCheck,
    TcgDxeHashAll,
    TcgDxeLogEvent,
    TcgDxePassThroughToTpm,
    TcgDxeHashLogExtendEvent
  },
  {
    sizeof (mTcgDxeData.BsCap),
    { 1, 2, 0, 0 },
    { 1, 2, 0, 0 },
    1,
    TRUE,
    FALSE
  },
  0,
  0,
  0,
  NULL,
  NULL
};

/**
  Initialize the Event Log and log events passed from the PEI phase.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.

**/
EFI_STATUS
EFIAPI
SetupEventLog (
  VOID
  )
{
  EFI_STATUS                              Status;
  TCG_PCR_EVENT                           *TcgEvent;
  EFI_PEI_HOB_POINTERS                    GuidHob;
  EFI_PHYSICAL_ADDRESS                    Lasa;

  if (PcdGet8 (PcdTpmPlatformClass) == TCG_PLATFORM_TYPE_CLIENT) {
    Lasa = mTcgClientAcpiTemplate.Lasa;

    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    EFI_SIZE_TO_PAGES (EFI_TCG_LOG_AREA_SIZE),
                    &Lasa
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mTcgClientAcpiTemplate.Lasa = Lasa;
    //
    // To initialize them as 0xFF is recommended
    // because the OS can know the last entry for that.
    //
    SetMem ((VOID *)(UINTN)mTcgClientAcpiTemplate.Lasa, EFI_TCG_LOG_AREA_SIZE, 0xFF);
    mTcgClientAcpiTemplate.Laml = EFI_TCG_LOG_AREA_SIZE;

  } else {
    Lasa = mTcgServerAcpiTemplate.Lasa;

    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    EFI_SIZE_TO_PAGES (EFI_TCG_LOG_AREA_SIZE),
                    &Lasa
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mTcgServerAcpiTemplate.Lasa = Lasa;
    //
    // To initialize them as 0xFF is recommended
    // because the OS can know the last entry for that.
    //
    SetMem ((VOID *)(UINTN)mTcgServerAcpiTemplate.Lasa, EFI_TCG_LOG_AREA_SIZE, 0xFF);
    mTcgServerAcpiTemplate.Laml = EFI_TCG_LOG_AREA_SIZE;
  }
  
  mTcgDxeData.LogAreaStartAddress = Lasa;
  mTcgDxeData.EventLogSize   = 0;
  mTcgDxeData.MaxLogAreaSize = EFI_TCG_LOG_AREA_SIZE;
  //
  // Determines if TCG Event Logging is needed.
  // All changes of the policy of TCG Event Logging should be done before here.
  //
  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_TCG_DXE_LOG_EVENT_I) != 0) {
    mTcgEventLogging = FALSE;
  }
  
  GuidHob.Raw = GetHobList ();
  while (!EFI_ERROR (Status) &&
         (GuidHob.Raw = GetNextGuidHob (&gTcgEventEntryHobGuid, GuidHob.Raw)) != NULL) {
    TcgEvent    = GET_GUID_HOB_DATA (GuidHob.Guid);
    GuidHob.Raw = GET_NEXT_HOB (GuidHob);
    Status = TcgDxeLogEventI (
               &mTcgDxeData,
               (TCG_PCR_EVENT_HDR*)TcgEvent,
               TcgEvent->Event
               );
  }

  return Status;
}

/**
  Measure and log an action string, and extend the measurement result into PCR[5].

  @param[in] String           A specific string that indicates an Action event.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcgMeasureAction (
  IN      CHAR8                     *String
  )
{
  TCG_PCR_EVENT_HDR                 TcgEvent;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_ACTION) != 0) {
    return EFI_SUCCESS;
  }

  TcgEvent.PCRIndex  = 5;
  TcgEvent.EventType = EV_EFI_ACTION;
  TcgEvent.EventSize = (UINT32)AsciiStrLen (String);
  return TcgDxeHashLogExtendEventI (
           &mTcgDxeData,
           (UINT8*)String,
           TcgEvent.EventSize,
           &TcgEvent,
           (UINT8 *) String
           );
}

/**
  Measure and log EFI handoff tables, and extend the measurement result into PCR[1].

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureHandoffTables (
  VOID
  )
{
  EFI_STATUS                        Status;
  SMBIOS_TABLE_ENTRY_POINT          *SmbiosTable;
  TCG_PCR_EVENT_HDR                 TcgEvent;
  EFI_HANDOFF_TABLE_POINTERS        HandoffTables;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_HANDOFF_TABLES) != 0) {
    return EFI_SUCCESS;
  }
  Status = EfiGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID **) &SmbiosTable
             );

  if (!EFI_ERROR (Status) && (SmbiosTable != NULL)) {
    TcgEvent.PCRIndex  = 1;
    TcgEvent.EventType = EV_EFI_HANDOFF_TABLES;
    TcgEvent.EventSize = sizeof (HandoffTables);

    HandoffTables.NumberOfTables = 1;
    HandoffTables.TableEntry[0].VendorGuid  = gEfiSmbiosTableGuid;
    HandoffTables.TableEntry[0].VendorTable = SmbiosTable;

    DEBUG ((DEBUG_INFO, "The Smbios Table starts at: 0x%x\n", SmbiosTable->TableAddress));
    DEBUG ((DEBUG_INFO, "The Smbios Table size: 0x%x\n", SmbiosTable->TableLength));

    Status = TcgDxeHashLogExtendEventI (
               &mTcgDxeData,
               (UINT8*)(UINTN)SmbiosTable->TableAddress,
               SmbiosTable->TableLength,
               &TcgEvent,
               (UINT8*)&HandoffTables
               );
  }

  return Status;
}

/**
  Measure and log Separator event, and extend the measurement result into a specific PCR.

  @param[in] PCRIndex         PCR index.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureSeparatorEvent (
  IN      TPM_PCRINDEX              PCRIndex
  )
{
  TCG_PCR_EVENT_HDR                 TcgEvent;
  UINT32                            EventData;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_SEPARATOR_EVENT) != 0) {
    return EFI_SUCCESS;
  }

  EventData = 0;
  TcgEvent.PCRIndex  = PCRIndex;
  TcgEvent.EventType = EV_SEPARATOR;
  TcgEvent.EventSize = (UINT32)sizeof (EventData);
  return TcgDxeHashLogExtendEventI (
           &mTcgDxeData,
           (UINT8 *)&EventData,
           sizeof (EventData),
           &TcgEvent,
           (UINT8 *)&EventData
           );
}

/**
  Measure and log an EFI variable, and extend the measurement result into a specific PCR.

  @param[in]  PCRIndex          PCR Index.
  @param[in]  EventType         Event type.
  @param[in]  VarName           A Null-terminated string that is the name of the vendor's variable.
  @param[in]  VendorGuid        A unique identifier for the vendor.
  @param[in]  VarData           The content of the variable data.
  @param[in]  VarSize           The size of the variable data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureVariable (
  IN      TPM_PCRINDEX              PCRIndex,
  IN      TCG_EVENTTYPE             EventType,
  IN      CHAR16                    *VarName,
  IN      EFI_GUID                  *VendorGuid,
  IN      VOID                      *VarData,
  IN      UINTN                     VarSize
  )
{
  EFI_STATUS                        Status;
  TCG_PCR_EVENT_HDR                 TcgEvent;
  UINTN                             VarNameLength;
  EFI_VARIABLE_DATA                 *VarLog;

  VarNameLength      = StrLen (VarName);
  TcgEvent.PCRIndex  = PCRIndex;
  TcgEvent.EventType = EventType;
  TcgEvent.EventSize = (UINT32)(sizeof (*VarLog) + VarNameLength * sizeof (*VarName) + VarSize
                        - sizeof (VarLog->UnicodeName) - sizeof (VarLog->VariableData));

  VarLog = (EFI_VARIABLE_DATA*)AllocatePool (TcgEvent.EventSize);
  if (VarLog == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VarLog->VariableName       = *VendorGuid;
  VarLog->UnicodeNameLength  = VarNameLength;
  VarLog->VariableDataLength = VarSize;
  CopyMem (
     VarLog->UnicodeName,
     VarName,
     VarNameLength * sizeof (*VarName)
     );
  if (VarSize != 0) {
    CopyMem (
       (CHAR16 *)VarLog->UnicodeName + VarNameLength,
       VarData,
       VarSize
       );
  }

  if (EventType == EV_EFI_VARIABLE_DRIVER_CONFIG || EventType == EV_EFI_VARIABLE_AUTHORITY) {
    //
    // Digest is the event data (EFI_VARIABLE_DATA_TREE)
    //
    Status = TcgDxeHashLogExtendEventI (
               &mTcgDxeData,
               (UINT8*)VarLog,
               TcgEvent.EventSize,
               &TcgEvent,
               (UINT8*)VarLog
               );
  } else {
    Status = TcgDxeHashLogExtendEventI (
               &mTcgDxeData,
               (UINT8*)VarData,
               VarSize,
               &TcgEvent,
               (UINT8*)VarLog
               );
  }

  FreePool (VarLog);
  return Status;
}

/**
  Read then Measure and log an EFI variable, and extend the measurement result into a specific PCR.

  @param[in]  PCRIndex          PCR Index.
  @param[in]  EventType         Event type.
  @param[in]   VarName          A Null-terminated string that is the name of the vendor's variable.
  @param[in]   VendorGuid       A unique identifier for the vendor.
  @param[out]  VarSize          The size of the variable data.
  @param[out]  VarData          Pointer to the content of the variable.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
ReadAndMeasureVariable (
  IN      TPM_PCRINDEX                    PCRIndex,
  IN      TCG_EVENTTYPE                   EventType,
  IN      CHAR16                          *VarName,
  IN      EFI_GUID                        *VendorGuid,
  OUT     UINTN                           *VarSize,
  OUT     VOID                            **VarData
  )
{
  EFI_STATUS                              Status;

  Status = GetVariable2 (VarName, VendorGuid, VarData, VarSize);
  if (EventType == EV_EFI_VARIABLE_DRIVER_CONFIG || EventType == EV_EFI_VARIABLE_AUTHORITY) {
    if (EFI_ERROR (Status)) {
      //
      // It is valid case, so we need handle it.
      //
      *VarData = NULL;
      *VarSize = 0;
    }
  } else {
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ASSERT (*VarData != NULL);
  }

  Status = MeasureVariable (
             PCRIndex,
             EventType,
             VarName,
             VendorGuid,
             *VarData,
             *VarSize
             );
  return Status;
}

/**
  Read then Measure and log an EFI boot variable, and extend the measurement result into PCR[5].

  @param[in]   VarName          A Null-terminated string that is the name of the vendor's variable.
  @param[in]   VendorGuid       A unique identifier for the vendor.
  @param[out]  VarSize          The size of the variable data.
  @param[out]  VarData          Pointer to the content of the variable.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
ReadAndMeasureBootVariable (
  IN      CHAR16                    *VarName,
  IN      EFI_GUID                  *VendorGuid,
  OUT     UINTN                     *VarSize,
  OUT     VOID                      **VarData
  )
{
  return ReadAndMeasureVariable (
           5,
           EV_EFI_VARIABLE_BOOT,
           VarName,
           VendorGuid,
           VarSize,
           VarData
           );
}

/**
  Read then Measure and log an EFI Secure variable, and extend the measurement result into PCR[7].

  @param[in]   VarName          A Null-terminated string that is the name of the vendor's variable.
  @param[in]   VendorGuid       A unique identifier for the vendor.
  @param[out]  VarSize          The size of the variable data.
  @param[out]  VarData          Pointer to the content of the variable.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
ReadAndMeasureSecureVariable (
  IN      CHAR16                          *VarName,
  IN      EFI_GUID                        *VendorGuid,
  OUT     UINTN                           *VarSize,
  OUT     VOID                            **VarData
  )
{
  return ReadAndMeasureVariable (
           7,
           EV_EFI_VARIABLE_DRIVER_CONFIG,
           VarName,
           VendorGuid,
           VarSize,
           VarData
           );
}

EFI_STATUS
EFIAPI
MeasureOsLoaderAuthority (
  IN  EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL *This,
  IN  EFI_SIGNATURE_DATA                            *Data,
  IN  UINTN                                         Size
  )
{
 EFI_STATUS                                         Status;

 if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_OS_LOADER_AUTHORITY) != 0) {
   return EFI_SUCCESS;
 }

 Status = MeasureVariable (
            7,
            EV_EFI_VARIABLE_AUTHORITY,
            EFI_IMAGE_SECURITY_DATABASE,
            &gEfiImageSecurityDatabaseGuid,
            Data,
            Size
            );

  return Status;
}

/**
  Measure and log all EFI boot variables, and extend the measurement result into a specific PCR.

  The EFI boot variables are BootOrder and Boot#### variables.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureAllBootVariables (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT16                            *BootOrder;
  UINTN                             BootCount;
  UINTN                             Index;
  VOID                              *BootVarData;
  UINTN                             Size;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_ALL_BOOT_VARIABLES) != 0) {
    return EFI_SUCCESS;
  }

  Status = ReadAndMeasureBootVariable (
             mBootVarName,
             &gEfiGlobalVariableGuid,
             &BootCount,
             (VOID **) &BootOrder
             );
  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }
  ASSERT (BootOrder != NULL);
  if (BootOrder == NULL) {
    return EFI_DEVICE_ERROR;
  }

  if (EFI_ERROR (Status)) {
    FreePool (BootOrder);
    return Status;
  }

  BootCount /= sizeof (*BootOrder);
  for (Index = 0; Index < BootCount; Index++) {
    UnicodeSPrint (mBootVarName, sizeof (mBootVarName), L"Boot%04x", BootOrder[Index]);
    Status = ReadAndMeasureBootVariable (
               mBootVarName,
               &gEfiGlobalVariableGuid,
               &Size,
               &BootVarData
               );
    if (!EFI_ERROR (Status)) {
      FreePool (BootVarData);
    }
  }

  FreePool (BootOrder);
  return EFI_SUCCESS;
}

/**
  Measure and log all EFI Secure variables, and extend the measurement result into a specific PCR.

  The EFI boot variables are BootOrder and Boot#### variables.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureAllSecureVariables (
  VOID
  )
{
  EFI_STATUS                              Status;
  VOID                                    *Data;
  UINTN                                   DataSize;
  UINTN                                   Index;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_SECURE_BOOT_VARIABLES) != 0) {
    return EFI_SUCCESS;
  }

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < sizeof (mVariableType) / sizeof (mVariableType[0]); Index++) {
    DataSize = 0;
    Data = NULL;
    Status = ReadAndMeasureSecureVariable (
               mVariableType[Index].VariableName,
               mVariableType[Index].VendorGuid,
               &DataSize,
               &Data
               );
    if (!EFI_ERROR (Status)) {
      if (Data != NULL) {
        FreePool (Data);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Measure and log launch of FirmwareDebugger, and extend the measurement result into a specific PCR.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureLaunchOfFirmwareDebugger (
  VOID
  )
{
  TCG_PCR_EVENT_HDR                       TcgEvent;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_LAUNCH_OF_FIRMWARE_DEBUGGER) != 0) {
    return EFI_SUCCESS;
  }

  TcgEvent.PCRIndex  = 7;
  TcgEvent.EventType = EV_EFI_ACTION;
  TcgEvent.EventSize = sizeof(FIRMWARE_DEBUGGER_EVENT_STRING) - 1;

  return TcgDxeHashLogExtendEventI (
           &mTcgDxeData,
           (UINT8 *)FIRMWARE_DEBUGGER_EVENT_STRING,
           sizeof(FIRMWARE_DEBUGGER_EVENT_STRING) - 1,
           &TcgEvent,
           (UINT8*)FIRMWARE_DEBUGGER_EVENT_STRING
           );
}

EFI_STATUS
EFIAPI
SaveEventLogAddres (
  void
  )
{
  EFI_STATUS                             Status;
  EFI_LEGACY_BIOS_PROTOCOL               *LegacyBios;
  VOID                                   *LegacyRegion = NULL;
  EFI_TPM_EVENT_LOG_INFORMATION_TABLE    TpmEventLogInfo;
  TCG_PCR_EVENT_HDR                      *EventHdr;
  UINTN                                  EventSize;
  UINT16                                 EventNum = 0;
  UINT32                                 LogAreaSize;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = LegacyBios->GetLegacyRegion (
                        LegacyBios,
                        sizeof(EFI_TPM_EVENT_LOG_INFORMATION_TABLE),
                        0x0002,  // 0xE0000 block
                        0x01,
                        &LegacyRegion
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  TpmEventLogInfo.Header           = (UINT32)SIGNATURE_32 ('I', 'L', 'E', 'T');
  TpmEventLogInfo.EventLogSize     = (UINT32)mTcgDxeData.EventLogSize;
  TpmEventLogInfo.LastEventAddress = (UINT32)(UINTN)mTcgDxeData.LastEvent;
  TpmEventLogInfo.StartAddress = (UINT32)mTcgDxeData.LogAreaStartAddress;
  LogAreaSize = (UINT32)mTcgDxeData.MaxLogAreaSize;

  if (LogAreaSize > TpmEventLogInfo.EventLogSize) {
    TpmEventLogInfo.LeftSize = (UINT16)(LogAreaSize - TpmEventLogInfo.EventLogSize);
  } else {
  	 TpmEventLogInfo.LeftSize = 0;
  }

  if (TpmEventLogInfo.LastEventAddress > TpmEventLogInfo.StartAddress) {
    EventHdr = (TCG_PCR_EVENT_HDR*)(UINTN)TpmEventLogInfo.StartAddress;
    while ((UINTN)EventHdr <= (UINTN)TpmEventLogInfo.LastEventAddress){
      EventNum ++;
      EventSize = sizeof (TCG_PCR_EVENT_HDR) + EventHdr->EventSize;
  	   EventHdr =  (TCG_PCR_EVENT_HDR*)((UINTN)EventHdr + EventSize);
    }
  }
  TpmEventLogInfo.Entries = EventNum;

  CopyMem ((VOID *) LegacyRegion, &TpmEventLogInfo, sizeof(EFI_TPM_EVENT_LOG_INFORMATION_TABLE));

  return EFI_SUCCESS;
}

/**
  Ready to Boot Event notification handler.

  Sequence of OS boot events is measured in this event notification handler.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
OnReadyToBoot (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                        Status;
  TPM_PCRINDEX                      PcrIndex;

  if (mBootAttempts == 0) {

    //
    // Measure handoff tables.
    //
    Status = MeasureHandoffTables ();
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "HOBs not Measured. Error!\n"));
    }

    //
    // Measure BootOrder & Boot#### variables.
    //
    Status = MeasureAllBootVariables ();
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Boot Variables not Measured. Error!\n"));
    }

    //
    // This is the first boot attempt.
    //
    Status = TcgMeasureAction (
               EFI_CALLING_EFI_APPLICATION
               );
    ASSERT_EFI_ERROR (Status);

    //
    // Measure secure boot variable
    //
    Status = MeasureAllSecureVariables ();
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MeasureAllSecureVariables - %r\n", Status));
    }

    //
    // Draw a line between pre-boot env and entering post-boot env.
    //
    for (PcrIndex = 0; PcrIndex < 8; PcrIndex++) {
      Status = MeasureSeparatorEvent (PcrIndex);
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Measure GPT. It would be done in SAP driver.
    //

    //
    // Measure PE/COFF OS loader. It would be done in SAP driver.
    //

    //
    // Read & Measure variable. BootOrder already measured.
    //

    //
    // Save TCPA Event Log Address for CSM use
    //
    Status = SaveEventLogAddres();
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "SaveEventLogAddres - %r\n", Status));
    }
    
  } else {
    //
    // 6. Not first attempt, meaning a return from last attempt
    //
    Status = TcgMeasureAction (
               EFI_RETURNING_FROM_EFI_APPLICATOIN
               );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((EFI_D_INFO, "TPM TcgDxe Measure Data when ReadyToBoot\n"));
  //
  // Increase boot attempt counter.
  //
  mBootAttempts++;
}

/**
  Install TCG ACPI Table when ACPI Table Protocol is available.

  A system's firmware uses an ACPI table to identify the system's TCG capabilities
  to the Post-Boot environment. The information in this ACPI table is not guaranteed
  to be valid until the Host Platform transitions from pre-boot state to post-boot state.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context
**/
VOID
EFIAPI
InstallAcpiTable (
  IN EFI_EVENT                      Event,
  IN VOID*                          Context
  )
{
  UINTN                             TableKey;
  EFI_STATUS                        Status;
  EFI_ACPI_TABLE_PROTOCOL           *AcpiTable;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (PcdGet8 (PcdTpmPlatformClass) == TCG_PLATFORM_TYPE_CLIENT) {
    Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
                            &mTcgClientAcpiTemplate,
                            sizeof (mTcgClientAcpiTemplate),
                            &TableKey
                            );
  } else {
    Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
                            &mTcgServerAcpiTemplate,
                            sizeof (mTcgServerAcpiTemplate),
                            &TableKey
                            );
  }
  ASSERT_EFI_ERROR (Status);
}

/**
  Exit Boot Services Event notification handler.

  Measure invocation and success of ExitBootServices.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
OnExitBootServices (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS    Status;

  //
  // Measure invocation of ExitBootServices,
  //
  Status = TcgMeasureAction (
             EFI_EXIT_BOOT_SERVICES_INVOCATION
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Measure success of ExitBootServices
  //
  Status = TcgMeasureAction (
             EFI_EXIT_BOOT_SERVICES_SUCCEEDED
             );
  ASSERT_EFI_ERROR (Status);
}

/**
  Exit Boot Services Failed Event notification handler.

  Measure Failure of ExitBootServices.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
OnExitBootServicesFailed (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS    Status;

  //
  // Measure Failure of ExitBootServices,
  //
  Status = TcgMeasureAction (
             EFI_EXIT_BOOT_SERVICES_FAILED
             );
  ASSERT_EFI_ERROR (Status);

}

/**
  Get TPM Deactivated state.

  @param[out] TPMDeactivatedFlag   Returns TPM Deactivated state.

  @retval EFI_SUCCESS              Operation completed successfully.
  @retval EFI_DEVICE_ERROR         The operation was unsuccessful.

**/
EFI_STATUS
GetTpmStatus (
     OUT  BOOLEAN                   *TPMDeactivatedFlag
  )
{
  EFI_STATUS                        Status;
  TPM_STCLEAR_FLAGS                 VFlags;

  Status = TpmCommGetFlags (
             mTcgDxeData.TpmHandle,
             TPM_CAP_FLAG_VOLATILE,
             &VFlags,
             sizeof (VFlags)
             );
  if (!EFI_ERROR (Status)) {
    *TPMDeactivatedFlag = VFlags.deactivated;
  }

  return Status;
}

EFI_STATUS
GetTpmDisabledFlag (
  OUT  BOOLEAN                   *TPMDisabledFlag
  )
{
  EFI_STATUS                     Status;
  TPM_PERMANENT_FLAGS            PFlags;

  Status = TpmCommGetFlags (
             mTcgDxeData.TpmHandle,
             TPM_CAP_FLAG_PERMANENT,
             &PFlags,
             sizeof (PFlags)
             );
  if (!EFI_ERROR (Status)) {
    *TPMDisabledFlag |= PFlags.disable << 1;
  }

  return Status;
}

EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL mMeasureOsLoaderAuthority = {
  FALSE,
  MeasureOsLoaderAuthority
  };

/**
  The driver's entry point.

  It publishes EFI TCG Protocol.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN    EFI_HANDLE                  ImageHandle,
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_EVENT                         Event;
  VOID                              *Registration;

  if (!CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    DEBUG ((EFI_D_ERROR, "No TPM12 instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  mTcgDxeData.TpmHandle = (TIS_TPM_HANDLE)(UINTN)TPM_BASE_ADDRESS;
  Status = TisPcRequestUseTpm (mTcgDxeData.TpmHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "TPM not detected!\n"));
    return Status;
  }

  POST_CODE (DXE_TCGDXE); //PostCode = 0x40, TPM initial in DXE

  Status = GetTpmDisabledFlag (&mTcgDxeData.BsCap.TPMPresentFlag);

  Status = GetTpmStatus (&mTcgDxeData.BsCap.TPMDeactivatedFlag);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      EFI_D_ERROR,
      "Line %d in file " __FILE__ ":\n    "
      "DriverEntry: TPM not working properly\n",
      __LINE__
      ));
    return Status;
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiTcgProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTcgDxeData.TcgProtocol
                  );
  if (!EFI_ERROR (Status) && !mTcgDxeData.BsCap.TPMDeactivatedFlag) {
    //
    // Setup the log area and copy event log from hob list to it
    //
    Status = SetupEventLog ();
    ASSERT_EFI_ERROR (Status);

    //
    // Measure handoff tables, Boot#### variables etc.
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               OnReadyToBoot,
               NULL,
               &Event
               );

    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    OnExitBootServices,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &Event
                    );

    //
    // Measure Exit Boot Service failed
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    OnExitBootServicesFailed,
                    NULL,
                    &gEventExitBootServicesFailedGuid,
                    &Event
                    );

    if (PcdGetBool (PcdH2ODdtSupported)
        || PcdGetBool (PcdStatusCodeUseSerial)
        || PcdGetBool (PcdStatusCodeUseDdt)
        || PcdGetBool (PcdStatusCodeUseUsb)) {
      Status = MeasureLaunchOfFirmwareDebugger ();
      DEBUG ((EFI_D_ERROR, "MeasureLaunchOfFirmwareDebugger - %r\n", Status));
    }

    Status = gBS->InstallProtocolInterface (
                &ImageHandle,
                &gEfiTrEEMeasureOsLoaderAuthorityProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mMeasureOsLoaderAuthority
                );
  }

  //
  // Install ACPI Table
  //
  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_INSTALL_TCG_ACPI_TABLE) == 0) {
    EfiCreateProtocolNotifyEvent (&gEfiAcpiTableProtocolGuid, TPL_CALLBACK, InstallAcpiTable, NULL, &Registration);
  }

  return Status;
}