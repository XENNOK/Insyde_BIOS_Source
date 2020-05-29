/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSetSmbiosMemoryData().
 The function DxeCsSvcSetSmbiosMemoryData() use chipset services to collection
 information of memory and set to SMBIOS.
	
***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/MemInfo.h>
#include <CommonSmbiosMemory.h>
//[-start-130325-IB06720210-modify]//
//#include <OemSmbiosMemory.h>
#include <Guid/SlotStatusGuid.h>
#include <Library/HobLib.h>
//[-end-130325-IB06720210-modify]//
#include <SaCommonDefinitions.h>
#include <SaRegs.h>

/**
 To collection information of memory and set to SMBIOS.

 @param[in, out]    ChipsetSmbiosMemData Point to SMBIOS memory data

 @retval            EFI_SUCCESS          This function always return successfully.
*/
EFI_STATUS
SetSmbiosMemoryData (
  IN OUT H2O_SMBIOS_MEM_CONFIG_DATA        *ChipsetSmbiosMemData
  )
{
  EFI_STATUS                      Status;
  EFI_PHYSICAL_ADDRESS            BaseAddress;
  UINT8                           DimmIndex;
  UINT8                           ChannelMode;
  MEM_INFO_PROTOCOL               *MemInfoHob;
  EFI_GUID                        MemInfoProtocolGuid = MEM_INFO_PROTOCOL_GUID;
//[-start-130325-IB06720210-modify]//
//  UINT8                           ChannelASlotNum;
//  UINT8                           ChannelBSlotNum;
//  UINT8                           ChannelASlotMap;
//  UINT8                           ChannelBSlotMap;
  UINT8                           SlotIndex;
  SLOT_STATUS_INFO                *SlotStatusInfo;
  VOID                            *HOB;
  VOID                            *HobList;
//[-end-130325-IB06720210-modify]//
  UINT16                          MaxSockets;

//[-start-130325-IB06720210-modify]//
//  ChannelASlotNum = 0;
//  ChannelBSlotNum = 0;
  SlotIndex  = 0;
  MaxSockets = 0;
  HOB            = NULL;
  HobList        = NULL;
  SlotStatusInfo = NULL;
//[-end-130325-IB06720210-modify]//
  
  Status = gBS->LocateProtocol (&MemInfoProtocolGuid, NULL, (VOID **)&MemInfoHob);

  //
  // These are Chipset specific.
  //
  //
  // Detect ECC  
  // Mobile system doesn't support ECC.
  //
  ChipsetSmbiosMemData->ArrayLocationData.MemoryErrorCorrection = MemoryErrorCorrectionNone;
  if (!EFI_ERROR(Status) && MemInfoHob->MemInfoData.EccSupport) {
    ChipsetSmbiosMemData->ArrayLocationData.MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
  }
  
  //
  // Data for TYPE 16 SMBIOS Structure
  //

  //
  // Create physical array and associated data for all mainboard memory
  //
  ChipsetSmbiosMemData->ArrayLocationData.Location = MemoryArrayLocationSystemBoard;
  ChipsetSmbiosMemData->ArrayLocationData.Use      = MemoryArrayUseSystemMemory;

  //
  // These are Chipset specific.
  //
  //
  // Detect ECC  
  // Mobile system doesn't support ECC.
  //
  ChipsetSmbiosMemData->ArrayLocationData.MemoryErrorCorrection = MemoryErrorCorrectionNone;
  if (MemInfoHob->MemInfoData.EccSupport) {
    ChipsetSmbiosMemData->ArrayLocationData.MemoryErrorCorrection = MemoryErrorCorrectionSingleBitEcc;
  }
//[-start-130325-IB06720210-modify]//
//  ChannelASlotMap = CHANNELA_SLOT_MAP;
//  ChannelBSlotMap = CHANNELB_SLOT_MAP;
//  
//  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
//    if ((ChannelASlotMap >> BitIndex) & BIT0) {
//      ChannelASlotNum ++;
//    }
//    if ((ChannelBSlotMap >> BitIndex) & BIT0) {
//      ChannelBSlotNum ++;
//    }
//  }
//  
//  //
//  //
//  MaxSockets = ChannelASlotNum + ChannelBSlotNum;
  //
  // Get Present Socket Number.
  //
  HOB = GetFirstGuidHob(&gSlotStatusGuid);

  if ( HOB != NULL ) {
    SlotStatusInfo = (SLOT_STATUS_INFO *)((UINT8 *)HOB + sizeof (EFI_HOB_GUID_TYPE));
    for (SlotIndex = 0; SlotIndex < ((SlotStatusInfo->MaxChannel) * (SlotStatusInfo->MaxDimmsInChannel)); SlotIndex++) {
      if (SlotStatusInfo->SlotStatus [SlotIndex] != NOT_PRESENT) {
        MaxSockets++;
      }
    }
    
  } else {
    MaxSockets = MAX_SOCKETS;
  }
//[-end-130325-IB06720210-modify]//
  //
  //  The MemoryCapacity is in kilobytes
  //
  ChipsetSmbiosMemData->ArrayLocationData.MaximumCapacity        = SA_MC_MAX_MEM_CAPACITY * SA_MC_MAX_SIDES * MaxSockets;;
  ChipsetSmbiosMemData->ArrayLocationData.NumberOfMemoryDevices  = MaxSockets;
  
  //
  // Get Memory size parameters for each rank from the chipset registers
  //

  //
  // We start from a base address of 0 for rank 0. We read the DRAM Row
  // Boundary Registers to find the end address of each rank.
  // Now, the size of each rank can be computed from subtracting the base
  // address of that rank from the end address read from the DRB. The
  // base address variable is then incremented by the size of each rank.
  //
  BaseAddress = 0;

  //
  // Channel 0
  //
  for (DimmIndex = 0; DimmIndex < SA_MC_MAX_SLOTS; DimmIndex++) {
    ChipsetSmbiosMemData->RowConfArray[DimmIndex].BaseAddress = BaseAddress;
    ChipsetSmbiosMemData->RowConfArray[DimmIndex*SA_MC_MAX_SLOTS].RowLength = LShiftU64(MemInfoHob->MemInfoData.dimmSize[DimmIndex], 20);
    BaseAddress += ChipsetSmbiosMemData->RowConfArray[DimmIndex].RowLength;
  }

  //
  // If the channel mode is Dual Channel Assymmetric, the the DRB registers for
  // Channel 1 start are offset from the last rank DRB value in Channel 0.
  // Otherwise they are offset from 0
  //
  // Leave it as Interleaved for now
  //
  ChannelMode = 1;
  if (ChannelMode == 1) {
    BaseAddress = 0;
  }

  //
  // Channel 1
  //
  for (DimmIndex = SA_MC_MAX_SLOTS; DimmIndex < SA_MC_MAX_SLOTS * SA_MC_MAX_CHANNELS; DimmIndex++) {
    ChipsetSmbiosMemData->RowConfArray[DimmIndex].BaseAddress = BaseAddress;
    ChipsetSmbiosMemData->RowConfArray[DimmIndex*SA_MC_MAX_SLOTS].RowLength = LShiftU64(MemInfoHob->MemInfoData.dimmSize[DimmIndex], 20);
    BaseAddress += ChipsetSmbiosMemData->RowConfArray[DimmIndex].RowLength;
  }

  ChipsetSmbiosMemData->MemoryDeviceData.ConfiguredMemoryClockSpeed = MemInfoHob->MemInfoData.ddrFreq;
  
#ifdef USE_CLOCK_RATE_AS_THE_UNIT_OF_CONFIGURED_CLOCK_SPEED 
  if (ChipsetSmbiosMemData->MemoryDeviceData.ConfiguredMemoryClockSpeed != 0) {
    ChipsetSmbiosMemData->MemoryDeviceData.ConfiguredMemoryClockSpeed /= 2;
  }
#endif

  ChipsetSmbiosMemData->Interleaved = ChannelMode;
  
  ChipsetSmbiosMemData->MemoryDeviceData.FormFactor = MemoryFormFactorSodimm;

  //
  // Generate Memory Device Mapped Address info (Type 18)
  //
  ChipsetSmbiosMemData->Memory32bitErrorInfo.ErrorType               = MemoryErrorOk;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.ErrorGranularity        = MemoryGranularityOtherUnknown;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.ErrorOperation          = MemoryErrorOperationUnknown;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.VendorSyndrome          = 0;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.MemoryArrayErrorAddress = 0x80000000;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.DeviceErrorAddress      = 0x80000000;
  ChipsetSmbiosMemData->Memory32bitErrorInfo.ErrorResolution         = 0x80000000;

  //
  // Memory Controller Information (Type 5 Obsolete) 
  //
  ChipsetSmbiosMemData->MemoryControllerInfo.ErrDetectMethod                  = ErrorDetectingMethodNone;
  ChipsetSmbiosMemData->MemoryControllerInfo.ErrCorrectCapability.None        = 1;
  ChipsetSmbiosMemData->MemoryControllerInfo.SupportInterleave                = MemoryInterleaveOneWay;
  ChipsetSmbiosMemData->MemoryControllerInfo.CurrentInterleave                = MemoryInterleaveOneWay;
  ChipsetSmbiosMemData->MemoryControllerInfo.MaxMemoryModuleSize              = 0x0D;
  ChipsetSmbiosMemData->MemoryControllerInfo.SupportSpeed.Other               = 1;
  ChipsetSmbiosMemData->MemoryControllerInfo.SupportMemoryType                = 1;
  ChipsetSmbiosMemData->MemoryControllerInfo.AssociatedMemorySlotNum          = (UINT8)MaxSockets;
  
  return EFI_SUCCESS;
}
