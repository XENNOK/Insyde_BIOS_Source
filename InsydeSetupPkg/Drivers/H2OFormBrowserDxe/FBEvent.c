/** @file
Event for formbrowser
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

#include "InternalH2OFormBrowser.h"
#include <Protocol/SetupMouse.h>
#include <Protocol/Cpu.h>

EFI_EVENT                                    *mInputEventList      = NULL;
H2O_INPUT_EVENT_DESCRIPTION                  *mInputEventDescList  = NULL;
UINT32                                        mInputEventListCount = 0;
LIST_ENTRY      mEventQueue           = INITIALIZE_LIST_HEAD_VARIABLE (mEventQueue);
EFI_LOCK        mEventQueueLock       = {TPL_CALLBACK, 4, 1};
EFI_GUID        mScuFormSetGuid       = {0x9f85453e, 0x2f03, 0x4989, 0xad, 0x3b, 0x4a, 0x84, 0x07, 0x91, 0xaf, 0x3a};
EFI_GUID        mSecureBootMgrFormSetGuid = {0xaa1305b9, 0x1f3, 0x4afb, 0x92, 0xe, 0xc9, 0xb9, 0x79, 0xa8, 0x52, 0xfd};
extern EFI_HII_HANDLE     mCurrentHiiHandle;
extern EFI_GUID           mCurrentFormSetGuid;
extern UINT16             mCurrentFormId;
extern BOOLEAN            mHiiPackageListUpdated;

extern FORM_BROWSER_FORMSET     *mSystemLevelFormSet;
extern UINT16                   mCurFakeQestId;
extern BOOLEAN                  mFinishRetrieveCall;
STATIC FORM_DISPLAY_ENGINE_STATEMENT mDisplayStatement;

USER_INPUT                    *gUserInput;
FORM_DISPLAY_ENGINE_FORM      *gFormData;

#define TICKS_PER_MS            10000U


FORM_BROWSER_FORM *
GetScuLoadDefaultForm (
  IN FORM_BROWSER_FORMSET                *FormSet
  )
{
  LIST_ENTRY                             *FormLink;
  FORM_BROWSER_FORM                      *Form;

  FormLink = GetFirstNode (&FormSet->FormListHead);
  Form     = FORM_BROWSER_FORM_FROM_LINK (FormLink);

  while (!IsNull (&FormSet->FormListHead, FormLink)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (FormLink);
    if (Form->FormId == 0xFFFF) {
      break;
    }
    FormLink = GetNextNode (&FormSet->FormListHead, FormLink);
  }

  return Form;
}

/**
 Detect key press.

 @param [in] Private         Formbrowser private data
 @param [in] Event           Display engine event type

 @retval EFI_SUCCESS         Detect key press success.

**/
EFI_STATUS
FBKeyPress (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  )
{

  H2O_DISPLAY_ENGINE_EVT_KEYPRESS        *KeyPress;
  UINT32                                  Index;
  H2O_DISPLAY_ENGINE_PROTOCOL            *TargetDE;

  //
  // BugBug this event need pass to display engine
  //
  KeyPress = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *)Event;

  //
  // change engine by ` key
  //
  if (KeyPress->KeyData.Key.UnicodeChar == '`') {
    for (Index = 0; Index < Private->EngineListCount; Index++) {
      if (Private->ActivatedEngine == Private->EngineList[Index]) {
        FBSetActivedEngine (Private, (H2O_CONSOLE_ID)((Index + 1) % Private->EngineListCount + 1));
        break;
      }
    }
    return EFI_SUCCESS;
  }

  //
  // check target
  // Rretrieve next display engine to pass key press
  //
  if (Event->Target != 0 && Event->Target != H2O_FORM_BROWSER_CONSOLE_NOT_ASSIGNED) {
    ASSERT (Event->Target >= 1 && (UINTN)Event->Target <= Private->EngineListCount);
    if (Event->Target == 0 || (UINTN)Event->Target > Private->EngineListCount) {
      return EFI_UNSUPPORTED;
    }
    TargetDE = Private->EngineList[Event->Target - 1];
  } else {
    TargetDE = Private->ActivatedEngine;
  }

  TargetDE->Notify (TargetDE, Event);

  return EFI_SUCCESS;
}

/**
 Formbrowser select question

 @param [in] Private         Formbrowser private data
 @param [in] Event           Display engine event type

 @retval Status              Formbrowser select question status

**/
STATIC
EFI_STATUS
FBSelectQuestion (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  )
{
  EFI_STATUS                             Status;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q        *SelectQ;
  FORM_BROWSER_FORM                      *Form;
  FORM_BROWSER_STATEMENT                 *Statement;
  FORM_DISPLAY_ENGINE_STATEMENT          DisplayStatement;

  SelectQ = (H2O_DISPLAY_ENGINE_EVT_SELECT_Q *)Event;
  Form = FBPageIdToForm (Private, SelectQ->PageId);

  if (Form == NULL) {
    return EFI_NOT_FOUND;
  }

  if (SelectQ->QuestionId != 0) {
    Statement = IdToQuestion2 (Form, SelectQ->QuestionId);
  } else {
    DisplayStatement.OpCode = SelectQ->IfrOpCode;
    Statement = GetBrowserStatement (&DisplayStatement);
  }

  if (Statement == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // BUGBUG: Differenet page,
  //         need follow ProcessGotoOpCode to change formset, form, and question
  //
  ASSERT (SelectQ->PageId == Private->FB.CurrentP->PageId);
  if (SelectQ->PageId != Private->FB.CurrentP->PageId) {
    return EFI_NOT_FOUND;
  }

  Private->FB.CurrentQ = &Statement->Statement;
  gCurrentSelection->Statement  = Statement;
  gCurrentSelection->QuestionId = Statement->QuestionId;
  gCurrentSelection->CurrentMenu->QuestionId = Statement->QuestionId;

  Status = FBBroadcastEvent (Event);

  return Status;
}

/**
 Refresh question.

 @param [in] Private         Formbrowser private data
 @param [in] Event           Display engine event type

 @retval Status              Refresh question status
**/
STATIC
EFI_STATUS
RefreshQuestion (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN FORM_BROWSER_STATEMENT              *Question
  )
{
  EFI_STATUS                      Status;
  UI_MENU_SELECTION               *Selection;

  Selection = gCurrentSelection;
  Status = GetQuestionValue (Selection->FormSet, Selection->Form, Question, GetSetValueWithHiiDriver);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Question value may be changed, need invoke its Callback()
  //
  Status = ProcessCallBackFunction(Selection, Selection->FormSet, Selection->Form, Question, EFI_BROWSER_ACTION_CHANGING, FALSE);

  CopyMem (&Question->Statement.HiiValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));

  return Status;
}


/**
 Formbrowser timer refresh entry list

 @param [in] Private         Formbrowser private data
 @param [in] Event           Display engine event type

 @retval EFI_SUCCESS         Formbrowser timer refresh entry list success
**/
EFI_STATUS
FBTimer (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  )
{
  H2O_DISPLAY_ENGINE_EVT_TIMER           *Timer;
  STATEMENT_REFRESH_ENTRY                *RefreshEntry;
  LIST_ENTRY                             *Link;
  H2O_DISPLAY_ENGINE_EVT_REFRESH_Q       RefreshQ;
  H2O_DISPLAY_ENGINE_PROTOCOL            *TargetDE;

  Timer = (H2O_DISPLAY_ENGINE_EVT_TIMER *)Event;

  ASSERT (Timer->Hdr.Target != H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST);

  //
  //Not yet implement; it should send to DE
  //
  if (Timer->Hdr.Target != H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER) {
    if (Event->Target > (INT32) Private->EngineListCount) {
      return EFI_NOT_FOUND;
    }

    TargetDE = Private->EngineList[Event->Target - 1];
    TargetDE->Notify (TargetDE, Event);
    return EFI_SUCCESS;
  }

  switch (Timer->TimerId) {

  case 0:
    //
    // refresh entry list
    //
    ZeroMem (&RefreshQ, sizeof (H2O_DISPLAY_ENGINE_EVT_REFRESH_Q));
    RefreshQ.Hdr.Size = sizeof (H2O_DISPLAY_ENGINE_EVT_REFRESH_Q);
    RefreshQ.Hdr.Type = H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q;

    Link = GetFirstNode (&Private->RefreshList);
    while (!IsNull (&Private->RefreshList, Link)) {
      RefreshEntry = (STATEMENT_REFRESH_ENTRY *) Link;
      Link = GetNextNode (&Private->RefreshList, Link);

      RefreshQuestion (Private, RefreshEntry->Statement);
      RefreshQ.PageId     = gCurrentSelection->Form->PageInfo.PageId;
      RefreshQ.QuestionId = RefreshEntry->Statement->QuestionId;
      RefreshQ.IfrOpCode  = RefreshEntry->Statement->OpCode;
      QueueEvent ((H2O_DISPLAY_ENGINE_EVT*)&RefreshQ);
    }
    break;

  default:
    ASSERT (FALSE);
  }

  return EFI_SUCCESS;
}

/**
 Get current time in nano second

 @return current time in nano second or 0 if get fail
**/
UINT64
EFIAPI
GetElapsedTimeInNanoSec (
  VOID
  )
{
  EFI_STATUS             Status;
  STATIC EFI_CPU_ARCH_PROTOCOL  *Cpu = NULL;
  UINT64                 CurrentTick;
  UINT64                 TimerPeriod;
  STATIC UINT64          TimerPeriodInPs = 0; // 1e-12 second

  if (Cpu == NULL) {
    Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &Cpu);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return 0;
    }
  }

  Status = Cpu->GetTimerValue (Cpu, 0, &CurrentTick, NULL);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "H2O form browser get tick fail\n"));
    return 0;
  }

  if (TimerPeriodInPs == 0) {
    Status = Cpu->GetTimerValue (Cpu, 0, &CurrentTick, &TimerPeriod);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "H2O form browser get tick fail\n"));
      return 0;
    }
    TimerPeriodInPs = DivU64x32 (TimerPeriod, 1000);
  }

  return MultU64x32 (DivU64x32 (CurrentTick, 1000), (UINT32) TimerPeriodInPs);
}

/**
 Formbrowser set timer.

 @param [in] Target           Target of killtimer
 @param [in] TimerId          A id to the function to be notified when the time-out value elapses
 @param [in] Context          Context of time event
 @param [in] TriggerTime      trigger time

 @retval EFI_SUCCESS          Formbrowser set timer successfully
**/
EFI_STATUS
FBSetTimer (
  IN INT32                                 Target,
  IN UINT32                                TimerId,
  IN H2O_FORM_BROWSER_TIMER_TYPE           Type,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *NotifyEvent,
  IN UINT64                                TimeoutInNanoSec
  )
{
  TIMERINFO                                *TimerInfo;
  H2O_DISPLAY_ENGINE_EVT                   *Event;
  LIST_ENTRY                               *Node;

  if (NotifyEvent == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // check current timer lsit to avoid register timer repeatedly
  //
  Node = GetFirstNode (&mFBPrivate.TimerList);
  while (!IsNull (&mFBPrivate.TimerList, Node)) {
    TimerInfo = TIMERINFO_FROM_LINK (Node);
    if ((TimerInfo->Target == Target) &&
        (TimerInfo->TimerId == TimerId) &&
        (TimerInfo->Type == Type) &&
        (TimerInfo->TimeoutInNanoSec == TimeoutInNanoSec)) {
      return EFI_SUCCESS;
    }
    Node = GetNextNode (&mFBPrivate.TimerList, Node);
  }

  TimerInfo = AllocateZeroPool (sizeof (TIMERINFO));
  if (TimerInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Event = AllocateCopyPool (NotifyEvent->Size, NotifyEvent);
  if (Event == NULL) {
    FreePool (TimerInfo);
    return EFI_OUT_OF_RESOURCES;
  }

  TimerInfo->Signature             = H2O_FORM_BROWSER_TIMER_INFO_SIGNATURE;
  TimerInfo->Target                = Target;
  TimerInfo->TimerId               = TimerId;
  TimerInfo->Type                  = Type;
  TimerInfo->NotifyEvent           = Event;
  TimerInfo->TimeoutInNanoSec      = TimeoutInNanoSec;
  TimerInfo->ClockExpiresInNanoSec = GetElapsedTimeInNanoSec () + TimeoutInNanoSec;

  InsertTailList (&mFBPrivate.TimerList, &TimerInfo->Link);

  return EFI_SUCCESS;
}

/**
 Formbrowser kill timer.

 @param [in] Target           Target of killtimer
 @param [in] TimerId          A id to the function to be notified when the time-out value elapses

 @retval TRUE                 Formbrowser kill timer success
 @retval FALSE                Formbrowser kill timer fail
**/
BOOLEAN
FBKillTimer (
  IN  INT32                    Target,
  IN  UINT32                   TimerId
  )
{
  LIST_ENTRY                             *Link;
  H2O_FORM_BROWSER_PRIVATE_DATA          *Private;
  TIMERINFO                              *TimerInfo;

  Private = &mFBPrivate;

  Link = GetFirstNode (&Private->TimerList);
  while (!IsNull (&Private->TimerList, Link)) {
    TimerInfo = TIMERINFO_FROM_LINK (Link);
    Link      = GetNextNode (&Private->TimerList, Link);

    if (TimerInfo->Target == Target && TimerInfo->TimerId == TimerId) {
      RemoveEntryList (&TimerInfo->Link);
      FreePool (TimerInfo->NotifyEvent);
      FreePool (TimerInfo);
      return TRUE;
    }
  }
  return FALSE;
}


/**
 Formbrowser broadcast event.

 @param [in] Event           Display engine event type

 @retval Status              Formbrowser broadcast event status
**/
EFI_STATUS
FBBroadcastEvent (
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event
  )
{
  EFI_STATUS                             Status;
  H2O_FORM_BROWSER_PRIVATE_DATA          *Private;
  UINT32                                 Index;

  Private = &mFBPrivate;

  Status = EFI_SUCCESS;
  for (Index = 0; Index < Private->EngineListCount; Index++) {
    Status = Private->EngineList[Index]->Notify (Private->EngineList[Index], Event);
  }

  return Status;
}

/**
 Queue of event.

 @param [in] Event           Display engine event type

 @retval EFI_SUCCESS         Queue of event success
**/
EFI_STATUS
QueueEvent (
  H2O_DISPLAY_ENGINE_EVT        *Event
  )
{
  H2O_FORM_BROWSER_PRIVATE_DATA  *Private;
  H2O_DISPLAY_ENGINE_EVENT_NODE  *Node;

  Private = &mFBPrivate;

  Node = (H2O_DISPLAY_ENGINE_EVENT_NODE*)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVENT_NODE));
  if (Node == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Node->Signature = H2O_DISPLAY_ENGINE_EVENT_NODE_SIGNATURE;
  Node->Event     = (H2O_DISPLAY_ENGINE_EVT *) AllocatePool (Event->Size);
  if (Node->Event == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Node->Event, Event, Event->Size);

  EfiAcquireLock (&mEventQueueLock);
  InsertTailList (&mEventQueue, &Node->Link);
  EfiReleaseLock (&mEventQueueLock);

  return EFI_SUCCESS;
}

/**
 Get next queue event.

 @param [in] Event           Display engine event type

 @retval EFI_SUCCESS         Get next queue event successfully
**/
STATIC
BOOLEAN
GetNextQueuedEvent (
  H2O_DISPLAY_ENGINE_EVT        **Event
  )
{
  H2O_FORM_BROWSER_PRIVATE_DATA  *Private;
  H2O_DISPLAY_ENGINE_EVENT_NODE  *Node;

  Private = &mFBPrivate;

  ASSERT (Event != NULL);

  EfiAcquireLock (&mEventQueueLock);
  if (IsListEmpty (&mEventQueue)) {
    EfiReleaseLock (&mEventQueueLock);
    return FALSE;
  }

  Node = H2O_DISPLAY_ENGINE_EVENT_NODE_FROM_LINK (
           (LIST_ENTRY *)GetFirstNode (&mEventQueue)
           );
  RemoveEntryList (&Node->Link);
  EfiReleaseLock (&mEventQueueLock);

  *Event = Node->Event;
  FreePool (Node);

  return TRUE;
}

/**
 Queue of event.

 @param [in] NumberOfEvents       Number Of events
 @param [in] UserEvents           Event list
 @param [out] UserIndex           Event index

 @retval EFI_SUCCESS              Queue of event success
**/
STATIC
EFI_STATUS
EFIAPI
FBWaitForEvent (
  IN UINTN        NumberOfEvents,
  IN EFI_EVENT    *UserEvents,
  OUT UINTN       *UserIndex
  )
{
  EFI_STATUS      Status;
  UINTN           Index;

  for (Index = 0; Index < NumberOfEvents; Index++) {

    Status = gBS->CheckEvent (UserEvents[Index]);

    //
    // provide index of event that caused problem
    //
    if (Status != EFI_NOT_READY) {
      *UserIndex = Index;
      return Status;
    }
  }

  return EFI_NOT_READY;
}

/**
 Get next event according to device type.

 @param [in] Event           Display engine event type

 @retval TRUE                Get next event success
 @retval FALSE               Get next event fail

**/
STATIC
BOOLEAN
GetNextEventTimeout (
  H2O_DISPLAY_ENGINE_EVT        **Event,
  UINTN                         TimeOut
  )
{

  EFI_STATUS                          Status;
  UINTN                               Index;
  EFI_INPUT_KEY                       Key;
  EFI_KEY_DATA                        KeyData;
  EFI_SIMPLE_POINTER_STATE            MouseState;
  H2O_DISPLAY_ENGINE_EVT_KEYPRESS     *KeyEvent;
  H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE *MouseEvent;
  H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *AbsPtrEvent;
  H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL      *SimpleTextIn;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *SimpleTextInEx;
  EFI_SIMPLE_POINTER_PROTOCOL         *SimplePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL       *AbsolutePointer;
  H2O_INPUT_EVENT_DESCRIPTION         *InputEventDesc;
  UINTN                               X;
  UINTN                               Y;
  BOOLEAN                             LeftButton;
  BOOLEAN                             RightButton;
  STATIC UINTN                        CursorX = 0;
  STATIC UINTN                        CursorY = 0;

  Private = &mFBPrivate;
  SimpleTextIn = NULL;
  AbsolutePointer = NULL;

  if (mInputEventListCount == 0) {
    return FALSE;
  }

  if (Private->SetupMouse != NULL) {
    Status = Private->SetupMouse->QueryState (Private->SetupMouse, &X, &Y, &LeftButton, &RightButton);

    if (!EFI_ERROR (Status)) {
      if (0) {
        MouseEvent = (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE *)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE));
        if (MouseEvent == NULL) {
          return FALSE;
        }
        MouseEvent->Hdr.Size = sizeof (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE);
        MouseEvent->Hdr.Type = H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE;
        MouseEvent->Hdr.Target = 0;
        MouseEvent->State.RelativeMovementX = (INT32)(X - CursorX);
        MouseEvent->State.RelativeMovementY = (INT32)(Y - CursorY);
        MouseEvent->State.LeftButton        = LeftButton;
        MouseEvent->State.RightButton       = RightButton;
        *Event = (H2O_DISPLAY_ENGINE_EVT*)MouseEvent;

        CursorX = X;
        CursorY = Y;
      } else {
        AbsPtrEvent = (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE));
        if (AbsPtrEvent == NULL) {
          return FALSE;
        }
        AbsPtrEvent->Hdr.Size = sizeof (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE);
        AbsPtrEvent->Hdr.Type = H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE;
        AbsPtrEvent->Hdr.Target = 0;
        AbsPtrEvent->AbsPtrState.CurrentX = (UINT64)(X);
        AbsPtrEvent->AbsPtrState.CurrentY = (UINT64)(Y);
        if (LeftButton) {
          AbsPtrEvent->AbsPtrState.ActiveButtons |= EFI_ABSP_TouchActive;
        }
        if (RightButton) {
          AbsPtrEvent->AbsPtrState.ActiveButtons |= EFI_ABS_AltActive;
        }
        *Event = (H2O_DISPLAY_ENGINE_EVT*) AbsPtrEvent;
      }

      return TRUE;
    }
  }

  Index  = 0;
  Status = FBWaitForEvent (
             (UINTN)mInputEventListCount,
             mInputEventList,
             &Index
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  InputEventDesc = &mInputEventDescList[Index];

  switch (InputEventDesc->DeviceType) {

  case H2O_FORM_BROWSER_CONSOLE_STI:
    SimpleTextIn = (EFI_SIMPLE_TEXT_INPUT_PROTOCOL*)InputEventDesc->Protocol;
    Status = SimpleTextIn->ReadKeyStroke (SimpleTextIn, &Key);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    KeyEvent = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_KEYPRESS));
    if (KeyEvent == NULL) {
      return FALSE;
    }
    KeyEvent->Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_KEYPRESS);
    KeyEvent->Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS;
    KeyEvent->Hdr.Target = InputEventDesc->ConsoleId;
    CopyMem (&KeyEvent->KeyData.Key, &Key, sizeof (EFI_INPUT_KEY));
    *Event = (H2O_DISPLAY_ENGINE_EVT*)KeyEvent;
    break;

  case H2O_FORM_BROWSER_CONSOLE_STI2:
    SimpleTextInEx = (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*)InputEventDesc->Protocol;
    Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    KeyEvent = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_KEYPRESS));
    if (KeyEvent == NULL) {
      return FALSE;
    }
    KeyEvent->Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_KEYPRESS);
    KeyEvent->Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS;
    KeyEvent->Hdr.Target = InputEventDesc->ConsoleId;
    CopyMem (&KeyEvent->KeyData, &KeyData, sizeof (EFI_KEY_DATA));
    *Event = (H2O_DISPLAY_ENGINE_EVT*)KeyEvent;
    break;

  case H2O_FORM_BROWSER_CONSOLE_SP:
    SimplePointer = (EFI_SIMPLE_POINTER_PROTOCOL*)InputEventDesc->Protocol;
    Status = SimplePointer->GetState (SimplePointer, &MouseState);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    MouseEvent = (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE *)AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE));
    if (MouseEvent == NULL) {
      return FALSE;
    }
    MouseEvent->Hdr.Size = sizeof (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE);
    MouseEvent->Hdr.Type = H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE;
    MouseEvent->Hdr.Target = InputEventDesc->ConsoleId;
    CopyMem (&MouseEvent->State, &MouseState, sizeof (EFI_SIMPLE_POINTER_STATE));
    *Event = (H2O_DISPLAY_ENGINE_EVT*)MouseEvent;
    break;

  default:
    ASSERT (FALSE);
    return FALSE;
  }

  return TRUE;
}

/**
 Check timer event function.
 If expired time came, add the notify event in queue.
**/
VOID
CheckTimerEvent (
  VOID
  )
{
  LIST_ENTRY                               *StartLink;
  LIST_ENTRY                               *CurrentLink;
  TIMERINFO                                *TimerInfo;

  if (IsListEmpty (&mFBPrivate.TimerList)) {
    return;
  }

  StartLink   = &mFBPrivate.TimerList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != StartLink) {
    TimerInfo   = TIMERINFO_FROM_LINK (CurrentLink);
    CurrentLink = CurrentLink->ForwardLink;

    if (GetElapsedTimeInNanoSec () >= TimerInfo->ClockExpiresInNanoSec) {
      QueueEvent (TimerInfo->NotifyEvent);

      if (TimerInfo->Type != H2O_FORM_BROWSER_TIMER_TYPE_PERIODIC) {
        FBKillTimer (TimerInfo->Target, TimerInfo->TimerId);
      } else {
        TimerInfo->ClockExpiresInNanoSec = GetElapsedTimeInNanoSec () + TimerInfo->TimeoutInNanoSec;
      }
    }
  }
}

/**
 Get next event.

 @param [in] Event           Display engine event type

 @retval TRUE                Get next event success
 @retval FALSE               Get next event fail

**/
BOOLEAN
GetNextEvent (
  IN H2O_DISPLAY_ENGINE_EVT        **Event
  )
{
  ASSERT (Event != NULL);

  for (;;) {
    FBHotPlugEventFunc ();

    if (GetNextQueuedEvent (Event)) {
      break;
    }

    if (GetNextEventTimeout (Event, 0L)) {
      break;
    }
    CheckTimerEvent ();
  }

  return TRUE;
}

/**
 Destroy event queue.

 @param [in] Private         Formbrowser private data

**/
VOID
DestroyEventQueue (
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private
  )
{
  H2O_DISPLAY_ENGINE_EVT               *Event;
  BOOLEAN                              IsEmpty;

  for (;;) {
    EfiAcquireLock (&mEventQueueLock);
    IsEmpty = IsListEmpty (&mEventQueue);
    EfiReleaseLock (&mEventQueueLock);
    if (IsEmpty) {
      break;
    }

    GetNextQueuedEvent (&Event);
    FreePool (Event);
  }
}

/**
 Convert event type to string.

 @param [in] Event           Display engine event type

 @return Event string        Get Event string

**/
CHAR16 *EventToStr(IN CONST H2O_EVT_TYPE EvtType)
{
#ifdef _MSC_VER
#define EVENTSTR(x) if(EvtType==x) return (L#x)
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_L);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_L);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT_Q);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_P);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER);
  EVENTSTR(H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP);
#endif
  return L"Unknown";
}


/**
 Formbrowser event callback function.

 @param [in] Event           Display engine event type

 @retval Status              Formbrowser event callback function status
**/
EFI_STATUS
FBEventCallback (
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event,
  OUT      BOOLEAN                       *ExitFlag
  )
{
  IN H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  EFI_STATUS                             Status;
  H2O_DISPLAY_ENGINE_EVT_CHANGE_Q        *ChangeQ;
  H2O_DISPLAY_ENGINE_EVT_SELECT_P        *SelectP;
  UINT16                                 HiiHandleIndex;
  UINT16                                 FormId;
  FORM_BROWSER_FORM                      *Form;
  UINT8                                  Operand;
  FORM_BROWSER_STATEMENT                 *Statement;
  FORM_ENTRY_INFO                        *MenuList;


  Private = &mFBPrivate;
  *ExitFlag = FALSE;

  if (Event == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (gUserInput, sizeof (USER_INPUT));

  Status    = EFI_SUCCESS;

  switch (Event->Type) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS:
    Status = FBKeyPress (Private, Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE:
    //
    // BugBug this event need pass to display engine
    // BUGBUG the event need pass to local display engine or remote display engine by console pair,
    //        not use hot key to deteminate event transmit
    //
    Status = Private->ActivatedEngine->Notify (Private->ActivatedEngine, Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_L:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_L:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_P:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_P:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_Q:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH:
    Status = FBBroadcastEvent (Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q:
    Operand = Private->FB.CurrentQ->Operand;

    if (Operand == EFI_IFR_REF_OP ||
        Operand == EFI_IFR_ACTION_OP ||
        Operand == EFI_IFR_RESET_BUTTON_OP) {
      mDisplayStatement.OpCode      = Private->FB.CurrentQ->IfrOpCode;
      gUserInput->SelectedStatement = &mDisplayStatement;
      *ExitFlag = TRUE;
    } else if (Operand != EFI_IFR_CHECKBOX_OP) {
      //
      // Process checkbox in ChangeQ directly, not OpenQ
      //
      BroadcastOpenDByQ (Private->FB.CurrentQ);
    }
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q:
    ChangeQ = (H2O_DISPLAY_ENGINE_EVT_CHANGE_Q *) Event;
    mDisplayStatement.OpCode = Private->FB.CurrentQ->IfrOpCode;
    gUserInput->SelectedStatement = &mDisplayStatement;
    CopyHiiValue (&gUserInput->InputValue, &ChangeQ->HiiValue);

    Statement = GetBrowserStatement(gUserInput->SelectedStatement);
    if (Statement != NULL && Statement->Operand == EFI_IFR_STRING_OP) {
      gUserInput->InputValue.Value.string = HiiSetString (gCurrentSelection->FormSet->HiiHandle, 0, (EFI_STRING) ChangeQ->HiiValue.Buffer, NULL);
    }
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q:
    Status = FBSelectQuestion (Private, Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P:
    SelectP = (H2O_DISPLAY_ENGINE_EVT_SELECT_P *) Event;
    HiiHandleIndex = (UINT16) (SelectP->PageId >> 16);
    FormId         = (UINT16) (SelectP->PageId & 0xFFFF);

    if (HiiHandleIndex > Private->HiiHandleCount) {
      break;
    }

    if (HiiHandleIndex == Private->HiiHandleIndex) {
      if (FormId == gCurrentSelection->FormId) {
        //
        // do nothing
        //
        break;
      }
      gCurrentSelection->FormId     = FormId;
      gCurrentSelection->QuestionId = 0;
      gCurrentSelection->Action     = UI_ACTION_REFRESH_FORM;
    } else {
      Private->HiiHandleIndex        = (UINTN) HiiHandleIndex;
      gCurrentSelection->Handle      = Private->HiiHandleList[Private->HiiHandleIndex];
      gCurrentSelection->FormId      = FormId;
      gCurrentSelection->QuestionId  = 0;
      gCurrentSelection->CurrentMenu = NULL;
      gCurrentSelection->Action      = UI_ACTION_REFRESH_FORMSET;
      CopyGuid (&gCurrentSelection->FormSetGuid, &gZeroGuid);

      //
      // Clear the menu history data.
      //
      while (!IsListEmpty (&mFBPrivate.FormBrowserEx2.FormViewHistoryHead)) {
        MenuList = FORM_ENTRY_INFO_FROM_LINK (mFBPrivate.FormBrowserEx2.FormViewHistoryHead.ForwardLink);
        RemoveEntryList (&MenuList->Link);
        FreePool (MenuList);
      }
    }
    gUserInput->Action = BROWSER_ACTION_NONE;
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT:
    FBBroadcastEvent (Event);
    gUserInput->Action = BROWSER_ACTION_NONE;
    gCurrentSelection->Action = UI_ACTION_EXIT;
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT:
    if (CompareGuid (gCurrentSelection->FormSet->ClassGuid, &mScuFormSetGuid)) {
      Status = BroadcastOpenDByHotKeyEvt (H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT);
      if (EFI_ERROR (Status)) {
        break;
      }
      Form = GetScuLoadDefaultForm (gCurrentSelection->FormSet);
      Status = ExtractDefault (
                 gCurrentSelection->FormSet,
                 Form,
                 EFI_HII_DEFAULT_CLASS_MANUFACTURING,
                 FormLevel,
                 GetDefaultForAll,
                 NULL,
                 FALSE
                 );
      gUserInput->Action = BROWSER_ACTION_NONE;
      gCurrentSelection->Action = UI_ACTION_REFRESH_FORMSET;
    } else {
      gUserInput->Action = BROWSER_ACTION_DEFAULT;
      gUserInput->DefaultId = EFI_HII_DEFAULT_CLASS_STANDARD;
    }
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT:
    if (CompareGuid (gCurrentSelection->FormSet->ClassGuid, &mScuFormSetGuid) ||
        CompareGuid (gCurrentSelection->FormSet->ClassGuid, &mSecureBootMgrFormSetGuid)) {
      Status = BroadcastOpenDByHotKeyEvt (H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT);
      if (EFI_ERROR (Status)) {
        break;
      }

      gUserInput->Action = BROWSER_ACTION_SUBMIT | BROWSER_ACTION_RESET;
      gCurrentSelection->Action = UI_ACTION_EXIT;
    } else {
      gUserInput->Action = BROWSER_ACTION_SUBMIT;
    }
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT:
    if (!FormExitPolicy()) {
      break;
    }
    gUserInput->Action = BROWSER_ACTION_FORM_EXIT;
    *ExitFlag = TRUE;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP:
    Status = BroadcastOpenDByHotKeyEvt (Event->Type);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER:
    Status = FBTimer (Private, Event);
    break;

  default:
    DEBUG ((EFI_D_INFO, "Invalid H2O Display Engine Event: %d\n", Event->Type));
    ASSERT (FALSE);
    break;
  }

  return Status;
}

/**
  OEM specifies whether Setup exits Page by ESC key.

  This function customized the behavior that whether Setup exits Page so that
  system able to boot when configuration is not changed.

  @retval  TRUE     Exits FrontPage
  @retval  FALSE    Don't exit FrontPage.
**/
BOOLEAN
EFIAPI
FormExitPolicy (
  VOID
  )
{
  if (CompareGuid (&gCurrentSelection->FormSet->Guid, &gFrontPageFormSetGuid)) {
    return FALSE;
  }
  return TRUE;
}


/**
  Confirm how to handle the changed data.

  @return Action BROWSER_ACTION_SUBMIT, BROWSER_ACTION_DISCARD or other values.
**/
UINTN
EFIAPI
ConfirmDataChange (
  VOID
  )
{
  EFI_STATUS                    Status;

  if (CompareGuid (gCurrentSelection->FormSet->ClassGuid, &mScuFormSetGuid) ||
      CompareGuid (gCurrentSelection->FormSet->ClassGuid, &mSecureBootMgrFormSetGuid)) {
    Status = BroadcastOpenDByHotKeyEvt (H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT);
    if (!EFI_ERROR (Status)) {
      gCurrentSelection->Action = UI_ACTION_EXIT;
      return BROWSER_ACTION_DISCARD;
    } else {
      return BROWSER_ACTION_NONE;
    }
  } else {
    Status = BroadcastOpenDByHotKeyEvt (H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT);
    if (!EFI_ERROR (Status)) {
      return BROWSER_ACTION_DISCARD;
    } else {
      return BROWSER_ACTION_NONE;
    }
  }
}

VOID
BrowserStatusProcess (
  VOID
  )
{
  CHAR16                        *ErrorInfo;
  UINT8                         TimeOut;
  EFI_STRING_ID                 StringToken;
  H2O_FORM_BROWSER_D            Dialog;
  EFI_IFR_OP_HEADER             *OpCodeBuf;

  if (gFormData->BrowserStatus == BROWSER_SUCCESS) {
    return;
  }

  StringToken          = 0;
  OpCodeBuf            = NULL;

  if (gFormData->HighLightedStatement != NULL) {
    OpCodeBuf = gFormData->HighLightedStatement->OpCode;
  }

  if (gFormData->BrowserStatus == (BROWSER_WARNING_IF)) {
    ASSERT (OpCodeBuf != NULL && OpCodeBuf->OpCode == EFI_IFR_WARNING_IF_OP);
    if (OpCodeBuf != NULL && OpCodeBuf->OpCode == EFI_IFR_WARNING_IF_OP) {
      TimeOut     = ((EFI_IFR_WARNING_IF *) OpCodeBuf)->TimeOut;
      StringToken = ((EFI_IFR_WARNING_IF *) OpCodeBuf)->Warning;
    }
  } else {
    TimeOut = 0;
    if ((gFormData->BrowserStatus == (BROWSER_NO_SUBMIT_IF)) &&
        (OpCodeBuf != NULL && OpCodeBuf->OpCode == EFI_IFR_NO_SUBMIT_IF_OP)) {
      StringToken = ((EFI_IFR_NO_SUBMIT_IF *) OpCodeBuf)->Error;
    } else if ((gFormData->BrowserStatus == (BROWSER_INCONSISTENT_IF)) &&
               (OpCodeBuf != NULL && OpCodeBuf->OpCode == EFI_IFR_INCONSISTENT_IF_OP)) {
      StringToken = ((EFI_IFR_INCONSISTENT_IF *) OpCodeBuf)->Error;
    }
  }

  if (StringToken != 0) {
    ErrorInfo = GetString (StringToken, gCurrentSelection->FormSet->HiiHandle);
  } else if (gFormData->ErrorString != NULL) {
    //
    // Only used to compatible with old setup browser.
    // Not use this field in new browser core.
    //
    ErrorInfo = gFormData->ErrorString;
  } else {
    switch (gFormData->BrowserStatus) {
    case BROWSER_SUBMIT_FAIL:
      ErrorInfo = gSaveFailed;
      break;

    case BROWSER_FORM_NOT_FOUND:
      ErrorInfo = gFormNotFound;
      break;

    case BROWSER_FORM_SUPPRESS:
      ErrorInfo = gFormSuppress;
      break;

    case BROWSER_PROTOCOL_NOT_FOUND:
      ErrorInfo = gProtocolNotFound;
      break;

    default:
      ErrorInfo = gBrwoserError;
      break;
    }
  }

  if (ErrorInfo != NULL) {
    CreateSimpleDialog (H2O_FORM_BROWSER_D_TYPE_SELECTION, 0, NULL, 1, &ErrorInfo, 1, &Dialog);
  }

  if (StringToken != 0) {
    FreePool (ErrorInfo);
  }
}

EFI_STATUS
EFIAPI
FormDisplay (
  IN  FORM_DISPLAY_ENGINE_FORM  *FormData,
  OUT USER_INPUT                *UserInputData
  )
{
  H2O_DISPLAY_ENGINE_EVT        *Event;
  BOOLEAN                       ExitFlag;
  H2O_DISPLAY_ENGINE_EVT_TIMER  TimerEvent;

  ASSERT (FormData != NULL);
  if (FormData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  gUserInput = UserInputData;
  gFormData  = FormData;

  //
  // Process the status info first.
  //
  BrowserStatusProcess();
  if (UserInputData == NULL) {
    //
    // UserInputData == NULL, means only need to print the error info, return here.
    //
    return EFI_SUCCESS;
  }

  //
  // SetTimer  300ms
  // Because system timer isn't on time,
  // so Time / Date refresh rate should be littler than 1s;
  //
  if (!IsListEmpty (&mFBPrivate.RefreshList)) {
    //
    // It should set timer for each item in refresh list. Temporarily set 300ms to refresh.
    //
    TimerEvent.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_TIMER);
    TimerEvent.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER;
    TimerEvent.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
    TimerEvent.TimerId    = H2O_FORM_BROWSER_TIMER_ID_REFRESH_LIST;
    TimerEvent.Time       = 1000000000; // 1 second

    FBSetTimer (
      H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER,
      H2O_FORM_BROWSER_TIMER_ID_REFRESH_LIST,
      H2O_FORM_BROWSER_TIMER_TYPE_PERIODIC,
      (CONST H2O_DISPLAY_ENGINE_EVT *) &TimerEvent,
      TimerEvent.Time
      );
  }

  FBRepaint (&mFBPrivate);

  //
  // event loop
  //
  Event = NULL;
  while (1) {
    if (!GetNextEvent (&Event)) {
      continue;
    }
    FBEventCallback (Event, &ExitFlag);
    if (ExitFlag) {
      FreePool (Event);
      break;
    }
    FreePool (Event);
  }

  DestroyEventQueue (&mFBPrivate);

  return EFI_SUCCESS;
}

