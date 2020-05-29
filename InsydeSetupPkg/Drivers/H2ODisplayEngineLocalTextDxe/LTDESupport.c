/** @file
  Display related functions for H2O display engine driver.
;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "H2ODisplayEngineLocalText.h"
#include "LTDEControl.h"
#include <Guid/BdsHii.h>

#define IN_RANGE(i, min, max) ((i >= min) && (i <= max)) ? TRUE : FALSE

#define IS_OVERLAP(Start1, End1, Start2, End2) ((End1 < Start2) || (Start1 > End2)) ? FALSE : TRUE

#define BORDER_LINE_CHAR_ADD_DOWN             BIT0
#define BORDER_LINE_CHAR_ADD_UP               BIT1
#define BORDER_LINE_CHAR_ADD_RIGHT            BIT2
#define BORDER_LINE_CHAR_ADD_LEFT             BIT3

/**
 Initialize layout definition into PanelList of private data

 @retval EFI_SUCCESS                Initialize layout definition successfully
 @retval EFI_NOT_FOUND              There is no panel information for layout
 @retval Other                      Fail to get screen field or layout data
**/
EFI_STATUS
ProcessLayout (
  VOID
  )
{
  EFI_STATUS                                     Status;
  RECT                                           ScreenField;
  H2O_LAYOUT_INFO                                *Layout;
  LIST_ENTRY                                     *PanelLink;
  H2O_PANEL_INFO                                 *Panel;
  H2O_FORM_BROWSER_SM                            *SetupMenuData;
  UINT32                                         LayoutId;
  INT32                                          BorderLineWidth;

  Status = GetScreenMaxXY ((UINT32 *) &ScreenField.right, (UINT32 *) &ScreenField.bottom);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ScreenField.left = 0;
  ScreenField.top  = 0;
  ScreenField.right--;
  ScreenField.bottom--;

  LayoutId = PcdGet32 (PcdLayoutIdLtde);
  Status = mDEPrivate->FBProtocol->GetSMInfo (mDEPrivate->FBProtocol, &SetupMenuData);
  if (!EFI_ERROR (Status)) {
    if (CompareGuid (&SetupMenuData->FormSetGuid, &gBootManagerFormSetGuid)) {
      LayoutId = PcdGet32 (PcdLayoutIdLtdeBootManager);
    } else if (CompareGuid (&SetupMenuData->FormSetGuid, &gBootMaintFormSetGuid) ||
               CompareGuid (&SetupMenuData->FormSetGuid, &gFileExploreFormSetGuid)) {
      LayoutId = PcdGet32 (PcdLayoutIdLtdeBootFromFile);
    }

    FreeSetupMenuData (SetupMenuData);
  }

  Status = GetLayoutById (LayoutId, &Layout);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PanelLink = &Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    PanelLink = PanelLink->ForwardLink;
    Panel     = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);

    if (Panel->PanelType == H2O_PANEL_TYPE_SETUP_MENU) {
      Panel->OrderFlag = H2O_PANEL_ORDER_FLAG_HORIZONTAL_FIRST;
    }

    GetPanelField (Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &ScreenField, &Panel->PanelField);

    SetRect (
      &Panel->PanelRelField,
      0,
      0,
      Panel->PanelField.right  - Panel->PanelField.left,
      Panel->PanelField.bottom - Panel->PanelField.top
      );

    Panel->WithScrollBarVertical    = FALSE;
    Panel->WithScrollBarHorizontal  = FALSE;
    Panel->ControlList.Count        = 0;
    Panel->ControlList.ControlArray = NULL;

    BorderLineWidth = GetBorderWidth (Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);
    if (BorderLineWidth > 0) {
      Panel->PanelRelField.bottom -= BorderLineWidth * 2;
      Panel->PanelRelField.right  -= BorderLineWidth * 2;
    }
  } while (!IsNodeAtEnd (&Layout->PanelListHead, PanelLink));

  mDEPrivate->Layout = Layout;

  return EFI_SUCCESS;
}

/**
 Display last char on the screen by specific color which defined in panel info

 @param[in] SimpleTextOut        The pointer of simple text out protocol for sepcific console device

 @retval EFI_SUCCESS             Success to display last char on the screen or the position of last char is not
                                 defined in any panel info
 @retval EFI_INVALID_PARAMETER   Input parameter is NULL
**/
EFI_STATUS
DisplayScreenLastChar (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL             *SimpleTextOut
  )
{
  LIST_ENTRY                                     *Link;
  H2O_PANEL_INFO                                 *Panel;
  H2O_PANEL_INFO                                 *TargetPanel;
  INT32                                          ScreenLastCharXValue;
  INT32                                          ScreenLastCharYValue;
  UINT32                                         PanelColorAttribute;
  INT32                                          OrgAttribute;
  INT32                                          OrgCursorColumn;
  INT32                                          OrgCursorRow;

  if (SimpleTextOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ScreenLastCharXValue = (INT32) (PcdGet32 (PcdDisplayEngineLocalTextColumn) - 1);
  ScreenLastCharYValue = (INT32) (PcdGet32 (PcdDisplayEngineLocalTextRow) - 1);
  TargetPanel          = NULL;

  Link = GetFirstNode (&mDEPrivate->Layout->PanelListHead);
  while (!IsNull (&mDEPrivate->Layout->PanelListHead, Link)) {
    Panel = H2O_PANEL_INFO_NODE_FROM_LINK (Link);

    if (Panel != NULL &&
        IsVisibility (Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL) &&
        Panel->PanelField.right  == ScreenLastCharXValue &&
        Panel->PanelField.bottom == ScreenLastCharYValue) {
      TargetPanel = Panel;
      break;
    }

    Link = GetNextNode (&mDEPrivate->Layout->PanelListHead, Link);
  }
  if (TargetPanel == NULL) {
    return EFI_SUCCESS;
  }

  GetPanelColorAttribute (TargetPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &PanelColorAttribute);
  OrgAttribute    = SimpleTextOut->Mode->Attribute;
  OrgCursorColumn = SimpleTextOut->Mode->CursorColumn;
  OrgCursorRow    = SimpleTextOut->Mode->CursorRow;

  SimpleTextOut->SetCursorPosition (SimpleTextOut, ScreenLastCharXValue, ScreenLastCharYValue);
  SimpleTextOut->SetAttribute (SimpleTextOut, PanelColorAttribute);
  SimpleTextOut->OutputString (SimpleTextOut, L" ");
  SimpleTextOut->SetAttribute (SimpleTextOut, OrgAttribute);
  SimpleTextOut->SetCursorPosition (SimpleTextOut, OrgCursorColumn, OrgCursorRow);

  return EFI_SUCCESS;
}

/**
 Add up line on border line char

 @param[in, out] BorderLineChar  Pointer to border line char
**/
VOID
BorderLineCharAddUp (
  IN OUT CHAR16                                  *BorderLineChar
  )
{
  if (BorderLineChar == NULL) {
    return;
  }

  switch (*BorderLineChar) {

  case BOXDRAW_HORIZONTAL:
    *BorderLineChar = BOXDRAW_UP_HORIZONTAL;
    break;

  case BOXDRAW_DOWN_RIGHT:
    *BorderLineChar = BOXDRAW_VERTICAL_RIGHT;
    break;

  case BOXDRAW_DOWN_LEFT:
    *BorderLineChar = BOXDRAW_VERTICAL_LEFT;
    break;

  case BOXDRAW_DOWN_HORIZONTAL:
    *BorderLineChar = BOXDRAW_VERTICAL_HORIZONTAL;
    break;
  }
}

/**
 Add down line on border line char

 @param[in, out] BorderLineChar  Pointer to border line char
**/
VOID
BorderLineCharAddDown (
  IN OUT CHAR16                                  *BorderLineChar
  )
{
  if (BorderLineChar == NULL) {
    return;
  }

  switch (*BorderLineChar) {

  case BOXDRAW_HORIZONTAL:
    *BorderLineChar = BOXDRAW_DOWN_HORIZONTAL;
    break;

  case BOXDRAW_UP_RIGHT:
    *BorderLineChar = BOXDRAW_VERTICAL_RIGHT;
    break;

  case BOXDRAW_UP_LEFT:
    *BorderLineChar = BOXDRAW_VERTICAL_LEFT;
    break;

  case BOXDRAW_UP_HORIZONTAL:
    *BorderLineChar = BOXDRAW_VERTICAL_HORIZONTAL;
    break;
  }
}

/**
 Add left line on border line char

 @param[in, out] BorderLineChar  Pointer to border line char
**/
VOID
BorderLineCharAddLeft (
  IN OUT CHAR16                                  *BorderLineChar
  )
{
  if (BorderLineChar == NULL) {
    return;
  }

  switch (*BorderLineChar) {

  case BOXDRAW_VERTICAL:
    *BorderLineChar = BOXDRAW_VERTICAL_LEFT;
    break;

  case BOXDRAW_UP_RIGHT:
    *BorderLineChar = BOXDRAW_UP_HORIZONTAL;
    break;

  case BOXDRAW_DOWN_RIGHT:
    *BorderLineChar = BOXDRAW_DOWN_HORIZONTAL;
    break;

  case BOXDRAW_VERTICAL_RIGHT:
    *BorderLineChar = BOXDRAW_VERTICAL_HORIZONTAL;
    break;
  }
}

/**
 Add right line on border line char

 @param[in, out] BorderLineChar  Pointer to border line char
**/
VOID
BorderLineCharAddRight (
  IN OUT CHAR16                                  *BorderLineChar
  )
{
  if (BorderLineChar == NULL) {
    return;
  }

  switch (*BorderLineChar) {

  case BOXDRAW_VERTICAL:
    *BorderLineChar = BOXDRAW_VERTICAL_RIGHT;
    break;

  case BOXDRAW_UP_LEFT:
    *BorderLineChar = BOXDRAW_UP_HORIZONTAL;
    break;

  case BOXDRAW_DOWN_LEFT:
    *BorderLineChar = BOXDRAW_DOWN_HORIZONTAL;
    break;

  case BOXDRAW_VERTICAL_LEFT:
    *BorderLineChar = BOXDRAW_VERTICAL_HORIZONTAL;
    break;
  }
}

/**
 Update border line char by action

 @param[in]      BorderLineCharAction  Add action of border line char
 @param[in, out] BorderLineChar        Pointer to border line char
**/
VOID
UpdateBorderLineChar (
  IN     UINT32                                  BorderLineCharAction,
  IN OUT CHAR16                                  *BorderLineChar
  )
{
  if ((BorderLineCharAction & BORDER_LINE_CHAR_ADD_DOWN) != 0) {
    BorderLineCharAddDown (BorderLineChar);
  }
  if ((BorderLineCharAction & BORDER_LINE_CHAR_ADD_UP) != 0) {
    BorderLineCharAddUp (BorderLineChar);
  }
  if ((BorderLineCharAction & BORDER_LINE_CHAR_ADD_RIGHT) != 0) {
    BorderLineCharAddRight (BorderLineChar);
  }
  if ((BorderLineCharAction & BORDER_LINE_CHAR_ADD_LEFT) != 0) {
    BorderLineCharAddLeft (BorderLineChar);
  }
}

/**
 Check if two fields are adjacent.

 @param[in]      BorderLineCharAction  Add action of border line char
 @param[in, out] BorderLineChar        Pointer to border line char
**/
BOOLEAN
IsFieldAdjacent (
  IN RECT                                        *Field1,
  IN RECT                                        *Field2
  )
{
  if (((Field1->right  == Field2->left  ) && (IS_OVERLAP (Field1->top , Field1->bottom, Field2->top , Field2->bottom))) ||
      ((Field1->left   == Field2->right ) && (IS_OVERLAP (Field1->top , Field1->bottom, Field2->top , Field2->bottom))) ||
      ((Field1->bottom == Field2->top   ) && (IS_OVERLAP (Field1->left, Field1->right , Field2->left, Field2->right ))) ||
      ((Field1->top    == Field2->bottom) && (IS_OVERLAP (Field1->left, Field1->right , Field2->left, Field2->right )))) {
    return TRUE;
  }

  return FALSE;
}

/**
 Update border line strings

 @param[in]      CurrentPanelField     Current panel field
 @param[in]      AdjacentPanelField    Adjacent panel field
 @param[in]      HorizonalLineWidth    Horizonal line width
 @param[in, out] TopHorizonalLine      Top horizonal border line string
 @param[in, out] BorderLineChar        Bottom horizonal border line string
 @param[in, out] BorderLineChar        Left vertical border line string
 @param[in, out] BorderLineChar        Right vertical border line string
**/
EFI_STATUS
UpdateBorderLineStr (
  IN     RECT                                    *CurrentPanelField,
  IN     RECT                                    *AdjacentPanelField,
  IN     UINT32                                  HorizonalLineWidth,
  IN OUT CHAR16                                  *TopHorizonalLine,
  IN OUT CHAR16                                  *BottomHorizonalLine,
  IN OUT CHAR16                                  *LeftVerticalLine,
  IN OUT CHAR16                                  *RightVerticalLine
  )
{
  if (CurrentPanelField->right == AdjacentPanelField->left) {
    if (IN_RANGE (CurrentPanelField->top, AdjacentPanelField->top, AdjacentPanelField->bottom)) {
      if (CurrentPanelField->top == AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN | BORDER_LINE_CHAR_ADD_RIGHT, &TopHorizonalLine[HorizonalLineWidth - 1]);
      } else if (CurrentPanelField->top == AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_RIGHT, &TopHorizonalLine[HorizonalLineWidth - 1]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_DOWN, &TopHorizonalLine[HorizonalLineWidth - 1]);
      }
    } else {
      if (CurrentPanelField->bottom > AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT, &RightVerticalLine[AdjacentPanelField->top - CurrentPanelField->top - 1]);
      }
    }

    if (IN_RANGE (CurrentPanelField->bottom, AdjacentPanelField->top, AdjacentPanelField->bottom)) {
      if (CurrentPanelField->bottom == AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN | BORDER_LINE_CHAR_ADD_RIGHT, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      } else if (CurrentPanelField->bottom == AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_RIGHT, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      }
    } else {
      if (CurrentPanelField->top < AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT, &RightVerticalLine[AdjacentPanelField->bottom - CurrentPanelField->top - 1]);
      }
    }

    return EFI_SUCCESS;
  }

  if (CurrentPanelField->left == AdjacentPanelField->right) {
    if (IN_RANGE (CurrentPanelField->top, AdjacentPanelField->top, AdjacentPanelField->bottom)) {
      if (CurrentPanelField->top == AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN | BORDER_LINE_CHAR_ADD_LEFT, &TopHorizonalLine[0]);
      } else if (CurrentPanelField->top == AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_LEFT, &TopHorizonalLine[0]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_DOWN, &TopHorizonalLine[0]);
      }
    } else {
      if (CurrentPanelField->bottom > AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT, &LeftVerticalLine[AdjacentPanelField->top - CurrentPanelField->top - 1]);
      }
    }

    if (IN_RANGE (CurrentPanelField->bottom, AdjacentPanelField->top, AdjacentPanelField->bottom)) {
      if (CurrentPanelField->bottom == AdjacentPanelField->top) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN | BORDER_LINE_CHAR_ADD_LEFT, &BottomHorizonalLine[0]);
      } else if (CurrentPanelField->bottom == AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_LEFT, &BottomHorizonalLine[0]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[0]);
      }
    } else {
      if (CurrentPanelField->top < AdjacentPanelField->bottom) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT, &LeftVerticalLine[AdjacentPanelField->bottom - CurrentPanelField->top - 1]);
      }
    }

    return EFI_SUCCESS;
  }

  if (CurrentPanelField->bottom == AdjacentPanelField->top) {
    if (IN_RANGE (CurrentPanelField->left, AdjacentPanelField->left, AdjacentPanelField->right)) {
      if (CurrentPanelField->left == AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[0]);
      } else if (CurrentPanelField->left == AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[0]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_LEFT, &BottomHorizonalLine[0]);
      }
    } else {
      if (CurrentPanelField->right > AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[AdjacentPanelField->left - CurrentPanelField->left]);
      }
    }

    if (IN_RANGE (CurrentPanelField->right, AdjacentPanelField->left, AdjacentPanelField->right)) {
      if (CurrentPanelField->right == AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      } else if (CurrentPanelField->right == AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT | BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_LEFT, &BottomHorizonalLine[HorizonalLineWidth - 1]);
      }
    } else {
      if (CurrentPanelField->left < AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_DOWN, &BottomHorizonalLine[AdjacentPanelField->right - CurrentPanelField->left]);
      }
    }

    return EFI_SUCCESS;
  }

  if (CurrentPanelField->top == AdjacentPanelField->bottom) {
    if (IN_RANGE (CurrentPanelField->left, AdjacentPanelField->left, AdjacentPanelField->right)) {
      if (CurrentPanelField->left == AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[0]);
      } else if (CurrentPanelField->left == AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT | BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[0]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_LEFT, &TopHorizonalLine[0]);
      }
    } else {
      if (CurrentPanelField->right > AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[AdjacentPanelField->left - CurrentPanelField->left]);
      }
    }

    if (IN_RANGE (CurrentPanelField->right, AdjacentPanelField->left, AdjacentPanelField->right)) {
      if (CurrentPanelField->right == AdjacentPanelField->left) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[HorizonalLineWidth - 1]);
      } else if (CurrentPanelField->right == AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_LEFT | BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[HorizonalLineWidth - 1]);
      } else {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_RIGHT | BORDER_LINE_CHAR_ADD_LEFT, &TopHorizonalLine[HorizonalLineWidth - 1]);
      }
    } else {
      if (CurrentPanelField->left < AdjacentPanelField->right) {
        UpdateBorderLineChar (BORDER_LINE_CHAR_ADD_UP, &TopHorizonalLine[AdjacentPanelField->right - CurrentPanelField->left]);
      }
    }

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;;
}

/**
 Print border line for current panel field.
 It will also deal with the border line char which is overlapped with previous panel field.

 @param[in] CurrentPanelField          Current panel field
 @param[in] PreviousPanelField         Previous panel field list
 @param[in] PreviousPanelFieldCount    Previous panel field list count
**/
EFI_STATUS
PrintBorderLineFunc (
  IN RECT                                        *CurrentPanelField,
  IN RECT                                        *PreviousPanelField,
  IN UINT32                                      PreviousPanelFieldCount
  )
{
  UINT32                                         Index;
  UINT32                                         HorizonalLineWidth;
  UINT32                                         VerticalLineHeight;
  CHAR16                                         *TopHorizonalLine;
  CHAR16                                         *BottomHorizonalLine;
  CHAR16                                         *LeftVerticalLine;
  CHAR16                                         *RightVerticalLine;
  EFI_STATUS                                     Status;

  //
  // Initialize horizonal and vetical border line data for current field
  //
  HorizonalLineWidth = (UINT32) (CurrentPanelField->right  - CurrentPanelField->left + 1);
  VerticalLineHeight = (UINT32) (CurrentPanelField->bottom - CurrentPanelField->top  + 1) - 2;

  TopHorizonalLine    = AllocatePool ((HorizonalLineWidth + 1) * sizeof (CHAR16));
  BottomHorizonalLine = AllocatePool ((HorizonalLineWidth + 1) * sizeof (CHAR16));
  LeftVerticalLine    = AllocatePool (VerticalLineHeight * sizeof (CHAR16));
  RightVerticalLine   = AllocatePool (VerticalLineHeight * sizeof (CHAR16));
  if (TopHorizonalLine == NULL || BottomHorizonalLine == NULL || LeftVerticalLine == NULL || RightVerticalLine == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TopHorizonalLine[0]                         = BOXDRAW_DOWN_RIGHT;
  TopHorizonalLine[HorizonalLineWidth - 1]    = BOXDRAW_DOWN_LEFT;
  BottomHorizonalLine[0]                      = BOXDRAW_UP_RIGHT;
  BottomHorizonalLine[HorizonalLineWidth - 1] = BOXDRAW_UP_LEFT;
  for (Index = 1; Index < HorizonalLineWidth - 1; Index++) {
    TopHorizonalLine[Index]    = BOXDRAW_HORIZONTAL;
    BottomHorizonalLine[Index] = BOXDRAW_HORIZONTAL;
  }
  TopHorizonalLine[HorizonalLineWidth]    = CHAR_NULL;
  BottomHorizonalLine[HorizonalLineWidth] = CHAR_NULL;

  for (Index = 0; Index < VerticalLineHeight; Index++) {
    LeftVerticalLine[Index]  = BOXDRAW_VERTICAL;
    RightVerticalLine[Index] = BOXDRAW_VERTICAL;
  }

  //
  // Update line data when current field cover to previous fields
  //
  for (Index = 0; Index < PreviousPanelFieldCount; Index++) {
    if (!IsFieldAdjacent (CurrentPanelField, &PreviousPanelField[Index])) {
      continue;
    }
    Status = UpdateBorderLineStr (
               CurrentPanelField,
               &PreviousPanelField[Index],
               HorizonalLineWidth,
               TopHorizonalLine,
               BottomHorizonalLine,
               LeftVerticalLine,
               RightVerticalLine
               );
    ASSERT (Status == EFI_SUCCESS);
  }

  //
  // Print border line of current field
  //
  DisplayString (CurrentPanelField->left, CurrentPanelField->bottom, L"%s", BottomHorizonalLine);
  DisplayString (CurrentPanelField->left, CurrentPanelField->top   , L"%s", TopHorizonalLine);

  for (Index = 0; Index < VerticalLineHeight; Index++) {
    DisplayString (CurrentPanelField->left , CurrentPanelField->top + 1 + Index, L"%c", LeftVerticalLine[Index]);
    DisplayString (CurrentPanelField->right, CurrentPanelField->top + 1 + Index, L"%c", RightVerticalLine[Index]);
  }

  FreePool ((VOID *) TopHorizonalLine);
  FreePool ((VOID *) BottomHorizonalLine);
  FreePool ((VOID *) LeftVerticalLine);
  FreePool ((VOID *) RightVerticalLine);

  return EFI_SUCCESS;
}

/**
 Display layout which includes screen last character and the border line for each panel.

 @retval EFI_SUCCESS                Display layout successfully
 @retval EFI_OUT_OF_RESOURCES       Allocate pool fail
**/
EFI_STATUS
DisplayLayout (
  VOID
  )
{
  UINT32                                         PanelColorAttribute;
  LIST_ENTRY                                     *Link;
  H2O_PANEL_INFO                                 *CurrentPanel;
  RECT                                           *PreviousPanelField;
  UINT32                                         PreviousPanelFieldLength;
  UINT32                                         PreviousPanelFieldNum;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE              *ConDevNode;

  //
  // Display screen last character when console device is in initial status.
  //
  Link = GetFirstNode (&mDEPrivate->ConsoleDevListHead);
  while (!IsNull (&mDEPrivate->ConsoleDevListHead, Link)) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    if (ConDevNode != NULL && ConDevNode->ConDevStatus == CONSOLE_DEVICE_STATUS_INIT) {
      DisplayScreenLastChar (ConDevNode->SimpleTextOut);
      ConDevNode->ConDevStatus = CONSOLE_DEVICE_STATUS_READY;
    }

    Link = GetNextNode (&mDEPrivate->ConsoleDevListHead, Link);
  }

  //
  // Display border line for each panel
  //
  PreviousPanelFieldLength = 10;
  PreviousPanelField       = AllocatePool (PreviousPanelFieldLength * sizeof (RECT));
  if (PreviousPanelField == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PreviousPanelFieldNum = 0;
  Link = GetFirstNode (&mDEPrivate->Layout->PanelListHead);
  while (!IsNull (&mDEPrivate->Layout->PanelListHead, Link)) {
    CurrentPanel = H2O_PANEL_INFO_NODE_FROM_LINK (Link);
    if (CurrentPanel != NULL &&
        IsVisibility (CurrentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL) &&
        GetBorderWidth (CurrentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL) > 0) {
      GetPanelColorAttribute (CurrentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &PanelColorAttribute);
      DEConOutSetAttribute (mDEPrivate, PanelColorAttribute);
      PrintBorderLineFunc (&CurrentPanel->PanelField, PreviousPanelField, PreviousPanelFieldNum);

      if (PreviousPanelFieldNum == PreviousPanelFieldLength) {
        PreviousPanelField = ReallocatePool (PreviousPanelFieldLength * sizeof (RECT), (PreviousPanelFieldLength + 10) * sizeof (RECT), PreviousPanelField);
        if (PreviousPanelField == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        PreviousPanelFieldLength += 10;
      }

      CopyRect (&PreviousPanelField[PreviousPanelFieldNum], &CurrentPanel->PanelField);
      PreviousPanelFieldNum++;
    }

    Link = GetNextNode (&mDEPrivate->Layout->PanelListHead, Link);
  }

  FreePool ((VOID *) PreviousPanelField);

  return EFI_SUCCESS;
}

/**
 Display setup menu panel

 @param [in] FBProtocol             Pointer to H2O form browser protocol

 @retval EFI_SUCCESS                Display setup menu panel successfully
 @retval EFI_NOT_FOUND              Can not find the setup menu panel info

**/
EFI_STATUS
DisplaySetupMenu (
  IN H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol
  )
{
  UINT32                                         Index;
  H2O_CONTROL_LIST                               *SetupMenuControls;
  RECT                                           SetupMenuField;
  RECT                                           CombineFields;
  H2O_PANEL_INFO                                 *SetupMenuPanel;
  H2O_COLOR_INFO                                 SetupMenuPanelColor;
  UINT32                                         X;
  UINT32                                         Y;
  UINT32                                         StartX;
  UINT32                                         StringWidth;
  LIST_ENTRY                                     *PanelLink;
  H2O_CONTROL_INFO                               *Control;

  if (FBProtocol == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  SetupMenuPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_MENU);
  if (SetupMenuPanel == NULL) {
    return EFI_NOT_FOUND;
  }

  if (!IsVisibility (SetupMenuPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) {
    return EFI_SUCCESS;
  }

  SetupMenuControls = &SetupMenuPanel->ControlList;
  CopyRect (&SetupMenuField, &SetupMenuPanel->PanelField);

  GetBackgroundColor (SetupMenuPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &SetupMenuPanelColor);

  //
  // Set the position of each setup menu item which is relative to PanelRelField of parent panel
  //
  StartX = 1;
  X      = StartX;
  Y      = 0;
  for (Index = 0; Index < SetupMenuControls->Count; Index++) {
    Control     = &SetupMenuControls->ControlArray[Index];
    StringWidth = (UINT32) (GetStringWidth (Control->Text.String) / sizeof (CHAR16)) - 1;

    SetRect (
      &Control->ControlField,
      X,
      Y,
      X + StringWidth - 1,
      Y
      );
    X = Control->ControlField.right + 1;

    if ((Control->PageId >> 16) != (FBProtocol->CurrentP->PageId >> 16)) {
      DisplayNormalControls (1, Control);
    }
  }

  CopyRect (&CombineFields, &SetupMenuField);
  CombineFields.right = SetupMenuField.left + SetupMenuControls->ControlArray[0].ControlField.left - StartX;
  ClearField (SetupMenuPanelColor.Attribute, &CombineFields, NULL);

  CombineFields.left  = SetupMenuField.left + SetupMenuControls->ControlArray[SetupMenuControls->Count - 1].ControlField.right + 1;
  CombineFields.right = SetupMenuField.right;
  ClearField (SetupMenuPanelColor.Attribute, &CombineFields, NULL);

  return EFI_SUCCESS;
}

/**
 Display hot key panel

 @retval EFI_SUCCESS                Display hoy key panel successfully
 @retval EFI_NOT_FOUND              Can not find the hot key panel info

**/
EFI_STATUS
DisplayHotkey (
  VOID
  )
{
  UINT32                                         Index;
  UINT32                                         HotKeyCount;
  H2O_CONTROL_INFO                               *HotKeyDescriptionControls;
  H2O_CONTROL_INFO                               *HotKeyControls;
  RECT                                           HotkeyField;
  RECT                                           EmptyField;
  H2O_PANEL_INFO                                 *HotKeyPanel;
  UINT32                                         PanelColorAttribute;
  INT32                                          X;
  INT32                                          Y;
  UINT32                                         HotKeyPanelHeight;
  UINT32                                         HotKeyPanelWidth;
  UINT32                                         KeyCountPerLine;
  UINT32                                         ItemWidth;
  UINT32                                         RemainderWidth;
  UINT32                                         RowIndex;
  UINT32                                         HotKeyControlWidth;
  LIST_ENTRY                                     *PanelLink;
  EFI_STATUS                                     Status;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  HotKeyPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_HOTKEY);
  if (HotKeyPanel == NULL) {
    return EFI_NOT_FOUND;
  }

  HotKeyCount               = HotKeyPanel->ControlList.Count / 2;
  HotKeyControls            = HotKeyPanel->ControlList.ControlArray;
  HotKeyDescriptionControls = HotKeyPanel->ControlList.ControlArray + HotKeyCount;
  CopyRect (&HotkeyField, &HotKeyPanel->PanelField);

  Status = GetPanelColorAttribute (HotKeyPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &PanelColorAttribute);
  if (EFI_ERROR (Status)) {
    PanelColorAttribute = PANEL_COLOR_ATTRIBUTE_HOT_KEY;
  }

  //
  // Init hot key bar location
  //
  HotKeyPanelHeight  = (UINT32) (HotkeyField.bottom - HotkeyField.top + 1);
  HotKeyPanelWidth   = (UINT32) (HotkeyField.right - HotkeyField.left + 1);
  KeyCountPerLine    = (HotKeyCount + 1) / HotKeyPanelHeight;
  ItemWidth          = (HotKeyPanelWidth - 2) / KeyCountPerLine;
  RemainderWidth     = (HotKeyPanelWidth - 2) % KeyCountPerLine;
  HotKeyControlWidth = 0;
  X                  = 0;

  //
  // Set the location of hot key item and print it.
  //
  for (Index = 0; Index < HotKeyCount; Index++) {
    //
    // Print each hot key item in vertical sequence. Item is composed of HotKeyControls string and HotKeyDescriptionControls string.
    // When start to print first item of current row, calculate the width of HotKeyControls, item width and x value of current row.
    //
    if ((Index % HotKeyPanelHeight) == 0) {
      HotKeyControlWidth = 0;
      for (RowIndex = 0; RowIndex < HotKeyPanelHeight; RowIndex++) {
        if (Index + RowIndex >= HotKeyCount) {
          break;
        }
        HotKeyControlWidth = MAX (HotKeyControlWidth, (UINT32) StrLen (HotKeyControls[Index + RowIndex].Text.String));
      }

      if (Index + HotKeyPanelHeight >= HotKeyCount) {
        ItemWidth += RemainderWidth;
      }

      X = (Index < HotKeyPanelHeight) ? 1 : HotKeyDescriptionControls[Index - HotKeyPanelHeight].ControlField.right + 1;
    }

    Y = Index % HotKeyPanelHeight;

    SetRect (
      &HotKeyControls[Index].ControlField,
      X,
      Y,
      X + (INT32) StrLen (HotKeyControls[Index].Text.String) - 1,
      Y
      );
    SetRect (
      &HotKeyDescriptionControls[Index].ControlField,
      X + (INT32) HotKeyControlWidth + 1,
      Y,
      X + ItemWidth - 1,
      Y
      );
    SetRect (
      &EmptyField,
      X + (INT32) StrLen (HotKeyControls[Index].Text.String),
      HotkeyField.top + Y,
      X + HotKeyControlWidth,
      HotkeyField.top + Y
      );

    DisplayControls (FALSE, FALSE, 1, &HotKeyControls[Index]);
    DisplayControls (FALSE, FALSE, 1, &HotKeyDescriptionControls[Index]);
    ClearField (PanelColorAttribute, &EmptyField, NULL);
  }

  //
  // Clear empty field
  //
  if (Index % HotKeyPanelHeight != 0) {
    for (; Index % HotKeyPanelHeight != 0; Index++) {
      Y = Index % HotKeyPanelHeight;
      SetRect (
        &EmptyField,
        X,
        HotkeyField.top + Y,
        X + ItemWidth - 1,
        HotkeyField.top + Y
        );
      ClearField (PanelColorAttribute, &EmptyField, NULL);
    }
  }

  SetRect (&EmptyField, HotkeyField.left, HotkeyField.top, HotkeyField.left, HotkeyField.bottom);
  ClearField (PanelColorAttribute, &EmptyField, NULL);

  if ((HotkeyField.bottom == (INT32) (PcdGet32 (PcdDisplayEngineLocalTextRow) - 1)) &&
      (HotkeyField.right  == (INT32) (PcdGet32 (PcdDisplayEngineLocalTextColumn) - 1))) {
    if (HotKeyPanelHeight > 1) {
      SetRect (&EmptyField, HotkeyField.right, HotkeyField.top, HotkeyField.right, HotkeyField.bottom - 1);
      ClearField (PanelColorAttribute, &EmptyField, NULL);
    }
  } else {
    SetRect (&EmptyField, HotkeyField.right, HotkeyField.top, HotkeyField.right, HotkeyField.bottom);
    ClearField (PanelColorAttribute, &EmptyField, NULL);
  }

  return EFI_SUCCESS;
}

/**
 Display title panel

 @param [in] TitleString            Title string
 @param [in] CoreVersionString      Core version string

 @retval EFI_SUCCESS                Display title panel successfully
 @retval EFI_NOT_FOUND              Can not find the title panel info

**/
EFI_STATUS
DisplayTitle (
  IN     CONST CHAR16                        *TitleString OPTIONAL,
  IN     CONST CHAR16                        *CoreVersionString OPTIONAL
  )
{
  H2O_PANEL_INFO                             *TitlePanel;
  LIST_ENTRY                                 *PanelLink;
  INT32                                      BorderLineWidth;
  H2O_CONTROL_INFO                           Control;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  TitlePanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_FORM_TITLE);
  if (TitlePanel == NULL) {
    DEBUG ((EFI_D_INFO, "[Local text display engine] Can not find the title panel info \n"));
    return EFI_NOT_FOUND;
  }

  ZeroMem (&Control, sizeof(H2O_CONTROL_INFO));
  Control.ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;
  Control.ParentPanel              = TitlePanel;

  if (TitleString != NULL) {
    Control.Text.String = (CHAR16 *) TitleString;

    BorderLineWidth = GetBorderWidth (TitlePanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);
    SetRect (
      &Control.ControlField,
      0,
      0,
      TitlePanel->PanelField.right - TitlePanel->PanelField.left - 2 * BorderLineWidth,
      TitlePanel->PanelField.bottom - TitlePanel->PanelField.top - 2 * BorderLineWidth
      );

    DisplayCenteredControl (FALSE, 1, &Control);
  }

  if (CoreVersionString != NULL) {
    Control.Text.String = (CHAR16 *) CoreVersionString;
    SetRect (
      &Control.ControlField,
      TitlePanel->PanelField.right - 10,
      0,
      TitlePanel->PanelField.right - TitlePanel->PanelField.left,
      0
      );

    DisplayNormalControls (1, &Control);
  }

  return EFI_SUCCESS;
}

/**
 Display form panel

 @param [in] FBProtocol             Pointer to H2O form browser protocol

 @retval EFI_SUCCESS                Display form panel successfully
 @retval EFI_NOT_FOUND              Can not find the form and help panel info

**/
EFI_STATUS
DisplayMenu (
  IN H2O_FORM_BROWSER_PROTOCOL               *FBProtocol
  )
{
  EFI_STATUS                                 Status;
  RECT                                       ClearedField;
  RECT                                       PromptField;
  UINT32                                     Index;
  H2O_CONTROL_LIST                           PromptControlList;
  H2O_CONTROL_LIST                           ValueControlList;
  H2O_CONTROL_LIST                           *HelpControlList;
  UINT32                                     MenuCount;
  H2O_PANEL_INFO                             *SetupPagePanel;
  H2O_PANEL_INFO                             *HelpTextPanel;
  H2O_PANEL_INFO                             PanelArray[2];
  H2O_CONTROL_LIST                           ControlListArray[2];
  UINT32                                     ColorAttribute;
  LIST_ENTRY                                 *PanelListHead;
  INT32                                      SetupPageBottom;
  INT32                                      HelpTextHeight;
  H2O_CONTROL_INFO                           *Control;
  INT32                                      BorderLineWidth;

  PanelListHead = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelListHead, PanelListHead->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  HelpControlList         = NULL;
  MenuCount               = 0;
  SetupPageBottom         = 0;
  HelpTextHeight          = 0;

  //
  // Set location
  //
  InitControlList (&PromptControlList);
  InitControlList (&ValueControlList);

  SetupPagePanel = GetPanelInfoByType (PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE);
  if (SetupPagePanel != NULL) {
    CopyPanel (&PanelArray[0], SetupPagePanel);
    CopyPanel (&PanelArray[1], SetupPagePanel);
    PanelArray[0].PanelField.right = PanelArray[0].PanelField.right / 2;
    PanelArray[1].PanelField.left  = PanelArray[1].PanelField.right / 2 + 1;

    MenuCount                      = SetupPagePanel->ControlList.Count / 2;
    PromptControlList.Count        = MenuCount;
    PromptControlList.ControlArray = SetupPagePanel->ControlList.ControlArray;
    ValueControlList.Count         = MenuCount;
    ValueControlList.ControlArray  = SetupPagePanel->ControlList.ControlArray + MenuCount;
    CopyMem (&ControlListArray[0], &PromptControlList, sizeof (H2O_CONTROL_LIST));
    CopyMem (&ControlListArray[1], &ValueControlList , sizeof (H2O_CONTROL_LIST));

    Status = GetControlsField (
               SetupPagePanel,
               2,
               &PanelArray[0],
               2,
               &ControlListArray[0]
               );
    for (Index = 0; Index < ValueControlList.Count; Index ++) {
      ValueControlList.ControlArray[Index].ControlField.left  += (SetupPagePanel->PanelField.right - SetupPagePanel->PanelField.left) / 2;
      ValueControlList.ControlArray[Index].ControlField.right += (SetupPagePanel->PanelField.right - SetupPagePanel->PanelField.left) / 2 + 1;
    }

    BorderLineWidth = GetBorderWidth (SetupPagePanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);
    SetupPageBottom = SetupPagePanel->PanelField.bottom - SetupPagePanel->PanelField.top;
    SetupPageBottom -= (BorderLineWidth * 2);// Two lines for border line.
  }

  HelpTextPanel = GetPanelInfoByType (PanelListHead, H2O_PANEL_TYPE_HELP_TEXT);
  if (HelpTextPanel != NULL) {
    HelpControlList = &HelpTextPanel->ControlList;
    BorderLineWidth = GetBorderWidth (HelpTextPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

    SetRect (
      &PanelArray[0].PanelField,
      0,
      0,
      (HelpTextPanel->PanelField.right - HelpTextPanel->PanelField.left - 2 * BorderLineWidth),
      (HelpTextPanel->PanelField.bottom - HelpTextPanel->PanelField.top - 2 * BorderLineWidth)
      );

    for (Index = 0; Index < HelpControlList->Count; Index++) {
      Control = &HelpControlList->ControlArray[Index];

      GetStringField (
        &Control->ControlField,
        &PanelArray[0].PanelField,
        NULL,
        Control->Text.String
        );

      Control->ControlField.left = 0;

      if (Control->ControlField.right < PanelArray[0].PanelField.right) {
        Control->ControlField.right = PanelArray[0].PanelField.right;
      }
      if (Control->ControlField.bottom < PanelArray[0].PanelField.bottom) {
        Control->ControlField.bottom = PanelArray[0].PanelField.bottom;
      }
    }

    HelpTextHeight = HelpTextPanel->PanelField.bottom - HelpTextPanel->PanelField.top + 1;
  }

  for (Index = 0; Index < MenuCount; Index ++) {
    //
    // Decide whether panel should have a vertical scroll bar.
    //
    if ((PromptControlList.ControlArray[Index].ControlField.bottom > SetupPageBottom) ||
        (ValueControlList.ControlArray[Index].ControlField.bottom  > SetupPageBottom)) {
      SetupPagePanel->WithScrollBarVertical = TRUE;
    }
    if (HelpTextPanel != NULL && HelpControlList->ControlArray[Index].ControlField.bottom >= HelpTextHeight) {
      HelpTextPanel->WithScrollBarVertical = TRUE;
    }
  }

  //
  // Clear Current Prompt and Value
  //
  if (SetupPagePanel != NULL) {
    SetRectEmpty (&ClearedField);

    Status = GetPanelColorAttribute (SetupPagePanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &ColorAttribute);
    if (EFI_ERROR (Status)) {
      ColorAttribute = PANEL_COLOR_ATTRIBUTE_SETUP_PAGE;
    }
    BorderLineWidth = GetBorderWidth (SetupPagePanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

    GetClearField (&ClearedField, &SetupPagePanel->PanelField, PromptControlList.ControlArray, MenuCount, BorderLineWidth);
    ClearField (ColorAttribute, &ClearedField, NULL);

    //
    // BUGBUG: Clear the separation line between Prompt and Value. It should remove this until combine Prompt and Value to one control.
    //
    CopyRect (&PromptField, &SetupPagePanel->PanelField);
    PromptField.right = PromptField.right / 2;
    SetRect (&ClearedField, PromptField.right - 1, PromptField.top + 1, PromptField.right, PromptField.bottom - 1);
    ClearField (ColorAttribute, &ClearedField, NULL);
  }

  if (MenuCount == 0) {
    Status = GetPanelColorAttribute (HelpTextPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &ColorAttribute);
    if (EFI_ERROR (Status)) {
      ColorAttribute = PANEL_COLOR_ATTRIBUTE_HELP_TEXT;
    }
    BorderLineWidth = GetBorderWidth (HelpTextPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

    GetClearField (&ClearedField, &HelpTextPanel->PanelField, HelpControlList->ControlArray, MenuCount, BorderLineWidth);
    ClearField (ColorAttribute, &ClearedField, NULL);
  }

  DisplayNormalControls (MenuCount, PromptControlList.ControlArray);
  DisplayNormalControls (MenuCount, ValueControlList.ControlArray);

  return EFI_SUCCESS;
}

/**
 Get all filed data from open dialog event

 @param [in    ] Dialog             Pointer to dialog event data
 @param [in,out] Field              Output field data
 @param [in,out] TitleField         Output title field data
 @param [in,out] BodyField          Output body field data
 @param [in,out] BodyInputField     Output body input field data
 @param [in,out] ButtonField        Output button field data

 @retval EFI_SUCCESS                Get field data successfully
 @retval EFI_INVALID_PARAMETER      Dialog is NULL

**/
STATIC
EFI_STATUS
GetDialogFields (
  IN     H2O_FORM_BROWSER_D                   *Dialog,
  IN OUT RECT                                 *DlgField ,
  IN OUT RECT                                 *TitleField,
  IN OUT RECT                                 **BodyField,
  IN OUT RECT                                 **BodyInputField,
  IN OUT RECT                                 **ButtonField
  )
{
  UINT32                                      ScreenColumn;
  UINT32                                      ScreenRow;
  RECT                                        DialogField;
  UINT32                                      DialogWidth;
  UINT32                                      DialogHeight;
  UINT32                                      Index;
  UINT32                                      StartRow;
  UINT32                                      StartColumn;
  UINT32                                      TitleWidth;
  UINT32                                      TitleHeight;
  UINT32                                      TotalBodyWidth;
  UINT32                                      OneSetBodyWidth;
  UINT32                                      MaxBodyWidth;
  UINT32                                      TotalBodyHeight;
  UINT32                                      TotalBodyInputHeight;
  UINT32                                      TotalButtonWidth;
  UINT32                                      *BodyWidth;
  UINT32                                      *BodyHeight;
  UINT32                                      *BodyInputWidth;
  UINT32                                      *BodyInputHeight;
  UINT32                                      *ButtonWidth;
  UINT32                                      *ButtonHeight;
  UINT32                                      BorderWidth;
  H2O_PANEL_INFO                              *Panel;
  BOOLEAN                                     BodyIsHorizontal;
  UINT32                                      LimitLineWidth;
  UINT32                                      SeparationWidth;
  UINT32                                      Height;
  UINT32                                      Width;
  UINT32                                      Count;
  RECT                                        *BodyFieldBuffer;
  RECT                                        *BodyInputFieldBuffer;
  RECT                                        *ButtonFieldBuffer;

  if (Dialog == NULL || DlgField == NULL || TitleField == NULL ||
      BodyField == NULL || BodyInputField == NULL || ButtonField == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Panel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
  if (Panel == NULL) {
    return EFI_NOT_FOUND;
  }

  BorderWidth        = GetBorderWidth (Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);
  SeparationWidth    = 1;
  BodyIsHorizontal   = ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_BODY_HORIZONTAL) != 0) ? TRUE : FALSE;
  GetScreenMaxXY (&ScreenColumn, &ScreenRow);

  //
  // Title
  //
  TitleWidth       = 0;
  TitleHeight      = 0;
  if (Dialog->TitleString != NULL) {
    LimitLineWidth = ScreenColumn - BorderWidth * 2;
    CalculateRequireSize (Dialog->TitleString, LimitLineWidth, &TitleWidth, &TitleHeight);
  }

  //
  // Body
  //
  BodyFieldBuffer = NULL;
  BodyWidth       = NULL;
  BodyHeight      = NULL;
  TotalBodyHeight = 0;
  MaxBodyWidth    = 0;
  if (Dialog->BodyStringCount != 0) {
    BodyFieldBuffer = AllocateZeroPool (Dialog->BodyStringCount * sizeof (RECT));
    BodyWidth       = AllocateZeroPool (Dialog->BodyStringCount * sizeof (UINT32));
    BodyHeight      = AllocateZeroPool (Dialog->BodyStringCount * sizeof (UINT32));
    if (BodyWidth == NULL || BodyHeight == NULL || BodyFieldBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    LimitLineWidth  = (Dialog->BodyInputCount != 0) ? (ScreenColumn / 2) : ScreenColumn;
    LimitLineWidth -= (BorderWidth * 2);

    for (Index = 0; Index < Dialog->BodyStringCount ; Index ++) {
      if (Dialog->BodyStringArray[Index] != NULL) {
        CalculateRequireSize (Dialog->BodyStringArray[Index], LimitLineWidth, &BodyWidth[Index], &BodyHeight[Index]);
      }
      MaxBodyWidth     = MAX (MaxBodyWidth, BodyWidth[Index]);
    }

    if (BodyIsHorizontal) {
      TotalBodyHeight = 1;
    } else {
      for (Index = 0; Index < Dialog->BodyStringCount ; Index ++) {
        TotalBodyHeight += BodyHeight[Index];
      }
    }
  }

  //
  // Body Input
  //
  BodyInputFieldBuffer = NULL;
  BodyInputWidth       = NULL;
  BodyInputHeight      = NULL;
  TotalBodyInputHeight = 0;
  if (Dialog->BodyInputCount != 0) {
    BodyInputFieldBuffer = AllocateZeroPool (Dialog->BodyInputCount * sizeof (RECT));
    BodyInputWidth       = AllocateZeroPool (Dialog->BodyInputCount * sizeof (UINT32));
    BodyInputHeight      = AllocateZeroPool (Dialog->BodyInputCount * sizeof (UINT32));
    if (BodyInputWidth == NULL || BodyInputHeight == NULL || BodyInputFieldBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    LimitLineWidth  = (Dialog->BodyStringCount != 0) ? (ScreenColumn / 2) : ScreenColumn;
    LimitLineWidth -= (BorderWidth * 2);
    Width           = (Dialog->ConfirmHiiValue.BufferLen - sizeof (CHAR16)) / sizeof (CHAR16);

    for (Index = 0; Index < Dialog->BodyInputCount ; Index ++) {
      if (Dialog->ConfirmHiiValue.Type == EFI_IFR_TYPE_STRING) {
        BodyInputWidth[Index]   = MIN (Width, LimitLineWidth);
        BodyInputHeight[Index]  = Width / LimitLineWidth;
        BodyInputHeight[Index] += (Width % LimitLineWidth != 0) ? 1 : 0;
      } else {
        if (Dialog->BodyInputStringArray[Index] != NULL) {
          CalculateRequireSize (Dialog->BodyInputStringArray[Index], LimitLineWidth, &BodyInputWidth[Index], &BodyInputHeight[Index]);
        }
      }
    }

    if (BodyIsHorizontal) {
      TotalBodyInputHeight = 1;
    } else {
      for (Index = 0; Index < Dialog->BodyInputCount ; Index++) {
        TotalBodyInputHeight += BodyInputHeight[Index];
      }
    }
  }

  //
  // Button
  //
  ButtonFieldBuffer = NULL;
  ButtonWidth       = NULL;
  ButtonHeight      = NULL;
  TotalButtonWidth  = 0;
  if (Dialog->ButtonCount != 0) {
    ButtonFieldBuffer = AllocateZeroPool (Dialog->ButtonCount * sizeof (RECT));
    ButtonWidth       = AllocateZeroPool (Dialog->ButtonCount * sizeof (UINT32));
    ButtonHeight      = AllocateZeroPool (Dialog->ButtonCount * sizeof (UINT32));
    if (ButtonWidth == NULL || ButtonHeight == NULL || ButtonFieldBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    LimitLineWidth = ScreenColumn - BorderWidth * 2;

    for (Index = 0; Index < Dialog->ButtonCount; Index ++) {
      if (Dialog->ButtonStringArray[Index] != NULL) {
        CalculateRequireSize (
          Dialog->ButtonStringArray[Index],
          LimitLineWidth,
          &ButtonWidth[Index],
          &ButtonHeight[Index]
          );
        ButtonWidth[Index] += 2; // 2 for char "[" and "]"
      }
      TotalButtonWidth += ButtonWidth[Index];
    }
  }

  //
  // Calculate the Dialog Width which is the max width value from title, (body and body input) and button
  //
  DialogWidth = TitleWidth;

  TotalBodyWidth = 0;
  Count = MAX (Dialog->BodyStringCount, Dialog->BodyInputCount);
  for (Index = 0; Index < Count; Index++) {
    if (Dialog->BodyStringCount > Index && Dialog->BodyInputCount > Index) {
      OneSetBodyWidth = BodyWidth[Index] + SeparationWidth + BodyInputWidth[Index];
    } else if (Dialog->BodyStringCount > Index) {
      OneSetBodyWidth = BodyWidth[Index];
    } else {
      OneSetBodyWidth = BodyInputWidth[Index];
    }

    TotalBodyWidth = BodyIsHorizontal ? (TotalBodyWidth + OneSetBodyWidth) : MAX (TotalBodyWidth, OneSetBodyWidth);
  }
  if (BodyIsHorizontal) {
    TotalBodyWidth += ((Count - 1) * SeparationWidth);
  }
  DialogWidth = MAX (DialogWidth, TotalBodyWidth);

  if (Dialog->ButtonCount != 0) {
    TotalButtonWidth += ((Dialog->ButtonCount - 1) * SeparationWidth);
  }
  DialogWidth = MAX (DialogWidth, TotalButtonWidth);

  DialogWidth += (BorderWidth * 2);
  if (DialogWidth > ScreenColumn) {
    DialogWidth = ScreenColumn - 1;
  }

  //
  // Calculate the Dialog Height
  //
  DialogHeight  = 0;
  DialogHeight += 1;                                                                 // For Border Line
  DialogHeight += (Dialog->TitleString != NULL) ? TitleHeight + 1 : 0;               // For Title with border line
  DialogHeight += MAX (TotalBodyHeight, TotalBodyInputHeight);                       // For body and body input
  DialogHeight += (Dialog->ButtonCount != 0) ? 2 : 0;                                // For Button with empty line
  DialogHeight += 1;                                                                 // For Border Line

  if (DialogHeight >= ScreenRow) {
    DialogHeight = ScreenRow - 1;
  }

  //
  // Set DialogField
  //
  SetRectEmpty (&DialogField);
  if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) == H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) {
    Panel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE);
    if (Panel != NULL) {
      UnionRect (&DialogField, &DialogField, &Panel->PanelField);
    }
    Panel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_HELP_TEXT);
    if (Panel != NULL) {
      UnionRect (&DialogField, &DialogField, &Panel->PanelField);
    }
  }
  if (IsRectEmpty (&DialogField)) {
    DialogField.left   = (ScreenColumn - DialogWidth) / 2;
    DialogField.right  = DialogField.left + DialogWidth - 1;
    DialogField.top    = (ScreenRow - DialogHeight) / 2;
    DialogField.bottom = DialogField.top + DialogHeight - 1;
  }

  CopyRect (DlgField, &DialogField);

  StartRow = DialogField.top + 1;

  //
  // Save Field : Title
  //
  if (Dialog->TitleString != NULL) {
    TitleField->left   = DialogField.left + (DialogField.right - DialogField.left + 1 - TitleWidth) / 2;
    TitleField->right  = TitleField->left + TitleWidth - 1;
    TitleField->top    = StartRow;
    TitleField->bottom = StartRow + TitleHeight - 1;
    StartRow += TitleHeight + 1;
  }

  //
  // Save Field : Body and Body Input
  //
  if (BodyFieldBuffer != NULL && BodyInputFieldBuffer != NULL) {
    if (BodyIsHorizontal) {
      StartColumn = DialogField.left + BorderWidth;

      for (Index = 0; (Index < Dialog->BodyStringCount || Index < Dialog->BodyInputCount); Index++) {
        if (Index < Dialog->BodyStringCount) {
          Width = (BodyWidth[Index] == 0) ? 0 : BodyWidth[Index] - 1;
          SetRect (
            &BodyFieldBuffer[Index],
            StartColumn,
            StartRow,
            StartColumn + Width,
            StartRow
            );
          StartColumn += ((BodyWidth[Index] == 0) ? 0 : (Width + 1));
          StartColumn += SeparationWidth;
        }
        if (Index < Dialog->BodyInputCount) {
          Width = (BodyInputWidth[Index] == 0) ? 0 : BodyInputWidth[Index] - 1;
          SetRect (
            &BodyInputFieldBuffer[Index],
            StartColumn,
            StartRow,
            StartColumn + Width,
            StartRow
            );
          StartColumn += ((BodyInputWidth[Index] == 0) ? 0 : (Width + 1));
          StartColumn += SeparationWidth;
        }
      }
    } else {
      for (Index = 0; (Index < Dialog->BodyStringCount || Index < Dialog->BodyInputCount); Index++) {
        Height = (Index < Dialog->BodyStringCount) ? BodyHeight[Index] : 0;
        Height = (Index < Dialog->BodyInputCount ) ? MAX(BodyInputHeight[Index], Height) : Height;

        if (Index < Dialog->BodyStringCount) {
          StartColumn = DialogField.left + BorderWidth;
          SetRect (
            &BodyFieldBuffer[Index],
            StartColumn,
            StartRow,
            StartColumn + MaxBodyWidth - 1,
            StartRow + Height - 1
            );
        }
        if (Index < Dialog->BodyInputCount) {
          StartColumn = DialogField.left + BorderWidth + MaxBodyWidth + SeparationWidth;
          SetRect (
            &BodyInputFieldBuffer[Index],
            StartColumn,
            StartRow,
            DialogField.right - BorderWidth,
            StartRow + Height - 1
            );
        }
        StartRow += Height;
      }
    }
  } else if (BodyFieldBuffer != NULL) {
    //
    // Only Has Body
    //
    StartColumn = DialogField.left + BorderWidth;
    if (BodyIsHorizontal) {
      for (Index = 0; (Index < Dialog->BodyStringCount); Index ++) {
        SetRect (
          &BodyFieldBuffer[Index],
          StartColumn,
          StartRow,
          StartColumn + BodyWidth[Index] - 1,
          StartRow
          );
        StartColumn += (BodyWidth[Index] + SeparationWidth);
      }
    } else {
      for (Index = 0; Index < Dialog->BodyStringCount; Index ++) {
        SetRect (
          &BodyFieldBuffer[Index],
          StartColumn,
          StartRow,
          DialogField.right - BorderWidth,
          StartRow + BodyHeight[Index] - 1
          );
        StartRow += BodyHeight[Index];
      }
    }
  } else if (BodyInputFieldBuffer != NULL) {
    //
    // Only Has Body Input
    //
    StartColumn = DialogField.left + BorderWidth;
    if (BodyIsHorizontal) {
      for (Index = 0; Index < Dialog->BodyInputCount; Index ++) {
        SetRect (
          &BodyInputFieldBuffer[Index],
          StartColumn,
          StartRow,
          StartColumn + BodyInputWidth[Index] - 1,
          StartRow
          );
        StartColumn += (BodyInputWidth[Index] + SeparationWidth);

      }
    } else {
      for (Index = 0; Index < Dialog->BodyInputCount; Index ++) {
        SetRect (
          &BodyInputFieldBuffer[Index],
          StartColumn,
          StartRow,
          DialogField.right - BorderWidth,
          StartRow + BodyInputHeight[Index] - 1
          );
        StartRow += BodyInputHeight[Index];
      }
    }
  }

  //
  // Save Field : Button
  //
  if (ButtonFieldBuffer != NULL) {
    StartRow    = DialogField.bottom - 1;
    StartColumn = DialogField.left + (DialogWidth - TotalButtonWidth) / 2;

    for (Index = 0; Index < Dialog->ButtonCount; Index++) {
      SetRect (
        &ButtonFieldBuffer[Index],
        StartColumn,
        StartRow,
        StartColumn + ButtonWidth[Index] - 1,
        StartRow
        );
      StartColumn += ButtonWidth[Index] + SeparationWidth;
    }
  }

  *BodyField       = BodyFieldBuffer;
  *BodyInputField  = BodyInputFieldBuffer;
  *ButtonField     = ButtonFieldBuffer;


  SafeFreePool ((VOID **) &BodyWidth);
  SafeFreePool ((VOID **) &BodyHeight);
  SafeFreePool ((VOID **) &BodyInputWidth);
  SafeFreePool ((VOID **) &BodyInputHeight);
  SafeFreePool ((VOID **) &ButtonWidth);
  SafeFreePool ((VOID **) &ButtonHeight);

  return EFI_SUCCESS;
}

EFI_STATUS
InitDialog (
  IN H2O_FORM_BROWSER_D                       *Dialog
  )
{
  RECT                                        DialogField;
  RECT                                        TitleField;
  RECT                                        *BodyField;
  RECT                                        *BodyInputField;
  RECT                                        *ButtonField;
  UINT32                                      Index;
  UINT32                                      Count;
  H2O_CONTROL_INFO                            *ControlArray;
  H2O_CONTROL_INFO                            *Control;
  H2O_PANEL_INFO                              *QuestionPanel;
  EFI_STATUS                                  Status;
  BOOLEAN                                     IsHelpDialog;
  UINT32                                      PseudoClass;
  UINT8                                       Operand;

  if (Dialog == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
  if (QuestionPanel == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = GetDialogFields (
             Dialog,
             &DialogField,
             &TitleField,
             &BodyField,
             &BodyInputField,
             &ButtonField
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IsHelpDialog = ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) != 0) ? TRUE : FALSE;
  PseudoClass  = IsHelpDialog ? H2O_STYLE_PSEUDO_CLASS_HELP : H2O_STYLE_PSEUDO_CLASS_NORMAL;
  Operand      = GetOperandByDialogType (Dialog->DialogType);

  //
  // Init Control Array
  //
  Count        = Dialog->BodyStringCount + Dialog->BodyInputCount + Dialog->ButtonCount;
  Count       += (Dialog->TitleString != NULL) ? 1 : 0;
  ControlArray = (H2O_CONTROL_INFO*) AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * Count);
  if (ControlArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Count = 0;

  //
  // Title
  //
  if (Dialog->TitleString != NULL) {
    Control              = &ControlArray[Count];
    Control->ControlId   = H2O_CONTROL_ID_DIALOG_TITLE;
    Control->Selectable  = FALSE;
    Control->Modifiable  = FALSE;
    Control->Operand     = Operand;
    Control->Text.String = AllocateCopyPool (StrSize (Dialog->TitleString), Dialog->TitleString);
    Control->ParentPanel = QuestionPanel;
    Control->ControlStyle.PseudoClass = PseudoClass;
    CopyRect (&Control->ControlField, &TitleField);
    Count++;
  }

  //
  // Body
  //
  for (Index = 0; Index < Dialog->BodyStringCount; Index++) {
    Control              = &ControlArray[Count];
    Control->ControlId   = H2O_CONTROL_ID_DIALOG_BODY;
    Control->Operand     = Operand;
    Control->Text.String = AllocateCopyPool (StrSize (Dialog->BodyStringArray[Index]), Dialog->BodyStringArray[Index]);
    Control->ParentPanel = QuestionPanel;
    Control->ControlStyle.PseudoClass = PseudoClass;

    if (Dialog->BodyHiiValueArray != NULL) {
      Control->Selectable = TRUE;
      Control->Modifiable = FALSE;
      Control->Editable   = FALSE;
      CopyMem (&Control->HiiValue, &Dialog->BodyHiiValueArray[Index], sizeof (EFI_HII_VALUE));
    } else if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE) == H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE) {
      Control->Selectable = TRUE;
      Control->Modifiable = FALSE;
      Control->Editable   = FALSE;
      CreateValueAsUint64 (&Control->HiiValue, (UINT64) Count);
    } else {
      Control->Selectable = FALSE;
      Control->Modifiable = FALSE;
      Control->Editable   = FALSE;
      Control->HiiValue.Type      = EFI_IFR_TYPE_OTHER;
      Control->HiiValue.Value.u64 = (UINT64) Count;
    }
    CopyRect (&Control->ControlField, &BodyField[Index]);
    Count++;
  }

  //
  // Body Input
  //
  for (Index = 0; Index < Dialog->BodyInputCount; Index++) {
    Control              = &ControlArray[Count];
    Control->ControlId   = H2O_CONTROL_ID_DIALOG_BODY_INPUT;
    Control->Operand     = Operand;
    Control->Text.String = AllocateCopyPool (StrSize (Dialog->BodyInputStringArray[Index]), Dialog->BodyInputStringArray[Index]);
    Control->ParentPanel = QuestionPanel;

    Control->Selectable = TRUE;
    Control->Modifiable = TRUE;
    Control->Editable   = TRUE;
    if (Dialog->H2OStatement != NULL) {
      Control->Maximum = Dialog->H2OStatement->Maximum;
      Control->Minimum = Dialog->H2OStatement->Minimum;
      Control->Step    = Dialog->H2OStatement->Step;
    }
    Control->ControlStyle.PseudoClass = IsHelpDialog ? PseudoClass : H2O_STYLE_PSEUDO_CLASS_DISABLED;
    if (Operand == EFI_IFR_NUMERIC_OP) {
      CopyMem (&Control->HiiValue, &Dialog->ConfirmHiiValue, sizeof (EFI_HII_VALUE));
    } else {
      CreateValueAsString (&Control->HiiValue, Dialog->ConfirmHiiValue.BufferLen, (UINT8 *) Control->Text.String);
    }
    CopyRect (&Control->ControlField, &BodyInputField[Index]);
    Count++;
  }

  //
  // Button
  //
  for (Index = 0; Index < Dialog->ButtonCount; Index++) {
    Control              = &ControlArray[Count];
    Control->ControlId   = H2O_CONTROL_ID_DIALOG_BUTTON;
    Control->Operand     = Operand;
    Control->Text.String = CatSPrint (NULL, L"[%s]", Dialog->ButtonStringArray[Index]);
    Control->ParentPanel = QuestionPanel;

    Control->Selectable = TRUE;
    Control->Modifiable = FALSE;
    Control->Editable   = FALSE;
    if (Dialog->ButtonHiiValueArray != NULL) {
      CopyMem (&Control->HiiValue, &Dialog->ButtonHiiValueArray[Index], sizeof (EFI_HII_VALUE));
    } else {
      CreateValueAsUint64 (&Control->HiiValue, (UINT64) Index);
    }
    CopyRect (&Control->ControlField, &ButtonField[Index]);
    Count++;
  }

  //
  // Save Question Panel Info
  //
  FreeControlList (&QuestionPanel->ControlList);
  QuestionPanel->ControlList.Count        = Count;
  QuestionPanel->ControlList.ControlArray = ControlArray;

  CopyRect (&QuestionPanel->PanelField, &DialogField);
  //
  // Set Panel Relative Field
  //
  QuestionPanel->PanelRelField.top    = 0;
  QuestionPanel->PanelRelField.bottom = QuestionPanel->PanelField.bottom - QuestionPanel->PanelField.top -
                                        GetBorderWidth(QuestionPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL) * 2;

  for (Index = 0; Index < Count; Index++) {
    Control = &QuestionPanel->ControlList.ControlArray[Index];

    if (Control->ControlField.right >= QuestionPanel->PanelField.right) {
      //
      // Set Panel WithScrollBarHorizontal
      //
      QuestionPanel->WithScrollBarHorizontal = TRUE;
    }
    if (Control->ControlField.bottom >= QuestionPanel->PanelField.bottom) {
      //
      // Set Panel WithScrollBarVertical
      //
      QuestionPanel->WithScrollBarVertical = TRUE;
    }
    //
    // Set ControlField
    //
    Control->ControlField.left   -= (QuestionPanel->PanelField.left + 1);
    Control->ControlField.right  -= (QuestionPanel->PanelField.left + 1);
    Control->ControlField.top    -= (QuestionPanel->PanelField.top  + 1);
    Control->ControlField.bottom -= (QuestionPanel->PanelField.top  + 1);
  }

  SafeFreePool ((VOID **) &BodyField);
  SafeFreePool ((VOID **) &BodyInputField);
  SafeFreePool ((VOID **) &ButtonField);

  return EFI_SUCCESS;
}

/**
 Display dialog

 @param [in]  Refresh               Flag to decide to refresh dialog
 @param [in]  Dialog                Pointer to dialog data
 @param [out] OldDialogPanel        Output dialog panel data

 @retval EFI_SUCCESS                Get field data successfully
 @retval EFI_INVALID_PARAMETER      Dialog is NULL
 @retval EFI_NOT_FOUND              Can not find the question panel info

**/
EFI_STATUS
DisplayDialog (
  IN        BOOLEAN                           Refresh,
  IN  CONST H2O_FORM_BROWSER_D                *Dialog,
  OUT       H2O_PANEL_INFO                    **OldDialogPanel
  )
{
  RECT                                        TitleBorderLineField;
  H2O_PANEL_INFO                              *QuestionPanel;
  UINT32                                      ColorAttribute;
  BOOLEAN                                     IsHelpDialog;
  EFI_STATUS                                  Status;
  UINT32                                      PseudoClass;
  UINT32                                      TitleStrHeight;


  QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
  if (QuestionPanel == NULL) {
    return EFI_NOT_FOUND;
  }

  if (!Refresh) {
    Status = InitDialog ((H2O_FORM_BROWSER_D *) Dialog);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (OldDialogPanel != NULL) {
    *OldDialogPanel = QuestionPanel;
  }

  IsHelpDialog = ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) != 0) ? TRUE : FALSE;
  PseudoClass  = IsHelpDialog ? H2O_STYLE_PSEUDO_CLASS_HELP : H2O_STYLE_PSEUDO_CLASS_NORMAL;

  //
  // Display
  //
  DisplayPanel (QuestionPanel, PseudoClass);

  if (Dialog->TitleString != NULL && QuestionPanel->ControlList.ControlArray != NULL) {
    Status = GetPanelColorAttribute (QuestionPanel, H2O_IFR_STYLE_TYPE_PANEL, PseudoClass, &ColorAttribute);
    if (EFI_ERROR (Status)) {
      ColorAttribute = IsHelpDialog ? PANEL_COLOR_ATTRIBUTE_HELP_TEXT : PANEL_COLOR_ATTRIBUTE_QUESTION;
    }
    TitleStrHeight = QuestionPanel->ControlList.ControlArray[0].ControlField.bottom - QuestionPanel->ControlList.ControlArray[0].ControlField.top + 1;

    TitleBorderLineField.left   = QuestionPanel->PanelField.left;
    TitleBorderLineField.right  = QuestionPanel->PanelField.right;
    TitleBorderLineField.top    = QuestionPanel->PanelField.top +
                                  GetBorderWidth (QuestionPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL) +
                                  TitleStrHeight;
    TitleBorderLineField.bottom = TitleBorderLineField.top;
    DisplayBorderLine (ColorAttribute, &TitleBorderLineField, NULL);
  }

  DisplayNormalControls (
    QuestionPanel->ControlList.Count,
    QuestionPanel->ControlList.ControlArray
    );

  return EFI_SUCCESS;
}

/**
 Display string

 @param [in] StartX                Target x-axis of string
 @param [in] StartY                Target y-axis of string
 @param [in] Fmt                   Format string
 @param [in] ...                   Variable argument list for format string

 @retval EFI_SUCCESS               Display string successfully
 @retval Other                     Output dstring fail

**/
EFI_STATUS
DisplayString (
  IN     UINT32                              StartX,
  IN     UINT32                              StartY,
  IN     CONST CHAR16                        *Fmt,
  ...
  )
{
  EFI_STATUS                                 Status;
  CHAR16                                     *String;
  INT32                                      Attribute;
  VA_LIST                                    Args;


  VA_START (Args, Fmt);

  DEConOutEnableCursor (mDEPrivate, FALSE);

  String = AllocateZeroPool (0x1000);
  if (String == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeVSPrint (String, 0x1000, Fmt, Args);

  DEConOutGetAttribute (mDEPrivate, &Attribute);

  DEConOutSetCursorPosition (mDEPrivate, StartX, StartY);
  DEConOutSetNarrowAttribute (mDEPrivate);
  Status = DEConOutOutputString (mDEPrivate, String);
  SafeFreePool ((VOID **) &String);

  VA_END (Args);

  return Status;
}

