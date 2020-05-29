/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "UiControls.h"

#pragma pack(1)

typedef struct _CELL_INFO {
  UI_CONTROL *Control;
  INTN        CellX;
  INTN        CellY;
  UINTN       SpanX;
  UINTN       SpanY;
} CELL_INFO;

typedef struct _UI_CELL_LAYOUT UI_CELL_LAYOUT;

struct _UI_CELL_LAYOUT {
  UI_CONTAINER Container;
  VOID         (*SetCellInfo)(UI_CELL_LAYOUT *This, UINTN CellCount, CELL_INFO *CellInfo);
  UINTN        CellCount;
  CELL_INFO    *CellInfo;
  INTN         CellWidth;
  INTN         CellHeight;
  INTN         CountX;
  INTN         CountY;
  INTN         WidthGap;
  INTN         HeightGap;
};

#pragma pack()

UINT32
WINAPI
RegisterUiCellLayout (
  HINSTANCE hInstance
  );

