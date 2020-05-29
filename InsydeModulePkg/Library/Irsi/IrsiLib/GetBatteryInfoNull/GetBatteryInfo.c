/** @file
  Library Instance implementation for IRSI Get Battery Information

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

#include <Uefi.h>
#include <Library/IrsiLib.h>


/**
  Irsi Get Battery Information function

  This routine reports battery information such as AC and battery status and the
  percentage of battery capacity that indicates low battery.

  @param BatteryInfoBuf         pointer to IRSI_GET_BATTERY_INFO structure
  @param BatteryInfoSize        the size of BatteryInfoBuf, normally it is equal to
                                sizeof(IRSI_GET_BATTERY_INFO)

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IrsiGetBatteryInfo (
  IN OUT  VOID     *BatteryInfoBuf
  )
{
   IRSI_GET_BATTERY_INFO *BatteryInfo;

   BatteryInfo                = (IRSI_GET_BATTERY_INFO *)BatteryInfoBuf;
   BatteryInfo->Header.StructureSize = sizeof(IRSI_GET_BATTERY_INFO);

   BatteryInfo->AcStatus      = TRUE;
   BatteryInfo->BatteryStatus = 0;
   BatteryInfo->BatteryBound  = 10;
   BatteryInfo->Header.ReturnStatus = IRSI_STATUS(EFI_SUCCESS);

   return EFI_SUCCESS;
}


/**
  Irsi Get Battery Information Initialization

  This routine is a LibraryClass constructor for IrsiGetBatteryInfo, it will
  register IrsiGetBatterInfo function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiGetBatteryInfoInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_GET_BATTERY_INFO_COMMAND,
               IrsiGetBatteryInfo
               );
               
  return Status;
}