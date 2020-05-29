//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef UI_SCREEN_OPERATION_H
#define UI_SCREEN_OPERATION_H


typedef enum {
  UiNoOperation,
  UiDefault,
  UiDiscard,
  UiSelect,
  UiUp,
  UiDown,
  UiLeft,
  UiRight,
  UiReset,
  UiSave,
  UiSaveCustom,
  UiDefaultCustom,
  UiSaveAndExit,
  UiPrevious,
  UiPageUp,
  UiPageDown,
  UiJumpMenu,
  UiShowHelpScreen,
  UiMaxOperation
} UI_SCREEN_OPERATION;


typedef struct {
  UINT16              ScanCode;
  CHAR16              UnicodeChar;
  UI_SCREEN_OPERATION ScreenOperation;
} SCAN_CODE_TO_SCREEN_OPERATION;

#endif /* _UI_SCREEN_OPERATION_H */