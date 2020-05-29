/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PerfTune.h

--*/

#ifndef PERF_TUNE_PROTOCOL_INCLUDED
#define PERF_TUNE_PROTOCOL_INCLUDED
//[-start-120816-IB10820105-modify]// 
#include <ChipsetSmiTable.h>
//[-end-120816-IB10820105-modify]// 
typedef struct _SPTT_DATA_PROTOCOL SPTT_DATA_PROTOCOL;
//EFI_FORWARD_DECLARATION (SPTT_TABLE_SERVICES_PROTOCOL_GUID);

//
// Global ID for the SPTT Clock Data Protocol
//
#define SPTT_DATA_PROTOCOL_GUID \
  { 0xef791955, 0xf629, 0x49c5, 0xab, 0x38, 0x98, 0xc3, 0x6b, 0x83, 0xba, 0x64 }

#define SPTT_TABLE_SERVICES_PROTOCOL_GUID \
  { 0x40918dc6, 0xde56, 0x4ed1, 0x86, 0xd5, 0x60, 0xdb, 0xf2, 0x58, 0xa4, 0x77 }



//////////////////////////////////////////////////////////////
//  Performance Tuning Structure Definitions
//////////////////////////////////////////////////////////////


// Performance Tuning Table Version Support
#define SPTT_TABLE_MAJOR_VERSION                              1
#define SPTT_TABLE_MINOR_VERSION                              4

// SMM BIOS Settings Structure Version
#define BIOS_SETTINGS_STRUCTURE_MAJOR_REVISION                2
#define BIOS_SETTINGS_STRUCTURE_MINOR_REVISION                0

#define DEVICE_SPECIFIC_DATA_REVISION_0                       0
#define DEVICE_SPECIFIC_DATA_REVISION_1                       1


// BIOS Setting Implementation Types (for HuronRiver basic only)
#define PERF_TUNE_BIOS_PROC_MULT_CONTROL                      0x00
#define PERF_TUNE_BIOS_HOST_CLOCK_CONTROL                     0x01
#define PERF_TUNE_BIOS_MEM_VOLTAGE_CONTROL                    0x05
#define PERF_TUNE_BIOS_TCL_CONTROL                            0x07
#define PERF_TUNE_BIOS_TRCD_CONTROL                           0x08
#define PERF_TUNE_BIOS_TRP_CONTROL                            0x09
#define PERF_TUNE_BIOS_TRAS_CONTROL                           0x0A
#define PERF_TUNE_BIOS_TWR_CONTROL                            0x0B
#define PERF_TUNE_BIOS_DDR_MULT_CONTROL                       0x13
#define PERF_TUNE_BIOS_TRFC_CONTROL                           0x15
#define PERF_TUNE_BIOS_TRRD_CONTROL                           0x16
#define PERF_TUNE_BIOS_TWTR_CONTROL                           0x17
#define PERF_TUNE_BIOS_NMODE_CONTROL                          0x18
#define PERF_TUNE_BIOS_TRTP_CONTROL                           0x19
#define PERF_TUNE_BIOS_TURBO_ENABLE_CONTROL                   0x1A
#define PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL                   0x1D
#define PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL                   0x1E
#define PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL                   0x1F
#define PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL                   0x20
#define PERF_TUNE_BIOS_TRC_CONTROL                            0x27
#define PERF_TUNE_BIOS_TFAW_CONTROL                           0x28
#define PERF_TUNE_BIOS_EIST_CONTROL                           0x29
#define PERF_TUNE_BIOS_MAX_TURBO_CPU_VOLTAGE_CONTROL          0x2E
#define PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL        0x2F
#define PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL       0x30
#define PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_EN_DIS_CONTROL       0x31
#define PERF_TUNE_BIOS_PKG_TDP_LOCK_EN_DIS_CONTROL            0x32
#define PERF_TUNE_BIOS_IA_CORE_CURRENT_MAX_CONTROL            0x39
#define PERF_TUNE_BIOS_IGFX_CORE_CURRENT_MAX_CONTROL          0x3A
#define PERF_TUNE_BIOS_GFX_TURBO_RATIO_LIMIT_CONTROL          0x3B
#define PERF_TUNE_BIOS_GFX_CORE_VOLTAGE_CONTROL               0x3C
#define PERF_TUNE_BIOS_XMP_CONTROL                            0x40

#define PERF_TUNE_BIOS_SETTINGS_MAX_COUNT                     0x3C


//
// BIOS Settings Structure Return Code
//
#define PERF_TUNE_BIOS_SUCCESS                                0x0
// Error Codes 
#define INVALID_SIGNATURE_INPUT                               0x8001
#define TABLE_LENGTH_TOO_SMALL_VALID_HEADER                   0x8002
#define TABLE_LENGTH_TOO_SMALL_NO_HEADER                      0x8003
#define UNKNOWN_COMMAND_IN_ECX                                0x8004
#define WATCH_DOG_TIMER_DURING_OS_BOOT_NOT_SUPPORTED          0x8005
#define INVALID_SMI_REVISION                                  0x8006
#define INTERNAL_BIOS_ERROR                                   0xFFFF
// Warining Codes
#define USE_OF_OLD_SIG                                        0x0001
#define TABLE_LEN_TOO_LARGE                                   0x0002
#define INTERNAL_BIOS_WARNING                                 0x00FF


//
// BIOS SPECIFIC ERROR CODE
//
#define SETUP_VAR_READ_ERROR                                  0x9001
#define SETUP_VAR_WRITE_ERROR                                 0x9002

#pragma pack(1)
typedef struct {
  UINT8               ImplementType; 
  UINT16              DefaultSetting;
  UINT16              Max;
  UINT8               Min;
  UINT8               Step;
  UINT8               TypeEmu;
  UINT8               AutoModeSupport;
  //UINT8               TypeEmu[52];
} CONTINUE_SETTING_DATA;


//
// Linked List Structure for the BIOS Settings
//
typedef struct _bios_settings_list_{
  UINT16                        Value;
  UINT8                         Precision;
  UINT8                         DataTypeEnum;
  UINT16                        BiosSetting;
  struct _bios_settings_list_   *NextSetting;
} BIOS_SETTINGS_LIST;


//
// BIOS Settings Data Structure Header Info
//
typedef struct{
   UINT32             Signature;
   UINT32             Length;
   UINT16             MajorRev;
   UINT16             MinorRev;
}BIOS_SETTINGS_DATA_HEADER;
//
// BIOS Settings Entry
//
typedef struct{
   UINT32             BiosImplementType;
   UINT32             SettingValue;
}BIOS_SETTING_ENTRY;
//
// BIOS Settings Data Structure
//
typedef struct{
  BIOS_SETTINGS_DATA_HEADER     Hdr;
  UINT32                        BiosSettingCount;
  BIOS_SETTING_ENTRY            BiosSettingEntry[PERF_TUNE_BIOS_SETTINGS_MAX_COUNT];
}BIOS_SETTINGS_DATA_STRUCT;




typedef struct{
  UINT32    Signature;
  UINT16    SetupQuestionLen;
  UINT16    HelpTxtLen;
  UINT16    RangeOfValues;
  UINT16    DefaultValueIndex;
  UINT16    CurrentValueIndex;
  UINT16    Res;
}CUSTOM_BIOS_SETUP_QUESTION;



typedef struct{
  UINT8 Tach1:1;
  UINT8 Tach2:1;
  UINT8 Tach3:1;
  UINT8 Tach4:1;
  UINT8 Res  :4;
}TACH_ASSOCIATION;

typedef struct{
  UINT8 Res1 :1;
  UINT8 Zone1:1;
  UINT8 Zone2:1;
  UINT8 Zone3:1;
  UINT8 Zone4:1;
  UINT8 Res2 :3;
} ZONE_FLAGS_BITS;

typedef union{
  ZONE_FLAGS_BITS Bits;
  UINT8           Data;
}ZONE_FLAGS;


typedef struct {
  ZONE_FLAGS_BITS       ZoneFlags;            // ZONE_FLAGS
  UINT8                 TemperatureUsage;     // TEMP_USAGE
} HECETA_ZONE_ASSOCIATION;

typedef struct{
  UINT32                Signature;            //'HEAT'
  UINT8                 Pwm1Enable;
  TACH_ASSOCIATION      Pwm1TachAssociation;  // TACH_ASSOCIATION
  UINT8                 Pwm2Enable;
  TACH_ASSOCIATION      Pwm2TachAssociation;  // TACH_ASSOCIATION
  UINT8                 Pwm3Enable;
  TACH_ASSOCIATION      Pwm3TachAssociation;  // TACH_ASSOCIATION
  UINT8                 Tach1Usage;           // TACH_USAGE
  UINT8                 Tach2Usage;           // TACH_USAGE
  UINT8                 Tach3Usage;           // TACH_USAGE
  UINT8                 Tach4Usage;           // TACH_USAGE
  UINT16                TControl;             // This is the TControl value for the processor
  UINT8                 SmbusAddr;            // This is the SMBus Address to the Heceta
  UINT8                 NumberOfZones;        // This is the number of zones contained in the Array of Zones.
  UINT16                Res;
}HECETA_THERMAL_DEVICE;

typedef struct{
  UINT16            SettingValue;
  UINT8             SettingPrecision;
  UINT8             Index;
  UINT8             Polarity;
}PCI_FREQ_DATA;



//
// SPTT
//
typedef struct{
  UINT32    Signature;
  UINT32    Length;
  UINT16    MajorVer;
  UINT16    MinorVer;
  UINT32    Reserved1;
  UINT32    Reserved2;
  UINT32    Reserved3;
  UINT16    SwSmiPort;
  UINT8     SwSmiCmd;
  UINT8     Checksum;
  UINT32    FeatureFlags;
}SYSTEM_PERFORMANCE_TUNING_TABLE;

//
// DDD table
//
typedef struct{
  UINT32              Signature;
  UINT8               DeviceType;
  UINT8               ControlType;
  UINT8               ImplementationType;
  UINT8               DeviceSpecificDataRevision;
}DEVICE_DESCRIPTION_TABLE;


//
//==============================
// BIOS Device Structure
//==============================
//

//
// Logical BIOS Settings Device Specific Data ($LBS, revision 1)
//
  typedef struct {
    UINT16              SettingValue;
    UINT8               SettingPrecision;
    UINT8               DataTypeEnum;
    UINT16              BiosSettingsValue;
    UINT8               SettingFlags;       // 1:negative, 0:positive
  } RAW_LBS_ENTRY_ITEM;

typedef struct {
  UINT8               ImplementType;
  UINT8               AutoModeSupport;
  UINT16              DefaultSetting;
  RAW_LBS_ENTRY_ITEM  RawSettingEntries[52];
} RAW_DDD_LBS_DATA;


typedef struct{
  UINT32              Signature;
  UINT8               AutoModeSupport;
  UINT8               Reserved1;
  UINT16              DefaultSetting;
  UINT16              Count;
  UINT16              Reserved2;
  //LOGICAL_BIOS_SETTING_ENTRY    Pseudo[Varies];
}LOGICAL_BIOS_SETTING_DEVICE;

  typedef struct{
    UINT16                SettingValue;
    UINT8                 SettingPrecision;
    UINT8                 DataTypeEnum;
    UINT16                BiosSettingsValue;
    UINT8                 SettingFlags;       // 1:negative, 0:positive
    UINT8                 Reserved1;
    UINT8                 Reserved2;
    UINT8                 Reserved3;
  }LOGICAL_BIOS_SETTING_ENTRY;

//
// Logical BIOS Settings (Continuous) Device Specific Data ($LBC, revision 0)
//
typedef struct {
  UINT8               ImplementType;
  UINT16              Min;
  UINT16              Max;
  UINT8               Step;
  UINT8               DataTypeEnum; 
  UINT8               AutoModeSupport;
  UINT8               MinSettingPerFlag;
  UINT16              DefaultSetting;
  UINT8               SettingPrecision;
} RAW_DDD_LBC_DATA;

typedef struct{
  UINT32              Signature;
  UINT16              Min;
  UINT16              Max;
  UINT8               Step;
  UINT8               DataTypeEnum; 
  UINT8               AutoModeSupport;
  UINT8               MinSettingPerFlag;
  UINT16              DefaultSetting;
  UINT8               SettingPrecision;
  UINT8               Reserved;
}LOGICAL_BIOS_CONTINUOUS_DEVICE;

//
// High Precision Logical BIOS Settings (Continuous) Device Specific Data (HLBC, revision 1)
//
typedef struct{
  UINT8               ImplementType;
  UINT32              MinValue;
  UINT32              MaxValue;
  UINT32              StepSize;
  UINT8               SettingPrecision;
  UINT8               SettingFlags;
  UINT16              MinData;
  UINT16              MaxData;
  UINT16              DataStepSize;
  UINT8               DataTypeEnum; 
  UINT8               AutoModeSupport;
  UINT8               MinMaxPerformance;
  UINT16              DefaultSetting;
} RAW_DDD_HLBC_DATA;

typedef struct{
  UINT32              Signature;
  UINT32              MinValue;
  UINT32              MaxValue;
  UINT32              StepSize;
  UINT8               SettingPrecision;
  UINT8               SettingFlags;
  UINT16              MinData;
  UINT16              MaxData;
  UINT16              DataStepSize;
  UINT8               DataTypeEnum; 
  UINT8               AutoModeSupport;
  UINT8               MinMaxPerformance;
  UINT8               Reserved1;
  UINT16              DefaultSetting;
  UINT16              Reserved2;
}HIGH_LOGICAL_BIOS_CONTINUOUS_DEVICE;

//------------------------------
// End. BIOS Device Structure
//------------------------------


//
//==============================
// Real-time Device Structure
//==============================
//

#define BYTE_WIDTH_TRANSFER 0
#define WORD_WIDTH_TRANSFER 1

//
// Voltage Device (SMBUS-based) Device Specific Data (SMVD, revision 1)
//
typedef struct{
  UINT32        Signature;
  UINT8         SmbusAddr;
  UINT8         Width; // VALUE_WIDTH
  UINT32        HwCtrlModeSettingValue;
  UINT8         HwCtrlModeSettingPrecision;
  UINT8         HwCtrlModeSupportFlags;
  UINT16        HwCtrlModeEntryCount;
  UINT16        ValueCount;
  UINT16        SettingsCount;
  //SMBUS_VOLTAGE_HW_CTRL_MODE_ENTRY   Pseudo1[Varies];
  //SMBUS_VOLTAGE_ENTRY                Pseudo2[Varies];
}SMBUS_VOLTAGE_DEVICE;

  typedef struct{
    UINT32    SettingValue;
    UINT8     SettingPrecision;
    UINT8     SettingFlags;
    UINT16    Reserved;
    //SMBUS_VOLTAGE_VALUE_ENTRY         Pesudo[Varies];
  }SMBUS_VOLTAGE_ENTRY;

    typedef struct{
      UINT8     SmbusCmd;
      UINT8     IgnoreOnRead;
      UINT16    ValueMask;
      UINT16    Value;
      UINT16    Reserved;
    }SMBUS_VOLTAGE_VALUE_ENTRY, SMBUS_VOLTAGE_HW_CTRL_MODE_ENTRY;


//
// Clock Device (SMBUS-based) Device Specific Data (SMCD, revision 0)
//
typedef struct{
  UINT32    Signature;
  UINT8     SmbusAddr;
  UINT8     Reserved1;
  UINT16    HwCtrlModeSettingValue;
  UINT8     HwCtrlModeSettingPrecision;
  UINT8     Reserved2;
  UINT16    HwCtrlModeEntryCount;
  UINT8     ByteCount;
  UINT8     BlkReadByteCntLoc;
  UINT8     BlkReadByteCntAndMask;
  UINT8     BlkReadByteCntOrMask;
  UINT8     PciDerivedClock;
  UINT8     PciRatioMultiplier;
  UINT8     PciRatioDivider;
  UINT8     Reserved3;
  UINT16    ClockValueEntryCount;
  UINT16    ClockSettingsEntryCount;
  //CLOCK_HW_CTRL_MODE_ENTRY              Pseudo1[Varies];
  //SMBUS_CLOCK_SETTINGS_ENTRY            Pseudo2[Vaires];
}SMBUS_CLOCK_DEVICE;

  typedef struct{
    UINT16            SettingValue;
    UINT8             SettingPrecision;
    UINT8             Res;
    //CLOCK_VALUE_ENTRY                     Pseudo[Varies];
  }SMBUS_CLOCK_SETTINGS_ENTRY;

    typedef struct{
      UINT8     ByteIndex;
      UINT8     ValueMask;
      UINT8     Value;
      UINT8     IgnoreOnRead;
    }CLOCK_VALUE_ENTRY, CLOCK_HW_CTRL_MODE_ENTRY;


//
// Voltage Device (IO-based) Device Specific Data (IOVX, revision 1)
//
typedef struct{
  UINT32        Signature;
  UINT16        VoltagValueSettingCount;
  UINT8         IOType; 
  UINT8         UseIndexPort;
  UINT32        IndexPort;
  UINT32        BasePortOrAddress;
  UINT8         SizeOfDataInBits;
  UINT8         Reserved0;
  UINT8         Reserved1;
  UINT8         Reserved2;
  UINT32        HwCtrlModeSettingValue;
  UINT8         HwCtrlModeSettingPrecision;
  UINT8         HwCtrlModeSupportFlags;
  UINT16        HwCtrlModeEntryCount;
  //IO_VOLTAGE_HW_CTRL_MODE_ENTRY       Pseudo1[Varies];
  //IO_VOLTAGE_VALUE_ENTRY              Pseudo2[Varies];
}IO_VOLTAGE_DEVICE;

  typedef struct{
    UINT32    SettingValue;
    UINT8     SettingPrecision;
    UINT8     SettingFlags;
    UINT16    VoltageEntrycount;
    //IO_VOLTAGE_VALUE_ENTRY              Pseudo[Varies];
  }IO_VOLTAGE_ENTRY;

    typedef struct{
      UINT32     OffsetOrIndexValue;
      UINT32     ValueMask;
      UINT32     Value;
    }IO_VOLTAGE_VALUE_ENTRY, IO_VOLTAGE_HW_CTRL_MODE_ENTRY;

//--------------------------------
// End. Real-time Device Structure
//--------------------------------



//////typedef struct{
//////  UINT32        Signature;
//////  UINT32        NumOfMemRegions;
//////}AMRT_TABLE;

// Performance Tuning Device Types
#define PERF_TUNE_END_OF_DEVICES              0
#define PERF_TUNE_VOLTAGE_DEVICE              1
#define PERF_TUNE_CLOCK_DEVICE                2
#define PERF_TUNE_THERMAL_DEVICE              3
#define PERF_TUNE_BIOS_SETTING_DEVICE         4
#define PERF_TUNE_CUSTOM_BIOS_QUESTION_DEVICE 5

// Performance Tuning Control Type and Implementation Type representing Not Applicable
#define PERF_TUNE_NOT_APPLICABLE 0

// Voltage Device Control Types
#define PERF_TUNE_VOLTAGE_ICH_GPIO_CONTROL    0
#define PERF_TUNE_VOLTAGE_SMBUS_CONTROL       1
#define PERF_TUNE_VOLTAGE_IO_CONTROL          2

// Clock Device Control Types
#define PERF_TUNE_CLOCK_SMBUS_CONTROL         0

// Thermal Device Control Types
#define PERF_TUNE_THERMAL_HECETA_CONTROL      0
#define PERF_TUNE_THERMAL_QST_CONTROL         1

// TACH_USAGE Enumeration
#define TACH_USAGE_UNKNOWN                    0x00
#define TACH_USAGE_CPU                        0x01
#define TACH_USAGE_CPU_SYSTEM                 0x02
#define TACH_USAGE_MCH                        0x03
#define TACH_USAGE_VOLTAGE_REGULATOR          0x04
#define TACH_USAGE_CHASSIS                    0x05
#define TACH_USAGE_CHASSIS_INLET              0x06
#define TACH_USAGE_CHASSIS_OUTLET             0x07
#define TACH_USAGE_POWER_SUPPLY               0x08
#define TACH_USAGE_POWER_SUPPLY_INLET         0x09
#define TACH_USAGE_POWER_SUPPLY_OUTLET        0x0A
#define TACH_USAGE_HARD_DISK                  0x0B
#define TACH_USAGE_GRAPHICS                   0x0C
#define TACH_USAGE_AUXILIARY                  0x0D
#define TACH_USAGE_UNUSED                     0xFF

// TEMP_USAGE Enumeration
#define TEMP_USAGE_UNKNOWN                    0x00
#define TEMP_USAGE_CPU_CORE                   0x01
#define TEMP_USAGE_CPU_DIE                    0x02
#define TEMP_USAGE_ICH                        0x03
#define TEMP_USAGE_MCH                        0x04
#define TEMP_USAGE_VOLTAGE_REGULATOR          0x05
#define TEMP_USAGE_MEMORY                     0x06
#define TEMP_USAGE_MOTHERBOARD_AMBIENT        0x07
#define TEMP_USAGE_SYSTEM_AMBIENT             0x08
#define TEMP_USAGE_CPU_INLET                  0x09
#define TEMP_USAGE_SYTSEM_INLET               0x0A
#define TEMP_USAGE_SYSTEM_OUTLET              0x0B
#define TEMP_USAGE_POWER_SUPPLY               0x0C
#define TEMP_USAGE_POWER_SUPPLY_INLET         0x0D
#define TEMP_USAGE_POSER_SUPPLY_OUTLET        0x0E
#define TEMP_USAGE_HARD_DRIVE                 0x0F
#define TEMP_USAGE_GPU                        0x10

// BIOS Settings Control Types
#define PERF_TUNE_BIOS_DISCRETE_CONTROL       0
#define PERF_TUNE_BIOS_CONTINUOUS_CONTROL     1
#define PERF_TUNE_HIGH_PRECISION_LOGICAL_BIOS_SETTING     2

// Voltage Device Implementation Types
#define PERF_TUNE_VOLTAGE_CPU_CONTROL         0
#define PERF_TUNE_VOLTAGE_MCH_CONTROL         1
#define PERF_TUNE_VOLTAGE_FSB_CONTROL         2
#define PERF_TUNE_VOLTAGE_ICH_CONTROL         3
#define PERF_TUNE_VOLTAGE_MEMORY_CONTROL      4
#define PERF_TUNE_VOLTAGE_IOH_CONTROL         5
#define PERF_TUNE_VOLTAGE_QPI_CONTROL         6
#define PERF_TUNE_VOLTAGE_UNCORE              7

// Clock Device Implementation Types
#define PERF_TUNE_CLOCK_HOST_CONTROL          0
#define PERF_TUNE_CLOCK_PCI_EXPRESS_CONTROL   1
#define PERF_TUNE_CLOCK_PCI_CONTROL           2

// BIOS DATA TYPE ENUMERATION
#define NO_DATA_TYPE                          0
#define MHz_DATA_TYPE                         1
#define GHz_DATA_TYPE                         2
#define Volts_DATA_TYPE                       3
#define Clocks_DATA_TYPE                      4
#define BIOS_Setup_Enable                     5
#define BIOS_Setup_Disable                    6
#define BIOS_Setup_Amps                       7
#define BIOS_Setup_Watts                      8

// DATA CONTROL TYPE
#define SETUP_VALUE                           0
#define DATA_VALUE                            1

//Setting Flags
#define FLAG_POSITIVE                         0
#define FLAG_NEGATIVE                         1

// Minimum Setting / Performance Flag 
#define FLAG_MIN_IS_LOWEST                    0
#define FLAG_MIN_IS_HIGHEST                   1

// SMI Bios Settings
#define BIOS_DEVICE_AUTOMATIC                 0xFFFFFFFE
#define BIOS_DEVICE_DISABLE                   0xFFFFFFFF


// Performance Tuning Table Signatures
#define SPTT_TABLE_SIG                            SIGNATURE_32('S', 'P', 'T', 'T')
#define DDD_TABLE_SIG                             SIGNATURE_32('$', 'D', 'D', 'D')
#define IO_VOLTAGE_DEVICE_SIG                     SIGNATURE_32('I', 'O', 'V', 'X')
#define DISCRETE_BIOS_SETTINGS_DEVICE_SIG         SIGNATURE_32('$', 'L', 'B', 'S')
#define CONTINUOUS_BIOS_SETTINGS_DEVICE_SIG       SIGNATURE_32('$', 'L', 'B', 'C')
#define HIGH_PRECISION_BIOS_SETTINGS_DEVICE_SIG   SIGNATURE_32('H', 'L', 'B', 'C')

//////#define AMRT_TABLE_SIG                            SIGNATURE_32('A', 'M', 'R', 'T')
#define SMBUS_CLOCK_DEVICE_SIG                    SIGNATURE_32('S', 'M', 'C', 'D')
#define SMBUS_VOLTAGE_DEVICE_SIG                  SIGNATURE_32('S', 'M', 'V', 'D')
#define HECETA_THERMAL_DEVICE_SIG                 SIGNATURE_32('H', 'E', 'A', 'T')
#define GPIO_VOLTAGE_DEVICE_SIG                   SIGNATURE_32('S', 'I', 'O', 'V')


// Automode Settings
#define AUTOMODE_NOT_SUPPORTED  0
#define AUTOMODE_SUPPORTED      1

// An arbitrarily large amount of space
#define DEFAULT_BUFFER_SIZE 0x18000

// Number of Memory Regions that need to be accessed by Iron City
#define NUM_MEM_REGIONS   1

//[-start-120217-IB03780424-remove]//
// Memory Frequency
//#define MEMORY_FREQUENCY_AUTO_MODE 0
//#define MEMORY_FREQUENCY_OFFSET    3
//[-end-120217-IB03780424-remove]//

//////////////////////////////////////////////////////////////
//  Performance Tuning Protocol Definitions
//////////////////////////////////////////////////////////////

/**
 This routine returns a set of DDD Tables to be included within the SPTT

 @param [out]  BufferStart      The beginning of a buffer containing DDD tables to be added to the SPTT
 @param [out]  BufferLength     The length of the buffer to be added

 @retval EFI_ABORTED
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *SPTT_GET_DATA) (
  OUT UINT8          **BufferStart,
  OUT UINTN          *BufferLength
  );

#pragma pack(1)

//
// Interface structure for the Flex FV protocol
//
struct _SPTT_DATA_PROTOCOL {

  SPTT_GET_DATA                GetData;

};

extern EFI_GUID gSpttDataProtocolGuid;

#pragma pack()


/**
 This routine returns a Discrete BIOS Setting DDD table

 @param [in]   RawDiscreteBiosSettingList  Table defined raw data of discrete bios settings
 @param [out]  BiosSettingTable  This is a pointer to the DDD table produced
 @param [out]  TableSize        This is the size of the DDD table produced

 @retval EFI_ABORTED
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *BUILD_DISCRETE_BIOS_SETTING_TABLE) (
  IN  RAW_DDD_LBS_DATA            *RawDiscreteBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );

/**
 This routine returns a Continuous BIOS Setting DDD table

 @param [in]   RawContinueBiosSettingList  Table defined raw data of continue bios settings
 @param [out]  BiosSettingTable  This is a pointer to the DDD table produced
 @param [out]  TableSize        This is the size of the DDD table produced

 @retval EFI_ABORTED
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *BUILD_CONTINUOUS_BIOS_SETTING_TABLE) (
  IN  RAW_DDD_LBC_DATA            *RawContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );

/**
 This routine returns a High Precision Continuous BIOS Setting DDD table

 @param [in]   RawHpContinueBiosSettingList   Table defined raw data of continue bios settings
 @param [out]  BiosSettingTable               This is a pointer to the DDD table produced
 @param [out]  TableSize                      This is the size of the DDD table produced

 @retval EFI_ABORTED
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *BUILD_HP_CONTINUOUS_BIOS_SETTING_TABLE) (
  IN  RAW_DDD_HLBC_DATA           *RawHpContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );


/**
 This routine reallocates the memory necessary for a specific buffer size if the current buffer is not large enough

 @param [in]   DataBuffer       This is a pointer to the head of the list to be deleted.
 @param [in]   DataLen          This is the size of the requested buffer
 @param [in, out] BufferSize    On input this is the size of the current buffer, On output this is the size of the new buffer

 @retval EFI_OUT_OF_RESOURCES
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *REALLOC_ON_BUFFER_OVERFLOW) (
  IN UINT8                    **DataBuffer,
  IN UINTN                    DataLen,
  IN OUT UINTN                *BufferSize
  );


/**
 This routine return a BIOS_SETTINGS_DATA_STRUCT from protocol

 @param [out]  BiosSettingsData  This is a pointer to receive the bios data.
 @param [in]   refresh

 @retval EFI_ABORTED
 @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *GET_BIOS_SETTINGS_DATA) (
  OUT BIOS_SETTINGS_DATA_STRUCT   *BiosSettingsData,
  IN  BOOLEAN                     refresh
  );

#pragma pack(1)

//
// Interface structure for the Flex FV protocol
//
typedef struct {

  BUILD_DISCRETE_BIOS_SETTING_TABLE                 BuildDiscreteBiosSettingTable;
  BUILD_CONTINUOUS_BIOS_SETTING_TABLE               BuildContinuousBiosSettingTable;
  BUILD_HP_CONTINUOUS_BIOS_SETTING_TABLE            BuildHpContinuousBiosSettingTable;
  REALLOC_ON_BUFFER_OVERFLOW                        ReallocIfBufferOverflow;
} SPTT_TABLE_SERVICES_PROTOCOL;

extern EFI_GUID gSpttDataProtocolGuid;
extern EFI_GUID gSpttTableServicesProtocolGuid;

#pragma pack()


#endif // multiple inclusion guard

