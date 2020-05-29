/** @file
  Provides an opportunity for OEM to calculate new CMOS checksum and write it.

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

/**
  This OemService provides an opportunity for OEM to calculate new CMOS checksum and write it. 
  The hook function of the service is SaveSetupConfig () that saves the setup configuration to CMOS and calls this service.

  @param       none                  Based on OEM design.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           The service is customized in the project.
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcCalculateWriteCmosChecksum (
  VOID
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
