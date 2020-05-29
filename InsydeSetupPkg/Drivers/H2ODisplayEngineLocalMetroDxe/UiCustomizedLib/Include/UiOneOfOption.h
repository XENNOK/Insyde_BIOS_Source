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

#ifndef _UI_SETUP_PAGE_ITEM_H
#define _UI_SETUP_PAGE_ITEM_H

#include <Protocol/H2OFormBrowser.h>
typedef struct _UI_SETUP_PAGE_ITEM        UI_SETUP_PAGE_ITEM;
typedef struct _UI_SETUP_PAGE_ITEM_CLASS  UI_SETUP_PAGE_ITEM_CLASS;

UI_SETUP_PAGE_ITEM_CLASS *
EFIAPI
GetSetupPageItemClass (
  VOID
  );

struct _UI_AUTO_ADJUST_HEIGHT {
  BOOLEAN                       AutoHeight;
  UINTN                         PromptLines;
  INT32                         Width;
  INT32                         MinHeight;
};


struct _UI_SETUP_PAGE_ITEM {
  UI_CONTROL                    Control;
  H2O_FORM_BROWSER_S            *Statement;
};

struct _UI_SETUP_PAGE_ITEM_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};


#endif
