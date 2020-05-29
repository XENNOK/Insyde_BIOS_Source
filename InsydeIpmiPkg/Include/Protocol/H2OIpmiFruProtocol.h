/** @file
 H2O IPMI FRU Protocol header file.

 H2O IPMI FRU Protocol provides an interface to get/set FRU data followed by FRU
 specification.

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

#ifndef _H2O_IPMI_FRU_PROTOCOL_H_
#define _H2O_IPMI_FRU_PROTOCOL_H_


#include <H2OIpmi.h>


//
// FruAccess Protocol GUID
//
#define H2O_IPMI_FRU_PROTOCOL_GUID \
  {0xe2816bf2, 0x6c03, 0x44c7, 0xa8, 0x6b, 0xfe, 0x44, 0x74, 0x71, 0xf5, 0x1a}

#define H2O_SMM_IPMI_FRU_PROTOCOL_GUID \
  {0xD291BE9E, 0x01EC, 0x4e14, 0xB4, 0x7E, 0x03, 0x59, 0x64, 0xD2, 0x9F, 0xC5}

typedef struct _H2O_IPMI_FRU_PROTOCOL H2O_IPMI_FRU_PROTOCOL;


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
#define FRU_CHASSIS_CUSTOM(X)             (0x0003+(X)|FRU_CHASSIS_INFO_AREA)

#define FRU_BOARD_MANUFACTURER            (0x0001|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_PRODUCT_NAME            (0x0002|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_SERIAL_NUMBER           (0x0003|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_PART_NUMBER             (0x0004|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_FRU_FILE_ID             (0x0005|FRU_BOARD_INFO_AREA)
#define FRU_BOARD_CUSTOM(X)               (0x0006+(X)|FRU_BOARD_INFO_AREA)

#define FRU_PRODUCT_MANUFACTURER_NAME     (0x0001|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_NAME                  (0x0002|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_PART_MODEL_NUMBER     (0x0003|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_VERSION               (0x0004|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_SERIAL_NUMBER         (0x0005|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_ASSET_TAG             (0x0006|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_FRU_FILE_ID           (0x0007|FRU_PRODUCT_INFO_AREA)
#define FRU_PRODUCT_CUSTOM(X)             (0x0008+(X)|FRU_PRODUCT_INFO_AREA)


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
(EFIAPI *READ_FIELD_DATA) (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN OUT UINT8                          *StringBufferSize,
  OUT    UINT8                          *StringBuffer
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
(EFIAPI *WRITE_FIELD_DATA) (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN OUT UINT8                          *StringBufferSize,
  IN     UINT8                          *StringBuffer
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
(EFIAPI *GET_CHASSIS_TYPE) (
  IN     UINT8                          FruId,
  IN OUT UINT8                          *ChassisTypeStrSize,
  OUT    UINT8                          *ChassisType,
  OUT    UINT8                          *ChassisTypeString
  );


/**
 Get EFI_TIME convert from MFG Date / Time field in Board area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[out]        MfgDateTime         The EFI_TIME struct convert from MFG Date / Time field in Board Info Area.

 @retval EFI_SUCCESS                    Get MFG date and time successfully.
 @retval EFI_UNSUPPORTED                Get MFG date and time fail.
*/
typedef
EFI_STATUS
(EFIAPI *GET_MFG_DATE_TIME) (
  IN  UINT8                             FruId,
  OUT EFI_TIME                          *MfgDateTime
  );


/**
 Read data from specified record.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that can store read data.
                                        OUT: Real read data.
 @param[out]        DataBuffer          Buffer to store read data.
 @param[out]        DataHeader          Record header of specified multi record.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
 @retval EFI_SUCCESS                    Get success.
*/
typedef
EFI_STATUS
(EFIAPI *READ_MULTI_RECORD) (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer,
  OUT    VOID                           *DataHeader OPTIONAL
  );


/**
 Write data to specified record.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write;
                                        OUT: Data size that can be written.
 @param[out]        DataBuffer          Buffer to store write data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Record buffer is smaller than write data.
 @retval EFI_SUCCESS                    Get success.
*/
typedef
EFI_STATUS
(EFIAPI *WRITE_MULTI_RECORD) (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer
  );


/**
 Read specified size of raw data from specified offset.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Offset              Offset of FRU that user wants to read from.
 @param[in, out]    DataBufferSize      IN:  Data buffer size. It also means data size that want to read.
                                        OUT: FRU data size that can be read from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
typedef
EFI_STATUS
(EFIAPI *READ_RAW_DATA) (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN OUT UINT16                         *DataBufferSize,
  OUT    VOID                           *DataBuffer
  );


/**
 Write specified size of raw data to specified offset.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Offset              Offset of FRU that user wants to write to.
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write.
                                        OUT: FRU data size that can be written from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
typedef
EFI_STATUS
(EFIAPI *WRITE_RAW_DATA) (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN OUT UINT16                         *DataBufferSize,
  IN     VOID                           *DataBuffer
  );


//
// H2O IpmiFru Protocol
//
struct _H2O_IPMI_FRU_PROTOCOL {
  READ_FIELD_DATA     ReadFieldData;
  WRITE_FIELD_DATA    WriteFieldData;
  GET_CHASSIS_TYPE    GetChassisType;
  GET_MFG_DATE_TIME   GetMfgDateTime;
  READ_MULTI_RECORD   ReadMultiRecord;
  WRITE_MULTI_RECORD  WriteMultiRecord;
  READ_RAW_DATA       ReadRawData;
  WRITE_RAW_DATA      WriteRawData;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gH2OIpmiFruProtocolGuid;
extern EFI_GUID gH2OSmmIpmiFruProtocolGuid;


#endif

