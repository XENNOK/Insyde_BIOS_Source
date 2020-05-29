/** @file
  Provide OEM to define SLP string which is based on OEM specification.

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
  This OemService provides OEM to define SLP string which based on OEM specification. 
  This service only supports the legacy OS (XP system).

  @param  *SlpStringLength      The length of SLP string.
  @param  *SlpStringAddress     A pointer to the address of SLP string. 

  @retval EFI_SUCCESS           Get SLP string success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallLegacyBiosOemSlpThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 *SlpLength;
  UINTN                                 *SlpAddress;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  SlpLength  = VA_ARG (Marker, UINTN *);
  SlpAddress = VA_ARG (Marker, UINTN *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallLegacyBiosOemSlp (
             SlpLength,
             SlpAddress
             );

  return Status;
}
