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

  H2OIpmiGlobal.h

Abstract:

  Header file that contains IPMI Global Commands, data structures and
  definitions

--*/

#ifndef _H2O_IPMI_GLOBAL_H_
#define _H2O_IPMI_GLOBAL_H_

#pragma pack(1)

//
// IPMI Device Global Command
//
#define H2O_IPMI_CMD_GET_DEVICE_ID          0x01
#define H2O_IPMI_CMD_COLD_RESET             0x02
#define H2O_IPMI_CMD_WARM_RESET             0x03
#define H2O_IPMI_CMD_GET_SELF_TEST_RESULTS  0x04
#define H2O_IPMI_CMD_TEST_ON                0x05
#define H2O_IPMI_CMD_SET_ACPI_POWER_STATE   0x06
#define H2O_IPMI_CMD_GET_ACPI_POWER_STATE   0x07
#define H2O_IPMI_CMD_GET_DEVICE_GUID        0x08

//
// GetSelfTestResult Response Data
//
#define GET_SELF_TEST_RESULTS_NO_ERROR         0x55
#define GET_SELF_TEST_RESULTS_NOT_IMPLEMENT    0x56
#define GET_SELF_TEST_RESULTS_INACCESSIBLE     0x57
#define GET_SELF_TEST_RESULTS_HARDWARE_ERROR   0x58

#define ERROR_BITFIELD_NO_ERROR                0x00
#define ERROR_BITFIELD_NO_ACCESS_SEL           0x80
#define ERROR_BITFIELD_NO_ACCESS_SDR           0x40
#define ERROR_BITFIELD_NO_ACCESS_FRU           0x20
#define ERROR_BITFIELD_IPMI_NO_RESPONSE        0x10
#define ERROR_BITFIELD_SDR_EMPTY               0x08
#define ERROR_BITFIELD_FRU_CORRUPTED           0x04
#define ERROR_BITFIELD_UPDATE_CORRUPTED        0x02
#define ERROR_BITFIELD_OPERATIONAL_CORRUPTED   0x01

//
// Structure to store information read back when executing GetDeviceId command
//
typedef struct {
  UINT8 DeviceId;
  UINT8 DeviceRevision    :4;
  UINT8 Reserved          :3;
  UINT8 ProvideSDRsDevice :1;
  UINT8 MajorFmRevision   :7;
  UINT8 DeviceAvailable   :1;
  UINT8 MinorFmRevision;
  UINT8 IpmiVersionMostSigBits  :4;
  UINT8 IpmiVersionLeastSigBits :4;
  UINT8 SensorDevice        :1;
  UINT8 SDRRepositoryDevice :1;
  UINT8 SELDevice           :1;
  UINT8 FRUInventoryDevice  :1;
  UINT8 IPMBEventReceiver   :1;
  UINT8 IPMBEventGenerator  :1;
  UINT8 Bridge              :1;
  UINT8 ChassisDevice       :1;
  UINT8 ManufacturerId[3];
  UINT8 ProductId[2];
  UINT8 AuxInfo[4];
} H2O_IPMI_BMC_INFO;

#pragma pack()

#endif