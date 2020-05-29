/** @file
 Multi Config Struct Define header file.

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


#ifndef _MULTI_CONFIG_STRUCT_DEF_H_
#define _MULTI_CONFIG_STRUCT_DEF_H_


#define MULTI_CONFIG_SIGNATURE       SIGNATURE_64('$','M','U','L','T','C','F','G')


//
// Define Config Data Type
//
#define MULTI_CONFIG_TYPE_FULL_SETUP_SETTING_VALUE          0x00
#define MULTI_CONFIG_TYPE_HOTKEY_SETTING_VALUE              0x01

//
// Define Full Setup require kind for GetFullSetupSetting function
//
#define SETUP_FOR_BIOS_POST    1
#define SETUP_FOR_LOAD_DEFAULT 2


//
// Define Config Data Attribute Bit Map
//
#define MULTI_CONFIG_ATTRIBUTE_VALID        0x00000001  ///< 1: Valid   0: Invalid
#define MULTI_CONFIG_ATTRIBUTE_WRITE        0x00000002  ///< 1: R/W     0: Read only
#define MULTI_CONFIG_ATTRIBUTE_SHOW         0x00000004  ///< 1: Show    0: Hide

#define MULTI_CONFIG_ATTRIBUTE_DEFAULT      0x20000000  ///< Default Setup select       1: Multi Config 0: VFR
#define MULTI_CONFIG_ATTRIBUTE_BIOS_POST    0x40000000  ///< Config Data for BIOS POST  1: Non-Active   0: Active
#define MULTI_CONFIG_ATTRIBUTE_ACTIVE       0x80000000  ///< Config Data                1: Non-Active   0: Acvive


#define MULTI_CONFIG_VARIABLE_NAME   L"MultiConfig"

#define MULTI_CONFIG_VARIABLE_GUID \
  { 0xE84DCE29, 0x9256, 0x4e88, 0x8A, 0xDF, 0x0A, 0x0C, 0xEE, 0x50, 0x68, 0xDB }


#pragma pack(1)


typedef struct _CONFIG_HEADER {
  UINT8   Type;             ///< Config Data Type
  UINT8   ConfigId;         ///< Identity Number for each Type
  UINT16  ConfigDataSize;   ///< Config Data Size
  UINT32  ConfigDataOffset; ///< Offset value from Signature
  UINT32  Attribute;        ///< Config Data Attribute
  UINT32  SkuId;            ///< Current Sku Id
  CHAR8   Name[8];          ///< Config Name
  UINT8   Reserved[4];      ///< Reserved
  UINT32  Crc32;            ///< CRC32 value for Config Data
} CONFIG_HEADER;


typedef struct _MULTI_CONFIG_REGION_HEADER {
  UINT64  Signature;        ///< '$','M','U','L','T','C','F','G' $MULTCFG
  UINT16  HeaderLength;     ///< Size of MULTI_CONFIG_REGION_HEADER plus size of CONFIG_HEADER multiply ConfigCount
  UINT16  ConfigCount;      ///< Current Config Count
  UINT32  TotalRegionSize;  ///< FV Region size
  UINT32  SetupStructSize;  ///< Setup Variable Data Structure size
  UINT32  SkuId;            ///< Current Sku Id
  UINT32  Reserved;         
  UINT32  HeaderCrc32;      ///< CRC32 value for MULTI_CONFIG_REGION_HEADER & all CONFIG_HEADER
} MULTI_CONFIG_REGION_HEADER;


typedef struct _MULTI_CONFIG_OPTION {
  UINT8  Active;            ///< Need to Reference Multi Config or not
  UINT8  Default;           ///< Multi Config use for default
  UINT8  ConfigType;        ///< Config Data Type
  UINT8  ConfigId;          ///< Identity Number for each Type
} MULTI_CONFIG_OPTION;


#pragma pack()


#endif

