//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "EventLog.h"
#include "EfiRuntimeLib.h"
#include "CmosLib.h"

EFI_DEVICE_PATH_PROTOCOL              *CompleteFilePath;
EFI_EVENT_LOG_INSTANCE                *ELPrivate;
EFI_EVENT_LOG_INSTANCE                *SmmELPrivate;
EFI_SMM_BASE_PROTOCOL                 *mSmmBase = NULL;
//[-start-130719-IB07390109-modify]//
EFI_SMM_SYSTEM_TABLE                  *mSmst = NULL;
//[-end-130719-IB07390109-modify]//
EFI_BOOT_SERVICES                     *gBS = NULL;
EFI_SMM_RUNTIME_PROTOCOL              *SmmRT = NULL;
EFI_EVENT                             mEvent;

VOID                                  *mEventLogRegistration;
EFI_ENABLE_FVB_WRITES                 EnableFvbWritesFunction = NULL;
EFI_HANDLE                            mImageHandle;
BOOLEAN                               InSmm;
EFI_LIST_ENTRY                        mEventLogList;
BOOLEAN                               mClearEventLog;


EFI_STATUS
EmuWriteEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  )
/*++

Routine Description:

  Write event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
  Event log will temporarily be stored in allocated pool and be recorded in our temp event log list.

Arguments:

Returns:

--*/
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_PROTOCOL                *DxeEventLogInstance;
  EFI_EVENT_LOG_DATA                    *EventLog;

  if (InSmm) {
    Status = gBS->LocateProtocol (&gEfiEventLogProtocolGuid, NULL, &DxeEventLogInstance);
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

  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (EFI_EVENT_LOG_DATA) ,
                  &EventLog
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EventLog->Signature = EVENT_LOG_DATA_SIGNATURE;

  EventLog->Buffer = GenernateEventLog (EventLogType, PostBitmap1, PostBitmap2, OptionDataSize, OptionLogData);
  if (EventLog->Buffer == NULL) {
    gBS->FreePool (EventLog);
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&mEventLogList, &EventLog->Link);

  return EFI_SUCCESS;
}


EFI_STATUS
EmuReadNextEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN OUT VOID                           **EventListAddress
  )
/*++

Routine Description:

  Read next event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
  This function will search next event log from Hob list and our temp event log list.

Arguments:

Returns:

--*/
{
  BOOLEAN                              GetFirstEventLog;
  EFI_STATUS                           Status;
  EFI_EVENT_LOG_PROTOCOL               *DxeEventLogInstance;
  EFI_EVENT_LOG_DATA                   *EventLog;
  VOID                                 *HobList;
  EVENT_LOG_ORGANIZATION               *HobLog;
  EFI_LIST_ENTRY                       *Link;

  GetFirstEventLog = FALSE;

  if (InSmm) {
    Status = gBS->LocateProtocol (&gEfiEventLogProtocolGuid, NULL, &DxeEventLogInstance);
    if (!EFI_ERROR (Status)) {
      Status = DxeEventLogInstance->ReadNext (DxeEventLogInstance, EventListAddress);
    }

    return Status;
  }

  //
  // First, read event log in Hob list which is created during PEI phase.
  // But if perform clear event log at this POST time, there is no need to read event log from Hob list.
  //
  if (!mClearEventLog) {
    Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (*EventListAddress == NULL) {
      Status = GetNextGuidHob (&HobList, &gEventLogHobGuid, (VOID **) &HobLog, NULL);
      if (!EFI_ERROR (Status)) {
        *EventListAddress = (VOID *) HobLog;
        return EFI_SUCCESS;
      }
    }

    for (;;) {
      Status = GetNextGuidHob (&HobList, &gEventLogHobGuid, (VOID **) &HobLog, NULL);
      if (EFI_ERROR (Status)) {
        break;
      }

      if (HobLog == (EVENT_LOG_ORGANIZATION *) *EventListAddress) {
        Status = GetNextGuidHob (&HobList, &gEventLogHobGuid, (VOID **) &HobLog, NULL);
        if (EFI_ERROR (Status)) {
          GetFirstEventLog = TRUE;
          break;
        }

        *EventListAddress = (VOID *) HobLog;
        return EFI_SUCCESS;
      }
    }
  }

  if (IsListEmpty (&mEventLogList)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get first event log.
  //
  if (*EventListAddress == NULL || GetFirstEventLog) {
    Link = mEventLogList.ForwardLink;
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    *EventListAddress = (VOID *) EventLog->Buffer;
    return EFI_SUCCESS;
  }

  //
  // Keep searching until match the input event log, then return next event log.
  //
  Link = mEventLogList.ForwardLink;
  while (Link != &mEventLogList) {
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    if (EventLog->Buffer == *EventListAddress) {
      Link = Link->ForwardLink;
      if (Link == &mEventLogList) {
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

EFI_STATUS
EmuClearEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This
  )
/*++

Routine Description:

  Clear event log emulation function which is used before gEfiNonVolatileVariableProtocolGuid is installed.
  It will set event logs, which are stored in Hob list, are invalid and clear our temp event log list.

Arguments:

Returns:

--*/
{
  EFI_EVENT_LOG_PROTOCOL               *DxeEventLogInstance;
  EFI_STATUS                           Status;
  EFI_LIST_ENTRY                       *Link;
  EFI_EVENT_LOG_DATA                   *EventLog;

  if (InSmm) {
    Status = gBS->LocateProtocol (&gEfiEventLogProtocolGuid, NULL, &DxeEventLogInstance);
    if (!EFI_ERROR (Status)) {
      Status = DxeEventLogInstance->Clear (DxeEventLogInstance);
    }

    return Status;
  }

  mClearEventLog = TRUE;

  Link = mEventLogList.ForwardLink;
  while (Link != &mEventLogList) {
    EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
    Link = Link->ForwardLink;
    gBS->FreePool (EventLog->Buffer);
    gBS->FreePool (EventLog);
  }
  InitializeListHead (&mEventLogList);

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

EFI_STATUS
FlashFdDetectDevice (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  OUT UINT8                             *Buffer
  )
/*++

Routine Description:

  Smart detect current flash device type

Arguments:

Returns:

  EFI status

--*/
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *ELPrivate;
  VOID                                  *ProtocolInterface;

  Status = gBS->LocateProtocol (&gEfiNonVolatileVariableProtocolGuid, NULL, &ProtocolInterface);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  ELPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);

  ASSERT(EnableFvbWritesFunction != NULL);

  EnableFvbWritesFunction (TRUE);
  Status = AutoDetectDevice (
             &(ELPrivate->DevicePtr),
             mSmst
             );
  EnableFvbWritesFunction (FALSE);

  if (Buffer != NULL) {
    EfiCopyMem (Buffer, ELPrivate->DevicePtr, sizeof (FLASH_DEVICE));
  }

  return Status;
}

EFI_STATUS
GetFlashMapInfo (
  IN EFI_EVENT_LOG_INSTANCE     *ELPrivate
  )
/*++

Routine Description:

  Get flash map information for GPNV and spare stores.

Arguments:

  ELPrivate               - Pointer to event log private data.

Returns:

  EFI_SUCCESS             - Gat flash map information successful.
  EFI_INVALID_PARAMETER   - Input parameter is NULL.
  EFI_NOT_FOUND           - One of GPNV or spare store is not found.
  Other                   - Get hob list fail.

--*/
{
  EFI_FLASH_MAP_ENTRY_DATA      *FlashMapEntryData;
  VOID                          *HobList;
  VOID                          *Buffer;
  EFI_STATUS                    Status;
  BOOLEAN                       GPNVFound;
  BOOLEAN                       SpareFound;

  if (ELPrivate == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GPNVFound  = FALSE;
  SpareFound = FALSE;

  for (;;) {
    Status = GetNextGuidHob (&HobList, &gEfiFlashMapHobGuid, &Buffer, NULL);
    if (EFI_ERROR (Status) || (GPNVFound && SpareFound)) {
      break;
    }

    FlashMapEntryData = (EFI_FLASH_MAP_ENTRY_DATA *) Buffer;

    switch (FlashMapEntryData->AreaType) {

    case EFI_FLASH_AREA_GPNV:
      //
      // Get the first GPNV store area
      //
      if (!GPNVFound) {
        ELPrivate->GPNVBase   = (UINT32)(FlashMapEntryData->Entries[0].Base);
        ELPrivate->GPNVLength = (UINT32)(FlashMapEntryData->Entries[0].Length);
        GPNVFound = TRUE;
      }
      break;

    case EFI_FLASH_AREA_FTW_BACKUP:
      if (!SpareFound) {
        ELPrivate->SpareBase = (UINTN) FlashMapEntryData->Entries[0].Base;
        ELPrivate->SpareSize = (UINTN) FlashMapEntryData->Entries[0].Length;
        SpareFound = TRUE;
      }
      break;

    default:
      break;
    }
  }

  if (!GPNVFound || !SpareFound) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetEventLogHob (
  IN  EFI_EVENT_LOG_PROTOCOL            *This
  )
{
  VOID                                 *HobList;
  VOID                                 *Buffer;
  EFI_STATUS                           Status;
  EVENT_LOG_ORGANIZATION               *HobLog;
  EVENT_LOG_ORGANIZATION               *CurrentLog;
  EFI_EVENT_LOG_INSTANCE               *EPrivate;
  UINTN                                 Length;

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  CurrentLog = (EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  ASSERT(EnableFvbWritesFunction != NULL);

  while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
    CurrentLog = (EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);

    if (((UINTN)CurrentLog > (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
        (UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase)) ||
        (CurrentLog->Length == 0)) {

      return EFI_SUCCESS;
    }
  }

  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (;;) {
    Status = GetNextGuidHob (&HobList, &gEventLogHobGuid, &Buffer, NULL);
    if (EFI_ERROR (Status) ) {
      break;
    }

    HobLog = (EVENT_LOG_ORGANIZATION *) Buffer;

    Length = (UINTN)HobLog->Length;

    Status = EventLogFlashFdProgram (
               EPrivate->DevicePtr,
               (UINT8 *)(UINTN)CurrentLog,
               Buffer,
               &Length,
               (((UINTN)CurrentLog) & ~(0xFFFF))
               );
    if (EFI_ERROR (Status) ) {
      return Status;
    }

    CurrentLog = (EVENT_LOG_ORGANIZATION *) ((UINT8*) CurrentLog + Length);
    if (((UINTN)CurrentLog > (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
        (UINTN)CurrentLog  < (UINTN)(EPrivate->GPNVBase)) ||
        (CurrentLog->Length == 0)) {
      return EFI_SUCCESS;
    }
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
EventLogVirtualAddressChangeEvent (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
/*++

Routine Description:

  Fixup internal data pointers so that the services can be called in virtual mode.

Arguments:

  Event     The event registered.
  Context   Event context.

Returns:

  None.

--*/
{
  gBS->UninstallProtocolInterface (
                  ELPrivate->Handle,
                  &gEfiEventLogProtocolGuid,
                  &ELPrivate->EventLogService
                  );

}

VOID
EFIAPI
NonVolatileVariableNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Initialize Flash device if flash device hasn't been initialized

Arguments:

  (Standard EFI notify event - EFI_EVENT_NOTIFY)

Returns:

  None

--*/
{
  EFI_STATUS     Status;
  EFI_LIST_ENTRY                       *Link;
  EVENT_LOG_ORGANIZATION               *CurrentLog;
  EFI_EVENT_LOG_DATA                   *EventLog;
  UINTN                                NumBytes;

  ASSERT(EnableFvbWritesFunction != NULL);

  if (ELPrivate->DevicePtr == NULL) {
    Status = FlashFdDetectDevice (&ELPrivate->EventLogService, NULL);
    ASSERT_EFI_ERROR (Status);

    if (InClearProcess (ELPrivate)) {
      UpdateEventLogFromBackup (ELPrivate);
    }

    //
    // Before gEfiNonVolatileVariableProtocolGuid is installed, some drivers may write or clear event log.
    // It should write those event logs into flash device in here.
    // First write event log from Hob list, then write event log from out temp event log list.
    //
    if (mClearEventLog) {
      EfiClearEventLog (&ELPrivate->EventLogService);
    } else {
      GetEventLogHob (&ELPrivate->EventLogService);
    }
    mClearEventLog = FALSE;

    if (!IsListEmpty (&mEventLogList)) {
      Link = mEventLogList.ForwardLink;

      CurrentLog = (EVENT_LOG_ORGANIZATION *) (UINTN) ELPrivate->GPNVBase;
      while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
        if (((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(ELPrivate->GPNVBase + ELPrivate->GPNVLength) ||
            (UINTN)CurrentLog< (UINTN)(ELPrivate->GPNVBase) ||
            (CurrentLog->Length == 0)){
          return;
        }
        CurrentLog = (EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);
      }

      while (Link != &mEventLogList) {
        EventLog = DATA_FROM_EFI_EVENT_LOG_THIS(Link);
        NumBytes = (UINTN) EventLog->Buffer->Length;

        Status = EventLogFlashFdProgram (
                   ELPrivate->DevicePtr,
                   (UINT8 *) (UINTN) CurrentLog,
                   (UINT8 *) (UINTN) EventLog->Buffer,
                   &NumBytes,
                   (((UINTN) CurrentLog) & ~(0xFFFF))
                   );

        CurrentLog = (EVENT_LOG_ORGANIZATION *) ((UINT8*) CurrentLog + NumBytes);
        if ((((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(ELPrivate->GPNVBase + ELPrivate->GPNVLength)) ||
            ((UINTN)CurrentLog < (UINTN)(ELPrivate->GPNVBase)) ||
            (CurrentLog->Length == 0)) {
          return;
        }

        Link = Link->ForwardLink;
        EventLogFreePool (EventLog->Buffer);
        EventLogFreePool (EventLog);
      }

      InitializeListHead (&mEventLogList);
    }
  }

  return;
}

VOID
EFIAPI
SmmNonVolatileVariableNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Initialize Flash device if flash device hasn't been initialized

Arguments:

  (Standard EFI notify event - EFI_EVENT_NOTIFY)

Returns:

  None

--*/
{
  EFI_STATUS     Status;

  if (SmmELPrivate->DevicePtr == NULL) {
    Status = FlashFdDetectDevice (&SmmELPrivate->EventLogService, NULL);
    ASSERT_EFI_ERROR (Status);
  }

  return;
}

EFI_DRIVER_ENTRY_POINT (EventLogEntryPoint)

EFI_STATUS
EventLogEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  EFI_STATUS                          Status;
  EFI_EVENT_LOG_PROTOCOL              *EventLog;
  VOID                                *EventRegistration;
  EFI_EVENT                           Event;
  VOID                                *Registration;
  VOID                                *ProtocolInterface;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL   *ChipsetLibServices;
  EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL *SmmChipsetLibServices;

  Event = 0;

  //
  // Make sure these global variables are initialized in both DXE and SMM.
  //
  mClearEventLog = FALSE;
  InitializeListHead (&mEventLogList);
  EfiInitializeSmmDriverLib (ImageHandle, SystemTable);
  mImageHandle = ImageHandle;

  Status = gBS->LocateProtocol (&gEfiEventLogProtocolGuid, NULL, &EventLog);
  if (EFI_ERROR(Status)) {
    InSmm = FALSE;

    Status = gBS->LocateProtocol (&gEfiChipsetLibServicesProtocolGuid, NULL, &ChipsetLibServices);
    ASSERT_EFI_ERROR (Status);

    EnableFvbWritesFunction = ChipsetLibServices->EnableFvbWrites;

    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (EFI_EVENT_LOG_INSTANCE),
                    &ELPrivate
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ELPrivate->Signature = EVENT_LOG_SIGNATURE;
    ELPrivate->DevicePtr = NULL;

    Status = GetFlashMapInfo (ELPrivate);
    if (EFI_ERROR (Status)) {
      gBS->FreePool (ELPrivate);
      return Status;
    }

    ELPrivate->EventLogService.Write         = EfiWriteEventLog;
    ELPrivate->EventLogService.Clear         = EfiClearEventLog;
    ELPrivate->EventLogService.ReadNext      = EfiReadNextEventLog;
    ELPrivate->EventLogService.DetectDevice  = FlashFdDetectDevice;

    ELPrivate->Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &ELPrivate->Handle,
                    &gEfiEventLogProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(ELPrivate->EventLogService)
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->LocateProtocol (&gEfiNonVolatileVariableProtocolGuid, NULL, &ProtocolInterface);
      if (EFI_ERROR (Status)) {
        Status = gBS->CreateEvent (
                        EFI_EVENT_NOTIFY_SIGNAL,
                        EFI_TPL_CALLBACK,
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

      Status = gBS->CreateEvent (
                      EFI_EVENT_NOTIFY_SIGNAL,
                      EFI_TPL_CALLBACK,
                      EventLogNotificationFunction,
                      NULL,
                      &mEvent
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gBS->RegisterProtocolNotify (
                      &gEfiSmmChipsetLibServicesProtocolGuid,
                      mEvent,
                      &EventRegistration
                      );
      ASSERT_EFI_ERROR (Status);
    }

    return Status;
  }
  //
  // Great!  We're now in SMM!
  //
  //
  // Initialize global variables
  //
  InSmm = TRUE;
  Status = gBS->LocateProtocol(&gEfiSmmBaseProtocolGuid, NULL, &mSmmBase);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = mSmmBase->GetSmstLocation(mSmmBase, &mSmst);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = SmmRT->LocateProtocol (
                    &gEfiSmmRuntimeProtocolGuid,
                    NULL,
                    &SmmRT
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = SmmRT->LocateProtocol (
                    &gEfiSmmChipsetLibServicesProtocolGuid,
                    NULL,
                    &SmmChipsetLibServices
                    );
  ASSERT_EFI_ERROR(Status);

  EnableFvbWritesFunction = SmmChipsetLibServices->EnableFvbWrites;

  Status = mSmmBase->SmmAllocatePool (
                       mSmmBase,
                       EfiRuntimeServicesData,
                       sizeof (EFI_EVENT_LOG_INSTANCE),
                       &SmmELPrivate
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmELPrivate->Signature = EVENT_LOG_SIGNATURE;
  SmmELPrivate->DevicePtr = NULL;

  Status = GetFlashMapInfo (SmmELPrivate);
  if (EFI_ERROR (Status)) {
    mSmmBase->SmmFreePool (mSmmBase, SmmELPrivate);
    return Status;
  }

  SmmELPrivate->EventLogService.Write          = EfiWriteEventLog;
  SmmELPrivate->EventLogService.Clear          = EfiClearEventLog;
  SmmELPrivate->EventLogService.ReadNext       = EfiReadNextEventLog;
  SmmELPrivate->EventLogService.DetectDevice   = FlashFdDetectDevice;

  SmmELPrivate->Handle = NULL;
  Status = SmmRT->InstallProtocolInterface (
                    &SmmELPrivate->Handle,
                    &gEfiEventLogProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(SmmELPrivate->EventLogService)
                    );
  if (!EFI_ERROR (Status)) {
    Status = SmmRT->LocateProtocol (&gEfiNonVolatileVariableProtocolGuid, NULL, &ProtocolInterface);
    if (EFI_ERROR (Status)) {
      Status = SmmRT->EnableProtocolNotify (
                        SmmNonVolatileVariableNotifyFunction,
                        NULL,
                        &gEfiNonVolatileVariableProtocolGuid,
                        &Event
                        );
      ASSERT_EFI_ERROR(Status);
    } else {
      SmmNonVolatileVariableNotifyFunction (Event, NULL);
    }
  }

  return Status;
}

EFI_STATUS
EfiReadNextEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN OUT VOID                             **EventListAddress
  )
{
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  EVENT_LOG_ORGANIZATION                *CurrentLog;


  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuReadNextEventLog (This, EventListAddress);
  }

  //
  //Current Log is first log
  //
  CurrentLog = (EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

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
  CurrentLog = (EVENT_LOG_ORGANIZATION *)*EventListAddress;

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

EFI_STATUS
EfiWriteEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
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
  EVENT_LOG_ORGANIZATION                *CurrentLog;
  EVENT_LOG_ORGANIZATION                *Buffer;
  UINTN                                 NumBytes;

  if ((OptionDataSize == 0) || (OptionLogData == NULL)) {
    OptionDataSize = 0;
    OptionLogData = NULL;
  }

  EPrivate = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);
  if (EPrivate->DevicePtr == NULL) {
    return EmuWriteEventLog (This, EventLogType, PostBitmap1, PostBitmap2, OptionDataSize, OptionLogData);
  }

  ASSERT(EnableFvbWritesFunction != NULL);

  CurrentLog = (EVENT_LOG_ORGANIZATION *)(UINTN)EPrivate->GPNVBase;

  while ((CurrentLog->Type != 0xFF) && (CurrentLog->Length != 0xFF)) {
    if (((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(EPrivate->GPNVBase + EPrivate->GPNVLength) ||
        (UINTN)CurrentLog< (UINTN)(EPrivate->GPNVBase) ||
        (CurrentLog->Length == 0)){
      return EFI_UNSUPPORTED;
    }
    CurrentLog = (EVENT_LOG_ORGANIZATION *)( (UINT8*)CurrentLog + CurrentLog->Length);
  }

  Buffer = GenernateEventLog (EventLogType, PostBitmap1, PostBitmap2, OptionDataSize, OptionLogData);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WriteAddress = (UINTN)CurrentLog;
  Dest = (UINT8 *)(UINTN)WriteAddress;
  Src = (UINT8 *)(UINTN)Buffer;
  NumBytes = (UINTN) Buffer->Length;

  Status = EventLogFlashFdProgram (
             EPrivate->DevicePtr,
             Dest,
             Src,
             &NumBytes,
             (WriteAddress & ~(0xFFFF))
             );

  EventLogFreePool (Buffer);

  return Status;
}

EFI_STATUS
EfiClearEventLog (
  IN  EFI_EVENT_LOG_PROTOCOL            *This
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT_LOG_INSTANCE                *EPrivate;
  UINT8                                 *BlockBuffer;
  UINTN                                 Offset;
  UINTN                                 BlockSize;
  UINT8                                 *SpareBackup;
  UINTN                                 SpareBackupSize;
  UINTN                                 SpareBackupBlockSize;
  UINTN                                 Size;
  EVENT_LOG_HEADER                      EventLogHeader;
  EVENT_LOG_ORGANIZATION                *ResetLog;

  BlockBuffer = NULL;
  SpareBackup = NULL;
  Size = 0;

  EPrivate  = INSTANCE_FROM_EFI_EVENT_LOG_THIS (This);

  if (EPrivate->DevicePtr == NULL) {
    return EmuClearEventLog (This);
  }

  //
  // Five steps:
  // 1.Backup spare data.
  // 2.Write event log data to spare store.
  // 3.Update EVENT_LOG_HEADER to spare store.
  // 4.Write event log data from spare store to GPNV store.
  // 5.Restore original data to spare store.
  //

  //
  // 1.Backup spare sub-region data, get spare data size and calculate spare data block size.
  //
  SpareBackup = EventLogAllocatePool (EPrivate->SpareSize);
  EfiCopyMem (
    SpareBackup,
    (VOID *) EPrivate->SpareBase,
    EPrivate->SpareSize
    );

  SpareBackupSize      = 0;
  SpareBackupBlockSize = 0;
  for (Offset = EPrivate->SpareSize; Offset > 0; Offset--) {
    if (SpareBackup[Offset - 1] != 0xFF) {
      SpareBackupSize = Offset;

      if ((SpareBackupSize & 0xFFF) != 0) {
        SpareBackupBlockSize = 0x1000;
      }
      SpareBackupBlockSize += (SpareBackupSize & ~(0xFFF));
      break;
    }
  }

  //
  // 2.Write whole block, which event log data is cleared, to spare store.
  //
  BlockSize = ((EPrivate->GPNVBase + EPrivate->GPNVLength - 1) & 0xFFFFF000) - (EPrivate->GPNVBase & 0xFFFFF000) + 0x1000;
  if (BlockSize + 0x1000 > EPrivate->SpareSize) {
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  BlockBuffer = EventLogAllocatePool (BlockSize);
  if (BlockBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // Copy whole block data to buffer and clear event logs.
  //
  EfiCopyMem (
    BlockBuffer,
    (VOID *) (UINTN) (EPrivate->GPNVBase & ~(0xFFF)),
    BlockSize
    );

  Offset = EPrivate->GPNVBase - (EPrivate->GPNVBase & 0xFFFFF000);
  EfiSetMem (
    BlockBuffer + Offset,
    EPrivate->GPNVLength,
    0xFF
    );

  //
  // Fill Cleared/Reset log as first log.
  //
  if (!mClearEventLog) {
    ResetLog = GenernateEventLog (EfiEventLogTypeLogAreaResetCleared, 0, 0, 0, NULL);
    if (ResetLog != NULL) {
      EfiCopyMem (
        BlockBuffer + Offset,
        ResetLog,
        (UINTN) ResetLog->Length
        );
      EventLogFreePool (ResetLog);
    }
  }

  if (SpareBackupSize != 0) {
    Status = EventLogFlashFdErase (
               EPrivate->DevicePtr,
               EPrivate->SpareBase,
               SpareBackupBlockSize
               );
    if (EFI_ERROR(Status)) {
      goto Done;
    }
  }

  Size = BlockSize;
  Status = EventLogFlashFdProgram (
             EPrivate->DevicePtr,
             (UINT8 *) (EPrivate->SpareBase + sizeof(EVENT_LOG_HEADER)),
             BlockBuffer,
             &Size,
             EPrivate->SpareBase
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  //
  // 3.Update EVENT_LOG_HEADER to spare store.
  //
  EventLogHeader.Signature = EVENT_LOG_CLEAR_PROCESS_SIGNATURE;
  EventLogHeader.Length    = (UINT32) EPrivate->GPNVLength;
  Size = sizeof(EVENT_LOG_HEADER);

  Status = EventLogFlashFdProgram (
             EPrivate->DevicePtr,
             (UINT8 *) EPrivate->SpareBase,
             (UINT8 *) &EventLogHeader,
             &Size,
             EPrivate->SpareBase
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  //
  // 4.Write event log data from spare store to GPNV store.
  //
  Status = EventLogFlashFdErase (
             EPrivate->DevicePtr,
             (EPrivate->GPNVBase & 0xFFFFF000),
             BlockSize
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  Size = BlockSize;
  Status = EventLogFlashFdProgram (
             EPrivate->DevicePtr,
             (UINT8 *) (UINTN) (EPrivate->GPNVBase & 0xFFFFF000),
             (UINT8 *) EPrivate->SpareBase + sizeof(EVENT_LOG_HEADER),
             &Size,
             ((EPrivate->GPNVBase & 0xFFFFF000) & ~(0xFFF))
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  //
  // 5.Restore original data to spare store
  //
  Size = BlockSize + 0x1000;
  Status = EventLogFlashFdErase (
             EPrivate->DevicePtr,
             EPrivate->SpareBase,
             Size
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  if (SpareBackupSize != 0) {
    Status = EventLogFlashFdProgram (
               EPrivate->DevicePtr,
               (UINT8 *) EPrivate->SpareBase,
               SpareBackup,
               &SpareBackupSize,
               EPrivate->SpareBase
               );
    if (EFI_ERROR(Status)) {
      goto Done;
    }
  }

Done:
  if (SpareBackup != NULL) {
    EventLogFreePool (SpareBackup);
  }
  if (BlockBuffer != NULL) {
    EventLogFreePool (BlockBuffer);
  }

  return Status;
}


EFI_DEVICE_PATH_PROTOCOL *
AppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
/*++

Routine Description:

  Function is used to append a Src1 and Src2 together.

Arguments:

  Src1  - A pointer to a device path data structure.
  Src2  - A pointer to a device path data structure.

Returns:

  A pointer to the new device path is returned.
  NULL is returned if space for the new device path could not be allocated from pool.
  It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

--*/
{
  EFI_STATUS                  Status;
  UINTN                       Size;
  UINTN                       Size1;
  UINTN                       Size2;
  EFI_DEVICE_PATH_PROTOCOL    *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *SecondDevicePath;

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL
  //
  Size1 = DevicePathSize (Src1);
  Size2 = DevicePathSize (Src2);
  Size = Size1 + Size2;

  if (Size1 != 0 && Size2 != 0) {
    Size -= sizeof(EFI_DEVICE_PATH_PROTOCOL);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **)&NewDevicePath);

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  gBS->CopyMem (NewDevicePath, Src1, Size1);

  //
  // Over write Src1 EndNode and do the copy
  //
  if (Size1 != 0) {
    SecondDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)((CHAR8 *)NewDevicePath + (Size1 - sizeof (EFI_DEVICE_PATH_PROTOCOL)));
  } else {
    SecondDevicePath = NewDevicePath;
  }

  gBS->CopyMem (SecondDevicePath, Src2, Size2);

  return NewDevicePath;
}

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!EfiIsDevicePathEnd (DevicePath)) {
    DevicePath = EfiNextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN)DevicePath - (UINTN)Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

VOID
EFIAPI
EventLogNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
/*++

Routine Description:


Arguments:

  Event   - The Event that is being processed

  Context - Event Context

Returns:
  None

--*/
{
  EFI_LOADED_IMAGE_PROTOCOL             *LoadedImage;
  EFI_STATUS                            Status;
  EFI_DEVICE_PATH_PROTOCOL              *ImageDevicePath;
  EFI_HANDLE                            Handle = NULL;
  EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL *SmmChipsetLibServices;
  DEBUG_CODE (
  BOOLEAN                               SmmBaseInSmm;
  UINTN                                 BackupDebugLevel;
  EFI_DEBUG_MASK_PROTOCOL               *DebugMaskProtocol;
  )

  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, &mSmmBase);
  ASSERT_EFI_ERROR(Status);


  Status = gBS->LocateProtocol (
                        &gEfiSmmChipsetLibServicesProtocolGuid,
                        NULL,
                        &SmmChipsetLibServices
                        );
  ASSERT_EFI_ERROR(Status);

  //
  // Get this driver's image's FilePath
  //
  Status = gBS->HandleProtocol (mImageHandle, &gEfiLoadedImageProtocolGuid, &LoadedImage);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->HandleProtocol (LoadedImage->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID*) &ImageDevicePath);
  ASSERT_EFI_ERROR (Status);

  CompleteFilePath = AppendDevicePath (ImageDevicePath, LoadedImage->FilePath);

  //
  // Don't output EFI Debug message in SMM mode to prevent from SMM stack over flow
  //
  DEBUG_CODE (
    BackupDebugLevel = 0;
    SmmBaseInSmm = FALSE;
    mSmmBase->InSmm (mSmmBase, &SmmBaseInSmm);
    if (SmmBaseInSmm) {
      DebugMaskProtocol = NULL;
      Status = gBS->HandleProtocol (
                      mImageHandle,
                      &gEfiDebugMaskProtocolGuid,
                      &DebugMaskProtocol
                      );
      if (!EFI_ERROR (Status) && DebugMaskProtocol != NULL) {
        DebugMaskProtocol->GetDebugMask (DebugMaskProtocol, &BackupDebugLevel);
        DebugMaskProtocol->SetDebugMask (DebugMaskProtocol, 0);
      }
    }
  )

  //
  // Load the image in memory to SMRAM. It will generate the SMI.
  //
  Handle = NULL;
  mSmmBase->Register (mSmmBase, CompleteFilePath, NULL, 0, &Handle, FALSE);

  //
  // Resotre orginal debug mask
  //
  DEBUG_CODE (
    if (SmmBaseInSmm) {
      DebugMaskProtocol = NULL;
      Status = gBS->HandleProtocol (
                      mImageHandle,
                      &gEfiDebugMaskProtocolGuid,
                      &DebugMaskProtocol
                      );
      if (!EFI_ERROR (Status) && DebugMaskProtocol != NULL) {
        DebugMaskProtocol->SetDebugMask (DebugMaskProtocol, BackupDebugLevel);
      }
    }
  )

  gBS->CloseEvent (mEvent);

  return;
}

EFI_STATUS
EventLogFlashFdErase (
  IN FLASH_DEVICE              *DevicePtr,
  IN UINTN                     LbaWriteAddress,
  IN UINTN                     EraseBlockSize
  )
/*++

Routine Description:

  Event log driver flash erase function.

Arguments:

  DevicePtr         - Pointer to flash device instance.
  LbaWriteAddress   - LBA write address.
  EraseBlockSize    - Erase size.

Returns:

  EFI_SUCCESS           - Flash erase successful.
  EFI_INVALID_PARAMETER - Flash device pointer is NULL.
  Other                 - Flash erase function return fail.

--*/
{
  EFI_STATUS                   Status;

  if (DevicePtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EnableFvbWritesFunction (TRUE);
  Status = DevicePtr->Erase (
                        LbaWriteAddress,
                        EraseBlockSize
                        );
  EnableFvbWritesFunction (FALSE);

  return Status;
}


EFI_STATUS
EventLogFlashFdProgram (
  IN FLASH_DEVICE              *DevicePtr,
  IN UINT8                     *Dest,
  IN UINT8                     *Src,
  IN UINTN                     *NumBytes,
  IN UINTN                     LbaWriteAddress
  )
/*++

Routine Description:

  Event log driver flash program function.

Arguments:

  DevicePtr         - Pointer to flash device instance.
  Dest              - Destination Offset.
  Src               - Pointer to source buffer.
  NumBytes          - Number of bytes which will be write into.
  LbaWriteAddress   - LBA write address.

Returns:

  EFI_SUCCESS           - Flash program successful.
  EFI_INVALID_PARAMETER - Flash device pointer is NULL.
  Other                 - Flash program function return fail.

--*/
{
  EFI_STATUS                   Status;

  if (DevicePtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EnableFvbWritesFunction (TRUE);
  Status = DevicePtr->Program (
                        Dest,
                        Src,
                        NumBytes,
                        LbaWriteAddress
                        );
  EnableFvbWritesFunction (FALSE);

  return Status;
}


VOID *
EventLogAllocatePool (
  IN UINTN            Size
  )
/*++

Routine Description:

  Event log driver allocate pool function.

Arguments:

  Size        - Request memory size.

Returns:

  The allocate pool, or NULL

--*/
{
  EFI_STATUS          Status;
  VOID                *Buffer;

  if (Size == 0) {
    return NULL;
  }

  Buffer = NULL;
  if (mSmst == NULL) {
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    Size,
                    &Buffer
                    );
  } else {
    Status = mSmst->SmmAllocatePool(
                      EfiRuntimeServicesData,
                      Size,
                      &Buffer
                      );
  }

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  return Buffer;
}

EFI_STATUS
EventLogFreePool (
  IN VOID            *Buffer
  )
/*++

Routine Description:

  Event log driver free pool function.

Arguments:

  Buffer                  - The allocated pool entry to free.

Returns:

  EFI_SUCCESS             - Free pool successful.
  EFI_INVALID_PARAMETER   - Inputer parameter is NULL.
  Other                   - System free pool function fail.

--*/
{
  EFI_STATUS         Status;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmst == NULL) {
    Status = gBS->FreePool (Buffer);
  } else {
    Status = mSmst->SmmFreePool (Buffer);
  }

  return Status;
}

BOOLEAN
InClearProcess (
  IN EFI_EVENT_LOG_INSTANCE     *ELPrivate
  )
/*++

Routine Description:

  Check system state is whether in the clear event log process

Arguments:

  ELPrivate - Point to event log private data.

Returns:

  TRUE      - System is in the clear event log process
  FALSE     - System isn't in the clear event log process

--*/
{
  EVENT_LOG_HEADER              *EventLogHeader;

  EventLogHeader = (EVENT_LOG_HEADER *) ELPrivate->SpareBase;

  if (EventLogHeader->Signature == EVENT_LOG_CLEAR_PROCESS_SIGNATURE &&
      EventLogHeader->Length    == (UINT32) ELPrivate->GPNVLength) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
UpdateEventLogFromBackup (
  IN EFI_EVENT_LOG_INSTANCE     *ELPrivate
  )
/*++

Routine Description:

  This function uses backup data to update event log in GPNV store.

Arguments:

  ELPrivate              - Point to event log private data.

Returns:

  EFI_SUCCESS            - Update event log in GPNV store successful.
  EFI_ABORTED            - It is not in the clear event log process or backup data size is not correct.
  EFI_OUT_OF_RESOURCES   - Allocate memory fail.
  Other                  - Erase or program flash device fail.

--*/
{
  UINT8                         *SpareBuffer;
  EFI_STATUS                    Status;
  UINTN                         Offset;
  UINTN                         Size;
  UINTN                         GPNVBlockSize;
  UINTN                         SpareBufferSize;
  UINTN                         SpareBufferBlockSize;

  //
  // Check system is whether in clear event log process. System only execute
  // restore event log from bakcup if system is in clear event log process.
  //
  if (!InClearProcess (ELPrivate)) {
    return EFI_ABORTED;;
  }

  SpareBuffer = EventLogAllocatePool (ELPrivate->SpareSize);
  if (SpareBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // 1.Read spare data, get spare data size and calculate block size.
  //
  EfiCopyMem (
    SpareBuffer,
    (VOID *) ELPrivate->SpareBase,
    ELPrivate->SpareSize
    );

  SpareBufferSize      = 0;
  SpareBufferBlockSize = 0;

  for (Offset = ELPrivate->SpareSize; Offset > 0; Offset--) {
    if (SpareBuffer[Offset - 1] != 0xFF) {
      SpareBufferSize = Offset;

      if ((SpareBufferSize & 0xFFF) != 0) {
        SpareBufferBlockSize = 0x1000;
      }
      SpareBufferBlockSize += (SpareBufferSize & ~(0xFFF));
      break;
    }
  }

  GPNVBlockSize = ((ELPrivate->GPNVBase + ELPrivate->GPNVLength - 1) & 0xFFFFF000) - (ELPrivate->GPNVBase & 0xFFFFF000) + 0x1000;

  if (SpareBufferSize > GPNVBlockSize + sizeof (EVENT_LOG_HEADER) ||
      SpareBufferSize < sizeof (EVENT_LOG_HEADER)) {
    SpareBufferBlockSize = ELPrivate->SpareSize;
    Status = EFI_ABORTED;
    goto Done;
  }

  //
  // 2.Write event log data from spare store.
  //
  Status = EventLogFlashFdErase (
             ELPrivate->DevicePtr,
             (ELPrivate->GPNVBase & 0xFFFFF000),
             GPNVBlockSize
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Size = SpareBufferSize - sizeof(EVENT_LOG_HEADER);

  Status = EventLogFlashFdProgram (
             ELPrivate->DevicePtr,
             (UINT8 *) (UINTN) (ELPrivate->GPNVBase & 0xFFFFF000),
             (UINT8 *) ELPrivate->SpareBase + sizeof(EVENT_LOG_HEADER),
             &Size,
             ((ELPrivate->GPNVBase & 0xFFFFF000) & ~(0xFFF))
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:
  //
  // Clear all of data in spare store before exiting this funciotn to prevent from system will enter this function every POST.
  //
  Status = EventLogFlashFdErase (
             ELPrivate->DevicePtr,
             ELPrivate->SpareBase,
             SpareBufferBlockSize
             );

  EventLogFreePool (SpareBuffer);

  return Status;
}

EVENT_LOG_ORGANIZATION *
GenernateEventLog (
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  )
/*++

Routine Description:

  Genernate event log data.

Arguments:

  EventLogType      - Event log type.
  PostBitmap1       - Post bitmap 1 which will be stored in data area of POST error type log.
  PostBitmap2       - Post bitmap 2 which will be stored in data area of POST error type log.
  OptionDataSize    - Option data size.
  OptionLogData     - Poiunter to option data.

Returns:

  Pointer to allocated memory which contain event log data or NULL.

--*/
{
  UINTN                                 DataFormatTypeLength;
  UINTN                                 NumBytes;
  EVENT_LOG_ORGANIZATION                *Buffer;

  DataFormatTypeLength = 0;

  switch (EventLogType) {

  case EfiEventLogTypePostError:
    DataFormatTypeLength = EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH;
    break;

  default:
    DataFormatTypeLength = EVENT_LOG_DATA_FORMAT_NONE_LENGTH;
    break;
  }
  NumBytes = EVENT_LOG_BASE_LENGTH + DataFormatTypeLength + OptionDataSize;

  Buffer = EventLogAllocatePool (NumBytes);
  if (Buffer == NULL) {
    return NULL;
  }

  EfiZeroMem (Buffer, NumBytes);

  Buffer->Year   = EfiReadCmos8(0x09);
  Buffer->Month  = EfiReadCmos8(0x08);
  Buffer->Day    = EfiReadCmos8(0x07);
  Buffer->Hour   = EfiReadCmos8(0x04);
  Buffer->Minute = EfiReadCmos8(0x02);
  Buffer->Second = EfiReadCmos8(0x00);

  Buffer->Type   = EventLogType;
  Buffer->Length = (UINT8) NumBytes;

  if (EventLogType == EfiEventLogTypePostError) {
    *(UINT32 *) (Buffer->Data)             = PostBitmap1;
    *(UINT32 *) (UINTN) (&Buffer->Data[4]) = PostBitmap2;
  }

  if (OptionLogData != NULL) {
    EfiCopyMem ((UINT8*) ((UINTN) Buffer + NumBytes - OptionDataSize), OptionLogData, OptionDataSize);
  }

  return Buffer;
}

