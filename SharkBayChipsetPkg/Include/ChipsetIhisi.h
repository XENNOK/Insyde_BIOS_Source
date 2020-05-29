/** @file
  This file provides IHISI Structure and define for Chipset driver

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

#ifndef _CHIPSET_IHISI_H_
#define _CHIPSET_IHISI_H_

#ifndef _IHISI_LIB_H_
//
// FBTS typedef struct
//
typedef struct {
  UINT8                                 Type;
  UINT32                                Address;
  UINT32                                Length;
} FBTS_PLATFORM_ROM_MAP;

typedef struct {
  FBTS_PLATFORM_ROM_MAP                 PlatFormRomMap [40];
} FBTS_PLATFORM_ROM_MAP_BUFFER;

typedef struct {
  UINT32                                LinearAddress;
  UINT32                                Size;
} FBTS_PLATFORM_PRIVATE_ROM;

typedef struct {
  FBTS_PLATFORM_PRIVATE_ROM             PlatFormRomMap [40];
} FBTS_PLATFORM_PRIVATE_ROM_BUFFER;
#endif

#define EFI_CACHE_VARIABLE_MTRR_BASE  0x200
#define EFI_CACHE_MTRR_VALID          0x800

typedef struct {
  UINT8      SizeIndex;
  UINT32     Base;
} FLASH_BASE_MAP_TABLE;

//[-start-130717-IB05400431-add]//
#define FLASH_SIZE_32768K           0x09
#define ROM_32M_BASE                0xFE000000
//[-end-130717-IB05400431-add]//
#define ROM_16M_BASE                0xFF000000
#define ROM_8M_BASE                 0xFF800000
#define ROM_4M_BASE                 0xFFC00000
#define ROM_2M_BASE                 0xFFE00000
#define ROM_1M_BASE                 0xFFF00000

#define FBTS_VERSION                0x2001        // Update current version to 2.0.1

#define FBTS_PERMISSION_DENY        0x0000
#define FBTS_PERMISSION_ALLOWED     0x0100

//
// FbtsGetPlatformInfo code.
//
#define  bit(a)                   1 << (a)
#define  AP_DO_NOTHING            0
#define  MODEL_NAME_CHECK         bit(0)
#define  MODEL_VERSION_CHECK      bit(1)
#define  ALLOW_SAME_VERSION_FILE  bit(2)
#define  VERIFY_FILE_CHECKSUM     bit(3)


typedef struct {
  UINT8      IhisiAreaType;
  UINT8      FlashAreaType;
} FLASH_MAP_MAPPING_TABLE;

#define FLASH_VALID_SIGNATURE           0x0FF0A55A      //Flash Descriptor is considered valid
#define FLASH_MAP_0_OFFSET              0x04
#define FLASH_REGION_BASE_MASK          0x00ff0000
#define FLASH_MAP_1_OFFSET              0x08
#define FLASH_MASTER_BASE_MASK          0x000000ff
#define PLATFORM_REGION_OFFSET          0x00
#define ROM_FILE_REGION_OFFSET          0x100
#define FLASH_MASTER_1_READ_ACCESS_BIT  16
#define FLASH_MASTER_1_WRITE_ACCESS_BIT 24
#define TDT_STATE_INACTIVE              0x0
#define TDT_STATE_ACTIVE                0x01
#define TDT_STATE_LOCK_OR_STOLEN        0x02
#define DOS_FLASH_TOOL                  0x01
#define WINDOWS_FLASH_TOOL              0x02
#define LINUX_FLASH_TOOL                0x03
#define SHELL_FLASH_TOOL                0x04
//
// $VER 0x52455624
//
#define FBTS_VERSION_SIGNATURE          0x52455624
//
// $IHISI
//
#define FBTS_ATp_VERSION_SIGNATURE      0x495349484924
//
// Insyde software corp vendor id
//
#define FBTS_VENDER_ID                  0x1656

#pragma pack(1)

typedef struct {
  UINT32                                Signature;
  UINT32                                Size;
  UINT8                                 DataType;
  UINT8                                 DataSize;
  UINT32                                PhysicalDataSize;
} AP_COMMUNICATION_DATA_TABLE;

//[-start-120917-IB05300333-modify]//
typedef struct {
  UINT32                                Signature;
  UINT32                                Size;
  UINT8                                 BlockSize;
  UINT8                                 DataSize;
  UINT32                                PhysicalDataSize;
  UINT16                                ErrorReturn;
  UINT64                                ReturnCode;
  UINT64                                ExtSignature;
  EFI_GUID                              ExtGuid;
  UINT32                                ExtErrorReturn;
} BIOS_COMMUNICATION_DATA_TABLE;
//[-end-120917-IB05300333-modify]//
//[-start-120913-IB05300329-add]//
typedef struct {
  UINT16  Version;
  UINT8   Reserved[2];
  UINT32  Algorithm;
} PUPC_HEADER;

typedef struct {
  UINT16  Version;
  UINT8   Reserved[2];
  UINT32  Algorithm;
  UINT8   VKeyMod[256];
  UINT8   VKeyExp[4];
  UINT8   Signature[256];
} PUPC_ALGORITHM_1;
//[-end-120913-IB05300329-add]//

#pragma pack()

//[-start-120913-IB05300329-modify]//
//[-start-120917-IB05300333-modify]//
//
// OEM Extra Data Communication Data type code.
//
typedef enum {
  Vbios = 1,                     // VBIOS
  Reserved02,                    // Reserved
  Reserved03,                    // Reserved
  // 04h~0Ch  User Define
  // 0Dh~4Fh  Reserved
  Oa30ReadWrite = 0x50,          // 50h = OA 3.0 Read/Write
  Oa30Erase,                     // 51h = OA 3.0 Erase (Reset to default)
  Oa30PopulateHeader,            // 52h = OA 3.0 Populate Header
  Oa30DePopulateHeader,          // 53h = OA 3.0 De-Populate Header
  LogoUpdate,                    // 54h = Logo Update (Write)
  CheckBiosSignBySystemBios,     // 55h = Check BIOS sign by System BIOS
  // 56h~FFh  Reserved
  PfatUpdate = 0x58,             // 58h = PFAT Update
  BiosPasswordErase,             // 59h = Erase BIOS Password
  PfatReturn                     // 5Ah = PFAT Return
  // 5Bh~FFh  Reserved
} OEM_EXTRA_DATA_INPUT_DATATYPE_CODE;
//[-end-120917-IB05300333-modify]//
//[-end-120913-IB05300329-modify]//

//
// OEM Extra Data Communication Data size definition.
//
typedef enum {
  DataSize64k,                 // 64k
  DataSize128k,                // 128k
  DataSize256k,                // 256k
  DataSize512k,                // 512k
  DataSize1024k,               // 1024k
  DataSize2048k,               // 2048k
  DataSize4096k,               // 4096k
  DataSize8192k,               // 8192k

  NotContained = 0xFD,         // BIOS send Data size is not contained in above 00~07h definition.
                               // The data size should refer to "Physical data size" filed which defines in offset 0Ah.
                               // (BIOS will send data to application for extra data read function 47h if field is FDh)
  SkipSizeCheck,               // BIOS acknowledge skip size check request from caller;
                               // BIOS will also skip flash data size check too.
                               // (Application will send data to BIOS for extra data write function 42h if field is FEh)
  AllowPhysicalSizeToCaller,   // BIOS report it allows the physical size to caller,
                               // the physical data size should refer to "Physical data size" filed which define in offset 0Ah.
                               // (Application will send data to BIOS for extra data write function 42h if field is FFh)
} OEM_EXTRA_DATA_DATASIZE_TABLE;

//
// OEM Extra Data Block size definition.
//
typedef enum {
  BlockSize4k,                 // 4k
  BlockSize64k,                // 64k
  // 02h~FEh = Reserved
  MaximunBlockSize = 0xFF,     // Maximum block size.
                               // Note: Maximum block size is defined in Physical Data size field (offset 0Ah).
                               // If this field is FFh and block size is smaller than 64k,
                               // Data size field (offset 09h) must be set to FFh or FDh too.
} OEM_EXTRA_DATA_OUTPUT_BLOCK_SIZE_TABLE;

//
// OEM Extra Data Write Shutdown mode description.
//
typedef enum {
  DoNothing,                   // 00h=Do nothing
  WindowsReboot,               // 01h=Windows reboot
  WindowsShutdown,             // 02h=Windows shutdown
  DosReboot,                   // 03h=DOS reboot
  DosShutdown,                 // 04h=DOS shutdown
  ContinueToFlash = 0xEF,      // Continue to flash
} OEM_EXTRA_DATA_SHUTDOWN_MODE_TABLE;

#endif
