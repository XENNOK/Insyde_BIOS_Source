/** @file

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

#ifndef _HARDWARE_MONITOR_EC_DXE_H_
#define _HARDWARE_MONITOR_EC_DXE_H_

#include <Uefi.h>

//[-start-130813-IB06720232-remove]//
//#include <OemEcLib.h>
//[-end-130813-IB06720232-remove]//
//#include <Library/EcLib.h>
#include <Library/DxeKscLib.h>

#include <Protocol/IsaNonPnpDevice.h>


#define KEY_HARDWARE_MONITOR_VOLTAGE            1600
#define KEY_HARDWARE_MONITOR_TEMPERATURE        1700
#define KEY_HARDWARE_MONITOR_FANSPEED           1800

//
// KeyValue = StringID
//
typedef enum {
  CPU_FAN_KEY,
  CPU_VR_TEMP_KEY,
  HEAT_EXCHANGER_FAN_TEMP_KEY,
  SKIN_TEMP_KEY,
  AMBIENT_TEMP_KEY,
  DIMM0_TEMP_KEY,
  DIMM1_TEMP_KEY,
  PCH_DTS_TEMP_KEY,
  TS_ON_DIMM0_TEMP_KEY,
  TS_ON_DIMM1_TEMP_KEY,
  CPU_TEMP_BY_PECI_KEY,
  VOLT_1_5V_KEY,
  VOLT_3_3V_KEY,
  VOLT_5V_KEY,
  VOLT_12V_KEY
} HARDWARE_MONITOR_KEY;

//
// EC Register
//
#define EC_REG_CPU_VR_TEMP               0x4D
#define EC_REG_CPU_GT_VR_TEMP            0xC0
#define EC_REG_CPU_FAN_SPEED             0x73
#define EC_REG_HEAT_EXG_FAN_TEMP         0x4E
#define EC_REG_SKIN_TEMP                 0x4F
#define EC_REG_AMBIENT_TEMP              0x50
#define EC_REG_DIMM0_TEMP                0x7E
#define EC_REG_DIMM1_TEMP                0x7F
#define EC_REG_PCH_DTS_TEMP              0x81
#define EC_REG_CPU_FRACTIONAL_TEMP_PECI  0x82
#define EC_REG_CPU_INTRGER_TEMP_PECI     0x83
#define EC_REG_TS_ON_DIMM0_TEMP          0x85
#define EC_REG_TS_ON_DIMM1_TEMP          0x86
#define EC_REG_VOLTAGE_5_VH              0xC9
#define EC_REG_VOLTAGE_5_VL              0xCA
#define EC_REG_VOLTAGE_12_VH             0xCB
#define EC_REG_VOLTAGE_12_VL             0xCC
#define EC_REG_VOLTAGE_3_3_VH            0xCD
#define EC_REG_VOLTAGE_3_3_VL            0xCE
#define EC_REG_VOLTAGE_1_05_VH           0xCF
#define EC_REG_VOLTAGE_1_05_VL           0xD0

//
//  formula Parameter define
//
#define  Vef                 3222   // 3.3/1024(Vref/1024) = 0.003222
#define  Scaling_Factor_12V  401    // 12V HW Scaling Factor
#define  Scaling_Factor_5V   200    // 5V HW Scaling Factor
#define  Scaling_Factor_3V   100    // 3V HW Scaling Factor      //debug
#define  Scaling_Factor_1_5V 50     // 1.5V HW Scaling Factor   // debug

// Structure used to return system status
// NOTE:
//   All voltages have been multiplied by 1000 so that integer data
//   types can be used.  To find the actual voltage divide the voltage
//   data member by 1000.

#pragma pack (1)

typedef struct {
  UINTN    V1_5;
  UINTN    Vccp;
  UINTN    V3_3;
  UINTN    V5;
  UINTN    V12;
  INT8     ProcessorTemp;
  INT8     InternalTemp;
  INT8     RemoteTemp;
  UINT16   Tach1;
  UINT16   Tach2;
  UINT16   Tach3;
  UINT16   Tach4;
  UINT8    CurrentPwm1;
  UINT8    CurrentPwm2;
  UINT8    CurrentPwm3;
  INT8     CPUVRTemp;
  INT8     CPUGTVRTemp;
  INT16    CPUFanSpeed;
  INT8     HeatExgFanTemp;
  INT8     SkinTemp;
  INT8     AmbientTemp;
  INT8     Dimm0Temp;
  INT8     Dimm1Temp;
  INT8     MinicardRegionTemp;
  INT8     PCHDTSTemp;
  INT8     MCHDTSTemp;
  INT8     TSonDimm0Temp;
  INT8     TSonDimm1Temp;
} EC_HARDWARE_MONITOR_TABLE;

#pragma pack ()

#endif
