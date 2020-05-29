/** @file

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

#ifndef _DE_SPECIFIC_QUESTION_BEHAVIOR_H_
#define _DE_SPECIFIC_QUESTION_BEHAVIOR_H_

#include <Protocol/SetupMouse.h>

CHAR16 *
GetQuestionValueStr (
  IN     H2O_FORM_BROWSER_Q                   *Question
  );

UINT64
GetBufferValue (
  IN     UINT8                                *Buffer,
  IN     UINT8                                Type,
  IN     UINT32                               Index
  );

EFI_STATUS
CheckSpecificQuestion (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY
  );

EFI_STATUS
ChangeQuestionValueByStep (
  IN  BOOLEAN                                 Increase,
  IN  H2O_FORM_BROWSER_Q                      *Question,
  OUT EFI_HII_VALUE                           *HiiValue
  );

EFI_STATUS
SendSelectQNotify (
  IN H2O_PAGE_ID                     PageId,
  IN EFI_QUESTION_ID                 QuestionId,
  IN EFI_IFR_OP_HEADER               *IfrOpCode
  );

EFI_STATUS
SendShutDNotify (
  IN H2O_FORM_BROWSER_PROTOCOL       *FBProtocol
  );

EFI_STATUS
SendOpenQNotify (
  IN H2O_PAGE_ID                     PageId,
  IN EFI_QUESTION_ID                 QuestionId,
  IN EFI_IFR_OP_HEADER               *IfrOpCode
  );

EFI_STATUS
SendSelectPNotify (
  IN H2O_PAGE_ID                     PageId
  );

EFI_STATUS
SendChangeQNotify (
  IN H2O_PAGE_ID                     PageId,
  IN EFI_QUESTION_ID                 QuestionId,
  IN EFI_HII_VALUE                   *HiiValue
  );

EFI_STATUS
SendDefaultNotify (
  VOID
  );

EFI_STATUS
SendSubmitExitNotify (
  VOID
  );

EFI_STATUS
SendDiscardExitNotify (
  VOID
  );

EFI_STATUS
SendSubmitNotify (
  VOID
  );

EFI_STATUS
SendDiscardNotify (
  VOID
  );

EFI_STATUS
SendRefreshNotify (
  VOID
  );

EFI_STATUS
SendShowHelpNotify (
  VOID
  );

H2O_CONTROL_INFO *
FindNextSelectableControl (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   ControlArrayCount,
  IN H2O_CONTROL_INFO                         *CurrentControl,
  IN BOOLEAN                                  GoDown,
  IN BOOLEAN                                  IsLoop
  );

BOOLEAN
IsValidHighlightStatement (
  IN H2O_CONTROL_INFO                            *StatementControl
  );

#endif
