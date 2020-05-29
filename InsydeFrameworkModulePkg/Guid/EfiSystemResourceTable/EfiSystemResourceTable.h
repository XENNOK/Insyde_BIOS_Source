/** @file
  Definitions for EFI System Resource Table

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

#ifndef _EFI_SYSTEM_RESOURCE_TABLE_H_
#define _EFI_SYSTEM_RESOURCE_TABLE_H_


#define EFI_SYSTEM_RESOURCE_TABLE_GUID \
  { \
    0xb122a263, 0x3661, 0x4f68, {0x99, 0x29, 0x78, 0xf8, 0xb0, 0xd6, 0x21, 0x80} \
  }
#define WINDOWS_UX_CAPSULE_GUID \
  { \
    0x3b8c8162, 0x188c, 0x46a4, {0xae, 0xc9, 0xbe, 0x43, 0xf1, 0xd6, 0x56, 0x97} \
  }

#define MAX_FIRMWARE_RESOURCES            4   //FixedPcdGet32(PcdFirmwareResourceMaximum)

#define UNKNOWN_FIRMWARE_TYPE             0
#define SYSTEM_FIRMWARE_TYPE              1
#define DEVICE_FIRMWARE_TYPE              2
#define FIRMWARE_DRIVER_TYPE              3


#define ESRT_SUCCESS                      0x00000000
#define ESRT_ERROR_UNSUCCESSFUL           0x00000001
#define ESRT_ERROR_INSUFFICIENT_RESOURCES 0x00000002
#define ESRT_ERROR_INCORRECT_VERSION      0x00000003
#define ESRT_ERROR_INVALID_IMAGE_FORMAT   0x00000004
#define ESRT_ERROR_AUTHENTICATION         0x00000005
#define ESRT_ERROR_AC_NOT_CONNECTED       0x00000006
#define ESRT_ERROR_INSUFFICIENT_BATTERY   0x00000007

#define LAST_ATTEMPT_VERSION_NAME         L"LastAttemptVersion"
#define LAST_ATTEMPT_STATUS               L"LastAttemptStatus"

typedef UINT32      ESRT_STATUS;

#pragma pack(1)

typedef struct {
  EFI_GUID          FirmwareClass;
  UINT32            FirmwareType;
  UINT32            FirmwareVersion;
  UINT32            LowestSupportedFirmwareVersion;
  UINT32            CapsuleFlags;
  UINT32            LastAttemptVersion;
  UINT32            LastAttemptStatus;
} FIRMWARE_RESOURCE;

typedef struct {
  UINT32            FirmwareResourceCount;
  UINT32            FirmwareResourceMaximum;
  UINT64            FirmwareResourceVersion;
  FIRMWARE_RESOURCE FirmwareResources[MAX_FIRMWARE_RESOURCES];
} EFI_SYSTEM_RESOURCE_TABLE;

#pragma pack()

extern EFI_GUID gEfiSystemResourceTableGuid;
extern EFI_GUID gEsrtSystemFirmwareGuid;
extern EFI_GUID gWindowsUxCapsuleGuid;
#endif