/** @file

  WheaSupport driver functions.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "WheaSupportDxe.h"

STATIC WHEA_SUPPORT_INST mWheaSupport = \
{
  EFI_WHEA_SUPPORT_SIGNATURE,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  0,
  NULL,
  0,
  NULL,
  0,
  NULL,
  FALSE,
  0,
  {NULL}
};

STATIC EFI_ACPI_SUPPORT_PROTOCOL       *mAcpiSupport = NULL;

STATIC BOOLEAN                         mReadyToBoot = FALSE;

/**
  This function calculates and updates an UINT8 checksum.

  @param [in] Buffer              Pointer to buffer to checksum
  

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Buffer is NULL.

**/
EFI_STATUS
InsydeWheaChecksum (
  IN VOID                              *Buffer
  )
{
  EFI_ACPI_DESCRIPTION_HEADER          *TableHeader;
  UINTN                                ChecksumOffset;
  UINTN                                Size;
  UINT8                                *Ptr;
  UINT8                                Sum;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TableHeader    = (EFI_ACPI_DESCRIPTION_HEADER *) Buffer;
  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);
  Size           = TableHeader->Length;
  //
  // Initialize pointer
  //
  Ptr = (UINT8 *)Buffer;
  //
  // set checksum to 0 first
  //
  Ptr[ChecksumOffset] = 0;
  //
  // add all content of buffer
  //
  Sum = 0;
  while (Size--) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  //
  // set checksum
  //
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}

/**
  Install Error Injection Instructions to Einj table

  @param [in]      This           A pointer of WheaSuppourt protocol Instance 
  @param [in]      Type           Error type
  @param [in]      Flags          Configuration Write Enable
  @param [in]      EnableError    Enabled
  @param [out]     SourceId       Source ID
  @param [in]      NoOfRecords    Record pre-allocate number
  @param [in]      MaxSections    Max sections per record
  @param [in, out] SourceData     Error Source data
  

  @retval EFI_SUCCESS             Add Error source to HEST table success
  @retval EFI_INVALID_PARAMETER   Invalid input parameter
  @retval EFI_ABORTED             HEST table not ready yet

**/
STATIC
EFI_STATUS
InsydeWheaAddErrorSource (
  IN     EFI_WHEA_SUPPORT_PROTOCOL     *This,
  IN     WHEA_ERROR_TYPE               Type,
  IN     UINTN                         Flags,
  IN     BOOLEAN                       EnableError,
  OUT    UINT16                        *SourceId,
  IN     UINTN                         NoOfRecords,
  IN     UINTN                         MaxSections,
  IN OUT VOID                          *SourceData
  )
{
  WHEA_SUPPORT_INST                         *WheaSupportPrivateData;
  APEI_WHEA_HARDWARE_ERROR_SOURCE_TABLE     *HestTable;
  APEI_WHEA_GENERIC_ERROR_SOURCE_ENTRY      *GenErrEntry;  
  GENERIC_HW_ERROR_SOURCE                   *GenHwErrSrc;
  APEI_WHEA_PCI_EXPRESS_BRIDGE_AER          *PcieBdgAerEntry;
  PCIE_BRIDGE_AER_SOURCE                    *PcieBdgAerSrc;
  APEI_WHEA_PCI_EXPRESS_DEVICE_AER          *PcieDevAerEntry;
  PCIE_DEVICE_AER_SOURCE                    *PcieDevAerSrc;
  APEI_WHEA_PCI_EXPRESS_ROOT_PORT_AER       *PcieRootAerEntry;
  PCIE_ROOT_AER_SOURCE                      *PcieRootAerSrc;
  PCIE_AER_SOURCE_COMMON                    *ErrSrcCommEntry;
  UINT8                                     *pData;
  EFI_STATUS                                Status;
  EFI_PHYSICAL_ADDRESS                      *ErrBlkAddrReg;
  
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  if (This == NULL || SourceData == NULL || Type > GenericHw) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get WHEA support protocol instance structure data
  //  
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);
  
  if (WheaSupportPrivateData->Hest == NULL) {
    return EFI_ABORTED;
  }

  HestTable = WheaSupportPrivateData->Hest;

  pData = (UINT8 *)HestTable;
  pData += HestTable->Header.Length;
  ErrSrcCommEntry = (PCIE_AER_SOURCE_COMMON *)pData;

  switch (Type) {
    
  case GenericHw:
    //
    // Add generic error type source to HEST table
    //
    if ((HestTable->Header.Length + sizeof(APEI_WHEA_GENERIC_ERROR_SOURCE_ENTRY)) > MAX_HEST_SIZE) {
      return EFI_OUT_OF_RESOURCES;
    }
    
    GenErrEntry = (APEI_WHEA_GENERIC_ERROR_SOURCE_ENTRY *)pData;
    GenHwErrSrc = (GENERIC_HW_ERROR_SOURCE *)SourceData;

    if (mWheaSupport.CurErrorStatusBlockSize < GenHwErrSrc->ErrorStatusSize) {
      return EFI_OUT_OF_RESOURCES;
    }  
    //
    // Assign the memory space in ErrorStatusBlock for this Error source
    //    
    GenErrEntry->ErrorStatusBlockLength = GenHwErrSrc->ErrorStatusSize;
    Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof(EFI_PHYSICAL_ADDRESS), &ErrBlkAddrReg);
    if (EFI_ERROR (Status)) {      
      return Status;
    }
    *(ErrBlkAddrReg) = (UINTN)mWheaSupport.ErrorStatusBlock;
    
    GenHwErrSrc->ErrorStatusAddress.AddressSpaceId = EFI_ACPI_5_0_SYSTEM_MEMORY;
    GenHwErrSrc->ErrorStatusAddress.RegisterBitWidth = 8 * sizeof(UINTN);
    GenHwErrSrc->ErrorStatusAddress.RegisterBitOffset = 0;
    
    GenHwErrSrc->ErrorStatusAddress.AccessSize = EFI_ACPI_5_0_QWORD;
    GenHwErrSrc->ErrorStatusAddress.Address = (UINTN)ErrBlkAddrReg;

    CopyMem (&GenErrEntry->ErrorStatusAddress, &GenHwErrSrc->ErrorStatusAddress, sizeof(EFI_ACPI_GAS));
    CopyMem (&GenErrEntry->NotificationStructure, &GenHwErrSrc->NotifyMethod, sizeof(HW_NOTIFY_METHOD));
   
    GenErrEntry->MaxRawDataLength = GenHwErrSrc->MaxErrorDataSize;
    GenErrEntry->RelatedSourceId = GenHwErrSrc->RelatedSourceId;

    HestTable->Header.Length += sizeof(APEI_WHEA_GENERIC_ERROR_SOURCE_ENTRY);
    //
    // Update the size of free space in ErrorStatusBlock
    //     
    mWheaSupport.CurErrorStatusBlockSize -= GenErrEntry->ErrorStatusBlockLength;
    pData = (UINT8 *)mWheaSupport.ErrorStatusBlock;
    pData += GenErrEntry->ErrorStatusBlockLength;
    mWheaSupport.ErrorStatusBlock = (VOID *)pData;
    break;

  case PcieBridgeAer:
    //
    // Add Pcie Bridge Aer error type source to HEST table
    //    
    if ((HestTable->Header.Length + sizeof(APEI_WHEA_PCI_EXPRESS_BRIDGE_AER)) > MAX_HEST_SIZE) {
      return EFI_OUT_OF_RESOURCES;
    }    
    PcieBdgAerEntry = (APEI_WHEA_PCI_EXPRESS_BRIDGE_AER *)pData;
    PcieBdgAerSrc = (PCIE_BRIDGE_AER_SOURCE *)SourceData;

    CopyMem (&PcieBdgAerEntry->Bus, PcieBdgAerSrc, sizeof(PCIE_BRIDGE_AER_SOURCE));
    HestTable->Header.Length += sizeof(APEI_WHEA_PCI_EXPRESS_BRIDGE_AER);
    break;

  case PcieDeviceAer:
    //
    // Add Pcie Device Aer error type source to HEST table
    //     
    if ((HestTable->Header.Length + sizeof(APEI_WHEA_PCI_EXPRESS_DEVICE_AER)) > MAX_HEST_SIZE) {
      return EFI_OUT_OF_RESOURCES;
    }     
    PcieDevAerEntry = (APEI_WHEA_PCI_EXPRESS_DEVICE_AER *)pData;
    PcieDevAerSrc = (PCIE_DEVICE_AER_SOURCE *)SourceData;

    CopyMem (&PcieDevAerEntry->Bus, PcieDevAerSrc, sizeof(PCIE_DEVICE_AER_SOURCE));    
    HestTable->Header.Length += sizeof(APEI_WHEA_PCI_EXPRESS_DEVICE_AER);
    break;

  case PcieRootAer:
    //
    // Add Pcie Roor bridge Aer error type source to HEST table
    //      
    if ((HestTable->Header.Length + sizeof(APEI_WHEA_PCI_EXPRESS_ROOT_PORT_AER)) > MAX_HEST_SIZE) {
      return EFI_OUT_OF_RESOURCES;
    }     
    PcieRootAerEntry = (APEI_WHEA_PCI_EXPRESS_ROOT_PORT_AER *)pData;
    PcieRootAerSrc = (PCIE_ROOT_AER_SOURCE *)SourceData;

    CopyMem (&PcieRootAerEntry->Bus, PcieRootAerSrc, sizeof(PCIE_ROOT_AER_SOURCE));
    HestTable->Header.Length += sizeof(APEI_WHEA_PCI_EXPRESS_ROOT_PORT_AER);
    break;    

  default:
    //
    // Unsupport Error type
    //      
    return EFI_UNSUPPORTED;
    
  }

  ErrSrcCommEntry->Type = (UINT16)Type;
  ErrSrcCommEntry->SourceId = (UINT16)HestTable->ErrorSourceCount;
  if (Type != XpfNmi) {
    ErrSrcCommEntry->Flags = (UINT8)Flags;  
  }  
  if (Type != XpfMce && Type != XpfNmi) {
    ErrSrcCommEntry->Enabled = (UINT8)EnableError;    
  }  
  ErrSrcCommEntry->NumberOfRecordsToPreAllocate = (UINT32)NoOfRecords;
  ErrSrcCommEntry->MaxSectionsPerRecord         = (UINT32)MaxSections;


  *SourceId = (UINT16)HestTable->ErrorSourceCount;
  HestTable->ErrorSourceCount++;
  //
  // Update Checksum
  //
  InsydeWheaChecksum (HestTable);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Add boot error log to memory and show it to Windows.

  @param [in] *This               This.
  @param [in] ErrorCondtion       Reserved.
  @param [in] ErrorSevirity       Generic error severity in data entry
                                    GENERIC_ERROR_RECOVERABLE       0
                                    GENERIC_ERROR_FATAL             1
                                    GENERIC_ERROR_CORRECTED         2
                                    GENERIC_ERROR_NONE              3
                                    GENERIC_ERROR_INFO              3
  @param [in] *FruID              Optional, FruID.
  @param [in] *FruDescription     Optional, FruDescription.
  @param [in] *ErrorType          Error type GUID.
  @param [in] ErrorDataSize       Error data size.
  @param [in] *ErrorData          Point to the address of error section.
  

  @retval EFI_SUCCESS             Add BOOT Error Log success.

**/
STATIC
EFI_STATUS
InsydeWheaAddBootErrorLog (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINT8                             ErrorCondtion,
  IN UINTN                             ErrorSevirity,
  IN EFI_GUID                          *FruID,          OPTIONAL
  IN CHAR8                             *FruDescription, OPTIONAL
  IN EFI_GUID                          *ErrorType,
  IN UINTN                             ErrorDataSize,
  IN UINT8                             *ErrorData       OPTIONAL
  )
{
  APEI_WHEA_GENERIC_ERROR_STATUS       *BootErrorAddress;
  APEI_WHEA_GENERIC_ERROR_DATA_ENTRY   *BootErrorDataEntry;
  VOID                                 *ErrorSection;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  //
  // Calculate Address 
  //
  BootErrorAddress = mWheaSupport.BootErrorRegion;

  BootErrorDataEntry = (APEI_WHEA_GENERIC_ERROR_DATA_ENTRY *) ((UINTN)BootErrorAddress + sizeof(APEI_WHEA_GENERIC_ERROR_STATUS));
  if (BootErrorDataEntry->Revision != 0){
    BootErrorDataEntry = (APEI_WHEA_GENERIC_ERROR_DATA_ENTRY *) ((UINTN)BootErrorDataEntry + BootErrorAddress->DataLength);
  }

  ErrorSection = (VOID *) ((UINTN)BootErrorDataEntry + sizeof(APEI_WHEA_GENERIC_ERROR_DATA_ENTRY));

  //
  // APEI_WHEA_GENERIC_ERROR_STATUS 
  //
  BootErrorAddress->ErrorSeverity = (UINT32)ErrorSevirity;
  BootErrorAddress->BlockStatus.ErrorDataEntryCount++;

  if (BootErrorAddress->ErrorSeverity == GENERIC_ERROR_FATAL) {
    if (BootErrorAddress->BlockStatus.UncorrectableErrorValid != 1) {
      BootErrorAddress->BlockStatus.UncorrectableErrorValid = 1;
    }
    else {
      BootErrorAddress->BlockStatus.MultipleUncorrectableErrors = 1;
    }
  }
  else {
    if (BootErrorAddress->BlockStatus.CorrectableErrorValid != 1) {
      BootErrorAddress->BlockStatus.CorrectableErrorValid = 1;
    }
    else {
      BootErrorAddress->BlockStatus.MultipleCorrectableErrors = 1;
    }
  }

  BootErrorAddress->RawDataOffset = 0;
  BootErrorAddress->RawDataLength = 0;
  BootErrorAddress->DataLength += ((UINT32)sizeof (APEI_WHEA_GENERIC_ERROR_DATA_ENTRY) + (UINT32)ErrorDataSize);

  //
  // APEI_WHEA_GENERIC_ERROR_DATA_ENTRY 
  //
  CopyMem(&BootErrorDataEntry->SectionType, ErrorType, sizeof(EFI_GUID));
  BootErrorDataEntry->ErrorSeverity   = (UINT32)ErrorSevirity;
  BootErrorDataEntry->Revision        = GENERIC_ERROR_SECTION_REVISION;
  BootErrorDataEntry->ErrorDataLength = (UINT32)ErrorDataSize;
  if (BootErrorAddress->BlockStatus.ErrorDataEntryCount <= 1) {
    BootErrorDataEntry->Flags |= EFI_ERROR_SECTION_FLAGS_PRIMARY;
  }
  else {
    BootErrorDataEntry->Flags |= EFI_ERROR_SECTION_FLAGS_LATENT_ERROR;
  }
  if (FruID != NULL) {
    BootErrorDataEntry->ValidationBits |= EFI_ERROR_SECTION_FRU_ID_VALID;
    CopyMem(&BootErrorDataEntry->FruId, FruID, sizeof(EFI_GUID));
  }
  if (FruDescription != NULL) {
    BootErrorDataEntry->ValidationBits |= EFI_ERROR_SECTION_FRU_STRING_VALID;
    CopyMem((BootErrorDataEntry->FruText), FruDescription, sizeof (CHAR8) * 20);
  }

  //
  // ERROR_SECTION 
  //
  CopyMem(ErrorSection, ErrorData, ErrorDataSize);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Return Error injection capability

  @param [in]  This               A pointer of WheaSuppourt protocol Instance.
  @param [out] InjectCapability   Return ErrorLogAddressRange size.

  @retval EFI_SUCCESS             Retrive data Success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.
  
**/
STATIC
EFI_STATUS
InsydeWheaGetErrorInjectionCapability (
  IN  EFI_WHEA_SUPPORT_PROTOCOL        *This,
  OUT UINTN                            *InjectCapability
  )
{
  WHEA_SUPPORT_INST                    *WheaSupportPrivateData;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));
  
  if (This == NULL || InjectCapability == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);

  //
  // Return Error Injection Capability
  //
  (*InjectCapability) = WheaSupportPrivateData->InstalledErrorInj;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Return ErrorLogAddressRange location and size.

  @param [in]  This               A pointer of WheaSuppourt protocol Instance.
  @param [out] ElarSize           Return ErrorLogAddressRange size.
  @param [out] LogAddress         Return ErrorLogAddressRange location.

  @retval EFI_SUCCESS             Retrive data Success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.
  
**/
STATIC
EFI_STATUS
InsydeWheaGetElar (
  IN  EFI_WHEA_SUPPORT_PROTOCOL        *This,
  OUT UINTN                            *ElarSize,
  OUT VOID                             **LogAddress
  )
{
  WHEA_SUPPORT_INST                    *WheaSupportPrivateData;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);

  (*ElarSize) = WheaSupportPrivateData->ErrorLogAddressRangeSize;
  (*LogAddress) = WheaSupportPrivateData->ErrorLogAddressRange;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));
  return EFI_SUCCESS;
}

/**
  Return ErrorStatusBlock location and size

  @param [in]  This               A pointer of WheaSuppourt protocol Instance.
  @param [out] ErrBlkSize         Return ErrorStatusBlock size.
  @param [out] ErrBlkAddr         Return ErrorStatusBlock location.
  
  @retval EFI_SUCCESS             Retrive data Success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.
  
**/
STATIC
EFI_STATUS
InsydeWheaGetErrorStatusBlock (
  IN  EFI_WHEA_SUPPORT_PROTOCOL        *This,
  OUT UINTN                            *ErrBlkSize,
  OUT VOID                             **ErrBlkAddr
  )
{
  WHEA_SUPPORT_INST                    *WheaSupportPrivateData;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);

  //
  // Return ErrorStatusBlock location and size
  //
  (*ErrBlkSize) = WheaSupportPrivateData->ErrorStatusBlockSize;
  (*ErrBlkAddr) = WheaSupportPrivateData->ErrorStatusBlock;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Install Error Injection Instructions to Einj table

  @param [in] This                A pointer of WheaSuppourt protocol Instance.
  @param [in] ErrorType           Supported Error type.
  @param [in] InstCount           Total instructions.
  @param [in] InstEntry           A pointer of instruction array.
  
  @retval EFI_SUCCESS             Update table success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.
  @retval EFI_ABORTED             EINJ table not ready yet.

**/
STATIC
EFI_STATUS
InsydeWheaInstallErrorInjectionMethod (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINTN                             ErrorType,
  IN UINTN                             InstCount,
  IN VOID                              *InstEntry
  )
{
  WHEA_SUPPORT_INST                    *WheaSupportPrivateData;
  APEI_WHEA_ERROR_INJECTION_TABLE      *EinjTable;
  UINT32                               InstrLength;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));
  //
  // Check input paprameter
  //  
  if (This == NULL || InstEntry == NULL || InstCount == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get WHEA support protocol instance structure data
  //
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);
  if (WheaSupportPrivateData->Einj == NULL) {
    return EFI_ABORTED;
  }

  WheaSupportPrivateData->InstalledErrorInj = ErrorType;

  EinjTable = WheaSupportPrivateData->Einj;

  InstrLength = (UINT32)(InstCount * sizeof(APEI_WHEA_EINJ_INJECTION_INSTRUCTION));

  if ((InstrLength + EinjTable->Header.Length) > MAX_EINJ_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Copy Instructions
  //
  CopyMem (EinjTable->InstructionEntry, InstEntry, InstrLength);
  
  EinjTable->InstructionEntryCount = (UINT32)InstCount;
  //
  // Update length
  //
  EinjTable->Header.Length += InstrLength;
  //
  // Update Checksum
  //  
  InsydeWheaChecksum (EinjTable);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Install Elog Instructions to ERST table

  @param [in] This                A pointer of WheaSuppourt protocol Instance 
  @param [in] InstCount           Total instructions
  @param [in] InstEntry           A pointer of instruction array
  
  @retval EFI_SUCCESS             Update table success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.
  @retval EFI_ABORTED             ERST table not ready yet.

**/
STATIC
EFI_STATUS
InsydeWheaInstallErrorRecordMethod (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINTN                             InstCount,
  IN VOID                              *InstEntry
  )
{

  WHEA_SUPPORT_INST                              *WheaSupportPrivateData;
  APEI_WHEA_ERROR_RECORD_SERIALIZATION_TABLE     *ErstTable;
  UINT32                                         InstrLength;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  //
  // Check input paprameter
  //
  if (This == NULL || InstEntry == NULL || InstCount == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get WHEA support protocol instance structure data
  //
  WheaSupportPrivateData = WHEA_SUPPORT_FROM_THIS (This);
  if (WheaSupportPrivateData->Einj == NULL) {
    return EFI_ABORTED;
  }

  ErstTable = WheaSupportPrivateData->Erst;

  InstrLength = (UINT32)(InstCount * sizeof(APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION));
  
  if ((InstrLength + ErstTable->Header.Length) > MAX_ERST_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Copy Instructions
  //
  CopyMem (ErstTable->InstructionEntry, InstEntry, InstrLength);
  
  ErstTable->InstructionEntryCount = (UINT32)InstCount;
  //
  // Update length
  //
  ErstTable->Header.Length += InstrLength;
  //
  // Update Checksum
  //
  InsydeWheaChecksum (ErstTable);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Installing Whea tables main function

  @param [in] WheaInst            A Instance of WheaPlatformSuppourt protocol
  
  @retval EFI_SUCCESS             Install success.
  @retval EFI_INVALID_PARAMETER   Invalid input parameter.

**/
STATIC
EFI_STATUS
InsydeWheaInstallWheaTables (
  IN WHEA_SUPPORT_INST                 *WheaInst
  )
{
  UINTN                                TableHandle;
  EFI_STATUS                           Status;
  EFI_ACPI_TABLE_VERSION               TableVersion;
  EFI_ACPI_DESCRIPTION_HEADER          *CurrentTable; 
  
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  
  if (WheaInst == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check table has been installed or not
  //
  if (!WheaInst->TablesInstalled) {
    
    TableVersion = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
    //
    // Install HEST table
    //
    CurrentTable = (EFI_ACPI_DESCRIPTION_HEADER *)WheaInst->Hest;
    if (CurrentTable != NULL && CurrentTable->Length > 0) {
      TableHandle = 0;
      Status = mAcpiSupport->SetAcpiTable (
                               mAcpiSupport,
                               CurrentTable,
                               TRUE,
                               TableVersion,
                               &TableHandle
                               );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Install ERST table
    //    
    CurrentTable = (EFI_ACPI_DESCRIPTION_HEADER *)WheaInst->Erst;
    if (CurrentTable != NULL && CurrentTable->Length > 0) {
      TableHandle = 0;
      Status = mAcpiSupport->SetAcpiTable (
                               mAcpiSupport,
                               CurrentTable,
                               TRUE,
                               TableVersion,
                               &TableHandle
                               );
      if (EFI_ERROR (Status)) {
        return Status;
      }      
    }
    //
    // Install BERT table
    //    
    CurrentTable = (EFI_ACPI_DESCRIPTION_HEADER *)WheaInst->Bert;
    if (CurrentTable != NULL && CurrentTable->Length > 0) {
      TableHandle = 0;
      Status = mAcpiSupport->SetAcpiTable (
                               mAcpiSupport,
                               CurrentTable,
                               TRUE,
                               TableVersion,
                               &TableHandle
                               );
      if (EFI_ERROR (Status)) {
        return Status;
      }      
    }
    //
    // Install EINJ table
    //    
    CurrentTable = (EFI_ACPI_DESCRIPTION_HEADER *)WheaInst->Einj;
    if (CurrentTable != NULL && CurrentTable->Length > 0) {
      TableHandle = 0;
      Status = mAcpiSupport->SetAcpiTable (
                               mAcpiSupport,
                               CurrentTable,
                               TRUE,
                               TableVersion,
                               &TableHandle
                               );
      if (EFI_ERROR (Status)) {
        return Status;
      }      
    }    
    //
    // Free temporay table
    //  
    if (WheaInst->Hest != NULL) {
      gBS->FreePool (WheaInst->Hest);
    }
    if (WheaInst->Erst != NULL) {
      gBS->FreePool (WheaInst->Erst);
    }
    if (WheaInst->Bert != NULL) {
      gBS->FreePool (WheaInst->Bert);      
    }
    if (WheaInst->Einj != NULL) {
      gBS->FreePool (WheaInst->Einj);
    }    

    WheaInst->TablesInstalled = TRUE;
  }
  
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Installing tables event callback.

  @param [in] Event               Event instance
  @param [in] Context             Event Context
  
**/
VOID
EFIAPI
InsydeWheaCheckInstallTables (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                           Status;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));
  //
  // Return if table has been installed
  //
  if (mReadyToBoot) {
    return;
  }

  Status = InsydeWheaInstallWheaTables (&mWheaSupport);
  if (EFI_ERROR (Status)) {
    return;
  }
  
  mReadyToBoot = TRUE;  

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

}

/**
  Installing tables event callback.

  @param [in] Header              Table Address.
  @param [in] TableName           Tabel Name.
  
  @retval EFI_SUCCESS             WHEA Table initial success.
  @retval EFI_INVALID_PARAMETER   Acpi Table Address is null.

**/
EFI_STATUS
InsydeWheaInitialTableHeader (
  IN EFI_ACPI_DESCRIPTION_HEADER       *Header,
  IN UINT32                            TableName
  )
{

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));

  if (Header == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Header->Signature = TableName;
  InsydeWheaUpdateAcpiTableIds (Header);    

  switch (TableName) {
    //
    // Initialize HEST table common header
    //
    case APEI_WHEA_HEST_TABLE_SIGNATURE:
      Header->Revision                    = APEI_WHEA_HEST_TABLE_REVISION;
      Header->Length                      = sizeof(APEI_WHEA_HARDWARE_ERROR_SOURCE_TABLE);
      mWheaSupport.Hest->ErrorSourceCount = 0;
    break;
    //
    // Initialize BERT table common header
    //
    case APEI_WHEA_BERT_TABLE_SIGNATURE:
      Header->Revision                         = APEI_WHEA_BERT_TABLE_REVISION;
      Header->Length                           = sizeof(APEI_WHEA_BOOT_ERROR_RECORD_TABLE);
      mWheaSupport.Bert->BootErrorRegion       = (UINTN) mWheaSupport.BootErrorRegion;
      mWheaSupport.Bert->BootErrorRegionLength = (UINT32) mWheaSupport.BootErrorRegionLen;
    break;
    //
    // Initialize ERST table common header
    //
    case APEI_WHEA_ERST_TABLE_SIGNATURE:
      Header->Revision                           = APEI_WHEA_ERST_TABLE_REVISION;
      Header->Length                             = sizeof(APEI_WHEA_ERST_HEADER);
      mWheaSupport.Erst->SerializationHeaderSize = sizeof(APEI_WHEA_ERST_HEADER);
    break;
    //
    // Initialize EINJ table common header
    //
    case APEI_WHEA_EINJ_TABLE_SIGNATURE:
      Header->Revision                       = APEI_WHEA_EINJ_TABLE_REVISION;
      Header->Length                         = sizeof(APEI_WHEA_EINJ_HEADER);
      mWheaSupport.Einj->InjectionHeaderSize = sizeof(APEI_WHEA_EINJ_HEADER);
    break;

    default:
    break;
  }
  InsydeWheaChecksum (Header);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Installing tables event callback.

  @param [in] None
  
  @retval EFI_SUCCESS             WHEA Table initial success.

**/
EFI_STATUS
InsydeWheaInitialTable (
  VOID
  )
{
  EFI_STATUS                           Status;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));
  //
  // Allocate memory space for HEST table
  //
  mWheaSupport.Hest = (APEI_WHEA_HARDWARE_ERROR_SOURCE_TABLE *) AllocateReservedZeroPool (MAX_HEST_SIZE);
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: HEST table : 0x%08x\n", mWheaSupport.Hest));
  //
  // Allocate memory space for BERT table
  //
  mWheaSupport.Bert = (APEI_WHEA_BOOT_ERROR_RECORD_TABLE *) AllocateReservedZeroPool (MAX_BERT_SIZE);
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: BERT table : 0x%08x\n", mWheaSupport.Bert));
  //
  // Allocate memory space for ERST table
  //
  mWheaSupport.Erst = (APEI_WHEA_ERROR_RECORD_SERIALIZATION_TABLE *) AllocateReservedZeroPool (MAX_ERST_SIZE);
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: ERST table : 0x%08x\n", mWheaSupport.Erst));
  //
  // Allocate memory space for EINJ table
  //
  mWheaSupport.Einj = (APEI_WHEA_ERROR_INJECTION_TABLE *) AllocateReservedZeroPool (MAX_EINJ_SIZE);
  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: EINJ table : 0x%08x\n", mWheaSupport.Einj));
  //
  // Allocate memory space Error status blocks
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, MAX_ERROR_STATUS_SIZE, &mWheaSupport.ErrorStatusBlock);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_SUPPORT] ERROR: Allocate Error Status Block :(%r)\n", Status));
    return Status;
  }
  ZeroMem (mWheaSupport.ErrorStatusBlock, MAX_ERROR_STATUS_SIZE);
  mWheaSupport.ErrorStatusBlockSize    = MAX_ERROR_STATUS_SIZE;
  mWheaSupport.CurErrorStatusBlockSize = mWheaSupport.ErrorStatusBlockSize;
  //
  // Allocate memory space for Error Log Address range
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, MAX_ERROR_LOG_RANGE_SIZE, &mWheaSupport.ErrorLogAddressRange);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_SUPPORT] ERROR: Allocate Error Log :(%r)\n", Status));
    return Status;
  }
  ZeroMem (mWheaSupport.ErrorLogAddressRange, MAX_ERROR_LOG_RANGE_SIZE);
  mWheaSupport.ErrorLogAddressRangeSize = MAX_ERROR_LOG_RANGE_SIZE;
  //
  // Allocate memory space for Boot Error Log space
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, MAX_BOOT_ERROR_LOG_SIZE, &mWheaSupport.BootErrorRegion);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_SUPPORT] ERROR: Allocate Boot Error Log Space :(%r)\n", Status));
    return Status;
  }
  ZeroMem (mWheaSupport.BootErrorRegion, MAX_BOOT_ERROR_LOG_SIZE);
  mWheaSupport.BootErrorRegionLen = MAX_BOOT_ERROR_LOG_SIZE;
  //
  // Initialize table 
  //
  InsydeWheaInitialTableHeader (&mWheaSupport.Hest->Header, APEI_WHEA_HEST_TABLE_SIGNATURE);
  InsydeWheaInitialTableHeader (&mWheaSupport.Bert->Header, APEI_WHEA_BERT_TABLE_SIGNATURE);
  InsydeWheaInitialTableHeader (&mWheaSupport.Erst->Header, APEI_WHEA_ERST_TABLE_SIGNATURE);
  InsydeWheaInitialTableHeader (&mWheaSupport.Einj->Header, APEI_WHEA_EINJ_TABLE_SIGNATURE);

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Entry point of the Whea support driver.

  @param [in] ImageHandle         A handle for the image that is initializing this driver
  @param [in] SystemTable         A pointer to the EFI system table

  @retval EFI_SUCCESS             Driver initialized successfully
  @retval EFI_OUT_OF_RESOURCES    Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InsydeWheaSupportEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           NewHandle;
  EFI_EVENT                            InstallTableEvent;

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() Start\n", __FUNCTION__));
  //
  // Initialize WHEA Table.
  //
  Status = InsydeWheaInitialTable();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Initialize protocol interface
  //
  mWheaSupport.WheaSupport.InsydeWheaAddErrorSource              = InsydeWheaAddErrorSource;
  mWheaSupport.WheaSupport.InsydeWheaAddBootErrorLog             = InsydeWheaAddBootErrorLog;
  mWheaSupport.WheaSupport.InsydeWheaInstallErrorInjectionMethod = InsydeWheaInstallErrorInjectionMethod;
  mWheaSupport.WheaSupport.InsydeWheaGetErrorInjectionCapability = InsydeWheaGetErrorInjectionCapability;
  mWheaSupport.WheaSupport.InsydeWheaGetElar                     = InsydeWheaGetElar;
  mWheaSupport.WheaSupport.InsydeWheaInstallErrorRecordMethod    = InsydeWheaInstallErrorRecordMethod;
  mWheaSupport.WheaSupport.InsydeWheaGetErrorStatusBlock         = InsydeWheaGetErrorStatusBlock;
  //
  // Install protocol
  //
  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gEfiWheaSupportProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mWheaSupport.WheaSupport
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Setup Notification for installing tables
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, &mAcpiSupport);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             InsydeWheaCheckInstallTables,
             NULL,
             &InstallTableEvent
             );

  DEBUG ((DEBUG_INFO, "[APEI_SUPPORT] INFO: %a() End\n", __FUNCTION__));

  return Status;
}
