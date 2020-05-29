/** @file
  
  SPD data format header file.  
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MrcSpdData_h_
#define _MrcSpdData_h_
#pragma pack (push, 1)

#include <MrcTypes.h>

#define MAX_XMP_PROFILES  (2)
#define SPD3_MANUF_SIZE   (SPD3_MANUF_END - SPD3_MANUF_START + 1) ///< The size of the SPD manufacturing data.
typedef union {
  struct {
    U8  BytesUsed                           :  4; ///< Bits 3:0
    U8  BytesTotal                          :  3; ///< Bits 6:4
    U8  CrcCoverage                         :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_DEVICE_DESCRIPTION_STRUCT;

typedef union {
  struct {
    U8  Minor                               :  4; ///< Bits 3:0
    U8  Major                               :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_REVISION_STRUCT;

typedef union {
  struct {
    U8  Type                                :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_DRAM_DEVICE_TYPE_STRUCT;

typedef union {
  struct {
    U8  ModuleType                          :  4; ///< Bits 3:0
    U8                                      :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_MODULE_TYPE_STRUCT;

typedef union {
  struct {
    U8  Density                             :  4; ///< Bits 3:0
    U8  BankAddress                         :  3; ///< Bits 6:4
    U8                                      :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_SDRAM_DENSITY_BANKS_STRUCT;

typedef union {
  struct {
    U8  ColumnAddress                       :  3; ///< Bits 2:0
    U8  RowAddress                          :  3; ///< Bits 5:3
    U8                                      :  2; ///< Bits 7:6
  } Bits;
  U8  Data;
} SPD_SDRAM_ADDRESSING_STRUCT;

typedef union {
  struct {
    U8  OperationAt1_50                     :  1; ///< Bits 0:0
    U8  OperationAt1_35                     :  1; ///< Bits 1:1
    U8  OperationAt1_25                     :  1; ///< Bits 2:2
    U8                                      :  5; ///< Bits 7:3
  } Bits;
  U8  Data;
} SPD_MODULE_NOMINAL_VOLTAGE_STRUCT;

typedef union {
  struct {
    U8  SdramDeviceWidth                    :  3; ///< Bits 2:0
    U8  RankCount                           :  3; ///< Bits 5:3
    U8                                      :  2; ///< Bits 7:6
  } Bits;
  U8  Data;
} SPD_MODULE_ORGANIZATION_STRUCT;

typedef union {
  struct {
    U8  PrimaryBusWidth                     :  3; ///< Bits 2:0
    U8  BusWidthExtension                   :  2; ///< Bits 4:3
    U8                                      :  3; ///< Bits 7:5
  } Bits;
  U8  Data;
} SPD_MODULE_MEMORY_BUS_WIDTH_STRUCT;

typedef union {
  struct {
    U8  Divisor                             :  4; ///< Bits 3:0
    U8  Dividend                            :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_FINE_TIMEBASE_STRUCT;

typedef union {
  struct {
    U8  Dividend                            :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_MEDIUM_TIMEBASE_DIVIDEND_STRUCT;

typedef union {
  struct {
    U8  Divisor                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_MEDIUM_TIMEBASE_DIVISOR_STRUCT;

typedef struct {
  SPD_MEDIUM_TIMEBASE_DIVIDEND_STRUCT Dividend; ///< Medium Timebase (MTB) Dividend
  SPD_MEDIUM_TIMEBASE_DIVISOR_STRUCT  Divisor;  ///< Medium Timebase (MTB) Divisor
} SPD_MEDIUM_TIMEBASE;

typedef union {
  struct {
    U8  tCKmin                              :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TCK_MIN_MTB_STRUCT;

typedef union {
  struct {
    U16 CL4                                 :  1; ///< Bits 0:0
    U16 CL5                                 :  1; ///< Bits 1:1
    U16 CL6                                 :  1; ///< Bits 2:2
    U16 CL7                                 :  1; ///< Bits 3:3
    U16 CL8                                 :  1; ///< Bits 4:4
    U16 CL9                                 :  1; ///< Bits 5:5
    U16 CL10                                :  1; ///< Bits 6:6
    U16 CL11                                :  1; ///< Bits 7:7
    U16 CL12                                :  1; ///< Bits 8:8
    U16 CL13                                :  1; ///< Bits 9:9
    U16 CL14                                :  1; ///< Bits 10:10
    U16 CL15                                :  1; ///< Bits 11:11
    U16 CL16                                :  1; ///< Bits 12:12
    U16 CL17                                :  1; ///< Bits 13:13
    U16 CL18                                :  1; ///< Bits 14:14
    U16                                     :  1; ///< Bits 15:15
  } Bits;
  U16 Data;
  U8  Data8;
} SPD_CAS_LATENCIES_SUPPORTED_STRUCT;

typedef union {
  struct {
    U8  tAAmin                              :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TAA_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tWRmin                              :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TWR_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRCDmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRCD_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRRDmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRRD_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRPmin                              :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRP_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRPab                               :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRP_AB_MTB_STRUCT;

typedef union {
  struct {
    S8  tRPabFine                           :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TRP_AB_FTB_STRUCT;

typedef union {
  struct {
    U8  tRASminUpper                        :  4; ///< Bits 3:0
    U8  tRCminUpper                         :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_TRAS_TRC_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRASmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRAS_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRCmin                              :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRC_MIN_MTB_STRUCT;

typedef union {
  struct {
    U16 tRFCmin                             :  16; ///< Bits 15:0
  } Bits;
  U16 Data;
  U8  Data8;
} SPD_TRFC_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tWTRmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TWTR_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tRTPmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TRTP_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tFAWminUpper                        :  4; ///< Bits 3:0
    U8                                      :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_TFAW_MIN_MTB_UPPER_STRUCT;

typedef union {
  struct {
    U8  tFAWmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TFAW_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  tCWLmin                             :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_TCWL_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  NMode                               :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_SYSTEM_COMMAND_RATE_STRUCT;

typedef union {
  struct {
    U16 tREFImin                            :  16; ///< Bits 15:0
  } Bits;
  U16 Data;
  U8  Data8;
} SPD_TREFI_MIN_MTB_STRUCT;

typedef union {
  struct {
    U8  RZQ6                                :  1; ///< Bits 0:0
    U8  RZQ7                                :  1; ///< Bits 1:1
    U8                                      :  5; ///< Bits 6:2
    U8  DllOff                              :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_SDRAM_OPTIONAL_FEATURES_STRUCT;

typedef union {
  struct {
    U8  ExtendedTemperatureRange            :  1; ///< Bits 0:0
    U8  ExtendedTemperatureRefreshRate      :  1; ///< Bits 1:1
    U8  AutoSelfRefresh                     :  1; ///< Bits 2:2
    U8  OnDieThermalSensor                  :  1; ///< Bits 3:3
    U8                                      :  3; ///< Bits 6:4
    U8  PartialArraySelfRefresh             :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_SDRAM_THERMAL_REFRESH_STRUCT;

typedef union {
  struct {
    U8  ThermalSensorAccuracy               :  7; ///< Bits 6:0
    U8  ThermalSensorPresence               :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_MODULE_THERMAL_SENSOR_STRUCT;

typedef union {
  struct {
    U8  NonStandardDeviceDescription        :  7; ///< Bits 6:0
    U8  SdramDeviceType                     :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_SDRAM_DEVICE_TYPE_STRUCT;

typedef union {
  struct {
    U8                                      :  8; ///< Bits 7:0
  } Bits;
  U8  Data;
} SPD_AUTO_SELF_REFRESH_PERF_STRUCT;

typedef union {
  struct {
    S8  tCKminFine                          :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TCK_MIN_FTB_STRUCT;

typedef union {
  struct {
    S8  tAAminFine                          :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TAA_MIN_FTB_STRUCT;

typedef union {
  struct {
    S8  tRCDminFine                         :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TRCD_MIN_FTB_STRUCT;

typedef union {
  struct {
    S8  tRPminFine                          :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TRP_MIN_FTB_STRUCT;

typedef union {
  struct {
    S8  tRCminFine                          :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TRC_MIN_FTB_STRUCT;

typedef union {
  struct {
    S8  tRRDminFine                         :  8; ///< Bits 7:0
  } Bits;
  S8  Data;
} SPD_TRRD_MIN_FTB_STRUCT;

typedef union {
  struct {
    U8  Height                              :  5; ///< Bits 4:0
    U8  RawCardExtension                    :  3; ///< Bits 7:5
  } Bits;
  U8  Data;
} SPD_UNBUF_MODULE_NOMINAL_HEIGHT;

typedef union {
  struct {
    U8  FrontThickness                      :  4; ///< Bits 3:0
    U8  BackThickness                       :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_UNBUF_MODULE_NOMINAL_THICKNESS;

typedef union {
  struct {
    U8  Card                                :  5; ///< Bits 4:0
    U8  Revision                            :  2; ///< Bits 6:5
    U8  Extension                           :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_UNBUF_REFERENCE_RAW_CARD;

typedef union {
  struct {
    U8  MappingRank1                        :  1; ///< Bits 0:0
    U8                                      :  7; ///< Bits 7:1
  } Bits;
  U8  Data;
} SPD_UNBUF_ADDRESS_MAPPING;

typedef union {
  struct {
    U8  Height                              :  5; ///< Bits 4:0
    U8                                      :  3; ///< Bits 7:5
  } Bits;
  U8  Data;
} SPD_RDIMM_MODULE_NOMINAL_HEIGHT;

typedef union {
  struct {
    U8  FrontThickness                      :  4; ///< Bits 3:0
    U8  BackThickness                       :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_RDIMM_MODULE_NOMINAL_THICKNESS;

typedef union {
  struct {
    U8  Card                                :  5; ///< Bits 4:0
    U8  Revision                            :  2; ///< Bits 6:5
    U8  Extension                           :  1; ///< Bits 7:7
  } Bits;
  U8  Data;
} SPD_RDIMM_REFERENCE_RAW_CARD;

typedef union {
  struct {
    U8  RegisterCount                       :  2; ///< Bits 1:0
    U8  DramRowCount                        :  2; ///< Bits 3:2
    U8                                      :  4; ///< Bits 7:4
  } Bits;
  U8  Data;
} SPD_RDIMM_MODULE_ATTRIBUTES;

typedef union {
  struct {
    U16 ContinuationCount                   :  7; ///< Bits 6:0
    U16 ContinuationParity                  :  1; ///< Bits 7:7
    U16 LastNonZeroByte                     :  8; ///< Bits 15:8
  } Bits;
  U16 Data;
  U8  Data8;
} SPD_MANUFACTURER_ID_CODE;

typedef struct {
  U8  Year;                                 ///< Year represented in BCD (00h = 2000)
  U8  Week;                                 ///< Year represented in BCD (47h = week 47)
} SPD_MANUFACTURING_DATE;

typedef union {
  U32 Data;
  U16 SerialNumber16[2];
  U8  SerialNumber8[4];
} SPD_MANUFACTURER_SERIAL_NUMBER;

typedef union {
  U16 Crc[1];
  U8  Data8[2];
} SPD_CYCLIC_REDUNDANCY_CODE;

typedef union {
  struct {
    U8  ProfileEnable1                :  1;                     ///< Bits 0:0
    U8  ProfileEnable2                :  1;                     ///< Bits 1:1
    U8  ProfileConfig1                :  2;                     ///< Bits 3:2
    U8  ProfileConfig2                :  2;                     ///< Bits 5:4
    U8                                :  2;                     ///< Bits 7:6
  } Bits;
  U8  Data;
} SPD_XMP_ORG_CONFIG;

typedef struct {
  U16                                 XmpId;                    ///< 176-177 XMP Identification String
  SPD_XMP_ORG_CONFIG                  XmpOrgConf;               ///< 178 XMP Organization & Configuration
  SPD_REVISION_STRUCT                 XmpRevision;              ///< 179 XMP Revision
  SPD_MEDIUM_TIMEBASE                 MediumTimeBase[MAX_XMP_PROFILES]; ///< 180-183 Medium Timebase (MTB)
  SPD_FINE_TIMEBASE_STRUCT            FineTimeBase;             ///< 184 Fine Timebase (FTB) Dividend / Divisor
} SPD_EXTREME_MEMORY_PROFILE_HEADER;

typedef union {
  struct {
    U8  Decimal : 5;
    U8  Integer : 2;
    U8          : 1;
  } Bits;
  U8 Data;
} SPD_VDD_VOLTAGE_LEVEL_STRUCT;

typedef struct {
  SPD_VDD_VOLTAGE_LEVEL_STRUCT        Vdd;                      ///< 185, 220 XMP Module VDD Voltage Level
  SPD_TCK_MIN_MTB_STRUCT              tCKmin;                   ///< 186, 221 XMP SDRAM Minimum Cycle Time (tCKmin)
  SPD_TAA_MIN_MTB_STRUCT              tAAmin;                   ///< 187, 222 XMP Minimum CAS Latency Time (tAAmin)
  SPD_CAS_LATENCIES_SUPPORTED_STRUCT  CasLatencies;             ///< 188-189, 223-224 XMP CAS Latencies Supported, Least Significant Byte
  SPD_TCWL_MIN_MTB_STRUCT             tCWLmin;                  ///< 190, 225 XMP Minimum CAS Write Latency Time (tCWLmin)
  SPD_TRP_MIN_MTB_STRUCT              tRPmin;                   ///< 191, 226 XMP Minimum Row Precharge Delay Time (tRPmin)
  SPD_TRCD_MIN_MTB_STRUCT             tRCDmin;                  ///< 192, 227 XMP Minimum RAS# to CAS# Delay Time (tRCDmin)
  SPD_TWR_MIN_MTB_STRUCT              tWRmin;                   ///< 193, 228 XMP Minimum Write Recovery Time (tWRmin)
  SPD_TRAS_TRC_MIN_MTB_STRUCT         tRASMintRCMinUpper;       ///< 194, 229 XMP Upper Nibbles for tRAS and tRC
  SPD_TRAS_MIN_MTB_STRUCT             tRASmin;                  ///< 195, 230 XMP Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
  SPD_TRC_MIN_MTB_STRUCT              tRCmin;                   ///< 196, 231 XMP Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
  SPD_TREFI_MIN_MTB_STRUCT            tREFImin;                 ///< 197-198, 232-233 XMP Maximum tREFI Time (Average Periodic Refresh Interval), Least Significant Byte
  SPD_TRFC_MIN_MTB_STRUCT             tRFCmin;                  ///< 199-200, 234-235 XMP Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
  SPD_TRTP_MIN_MTB_STRUCT             tRTPmin;                  ///< 201, 236 XMP Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
  SPD_TRRD_MIN_MTB_STRUCT             tRRDmin;                  ///< 202, 237 XMP Minimum Row Active to Row Active Delay Time (tRRDmin)
  SPD_TFAW_MIN_MTB_UPPER_STRUCT       tFAWMinUpper;             ///< 203, 238 XMP Upper Nibble for tFAW
  SPD_TFAW_MIN_MTB_STRUCT             tFAWmin;                  ///< 204, 239 XMP Minimum Four Activate Window Delay Time (tFAWmin)
  SPD_TWTR_MIN_MTB_STRUCT             tWTRmin;                  ///< 205, 240 XMP Minimum Internal Write to Read Command Delay Time (tWTRmin)
  U8                                  Reserved1[207 - 206 + 1]; ///< 206-207, 241-242 XMP Reserved
  SPD_SYSTEM_COMMAND_RATE_STRUCT      SystemCmdRate;            ///< 208, 243 XMP System ADD/CMD Rate (1N or 2N mode)
  SPD_AUTO_SELF_REFRESH_PERF_STRUCT   AsrPerf;                  ///< 209, 244 XMP SDRAM Auto Self Refresh Performance (Sub 1x Refresh and IDD6 impact)
  U8                                  VoltageLevel;             ///< 210, 245 XMP Memory Controller Voltage Level
  SPD_TCK_MIN_FTB_STRUCT              tCKminFine;               ///< 211, 246 XMP Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  SPD_TAA_MIN_FTB_STRUCT              tAAminFine;               ///< 212, 247 XMP Fine Offset for Minimum CAS Latency Time (tAAmin)
  SPD_TRP_MIN_FTB_STRUCT              tRPminFine;               ///< 213, 248 XMP Minimum Row Precharge Delay Time (tRPmin)
  SPD_TRCD_MIN_FTB_STRUCT             tRCDminFine;              ///< 214, 249 XMP Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  SPD_TRC_MIN_FTB_STRUCT              tRCminFine;               ///< 215, 250 XMP Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
  U8                                  Reserved2[218 - 216 + 1]; ///< 216-218, 251-253 XMP Reserved
  U8                                  VendorPersonality;        ///< 219, 254 XMP Vendor Personality
} SPD_EXTREME_MEMORY_PROFILE_DATA;

typedef struct {
  SPD_EXTREME_MEMORY_PROFILE_HEADER   Header;                   ///< 176-184 XMP header
  SPD_EXTREME_MEMORY_PROFILE_DATA     Data[MAX_XMP_PROFILES];   ///< 185-254 XMP profiles
} SPD_EXTREME_MEMORY_PROFILE;

typedef struct {
  SPD_DEVICE_DESCRIPTION_STRUCT       Description;              ///< 0   Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage 1, 2
  SPD_REVISION_STRUCT                 Revision;                 ///< 1   SPD Revision
  SPD_DRAM_DEVICE_TYPE_STRUCT         DramDeviceType;           ///< 2   DRAM Device Type
  SPD_MODULE_TYPE_STRUCT              ModuleType;               ///< 3   Module Type
  SPD_SDRAM_DENSITY_BANKS_STRUCT      SdramDensityAndBanks;     ///< 4   SDRAM Density and Banks
  SPD_SDRAM_ADDRESSING_STRUCT         SdramAddressing;          ///< 5   SDRAM Addressing
  SPD_MODULE_NOMINAL_VOLTAGE_STRUCT   ModuleNominalVoltage;     ///< 6   Module Nominal Voltage, VDD
  SPD_MODULE_ORGANIZATION_STRUCT      ModuleOrganization;       ///< 7   Module Organization
  SPD_MODULE_MEMORY_BUS_WIDTH_STRUCT  ModuleMemoryBusWidth;     ///< 8   Module Memory Bus Width
  SPD_FINE_TIMEBASE_STRUCT            FineTimebase;             ///< 9   Fine Timebase (FTB) Dividend / Divisor
  SPD_MEDIUM_TIMEBASE                 MediumTimebase;           ///< 10-11 Medium Timebase (MTB) Dividend
  SPD_TCK_MIN_MTB_STRUCT              tCKmin;                   ///< 12  SDRAM Minimum Cycle Time (tCKmin)
  U8                                  Reserved1;                ///< 13  Reserved
  SPD_CAS_LATENCIES_SUPPORTED_STRUCT  CasLatencies;             ///< 14-15 CAS Latencies Supported
  SPD_TAA_MIN_MTB_STRUCT              tAAmin;                   ///< 16  Minimum CAS Latency Time (tAAmin)
  SPD_TWR_MIN_MTB_STRUCT              tWRmin;                   ///< 17  Minimum Write Recovery Time (tWRmin)
  SPD_TRCD_MIN_MTB_STRUCT             tRCDmin;                  ///< 18  Minimum RAS# to CAS# Delay Time (tRCDmin)
  SPD_TRRD_MIN_MTB_STRUCT             tRRDmin;                  ///< 19  Minimum Row Active to Row Active Delay Time (tRRDmin)
  SPD_TRP_MIN_MTB_STRUCT              tRPmin;                   ///< 20  Minimum Row Precharge Delay Time (tRPmin)
  SPD_TRAS_TRC_MIN_MTB_STRUCT         tRASMintRCMinUpper;       ///< 21  Upper Nibbles for tRAS and tRC
  SPD_TRAS_MIN_MTB_STRUCT             tRASmin;                  ///< 22  Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
  SPD_TRC_MIN_MTB_STRUCT              tRCmin;                   ///< 23  Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
  SPD_TRFC_MIN_MTB_STRUCT             tRFCmin;                  ///< 24-25  Minimum Refresh Recovery Delay Time (tRFCmin)
  SPD_TWTR_MIN_MTB_STRUCT             tWTRmin;                  ///< 26  Minimum Internal Write to Read Command Delay Time (tWTRmin)
  SPD_TRTP_MIN_MTB_STRUCT             tRTPmin;                  ///< 27  Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
  SPD_TFAW_MIN_MTB_UPPER_STRUCT       tFAWMinUpper;             ///< 28  Upper Nibble for tFAW
  SPD_TFAW_MIN_MTB_STRUCT             tFAWmin;                  ///< 29  Minimum Four Activate Window Delay Time (tFAWmin)
  SPD_SDRAM_OPTIONAL_FEATURES_STRUCT  SdramOptionalFeatures;    ///< 30  SDRAM Optional Features
  SPD_SDRAM_THERMAL_REFRESH_STRUCT    ThermalAndRefreshOptions; ///< 31  SDRAMThermalAndRefreshOptions
  SPD_MODULE_THERMAL_SENSOR_STRUCT    ModuleThermalSensor;      ///< 32  Module Thermal Sensor
  SPD_SDRAM_DEVICE_TYPE_STRUCT        SdramDeviceType;          ///< 33  SDRAM Device Type
  SPD_TCK_MIN_FTB_STRUCT              tCKminFine;               ///< 34  Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  SPD_TAA_MIN_FTB_STRUCT              tAAminFine;               ///< 35  Fine Offset for Minimum CAS Latency Time (tAAmin)
  SPD_TRCD_MIN_FTB_STRUCT             tRCDminFine;              ///< 36  Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  SPD_TRP_MIN_FTB_STRUCT              tRPminFine;               ///< 37  Minimum Row Precharge Delay Time (tRPmin)
  SPD_TRC_MIN_FTB_STRUCT              tRCminFine;               ///< 38  Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
#if (SUPPORT_LPDDR3 == SUPPORT)
  SPD_TRP_AB_MTB_STRUCT               tRPab;                    ///< 39  Minimum Row Precharge Delay Time for all banks (tRPab)
  SPD_TRP_AB_FTB_STRUCT               tRPabFine;                ///< 40  Fine Offset for Minimum Row Precharge Delay Time for all banks (tRPab)
  U8                                  Reserved2[59 - 41 + 1];   ///< 41 - 59 Reserved
#else
  U8                                  Reserved2[59 - 39 + 1];   ///< 39 - 59 Reserved
#endif
} SPD_GENERAL_SECTION;

typedef struct {
  SPD_UNBUF_MODULE_NOMINAL_HEIGHT     ModuleNominalHeight;      ///< 60 Module Nominal Height
  SPD_UNBUF_MODULE_NOMINAL_THICKNESS  ModuleMaximumThickness;   ///< 61 Module Maximum Thickness
  SPD_UNBUF_REFERENCE_RAW_CARD        ReferenceRawCardUsed;     ///< 62 Reference Raw Card Used
  SPD_UNBUF_ADDRESS_MAPPING           AddressMappingEdgeConn;   ///< 63 Address Mapping from Edge Connector to DRAM
  U8                                  Reserved[116 - 64 + 1];   ///< 64-116 Reserved
} SPD_MODULE_UNBUFFERED;

typedef struct {
  SPD_RDIMM_MODULE_NOMINAL_HEIGHT     ModuleNominalHeight;      ///< 60 Module Nominal Height
  SPD_RDIMM_MODULE_NOMINAL_THICKNESS  ModuleMaximumThickness;   ///< 61 Module Maximum Thickness
  SPD_RDIMM_REFERENCE_RAW_CARD        ReferenceRawCardUsed;     ///< 62 Reference Raw Card Used
  SPD_RDIMM_MODULE_ATTRIBUTES         DimmModuleAttributes;     ///< 63 DIMM Module Attributes
  U8                                  Reserved[116 - 64 + 1];   ///< 64-116 Reserved
} SPD_MODULE_REGISTERED;

typedef union {
  SPD_MODULE_UNBUFFERED               Unbuffered;
  SPD_MODULE_REGISTERED               Registered;
} SPD_MODULE_SPECIFIC;

typedef struct {
  U8                                  Location;                 ///< 119 Module Manufacturing Location
} SPD_MANUFACTURING_LOCATION;

typedef struct {
  SPD_MANUFACTURER_ID_CODE            IdCode;                   ///< 117-118 Module Manufacturer ID Code
  SPD_MANUFACTURING_LOCATION          Location;                 ///< 119 Module Manufacturing Location
  SPD_MANUFACTURING_DATE              Date;                     ///< 120-121 Module Manufacturing Year, in BCD (range: 2000-2255)
  SPD_MANUFACTURER_SERIAL_NUMBER      SerialNumber;             ///< 122-125 Module Serial Number
} SPD_UNIQUE_MODULE_ID;

typedef struct {
  U8                          ModulePartNumber[145 - 128 + 1];        ///< 128-145 Module Part Number
} SPD_MODULE_PART_NUMBER;

typedef struct {
  U8                          ModuleRevisionCode[147 - 146 + 1];      ///< 146-147 Module Revision Code
} SPD_MODULE_REVISION_CODE;

typedef struct {
  U8                          ManufactureSpecificData[175 - 150 + 1]; ///< 150-175 Manufacturer's Specific Data
} SPD_MANUFACTURE_SPECIFIC;

///
/// DDR3 Serial Presence Detect structure
///
typedef struct {
  SPD_GENERAL_SECTION         General;                                ///< 0-59 General Section
  SPD_MODULE_SPECIFIC         Module;                                 ///< 60-116 Module-Specific Section
  SPD_UNIQUE_MODULE_ID        ModuleId;                               ///< 117-125 Unique Module ID
  SPD_CYCLIC_REDUNDANCY_CODE  Crc;                                    ///< 126-127 Cyclical Redundancy Code (CRC)
  SPD_MODULE_PART_NUMBER      ModulePartNumber;                       ///< 128-145 Module Part Number
  SPD_MODULE_REVISION_CODE    ModuleRevisionCode;                     ///< 146-147 Module Revision Code
  SPD_MANUFACTURER_ID_CODE    DramIdCode;                             ///< 148-149 Dram Manufacturer ID Code
  SPD_MANUFACTURE_SPECIFIC    ManufactureSpecificData;                ///< 150-175 Manufacturer's Specific Data
  SPD_EXTREME_MEMORY_PROFILE  Xmp;                                    ///< 176-254 Intel(r) Extreme Memory Profile support
  U8                          Reserved;                               ///< 255 Reserved
} MrcSpdDdr3;
typedef union {
  MrcSpdDdr3  Ddr3;
} MrcSpd;

typedef struct {
  SPD_MODULE_TYPE_STRUCT              ModuleType;           ///< 3       Module Type
  SPD_MODULE_MEMORY_BUS_WIDTH_STRUCT  ModuleMemoryBusWidth; ///< 8/14    Module Memory Bus Width
} SMBIOS_SPD_SAVE;

typedef struct {
  SPD_UNIQUE_MODULE_ID                ModuleId;             ///< 117-125 Unique Module ID
  SPD_CYCLIC_REDUNDANCY_CODE          Crc;                  ///< 126-127 Cyclical Redundancy Code (CRC)
  SPD_MODULE_PART_NUMBER              ModulePartNumber;     ///< 128-145 Module Part Number
  SPD_MODULE_REVISION_CODE            ModuleRevisionCode;   ///< 146-147 Module Revision Code
} MANUFACTURING_SPD_SAVE_DDR3;

typedef union {
  MANUFACTURING_SPD_SAVE_DDR3 Ddr3Data;
} MANUFACTURING_SPD_SAVE;

typedef struct {
  SMBIOS_SPD_SAVE         SmbiosData;
  MANUFACTURING_SPD_SAVE  ManufacturingData;
} MrcSpdSave;

#pragma pack (pop)
#endif // _MrcSpdData_h_
