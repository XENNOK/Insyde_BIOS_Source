/** @file

  Header file for Me Chipset Lib

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

#ifndef _ME_CHIPSET_LIB_H_
#define _ME_CHIPSET_LIB_H_

typedef enum {
  HECI1 = 1,
  HECI2,
  IDER,
  SOL,
  USBR1,
  USBR2,
  FDSWL,
} ME_DEVICE;

typedef enum {
  Disabled= 0,
  Enabled,
} ME_DEVICE_FUNC_CTRL;

/**
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control

  @retval None
**/
VOID
MeDeviceControl (
  IN  ME_DEVICE                   WhichDevice,
  IN  ME_DEVICE_FUNC_CTRL         DeviceFuncCtrl
  )
;

#endif
