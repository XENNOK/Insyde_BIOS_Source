/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c) 2007 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  UefiIfrLibrary.h

Abstract:

  The file contain all library function for Ifr Operations.

--*/

#ifndef _IFRLIBRARY_H
#define _IFRLIBRARY_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "TianoHii.h"

#include EFI_PROTOCOL_DEFINITION (HiiFont)
#include EFI_PROTOCOL_DEFINITION (HiiImage)
#include EFI_PROTOCOL_DEFINITION (HiiString)
#include EFI_PROTOCOL_DEFINITION (HiiDatabase)
#include EFI_PROTOCOL_DEFINITION (HiiConfigRouting)
#include EFI_PROTOCOL_DEFINITION (HiiConfigAccess)
#include EFI_PROTOCOL_DEFINITION (FormBrowser2)
#include EFI_PROTOCOL_DEFINITION (SimpleTextOut)

#include EFI_GUID_DEFINITION (GlobalVariable)

#define IFR_LIB_DEFAULT_STRING_SIZE     0x200

//
// The architectural variable "Lang" and "LangCodes" are deprecated in UEFI
// specification. While, UEFI specification also states that these deprecated
// variables may be provided for backwards compatibility.
// If "LANG_SUPPORT" is defined, "Lang" and "LangCodes" will be produced;
// If "LANG_SUPPORT" is undefined, "Lang" and "LangCodes" will not be produced.
//
#define LANG_SUPPORT

#define EFI_LANGUAGE_VARIABLE           L"Lang"
#define EFI_LANGUAGE_CODES_VARIABLE     L"LangCodes"

#define UEFI_LANGUAGE_VARIABLE          L"PlatformLang"
#define UEFI_LANGUAGE_CODES_VARIABLE    L"PlatformLangCodes"

//
// Limited buffer size recommended by RFC4646 (4.3.  Length Considerations)
// (42 characters plus a NULL terminator)
//
#define RFC_3066_ENTRY_SIZE             (42 + 1)
#define ISO_639_2_ENTRY_SIZE            3

#define INVALID_VARSTORE_ID             0

#define QUESTION_FLAGS              (EFI_IFR_FLAG_READ_ONLY | EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED | EFI_IFR_FLAG_OPTIONS_ONLY)
#define QUESTION_FLAGS_MASK         (~QUESTION_FLAGS)

extern EFI_GUID                  mIfrVendorGuid;
extern EFI_HII_DATABASE_PROTOCOL *gIfrLibHiiDatabase;
extern EFI_HII_STRING_PROTOCOL   *gIfrLibHiiString;

#pragma pack(1)
typedef struct {
  EFI_STRING_ID       StringToken;
  EFI_IFR_TYPE_VALUE  Value;
  UINT8               Flags;
} IFR_OPTION;
#pragma pack()

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  UINT32                         Reserved;
  UINT64                         UniqueId;
} HII_VENDOR_DEVICE_PATH_NODE;
#pragma pack()

typedef struct {
  HII_VENDOR_DEVICE_PATH_NODE    Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

typedef struct {
  //
  // Buffer size allocated for Data.
  //
  UINT32                BufferSize;

  //
  // Offset in Data to append the newly created opcode binary.
  // It will be adjusted automatically in Create***OpCode(), and should be
  // initialized to 0 before invocation of a serial of Create***OpCode()
  //
  UINT32                Offset;

  //
  // The destination buffer for created op-codes
  //
  UINT8                 *Data;
} EFI_HII_UPDATE_DATA;

/**
 This function locate Hii relative protocols for later usage.

 @param None

 @retval None.

**/
VOID
LocateHiiProtocols (
  VOID
  );

EFI_STATUS
CreateSuppressIfCode (
  IN OUT EFI_HII_UPDATE_DATA *Data
  );
  
EFI_STATUS
CreateIdEqualCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINT16               Value,
  IN OUT EFI_HII_UPDATE_DATA *Data
  );  
  
EFI_STATUS
CreateIdEqualListCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINT8                OptionNum,
  IN     UINT16               *Value,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  ); 
  
EFI_STATUS
CreateOneOfOptionOpCodeEx (
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     UINT8                Type,
  IN     EFI_QUESTION_ID      QuestionId2,
  IN     UINTN                OptionCount2,
  IN     UINT8                *PrimaryTypeList,
  IN     UINT8                *MapList,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );
  
EFI_STATUS
CreateOneOfOpCodeEx (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_VARSTORE_ID      VarStoreId,
  IN     UINT16               VarOffset,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     UINT8                OneOfFlags,
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     EFI_QUESTION_ID      QuestionId2,
  IN     UINTN                OptionCount2,
  IN     UINT8                *PrimaryTypeList,
  IN     UINT8                *MapList,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

//
// Exported Library functions
//
/**
 Create EFI_IFR_END_OP opcode.

 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateEndOpCode (
  IN OUT EFI_HII_UPDATE_DATA *Data
  );

/**
 Create EFI_IFR_DEFAULT_OP opcode.

 @param [in]   Value            Value for the default
 @param [in]   Type             Type for the default
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateDefaultOpCode (
  IN     EFI_IFR_TYPE_VALUE  *Value,
  IN     UINT8               Type,
  IN OUT EFI_HII_UPDATE_DATA *Data
  );

/**
 Create EFI_IFR_ACTION_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   QuestionConfig   String ID for configuration
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateActionOpCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     EFI_STRING_ID        QuestionConfig,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create EFI_IFR_SUBTITLE_OP opcode.

 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   Flags            Subtitle opcode flags
 @param [in]   Scope            Subtitle Scope bit
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateSubTitleOpCode (
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               Flags,
  IN      UINT8               Scope,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create EFI_IFR_TEXT_OP opcode.

 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   TextTwo          String ID for text two
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateTextOpCode (
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      EFI_STRING_ID       TextTwo,
  IN OUT  EFI_HII_UPDATE_DATA *Data
  );

/**
 Create EFI_IFR_REF_OP opcode.

 @param [in]   FormId           Destination Form ID
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   QuestionId       Question ID
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateGotoOpCode (
  IN      EFI_FORM_ID         FormId,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      EFI_QUESTION_ID     QuestionId,
  IN OUT  EFI_HII_UPDATE_DATA *Data
  );

EFI_STATUS
CreateOneOfOptionOpCode (
  IN     UINTN                OptionCount,
  IN     IFR_OPTION           *OptionsList,
  IN     UINT8                Type,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  )
;

/**
 Create EFI_IFR_ONE_OF_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   VarStoreId       Storage ID
 @param [in]   VarOffset        Offset in Storage
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   OneOfFlags       Flags for oneof opcode
 @param [in]   OptionsList      List of options
 @param [in]   OptionCount      Number of options in option list
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateOneOfOpCode (
  IN     EFI_QUESTION_ID      QuestionId,
  IN     EFI_VARSTORE_ID      VarStoreId,
  IN     UINT16               VarOffset,
  IN     EFI_STRING_ID        Prompt,
  IN     EFI_STRING_ID        Help,
  IN     UINT8                QuestionFlags,
  IN     UINT8                OneOfFlags,
  IN     IFR_OPTION           *OptionsList,
  IN     UINTN                OptionCount,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create EFI_IFR_ORDERED_LIST_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   VarStoreId       Storage ID
 @param [in]   VarOffset        Offset in Storage
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   Flags            Flags for ordered list opcode
 @param [in]   DataType         Type for option value
 @param [in]   MaxContainers    Maximum count for options in this ordered list
 @param [in]   OptionsList      List of options
 @param [in]   OptionCount      Number of options in option list
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateOrderedListOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               Flags,
  IN      UINT8               DataType,
  IN      UINT8               MaxContainers,
  IN      IFR_OPTION          *OptionsList,
  IN     UINTN                OptionCount,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create EFI_IFR_CHECKBOX_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   VarStoreId       Storage ID
 @param [in]   VarOffset        Offset in Storage
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   CheckBoxFlags    Flags for checkbox opcode
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateCheckBoxOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               CheckBoxFlags,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create EFI_IFR_NUMERIC_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   VarStoreId       Storage ID
 @param [in]   VarOffset        Offset in Storage
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   NumericFlags     Flags for numeric opcode
 @param [in]   Minimum          Numeric minimum value
 @param [in]   Maximum          Numeric maximum value
 @param [in]   Step             Numeric step for edit
 @param [in]   Default          Numeric default value
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateNumericOpCode (
  IN     EFI_QUESTION_ID     QuestionId,
  IN     EFI_VARSTORE_ID     VarStoreId,
  IN     UINT16              VarOffset,
  IN     EFI_STRING_ID       Prompt,
  IN     EFI_STRING_ID       Help,
  IN     UINT8               QuestionFlags,
  IN     UINT8               NumericFlags,
  IN     UINT64              Minimum,
  IN     UINT64              Maximum,
  IN     UINT64              Step,
  IN     UINT64              Default,
  IN OUT EFI_HII_UPDATE_DATA *Data
  );

/**
 Create EFI_IFR_STRING_OP opcode.

 @param [in]   QuestionId       Question ID
 @param [in]   VarStoreId       Storage ID
 @param [in]   VarOffset        Offset in Storage
 @param [in]   Prompt           String ID for Prompt
 @param [in]   Help             String ID for Help
 @param [in]   QuestionFlags    Flags in Question Header
 @param [in]   StringFlags      Flags for string opcode
 @param [in]   MinSize          String minimum length
 @param [in]   MaxSize          String maximum length
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateStringOpCode (
  IN      EFI_QUESTION_ID     QuestionId,
  IN      EFI_VARSTORE_ID     VarStoreId,
  IN      UINT16              VarOffset,
  IN      EFI_STRING_ID       Prompt,
  IN      EFI_STRING_ID       Help,
  IN      UINT8               QuestionFlags,
  IN      UINT8               StringFlags,
  IN      UINT8               MinSize,
  IN      UINT8               MaxSize,
  IN OUT EFI_HII_UPDATE_DATA  *Data
  );

/**
 Create GUIDed opcode for banner.

 @param [in]   Title            String ID for title
 @param [in]   LineNumber       Line number for this banner
 @param [in]   Alignment        Alignment for this banner, left, center or right
 @param [in, out] Data          Destination for the created opcode binary

 @retval EFI_SUCCESS            Opcode create success

**/
EFI_STATUS
CreateBannerOpCode (
  IN      EFI_STRING_ID       Title,
  IN      UINT16              LineNumber,
  IN      UINT8               Alignment,
  IN OUT  EFI_HII_UPDATE_DATA *Data
  );

/**
 Assemble EFI_HII_PACKAGE_LIST according to the passed in packages.

 @param [in]   NumberOfPackages  Number of packages.
 @param [in]   GuidId           Package GUID.
 @param        ...

 @return Pointer of EFI_HII_PACKAGE_LIST_HEADER.

**/
EFI_HII_PACKAGE_LIST_HEADER *
PreparePackageList (
  IN UINTN                    NumberOfPackages,
  IN EFI_GUID                 *GuidId,
  ...
  );

/**
 The HII driver handle passed in for HiiDatabase.NewPackageList() requires
 that there should be DevicePath Protocol installed on it.
 This routine create a virtual Driver Handle by installing a vendor device
 path on it, so as to use it to invoke HiiDatabase.NewPackageList().

 @param [out]   DriverHandle    Handle to be returned

 @retval EFI_SUCCESS            Handle destroy success.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  );

/**
 Destroy the Driver Handle created by CreateHiiDriverHandle().

 @param [in]   DriverHandle    Handle returned by CreateHiiDriverHandle()
 
 @retval EFI_SUCCESS            Handle destroy success.
 @return other        Handle destroy fail.

**/
EFI_STATUS
DestroyHiiDriverHandle (
  IN EFI_HANDLE                 DriverHandle
  );

/**
 Find HII Handle associated with given Device Path.

 @param [in]   HiiDatabase      Point to EFI_HII_DATABASE_PROTOCOL instance.
 @param [in]   DevicePath       Device Path associated with the HII package list handle.

 @retval Handle                 HII package list Handle associated with the Device Path.
 @retval NULL                   Hii Package list handle is not found.

**/
EFI_HII_HANDLE
DevicePathToHiiHandle (
  IN EFI_HII_DATABASE_PROTOCOL  *HiiDatabase,
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

/**
 Configure the buffer accrording to ConfigBody strings.

 @param [in]   Buffer           the start address of buffer.
 @param [in]   BufferSize       the size of buffer.
 @param        Number           the number of the strings.
 @param        ...

 @retval EFI_BUFFER_TOO_SMALL   the BufferSize is too small to operate.
 @retval EFI_INVALID_PARAMETER  Buffer is NULL or BufferSize is 0.
 @retval EFI_SUCCESS            Operation successful.

**/
EFI_STATUS
ExtractDefault(
  IN VOID                         *Buffer,
  IN UINTN                        *BufferSize,
  UINTN                           Number,
  ...
  );

/**
 Construct <ConfigAltResp> for a buffer storage.

 @param [in]   ConfigRequest    The Config request string. If set to NULL, all the
                                configurable elements will be extracted from BlockNameArray.
 @param [out]  Progress         On return, points to a character in the Request.
 @param [out]  ConfigAltResp    The returned <ConfigAltResp>.
 @param [in]   Guid             GUID of the buffer storage.
 @param [in]   Name             Name of the buffer storage.
 @param [in]   DriverHandle     The DriverHandle which is used to invoke HiiDatabase
                                protocol interface NewPackageList().
 @param [in]   BufferStorage    Content of the buffer storage.
 @param [in]   BufferStorageSize  Length in bytes of the buffer storage.
 @param [in]   BlockNameArray   Array generated by VFR compiler.
 @param [in]   NumberAltCfg     Number of Default value array generated by VFR compiler.
                                The sequential input parameters will be number of
                                AltCfgId and DefaultValueArray pairs. When set to 0,
                                there will be no <AltResp>.
 @param        ...

 @retval EFI_OUT_OF_RESOURCES   Run out of memory resource.
 @retval EFI_INVALID_PARAMETER  ConfigAltResp is NULL.
 @retval EFI_SUCCESS            Operation successful.

**/
EFI_STATUS
ConstructConfigAltResp (
  IN  EFI_STRING                  ConfigRequest,  OPTIONAL
  OUT EFI_STRING                  *Progress,
  OUT EFI_STRING                  *ConfigAltResp,
  IN  EFI_GUID                    *Guid,
  IN  CHAR16                      *Name,
  IN  EFI_HANDLE                  *DriverHandle,
  IN  VOID                        *BufferStorage,
  IN  UINTN                       BufferStorageSize,
  IN  VOID                        *BlockNameArray, OPTIONAL
  IN  UINTN                       NumberAltCfg,
  ...
//IN  UINT16                      AltCfgId,
//IN  VOID                        *DefaultValueArray,
  );

/**
 Extract Hii package list GUID for given HII handle.

 @param [in]   Handle           Hii handle
 @param [out]  Guid             Package list GUID

 @retval EFI_SUCCESS            Successfully extract GUID from Hii database.

**/
EFI_STATUS
ExtractGuidFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     EFI_GUID            *Guid
  );

/**
 Extract formset class for given HII handle.

 @param [in]   Handle           Hii handle
 @param [out]  Class            Class of the formset
 @param [out]  FormSetTitle     Formset title string
 @param [out]  FormSetHelp      Formset help string

 @retval EFI_SUCCESS            Successfully extract Class for specified Hii handle.

**/
EFI_STATUS
ExtractClassFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     UINT16              *Class,
  OUT     EFI_STRING_ID       *FormSetTitle,
  OUT     EFI_STRING_ID       *FormSetHelp
  );

/**
 Extract formset ClassGuid for given HII handle.

 @param [in]   Handle           Hii handle
 @param [out]  NumberOfClassGuid  Number of ClassGuid
 @param [out]  ClassGuid        Pointer to callee allocated buffer, an array of ClassGuid
 @param [out]  FormSetTitle     Formset title string
 @param [out]  FormSetHelp      Formset help string

 @retval EFI_SUCCESS            Successfully extract Class for specified Hii handle.

**/
EFI_STATUS
ExtractClassGuidFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     UINT8               *NumberOfClassGuid,
  OUT     EFI_GUID            **ClassGuid,
  OUT     EFI_STRING_ID       *FormSetTitle,
  OUT     EFI_STRING_ID       *FormSetHelp
  );

/**
 Converts the unicode character from uppercase to lowercase.

 @param [in, out] Str           String to be converted


**/
VOID
ToLower (
  IN OUT CHAR16    *Str
  );

/**
 Converts binary buffer to Unicode string in reversed byte order to BufToHexString().

 @param [in] Str                String for output
 @param [in] Buffer             Binary buffer.
 @param [in] BufferSize         Size of the buffer in bytes.
 
 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
BufferToHexString (
  IN OUT CHAR16    *Str,
  IN UINT8         *Buffer,
  IN UINTN         BufferSize
  );

/**
 Converts Hex String to binary buffer in reversed byte order to HexStringToBuf().

 @param [in, out] Buffer        Pointer to buffer that receives the data.
 @param [in, out] BufferSize    Length in bytes of the buffer to hold converted data.
                                If routine return with EFI_SUCCESS, containing length of converted data.
                                If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
 @param [in] Str                String to be converted from.
 
 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
HexStringToBuffer (
  IN OUT UINT8         *Buffer,
  IN OUT UINTN         *BufferSize,
  IN CHAR16            *Str
  );

/**
 Convert binary representation Config string (e.g. "0041004200430044") to the
 original string (e.g. "ABCD"). Config string appears in <ConfigHdr> (i.e.
 "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

 @param [in, out] UnicodeString      Original Unicode string.
 @param [in, out] StrBufferLen       On input: Length in bytes of buffer to hold the Unicode string.
                                      Includes tailing '\0' character.
                                      On output:
                                         If return EFI_SUCCESS, containing length of Unicode string buffer.
                                         If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
 @param [in] ConfigString      Binary representation of Unicode String, <string> := (<HexCh>4)+
 
 @retval EFI_SUCCESS            Routine success.
 @retval EFI_BUFFER_TOO_SMALL   The string buffer is too small.

**/
EFI_STATUS
ConfigStringToUnicode (
  IN OUT CHAR16                *UnicodeString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *ConfigString
  );

/**
 Convert Unicode string to binary representation Config string, e.g.
 "ABCD" => "0041004200430044". Config string appears in <ConfigHdr> (i.e.
 "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

 @param [in, out] ConfigString  Binary representation of Unicode String, <string> := (<HexCh>4)+
 @param [in, out] StrBufferLen  On input: Length in bytes of buffer to hold the Unicode string.
                                Includes tailing '\0' character.
                                On output:
                                  If return EFI_SUCCESS, containing length of Unicode string buffer.
                                  If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
 @param [in] UnicodeString      Original Unicode string.
 
 @retval EFI_SUCCESS            Routine success.
 @retval EFI_BUFFER_TOO_SMALL   The string buffer is too small.

**/
EFI_STATUS
UnicodeToConfigString (
  IN OUT CHAR16                *ConfigString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *UnicodeString
  );

/**
 Construct <ConfigHdr> using routing information GUID/NAME/PATH.

 @param [in, out] ConfigHdr     Pointer to the ConfigHdr string.
 @param [in, out] StrBufferLen  On input: Length in bytes of buffer to hold the ConfigHdr string. Includes tailing '\0' character.
                                On output:
                                If return EFI_SUCCESS, containing length of ConfigHdr string buffer.
                                If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
 @param [in]   Guid             Routing information: GUID.
 @param [in]   Name             Routing information: NAME.
 @param [in]   DriverHandle     Driver handle which contains the routing information: PATH.

 @retval EFI_SUCCESS            Routine success.
 @retval EFI_BUFFER_TOO_SMALL   The ConfigHdr string buffer is too small.

**/
EFI_STATUS
ConstructConfigHdr (
  IN OUT CHAR16                *ConfigHdr,
  IN OUT UINTN                 *StrBufferLen,
  IN EFI_GUID                  *Guid,
  IN CHAR16                    *Name, OPTIONAL
  IN EFI_HANDLE                *DriverHandle
  );

/**
 Determines if the Routing data (Guid and Name) is correct in <ConfigHdr>.

 @param [in]   ConfigString     Either <ConfigRequest> or <ConfigResp>.
 @param [in]   StorageGuid      GUID of the storage.
 @param [in]   StorageName      Name of the stoarge.
 
 @retval TRUE                   Routing information is correct in ConfigString.
 @retval FALSE                  Routing information is incorrect in ConfigString.

**/
BOOLEAN
IsConfigHdrMatch (
  IN EFI_STRING                ConfigString,
  IN EFI_GUID                  *StorageGuid, OPTIONAL
  IN CHAR16                    *StorageName  OPTIONAL
  );

/**
 Search BlockName "&OFFSET=Offset&WIDTH=Width" in a string.

 @param [in, out] String        The string to be searched in.
 @param        Offset           Offset in BlockName.
 @param        Width            Width in BlockName.

 @retval TRUE                   Block name found.
 @retval FALSE                  Block name not found.

**/
BOOLEAN
FindBlockName (
  IN OUT CHAR16                *String,
  UINTN                        Offset,
  UINTN                        Width
  );

/**
 This routine is invoked by ConfigAccess.Callback() to retrived uncommitted data from Form Browser.

 @param    VariableGuid         An optional field to indicate the target variable GUID name to use.
 @param    VariableName         An optional field to indicate the target human-readable variable name.
 @param    BufferSize           On input: Length in bytes of buffer to hold retrived data.
                                On output:
                                  If return EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
 @param    Buffer               Buffer to hold retrived data.
 
 @retval EFI_SUCCESS            Routine success.
 @retval EFI_BUFFER_TOO_SMALL   The intput buffer is too small.

**/
EFI_STATUS
GetBrowserData (
  EFI_GUID                   *VariableGuid, OPTIONAL
  CHAR16                     *VariableName, OPTIONAL
  UINTN                      *BufferSize,
  UINT8                      *Buffer
  );

/**
 Determines the handles that are currently active in the database.
 It's the caller's responsibility to free handle buffer.

 @param [in, out] HandleBufferLength  On input, a pointer to the length of the handle buffer. On output,
                                the length of the handle buffer that is required for the handles found.
 @param [out]  HiiHandleBuffer  Pointer to an array of Hii Handles returned.

 @retval EFI_SUCCESS            Get an array of Hii Handles successfully.
 @retval EFI_INVALID_PARAMETER  Hii is NULL.
 @retval EFI_NOT_FOUND          Database not found.

**/
EFI_STATUS
GetHiiHandles (
  IN OUT UINTN                     *HandleBufferLength,
  OUT    EFI_HII_HANDLE            **HiiHandleBuffer
  );

/**
 This routine is invoked by ConfigAccess.Callback() to update uncommitted data of Form Browser.

 @param    VariableGuid         An optional field to indicate the target variable GUID name to use.
 @param    VariableName         An optional field to indicate the target human-readable variable name.
 @param    BufferSize           Length in bytes of buffer to hold retrived data.
 @param    Buffer               Buffer to hold retrived data.
 @param    RequestElement       An optional field to specify which part of the buffer data
                                will be send back to Browser. If NULL, the whole buffer of
                                data will be committed to Browser.
                                <RequestElement> ::= &OFFSET=<Number>&WIDTH=<Number>*
 
 @retval EFI_SUCCESS            Routine success.
 @return Other         Updating Browser uncommitted data failed.

**/
EFI_STATUS
SetBrowserData (
  EFI_GUID                   *VariableGuid, OPTIONAL
  CHAR16                     *VariableName, OPTIONAL
  UINTN                      BufferSize,
  UINT8                      *Buffer,
  CHAR16                     *RequestElement  OPTIONAL
  );

/**
 Convert language code from RFC3066 to ISO639-2.

 @param        LanguageRfc3066  RFC3066 language code.
 @param        LanguageIso639   ISO639-2 language code.

 @retval EFI_SUCCESS            Language code converted.
 @retval EFI_NOT_FOUND          Language code not found.

**/
EFI_STATUS
ConvertRfc3066LanguageToIso639Language (
  CHAR8   *LanguageRfc3066,
  CHAR8   *LanguageIso639
  );

/**
 Convert language code list from RFC3066 to ISO639-2, e.g. "en-US;fr-FR" will
 be converted to "engfra".

 @param        SupportedLanguages  The RFC3066 language list.

 @return The ISO6392 language list.

**/
CHAR8 *
Rfc3066ToIso639 (
  CHAR8  *SupportedLanguages
  );

/**
 Determine what is the current language setting

 @param [out]  Lang             Pointer of system language

 @return Status code

**/
EFI_STATUS
GetCurrentLanguage (
  OUT     CHAR8               *Lang
  );

/**
 Get next language from language code list.

 @param [in, out] LangCode      The language code.
 @param [out]  Lang             Returned language.

 @retval None.

**/
VOID
GetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  );

/**
 This function returns the list of supported languages, in the format specified
 in UEFI specification Appendix M.

 @param [in]   HiiHandle        The HII package list handle.

 @return The supported languages.

**/
CHAR8 *
GetSupportedLanguages (
  IN EFI_HII_HANDLE           HiiHandle
  );

/**
 This function returns the number of supported languages

 @param [in]   HiiHandle        The HII package list handle.

 @retval The                    number of supported languages.

**/
UINT16
GetSupportedLanguageNumber (
  IN EFI_HII_HANDLE           HiiHandle
  );

/**
 Get string specified by StringId form the HiiHandle.

 @param [in]   HiiHandle        The HII handle of package list.
 @param [in]   StringId         The String ID.
 @param [out]  String           The output string.

 @retval EFI_NOT_FOUND          String is not found.
 @retval EFI_SUCCESS            Operation is successful.
 @retval EFI_OUT_OF_RESOURCES   There is not enought memory in the system.
 @retval EFI_INVALID_PARAMETER  The String is NULL.

**/
EFI_STATUS
GetStringFromHandle (
  IN  EFI_HII_HANDLE                  HiiHandle,
  IN  EFI_STRING_ID                   StringId,
  OUT EFI_STRING                      *String
  );

/**
 Get the string given the StringId and String package Producer's Guid.

 @param [in]   ProducerGuid     The Guid of String package list.
 @param [in]   StringId         The String ID.
 @param [out]  String           The output string.

 @retval EFI_NOT_FOUND          String is not found.
 @retval EFI_SUCCESS            Operation is successful.
 @retval EFI_OUT_OF_RESOURCES   There is not enought memory in the system.

**/
EFI_STATUS
GetStringFromToken (
  IN  EFI_GUID                        *ProducerGuid,
  IN  EFI_STRING_ID                   StringId,
  OUT EFI_STRING                      *String
  );

/**
 This function adds the string into String Package of each language.

 @param [in]   PackageList      Handle of the package list where this string will be added.
 @param [out]  StringId         On return, contains the new strings id, which is unique within PackageList.
 @param [in]   String           Points to the new null-terminated string.

 @retval EFI_SUCCESS            The new string was added successfully.
 @retval EFI_NOT_FOUND          The specified PackageList could not be found in database.
 @retval EFI_OUT_OF_RESOURCES   Could not add the string due to lack of resources.
 @retval EFI_INVALID_PARAMETER  String is NULL or StringId is NULL is NULL.

**/
EFI_STATUS
IfrLibNewString (
  IN  EFI_HII_HANDLE                  PackageList,
  OUT EFI_STRING_ID                   *StringId,
  IN  CONST EFI_STRING                String
  );

/**
 This function try to retrieve string from String package of current language.
 If fail, it try to retrieve string from String package of first language it support.

 @param [in]   PackageList      The package list in the HII database to search for the specified string.
 @param [in]   StringId         The string's id, which is unique within PackageList.
 @param [out]  String           Points to the new null-terminated string.
 @param [in, out] StringSize    On entry, points to the size of the buffer pointed to by String, in bytes. On return,
                                points to the length of the string, in bytes.

 @retval EFI_SUCCESS            The string was returned successfully.
 @retval EFI_NOT_FOUND          The string specified by StringId is not available.
 @retval EFI_BUFFER_TOO_SMALL   The buffer specified by StringLength is too small to hold the string.
 @retval EFI_INVALID_PARAMETER  The String or StringSize was NULL.

**/
EFI_STATUS
IfrLibGetString (
  IN  EFI_HII_HANDLE                  PackageList,
  IN  EFI_STRING_ID                   StringId,
  OUT EFI_STRING                      String,
  IN  OUT UINTN                       *StringSize
  );

/**
 This function updates the string in String package of current language.

 @param [in]   PackageList      The package list containing the strings.
 @param [in]   StringId         The string's id, which is unique within PackageList.
 @param [in]   String           Points to the new null-terminated string.

 @retval EFI_SUCCESS            The string was updated successfully.
 @retval EFI_NOT_FOUND          The string specified by StringId is not in the database.
 @retval EFI_INVALID_PARAMETER  The String was NULL.
 @retval EFI_OUT_OF_RESOURCES   The system is out of resources to accomplish the task.

**/
EFI_STATUS
IfrLibSetString (
  IN EFI_HII_HANDLE                   PackageList,
  IN EFI_STRING_ID                    StringId,
  IN CONST EFI_STRING                 String
  );

/**
 Draw a dialog and return the selected key.

 @param [in]   NumberOfLines    The number of lines for the dialog box
 @param [out]  KeyValue         The EFI_KEY value returned if HotKey is TRUE..
 @param [in]   String           Pointer to the first string in the list
 @param        ...              A series of (quantity == NumberOfLines) text strings which
                                will be used to construct the dialog box

 @retval EFI_SUCCESS            Displayed dialog and received user interaction
 @retval EFI_INVALID_PARAMETER  One of the parameters was invalid.

**/
EFI_STATUS
IfrLibCreatePopUp (
  IN  UINTN                       NumberOfLines,
  OUT EFI_INPUT_KEY               *KeyValue,
  IN  CHAR16                      *String,
  ...
  );

/**
 This function initialize the data structure for dynamic opcode.

 @param [in, out] UpdateData    The adding data;
 @param [in]   BufferSize       Length of the buffer to fill dynamic opcodes.

 @retval EFI_SUCCESS            Update data is initialized.
 @retval EFI_INVALID_PARAMETER  UpdateData is NULL.
 @retval EFI_OUT_OF_RESOURCES   No enough memory to allocate.

**/
EFI_STATUS
IfrLibInitUpdateData (
  IN OUT EFI_HII_UPDATE_DATA   *UpdateData,
  IN UINT32                    BufferSize
  );

/**
 This function free the resource of update data.

 @param [in]   UpdateData       The adding data;

 @retval EFI_SUCCESS            Resource in UpdateData is released.
 @retval EFI_INVALID_PARAMETER  UpdateData is NULL.

**/
EFI_STATUS
IfrLibFreeUpdateData (
  IN EFI_HII_UPDATE_DATA       *UpdateData
  );

/**
 This function allows the caller to update a form that has
 previously been registered with the EFI HII database.

 @param [in]   Handle           Hii Handle
 @param [in]   FormSetGuid      The formset should be updated.
 @param [in]   FormId           The form should be updated.
 @param [in]   Label            Update information starting immediately after this label in the IFR
 @param [in]   Insert           If TRUE and Data is not NULL, insert data after Label.
                                If FALSE, replace opcodes between two labels with Data.
 @param [in]   Data             The adding data; If NULL, remove opcodes between two Label.

 @retval EFI_SUCCESS            Update success.
 @return Other         Update fail.

**/
EFI_STATUS
IfrLibUpdateForm (
  IN EFI_HII_HANDLE            Handle,
  IN EFI_GUID                  *FormSetGuid, OPTIONAL
  IN EFI_FORM_ID               FormId,
  IN UINT16                    Label,
  IN BOOLEAN                   Insert,
  IN EFI_HII_UPDATE_DATA       *Data
  );
#endif
