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

#ifndef _UI_LIST_VIEW_H
#define _UI_LIST_VIEW_H

typedef struct _UI_LIST_VIEW UI_LIST_VIEW;
typedef struct _UI_LIST_VIEW_CLASS UI_LIST_VIEW_CLASS;

#define LIST_VIEW_CLASS(Control)                                      ((UI_LIST_VIEW_CLASS *)(((UI_CONTROL *)(Control))->Class))

UI_LIST_VIEW_CLASS *
EFIAPI
GetListViewClass (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *UI_LIST_VIEW_PERFORM_ITEM_CLICK) (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  );

typedef
BOOLEAN
(EFIAPI *UI_LIST_VIEW_SET_SELECTION) (
  UI_LIST_VIEW                  *This,
  INT32                         Index,
  BOOLEAN                       Forward
  );

typedef
INT32
(EFIAPI *UI_LIST_VIEW_GET_SELECTION) (
  UI_LIST_VIEW                  *This
  );

typedef
INT32
(EFIAPI *UI_LIST_VIEW_FIND_NEXT_SELECTION) (
  UI_LIST_VIEW                  *This,
  INT32                         Index,
  UINTN                         Direction
  );

typedef
EFI_STATUS
(EFIAPI *UI_LIST_VIEW_SWITCH_ITEMS) (
  UI_LIST_VIEW                  *This,
  INT32                         Index1,
  INT32                         Index2
  );

typedef
VOID
(EFIAPI *UI_LIST_VIEW_ON_ITEM_CLICK) (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  );

typedef
VOID
(EFIAPI *UI_LIST_VIEW_ON_ITEM_SELECTED) (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  );

typedef enum _UI_LIST_VIEW_LAYOUT {
  UiListViewVerticalLayout,
  UiListViewHorizontalLayout,
  UiListViewCellSpanningLayout,

} UI_LIST_VIEW_LAYOUT;


struct _UI_LIST_VIEW {
  UI_VERTICAL_LAYOUT            Container;
  INT32                         CurSel;
  UINT32                        CellWidth;
  UINT32                        CellHeight;

  UI_LIST_VIEW_LAYOUT           Layout;

  UI_LIST_VIEW_ON_ITEM_CLICK    OnItemClick;
  UI_LIST_VIEW_ON_ITEM_SELECTED OnItemSelected;

  BOOLEAN                       MoveItemSupport;
  RECT                          MoveItemOrgRc;
  POINT                         MoveItemOrgPt;
  INT32                         IntensityOfRepeatMoveToNext;
};


struct _UI_LIST_VIEW_CLASS {
  UI_VERTICAL_LAYOUT_CLASS         ParentClass;

  UI_LIST_VIEW_PERFORM_ITEM_CLICK  PerformItemClick;
  UI_LIST_VIEW_SET_SELECTION       SetSelection;
  UI_LIST_VIEW_GET_SELECTION       GetSelection;
  UI_LIST_VIEW_FIND_NEXT_SELECTION FindNextSelection;
  UI_LIST_VIEW_SWITCH_ITEMS        SwitchItems;
};

#endif
