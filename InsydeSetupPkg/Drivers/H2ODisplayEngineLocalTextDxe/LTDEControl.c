/** @file
  Control (which make up panel) related Functions for H2O display engine driver.

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

/**
 Get current resolution of screen

 @param [out] X           The horizontal value of screen
 @param [out] Y           The vertical value of screen

 @retval EFI_SUCCESS      Get screen resolution successfully
 @retval Other            Fail to get screen resolution

**/
EFI_STATUS
GetScreenMaxXY (
  OUT    UINT32                              *X,
  OUT    UINT32                              *Y
  )
{
  return DEConOutQueryModeWithoutModeNumer (mDEPrivate, X, Y);
}

/**
 Combine the region of multiple input fields

 @param [out] ResultField       Result of combined field
 @param  ...                    A variable argument list containing series of
                                field, the last field must be NULL.

 @retval EFI_SUCCESS            Get screen resolution successfully
 @retval EFI_INVALID_PARAMETER  ResultField is NULL
 @retval EFI_NOT_FOUND          There is no input field

**/
STATIC
EFI_STATUS
CombineField (
  OUT    RECT                                *ResultField,
  ...
  )
{
  VA_LIST                                    Args;
  RECT                                       *Field;

  if (ResultField == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VA_START (Args, ResultField);

  Field = VA_ARG (Args, RECT *);
  if (Field == NULL) {
    return EFI_NOT_FOUND;
  }

  CopyRect (ResultField, Field);

  while (TRUE) {
    Field = VA_ARG (Args, RECT *);
    if (Field == NULL) {
      break;
    }

    ResultField->left   = MIN (ResultField->left  , Field->left);
    ResultField->top    = MIN (ResultField->top   , Field->top);
    ResultField->right  = MAX (ResultField->right , Field->right);
    ResultField->bottom = MAX (ResultField->bottom, Field->bottom);
  }

  return EFI_SUCCESS;
}

/**
 Get formatted string which follow the input line width

 @param [in]  InputString       The input string
 @param [in]  LineWidth         Line width which output string should follow
 @param [out] LineNum           Line number of formatted string
 @param [out] LineOffsetArray   Array of input string offset for each line of output formatted string
 @param [out] OutputString      Pointer to output formatted string

 @retval EFI_SUCCESS            Get screen resolution successfully
 @retval EFI_INVALID_PARAMETER  InputString, LineNum or OutputString is NULL or LineWidth is zero

**/
STATIC
EFI_STATUS
GetLineByWidth (
  IN      CHAR16                      *InputString,
  IN      UINT32                      LineWidth,
  OUT     UINT32                      *LineNum,
  OUT     UINT32                      **LineOffsetArray,
  OUT     CHAR16                      **OutputString
  )
{
  BOOLEAN                             IsWidth;
  BOOLEAN                             Finished;
  BOOLEAN                             AdjustStr;
  UINT32                              AdjCount;
  UINT32                              CharCount;
  UINT32                              TempCharCount;
  UINT32                              MaxLineNum;
  UINT32                              BufferSize;
  CHAR16                              *StringBuffer;
  UINT32                              InputStrOffset;
  UINT32                              OutputStrOffset;
  UINT32                              StrWidth;
  UINT32                              IncrementValue;
  CHAR16                              Character;
  UINT32                              *OffsetArray;
  UINT32                              OffsetArrayCount;

  if (InputString == NULL || LineNum == NULL || OutputString == NULL || LineOffsetArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (LineWidth < 1) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Try to calculate the maximun using memory size
  //
  MaxLineNum    = (UINT32) StrSize (InputString) / LineWidth;
  BufferSize    = MaxLineNum * 4 + (UINT32) StrSize (InputString);
  *OutputString = AllocateZeroPool (BufferSize);
  if (*OutputString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  IsWidth          = FALSE;
  Finished         = FALSE;
  InputStrOffset   = 0;
  OutputStrOffset  = 0;
  *LineNum         = 0;
  IncrementValue   = 1;
  OffsetArrayCount = 10;
  OffsetArray      = AllocateZeroPool (OffsetArrayCount * sizeof(UINT32));
  if (OffsetArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  while (!Finished) {
    if (IsWidth) {
      (*OutputString)[OutputStrOffset] = WIDE_CHAR;
      OutputStrOffset++;
    }

    StrWidth  = 0;
    CharCount = 0;
    AdjustStr = FALSE;

    do {
      //
      // Advance to the null-terminator or to the first width directive
      //
      Character = InputString[InputStrOffset + CharCount];

      while ((Character != CHAR_CARRIAGE_RETURN)&&
             (Character != NARROW_CHAR) &&
             (Character != WIDE_CHAR) &&
             (Character != 0) &&
             (StrWidth < LineWidth)) {
        CharCount++;
        StrWidth += IncrementValue;
        if (IncrementValue == 1 && ConsoleLibGetGlyphWidth (Character) == 2) {
          //
          // If character belongs to wide glyph, set the right glyph offset.
          //
          StrWidth++;
        }

        Character = InputString[InputStrOffset + CharCount];
      }

      if (StrWidth > LineWidth) {
        CharCount--;
        StrWidth = StrWidth - IncrementValue;
        AdjustStr = TRUE;
        break;
      } else if (StrWidth == LineWidth) {
        break;
      }
      //
      // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
      // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
      //
      if (InputString[InputStrOffset + CharCount] == NARROW_CHAR) {
        //
        // Skip to the next character
        //
        CharCount++;
        IncrementValue = 1;
        IsWidth = FALSE;
      } else if (InputString[InputStrOffset + CharCount] == WIDE_CHAR) {
        //
        // Skip to the next character
        //
        CharCount++;
        IncrementValue = 2;
        IsWidth = TRUE;
      }
    } while (InputString[InputStrOffset + CharCount] != 0 && InputString[InputStrOffset + CharCount] != CHAR_CARRIAGE_RETURN);

    //
    // If space,narrow and wide character before end of the string, increase index to skip these characters
    //
    AdjCount      = 0;
    while (InputString[InputStrOffset + CharCount + AdjCount] != 0) {
      if ((InputString[InputStrOffset + CharCount + AdjCount] != WIDE_CHAR) &&
          (InputString[InputStrOffset + CharCount + AdjCount] != NARROW_CHAR) &&
          (InputString[InputStrOffset + CharCount + AdjCount] != CHAR_SPACE)) {
        break;
      }
      AdjCount++;
    }

    if (InputString[InputStrOffset + CharCount + AdjCount] == 0) {
      Finished = TRUE;
    } else {
      if (StrWidth == LineWidth || AdjustStr) {
        //
        // Rewind the string from the maximum size until we see a space to break the line
        //
        if (!IsWidth) {
          TempCharCount = CharCount;

          for (; (InputString[InputStrOffset + CharCount] != CHAR_SPACE) && (CharCount != 0); CharCount--, StrWidth = StrWidth - IncrementValue)
            ;
          if (CharCount == 0) {
            CharCount = TempCharCount;
          }
        }
      }
    }

    if (BufferSize < (UINT32) (OutputStrOffset + (CharCount + 2) * sizeof (CHAR16))) {
       BufferSize  *= 2;
       StringBuffer = AllocateZeroPool (BufferSize);
       if (StringBuffer == NULL) {
         return EFI_OUT_OF_RESOURCES;
       }
       CopyMem (StringBuffer, *OutputString, OutputStrOffset * sizeof (CHAR16));
       SafeFreePool ((VOID **) OutputString);
       *OutputString = StringBuffer;
    }

    CopyMem (&(*OutputString)[OutputStrOffset], &InputString[InputStrOffset], CharCount * sizeof (CHAR16));

    //
    // Increase offset to add null for end of one line
    //
    OutputStrOffset += (CharCount + 1);
    //
    // if not last string, we need increment the index to the first non-space character
    //
    if (!Finished) {
      while (InputString[InputStrOffset + CharCount] != 0) {
        if ((InputString[InputStrOffset + CharCount] != WIDE_CHAR) &&
            (InputString[InputStrOffset + CharCount] != NARROW_CHAR) &&
            (InputString[InputStrOffset + CharCount] != CHAR_SPACE) &&
            (InputString[InputStrOffset + CharCount] != CHAR_CARRIAGE_RETURN)) {
          break;
        }
        InputStrOffset++;
      }
    }

    if (*LineNum >= OffsetArrayCount) {
      OffsetArray = ReallocatePool (
                      OffsetArrayCount * sizeof(UINT32),
                      (OffsetArrayCount + 10) * sizeof(UINT32),
                      OffsetArray
                      );
      if (OffsetArray == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      OffsetArrayCount += 10;
    }
    OffsetArray[*LineNum] = InputStrOffset;
    InputStrOffset        = InputStrOffset + CharCount;
    (*LineNum)++;
  }

  *LineOffsetArray = AllocateCopyPool ((*LineNum) * sizeof(UINT32), OffsetArray);
  FreePool (OffsetArray);
  if (*LineOffsetArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
 Get result filed data from input source field data and input control info list

 @param [in,out] ResultField        Output field data
 @param [in]  SourceField           Input source field data
 @param [in]  UsedMenu              Used contorl info list
 @param [in]  MenuCount             Number of Used contorl info

 @retval EFI_SUCCESS                Get result filed data successfully
 @retval EFI_INVALID_PARAMETER      ResultField, SourceField or UsedMenu is NULL or MenuCount is zero

**/
EFI_STATUS
GetClearField (
  IN OUT RECT                                    *ResultField,
  IN     RECT                                    *SourceField,
  IN     H2O_CONTROL_INFO                        *UsedMenu,
  IN     UINT32                                  MenuCount,
  IN     UINT32                                  BorderLineWidth
  )
{
  RECT                                           OutField;
  RECT                                           MenuField;
  UINT32                                         Index;
  INT32                                          CutUsedHight;

  if (ResultField == NULL || SourceField == NULL || UsedMenu == NULL || MenuCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  CopyRect (&OutField, SourceField);
  OutField.left   += BorderLineWidth;
  OutField.top    += BorderLineWidth;
  OutField.right  -= BorderLineWidth;
  OutField.bottom -= BorderLineWidth;
  if (UsedMenu == NULL || MenuCount == 0) {
    CopyRect (ResultField, &OutField);
    return EFI_SUCCESS;
  }

  SetRectEmpty (&MenuField);
  for (Index = 0; Index < MenuCount; Index ++) {
    if (UsedMenu[Index].ControlField.left   >= (UsedMenu[Index].ParentPanel->PanelField.left + UsedMenu[Index].ParentPanel->PanelRelField.left  ) &&
        UsedMenu[Index].ControlField.right  <= (UsedMenu[Index].ParentPanel->PanelField.left + UsedMenu[Index].ParentPanel->PanelRelField.right ) &&
        UsedMenu[Index].ControlField.top    >= (UsedMenu[Index].ParentPanel->PanelField.top  + UsedMenu[Index].ParentPanel->PanelRelField.top   ) &&
        UsedMenu[Index].ControlField.bottom <= (UsedMenu[Index].ParentPanel->PanelField.top  + UsedMenu[Index].ParentPanel->PanelRelField.bottom)
        ) {
      CombineField (&MenuField, &MenuField, &UsedMenu[Index].ControlField, NULL);
    }
  }

  CutUsedHight = MenuField.bottom - MenuField.top + 1;

  if (OutField.top + CutUsedHight <= OutField.bottom) {
    OutField.top += CutUsedHight;
  }

  CopyRect (ResultField, &OutField);

  return EFI_SUCCESS;
}

/**
 Get string filed data from input panel field data and input string

 @param [in,out] ResultField         Output string field data
 @param [in]     PanelField          Input panel field data
 @param [in,out] LimitInField        Limit ield data
 @param [in]     Text                Pointer to input string

 @retval EFI_SUCCESS                Get string filed data successfully
 @retval Other                      Fail by calling GetLineByWidth ()

**/
EFI_STATUS
GetStringField (
  IN OUT RECT                                *ResultField,
  IN     RECT                                *PanelField,
  IN OUT RECT                                *LimitInField OPTIONAL,
  IN     CHAR16                              *Text
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Y;
  UINT32                                     X;
  UINT32                                     LineWidth;
  UINT32                                     SeparateStringNum;
  UINT32                                     *SeparateStringOffset;
  CHAR16                                     *SeparateStrings;
  UINT32                                     Index;
  UINT32                                     MaxStringWidth;
  UINT32                                     StringWidth;

  //
  // Get Text Row, Col and Width
  //
  // |--------------------------> Org LineWidth <-------------------------------------|
  //  --------------------------------------------------------------------------------
  // |     ? char     |---------> UserLineWidth <--------|      (? + 1) chars         |
  //  --------------------------------------------------------------------------------
  // ^(Field->Left) ^ (LimitInField->Left)           ^ (LimitInField->Right)       ^(Field->Right)
  //
  Y         = PanelField->top;
  X         = PanelField->left;
  LineWidth = PanelField->right - PanelField->left;

  if (LimitInField != NULL) {
    Y += LimitInField->top;
    X += LimitInField->left;

    if (LimitInField->right > LimitInField->left) {
      LineWidth = LimitInField->right - LimitInField->left;
    } else {
      LineWidth -= LimitInField->left;
    }
  }

  //
  // Get string list (Segment Text with LineWidth and count number of lines)
  //
  SeparateStrings = NULL;
  Status = GetLineByWidth (
             Text,
             LineWidth,
             &SeparateStringNum,
             &SeparateStringOffset,
             &SeparateStrings
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (LimitInField != NULL) {
    //
    // Set LimitInField
    //
    MaxStringWidth = 0;
    for (Index = 0; Index < SeparateStringNum; Index ++) {
      StringWidth    = ((UINT32) GetStringWidth (&SeparateStrings[SeparateStringOffset[Index]]) / sizeof (CHAR16)) - 1;
      MaxStringWidth = MAX (MaxStringWidth, StringWidth);
    }

    LimitInField->right  = MaxStringWidth;
    LimitInField->bottom = LimitInField->top + SeparateStringNum - 1;
  }

  ResultField->left   = X;
  ResultField->right  = X + LineWidth - 1;
  ResultField->top    = Y;
  ResultField->bottom = Y + SeparateStringNum - 1;

  SafeFreePool ((VOID **) &SeparateStrings);
  SafeFreePool ((VOID **) &SeparateStringOffset);

  return EFI_SUCCESS;
}

/**
 Get control field data

 @param [in] PanelCount             Panel list count
 @param [in] PanelList              Panel list
 @param [in] ControlListCount       Control list count
 @param [in] ControlListArray       Control list

 @retval EFI_SUCCESS                Get control field data successfully
 @retval EFI_INVALID_PARAMETER      PanelCount or ControlListCount is zero or PanelList or ControlListArray is NULL

**/
EFI_STATUS
GetControlsField (
  IN H2O_PANEL_INFO                          *StylePanel,
  IN UINT32                                  PanelCount,
  IN H2O_PANEL_INFO                          *PanelList,
  IN UINT32                                  ControlListCount,
  IN H2O_CONTROL_LIST                        *ControlListArray
  )
{
  BOOLEAN                                    IsHorizontalOrder;
  BOOLEAN                                    IsNoScrollBar;
  BOOLEAN                                    IsJustified;
  BOOLEAN                                    IsMinimum;
  UINT32                                     PanelIndex;
  UINT32                                     ControlListIndex;
  UINT32                                     ControlIndex;
  RECT                                       PanelField;
  RECT                                       *PanelRelField;
  RECT                                       *LimitField;
  INT32                                      MaxEndX;
  INT32                                      MaxEndY;
  INT32                                      *MaxWidth;
  INT32                                      *MaxHeight;
  UINT32                                     BorderWidth;
  UINT32                                     ControlCount;
  H2O_CONTROL_INFO                           *Control;
  UINT32                                     ControlWidth;
  UINT32                                     ControlHeight;
  UINT32                                     PageCount;
  INT32                                      PageWidth;
  INT32                                      PageHeight;
  INT32                                      PageIndex;
  BOOLEAN                                    IsNextPage;
  UINT32                                     Index;
  UINT32                                     OrderFlag;

  if (PanelCount == 0 || PanelList == NULL || ControlListCount == 0 || ControlListArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PanelCount != ControlListCount) {
    return EFI_INVALID_PARAMETER;
  }

  OrderFlag         = PanelList[0].OrderFlag;
  IsHorizontalOrder = ((OrderFlag & H2O_PANEL_ORDER_FLAG_HORIZONTAL_FIRST) != 0) ? TRUE : FALSE;
  IsNoScrollBar     = ((OrderFlag & H2O_PANEL_ORDER_FLAG_NO_SCROLL_BAR   ) != 0) ? TRUE : FALSE;
  IsJustified       = ((OrderFlag & H2O_PANEL_ORDER_FLAG_JUSTIFIED       ) != 0) ? TRUE : FALSE;
  IsMinimum         = ((OrderFlag & H2O_PANEL_ORDER_FLAG_MINIMUM         ) != 0) ? TRUE : FALSE;

  BorderWidth = GetBorderWidth (StylePanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

  //
  // Get max control count from ControlListArray
  //
  ControlCount = 0;
  for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex++) {
    ControlCount = MAX (ControlCount, ControlListArray[ControlListIndex].Count);
  }

  //
  // Init LimitField
  //
  LimitField    = AllocateZeroPool (sizeof (RECT)   * PanelCount);
  MaxWidth      = AllocateZeroPool (sizeof (UINT32) * PanelCount);
  MaxHeight     = AllocateZeroPool (sizeof (UINT32) * PanelCount);
  PanelRelField = AllocateZeroPool (sizeof (RECT)   * PanelCount);
  if (LimitField == NULL || MaxWidth == NULL || MaxHeight == NULL || PanelRelField == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MaxEndX = 0;
  MaxEndY = 0;

  for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex++) {
    CopyRect (&PanelRelField[ControlListIndex] , &PanelList[ControlListIndex].PanelField);

    PanelRelField[ControlListIndex].right  = (PanelRelField[ControlListIndex].right  - PanelRelField[ControlListIndex].left) + 1;
    PanelRelField[ControlListIndex].bottom = (PanelRelField[ControlListIndex].bottom - PanelRelField[ControlListIndex].top ) + 1;
    PanelRelField[ControlListIndex].left   = 0;
    PanelRelField[ControlListIndex].top    = 0;
    PanelRelField[ControlListIndex].right  -= (2 * BorderWidth);
    PanelRelField[ControlListIndex].bottom -= (2 * BorderWidth);
  }

  for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex++) {
    //
    // Get each control in each ControlArray
    //
    for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex++) {
      if (ControlIndex >= ControlListArray[ControlListIndex].Count) {
        continue;
      }

      //
      // Get Field for all ControlArray
      //
      Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];

      GetStringField (
        &Control->ControlField,
        &PanelRelField[ControlListIndex],
        &LimitField[ControlListIndex],
        Control->Text.String
        );
      //
      // Get max width of field
      //
      MaxWidth[ControlListIndex]  = MAX (MaxWidth[ControlListIndex] , LimitField[ControlListIndex].right);
      MaxHeight[ControlListIndex] = MAX (MaxHeight[ControlListIndex], LimitField[ControlListIndex].bottom - LimitField[ControlListIndex].top + 1);
      //
      // Get max end of field
      //
      MaxEndX = MAX (MaxEndX, LimitField[ControlListIndex].right);
      MaxEndY = MAX (MaxEndY, LimitField[ControlListIndex].bottom);
    }

    //
    // Update Limit Field
    //
    for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex++) {
      if (ControlIndex >= ControlListArray[ControlListIndex].Count) {
        continue;
      }

      Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];

      if (IsHorizontalOrder) {
        LimitField[ControlListIndex].left   = MaxEndX + 1;
        LimitField[ControlListIndex].right  = 0;
        LimitField[ControlListIndex].bottom = 0;

        Control->ControlField.right = PanelRelField[ControlListIndex].left + MaxEndX;
      } else {
        LimitField[ControlListIndex].top    = MaxEndY + 1;
        LimitField[ControlListIndex].right  = 0;
        LimitField[ControlListIndex].bottom = 0;

        Control->ControlField.bottom = PanelRelField[ControlListIndex].top + MaxEndY;
      }
    }
  }

  //
  // Set Page Index
  //
  CopyRect (&PanelField, &PanelList[0].PanelField);
  for (PanelIndex = 0; PanelIndex < PanelCount; PanelIndex ++) {
    UnionRect (&PanelField, &PanelField, &PanelList[PanelIndex].PanelField);
  }
  if (BorderWidth != 0) {
    PanelField.left   += BorderWidth;
    PanelField.right  -= BorderWidth;
    PanelField.top    += BorderWidth;
    PanelField.bottom -= BorderWidth;
  }
  PanelField.right  = (PanelField.right - PanelField.left) + 1;
  PanelField.left   = 0;
  PanelField.bottom = (PanelField.bottom - PanelField.top) + 1;
  PanelField.top    = 0;

  PageWidth  = PanelField.right - PanelField.left + 1;
  PageHeight = PanelField.bottom - PanelField.top + 1;
  PageCount = 1;

  if (IsHorizontalOrder && MaxEndX >= PageWidth) {
    //
    // With Scroll Bar Horizontal
    //
    for (PanelIndex = 0; PanelIndex < PanelCount; PanelIndex ++) {
      PanelList[PanelIndex].WithScrollBarHorizontal = TRUE;
    }
    PageCount = MaxEndX / PageWidth + 1;
    PageIndex = 0;

    for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
      IsNextPage = FALSE;
      for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
        //
        // Get Field for all ControlArray
        //
        if (ControlListArray[ControlListIndex].Count > ControlIndex) {
          Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
          if (Control->ControlField.right > (PageWidth * (PageIndex + 1) + PanelField.left)) {
            IsNextPage = TRUE;
            break;
          }
        }
      }
      if (IsNextPage) {
        PageIndex ++;
      }
      for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
        //
        // Get Field for all ControlArray
        //
        if (ControlListArray[ControlListIndex].Count > ControlIndex) {
          Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
          //Control->PageUpDownIndex = (UINT32) PageIndex;
          //Control->ControlField.Left -= (PageWidth * PageIndex);
          //Control->ControlField.Right -= (PageWidth * PageIndex);
        }
      }
    }
  } else if (MaxEndY >= PageHeight) {
    //
    // With Scroll Bar Vertical
    //
    for (PanelIndex = 0; PanelIndex < PanelCount; PanelIndex ++) {
      PanelList[PanelIndex].WithScrollBarVertical = TRUE;
    }
    PageCount = MaxEndY / PageHeight + 1;
    PageIndex = 0;

    for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
      IsNextPage = FALSE;
      for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
        //
        // Get Field for all ControlArray
        //
        if (ControlListArray[ControlListIndex].Count > ControlIndex) {
          Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
          if (Control->ControlField.bottom > (PageHeight * (PageIndex + 1) + PanelField.top - 1)) {
            IsNextPage = TRUE;
            break;
          }
        }
      }
      if (IsNextPage) {
        PageIndex ++;
      }
      for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
        //
        // Get Field for all ControlArray
        //
        if (ControlListArray[ControlListIndex].Count > ControlIndex) {
          Control = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
          //Control->PageUpDownIndex = (UINT32) PageIndex;
          //Control->ControlField.Top -= (PageHeight * PageIndex);
          //Control->ControlField.Bottom -= (PageHeight * PageIndex);
        }
      }
    }
  }

  //
  // No Scroll Bar
  //
  if (IsNoScrollBar) {
    if (IsHorizontalOrder && PanelList[0].WithScrollBarHorizontal) {
      ControlHeight = MaxHeight[0];
      if (IsJustified) {
        //
        // Justified
        //
        ControlHeight = PageHeight / PageCount;
      }
      for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
        for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
          if (ControlListArray[ControlListIndex].Count > ControlIndex) {
            Control                      = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
            Control->ControlField.top    = PanelField.top; //+ Control->PageUpDownIndex * ControlHeight;
            Control->ControlField.bottom = Control->ControlField.top + ControlHeight - 1;
          }
        }
      }
    } else if (PanelList[0].WithScrollBarVertical) {
      ControlWidth = MaxWidth[0];
      if (IsJustified) {
        //
        // Justified
        //
        ControlWidth = PageWidth / PageCount;
      }
      for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
        for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
          if (ControlListArray[ControlListIndex].Count > ControlIndex) {
            Control                     = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
            Control->ControlField.left  = PanelField.left; //+ Control->PageUpDownIndex * ControlWidth;
            Control->ControlField.right = Control->ControlField.left + ControlWidth - 1;
            if (ControlListIndex != 0) {
              for (Index = 0; Index < ControlListIndex; Index ++) {
                Control->ControlField.left += (MaxWidth[Index] + 1);
              }
            }
            if (ControlListIndex != (ControlListCount - 1)) {
              for (Index = (ControlListCount - 1); Index > ControlListIndex; Index --) {
                Control->ControlField.right -= (MaxWidth[Index] + 1);
              }
            }
          }
        }
      }
    }
  }

  if (IsMinimum) {
    if (IsHorizontalOrder) {
      for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
        for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
          if (ControlListArray[ControlListIndex].Count > ControlIndex) {
            ControlWidth = MaxWidth[ControlListIndex];
            ControlHeight               = MaxHeight[ControlListIndex];
            Control                     = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
            Control->ControlField.left  = PanelField.left + ControlListIndex * ControlWidth;
            Control->ControlField.right = Control->ControlField.left + ControlWidth - 1;
            if (ControlListIndex != 0) {
              for (Index = 0; Index < ControlListIndex; Index ++) {
                Control->ControlField.left += (MaxWidth[Index] + 1);
              }
            }
            if (ControlListIndex != (ControlListCount - 1)) {
              for (Index = (ControlListCount - 1); Index > ControlListIndex; Index --) {
                Control->ControlField.right -= (MaxWidth[Index] + 1);
              }
            }
            Control->ControlField.top    = PanelField.top + ControlListIndex * ControlHeight;
            Control->ControlField.bottom = Control->ControlField.top + ControlHeight - 1;
          }
        }
      }
    } else if (PanelList[0].WithScrollBarVertical) {
      ControlWidth = MaxWidth[0];
      for (ControlIndex = 0; ControlIndex < ControlCount; ControlIndex ++) {
        for (ControlListIndex = 0; ControlListIndex < ControlListCount; ControlListIndex ++) {
          if (ControlListArray[ControlListIndex].Count > ControlIndex) {
            ControlWidth                 = MaxWidth[ControlListIndex];
            ControlHeight                = MaxHeight[ControlListIndex];
            Control                      = &ControlListArray[ControlListIndex].ControlArray[ControlIndex];
            Control->ControlField.top    = PanelField.top + ControlListIndex * ControlHeight;
            Control->ControlField.bottom = Control->ControlField.top + ControlHeight - 1;
            if (ControlListIndex != 0) {
              for (Index = 0; Index < ControlListIndex; Index ++) {
                Control->ControlField.top += (MaxHeight[Index] + 1);
              }
            }
            if (ControlListIndex != (ControlListCount - 1)) {
              for (Index = (ControlListCount - 1); Index > ControlListIndex; Index --) {
                Control->ControlField.bottom -= (MaxHeight[Index] + 1);
              }
            }
            Control->ControlField.left  = PanelField.top + ControlListIndex * ControlWidth;
            Control->ControlField.right = Control->ControlField.top + ControlWidth - 1;
          }
        }
      }
    }
  }

  SafeFreePool ((VOID **) &LimitField);
  SafeFreePool ((VOID **) &MaxWidth);
  SafeFreePool ((VOID **) &MaxHeight);
  SafeFreePool ((VOID **) &PanelRelField);

  return EFI_SUCCESS;
}

/**
 Get cut control data

 @param[in] Control                 Control info
 @param[in] LimitField              Limit field data
 @param[out] ResultControlField     Result field data
 @param[out] ResultControlString    Result control string

 @retval EFI_SUCCESS                Get control field data successfully
 @retval EFI_INVALID_PARAMETER      Control, LimitField, ResultControlField or ResultControlString is NULL
 @retval EFI_NOT_FOUND              ControlField is in LimitField
 @retval EFI_OUT_OF_RESOURCES       Get formatted string fail by call GetLineByWidth () function

**/
STATIC
EFI_STATUS
GetCutControl (
  IN     H2O_CONTROL_INFO                    *Control,
  IN     RECT                                *LimitField,
  OUT    RECT                                *ResultControlField,
  OUT    CHAR16                              **ResultControlString
  )
{
  EFI_STATUS                                 Status;

  CHAR16                                     *TempString;
  UINT32                                     LimitWidth;
  UINT32                                     LimitHieght;
  CHAR16                                     *SeparateStrings;
  UINT32                                     *SeparateStringOffset;
  UINT32                                     SeparateStringNum;

  if (Control == NULL || LimitField == NULL || ResultControlField == NULL || ResultControlString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Control->ControlField.left   >= LimitField->left  &&
      Control->ControlField.right  <= LimitField->right &&
      Control->ControlField.top    >= LimitField->top   &&
      Control->ControlField.bottom <= LimitField->bottom
      ) {
    //
    // Cut this control is not unnecessary
    //
    return EFI_NOT_FOUND;
  }

  LimitWidth  = LimitField->right - LimitField->left - 2;
  LimitHieght = LimitField->bottom - LimitField->top + 1;

  Status = GetLineByWidth (
             Control->Text.String,
             LimitWidth,
             &SeparateStringNum,
             &SeparateStringOffset,
             &SeparateStrings
             );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  TempString = AllocateCopyPool (StrSize (Control->Text.String), Control->Text.String);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (SeparateStringNum > LimitHieght) {
    TempString[SeparateStringOffset[LimitHieght] - 1] = '\0';
  }
  CopyMem (ResultControlField, LimitField, sizeof (RECT));
  ResultControlField->right -= 2;
  *ResultControlString = TempString;

  SafeFreePool ((VOID **) &SeparateStringOffset);

  return EFI_SUCCESS;
}

/**
 Clear field

 @param [in] Attribute             Attribute for background and foreground colors for the OutputString ()
 @param [in] ...                   field data followed by following field data. A NULL terminates the list.

 @return The number of character is cleared

**/
UINT32
ClearField (
  IN  CONST UINT32                          Attribute,
  ...
  )
{
  VA_LIST                                    Args;
  RECT                                       *Field;
  UINT32                                     Count;

  INT32                                      Index;
  INT32                                      ClearWidth;
  CHAR16                                     *ClearString;
  INT32                                      ScreenRow;
  INT32                                      ScreenColumn;

  //
  // Set Field Attribute
  //
  DEConOutSetAttribute (mDEPrivate, (UINTN)Attribute);
  ScreenRow    = (INT32) (PcdGet32 (PcdDisplayEngineLocalTextRow) - 1);
  ScreenColumn = (INT32) (PcdGet32 (PcdDisplayEngineLocalTextColumn) - 1);
  //
  // Clear all Fields
  //
  VA_START (Args, Attribute);
  Count = 0;
  while (TRUE) {
    //
    // Get one of Field
    //
    Field = VA_ARG(Args, RECT *);
    if (Field == NULL) {
      break;
    }

    //
    // Clear one of Field
    //
    ClearWidth = Field->right - Field->left + 1;
    ClearString = AllocateZeroPool (sizeof (CHAR16) * (ClearWidth + 1));
    if (ClearString == NULL) {
      return 0;
    }
    for (Index = 0; Index < ClearWidth; Index ++) {
      ClearString[Index] = ' ';
    }
    for (Index = Field->top; Index <= Field->bottom; Index ++) {
      Count += ClearWidth;
      if ((Index == ScreenRow) && (Field->left + ClearWidth - 1 == ScreenColumn)) {
        ClearString[ClearWidth - 1] = CHAR_NULL;
      }

      DisplayString (Field->left, Index, L"%s", ClearString);
    }
    SafeFreePool ((VOID **) &ClearString);
  }
  VA_END (Args);

  return Count;
}

/**
 Clear field difference between border

 @param [in] Attribute             Attribute for background and foreground colors for the OutputString ()
 @param [in] ...                   field data followed by following field data. A NULL terminates the list.

 @return The number of character is cleared

**/
UINT32
ClearContentField (
  IN     UINT32                              Attribute,
  ...
  )
{
  VA_LIST                                    Args;
  RECT                                       *Field;
  UINT32                                     Count;

  INT32                                      Index;
  INT32                                      ClearWidth;
  CHAR16                                     *ClearString;

  //
  // Set Field Attribute
  //
  DEConOutSetAttribute (mDEPrivate, Attribute);

  //
  // Clear all Fields
  //
  VA_START (Args, Attribute);
  Count = 0;
  while (TRUE) {
    //
    // Get one of Field
    //
    Field = VA_ARG(Args, RECT *);
    if (Field == NULL) {
      break;
    }

    //
    // Clear one of Field
    //
    ASSERT (Field->right > Field->left);
    if (Field->right <= Field->left) {
      continue;
    }
    ClearWidth  = Field->right - Field->left - 1;
    ClearString = AllocateZeroPool (sizeof (CHAR16) * (ClearWidth + 1));
    if (ClearString == NULL) {
      return 0;
    }
    for (Index = 0; Index < ClearWidth; Index ++) {
      ClearString[Index] = ' ';
    }
    for (Index = Field->top + 1; Index <= Field->bottom - 1; Index ++) {
      Count += ClearWidth;
      DisplayString (Field->left + 1, Index, L"%s", ClearString);
    }
    SafeFreePool ((VOID **) &ClearString);
  }
  VA_END (Args);

  return Count;
}

/**
 Clear string in a region

 @param [in] Attribute             Attribute for background and foreground colors for the OutputString ()
 @param [in] Col                   Start column of clear string
 @param [in] Row                   Start row of clear string
 @param [in] LineWidth             String width to be clear

 @return The number of character is cleared

**/
UINT32
ClearString (
  IN     UINT32                          Attribute,
  IN     UINT32                          Col,
  IN     UINT32                          Row,
  IN     UINT32                          LineWidth
  )
{
  RECT                                   Field;

  Field.left   = Col;
  Field.right  = Col + LineWidth - 1;
  Field.top    = Row;
  Field.bottom = Row;

  //
  // Clear all Fields
  //
  return ClearField (Attribute, &Field, NULL);
}

/**
 Display border line

 @param [in] Attribute             Attribute for background and foreground colors for the OutputString ()
 @param [in] ...                   field data followed by following field data. A NULL terminates the list.

 @return The number of field is displayed

**/
UINT32
DisplayBorderLine (
  IN     UINT32                          Attribute,
  ...
  )
{
  VA_LIST                                Args;
  RECT                                   *Field;
  INT32                                  Width;
  INT32                                  Height;
  INT32                                  X;
  INT32                                  Y;
  UINT32                                 Count;
  CHAR16                                 Character;
  CHAR16                                 *String;
  UINT32                                 StringCount;

  //
  // Set Border Line Attribute
  //
  DEConOutSetAttribute (mDEPrivate, Attribute);

  //
  // Display Border Line for all Fields
  //
  VA_START (Args, Attribute);
  Count = 0;
  while (TRUE) {
    //
    // Get one of Field
    //
    Field = VA_ARG( Args, RECT *);
    if (Field == NULL) {
      break;
    }
    Count ++;

    //
    // Display Border Line for one of Field
    //
    Width  = Field->right  - Field->left;
    Height = Field->bottom - Field->top;
    String = AllocateZeroPool (Width * sizeof(CHAR16));
    if (String == NULL) {
      return 0;
    }
    StringCount = 0;

    if (Width < 0 || Height < 0) {
      continue;
    } else if (Width == 0 && Height >= 1) {
      Character = BOXDRAW_DOWN_HORIZONTAL;
      DisplayString (Field->left, Field->top, L"%c", Character);
      Character = BOXDRAW_UP_HORIZONTAL;
      DisplayString (Field->left, Field->bottom, L"%c", Character);
    } else if (Width >= 1 && Height == 0) {
      Character = BOXDRAW_VERTICAL_RIGHT;
      DisplayString (Field->left, Field->top, L"%c", Character);
      Character = BOXDRAW_VERTICAL_LEFT;
      DisplayString (Field->right, Field->top, L"%c", Character);
    } else if (Width >= 1 && Height >= 1) {
      Character = BOXDRAW_DOWN_RIGHT;
      DisplayString (Field->left, Field->top, L"%c", Character);
      Character = BOXDRAW_UP_RIGHT;
      DisplayString (Field->left, Field->bottom, L"%c", Character);
      Character = BOXDRAW_DOWN_LEFT;
      DisplayString (Field->right, Field->top, L"%c", Character);
      Character = BOXDRAW_UP_LEFT;
      DisplayString (Field->right, Field->bottom, L"%c", Character);
    }

    if (Width > 1) {
      for (X = Field->left + 1; X <= Field->right - 1; X++) {
        String[StringCount++] = BOXDRAW_HORIZONTAL;
      }
      DisplayString (Field->left + 1, Field->top, L"%s", String);
      DisplayString (Field->left + 1, Field->bottom, L"%s", String);
    }
    if (Height > 1) {
      for (Y = Field->top + 1; Y <= Field->bottom - 1; Y++) {
        Character = BOXDRAW_VERTICAL;
        DisplayString (Field->left, Y, L"%c", Character);
        DisplayString (Field->right, Y, L"%c", Character);
      }
    }
    SafeFreePool ((VOID **) &String);
  }
  VA_END (Args);

  return Count;
}

/**
 Display panel

 @param [in] Panel                   Target panel info

 @retval EFI_SUCCESS                Display panel successfully

**/
EFI_STATUS
DisplayPanel (
  IN H2O_PANEL_INFO                         *Panel,
  IN UINT32                                  PseudoClass
  )
{
  INT32                                      Index;
  INT32                                      ControlMinY;
  INT32                                      ControlMaxY;
  INT32                                      TotalControlHeight;
  INT32                                      ScrollBarHeight;
  INT32                                      ScrollBarStartY;
  INT32                                      ScrollBarEndY;
  INT32                                      SelectStartY;
  INT32                                      SelectEndY;
  UINT32                                     Attribute;

  Attribute = 0;
  GetPanelColorAttribute (Panel, H2O_IFR_STYLE_TYPE_PANEL, PseudoClass, &Attribute);

  //
  // Clear Field and Display Border Line
  //
  if (Panel->PanelType == H2O_PANEL_TYPE_QUESTION) {
    if ((GetBorderWidth(Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) > 0) {
      ClearField (Attribute, &Panel->PanelField, NULL);
      DisplayBorderLine (Attribute, &Panel->PanelField, NULL);
    }

    //
    // Display Scroll Bar
    //
    if (Panel->WithScrollBarVertical) {
      for (Index = 0, ControlMinY = 0, ControlMaxY = 0; Index < (INT32) Panel->ControlList.Count; Index ++) {
        ControlMinY = MIN (ControlMinY, Panel->ControlList.ControlArray[Index].ControlField.top);
        ControlMaxY = MAX (ControlMaxY, Panel->ControlList.ControlArray[Index].ControlField.bottom);
      }
      TotalControlHeight = ControlMaxY - ControlMinY;
      //
      // Get Scroll Bar Location
      //
      ScrollBarStartY = Panel->PanelField.top;
      ScrollBarEndY   = Panel->PanelField.bottom;
      if ((GetBorderWidth(Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) > 0) {
        ScrollBarStartY += 1;
        ScrollBarEndY   -= 1;
      }
      if (Panel->PanelType == H2O_PANEL_TYPE_QUESTION) {
        //
        // Skip Title
        //
        ScrollBarStartY += 2;
      }
      ScrollBarHeight = ScrollBarEndY - ScrollBarStartY;
      //
      // Get Select Scroll Bar Location
      //
      SelectStartY = Panel->PanelRelField.top * ScrollBarHeight / TotalControlHeight;
      SelectEndY = Panel->PanelRelField.bottom * ScrollBarHeight / TotalControlHeight;
      for (Index = 0; Index < ScrollBarHeight; Index ++) {
        //
        // Set Scroll Bar Attribute
        //
        if (Index >= SelectStartY && Index <= SelectEndY) {
          DEConOutSetAttribute (mDEPrivate, EFI_BACKGROUND_CYAN);
        } else {
          DEConOutSetAttribute (mDEPrivate, EFI_BACKGROUND_BLACK);
        }
        //
        // Display Scroll Bar
        //
        DisplayString (Panel->PanelField.right - 2, ScrollBarStartY + Index, L"  ");
      }
    }
  }
  return EFI_SUCCESS;
}

/**
 Display control info list

 @param [in] IsCentered              Flag to decide to display in center
 @param [in] ButtonStartEndChar      Flag to decide to display string within []
 @param [in] ControlCount            Control list count
 @param [in] ControlArray            Control list

 @retval EFI_SUCCESS                Display control successfully

**/
EFI_STATUS
DisplayControls (
  IN CONST BOOLEAN                           IsCentered,
  IN       BOOLEAN                           ButtonStartEndChar,
  IN       UINT32                            ControlCount,
  IN       H2O_CONTROL_INFO                  *ControlArray
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Attribute;
  RECT                                       ClearControlField;
  UINT32                                     SeparateStringIndex;
  UINT32                                     SeparateStringNum;
  UINT32                                     *SeparateStringOffset;
  CHAR16                                     *SeparateStrings;
  CHAR16                                     *TempString;
  UINT32                                     StringWidth;
  UINT32                                     FieldWidth;
  UINT32                                     FieldHeight;
  UINT32                                     CenteredStartX;
  INT32                                      CenteredStartY;
  UINT32                                     Index;
  CHAR16                                     *String;
  H2O_CONTROL_INFO                           Control;
  INT32                                      BorderLineWidth;

  if (ControlCount == 0 || ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BorderLineWidth = GetBorderWidth (ControlArray[0].ParentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

  for (Index = 0; Index < ControlCount; Index++) {
    CopyMem (&Control, &ControlArray[Index], sizeof (H2O_CONTROL_INFO));

    //
    // Calculate the absolute position of each control field
    //
    OffsetRect (&Control.ControlField, Control.ParentPanel->PanelField.left    , Control.ParentPanel->PanelField.top);
    OffsetRect (&Control.ControlField, -Control.ParentPanel->PanelRelField.left, -Control.ParentPanel->PanelRelField.top);
    OffsetRect (&Control.ControlField, BorderLineWidth                         , BorderLineWidth);

    Attribute = 0;
    GetPanelColorAttribute (Control.ParentPanel, H2O_IFR_STYLE_TYPE_PANEL, Control.ControlStyle.PseudoClass, &Attribute);

    //
    // Separate string by filed width
    //
    FieldWidth = Control.ControlField.right  - Control.ControlField.left + 1;
    String     = CatSPrint (NULL, (ButtonStartEndChar ? L"[%s]" : L"%s"), Control.Text.String);

    Status = GetLineByWidth (
               String,
               FieldWidth,
               &SeparateStringNum,
               &SeparateStringOffset,
               &SeparateStrings
               );
    SafeFreePool ((VOID **) &String);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Display Control
    //
    DEConOutSetAttribute (mDEPrivate, Attribute);

    TempString     = SeparateStrings;
    CenteredStartX = 0;
    CenteredStartY = 0;
    FieldHeight    = Control.ControlField.bottom - Control.ControlField.top  + 1;

    if (IsCentered) {
      if (FieldHeight > 1 && (FieldHeight - SeparateStringNum) > 2) {
        CenteredStartY = (FieldHeight - SeparateStringNum) / 2 - 1;

        CopyRect (&ClearControlField, &Control.ControlField);
        ClearControlField.bottom = ClearControlField.top + CenteredStartY - 1;
        ClearField (Attribute, &ClearControlField, NULL);
      }
    }

    for (SeparateStringIndex = 0; SeparateStringIndex < SeparateStringNum; SeparateStringIndex ++) {
      if (IsCentered) {
        CenteredStartX = Control.ControlField.left + ((FieldWidth - (UINT32)StrLen (TempString)) / 2) - 1;

        CopyRect (&ClearControlField, &Control.ControlField);
        ClearControlField.right   = Control.ControlField.left + CenteredStartX - 1;
        ClearControlField.top    += CenteredStartY + SeparateStringIndex;
        ClearControlField.bottom  = ClearControlField.top;

        if (ClearControlField.right >= ClearControlField.left) {
          ClearField (Attribute, &ClearControlField, NULL);
        }
      }

      //
      // Print one of separate string
      //
      DisplayString (
        Control.ControlField.left + CenteredStartX,
        Control.ControlField.top + CenteredStartY + SeparateStringIndex,
        L"%s",
        TempString
        );
      TempString += StrLen (TempString) + 1;
      if (TempString[0] == 0x000a) {
        TempString++;
      }

      CopyRect (&ClearControlField, &Control.ControlField);
      StringWidth = ((UINT32) GetStringWidth (&SeparateStrings[SeparateStringOffset[SeparateStringIndex]]) / sizeof (CHAR16)) - 1;
      ClearControlField.left   += (StringWidth > 0) ?  (CenteredStartX + StringWidth) : CenteredStartX;
      ClearControlField.top    += CenteredStartY + SeparateStringIndex;
      ClearControlField.bottom  = ClearControlField.top;
      if (ClearControlField.right >= ClearControlField.left) {
        ClearField (Attribute, &ClearControlField, NULL);
      }
    }

    //
    // Clear remaining field
    //
    CopyRect (&ClearControlField, &Control.ControlField);
    ClearControlField.top += CenteredStartY + SeparateStringIndex;
    if (ClearControlField.bottom >= ClearControlField.top) {
      ClearField (Attribute, &ClearControlField, NULL);
    }

    SafeFreePool((VOID **) &SeparateStrings);
    SafeFreePool((VOID **) &SeparateStringOffset);
  }

  return EFI_SUCCESS;
}

/**
 Display normal control info list

 @param [in] ControlCount            Control list count
 @param [in] ControlArray            Control list

 @retval EFI_SUCCESS                Display control successfully
 @retval EFI_INVALID_PARAMETER      ControlCount is zero or ControlArray is NULL

**/
EFI_STATUS
DisplayNormalControls (
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Count;
  UINT32                                     Index;
  UINT32                                     Start;
  BOOLEAN                                    SetStart;

  if (ControlCount == 0 || ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check array is not empty
  //
  Count = 0;
  Index = 0;
  Start = 0;
  SetStart = FALSE;
  while (ControlArray[Index].Text.String != NULL) {
    if (ControlArray[Index].ControlField.left   >= ControlArray[Index].ParentPanel->PanelRelField.left  &&
        ControlArray[Index].ControlField.right  <= ControlArray[Index].ParentPanel->PanelRelField.right &&
        ControlArray[Index].ControlField.top    >= ControlArray[Index].ParentPanel->PanelRelField.top   &&
        ControlArray[Index].ControlField.bottom <= ControlArray[Index].ParentPanel->PanelRelField.bottom) {
      if (!SetStart) {
        SetStart = TRUE;
        Start = Index;
      }
      Count++;
    }
    Index++;

    if (ControlCount <= Index) {
      break;
    }
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  if (SetStart) {
    Status = DisplayControls (
               FALSE,
               FALSE,
               Count,
               &ControlArray[Start]
               );
  }
  return Status;
}

/**
 Display button control info list

 @param [in] ButtonStartEndChar     Flag to decide to display string within []
 @param [in] ControlCount           Control list count
 @param [in] ControlArray           Control list

 @retval EFI_SUCCESS                Display control successfully
 @retval EFI_INVALID_PARAMETER      ControlCount is zero or ControlArray is NULL

**/
EFI_STATUS
DisplayButtonControls (
  IN     BOOLEAN                             ButtonStartEndChar,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Count;
  UINT32                                     Index;
  UINT32                                     Start;
  BOOLEAN                                    SetStart;


  if (ControlCount == 0 || ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check array is not empty
  //
  Count = 0;
  Index = 0;
  Start = 0;
  SetStart = FALSE;
  while (ControlArray[Index].Text.String != NULL) {
    if (
      ControlArray[Index].ControlField.left   >= ControlArray[Index].ParentPanel->PanelRelField.left &&
      ControlArray[Index].ControlField.right  <= ControlArray[Index].ParentPanel->PanelRelField.right &&
      ControlArray[Index].ControlField.top    >= ControlArray[Index].ParentPanel->PanelRelField.top &&
      ControlArray[Index].ControlField.bottom <= ControlArray[Index].ParentPanel->PanelRelField.bottom
      ) {
      if (!SetStart) {
        SetStart = TRUE;
        Start = Index;
      }
      Count ++;
    }
    Index ++;
    if (ControlCount <= Index) {
      break;
    }
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  if (SetStart) {
    //
    // Display buttons
    //
    Status = DisplayControls (
               FALSE,
               ButtonStartEndChar,
               Count,
               ControlArray
               );
  }
  return Status;
}

/**
 Display centered control info list

 @param [in] ButtonStartEndChar     Flag to decide to display string within []
 @param [in] ControlCount           Control list count
 @param [in] ControlArray           Control list

 @retval EFI_SUCCESS                Display control successfully
 @retval EFI_INVALID_PARAMETER      ControlCount is zero or ControlArray is NULL

**/
EFI_STATUS
DisplayCenteredControl (
  IN     BOOLEAN                             ButtonStartEndChar,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Count;
  UINT32                                     Index;
  UINT32                                     Start;
  BOOLEAN                                    SetStart;


  if (ControlCount == 0 || ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check array is not empty
  //
  Count = 0;
  Index = 0;
  Start = 0;
  SetStart = FALSE;
  while (ControlArray[Index].Text.String != NULL) {
    if (
      ControlArray[Index].ControlField.left   >= ControlArray[Index].ParentPanel->PanelRelField.left &&
      ControlArray[Index].ControlField.right  <= ControlArray[Index].ParentPanel->PanelRelField.right &&
      ControlArray[Index].ControlField.top    >= ControlArray[Index].ParentPanel->PanelRelField.top &&
      ControlArray[Index].ControlField.bottom <= ControlArray[Index].ParentPanel->PanelRelField.bottom
      ) {
      if (!SetStart) {
        SetStart = TRUE;
        Start = Index;
      }
      Count ++;
    }
    Index ++;
    if (ControlCount <= Index) {
      break;
    }
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  if (SetStart) {
    //
    // Display buttons
    //
    Status = DisplayControls (
               TRUE,
               ButtonStartEndChar,
               Count,
               ControlArray
               );
  }
  return Status;
}

/**
 Display control in highlight

 @param [in] ButtonStartEndChar     Flag to decide to display string within []
 @param [in] Control                Control info

 @retval EFI_SUCCESS                Display control in highlight successfully
 @retval EFI_INVALID_PARAMETER      Control is NULL
 @retval Other                      Display control fail

**/
EFI_STATUS
DisplayHighLightControl (
  IN BOOLEAN                                 ButtonStartEndChar,
  IN H2O_CONTROL_INFO                        *Control
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     TempPseudoClass;
  CHAR16                                     *TempString;
  RECT                                       TempField;

  if (Control == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status     = EFI_NOT_FOUND;
  TempString = NULL;

  if (Control->ParentPanel->PanelType == H2O_PANEL_TYPE_HELP_TEXT){
    TempString = Control->Text.String;
    CopyRect (&TempField, &Control->ControlField);
    //
    // Cut Control
    // Cut: If content exceed control, it cut content
    //
    Status = GetCutControl (Control, &Control->ParentPanel->PanelRelField, &Control->ControlField, &Control->Text.String);
    if (Status == EFI_SUCCESS) {
      //
      // Really cut
      //
      Control->ParentPanel->WithScrollBarVertical = TRUE;
      DisplayPanel (Control->ParentPanel, H2O_STYLE_PSEUDO_CLASS_NORMAL);
    } else {
      TempString = NULL;
      Control->ParentPanel->WithScrollBarVertical = FALSE;
    }
  }

  if (Control->ControlField.left   >= Control->ParentPanel->PanelRelField.left  &&
      Control->ControlField.right  <= Control->ParentPanel->PanelRelField.right &&
      Control->ControlField.top    >= Control->ParentPanel->PanelRelField.top   &&
      Control->ControlField.bottom <= Control->ParentPanel->PanelRelField.bottom
      ) {
    TempPseudoClass = Control->ControlStyle.PseudoClass;
    //
    // Highlight
    //
    if (Control->Editable) {
      Control->ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_FOCUS;
    } else if (Control->Selectable) {
      Control->ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_FOCUS;
    } else {
      Control->ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_HIGHLIGHT;
    }
    Status = DisplayControls (
               FALSE,
               ButtonStartEndChar,
               1,
               Control
               );
    Control->ControlStyle.PseudoClass = TempPseudoClass;

    if (TempString != NULL) {
      SafeFreePool ((VOID **) &Control->Text.String);
      Control->Text.String = TempString;
      CopyRect (&Control->ControlField, &TempField);
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
 Display control in specific PseudoClass

 @param [in] PseudoClass            Displayed controls PseudoClass
 @param [in] ControlArray           Controls info

 @retval EFI_SUCCESS                Display control in highlight successfully
 @retval EFI_INVALID_PARAMETER      Control is NULL
 @retval Other                      Display control fail

**/
EFI_STATUS
DisplayPseudoClassControl (
  IN     UINT32                              PseudoClass,
  IN     UINT32                              ControlCount,
  IN     H2O_CONTROL_INFO                    *ControlArray
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     Count;
  UINT32                                     Index;
  UINT32                                     Start;
  BOOLEAN                                    SetStart;
  UINT32                                     TempPseudoClass;


  if (ControlCount == 0 || ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check array is not empty
  //
  Count = 0;
  Index = 0;
  Start = 0;
  SetStart = FALSE;
  while (ControlArray[Index].Text.String != NULL) {
    if (
      ControlArray[Index].ControlField.left   >= ControlArray[Index].ParentPanel->PanelRelField.left  &&
      ControlArray[Index].ControlField.right  <= ControlArray[Index].ParentPanel->PanelRelField.right &&
      ControlArray[Index].ControlField.top    >= ControlArray[Index].ParentPanel->PanelRelField.top   &&
      ControlArray[Index].ControlField.bottom <= ControlArray[Index].ParentPanel->PanelRelField.bottom
      ) {
      if (!SetStart) {
        SetStart = TRUE;
        Start = Index;
      }
      //
      // Change PseudoClass
      //
      TempPseudoClass = ControlArray[Index].ControlStyle.PseudoClass;
      ControlArray[Index].ControlStyle.PseudoClass = PseudoClass;

      Status = EFI_NOT_FOUND;
      Status = DisplayControls (
                 FALSE,
                 FALSE,
                 1,
                 &ControlArray[Index]
                 );
      ControlArray[Index].ControlStyle.PseudoClass = TempPseudoClass;
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Count ++;
    }
    Index ++;
    if (ControlCount <= Index) {
      break;
    }
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 Process page up for panel info

 @param [in] Panel                  Panel info

 @retval EFI_SUCCESS                Process page up successfully

**/
EFI_STATUS
PanelPageUp (
  IN     H2O_PANEL_INFO                      *Panel
  )
{
  INT32                                      MoveHeight;


  MoveHeight = Panel->PanelField.bottom - Panel->PanelField.top;
  if ((GetBorderWidth(Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) > 0) {
    MoveHeight -= (BORDER_TOP_WIDTH_DEFLULT + BORDER_BOTTOM_WIDTH_DEFLULT);
  }

  if (Panel->PanelRelField.top < MoveHeight) {
    Panel->PanelRelField.top    = 0;
    Panel->PanelRelField.bottom = MoveHeight;
  } else {
    Panel->PanelRelField.top    -= MoveHeight;
    Panel->PanelRelField.bottom -= MoveHeight;
  }

  return EFI_SUCCESS;
}

/**
 Process page down for panel info

 @param [in] Panel                  Panel info

 @retval EFI_SUCCESS                Process page down successfully

**/
EFI_STATUS
PanelPageDown (
  IN     H2O_PANEL_INFO                      *Panel
  )
{
  INT32                                      MoveHeight;
  UINT32                                     Index;
  INT32                                      ControlMaxY;


  MoveHeight = Panel->PanelField.bottom - Panel->PanelField.top;
  if ((GetBorderWidth(Panel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) > 0) {
    MoveHeight -= (BORDER_TOP_WIDTH_DEFLULT + BORDER_BOTTOM_WIDTH_DEFLULT);
  }

  ControlMaxY = 0;
  for (Index = 0; Index < Panel->ControlList.Count; Index ++) {
    ControlMaxY = MAX (ControlMaxY, Panel->ControlList.ControlArray[Index].ControlField.bottom);
  }

  if (ControlMaxY < MoveHeight) {
    Panel->PanelRelField.top    = 0;
    Panel->PanelRelField.bottom = MoveHeight;
  } else {
    Panel->PanelRelField.top    += MoveHeight;
    Panel->PanelRelField.bottom += MoveHeight;
    if (Panel->PanelRelField.bottom > ControlMaxY) {
      Panel->PanelRelField.top    = ControlMaxY - MoveHeight;
      Panel->PanelRelField.bottom = ControlMaxY;
    }
  }

  return EFI_SUCCESS;
}

/**
 Update control info to make sure the field of control is within page

 @param [in] Control                Control info

 @retval EFI_SUCCESS                Update control info successfully
 @retval EFI_INVALID_PARAMETER      Control is NULL or Control->ParentPanel is NULL
 @retval EFI_NOT_FOUND              No need to update control info

**/
EFI_STATUS
EnsureControlInPanel (
  IN     H2O_CONTROL_INFO                    *Control
  )
{
  if (Control == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Control->ParentPanel == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Control->ControlField.left   < Control->ParentPanel->PanelRelField.left  ||
      Control->ControlField.right  > Control->ParentPanel->PanelRelField.right ||
      Control->ControlField.top    < Control->ParentPanel->PanelRelField.top   ||
      Control->ControlField.bottom > Control->ParentPanel->PanelRelField.bottom
      ) {
    //
    // Change panel
    //
    Control->ParentPanel->PanelRelField.left   = Control->ControlField.left;
    Control->ParentPanel->PanelRelField.top    = Control->ControlField.top;
    Control->ParentPanel->PanelRelField.right  = Control->ParentPanel->PanelRelField.left + (Control->ParentPanel->PanelField.right - Control->ParentPanel->PanelField.left);
    Control->ParentPanel->PanelRelField.bottom = Control->ParentPanel->PanelRelField.top  + (Control->ParentPanel->PanelField.bottom - Control->ParentPanel->PanelField.top);
    if ((GetBorderWidth(Control->ParentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) > 0) {
      Control->ParentPanel->PanelRelField.right  -= (BORDER_TOP_WIDTH_DEFLULT + BORDER_BOTTOM_WIDTH_DEFLULT);
      Control->ParentPanel->PanelRelField.bottom -= (BORDER_TOP_WIDTH_DEFLULT + BORDER_BOTTOM_WIDTH_DEFLULT);
    }
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

BOOLEAN
CheckPressControls (
  IN     BOOLEAN                              Keyboard,
  IN     EFI_INPUT_KEY                        *Key,
  IN     INT32                                RealMouseX,
  IN     INT32                                RealMouseY,
  IN     UINT32                               ControlCount,
  IN     H2O_CONTROL_INFO                     *ControlArray,
  IN OUT H2O_CONTROL_INFO                     **SelectedControl
  )
{
  INT32                                       MouseX;
  INT32                                       MouseY;
  UINT32                                      MaxCol;
  UINT32                                      MaxRow;
  H2O_CONTROL_INFO                            *CurrentControl;
  UINT32                                      Index;
  UINT32                                      SelectedIndex;
  BOOLEAN                                     Found;
  RECT                                        ControlField;
  INT32                                       BorderLineWidth;

  if (Key == NULL) {
    return FALSE;
  }

  GetScreenMaxXY (&MaxCol, &MaxRow);

  if (RealMouseX >= (INT32) (MaxCol - 1) * NARROW_TEXT_WIDTH) {
    MouseX = (INT32) MaxCol;
  } else {
    MouseX = (RealMouseX > 0) ? (RealMouseX / NARROW_TEXT_WIDTH) : 0;
  }

  if (RealMouseY >= (INT32) MaxRow * TEXT_HEIGHT) {
    MouseY = (INT32) MaxRow;
  } else {
    MouseY = (RealMouseY > 0) ? (RealMouseY / TEXT_HEIGHT) : 0;
  }

  Found          = FALSE;
  CurrentControl = *SelectedControl;

  if (!Keyboard) {
    //
    // Mouse Event
    //
    if (Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (ControlCount == 0) {
        return Found;
      }

      //
      // Press Mouse Left
      //
      BorderLineWidth = GetBorderWidth (ControlArray[0].ParentPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);

      for (Index = 0; Index < ControlCount; Index++) {
        ControlField.left   = ControlArray[Index].ControlField.left   + ControlArray[Index].ParentPanel->PanelField.left + BorderLineWidth;
        ControlField.right  = ControlArray[Index].ControlField.right  + ControlArray[Index].ParentPanel->PanelField.left + BorderLineWidth;
        ControlField.top    = ControlArray[Index].ControlField.top    + ControlArray[Index].ParentPanel->PanelField.top  + BorderLineWidth;
        ControlField.bottom = ControlArray[Index].ControlField.bottom + ControlArray[Index].ParentPanel->PanelField.top  + BorderLineWidth;

        if (MouseX >= ControlField.left  && MouseY >= ControlField.top &&
            MouseX <= ControlField.right && MouseY <= ControlField.bottom) {
          //
          // Control has been pressed
          //
          CurrentControl = &ControlArray[Index];
          Found          = TRUE;
          break;
        }
      }
    } else if (Key->ScanCode == SCAN_ESC) {
      //
      // Press Mouse Right: Exit
      //
      CurrentControl = NULL;
      Found          = TRUE;
    }
  } else {
    //
    // Keyboard Event
    //
    switch (Key->UnicodeChar) {

    case CHAR_CARRIAGE_RETURN:
      CurrentControl = *SelectedControl;
      Found          = TRUE;
      break;

    default:
      if(Key->ScanCode == SCAN_ESC) {
        CurrentControl = NULL;
        Found          = TRUE;
      } else if (Key->ScanCode == SCAN_LEFT || Key->ScanCode == SCAN_UP) {
        for (Index = 0; Index < ControlCount; Index ++) {
          if ((UINTN)(UINTN *)CurrentControl == (UINTN)(UINTN *)&ControlArray[Index]) {
            SelectedIndex  = (Index > 0) ? (Index - 1) : (ControlCount - 1);
            CurrentControl = &ControlArray[SelectedIndex];
            Found          = TRUE;
            break;
          }
        }
      } else if (Key->ScanCode == SCAN_RIGHT || Key->ScanCode == SCAN_DOWN) {
        for (Index = 0; Index < ControlCount; Index ++) {
          if ((UINTN)(UINTN *)CurrentControl == (UINTN)(UINTN *)&ControlArray[Index]) {
            SelectedIndex  = (Index < (ControlCount - 1)) ? (Index + 1) : 0;
            CurrentControl = &ControlArray[SelectedIndex];
            Found          = TRUE;
            break;
          }
        }
      }
      break;
    }
  }

  *SelectedControl = CurrentControl;

  return Found;
}

EFI_STATUS
CalculateRequireSize (
  IN  CHAR16                                 *DisplayString,
  IN  UINT32                                 LimitLineWidth,
  OUT UINT32                                 *RequireWidth,
  OUT UINT32                                 *RequireHeight
  )
{
  EFI_STATUS                                 Status;
  UINT32                                     SeparateStringNum;
  UINT32                                     *SeparateStringOffset;
  CHAR16                                     *SeparateString;
  UINT32                                     Index;
  UINT32                                     StringWidth;
  UINT32                                     MaxStringWidth;

  if (DisplayString == NULL || RequireWidth == NULL || RequireHeight == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLineByWidth (
             DisplayString,
             LimitLineWidth,
             &SeparateStringNum,
             &SeparateStringOffset,
             &SeparateString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MaxStringWidth = 0;
  for (Index = 0; Index < SeparateStringNum; Index++) {
    StringWidth    = ((UINT32) GetStringWidth (&SeparateString[SeparateStringOffset[Index]]) / sizeof (CHAR16)) - 1;
    MaxStringWidth = MAX (MaxStringWidth, StringWidth);
  }

  *RequireWidth  = MaxStringWidth;
  *RequireHeight = SeparateStringNum;

  SafeFreePool ((VOID **) &SeparateString);
  SafeFreePool ((VOID **) &SeparateStringOffset);

  return EFI_SUCCESS;
}

