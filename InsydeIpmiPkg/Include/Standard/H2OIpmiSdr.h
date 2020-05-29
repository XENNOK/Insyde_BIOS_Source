/** @file
 H2O IPMI SDR header file.

 This files contains SDR related definitions and structures defined in IPMI
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

#ifndef _H2O_IPMI_SDR_H_
#define _H2O_IPMI_SDR_H_


//
// SDR Device Commands
//
#define H2O_IPMI_CMD_GET_SDR_REPOSITORY_INFO           0x20
#define H2O_IPMI_CMD_GET_SDR_REPOSITORY_AOLLOC_INFO    0x21
#define H2O_IPMI_CMD_RESERVE_SDR_REPOSITORY            0x22
#define H2O_IPMI_CMD_GET_SDR                           0x23
#define H2O_IPMI_CMD_ADD_SDR                           0x24
#define H2O_IPMI_CMD_PARTIAL_ADD_SDR                   0x25
#define H2O_IPMI_CMD_DELETE_SDR                        0x26
#define H2O_IPMI_CMD_CLEAR_SDR_REPOSITORY              0x27
#define H2O_IPMI_CMD_GET_SDR_REPOSITORY_TIME           0x28
#define H2O_IPMI_CMD_SET_SDR_REPOSITORY_TIME           0x29
#define H2O_IPMI_CMD_ENTER_SDR_REPOSITORY_UPDATE_MODE  0x2A
#define H2O_IPMI_CMD_EXIT_SDR_REPOSITORY_UPDATE_MODE   0x2B
#define H2O_IPMI_CMD_RUN_INITIALIZATION_AGENT          0x2C

//
// SDR Record Type
//
#define SDR_RECORD_TYPE_FULL_SENSOR             0x01
#define SDR_RECORD_TYPE_COMPACT_SENSOR          0x02
#define SDR_RECORD_TYPE_EVENTONLY_SENSOR        0x03
#define SDR_RECORD_TYPE_ENTITY_ASSOC            0x08
#define SDR_RECORD_TYPE_DEVICE_ENTITY_ASSOC     0x09
#define SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR  0x10
#define SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR      0x11
#define SDR_RECORD_TYPE_MC_DEVICE_LOCATOR       0x12
#define SDR_RECORD_TYPE_MC_CONFIRMATION         0x13
#define SDR_RECORD_TYPE_BMC_MSG_CHANNEL_INFO    0x14
#define SDR_RECORD_TYPE_OEM                     0xc0

//
// Bit mask in DataStatus
//
#define SDR_LOWER_NON_CRITICAL_AVAILABLE   0x0001
#define SDR_LOWER_CRITICAL_AVAILABLE       0x0002
#define SDR_LOWER_NON_RECOVER_AVAILABLE    0x0004
#define SDR_UPPER_NON_CRITICAL_AVAILABLE   0x0008
#define SDR_UPPER_CRITICAL_AVAILABLE       0x0010
#define SDR_UPPER_NON_RECOVER_AVAILABLE    0x0020
#define SDR_VALUE_AVAILABLE                0x0040
#define SDR_SENSOR_TYPE_ANALOG             0x0080
#define SDR_BELOW_LOWER_NON_CRITICAL       0x0100
#define SDR_BELOW_LOWER_CRITICAL           0x0200
#define SDR_BELOW_LOWER_NON_RECOVER        0x0400
#define SDR_ABOVE_UPPER_NON_CRITICAL       0x0800
#define SDR_ABOVE_UPPER_CRITICAL           0x1000
#define SDR_ABOVE_UPPER_NON_RECOVER        0x2000

#pragma pack(1)

//
// Get SDR Request Data
//
typedef struct {
  UINT16 ReservationId;
  UINT16 RecordId;
  UINT8  Offset;
  UINT8  ByteToRead;
} H2O_IPMI_GET_SDR_REQUEST_DATA;

//
// Get SDR Repository Info
//
typedef struct {
  UINT8  SdrVersion;
  UINT16 RecordCount;
  UINT16 FreeSpace;
  UINT32 MostRecentAddTime;
  UINT32 MostRecentEraseTime;
  UINT8  GetSdrRepoAllocInfoSupport :1;
  UINT8  ReserveSdrRepoSupport      :1;
  UINT8  PartialAddSdrSupport       :1;
  UINT8  DeleteSdrSupport           :1;
  UINT8  Reserved                   :1;
  UINT8  SdrRepoUpdateOp            :2;
  UINT8  OverflowFlag               :1;
} H2O_IPMI_GET_SDR_REPOSITORY_INFO;

//
// Sensor Data Record Formats
//
typedef struct {
  UINT16 RecordId;
  UINT8  SdrVersion;
  UINT8  RecordType;
  UINT8  RecordLength;
} H2O_IPMI_SDR_RECORD_HEADER;

//
// Type/Length Byte
//
typedef struct {
  UINT8 Length    :5;
  UINT8 Reserved  :1;
  UINT8 Type      :2;
} TYPE_LENGTH_BYTE;

//
// SDR Type 01h, Full Sensor Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;   // Offset 1~5
  UINT8  SystemSoftwareId         :1;  // Offset 6
  UINT8  I2CSlaveAddress          :7;
  UINT8  SensorOwnerLun           :2;  // Offset 7
  UINT8  FruDeviceOwnerLun        :2;
  UINT8  ChannelNumber            :4;
  UINT8  SensorNumber;                 // Offset 8
  UINT8  EntityId;                     // Offset 9
  UINT8  InstanceNumber           :7;  // Offset 10
  UINT8  LogicalEntity            :1;
  UINT8  SensorDefaultState       :2;  // Offset 11
  UINT8  InitSensorType           :1;
  UINT8  InitHysteresis           :1;
  UINT8  InitThreshold            :1;
  UINT8  InitEvent                :1;
  UINT8  InitScanning             :1;
  UINT8  SettableSensor           :1;
  UINT8  SensorEventMsgCtrlSupport:2;  // Offset 12
  UINT8  SensorThresholdSupport   :2;
  UINT8  SensorHysteresisSupport  :2;
  UINT8  SensorAutoRearmSupport   :1;
  UINT8  IgnoreSensor             :1;
  UINT8  SensorType;                   // Offset 13
  UINT8  EventReadingType;             // Offset 14
  UINT16 AssertionEventMask;           // Offset 15~16
  UINT16 DeassertionEventMask;         // Offset 17~18
  UINT16 DiscreateReadingMask;         // Offset 19~20
  UINT8  Percentage               :1;  // Offset 21
  UINT8  ModifierUnit             :2;
  UINT8  RateUnit                 :3;
  UINT8  AnalogDataFormat         :2;
  UINT8  SensorUnitBase;               // Offset 22
  UINT8  SensorUnitModifier;           // Offset 23
  UINT8  LinearizationEnum        :7;  // Offset 24
  UINT8  Reserved1                :1;
  UINT8  Mls8bits;                     // Offset 25
  UINT8  Tolerance                :6;  // Offset 26
  UINT8  Mms2bits                 :2;
  UINT8  Bls8bits;                     // Offset 27
  UINT8  Accuracyls6bits          :6;  // Offset 28
  UINT8  Bms2bits                 :2;
  UINT8  SensorDirection          :2;  // Offset 29
  UINT8  AccuracyExp              :2;
  UINT8  Accuracyms4bits          :4;
  UINT8  BExp                     :4;  // Offset 30
  UINT8  RExp                     :4;
  UINT8  AnalogFlagsReading       :1;  // Offset 31
  UINT8  AnalogFlagsMax           :1;
  UINT8  AnalogFlagsMin           :1;
  UINT8  Reserved2                :5;
  UINT8  NormalReading;                // Offset 32
  UINT8  NormalMax;                    // Offset 33
  UINT8  NormalMin;                    // Offset 34
  UINT8  SensorMaxReading;             // Offset 35
  UINT8  SensorMinReading;             // Offset 36
  UINT8  UpNonRecoverThreshold;        // Offset 37
  UINT8  UprCriticalThreshold;         // Offset 38
  UINT8  UpNonCriticalThreshold;       // Offset 39
  UINT8  LowNonRecoverThreshold;       // Offset 40
  UINT8  LowCriticalThreshold;         // Offset 41
  UINT8  LowNonCriticalThreshold;      // Offset 42
  UINT8  PositiveThreshold;            // Offset 43
  UINT8  NegativeThreshold;            // Offset 44
  UINT16 Reserved3;                    // Offset 45~46
  UINT8  Oem;                          // Offset 47
  TYPE_LENGTH_BYTE  IdStringCode;      // Offset 48
  UINT8  IdString[16];                 // Offset 49~64
} H2O_IPMI_SDR_FULL_SENSOR_RECORD;

//
// SDR Type 02h, Compact Sensor Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;   // Offset 1~5
  UINT8  SystemSoftwareId         :1;  // Offset 6
  UINT8  I2CSlaveAddress          :7;
  UINT8  SensorOwnerLun           :2;  // Offset 7
  UINT8  FruDeviceOwnerLun        :2;
  UINT8  ChannelNumber            :4;
  UINT8  SensorNumber;                 // Offset 8
  UINT8  EntityId;                     // Offset 9
  UINT8  InstanceNumber           :7;  // Offset 10
  UINT8  LogicalEntity            :1;
  UINT8  SensorDefaultState       :2;  // Offset 11
  UINT8  InitSensorType           :1;
  UINT8  InitHysteresis           :1;
  UINT8  Reserved1                :1;
  UINT8  InitEvent                :1;
  UINT8  InitScanning             :1;
  UINT8  SettableSensor           :1;
  UINT8  SensorEventMsgCtrlSupport:2;  // Offset 12
  UINT8  SensorThresholdSupport   :2;
  UINT8  SensorHysteresisSupport  :2;
  UINT8  SensorAutoRearmSupport   :1;
  UINT8  IgnoreSensor             :1;
  UINT8  SensorType;                   // Offset 13
  UINT8  EventReadingType;             // Offset 14
  UINT16 AssertionEventMask;           // Offset 15~16
  UINT16 DeassertionEventMask;         // Offset 17~18
  UINT16 DiscreateReadingMask;         // Offset 19~20
  UINT8  Percentage               :1;  // Offset 21
  UINT8  ModifierUnit             :2;
  UINT8  RateUnit                 :3;
  UINT8  Reserved2                :2;
  UINT8  SensorUnitBase;               // Offset 22
  UINT8  SensorUnitModifier;           // Offset 23
  UINT8  ShareCount               :4;  // Offset 24
  UINT8  IdStringModifierType     :2;
  UINT8  SensorDirection          :2;
  UINT8  IdStringModifierOffset   :7;  // Offset 25
  UINT8  EntityInstanceShareing   :1;
  UINT8  PositiveThreshold;            // Offset 26
  UINT8  NegativeThreshold;            // Offset 27
  UINT8  Reserved[3];                  // Offset 28~30
  UINT8  Oem;                          // Offset 31
  TYPE_LENGTH_BYTE  IdStringCode;      // Offset 32
  UINT8  IdString[16];                 // Offset 33~48
} H2O_IPMI_SDR_COMPACT_SENSOR_RECORD;

//
// SDR Type 03h, Event-Only Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;   // Offset 1~5
  UINT8  SystemSoftwareId       :1;    // Offset 6
  UINT8  I2CSlaveAddress        :7;
  UINT8  SensorOwnerLun         :2;    // Offset 7
  UINT8  FruDeviceOwnerLun      :2;
  UINT8  ChannelNumber          :4;
  UINT8  SensorNumber;                 // Offset 8
  UINT8  EntityId;                     // Offset 9
  UINT8  InstanceNumber         :7;    // Offset 10
  UINT8  LogicalEntity          :1;
  UINT8  SensorType;                   // Offset 11
  UINT8  EventReadingType;             // Offset 12
  UINT8  ShareCount             :4;    // Offset 13
  UINT8  IdStringModifierType   :2;
  UINT8  SensorDirection        :2;
  UINT8  IdStringModifierOffset :7;    // Offset 14
  UINT8  EntityInstanceShareing :1;
  UINT8  Reserved;                     // Offset 15
  UINT8  Oem;                          // Offset 16
  TYPE_LENGTH_BYTE  IdStringCode;      // Offset 17
  UINT8  IdString[16];                 // Offset 18~33
} H2O_IPMI_SDR_EVENT_ONLY_SENSOR_RECORD;

//
// SDR Type 08h,
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;   // Offset 1~5
  UINT8  ContianerEntityId;            // Offset 6
  UINT8  ContianerEntityInstance;      // Offset 7
  UINT8  Reserved   :5;                // Offset 8
  UINT8  Accessible :1;
  UINT8  RecordLink :1;
  UINT8  Range      :1;
  UINT8  EntityId1;                    // Offset 9
  UINT8  InstanceId1;                  // Offset 10
  UINT8  EntityId2;                    // Offset 11
  UINT8  InstanceId2;                  // Offset 12
  UINT8  EntityId3;                    // Offset 13
  UINT8  InstanceId3;                  // Offset 14
  UINT8  EntityId4;                    // Offset 15
  UINT8  InstanceId4;                  // Offset 16
} H2O_IPMI_SDR_ENTITY_ASSOCIATION_RECORD;

//
// Container/Contained Entity Device Address
// Container/Contained Entity Device Channel
//
typedef struct {
  UINT8  Reserved1      :1;
  UINT8  SlaveAddress   :7;
  UINT8  Reserved2       :4;
  UINT8  ChannelNumber   :4;
} ENTITY_ADDRESS_CHANNEL;

//
// Container/Contained Entity Device Address
// Container/Contained Entity Device Channel
// with ID/Instance
//
typedef struct {
  ENTITY_ADDRESS_CHANNEL Entity;
  UINT8 EntityId;
  UINT8 EntityInstance;
} ENTITY_ADDRESS_CHANNEL_ID;

//
// SDR Type 09h - Device-relative Entity Association Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;          // Offset 1~5
  UINT8  ContainerEntityId;                   // Offset 6
  UINT8  ContainerEntityInstance;             // Offset 7
  ENTITY_ADDRESS_CHANNEL ConainterEntity;     // Offset 8~9
  UINT8  Reserved                   :5;       // Offset 10
  UINT8  Accessible                 :1;
  UINT8  RecordLink                 :1;
  UINT8  Range                      :1;
  ENTITY_ADDRESS_CHANNEL_ID ContainedEntity1; // Offset 11~14
  ENTITY_ADDRESS_CHANNEL_ID ContainedEntity2; // Offset 15~18
  ENTITY_ADDRESS_CHANNEL_ID ContainedEntity3; // Offset 19~22
  ENTITY_ADDRESS_CHANNEL_ID ContainedEntity4; // Offset 23~26
  UINT8  Reserved1[6];                        // Offset 27~32
} H2O_IPMI_SDR_DEVICE_RELATIVE_ENTITY_ASSOCIATION_RECORD;

//
// SDR Type 10h - Generic Device Locator Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;    // Offset 1~5
  UINT8  Reserved1            :1;       // Offset 6
  UINT8  DeviceAccessAddress  :7;
  UINT16 PrivateBusId         :3;       // Offset 7~8
  UINT16 Lun                  :2;
  UINT16 ChannelNumber        :4;
  UINT16 DeviceSlaveAddress   :7;
  UINT8  AddressSpan          :3;       // Offset 9
  UINT8  Reserved2            :5;
  UINT8  Reserved3;                     // Offset 10
  UINT8  DeviceType;                    // Offset 11
  UINT8  DeviceTypeModifier;            // Offset 12
  UINT8  EntityId;                      // Offset 13
  UINT8  EntityInstance;                // Offset 14
  UINT8  Oem;                           // Offset 15
  TYPE_LENGTH_BYTE  IdStringCode;       // Offset 16
  UINT8  IdString[16];                  // Offset 17~32
} H2O_IPMI_SDR_GENERIC_DEVICE_LOCATOR_RECORD;

//
// SDR Type 11h - FRU Device Locator Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;    // Offset 1~5
  UINT8  Reserved1            :1;       // Offset 6
  UINT8  DeviceAccessAddress  :7;
  UINT8  FruDeviceId;                   // Offset 7
  UINT8  PrivateBusId         :3;       // Offset 8
  UINT8  Lun                  :2;
  UINT8  Reserved2            :2;
  UINT8  LogicalFruDevice     :1;
  UINT8  Reserved3            :4;       // Offset 9
  UINT8  ChannelNumber        :4;
  UINT8  Reserved4;                     // Offset 10
  UINT8  DeviceType;                    // Offset 11
  UINT8  DeviceTypeModifier;            // Offset 12
  UINT8  FruEntityId;                   // Offset 13
  UINT8  FruEntityInstance;             // Offset 14
  UINT8  Oem;                           // Offset 15
  TYPE_LENGTH_BYTE  IdStringCode;       // Offset 16
  UINT8  IdString[16];                  // Offset 17~32
} H2O_IPMI_SDR_FRU_DEVICE_LOCATOR_RECORD;

//
// SDR Type 12h - Management Controller Device Locator Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;    // Offset 1~5
  UINT8  Reserved1              :1;     // Offset 6
  UINT8  DeviceAccessAddress    :7;
  UINT8  Reserved2              :4;     // Offset 7
  UINT8  ChannelNumber          :4;
  UINT8  EventMsgGenerate       :2;     // Offset 8
  UINT8  LogInitialization      :1;
  UINT8  ControllerLogInitial   :1;
  UINT8  Reserved3              :1;
  UINT8  Static                 :1;
  UINT8  AcpiDevPwrStateNotify  :1;
  UINT8  AcpiSysPwrStateNotify  :1;
  UINT8  SensorDevice           :1;     // Offset 9
  UINT8  SdrRepositoryDevice    :1;
  UINT8  SelDevice              :1;
  UINT8  FruInventoryDevice     :1;
  UINT8  IpmbEventReceiver      :1;
  UINT8  IpmbEventGenerator     :1;
  UINT8  Bridge                 :1;
  UINT8  ChassisDevice          :1;
  UINT8  Reserved4[3];                  // Offset 10~12
  UINT8  Entityid;                      // Offset 13
  UINT8  EntityInstance;                // Offset 14
  UINT8  Oem;                           // Offset 15
  TYPE_LENGTH_BYTE IdStringCode;        // Offset 16
  UINT8  IdString[16];                  // Offset 17~32
}H2O_IPMI_SDR_MC_DEVICE_LOCATOR_RECORD;

//
// SDR Type 13h - Management Controller Confirmation Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;    // Offset 1~5
  UINT8  Reserved1              :1;     // Offset 6
  UINT8  DeviceAccessAddress    :7;
  UINT8  DeviceId;                      // Offset 7
  UINT8  DeviceRevision         :4;     // Offset 8
  UINT8  ChannelNumber          :4;
  UINT8  MajorFirmwareRevision  :6;     // Offset 9
  UINT8  Reserved2              :1;
  UINT8  MinorFirmwareRevision;         // Offset 10
  UINT8  IpmiVersionMsb         :4;     // Offset 11
  UINT8  IpmiVersionLsb         :4;
  UINT8  ManufacturerId[3];             // Offset 12~14
  UINT16 ProductId;                     // Offset 15~16
  UINT8  DeviceGuid[16];                // Offset 17~32
} H2O_IPMI_SDR_MC_CONFIRMATION_RECORD;

//
// SDR Type 14h - BMC Message Channel Info Record
//
typedef struct {
  H2O_IPMI_SDR_RECORD_HEADER Header;    // Offset 1~5
  UINT8  ChannelProtocol    :4;         // Offset 6
  UINT8  MessageReceiveLun  :3;
  UINT8  TransmitSupport    :1;
  UINT8  Channel1Info;                  // Offset 7
  UINT8  Channel2Info;                  // Offset 8
  UINT8  Channel3Info;                  // Offset 9
  UINT8  Channel4Info;                  // Offset 10
  UINT8  Channel5Info;                  // Offset 11
  UINT8  Channel6Info;                  // Offset 12
  UINT8  Channel7Info;                  // Offset 13
  UINT8  MsgInterruptType;              // Offset 14
  UINT8  EventMsgInterruptType;         // Offset 15
  UINT8  Reserved;                      // Offset 16
} H2O_IPMI_SDR_BMC_MSG_CHANNEL_INFO_RECORD;

typedef struct _H2O_IPMI_SDR_RAW {
  UINT16          NextId;
  union {
    H2O_IPMI_SDR_FULL_SENSOR_RECORD                         FullSensor;
    H2O_IPMI_SDR_COMPACT_SENSOR_RECORD                      CompactSensor;
    H2O_IPMI_SDR_EVENT_ONLY_SENSOR_RECORD                   EventOnlySensor;
    H2O_IPMI_SDR_ENTITY_ASSOCIATION_RECORD                  EntityAssociation;
    H2O_IPMI_SDR_DEVICE_RELATIVE_ENTITY_ASSOCIATION_RECORD  DeviceEntity;
    H2O_IPMI_SDR_GENERIC_DEVICE_LOCATOR_RECORD              GenericDeviceLocator;
    H2O_IPMI_SDR_FRU_DEVICE_LOCATOR_RECORD                  FruDeviceLocator;
    H2O_IPMI_SDR_MC_DEVICE_LOCATOR_RECORD                   McDeviceLocator;
    H2O_IPMI_SDR_MC_CONFIRMATION_RECORD                     McConfirmation;
    H2O_IPMI_SDR_BMC_MSG_CHANNEL_INFO_RECORD                BMCChannelinfo;
  } SdrRawData;
} H2O_IPMI_SDR_RAW;

#pragma pack()


#endif

