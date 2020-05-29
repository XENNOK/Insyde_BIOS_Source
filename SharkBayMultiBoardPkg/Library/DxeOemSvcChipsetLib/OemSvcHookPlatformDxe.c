/** @file
  This function provides an interface to hook before and after DxePlatformEntryPoint (PlatformDxe.inf).

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcChipsetLib.h>
#include <SetupConfig.h>
//[-start-120719-IB03600493-add]//
//[-start-130812-IB06720232-modify]//
//#include <OemEcLib.h>
#include <Library/CommonEcLib.h>
#include <OemEc.h>
#include <KscLib.h>
#include <Library/DebugLib.h>
//[-end-130812-IB06720232-modify]//
//[-end-120719-IB03600493-add]//

EFI_STATUS
SetECShutdownTemperature (
  UINT8    ShutdownTemperature
);

/**
 This function provides an interface to hook before and after DxePlatformEntryPoint (PlatformDxe.inf).PlatformDxe.inf
 is responsible for setting up any platform specific policy or initialization information.

 @param[in out]     *SetupVariable      On entry, points to SYSTEM_CONFIGURATION instance.
                                        On exit , points to updated SYSTEM_CONFIGURATION instance.
 @param[in out]     *PciRootBridgeIo    On entry, points to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
                                        On exit , points to updated EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
 @param[in out]     Flag                TRUE : Before DxePlatformEntryPoint.
                                        FALSE: After DxePlatformEntryPoint.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookPlatformDxe (
  IN  CHIPSET_CONFIGURATION            *SetupVariable,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  IN  BOOLEAN                          Flag
  )
{
  if ( Flag )
  {
    //Operation 1
    return EFI_UNSUPPORTED;
  } else {
    //Operation 2
    //[-start-120719-IB03600493-add]//
    //
    // Set EC shut down temperature
    //
    SetECShutdownTemperature (120);
    //[-end-120719-IB03600493-add]//
    return EFI_MEDIA_CHANGED;
  }

}


//[-start-130812-IB06720232-add]//
/**
  Set EC shutdown temperature.

  @param    Shutdowntemperature         EC shutdown temperature
  @retval   EFI_SUCCESS 
  @retval   Others
**/
EFI_STATUS
SetECShutdownTemperature (
  UINT8    ShutdownTemperature
)
{
  EFI_STATUS    Status;  

  Status = WaitKbcIbe (SMC_CMD_STATE);
  if (Status == EFI_SUCCESS) {
    Status = WriteKbc (SMC_CMD_STATE, KSC_C_SET_CTEMP);
  } 
  Status = WaitKbcIbe (SMC_CMD_STATE);
  if (Status == EFI_SUCCESS) {
    Status = WriteKbc (SMC_DATA, ShutdownTemperature);
  }
  ASSERT_EFI_ERROR (Status);
  return Status;
}

//[-end-130812-IB06720232-add]//
