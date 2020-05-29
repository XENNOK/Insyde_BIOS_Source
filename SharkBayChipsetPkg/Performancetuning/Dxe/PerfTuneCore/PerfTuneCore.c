/** @file

  File that provides the core functions necessary for the
  building of the SPTT tables.

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
;* Copyright (c) 2007 - 2009 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include "PerfTuneCore.h"

//#if defined(IDCC2_SUPPORTED) && (IDCC2_SUPPORTED != 0)

STATIC SPTT_TABLE_SERVICES_PROTOCOL       mSpttTableServicesProtocol = {
  BuildDiscreteBiosSettingTable,
  BuildContinuousBiosSettingTable,
  BuildHpContinuousBiosSettingTable,
  ReallocIfBufferOverflow
};

/**
  
  Initializes the SMM Handler Driver
  
  @param  ImageHandle             
  @param  SystemTable                       

  @retval          

**/
EFI_STATUS
InitializePerfTuneTableServices (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

//#if defined(IDCC2_SUPPORTED) && (IDCC2_SUPPORTED != 0)

  EFI_STATUS                Status;
  EFI_HANDLE                ProtocolHandle = NULL;
  UINTN                     VarSize;
//[-start-130710-IB05160465-modify]//
  VOID                      *SystemConfiguration;

  VarSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VarSize);

  gRT->GetVariable (
         L"Setup",
         &gSystemConfigurationGuid,
         NULL,
         &VarSize,
         SystemConfiguration
         );

  //
  // Install protocol
  //
  Status = gBS->InstallProtocolInterface(
                  &ProtocolHandle,
                  &gSpttTableServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSpttTableServicesProtocol
                  );


//////  DxeMemoryOverrideInit(ImageHandle, SystemTable);
//////  DxeMiscCpuOverrideInit(ImageHandle, SystemTable);

  FreePool (SystemConfiguration);
//[-end-130710-IB05160465-modify]//

  return EFI_SUCCESS;
}

/**
  
  This routine returns a Discrete BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildDiscreteBiosSettingTable (
  IN  RAW_DDD_LBS_DATA            *RawDiscreteBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  )
{
  UINT8                             *TableBuilder;
  UINT8                             *TmpTable;
  UINTN                             BufferSize = DEFAULT_BUFFER_SIZE;
  UINTN                             DataLen = 0;

  DEVICE_DESCRIPTION_TABLE          *DddTable;
  LOGICAL_BIOS_SETTING_DEVICE       *DiscreteDevice;
  LOGICAL_BIOS_SETTING_ENTRY        *DiscreteDeviceEntry;
  UINT16                            Index;

  RAW_DDD_LBS_DATA                  *TmpRaw = RawDiscreteBiosSettingList;


  TableBuilder = AllocateZeroPool(BufferSize);

  // Generic Device Description Data Table builder
  while (TmpRaw->ImplementType != 0xff) {
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(DEVICE_DESCRIPTION_TABLE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    DddTable = (DEVICE_DESCRIPTION_TABLE *)TmpTable;
    DddTable->Signature = DDD_TABLE_SIG; // Signature = "$DDD"
    DddTable->DeviceType = PERF_TUNE_BIOS_SETTING_DEVICE;
    DddTable->ControlType = PERF_TUNE_BIOS_DISCRETE_CONTROL;
    DddTable->ImplementationType = TmpRaw->ImplementType;
    DddTable->DeviceSpecificDataRevision = DEVICE_SPECIFIC_DATA_REVISION_1;  // for $LBS
    
    // Table Header builder for Discrete BIOS Settings
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(LOGICAL_BIOS_SETTING_DEVICE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    DiscreteDevice = (LOGICAL_BIOS_SETTING_DEVICE *)TmpTable;
    DiscreteDevice->Signature       = DISCRETE_BIOS_SETTINGS_DEVICE_SIG;
    DiscreteDevice->AutoModeSupport = TmpRaw->AutoModeSupport;
    DiscreteDevice->DefaultSetting  = TmpRaw->DefaultSetting;

    for (Index = 0; TmpRaw->RawSettingEntries[Index].SettingValue != 0xff; Index++) {
      // Table builder for the BIOS Setting Entries
      TmpTable = TableBuilder + DataLen;
      DataLen += sizeof(LOGICAL_BIOS_SETTING_ENTRY);
      ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

      DiscreteDeviceEntry = (LOGICAL_BIOS_SETTING_ENTRY *)TmpTable;
      DiscreteDeviceEntry->SettingValue       = TmpRaw->RawSettingEntries[Index].SettingValue;
      DiscreteDeviceEntry->SettingPrecision   = TmpRaw->RawSettingEntries[Index].SettingPrecision;
      DiscreteDeviceEntry->DataTypeEnum       = TmpRaw->RawSettingEntries[Index].DataTypeEnum;
      DiscreteDeviceEntry->BiosSettingsValue  = TmpRaw->RawSettingEntries[Index].BiosSettingsValue;
      DiscreteDeviceEntry->SettingFlags       = TmpRaw->RawSettingEntries[Index].SettingFlags;
    }

    DiscreteDevice->Count           = Index;

    TmpRaw++;
  }

  *BiosSettingTable = TableBuilder;
  *TableSize = DataLen;

  return EFI_SUCCESS;
}

/**
  
  This routine returns a Continuous BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildContinuousBiosSettingTable (
  IN  RAW_DDD_LBC_DATA            *RawContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  )
{
  UINT8                             *TableBuilder;
  UINT8                             *TmpTable;
  DEVICE_DESCRIPTION_TABLE          *DddTable;
  LOGICAL_BIOS_CONTINUOUS_DEVICE    *ContinuousDevice;
  UINTN                             BufferSize = DEFAULT_BUFFER_SIZE;
  UINTN                             DataLen = 0;

  RAW_DDD_LBC_DATA                  *TmpRaw = RawContinueBiosSettingList;

  TableBuilder = AllocateZeroPool(BufferSize);

  // Generic Device Description Data Table builder
  while (TmpRaw->ImplementType != 0xff) {
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(DEVICE_DESCRIPTION_TABLE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    DddTable = (DEVICE_DESCRIPTION_TABLE *)TmpTable;
    DddTable->Signature = DDD_TABLE_SIG; // Signature = "$DDD"
    DddTable->DeviceType = PERF_TUNE_BIOS_SETTING_DEVICE;
    DddTable->ControlType = PERF_TUNE_BIOS_CONTINUOUS_CONTROL;
    DddTable->ImplementationType = TmpRaw->ImplementType;
    DddTable->DeviceSpecificDataRevision = DEVICE_SPECIFIC_DATA_REVISION_0;   // for $LBC
    
    // Table Header builder for Discrete BIOS Settings
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(LOGICAL_BIOS_CONTINUOUS_DEVICE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    ContinuousDevice = (LOGICAL_BIOS_CONTINUOUS_DEVICE *)TmpTable;
    ContinuousDevice->Signature         = CONTINUOUS_BIOS_SETTINGS_DEVICE_SIG;
    ContinuousDevice->Min               = TmpRaw->Min;
    ContinuousDevice->Max               = TmpRaw->Max;
    ContinuousDevice->Step              = TmpRaw->Step;
    ContinuousDevice->DataTypeEnum      = TmpRaw->DataTypeEnum;
    ContinuousDevice->AutoModeSupport   = TmpRaw->AutoModeSupport;
    ContinuousDevice->MinSettingPerFlag = TmpRaw->MinSettingPerFlag;
    ContinuousDevice->DefaultSetting    = TmpRaw->DefaultSetting;
    ContinuousDevice->SettingPrecision  = TmpRaw->SettingPrecision;

    TmpRaw++;
  }

  *BiosSettingTable = TableBuilder;
  *TableSize = DataLen;

  return EFI_SUCCESS;
}

/**
  
  This routine returns a High Precision Continuous BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildHpContinuousBiosSettingTable(
  IN  RAW_DDD_HLBC_DATA           *RawHpContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  )
{
  UINT8                                   *TableBuilder;
  UINT8                                   *TmpTable;
  DEVICE_DESCRIPTION_TABLE                *DddTable;
  HIGH_LOGICAL_BIOS_CONTINUOUS_DEVICE     *ContinuousDevice;
  UINTN                                   BufferSize = DEFAULT_BUFFER_SIZE;
  UINTN                                   DataLen = 0;

  RAW_DDD_HLBC_DATA                       *TmpRaw = RawHpContinueBiosSettingList;

  TableBuilder = AllocateZeroPool(BufferSize);

  // Generic Device Description Data Table builder
  while (TmpRaw->ImplementType != 0xff) {
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(DEVICE_DESCRIPTION_TABLE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    DddTable = (DEVICE_DESCRIPTION_TABLE *)TmpTable;
    DddTable->Signature = DDD_TABLE_SIG; // Signature = "$DDD"
    DddTable->DeviceType = PERF_TUNE_BIOS_SETTING_DEVICE;
    DddTable->ControlType = PERF_TUNE_HIGH_PRECISION_LOGICAL_BIOS_SETTING;
    DddTable->ImplementationType = TmpRaw->ImplementType;
    DddTable->DeviceSpecificDataRevision = DEVICE_SPECIFIC_DATA_REVISION_1;   // for HLBC
    
    // Table Header builder for Discrete BIOS Settings
    TmpTable = TableBuilder + DataLen;
    DataLen += sizeof(HIGH_LOGICAL_BIOS_CONTINUOUS_DEVICE);
    ReallocIfBufferOverflow(&TableBuilder, DataLen, &BufferSize);

    ContinuousDevice = (HIGH_LOGICAL_BIOS_CONTINUOUS_DEVICE *)TmpTable;
    ContinuousDevice->Signature         = HIGH_PRECISION_BIOS_SETTINGS_DEVICE_SIG;
    ContinuousDevice->MinValue          = TmpRaw->MinValue;
    ContinuousDevice->MaxValue          = TmpRaw->MaxValue;
    ContinuousDevice->StepSize          = TmpRaw->StepSize;
    ContinuousDevice->SettingPrecision  = TmpRaw->SettingPrecision;
    ContinuousDevice->SettingFlags      = TmpRaw->SettingFlags;
    ContinuousDevice->MinData           = TmpRaw->MinData;
    ContinuousDevice->MaxData           = TmpRaw->MaxData;
    ContinuousDevice->DataStepSize      = TmpRaw->DataStepSize;
    ContinuousDevice->DataTypeEnum      = TmpRaw->DataTypeEnum; 
    ContinuousDevice->AutoModeSupport   = TmpRaw->AutoModeSupport;
    ContinuousDevice->MinMaxPerformance = TmpRaw->MinMaxPerformance;
    ContinuousDevice->DefaultSetting    = TmpRaw->DefaultSetting;

    TmpRaw++;
  }

  *BiosSettingTable = TableBuilder;
  *TableSize = DataLen;

  return EFI_SUCCESS;
}

/**
  
  This routine reallocates the memory necessary for a specific buffer size if the current buffer is not large enough
  
  @param  DataBuffer      This is the address of a pointer to the data buffer.      
  @param  DataLen          This is the size of the requested buffer
  @param  BufferSize       On input this is the size of the current buffer, On output this is the size of the new buffer

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
ReallocIfBufferOverflow (
  IN UINT8                    **DataBuffer,
  IN UINTN                    DataLen,
  IN OUT UINTN                *BufferSize
  )
{
  UINT8                           *backupTable;

  // If there isn't enough space, reallocate more...
  if (DataLen > *BufferSize) {
    do {
      *BufferSize *= 2;
    } while (DataLen > *BufferSize);

    backupTable = AllocateZeroPool(*BufferSize);
    CopyMem(backupTable, *DataBuffer, DataLen);
    gBS->FreePool(*DataBuffer);
    *DataBuffer = backupTable;
  }

  return EFI_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
//
// The following routines are support methods for the rest of the file
//
////////////////////////////////////////////////////////////////////////

/**
  
  This routine is responsible for converting a string into a numeric value and an associated 
  precision.
  
  @param  Str      This is the string to be converted  
  @param  Val      This is the value the string was converted to
  @param  Prec    This is the number of digits of precision in the value returned 
                          (i.e. how many digits should be considered part of the decimal)

  @retval  None

**/
VOID
StringToSettingsValue(
  CHAR16 *Str,
  UINT16 *Val,
  UINT8  *Prec
  )
{
  UINT16  u;
  CHAR8   c;
  UINT16  m;
  UINT16  n;
  UINT8   *tmp;
  UINT8   cnt;

  tmp = (UINT8 *) Str;

  //
  // Find bounds for rollover
  //
  m = (UINT16) -1 / 10;
  n = (UINT16) -1 % 10;

  //
  // convert digits
  //
  u = 0;
  cnt = 0;
  c = *(tmp);
  while (c) {
    if (c == '.') {
      cnt=1;
    } else {
      if (c >= '0' && c <= '9') {
        if (u > m || u == m && c - '0' > (INTN) n) {
          *Val = 0;
          *Prec = 0;
          return;
        }
        u = (u * 10) + c - '0';
        if (cnt != 0) {
          cnt++;
        }
      } else {
        break;
      }
    }

    tmp+=2; // jump to the next char in the unicode string
    c = *(tmp);
  }

  *Val = u;
  if (cnt == 0) {
    *Prec = 0;
  } else {
    *Prec = cnt - 1;
  }
}

/**
  
  This routine is a math function which multiplies the Operand times ten to the power of the 
  exponent.
  
  @param  Operand      This is the operand to be acted upon
  @param  Exp             This is the exponent value

  @retval  This is the resulting value of Operand * (10 ^ Exp)

**/
UINT16 
PowTen(
  IN  UINT16 Operand,
  IN  UINT16 Exp
  )
{
  UINT16 i;
  UINT16 val = 1;

  for (i = 0; i < Exp; i++) {
    val *= 10;
  }

  return val * Operand;
}


//#endif

