/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _UI_NUMBER_PICKER_H
#define _UI_NUMBER_PICKER_H

#include <Protocol/H2OFormBrowser.h>

typedef struct _UI_NUMBER_PICKER         UI_NUMBER_PICKER;
typedef struct _UI_NUMBER_PICKER_CLASS   UI_NUMBER_PICKER_CLASS;

UI_NUMBER_PICKER_CLASS *
EFIAPI
GetNumberPickerClass (
  VOID
  );

typedef enum {
  ALWAYS_DISPLAY_SELECTION = 0,
  DISPLAY_SELECTION_WHEN_HOVER,
  DISPLAY_SELECTION_WHEN_PRESS
} UI_NUMBER_PICKER_DISPLAY_MODE;

typedef
VOID
(EFIAPI *UI_NUMBER_PICKER_ON_CURRENT_INDEX_CHANGE) (
  UI_NUMBER_PICKER               *This,
  INT32                          Index
  );

typedef
VOID
(EFIAPI *UI_NUMBER_PICKER_ON_ITEM_SELECTED) (
  UI_NUMBER_PICKER              *This
  );

struct _UI_NUMBER_PICKER {
  UI_CONTROL                                Control;

  UI_CONTROL                                *UpArrowControl;
  UI_CONTROL                                *UpArrowImageControl;
  UI_CONTROL                                *PreviousValueControl;
  UI_CONTROL                                *CurrentValueControl;
  UI_CONTROL                                *NextValueControl;
  UI_CONTROL                                *DownArrowControl;
  UI_CONTROL                                *DownArrowImageControl;
  //
  // XML defined attritube
  //
  CHAR16                                    **List;
  UINT32                                    ListCount;
  INT32                                     CurrentIndex;
  BOOLEAN                                   IsLoop;
  UI_NUMBER_PICKER_DISPLAY_MODE             DisplayMode;
  INT32                                     PixelPerStep;

  UI_NUMBER_PICKER_ON_CURRENT_INDEX_CHANGE  OnCurrentIndexChange;
  UI_NUMBER_PICKER_ON_ITEM_SELECTED         OnItemSelected;
  //
  // Record user control status
  //
  BOOLEAN                                   Hover;
  POINT                                     ButtonDownPoint;
  INT32                                     ButtonDownListIndex;
  UI_CONTROL                                *ButtonDownControl;
};

struct _UI_NUMBER_PICKER_CLASS {
  UI_CONTROL_CLASS               ParentClass;
};

#endif
