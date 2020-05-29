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

  H2OIpmiFru.h

Abstract:

  Header file that contains IPMI SDR Device Commands, data structures and
  definitions

--*/

#ifndef _H2O_IPMI_FRU_H_
#define _H2O_IPMI_FRU_H_
//
// Multi Record Id
//
#define FRU_RECORD_POWER_SUPPLY_INFO      0x00
#define FRU_RECORD_DC_OUTPUT              0x01
#define FRU_RECORD_DC_LOAD                0x02
#define FRU_RECORD_MANAGE_ACCESS          0x03
#define FRU_RECORD_BASE_COMPABILITY       0x04
#define FRU_RECORD_EXTENDED_COMPABILITY   0x05
#define FRU_RECORD_IGNORE_RECORDTYPE      0xBF
#define FRU_RECORD_OEM_MIN                0xC0
#define FRU_RECORD_OEM_MAX                0xFF

#pragma pack (1)

//
// Power Supply Information (Record Type 0x00)
//
typedef struct _FRU_POWER_SUPPLY {
  UINT16          Reserved;
  UINT16          PeakVA;
  UINT8           InrushCurrent;
  UINT8           InrushInterval;
  UINT16          LowInVol1;
  UINT16          HighInVol1;
  UINT16          LowInVol2;
  UINT16          HighInVol2;
  UINT8           LowInFrequency;
  UINT8           HighInFrequency;
  UINT8           AC_DropOut;
  UINT8           PredictiveFailSupport   :1;
  UINT8           PowerFactorCorrection   :1;
  UINT8           Autoswitch              :1;
  UINT8           HotSwapSupport          :1;
  UINT8           TachometerPulses        :1;
  UINT8           Reserved1               :3;
  UINT16          PeakCapacity            :12;
  UINT16          HoldUpTime              :4;
  UINT8           Voltage2                :4;
  UINT8           Voltage1                :4;
  UINT8           TotalCombinedWattage[2];
  UINT8           LowerThreshold;
} FRU_POWER_SUPPLY;

//
// DC Output (Record Type 0x01)
//
typedef struct _FRU_DC_OUT {
  UINT8         OutputNumber  :4;
  UINT8         Reserved      :3;
  UINT8         Standby       :1;
  UINT16        NominalVoltage;
  UINT16        MaximumNegativeVoltage;
  UINT16        MaximumPositiveVoltage;
  UINT16        RippleNoisePk;
  UINT16        MinimumCurrentDraw;
  UINT16        MaximumCurrentDraw;
} FRU_DC_OUT;

//
// DC Load (Record Type 0x02)
//
typedef struct _FRU_DC_LOAD {
  UINT8         OutputNumber  :4;
  UINT8         Reserved      :4;
  UINT16        NominalVoltage;
  UINT16        SpecMinimumVoltage;
  UINT16        SpecMaximumVoltage;
  UINT16        SpecRippleNoisePk;
  UINT16        MinimumCurrentDraw;
  UINT16        MaximumCurrentDraw;
} FRU_DC_LOAD;

//
// Management Access Record (Record Type 0x03)
//
enum FRU_SUB_TYPE {
       SystemManagementURL, 
       SystemName, 
       SystemPingAddress, 
       ComponentManagementURL, 
       Component_Name, 
       ComponentPingAddress,
       SystemUniqueID
       };

typedef struct _FRU_MANAGEMENT_ACCESS {
  enum FRU_SUB_TYPE    SubType;
  union {
    CHAR8      SystemURL[254];
    CHAR8      SystemName[64];
    CHAR8      SystemPing[64];
    CHAR8      ComponetURL[254];
    CHAR8      ComponetName[254];
    CHAR8      ComponetPing[64];
    UINT8      SystemUniqueID[16];
  } Data;
} FRU_MANAGEMENT_ACCES;

//
// Base Compatibility Record (Record Type 0x04)
// Extended Compatibility Record (Record Type 0x05)
//
typedef struct _FRU_COMPATIBILITY {
  UINT8         ManufacturerId[3];
  UINT8         EntityIDCode;
  UINT8         CompatibilityBase;
  UINT8         CodeStart :7;
  UINT8         Reserved  :1;
  UINT8         CodeRangeMask;
  UINT8         Data[247];
} FRU_BASE_COMPATIBILITY, FRU_EXTENDEN_COMPATIBILITY;

//
// OEM Record Structure (Record Type 0xC0::0xFF)
//
typedef struct {
  UINT8   ManufacturerId[3];
  UINT8   Data[252];
} FRU_RECORD_OEM;

//
// Multi Record Header Structure
//
typedef struct _H2O_IPMI_FRU_RECORD_HEADER{
  UINT8   RecordId;
  UINT8   Version   :4;
  UINT8   Reserved  :3;
  UINT8   EndOfList :1;
  UINT8   Length;
  UINT8   RecordChecksum;
  UINT8   HeaderChecksum;
} H2O_IPMI_FRU_RECORD_HEADER;

#pragma pack ()

#endif

