/** @file

  BIOS Storage kernel Dxe implementation.

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


#include <BiosStorageKernelDxe.h>

EFI_EVENT_LOG_INSTANCE                *ELPrivate;
EFI_EVENT_LOG_INSTANCE                *SmmELPrivate;
EFI_SMM_SYSTEM_TABLE2                 *gSmst=NULL;

LIST_ENTRY                            mBiosStorageKernelList;
BOOLEAN                               mClearBiosStorageKernel;
BOOLEAN                               mSwitchToSmm = FALSE; // Switch the GPNV region access to SMM driver.

extern EFI_RUNTIME_SERVICES           *gRT;
extern BOOLEAN                        mSpiDeviceType;
extern SPI_CONFIG_BLOCK               mSpiConfigBlock;

EFI_EVENT                             mVarWriteEvent;
/**
 
 Write event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
 Event log will temporarily be stored in allocated pool and be recorded in our temp event log list.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EmuWriteEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_DATA                    *EventLog;
  UINTN                                 DataFormatTypeLength;
  UINTN                                 NumBytes;
  
  Status = gBS->AllocatePool (
                  EfiBootServicesData, 
                  sizeof (EFI_EVENT_LOG_DATA) ,
                  (VOID **)&EventLog
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  EventLog->Signature = EVENT_LOG_DATA_SIGNATURE;

  DataFormatTypeLength = 0;
  switch (EventLogType) {

  case EfiEventLogTypePostError:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH;
    break;

  default:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_NONE_LENGTH;
    break;
  }
  NumBytes = BIOS_EVENT_LOG_BASE_LENGTH + DataFormatTypeLength + OptionDataSize;

  Status = gBS->AllocatePool (
                  EfiBootServicesData, 
                  NumBytes,
                  (VOID **)&(EventLog->Buffer)
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (EventLog);
    return Status;
  }

  ZeroMem (EventLog->Buffer, NumBytes);

  EventLog->Buffer->Year   = ReadCmos8(0x09);
  EventLog->Buffer->Month  = ReadCmos8(0x08);
  EventLog->Buffer->Day    = ReadCmos8(0x07);
  EventLog->Buffer->Hour   = ReadCmos8(0x04);
  EventLog->Buffer->Minute = ReadCmos8(0x02);
  EventLog->Buffer->Second = ReadCmos8(0x00);
  EventLog->Buffer->Type   = EventLogType;
  EventLog->Buffer->Length = (UINT8) NumBytes;

  if (EventLogType == EfiEventLogTypePostError) {
    *(UINT32 *) (EventLog->Buffer->Data)             = PostBitmap1;
    *(UINT32 *) (UINTN) (&EventLog->Buffer->Data[4]) = PostBitmap2;
  }

  if (OptionLogData != NULL) {
    CopyMem ((UINT8*) ((UINTN) EventLog->Buffer + NumBytes - OptionDataSize), OptionLogData, OptionDataSize);
  }

  InsertTailList (&mBiosStorageKernelList, &EventLog->Link);
  
  return EFI_SUCCESS;
}


/**
 
 Read next event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
 This function will search next event log from Hob list and our temp event log list.

 @param[in]         This                
 @param[in]         EventListAddress           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EmuReadNextEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN OUT VOID                           **EventListAddress
  )
{
  BOOLEAN                              GetFirstEventLog;
  EFI_STATUS                           Status;
  EFI_EVENT_LOG_DATA                   *EventLog;
  VOID                                 *HobList;
  BIOS_EVENT_LOG_ORGANIZATION          *HobLog;
  LIST_ENTRY                           *Link;

  GetFirstEventLog = FALSE;

  //
  // First, read event log in Hob list which is created during PEI phase.
  // But if perform clear event log at this POST time, there is no need to read event log from Hob list.
  //
  if (!mClearBiosStorageKernel) {
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (*EventListAddress == NULL) {
      HobLog = GetNextGuidHob (&gBiosEventLogHobGuid, HobList);
      if (HobLog != NULL) {
        *EventListAddress = (VOID *)(HobLog + sizeof (EFI_HOB_GUID_TYPE));
        return EFI_SUCCESS;
      }
    }

    for (;;) {
      HobLog = GetNextGuidHob (&gBiosEventLogHobGuid, HobList);
      if (HobLog == NULL) {
        break;
      }
      HobLog = (BIOS_EVENT_LOG_ORGANIZATION *)(HobLog + sizeof (EFI_HOB_GUID_TYPE));
      if (HobLog == (BIOS_EVENT_LOG_ORGANIZATION *) *EventListAddress) {
        HobLog = GetNextGuidHob (&gBiosEventLogHobGuid, HobList);
        if (HobLog == NULL) {
          GetFirstEventLog = TRUE;
          break;
        }
        *EventListAddress = (VOID *)(HobLog + sizeof (EFI_HOB_GUID_TYPE));
        return EFI_SUCCESS;
      }
    }
  }

  if (IsListEmpty (&mBiosStorageKernelList)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get first event log.
  //
  if (*EventListAddress == NULL || GetFirstEventLog) {
    Link = mBiosStorageKernelList.ForwardLink;
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    *EventListAddress = (VOID *) EventLog->Buffer;
    return EFI_SUCCESS;
  }

  //
  // Keep searching until match the input event log, then return next event log.
  //
  Link = mBiosStorageKernelList.ForwardLink;
  while (Link != &mBiosStorageKernelList) {
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    if (EventLog->Buffer == *EventListAddress) {
      Link = Link->ForwardLink;
      if (Link == &mBiosStorageKernelList) {
        return EFI_NOT_FOUND;
      }
      
      EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
      *EventListAddress = (VOID *) EventLog->Buffer;
      return EFI_SUCCESS;
    }
    
    Link = Link->ForwardLink;
  }

  return EFI_NOT_FOUND;
}

/**
 
 Clear event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
 It will set event logs, which are stored in Hob list, are invalid and clear our temp event log list.

 @param[in]         This                          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EmuClearEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL      *This
  )
{
  EFI_STATUS                           Status;
  LIST_ENTRY                           *Link;
  EFI_EVENT_LOG_DATA                   *EventLog;
  
  mClearBiosStorageKernel = TRUE;
  
  Link = mBiosStorageKernelList.ForwardLink;
  while (Link != &mBiosStorageKernelList) {
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    Link = Link->ForwardLink;
    gBS->FreePool (EventLog->Buffer);
    gBS->FreePool (EventLog);
  }
  InitializeListHead (&mBiosStorageKernelList);
  
  Status = This->Write (
                   This,
                   EfiEventLogTypeLogAreaResetCleared,
                   0,
                   0,
                   0,
                   NULL
                   );
  return Status;
}

/**
 Smart detect current flash device type

 @param[in]         This                
 @param[in]         Buffer           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
FlashFdDetectDevice (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *ELPrivate;

  ELPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);

  Status = GetFlashDevice (&(ELPrivate->DevicePtr));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Buffer != NULL) {
    CopyMem (Buffer, ELPrivate->DevicePtr, sizeof (FLASH_DEVICE));
  }
  
  return Status;
}


/**
 Get the GPNV map.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetGPNVMap (
  VOID
  )
{
  if (!PcdGet32 (PcdBiosEventStorageBase)) {
    return EFI_NOT_FOUND;
  }
  
  ELPrivate->GPNVBase   = PcdGet32 (PcdBiosEventStorageBase);
  ELPrivate->GPNVLength = PcdGet32 (PcdBiosEventStorageSize);

  ELPrivate->SpareBase = PcdGet32 (PcdBiosEventStorageBase);
  ELPrivate->SpareSize = PcdGet32 (PcdBiosEventStorageSize);

  return EFI_SUCCESS;

}

/**
 Get the HOB of Event log.

 @param[in]         This                          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetEventLogHob (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL      *This
  )
{
  VOID                                 *HobList;
  VOID                                 *Buffer;
  EFI_STATUS                           Status;
  BIOS_EVENT_LOG_ORGANIZATION          *HobLog;
  BIOS_EVENT_LOG_ORGANIZATION          *CurrentLog;
  EFI_EVENT_LOG_INSTANCE               *EPrivate;
  UINTN                                 Length;

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
    CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);

    if (((UINTN)CurrentLog > (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
        (UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase)) ||
        (CurrentLog->Length == 0)) {

      return EFI_SUCCESS;
    }
  }

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (;;) {
    Buffer = GetNextGuidHob (&gBiosEventLogHobGuid, HobList);
    if (Buffer == NULL) {
      break;
    }
    HobLog = (BIOS_EVENT_LOG_ORGANIZATION *)((BIOS_EVENT_LOG_ORGANIZATION *)Buffer + sizeof (EFI_HOB_GUID_TYPE));
    Length = (UINTN)HobLog->Length;
    Status = FlashProgram (
                          (UINT8 *)(UINTN)CurrentLog,
                          Buffer,
                          &Length,
                          (((UINTN)CurrentLog) & ~(0xFFFF))
                          );

    if (EFI_ERROR (Status) ) {
      return Status;
    }
    
    HobList = GET_NEXT_HOB (Buffer);

    CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *) ((UINT8*) CurrentLog + Length);
    if (((UINTN)CurrentLog > (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
        (UINTN)CurrentLog  < (UINTN)(EPrivate->GPNVBase)) ||
        (CurrentLog->Length == 0)) {
      return EFI_SUCCESS;
    }
  }

  return EFI_SUCCESS;
}

/**
 Fixup internal data pointers so that the services can be called in virtual mode.
          
 @param[in] Event     The event registered.
 @param[in] Context   Event context.
  
 @retval EFI Status                  
*/
VOID
EFIAPI
EventLogVirtualAddressChangeEvent (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  gBS->UninstallProtocolInterface (
                  ELPrivate->Handle,
                  &gEfiBiosEventLogProtocolGuid,
                  &ELPrivate->EventLogService
                  );

}

/**
 Initialize Flash device if flash device hasn't been initialized         
 
 @retval EFI Status                  
*/
VOID
EFIAPI
NonVolatileVariableNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                           Status;
  LIST_ENTRY                           *Link;
  BIOS_EVENT_LOG_ORGANIZATION          *CurrentLog;
  EFI_EVENT_LOG_DATA                   *EventLog;
  UINTN                                NumBytes;
  
  if (ELPrivate->DevicePtr == NULL) {
    Status = FlashFdDetectDevice (&ELPrivate->EventLogService, NULL);
    ASSERT_EFI_ERROR (Status);

    //
    // Before gEfiNonVolatileVariableProtocolGuid is installed, some drivers may write or clear event log.
    // It should write those event logs into flash device in here.
    // First write event log from Hob list, then write event log from out temp event log list.
    //
    if (mClearBiosStorageKernel) {
      EfiClearEventLog (&ELPrivate->EventLogService);
    } else {
      GetEventLogHob (&ELPrivate->EventLogService);
    }
    mClearBiosStorageKernel = FALSE;

    if (!IsListEmpty (&mBiosStorageKernelList)) {
      Link = mBiosStorageKernelList.ForwardLink;

      CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *) (UINTN) ELPrivate->GPNVBase;
      while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
        if (((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(ELPrivate->GPNVBase + ELPrivate->GPNVLength) ||
            (UINTN)CurrentLog< (UINTN)(ELPrivate->GPNVBase) ||
            (CurrentLog->Length == 0)){
          return;
        }
        CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);
      }
      
      while (Link != &mBiosStorageKernelList) {
        EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
        NumBytes = (UINTN) EventLog->Buffer->Length;
        
        Status = FlashProgram (
                              (UINT8 *)(UINTN)CurrentLog,
                              (UINT8 *) (UINTN) EventLog->Buffer,
                              &NumBytes,
                              (((UINTN)CurrentLog) & ~(0xFFFF))
                              );

        CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *) ((UINT8*) CurrentLog + NumBytes);
        if ((((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(ELPrivate->GPNVBase + ELPrivate->GPNVLength)) ||
            ((UINTN)CurrentLog < (UINTN)(ELPrivate->GPNVBase)) ||
            (CurrentLog->Length == 0)) {
          return;
        }
        
        Link = Link->ForwardLink;
        gBS->FreePool (EventLog->Buffer);
        gBS->FreePool (EventLog);
      }      

      InitializeListHead (&mBiosStorageKernelList);
    }
  }

  return;
}

/**
 Initialize Flash device if flash device hasn't been initialized         
 
 @retval EFI Status                  
*/
VOID
EFIAPI
SmmNonVolatileVariableNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS     Status;

  if (SmmELPrivate->DevicePtr == NULL) {
    Status = FlashFdDetectDevice (&SmmELPrivate->EventLogService, NULL);
    ASSERT_EFI_ERROR (Status);
  }
  
  return;
}

/**
 Entry point of this driver. Install Event Log protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosStorageKernelEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  EFI_EVENT                           Event;
  VOID                                *Registration;
  VOID                                *ProtocolInterface;
  EFI_EVENT                           BiosProtectReadyToBootEvent;
  VOID                                *VariableWriteEventRegistration; 

  //
  // Make sure the NV space is available for BIOS Storage.
  // 
  if ((!PcdGet32 (PcdBiosEventStorageBase)) || (!PcdGet32 (PcdBiosEventStorageSize))) {
    return EFI_UNSUPPORTED;
  }
  
  Event = 0;
  //
  // Make sure these global variables are initialized in both DXE and SMM.
  //
  mClearBiosStorageKernel = FALSE;
  InitializeListHead (&mBiosStorageKernelList);

  gRT = gST->RuntimeServices;
  
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_EVENT_LOG_INSTANCE),
                  (VOID **)&ELPrivate
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ELPrivate->Signature                      = EVENT_LOG_SIGNATURE;
  ELPrivate->DevicePtr                      = NULL;
  ELPrivate->GPNVBase                       = PcdGet32 (PcdBiosEventStorageBase);
  ELPrivate->GPNVLength                     = PcdGet32 (PcdBiosEventStorageSize);
  ELPrivate->EventLogService.Write          = EfiWriteEventLog;
  ELPrivate->EventLogService.Clear          = EfiClearEventLog;
  ELPrivate->EventLogService.ReadNext       = EfiReadNextEventLog;
  ELPrivate->EventLogService.DetectDevice   = FlashFdDetectDevice;
  ELPrivate->EventLogService.OverWrite      = EfiOverWriteEventLog;

  ELPrivate->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                                      &ELPrivate->Handle,
                                      &gEfiBiosEventLogProtocolGuid,
                                      EFI_NATIVE_INTERFACE,
                                      &(ELPrivate->EventLogService)
                                      );
  if (!EFI_ERROR (Status)) {
    if (ELPrivate->DevicePtr == NULL) {
      Status = FlashFdDetectDevice (&ELPrivate->EventLogService, NULL);
      ASSERT_EFI_ERROR (Status);
    }
    Status = gBS->LocateProtocol (&gEfiNonVolatileVariableProtocolGuid, NULL, (VOID **)&ProtocolInterface);
    if (EFI_ERROR (Status)) {
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      NonVolatileVariableNotifyFunction,
                      NULL,
                      &Event
                      );
      ASSERT_EFI_ERROR(Status);

      Status = gBS->RegisterProtocolNotify (
                      &gEfiNonVolatileVariableProtocolGuid,
                      Event,
                      &Registration
                      );
      ASSERT_EFI_ERROR(Status);
    } else {
      NonVolatileVariableNotifyFunction (Event, NULL);
    }

    //
    // Use ReadyToBootEvent to make sure BiosProtectEvent()
    // performed whether BIOS protection is enabled.
    //
    Status = EfiCreateEventReadyToBootEx (
                                          TPL_CALLBACK,
                                           BiosProtectEvent,
                                           NULL,
                                           &BiosProtectReadyToBootEvent
                                           );
    ASSERT_EFI_ERROR (Status);

    //
    // Create an evnet - Wait for Variable Write Architecture Protocol
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    InstalledVarWriteNotificationFunction,
                    NULL,
                    &mVarWriteEvent
                    );

    if (!EFI_ERROR (Status)) {
      gBS->RegisterProtocolNotify (
             &gEfiVariableWriteArchProtocolGuid,
             mVarWriteEvent,
             &VariableWriteEventRegistration
             );
    }
  }

  Status = GetEventLogHob (&ELPrivate->EventLogService);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Event Log Get Event Log Hob failed.\n"));
  }

  return EFI_SUCCESS;
}

/**
 Read function of Event log.s 

 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EfiReadNextEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL         *This,
  IN OUT VOID                             **EventListAddress
  )
{
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  BIOS_EVENT_LOG_ORGANIZATION           *CurrentLog;


  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuReadNextEventLog (This, EventListAddress);
  }

  //
  //Current Log is first log
  //
  CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  //
  //Check EventListAddress.
  //If it is NULL, and the first log exist, return first Log
  //
  if (*EventListAddress == NULL) {
    if ((CurrentLog->Type == 0xFF) && (CurrentLog->Length == 0xFF)) {

      return EFI_NOT_FOUND;
    }
    *EventListAddress = (UINT8 *)(UINTN)EPrivate->GPNVBase;
    return EFI_SUCCESS;
  }

  //
  //Check current log
  //
  CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)*EventListAddress;

  //
  //If the EventListAddress out of range, return error
  //
  if (((UINTN)CurrentLog > (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
      (UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase)) ||
      (CurrentLog->Length == 0)){

      return EFI_INVALID_PARAMETER;
  }

  //
  //If the EventListAddress point to the NULL, return
  //
  if ((CurrentLog->Type == 0xFF) && (CurrentLog->Length == 0xFF)) {

    return EFI_NOT_FOUND;
  }

  //
  //Find out and point to the next Event Log
  //
  *EventListAddress = (UINT8 *)( (UINT8*)CurrentLog + CurrentLog->Length);

  return EFI_SUCCESS;

}

/**
  Write event to BIOS Storage.

  @param [in] This                 This PPI interface.
  @param [in] BiosStorageType      Event log type.
  @param [in] PostBitmap1          Post bitmap 1 which will be stored in data area of POST error type log.
  @param [in] PostBitmap2          Post bitmap 2 which will be stored in data area of POST error type log.
  @param [in] OptionDataSize       Optional data size.
  @param [in] OptionLogData        Pointer to optional data.
  
  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
EfiWriteEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  )
{
  UINT8                                 *Src;
  UINT8                                 *Dest;
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  UINTN                                 WriteAddress;
  BIOS_EVENT_LOG_ORGANIZATION           *CurrentLog;
  BIOS_EVENT_LOG_ORGANIZATION           *Buffer;
  UINTN                                 NumBytes;
  UINTN                                 DataFormatTypeLength;
  EFI_EVENT_LOG_BUF                     *EventLogBuf = NULL;

  if ((OptionDataSize == 0) || (OptionLogData == NULL)) {
    OptionDataSize = 0;
    OptionLogData = NULL;
  }

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuWriteEventLog (This, EventLogType, PostBitmap1, PostBitmap2, OptionDataSize, OptionLogData);
  }
  
  EventLogBuf = EventLogAllocateZeroBuffer (sizeof(EFI_EVENT_LOG_BUF));
  if (EventLogBuf == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (SwitchToSmm()) {
    EventLogBuf->ActionType     = EVENT_LOG_WRITE_LOG;
    EventLogBuf->This           = This;
    EventLogBuf->EventLogType   = EventLogType;
    EventLogBuf->PostBitmap1    = PostBitmap1;
    EventLogBuf->PostBitmap2    = PostBitmap2;
    EventLogBuf->OptionDataSize = OptionDataSize;
    EventLogBuf->OptionLogData  = OptionLogData;
    
    SmmEventLogCall ((UINT8 *)EventLogBuf, sizeof(EFI_EVENT_LOG_BUF), (UINT8)EVENT_LOG_WRITE_LOG, (UINT16)SW_SMI_IO_PORT);
    return EFI_SUCCESS;
  }

  CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
    if (((UINTN)CurrentLog + CurrentLog->Length + BIOS_EVENT_LOG_BASE_LENGTH + OptionDataSize) >= 
         (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength)) {
      //
      // Event Full
      //
      return EFI_OUT_OF_RESOURCES;
    }

    if ((UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase) ||
        (CurrentLog->Length == 0)) {

      return EFI_UNSUPPORTED;
    }
    CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);
  }

  DataFormatTypeLength = 0;
  switch (EventLogType) {

  case EfiEventLogTypePostError:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH;
    break;

  default:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_NONE_LENGTH;
    break;
  }
  NumBytes = BIOS_EVENT_LOG_BASE_LENGTH + DataFormatTypeLength + OptionDataSize;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  NumBytes,
                  (VOID **)&Buffer
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Event Log allocate pool failed: %r\n", Status));
    return Status;
  }

  ZeroMem (Buffer, NumBytes);

  Buffer->Year   = ReadCmos8(0x09);
  Buffer->Month  = ReadCmos8(0x08);
  Buffer->Day    = ReadCmos8(0x07);
  Buffer->Hour   = ReadCmos8(0x04);
  Buffer->Minute = ReadCmos8(0x02);
  Buffer->Second = ReadCmos8(0x00);

  Buffer->Type   = EventLogType;
  Buffer->Length = (UINT8)NumBytes;

  if (EventLogType == EfiEventLogTypePostError) {
    *(UINT32 *)(Buffer->Data)            = PostBitmap1;
    *(UINT32 *)(UINTN)(&Buffer->Data[4]) = PostBitmap2;
  }

  if (OptionLogData!= NULL) {
    CopyMem ((UINT8*)((UINTN)Buffer+NumBytes-OptionDataSize), OptionLogData, OptionDataSize);
  }

  WriteAddress = (UINTN)CurrentLog;
  Dest = (UINT8 *)(UINTN)WriteAddress;
  Src = (UINT8 *)(UINTN)Buffer;

  Status = FlashProgram (
                        Dest,
                        Src,
                        &NumBytes,
                        (((UINTN)CurrentLog) & ~(0xFFFF))
                        );

  //
  //  !!WORKARUOUD!! - Force Cache update
  //
  *Dest = 0;
  
  gBS->FreePool(Buffer);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Event Log allocate pool failed: %r\n", Status));
  }

  return Status;

}

/**
 Clear function of Event log.           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EfiClearEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  UINT8                                 *Buffer;
  UINT32                                i;
  UINTN                                 BlockSize;
  UINTN                                 FdSupportEraseSize;
  UINTN                                 EraseCount;
  UINTN                                 EraseStartAddress;
  EFI_EVENT_LOG_BUF                     *EventLogBuf = NULL;

  EPrivate  = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuClearEventLog (This);
  }

  EventLogBuf = EventLogAllocateZeroBuffer (sizeof(EFI_EVENT_LOG_BUF));
  if (EventLogBuf == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (SwitchToSmm ()) {
    EventLogBuf->ActionType = EVENT_LOG_CLEAR_LOG;
    EventLogBuf->This = This;
    
    SmmEventLogCall ((UINT8 *)EventLogBuf, sizeof(EFI_EVENT_LOG_BUF), (UINT8)EVENT_LOG_WRITE_LOG, (UINT16)SW_SMI_IO_PORT);
    return EFI_SUCCESS;
  }
  
  FdSupportEraseSize = 0;
  if (mSpiDeviceType == TRUE) {
    FdSupportEraseSize = mSpiConfigBlock.BlockEraseSize;
  } else {
    //
    // Could not find the erase sector size, use default value 64k.
    // 
    FdSupportEraseSize = FLASH_BLOCK_SIZE;
  }
  
  EraseCount = GET_ERASE_SECTOR_NUM ( 
                                 EPrivate->GPNVBase,
                                 EPrivate->GPNVLength,
                                 EPrivate->GPNVBase & ~(FdSupportEraseSize - 1),
                                 FdSupportEraseSize
                                 );
  EraseStartAddress = EPrivate->GPNVBase & ~(FdSupportEraseSize - 1);
  
  BlockSize = FdSupportEraseSize * EraseCount;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  BlockSize,
                  (VOID **)&Buffer
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Event Log allocate pool failed: %r\n", Status));
    return Status;
  }

  //
  //Copy whole block data to buffer
  //
  CopyMem(
    Buffer,
    (VOID *)(UINTN)(EPrivate->GPNVBase & (~(FdSupportEraseSize - 1))),
    BlockSize
    );

  //
  //Copy modified GPNV data to buffer
  //
  for (i=0;i<EPrivate->GPNVLength;i++) {
    *(UINT8 *)((Buffer + (EPrivate->GPNVBase) - (EPrivate->GPNVBase & ((UINT32)(~0)) & (~(FdSupportEraseSize - 1))))+i) = 0xFF;
  }

  //
  //Flash GPNV
  //
  for (i = 0; i < EraseCount; i++) {
    
    Status = FlashErase (
                         (EPrivate->GPNVBase & ((UINT32)(~0)) & (~(FdSupportEraseSize - 1))) + FdSupportEraseSize * i,
                         FdSupportEraseSize
                         );
  }

  Status = FlashProgram (
                        (UINT8 *)(UINTN)(EPrivate->GPNVBase & ((~(FdSupportEraseSize - 1)))),
                        Buffer,
                        &BlockSize,
                        ((EPrivate->GPNVBase & (UINT32)(~0)) & ~(FdSupportEraseSize - 1))
                        );
  //
  //  !!WORKARUOUD!! - Force Cache update
  //
//  EfiWbinvd ();
  gBS->FreePool(Buffer);

  if (!mClearBiosStorageKernel) {
    Status = EPrivate->EventLogService.Write(
                                         &EPrivate->EventLogService,
                                         EfiEventLogTypeLogAreaResetCleared,
                                         0,
                                         0,
                                         0,
                                         NULL
                                         );
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Event Log write data failed: %r\n", Status));
  }
  
  return Status;
}

/**
  Write the BIOS Event Log with overwrite method.

  @param [in] This                 This PPI interface.
  @param [in] InputBuffer          The event log data to logging..
  
  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
EfiOverWriteEventLog (
  IN  EFI_BIOS_EVENT_LOG_PROTOCOL       *This,
  IN  BIOS_EVENT_LOG_ORGANIZATION       *InputBuffer
  )
{
  UINT8                                 *Src;
  UINT8                                 *Dest;
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  UINTN                                 WriteAddress;
  BIOS_EVENT_LOG_ORGANIZATION           *CurrentLog;
  BIOS_EVENT_LOG_ORGANIZATION           *Buffer;
  UINTN                                 NumBytes;
  UINTN                                 BaseLenght;
  
  if ((This == NULL) || (InputBuffer == NULL)) {
    return EFI_UNSUPPORTED;
  }

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);

  CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
    if (((UINTN)CurrentLog + CurrentLog->Length + InputBuffer->Length) >= 
        (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength)) {
      //
      // Event Full
      //
      return EFI_OUT_OF_RESOURCES;
    }

    if ((UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase) ||
        (CurrentLog->Length == 0)) {

      return EFI_UNSUPPORTED;
    }
    CurrentLog = (BIOS_EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);
  }

  NumBytes = InputBuffer->Length;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  NumBytes,
                  (VOID **)&Buffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, NumBytes);

  Buffer->Year   = InputBuffer->Year;
  Buffer->Month  = InputBuffer->Month;
  Buffer->Day    = InputBuffer->Day;
  Buffer->Hour   = InputBuffer->Hour;
  Buffer->Minute = InputBuffer->Minute;
  Buffer->Second = InputBuffer->Second;

  Buffer->Type   = InputBuffer->Type;;
  Buffer->Length = InputBuffer->Length;;

  BaseLenght = BIOS_EVENT_LOG_BASE_LENGTH;
  
  if (InputBuffer->Length > BIOS_EVENT_LOG_BASE_LENGTH) {
    CopyMem ((UINT8*)((UINTN)Buffer+NumBytes-(InputBuffer->Length - BaseLenght)), 
                &InputBuffer->Data[0], 
                InputBuffer->Length - BaseLenght);
  }

  WriteAddress = (UINTN)CurrentLog;
  Dest = (UINT8 *)(UINTN)WriteAddress;
  Src = (UINT8 *)(UINTN)Buffer;

  Status = FlashProgram (
                      Dest,
                      Src,
                      &NumBytes,
                      (WriteAddress & ~(0xFFFF))
                      );
  //
  //  !!WORKARUOUD!! - Force Cache update
  //
  *Dest = 0;
  
  gBS->FreePool(Buffer);

  return Status;

}

/**
  Check if BIOS protection is enabled. 
         
 @param[in]  Event             A pointer to the Event that triggered the callback.
 @param[in]  Context           A pointer to private data registered with the callback function.
  
 @retval EFI Status                  
*/
VOID
EFIAPI
BiosProtectEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

#ifdef SECURE_BOOT_SUPPORT
  mSwitchToSmm = TRUE;
#endif

#ifdef SECURE_FLASH_SUPPORT
  mSwitchToSmm = TRUE;
#endif
}

/**
  Check if accessing BIOS GPNV region need to switch to SMM driver. 
           
  @retval TRUE  Switch the access BIOS GPNV region by SMM driver.
          FALSE Don't switch.
*/
BOOLEAN
EFIAPI
SwitchToSmm (
  VOID
  )
{
  EFI_STATUS                            Status;
  H2O_EVENT_LOG_VARIABLE                EventLogVar;  
  UINTN                                 EventLogVarSize;

  if (mSwitchToSmm == TRUE) {
    return TRUE;
  }

  Status = gRT->GetVariable (
                         L"EventLogVar",
                         &gH2OEventLogVariableGuid,
                         NULL,
                         &EventLogVarSize,
                         &EventLogVar
                         );
  if (EFI_ERROR (Status)) {
    return FALSE;
  } else {
    if (EventLogVar.SmmEventLogReady == TRUE) {
      //
      // Event Log SMM driver is ready, then switch to the smm driver to access the BIOS GPNV region.
      //
      mSwitchToSmm = TRUE;
      return TRUE;
    } else {
      return FALSE;
    }
  }
  
}

/**
 Allocate Pool and return the pointer.

 @param[in]         This                
 @param[in]         SelId               
 @param[out]        SelData             
 
 @retval EFI Status                  
*/
VOID *
EFIAPI
EventLogAllocateZeroBuffer (
  IN UINTN     Size
  )
{
  VOID           *Buffer;
  EFI_STATUS     Status;

  Status = gBS->AllocatePool (
                          EfiBootServicesData,
                          Size,
                          (VOID **)&Buffer
                          );
  if (!EFI_ERROR (Status)) {
    ZeroMem (Buffer, Size);
  } else {
    Buffer = NULL;
  }
  return Buffer;
  
}

/**
 
 Notification function when Variable Architecture Protocol Installed
            
 @param[in] Event   - The Event that is being processed
 @param[in] Context - Event Context
  
 @retval EFI Status                  
*/
VOID
EFIAPI
InstalledVarWriteNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  EFI_STATUS                              Status;
  H2O_EVENT_STORAGE_VARIABLE              H2OEventStorageVar;  
  UINTN                                   VarSize;
  CHAR16                                  *VarName = H2O_EVENT_STORAGE_VARIABLE_NAME;

  //
  // Set EventStorage variable to store the BIOS Storege address.
  //
  VarSize = sizeof(H2O_EVENT_STORAGE_VARIABLE);
  gRT->GetVariable (
                H2O_EVENT_STORAGE_VARIABLE_NAME,
                &gH2OEventStorageVariableGuid,
                NULL,
                &VarSize,
                &H2OEventStorageVar
                );

  H2OEventStorageVar.BiosEventStorageExist = TRUE;
  H2OEventStorageVar.BiosEventStorageBaseAddress = (UINT32) (UINTN)(ELPrivate->GPNVBase);
  H2OEventStorageVar.BiosEventStorageLength = (UINT32) (UINTN)(ELPrivate->GPNVLength);

  VarSize = sizeof(H2O_EVENT_STORAGE_VARIABLE);
  Status = gRT->SetVariable (
                         VarName,
                         &gH2OEventStorageVariableGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                         VarSize,
                         &H2OEventStorageVar
                         );   
  if (EFI_ERROR (Status)) {
    return;
  }
  
  gBS->CloseEvent (mVarWriteEvent);

  return;

}


