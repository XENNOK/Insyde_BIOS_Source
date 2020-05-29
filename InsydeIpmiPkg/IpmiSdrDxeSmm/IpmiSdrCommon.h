/** @file
 H2O IPMI SDR module header file for common function.

 This header file contains common function prototype definitions of this module.

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

#ifndef _IPMI_SDR_COMMON_H_
#define _IPMI_SDR_COMMON_H_


#include <Protocol/H2OIpmiSdrProtocol.h>

#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiInterfaceLib.h>

#include <IpmiSdrMath.h>

//
// Common definition
//
#define DEFAULT_SENSOR_NAME     L"No Name"
#define DEFAULT_NO_VALUE        L"N/A"
#define DISCRETE_UNIT           L"Discrete"
#define MAX_SDR_DATA_LEN        0x50
#define MAX_SENSOR_NUMBER       0x100
#define DEFAULT_SDR_VERSION     0x51

//
// Sensor related definitions
//
#define SDR_SENSOR_L_LINEAR     0x00
#define SDR_SENSOR_L_LN         0x01
#define SDR_SENSOR_L_LOG10      0x02
#define SDR_SENSOR_L_LOG2       0x03
#define SDR_SENSOR_L_E          0x04
#define SDR_SENSOR_L_EXP10      0x05
#define SDR_SENSOR_L_EXP2       0x06
#define SDR_SENSOR_L_1_X        0x07
#define SDR_SENSOR_L_SQR        0x08
#define SDR_SENSOR_L_CUBE       0x09
#define SDR_SENSOR_L_SQRT       0x0a
#define SDR_SENSOR_L_CUBERT     0x0b
#define SDR_SENSOR_L_NONLINEAR  0x70


//
// Sensor thresholds definitaions
//
#define SDR_SENSOR_T_UPPER_NON_RECOV_SPECIFIED  0x20
#define SDR_SENSOR_T_UPPER_CRIT_SPECIFIED       0x10
#define SDR_SENSOR_T_UPPER_NON_CRIT_SPECIFIED   0x08
#define SDR_SENSOR_T_LOWER_NON_RECOV_SPECIFIED  0x04
#define SDR_SENSOR_T_LOWER_CRIT_SPECIFIED       0x02
#define SDR_SENSOR_T_LOWER_NON_CRIT_SPECIFIED   0x01

#define GetInt32(Value, Bits) \
  ((Value & ((1<<((Bits)-1)))) ? (-((Value) & (1<<((Bits)-1))) | (Value)) : (Value))


#pragma pack(1)

typedef struct {
  CHAR16  *Long;
  CHAR16  *Short;
} SDR_UNIT_STRING_STRUCT;

#pragma pack()


/**
 Fill private data content.

*/
VOID
InitialIpmiSdrContent (
  VOID
  );


extern H2O_IPMI_SDR_PROTOCOL        mSdrProtocol;


#endif

