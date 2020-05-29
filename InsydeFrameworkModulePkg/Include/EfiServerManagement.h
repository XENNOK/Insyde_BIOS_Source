//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  EfiServerManagement.h

Abstract:

  Generic Definations for Server Management Modules.


Revision History:

--*/

#ifndef _EFI_SERVER_MANAGEMENT_H
#define _EFI_SERVER_MANAGEMENT_H

//
// Defines
//
#define PRIM_IPMB_CHANNEL       0x0
#define BMC_LUN                 0x0
#define PRESENT_INTERFACE       0xE
#define SYSTEM_INTERFACE        0xF
#define EFI_COMPLETE_SEL_RECORD 0xFF
#define IPMI_UNSPECIFIED_ERROR  0xFF

//
// Net Function Defines.
//
#define EFI_SM_CHASSIS      0x0
#define EFI_SM_BRIDGE       0x2
#define EFI_SM_SENSOR_EVENT 0x4
#define EFI_SM_APPLICATION  0x6
#define EFI_SM_FIRMWARE     0x8
#define EFI_SM_STORAGE      0xA
#define EFI_SM_TRANSPORT    0xC
#define EFI_SM_GROUP_EXT    0x2C
#define EFI_SM_OEM_GROUP    0x2E
#define EFI_SM_INTEL_OEM    0x30
#define EFI_SM_SOL_OEM      0x34

//
// IPMI Command Definations.
//
#define EFI_IPMI_GET_DEVICE_ID          1
#define EFI_IPMI_COLD_RESET             2
#define EFI_IPMI_WARM_RESET             3
#define EFI_IPMI_GET_SELF_TEST_RESULTS  4
#define EFI_IPMI_MFG_MODE_ON            5
#define EFI_IPMI_SET_ACPI_POWER_STATE   6
#define EFI_IPMI_GET_ACPI_POWER_STATE   7
#define EFI_IPMI_GET_DEVICE_GUID        8

#define EFI_IPMI_GET_MESSAGE_FLAGS      0x31
#define EFI_IPMI_GET_MESSAGE            0x33
#define EFI_IPMI_SEND_MESSAGE           0x34

#define RESERVE_SEL_ENTRY               0x42
#define ADD_SEL_ENTRY                   0x44
#define GET_SEL_ENTRY                   0x43
#define DELETE_SEL_ENTRY                0x46
#define CLEAR_SEL_ENTRY                 0x47
#define SET_BMC_GLOBALS                 0x2E
#define GET_BMC_GLOBALS                 0x2F
#define SET_SEL_TIME                    0x49

#define GET_SELF_TEST_RESULTS           0x4

#define NMI_ENABLE_DISABLE              0xF7

//
// Controller Attributes
//
#define EFI_IPMI_SENSOR_DEVICE_SUPPORT    0x1
#define EFI_IPMB_SDR_REPOSITORY_SUPPORT   0x2
#define EFI_IPMI_SEL_DEVICE_SUPPORT       0x4
#define EFI_IPMI_FRU_INVENTORY_SUPPORT    0x8
#define EFI_IPMB_EVENT_RECEIVER_SUPPORT   0x10
#define EFI_IPMB_EVENT_GENERATOR_SUPPORT  0x20
#define EFI_ICMB_BRIDGE_SUPPORT           0x40
#define EFI_ICMB_CHASSIS_DEVICE_SUPPORT   0x80
#define EFI_SM_TCP_SUPPORT                0x100
#define EFI_SM_UDP_SUPPORT                0x200
#define EFI_SM_IPV4_SUPPORT               0x400
#define EFI_SM_IPV6_SUPPORT               0x800
#define EFI_SM_RS232_SUPPORT              0x1000

//
// Sensor Type Definations
//
typedef enum {
  EfiSensorReserved,
  EfiSensorTemperature,
  EfiSensorVoltage,
  EfiSensorCurrent,
  EfiSensorFan,
  EfiSensorPhysicalSecurity,
  EfiSensorPlatformSecurityViolationAttempt,
  EfiSensorProcessor,
  EfiSensorPowerSupply,
  EfiSensorPowerUnit,
  EfiSensorCoolingDevice,
  EfiSensorOtherUnits,
  EfiSensorMemory,
  EfiSensorDriveSlot,
  EfiSensorPOSTMemoryResize,
  EfiSensorSystemFirmwareProgress,
  EfiSensorEventLoggingDisabled,
  EfiSensorWatchdog1,
  EfiSensorSystemEvent,
  EfiSensorCriticalInterrupt,
  EfiSensorButton,
  EfiSensorModuleBoard,
  EfiSensorMicrocontrollerCoprocessor,
  EfiSensorAddinCard,
  EfiSensorChassis,
  EfiSensorChipSet,
  EfiSensorOtherFRU,
  EfiSensorCableInterconnect,
  EfiSensorTerminator,
  EfiSensorSystemBootInitiated,
  EfiSensorBootError,
  EfiSensorOSBoot,
  EfiSensorOSCriticalStop,
  EfiSensorSlotConnector,
  EfiSensorSystemACPIPowerState,
  EfiSensorWatchdog2,
  EfiSensorPlatformAlert,
  EfiSensorEntityPresence,
  EfiSensorMonitorASIC,
  EfiSensorLAN,
  EfiSensorManagementSubsystemHealth
} EFI_SM_SENSOR_TYPE;

//
// Sensor Event Type Code
//
#define EFI_SENSOR_THRESHOLD_EVENT_TYPE 1
#define EFI_SENSOR_SPECIFIC_EVENT_TYPE  0x6F

//
// THRESHOLD SENSOR TYPE BIT MASK
//
#define LOWER_NON_CRITICAL_GOING_LOW  0x1
#define LOWER_NON_CRITICAL_GOING_HI   0x2
#define LOWER_CRITICAL_GOING_LOW      0x4
#define LOWER_CRITICAL_GOING_HI       0x8
#define LOWER_NON_RECOVER_GOING_LOW   0x10
#define LOWER_NON_RECOVER_GOING_HI    0x20
#define UPPER_NON_CRITICAL_GOING_LOW  0x40
#define UPPER_NON_CRITICAL_GOING_HI   0x80
#define UPPER_CRITICAL_GOING_LOW      0x100
#define UPPER_CRITICAL_GOING_HI       0x200
#define UPPER_NON_RECOVER_GOING_LOW   0x400
#define UPPER_NON_RECOVER_GOING_HI    0x800

//
// Server Management COM Addressing types
//
typedef enum {
  EfiSmReserved,
  EfiSmIpmb,
  EfiSmIcmb1_0,
  EfiSmIcmb0_9,
  EfiSm802_3_Lan,
  EfiSmRs_232,
  EfiSmOtherLan,
  EfiSmPciSmBus,
  EfiSmSmBus1_0,
  EfiSmSmBus2_0,
  EfiSmUsb1_x,
  EfiSmUsb2_x,
  EfiSmBmc
} EFI_SM_CHANNEL_MEDIA_TYPE;

typedef enum {
  EfiSmTcp,
  EfiSmUdp,
  EfiSmIcmp,
  EfiSmIpmi
} EFI_SM_PROTOCOL_TYPE;

typedef enum {
  EfiSmMessage,
  EfiSmRawData
} EFI_SM_DATA_TYPE;

typedef struct {
  BOOLEAN IpAddressType;
  UINT16  IpPort;
  UINT8   IpAddress[16];
} EFI_SM_IP_ADDRESS;

typedef struct {
  UINT8 SlaveAddress;
  UINT8 LunAddress;
  UINT8 NetFunction;
  UINT8 ChannelAddress;
} EFI_SM_IPMI_ADDRESS;

typedef struct {
  UINT8 SerialPortNumber;
} EFI_SM_SERIAL_ADDRESS;

typedef union {
  EFI_SM_IP_ADDRESS     IpAddress;
  EFI_SM_IPMI_ADDRESS   BmcAddress;
  EFI_SM_SERIAL_ADDRESS SerialAddress;
} EFI_SM_COM_ADDRESS_TYPE;

typedef struct {
  EFI_SM_CHANNEL_MEDIA_TYPE ChannelType;
  EFI_SM_COM_ADDRESS_TYPE   Address;
} EFI_SM_COM_ADDRESS;

#pragma pack(1)
//
// Sensor Reading Data
//
typedef enum {
  DataLinear,                                   // Linear
  DataNaturalLog,                               // Ln(x)
  DataLog10,                                    // Log10(x)
  DataLog2,                                     // Log2(x)
  Datae,                                        // e
  DataExp10,                                    // Exp 10
  DataExp2,                                     // Exp 2
  DataInverse,                                  // 1/x
  DataSqr,                                      // Sqr
  DataCube,                                     // Cube
  DataSqrt,                                     // Square Root
  DataCubeInverse                               // Cube-1 (x)
} LINERIZATION_TYPE;

typedef union {
  UINT8   SensorUint8Data[2];
  UINT16  SensorUint16Data;
} SENSOR_SPLIT_DATA;

typedef struct {
  LINERIZATION_TYPE Linearization;              // L
  UINT8             Tolerance;                  // Tolerance
  UINT8             AdditiveOffsetExp;          // k1
  UINT8             AccuracyExp;                // Accuracy Exponential
  UINT8             ResultExponent;             // k2
  SENSOR_SPLIT_DATA IntegerConstantMultiplier;  // M
  SENSOR_SPLIT_DATA AdditiveOffset;             // B
  SENSOR_SPLIT_DATA Accuracy;                   // Accuracy
} EFI_SENSOR_CONVERSION_DATA;

//
// Server Management Controller Information
//
typedef struct {
  UINT8   DeviceId;
  UINT8   DeviceRevision : 4;
  UINT8   Reserved : 3;
  UINT8   DeviceSdr : 1;
  UINT8   MajorFirmwareRev : 7;
  UINT8   UpdateMode : 1;
  UINT8   MinorFirmwareRev;
  UINT8   SpecificationVersion;
  UINT8   SensorDeviceSupport : 1;
  UINT8   SdrRepositorySupport : 1;
  UINT8   SelDeviceSupport : 1;
  UINT8   FruInventorySupport : 1;
  UINT8   IPMBMessageReceiver : 1;
  UINT8   IPMBMessageGenerator : 1;
  UINT8   BridgeSupport : 1;
  UINT8   ChassisSupport : 1;
  UINT8   ManufacturerId[3];
  UINT16  ProductId;
  UINT32  AuxFirmwareRevInfo;
} EFI_SM_CTRL_INFO;

//
//  IPMI SDR Records Data Structure
//
typedef struct {
  UINT16  RecordId;                     // 1
  UINT8   Version;                      // 3
  UINT8   RecordType;                   // 4
  UINT8   RecordLength;                 // 5
  UINT8   OwnerId;                      // 6
  UINT8   OwnerLun;                     // 7
  UINT8   SensorNumber;                 // 8
  UINT8   EntityId;                     // 9
  UINT8   EntityInstance;               // 10
  UINT8   EventScanningEnabled : 1;     // 11
  UINT8   EventScanningDisabled : 1;    // 11
  UINT8   InitSensorType : 1;           // 11
  UINT8   InitHysteresis : 1;           // 11
  UINT8   InitThresholds : 1;           // 11
  UINT8   InitEvent : 1;                // 11
  UINT8   InitScanning : 1;             // 11
  UINT8   Reserved : 1;                 // 11
  UINT8   EventMessageControl : 2;      // 12
  UINT8   ThresholdAccessSupport : 2;   // 12
  UINT8   HysteresisSupport : 2;        // 12
  UINT8   ReArmSupport : 1;             // 12
  UINT8   IgnoreSensor : 1;             // 12
  UINT8   SensorType;                   // 13
  UINT8   EventType;                    // 14
  UINT8   Reserved1[7];                 // 15
  UINT8   UnitType;                     // 22
  UINT8   Reserved2;                    // 23
  UINT8   Linearization : 7;            // 24
  UINT8   Reserved3 : 1;                // 24
  UINT8   MLo;                          // 25
  UINT8   Toleremce : 6;                // 26
  UINT8   MHi : 2;                      // 26
  UINT8   BLo;                          // 27
  UINT8   AccuracyLow : 6;              // 28
  UINT8   BHi : 2;                      // 28
  UINT8   Reserved4 : 2;                // 29
  UINT8   AccuracyExp : 2;              // 29
  UINT8   AccuracyHi : 4;               // 29
  UINT8   BExp : 4;                     // 30
  UINT8   RExp : 4;                     // 30
  UINT8   NominalReadingSpscified : 1;  // 31
  UINT8   NominalMaxSpscified : 1;      // 31
  UINT8   NominalMinSpscified : 1;      // 31
  UINT8   Reserved5 : 5;                // 31
  UINT8   NominalReading;               // 32
  UINT8   Reserved6[4];                 // 33
  UINT8   UpperNonRecoverThreshold;     // 37
  UINT8   UpperCriticalThreshold;       // 38
  UINT8   UpperNonCriticalThreshold;    // 39
  UINT8   LowerNonRecoverThreshold;     // 40
  UINT8   LowerCriticalThreshold;       // 41
  UINT8   LowerNonCriticalThreshold;    // 42
  UINT8   Reserved7[5];                 // 43
  UINT8   IdStringLength;               // 48
  UINT8   AsciiIdString[16];            // 49 - 64
} IPMI_SDR_RECORD_STRUCT_1;

typedef struct {
  UINT16  RecordId;                     // 1
  UINT8   Version;                      // 3
  UINT8   RecordType;                   // 4
  UINT8   RecordLength;                 // 5
  UINT8   OwnerId;                      // 6
  UINT8   OwnerLun;                     // 7
  UINT8   SensorNumber;                 // 8
  UINT8   EntityId;                     // 9
  UINT8   EntityInstance;               // 10
  UINT8   SensorScanning : 1;           // 11
  UINT8   EventScanning : 1;            // 11
  UINT8   InitSensorType : 1;           // 11
  UINT8   InitHysteresis : 1;           // 11
  UINT8   InitThresholds : 1;           // 11
  UINT8   InitEvent : 1;                // 11
  UINT8   InitScanning : 1;             // 11
  UINT8   Reserved : 1;                 // 11
  UINT8   EventMessageControl : 2;      // 12
  UINT8   ThresholdAccessSupport : 2;   // 12
  UINT8   HysteresisSupport : 2;        // 12
  UINT8   ReArmSupport : 1;             // 12
  UINT8   IgnoreSensor : 1;             // 12
  UINT8   SensorType;                   // 13
  UINT8   EventType;                    // 14
  UINT8   Reserved1[7];                 // 15
  UINT8   UnitType;                     // 22
  UINT8   Reserved2[9];                 // 23
  UINT8   IdStringLength;               // 32
  UINT8   AsciiIdString[16];            // 33 - 48
} IPMI_SDR_RECORD_STRUCT_2;

typedef struct {
  UINT8 Reserved1 : 1;
  UINT8 ControllerSlaveAddress : 7;
  UINT8 FruDeviceId;
  UINT8 BusId : 3;
  UINT8 Lun : 2;
  UINT8 Reserved : 2;
  UINT8 LogicalFruDevice : 1;
  UINT8 Reserved3 : 4;
  UINT8 ChannelNumber : 4;
} EFI_FRU_DATA_INFO;

typedef struct {
  UINT16            RecordId;           // 1
  UINT8             Version;            // 3
  UINT8             RecordType;         // 4
  UINT8             RecordLength;       // 5
  EFI_FRU_DATA_INFO FruDeviceData;      // 6
  UINT8             Reserved1;          // 10
  UINT8             DeviceType;         // 11
  UINT8             DeviceTypeModifier; // 12
  UINT8             FruEntityId;        // 13
  UINT8             FruEntityInstance;  // 14
  UINT8             OemReserved;        // 15
  UINT8             Length : 4;         // 16
  UINT8             Reserved2 : 1;      // 16
  UINT8             StringType : 3;     // 16
  UINT8             String[16];         // 17
} IPMI_SDR_RECORD_STRUCT_11;

typedef union {
  IPMI_SDR_RECORD_STRUCT_1  SensorType1;
  IPMI_SDR_RECORD_STRUCT_2  SensorType2;
  IPMI_SDR_RECORD_STRUCT_11 SensorType11;
} IPMI_SENSOR_RECORD_STRUCT;

#pragma pack()

typedef enum {
  Unicode,
  BcdPlus,
  Ascii6BitPacked,
  AsciiLatin1
} SENSOR_ID_STRING_TYPE;

//
// SENSOR Structures
//
typedef struct {
  BOOLEAN                     Valid;              // Data is Valid
  EFI_SENSOR_CONVERSION_DATA  ConversionParam;    // Conversion Parameters
  UINT8                       UpperNonRec;        // Upper Non Recoverable
  UINT8                       UpperCritical;      // Upper Critical
  UINT8                       UpperNonCritical;   // Upper Non Critical
  UINT8                       LowerNonRec;        // Lower Non Recoverable
  UINT8                       LowerCritical;      // Lower Critical
  UINT8                       LowerNonCritical;   // Lower Non Critical
} EFI_SENSOR_THRESHOLD_STRUCT;

typedef struct {
  BOOLEAN                     Valid;              // Structure Valid
  EFI_SENSOR_CONVERSION_DATA  ConversionParam;    // Conversion Parameters
  SENSOR_ID_STRING_TYPE       SensorIdStringType; // Sensor ID String type
  UINT8                       NominalReading;     // Nominal Reading of the Sensor
  UINT8                       SensorId[16];       // Sensor Description
} EFI_SENSOR_READING_STRUCT;

//
//  IPMI HOB
//
typedef struct {
  UINT16  IoBasePort;
} IPMI_HOB_DATA;

//
// COM Layer Callback
//
typedef
EFI_STATUS
(EFIAPI *EFI_SM_CALLBACK_PROC) (
  OUT  EFI_STATUS                          Status,
  IN  VOID                                 *UserContext
  );

typedef struct {
  EFI_SM_CALLBACK_PROC  SmCallback;
  VOID                  *UserContext;
} EFI_SM_CALLBACK;

#endif
