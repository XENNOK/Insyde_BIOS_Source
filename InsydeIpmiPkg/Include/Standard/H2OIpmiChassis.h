/** @file
 H2O IPMI chassis header file.

 This files contains chassis related definitions and structures defined in IPMI
 specification.

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

#ifndef _H2O_IPMI_CHASSIS_H_
#define _H2O_IPMI_CHASSIS_H_


#pragma pack(1)

//
// Chassis Device Command
//
#define H2O_IPMI_CMD_GET_CHASSIS_CAPABILITY     0x00
#define H2O_IPMI_CMD_GET_CHASSIS_STATUS         0x01
#define H2O_IPMI_CMD_CHASSIS_CONTROL            0x02
#define H2O_IPMI_CMD_CHASSIS_RESET              0x03
#define H2O_IPMI_CMD_CHASSIS_IDENTIFY           0x04
#define H2O_IPMI_CMD_SET_FRONT_PANEL_BTN_ENABLE 0x0A
#define H2O_IPMI_CMD_SET_CHASSIS_CAPABILITY     0x05
#define H2O_IPMI_CMD_SET_POWER_RESTORE_POLICY   0x06
#define H2O_IPMI_CMD_SET_POWER_CYCLE_INTERVAL   0x0B
#define H2O_IPMI_CMD_GET_SYSTEM_RESTART_CAUSE   0x07
#define H2O_IPMI_CMD_SET_SYSTEM_BOOT_OPTIONS    0x08
#define H2O_IPMI_CMD_GET_SYSTEM_BOOT_OPTIONS    0x09
#define H2O_IPMI_CMD_GET_POH_COUNTER            0x0F

//
// Get POH Counter
//
typedef struct {
  UINT8  MinutesPerCounter;
  UINT32 Counter;
} H2O_IPMI_POH_COUNTER;

//
// Front Panel Button Capabilities and disable/enable status
//
#define BUTTON_STANDBY_DISABLE_ALLOW       0x80
#define BUTTON_DIAGNOSTIC_DISABLE_ALLOW    0x40
#define BUTTON_RESET_DISABLE_ALLOW         0x20
#define BUTTON_POWER_DISABLE_ALLOW         0x10
#define BUTTON_STANDBY_DISABLED            0x08
#define BUTTON_DIAGNOSTIC_DISABLED         0x04
#define BUTTON_RESET_DISABLED              0x02
#define BUTTON_POWER_DISABLED              0x01

#define DISABLE_BUTTON_STANDBY            0x08
#define DISABLE_BUTTON_DIAGNOSTIC         0x04
#define DISABLE_BUTTON_RESET              0x02
#define DISABLE_BUTTON_POWER              0x01

typedef struct {
  //
  // Byte 1: Current Power State
  //
  UINT8  PowerOn              :1;
  UINT8  PowerOverload        :1;
  UINT8  Interlock            :1;
  UINT8  PowerFault           :1;
  UINT8  PowerCtrlFault       :1;
  UINT8  PowerRestorePolicy   :2;
  UINT8  Reserved             :1;

  //
  // Byte 2: Last Power Event
  //
  UINT8  AcFailed             :1;
  UINT8  LastPowerOverload    :1;
  UINT8  LastPowerInterlock   :1;
  UINT8  LastPowerFault       :1;
  UINT8  LastPowerOn          :1;
  UINT8  Reserved1            :3;

  //
  // Byte 3: Misc. Chassis State
  //
  UINT8  ChassisIntrusion       :1;
  UINT8  FrontPanelLockout      :1;
  UINT8  DriveFault             :1;
  UINT8  CoolingFault           :1;
  UINT8  ChassisIdentifyState   :2;
  UINT8  ChassisIdentifySupport :1;
  UINT8  Reserved2              :1;

  //
  // Byte 4:  Front Panel Button Capabilities and disable/enable status
  //
  UINT8  PowerBtnDisabled       :1;
  UINT8  ResetBtnDisabled       :1;
  UINT8  DiagIntBtnDisabled     :1;
  UINT8  StandbyBtnDisabled     :1;
  UINT8  PowerBtnDisableAllow   :1;
  UINT8  ResetBtnDisableAllow   :1;
  UINT8  DiagIntBtnDisableAllow :1;
  UINT8  StandbyBtnDisableAllow :1;
  
} H2O_IPMI_GET_CHASSIS_STATUS;

//
// Parameters of "Get System Boot Option"/"Set System Boot Option" command
//
#define BOOT_OPTION_PARA_SET_IN_PROGRESS              0
#define BOOT_OPTION_PARA_SERVICE_PARTITION_SELECTOR   1
#define BOOT_OPTION_PARA_SERVICE_PARTITION_SCAN       2
#define BOOT_OPTION_PARA_BOOT_FLAG_VAILD_BIT_CLEAN    3
#define BOOT_OPTION_PARA_BOOT_INFO_ACK                4
#define BOOT_OPTION_PARA_BOOT_FLAGS                   5
#define BOOT_OPTION_PARA_BOOT_INITIATOR_INFO          6
#define BOOT_OPTION_PARA_BOOT_INITIATOR_MAILBOX       7

#define BOOT_OPTION_BOOT_TYPE_LEGACY        0
#define BOOT_OPTION_BOOT_TYPE_EFI           1
#define BOOT_OPTION_LOCK_KEYBOARD           1
#define BOOT_OPTION_EFI_BOOT_SUPPORTED      0
#define BOOT_OPTION_EFI_BOOT_NOT_SUPPORTED  1
#define BOOT_OPTION_BOOT_FLAG_VALID         1

typedef enum {
  NoOverride = 0x0,
  ForcePxe,
  ForceDefaultHardDisk,
  ForceDefaultHardDiskSafeMode,
  ForceDefaultDiagnosticPartition,
  ForceDefaultCD,
  ForceSetupUtility,
  ForceRemoteRemovableMedia,
  ForceRemoteCD,
  ForcePrimaryRemoteMedia,
  ForceRemoteHardDisk = 0xB,
  ForcePrimaryRemovableMedia = 0xF
} BOOT_DEVICE_SELECTOR;

//
// Get System Boot Option data structure
//
typedef struct {
  UINT8 ParameterVersion           :4;
  UINT8 Reserved1                  :4;
  UINT8 ParameterSelector          :7;
  UINT8 ParameterValid             :1;
  //
  // Boot Flags Data 1
  //
  UINT8 Reserved2                  :5;
  UINT8 BiosBootType               :1;
  UINT8 Persistent                 :1;
  UINT8 BootFlagsValid             :1;
  //
  // Boot Flags Data 2
  //
  UINT8 LockResetBtn               :1;
  UINT8 ScreenBlank                :1;
  UINT8 BootDeviceSelector         :4;
  UINT8 LockKeyboard               :1;
  UINT8 ClearCmos                  :1;
  //
  // Boot Flags Data 3
  //
  UINT8 ConsoleRedirectionControl  :2;
  UINT8 LockSleepBtn               :1;
  UINT8 UserPasswordByPass         :1;
  UINT8 Reserved3                  :1;
  UINT8 FirmwareVerbosity          :2;
  UINT8 LockPowerBtn               :1;
  //
  // Boot Flags Data 4
  //
  UINT8 MuxControlOverride         :3;
  UINT8 ShareModeOverride          :1;
  UINT8 Reserved4                  :4;
  //
  // Boot Flags Data 5
  //
  UINT8 DeviceInstanceSelector     :5;
  UINT8 Reserved5                  :3;
} H2O_IPMI_GET_BOOT_OPTION;

//
// Set System Boot Option data structure
//
typedef struct {
  UINT8 ParameterSelector          :7;
  UINT8 Reserved1                  :1;
  //
  // Boot Flags Data 1
  //
  UINT8 Reserved2                  :5;
  UINT8 BiosBootType               :1;
  UINT8 Persistent                 :1;
  UINT8 BootFlagsValid             :1;
  //
  // Boot Flags Data 2
  //
  UINT8 LockResetBtn               :1;
  UINT8 ScreenBlank                :1;
  UINT8 BootDeviceSelector         :4;
  UINT8 LockKeyboard               :1;
  UINT8 ClearCmos                  :1;
  //
  // Boot Flags Data 3
  //
  UINT8 ConsoleRedirectionControl  :2;
  UINT8 LockSleepBtn               :1;
  UINT8 UserPasswordByPass         :1;
  UINT8 Reserved3                  :1;
  UINT8 FirmwareVerbosity          :2;
  UINT8 LockPowerBtn               :1;
  //
  // Boot Flags Data 4
  //
  UINT8 MuxControlOverride         :3;
  UINT8 ShareModeOverride          :1;
  UINT8 Reserved4                  :4;
  //
  // Boot Flags Data 5
  //
  UINT8 DeviceInstanceSelector     :4;
  UINT8 Reserved5                  :4;
} H2O_IPMI_SET_BOOT_OPTION;

#pragma pack()


#endif

