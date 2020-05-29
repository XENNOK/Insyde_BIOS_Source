/** @file
  Definition for Base OEM Services Chipset Lib.

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

#ifndef _BASE_OEM_SVC_CHIPSET_LIB_H_
#define _BASE_OEM_SVC_CHIPSET_LIB_H_

#include <Uefi.h>
#include <Library/FdSupportLib.h>

/**
 This function offers an interface to modify FLASH_DEVICE matrix before the system detects FlashDevice.
 
 @param[in, out]    ***OemFlashDevice   On entry, points to FLASH_DEVICE matrix.
                                        On exit , the size of updated FLASH_DEVICE matrix.
 @param[in, out]    *Size               On entry, the size of FLASH_DEVICE matrix.
                                        On exit , the size of updated FLASH_DEVICE matrix.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
//[-start-130916-IB05670200-remove]//
//EFI_STATUS
//UpdateFlashDevice (
//  FLASH_DEVICE       ***OemFlashDevice,
//  UINTN              *Size
//  );
//[-end-130916-IB05670200-remove]//

//[-start-130808-IB06720232-add]//
EFI_STATUS
OemSvcEcSaveRestoreKbc (
  IN  BOOLEAN                       SaveRestoreFlag
  );

EFI_STATUS
OemSvcEcPowerState (
  IN  BOOLEAN                       *PowerStateIsAc
  );

EFI_STATUS
OemSvcEcVersion (
  OUT EFI_STATUS   *ReadEcVersionStatus,
  OUT UINT8        *MajorNum,
  OUT UINT8        *MinorNum
  );

EFI_STATUS
OemSvcEcSetDswMode (
  OUT EFI_STATUS   *SetDswModeStatus,
  IN  UINT8        DswMode
  );

EFI_STATUS
OemSvcEcGetLidState (
  OUT EFI_STATUS   *EcGetLidState,
  OUT UINT8        *LidIsOpen
  );
//[-end-130808-IB06720232-add]//

#endif