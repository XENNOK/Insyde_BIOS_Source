/** @file

  Store DEBUG() Message SMM implementation.

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

#include <StoreDebugMessageSmm.h>
 
//[-start-140512-IB08400265-modify]//
EFI_SMM_RSC_HANDLER_PROTOCOL          *mRscHandlerProtocol   = NULL;
BOOLEAN                               mDebugMessageEn = FALSE;
//[-end-140512-IB08400265-modify]//

//[-start-140512-IB08400265-add]//
/**
 
 Reallocate a memory buffer for store DEBUG messages.
 And copy original DEBUG Message data to this memory.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ResizeMemoryBuffer (
  VOID
  )    
{    
  EFI_STATUS                                Status;
  DEBUG_MESSAGE_HEAD_STRUCTURE              *DebugMessageHead=NULL;
  DEBUG_MESSAGE_HEAD_STRUCTURE              *OldDebugMessageHead=NULL;
  UINT32                                    TableSize;    
  EFI_PHYSICAL_ADDRESS                      DebugMessageBuffer;   
  DEBUG_MESSAGE_EF_SEGMENT_STRUCTURE        DebugMessageStructure;
  UINT32                                    OldDebugMsgSize;

  OldDebugMessageHead = (DEBUG_MESSAGE_HEAD_STRUCTURE *)(UINTN)(PcdGet32(PcdStoreDebugMsgMemAddress));
  if (OldDebugMessageHead->TotalSize == PcdGet32(PcdMaxStoreDebugMsgSize)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // Allocate reserved memory for Storing DEBUG message.
  //
  DebugMessageBuffer = 0xFFFFFFFF;
  TableSize = PcdGet32(PcdMaxStoreDebugMsgSize);

  //
  // Use EfiReservedMemoryType memory section to store messages.
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES(TableSize),
                  &DebugMessageBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32 (PcdStoreDebugMsgMemAddress, (UINT32)DebugMessageBuffer);
  
  //
  // Store DEBUG Message Head address and add signature '_EMSGE_', Entry/End Address and total size information.
  //
  DebugMessageHead = (DEBUG_MESSAGE_HEAD_STRUCTURE *)(UINTN)DebugMessageBuffer;
  ZeroMem ((VOID *)DebugMessageHead, TableSize);
  DebugMessageHead->DumpDebugMessageSignature   = DUMP_DEBUG_MESSAGE_INSTANCE_SIGNATURE;
  DebugMessageHead->TotalSize                   = TableSize;
  DebugMessageHead->AdmgStartAddr               = (UINT32)(DebugMessageBuffer + sizeof(DEBUG_MESSAGE_HEAD_STRUCTURE));    
  DebugMessageHead->AdmgEndAddr                 = (UINT32)(DebugMessageBuffer + sizeof(DEBUG_MESSAGE_HEAD_STRUCTURE));    
  DebugMessageHead->AdmgSize                    = (UINT32)(TableSize - sizeof(DEBUG_MESSAGE_HEAD_STRUCTURE));

  mDebugMessageEn = TRUE;

  //
  // Copy DEBUG messages which stored in old memory space.
  //
  OldDebugMsgSize = (OldDebugMessageHead->AdmgEndAddr - OldDebugMessageHead->AdmgStartAddr);
  CopyMem ((VOID*)(UINTN)(DebugMessageHead->AdmgStartAddr), (VOID*)(UINTN)(OldDebugMessageHead->AdmgStartAddr), OldDebugMsgSize);
  DebugMessageHead->AdmgEndAddr += OldDebugMsgSize;
    
  Status = gBS->FreePages ((EFI_PHYSICAL_ADDRESS) OldDebugMessageHead, EFI_SIZE_TO_PAGES(PcdGet32(PcdStoreDebugMsgSize)));
  //
  // Set the value to "DebugMsg" variable.
  //
  DebugMessageStructure.DumpDebugMessageSignature = DUMP_DEBUG_MESSAGE_INSTANCE_SIGNATURE;
  DebugMessageStructure.DumpDebugMessageHeadAddr  = (PcdGet32(PcdStoreDebugMsgMemAddress));
  
  Status = gRT->SetVariable (
                  L"DebugMsg",
                  &gH2ODebugMessageVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (DEBUG_MESSAGE_EF_SEGMENT_STRUCTURE),
                  &DebugMessageStructure
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}
//[-end-140512-IB08400265-add]//

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

  If Data is NULL, then ASSERT().
  If Filename is NULL, then ASSERT().
  If Description is NULL, then ASSERT().
  If LineNumber is NULL, then ASSERT().

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

  If Data is NULL, then ASSERT().
  If ErrorLevel is NULL, then ASSERT().
  If Marker is NULL, then ASSERT().
  If Format is NULL, then ASSERT().

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
  IN UINTN                                 StringSize
  )  
{
  DEBUG_MESSAGE_HEAD_STRUCTURE            *DebugMessageHead = NULL;
  CHAR8                                   *StoreAddr;
//[-start-140512-IB08400265-add]//
  EFI_STATUS                              Status;
//[-end-140512-IB08400265-add]//

//[-start-140512-IB08400265-modify]//
  if (PcdGet32(PcdStoreDebugMsgMemAddress) == 0xFFFFFFFF) {
    return EFI_UNSUPPORTED;
  }
  DebugMessageHead = (DEBUG_MESSAGE_HEAD_STRUCTURE *)(UINTN)(PcdGet32(PcdStoreDebugMsgMemAddress));

  if ((DebugMessageHead->AdmgEndAddr + StringSize) > (DebugMessageHead->AdmgStartAddr + DebugMessageHead->AdmgSize)) {
    //
    // Reallocate the memory space to allowed more DEBUG messages.
    //
    Status = ResizeMemoryBuffer();
    if (!EFI_ERROR (Status)) {
      DebugMessageHead = (DEBUG_MESSAGE_HEAD_STRUCTURE *)(UINTN)(PcdGet32(PcdStoreDebugMsgMemAddress));
    } else {
      //
      // The total reserved memory space is not available for more DEBUG messages.
      //
      mDebugMessageEn = FALSE;
      return EFI_UNSUPPORTED;
    }
  }
//[-end-140512-IB08400265-modify]//
  StoreAddr = (CHAR8 *)(UINTN)DebugMessageHead->AdmgEndAddr;
  CopyMem (StoreAddr, MessageString, StringSize);
  DebugMessageHead->AdmgEndAddr += (UINT32)StringSize;

  return EFI_SUCCESS;
}

/**
 This function is a handler for OEM/ODM to create a specific function to handle DEBUG message.         
 
  @param[in] CodeType             Indicates the type of status code being reported.
  @param[in] CodeValue            Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
DebugMessageStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ) 
{  
  EFI_STATUS      Status;
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[MAX_DEBUG_MESSAGE_LENGTH];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINTN           CharCount;
  BASE_LIST       Marker;

  //
  // The service is not available, just return.
  //
  if (mDebugMessageEn ==  FALSE) {
    return EFI_UNSUPPORTED;
  }

//[-start-140326-IB08400258-add]//
  Buffer[0] = '\0';

  if (Data != NULL && ExtractAssertInfo (CodeType, CodeValue, Data, &Filename, &Description, &LineNumber)) {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rDXE_ASSERT!: %a (%d): %a\n\r",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if (Data != NULL && ExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
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
//[-end-140326-IB08400258-add]//

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
 
 Allocate a memory buffer for store DEBUG messages.
 And inital DEBUG Message Head information to this memory.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
DebugMessageMemBufferInit (
  VOID
  )    
{    
//[-start-140512-IB08400265-modify]//
  if (PcdGet32(PcdStoreDebugMsgMemAddress) == 0xFFFFFFFF) {
    return EFI_UNSUPPORTED;
  }
  
  mDebugMessageEn = TRUE;
  
  return EFI_SUCCESS;
//[-end-140512-IB08400265-modify]//
}

/**
  Notification function for ReportStatusCode Handler Protocol

  This routine is the notification function for EFI_RSC_HANDLER_PROTOCOL

  @param[in]         Event                
  @param[in]         Context                           
  
  @retval VOID                

**/
EFI_STATUS
EFIAPI
RscHandlerProtocolCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  EFI_STATUS                        Status;

  Status = gSmst->SmmLocateProtocol (
                                 &gEfiSmmRscHandlerProtocolGuid,
                                 NULL,
                                 (VOID **) &mRscHandlerProtocol
                                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Register the worker function to ReportStatusCodeRouter
  //
  Status = mRscHandlerProtocol->Register (DebugMessageStatusCode);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
//[-start-140512-IB08400265-modify]//
  return Status;
//[-end-140512-IB08400265-modify]//
}

/**
 The DEBUG Message driver will handle all events during DXE, SMM and BDS phase.
 The DEBUG messages came from Status Code reported.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StoreDebugMessageSmmEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  ) 
{
  EFI_STATUS                                Status;
  EFI_EVENT                                 RscHandlerProtocolEvent;

  //
  // Initialize the memory space for DEBUG messages.
  //
  Status = DebugMessageMemBufferInit ();
  if (EFI_ERROR (Status)) {
    return Status;
  }    

  Status = gSmst->SmmLocateProtocol (
                                 &gEfiSmmRscHandlerProtocolGuid,
                                 NULL,
                                 (VOID **) &mRscHandlerProtocol
                                 );
  if (EFI_ERROR (Status)) {
    //
    // Register callback for loading Event storage driver.
    //
    Status = gSmst->SmmRegisterProtocolNotify (
                                         &gEfiSmmRscHandlerProtocolGuid,
                                         RscHandlerProtocolCallback,
                                         &RscHandlerProtocolEvent
                                         );
  } else {
//[-start-140512-IB08400265-modify]//
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = mRscHandlerProtocol->Register (DebugMessageStatusCode);
    if (EFI_ERROR (Status)) {
      return Status;
    }
	
//[-end-140512-IB08400265-modify]//
  }
  
  return Status;
}
