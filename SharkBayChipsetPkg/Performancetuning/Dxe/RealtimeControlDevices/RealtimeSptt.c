/** @file

  File for building the Voltage $DDD table necessary to add to the SPTT
  table used for Performance Tuning in XTU

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
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include "RealtimeSptt.h"

STATIC SPTT_DATA_PROTOCOL       mSpttDataProtocol = {
  GetRealtimeSettingData
};

STATIC EFI_SMBUS_HC_PROTOCOL        *mSmbusController;
STATIC SPTT_TABLE_SERVICES_PROTOCOL *mSpttTableServices;
//[-start-130710-IB05160465-modify]//
CHIPSET_CONFIGURATION               *mCurrentConfiguration;
//[-end-130710-IB05160465-modify]//
DXE_XTU_POLICY_PROTOCOL             *mXtuPlatformPolicyInstance;

/**
  
  Entry point and initialization function for the Voltage Override component.  This installs 
  the SPTT Data protocol for each independently controllable voltage in the system.

  @param  ImageHandle             Not Used
  @param  SystemTable              Not Used

  @retval EFI_SUCCESS              Initialization completed successfully
  @retval All others                     Failure to initialize the Tuning Core correctly

**/
EFI_STATUS
RealtimeSpttInit (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{

  EFI_STATUS                Status;
  EFI_HANDLE                ProtocolHandle = NULL;

  //
  // Find the SMBus Protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmbusHcProtocolGuid,
                                NULL,
                                (VOID **)&mSmbusController);
   DEBUG ((EFI_D_ERROR, " LocateProtocol mSmbusController Status: %r\n",Status));  

  Status = gBS->LocateProtocol (&gDxePlatformXtuPolicyGuid, NULL, (VOID **)&mXtuPlatformPolicyInstance);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gSpttTableServicesProtocolGuid,
                  NULL,
                  (VOID **)&mSpttTableServices
                  );
   DEBUG ((EFI_D_ERROR, " LocateProtocol mSpttTableServices Status: %r\n",Status)); 
  //
  // Install protocol
  //
  Status = gBS->InstallProtocolInterface(
                  &ProtocolHandle,
                  &gSpttDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSpttDataProtocol
                  );
 DEBUG ((EFI_D_ERROR, " InstallProtocolInterface mSpttDataProtocol Status: %r\n",Status)); 

// InitSpttClockData( );
  return EFI_SUCCESS;
}


EFI_STATUS
GetRealtimeSettingData (
  UINT8                         **outputTable,
  UINTN                         *outputTableSize
  )
{
  EFI_STATUS                    Status;
  UINT8                         *DataPtr;
  UINTN                         TotalLen = 0;
  UINTN                         Index = 0;  
  UINTN                         DefBufferSize = DEFAULT_BUFFER_SIZE;  
  UINTN                         VariableSize;
  EFI_GUID                      GuidId = SYSTEM_CONFIGURATION_GUID;
  IO_VOLTAGE_DEVICE             *IovoltageDevice;
  IO_VOLTAGE_ENTRY              *IovoltageEntry;
  IO_VOLTAGE_VALUE_ENTRY        *IovoltageValueEntry;    
  DEVICE_DESCRIPTION_TABLE      *DeviceDescriptionTable;  
  UINT8                         *Header;  
//////  DEVICE_DESCRIPTION_TABLE      *HostClock;
//////  SMBUS_CLOCK_DEVICE            *HostClockDev;
//////  SMBUS_CLOCK_SETTINGS_ENTRY    *HostClockDevEntry;
//////  CLOCK_HW_CTRL_MODE_ENTRY      *HostClockHwCtrlModeEntry;
//////  UINTN                         HostClockDevEntryLen;  
//////  UINT8                         MaximumValue = 50;
//////  UINT8                         MinimumValue = 1;
//////  CLOCK_VALUE_ENTRY             *HostClockValueEntry;
//////  UINT16                        MDividerValue = 13;
//////  UINT16                        NDividerValue = 0;
//////  UINT16                        i;
//////  SMBUS_VOLTAGE_DEVICE          *VoltageDevice;
//////  SMBUS_VOLTAGE_ENTRY           *VoltageEntry;
//////  SMBUS_VOLTAGE_VALUE_ENTRY     *VoltageValueEntry;  
//////  UINTN                         VoltageDeviceEntryLen;
//////  UINT8                         VoltageSetting;
//////  UINT8                         VoltageParameter9 = 0x2F;

  *outputTable = AllocateZeroPool(DefBufferSize);
  DataPtr = *outputTable;
  Header  = *outputTable;
//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  mCurrentConfiguration = AllocateZeroPool (VariableSize);

  Status = gRT->GetVariable(L"Setup",
                   &GuidId,
                   NULL,
                   &VariableSize,
                   mCurrentConfiguration
                   );                           
  if (EFI_ERROR (Status)) {
  	DEBUG ((EFI_D_ERROR, " GetVariable error :%r :\n",Status));
    mCurrentConfiguration->VIDVal = 0;
  }
//[-end-130709-IB05160465-modify]//

  //
  //============================================
  // DeviceType      : Realtime Voltage
  // Control         : IO
  // Implementation  : Memory Voltage
  //============================================
  //
  if (mXtuPlatformPolicyInstance->EnableRealtimeDevicesSptt) {
    TotalLen += sizeof(DEVICE_DESCRIPTION_TABLE);
    mSpttTableServices->ReallocIfBufferOverflow(&Header, TotalLen, &DefBufferSize);

    DeviceDescriptionTable = (DEVICE_DESCRIPTION_TABLE *)DataPtr;
    DeviceDescriptionTable->Signature                   = DDD_TABLE_SIG;  
    DeviceDescriptionTable->DeviceType                  = PERF_TUNE_VOLTAGE_DEVICE;
    DeviceDescriptionTable->ControlType                 = PERF_TUNE_VOLTAGE_IO_CONTROL;
    DeviceDescriptionTable->ImplementationType          = PERF_TUNE_VOLTAGE_MEMORY_CONTROL;  
    DeviceDescriptionTable->DeviceSpecificDataRevision  = DEVICE_SPECIFIC_DATA_REVISION_1;

    DataPtr += TotalLen;
    IovoltageDevice = (IO_VOLTAGE_DEVICE *) DataPtr;
    TotalLen += sizeof(IO_VOLTAGE_DEVICE);
    mSpttTableServices->ReallocIfBufferOverflow(&Header, TotalLen, &DefBufferSize); 
    IovoltageDevice->Signature                    = IO_VOLTAGE_DEVICE_SIG;
    IovoltageDevice->VoltagValueSettingCount      = 4;
    IovoltageDevice->IOType                       = 2;
    IovoltageDevice->UseIndexPort                 = 0;
    IovoltageDevice->IndexPort                    = 0xFFFFFFFF;
    IovoltageDevice->BasePortOrAddress            = PCH_GPIO_BASE_ADDRESS;
    IovoltageDevice->SizeOfDataInBits             = 32;
    IovoltageDevice->HwCtrlModeSettingValue       = 0;
    IovoltageDevice->HwCtrlModeSettingPrecision   = 0;
    IovoltageDevice->HwCtrlModeSupportFlags       = 0;
    IovoltageDevice->HwCtrlModeEntryCount         = 0;
    
    DataPtr += sizeof(IO_VOLTAGE_DEVICE);

    //
    // Memory Voltage :
    //         | 1.50v | 1.55v | 1.60v | 1.65v
    //---------+-------+-------+-------+--------
    // GPIO_8  |   1   |   0   |   1   |   0
    //---------+-------+-------+-------+--------
    // GPIO_46 |   1   |   1   |   0   |   0
    //

    for (Index = 0 ; Index < IovoltageDevice->VoltagValueSettingCount ; ++Index ) {

      IovoltageEntry = (IO_VOLTAGE_ENTRY *)DataPtr;
      TotalLen += sizeof(IO_VOLTAGE_ENTRY);
      mSpttTableServices->ReallocIfBufferOverflow(&Header, TotalLen, &DefBufferSize); 
    	IovoltageEntry->SettingValue      = (UINT32)(150 + Index *5) ;
    	IovoltageEntry->SettingPrecision  = 2;
      IovoltageEntry->SettingFlags      = 0;
    	IovoltageEntry->VoltageEntrycount = 2;
      
        // Entry1
      	DataPtr +=  sizeof(IO_VOLTAGE_ENTRY);
      	IovoltageValueEntry = (IO_VOLTAGE_VALUE_ENTRY *)DataPtr;

        TotalLen += sizeof(IO_VOLTAGE_VALUE_ENTRY);
        mSpttTableServices->ReallocIfBufferOverflow(&Header, TotalLen, &DefBufferSize); 
        IovoltageValueEntry->OffsetOrIndexValue = R_PCH_GPIO_LVL;   // GP_LVL
        IovoltageValueEntry->ValueMask          = (UINT32)(~BIT8);  // GPIO_8
        IovoltageValueEntry->Value              = ((~Index) & BIT0) ? (UINT32)(BIT8) : 0;
        
        // Entry2
        DataPtr +=  sizeof(IO_VOLTAGE_VALUE_ENTRY);
      	IovoltageValueEntry = (IO_VOLTAGE_VALUE_ENTRY *)DataPtr;

        TotalLen += sizeof(IO_VOLTAGE_VALUE_ENTRY);
        mSpttTableServices->ReallocIfBufferOverflow(&Header, TotalLen, &DefBufferSize); 
        IovoltageValueEntry->OffsetOrIndexValue = R_PCH_GPIO_LVL2;  // GP_LVL2
        IovoltageValueEntry->ValueMask          = (UINT32)(~BIT14); // GPIO_46
        IovoltageValueEntry->Value              = ((~Index) & BIT1) ? (UINT32)(BIT14) : 0;
        DataPtr +=  sizeof(IO_VOLTAGE_VALUE_ENTRY);
     
    }
    DataPtr = (*outputTable + TotalLen);
  }


 *outputTableSize = TotalLen;
  
  return EFI_SUCCESS;
}

