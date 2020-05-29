/** @file

  Include for USB Key Provisioning for AMT.

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

#ifndef _USB_PROVISION_H_
#define _USB_PROVISION_H_

#include <IndustryStandard/Pci.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Guid/FileInfo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>

#pragma pack(1)

typedef struct {
  EFI_GUID  FileTypeUUID;
  UINT16    RecordChunkCount;
  UINT16    RecordHeaderByteCount;
  UINT32    RecordNumber;
  UINT8     MajorVersion;
  UINT8     MinorVersion;
  UINT16    FileFlags;
  UINT32    DataRecordCount;
  UINT32    DataRecordsConsumed;
  UINT16    DataRecordChunkCount;
  UINT16    Reserved2;
  UINT16    ModuleList[(127 - 9) * 2];
} FILE_HEADER_RECORD;

///
/// File Flags definition (V2 and above)
///
#define FILE_FLAGS_DONT_CONSUME_RECORDS 0x01

//
// FileTypeUUID
//
#define FILE_TYPE_UUID \
  { \
    0x71FB16B5, 0xCB87, 0x4AF9, 0xB4, 0x41, 0xCA, 0x7B, 0x38, 0x35, 0x78, 0xF9 \
  }

#define FILE_TYPE_UUID_2X \
  { \
    0x5881B296, 0x6BCF, 0x4C72, 0x8B, 0x91, 0xA1, 0x5E, 0x51, 0x2E, 0x99, 0xC4 \
  }

#define FILE_TYPE_UUID_3 \
  { \
    0xC6F6F7A7, 0xC489, 0x47F6, 0x93, 0xED, 0xE2, 0xE5, 0x02, 0x0D, 0xA5, 0x1D \
  }

#define FILE_TYPE_UUID_4 \
  { \
    0x5234A9AA, 0x29E1, 0x44A9, 0x8D, 0x4D, 0x08, 0x1C, 0x07, 0xB9, 0x63, 0x53 \
  }

//
// MajorVersion
//
#define MAJOR_VERSION     0x01  ///< Check this value
#define MINOR_VERSION     0x00  ///< Check this value
#define MAJOR_VERSION_20  0x02
#define MINOR_VERSION_20  0x00
#define MAJOR_VERSION_21  0x02
#define MINOR_VERSION_21  0x01
#define MAJOR_VERSION_30  0x03
#define MINOR_VERSION_30  0x00
#define MAJOR_VERSION_40  0x04
#define MINOR_VERSION_40  0x00

#define VERSION_10        0x0001
#define VERSION_20        0x0002
#define VERSION_21        0x0102
#define VERSION_30        0x0003

typedef struct {
  UINT32  RecordTypeIdentifier;
  UINT32  RecordFlags;
  UINT32  Reserved1[2];
  UINT16  RecordChunkCount;
  UINT16  RecordHeaderByteCount;
  UINT32  RecordNumber;
} DATA_RECORD_HEADER;

//
// RecordTypeIdentifier
//
#define DATA_RECORD_TYPE_INVALID      0
#define DATA_RECORD_TYPE_DATA_RECORD  1

///
/// RecordFlags
///
#define DATA_RECORD_FLAG_VALID  1

typedef struct {
  UINT16  ModuleIdentifier;
  UINT16  VariableIdentifier;
  UINT16  VariableLenght;
  UINT16  Reserved1;
  VOID    *VariableValue;
} DATA_RECORD_ENTRY;

//
// ModuleIdentifier
//
#define MODULE_IDENTIFIER_INVALID       0
#define MODULE_IDENTIFIER_ME_KERNEL     1
#define MODULE_IDENTIFIER_INTEL_AMT_CM  2

//
// VariableIdentifier
// MODULE_IDENTIFIER_ME_KERNEL
//
#define VARIABLE_IDENTIFIER_ME_KERNEL_INVALID               0
#define VARIABLE_IDENTIFIER_ME_KERNEL_CURRENT_MEBX_PASSWORD 1
#define VARIABLE_IDENTIFIER_ME_KERNEL_NEW_MEBX_PASSWORD     2

//
// MODULE_IDENTIFIER_INTEL_AMT_CM
//
#define VARIABLE_IDENTIFIER_INTEL_AMT_CM_INVALID  0
#define VARIABLE_IDENTIFIER_INTEL_AMT_CM_PID      1
#define VARIABLE_IDENTIFIER_INTEL_AMT_CM_PPS      2

typedef struct {
  UINT32  USBKeyLocationInfo;
  UINT32  Reserved1[3];
} USB_KEY_PROVISIONING;

//
// DISK Definition
//
///
/// PART_RECORD
///
typedef struct _PART_RECORD {
  UINT8   BootIndicator;
  UINT8   StartHead;
  UINT8   StartSector;
  UINT8   StartCylinder;
  UINT8   PartitionType;
  UINT8   EndHead;
  UINT8   EndSector;
  UINT8   EndCylinder;
  UINT32  StartLba;
  UINT32  SizeInLba;
} PART_RECORD;

///
/// MBR Partition table
///
typedef struct _MBR {
  UINT8       CodeArea[440];
  UINT32      OptionalDiskSig;
  UINT16      Reserved;
  PART_RECORD PartRecord[4];
  UINT16      Sig;
} MASTER_BOOT_RECORD;

typedef struct _BPB_RECORD {
  UINT8   BS_JmpBoot[3];
  UINT8   BS_OEMName[8];
  UINT16  BPB_BytePerSec;
  UINT8   BPB_SecPerClus;
  UINT16  BPB_RsvdSecCnt;
  UINT8   BPB_NumFATs;
  UINT16  BPB_RootEntCnt;
  UINT16  BPB_TotSec16;
  UINT8   BPB_Media;
  UINT16  BPB_FATSz16;
  UINT16  BPB_SecPerTrk;
  UINT16  BPB_NumHeads;
  UINT32  BPB_Hiddsec;
  UINT32  BPB_TotSec32;
} BPB_RECORD;

#pragma pack()

#define CHUNK_SIZE                      512

#define USB_PROVISIONING_DATA_FILE_NAME L"\\setup.bin"

#define USB_DATA_RECORD_CONSUMED_OFFSET OFFSET_OF (FILE_HEADER_RECORD, DataRecordsConsumed)

/**
  Remove Consumed Data Record in USB Key Provisioning Data File

  @param[in] None.

  @retval None
**/
VOID
UsbConsumedDataRecordRemove (
  VOID
  );

/**
  Restore Consumed Data Record in USB Key Provisioning Data File

  @param[in] None.

  @retval None
**/
VOID
UsbConsumedDataRecordRestore (
  VOID
  );

/**
  Check USB Key Provisioning Data

  @param[in] None.

  @retval None
**/
VOID
UsbKeyProvisioning (
  VOID
  );


#endif /// _USB_PROVISION_H_
