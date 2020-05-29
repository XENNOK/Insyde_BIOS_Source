/** @file


;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _DYNAMIC_HOTKEY_DXE_H_
#define _DYNAMIC_HOTKEY_DXE_H_

#include <Uefi.h>
#include <PostKey.h>
#include <KernelSetupConfig.h>
#include <Csm/LegacyBiosDxe/LegacyBiosInterface.h>

#include <Protocol/DynamicHotKey.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/LegacyRegion2.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>

#include <Library/DebugLib.h>
#include <Library/MultiConfigBaseLib.h>

#include <Guid/DynamicHotKey.h>


extern UINT8  DynamicHotKeyDxeStrings[];
//
//Macro
//
#define GET_ARRAY_COUNT(a)                        (sizeof(a)/sizeof((a)[0]))
#define FREE(a)                                   do{if(a != NULL) FreePool(a); a=NULL;}while(0);
#define GET_NEXT_DYNAMIC_STRING_PTR(a)            (DYNAMIC_STRING_EXTERNAL *)((UINTN)a + sizeof(DYNAMIC_STRING_EXTERNAL) - sizeof(CHAR8) + AsciiStrSize(&(a->String)))

//
// Multi Config Region Base
//
#define FLASH_REGION_MULTI_CONFIG_BASE  PcdGet32(PcdFlashNvStorageMultiConfigBase)

//
// Multi Config Region Size
//
#define FLASH_REGION_MULTI_CONFIG_SIZE  PcdGet32(PcdFlashNvStorageMultiConfigSize)

//
// Table max limit 
//
#define HOTKEY_TABLE_PTR_SIGNATURE      SIGNATURE_64 ('$', 'H', 'K', 'S', 'T', 'R', 'S', 'I')
#define HOTKEY_INFO_SIGNATURE           SIGNATURE_64 ('$', 'H', 'K', 'I', 'N', 'F', 'O', 'R')

#define HOTKEY_AND_STRING_TABLE_MAX_ADDRESS            0xFFFFFFFF  //4G
#define HOTKEY_TABLE_PTR_ALIGNMENT                     0x10
#define F0000Region                                    0x01
#define E0000Region                                    0x02

#define OEM_STRING_LOCATION_X                          50

//
// If set x or y value of OEM badging string to -1, system will auto place string at
// below boot image when BGRT is present or center of screen when BGRT is not exist.
//
#define OEM_STRING_GRAPHICAL_LOCATION_X                (UINTN) -1
#define OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X     (UINTN) -1
#define OEM_STRING_GRAPHICAL_LOCATION_Y                (UINTN) -1
#define OEM_STRING1_LOCATION_Y                         500
#define OEM_STRING2_LOCATION_Y                         575
#define OEM_STRING3_LOCATION_Y                         550
#define OEM_STRING4_LOCATION_Y                         625
#define ME_INFORM_STRING_LOCATION_X                    50
#define ME_INFORM_STRING1_LOCATION_Y                   0


#define STRING_LOCATION_MIN                            0
#define STRING_LOCATION_CENTER                         (UINTN) (-1)
#define STRING_LOCATION_MAX                            (UINTN) (-2)


//
// Dynamic string definitions
//
#define DSTRING_NO_HOTKEY                              0xFF
#define DSTRING_NOT_SYSTEM_INFO                        0xFF

#ifndef STRING_REF
typedef UINT16  STRING_REF;
#endif

#pragma pack(1)

//
// Structure for hot key defined in NV
//
typedef struct {
  UINT8   KeyId;              ///< ID of this hot key. 0xFF means the end of hot key list.
  UINT8   ScanCode;           ///< IBM PC base scan code, please see InsydeModulePkg/Universal/Console/MonitorKeyDxe/MonitorKey.c for reference
  UINT8   ShiftKey;           ///< 1 means this hot key works with shift key pressed
  UINT8   AltKey;             ///< 1 means this hot key works with alt key pressed
  UINT8   CtrlKey;            ///< 1 means this hot key works with ctrl key pressed
  UINT8   Operation;          ///< Operation after hot key pressed, please see SharkBayChipsetPkg/Include/PostKey.h for reference
} DYNAMIC_HOTKEY;

//
// Internal use hot key structure
//
typedef struct {
  UINT8           BitIndex;      ///< The bit number this hot key will be added into MonitorKey driver
  DYNAMIC_HOTKEY  HotKeyList;    ///< Structure for hot key defined in NV
} DYNAMIC_HOTKEY_INTERNAL;

//
// Structure of Flag in dynamic string
//
typedef struct {
  UINT16   TextMode    :1;       ///< 1: This string will be shown only in text mode.
  UINT16   GraphicMode :1;       ///< 1: This string will be shown only in graphic mode.
  UINT16   BeforePress :1;       ///< 1: This string will be shown before hot key pressed. Ignore if KeyId is 0xFF.
  UINT16   AfterPress  :1;       ///< 1: This string will be shown after hot key pressed. Ignore if KeyId is 0xFF.
  UINT16   Reserved    :9;
} DYNAMIC_STRING_FLAG;

//
// Structure for string defined in NV and for internal use both
//
typedef struct {
  UINT8                KeyId;         ///< ID of this hot key. Use to connect to specified hot key. 0xFF means no hot key connected.
  UINT8                SystemInfoId;  ///< ID of this string. Use to distinguish to specified system info string. 0xFF means it's not a system string.
  UINT16               LocationX;     ///< String coordinate X.
  UINT16               LocationY;     ///< String coordinate Y.
  DYNAMIC_STRING_FLAG  Flag;          ///< Some attribue. See above definitions.
  EFI_UGA_PIXEL        Foreground;    ///Foreground color for string output.
  EFI_UGA_PIXEL        Background;    ///Background color for string output.
  STRING_REF           StringToken;   ///< StringToken
} DYNAMIC_STRING;

typedef struct {
  UINT8                KeyId;         ///< ID of this hot key. Use to connect to specified hot key. 0xFF means no hot key connected.
  UINT8                SystemInfoId;  ///< ID of this string. Use to distinguish to specified system info string. 0xFF means it's not a system string.
  UINT16               LocationX;     ///< String coordinate X.
  UINT16               LocationY;     ///< String coordinate Y.
  DYNAMIC_STRING_FLAG  Flag;          ///< Some attribue. See above definitions.
  EFI_UGA_PIXEL        Foreground;    ///Foreground color for string output.
  EFI_UGA_PIXEL        Background;    ///Background color for string output.
  CHAR8                String;        ///< String will be shown. Must be end with \0 character.
} DYNAMIC_STRING_EXTERNAL;

typedef struct {
  UINT8                OperationId;   ///< ID of this hot key operation. Use to indicate the operation of hot key.
  STRING_REF           StringToken;   ///< StringToken
} DYNAMIC_OPERATION;

typedef struct {
  UINT8                OperationId;   ///< ID of this hot key. Use to connect to specified hot key. 0xFF means no hot key connected.
  CHAR8                String;        ///< String will be shown. Must be end with \0 character.
} DYNAMIC_OPERATION_EXTERNAL;

typedef struct {
  UINT64    Signature;
  UINT8     Version;
  UINT64    DynamicHotKeyDxeStringsOffset;
  UINT64    mHotKeyDefineOffset;
  UINT64    StringDefineOffset;
  UINT64    SystemInfoTableOffset;
  UINT64    OperationTableOffset;
  UINT8     HotkeyCount;
  UINT8     StringCount;
  UINT8     SystemInfoCount;
  UINT8     OperationCount;
  UINT8     Reserve[3];
} DYNAMIC_HOTKEY_PACKAGE_HEAD;

typedef enum {
  INITIAL_VALUE,
  SET_COUNT,
  GET_LEFT_COUNT,
  GET_CENTER_COUNT,
  GET_RIGHT_COUNT
} BOTTOM_STRING_COUNT_OPERATION;

//
// Dynamic hot key definitions
//
typedef enum {
  KEYID0,
  KEYID1,
  KEYID2,
  KEYID3,
  KEYID4,
//[-start-140630-IB10301112-add]//
  KEYID5
//[-end-140630-IB10301112-add]//
} DYNAMIC_HOTKEY_DEFINITION;

typedef enum {
  BIOS_VERSION,
  BIOS_RELEASE_DATE,
  CPU_TYPE,
  MEMORY_BUS_SPEED,
  CPU_ID
} INFO_TYPE;

typedef struct {
  UINT8                Type;          ///< Type for communication between BIOS and utility
  STRING_REF           StringToken;   ///< StringToken
  UINT16               LocationX;     ///< String coordinate X.
  UINT16               LocationY;     ///< String coordinate Y.
} DYNAMIC_SYSTEM_INFO;

typedef struct {
  UINT8                SystemInfoId; ///< ID of this string. Use to distinguish to specified system info string. 0xFF means it's not a system string.
  UINT16               LocationX;    ///< String coordinate X.
  UINT16               LocationY;    ///< String coordinate Y.
  CHAR8                String;       ///< String will be shown. Must be end with \0 character.
} DYNAMIC_SYSTEM_INFO_EXTERNAL;

typedef struct _HOTKEY_AND_STRING_TABLE {
  UINT64                Signature;
  UINT8                 HotkeyCount;
  UINT8                 StringCount;
  UINT8                 SystemInfoCount;
  UINT8                 OperationCount;  
} HOTKEY_AND_STRING_TABLE;

typedef struct _HOTKEY_AND_STRING_PTR {
  UINT64                Signature;
  UINT32                Address;
  UINT32                Size;
} HOTKEY_AND_STRING_PTR;

#pragma pack()


#endif

