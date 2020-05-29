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

#ifndef _UI_DATE_ITEM_H
#define _UI_DATE_ITEM_H

#include <Protocol/H2OFormBrowser.h>
#include "UiControls.h"

typedef struct _UI_DATE_ITEM        UI_DATE_ITEM;
typedef struct _UI_DATE_ITEM_CLASS  UI_DATE_ITEM_CLASS;

UI_DATE_ITEM_CLASS *
EFIAPI
GetDateItemClass (
  VOID
  );

typedef
VOID
(EFIAPI *UI_DATE_ITEM_ON_DATE_CHANGE) (
  UI_DATE_ITEM                  *This,
  EFI_TIME                      *EfiTime
  );

struct _UI_DATE_ITEM {
  UI_CONTROL                    Control;

  UI_NUMBER_PICKER              *YearNumberPicker;
  UI_NUMBER_PICKER              *MonthNumberPicker;
  UI_NUMBER_PICKER              *DayNumberPicker;
  UI_NUMBER_PICKER              *SelectedControl;

  //
  // XML defined attritube
  //
  EFI_TIME                      EfiTime;

  UI_DATE_ITEM_ON_DATE_CHANGE   OnDateChange;
};

struct _UI_DATE_ITEM_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};


#endif
