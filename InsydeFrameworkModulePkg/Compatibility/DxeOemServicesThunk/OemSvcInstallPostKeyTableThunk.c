/** @file
  Provide OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager).

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
  This OemService provides OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager). 

  @param  KeyDetected           A bit map of the monitored keys found.
                                Bit N corresponds to KeyList[N] as provided by the 
                                GetUsbPlatformOptions () API of UsbLegacy protocol.
  @param  ScanCode              The Scan Code.
  @param  *PostOperation        Point to the operation flag which imply the behavior of utility choosing in post time. For example: choose the SCU or Boot Manager. 

  @retval EFI_SUCCESS           Get post operation success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallPostKeyTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 KeyDetected;
  UINT16                                ScanCode;
  UINTN                                 *PostOperation;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  KeyDetected   = VA_ARG (Marker, UINTN);
  ScanCode      = VA_ARG (Marker, UINT16);
  PostOperation = VA_ARG (Marker, UINTN *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallPostKeyTable (
             KeyDetected,
             ScanCode,
             PostOperation
             );

  return Status;
}
