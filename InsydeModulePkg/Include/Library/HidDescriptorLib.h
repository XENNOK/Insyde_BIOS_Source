/** @file

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

#ifndef _HID_DESCRIPTOR_LIB_H_
#define _HID_DESCRIPTOR_LIB_H_

#include <Uefi.h>

#define NULL_COMMAND                            0xffffffff
#define RESET                                   0x0100
#define GET_REPORT_FEATURE                      0x0230
#define SET_REPORT_FEATURE                      0x0330
#define SET_IDLE                                0x0500
#define SET_POWER                               0x0800
#define POWER_ON                                0x0000
#define POWER_SLEEP                             0x0001

#define FIELD_BIT0                              0x1
#define FIELD_BIT1                              0x2
#define FIELD_BIT2                              0x4
#define FIELD_BIT3                              0x8
#define FIELD_BIT4                              0x10
#define FIELD_BIT5                              0x20
#define FIELD_BIT6                              0x40
#define FIELD_BIT7                              0x80

//
// HID report item format
//
#define HID_ITEM_FORMAT_SHORT                   0
#define HID_ITEM_FORMAT_LONG                    1

//
// Special tag indicating long items
//
#define HID_ITEM_TAG_LONG                       15

//
// HID report descriptor item type (prefix bit 2,3)
//
#define HID_ITEM_TYPE_MAIN                      0
#define HID_ITEM_TYPE_GLOBAL                    1
#define HID_ITEM_TYPE_LOCAL                     2
#define HID_ITEM_TYPE_RESERVED                  3

//
// HID report descriptor main item tags
//
#define HID_MAIN_ITEM_TAG_INPUT                 8
#define HID_MAIN_ITEM_TAG_OUTPUT                9
#define HID_MAIN_ITEM_TAG_FEATURE               11
#define HID_MAIN_ITEM_TAG_BEGIN_COLLECTION      10
#define HID_MAIN_ITEM_TAG_END_COLLECTION        12

//
// HID report descriptor main item contents
//
#define HID_MAIN_ITEM_CONSTANT                  0x001
#define HID_MAIN_ITEM_VARIABLE                  0x002
#define HID_MAIN_ITEM_RELATIVE                  0x004
#define HID_MAIN_ITEM_WRAP                      0x008
#define HID_MAIN_ITEM_NONLINEAR                 0x010
#define HID_MAIN_ITEM_NO_PREFERRED              0x020
#define HID_MAIN_ITEM_NULL_STATE                0x040
#define HID_MAIN_ITEM_VOLATILE                  0x080
#define HID_MAIN_ITEM_BUFFERED_BYTE             0x100

//
// HID report descriptor collection item types
//
#define HID_COLLECTION_PHYSICAL                 0
#define HID_COLLECTION_APPLICATION              1
#define HID_COLLECTION_LOGICAL                  2

//
// HID report descriptor global item tags
//
#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE          0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM     1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM     2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM    3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM    4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT       5
#define HID_GLOBAL_ITEM_TAG_UNIT                6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE         7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID           8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT        9
#define HID_GLOBAL_ITEM_TAG_PUSH                10
#define HID_GLOBAL_ITEM_TAG_POP                 11

//
// HID report descriptor local item tags
//
#define HID_LOCAL_ITEM_TAG_USAGE                0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM        1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM        2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX     3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM   4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM   5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX         7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM       8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM       9
#define HID_LOCAL_ITEM_TAG_DELIMITER            10

//
// HID usage tables
//
#define HID_USAGE_PAGE                          0xffff0000

#define HID_UP_GENDESK                          0x01
#define HID_UP_KEYBOARD                         0x07
#define HID_UP_LED                              0x08
#define HID_UP_BUTTON                           0x09
#define HID_UP_CONSUMER                         0x0C
#define HID_UP_DIGITIZERS                       0x0D
#define HID_UP_PID                              0x0f

#define HID_USAGE                               0x0000ffff

#define HID_GD_POINTER                          0x01
#define HID_GD_MOUSE                            0x02
#define HID_GD_JOYSTICK                         0x04
#define HID_GD_GAMEPAD                          0x05
#define HID_GD_KEYBOARD                         0x06
#define HID_GD_HATSWITCH                        0x39
#define HID_GD_X                                0x30
#define HID_GD_Y                                0x31
#define HID_GD_WHEEL                            0x38

#define HID_DIGITIZERS_DIGITIZER                0x01
#define HID_DIGITIZERS_PEN                      0x02
#define HID_DIGITIZERS_LIGHT_PEN                0x03
#define HID_DIGITIZERS_TOUCH_SCREEN             0x04
#define HID_DIGITIZERS_TOUCH_PAD                0x05
#define HID_DIGITIZERS_DEVICE_CONFIGURATION     0x0E
#define HID_DIGITIZERS_TIP_SWITCH               0x42
#define HID_DIGITIZERS_SECONDARY_TIP_SWITCH     0x43
#define HID_DIGITIZERS_BARREL_SWITCH            0x44
#define HID_DIGITIZERS_DEVICE_MODE              0x52
#define HID_DIGITIZERS_DEVICE_ID                0x53
#define HID_DIGITIZERS_CONTACT_COUNT            0x54
#define HID_DIGITIZERS_CONTACT_COUNT_MAXIMUM    0x55
#define HID_DIGITIZERS_SCAN_TIME                0x56

#define HID_DIGITIZERS_DEVICE_MODE_MOUSE        0x00
#define HID_DIGITIZERS_DEVICE_MODE_SINGLE_INPUT 0x01
#define HID_DIGITIZERS_DEVICE_MODE_MULTI_INPUT  0x02

//
// HID Item general structure
//
typedef struct _hid_item {
  UINT16                        Format;
  UINT8                         Size;
  UINT8                         Type;
  UINT8                         Tag;
  union {
    UINT8                         U8;
    UINT16                        U16;
    UINT32                        U32;
    INT8                          I8;
    INT16                         I16;
    INT32                         I32;
    UINT8                         *LongData;
  } Data;
} HID_ITEM;

typedef struct {
  UINT16                        UsagePage;
  INT32                         LogicMin;
  INT32                         LogicMax;
  INT32                         PhysicalMin;
  INT32                         PhysicalMax;
  UINT16                        UnitExp;
  UINT16                        Unit;
  UINT8                         ReportId;
  UINT8                         Reserved0;
  UINT16                        ReportSize;
  UINT16                        ReportCount;
} HID_GLOBAL;

#define INVALID_USAGE_POSITION  ((UINT8) -1)

typedef struct {
  UINT16                        Usage[16];  /* usage array */
  UINT16                        UsageIndex;
  UINT16                        UsageMin;
  UINT16                        UsageMax;
} HID_LOCAL;

typedef struct {
  UINT16                        Type;
  UINT16                        Usage;
} HID_COLLECTION;

typedef struct {
  HID_GLOBAL                    Global;
  HID_GLOBAL                    GlobalStack[8];
  UINT32                        GlobalStackPtr;
  HID_LOCAL                     Local;
  HID_COLLECTION                CollectionStack[8];
  UINT32                        CollectionStackPtr;
  UINT16                        TopLevelCollectionUsagePage;
  BOOLEAN                       DataValid;
} HID_PARSER;

typedef struct {
  UINT8                         DataValid;
  UINT8                         BitLength;
  UINT16                        BitOffset;
  UINT32                        Min;
  UINT32                        Max;
} REPORT_FIELD;

typedef struct {
  REPORT_FIELD                  FieldModKeyCode;
  REPORT_FIELD                  FieldKeyCode;
} KEYBOARD_INPUT_DATA;

typedef struct {
  REPORT_FIELD                  FieldLed;
} KEYBOARD_LED_DATA;

typedef struct {
  REPORT_FIELD                  FieldX;
  REPORT_FIELD                  FieldY;
  REPORT_FIELD                  FieldZ;
  REPORT_FIELD                  FieldButton1;
  REPORT_FIELD                  FieldButton2;
} MOUSE_INPUT_DATA;

typedef struct {
  REPORT_FIELD                  FieldX;
  REPORT_FIELD                  FieldY;
  REPORT_FIELD                  FieldTouchActive;
  REPORT_FIELD                  FieldAltActive;
  REPORT_FIELD                  FieldContactCount;
} TOUCH_PANEL_INPUT_DATA;

typedef struct {
  REPORT_FIELD                  FieldDeviceMode;
  REPORT_FIELD                  FieldDeviceId;
} TOUCH_PANEL_MODE_DATA;

#define ATTR_KEYBOARD_INPUT     0
#define ATTR_KEYBOARD_LED       1
#define ATTR_MOUSE_INPUT        2
#define ATTR_TOUCH_PANEL_INPUT  3
#define ATTR_TOUCH_PANEL_MODE   4

typedef struct {
  UINT8                         DataValid;
  UINT8                         Id;
  UINT16                        DataClass;
  UINT8                         DataType;
  UINT8                         DataAttr;
  UINT16                        DataSize;
  union {
    KEYBOARD_INPUT_DATA         Keyboard;
    KEYBOARD_LED_DATA           KeyboardLed;
    MOUSE_INPUT_DATA            Mouse;
    TOUCH_PANEL_INPUT_DATA      TouchPanel;
    TOUCH_PANEL_MODE_DATA       TouchPanelMode;
  }                             Data;
} REPORT_GROUP;

typedef struct {
  UINTN                         Total;
  UINTN                         FirstFeatureID;
  UINTN                         MaximumFeatureLength;
  REPORT_GROUP                  ReportGroup[16];
  REPORT_GROUP                  *BiggestInputReportGroup;
  REPORT_GROUP                  *DeviceConfigurationReportGroup;
} REPORT_FIELD_INFO;

#pragma pack(1)
typedef struct {
  UINT16                        HIDDescLength;          // The length, in unsigned bytes, of the complete Hid Descriptor
  UINT16                        Version;                // The version number, in binary coded decimal (BCD) format. Devices should default to 0x0100
  UINT16                        ReportDescLength; 	// The length, in unsigned bytes, of the Report Descriptor. 
  UINT16                        ReportDescRegister;     // The register index containing the Report Descriptor on the DEVICE. 
  UINT16                        InputRegister;          // This field identifies, in unsigned bytes, the register number to read the input report from the DEVICE. 
  UINT16                        MaxInputLength;         // This field identifies in unsigned bytes the length of the largest Input Report to be read from the Input Register (Complex HID Devices will need various sized reports). 
  UINT16                        OutputRegister;         // This field identifies, in unsigned bytes, the register number to send the output report to the DEVICE. 
  UINT16                        MaxOutputLength;        // This field identifies in unsigned bytes the length of the largest output Report to be sent to the Output Register (Complex HID Devices will need various sized reports). 
  UINT16                        CommandRegister;        // This field identifies, in unsigned bytes, the register number to send command requests to the DEVICE 
  UINT16                        DataRegister;           // This field identifies in unsigned bytes the register number to exchange data with the Command Request 
  UINT16                        VendorID;               // This field identifies the DEVICE manufacturers Vendor ID. 
  UINT16                        ProductID;              // This field identifies the DEVICE¡¦s unique model / Product ID. 
  UINT16                        VersionID;              // This field identifies the DEVICE¡¦s firmware revision number. 
  UINT32                        Reserved;
} HID_DESCRIPTOR;
#pragma pack()

/**

  Get Report Field Value according the report field description

  @param  ReportData            Data payload
  @param  ReportDataSize        Data size
  @param  Field                 The format of data

  @retval report field value

**/
UINTN
GetReportFieldValue (
  IN  UINT8                     *ReportData,
  IN  UINTN                     ReportDataSize,
  IN  REPORT_FIELD              *Field
  );

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
  OUT UINT8                     *ReportData,
  IN  UINTN                     ReportDataSize,
  IN  REPORT_FIELD              *Field,
  IN  UINTN                     NewFieldValue
  );

/**

  Parse Touch Panel Report Descriptor

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
  );

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
  );

#endif
