//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*--
Module Name:

  H2OIpmiSdrProtocol.h

Abstract:

  H2O IPMI SDR Protocol definition file

--*/

#ifndef _H2O_IPMI_SDR_PROTOCOL_H_
#define _H2O_IPMI_SDR_PROTOCOL_H_


#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiInterfaceProtocol)



//
// Common Protocol Definitions
//
#define SDR_ID_ALL                         0xFFFF
#define SDR_STRING_LENGTH                  0x20

//
// Flags for display
//
#define DISPLAY_LONG_UNIT                  0x0001

//
#define TYPE_LENGTH_BYTE_UNICODE    0x00
#define TYPE_LENGTH_BYTE_BCD_PLUS   0x01
#define TYPE_LENGTH_BYTE_6BIT_ASCII 0x02
#define TYPE_LENGTH_BYTE_8BIT_ASCII 0x03



#pragma pack(1)

//
// Common structure to pass SDR data
//
typedef struct {
  UINT16  RecordId;
  CHAR16  Name[SDR_STRING_LENGTH];
  UINT16  DataStatus;
  UINT64  Value;
  CHAR16  ValueStr[SDR_STRING_LENGTH];
  CHAR16  Unit[SDR_STRING_LENGTH];
  UINT64  LowNonCriticalThres;
  CHAR16  LowNonCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  LowCriticalThres;
  CHAR16  LowCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  LowNonRecoverThres;
  CHAR16  LowNonRecoverThresStr[SDR_STRING_LENGTH];
  UINT64  UpNonCriticalThres;
  CHAR16  UpNonCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  UpCriticalThres;
  CHAR16  UpCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  UpNonRecoverThres;
  CHAR16  UpNonRecoverThresStr[SDR_STRING_LENGTH];
  UINT8   SensorNumber;
} SDR_DATA_STURCT;

#pragma pack()



EFI_FORWARD_DECLARATION (H2O_IPMI_SDR_PROTOCOL);



//
// Protocol Guid
//
#define H2O_IPMI_SDR_PROTOCOL_GUID \
  { 0x80b57671, 0xaf44, 0x425f, 0x98, 0x8c, 0x53, 0xd1, 0xa4, 0xf8, 0x0f, 0x7b}



typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_GET_SDR_DATA) (
  IN  H2O_IPMI_SDR_PROTOCOL       *This,
  IN  UINT16                      RecordId,
  IN  UINT16                      Flag,
  OUT UINT16                      *Count,
  OUT SDR_DATA_STURCT             **Record
);
/*++

Routine Description:

  Get sensor data from "SDR Repository Device" or "Device SDR".

Arguments:

  This       - Pointer to H2O_IPMI_SDR_PROTOCOL instance

  RecordId   - Speficied ID of SDR that want to get. 0xFFFF means getting
               all recoreds.

  Flag       - Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                         return full unit string in SDR_DATA_STURCT. Otherwise
                         it will return short unit string by default.
               High byte: decimal number in all FLOAT string. Maximum is 10 digital.

  Count      - Number of records if user passes 0xFFFF in RecordId.
   
  Record     - A buffer to SDR_DATA_STURCT. It is caller's responsibility
               to free allocated memory.

Returns:

  EFI_SUCCESS            - Get SDR data success

  EFI_UNSUPPORTED        - Speificed Id does not exist or cannot get SDR data

  EFI_INVALID_PARAMETER  - 1. This is NULL
                           2. Count is NULL
                           3. Record is NULL
  
--*/

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_GET_SDR_RAW) (
  IN  H2O_IPMI_SDR_PROTOCOL       *This,
  IN  UINT16                      RecordId,
  OUT H2O_IPMI_SDR_RAW            **Record
);
/*++

Routine Description:

  Get sensor data from "SDR Repository Device" or "Device SDR".

Arguments:

  This       - Pointer to H2O_IPMI_SDR_PROTOCOL instance

  RecordId   - Speficied ID of SDR that want to get.
  
  Record     - A buffer to SDR_DATA_STURCT. It is caller's responsibility
               to free allocated memory.

Returns:

  EFI_SUCCESS            - Get SDR data success

  EFI_UNSUPPORTED        - Speificed Id does not exist or cannot get SDR data

  EFI_INVALID_PARAMETER  - 1. This is NULL
                           2. Count is NULL
                           3. Record is NULL
  
--*/


typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_SENSOR_READING_TO_STR) (
  IN  H2O_IPMI_SDR_PROTOCOL       *This,
  IN  UINT8                       SensorNumber,
  IN  UINT8                       SensorReading,
  IN  UINT8                       Precision,
  OUT CHAR16                      *ConvertValueStr
);
/*++

Routine Description:

  Convert analog sensor reading or threshold event data (trigger reading or trigger threshold) value to string.

Arguments:

  This              - Pointer to H2O_IPMI_SDR_PROTOCOL instance

  SensorNumber      - Unique number identifying the sensor.
  
  SensorReading     - A byte sensor reading or event data (contain trigger reading or trigger threshold) value need to convert.

  Precision         - The precision of value to string.
  
  ConvertValueStr   - The point of character string space to carry converted value string. Caller must prepare a big enough
                      CHAR16 buffer to store converted string.

Returns:

  EFI_SUCCESS            - Convert sensor reading value success

  EFI_NOT_READY          - Get sensor data record fail

  EFI_UNSUPPORTED        - Unique sensor number can't find the corresponding SDR or the record type of sensor number is not full type

  EFI_INVALID_PARAMETER  - 1. This is NULL
                           2. ConvertValueStr is NULL
  
--*/


typedef struct _H2O_IPMI_SDR_PROTOCOL {
  H2O_IPMI_SDR_PROTOCOL_GET_SDR_DATA             GetSdrData;
  H2O_IPMI_SDR_PROTOCOL_GET_SDR_RAW              GetSdrRaw;
  H2O_IPMI_SDR_PROTOCOL_SENSOR_READING_TO_STR    SensorReadingToStr;
} H2O_IPMI_SDR_PROTOCOL;

extern EFI_GUID gH2OIpmiSdrProtocolGuid;

#endif


