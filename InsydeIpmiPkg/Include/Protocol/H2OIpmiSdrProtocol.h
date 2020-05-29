/** @file
 H2O IPMI SDR Protocol header file.

 H2O IPMI SDR Protocol provides an interface to retrive SDRs data, including
 translated strings or raw data only.

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

#ifndef _H2O_IPMI_SDR_PROTOCOL_H_
#define _H2O_IPMI_SDR_PROTOCOL_H_


#include <H2OIpmi.h>


//
// Common Protocol Definitions
//
#define SDR_ID_ALL                       0xFFFF
#define SDR_STRING_LENGTH                0x20

//
// Flags for display
//
#define DISPLAY_LONG_UNIT                0x0001

//
// Sensor name encode
//
#define TYPE_LENGTH_BYTE_UNICODE         0x00
#define TYPE_LENGTH_BYTE_BCD_PLUS        0x01
#define TYPE_LENGTH_BYTE_6BIT_ASCII      0x02
#define TYPE_LENGTH_BYTE_8BIT_ASCII      0x03


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
  UINT8   Reserve;
} SDR_DATA_STURCT;

#pragma pack()


//
// Protocol Guid
//
#define H2O_IPMI_SDR_PROTOCOL_GUID \
  {0x80b57671, 0xaf44, 0x425f, 0x98, 0x8c, 0x53, 0xd1, 0xa4, 0xf8, 0x0f, 0x7b}

#define H2O_SMM_IPMI_SDR_PROTOCOL_GUID \
  {0xDE083EA7, 0x7495, 0x4439, 0xBF, 0xB6, 0xC4, 0xF5, 0xDD, 0xB9, 0xA2, 0x6F}

typedef struct _H2O_IPMI_SDR_PROTOCOL H2O_IPMI_SDR_PROTOCOL;


/**
 Get sensor data from "SDR Repository Device" or "Device SDR".

 @param[in]         This                Pointer to H2O_IPMI_SDR_PROTOCOL instance.
 @param[in]         RecordId            Speficied ID of SDR that want to get.
                                        0xFFFF means getting all recoreds.
 @param[in]         Flag                Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                        return full unit string in SDR_DATA_STURCT. Otherwise
                                        it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string. Maximum is 10 digital.
 @param[out]        Count               Number of records if user passes 0xFFFF in RecordId.
 @param[out]        Record              A buffer to SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Speificed Id does not exist or cannot get SDR data.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. Count is NULL
                                        3. Record is NULL
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_GET_SDR_DATA) (
  IN  H2O_IPMI_SDR_PROTOCOL             *This,
  IN  UINT16                            RecordId,
  IN  UINT16                            Flag,
  OUT UINT16                            *Count,
  OUT SDR_DATA_STURCT                   **Record
);


/**
 Get sensor data from "SDR Repository Device" or "Device SDR".

 @param[in]         This                Pointer to H2O_IPMI_SDR_PROTOCOL instance.
 @param[in]         RecordId            Speficied ID of SDR that want to get.
 @param[out]        Record              A buffer to SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Speificed Id does not exist or cannot get SDR data.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. Count is NULL
                                        3. Record is NULL
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_GET_SDR_RAW) (
  IN  H2O_IPMI_SDR_PROTOCOL             *This,
  IN  UINT16                            RecordId,
  OUT H2O_IPMI_SDR_RAW                  **Record
);


/**
 Convert analog sensor reading or threshold event data (trigger reading or trigger threshold) value to string.

 @param[in]         This                A pointer to H2O_IPMI_SDR_PROTOCOL structure.
 @param[in]         SensorNumber        Unique number identifying the sensor.
 @param[in]         SensorReading       A byte sensor reading or event data (contain trigger reading or
                                        trigger threshold) value need to convert.
 @param[in]         Precision           The precision of value to string.
 @param[out]        ConvertValueStr     The point of CHAR16 string space to carry converted value string.

 @retval EFI_SUCCESS                    Convert sensor reading value success.
 @retval EFI_NOT_READY                  Get sensor data record fail.
 @retval EFI_UNSUPPORTED                Unique sensor number can't find the corresponding SDR or
                                        the record type of sensor number is not full type.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. ConvertValueStr is NULL
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_SDR_PROTOCOL_SENSOR_READING_TO_STR) (
  IN  H2O_IPMI_SDR_PROTOCOL             *This,
  IN  UINT8                             SensorNumber,
  IN  UINT8                             SensorReading,
  IN  UINT8                             Precision,
  OUT CHAR16                            *ConvertValueStr
);


//
// H2O_IPMI_SDR_PROTOCOL structure
//
struct _H2O_IPMI_SDR_PROTOCOL {
  H2O_IPMI_SDR_PROTOCOL_GET_SDR_DATA             GetSdrData;
  H2O_IPMI_SDR_PROTOCOL_GET_SDR_RAW              GetSdrRaw;
  H2O_IPMI_SDR_PROTOCOL_SENSOR_READING_TO_STR    SensorReadingToStr;
};


extern EFI_GUID gH2OIpmiSdrProtocolGuid;
extern EFI_GUID gH2OSmmIpmiSdrProtocolGuid;


#endif

