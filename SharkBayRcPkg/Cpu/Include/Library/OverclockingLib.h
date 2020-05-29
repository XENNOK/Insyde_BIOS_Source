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
   Copyright (c)  2005 - 2012 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.
*/
#ifndef _OVERCLOCKING_LIBRARY_H_
#define _OVERCLOCKING_LIBRARY_H_

//
//  OC Mailbox MSR
//
#define MSR_OC_MAILBOX        0x00000150
#define OC_LIB_WAIT_TIMEOUT   5000                     ///< 5 milliseconds

//
//  OC Mailbox commands
//
#define OC_LIB_CMD_GET_OC_CAPABILITIES              0x01
#define OC_LIB_CMD_GET_PER_CORE_RATIO_LIMIT         0x02
#define OC_LIB_CMD_GET_VOLTAGE_FREQUENCY            0x10
#define OC_LIB_CMD_SET_VOLTAGE_FREQUENCY            0x11
#define OC_LIB_CMD_GET_SVID_CONFIG                  0x12
#define OC_LIB_CMD_SET_SVID_CONFIG                  0x13
#define OC_LIB_CMD_GET_GLOBAL_CONFIG                0x14
#define OC_LIB_CMD_SET_GLOBAL_CONFIG                0x15

//
//  OC Mailbox completion codes
//
#define OC_LIB_COMPLETION_CODE_SUCCESS              0x00
#define OC_LIB_COMPLETION_CODE_OC_LOCKED            0x01
#define OC_LIB_COMPLETION_CODE_INVALID_DOMAIN       0x02
#define OC_LIB_COMPLETION_CODE_MAX_RATIO_EXCEEDED   0x03
#define OC_LIB_COMPLETION_CODE_MAX_VOLTAGE_EXCEEDED 0x04
#define OC_LIB_COMPLETION_CODE_OC_NOT_SUPPORTED     0x05
#define OC_LIB_COMPLETION_CODE_WRITE_FAILED         0x06
#define OC_LIB_COMPLETION_CODE_READ_FAILED          0x07

//
// Domain ID definitions
//
#define OC_LIB_DOMAIN_ID_IA_CORE    0x00
#define OC_LIB_DOMAIN_ID_GT         0x01
#define OC_LIB_DOMAIN_ID_CLR        0x02
#define OC_LIB_DOMAIN_ID_UNCORE     0x03
#define OC_LIB_DOMAIN_ID_IOA        0x04
#define OC_LIB_DOMAIN_ID_IOD        0x05

//
// Bit 10 is the S11.0.10V sign bit
//
#define FIXED_POINT_SIGN_BIT_MASK 0x0400
#define INT16_SIGN_BIT_MASK       0x8000

//
//  Voltage Conversion defines
//
#define MILLIVOLTS_PER_VOLT  1000
#define MAX_TARGET_MV      4095
#define MAX_OFFSET_MV      500

#define CONVERT_TO_FIXED_POINT_VOLTS  0
#define CONVERT_TO_BINARY_MILLIVOLT   1

//
// Masks and offsets
//
#define BIT0_MASK             0x1
#define MAX_RATIO_MASK        0x000000FF
#define VOLTAGE_TARGET_MASK   0x000FFF00
#define VOLTAGE_TARGET_OFFSET 8
#define VOLTAGE_MODE_MASK     0x00100000
#define VOLTAGE_MODE_OFFSET   20
#define VOLTAGE_OFFSET_MASK   0xFFE00000
#define VOLTAGE_OFFSET_OFFSET 21

#define SVID_DISABLE_MASK     0x80000000
#define SVID_DISABLE_OFFSET   31
#define SVID_VOLTAGE_MASK     0x00000FFF

#define FIVR_FAULTS_MASK        0x00000001
#define FIVR_EFFICIENCY_MASK    0x00000002
#define FIVR_EFFICIENCY_OFFSET  1

#define OC_CAPS_MAX_RATIO_MASK          0x000000FF
#define OC_CAPS_RATIO_SUPPORT_MASK      0x00000100
#define OC_CAPS_RATIO_SUPPORT_OFFSET    8
#define OC_CAPS_OVERRIDE_SUPPORT_MASK   0x00000200
#define OC_CAPS_OVERRIDE_SUPPORT_OFFSET 9
#define OC_CAPS_OFFSET_SUPPORT_MASK     0x00000400
#define OC_CAPS_OFFSET_SUPPORT_OFFSET   10

//
// Voltage offset definitions
//
#define OC_LIB_OFFSET_ADAPTIVE  0
#define OC_LIB_OFFSET_OVERRIDE  1
#define OC_LIB_VOLTAGE_DO_NOT_UPDATE  0xFFFF

///
///  OC Library structures
///
typedef struct {
  UINT32 CommandData;
  UINT8  CommandCompletion : 8;
  UINT8  Param1            : 8;
  UINT8  Param2            : 8;
  UINT8  Reserved          : 7;
  UINT8  RunBusy           : 1;
} OC_LIBRARY_COMMAND;

typedef union _OC_MAILBOX_COMMAND {
  UINT32 InterfaceData;
  struct {
    UINT8 CommandCompletion : 8;
    UINT8 Param1            : 8;
    UINT8 Param2            : 8;
    UINT8 Reserved          : 7;
    UINT8 RunBusy           : 1;
  } Fields;
} OC_MAILBOX_COMMAND;

typedef struct _OC_MAILBOX_ITEM {
  UINT32             Data;
  OC_MAILBOX_COMMAND Interface;
} OC_MAILBOX_ITEM;

typedef struct {
  UINT8  MaxOcRatio;
  UINT8  VoltageTargetMode;
  UINT16 VoltageTarget;
  INT16  VoltageOffset;
} VOLTAGE_FREQUENCY_SETTINGS;

typedef struct {
  VOLTAGE_FREQUENCY_SETTINGS VfSettings;
  UINT8                      DomainId;
} VOLTAGE_FREQUENCY_ITEM;

typedef enum {
  IaCore,
  Gt,
  Clr,
  Uncore
} CPU_DOMAIN_ID;

typedef struct {
  UINT16  VoltageTarget;
  BOOLEAN SvidDisable;
} SVID_CONFIG_ITEM;

typedef struct {
  UINT8   MaxOcRatioLimit;
  BOOLEAN RatioOcSupported;
  BOOLEAN VoltageOverridesSupported;
  BOOLEAN VoltageOffsetSupported;
  UINT8   DomainId;
} OC_CAPABILITIES_ITEM;

typedef struct {
  UINT8 MaxOcRatioLimit1C;
  UINT8 MaxOcRatioLimit2C;
  UINT8 MaxOcRatioLimit3C;
  UINT8 MaxOcRatioLimit4C;
  UINT8 Index;
} CORE_RATIO_LIMITS_ITEM;

typedef struct {
  UINT8 DisableFivrFaults;
  UINT8 DisableFivrEfficiency;
} GLOBAL_CONFIG_ITEM;

typedef struct {
  UINT32 MaxOcRatio       : 8;
  UINT32 VoltageTargetU12 : 12;
  UINT32 TargetMode       : 1;
  UINT32 VoltageOffsetS11 : 11;
} VF_MAILBOX_COMMAND_DATA;

/**

  Gets the Voltage and Frequency information for a given CPU domain

  @param[OUT] *VfSettings
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
GetVoltageFrequencyItem (
  OUT VOLTAGE_FREQUENCY_ITEM  * VfSettings,
  OUT UINT32                  *LibStatus
  );

/**

  Sets the Voltage and Frequency information for a given CPU domain

  @param[IN]  *VfSettings
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
SetVoltageFrequencyItem (
  IN VOLTAGE_FREQUENCY_ITEM  VfSettings,
  OUT UINT32                *LibStatus
  );

/**

  Get the global FIVR Configuration information

  @param[OUT] *FivrConfig
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
GetFivrConfig (
  OUT GLOBAL_CONFIG_ITEM  *FivrConfig,
  OUT UINT32              *LibStatus
  );

/**

  Set the Global FIVR Configuration information

  @param[IN]  FivrConfig
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
SetFivrConfig (
  IN GLOBAL_CONFIG_ITEM FivrConfig,
  OUT UINT32            *LibStatus
  );

/**

  Get the SVID Configuration information

  @param[OUT] *SvidConfig
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
GetSvidConfig (
  OUT SVID_CONFIG_ITEM  *SvidConfig,
  OUT UINT32            *LibStatus
  );

/**

  Set the SVID Configuration information

  @param[IN]  SvidConfig
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
SetSvidConfig (
  IN SVID_CONFIG_ITEM SvidConfig,
  OUT UINT32          *LibStatus
  );

/**

  Get the overclocking capabilities for a given CPU Domain

  @param[OUT] *OcCapabilities
  @param[OUT] *LibStatus

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
GetOcCapabilities (
  OUT OC_CAPABILITIES_ITEM  *OcCapabilities,
  OUT UINT32                *LibStatus
  );

/**

  Converts the input voltage target to the fixed point U12.2.10 Volt format or
  the Binary millivolts representation based on the ConversionType

@param[IN]  InputVoltageTarget
@param[OUT] *OutputVoltageTarget
@param[IN]  ConversionType - 0:fixed point, 1:Binary millivolts

**/
VOID
ConvertVoltageTarget (
  IN  UINT16  InputVoltageTarget,
  OUT UINT16* OutputVoltageTarget,
  IN  UINT8   ConversionType
  );

/**

  Converts the input votlage Offset to the fixed point S11.0.10 Volt format or
  to Binary illivolts representation based on the ConversionType.

@param[IN]  InputVoltageTarget
@param[OUT] *OutputVoltageTarget
@param[IN]  ConversionType - 0:fixed point, 1:Signed Binary millivolts

**/
VOID
ConvertVoltageOffset (
  IN  INT16   InputVoltageOffset,
  OUT INT16*  OutputVoltageOffset,
  IN  UINT8   ConversionType
  );

/**

  Converts the input data to valid mailbox command format based on CommandID

@param[IN]  InputData
@param[OUT] *MailboxData
@param[IN]  CommandId

**/
VOID
ConvertToMailboxFormat (
  IN  VOID*             InputData,
  OUT OC_MAILBOX_ITEM*  MailboxData,
  IN  UINT32            CommandId
  );

#endif
