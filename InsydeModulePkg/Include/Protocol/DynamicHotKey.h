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

#ifndef _DYNAMIC_HOTKEY_H_
#define _DYNAMIC_HOTKEY_H_


#include <Protocol/UsbLegacyPlatform.h>
#include <Protocol/UgaDraw.h>


#define DYNAMIC_HOTKEY_PROTOCOL_GUID \
  {0xd804fdef, 0xd62a, 0x464a, {0x8b, 0x7d, 0xad, 0xc6, 0x9b, 0xc1, 0x9c, 0xbb}}

typedef struct _DYNAMIC_HOTKEY_PROTOCOL DYNAMIC_HOTKEY_PROTOCOL;

/**
 Read specified string/data based on field from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Field               Field id want to get.
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of string buffer.
 @param[out]        StringBuffer        Buffer to store string/data.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
typedef
EFI_STATUS
(EFIAPI *GET_DYNAMIC_HOTKEY_LIST) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN OUT KEY_ELEMENT                **HotKeyList
  );

/**
 Read specified string/data based on field from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Field               Field id want to get.
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of string buffer.
 @param[out]        StringBuffer        Buffer to store string/data.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
typedef
EFI_STATUS
(EFIAPI *GET_DYNAMIC_HOTKEY_OPERATION) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     UINT32                     KeyDetected,
  OUT    UINTN                      *Operation
  );

/**
 Write specified string/data based on field to FRU.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Field               Field id want to write.
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of write string buffer.
 @param[in]         StringBuffer        Buffer to store string/data.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than write data size.
*/
typedef
EFI_STATUS
(EFIAPI *GET_DYNAMIC_STRING_COUNT) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     BOOLEAN                    KeyPressed,
  OUT    UINTN                      *StringCount
  );
  


/**
 Get Chassis Type value/string field in Chassis info area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in,out]     ChassisTypeStrSize  When in, the size of chassis type string in bytes to store the result;
                                        When out, the real size of string buffer in bytes.
 @param[out]        ChassisType         Enumeration value for chassis type field.
 @param[out]        ChassisTypeString   Chassis type string.

 @retval EFI_UNSUPPORTED                Common header didn't exist.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
typedef
EFI_STATUS
(EFIAPI *GET_DYNAMIC_STRING) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     UINTN                      Index,
  IN     BOOLEAN                    KeyPressed,
  OUT    CHAR16                     **String,
  OUT    UINTN                      *CoordinateX,
  OUT    UINTN                      *CoordinateY
  );

typedef
EFI_STATUS
(EFIAPI *GET_DYNAMIC_STRING_COLOR) (
  IN     UINTN                      Index,
  IN     BOOLEAN                    KeyPressed,  
  OUT    EFI_UGA_PIXEL              *Foreground,
  OUT    EFI_UGA_PIXEL              *Background    
  );

/**
 Get Chassis Type value/string field in Chassis info area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in,out]     ChassisTypeStrSize  When in, the size of chassis type string in bytes to store the result;
                                        When out, the real size of string buffer in bytes.
 @param[out]        ChassisType         Enumeration value for chassis type field.
 @param[out]        ChassisTypeString   Chassis type string.

 @retval EFI_UNSUPPORTED                Common header didn't exist.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
typedef
BOOLEAN
(EFIAPI *IS_DYNAMIC_HOTKEY_PRESSED) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This
  );


typedef
EFI_STATUS
(EFIAPI *ADJUST_STRING_POSTION) (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     BOOLEAN                    IsClearCountMode,
  IN     BOOLEAN                    IsGraphicMode,
  IN     UINTN                      StrDestCenterY,
  IN     CHAR16                     *TmpStr,
  IN OUT UINTN                      *CoordinateX,
  IN OUT UINTN                      *CoordinateY
  );

typedef
EFI_STATUS
(EFIAPI *SHOW_SYSTEM_INFO) (
  IN OUT UINT8                      *PrintLine
  );

//
// Dynamic HotKey Protocol
//
struct _DYNAMIC_HOTKEY_PROTOCOL {
  GET_DYNAMIC_HOTKEY_LIST        GetDynamicHotKeyList;
  GET_DYNAMIC_HOTKEY_OPERATION   GetDynamicHotKeyOperation;
  GET_DYNAMIC_STRING_COUNT       GetDynamicStringCount;
  GET_DYNAMIC_STRING             GetDynamicString;
  GET_DYNAMIC_STRING_COLOR       GetDynamicStringColor;
  IS_DYNAMIC_HOTKEY_PRESSED      IsDynamicHotKeyPressed;
  ADJUST_STRING_POSTION          AdjustStringPosition;
  SHOW_SYSTEM_INFO               ShowSystemInformation;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gDynamicHotKeyProtocolGuid;


#endif

