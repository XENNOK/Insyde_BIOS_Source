//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  H2OIpmiFruProtocol.h

Abstract:

  Definition of the H2O Ipmi Fru Protocol.

--*/

#ifndef _H2O_IPMI_FRU_PROTOCOL_H_
#define _H2O_IPMI_FRU_PROTOCOL_H_

//
// Includes
//
#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiInterfaceProtocol)

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (H2O_IPMI_FRU_PROTOCOL);

//
// FruAccess Protocol GUID
//
#define H2O_IPMI_FRU_PROTOCOL_GUID \
  { \
    0xe2816bf2, 0x6c03, 0x44c7, 0xa8, 0x6b, 0xfe, 0x44, 0x74, 0x71, 0xf5, 0x1a \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gH2OIpmiFruProtocolGuid;

//
// Area Definition
//
#define FRU_CHASSIS_INFO_AREA         0x1000
#define FRU_BOARD_INFO_AREA           0x2000
#define FRU_PRODUCT_INFO_AREA         0x4000
#define FRU_SUPPORT_FIELD_END         0xFFFF

//
// Field Definition
//
#define FRU_CHASSIS_PART_NUMBER           (0x0001|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_SERIAL_NUMBER         (0x0002|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM0               (0x0003|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM1               (0x0004|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM2               (0x0005|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM3               (0x0006|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM4               (0x0007|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM5               (0x0008|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM6               (0x0009|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM7               (0x000A|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM8               (0x000B|FRU_CHASSIS_INFO_AREA)
#define FRU_CHASSIS_CUSTOM9               (0x000C|FRU_CHASSIS_INFO_AREA)

#define FRU_BOARD_MANUFACTURER            (0x0001|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_PRODUCT_NAME            (0x0002|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_SERIAL_NUMBER           (0x0003|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_PART_NUMBER             (0x0004|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_FRU_FILE_ID             (0x0005|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM0                 (0x0006|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM1                 (0x0007|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM2                 (0x0008|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM3                 (0x0009|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM4                 (0x000A|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM5                 (0x000B|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM6                 (0x000C|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM7                 (0x000D|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM8                 (0x000E|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM9                 (0x000F|FRU_BOARD_INFO_AREA)

#define FRU_PRODUCT_MANUFACTURER_NAME     (0x0001|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_NAME                  (0x0002|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_PART_MODEL_NUMBER     (0x0003|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_VERSION               (0x0004|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_SERIAL_NUMBER         (0x0005|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_ASSET_TAG             (0x0006|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_FRU_FILE_ID           (0x0007|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM0               (0x0008|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM1               (0x0009|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM2               (0x000A|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM3               (0x000B|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM4               (0x000C|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM5               (0x000D|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM6               (0x000E|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM7               (0x000F|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM8               (0x0010|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM9               (0x0011|FRU_PRODUCT_INFO_AREA)

//
// H2O FruAccess Protocol Function Prototype
//
typedef
EFI_STATUS
(EFIAPI *READ_FIELD_DATA) (
  IN       UINT8  FruId,
  IN       UINT16 Field,
  IN  OUT  UINT8  *StringBufferSize,
  OUT      UINT8  *StringBuffer
  );

typedef
EFI_STATUS
(EFIAPI *WRITE_FIELD_DATA) (
  IN       UINT8  FruId,
  IN       UINT16 Field,
  IN  OUT  UINT8  *StringBufferSize,
  IN       UINT8  *StringBuffer
  );

typedef
EFI_STATUS
(EFIAPI *GET_CHASSIS_TYPE) (
  IN       UINT8  FruId,
  IN OUT   UINT8  *ChassisTypeStringSize,
  OUT      UINT8  *ChassisType,
  OUT      UINT8  *ChassisTypeString
  );

typedef
EFI_STATUS
(EFIAPI *GET_MFG_DATE_TIME) (
  IN       UINT8     FruId,
  OUT      EFI_TIME  *MfgDateTime
  );

typedef
EFI_STATUS
(EFIAPI *READ_MULTI_RECORD) (
  IN       UINT8  FruId,
  IN       UINT8  RecordType,
  IN       UINTN  RecordIndex,
  IN  OUT  UINT8  *DataBufferSize,
  OUT      VOID   *DataBuffer,
  OUT      VOID   *DataHeader   OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *WRITE_MULTI_RECORD) (
  IN       UINT8  FruId,
  IN       UINT8  RecordType,
  IN       UINTN  RecordIndex,
  IN  OUT  UINT8  *DataBufferSize,
  OUT      VOID   *DataBuffer
  );

//
// H2O IpmiFru Protocol
//
typedef struct _H2O_IPMI_FRU_PROTOCOL {
  READ_FIELD_DATA     ReadFieldData;
  WRITE_FIELD_DATA    WriteFieldData;
  GET_CHASSIS_TYPE    GetChassisType;
  GET_MFG_DATE_TIME   GetMfgDateTime;
  READ_MULTI_RECORD   ReadMultiRecord;
  WRITE_MULTI_RECORD  WriteMultiRecord;
} H2O_IPMI_FRU_PROTOCOL;

#endif


