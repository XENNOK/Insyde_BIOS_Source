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

#include <Uefi.h>
#include <CpuRegs.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/OverclockingLib.h>

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
  )
{
  EFI_STATUS      Status;
  UINT32          CommandId;
  UINT16          TempVoltageTarget;
  INT16           TempVoltageOffset;
  OC_MAILBOX_ITEM VfMsg;

  Status = EFI_SUCCESS;

  ZeroMem(&VfMsg,sizeof(VfMsg));
  //
  // Convert v/f command to Mailbox command format
  //
  CommandId = OC_LIB_CMD_GET_VOLTAGE_FREQUENCY;

  ConvertToMailboxFormat((VOID*)VfSettings, &VfMsg, CommandId);

  //
  //  Read From the OC Library
  //
  Status = MailboxRead(MAILBOX_TYPE_OC, VfMsg.Interface.InterfaceData, &VfMsg.Data, LibStatus);

  //
  //  Copy mailbox data to VfSettings
  //
  if ( (Status == EFI_SUCCESS) && (*LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS)){
    VfSettings->VfSettings.MaxOcRatio = (UINT8) (VfMsg.Data & MAX_RATIO_MASK);
    VfSettings->VfSettings.VoltageTargetMode = (UINT8) ( (VfMsg.Data & VOLTAGE_MODE_MASK) >> VOLTAGE_MODE_OFFSET);

    TempVoltageTarget = (UINT16) (VfMsg.Data  & VOLTAGE_TARGET_MASK) >> VOLTAGE_TARGET_OFFSET;
    ConvertVoltageTarget(TempVoltageTarget, &VfSettings->VfSettings.VoltageTarget, CONVERT_TO_BINARY_MILLIVOLT);

    TempVoltageOffset = (INT16)((VfMsg.Data  & VOLTAGE_OFFSET_MASK) >> VOLTAGE_OFFSET_OFFSET);
    ConvertVoltageOffset(TempVoltageOffset, &VfSettings->VfSettings.VoltageOffset, CONVERT_TO_BINARY_MILLIVOLT);
  }

  return Status;
}

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
  )

{
  EFI_STATUS       Status;
  UINT32           CommandId;
  OC_MAILBOX_ITEM  VfMsg;

  Status = EFI_SUCCESS;

  //
  // Convert v/f Commands to Mailbox command format
  //
  CommandId = OC_LIB_CMD_SET_VOLTAGE_FREQUENCY;
  ConvertToMailboxFormat((VOID*)&VfSettings, &VfMsg, CommandId);

  //
  // Write the v/f Settings to the OC Mailbox
  //
  Status = MailboxWrite(MAILBOX_TYPE_OC, VfMsg.Interface.InterfaceData, VfMsg.Data, LibStatus);

  return Status;
}

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
  )

{
  EFI_STATUS      Status;
  UINT32          CommandId;
  OC_MAILBOX_ITEM FivrMsg;
  
  Status = EFI_SUCCESS;
  ZeroMem(&FivrMsg, sizeof(FivrMsg));

  //
  // Convert FIVR message to Mailbox command format
  //
  CommandId = OC_LIB_CMD_GET_GLOBAL_CONFIG;
  ConvertToMailboxFormat((VOID*)FivrConfig, &FivrMsg, CommandId);

  //
  //  Read From the OC Library
  //
  Status = MailboxRead(MAILBOX_TYPE_OC, FivrMsg.Interface.InterfaceData, &FivrMsg.Data, LibStatus);

  //
  //  Copy mailbox data to FivrConfig
  //
  if ( (Status == EFI_SUCCESS) && (*LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS)) {
    FivrConfig->DisableFivrFaults = FivrMsg.Data & FIVR_FAULTS_MASK;
    FivrConfig->DisableFivrEfficiency = (FivrMsg.Data & FIVR_EFFICIENCY_MASK) >> FIVR_EFFICIENCY_OFFSET;
  }

  return Status;
}

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
  )
{
  EFI_STATUS      Status;
  UINT32          CommandId;
  OC_MAILBOX_ITEM FivrMsg;

  Status = EFI_SUCCESS;

  //
  // Convert FIVR Command to Mailbox command format
  //
  CommandId = OC_LIB_CMD_SET_GLOBAL_CONFIG;
  ConvertToMailboxFormat((VOID*)&FivrConfig, &FivrMsg, CommandId);

  //
  // Write the FIVR Settings to the OC Mailbox
  //
  Status = MailboxWrite(MAILBOX_TYPE_OC, FivrMsg.Interface.InterfaceData, FivrMsg.Data, LibStatus);

  return Status;

}

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
  )
{
  EFI_STATUS      Status;
  UINT32          CommandId;
  OC_MAILBOX_ITEM SvidMsg;
  
  Status = EFI_SUCCESS;
  ZeroMem(&SvidMsg, sizeof(SvidMsg));

  //
  // Convert SVID message to Mailbox command format
  //
  CommandId = OC_LIB_CMD_GET_SVID_CONFIG;
  ConvertToMailboxFormat((VOID*)SvidConfig, &SvidMsg, CommandId);

  //
  //  Read From the OC Library
  //
  Status = MailboxRead(MAILBOX_TYPE_OC, SvidMsg.Interface.InterfaceData, &SvidMsg.Data, LibStatus);

  //
  //  Copy mailbox data to SvidConfig
  //
  if ( (Status == EFI_SUCCESS) && (*LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS)){
    SvidConfig->VoltageTarget = (UINT16) SvidMsg.Data & SVID_VOLTAGE_MASK;
    SvidConfig->SvidDisable = (UINT8) ((SvidMsg.Data & SVID_DISABLE_MASK) >> SVID_DISABLE_OFFSET);
  }

  return Status;
}

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
  )
{
  EFI_STATUS      Status;
  UINT32          CommandId;
  OC_MAILBOX_ITEM SvidMsg;

  Status = EFI_SUCCESS;
  
  //
  // Convert SVID Commands to Mailbox command format
  //
  CommandId = OC_LIB_CMD_SET_SVID_CONFIG;
  ConvertToMailboxFormat((VOID*)&SvidConfig, &SvidMsg, CommandId);

  //
  // Write the Svid Settings to the OC Mailbox
  //
  Status = MailboxWrite(MAILBOX_TYPE_OC, SvidMsg.Interface.InterfaceData, SvidMsg.Data, LibStatus);

  return Status;
}

EFI_STATUS
EFIAPI 
GetOcCapabilities (
  OUT OC_CAPABILITIES_ITEM  *OcCapabilities,
  OUT UINT32                *LibStatus
  )
/**

  Get the overclocking capabilities for a given CPU Domain

  @param[OUT] *OcCapabilities
  @param[OUT] *LibStatus
  
  @retval EFI_STATUS

**/
{
  EFI_STATUS      Status;
  UINT32          CommandId;
  OC_MAILBOX_ITEM OcCapsMsg;

  Status = EFI_SUCCESS;

  ZeroMem(&OcCapsMsg,sizeof(OC_MAILBOX_ITEM));

  //
  // Convert OC capabilties message to Mailbox command format
  //
  CommandId = OC_LIB_CMD_GET_OC_CAPABILITIES;
  ConvertToMailboxFormat((VOID*)OcCapabilities, &OcCapsMsg, CommandId);

  //
  //  Read From the OC Library
  //
  Status = MailboxRead(MAILBOX_TYPE_OC, OcCapsMsg.Interface.InterfaceData, &OcCapsMsg.Data, LibStatus);

  //
  //  Copy mailbox data to OC Capabilities structure
  //
  if ( (Status == EFI_SUCCESS) && (*LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS)){
    OcCapabilities->MaxOcRatioLimit = 
      (UINT8) OcCapsMsg.Data & OC_CAPS_MAX_RATIO_MASK;

    OcCapabilities->RatioOcSupported = 
      (UINT8) ((OcCapsMsg.Data & OC_CAPS_RATIO_SUPPORT_MASK) >> OC_CAPS_RATIO_SUPPORT_OFFSET);

    OcCapabilities->VoltageOverridesSupported = 
      (UINT8) ((OcCapsMsg.Data & OC_CAPS_OVERRIDE_SUPPORT_MASK) >> OC_CAPS_OVERRIDE_SUPPORT_OFFSET);

    OcCapabilities->VoltageOffsetSupported = 
      (UINT8) ((OcCapsMsg.Data & OC_CAPS_OFFSET_SUPPORT_MASK) >> OC_CAPS_OFFSET_SUPPORT_OFFSET);
  }

  return Status;
}

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
  )

{
UINT32 Remainder;
  ///  Fixed point representation:
  ///
  ///  U12.2.10V format
  ///  | | | |
  ///  | | | v
  ///  | | v Exponent
  ///  | v Significand Size
  ///  v Size
  ///  Signed/Unsigned
  ///
  ///  Float Value = Significand x (Base ^ Exponent)
  ///  (Base ^ Exponent) = 2 ^ 10 = 1024
  ///
  Remainder = 0;

  if (InputVoltageTarget == 0){
    *OutputVoltageTarget = 0;
    return;
  }

  if(ConversionType == CONVERT_TO_FIXED_POINT_VOLTS){
    //
    // Input Voltage is in number of millivolts. Clip the input Voltage 
    // to the max allowed by the fixed point format
    //
    if (InputVoltageTarget > MAX_TARGET_MV)
      InputVoltageTarget = MAX_TARGET_MV;  

    ///
    /// InputTargetVoltage is the significand in mV. Need to convert to Volts
    ///
    *OutputVoltageTarget = (UINT16) DivU64x32Remainder (
                    (UINT64) (InputVoltageTarget * 1024), MILLIVOLTS_PER_VOLT,&Remainder);

    if (Remainder >= 500) {
      *OutputVoltageTarget += 1;
    }
  } else if (ConversionType == CONVERT_TO_BINARY_MILLIVOLT) {
    ///
    /// InputVoltage is specified in fixed point representation, need to
    /// convert to millivolts
    ///
    *OutputVoltageTarget = (UINT16) DivU64x32Remainder (
                    (UINT64) (InputVoltageTarget * MILLIVOLTS_PER_VOLT), 1024,&Remainder);

    if (Remainder >= 500) {
      *OutputVoltageTarget += 1;
    }
  }

  return;
}

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
  )
{
  BOOLEAN NumIsNegative;
  UINT32  Remainder;
  ///  Fixed point representation:
  ///
  ///  S11.0.10V format
  ///  | | | |
  ///  | | | v
  ///  | | v Exponent
  ///  | v Significand Size
  ///  v Size
  ///  Signed/Unsigned
  ///
  ///  Float Value = Significand x (Base ^ Exponent)
  ///  (Base ^ Exponent) = 2 ^ 10 = 1024
  ///
  *OutputVoltageOffset = 0;
  NumIsNegative = FALSE;
  Remainder = 0;

  if (InputVoltageOffset == 0){
    *OutputVoltageOffset = 0;
    return;
  }

  if (ConversionType == CONVERT_TO_FIXED_POINT_VOLTS){
    //
    // Input Voltage is in INT16 representation. Check if numenr is negative
    //
    if ( (InputVoltageOffset & INT16_SIGN_BIT_MASK) != 0){
      NumIsNegative = TRUE;
      //
      // Need to 2's complement adjust to make this number positive for 
      // voltage calculation
      //
      InputVoltageOffset = (~InputVoltageOffset+1) & (INT16_SIGN_BIT_MASK -1);
    }

    //
    // Clip the input Voltage Offset to 500mv
    //
    if (InputVoltageOffset > MAX_OFFSET_MV) {
      InputVoltageOffset = MAX_OFFSET_MV;
    }

    ///
    /// Convert to fixed point representation
    ///
    *OutputVoltageOffset = (UINT16) DivU64x32Remainder (
                    (UINT64) (InputVoltageOffset * 1024), MILLIVOLTS_PER_VOLT,&Remainder);

    if (Remainder >= 500) {
      *OutputVoltageOffset += 1;
    }

    if (NumIsNegative) {
      /// 2's complement back to a negative number
      *OutputVoltageOffset = ~(*OutputVoltageOffset) + 1;
    }
  } else if (ConversionType == CONVERT_TO_BINARY_MILLIVOLT) {
    //
    // Input Voltage is in fixed point representation. Check if number negative
    //
    if( (InputVoltageOffset & FIXED_POINT_SIGN_BIT_MASK)!= 0){
      NumIsNegative = TRUE;
      //
      // Need to 2's complement adjust to make this number positive for 
      // voltage calculation
      //
      InputVoltageOffset = (~InputVoltageOffset+1) & (FIXED_POINT_SIGN_BIT_MASK -1);
    }

    ///
    ///  Convert to INT16 representation in millivolts
    ///
    *OutputVoltageOffset = (UINT16) DivU64x32Remainder (
                    (UINT64) (InputVoltageOffset * MILLIVOLTS_PER_VOLT), 1024,&Remainder);

    if (Remainder >= 500) {
      *OutputVoltageOffset += 1;
    }

    if (NumIsNegative) {
      /// 2's complement back to a negative number
      *OutputVoltageOffset = ~(*OutputVoltageOffset) + 1;
    }
  }

  return;
}

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
  )
{
  VOLTAGE_FREQUENCY_ITEM* VfItem;
  SVID_CONFIG_ITEM*       SvidItem;
  OC_CAPABILITIES_ITEM*   OcCapItem;
  CORE_RATIO_LIMITS_ITEM* CoreRatioItem;
  GLOBAL_CONFIG_ITEM*     GlobalConfigItem;
  VF_MAILBOX_COMMAND_DATA VfMailboxCommandData;
  UINT16  TempVoltage;

  //
  //  Initialize local varaibles and mailbox data
  //
  ZeroMem ((UINT32*)MailboxData, sizeof(OC_MAILBOX_ITEM));

  // 
  // Then make a decision based on CommandId how to format
  //
  switch (CommandId){
    case OC_LIB_CMD_GET_OC_CAPABILITIES:
      OcCapItem = (OC_CAPABILITIES_ITEM*) InputData;
      //
      // OC Capabilities are returned on a per domain basis
      //
      MailboxData->Data = 0;
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_GET_OC_CAPABILITIES;
      MailboxData->Interface.Fields.Param1 = OcCapItem->DomainId;
      break;

    case OC_LIB_CMD_GET_PER_CORE_RATIO_LIMIT:
      CoreRatioItem = (CORE_RATIO_LIMITS_ITEM*) InputData;
      //
      // Core Ratio Limits are only valid in the IA Core domain
      //
      MailboxData->Data = 0;
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_GET_PER_CORE_RATIO_LIMIT;
      MailboxData->Interface.Fields.Param1 = OC_LIB_DOMAIN_ID_IA_CORE;
      MailboxData->Interface.Fields.Param2 = CoreRatioItem->Index;
      break;

    case OC_LIB_CMD_GET_VOLTAGE_FREQUENCY:
      VfItem = (VOLTAGE_FREQUENCY_ITEM*) InputData;
      //
      // Voltage Frequency Settings are on a per domain basis
      //
      MailboxData->Data = 0;
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_GET_VOLTAGE_FREQUENCY;
      MailboxData->Interface.Fields.Param1 = VfItem->DomainId;
      break;

    case OC_LIB_CMD_SET_VOLTAGE_FREQUENCY:
      VfItem = (VOLTAGE_FREQUENCY_ITEM*) InputData;
      //
      // Voltages are stored in a fixed point format
      //
      VfMailboxCommandData.MaxOcRatio = VfItem->VfSettings.MaxOcRatio;

      TempVoltage = 0;
      ConvertVoltageTarget(VfItem->VfSettings.VoltageTarget, &TempVoltage, CONVERT_TO_FIXED_POINT_VOLTS);
      VfMailboxCommandData.VoltageTargetU12 = TempVoltage;
      
      VfMailboxCommandData.TargetMode = VfItem->VfSettings.VoltageTargetMode;

      TempVoltage = 0;
      ConvertVoltageOffset(VfItem->VfSettings.VoltageOffset, (INT16 *) &TempVoltage, CONVERT_TO_FIXED_POINT_VOLTS);
      VfMailboxCommandData.VoltageOffsetS11 = TempVoltage;

      CopyMem(&MailboxData->Data, &VfMailboxCommandData, sizeof(VfMailboxCommandData));
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_SET_VOLTAGE_FREQUENCY;
      MailboxData->Interface.Fields.Param1 = VfItem->DomainId;
      break;

    case OC_LIB_CMD_GET_SVID_CONFIG:
      MailboxData->Data = 0;
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_GET_SVID_CONFIG;
      MailboxData->Interface.Fields.Param1 = 0;
      break;

    case OC_LIB_CMD_SET_SVID_CONFIG:
      SvidItem = (SVID_CONFIG_ITEM*) InputData;
      ConvertVoltageTarget(SvidItem->VoltageTarget, &TempVoltage, CONVERT_TO_FIXED_POINT_VOLTS);
      MailboxData->Data = TempVoltage | (SvidItem->SvidDisable << SVID_DISABLE_OFFSET);
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_SET_SVID_CONFIG;
      MailboxData->Interface.Fields.Param1 = 0;
      break;

    case OC_LIB_CMD_GET_GLOBAL_CONFIG:
      MailboxData->Data = 0;
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_GET_GLOBAL_CONFIG;
      MailboxData->Interface.Fields.Param1 = 0;
      break;

    case OC_LIB_CMD_SET_GLOBAL_CONFIG:
      GlobalConfigItem = (GLOBAL_CONFIG_ITEM*) InputData;
      MailboxData->Data = 
        (GlobalConfigItem->DisableFivrFaults & BIT0_MASK) | 
        ((GlobalConfigItem->DisableFivrEfficiency & BIT0_MASK) << FIVR_EFFICIENCY_OFFSET);
      MailboxData->Interface.Fields.CommandCompletion = OC_LIB_CMD_SET_GLOBAL_CONFIG;
      MailboxData->Interface.Fields.Param1 = 0;
      break;

    default:
      DEBUG ((EFI_D_ERROR, "(OC MAILBOX) Unknown Command ID\n"));

      break;

  }

}

