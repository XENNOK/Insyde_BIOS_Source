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

  H2OIpmiSensor.h

Abstract:

  Header file that contains IPMI Sensor Device Commands, data structures and
  definitions

--*/

#ifndef _H2O_IPMI_SENSOR_H_
#define _H2O_IPMI_SENSOR_H_

#pragma pack(1)

//
// Sensor Device Commands
//
#define H2O_IPMI_CMD_GET_DEVICE_SDR_INFO              0x20
#define H2O_IPMI_CMD_GET_DEVICE_SDR                   0x21
#define H2O_IPMI_CMD_RESERVE_DEVICE_SDR_REPOSITORY    0x22
#define H2O_IPMI_CMD_GET_SENSOR_READING_FACTORS       0x23
#define H2O_IPMI_CMD_SET_SENSOR_HYSTERESIS            0x24
#define H2O_IPMI_CMD_GET_SENSOR_HYSTERESIS            0x25
#define H2O_IPMI_CMD_SET_SENSOR_THRESHOLD             0x26
#define H2O_IPMI_CMD_GET_SENSOR_THRESHOLD             0x27
#define H2O_IPMI_CMD_SET_SENSOR_EVENT_ENABLE          0x28
#define H2O_IPMI_CMD_GET_SENSOR_EVENT_ENABLE          0x29
#define H2O_IPMI_CMD_REARM_SENSOR_EVENT               0x2A
#define H2O_IPMI_CMD_GET_SENSOR_EVENT_STATUS          0x2B
#define H2O_IPMI_CMD_GET_SENSOR_READING               0x2D
#define H2O_IPMI_CMD_SET_SENSOR_TYPE                  0x2E
#define H2O_IPMI_CMD_GET_SENSOR_TYPE                  0x2F
#define H2O_IPMI_CMD_SET_SENSOR_READING_EVENT_STATUS  0x30

//
// Get Sensor Reading
//
typedef struct {
  UINT8 SensorReading;
  UINT8 Reserved           :5;
  UINT8 ReadingUnavailable :1;
  UINT8 ScanDisable        :1;
  UINT8 AllEventMsgDisable :1;
  UINT8 Data[2];
} H2O_IPMI_GET_SENSOR_READING;

typedef struct {
  UINT8  Count;
  UINT8  Flag;
  UINT8  PopChangeIndicator[4];
} H2O_IPMI_GET_DEVICE_SDR_INFO;


#pragma pack()

#endif
