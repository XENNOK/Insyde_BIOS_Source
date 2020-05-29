/** @file
  IRSI Library Class definitions

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

#ifndef _IRSI_LIB_H_
#define _IRSI_LIB_H_

#include <PiDxe.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FvRegionInfo.h>
#include <Library/IrsiRegistrationLib.h>

#define IRSI_GET_VERSION_COMMAND               0x00000001
#define IRSI_GET_BIOS_INFO_COMMAND             0x00000002
#define IRSI_GET_PLATFORM_INFO_COMMAND         0x00000003
#define IRSI_GET_BATTERY_INFO_COMMAND          0x00000004
#define IRSI_READ_IMAGE_COMMAND                0x00000005
#define IRSI_WRITE_IMAGE_COMMAND               0x00000006
#define IRSI_READ_VARIABLE_COMMAND             0x00000007
#define IRSI_WRITE_VARIABLE_COMMAND            0x00000008
#define IRSI_FACTORY_COPY_MANAGEMENT_COMMAND   0x00000009
#define IRSI_UPDATE_CAPSULE_COMMAND            0x0000000A

#define IRSI_ERASE_OA_KEY_COMMAND              0x00005002

#define BACKUP_FACTORY_COPY                    0x00000000
#define RESTORE_FACTORY_COPY                   0x00000001


#define MODEL_NAME_LEN                         128
#define MODEL_VERSION_LEN                      128
#define TOOL_VERSION_LEN                       32

#pragma pack(1)

typedef struct {
  IRSI_HEADER         Header;
  UINT16              ToolEnvironment;
  UINT16              ToolType;
  CHAR8               ToolVersion[TOOL_VERSION_LEN];
  UINT64              SpecVersion;
} IRSI_GET_VERSION;

typedef struct {
  IRSI_HEADER         Header;
  UINT32              BiosSize;
} IRSI_GET_BIOS_INFO;

typedef struct {
  IRSI_HEADER         Header;
  CHAR16              ModelName[MODEL_NAME_LEN];
  CHAR16              ModelVersion[MODEL_VERSION_LEN];
} IRSI_GET_PLATFORM_INFO;

typedef struct {
  IRSI_HEADER         Header;
  UINT32              AcStatus;
  UINT32              BatteryStatus;
  UINT32              BatteryBound;
} IRSI_GET_BATTERY_INFO;

typedef struct {
  IRSI_HEADER         Header;
  EFI_GUID            VariableGuid;
  PHYSICAL_ADDRESS    VariableNamePtr;
  UINT32              VariableSize;
  UINT32              Attribute;
  UINT16              VariableChecksum;
  PHYSICAL_ADDRESS    VariableDataPtr;
} IRSI_VARIABLE;

typedef struct {
  IRSI_HEADER         Header;
  EFI_GUID            ImageTypeGuid;
  UINT32              ImageSize;
  PHYSICAL_ADDRESS    ImageBufferPtr;
  UINT8               UpdateStatus;
  UINT8               Reserved[7];
} IRSI_READ_IMAGE;

typedef IRSI_READ_IMAGE IRSI_WRITE_IMAGE;

typedef struct {
  IRSI_HEADER         Header;
  UINT32              FactoryCopyCmd;
} IRSI_FACTORY_COPY_MANAGEMENT;
typedef struct {
  IRSI_HEADER         Header;
  UINT32              BootPathSize;
  CHAR16              BootPath[1];
} IRSI_UPDATE_CAPSULE;
#pragma pack()

#endif

