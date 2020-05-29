/** @file

  BIOS Storage Kernel SMM implementation.

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

#include <BiosStorageKernelSmm.h>

EFI_EVENT_LOG_INSTANCE                *ELPrivate;
EFI_EVENT_LOG_INSTANCE                *SmmELPrivate;
EFI_EVENT                             mEvent;

LIST_ENTRY                            mBiosStorageKernelList;
BOOLEAN                               mClearBiosStorageKernel;

extern BOOLEAN                        mSpiDeviceType;
extern SPI_CONFIG_BLOCK               mSpiConfigBlock;
EFI_SMM_CPU_PROTOCOL                  *mSmmCpu = NULL;

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
  EFI_BIOS_EVENT_LOG_PROTOCOL           *DxeEventLogInstance;

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&DxeEventLogInstance);
  if (!EFI_ERROR (Status)) {
    Status = DxeEventLogInstance->Write (
                                    DxeEventLogInstance, 
                                    EventLogType, 
                                    PostBitmap1, 
                                    PostBitmap2, 
                                    OptionDataSize, 
                                    OptionLogData
                                    );
  }
  
  return Status;
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
  EFI_STATUS                           Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL          *DxeEventLogInstance;

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&DxeEventLogInstance);
  if (!EFI_ERROR (Status)) {
    Status = DxeEventLogInstance->ReadNext (DxeEventLogInstance, EventListAddress);
  }
  
  return Status;
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
  EFI_BIOS_EVENT_LOG_PROTOCOL          *DxeEventLogInstance;
  EFI_STATUS                           Status;

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&DxeEventLogInstance);
  if (!EFI_ERROR (Status)) {
    Status = DxeEventLogInstance->Clear (DxeEventLogInstance);
  }
  
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

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);

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
  EFI_SMM_SW_DISPATCH2_PROTOCOL       *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT         SwContext;
  EFI_HANDLE                          SwHandle;
  H2O_EVENT_LOG_VARIABLE              EventLogVar;  
  UINTN                               EventLogVarSize;

  //
  // Make sure the NV space is available for BIOS Storage.
  // 
  if ((!PcdGet32 (PcdBiosEventStorageBase)) || (!PcdGet32 (PcdBiosEventStorageSize))) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Make sure these global variables are initialized in both DXE and SMM.
  //
  mClearBiosStorageKernel = FALSE;
  InitializeListHead (&mBiosStorageKernelList);

  Status = gSmst->SmmAllocatePool (
                       EfiRuntimeServicesData,
                       sizeof (EFI_EVENT_LOG_INSTANCE),
                       (VOID **)&SmmELPrivate
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmELPrivate->Signature                      = EVENT_LOG_SIGNATURE;
  SmmELPrivate->DevicePtr                      = NULL;
  SmmELPrivate->GPNVBase                       = PcdGet32 (PcdBiosEventStorageBase);
  SmmELPrivate->GPNVLength                     = PcdGet32 (PcdBiosEventStorageSize);
  SmmELPrivate->EventLogService.Write          = EfiWriteEventLog;
  SmmELPrivate->EventLogService.Clear          = EfiClearEventLog;
  SmmELPrivate->EventLogService.ReadNext       = EfiReadNextEventLog;
  SmmELPrivate->EventLogService.DetectDevice   = FlashFdDetectDevice;
  SmmELPrivate->EventLogService.OverWrite      = EfiOverWriteEventLog;
  
  SmmELPrivate->Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                                           &SmmELPrivate->Handle,
                                           &gEfiBiosSmmEventLogProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           &(SmmELPrivate->EventLogService)
                                           );

  if (!EFI_ERROR (Status)) {
    if (SmmELPrivate->DevicePtr == NULL) {
      Status = FlashFdDetectDevice (&SmmELPrivate->EventLogService, NULL);
      ASSERT_EFI_ERROR (Status);
    }
    //
    // Register an API for Clear and Write Event log with SW SMI callback function(0x99).
    //
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch);
    if (EFI_ERROR (Status)) {
      return Status;
    } else {
      SwContext.SwSmiInputValue = SMM_EVENT_LOG_SMI;
      Status = SwDispatch->Register (
                                    SwDispatch,
                                    EventLogToSmm,
                                    &SwContext,
                                    &SwHandle
                                    );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      EventLogVar.SmmEventLogReady = TRUE;
      EventLogVarSize = sizeof (EventLogVar);
      Status = gRT->SetVariable (
                             L"EventLogVar",
                             &gH2OEventLogVariableGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                             EventLogVarSize,
                             &EventLogVar
                             );
    }
  }

  return Status;
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
 Write function of Event log.s          
 
 @retval EFI Status                  
*/
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

  if ((OptionDataSize == 0) || (OptionLogData == NULL)) {
    OptionDataSize = 0;
    OptionLogData = NULL;
  }

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuWriteEventLog (This, EventLogType, PostBitmap1, PostBitmap2, OptionDataSize, OptionLogData);
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
  if (gSmst == NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    NumBytes,
                    (VOID **)&Buffer
                    );
  } else {
    Status = gSmst->SmmAllocatePool(
                      EfiRuntimeServicesData,
                      NumBytes,
                      (VOID **)&Buffer
                      );
  }
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
  
  if (gSmst == NULL) {
    gBS->FreePool(Buffer);
  } else {
    gSmst->SmmFreePool (Buffer);
  }

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

  EPrivate  = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);

  if (EPrivate->DevicePtr == NULL) {
    return EmuClearEventLog (This);
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

  if (gSmst == NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    BlockSize,
                    (VOID **)&Buffer
                    );
  } else {
    Status = gSmst->SmmAllocatePool(
                      EfiRuntimeServicesData,
                      BlockSize,
                      (VOID **)&Buffer
                      );
  }
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
  if (gSmst == NULL) {
    gBS->FreePool(Buffer);
  } else {
    gSmst->SmmFreePool (Buffer);
  }

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
 Over Write function of Event Log.

 @param[in]         This                
 @param[in]         SelId               
 @param[out]        SelData             
 
 @retval EFI Status                  
*/
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

  if (gSmst == NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    NumBytes,
                    (VOID **)&Buffer
                    );
  } else {
    Status = gSmst->SmmAllocatePool(
                      EfiRuntimeServicesData,
                      NumBytes,
                      (VOID **)&Buffer
                      );
  }
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
  
  if (gSmst == NULL) {
    gBS->FreePool(Buffer);
  } else {
    gSmst->SmmFreePool (Buffer);
  }

  return Status;

}

/**
 A function for Clear and Write event log in SMM via SW SMI.           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogToSmm (
  IN     EFI_HANDLE                    Handle,
  IN     CONST VOID                    *Context,
  IN OUT VOID                          *CommBuffer,
  IN OUT UINTN                         *CommBufferSize
)
{
  EFI_STATUS                            Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL           *EventLog;  
  UINTN                                 Index;
  UINT32                                Ebx;  
  EFI_EVENT_LOG_BUF                     *EventLogBuf;
  UINT32                                RegisterData;

  //
  // Find out which CPU triggered the S/W SMI
  //
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, Index, &Ebx);
    if (Ebx == EVENT_LOG_EBX_SIGNATURE) {
      break;
    }
  }
  Status = GetDwordRegisterByCpuIndex (
                                   EFI_SMM_SAVE_STATE_REGISTER_RSI,
                                   Index,
                                   &RegisterData
                                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Index == gSmst->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }

  
  EventLogBuf = (EFI_EVENT_LOG_BUF *)(UINTN) RegisterData;

  Status = gSmst->SmmLocateProtocol (&gEfiBiosSmmEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  switch (EventLogBuf->ActionType) {
  case EVENT_LOG_CLEAR_LOG:
    EfiClearEventLog (EventLog);
    break;
  case EVENT_LOG_WRITE_LOG:
    Status = EfiWriteEventLog (
                  EventLog, 
                  EventLogBuf->EventLogType, 
                  EventLogBuf->PostBitmap1, 
                  EventLogBuf->PostBitmap2, 
                  EventLogBuf->OptionDataSize, 
                  EventLogBuf->OptionLogData
                  );
    break;
  default:
    break;
  }

  return Status;
}

/**
  This fucntion uses to read saved CPU double word register by CPU index

  @param[in]  RegisterNum   Register number which want to get
  @param[in]  CpuIndex      CPU index number to get register.
  @param[in]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Read double word register successfully
  @return Other             Any error occured while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
EFIAPI
GetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  OUT UINT32                            *RegisterData
  )
{
  EFI_STATUS                            Status;

  if (mSmmCpu == NULL) {
    Status = gSmst->SmmLocateProtocol (
                                   &gEfiSmmCpuProtocolGuid,
                                   NULL,
                                   (VOID **)&mSmmCpu
                                   );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  Status = mSmmCpu->ReadSaveState (
                               mSmmCpu,
                               sizeof (UINT32),
                               RegisterNum,
                               CpuIndex,
                               RegisterData
                               );
  return Status;
}

/**
  This fucntion uses to set saved CPU double word register by CPU index

  @param[in]  RegisterNum   Register number which want to get
  @param[in]  CpuIndex      CPU index number to get register.
  @param[in]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Set double word register successfully
  @return Other             Any error occured while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
EFIAPI
SetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  IN  UINT32                            *RegisterData
  )
{
  EFI_STATUS                            Status;

  if (mSmmCpu == NULL) {
    Status = gSmst->SmmLocateProtocol (
                                   &gEfiSmmCpuProtocolGuid,
                                   NULL,
                                   (VOID **)&mSmmCpu
                                   );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  Status = mSmmCpu->WriteSaveState (
                                mSmmCpu,
                                sizeof (UINT32),
                                RegisterNum,
                                CpuIndex,
                                RegisterData
                                );
  return Status;
}
