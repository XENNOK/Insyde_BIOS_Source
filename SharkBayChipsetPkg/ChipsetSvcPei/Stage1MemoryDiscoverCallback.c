/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcPlatformStage1Init().
 The function PeiCsSvcPlatformStage1Init() use chipset services to install
 Firmware Volume Hob's once there is main memory

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/HobLib.h>

/**
 Install Firmware Volume Hob's once there is main memory

 @param[in]         BootMode            Boot mode

 @retval            EFI_SUCCESS         EFI_SUCCESS if the interface could be successfully installed
*/
EFI_STATUS
Stage1MemoryDiscoverCallback (
  IN  EFI_BOOT_MODE       *BootMode
  )
{
  return EFI_SUCCESS;
}