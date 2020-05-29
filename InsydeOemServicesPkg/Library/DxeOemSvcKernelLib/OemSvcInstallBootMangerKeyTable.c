/** @file
  Provides an opportunity for OEM to define the operation keys of Boot Manager Utility.

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

/*++

Todo:
  Define the relateaed data.
  
SCAN_CODE_TO_SCREEN_OPERATION     ScanCodeToOperation[] = {
  //  UINT16              ScanCode;
  //  CHAR16              UnicodeChar;
  //  UI_SCREEN_OPERATION ScreenOperation;

//{ ScanCode, UnicodeChar , ScreenOperation}

  { 0,        0,            UiMaxOperation }

};

--*/

/**
  This OemServices provides OEM to define the operation keys of Boot Manager Utility.

  @param[out]  *ScanCodeToOperationCount  The number of the operations in Boot Manager Utility.
  @param[out]  *mScanCodeToOperation      The table defines the operation of scan code for Boot Manager Utility.

  @retval EFI_UNSUPPORTED                 Returns unsupported by default.
  @retval EFI_SUCCESS                     Get key table success.
  @retval EFI_MEDIA_CHANGED               The value of IN OUT parameter is changed. 
  @retval Others                          Base on OEM design.
**/
EFI_STATUS
OemSvcInstallBootMangerKeyTable (
  OUT UINTN                                 *ScanCodeToOperationCount,
  OUT SCAN_CODE_TO_SCREEN_OPERATION         **mScanCodeToOperation
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
