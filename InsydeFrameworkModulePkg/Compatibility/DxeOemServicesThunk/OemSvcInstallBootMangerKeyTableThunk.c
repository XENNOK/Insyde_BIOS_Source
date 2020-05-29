/** @file
  Provides OEM to define the operation keys of Boot Manager Utility.

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
  This OemServices provides OEM to define the operation keys of Boot Manager Utility.

  @param  *ScanCodeToOperationCount  The number of the operations in Boot Manager Utility.
  @param  *mScanCodeToOperation      The table defines the operation of scan code for Boot Manager Utility.

  @retval EFI_SUCCESS                Get key table success.
  @retval Others                     Base on OEM design.
**/
EFI_STATUS
OemSvcInstallBootMangerKeyTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 *ScanCodeToOperationCount;
  SCAN_CODE_TO_SCREEN_OPERATION         **mScanCodeToOperation;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  ScanCodeToOperationCount = VA_ARG (Marker, UINTN *);
  mScanCodeToOperation     = VA_ARG (Marker, SCAN_CODE_TO_SCREEN_OPERATION **);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallBootMangerKeyTable (
             ScanCodeToOperationCount,
             mScanCodeToOperation
             );

  return Status;
}
