/** @file
  This module implements TrEE Protocol.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/PeImage.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Tpm2Acpi.h>
#include <IndustryStandard/TcpaAcpi.h>

#include <Guid/GlobalVariable.h>
#include <Guid/SmBios.h>
#include <Guid/HobList.h>
#include <Guid/TcgEventHob.h>
#include <Guid/EventGroup.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/EventExitBootServiceFailed.h>
#include <Guid/TpmInstance.h>
#include <Guid/DebugMask.h>

#include <Protocol/ServiceBinding.h>
#include <Protocol/TrEEMeasureOsLoaderAuthority.h>
#include <Protocol/AcpiTable.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/HashLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/PerformanceLib.h>

#include <PostCode.h>
#include <TpmPolicy.h>
#include <TrEEData.h>

#define TPM2_CONTROL_AREA_SIZE            1   // 1 Pages for Control Area
typedef struct {
  CHAR16                                  *VariableName;
  EFI_GUID                                *VendorGuid;
} VARIABLE_TYPE;

#define EFI_TCG_LOG_AREA_SIZE             0x10000
#define TREE_DEFAULT_MAX_COMMAND_SIZE     0x1000
#define TREE_DEFAULT_MAX_RESPONSE_SIZE    0x1000
#define TPM2_DEV_INTERNAL_VARIABLE        L"Tpm2Dev"
//
// Variable defination
//
#define CLEAR_NEEDED                      BIT0

EFI_TCG_CLIENT_ACPI_TABLE mTcgClientAcpiTemplate = {
  {
    EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE,
    sizeof (mTcgClientAcpiTemplate),
    0x02                                  //Revision
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  0,                                      // 0 for PC Client Platform Class
  0,                                      // Log Area Max Length
  (EFI_PHYSICAL_ADDRESS) (SIZE_4GB - 1)   // Log Area Start Address
};

//
// The following EFI_TCG_SERVER_ACPI_TABLE default setting is just one example,
// the TPM device connectes to LPC, and also defined the ACPI _UID as 0xFF,
// this _UID can be changed and should match with the _UID setting of the TPM
// ACPI device object
//
EFI_TCG_SERVER_ACPI_TABLE mTcgServerAcpiTemplate = {
  {
    EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE,
    sizeof (mTcgServerAcpiTemplate),
    0x02                                  //Revision
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  1,                                      // 1 for Server Platform Class
  0,                                      // Reserved
  0,                                      // Log Area Max Length
  (EFI_PHYSICAL_ADDRESS) (SIZE_4GB - 1),  // Log Area Start Address
  0x0100,                                 // TCG Specification revision 1.0
  2,                                      // Device Flags
  0,                                      // Interrupt Flags
  0,                                      // GPE
  {0},                                    // Reserved 3 bytes
  0,                                      // Global System Interrupt
  {
    EFI_ACPI_3_0_SYSTEM_MEMORY,
    0,
    0,
    EFI_ACPI_3_0_BYTE,
    0xfed40000                            // Base Address
  },
  0,                                      // Reserved
  {0},                                    // Configuration Address
  0xFF,                                   // ACPI _UID value of the device, can be changed for different platforms
  0,                                      // ACPI _UID value of the device, can be changed for different platforms
  0,                                      // ACPI _UID value of the device, can be changed for different platforms
  0                                       // ACPI _UID value of the device, can be changed for different platforms
};

TREE_DXE_DATA mTrEEDxeData = {
  {
    sizeof (mTrEEDxeData.BsCap),          // Size
    { 1, 0 },                             // StructureVersion
    { 1, 0 },                             // ProtocolVersion
    TREE_BOOT_HASH_ALG_SHA1,              // HashAlgorithmBitmap
    TREE_EVENT_LOG_FORMAT_TCG_1_2,        // SupportedEventLogs
    TRUE,                                 // TrEEPresentFlag
    TREE_DEFAULT_MAX_COMMAND_SIZE,        // MaxCommandSize
    TREE_DEFAULT_MAX_RESPONSE_SIZE,       // MaxResponseSize
    0                                     // ManufacturerID
  },
  0,                                      // LogAreaStartAddress;
  0,                                      // EventLogSize;
  0,                                      // MaxLogAreaSize;
  NULL,                                   // *LastEvent;
  FALSE,                                  // EventLogStarted;
  FALSE,                                  // EventLogTruncated;
  {
    NULL,
    NULL,
    NULL,
    NULL
  },                                      // TrEEProtocol
  0,                                      // Auth
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

EFI_STATUS
EFIAPI
MeasurePeImageAndExtend (
  IN  UINT32                              PCRIndex,
  IN  EFI_PHYSICAL_ADDRESS                ImageAddress,
  IN  UINTN                               ImageSize,
  OUT TPML_DIGEST_VALUES                  *DigestList
  );

/**
  The EFI_TREE_PROTOCOL GetCapability function call provides protocol
  capability information and state information about the TrEE.

  @param[in]      This               Indicates the calling context
  @param[in, out] ProtocolCapability The caller allocates memory for a TREE_BOOT_SERVICE_CAPABILITY
                                     structure and sets the size field to the size of the structure allocated.
                                     The callee fills in the fields with the EFI protocol capability information
                                     and the current TrEE state information up to the number of fields which
                                     fit within the size of the structure passed in.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
                                 The ProtocolCapability variable will not be populated.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.
                                 The ProtocolCapability variable will not be populated.
  @retval EFI_BUFFER_TOO_SMALL   The ProtocolCapability variable is too small to hold the full response.
                                 It will be partially populated (required Size field will be set).
**/
EFI_STATUS
EFIAPI
TrEEGetCapability (
  IN EFI_TREE_PROTOCOL                    *This,
  IN OUT TREE_BOOT_SERVICE_CAPABILITY     *ProtocolCapability
  )
{
  TREE_DXE_DATA                           *TrEEData;

  DEBUG ((EFI_D_ERROR, "TrEEGetCapability ...\n"));

  if ((This == NULL) || (ProtocolCapability == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TrEEData = &mTrEEDxeData;

  if (ProtocolCapability->Size < TrEEData->BsCap.Size) {
    ProtocolCapability->Size = TrEEData->BsCap.Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (ProtocolCapability, &TrEEData->BsCap, TrEEData->BsCap.Size);
  DEBUG ((EFI_D_ERROR, "TrEEGetCapability - %r\n", EFI_SUCCESS));
  return EFI_SUCCESS;
}

/**
  The EFI_TREE_PROTOCOL Get Event Log function call allows a caller to
  retrieve the address of a given event log and its last entry.

  @param[in]  This               Indicates the calling context
  @param[in]  EventLogFormat     The type of the event log for which the information is requested.
  @param[out] EventLogLocation   A pointer to the memory address of the event log.
  @param[out] EventLogLastEntry  If the Event Log contains more than one entry, this is a pointer to the
                                 address of the start of the last entry in the event log in memory.
  @param[out] EventLogTruncated  If the Event Log is missing at least one entry because an event would
                                 have exceeded the area allocated for events, this value is set to TRUE.
                                 Otherwise, the value will be FALSE and the Event Log will be complete.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect
                                 (e.g. asking for an event log whose format is not supported).
**/
EFI_STATUS
EFIAPI
TrEEGetEventLog (
  IN EFI_TREE_PROTOCOL                    *This,
  IN TREE_EVENT_LOG_FORMAT                EventLogFormat,
  OUT EFI_PHYSICAL_ADDRESS                *EventLogLocation,
  OUT EFI_PHYSICAL_ADDRESS                *EventLogLastEntry,
  OUT BOOLEAN                             *EventLogTruncated
  )
{
  TREE_DXE_DATA                           *TrEEData;

  DEBUG ((EFI_D_ERROR, "TrEEGetEventLog ...\n"));

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (EventLogFormat != TREE_EVENT_LOG_FORMAT_TCG_1_2) {
    return EFI_INVALID_PARAMETER;
  }

  TrEEData = &mTrEEDxeData;
  if (!TrEEData->BsCap.TrEEPresentFlag) {
    if (EventLogLocation != NULL) {
      *EventLogLocation = 0;
    }
    if (EventLogLastEntry != NULL) {
      *EventLogLastEntry = 0;
    }
    if (EventLogTruncated != NULL) {
      *EventLogTruncated = FALSE;
    }
    return EFI_SUCCESS;
  }

  if (EventLogLocation != NULL) {
    *EventLogLocation  = (EFI_PHYSICAL_ADDRESS)(UINTN) TrEEData->LogAreaStartAddress;
    DEBUG ((EFI_D_ERROR, "TrEEGetEventLog (EventLogLocation - %x)\n", *EventLogLocation));
  }

  if (EventLogLastEntry != NULL) {
    if (!TrEEData->EventLogStarted) {
      *EventLogLastEntry = (EFI_PHYSICAL_ADDRESS)(UINTN) 0x0;
    } else {
      *EventLogLastEntry = (EFI_PHYSICAL_ADDRESS)(UINTN) TrEEData->LastEvent;
    }
    DEBUG ((EFI_D_ERROR, "TrEEGetEventLog (EventLogLastEntry - %x)\n", *EventLogLastEntry));
  }

  if (EventLogTruncated != NULL) {
    *EventLogTruncated = TrEEData->EventLogTruncated;
    DEBUG ((EFI_D_ERROR, "TrEEGetEventLog (EventLogTruncated - %x)\n", *EventLogTruncated));
  }

  DEBUG ((EFI_D_ERROR, "TrEEGetEventLog - %r\n", EFI_SUCCESS));
  return EFI_SUCCESS;
}

/**
  Add a new entry to the Event Log.

  @param[in, out] EventLogPtr   Pointer to the Event Log data.
  @param[in, out] LogSize       Size of the Event Log.
  @param[in]      MaxSize       Maximum size of the Event Log.
  @param[in]      NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in]      NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           The new event log entry was added.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.

**/
EFI_STATUS
TcgCommLogEvent (
  IN OUT  UINT8                           **EventLogPtr,
  IN OUT  UINTN                           *LogSize,
  IN      UINTN                           MaxSize,
  IN      TCG_PCR_EVENT_HDR               *NewEventHdr,
  IN      UINT8                           *NewEventData
  )
{
  UINTN                            NewLogSize;

  //
  // Prevent Event Overflow
  //
  if (NewEventHdr->EventSize > (UINTN)(~0) - sizeof (*NewEventHdr)) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewLogSize = sizeof (*NewEventHdr) + NewEventHdr->EventSize;
  if (NewLogSize > MaxSize - *LogSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  *EventLogPtr += *LogSize;
  *LogSize += NewLogSize;
  CopyMem (*EventLogPtr, NewEventHdr, sizeof (*NewEventHdr));
  CopyMem (
    *EventLogPtr + sizeof (*NewEventHdr),
    NewEventData,
    NewEventHdr->EventSize
    );
  return EFI_SUCCESS;
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
  IN TREE_DXE_DATA                        *TrEEData,
  IN TCG_PCR_EVENT_HDR                    *NewEventHdr,
  IN UINT8                                *NewEventData
  )
{
  EFI_STATUS                              Status;

  if (!mTcgEventLogging) {
    return EFI_SUCCESS;
  }
  TrEEData->LastEvent = (UINT8 *)(UINTN)TrEEData->LogAreaStartAddress;

  Status = TcgCommLogEvent (
             (UINT8 **)&TrEEData->LastEvent,
             &TrEEData->EventLogSize,
             TrEEData->MaxLogAreaSize,
             NewEventHdr,
             NewEventData
             );

  if (Status == EFI_DEVICE_ERROR) {
    return EFI_DEVICE_ERROR;
  } else if (Status == EFI_OUT_OF_RESOURCES) {
    TrEEData->EventLogTruncated = TRUE;
    return EFI_VOLUME_FULL;
  } else if (Status == EFI_SUCCESS) {
    TrEEData->EventLogStarted = TRUE;
  }

  return Status;
}

/**
  This function get SHA1 digest from digest list.

  @param DigestList digest list
  @param Sha1Digest SHA1 digest

  @retval EFI_SUCCESS   Sha1Digest is found and returned.
  @retval EFI_NOT_FOUND Sha1Digest is not found.
**/
EFI_STATUS
Tpm2GetSha1FromDigestList (
  IN TPML_DIGEST_VALUES                   *DigestList,
  IN TPM_DIGEST                           *Sha1Digest
  )
{
  UINTN                                   Index;

  for (Index = 0; Index < DigestList->count; Index++) {
    if (DigestList->digests[Index].hashAlg == TPM_ALG_SHA1) {
      CopyMem (
        Sha1Digest,
        DigestList->digests[Index].digest.sha1,
        SHA1_DIGEST_SIZE
        );
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
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
  IN      TREE_DXE_DATA                   *TrEEData,
  IN      UINT64                          Flags,
  IN      UINT8                           *HashData,
  IN      UINT64                          HashDataLen,
  IN OUT  TCG_PCR_EVENT_HDR               *NewEventHdr,
  IN      UINT8                           *NewEventData
  )
{
  EFI_STATUS                              Status;
  TPML_DIGEST_VALUES                      DigestList;
  EFI_TPL                                 OldTpl;

  if (HashData == NULL && HashDataLen > 0) {
    return EFI_INVALID_PARAMETER;
  }

  Status = HashAndExtend (
             NewEventHdr->PCRIndex,
             HashData,
             (UINTN)HashDataLen,
             &DigestList
             );
  if (!EFI_ERROR (Status)) {
    Status = Tpm2GetSha1FromDigestList (&DigestList, &NewEventHdr->Digest);
    if (!EFI_ERROR (Status)) {
      if ((Flags & TREE_EXTEND_ONLY) == 0) {
        //
        // Enter critical region
        //
        OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
        Status = TcgDxeLogEventI (TrEEData, NewEventHdr, NewEventData);
        //
        // Exit critical region
        //
        gBS->RestoreTPL (OldTpl);
      }
    }
  }

  return Status;
}

/**
  The EFI_TREE_PROTOCOL HashLogExtendEvent function call provides callers with
  an opportunity to extend and optionally log events without requiring
  knowledge of actual TPM commands.
  The extend operation will occur even if this function cannot create an event
  log entry (e.g. due to the event log being full).

  @param[in]  This               Indicates the calling context
  @param[in]  Flags              Bitmap providing additional information.
  @param[in]  DataToHash         Physical address of the start of the data buffer to be hashed.
  @param[in]  DataToHashLen      The length in bytes of the buffer referenced by DataToHash.
  @param[in]  Event              Pointer to data buffer containing information about the event.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
  @retval EFI_VOLUME_FULL        The extend operation occurred, but the event could not be written to one or more event logs.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.
  @retval EFI_UNSUPPORTED        The PE/COFF image type is not supported.
**/
EFI_STATUS
EFIAPI
TrEEHashLogExtendEvent (
  IN EFI_TREE_PROTOCOL                    *This,
  IN UINT64                               Flags,
  IN EFI_PHYSICAL_ADDRESS                 DataToHash,
  IN UINT64                               DataToHashLen,
  IN TrEE_EVENT                           *Event
  )
{
  EFI_STATUS                              Status;
  TREE_DXE_DATA                           *TrEEData;
  TCG_PCR_EVENT_HDR                       NewEventHdr;
  TPML_DIGEST_VALUES                      DigestList;
  EFI_TPL                                 OldTpl;

  DEBUG ((EFI_D_ERROR, "TrEEHashLogExtendEvent ...\n"));

  if ((This == NULL) || (DataToHash == 0) || (Event == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TrEEData = &mTrEEDxeData;

  if (!TrEEData->BsCap.TrEEPresentFlag) {
    return EFI_UNSUPPORTED;
  }

  if (Event->Size < Event->Header.HeaderSize + sizeof (UINT32)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Event->Header.PCRIndex > MAX_PCR_INDEX) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Flags & TREE_EXTEND_ONLY) == 0) {
    if (TrEEData->EventLogTruncated) {
      return EFI_VOLUME_FULL;
    }
  }

  if ((Flags & PE_COFF_IMAGE) != 0) {
    NewEventHdr.PCRIndex  = Event->Header.PCRIndex;
    NewEventHdr.EventType = Event->Header.EventType;
    NewEventHdr.EventSize = Event->Size - sizeof (UINT32) - Event->Header.HeaderSize;

    Status = MeasurePeImageAndExtend (
               NewEventHdr.PCRIndex,
               DataToHash,
               (UINTN)DataToHashLen,
               &DigestList
               );
    if (!EFI_ERROR (Status)) {
      Status = Tpm2GetSha1FromDigestList (&DigestList, &NewEventHdr.Digest);
      if (!EFI_ERROR (Status)) {
        if ((Flags & TREE_EXTEND_ONLY) == 0) {
          //
          // Enter critical region
          //
          OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);

          Status = TcgDxeLogEventI (TrEEData, &NewEventHdr, Event->Event);

          //
          // Exit critical region
          //
          gBS->RestoreTPL (OldTpl);
        }
      }
    }
  } else {
    NewEventHdr.PCRIndex  = Event->Header.PCRIndex;
    NewEventHdr.EventType = Event->Header.EventType;
    NewEventHdr.EventSize = Event->Size - sizeof (UINT32) - Event->Header.HeaderSize;

    Status = TcgDxeHashLogExtendEventI (
               TrEEData,
               Flags,
               (UINT8 *)(UINTN)DataToHash,
               DataToHashLen,
               &NewEventHdr,
               Event->Event
               );
  }
  DEBUG ((EFI_D_ERROR, "TrEEHashLogExtendEvent - %r\n", Status));
  return Status;
}

/**
  This service enables the sending of commands to the TrEE.

  @param[in]  This                     Indicates the calling context
  @param[in]  InputParameterBlockSize  Size of the TrEE input parameter block.
  @param[in]  InputParameterBlock      Pointer to the TrEE input parameter block.
  @param[in]  OutputParameterBlockSize Size of the TrEE output parameter block.
  @param[in]  OutputParameterBlock     Pointer to the TrEE output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_INVALID_PARAMETER  One or more of the parameters are incorrect.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small.
**/
EFI_STATUS
EFIAPI
TrEESubmitCommand (
  IN EFI_TREE_PROTOCOL                    *This,
  IN UINT32                               InputParameterBlockSize,
  IN UINT8                                *InputParameterBlock,
  IN UINT32                               OutputParameterBlockSize,
  IN UINT8                                *OutputParameterBlock
  )
{
  TREE_DXE_DATA                           *TrEEData;
  EFI_STATUS                              Status;

  DEBUG ((EFI_D_ERROR, "TrEESubmitCommand ...\n"));

  if ((This == NULL) ||
      (InputParameterBlockSize == 0) || (InputParameterBlock == NULL) ||
      (OutputParameterBlockSize == 0) || (OutputParameterBlock == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TrEEData = &mTrEEDxeData;

  if (!TrEEData->BsCap.TrEEPresentFlag) {
    return EFI_UNSUPPORTED;
  }

  if (InputParameterBlockSize >= TrEEData->BsCap.MaxCommandSize) {
    return EFI_INVALID_PARAMETER;
  }
  if (OutputParameterBlockSize >= TrEEData->BsCap.MaxResponseSize) {
    return EFI_INVALID_PARAMETER;
  }

  Status = Tpm2SubmitCommand (
             InputParameterBlockSize,
             InputParameterBlock,
             &OutputParameterBlockSize,
             OutputParameterBlock
             );
  DEBUG ((EFI_D_ERROR, "TrEESubmitCommand - %r\n", Status));
  return Status;
}

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
  
  mTrEEDxeData.LogAreaStartAddress = Lasa;
  mTrEEDxeData.EventLogSize   = 0;
  mTrEEDxeData.MaxLogAreaSize = EFI_TCG_LOG_AREA_SIZE;
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
               &mTrEEDxeData,
               (TCG_PCR_EVENT_HDR*)TcgEvent,
               TcgEvent->Event
               );
  }

  return Status;
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
  Measure and log an action string, and extend the measurement result into PCR[5].

  @param[in] String           A specific string that indicates an Action event.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The operation was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcgMeasureAction (
  IN      CHAR8                           *String
  )
{
  TCG_PCR_EVENT_HDR                       TcgEvent;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_ACTION) != 0) {
    return EFI_SUCCESS;
  }

  TcgEvent.PCRIndex  = 5;
  TcgEvent.EventType = EV_EFI_ACTION;
  TcgEvent.EventSize = (UINT32)AsciiStrLen (String);
  return TcgDxeHashLogExtendEventI (
           &mTrEEDxeData,
           0,
           (UINT8 *)String,
           TcgEvent.EventSize,
           &TcgEvent,
           (UINT8 *)String
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
  EFI_STATUS                              Status;
  SMBIOS_TABLE_ENTRY_POINT                *SmbiosTable;
  TCG_PCR_EVENT_HDR                       TcgEvent;
  EFI_HANDOFF_TABLE_POINTERS              HandoffTables;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_HANDOFF_TABLES) != 0) {
    return EFI_SUCCESS;
  }
  Status = EfiGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID **)&SmbiosTable
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
               &mTrEEDxeData,
               0,
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
  IN      TPM_PCRINDEX                    PCRIndex
  )
{
  TCG_PCR_EVENT_HDR                       TcgEvent;
  UINT32                                  EventData;

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_MEASURE_SEPARATOR_EVENT) != 0) {
    return EFI_SUCCESS;
  }

  EventData = 0;
  TcgEvent.PCRIndex  = PCRIndex;
  TcgEvent.EventType = EV_SEPARATOR;
  TcgEvent.EventSize = (UINT32)sizeof (EventData);
  return TcgDxeHashLogExtendEventI (
           &mTrEEDxeData,
           0,
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
  IN      TPM_PCRINDEX                    PCRIndex,
  IN      TCG_EVENTTYPE                   EventType,
  IN      CHAR16                          *VarName,
  IN      EFI_GUID                        *VendorGuid,
  IN      VOID                            *VarData,
  IN      UINTN                           VarSize
  )
{
  EFI_STATUS                              Status;
  TCG_PCR_EVENT_HDR                       TcgEvent;
  UINTN                                   VarNameLength;
  EFI_VARIABLE_DATA_TREE                  *VarLog;

  VarNameLength      = StrLen (VarName);
  TcgEvent.PCRIndex  = PCRIndex;
  TcgEvent.EventType = EventType;
  TcgEvent.EventSize = (UINT32)(sizeof (*VarLog) + VarNameLength * sizeof (*VarName) + VarSize
                        - sizeof (VarLog->UnicodeName) - sizeof (VarLog->VariableData));

  VarLog = (EFI_VARIABLE_DATA_TREE*)AllocatePool (TcgEvent.EventSize);
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
               &mTrEEDxeData,
               0,
               (UINT8*)VarLog,
               TcgEvent.EventSize,
               &TcgEvent,
               (UINT8*)VarLog
               );
  } else {
    Status = TcgDxeHashLogExtendEventI (
               &mTrEEDxeData,
               0,
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
  IN  CHAR16                              *VarName,
  IN  EFI_GUID                            *VendorGuid,
  OUT UINTN                               *VarSize,
  OUT VOID                                **VarData
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
  EFI_STATUS                              Status;
  UINT16                                  *BootOrder;
  UINTN                                   BootCount;
  UINTN                                   Index;
  VOID                                    *BootVarData;
  UINTN                                   Size;

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
           &mTrEEDxeData,
           0,
           (UINT8 *)FIRMWARE_DEBUGGER_EVENT_STRING,
           sizeof(FIRMWARE_DEBUGGER_EVENT_STRING) - 1,
           &TcgEvent,
           (UINT8 *)FIRMWARE_DEBUGGER_EVENT_STRING
           );
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
  IN      EFI_EVENT                       Event,
  IN      VOID                            *Context
  )
{
  EFI_STATUS                              Status;
  TPM_PCRINDEX                            PcrIndex;

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
  } else {
    //
    // Not first attempt, meaning a return from last attempt
    //
    Status = TcgMeasureAction (
               EFI_RETURNING_FROM_EFI_APPLICATOIN
               );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((EFI_D_ERROR, "TPM2 TrEEDxe Measure Data when ReadyToBoot\n"));
  //
  // Increase boot attempt counter.
  //
  mBootAttempts++;
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
  IN      EFI_EVENT                       Event,
  IN      VOID                            *Context
  )
{
  EFI_STATUS                              Status;

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
  IN      EFI_EVENT                       Event,
  IN      VOID                            *Context
  )
{
  EFI_STATUS                              Status;

  //
  // Measure Failure of ExitBootServices,
  //
  Status = TcgMeasureAction (
             EFI_EXIT_BOOT_SERVICES_FAILED
             );
  ASSERT_EFI_ERROR (Status);

}

EFI_STATUS
InstallTrEE (
  IN OUT EFI_HANDLE                       *TrEEHandle
  )
{
  mTrEEDxeData.TrEEProtocol.GetCapability      = TrEEGetCapability;
  mTrEEDxeData.TrEEProtocol.GetEventLog        = TrEEGetEventLog;
  mTrEEDxeData.TrEEProtocol.HashLogExtendEvent = TrEEHashLogExtendEvent;
  mTrEEDxeData.TrEEProtocol.SubmitCommand      = TrEESubmitCommand;

  return gBS->InstallMultipleProtocolInterfaces (
                TrEEHandle,
                &gEfiTrEEProtocolGuid,
                &mTrEEDxeData.TrEEProtocol,
                NULL
                );
}

EFI_STATUS
UninstallTrEE (
  IN EFI_HANDLE                           TrEEHandle
  )
{
  return gBS->UninstallMultipleProtocolInterfaces (
                TrEEHandle,
                &gEfiTrEEProtocolGuid,
                &mTrEEDxeData.TrEEProtocol,
                NULL
                );
}

EFI_STATUS
EFIAPI
TrEEServiceBindingCreateChild (
  IN EFI_SERVICE_BINDING_PROTOCOL         *This,
  IN EFI_HANDLE                           *ChildHandle
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              Handle;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Handle = NULL;
  Status = InstallTrEE (&Handle);
  *ChildHandle = Handle;

  return Status;
}

EFI_STATUS
EFIAPI
TrEEServiceBindingDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL         *This,
  IN EFI_HANDLE                           ChildHandle
  )
{
  EFI_STATUS                              Status;

  if ((This == NULL) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = UninstallTrEE (ChildHandle);
  return Status;
}

EFI_SERVICE_BINDING_PROTOCOL mTrEEServiceBinding = {
  TrEEServiceBindingCreateChild,
  TrEEServiceBindingDestroyChild
};

EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL mMeasureOsLoaderAuthority = {
  FALSE,
  MeasureOsLoaderAuthority
  };

/**
  Initial control area specific data.

  @retval EFI_SUCCESS     Operation completed successfully.
**/
EFI_STATUS
ControlAreaInit (
  VOID
  )
{
  EFI_STATUS                              Status;
  EFI_PHYSICAL_ADDRESS                    MemoryAddress;
  EFI_TPM2_ACPI_CONTROL_AREA              *ControlAreaHdr;

  //
  //  Create a page for control area
  //
  MemoryAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  TPM2_CONTROL_AREA_SIZE,
                  &MemoryAddress
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem ((UINT8*)(UINTN) MemoryAddress, EFI_PAGES_TO_SIZE (TPM2_CONTROL_AREA_SIZE));

  ControlAreaHdr = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN) MemoryAddress;
  //
  // Update PCD
  //
  PcdSet64 (PcdTpm2ControlArea, MemoryAddress);
  //
  // Command and Response Area
  //
  ControlAreaHdr->CommandSize  = EFI_PAGES_TO_SIZE (TPM2_CONTROL_AREA_SIZE) - sizeof (EFI_TPM2_ACPI_CONTROL_AREA);
  ControlAreaHdr->ResponseSize = EFI_PAGES_TO_SIZE (TPM2_CONTROL_AREA_SIZE) - sizeof (EFI_TPM2_ACPI_CONTROL_AREA);
  MemoryAddress += sizeof (EFI_TPM2_ACPI_CONTROL_AREA);
  ControlAreaHdr->Command      = MemoryAddress;
  ControlAreaHdr->Response     = MemoryAddress;

  return EFI_SUCCESS;
}

/**
  Initial TrEE specific data.

  @retval EFI_SUCCESS     Operation completed successfully.
**/
EFI_STATUS
TrEEInit (
  VOID
  )
{
  EFI_STATUS                              Status;
  UINT32                                  MaxCommandSize;
  UINT32                                  MaxResponseSize;
  UINT32                                  TpmHashAlgorithmBitmap;  
  TPML_PCR_SELECTION                      Pcrs;  
  UINTN                                   Index;

  //
  // Fill information
  //
  Status = Tpm2GetCapabilityManufactureID (&mTrEEDxeData.BsCap.ManufacturerID);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityManufactureID fail!\n"));
    return Status;
  }
  DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityManufactureID - %08x\n", mTrEEDxeData.BsCap.ManufacturerID));
  
  Status = Tpm2GetCapabilityMaxCommandResponseSize (&MaxCommandSize, &MaxResponseSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityMaxCommandResponseSize fail!\n"));
    return Status;
  }
  mTrEEDxeData.BsCap.MaxCommandSize  = (UINT16) MaxCommandSize;
  mTrEEDxeData.BsCap.MaxResponseSize = (UINT16) MaxResponseSize;
  DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityMaxCommandResponseSize - %08x, %08x\n", MaxCommandSize, MaxResponseSize));
  
  Status = Tpm2GetCapabilityPcrs (&Pcrs);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityPcrs fail!\n"));
    TpmHashAlgorithmBitmap = TREE_BOOT_HASH_ALG_SHA1;
  } else {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityPcrs Count - %08x\n", Pcrs.count));
    TpmHashAlgorithmBitmap = 0;
    for (Index = 0; Index < Pcrs.count; Index++) {
      DEBUG ((EFI_D_ERROR, "hash - %x\n", Pcrs.pcrSelections[Index].hash));
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        TpmHashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA1;
        break;
      case TPM_ALG_SHA256:
        TpmHashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA256;
        break;
      case TPM_ALG_SHA384:
        TpmHashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA384;
        break;
      case TPM_ALG_SHA512:
        TpmHashAlgorithmBitmap |= TREE_BOOT_HASH_ALG_SHA512;
        break;
      case TPM_ALG_SM3_256:
        // TBD: Spec not define TREE_BOOT_HASH_ALG_SM3_256 yet
        break;
      }
    }
  }
  DEBUG ((EFI_D_ERROR, "TPM.HashAlgorithmBitmap - 0x%08x\n", TpmHashAlgorithmBitmap));

  DEBUG ((EFI_D_ERROR, "TrEE.SupportedEventLogs - 0x%08x\n", mTrEEDxeData.BsCap.SupportedEventLogs));
  mTrEEDxeData.BsCap.HashAlgorithmBitmap = TpmHashAlgorithmBitmap;
  DEBUG ((EFI_D_ERROR, "TrEE.HashAlgorithmBitmap - 0x%08x\n", mTrEEDxeData.BsCap.HashAlgorithmBitmap));

  return EFI_SUCCESS;
}

/**
  Replaces the current platformAuth with a value from the RNG.

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval EFI_UNSUPPORTED      Can't identify the capablity of TPM.
  @retval other                The operation was unsuccessful.
**/
EFI_STATUS
EFIAPI
TpmPlatformAuthChange (
  VOID
  )
{
  EFI_STATUS            Status;
  TPM2B_AUTH            NewAuth;
  TPML_ALG_PROPERTY     AlgList;
  UINTN                 Index;

  NewAuth.size = SHA1_DIGEST_SIZE;
  Status = Tpm2GetCapabilitySupportedAlg (&AlgList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilitySupportedAlg fail!\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilitySupportedAlg - %08x\n", AlgList.count));
    for (Index = 0; Index < AlgList.count; Index++) {
      DEBUG ((EFI_D_ERROR, "alg - %x\n", AlgList.algProperties[Index].alg));
      switch (AlgList.algProperties[Index].alg) {
      case TPM_ALG_SHA1:
        if (NewAuth.size < SHA1_DIGEST_SIZE) {
          NewAuth.size = SHA1_DIGEST_SIZE;
        }
        break;
      case TPM_ALG_SHA256:
        if (NewAuth.size < SHA256_DIGEST_SIZE) {
          NewAuth.size = SHA256_DIGEST_SIZE;
        }
        break;
      case TPM_ALG_SHA384:
        if (NewAuth.size < SHA384_DIGEST_SIZE) {
          NewAuth.size = SHA384_DIGEST_SIZE;
        }
        break;
      case TPM_ALG_SHA512:
        if (NewAuth.size < SHA512_DIGEST_SIZE) {
          NewAuth.size = SHA512_DIGEST_SIZE;
        }
        break;
      case TPM_ALG_SM3_256:
        // TBD: Spec not define TREE_BOOT_HASH_ALG_SM3_256
        break;
      }
    }
  }

  RandomSeed (NULL, 0);
  RandomBytes (NewAuth.buffer, NewAuth.size);
  Status = Tpm2HierarchyChangeAuth (TPM_RH_PLATFORM, NULL, &NewAuth);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Tpm2HierarchyChangeAuth failed!\n"));
  } else {
    mTrEEDxeData.Auth = NewAuth;
  }

  return Status;
}

/**
  Change hierarchies with empty platformAuth.

  @param [in]  TrEEProtocol    Pointer to EFI_TREE_PROTOCOL

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval other                The operation was unsuccessful.
**/
EFI_STATUS
EFIAPI
TpmDisableShAndEh (
  VOID
  )
{
  EFI_STATUS                Status;

  Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, NULL, TPM_RH_OWNER, NO);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to disable Storage Hierarchy!\n"));
    return Status;
  }
  
  Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, NULL, TPM_RH_ENDORSEMENT, NO);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to disable Endorsement Hierarchy!\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  The driver's entry point. It publishes EFI TrEE Protocol.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_EVENT                               Event;
  VOID                                    *Registration;

  if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceNoneGuid) ||
      CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    DEBUG ((EFI_D_ERROR, "No TPM2 instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  Status = Tpm2RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "TPM2 not detected!\n"));
    return Status;
  }

  POST_CODE (DXE_TCGDXE); //PostCode = 0x40, TPM initial in DXE

  Status = TrEEInit ();
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "TrEEInit Unsuccessful\n"));
    return Status;
  }

  if (!mTrEEDxeData.BsCap.TrEEPresentFlag) {
    DEBUG((EFI_D_ERROR, "TPM2 is disabled\n"));
  }

  if (mTrEEDxeData.BsCap.TrEEPresentFlag) {
    //
    // Disable Storage Hierarchy and Endorsement Hierarchy
    //
    if (!PcdGetBool (PcdTpm2Enable)) {
      Status = TpmDisableShAndEh ();
      if (EFI_ERROR (Status)) {
        PcdSetBool (PcdTpm2Enable, TRUE);
      }
    }
    //
    // Change platformAuth
    //
    if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_TPM_REVOKE_TRUST) == 0) {
      Status = TpmPlatformAuthChange ();
      ASSERT_EFI_ERROR (Status);
    }
    //
    // Setup the log area and copy event log from hob list to it
    //
    Status = SetupEventLog ();
    ASSERT_EFI_ERROR (Status);
    
    if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_INSTALL_TCG_ACPI_TABLE) == 0) {
   	  //
   	  // Prepare Control Area content
   	  //
      Status = ControlAreaInit ();
      ASSERT_EFI_ERROR (Status);
      //
      // Install TCPA ACPI Table
      //
      EfiCreateProtocolNotifyEvent (
        &gEfiAcpiTableProtocolGuid, 
        TPL_CALLBACK, 
        InstallAcpiTable, 
        NULL, 
        &Registration
        );
    }

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

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiTrEEServiceBindingProtocolGuid,
                  &mTrEEServiceBinding,
                  NULL
                  );
  DEBUG ((EFI_D_ERROR, "InstallTrEEServiceBinding - %r\n", Status));
  //
  // Publish the EFI_TREE_PROTOCOL only for consumption by OS to ensure
  // compatibility with OS calls to EFI_TREE_PROTOCOL
  //
  Status = InstallTrEE (&ImageHandle);
  DEBUG ((EFI_D_ERROR, "InstallTrEE - %r\n", Status));

  return Status;
}
