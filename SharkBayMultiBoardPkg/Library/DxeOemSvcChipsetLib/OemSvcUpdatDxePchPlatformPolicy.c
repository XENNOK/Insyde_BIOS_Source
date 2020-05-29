/** @file
  This function offers an interface to modify DXE_PCH_PLATFORM_POLICY_PROTOCOL Data before the system
  installs DXE_PCH_PLATFORM_POLICY_PROTOCOL.
;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Protocol/SetupUtility.h>
#include <OemBoardId.h>


/**
 This function offers an interface to modify DXE_PCH_PLATFORM_POLICY_PROTOCOL Data before the system
 installs DXE_PCH_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *PchPolicy          On entry, points to DXE_PCH_PLATFORM_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_PCH_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatDxePchPlatformPolicy (
  IN OUT DXE_PCH_PLATFORM_POLICY_PROTOCOL         *PchPolicy
  )
{
  EFI_SETUP_UTILITY_PROTOCOL       *SetupUtility;
  CHIPSET_CONFIGURATION            *SetupVariable;
  EFI_STATUS                       Status;
  
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  
  if (FeaturePcdGet (PcdUltFlag) && PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_HARRIS_BEACH) {
    PchPolicy->SerialIoConfig->I2c0VoltageSelect = PchSerialIoIs33V;
    PchPolicy->DeviceEnabling->SerialIoDma    = SetupVariable->EnableSerialIoDma;
    PchPolicy->DeviceEnabling->SerialIoI2c0   = SetupVariable->EnableSerialIoI2c0;
    PchPolicy->DeviceEnabling->SerialIoI2c1   = SetupVariable->EnableSerialIoI2c1;
    PchPolicy->DeviceEnabling->SerialIoSpi0   = PCH_DEVICE_DISABLE;
    PchPolicy->DeviceEnabling->SerialIoSpi1   = PCH_DEVICE_DISABLE;
    PchPolicy->DeviceEnabling->SerialIoUart0  = SetupVariable->EnableSerialIoUart0;
    PchPolicy->DeviceEnabling->SerialIoUart1  = SetupVariable->EnableSerialIoUart1;
    PchPolicy->DeviceEnabling->SerialIoSdio   = SetupVariable->EnableSerialIoSdio;
    PchPolicy->DeviceEnabling->AudioDsp       = PCH_DEVICE_DISABLE;  
  }

  return EFI_UNSUPPORTED;
}
