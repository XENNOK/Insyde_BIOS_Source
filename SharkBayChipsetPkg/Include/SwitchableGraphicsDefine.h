/** @file
  SwitchableGraphicsDefine.h

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SWITCHABLE_GRAPHICS_DEFINE_H_
#define _SWITCHABLE_GRAPHICS_DEFINE_H_

#ifndef DGPU_DEVICE_NUM
#define DGPU_DEVICE_NUM                       (0x00)
#endif

#ifndef DGPU_FUNCTION_NUM
#define DGPU_FUNCTION_NUM                     (0x00)
#endif

#ifndef AMD_VID
#define AMD_VID                               (0x1002)
#endif

#ifndef NVIDIA_VID
#define NVIDIA_VID                            (0x10DE)
#endif

#ifndef PEG_CAP_ID
#define PEG_CAP_ID                            (0x10)
#endif

#ifndef PEG_CAP_VER
#define PEG_CAP_VER                           (0x2)
#endif

#ifndef NVIDIA_DGPU_HDA_REGISTER
#define NVIDIA_DGPU_HDA_REGISTER              (0x488)
#endif

//
// Support and Unsupport define
//
typedef enum {
  Unsupport = 0,
  Support
} SUPPORT_SETTING;

//
// Active and Inactive define
//
typedef enum {
  Inactive = 0,
  Active
} ACTIVE_SETTING;

//
// PrimaryDisplay settings
//
typedef enum {
  DisplayModeIgpu  = 0,
  DisplayModeDgpu,
  DisplayModePci,
  DisplayModeAuto,
  DisplayModeSg
} PRIMARY_DISPLAY_SETTING;

//
// IGDControl settings define
//
typedef enum {
  IgdDisable  = 0,
  IgdEnable,
  IgdAuto
} IGD_CONTROL_SETTING;

//
// AlwaysEnablePeg settings
//
typedef enum {
  DontAlwaysEnablePeg  = 0,
  AlwaysEnablePeg
} ALWAYS_ENABLE_PEG_SETTING;

//
// EnSGFunction settings
//
typedef enum {
  DisableSgFunction  = 0,
  EnableSgFunction
} ENABLE_SG_FUNCTION_SETTING;

//
// PowerXpress settings
//
typedef enum {
  MuxedFixed   = 1,
  MuxlessFixed = 5,
  MuxlessDynamic,
  MuxlessFixedDynamic,
  FullDgpuPowerOffDynamic,
  FixedFullDgpuPowerOffDynamic = 13
} POWER_XPRESS_SETTING;

//
// SgMode settings
//
typedef enum {
  SgModeDisabled  = 0,
  SgModeMuxed,
  SgModeMuxless,
  SgModeDgpu
} SG_MODE_SETTING;

//
// BootType settings
//
typedef enum {
  DualBootType  = 0,
  LegacyBootType,
  EfiBootType
} BOOT_TYPE_SETTING;

//
// SgGpioSupport setting
//
typedef enum {
  SgGpioNotSupported  = 0,
  SgGpioSupported
} SG_GPIO_SUPPORT_SETTING;

#endif
