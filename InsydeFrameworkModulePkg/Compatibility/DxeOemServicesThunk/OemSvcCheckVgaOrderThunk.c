/** @file
  Provide OEM to find the active VGA device and modify the primacy VGA device in the platform.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>


/**
  This OemService provides OEM to find the active VGA device in the platform, 
  and OEM can modify the policy to determine the primacy VGA device.

  @param  *PlugInPciVgaDevicePath  A pointer to the plug in PCI VGA device. If this device is unsupported, it points to NULL.
  @param  *PossibleAgpDevicePath   A pointer to the AGP VGA device or on-chip AGP device. If this device is unsupported, it points to NULL.
  @param  *PossiblePcieDevicePath  A pointer to the PCI-e VGA device. If this device is unsupported, it points to NULL.
  @param  *SelectedVGA             A pointer to the selected number of VGA device. The hooked function based on this parameter to determine the active VGA.

  @retval EFI_SUCCESS              Select VGA device success.
  @retval EFI_UNSUPPORTED          There is no active VGA device.
**/
EFI_STATUS
OemSvcCheckVgaOrderThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}


