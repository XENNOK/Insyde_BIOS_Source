/** @file
  Provide OEM to get the HotPlugBridgeInfo table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>
#include <SetupConfig.h>
#include <Protocol/SetupUtility.h>
#include <Library/UefiBootServicesTableLib.h>
//[-start-130206-IB03780481-add]//
#include <OemThunderbolt.h>
//[-end-130206-IB03780481-add]//
#include <Library/BaseLib.h>

//[-start-130709-IB05400426-modify]//
//#ifndef THUNDERBOLT_SUPPORT
HOT_PLUG_BRIDGE_INFO  HotPlugBridgeInfo [] = {
//  NodeUID    DevNum,     FuncNum,    ReservedBusCount,     ReservedIoRange,   ReservedNonPrefetchableMmio,  AlignemntOfReservedNonPrefetchableMmio, ReservedPrefetchableMmio, AlignemntOfReservedPrefetchableMmio
  {   0x00,     0x1C,           0,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           1,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           2,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           3,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           4,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           5,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           6,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000},
  {   0x00,     0x1C,           7,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000}, 
//  {   0x01,           0,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000}, 
  { END_OF_TABLE,  0,           0,                   0,                   0,                             0,                         0,                             0,                         0}, //End of Table, Cannot be removed.
};
//#else
////[-start-130513-IB10300037-modify]//
////[-start-130307-IB03780481-modify]//
//HOT_PLUG_BRIDGE_INFO  HotPlugBridgeInfo [] = {
////  DevNum,     FuncNum,    ReservedBusCount,     ReservedIoRange,  ReservedNonPrefetchableMmio,  AlignemntOfReservedNonPrefetchableMmio, ReservedPrefetchableMmio, AlignemntOfReservedPrefetchableMmio
//  // Thunderbolt sample code 1.3 add resource limit check. Please allocate enough resource here to prevent CIO is disabled by sample code.
//  {   0x1C,           4,                  56,                 0x0,                    0x16100000,                 0x4000000,                    0x22000000,                0x10000000},
//  {   0x01,           0,                   5,              0x1000,                     0x1000000,                 0x1000000,                     0x1000000,                 0x1000000}, 
//  { END_OF_TABLE,     0,                   0,                   0,                             0,                         0,                             0,                         0}, //End of Table, Cannot be removed.
//};
////[-end-130307-IB03780481-modify]//
////[-end-130513-IB10300037-modify]//
//#endif
//[-end-130709-IB05400426-modify]//


/**
  Provide OEM to get the HotPlugBridgeInfo table. 
  This table contains the hot plug bridge address info and defines reserved BUS, I/O and memory range for bridge device.

  @param[out]  *HotPlugBridgeInfoTable  Point to HOT_PLUG_BRIDGE_INFO Table

  @retval      EFI_MEDIA_CHANGED        Get hot plug bridge info success.
  @retval      Others                   Depends on customization.
**/
EFI_STATUS
OemSvcGetHotplugBridgeInfo (
  OUT HOT_PLUG_BRIDGE_INFO              **HotPlugBridgeInfoTable
  )
{
  EFI_STATUS                            Status;
  EFI_SETUP_UTILITY_PROTOCOL            *EfiSetupUtility;
  SYSTEM_CONFIGURATION                  *SystemConfiguration = NULL; 
//[-start-130220-IB03780481-add]//
#ifdef THUNDERBOLT_SUPPORT
  UINT8                                 Index;
#endif
//[-end-130220-IB03780481-add]//

  //
  // Get NVRAM data
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);
  if (Status == EFI_SUCCESS) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)EfiSetupUtility->SetupNvData;
    if(!SystemConfiguration->PcieRootPortHotPlug0) {
      HotPlugBridgeInfo[0].NodeUID = IGNORE_DEVICE;
    }
//[-start-130709-IB05400426-modify]//
//#ifndef THUNDERBOLT_SUPPORT
    if(!SystemConfiguration->PcieRootPortHotPlug1) {
      HotPlugBridgeInfo[1].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug2) {
      HotPlugBridgeInfo[2].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug3) {
      HotPlugBridgeInfo[3].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug4) {
      HotPlugBridgeInfo[4].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug5) {
      HotPlugBridgeInfo[5].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug6) {
      HotPlugBridgeInfo[6].NodeUID = IGNORE_DEVICE;
    }
    if(!SystemConfiguration->PcieRootPortHotPlug7) {
      HotPlugBridgeInfo[7].NodeUID = IGNORE_DEVICE;
    }
//#endif
//[-end-130709-IB05400426-modify]//

//[-start-130206-IB03780481-add]//
#ifdef THUNDERBOLT_SUPPORT
//[-start-130709-IB05400426-modify]//
    if (SystemConfiguration->TbtDevice != 0x00) {
      //
      // Current Format: HotPlugBridgeInfo[ "Function Num" ]
      //
      if (SystemConfiguration->TbtHotPlug) {
        HotPlugBridgeInfo[(SystemConfiguration->TbtDevice - 1)].NodeUID = THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM;
      }

      Index = 0;
      while (HotPlugBridgeInfo[Index].NodeUID != END_OF_TABLE){
        if ((HotPlugBridgeInfo[Index].NodeUID == THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM) &&
            (HotPlugBridgeInfo[Index].FuncNum == (SystemConfiguration->TbtDevice - 1))) {
          HotPlugBridgeInfo[Index].ReservedBusCount                       = SystemConfiguration->TbtRsvdBus;
//[-start-130513-IB10300037-modify]//
//        HotPlugBridgeInfo[Index].ReservedIoRange                        = SystemConfiguration->TbtRsvdIo;
          HotPlugBridgeInfo[Index].ReservedIoRange                        = 0x1000;
//[-end-130513-IB10300037-modify]//
          HotPlugBridgeInfo[Index].ReservedNonPrefetchableMmio            = LShiftU64 (SystemConfiguration->TbtRsvdMem, 20);
          HotPlugBridgeInfo[Index].ReservedPrefetchableMmio               = LShiftU64 (SystemConfiguration->TbtRsvdPmem, 20);
          HotPlugBridgeInfo[Index].AlignemntOfReservedNonPrefetchableMmio = LShiftU64 (1, SystemConfiguration->TbtRsvdMemAlign);
          HotPlugBridgeInfo[Index].AlignemntOfReservedPrefetchableMmio    = LShiftU64 (1, SystemConfiguration->TbtRsvdPmemAlign);
          break;
        }
        Index++;
      }
    }
//[-end-130709-IB05400426-modify]//
#endif
//[-end-130206-IB03780481-add]//
  }
  *HotPlugBridgeInfoTable = HotPlugBridgeInfo;

  return EFI_MEDIA_CHANGED;
}
