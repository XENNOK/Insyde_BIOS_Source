/** @file
  Modify resources befer installing protocol.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/CpuIo2.h>
#include <Library/SioLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  Before install device protocol, it can change value to PCD data, per device info in PCD

  @param[in]       SioInstance      Which SIO pass in.
  @param[IN, OUT]  *PcdPointer      PCD data 
  @param[IN]       SioConfig        The index port of the configuration of the SIO

  @retval EFI_UNSUPPORTED           Returns unsupported by default.
  @retval EFI_SUCCESS               The service is customized in the project.
  @retval EFI_MEDIA_CHANGED         The value of IN OUT parameter is changed. 
  @retval Others                    Depends on customization.
**/
EFI_STATUS
OemSvcDeviceResourceSioDxe (
  IN UINT8                           SioInstance,
  IN OUT SIO_DEVICE_LIST_TABLE        *PcdPointer,
  IN UINT16                           SioConfig
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
