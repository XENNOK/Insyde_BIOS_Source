/** @file
  The helper library for HID descriptor parsing

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HidDescriptorLib.h>

STATIC HID_PARSER               mHidParser;
STATIC struct {
  UINT16  ClassCode;
  UINT16  UsagePage;
  UINT8   Usage;
  UINT8   Value;
  UINT8   Attr;
  UINT8   Offset;
} mHidReportField[] = {
  //
  // The report field lookup table for touch panel
  //
  { 0x0d00, HID_UP_GENDESK,    HID_GD_X,                            0xff, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldX              },
  { 0x0d00, HID_UP_GENDESK,    HID_GD_Y,                            0xff, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldY              },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_TIP_SWITCH,           0x01, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldTouchActive    },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_SECONDARY_TIP_SWITCH, 0x01, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldAltActive      },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_BARREL_SWITCH,        0x01, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldAltActive      },
  { 0x0d00, HID_UP_BUTTON,     1,                                   0x01, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldTouchActive    },
  { 0x0d00, HID_UP_BUTTON,     2,                                   0x01, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldAltActive      },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_CONTACT_COUNT,        0xff, ATTR_TOUCH_PANEL_INPUT, (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanel.FieldContactCount   },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_DEVICE_MODE,          0xff, ATTR_TOUCH_PANEL_MODE,  (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanelMode.FieldDeviceMode },
  { 0x0d00, HID_UP_DIGITIZERS, HID_DIGITIZERS_DEVICE_ID,            0xff, ATTR_TOUCH_PANEL_MODE,  (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.TouchPanelMode.FieldDeviceId   },
  //
  // The report field lookup table for pad
  //
  { 0x0101, HID_UP_GENDESK,    HID_GD_X,                            0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldX                   },
  { 0x0101, HID_UP_GENDESK,    HID_GD_Y,                            0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldY                   },
  { 0x0101, HID_UP_GENDESK,    HID_GD_WHEEL,                        0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldZ                   },
  { 0x0101, HID_UP_BUTTON,     1,                                   0x01, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldButton1             },
  { 0x0101, HID_UP_BUTTON,     2,                                   0x01, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldButton2             },
  //
  // The report field lookup table for mouse
  //
  { 0x0102, HID_UP_GENDESK,    HID_GD_X,                            0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldX                   },
  { 0x0102, HID_UP_GENDESK,    HID_GD_Y,                            0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldY                   },
  { 0x0102, HID_UP_GENDESK,    HID_GD_WHEEL,                        0xff, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldZ                   },
  { 0x0102, HID_UP_BUTTON,     1,                                   0x01, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldButton1             },
  { 0x0102, HID_UP_BUTTON,     2,                                   0x01, ATTR_MOUSE_INPUT,       (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Mouse.FieldButton2             },
  //
  // The report field lookup table for keyboard
  //
  { 0x0106, HID_UP_KEYBOARD,   0xff,                                0x01, ATTR_KEYBOARD_INPUT,    (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Keyboard.FieldModKeyCode       },
  { 0x0106, HID_UP_KEYBOARD,   0xff,                                0xff, ATTR_KEYBOARD_INPUT,    (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.Keyboard.FieldKeyCode          },
  { 0x0106, HID_UP_LED,        0xff,                                0x01, ATTR_KEYBOARD_LED,      (UINT8)(UINTN)&((REPORT_GROUP*)0)->Data.KeyboardLed.FieldLed           },
  //
  // End of table
  //
  { 0x0000, 0,                 0,                                   0,    0,                      0                                                                      },
};

/**

  Get Next Item

  @param  StartPos              Start Position
  @param  EndPos                End Position
  @param  HidItem               HidItem to return

  @retval Position

**/
STATIC
UINT8 *
GetNextItem (
  IN  UINT8                     *StartPos,
  IN  UINT8                     *EndPos,
  OUT HID_ITEM                  *HidItem
  )
{
  UINT8 Temp;

  if ((EndPos - StartPos) <= 0) {
    return NULL;
  }

  Temp = *StartPos;
  StartPos++;
  //
  // bit 2,3
  //
  HidItem->Type = (UINT8) ((Temp >> 2) & 0x03);
  //
  // bit 4-7
  //
  HidItem->Tag = (UINT8) ((Temp >> 4) & 0x0F);

  if (HidItem->Tag == HID_ITEM_TAG_LONG) {
    //
    // Long Items are not supported by HID rev1.0,
    // although we try to parse it.
    //
    HidItem->Format = HID_ITEM_FORMAT_LONG;

    if ((EndPos - StartPos) >= 2) {
      HidItem->Size = *StartPos++;
      HidItem->Tag  = *StartPos++;

      if ((EndPos - StartPos) >= HidItem->Size) {
        HidItem->Data.LongData = StartPos;
        StartPos += HidItem->Size;
        return StartPos;
      }
    }
  } else {
    HidItem->Format = HID_ITEM_FORMAT_SHORT;
    //
    // bit 0, 1
    //
    HidItem->Size   = (UINT8) (Temp & 0x03);
    switch (HidItem->Size) {

    case 0:
      //
      // No data
      //
      return StartPos;

    case 1:
      //
      // One byte data
      //
      if ((EndPos - StartPos) >= 1) {
        HidItem->Data.U8 = *StartPos++;
        return StartPos;
      }

    case 2:
      //
      // Two byte data
      //
      if ((EndPos - StartPos) >= 2) {
        HidItem->Data.U16 = *(UINT16*)StartPos;
        StartPos += 2;
        return StartPos;
      }

    case 3:
      //
      // 4 byte data, adjust size
      //
      HidItem->Size++;
      if ((EndPos - StartPos) >= 4) {
        HidItem->Data.U32 = *(UINT32*)StartPos;
        StartPos += 4;
        return StartPos;
      }
    }
  }

  return NULL;
}

/**

  Get Item Data

  @param  HidItem               HID_ITEM

  @retval HidItem Data
  
**/
STATIC
UINT32
GetItemData (
  IN  HID_ITEM                  *HidItem
  )
{
  //
  // Get Data from HID_ITEM structure
  //
  switch (HidItem->Size) {

  case 1:
    return HidItem->Data.U8;

  case 2:
    return HidItem->Data.U16;

  case 4:
    return HidItem->Data.U32;
  }

  return 0;
}

/**

  Parse Local Item

  @param  LocalItem             Local Item

**/
STATIC
VOID
ParseLocalItem (
  IN  HID_ITEM                  *LocalItem
  )
{
  if (LocalItem->Size == 0) {
    //
    // No expected data for local item
    //
    return ;
  }
  switch (LocalItem->Tag) {
  case HID_LOCAL_ITEM_TAG_USAGE:
    mHidParser.Local.Usage[mHidParser.Local.UsageIndex] = (UINT16)GetItemData (LocalItem);
    mHidParser.Local.UsageIndex ++;
    break;
  case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:
    mHidParser.Local.UsageMin = (UINT16)GetItemData (LocalItem);
    break;
  case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:
    mHidParser.Local.UsageMax = (UINT16)GetItemData (LocalItem);
    break;
  }
}

STATIC
VOID
ParseGlobalItem (
  IN  HID_ITEM                  *GlobalItem
  )
{
  switch (GlobalItem->Tag) {
  case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
    mHidParser.Global.LogicMax = (INT32)GetItemData (GlobalItem);
    break;

  case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
    mHidParser.Global.LogicMin = (INT32)GetItemData (GlobalItem);
    break;

  case HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
    mHidParser.Global.UsagePage = (UINT16)GetItemData (GlobalItem);
    break;

  case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
    mHidParser.Global.ReportSize = (UINT16)GetItemData (GlobalItem);
    break;

  case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
    mHidParser.Global.ReportCount = (UINT16)GetItemData (GlobalItem);
    break;

  case HID_GLOBAL_ITEM_TAG_REPORT_ID:
    mHidParser.Global.ReportId = (UINT8) GetItemData (GlobalItem);
    break;
  
  case HID_GLOBAL_ITEM_TAG_PUSH:
    CopyMem (
      &mHidParser.GlobalStack[mHidParser.GlobalStackPtr],
      &mHidParser.Global,
      sizeof (HID_GLOBAL)
      );
    mHidParser.GlobalStackPtr ++;
    break;

  case HID_GLOBAL_ITEM_TAG_POP:
    mHidParser.GlobalStackPtr --;
    CopyMem (
      &mHidParser.Global,
      &mHidParser.GlobalStack[mHidParser.GlobalStackPtr],
      sizeof (HID_GLOBAL)
      );
    break;
  }
}

/**

  Usage searching

  @param  Usage                 Target

  @retval Usage position

**/
STATIC
UINT8
SearchUsage (
  IN  UINT16                    Usage
  )
{
  UINT8 Index;
  
  if (Usage == 0xff) return 0;
  if (mHidParser.Local.UsageIndex) {
    for (Index = 0; Index < mHidParser.Local.UsageIndex; Index ++) {
      if (mHidParser.Local.Usage[Index] == Usage) {
        return Index;
      }
    }
  } else if (mHidParser.Local.UsageMin && mHidParser.Local.UsageMax) {
    if (Usage >= mHidParser.Local.UsageMin && Usage <= mHidParser.Local.UsageMax) {
      return (UINT8)(Usage - mHidParser.Local.UsageMin);
    }
  }
  return INVALID_USAGE_POSITION;
}

/**

  Finds the report group data corresponding to the current Report ID.  If no report group data
  exists with the current Report ID, a new report group data with the current Report ID is started.

  @param  ReportFieldInfo       REPORT_FIELD_INFO that stores all the report group data
  @param  CurrReportGroup       the report group data corresponding to the current Report ID

  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
FindCurrentReportGroup (
  IN  REPORT_FIELD_INFO         *ReportFieldInfo,
  IN  UINTN                     DataType,
  OUT REPORT_GROUP              **CurrReportGroup
  )
{
  UINTN             Count;
  REPORT_GROUP      *ReportGroup;

  if (!(mHidParser.DataValid)) {
    return EFI_NOT_FOUND;
  }
  //
  // Searching for stock report group
  //
  for (Count = 0, ReportGroup = ReportFieldInfo->ReportGroup; Count < ReportFieldInfo->Total; ReportGroup ++, Count ++) {
    if (ReportGroup->Id == mHidParser.Global.ReportId && ReportGroup->DataType == DataType) {
      //
      // Return stock report group due to the ID exist in the array
      //
      *CurrReportGroup = ReportGroup;
      return EFI_SUCCESS;
    }
  }
  //
  // If current Report ID not found, start a new report group
  //
  ReportGroup = &ReportFieldInfo->ReportGroup[ReportFieldInfo->Total];
  ReportGroup->Id = mHidParser.Global.ReportId;
  ReportGroup->DataType = (UINT8)DataType;
  if (ReportGroup->Id != 0) {
    ReportGroup->DataSize = 8;
  }
  ReportFieldInfo->Total ++;
  *CurrReportGroup = ReportGroup;
  return EFI_SUCCESS;
}

/**

  Fills in report field data. BitOffset and Usage are used together to calculate
  the bit offset at which ReportField should start.

  @param  BitOffset             The number of bits defined for the report data structure so far
  @param  Usage                 The usage corresponding to ReportField
  @param  ReportField           The report field whose data should be updated

  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
FillInReportFieldData (
  IN  UINT16            BitOffset,
  IN  UINT16            Usage,
  OUT REPORT_GROUP      *ReportGroup
  )
{
  UINTN         Index;
  UINT8         UsagePos;
  UINT8         Attr;
  UINT16        RootClassCode;
  REPORT_FIELD  *ReportField;
  
  if (ReportGroup == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  UsagePos = SearchUsage (Usage);
  if (UsagePos == INVALID_USAGE_POSITION) {
    return EFI_NOT_FOUND;
  }
  //
  // Setup the class code used to identify the device class
  //
  RootClassCode = (mHidParser.TopLevelCollectionUsagePage << 8) | (mHidParser.CollectionStack[0].Usage);
  if (mHidParser.TopLevelCollectionUsagePage == HID_UP_DIGITIZERS) {
    //
    // Don't care about page code if it is touch panel
    //
    RootClassCode &= 0xff00;
  }
  for (Index = 0, ReportField = NULL, Attr = 0; mHidReportField[Index].ClassCode != 0; Index ++) {
    if (mHidReportField[Index].ClassCode == RootClassCode &&
        mHidReportField[Index].UsagePage == mHidParser.Global.UsagePage &&
        (mHidReportField[Index].Usage == 0xff || mHidReportField[Index].Usage == Usage) &&
        (mHidReportField[Index].Value == 0xff || mHidReportField[Index].Value == mHidParser.Global.LogicMax)) {
      ReportField = (REPORT_FIELD*)((UINT8*)ReportGroup + mHidReportField[Index].Offset);
      Attr        = mHidReportField[Index].Attr;
      if (ReportField->DataValid) {
        return EFI_SUCCESS;
      }
      break;
    }
  }
  if (ReportField) {
    ReportField->Max       = mHidParser.Global.LogicMax;
    ReportField->Min       = mHidParser.Global.LogicMin;
    ReportField->BitLength = (UINT8) mHidParser.Global.ReportSize;
    ReportField->BitOffset = BitOffset + (UsagePos * mHidParser.Global.ReportSize);
    ReportField->DataValid = TRUE;
    if (!ReportGroup->DataValid) {
      ReportGroup->DataValid = TRUE;
      ReportGroup->DataClass = RootClassCode;
      ReportGroup->DataAttr  = Attr;
    }
  }
  return EFI_SUCCESS;
}

/**

  Updates report group data.

  @param  ReportFieldInfo       REPORT_FIELD_INFO that stores all the report group data
  @param  MainItem              HID_ITEM to parse

  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
UpdateReportGroupData (
  IN  REPORT_FIELD_INFO         *ReportFieldInfo,
  IN  HID_ITEM                  *MainItem
  )
{
  EFI_STATUS        Status;
  REPORT_GROUP      *ReportGroup;
  UINT16            *BitOffset;
  UINT8             Data8;

  if ((ReportFieldInfo == NULL) ||
      (MainItem == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = FindCurrentReportGroup (ReportFieldInfo, MainItem->Tag, &ReportGroup);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BitOffset = &ReportGroup->DataSize;
  Data8 = (UINT8) GetItemData (MainItem);
  //
  // If the item tag has only constant bits, there is no new data to save so just update the bit
  // offset and return.
  //
  if ((Data8 & HID_MAIN_ITEM_CONSTANT) == HID_MAIN_ITEM_CONSTANT) {
    *BitOffset += (mHidParser.Global.ReportSize * mHidParser.Global.ReportCount);
    return EFI_SUCCESS;
  }

  switch (MainItem->Tag) {
  case HID_MAIN_ITEM_TAG_INPUT:
    switch (mHidParser.Global.UsagePage) {
    case HID_UP_GENDESK:
      if (mHidParser.TopLevelCollectionUsagePage == HID_UP_DIGITIZERS && 
          (((Data8 & HID_MAIN_ITEM_RELATIVE) == HID_MAIN_ITEM_RELATIVE) ||
          (((UINT32) mHidParser.Global.LogicMin) >= ((UINT32) mHidParser.Global.LogicMax)))) {
        //
        // Touch panel should report absolute X and Y values; otherwise, the report data will be
        // interpreted incorrectly and result in undefined behaviour.
        //
        break;
      }
      FillInReportFieldData (*BitOffset, HID_GD_X, ReportGroup);
      FillInReportFieldData (*BitOffset, HID_GD_Y, ReportGroup);
      FillInReportFieldData (*BitOffset, HID_GD_WHEEL, ReportGroup);
      break;
    case HID_UP_DIGITIZERS:
      FillInReportFieldData (*BitOffset, HID_DIGITIZERS_TIP_SWITCH, ReportGroup);
      Status = FillInReportFieldData (*BitOffset, HID_DIGITIZERS_SECONDARY_TIP_SWITCH, ReportGroup);
      if (EFI_ERROR (Status)) {
        FillInReportFieldData (*BitOffset, HID_DIGITIZERS_BARREL_SWITCH, ReportGroup);
      }
      FillInReportFieldData (*BitOffset, HID_DIGITIZERS_CONTACT_COUNT, ReportGroup);
      break;
    case HID_UP_BUTTON:
      FillInReportFieldData (*BitOffset, 1, ReportGroup);
      FillInReportFieldData (*BitOffset, 2, ReportGroup);
      break;
    case HID_UP_KEYBOARD:
      FillInReportFieldData (*BitOffset, 0xff, ReportGroup);
      break;
    }
    break;

  case HID_MAIN_ITEM_TAG_OUTPUT:
    switch (mHidParser.Global.UsagePage) {
    case HID_UP_LED:
      FillInReportFieldData (*BitOffset, 0xff, ReportGroup);
      break;
    }
    break;

  case HID_MAIN_ITEM_TAG_FEATURE:
    switch (mHidParser.Global.UsagePage) {
    case HID_UP_DIGITIZERS:
      FillInReportFieldData (*BitOffset, HID_DIGITIZERS_DEVICE_MODE, ReportGroup);
      FillInReportFieldData (*BitOffset, HID_DIGITIZERS_DEVICE_ID, ReportGroup);
      break;

    case HID_UP_LED:
      FillInReportFieldData (*BitOffset, 0xff, ReportGroup);
      break;
    }
    break;

  default:
    return EFI_INVALID_PARAMETER;
    break;
  }
  if (ReportGroup->DataValid) {
    if (ReportGroup->DataAttr == ATTR_TOUCH_PANEL_MODE) {
      if (!ReportFieldInfo->DeviceConfigurationReportGroup && 
          mHidParser.TopLevelCollectionUsagePage == HID_UP_DIGITIZERS &&
          mHidParser.CollectionStack[0].Usage == HID_DIGITIZERS_DEVICE_CONFIGURATION) {
        ReportFieldInfo->DeviceConfigurationReportGroup = ReportGroup;
      }
      if (!ReportFieldInfo->FirstFeatureID) {
        ReportFieldInfo->FirstFeatureID = ReportGroup->Id;
      }
    }
  }
  *BitOffset += (mHidParser.Global.ReportSize * mHidParser.Global.ReportCount);
  return EFI_SUCCESS;
}

/**

  Determines whether the given UsagePage is defined (according to Chapter 3 Table 1 Usage Page
  Summary of the document USB HID Usage Tables version 1.12).

  @param  UsagePage             HID Usage Page

  @retval TRUE
  @retval FALSE                 The UsagePage is either undefined, reserved, or vendor-defined.

**/
STATIC
BOOLEAN
IsUsagePageValid (
  UINT16  UsagePage
  )
{
  if (((UsagePage > 0x00) && (UsagePage < 0x0E)) ||
      (UsagePage == 0x0F) ||
      (UsagePage == 0x10) ||
      (UsagePage == 0x14) ||
      (UsagePage == 0x40) ||
      ((UsagePage > 0x7F) && (UsagePage < 0x88)) ||
      ((UsagePage > 0x8B) && (UsagePage < 0x8F)) ||
      ((UsagePage > 0x8F) && (UsagePage < 0x92))) {
    return TRUE;
  }
  return FALSE;
}

/**

  Parse Main Item

  @param  ReportFieldInfo       REPORT_FIELD_INFO that stores all the report group data
  @param  MainItem              HID_ITEM to parse

**/
STATIC
VOID
ParseMainItem (
  IN  REPORT_FIELD_INFO         *ReportFieldInfo,
  IN  HID_ITEM                  *MainItem
  )
{
  UINT16                Usage;

  switch (MainItem->Tag) {
  case HID_MAIN_ITEM_TAG_INPUT:
  case HID_MAIN_ITEM_TAG_OUTPUT:
  case HID_MAIN_ITEM_TAG_FEATURE:
    if (mHidParser.DataValid) {
      UpdateReportGroupData (ReportFieldInfo, MainItem);
    }
    break;
    
  case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
    Usage = (mHidParser.Local.UsageIndex > 0) ? mHidParser.Local.Usage[mHidParser.Local.UsageIndex - 1] : 0;
    mHidParser.CollectionStack[mHidParser.CollectionStackPtr].Type  = (UINT16)GetItemData (MainItem);
    mHidParser.CollectionStack[mHidParser.CollectionStackPtr].Usage = Usage;
    if (!mHidParser.DataValid && mHidParser.CollectionStackPtr == 0) {
      mHidParser.TopLevelCollectionUsagePage = mHidParser.Global.UsagePage;
      //
      // Parse all collections whose usage pages are not undefined, reserved, or vendor-defined.
      //
      mHidParser.DataValid = IsUsagePageValid (mHidParser.Global.UsagePage);
    }
    mHidParser.CollectionStackPtr ++;
    break;

  case HID_MAIN_ITEM_TAG_END_COLLECTION:
    mHidParser.CollectionStackPtr --;
    if (mHidParser.DataValid && mHidParser.CollectionStackPtr == 0) {
      mHidParser.DataValid = FALSE;
    }
    break;
  }
  //
  // Reset local parameters
  //
  mHidParser.Local.UsageIndex = 0;
  mHidParser.Local.UsageMin   = 0;
  mHidParser.Local.UsageMax   = 0;
}

/**

  Parse Hid Item

  @param  ReportFieldInfo       REPORT_FIELD_INFO that stores all the report group data
  @param  HidItem               HidItem to parse

**/
STATIC
VOID
ParseHidItem (
  IN  REPORT_FIELD_INFO         *ReportFieldInfo,
  IN  HID_ITEM                  *HidItem
  )
{
  switch (HidItem->Type) {

  case HID_ITEM_TYPE_MAIN:
    //
    // For Main Item, parse main item
    //
    ParseMainItem (ReportFieldInfo, HidItem);
    break;

  case HID_ITEM_TYPE_GLOBAL:
    //
    // For global Item, parse global item
    //
    ParseGlobalItem (HidItem);
    break;

  case HID_ITEM_TYPE_LOCAL:
    //
    // For Local Item, parse local item
    //
    ParseLocalItem (HidItem);
    break;
  }
}

/**

  Get Report Field Value according the report field description

  @param  ReportData            Data payload
  @param  ReportDataSize        Data size
  @param  Field                 The format of data

  @retval report field value

**/
UINTN
GetReportFieldValue (
  IN  UINT8             *ReportData,
  IN  UINTN             ReportDataSize,
  IN  REPORT_FIELD      *Field
  )
{
  UINT8   BitOffsetInAByte;
  UINTN   *StartAddress;
  UINTN   MaskToKeepBits;
  UINTN   DataN;

  if ((ReportData == NULL) ||
      (Field == NULL) ||
      (!(Field->DataValid)) ||
      (((UINTN) Field->BitOffset + Field->BitLength)) > (ReportDataSize * 8)) {
    return 0;
  }
  BitOffsetInAByte = (Field->BitOffset % 8);
  if ((Field->BitLength + BitOffsetInAByte) > (sizeof (UINTN) * 8)) {
    return 0;
  }
  StartAddress = (UINTN *) (ReportData + (Field->BitOffset / 8));
  DataN = *StartAddress;
  DataN >>= BitOffsetInAByte;
  MaskToKeepBits = (1 << Field->BitLength) - 1;
  return DataN &= MaskToKeepBits;
}

/**

  Set Report Field Value according the report field description

  @param  ReportData            Data payload
  @param  ReportDataSize        Data size
  @param  Field                 The format of data

  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER Invalid parameter

**/
EFI_STATUS
SetReportFieldValue (
  OUT UINT8             *ReportData,
  IN  UINTN             ReportDataSize,
  IN  REPORT_FIELD      *Field,
  IN  UINTN             NewFieldValue
  )
{
  UINT8   BitOffsetInAByte;
  UINTN   MaskToKeepBits;
  UINTN   *StartAddress;

  if ((ReportData == NULL) ||
      (Field == NULL) ||
      (!(Field->DataValid)) ||
      (((UINTN) Field->BitOffset + Field->BitLength)) > (ReportDataSize * 8)) {
    return EFI_INVALID_PARAMETER;
  }
  BitOffsetInAByte = (Field->BitOffset % 8);
  if ((Field->BitLength + BitOffsetInAByte) > (sizeof (UINTN) * 8)) {
    return EFI_BUFFER_TOO_SMALL;
  }
  MaskToKeepBits = (1 << Field->BitLength) - 1;
  MaskToKeepBits <<= BitOffsetInAByte;
  NewFieldValue <<= BitOffsetInAByte;
  NewFieldValue &= MaskToKeepBits;
  StartAddress = (UINTN *) (ReportData + (Field->BitOffset / 8));
  *StartAddress &= (~MaskToKeepBits);
  *StartAddress |= NewFieldValue;
  return EFI_SUCCESS;
}

/**

  Parse Report Descriptor

  @param  ReportDescriptor      Report descriptor to parse
  @param  ReportSize            Report descriptor size
  @param  ReportFieldInfo       REPORT_FIELD_INFO that stores all the report group data

  @retval EFI_DEVICE_ERROR      Report descriptor error
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
ParseReportDescriptor (
  IN     UINT8                  *ReportDescriptor,
  IN     UINTN                  ReportSize,
  IN OUT REPORT_FIELD_INFO      *ReportFieldInfo
  )
{
  UINT8             *DescriptorEnd;
  UINT8             *ptr;
  HID_ITEM          HidItem;
  REPORT_GROUP      *ReportGroup;
  UINTN             Count;
  UINTN             MaximumSize;

  ZeroMem (&mHidParser, sizeof (HID_PARSER));
  ReportFieldInfo->Total = 0;
  DescriptorEnd = ReportDescriptor + ReportSize;
  ptr           = GetNextItem (ReportDescriptor, DescriptorEnd, &HidItem);
  while (ptr != NULL) {
    if (HidItem.Format != HID_ITEM_FORMAT_SHORT) {
      //
      // Long Format Item is not supported at current HID revision
      //
      return EFI_DEVICE_ERROR;
    }
    ParseHidItem (ReportFieldInfo, &HidItem);
    ptr = GetNextItem (ptr, DescriptorEnd, &HidItem);
  }
  //
  // Find out the report group in biggest reported data size
  //
  Count       = ReportFieldInfo->Total;
  ReportGroup = ReportFieldInfo->ReportGroup;
  MaximumSize = 0;
  while (Count > 0) {
    if (ReportGroup->DataValid) {
      //
      // Convert the report data size in bits to size in bytes.
      //
      ReportGroup->DataSize = (ReportGroup->DataSize + 7) / 8;
      if (ReportGroup->DataType == HID_MAIN_ITEM_TAG_INPUT && ReportGroup->DataSize > MaximumSize) {
        MaximumSize = ReportGroup->DataSize;
        ReportFieldInfo->BiggestInputReportGroup = ReportGroup;
      } else if (ReportGroup->DataType == HID_MAIN_ITEM_TAG_FEATURE && ReportGroup->DataSize > ReportFieldInfo->MaximumFeatureLength) {
        ReportFieldInfo->MaximumFeatureLength = ReportGroup->DataSize;
      }
    }
    ReportGroup++;
    Count--;
  }
  return EFI_SUCCESS;
}

/**

  Finding the class code to match the desire type

  @param  ReportDescriptor              Report descriptor to parse
  @param  ReportSize                    Report descriptor size
  @param  ClassCode                     The target usage page and usage code

  @retval TRUE                          Matched
  @retval FALSE                         Not found

**/
BOOLEAN
MatchHidDeviceType (
  IN  UINT8                     *ReportDescriptor,
  IN  UINTN                     ReportSize,
  IN  UINT16                    ClassCode
  )
{
  UINT8     *DescriptorEnd;
  UINT8     *ptr;
  HID_ITEM  HidItem;
  UINT8     CurrentUsagePage = 0;
  UINT8     CurrentUsage = 0;
  UINT8     TargetUsagePage = (UINT8)(ClassCode >> 8);
  UINT8     TargetUsage = (UINT8)(ClassCode & 0xff);
  
  DescriptorEnd = ReportDescriptor + ReportSize;

  ptr           = GetNextItem (ReportDescriptor, DescriptorEnd, &HidItem);

  while (ptr != NULL) {
    if (HidItem.Format != HID_ITEM_FORMAT_SHORT) {
      //
      // Long Format Item is not supported at current HID revision
      //
      return FALSE;
    }
    switch (HidItem.Type) {

    case HID_ITEM_TYPE_GLOBAL:
      //
      // For global Item, parse global item
      //
      if (HidItem.Tag == HID_GLOBAL_ITEM_TAG_USAGE_PAGE) {
        CurrentUsagePage = (UINT8) GetItemData (&HidItem);
      }
      break;

    case HID_ITEM_TYPE_LOCAL:
      //
      // For Local Item, parse local item
      //
      if (HidItem.Tag == HID_LOCAL_ITEM_TAG_USAGE) {
        CurrentUsage = (UINT8) GetItemData (&HidItem);
      }
      //
      // Check Touch device by Usage Page is DIGITIZERS and Usage is Touch Screen,
      // or Usage Page is DIGITIZERS and Usage is PEN
      //
      if (CurrentUsagePage == TargetUsagePage && CurrentUsage == TargetUsage) {
        return TRUE;
      }
      break;

    }
    ptr = GetNextItem (ptr, DescriptorEnd, &HidItem);
  }
  return FALSE;
}
