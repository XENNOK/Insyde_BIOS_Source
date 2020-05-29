/** @file
  DebugDriver driver will print out the driver connection information.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DEBUGDRIVER_DRIVER_H
#define _DEBUGDRIVER_DRIVER_H

#include <Uefi.h>
#include <PiDxe.h>
#include <Library\DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePathToText.h>

#define LANGUAGE_CODE_ENGLISH1   "eng"
#define LANGUAGE_CODE_ENGLISH2   "en-US"
#define MAX_DRIVER_BINDING_INFO         300


typedef struct _EFI_DRIVER_BINDING_INFO {
  EFI_DRIVER_BINDING_PROTOCOL           *DriveBinding;
  EFI_DRIVER_BINDING_SUPPORTED          Supported;
  EFI_DRIVER_BINDING_START              Start;
  EFI_DRIVER_BINDING_STOP               Stop;
} EFI_DRIVER_BINDING_INFO;

#endif
