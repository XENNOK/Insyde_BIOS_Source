/** @file

Device install

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioInitDxe.h"

/**
  brief-description of function. 

  extended description of function.  
  
**/
EFI_STATUS
InstallAcpi ( 
  IN SIO_DEVICE_LIST_TABLE* DeviceList
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT16                    IoResource;

  IoResource = UpsideDown(DeviceList->DeviceBaseAdr);

  // Set resources and enable this function
  IDW8 (SIO_LDN, DeviceList->DeviceLdn);    
  IDW8 (SIO_BASE_IO_ADDR1_MSB, (UINT8)(IoResource >> 8)); 
  IDW8 (SIO_BASE_IO_ADDR1_LSB, (UINT8)(IoResource & 0xFF));
  IDW8 (SIO_DEV_ACTIVE, TRUE);
    
  return Status;
}

