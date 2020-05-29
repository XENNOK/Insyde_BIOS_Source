/** @file
  It8728F Super I/O Protocol

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

#ifndef _IT8728F_PROTOCOL_H_
#define _IT8728F_PROTOCOL_H_

#define SIO_IT8728F_PROTOCOL_GUID \
  { \
    0x9be05994, 0x200, 0x4170, 0xa6, 0x14, 0xed, 0x96, 0x15, 0xe3, 0x1c, 0x6b \
  }

#include <Protocol/IsaAcpi.h>
#include <SuperIoDefine.h>

typedef struct _SIO_IT8728F_PROTOCOL SIO_IT8728F_PROTOCOL;

typedef enum {
  HWM_FAN_TACHOMETER1,
  HWM_FAN_TACHOMETER2,
  HWM_FAN_TACHOMETER3,
  HWM_VIN0,
  HWM_VIN1,
  HWM_VIN2,
  HWM_VIN3,
  HWM_VIN4,
  HWM_VIN5,
  HWM_VIN6,
  HWM_3VSB,
  HWM_VBAT,
  HWM_TMPIN1,
  HWM_TMPIN2,
  HWM_TMPIN3,
} HWM_VALUE_SELECT;

typedef struct {
  UINT8     Address;
  UINT8     Data;
} EFI_SIO_IT8728F_EC_CTRL_REG;

typedef
VOID
(EFIAPI *SET_SCRIPT_FLAG) (
  IN BOOLEAN  Flag
  );

typedef
EFI_STATUS
(EFIAPI *INITIAL_IT8728F) (
  IN UINT16  Port
  );

typedef
VOID
(EFIAPI *CONFIG_MODE) (
  IN BOOLEAN  OnOff
  );

typedef
VOID
(EFIAPI *INIT_FUNCTION) (
  VOID
  );

typedef
VOID
(EFIAPI *ENV_CTRL_INIT_FUNCTION) (
  IN UINT16  Port,
  IN BOOLEAN Enable
  );

typedef
VOID
(EFIAPI *ENV_CTRL_PME_FUNCTION) (
  IN UINT16  Port,
  IN BOOLEAN Enable
  );

typedef
VOID
(EFIAPI *ENV_CTRL_REG_INIT_FUNCTION) (
  IN EFI_SIO_IT8728F_EC_CTRL_REG  *EcCtrlRegisterList
  );

typedef
VOID
(EFIAPI *ENV_CTRL_WRITE) (
  IN UINT8       Register,
  IN UINT8       Value8
  );

typedef
UINT8
(EFIAPI *ENV_CTRL_READ) (
  IN UINT8 Index
  );

typedef
VOID
(EFIAPI *ENV_FAN_CTRL) (
  IN  HWM_VALUE_SELECT  Select,
  IN  UINT8             OffTemperature,
  IN  UINT8             StartTemperature,
  IN  UINT8             FullSpeedTemperature,
  IN  UINT8             StartPWM,
  IN  UINT8             Slop,
  IN  UINT8             Delta
  );

typedef
EFI_STATUS
(EFIAPI *HW_MONITOR_READ) (
  IN  HWM_VALUE_SELECT  Select,
  IN  UINTN             R1,
  IN  UINTN             R2,
  OUT INTN              *Value
  );

typedef struct {
  INIT_FUNCTION         InitFunction;
  CONFIG_REG_READ       ConfigRegRead;
  CONFIG_REG_WRITE      ConfigRegWrite;
} EFI_SIO_IT8728F_GPIO_FUNCTION;

typedef struct {
  ENV_CTRL_INIT_FUNCTION     InitFunction;
  ENV_CTRL_PME_FUNCTION      PmeFunction;
  ENV_CTRL_REG_INIT_FUNCTION InitECCtrlReg;
  CONFIG_REG_READ            ConfigRegRead;
  CONFIG_REG_WRITE           ConfigRegWrite;
  ENV_CTRL_WRITE             EnvCtrlWrite;
  ENV_CTRL_READ              EnvCtrlRead;
  ENV_FAN_CTRL               EnvCtrlFanControl;
  HW_MONITOR_READ            HWMValueRead;
} EFI_SIO_IT8728F_ENV_CTRL_FUNCTION;

typedef struct {
  EFI_UNI_SIO                            *Sio;
  INITIAL_IT8728F                        InitSio;
  SET_SCRIPT_FLAG                        SetScriptFlag;
  CONFIG_MODE                            SioConfigMode;
  EFI_SIO_ISA_FUNCTION                   *Com1;
  EFI_SIO_ISA_FUNCTION                   *Com2;
  EFI_SIO_ISA_FUNCTION                   *Ps2K;
  EFI_SIO_ISA_FUNCTION                   *Ps2M;
  EFI_SIO_ISA_FUNCTION                   *Lpt1;
  EFI_SIO_ISA_FUNCTION                   *Fdc;
  EFI_SIO_ISA_FUNCTION                   *CIR;
  EFI_SIO_IT8728F_ENV_CTRL_FUNCTION       *EnvCtrl;
  EFI_SIO_IT8728F_GPIO_FUNCTION          *Gpio;
} EFI_SIO_IT8728F_FUNCTION;

extern EFI_GUID gEfiSioIt8728fProtocolGuid;

#endif
