/** @file
  H2O Form Browser Protocol Header

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

#ifndef _H2O_FORM_BROWSER_PROTOCOL_H_
#define _H2O_FORM_BROWSER_PROTOCOL_H_

#include <Uefi/UefiInternalFormRepresentation.h>
#include <Protocol/HiiImage.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/DisplayProtocol.h>

#define H2O_FORM_BROWSER_PROTOCOL_GUID \
{ \
  0xe1c17f13, 0xba6b, 0x42e5, 0x98, 0x5a, 0xb4, 0x18, 0x7c, 0xa0, 0x1a, 0x3f \
}

typedef struct _H2O_FORM_BROWSER_PROTOCOL  H2O_FORM_BROWSER_PROTOCOL;
typedef struct _H2O_FORM_BROWSER_O H2O_FORM_BROWSER_O;

typedef UINT32  H2O_PAGE_ID;
typedef UINT32  H2O_FORM_SET_ID;
typedef UINT32  H2O_STATEMENT_ID;

//
// Animation related definitions
//
typedef struct _H2O_ANIMATION_CELL {
  UINT16                    OffsetX;         ///< The X offset from the upper left hand corner of the logical window to position the indexed image.
  UINT16                    OffsetY;         ///< The Y offset from the upper left hand corner of the logical window to position the indexed image.
  EFI_IMAGE_INPUT           *Image;          ///< The image to display at the specified offset from the upper left hand corner of the logical window.
  UINT16                    Delay;           ///< The number of milliseconds to delay. If value is zero, no delay.
} H2O_ANIMATION_CELL;

typedef struct _H2O_ANIMATION_INPUT {
  UINT16                    Width;           ///< The overall width of the set of images (logical window width).
  UINT16                    Height;          ///< The overall height of the set of images (logical window height).
  UINT16                    CellCount;       ///< The number of EFI_HII_ANIMATION_CELL contained in the animation sequence.
//  H2O_ANIMATION_CELL       AnimationCell[];
} H2O_ANIMATION_INPUT;

//
// Hot Key Information
//
typedef  UINT32     H2O_EVT_TYPE;
typedef struct {
  EFI_KEY_DATA        KeyData;        ///< Description key state, unicode and scancode
  CHAR16              *Mark;          ///< Mark specified string
  CHAR16              *String;        ///< Key string
  H2O_EVT_TYPE        SendEvtType;    ///< H2O event type
} HOT_KEY_INFO;

#define H2O_DISPLAY_GRAYOUT          BIT0
#define H2O_DISPLAY_LOCK             BIT1
#define H2O_DISPLAY_READONLY         BIT2

//
// Statement related definitions
//
typedef struct _FORM_BROWSER_FORM FORM_BROWSER_FORM;

#define H2O_FORM_BROWSER_STATEMENT_SIGNATURE  SIGNATURE_32 ('H', 'F', 'S', 'T')
typedef struct _H2O_FORM_BROWSER_S {
  UINT32                    Signature;       ///< Particular statement signature. This signature must be 'H', 'F', 'S', 'T'
  UINT32                    Size;            ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_S)
  H2O_PAGE_ID               PageId;          ///< The page ID of this Statement.
  H2O_STATEMENT_ID          StatementId;     ///< The Statement ID of this Statement.
  UINT8                     Operand;        ///< UINT8 to save corresponding IFR OP code.
  EFI_IFR_OP_HEADER         *IfrOpCode;
  BOOLEAN                   Selectable;      ///< If set, this Statement is selectable. Otherwise, it ins't selectable.
  UINT32                    DisplayAttribute;///< Statement attributes: GRAYOUT, LOCK and READONLY
  //
  // Statement Header
  //
  CHAR16                    *Prompt;         ///< The promt string for this particular Statement.
  CHAR16                    *Help;           ///< The help string for this particular Statement.
  CHAR16                    *TextTwo;        ///< The secondary string for this particular Statement.Only use if OpCode is EFI_IFR_TEXT.
  //
  // Question Header
  //
  EFI_QUESTION_ID           QuestionId;      ///< Specific Question ID for this statement. The value of zero is reserved.
  EFI_VARSTORE_ID           VarStoreId;      ///< A value of zero indicates no variable storage.
                                             ///< can use this VarStoreId to get VarStoreInfo from GetVSinfo ().
  CHAR16                    *VariableName;   ///< Name/Value or EFI Variable name.
  UINT16                    VariableOffset;  ///< A 16-bit Buffer Storage offset (VarOffset).
  UINT8                     QuestionFlags;   ///< A bit-mask that determines which unique settings are active for this question.
  EFI_HII_VALUE             HiiValue;        ///< Edit copy for checkbox, numberic, oneof.
  //
  // OpCode specific members
  //
  UINT8                     Flags;           ///< Flag to save EFI_IFR_NUMERIC_SIZE and EFI_IFR_DISPLAY.
                                             ///< EFI_IFR_ORDERED_LIST, EFI_IFR_STRING,EFI_IFR_SUBTITLE,EFI_IFR_TIME, EFI_IFR_BANNER.
  UINT8                     ContainerCount;  ///< for EFI_IFR_ORDERED_LIST.
  UINT64                    Minimum;         ///< for EFI_IFR_ONE_OF/EFI_IFR_NUMERIC, it's Min/Max value.
  UINT64                    Maximum;         ///< for EFI_IFR_STRING/EFI_IFR_PASSWORD, it's Min/Max length.
  UINT64                    Step;            ///< The the minimum increment between values for EFI_IFR_ONE_OF/EFI_IFR_NUMERIC.
  EFI_DEFAULT_ID            DefaultId;       ///< For EFI_IFR_RESET_BUTTON to restore specific Default Store.
  EFI_GUID                  RefreshGuid;     ///< For EFI_IFR_REFRESH_ID. it can only use in questions.
  UINT8                     RefreshInterval; ///< For For EFI_IFR_REFRESH. Minimum number of seconds before the question value should be refreshed.
                                             ///< A value of zero indicates the question should not be refreshed automatically.
  EFI_IMAGE_INPUT           *Image;          ///< The optional image that will displayed with this statement.
  H2O_ANIMATION_INPUT       *Animation;      ///< The optional animation that will displayed with this statement.

  UINTN                     NumberOfOptions; /// Number of options
  H2O_FORM_BROWSER_O        *Options;        /// EFI_IFR_ONE_OF_OPTION list (H2O_FORM_BROWSER_O)

  //
  // Validation
  //
  BOOLEAN                   Inconsistent;    ///< TRUE means validation proccess is failed.
  CHAR16                    *ErrorText;      ///< Error string. Only use if Inconsistent is TRUE.
} H2O_FORM_BROWSER_S;

//
// Page related definitions
//
#define H2O_FORM_PAGE_SIGNATURE         SIGNATURE_32 ('H', 'F', 'P', 'G')
typedef struct _H2O_FORM_BROWSER_P {
  UINT32                    Signature;              ///< Particular statement signature. This signature must be 'H', 'F', 'P', 'G'.
  UINT32                    Size;                   ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_P).
  H2O_PAGE_ID               PageId;                 ///< Specific page ID.
  CHAR16                    *PageTitle;             ///< Title text for the page.
  EFI_IMAGE_INPUT           *Image;                 ///< The optional image that will displayed with this page.
  H2O_ANIMATION_INPUT       *Animation;             ///< The optional animation that will displayed with this page.
  UINT32                    NumberOfStatementIds;   ///< Number of Statement Ids in this page.
  H2O_STATEMENT_ID          *StatementIds;          ///< Array of Statement ID.
  EFI_QUESTION_ID           *QuestionIds;           ///< Array of Question ID.
  HOT_KEY_INFO              *HotKeyInfo;            ///< Hot key information.
} H2O_FORM_BROWSER_P;

typedef struct _H2O_FORM_BROWSER_S H2O_FORM_BROWSER_Q;

#define H2O_FORM_BROWSER_QUESTION_DEFAULT_SIGNATURE  SIGNATURE_32 ('H', 'F', 'Q', 'D')
typedef struct _H2O_FORM_BROWSER_QD {
  UINT32                    Signature;           ///< Particular statement signature. This signature must be 'H', 'F', 'Q', 'D'
  UINT32                    Size;                ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_QD).
  H2O_PAGE_ID               PageId;              ///< The page ID of this Question.
  EFI_QUESTION_ID           QuestionId;          ///< Specific Question ID for this question.
  EFI_DEFAULT_ID            DefaultId;           ///< Identifies the default store for this value.
  EFI_HII_VALUE             Value;               ///< Default value
} H2O_FORM_BROWSER_QD;

#define H2O_FORM_BROWSER_OPTION_SIGNATURE  SIGNATURE_32 ('H', 'F', 'O', 'P')
struct _H2O_FORM_BROWSER_O {
  UINT32                    Signature;           ///< Particular statement signature. This signature must be 'H', 'F', 'O', 'P'
  UINT32                    Size;                ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_O).
  CHAR16                    *Text;               ///< The text description for the option.
  EFI_IMAGE_INPUT           *Image;              ///< The optional image that will displayed with this Option.
  H2O_ANIMATION_INPUT       *Animation;          ///< The optional animation that will displayed with this Option.
  EFI_HII_VALUE             HiiValue;            ///< The value for the option.
  BOOLEAN                   Default;             ///< If set, this is the option selected when the user asks for the defaults.
  BOOLEAN                   ManufactureDefault;  ///< If set, this is the option selected when manufacturing defaults are set.
  BOOLEAN                   Visibility;          ///< If set, this option is visible. Otherwise, it will not displayed.
};
#define H2O_FORM_BROWSER_O_FROM_LINK(a) CR (a, H2O_FORM_BROWSER_O, Link, H2O_FORM_BROWSER_OPTION_SIGNATURE)

//
// Variable storage related definitions
//
#define H2O_HII_VARSTORE_BUFFER                 0       ///< mapping to EFI_IFR_VARSTORE_OP
#define H2O_HII_VARSTORE_NAME_VALUE             1       ///< mapping to EFI_IFR_VARSTORE_NAME_VALUE_OP
#define H2O_HII_VARSTORE_EFI_VARIABLE           2       ///< mapping to EFI_IFR_VARSTORE_EFI_OP
#define H2O_HII_VARSTORE_EFI_VARIABLE_BUFFER    3       ///< mapping to EFI_IFR_VARSTORE_EFI_OP

#define H2O_NAME_VALUE_NODE_SIGNATURE  SIGNATURE_32 ('H', 'N', 'V', 'N')
typedef struct {
  UINT32                    Signature;           ///< Particular statement signature. This signature must be 'H', 'N', 'V', 'N'
  UINT32                    Size;                ///< The size should be initialize to sizeof (H2O_NAME_VALUE_NODE).
  CHAR16                    *Name;               ///< Storage name for Name/Value node.
  CHAR16                    *Value;              ///< Value for Name/Value node.
  CHAR16                    *EditValue;          ///< Edit copy for Name/Value node.
} H2O_NAME_VALUE_NODE;


#define H2O_FORMSET_BROWSER_VARIABLE_STORE_SIGNATURE  SIGNATURE_32 ('H', 'F', 'V', 'S')
typedef struct _H2O_FORM_BROWSER_VS {
  UINT32                    Signature;           ///< Particular statement signature. This signature must be 'H', 'F', 'V', 'S'.
  UINT32                    Size;                ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_VS).
  H2O_PAGE_ID               PageId;              ///< Specific page ID for this Variable Storage.
  UINT8                     Type;                ///< Storage type.
  EFI_VARSTORE_ID           VarStoreId;          ///< Specific variable store ID for this Variable Storage.
  EFI_GUID                  Guid;                ///< GUID for Buffer, Name/Value or EFI variable storage.
  CHAR16                    *Name;               ///< Storage name for Buffer or EFI variable storage.
  UINT16                    StorageSize;         ///< The size of the variable store.
  UINT8                     *Buffer;             ///< Buffer Storage.
  UINT8                     *EditBuffer;         ///< Edit copy for Buffer Storage.
  UINT32                    NumberOfNameValue;   ///< Number of Name/Value nodes.
  H2O_NAME_VALUE_NODE      *NameValue;           ///< Point to Name/Value array.
  UINT32                    Attributes;          ///< For EFI_IFR_VARSTORE_EFI: EFI Variable attribute.
  CHAR16                    *ConfigHdr;          ///< <ConfigHdr>.
  CHAR16                    *ConfigRequest;      ///< <ConfigRequest> = <ConfigHdr> + <RequestElement>.
  UINT32                    ElementCount;        ///< Number of <RequestElement> in the <ConfigRequest>.
  UINT32                    SpareStrLen;         ///< Spare length of ConfigRequest string buffer.
} H2O_FORM_BROWSER_VS;

//
// Default storage related definitions
//
#define H2O_FORMSET_BROWSER_DEFAULT_STORE_SIGNATURE  SIGNATURE_32 ('H', 'F', 'D', 'S')
typedef struct _H2O_FORM_BROWSER_DS {
  UINT32                    Signature;           ///< Particular statement signature. This signature must be 'H', 'F', 'D', 'S'
  UINT32                    Size;                ///< The size should be initialized to sizeof (H2O_FORM_BROWSER_DS).
  H2O_PAGE_ID               PageId;              ///< Specific page ID for this Variable Storage.
  EFI_DEFAULT_ID            DefaultId;           ///< Identifies the default store for this value.
  UINT32                    NumberOfQD;          ///< Number of H2O_FORM_BROWSER_QD instances.
  H2O_FORM_BROWSER_QD       *QuestionDefault;    ///< Pointer to Question Default array.
} H2O_FORM_BROWSER_DS;

//
// Console device related definitions
//
typedef INT32     H2O_CONSOLE_ID;
typedef UINT32    H2O_CONSOLE_DEVICE_TYPE;
typedef struct _H2O_FORM_BROWSER_CONSOLE_DEV {
  UINT32                    Size;                ///< The size should be initialize to sizeof (H2O_FORM_BROWSER_CONSOLE_DEV).
  CONST CHAR8               *Name;               ///< Pointer to the console device name.
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;         ///< Console device device path.
  EFI_HANDLE                Handle;              ///< Console device handle.
  H2O_CONSOLE_DEVICE_TYPE   DeviceType;          ///< Console device type.
  H2O_CONSOLE_ID            ConsoleId;           ///< Console device id.
  EFI_GUID                  DisplayEngine;       ///< Belong a specified display engine.
} H2O_FORM_BROWSER_CONSOLE_DEV;

//
// DeviceType related definitions
//
#define H2O_FORM_BROWSER_CONSOLE_STI                    0x00000001
#define H2O_FORM_BROWSER_CONSOLE_STI2                   0x00000002
#define H2O_FORM_BROWSER_CONSOLE_SP                     0x00000004
#define H2O_FORM_BROWSER_CONSOLE_AP                     0x00000008
#define H2O_FORM_BROWSER_CONSOLE_STO                    0x00000010
#define H2O_FORM_BROWSER_CONSOLE_GO                     0x00000020
#define H2O_FORM_BROWSER_CONSOLE_SYSTEM                 0x00000000
#define H2O_FORM_BROWSER_CONSOLE_NOT_ASSIGNED           0xFFFFFFFF

//
// Setup Menu related definitions
//
typedef struct _SETUP_MENU_INFO {
  H2O_PAGE_ID         PageId;
  CHAR16              *PageTitle;
  EFI_IMAGE_INPUT     *PageImage;
} SETUP_MENU_INFO;

#define H2O_FORMSET_BROWSER_SETUP_MENU_SIGNATURE  SIGNATURE_32 ('F', 'B', 'S', 'M')
typedef struct _H2O_FORM_BROWSER_SM {
  UINT32                    Signature;            ///< Particular statement signature. This signature must be 'F', 'B', 'S', 'M'.
  EFI_GUID                  FormSetGuid;
  CHAR16                    *TitleString;
  CHAR16                    *CoreVersionString;   ///< The CoreVersionString should be initialized to "Rev. 5.0".
  UINT32                    NumberOfSetupMenus;   ///< Number of setup menu.
  SETUP_MENU_INFO           *SetupMenuInfoList;   ///< Setup menu information.
} H2O_FORM_BROWSER_SM;

//
// Dialog related definitions
//
#define H2O_FORMSET_BROWSER_DIALOG_SIGNATURE  SIGNATURE_32 ('F', 'B', 'D', 'G')
typedef struct {
  UINT32                    Signature;               ///< Particular statement signature. This signature must be 'F', 'B', 'D', 'G'.
  UINT32                    DialogType;              ///< Dialog type.
  UINT32                    Attribute;               ///< Dialog color setting.
  CHAR16                    *TitleString;            ///< Dialog title string.
  UINT32                    BodyStringCount;         ///< The count of body.
  UINT32                    BodyInputCount;          ///< The count of body which allow user to  input.
  UINT32                    ButtonCount;             ///< Number of buttons.
  CHAR16                    **BodyStringArray;       ///< Array of body string.
  CHAR16                    **BodyInputStringArray;  ///< Array of body input string.
  CHAR16                    **ButtonStringArray;     ///< Array of button string.
  EFI_HII_VALUE             *BodyHiiValueArray;      ///< Array of body HII value.
  EFI_HII_VALUE             *ButtonHiiValueArray;    ///< Array of button HII value.
  EFI_HII_VALUE             ConfirmHiiValue;         ///< Dialog select HII value
  H2O_FORM_BROWSER_Q        *H2OStatement;
} H2O_FORM_BROWSER_D;

//
// Dialog Type
//
#define H2O_FORM_BROWSER_D_TYPE_MSG                           0x00000001
#define H2O_FORM_BROWSER_D_TYPE_CONFIRM                       0x00000002
#define H2O_FORM_BROWSER_D_TYPE_SELECTION                     0x00000004
#define H2O_FORM_BROWSER_D_TYPE_SELECTION_CANCEL              0x00000008
#define H2O_FORM_BROWSER_D_TYPE_BODY_HORIZONTAL               0x00000010  // 0:vertical,1:horizontal
#define H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE               0x00000020  // 0:label,1:selectable
#define H2O_FORM_BROWSER_D_TYPE_BODY_INPUT_INTERLACE_VERTICAL 0x00000040  // 0:input box is interlace horziontally near by body
                                                                          // 1:input box is interlace vertically near by body
#define H2O_FORM_BROWSER_D_TYPE_BODY_INPUT_BOX_MULTI_LINE     0x00000080  // 0:single line,1:multi line
#define H2O_FORM_BROWSER_D_TYPE_BODY_align                    0x00000300  // bit8~9: 00: align left
                                                                          //         01: align centered
                                                                          //         10: align right
                                                                          //         11: align full
#define H2O_FORM_BROWSER_D_TYPE_SHOW_HELP                     0x00001000  // Specific dialog

typedef enum {
  H2O_FORM_BROWSER_D_TYPE_ONE_OF = 1,
  H2O_FORM_BROWSER_D_TYPE_ORDERED_LIST,
  H2O_FORM_BROWSER_D_TYPE_NUMERIC,
  H2O_FORM_BROWSER_D_TYPE_STRING,
  H2O_FORM_BROWSER_D_TYPE_DATE,
  H2O_FORM_BROWSER_D_TYPE_TIME,
  H2O_FORM_BROWSER_D_TYPE_PASSWORD
} H2O_FORM_BROWSER_D_TYPE_Q;

#define H2O_FORM_BROWSER_D_TYPE_QUESTIONS                     0x00FF0000

//
// Display Engine Event Type Definitions
//
typedef struct _H2O_DISPLAY_ENGINE_EVT {
  UINT32        Size;                        ///< Unsigned integer specifies the size of the structure, in bytes. Producers of this
                                             ///< structure should initialize this field to sizeof(H2O_DISPLAY_ENGINE_EVT).
                                             ///< Consumers of this structure should check this field before accessing any
                                             ///< members beyond Type.
  H2O_EVT_TYPE  Type;                        ///< Enumerated value that specifies the Notification.
  INT32         Target;                      ///< Bitmask that specifies the console target for the notification. Bit 0 indicates
                                             ///< Console 0, Bit 1 indicates Console 1, etc. A value of 0 in this field indicates that
                                             ///< the notification is meant for the Forms Browser itself. 0xffffffff indicates a
                                             ///< broadcast to all Display Engines.
} H2O_DISPLAY_ENGINE_EVT;

//
// The sources of the event is form browser
//
#define H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS                 0x00000001
#define H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE             0x00000002
#define H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE             0x00000003
#define H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER                    0x00000004
#define H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_L                   0x00000005
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_L                   0x00000006
#define H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_P                   0x00000007
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_P                   0x00000008
#define H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q                   0x00000009
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_Q                   0x0000000A
#define H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH                  0x0000000B
#define H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q                0x0000000C
#define H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D                   0x0000000D
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D                   0x0000000E

//
// The sources of the event is form browser or display engine
//
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q                 0x00001001
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P                 0x00001002
#define H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q               0x00001003
#define H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q                 0x00001004
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT                  0x00001005
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT_Q                0x00001006
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT_P                0x00001007

//
// The sources of the EVT is display engine
//
#define H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT                     0x00002001
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT              0x00002002
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT             0x00002003
#define H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT_P                   0x00002004
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT_P            0x00002005
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT_P           0x00002006
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_P                 0x00002007
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_P                0x00002008
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT                   0x00002009
#define H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD                  0x0000200A
#define H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP                0x0000200B

//
// Event Notify Structures
//
//
// H2O_DISPLAY_ENGINE_EVT_KEYPRESS
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  EFI_DEVICE_PATH                     *DevicePath;
  EFI_KEY_DATA                        KeyData;
} H2O_DISPLAY_ENGINE_EVT_KEYPRESS;

//
// H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  EFI_SIMPLE_POINTER_STATE              State;           ///< Describe relative position state.
} H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE;

//
// H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;              ///< The header contains a H2O display engine event type.
  EFI_ABSOLUTE_POINTER_STATE            AbsPtrState;      ///< Describe absolute position state.
} H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE;

//
// H2O_DISPLAY_ENGINE_EVT_TIMER
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  UINT64                                Time;            ///< Trigger time.
  INT32                                 TimerId;         ///< A id to the fnction to be notified when the time-out value elapses.
} H2O_DISPLAY_ENGINE_EVT_TIMER;

//
// H2O_DISPLAY_ENGINE_EVT_OPEN_L
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_OPEN_L;

//
// H2O_DISPLAY_ENGINE_EVT_SHUT_L
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.} H2O_DISPLAY_ENGINE_EVT_SHUT_L;
} H2O_DISPLAY_ENGINE_EVT_SHUT_L;
//
// H2O_DISPLAY_ENGINE_EVT_OPEN_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_OPEN_P;

//
// H2O_DISPLAY_ENGINE_EVT_SHUT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_SHUT_P;

//
// H2O_DISPLAY_ENGINE_EVT_OPEN_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_IFR_OP_HEADER                     *IfrOpCode;
} H2O_DISPLAY_ENGINE_EVT_OPEN_Q;

//
// H2O_DISPLAY_ENGINE_EVT_SHUT_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_IFR_OP_HEADER                     *IfrOpCode;
} H2O_DISPLAY_ENGINE_EVT_SHUT_Q;

//
// H2O_DISPLAY_ENGINE_EVT_REFRESH
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_REFRESH;

//
// H2O_DISPLAY_ENGINE_EVT_REFRESH_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_IFR_OP_HEADER                     *IfrOpCode;      ///< The pointer of IFR op code.
} H2O_DISPLAY_ENGINE_EVT_REFRESH_Q;

//
// H2O_DISPLAY_ENGINE_EVT_OPEN_D
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_FORM_BROWSER_D                    Dialog;          ///<  The dialog  information.
} H2O_DISPLAY_ENGINE_EVT_OPEN_D;

//
// H2O_DISPLAY_ENGINE_EVT_SHUT_D
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_SHUT_D;

//
// H2O_DISPLAY_ENGINE_EVT_SELECT_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_IFR_OP_HEADER                     *IfrOpCode;
} H2O_DISPLAY_ENGINE_EVT_SELECT_Q;

//
// H2O_DISPLAY_ENGINE_EVT_SELECT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_SELECT_P;

//
// H2O_DISPLAY_ENGINE_EVT_CHANGING_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_HII_VALUE                         BodyHiiValue;    ///< The body HII value.
  EFI_HII_VALUE                         ButtonHiiValue;  ///< Press button HII value.
} H2O_DISPLAY_ENGINE_EVT_CHANGING_Q;

//
// H2O_DISPLAY_ENGINE_EVT_CHANGE_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
  EFI_HII_VALUE                         HiiValue;        ///< The question HII values.
} H2O_DISPLAY_ENGINE_EVT_CHANGE_Q;

//
// H2O_DISPLAY_ENGINE_EVT_DEFAULT
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_DEFAULT;

//
// H2O_DISPLAY_ENGINE_EVT_DEFAULT_Q
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
  EFI_QUESTION_ID                       QuestionId;      ///< The question identifier.
} H2O_DISPLAY_ENGINE_EVT_DEFAULT_Q;

//
// H2O_DISPLAY_ENGINE_EVT_DEFAULT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_DEFAULT_P;

//
// H2O_DISPLAY_ENGINE_EVT_EXIT
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_EXIT;

//
// H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT;

//
// H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT;

//
// H2O_DISPLAY_ENGINE_EVT_EXIT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_EXIT_P;

//
// H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT_P;

//
// H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT_P;

//
// H2O_DISPLAY_ENGINE_EVT_SUBMIT_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_SUBMIT_P;

//
// H2O_DISPLAY_ENGINE_EVT_DISCARD_P
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
  H2O_PAGE_ID                           PageId;          ///< The page identifier.
} H2O_DISPLAY_ENGINE_EVT_DISCARD_P;

//
// H2O_DISPLAY_ENGINE_EVT_SUBMIT
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_SUBMIT;

//
// H2O_DISPLAY_ENGINE_EVT_DISCARD
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_DISCARD;

//
// H2O_DISPLAY_ENGINE_EVT_SHOW_HELP
//
typedef struct {
  H2O_DISPLAY_ENGINE_EVT                Hdr;             ///< The header contains a H2O display engine event type.
} H2O_DISPLAY_ENGINE_EVT_SHOW_HELP;

//
// Event Target
//
#define H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER  0
#define H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST     -1


//
// Protocol definitions
//
/**
  Get page data according to specific page ID.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Specific input page ID.
  @param[out] PageInfo           A pointer to point H2O_FORM_BROWSER_P instance.

  @retval EFI_SUCCESS            Get page data successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or PageInfo is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific input page ID.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_PAGE_INFO) (
  IN     H2O_FORM_BROWSER_PROTOCOL      *This,
  IN     H2O_PAGE_ID                    PageId,
  OUT    H2O_FORM_BROWSER_P             **PageInfo
  );

/**
  Get all of page IDs.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[out] PageCount          A pointer to the number of page ID in PageBuffer.
  @param[out] PageBuffer         A pointer to a buffer which will be allocated by the function which contains all
                                 of H2O_PAGE_ID. The buffer should be freed by the caller.

  @retval EFI_SUCCESS            Get array of page IDs successful.
  @retval EFI_NOT_FOUND          No page IDs match the search.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate PageIdBuffer.
  @retval EFI_INVALID_PARAMETER  This is NULL, PageIdCount is NULL or PageIdBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_PAGES_ALL) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  OUT     UINT32                         *PageIdCount,
  OUT     H2O_PAGE_ID                    **PageIdBuffer
  );

/**
  Get Statement data according to Page identifier and Statement identifier.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[in]  StatementId        Input Statement ID.
  @param[out] StatementData      A pointer to H2O_FORM_BROWSER_S instance.

  @retval EFI_SUCCESS            Get Statement data successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, StatementData is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific Statement data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_STATEMENT_INFO) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  IN      H2O_STATEMENT_ID               StatementId,
  OUT     H2O_FORM_BROWSER_S             **StatementData
  );


/**
  Get Question data according to Page identifier and Question identifier.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[in]  QuestionId         Input Question ID.
  @param[out] QuestionData       A pointer to H2O_FORM_BROWSER_Q instance.

  @retval EFI_SUCCESS            Get Question data successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, QuestionData is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific Question data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_QUESTION_INFO) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  IN      EFI_QUESTION_ID                QuestionId,
  OUT     H2O_FORM_BROWSER_Q             **QuestionData
  );

/**
  Get all of Question identifiers from specific Page.

  @param[in]  This               Get array of Question identifiers successful.
  @param[in]  PageId             Input Page identifier.
  @param[out] QuestionIdCount    A pointer to the number of Question ID in QuestionIdBuffer.
  @param[out] QuestionIdBuffer   A pointer to a buffer which will be allocated by the function which contains all
                                 of Question IDs. The buffer should be freed by the caller.

  @retval EFI_SUCCESS            Get array of Question IDs successful.
  @retval EFI_NOT_FOUND          No Question ID match the search.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate QuestionIdBuffer.
  @retval EFI_INVALID_PARAMETER  This is NULL, QuestionIdCount is NULL or QuestionIdBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_QUESTIONS_ALL) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  OUT     UINT32                         *QuestionIdCount,
  OUT     EFI_QUESTION_ID                **QuestionIdBuffer
  );

/**
  According to Page identifier and Question identifier to get all of types default values in this question.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[in]  QuestionId         Input Question ID.
  @param[out] DefaultCount       A pointer to the number of H2O_FORM_BROWSER_QD instance in DefaultBuffer.
  @param[out] DefaultBuffer      A pointer to a buffer which will be allocated by the function which contains all
                                 of H2O_FORM_BROWSER_QD instances. The buffer should be freed by the caller.

  @retval EFI_SUCCESS            Get Question defaults successful.
  @retval EFI_NOT_FOUND          Cannot find default values from specific Question data.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate DefaultBuffer.
  @retval EFI_INVALID_PARAMETER  This is NULL, DefaultCount is NULL or DefaultBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_QUESTION_DEFAULTS) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  IN      EFI_QUESTION_ID                QuestionId,
  OUT     UINT32                         *DefaultCount,
  OUT     H2O_FORM_BROWSER_QD            **DefaultBuffer
  );

/**
  Get Variable Store data according to specific Variable Store ID.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[in]  VarStoreId         Specific input Variable Store ID.
  @param[out] VarbleStoreData    Pointer to H2O_FORM_BROWSER_VS instance.

  @retval EFI_SUCCESS            Get Variable Store successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or VariableStoreData is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific Variable Store data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_VARSTORE_INFO) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  IN      EFI_VARSTORE_ID                VarStoreId,
  OUT     H2O_FORM_BROWSER_VS            **VarbleStoreData
  );


/**
  Get all of Variable Store identifiers from specific Page.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[out] VarStoreCount      A pointer to the number of Variable Store ID in VarStoreBuffer.
  @param[out] VarStoreBuffer     A pointer to a buffer which will be allocated by the function which contains all
                                 of EFI_VARSTORE_ID instances. The buffer should be freed by the caller.

  @retval EFI_SUCCESS            Get array of Variable Store IDs successful.
  @retval EFI_NOT_FOUND          No Variable Store ID match the search.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate VarStoreBuffer.
  @retval EFI_INVALID_PARAMETER  This is NULL,, VarStoreCount is NULL or VarStoreBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_VARSTORES_ALL) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  OUT     UINTN                          *VarStoreCount,
  OUT     EFI_VARSTORE_ID                **VarStoreBuffer
  );

/**
  Get Default Store data according to specific Default Store identifier.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId             Input Page identifier.
  @param[in]  DefaultStoreId     Specific input Variable Store ID.
  @param[out] DefaultStoreData   A pointer to H2O_FORM_BROWSER_DS instance.

  @retval EFI_SUCCESS            Get Default Store successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or DefaultStoreData is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific Default Store data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_DEFAULT_STORE_INFO) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  IN      EFI_DEFAULT_ID                 DefaultStoreId,
  OUT     H2O_FORM_BROWSER_DS            **DefaultStoreData
  );

/**
  Get all of Default Store identifiers from specific Page.

  @param[in]  This                  A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in]  PageId                Input Page identifier.
  @param[out] DefaultStoreIdCount   A pointer to the number of Default Store ID in DefaultStoreIdBuffer.
  @param[out] DefaultStoreIdBuffer  A pointer to a buffer which will be allocated by the function which contains all
                                    of EFI_DEFAULT_ID instances.

  @retval EFI_SUCCESS               Get array of Default Store IDs successful.
  @retval EFI_NOT_FOUND             No Default Store ID match the search.
  @retval EFI_OUT_OF_RESOURCES      There are not enough resources available to allocate DefaultStoreIdBuffer.
  @retval EFI_INVALID_PARAMETER     This is NULL, DefaultStoreIdCount is NULL or DefaultStoreIdBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_DEFAULT_STORES_ALL) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_PAGE_ID                    PageId,
  OUT     UINT32                         *DefaultStoreIdCount,
  OUT     EFI_DEFAULT_ID                 **DefaultStoreIdBuffer
  );


/**
  Add console device to supported list.

  @param[in] This                 A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in] ConsoleDev           A pointer to H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS             Add console device to supported list successful.
  @retval EFI_ALREADY_STARTED     The console device is already in supported list.
  @retval EFI_INVALID_PARAMETER   This is NULL or CosoleDev is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_ADD_CONSOLE) (
  IN      H2O_FORM_BROWSER_PROTOCOL      *This,
  IN      H2O_FORM_BROWSER_CONSOLE_DEV   *ConsoleDev
  );



/**
  Return information about all installed Consoles, the devices associated with them and
  which Display Engine is currently managing them.

  @param[in]  This               Pointer to current instance of this protocol.
  @param[in]  CId                Unsigned integer that specifies the console index. -1 = all consoles.
  @param[out] CDevCount          Pointer to returned console device count.
  @param[out] CDevs              Pointer to returned pointer to array of console device descriptors. The array
                                 should be freed by the caller. CDevCount indicates the number of elements in the
                                 array.

  @retval EFI_SUCCESS            Get array of console devices successful.
  @retval EFI_NOT_FOUND          No consoleID match the search.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate CDevs.
  @retval EFI_INVALID_PARAMETER  This is NULL, CDevCount is NULL or CDevs is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_CONSOLE_INFO) (
  IN     H2O_FORM_BROWSER_PROTOCOL             *This,
  IN     H2O_CONSOLE_ID                        CId,
  OUT    UINT32                                *CDevCount,
  OUT    H2O_FORM_BROWSER_CONSOLE_DEV          **CDevs
  );

/**
  Return array of Console IDs.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[out] CIdCount           Pointer to returned console ID count.
  @param[out] CIdBuffer          Pointer to returned pointer to array of console ID. The array should be freed by
                                 the caller. CIdCount indicates the number of elements in the array.

  @retval EFI_SUCCESS            Get array of console IDs successful.
  @retval EFI_NOT_FOUND          No console IDs ID match the search.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to allocate CIdBuffer.
  @retval EFI_INVALID_PARAMETER  This is NULL, CIdCount is NULL or CIdBuffer is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_CONSOLES_ALL) (
  IN     H2O_FORM_BROWSER_PROTOCOL             *This,
  OUT    UINT32                                *CIdCount,
  OUT    H2O_CONSOLE_ID                        **CIdBuffer
  );


/**
  Return data about the Setup Menu.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[out] SetupMenuData      A pointer to returned setup menu data

  @retval EFI_SUCCESS            Get setup menu data successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or SetupMenuData is NULL.
  @retval EFI_NOT_FOUND          Cannot find setup menu data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_GET_SETUP_MENU_INFO) (
  IN     H2O_FORM_BROWSER_PROTOCOL       *This,
  OUT    H2O_FORM_BROWSER_SM             **SetupMenuData
  );

/**
  Update setup menu data.

  @param[in]  This               A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[out] SetupMenuData      A pointer to input setup menu data

  @retval EFI_SUCCESS            Update setup menu info successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or SetupMenuData is NULL.
  @retval EFI_UNSUPPORTED        Cannot update setup menu data.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_UPDATE_SETUP_MENU) (
  IN     H2O_FORM_BROWSER_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_SM             *SetupMenuData
  );

/**
  Add the notification to the notification queue and signal the Notification event.

  @param[in] This                A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in] Notify              A pointer to the H2O_DISPLAY_ENGINE_EVT instance.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_NOTIFY) (
  IN       H2O_FORM_BROWSER_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Notify
  );

/**
  Add the notification to the notification queue and signal the notification event after
  a period of time.

  @param[in] This                A pointer to the H2O_FORM_BROWSER_PROTOCOL instance.
  @param[in] Notify              A pointer to the H2O_DISPLAY_ENGINE_EVT instance.
  @param[in] TriggerTime         The number of 100ns units until the timer expires.  A
                                 TriggerTime of 0 is legal. If TriggerTime is 0, signal
                                 event immediately.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_FORM_BROWSER_REGISTER_TIMER) (
  IN       H2O_FORM_BROWSER_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Notify,
  IN       UINT64                        TriggerTime
  );

struct _H2O_FORM_BROWSER_PROTOCOL {
  UINT32                                   Size;
  H2O_FORM_BROWSER_P                      *CurrentP;
  H2O_FORM_BROWSER_Q                      *CurrentQ;
  H2O_FORM_BROWSER_GET_PAGE_INFO           GetPInfo;
  H2O_FORM_BROWSER_GET_PAGES_ALL           GetPAll;
  H2O_FORM_BROWSER_GET_STATEMENT_INFO      GetSInfo;
  H2O_FORM_BROWSER_GET_QUESTION_INFO       GetQInfo;
  H2O_FORM_BROWSER_GET_QUESTIONS_ALL       GetQAll;
  H2O_FORM_BROWSER_GET_QUESTION_DEFAULTS   GetQDefaults;
  H2O_FORM_BROWSER_GET_VARSTORE_INFO       GetVSInfo;
  H2O_FORM_BROWSER_GET_VARSTORES_ALL       GetVSAll;
  H2O_FORM_BROWSER_GET_DEFAULT_STORE_INFO  GetDSInfo;
  H2O_FORM_BROWSER_GET_DEFAULT_STORES_ALL  GetDSAll;
  H2O_FORM_BROWSER_ADD_CONSOLE             AddC;
  H2O_FORM_BROWSER_GET_CONSOLE_INFO        GetCInfo;
  H2O_FORM_BROWSER_GET_CONSOLES_ALL        GetCAll;
  H2O_FORM_BROWSER_GET_SETUP_MENU_INFO     GetSMInfo;
  H2O_FORM_BROWSER_UPDATE_SETUP_MENU       UpdateSMInfo;
  H2O_FORM_BROWSER_NOTIFY                  Notify;
  H2O_FORM_BROWSER_REGISTER_TIMER          RegisterTimer;
};

extern GUID gH2OFormBrowserProtocolGuid;

#endif
