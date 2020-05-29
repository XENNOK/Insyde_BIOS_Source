/** @file

  POST Message Dxe implementation.

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

#include <PostMessageDxe.h>

EFI_PHYSICAL_ADDRESS                  gPostMessageBuffer;  
EFI_PHYSICAL_ADDRESS                  gPostMsgStatusCodeBuffer=0xFFFFFFFF;  
BOOLEAN                               gPostMessageEn=FALSE;
BOOLEAN                               gProgressCodeEn=FALSE;
BOOLEAN                               gErrorCodeEn=FALSE;
BOOLEAN                               gDebugCodeEn=FALSE;
BOOLEAN                               gLogPostMsg=FALSE;
BOOLEAN                               gShowPostMsg=FALSE;
BOOLEAN                               gBeepPostMsg=FALSE;
BOOLEAN                               gStalltoShow=FALSE;
BOOLEAN                               gSupportPostMessageFun=FALSE;
BOOLEAN                               gOemHookOnly=FALSE;

extern POST_MESSAGE_LIST              gPostMessageList[];
extern OEM_POST_MESSAGE_STRING        gOemPostMessageStr[];
extern OEM_POST_MESSAGE_LOG           gOemPostMessageLog[];

extern UINTN                          gPostMessageListSize;
extern UINTN                          gOemPostMessageStrSize;
extern UINTN                          gOemPostMessageLogSize;

H2O_EVENT_LOG_STRING_PROTOCOL         gEventLogString = {
  PostMessageStringHandler
  };

H2O_POST_MESSAGE_PROTOCOL            gPostMessageProtocol = {
  PostMessageStatusCode
  };

EFI_RSC_HANDLER_PROTOCOL              *mRscHandlerProtocol   = NULL;
EFI_EVENT                             mExitBootServicesEvent = NULL;

/**
 
 Get "PeiPostMessage" HOBs       
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
CollectPeiPostMessageHob (
  VOID
  )
{  
  EFI_STATUS                           Status;
  VOID                                 *HobList;
  VOID                                 *Buffer;
  PEI_POST_MESSAGE_DATA_HOB            *PeiPostMessageHob;

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (;;) {
    Buffer = GetNextGuidHob (&gH2OPeiPostMessageHobGuid, HobList);
    if (Buffer == NULL) {
      break;
    }
    
    PeiPostMessageHob = (PEI_POST_MESSAGE_DATA_HOB *)((UINT8 *)Buffer + sizeof (EFI_HOB_GUID_TYPE));
    PostMessageHobHandleHook (PeiPostMessageHob);
    HobList = GET_NEXT_HOB (Buffer);
  }

  return EFI_SUCCESS;
}

/**
 Notification function when Variable Architecture Protocol Installed

 @param[in]  Event   - The Event that is being processed
 @param[in]  Context - Event Context         
 
 @retval EFI Status                  
*/
VOID
EFIAPI
InstalledVarNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{  
  EFI_STATUS                      Status;

  //
  // Store "POST Message Memory Head Address" for SMM driver to refer.
  //
  Status = gRT->SetVariable (
                      L"PostMsgStatusCode",
                      &gH2OPostMessageVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      sizeof (EFI_PHYSICAL_ADDRESS),
                      &gPostMsgStatusCodeBuffer
                      );
  if (EFI_ERROR (Status)) {
    //
    // PASS. Don't care
    //
  }

  gBS->CloseEvent (Event);

  return;

}

/**
 Get F-Segment Table Address

 @param[in]         Size - Require Size          
 
 @retval EFI Status                  
*/
UINT32
EFIAPI
GetFSegmentTableAddr (
  IN  UINT16      Size
  ) 
{
  EFI_STATUS                  Status;
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
  EFI_LEGACY_REGION_PROTOCOL  *LegacyRegion;
  EFI_IA32_REGISTER_SET       Regs;
  EFI_COMPATIBILITY16_TABLE   *Table;
  UINT32                      BufferAddr;
  UINT8                       *Ptr;
  
  Table = NULL;
  BufferAddr = 0;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if(EFI_ERROR (Status)) {
    return 0;
  }

  Status = gBS->LocateProtocol(&gEfiLegacyRegionProtocolGuid, NULL, (VOID **)&LegacyRegion);
  if(EFI_ERROR (Status)) {
    return 0;
  }

  for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *) ((UINTN) 0x100000); Ptr += 0x10) {
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      Table = (EFI_COMPATIBILITY16_TABLE *) Ptr;
     break;
    }
  }

  LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));  // aligned in 0x10 boundry
  Regs.X.AX = 0x06;                                    // Legacy16GetTableAddress    allocate legacy memory
  Regs.X.BX = 0x01;                                    // memory resident in F000 segment
  Regs.X.CX = (UINT16) Size;                           // allocate size
  Status = LegacyBios->FarCall86 (
                              LegacyBios,
                              Table->Compatibility16CallSegment,
                              Table->Compatibility16CallOffset,
                              &Regs,
                              NULL,
                              0
                              );

  BufferAddr = (UINT32) (Regs.X.DS *16 + Regs.X.BX);

  LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);

  return BufferAddr;
}

/**
 Publish Post Message Signature to F-Segment       

 @param[in]  Event   - The Event that is being processed
 @param[in]  Context - Event Context
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PublishPostMessageSignature (
  VOID
  ) 
{
  EFI_STATUS                              Status;
  POST_MESSAGE_F_SEGMENT_STRUCTURE       *PostMessageEntry;
  EFI_LEGACY_REGION_PROTOCOL              *LegacyRegion;

  Status = gBS->LocateProtocol(&gEfiLegacyRegionProtocolGuid, NULL, (VOID **)&LegacyRegion);
  if(EFI_ERROR (Status))
    return Status;

  //
  // 1. Create a "_EMSG_" signature on F-Segment to point prior allocated reserved memory area(64Kb).
  // 
  PostMessageEntry = 0;
  PostMessageEntry = (POST_MESSAGE_F_SEGMENT_STRUCTURE *)(UINTN)GetFSegmentTableAddr (sizeof(POST_MESSAGE_F_SEGMENT_STRUCTURE));
  if(PostMessageEntry != 0) {
    LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);
    PostMessageEntry->PostMessageSignature  = POST_MESSAGE_INSTANCE_SIGNATURE;
    PostMessageEntry->PostMessageHeadAddr   = gPostMessageBuffer;
    LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);
  }
  
  return EFI_SUCCESS;
}

/**
 1. Scan others Devices.
 2. Show Post Message.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PublishPostMessageTable (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )   
{
  //
  // 1. Scan others device for OEM requirement.
  //    IT NEED TO CALL BEFORE CALL "StorePostMessage()".
  //
  ScanOtherDevices ();

  //
  // 2. Store Post message from linking list node
  //
  StorePostMessage ();
  
  //
  // 3. Call Hook function to do OEM/ODM requirement.
  //
  PublishPostMessageTableHook ();

  //
  // 4. Create a signature in F-Segment for user or tool to find all Post messages which is in reserved memory area.
  //
  PublishPostMessageSignature ();

  //
  // 5. Close Evnet
  //
  gBS->CloseEvent(Event);  
  gSupportPostMessageFun= FALSE;
  
  return EFI_SUCCESS;
}

/**
 
 This function will check the CodeType and CodeValue of the message 
 to see if the message need to store to Reserved memory.     
 
 @retval Boolean                  
*/
BOOLEAN
EFIAPI
CheckStoreMsg (
  EFI_STATUS_CODE_TYPE           CodeType,
  EFI_STATUS_CODE_VALUE          CodeValue
  )  
{
  UINTN                          Index;

  for (Index = 0; Index < gPostMessageListSize; Index++) {
    if ((gPostMessageList[Index].CodeType == CodeType) && (gPostMessageList[Index].CodeValue == CodeValue)) {
      //
      // Match the message, then check the behavior bitmap.
      //
      if ((gPostMessageList[Index].BehaviorBitMap & BEHAVIOR_STORE_MESSAGE) == BEHAVIOR_STORE_MESSAGE) {
        // Store behavior is needed.
        return TRUE;
      } else {
        return FALSE;
      }
    }  
  }

  //
  // Not match the message.
  //
  return FALSE;
}

/**
 
 This function will copy all Post messages from temp buffer to reserved memory area.
 The reserved memory has two separated areas, First is for ASCII code, other is for Unicode.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StorePostMessage (
  VOID
  )    
{  
  UINTN                                                 Index;
  TEMP_BUFFER_HEAD_STRUCTURE                            *TempBufferHead=NULL;
  TEMP_BUFFER_STRUCTURE                                 *TempBuffer;
  BOOLEAN                                               StoreMsg;
  
  if ((gPostMsgStatusCodeBuffer == 0xFFFFFFFF) || (gPostMsgStatusCodeBuffer == 0)) {
    //
    // No Post Storage can use.
    //
    return EFI_UNSUPPORTED;
  }

  TempBufferHead = (TEMP_BUFFER_HEAD_STRUCTURE *)(UINTN)gPostMsgStatusCodeBuffer;
  TempBuffer     = (TEMP_BUFFER_STRUCTURE *)(UINTN) TempBufferHead->TempStartAddr;

  do {
    if (TempBuffer->CodeType != 0) {
      //
      // Check if the message data in TempBuffer need to be stored.
      //
      StoreMsg = FALSE;
      StoreMsg = CheckStoreMsg (TempBuffer->CodeType, TempBuffer->CodeValue);
      
      if (StoreMsg == TRUE) {
        for (Index = 0; Index < gOemPostMessageStrSize; Index++) {
          if ((TempBuffer->CodeType == gOemPostMessageStr[Index].CodeType) && (TempBuffer->CodeValue == gOemPostMessageStr[Index].CodeValue)) {
            //
            // First reserved memory area is for ASCII code
            //
            StoreAsciiPostMessage (gOemPostMessageStr[Index].CodeType, gOemPostMessageStr[Index].CodeValue, gOemPostMessageStr[Index].CodeString);
            //
            // Second reserved memory area is for Unicode
            //
            StoreUnicodePostMessage (gOemPostMessageStr[Index].CodeType, gOemPostMessageStr[Index].CodeValue, gOemPostMessageStr[Index].CodeString);
          }
        }
      }
      
      TempBuffer++;
    }
  } while ((TempBuffer->CodeType !=0) || ((UINTN)TempBuffer < (UINTN)TempBufferHead->TempEndAddr));

  //
  // 1. Set TempSignature=0 to identify TempBuffer memory space was freed.
  // 2. Free TempBuffer memory space.
  // 3. Disable Callback function
  //
  TempBufferHead->TempSignature = 0;
  gBS->FreePages (gPostMsgStatusCodeBuffer, EFI_SIZE_TO_PAGES(POST_MESSAGE_SIZE));
  gSupportPostMessageFun= FALSE;
  
  return EFI_SUCCESS;
}

/**
 
 This function is a handler for BIOS to handle Post message.

 (See Tiano Runtime Specification)           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ) 
{
  EFI_STATUS                  Status;
  
  if (gPostMessageEn==FALSE) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // 1. OEM/ODM first - Call PostMessageStatusCodeHook() for OEM/ODM to handle event
  //
  Status = PostMessageStatusCodeHook (CodeType, CodeValue, Instance, CallerId, Data);
  if (Status == EFI_ABORTED) {
    //
    // OEM/ODM hook function only.
    // Return EFI_SUCCESS after finishing Hook function.
    //
    return EFI_SUCCESS;
  }
  
  //
  // 2. Log Post message to Temp buffer in memory area for future use.
  //
  if (gSupportPostMessageFun) {
    LogPostMessage (CodeType, CodeValue);
  }
      
  return EFI_SUCCESS;
}

/**

 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogPostMessage (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue
  )   
{
  TEMP_BUFFER_HEAD_STRUCTURE                *TempBufferHead=NULL;
    
  if ((gPostMsgStatusCodeBuffer == 0xFFFFFFFF) || (gPostMsgStatusCodeBuffer == 0)) {
    //
    // No Post msg Storage can use.
    //
    return EFI_UNSUPPORTED;
  }

  TempBufferHead = (TEMP_BUFFER_HEAD_STRUCTURE *)(UINTN)gPostMsgStatusCodeBuffer;

  if (TempBufferHead->TempSignature != MESG_TEMP_INSTANCE_SIGNATURE) {
    //
    // Tempbuffer memory space don't exist.
    //
    gSupportPostMessageFun=FALSE;
    gPostMsgStatusCodeBuffer=0;
    return EFI_UNSUPPORTED;
  }
  
  if ((TempBufferHead->TempEndAddr+sizeof(TEMP_BUFFER_STRUCTURE)) > 
      (TempBufferHead->TempStartAddr + TempBufferHead->TempSize)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  gBS->CopyMem ((VOID *)(UINTN)TempBufferHead->TempEndAddr, &CodeType, sizeof(EFI_STATUS_CODE_TYPE));
  TempBufferHead->TempEndAddr += sizeof (EFI_STATUS_CODE_TYPE);
  gBS->CopyMem ((VOID *)(UINTN)TempBufferHead->TempEndAddr, &CodeValue, sizeof(EFI_STATUS_CODE_VALUE));
  TempBufferHead->TempEndAddr += sizeof (EFI_STATUS_CODE_VALUE); 

  return EFI_SUCCESS;
}


/**         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StoreAsciiPostMessage (
  IN EFI_STATUS_CODE_TYPE                   CodeType,
  IN EFI_STATUS_CODE_VALUE                  CodeValue,
  IN CHAR16                                 *EventString
  )  
{
  
  POST_MESSAGE_HEAD_STRUCTURE            *PostMessageHead=NULL;
  UINTN                                   StringSize;
  CHAR8                                   *StoreAddr;

  if (((CodeType == EFI_PROGRESS_CODE) && (gProgressCodeEn == FALSE))  ||
    ((CodeType == EFI_ERROR_CODE)      && (gErrorCodeEn    == FALSE))  ||
    ((CodeType == EFI_DEBUG_CODE)      && (gDebugCodeEn    == FALSE))) {  
    return EFI_UNSUPPORTED;
  }
    
  if (gPostMessageBuffer == 0xFFFFFFFF) {
    //
    // No Event Storage can use.
    //
    return EFI_UNSUPPORTED;
  }

  PostMessageHead = (POST_MESSAGE_HEAD_STRUCTURE *)(UINTN)gPostMessageBuffer;
  StringSize = StrLen (EventString);

  if ((PostMessageHead->ApmEndAddr+StringSize) > (PostMessageHead->ApmStartAddr + PostMessageHead->ApmSize)) {
    return EFI_OUT_OF_RESOURCES;
  }
  StoreAddr = (CHAR8 *)(UINTN)PostMessageHead->ApmEndAddr;
  UnicodeStrToAsciiStr (EventString, StoreAddr);
  PostMessageHead->ApmEndAddr += (UINT32)StringSize;
  StoreAddr = (CHAR8 *)(UINTN)PostMessageHead->ApmEndAddr;
  UnicodeStrToAsciiStr (L"\r\n", StoreAddr);
  PostMessageHead->ApmEndAddr += 2;

  return EFI_SUCCESS;
}


/**        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StoreUnicodePostMessage (
  IN EFI_STATUS_CODE_TYPE                   CodeType,
  IN EFI_STATUS_CODE_VALUE                  CodeValue,
  IN CHAR16                                 *EventString
  )  
{
  POST_MESSAGE_HEAD_STRUCTURE            *PostMessageHead=NULL;
  UINTN                                   StringSize;
  CHAR16                                  *StoreAddr;

  if (gShowPostMsg==FALSE) {
    return EFI_UNSUPPORTED;
  }
  
  if (((CodeType == EFI_PROGRESS_CODE) && (gProgressCodeEn == FALSE))  ||
    ((CodeType == EFI_ERROR_CODE)      && (gErrorCodeEn    == FALSE))  ||
    ((CodeType == EFI_DEBUG_CODE)      && (gDebugCodeEn    == FALSE))) {  
    return EFI_UNSUPPORTED;
  }

  if (gPostMessageBuffer == 0xFFFFFFFF) {
    //
    // No Event Storage can use.
    //
    return EFI_UNSUPPORTED;
  }

  PostMessageHead = (POST_MESSAGE_HEAD_STRUCTURE *)(UINTN)gPostMessageBuffer;
  StringSize = StrLen (EventString) * 2;

  if ((PostMessageHead->UpmEndAddr+StringSize) > (PostMessageHead->UpmStartAddr + PostMessageHead->UpmSize)) {
    return EFI_OUT_OF_RESOURCES;
  }
  StoreAddr = (CHAR16 *)(UINTN)PostMessageHead->UpmEndAddr;
  StrCat (StoreAddr, EventString);
  PostMessageHead->UpmEndAddr += (UINT32)StringSize;
  StoreAddr = (CHAR16 *)(UINTN)PostMessageHead->UpmEndAddr;  
  StrCat (StoreAddr, L"\r\n");
  PostMessageHead->UpmEndAddr += 4;

  return EFI_SUCCESS;
}

/**
 Provide Post Message relatived string.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageStringHandler (
  IN VOID                                   *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  )    
{
  UINTN                                   Index;
  STORAGE_EVENT_LOG_INFO                  *StorageEventInfo;
  VOID                                    *TempString=NULL;

  StorageEventInfo = (STORAGE_EVENT_LOG_INFO *)EventInfo; 
  for (Index = 0; Index < gOemPostMessageLogSize; Index++) {
    if ((gOemPostMessageLog[Index].EventID.SensorType == StorageEventInfo->EventID.SensorType) &&
        (gOemPostMessageLog[Index].EventID.SensorNum  == StorageEventInfo->EventID.SensorNum ) &&
        (gOemPostMessageLog[Index].EventID.EventType  == StorageEventInfo->EventID.EventType ) &&
        (gOemPostMessageLog[Index].EventData[0]       == StorageEventInfo->Data[0]           ) &&
        (gOemPostMessageLog[Index].EventData[1]       == StorageEventInfo->Data[1]           ) &&
        (gOemPostMessageLog[Index].EventData[2]       == StorageEventInfo->Data[2]           )
        ) {
      //
      // We got it. Return relatived string
      //
      *StringSize = (StrLen (gOemPostMessageLog[Index].CodeString) * 2)+2;
      gBS->AllocatePool (EfiBootServicesData, *StringSize, (VOID **)&TempString);
      StrCpy(TempString, gOemPostMessageLog[Index].CodeString);
      
      *EventString = TempString;
      return EFI_SUCCESS;
    }
  }
  
  return EFI_NOT_FOUND;
}

/**
 
 Allocate a EfiReservedMemoryType type of memory buffer for store event.
 And inital Post Message Head information to this memory.
         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
InitialPostMessageMemBuffer (
  VOID
  )    
{    
  EFI_STATUS                                Status;
  POST_MESSAGE_HEAD_STRUCTURE              *PostMessageHead=NULL;
  UINTN                                     TableSize;  

  //
  // Allocate reserved memory for Memory Storage.
  //
  gPostMessageBuffer = 0xFFFFFFFF;
  TableSize = POST_MESSAGE_SIZE;
  Status = gBS->AllocatePages (
                           AllocateMaxAddress,
                           EfiReservedMemoryType,
                           EFI_SIZE_TO_PAGES(TableSize),
                           &gPostMessageBuffer
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Store Post Message Head address and add signature '_EMSGE_', Entry/End Address and total size information.
  //
  PostMessageHead = (POST_MESSAGE_HEAD_STRUCTURE *)(UINTN)gPostMessageBuffer;
  ZeroMem ((VOID *)PostMessageHead, TableSize);
  PostMessageHead->UpmSignature                = MESG_UPM_INSTANCE_SIGNATURE;
  PostMessageHead->UpmStartAddr                = (UINT32)gPostMessageBuffer + sizeof(POST_MESSAGE_HEAD_STRUCTURE);    
  PostMessageHead->UpmEndAddr                  = (UINT32)gPostMessageBuffer + sizeof(POST_MESSAGE_HEAD_STRUCTURE);    
  PostMessageHead->UpmSize                     = (UINT32)(UNICODE_POST_MESSAGE_SIZE-sizeof(POST_MESSAGE_HEAD_STRUCTURE));

  PostMessageHead->ApmSignature                = MESG_APM_INSTANCE_SIGNATURE;
  PostMessageHead->ApmStartAddr                = (UINT32)gPostMessageBuffer + UNICODE_POST_MESSAGE_SIZE;    
  PostMessageHead->ApmEndAddr                  = (UINT32)gPostMessageBuffer + UNICODE_POST_MESSAGE_SIZE;    
  PostMessageHead->ApmSize                     = (UINT32)(ASCII_POST_MESSAGE_SIZE);

  return EFI_SUCCESS;
}

/**
  Notification function for ReportStatusCode Handler Protocol

  This routine is the notification function for EFI_RSC_HANDLER_PROTOCOL

  @param[in]         Event                
  @param[in]         Context                           
  
  @retval VOID                

**/
static
VOID
EFIAPI
RscHandlerProtocolCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (
                            &gEfiRscHandlerProtocolGuid,
                            NULL,
                            (VOID **) &mRscHandlerProtocol
                            );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Register the worker function to ReportStatusCodeRouter
  //
  Status = mRscHandlerProtocol->Register (PostMessageStatusCode, TPL_HIGH_LEVEL);
  
  ASSERT_EFI_ERROR (Status);

  return;
}

/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
   if (mRscHandlerProtocol != NULL) {
     mRscHandlerProtocol->Unregister (PostMessageStatusCode);
   }
}

/**
 
 Allocate a EfiReservedMemoryType type of memory buffer for store event.
 And inital Post Message Head information to this memory.       
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
InitialPostMessageTempMemBuffer (
  VOID
  )  
{    
  EFI_STATUS                                Status;
  TEMP_BUFFER_HEAD_STRUCTURE                *TempBufferHead=NULL;
  UINTN                                     TableSize;  

  //
  // Allocate 64Kb temp buffer for DXE and SMM driver to log evnet message informaton.
  //
  gPostMsgStatusCodeBuffer = 0xFFFFFFFF;
  TableSize = POST_MESSAGE_SIZE;
  Status = gBS->AllocatePages (
                           AllocateMaxAddress,
                           EfiBootServicesData,
                           EFI_SIZE_TO_PAGES(TableSize),
                           &gPostMsgStatusCodeBuffer
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  TempBufferHead = (TEMP_BUFFER_HEAD_STRUCTURE *)(UINTN)gPostMsgStatusCodeBuffer;
  ZeroMem ((VOID *)TempBufferHead, TableSize);
  TempBufferHead->TempSignature                 = MESG_TEMP_INSTANCE_SIGNATURE;
  TempBufferHead->TempStartAddr                 = (UINT32)gPostMsgStatusCodeBuffer + sizeof(TEMP_BUFFER_HEAD_STRUCTURE);
  TempBufferHead->TempEndAddr                   = (UINT32)gPostMsgStatusCodeBuffer + sizeof(TEMP_BUFFER_HEAD_STRUCTURE);
  TempBufferHead->TempSize                      = (UINT32)(POST_MESSAGE_SIZE-sizeof(TEMP_BUFFER_HEAD_STRUCTURE)); 


  return EFI_SUCCESS;
}

/**
 The Post Message driver will handle all events during DXE and BDS phase.
 The events are come from Status Code reported.
 Defaulted setting the event will show on screen after BIOS POST.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageDxeEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  ) 
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                Handle;  
  VOID                                      *EventRegistration;
  EFI_EVENT                                 ReadyToBootEvent;
  EFI_EVENT                                 VarEvent;
  EFI_EVENT                                 RscHandlerProtocolEvent;

  if (PcdGetBool(PcdPostMessageEn) == FALSE) {
    return EFI_UNSUPPORTED;
  }
  
  Handle = NULL; 
  Status = InitialPostMessageMemBuffer ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = InitialPostMessageTempMemBuffer ();
  if (EFI_ERROR (Status)) {
    return Status;
  } 

  gPostMessageEn      = (BOOLEAN)PcdGetBool(PcdPostMessageEn);
  gProgressCodeEn     = (BOOLEAN)PcdGetBool(PcdPostMessageProgressCode);
  gErrorCodeEn        = (BOOLEAN)PcdGetBool(PcdPostMessageErrorCode);
  gDebugCodeEn        = (BOOLEAN)PcdGetBool(PcdPostMessageDebugCode);
  gLogPostMsg         = (BOOLEAN)PcdGetBool(PcdPostMessageLogPostMsg);
  gShowPostMsg        = (BOOLEAN)PcdGetBool(PcdPostMessageShowPostMsg);
  gBeepPostMsg        = (BOOLEAN)PcdGetBool(PcdPostMessageBeepPostMsg);
    
  //
  // Install Post message log protocol for user to log Post message
  //
  Status = gBS->InstallProtocolInterface (
                                      &Handle,
                                      &gH2OPostMessageProtocolGuid,
                                      EFI_NATIVE_INTERFACE,
                                      &gPostMessageProtocol
                                      );
  
  if (!EFI_ERROR (Status)) {
    //
    // Install Event Log String Protocol
    //
    Status = gBS->InstallProtocolInterface (
                                        &Handle,
                                        &gH2OEventLogStringProtocolGuid,
                                        EFI_NATIVE_INTERFACE,
                                        &gEventLogString
                                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    //
    // Create ready to boot event, to publish Post Message Table
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               PublishPostMessageTable,
               NULL,
               &ReadyToBootEvent
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Create an evnet - Wait for Variable Write Architecture Protocol
    //
    Status = gBS->CreateEvent (
                           EVT_NOTIFY_SIGNAL,
                           TPL_CALLBACK,
                           InstalledVarNotificationFunction,
                           NULL,
                           &VarEvent
                           );
    if (!EFI_ERROR (Status)) {
      gBS->RegisterProtocolNotify (
                               &gEfiVariableWriteArchProtocolGuid,
                               VarEvent,
                               &EventRegistration
                               );
    }      

    Status = gBS->LocateProtocol (
                              &gEfiRscHandlerProtocolGuid,
                              NULL,
                              (VOID **) &mRscHandlerProtocol
                              );
    if (EFI_ERROR (Status)) {
      //
      // Register callback for loading Event storage driver.
      //
      Status = gBS->CreateEventEx (
                               EVT_NOTIFY_SIGNAL,
                               TPL_NOTIFY,
                               RscHandlerProtocolCallback,
                               NULL,
                               &gEfiRscHandlerProtocolGuid,
                               &RscHandlerProtocolEvent
                               );
      
    } else {
      //
      // Register the worker function to ReportStatusCodeRouter
      //
      Status = mRscHandlerProtocol->Register (PostMessageStatusCode, TPL_HIGH_LEVEL);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    Status = gBS->CreateEventEx (
                             EVT_NOTIFY_SIGNAL,
                             TPL_NOTIFY,
                             UnregisterBootTimeHandlers,
                             NULL,
                             &gEfiEventExitBootServicesGuid,
                             &mExitBootServicesEvent
                             );
    
  }

  if (Status == EFI_SUCCESS) {
    gSupportPostMessageFun = TRUE;
    //
    // Handle Post Message Hob
    //
    CollectPeiPostMessageHob ();
  }

  return Status;
}
