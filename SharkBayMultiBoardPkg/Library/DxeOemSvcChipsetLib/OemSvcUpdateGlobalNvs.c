/** @file
  This function provides an interface to update GlobalNvs table content.
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
//[-start-121219-IB10820205-modify]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-start-130422-IB05400398-add]//
#include <Library/PcdLib.h>
#include <OemBoardId.h>
//[-end-130422-IB05400398-add]//
/**
 This function provides an interface to update GlobalNvs table content.

 @param[in, out]    mGlobalNvsArea      On entry, points to EFI_GLOBAL_NVS_AREA  instance.
                                        On exit , points to updated EFI_GLOBAL_NVS_AREA  instance.
 @param[in, out]    mOemGlobalNvsArea   On entry, points to EFI_OEM_GLOBAL_NVS_AREA  instance.
                                        On exit , points to updated EFI_OEM_GLOBAL_NVS_AREA  instance.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateGlobalNvs (
  EFI_GLOBAL_NVS_AREA                   *mGlobalNvsArea,
  EFI_OEM_GLOBAL_NVS_AREA               *mOemGlobalNvsArea
  ) {
//[-start-130401-IB10920020-modify]//
//[-start-130509-IB10310019-modify]//
  mOemGlobalNvsArea->OemEmbeddedControllerFlag = PcdGet8 (PcdPchPchPlatformDataEcPresent);
//[-end-130509-IB10310019-modify]//
//[-end-130401-IB10920020-modify]//
//[-start-130422-IB05400398-add]//
  if (PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_HARRIS_BEACH) {
    //
    // This setting is only working on Harris Beach
    //
    mGlobalNvsArea->I2c0SensorDeviceSelection =  1;
    mGlobalNvsArea->I2c1SensorDeviceSelection =  9;
    mGlobalNvsArea->AudioDspCodec          =  0xFF;
  }
//[-end-130422-IB05400398-add]//

  return EFI_MEDIA_CHANGED;
}
//[-end-121219-IB10820205-modify]//