/** @file
  Abstraction of Alert Standard Format (ASF) 2.0

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

#ifndef __ALERT_STANDARD_FORMAT_PROTOCOL_H__
#define __ALERT_STANDARD_FORMAT_PROTOCOL_H__

#define EFI_ALERT_STANDARD_FORMAT_PROTOCOL_GUID \
  { 0xcc93a70b, 0xec27, 0x49c5, 0x8b, 0x34, 0x13, 0x93, 0x1e, 0xfe, 0xd6, 0xe2 }

typedef struct _EFI_ALERT_STANDARD_FORMAT_PROTOCOL   EFI_ALERT_STANDARD_FORMAT_PROTOCOL;

#pragma pack(1)
typedef struct {
  UINT8     SubCommand;
  UINT8     Version;
  UINT32    IanaId;
  UINT8     SpecialCommand;
  UINT16    SpecialCommandParam;
  UINT16    BootOptions;
  UINT16    OemParameters;
} EFI_ASF_BOOT_OPTIONS;

typedef struct {
  UINT8     SubCommand;
  UINT8     Version;
  UINT8     EventSensorType;
  UINT8     EventType;
  UINT8     EventOffset;
  UINT8     EventSourceType;
  UINT8     EventSeverity;
  UINT8     SensorDevice;
  UINT8     SensorNumber;
  UINT8     Entity;
  UINT8     EntityInstance;
  UINT8     Data0;
  UINT8     Data1;
  UINT8     Data2;
  UINT8     Data3;
  UINT8     Data4;  

} EFI_ASF_MESSAGE;

typedef struct {
  UINT8     SubCommand;
  UINT8     Version;
} EFI_ASF_CLEAR_BOOT_OPTIONS;
#pragma pack()

//
// Special Command Attributes
//
#define NOP                             0x00
#define FORCE_PXE                       0x01
#define FORCE_HARDDRIVE                 0x02
#define FORCE_SAFEMODE                  0x03
#define FORCE_DIAGNOSTICS               0x04
#define FORCE_CDDVD                     0x05

//
// Boot Options Mask
#define LOCK_POWER_BUTTON               0x0002 // 0000 0000 0000 0010 - bit 1
#define LOCK_RESET_BUTTON               0x0004 // 0000 0000 0000 0200 - bit 2
#define LOCK_KEYBOARD                   0x0020 // 0000 0000 0010 0000 - bit 5
#define LOCK_SLEEP_BUTTON               0x0040 // 0000 0000 0100 0000 - bit 6

#define USER_PASSWORD_BYPASS            0x0800 // 0000 1000 0000 0000 - bit 3
#define FORCE_PROGRESS_EVENTS           0x1000 // 0001 0000 0000 0000 - bit 4
#define FIRMWARE_VERBOSITY_DEFAULT      0x0000 // 0000 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_QUIET        0x2000 // 0010 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_VERBOSE      0x4000 // 0100 0000 0000 0000 - bit 6:5
#define FIRMWARE_VERBOSITY_BLANK        0x6000 // 0110 0000 0000 0000 - bit 6:5
#define CONFIG_DATA_RESET               0x8000 // 1000 0000 0000 0000 - bit 7

#define ASF_BOOT_OPTIONS_PRESENT        0x16
#define ASF_BOOT_OPTIONS_NOT_PRESENT    0x17

#define ASF_INDUSTRY_IANA               0x11BE // 4542

/**
 Return the SMBus address used by the ASF driver.

 @param [in]   This
 @param [out]  SmbusDeviceAddress

 @retval EFI_SUCCESS            Address returned
 @retval EFI_INVALID_PARAMETER  Invalid SMBus address

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ALERT_STANDARD_FORMAT_PROTOCOL_GET_SMBUSADDR) (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  OUT UINTN                                *SmbusDeviceAddress
);

/**
 Set the SMBus address used by the ASF driver. 0 is an invalid address.

 @param [in]   This
 @param [in]   SmbusDeviceAddress

 @retval EFI_SUCCESS            Address set
 @retval EFI_INVALID_PARAMETER  Invalid SMBus address

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ALERT_STANDARD_FORMAT_PROTOCOL_SET_SMBUSADDR) (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  UINTN                                SmbusDeviceAddress
);

/**
 Return the ASF Boot Options obtained from the controller. If the
 Boot Options parameter is NULL and no boot options have been retrieved,
 Query the ASF controller for its boot options.

 @param [in]   This
 @param [in, out] AsfBootOptions  Pointer to ASF boot options to copy current ASF Boot options

 @retval EFI_SUCCESS            Boot options copied
 @retval EFI_NOT_READY          No boot options

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ALERT_STANDARD_FORMAT_PROTOCOL_GET_BOOT_OPTIONS) (
  IN      EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  OUT EFI_ASF_BOOT_OPTIONS                 **AsfBootOptions
);

/**
 Send ASF Message.

 @param [in]   This
 @param [in]   AsfMessage       Pointer to ASF message

 @retval EFI_SUCCESS            Boot options copied
 @retval EFI_INVALID_PARAMETER  Invalid pointer
 @retval EFI_NOT_READY          No controller

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ALERT_STANDARD_FORMAT_PROTOCOL_SEND_ASF_MESSAGE) (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  EFI_ASF_MESSAGE                      *AsfMessage
);

struct _EFI_ALERT_STANDARD_FORMAT_PROTOCOL {
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL_GET_SMBUSADDR          GetSmbusAddr;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL_SET_SMBUSADDR          SetSmbusAddr;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL_GET_BOOT_OPTIONS       GetBootOptions;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL_SEND_ASF_MESSAGE       SendAsfMessage;
};

extern EFI_GUID gEfiAlertStandardFormatProtocolGuid;

#endif
