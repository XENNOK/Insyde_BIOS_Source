/** @file
  Provide a chance for OEM platform code to feedback its proprietary settings of the hardware switches.

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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/PeiChipsetSvcLib.h>

/**
  This callback is designed to provide a chance for OEM platform code to feedback its proprietary settings of the hardware switches. 
  It offers an opportunity for OEM customers to indicate if the port is not routed in hardware or 
  if OEM wants it to be not auto detected while it is set to 'PORT_ENABLE'. 
  That means, OEM has 3 choices to a port of PCI-e Ports - 'PORT_DISABLE', 'PCIE_PORT_AUTO', 
  and 'PORT_ENABLE'. For Usb and Sata ports, only two options are available -'PORT_DISABLE' and 'PORT_ENABLE'.

  @param  *PlatformHardwareSwitch    A Pointer to PLATFORM_HARDWARE_SWITCH.

  @retval EFI_SUCCESS                Set hardware switch success.
  @retval EFI_NOT_FOUND		         Locate ReadOnlyVariablePpi failed.
  @retval Others                     Base on OEM design.
**/
EFI_STATUS
OemSvcSetPlatformHardwareSwitchThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  PLATFORM_HARDWARE_SWITCH              **PlatformHardwareSwitchDptr;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  PlatformHardwareSwitchDptr = VA_ARG (Marker, PLATFORM_HARDWARE_SWITCH **);
  VA_END (Marker);

  PeiCsSvcSetPlatformHardwareSwitch(PlatformHardwareSwitchDptr);
  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcSetPlatformHardwareSwitch (
             PlatformHardwareSwitchDptr
             );

  return Status;
}

