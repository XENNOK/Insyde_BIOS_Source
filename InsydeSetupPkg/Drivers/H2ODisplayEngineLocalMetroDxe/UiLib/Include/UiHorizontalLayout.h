/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _UI_HORIZONTAL_LAYOUT_H
#define _UI_HORIZONTAL_LAYOUT_H

typedef struct _UI_HORIZONTAL_LAYOUT UI_HORIZONTAL_LAYOUT;
typedef struct _UI_HORIZONTAL_LAYOUT_CLASS UI_HORIZONTAL_LAYOUT_CLASS;

UI_HORIZONTAL_LAYOUT_CLASS  *
EFIAPI
GetHorizontalLayoutClass (
  VOID
  );

struct _UI_HORIZONTAL_LAYOUT {
  UI_CONTROL                    Container;

};

struct _UI_HORIZONTAL_LAYOUT_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};

#endif
