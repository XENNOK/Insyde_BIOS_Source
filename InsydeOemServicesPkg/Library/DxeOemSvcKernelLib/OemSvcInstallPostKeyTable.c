/** @file
  Provide OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager).

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
  
SCAN_TO_OPERATION     PostKeyToOperation[] = {
  //  UINT32              KeyBit
  //  UINT16              ScanCode
  //  UINT8               PostOperation

//{ KeyBit, ScanCode,  PostOperation }
  { 0,      SCAN_NULL, NO_OPERATION  }
};

--*/

/**
  This OemService provides OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager). 

  @param[in]   KeyDetected           A bit map of the monitored keys found.
                                     Bit N corresponds to KeyList[N] as provided by the 
                                     GetUsbPlatformOptions () API of UsbLegacy protocol.
  @param[in]   ScanCode              The Scan Code.
  @param[out]  *PostOperation        Point to the operation flag which imply the behavior of utility choosing in post time. For example: choose the SCU or Boot Manager. 

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get post operation success.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallPostKeyTable (
  IN  UINTN                          KeyDetected,
  IN  UINT16                         ScanCode,
  OUT UINTN                          *PostOperation
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
