/** @file
 Structure and function definition for Layout Library

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

#ifndef _LAYOUT_LIB_H_
#define _LAYOUT_LIB_H_

#include <Uefi.h>
#include <H2OHii.h>
#include <Protocol/H2OFormBrowser.h>
#include <Protocol/H2ODisplayEngine.h>
#include <Protocol/HiiFont.h>

#include <Library/RectLib.h>


#define H2O_COLOR_NAME_MAX_LENGTH              20
typedef struct _RGB_ATTR_STR {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                Rgb;
  UINT32                                       Attribute;
  CHAR8                                        String[H2O_COLOR_NAME_MAX_LENGTH];
} RGB_ATTR_STR;

typedef struct _H2O_COLOR_INFO {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                Rgb;
  UINT32                                       Attribute;
} H2O_COLOR_INFO;

typedef struct _H2O_STRING_INFO {
  RECT                                         StringField;
  CHAR16                                       *String;
} H2O_STRING_INFO;

typedef struct _H2O_IMAGE_INFO {
  RECT                                         ImageField;

  EFI_IMAGE_ID                                 ImageId;
  EFI_ANIMATION_ID                             AnimationId;
  EFI_EVENT                                    AnimationRefreshEvent;

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                *CurrentBlt;
  UINT32                                       CurrentBltWidth;
  UINT32                                       CurrentBltHeight;
} H2O_IMAGE_INFO;

typedef struct _H2O_CSS_LENGTH {
  UINT8                                        Type;
  union {
    //
    // Relative
    //
    UINT32                                     Px; ///< Pixels
    UINT32                                     Em; ///< Relative to font size
    UINT32                                     Ex; ///< Relative to Lowercase x height
    UINT32                                     Percentage;
    BOOLEAN                                    Auto;
    //
    // Absolute
    //
    UINT32                                     In; ///< Inch
    UINT32                                     Cm; ///< Centimeter
    UINT32                                     Mm; ///< Millimeter
    UINT32                                     Pt; ///< points, 1 pt = 1/72 in
    UINT32                                     Pc; ///< picas, 1 pc = 12 pt
  } Value;
} H2O_CSS_LENGTH;

typedef struct {
  UINT32                                       Type;
  union {
    BOOLEAN                                    Bool;
    UINT8                                      U8;
    UINT16                                     U16;
    UINT32                                     U32;
    UINT64                                     U64;
    H2O_CSS_LENGTH                             Length;
    H2O_COLOR_INFO                             Color;
    H2O_IMAGE_INFO                             Image;
    RECT                                       Rect;
  } Value;
} H2O_PROPERTY_VALUE;

typedef enum {
  H2O_PROPERTY_VALUE_TYPE_BOOLEAN = 1,
  H2O_PROPERTY_VALUE_TYPE_UINT8,
  H2O_PROPERTY_VALUE_TYPE_UINT16,
  H2O_PROPERTY_VALUE_TYPE_UINT32,
  H2O_PROPERTY_VALUE_TYPE_UINT64,
  H2O_PROPERTY_VALUE_TYPE_STR,
  H2O_PROPERTY_VALUE_TYPE_RECT,
  H2O_PROPERTY_VALUE_TYPE_CSS_LENGTH,
  H2O_PROPERTY_VALUE_TYPE_COLOR,
  H2O_PROPERTY_VALUE_TYPE_IMAGE,
  H2O_PROPERTY_VALUE_TYPE_MAX
} H2O_PROPERTY_VALUE_TYPE;

typedef struct _H2O_PANEL_INFO H2O_PANEL_INFO;
typedef struct _H2O_LAYOUT_INFO H2O_LAYOUT_INFO;

#define H2O_PROPERTY_INFO_NODE_SIGNATURE SIGNATURE_32 ('P', 'R', 'O', 'P')
typedef struct _H2O_PROPERTY_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  CHAR8                                        *Identifier;
  CHAR8                                        *Value;
} H2O_PROPERTY_INFO;
#define H2O_PROPERTY_INFO_NODE_FROM_LINK(a) CR (a, H2O_PROPERTY_INFO, Link, H2O_PROPERTY_INFO_NODE_SIGNATURE)

#define H2O_STYLE_INFO_NODE_SIGNATURE SIGNATURE_32 ('S', 'T', 'Y', 'L')
typedef struct _H2O_STYLE_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;

  UINT32                                       StyleType;          ///< From styletypekeyword. See H2O_IFR_STYLE_TYPE_xxx.
  CHAR8                                        *ClassName;
  UINT32                                       PseudoClass;        ///< See H2O_STYLE_PSEUDO_CLASS_xxxx.

  LIST_ENTRY                                   PropertyListHead;
} H2O_STYLE_INFO;
#define H2O_STYLE_INFO_NODE_FROM_LINK(a) CR (a, H2O_STYLE_INFO, Link, H2O_STYLE_INFO_NODE_SIGNATURE)

typedef enum {
  H2O_STYLE_PSEUDO_CLASS_NORMAL = 0,
  H2O_STYLE_PSEUDO_CLASS_SELECTABLE,
  H2O_STYLE_PSEUDO_CLASS_GRAYOUT,
  H2O_STYLE_PSEUDO_CLASS_HIGHLIGHT,
  H2O_STYLE_PSEUDO_CLASS_LINK,
  H2O_STYLE_PSEUDO_CLASS_VISITED,
  H2O_STYLE_PSEUDO_CLASS_HOVER,
  H2O_STYLE_PSEUDO_CLASS_FOCUS,
  H2O_STYLE_PSEUDO_CLASS_ACTIVE,
  H2O_STYLE_PSEUDO_CLASS_ENABLED,
  H2O_STYLE_PSEUDO_CLASS_DISABLED,
  H2O_STYLE_PSEUDO_CLASS_NOT,
  H2O_STYLE_PSEUDO_CLASS_HELP,
  H2O_STYLE_PSEUDO_CLASS_MAX
} H2O_STYLE_PSEUDO_CLASS;

typedef enum {
  H2O_CONTROL_ID_DIALOG_TITLE = 1,
  H2O_CONTROL_ID_DIALOG_BODY,
  H2O_CONTROL_ID_DIALOG_BODY_INPUT,
  H2O_CONTROL_ID_DIALOG_BUTTON,
  H2O_CONTROL_ID_MAX
} H2O_CONTROL_ID;

typedef struct _H2O_CONTROL_INFO {
  UINT32                                       ControlId;
  H2O_STYLE_INFO                               ControlStyle;

  RECT                                         ControlField;
  RECT                                         ControlRelativeField; ///< from panel

  H2O_STRING_INFO                              Text;
  H2O_STRING_INFO                              HelpText;
  H2O_IMAGE_INFO                               BackgroundImage;
  EFI_IMAGE_INPUT                              HelpImage;
  EFI_HII_VALUE                                HiiValue;

  H2O_PANEL_INFO                               *ParentPanel;
  BOOLEAN                                      Editable;
  //
  // Statement
  //
  H2O_PAGE_ID                                  PageId;
  H2O_STATEMENT_ID                             StatementId;
  EFI_QUESTION_ID                              QuestionId;
  UINT8                                        Operand;
  EFI_IFR_OP_HEADER                            *IfrOpCode;
  BOOLEAN                                      Visible;
  BOOLEAN                                      Selectable;
  BOOLEAN                                      Modifiable;

  UINT64                                       Minimum;
  UINT64                                       Maximum;
  UINT64                                       Step;
} H2O_CONTROL_INFO;

typedef struct _H2O_CONTROL_LIST {
  UINT32                                       Count;
  H2O_CONTROL_INFO                             *ControlArray;
} H2O_CONTROL_LIST;

#define H2O_PANEL_INFO_NODE_SIGNATURE SIGNATURE_32 ('P', 'A', 'N', 'E')
struct _H2O_PANEL_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       PanelId;
  UINT32                                       PanelType; ///< See H2O_PANEL_TYPE_xxx.
  EFI_GUID                                     PanelGuid;

  RECT                                         PanelField;
  RECT                                         PanelRelField; ///< for page-up and page-down

  H2O_IMAGE_INFO                               ContentsImage;
  H2O_IMAGE_INFO                               BackgroundImage;

  UINT32                                       OrderFlag; ///< See H2O_PANEL_ORDER_FLAG_xxxx.
  BOOLEAN                                      WithScrollBarVertical;
  BOOLEAN                                      WithScrollBarHorizontal;

  H2O_LAYOUT_INFO                              *ParentLayout;
  LIST_ENTRY                                   StyleListHead;
  H2O_CONTROL_LIST                             ControlList;
} ;
#define H2O_PANEL_INFO_NODE_FROM_LINK(a) CR (a, H2O_PANEL_INFO, Link, H2O_PANEL_INFO_NODE_SIGNATURE)

#define H2O_PANEL_TYPE_SCREEN                  0x00000000 ///< default
#define H2O_PANEL_TYPE_HOTKEY                  0x00000001
#define H2O_PANEL_TYPE_HELP_TEXT               0x00000002
#define H2O_PANEL_TYPE_SETUP_MENU              0x00000004
#define H2O_PANEL_TYPE_SETUP_PAGE              0x00000008
#define H2O_PANEL_TYPE_FORM_TITLE              0x00000010
#define H2O_PANEL_TYPE_QUESTION                0x00000020
#define H2O_PANEL_TYPE_OWNER_DRAW              0x00000040
#define H2O_PANEL_TYPE_SETUP_PAGE2             0x00000080

#define H2O_PANEL_ORDER_FLAG_HORIZONTAL_FIRST  0x00000001 ///< Default Vertical First Order
#define H2O_PANEL_ORDER_FLAG_NO_SCROLL_BAR     0x00000002
#define H2O_PANEL_ORDER_FLAG_JUSTIFIED         0x00000004
#define H2O_PANEL_ORDER_FLAG_MINIMUM           0x00000008

#define RESOLUTION_VERTICAL                    0x00000000 ///< Default
#define RESOLUTION_HORIZONTAL                  0x00000001

#define DISPLAY_NONE                           0x00000000 ///< Default
#define DISPLAY_LEFT                           0x00000001
#define DISPLAY_TOP                            0x00000002
#define DISPLAY_RIGHT                          0x00000004
#define DISPLAY_BOTTOM                         0x00000008

typedef struct _H2O_PANEL_LIST {
  UINT32                                       Count;
  H2O_PANEL_INFO                               *PanelArray;
} H2O_PANEL_LIST;

#define H2O_LAYOUT_INFO_NODE_SIGNATURE SIGNATURE_32 ('L', 'A', 'Y', 'O')
struct _H2O_LAYOUT_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       LayoutId;
  LIST_ENTRY                                   PanelListHead;
  LIST_ENTRY                                   StyleListHead;
};
#define H2O_LAYOUT_INFO_NODE_FROM_LINK(a) CR (a, H2O_LAYOUT_INFO, Link, H2O_LAYOUT_INFO_NODE_SIGNATURE)

EFI_STATUS
BltCopy(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Blt,
  IN     UINTN                           BltStartX,
  IN     UINTN                           BltStartY,
  IN     UINTN                           BltWidth,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *DestBlt,
  IN     UINTN                           DestStartX,
  IN     UINTN                           DestStartY,
  IN     UINTN                           DestWidth,
  IN     UINTN                           DestHeight,
  IN     UINTN                           CopyWidth,
  IN     UINTN                           CopyHeight
  );

EFI_STATUS
BltCombine(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BgBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *FgBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  );

EFI_STATUS
BltTransparent(
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *SrcBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  );

EFI_STATUS
CreateImageByColor (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb,
  IN     RECT                                *Field,
  IN OUT EFI_IMAGE_INPUT                     *ImageInput
  );

EFI_STATUS
GetLayoutById (
  IN     UINT32                                LayoutId,
  OUT    H2O_LAYOUT_INFO                       **Layout
  );

EFI_STATUS
GetImageById (
  IN     UINT32                                ImageId,
  OUT    EFI_IMAGE_INPUT                       *ImageInput
  );

EFI_STATUS
SeparateStringByFont (
  IN     CHAR16                                *String,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  IN     UINT32                                LimitWidth,
  OUT    EFI_IMAGE_OUTPUT                      **CharImageOutArray OPTIONAL,
  OUT    UINT32                                *SeparateStrCount,
  OUT    UINT32                                **SeparateStrOffsetArray OPTIONAL,
  OUT    UINT32                                **SeparateStrWidthArray OPTIONAL,
  OUT    UINTN                                 **SeparateStrArray
  );

EFI_STATUS
GetStringImageByFont (
  IN     CHAR16                                *String,
  IN     RECT                                  *Field,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  OUT    EFI_IMAGE_OUTPUT                      **ImageOutput
  );

BOOLEAN
IsVisibility (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

EFI_STATUS
GetPanelField (
  IN  H2O_PANEL_INFO                           *Panel,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  RECT                                     *RelativeField,
  OUT RECT                                     *PanelField
  );

EFI_STATUS
GetPaddingRect(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    RECT                                  *Rect
  );

EFI_STATUS
GetForegroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  );

EFI_STATUS
GetBackgroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  );

EFI_STATUS
GetPanelColorAttribute (
  IN  H2O_PANEL_INFO                           *Panel,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  OUT UINT32                                   *PanelColorAttribute
  );

EFI_STATUS
GetBackgroundImage (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_IMAGE_INFO                        *Image
  );

UINT32
GetBorderWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

UINT32
GetFontSize (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

CHAR16 *
GetFontName (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

UINT32
GetResolution (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

UINT32
GetHelpImageDisplay (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

BOOLEAN
IsHelpTextShow (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

UINT32
GetHelpTextFontSize (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

EFI_STATUS
GetScrollbarColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  );

UINT32
GetScrollbarWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

H2O_PANEL_INFO *
GetPannelInfo (
  IN H2O_LAYOUT_INFO                           *LayoutInfo,
  IN UINT32                                    PanelType
  );

#endif

