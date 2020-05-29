/** @file

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

#include "LayoutSupportLib.h"

RGB_ATTR_STR                               mRgbAttrStr[] = {
  //
  // B    G    R   reserved
  //
  {{0x00, 0x00, 0x00, 0x00}, EFI_BLACK       , {"black"       }},
  {{0x98, 0x00, 0x00, 0x00}, EFI_LIGHTBLUE   , {"lightblue"   }},
  {{0x00, 0x98, 0x00, 0x00}, EFI_LIGHTGREEN  , {"lightgreen"  }},
  {{0x98, 0x98, 0x00, 0x00}, EFI_LIGHTCYAN   , {"lightcyan"   }},
  {{0x00, 0x00, 0x98, 0x00}, EFI_LIGHTRED    , {"lightred"    }},
  {{0x98, 0x00, 0x98, 0x00}, EFI_MAGENTA     , {"magenta"     }},
  {{0x00, 0x98, 0x98, 0x00}, EFI_BROWN       , {"brown"       }},
  {{0x98, 0x98, 0x98, 0x00}, EFI_LIGHTGRAY   , {"lightgray"   }},
  {{0x30, 0x30, 0x30, 0x00}, EFI_DARKGRAY    , {"darkgray"    }},
  {{0xff, 0x00, 0x00, 0x00}, EFI_BLUE        , {"blue"        }},
  {{0x00, 0xff, 0x00, 0x00}, EFI_GREEN       , {"green"       }},
  {{0xff, 0xff, 0x00, 0x00}, EFI_CYAN        , {"cyan"        }},
  {{0x00, 0x00, 0xff, 0x00}, EFI_RED         , {"red"         }},
  {{0xff, 0x00, 0xff, 0x00}, EFI_LIGHTMAGENTA, {"lightmagenta"}},
  {{0x00, 0xff, 0xff, 0x00}, EFI_YELLOW      , {"yellow"      }},
  {{0xff, 0xff, 0xff, 0x00}, EFI_WHITE       , {"white"       }},
  {{0x00, 0x00, 0x00, 0xff}, EFI_BLACK       , {"black"       }}
};

STATIC
BOOLEAN
StringToRgbAttr (
  IN     CHAR8                               *String,
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb,
  OUT    UINT32                              *Attribute
  )
{
  UINT32                                     Index;

  for (Index = 0; mRgbAttrStr[Index].Rgb.Reserved == 0; Index ++) {
    if (AsciiStrCmp (String, &mRgbAttrStr[Index].String[0]) == 0) {
      CopyMem (Rgb, &mRgbAttrStr[Index].Rgb, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      *Attribute = mRgbAttrStr[Index].Attribute;
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
IsNumberStr (
  IN     BOOLEAN                             IsHex,
  IN     CHAR8                               *String
  )
{
  UINT32                                     Index;

  Index = 0;
  while (String[Index] == '\0') {
    if (!IsHex && (String[Index] <= '0' || String[Index] >= '9')) {
      return FALSE;
    } else if (IsHex &&
      (String[Index] <= '0' || String[Index] >= '9') &&
      (String[Index] <= 'a' || String[Index] >= 'f') &&
      (String[Index] <= 'A' || String[Index] >= 'F')) {
      return FALSE;
    }
  }

  return TRUE;
}

STATIC
UINT32
Abs (
  IN     UINT32                              First,
  IN     UINT32                              Second
  )
{
  UINT32                                     Result;

  Result = 0;
  if (First >= Second) {
    Result = First - Second;
  } else {
    Result = Second - First;
  }

  return Result;
}

STATIC
UINT32
ColorGap (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Color1,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Color2
  )
{
  UINT32                                     Result;
  UINT32                                     GapR;
  UINT32                                     GapG;
  UINT32                                     GapB;

  GapR = Abs ((UINT32)Color1->Red, (UINT32)Color2->Red);
  GapG = Abs ((UINT32)Color1->Green, (UINT32)Color2->Green);
  GapB = Abs ((UINT32)Color1->Blue, (UINT32)Color2->Blue);
  Result = GapR * GapR + GapG * GapG + GapB * GapB;

  return Result;
}

STATIC
UINT32
RgbToAttribute (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb
  )
{
  UINT32                                     Index;
  UINT32                                     Gap;
  UINT32                                     MinGap;
  UINT32                                     MinIndex;
  UINT32                                     Attribute;


  MinGap = 0;
  MinIndex = 0;
  for (Index = 0; mRgbAttrStr[Index].Rgb.Reserved == 0; Index ++) {
    Gap = ColorGap (Rgb, &mRgbAttrStr[Index].Rgb);
    if (Gap <= MinGap) {
      //
      // Get Most Closest Attribute
      //
      MinGap = Gap;
      MinIndex = Index;
      if (MinGap == 0) {
        //
        // Same Color
        //
        break;
      }
    }
  }
  Attribute = mRgbAttrStr[MinIndex].Attribute;

  return Attribute;
}

STATIC
EFI_STATUS
ParseHelpImageDisplay (
  IN     CHAR8                                 *ValueStr,
  OUT    UINT16                                *Result
  )
{
  CHAR8                                        *StrPtr;
  UINT32                                       StrLength;


  StrLength = (UINT32) AsciiStrLen (ValueStr);
  StrPtr = (CHAR8 *)AllocateCopyPool (StrLength + 1, ValueStr);
  StrPtr[StrLength] = '\0';

  if (AsciiStrCmp (StrPtr, "left") == 0) {
    *Result = DISPLAY_LEFT;
  } else if (AsciiStrCmp (StrPtr, "top") == 0) {
    *Result = DISPLAY_TOP;
  } else if (AsciiStrCmp (StrPtr, "right") == 0) {
    *Result = DISPLAY_RIGHT;
  } else if (AsciiStrCmp (StrPtr, "bottom") == 0) {
    *Result = DISPLAY_BOTTOM;
  } else {
    *Result = DISPLAY_NONE;
  }

  SafeFreePool ((VOID **) &StrPtr);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseResolution (
  IN     CHAR8                                 *ValueStr,
  OUT    UINT16                                *Result
  )
{
  CHAR8                                        *StrPtr;
  UINT32                                       StrLength;


  StrLength = (UINT32) AsciiStrLen (ValueStr);
  StrPtr = (CHAR8 *)AllocateCopyPool (StrLength + 1, ValueStr);
  StrPtr[StrLength] = '\0';

  if (AsciiStrCmp (StrPtr, "horizontal") == 0) {
    *Result = RESOLUTION_HORIZONTAL;
  } else {
    *Result = RESOLUTION_VERTICAL;
  }

  SafeFreePool ((VOID **) &StrPtr);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseIdValue (
  IN     CHAR8                                 *ValueStr,
  OUT    UINT16                                *Result
  )
{
  CHAR8                                        *StrPtr;
  UINT32                                       StrLength;


  StrLength = (UINT32) AsciiStrLen (ValueStr);
  StrPtr = (CHAR8 *)AllocateCopyPool (StrLength + 1, ValueStr);
  StrPtr[StrLength] = '\0';

  if (StrPtr[0] == '0' && StrPtr[1] == 'x') {
    if (IsNumberStr (TRUE, StrPtr)) {
      *Result = (UINT16)AsciiStrHexToUintn (StrPtr);
    }
  } else {
    if (IsNumberStr (FALSE, StrPtr)) {
      *Result = (UINT16)AsciiStrDecimalToUintn (StrPtr);
    }
  }

  SafeFreePool ((VOID **) &StrPtr);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseColorValue (
  IN     BOOLEAN                               Background,
  IN     CHAR8                                 *ValueStr,
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *Rgb,
  OUT    UINT32                                *Attribute
  )
{
  UINT32                                       RgbNumber;
  UINT32                                       TempAttribute;
  CHAR8                                        *StrPtr;


  StrPtr = ValueStr;
  TempAttribute = 0;

  if (!StringToRgbAttr(StrPtr, Rgb, &TempAttribute) && StrPtr[0] == 'r' && StrPtr[1] == 'g' && StrPtr[2] == 'b') {
    StrPtr += 3;
    if (IsNumberStr (FALSE, StrPtr)) {
      RgbNumber = (UINT32)AsciiStrHexToUintn (StrPtr);
      CopyMem (Rgb, &RgbNumber, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      TempAttribute = RgbToAttribute (Rgb);
    }
  }
  if (Background) {
    if (TempAttribute == EFI_WHITE) {
      TempAttribute = EFI_LIGHTGRAY;
    }
    TempAttribute <<= 4;
    *Attribute &= 0x0f;
    *Attribute |= TempAttribute;
  } else {
    *Attribute &= 0xf0;
    *Attribute |= TempAttribute;
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseLengthValue (
  IN     CHAR8                                 *ValueStr,
  IN     UINT32                                RelativeValue,
  OUT    UINT32                                *Result
  )
{
  CHAR8                                        *StrPtr;
  UINT32                                       StrLength;


  StrLength = (UINT32) AsciiStrLen (ValueStr);
  StrPtr = (CHAR8 *)AllocateCopyPool (StrLength + 1, ValueStr);
  StrPtr[StrLength] = '\0';

  if (StrPtr[StrLength - 1] == '%') {
    StrPtr[StrLength - 1] = '\0';
    if (IsNumberStr (FALSE, StrPtr)) {
      *Result = RelativeValue * (UINT32) AsciiStrDecimalToUintn (StrPtr) / 100;
    }
  } else if (StrPtr[StrLength - 2] == 'e' && StrPtr[StrLength - 1] == 'm') {
    StrPtr[StrLength - 2] = '\0';
    if (StrPtr[0] == '-') {
      StrPtr += 1;
      if (IsNumberStr (FALSE, StrPtr)) {
        *Result = RelativeValue - (UINT32) AsciiStrDecimalToUintn (StrPtr);
      }
      StrPtr -= 1;
    } else {
      if (IsNumberStr (FALSE, StrPtr)) {
        *Result = (UINT32) AsciiStrDecimalToUintn (StrPtr);
      }
    }
  } else if (StrPtr[0] == '0' && StrPtr[1] == 'x') {
    if (IsNumberStr (TRUE, StrPtr)) {
      *Result = (UINT32) AsciiStrHexToUintn (StrPtr);
    }
  } else {
    if (IsNumberStr (FALSE, StrPtr)) {
      *Result = (UINT32) AsciiStrDecimalToUintn (StrPtr);
    }
  }

  SafeFreePool ((VOID **) &StrPtr);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseBooleanValue (
  IN     CHAR8                                 *ValueStr,
  OUT    BOOLEAN                               *Result
  )
{
  CHAR8                                        *StrPtr;


  StrPtr = ValueStr;

  *Result = FALSE;
  if ((StrPtr[0] == 'T' || StrPtr[0] == 't') &&
    (StrPtr[1] == 'R' || StrPtr[1] == 'r') &&
    (StrPtr[2] == 'U' || StrPtr[2] == 'u') &&
    (StrPtr[3] == 'E' || StrPtr[3] == 'e')) {
    *Result = TRUE;
  } else if ((StrPtr[0] == 'F' || StrPtr[0] == 'f') &&
    (StrPtr[1] == 'A' || StrPtr[1] == 'a') &&
    (StrPtr[2] == 'L' || StrPtr[2] == 'l') &&
    (StrPtr[3] == 'S' || StrPtr[3] == 's') &&
    (StrPtr[4] == 'E' || StrPtr[4] == 'e')) {
    *Result = FALSE;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseProperty (
  IN     RECT                                  *RelativeField OPTIONAL,
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  ZeroMem (PropValue, sizeof (H2O_PROPERTY_VALUE));

  if (AsciiStrCmp (Property->Identifier, "left") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->right - RelativeField->left), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "top") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->bottom - RelativeField->top), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "right") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->right - RelativeField->left), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "bottom") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->bottom - RelativeField->top), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "padding-left") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->right - RelativeField->left), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "padding-top") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->bottom - RelativeField->top), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "padding-right") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->right - RelativeField->left), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "padding-bottom") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, (RelativeField->bottom - RelativeField->top), &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "foreground-color") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_COLOR;
    ParseColorValue (FALSE, Property->Value, &PropValue->Value.Color.Rgb, &PropValue->Value.Color.Attribute);
  } else if (AsciiStrCmp (Property->Identifier, "background-color") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_COLOR;
    ParseColorValue (TRUE, Property->Value, &PropValue->Value.Color.Rgb, &PropValue->Value.Color.Attribute);
  } else if (AsciiStrCmp (Property->Identifier, "border-width") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, 1, &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "visibility") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_BOOLEAN;
    ParseBooleanValue (Property->Value, &PropValue->Value.Bool);
  } else if (AsciiStrCmp (Property->Identifier, "background-image") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_IMAGE;
    ParseIdValue (Property->Value, (UINT16 *)&PropValue->Value.Image.ImageId);
  } else if (AsciiStrCmp (Property->Identifier, "font-size") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, 1, &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "font-name") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_STR;
  } else if (AsciiStrCmp (Property->Identifier, "resolution") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseResolution (Property->Value, (UINT16 *)&PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "help-image-display") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseHelpImageDisplay (Property->Value, (UINT16 *)&PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "help-text-show") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_BOOLEAN;
    ParseBooleanValue (Property->Value, &PropValue->Value.Bool);
  } else if (AsciiStrCmp (Property->Identifier, "help-text-font-size") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, 1, &PropValue->Value.U32);
  } else if (AsciiStrCmp (Property->Identifier, "scrollbar-color") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_COLOR;
    ParseColorValue (TRUE, Property->Value, &PropValue->Value.Color.Rgb, &PropValue->Value.Color.Attribute);
  } else if (AsciiStrCmp (Property->Identifier, "scrollbar-width") == 0) {
    PropValue->Type = H2O_PROPERTY_VALUE_TYPE_UINT32;
    ParseLengthValue (Property->Value, 1, &PropValue->Value.U32);
  } else {
    //
    // Mismatched Identifier String
    //
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetPropertyFromPropertyList (
  IN     LIST_ENTRY                            *PropertyListHead,
  IN     CHAR8                                 *Identifier,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *Link;
  H2O_PROPERTY_INFO                            *CurrentProperty;


  if (PropertyListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Identifier == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Link = PropertyListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;

  *Property = NULL;
  CurrentProperty = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentProperty = H2O_PROPERTY_INFO_NODE_FROM_LINK (Link);

    if (AsciiStrCmp (CurrentProperty->Identifier, Identifier) == 0) {
      ParseProperty (RelativeField, CurrentProperty, PropValue);
      *Property = CurrentProperty;
      Status = EFI_SUCCESS;
    }
  } while (!IsNodeAtEnd (PropertyListHead, Link));

  return Status;
}

STATIC
EFI_STATUS
GetPropertyFromStyleList (
  IN     LIST_ENTRY                            *StyleListHead,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *Identifier,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *Link;
  H2O_STYLE_INFO                               *CurrentStyle;

  H2O_PROPERTY_INFO                            *TempProperty;
  UINT8                                        Priority;
  UINT8                                        PriorityOld;
  BOOLEAN                                      Changed;


  if (StyleListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Identifier == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Link = StyleListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  Priority = 0xff;
  PriorityOld = Priority;
  Changed = FALSE;
  *Property = NULL;

  CurrentStyle = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentStyle = H2O_STYLE_INFO_NODE_FROM_LINK (Link);

    if (StyleType != 0 && CurrentStyle->StyleType != 0 && StyleType != CurrentStyle->StyleType) {
      continue;
    }

    if (PseudoClass != 0 && CurrentStyle->PseudoClass != 0 && PseudoClass != CurrentStyle->PseudoClass) {
      continue;
    }

    if (StyleType != 0 && CurrentStyle->StyleType != 0 && PseudoClass != 0 && CurrentStyle->PseudoClass != 0) {
      if (StyleType == CurrentStyle->StyleType && (PseudoClass == CurrentStyle->PseudoClass)) {
        //
        // ".ClassName StyleType : PseudoClass" priority is the highest
        //
        if (Priority >= 1) {
          Priority = 1;
          Changed = TRUE;
        }
      }
    } else if (StyleType != 0 && CurrentStyle->StyleType != 0) {
      if (StyleType == CurrentStyle->StyleType) {
        //
        // ".ClassName StyleType" priority is the second
        //
        if (Priority >= 2) {
          Priority = 2;
          Changed = TRUE;
        }
      }
    } else if (PseudoClass != 0 && CurrentStyle->PseudoClass != 0) {
      if (PseudoClass == CurrentStyle->PseudoClass) {
        //
        // ".ClassName : PseudoClass" priority is the third
        //
        if (Priority >= 3) {
          Priority = 3;
          Changed = TRUE;
        }
      }
    } else if (CurrentStyle->StyleType == 0 && CurrentStyle->PseudoClass == 0) {
      //
      // ".ClassName" priority is the last
      //
      if (Priority >= 4) {
        Priority = 4;
        Changed = TRUE;
      }
    }

    if (Changed) {
      Status = GetPropertyFromPropertyList (&CurrentStyle->PropertyListHead, Identifier, RelativeField, &TempProperty, PropValue);
      if (Status == EFI_SUCCESS) {
        *Property = TempProperty;
        PriorityOld = Priority;
      } else {
        Priority = PriorityOld;
      }
      Changed = FALSE;
    }
  } while (!IsNodeAtEnd (StyleListHead, Link));

  if (*Property == NULL) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetPropertyFromPanel (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *Identifier,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;

  if (Panel == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Identifier == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  //
  // Panel itself style
  //
  Status = GetPropertyFromStyleList (&Panel->StyleListHead, StyleType, PseudoClass, Identifier, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  //
  // Panel's parent layout style
  //
  Status = GetPropertyFromStyleList (&Panel->ParentLayout->StyleListHead, StyleType, PseudoClass, Identifier, RelativeField, Property, PropValue);

  return Status;
}

BOOLEAN
IsVisibility (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return FALSE;
  }

  Status = EFI_NOT_FOUND;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "visibility", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default visibility is true
    //
    return TRUE;
  }

  return PropValue.Value.Bool;
}

EFI_STATUS
GetPanelField (
  IN  H2O_PANEL_INFO                           *Panel,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  RECT                                     *RelativeField,
  OUT RECT                                     *PanelField
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;
  RECT                                         ResultRect;

  if (Panel == NULL || PanelField == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "left", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.left = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "top", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.top = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "right", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.right = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "bottom", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.bottom = PropValue.Value.U32;

  CopyRect (PanelField, &ResultRect);

  return EFI_SUCCESS;
}

EFI_STATUS
GetPaddingRect(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    RECT                                  *Rect
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;
  RECT                                         ResultRect;

  if (Panel == NULL || Rect == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "padding-left", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.left = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "padding-top", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.top = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "padding-right", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.right = PropValue.Value.U32;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "padding-bottom", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResultRect.bottom = PropValue.Value.U32;

  CopyRect (Rect, &ResultRect);

  return EFI_SUCCESS;
}

EFI_STATUS
GetForegroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "foreground-color", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Color, &PropValue.Value.Color, sizeof (H2O_COLOR_INFO));

  return EFI_SUCCESS;
}

EFI_STATUS
GetBackgroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "background-color", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Color, &PropValue.Value.Color, sizeof (H2O_COLOR_INFO));

  return EFI_SUCCESS;
}

EFI_STATUS
GetPanelColorAttribute (
  IN  H2O_PANEL_INFO                           *Panel,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  OUT UINT32                                   *PanelColorAttribute
  )
{
  EFI_STATUS                                   Status;
  H2O_COLOR_INFO                               Color;
  UINT32                                       Attribute;

  Attribute = 0;

  Status = GetForegroundColor (Panel, StyleType, PseudoClass, &Color);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Attribute |= Color.Attribute;

  Status = GetBackgroundColor (Panel, StyleType, PseudoClass, &Color);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Attribute |= Color.Attribute;

  *PanelColorAttribute = Attribute;

  return EFI_SUCCESS;
}

EFI_STATUS
GetBackgroundImage (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_IMAGE_INFO                        *Image
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Image == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "background-image", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Image, &PropValue.Value.Image, sizeof (H2O_IMAGE_INFO));

  return EFI_SUCCESS;
}

UINT32
GetBorderWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "border-width", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

UINT32
GetFontSize (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "font-size", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

CHAR16 *
GetFontName (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;
  CHAR16                                       *FontName;

  if (Panel == NULL) {
    return NULL;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "font-name", NULL, &Property, &PropValue);
  if (Status != EFI_SUCCESS) {
    //
    // Not found, default border width is 0
    //
    return NULL;
  }

  FontName = AllocateZeroPool (sizeof (CHAR16) * (AsciiStrLen (Property->Value) + 1));
  if (FontName != NULL) {
    AsciiStrToUnicodeStr (Property->Value, FontName);
  }

  return FontName;
}

UINT32
GetResolution (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "resolution", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

UINT32
GetHelpImageDisplay (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "help-image-display", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

BOOLEAN
IsHelpTextShow (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return FALSE;
  }

  Status = EFI_NOT_FOUND;

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "help-text-show", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default help-text-show is false
    //
    return FALSE;
  }

  return PropValue.Value.Bool;
}

UINT32
GetHelpTextFontSize (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "help-text-font-size", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

EFI_STATUS
GetScrollbarColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "scrollbar-color", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Color, &PropValue.Value.Color, sizeof (H2O_COLOR_INFO));

  return EFI_SUCCESS;
}

UINT32
GetScrollbarWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromPanel (Panel, StyleType, PseudoClass, "scrollbar-width", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default scrollbar width is 0
    //
    return 0;
  }

  return PropValue.Value.U32;
}

H2O_PANEL_INFO *
GetPannelInfo (
  IN H2O_LAYOUT_INFO                           *LayoutInfo,
  IN UINT32                                    PanelType
  )
{
  LIST_ENTRY                                   *PanelLink;
  H2O_PANEL_INFO                               *Panel;

  ASSERT (LayoutInfo != NULL);

  PanelLink = &LayoutInfo->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return NULL;
  }

  do {
    PanelLink = PanelLink->ForwardLink;
    Panel     = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);

    if (Panel->PanelType == PanelType) {
      return Panel;
    }
  } while (!IsNodeAtEnd (&LayoutInfo->PanelListHead, PanelLink));

  return NULL;
}

