/** @file
  Implementation of StoreDebugMessagePei module.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <StoreDebugMessagePei.h>

EFI_PEI_NOTIFY_DESCRIPTOR mRscHandlerPpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRscHandlerPpiGuid,
  RscHandlerPpiNotifyCallback
  };

EFI_PEI_NOTIFY_DESCRIPTOR mMemoryDiscoveredPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  DebugMessageMemBufferInit
};

/**
  Notification function for ReportStatusCode Handler Ppi

  This routine is the notification function for EFI_RSC_HANDLER_PPI

  @param  PeiServices           Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor      Address of the notification descriptor data structure. Type
                                EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                   Address of the PPI that was installed.

  @retval EFI_STATUS            

**/
EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_RSC_HANDLER_PPI           *RscHandlerPpi;

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (!EFI_ERROR(Status)) {
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = RscHandlerPpi->Register (PeiDebugMessageStatusCode);
  } 
  ASSERT_EFI_ERROR (Status);

  return Status;
}

//[-start-140326-IB08400258-add]//
/**
  Extracts ASSERT() information from a status code structure.

  Converts the status code specified by CodeType, Value, and Data to the ASSERT()
  arguments specified by Filename, Description, and LineNumber.  If CodeType is
  an EFI_ERROR_CODE, and CodeType has a severity of EFI_ERROR_UNRECOVERED, and
  Value has an operation mask of EFI_SW_EC_ILLEGAL_SOFTWARE_STATE, extract
  Filename, Description, and LineNumber from the optional data area of the
  status code buffer specified by Data.  The optional data area of Data contains
  a Null-terminated ASCII string for the FileName, followed by a Null-terminated
  ASCII string for the Description, followed by a 32-bit LineNumber.  If the
  ASSERT() information could be extracted from Data, then return TRUE.
  Otherwise, FALSE is returned.

  @param  CodeType     The type of status code being converted.
  @param  Value        The status code value being converted.
  @param  Data         Pointer to status code data buffer.
  @param  Filename     Pointer to the source file name that generated the ASSERT().
  @param  Description  Pointer to the description of the ASSERT().
  @param  LineNumber   Pointer to source line number that generated the ASSERT().

  @retval  TRUE   The status code specified by CodeType, Value, and Data was
                  converted ASSERT() arguments specified by Filename, Description,
                  and LineNumber.
  @retval  FALSE  The status code specified by CodeType, Value, and Data could
                  not be converted to ASSERT() arguments.

**/
BOOLEAN
EFIAPI
ExtractAssertInfo (
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  IN CONST EFI_STATUS_CODE_DATA  *Data,
  OUT CHAR8                      **Filename,
  OUT CHAR8                      **Description,
  OUT UINT32                     *LineNumber
  )
{
  EFI_DEBUG_ASSERT_DATA  *AssertData;

  if (((CodeType & EFI_STATUS_CODE_TYPE_MASK)      == EFI_ERROR_CODE) &&
      ((CodeType & EFI_STATUS_CODE_SEVERITY_MASK)  == EFI_ERROR_UNRECOVERED) &&
      ((Value    & EFI_STATUS_CODE_OPERATION_MASK) == EFI_SW_EC_ILLEGAL_SOFTWARE_STATE)) {
    AssertData   = (EFI_DEBUG_ASSERT_DATA *)(Data + 1);
    *Filename    = (CHAR8 *)(AssertData + 1);
    *Description = *Filename + AsciiStrLen (*Filename) + 1;
    *LineNumber  = AssertData->LineNumber;
    return TRUE;
  }
  return FALSE;
}

/**
  Extracts DEBUG() information from a status code structure.

  Converts the status code specified by Data to the DEBUG() arguments specified
  by ErrorLevel, Marker, and Format.  If type GUID in Data is
  EFI_STATUS_CODE_DATA_TYPE_DEBUG_GUID, then extract ErrorLevel, Marker, and
  Format from the optional data area of the status code buffer specified by Data.
  The optional data area of Data contains a 32-bit ErrorLevel followed by Marker
  which is 12 UINTN parameters, followed by a Null-terminated ASCII string for
  the Format.  If the DEBUG() information could be extracted from Data, then
  return TRUE.  Otherwise, FALSE is returned.

  @param  Data        Pointer to status code data buffer.
  @param  ErrorLevel  Pointer to error level mask for a debug message.
  @param  Marker      Pointer to the variable argument list associated with Format.
  @param  Format      Pointer to a Null-terminated ASCII format string of a
                      debug message.

  @retval  TRUE   The status code specified by Data was converted DEBUG() arguments
                  specified by ErrorLevel, Marker, and Format.
  @retval  FALSE  The status code specified by Data could not be converted to
                  DEBUG() arguments.

**/
BOOLEAN
EFIAPI
ExtractDebugInfo (
  IN CONST EFI_STATUS_CODE_DATA  *Data,
  OUT UINT32                     *ErrorLevel,
  OUT BASE_LIST                  *Marker,
  OUT CHAR8                      **Format
  )
{
  EFI_DEBUG_INFO  *DebugInfo;

  //
  // If the GUID type is not EFI_STATUS_CODE_DATA_TYPE_DEBUG_GUID then return FALSE
  //
  if (!CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeDebugGuid)) {
    return FALSE;
  }

  //
  // Retrieve the debug information from the status code record
  //
  DebugInfo = (EFI_DEBUG_INFO *)(Data + 1);

  *ErrorLevel = DebugInfo->ErrorLevel;

  //
  // The first 12 * sizeof (UINT64) bytes following EFI_DEBUG_INFO are for variable arguments
  // of format in DEBUG string. Its address is returned in Marker and has to be 64-bit aligned.
  // It must be noticed that EFI_DEBUG_INFO follows EFI_STATUS_CODE_DATA, whose size is
  // 20 bytes. The size of EFI_DEBUG_INFO is 4 bytes, so we can ensure that Marker
  // returned is 64-bit aligned.
  // 64-bit aligned is a must, otherwise retrieving 64-bit parameter from BASE_LIST will
  // cause unalignment exception.
  //
  *Marker = (BASE_LIST) (DebugInfo + 1);
  *Format = (CHAR8 *)(((UINT64 *)*Marker) + 12);

  return TRUE;
}

//[-end-140326-IB08400258-add]//

/**
 Store DEBUG messages string to reserved memory.

 @param[in]  MessageString  - The String for the DEBUG messages.
 @param[in]  StringSize     - String size of the MessageString.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StoreAsciiDebugMessage (
  IN CHAR8                                 *MessageString,
  IN UINT32                                StringSize
  )  
{
  PEI_DEBUG_MESSAGE_HEAD_STRUCTURE        *DebugMessageHead = NULL;
  CHAR8                                   *StoreAddr;

  DebugMessageHead = (PEI_DEBUG_MESSAGE_HEAD_STRUCTURE *)(UINT32)PcdGet32(PcdStoreDebugMsgEntryPei);

  if ((DebugMessageHead->AdmgEndAddr + StringSize) > (DebugMessageHead->AdmgStartAddr + DebugMessageHead->AdmgSize)) {
    //
    // The total reserved memory space is not available for more DEBUG messages.
    //
    PcdSetBool (PcdStoreDebugMsgEntryPeiEn, FALSE);
    return EFI_UNSUPPORTED;
  }
  StoreAddr = (CHAR8 *)(UINT32)DebugMessageHead->AdmgEndAddr;
  CopyMem (StoreAddr, MessageString, StringSize);
  DebugMessageHead->AdmgEndAddr += (UINT32)StringSize;

  return EFI_SUCCESS;
}

/**
  Report Status Code to Store DEBUG Message PEI module.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     
**/
EFI_STATUS
EFIAPI
PeiDebugMessageStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  ) 
{
  EFI_STATUS      Status;
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[MAX_DEBUG_MESSAGE_LENGTH];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINT32          CharCount;
  BASE_LIST       Marker;

  //
  // The module is not initialized for the service, 
  // or the memory space for storing DEBUG messages is not available.
  //
  if (PcdGetBool (PcdStoreDebugMsgEntryPeiEn) == FALSE) {
    return EFI_UNSUPPORTED;
  }

//[-start-140326-IB08400258-modify]//
  Buffer[0] = '\0';

  if (Data != NULL && ExtractAssertInfo (CodeType, CodeValue, Data, &Filename, &Description, &LineNumber)) {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rPEI_ASSERT!: %a (%d): %a\n\r",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if (Data != NULL && ExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format))  { 
    //
    // Print DEBUG() information into output buffer.
    //
    CharCount = AsciiBSPrint (
                  Buffer, 
                  sizeof (Buffer), 
                  Format, 
                  Marker
                  );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    //
    // Print ERROR information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "ERROR: C%x:V%x I%x",
                  CodeType,
                  CodeValue,
                  Instance
                  );

    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }

    CharCount += AsciiSPrint (
                   &Buffer[CharCount],
                   (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                   "\n\r"
                   );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    //
    // Print PROGRESS information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "PROGRESS CODE: V%x I%x\n\r",
                  CodeValue,
                  Instance
                  );
  } else if (Data != NULL &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             ((EFI_STATUS_CODE_STRING_DATA *) Data)->StringType == EfiStringAscii) {
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a\n\r",
                  ((EFI_STATUS_CODE_STRING_DATA *) Data)->String.Ascii
                  );
  } else {
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "Undefined: C%x:V%x I%x\n\r",
                  CodeType,
                  CodeValue,
                  Instance
                  );
  }
//[-end-140326-IB08400258-modify]//

  if (CharCount == 0) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Store the DEBUG message to reserved memory space with ASCII code.
  //
  Status = StoreAsciiDebugMessage (Buffer, CharCount);

  return Status;
}

/**
 
 Allocate a EfiBootServicesData type of memory buffer for store event.
 And inital DEBUG Message Head information to this memory.
         
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
DebugMessageMemBufferInit (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )    
{    
  UINT32                                    TableSize;    
  PEI_DEBUG_MESSAGE_HEAD_STRUCTURE          *PeiDebugMessageBuffer = NULL;  
    
  //
  // Allocate bootservice memory for Storing DEBUG message..
  //
  TableSize = PcdGet32(PcdStoreDebugMsgSizePei);

  PeiDebugMessageBuffer = AllocatePages (EFI_SIZE_TO_PAGES (TableSize));
  if (PeiDebugMessageBuffer == NULL) { 
    return EFI_UNSUPPORTED;
  }

  ZeroMem ((VOID *) PeiDebugMessageBuffer, TableSize);
  PeiDebugMessageBuffer->AdmgStartAddr = (UINT32)((UINT8 *)PeiDebugMessageBuffer + sizeof(PEI_DEBUG_MESSAGE_HEAD_STRUCTURE));
  PeiDebugMessageBuffer->AdmgEndAddr   = (UINT32)((UINT8 *)PeiDebugMessageBuffer + sizeof(PEI_DEBUG_MESSAGE_HEAD_STRUCTURE));
  PeiDebugMessageBuffer->AdmgSize      = (UINT32)(TableSize - sizeof(PEI_DEBUG_MESSAGE_HEAD_STRUCTURE));

  //
  // Set the PCD value so that not only for PEI service, 
  // but also let DXE driver to find the memory space and get the stored messages.
  //
  PcdSet32 (PcdStoreDebugMsgEntryPei, (UINT32)PeiDebugMessageBuffer);
  PcdSetBool (PcdStoreDebugMsgEntryPeiEn, TRUE);
  
  return EFI_SUCCESS;
}

/**
  Perform PEI Post Message Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
StoreDebugMessagePeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_RSC_HANDLER_PPI               *RscHandlerPpi;

  if (PcdGetBool (PcdStoreDebugMsgEnPei) == FALSE) {
    return EFI_UNSUPPORTED;
  }

  //
  // register callback on "gEfiPeiMemoryDiscoveredPpiGuid" PPI
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, &mMemoryDiscoveredPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }
    
  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (EFI_ERROR (Status)) {
    //
    // Register callback function for PEI_RSC_HANDLER_PPI.
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mRscHandlerPpiNotify); 
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Status = RscHandlerPpi->Register (PeiDebugMessageStatusCode);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  return Status;
}

