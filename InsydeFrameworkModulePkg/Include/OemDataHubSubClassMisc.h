//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _OEMDATAHUB_SUBCLASS_MISC_H_
#define _OEMDATAHUB_SUBCLASS_MISC_H_
#include EFI_GUID_DEFINITION(DataHubRecords)
#include "Numbers.h"

#pragma pack(1)
//
// SMBIOS Type 0
//
typedef enum {
  //
  // SMBIOS 2.7+ (Type0)
  //
  // BIOS Characteristics Extension Byte2:
  // ..
  // Bit3   UEFI Specification supported.             --2.7+
  // Bit4   SMBIOS table describes a virtual machine. --2.7+
  // Bit5:7 Reserved, set to 0.
  //
  EfiMiscUefiSpecificationSupported = BIT0,
  EfiMiscSMBIOSTableDescribes       = BIT1
} EFI_MISC_BIOS_CHARACTERISTICS_EXTENSION_BYTE2;

//
// SMBIOS Type 3
//
#define EFI_MISC_CHASSIS_MANUFACTURER_PLUS_RECORD_NUMBER  0x00000005
typedef struct {
  EFI_MISC_CHASSIS_MANUFACTURER    EfiMiscChassisManufacture;
  //
  //SMBIOS 2.7+
  //
  STRING_REF                       ChassisSkuNumber;
} EFI_MISC_CHASSIS_MANUFACTURER_PLUS;

//
// SMBIOS Type 9
//
typedef enum {  
  //
  // SMBIOS 2.7.1+
  //
  EfiSlotTypePciExpressGen3    = 0xB1,
  EfiSlotTypePciExpressGen3X1  = 0xB2,
  EfiSlotTypePciExpressGen3X2  = 0xB3,
  EfiSlotTypePciExpressGen3X4  = 0xB4,
  EfiSlotTypePciExpressGen3X8  = 0xB5,
  EfiSlotTypePciExpressGen3X16 = 0xB6
} EFI_MISC_SLOT_TYPE_PLUS;

//
// SMBIOS Type 11
//
#define EFI_MISC_OEM_STRING_PLUS_RECORD_NUMBER 0x00000009

typedef struct {
  UINT8                               OemStringCount;
  STRING_REF                          OemStringRef[1];
} EFI_MISC_OEM_STRING_PLUS;

//
// SMBIOS Type 21
//
#define EFI_MISC_ONBOARD_DEVICE_TYPE_DATA_RECORD_NUMBER 0x0000000F

//
// SMBIOS Type 22
//
#define BatteryCapacityUnknown          0x0000
#define BatteryVoltageUnknown           0x0000
#define BatteryMaximumPercentErr        0xFF
#define BatterySbdsSerialNumberUnknown  0xFFFF
#define BatterySbdsManufactureDate      0x3750
#define BatteryDesignCapacityMultiplier 0x01
#define BatteryOemSpecific              0xFFFF

//
// SMBIOS Type 38
//
#define EFI_MISC_IPMI_INTERFACE_TYPE_DATA_PLUS_RECORD_NUMBER  0x0000001D

typedef struct {
  EFI_MISC_IPMI_INTERFACE_TYPE_DATA     EfiMiscIpmiInterfaceTypeData;

  UINT8                                 BaseAddressModifier_InterruptInfo;
  UINT8                                 InterruptNumber;

} EFI_MISC_IPMI_INTERFACE_TYPE_DATA_PLUS;

//
// SMBIOS Type 136 ( 0x88 ) - Compatible Revision ID Record
//
#define EFI_MISC_COMPATIBLE_REVISION_ID_RECORD_NUMBER 0x0000002F

typedef struct {
  UINT16        FieldValue;
} EFI_MISC_COMPATIBLE_REVISION_ID;

//
// Generic Data Record - All SMBIOS Type
//
#define EFI_MISC_SMBIOS_STRUCT_ENCAPSULATION_RECORD_NUMBER  0x0000001F

//
// SMBIOS Type 27
//
#define EFI_MISC_COOLING_DEVICE_TEMP_LINK_PLUS_RECORD_NUMBER 0x00000015

typedef struct {
  EFI_MISC_COOLING_DEVICE_TEMP_LINK         EfiMiscCoolingDevice;

  //
  // SMBIOS 2.7+
  //
  STRING_REF                                Description;
} EFI_MISC_COOLING_DEVICE_TEMP_LINK_PLUS;

typedef union {
  EFI_MISC_SUBCLASS_RECORDS                     EfiMiscSubclassRecords;
  EFI_MISC_COMPATIBLE_REVISION_ID               MiscCompatibleRevisionID;
} EFI_MISC_SUBCLASS_RECORDS_PLUS;

#pragma pack()
#endif
