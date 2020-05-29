/** @file
  Provide OEM to calculate new CMOS checksum and write it.

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
  This OemService provides OEM to calculate new CMOS checksum and write it. 
  The hook function of the service is SaveSetupConfig () that provides to save the setup configuration to CMOS and call this service.

  @retval EFI_SUCCESS           OEM Callback function is success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcCalculateWriteCmosChecksumThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcCalculateWriteCmosChecksum ();

  return Status;
}
