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

#ifndef _UI_SETUP_MENU_ITEM_H
#define _UI_SETUP_MENU_ITEM_H

typedef struct _UI_SETUP_MENU_ITEM        UI_SETUP_MENU_ITEM;
typedef struct _UI_SETUP_MENU_ITEM_CLASS  UI_SETUP_MENU_ITEM_CLASS;

UI_SETUP_MENU_ITEM_CLASS *
EFIAPI
GetSetupMenuItemClass (
  VOID
  );

struct _UI_SETUP_MENU_ITEM {
  UI_CONTROL                    Control;
  SETUP_MENU_INFO               *SetupMenuInfo;
};

struct _UI_SETUP_MENU_ITEM_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};


#endif
