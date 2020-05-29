//;******************************************************************************
//;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************


#ifndef _INSYDEMODULE_HII_H
#define _INSYDEMODULE_HII_H

///
/// GUIDed opcodes defined for H2O implementation.
///
#define H2O_IFR_EXT_GUID \
  { \
    0x38237648, 0x09cc, 0x47c4, {0x8b, 0x5f, 0xb0, 0x9f, 0x06, 0x89, 0x0d, 0xf7} \
  }
extern EFI_GUID gH2OIfrExtGuid;

#pragma pack(1)

///
/// H2O implementation extension opcodes, new extension can be added here later.
///
#define H2O_IFR_EXT_HOTKEY            0x0000
#define H2O_IFR_EXT_LAYOUT            0x0001
#define H2O_IFR_EXT_STYLE             0x0002
#define H2O_IFR_EXT_HELPIMAGE         0x0003
#define H2O_IFR_EXT_PROPERTY          0x0004
#define H2O_IFR_EXT_STYLE_REF         0x0005
#define H2O_IFR_EXT_TEXT              0x0006

///
/// HotKey opcode.
///
typedef struct _EFI_IFR_GUID_HOTKEY {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_HOTKEY
  ///
  UINT16              Function;
  EFI_STRING_ID       HotKey;
} EFI_IFR_GUID_HOTKEY;

///
/// Layout opcode.
///
typedef struct _H2O_IFR_GUID_LAYOUT {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_LAYOUT
  ///
  UINT16              Function;
  UINT16              LayoutId;
} H2O_IFR_GUID_LAYOUT;


#define H2O_IFR_STYLE_TYPE_PANEL       0x00
#define H2O_IFR_STYLE_TYPE_LAYOUT      0x01
#define H2O_IFR_STYLE_TYPE_FORM        0x02
#define H2O_IFR_STYLE_TYPE_FORMSET     0x03
#define H2O_IFR_STYLE_TYPE_ACTION      0x04
#define H2O_IFR_STYLE_TYPE_CHECKBOX    0x05
#define H2O_IFR_STYLE_TYPE_DATE        0x06
#define H2O_IFR_STYLE_TYPE_GOTO        0x07
#define H2O_IFR_STYLE_TYPE_ONEOF       0x08
#define H2O_IFR_STYLE_TYPE_NUMERIC     0x09
#define H2O_IFR_STYLE_TYPE_ORDEREDLIST 0x0a
#define H2O_IFR_STYLE_TYPE_PASSWORD    0x0b
#define H2O_IFR_STYLE_TYPE_STRING      0x0c
#define H2O_IFR_STYLE_TYPE_TIME        0x0d
#define H2O_IFR_STYLE_TYPE_TEXT        0x0e
#define H2O_IFR_STYLE_TYPE_SUBTITLE    0x0f
#define H2O_IFR_STYLE_TYPE_RESETBUTTON 0x10
#define H2O_IFR_STYLE_TYPE_SHEET       0xff

///
/// Style opcode.
///
typedef struct _H2O_IFR_GUID_STYLE {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_STYLE
  ///
  UINT16              Function;
  UINT8               Type;
  CHAR8               Name[1];
} H2O_IFR_GUID_STYLE;


///
/// HelpImage opcode.
///
typedef struct _H2O_IFR_GUID_HELPIMAGE {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_HELPIMAGE
  ///
  UINT16              Function;
  EFI_IMAGE_ID        HelpImage;
} H2O_IFR_GUID_HELPIMAGE;

///
/// Property opcode.
///
typedef struct _H2O_IFR_GUID_PROPERTY {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_PROPERTY
  ///
  UINT16              Function;
  CHAR8               Property[1];
} H2O_IFR_GUID_PROPERTY;

///
/// Style Ref opcode.
///
typedef struct _H2O_IFR_GUID_STYLE_REF {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_STYLE_REF
  ///
  UINT16              Function;
  UINT8               Type;
  CHAR8               Name[1];
} H2O_IFR_GUID_STYLE_REF;

///
/// Extended Text opcode.
///
typedef struct _H2O_IFR_GUID_TEXT {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_TEXT
  ///
  UINT16              Function;
  EFI_STRING_ID       Text;
} H2O_IFR_GUID_TEXT;



#pragma pack()

#endif

