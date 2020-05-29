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

#ifndef _H2O_FORM_TITLE_PANEL_H
#define _H2O_FORM_TITLE_PANEL_H

typedef struct _H2O_FORM_TITLE_PANEL H2O_FORM_TITLE_PANEL;
typedef struct _H2O_FORM_TITLE_PANEL_CLASS H2O_FORM_TITLE_PANEL_CLASS;

H2O_FORM_TITLE_PANEL_CLASS *
EFIAPI
GetH2OFormTitlePanelClass (
  VOID
  );

struct _H2O_FORM_TITLE_PANEL {
  UI_CONTROL                    Parent;
};

struct _H2O_FORM_TITLE_PANEL_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};

#endif
