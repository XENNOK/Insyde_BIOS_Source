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

#ifndef _UI_VERTICLA_LAYOUT_H
#define _UI_VERTICLA_LAYOUT_H

typedef struct _UI_VERTICAL_LAYOUT UI_VERTICAL_LAYOUT;
typedef struct _UI_VERTICAL_LAYOUT_CLASS UI_VERTICAL_LAYOUT_CLASS;

UI_VERTICAL_LAYOUT_CLASS *
EFIAPI
GetVerticalLayoutClass (
  VOID
  );

struct _UI_VERTICAL_LAYOUT {
  UI_CONTROL                    Container;

};


struct _UI_VERTICAL_LAYOUT_CLASS {
  UI_CONTROL                    ParentClass;
};

#endif
