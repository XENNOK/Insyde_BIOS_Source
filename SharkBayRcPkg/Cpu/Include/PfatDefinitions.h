/** @file
  Describes the functions visible to the rest of the Pfat.

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PFAT_DEFINITIONS_H_
#define _PFAT_DEFINITIONS_H_

#ifndef ALIGN_256KB
#define ALIGN_256KB    0x00040000
#endif
#ifndef EFI_PAGE_SIZE
#define EFI_PAGE_SIZE  0x00001000
#endif

///
/// PFAT Module Commands
///
#define PFAT_COMMAND_NOP              0x00             ///< NOP
#define PFAT_COMMAND_BEGIN            0x01             ///< PFAT Begin
#define PFAT_COMMAND_WRITE_INDEX      0x10             ///< PFAT Write Index
#define PFAT_COMMAND_WRITE_IMM        0x11             ///< PFAT Write Inmediate
#define PFAT_COMMAND_READ_INDEX       0x12             ///< PFAT Read Index
#define PFAT_COMMAND_READ_IMM         0x13             ///< PFAT Read Inmediate
#define PFAT_COMMAND_ERASE_BLK        0x14             ///< PFAT Erase Block
#define PFAT_COMMAND_EC_CMD_WR_INDEX  0x20             ///< EC_CMD Write Index
#define PFAT_COMMAND_EC_CMD_WR_IMM    0x21             ///< EC_CMD Write Inmediate
#define PFAT_COMMAND_EC_STS_RD        0x22             ///< EC_CMD Read Status
#define PFAT_COMMAND_EC_DATA_WR_INDEX 0x23             ///< EC_DATA Write Index
#define PFAT_COMMAND_EC_DATA_WR_IMM   0x24             ///< EC_DATA Write Inmediate
#define PFAT_COMMAND_EC_DATA_RD       0x25             ///< EC_DATA Read
#define PFAT_COMMAND_SET_BUFFER_INDEX 0x53             ///< PFAT Set Buffer Index
#define PFAT_COMMAND_SET_FLASH_INDEX  0x55             ///< PFAT Set Flash Index
#define PFAT_COMMAND_END              0xFF             ///< PFAT End

///
/// PFAT Module Error Codes
///
#define ERR_OK                      0x0000             ///< Operation completed without error
#define ERR_UNSUPPORTED_CPU         0x0001             ///< PFAT module detected an incompatibility with the installed CPU
#define ERR_BAD_DIRECTORY           0x0002             ///< PFAT_DIRECTORY check failed
#define ERR_BAD_PPDT                0x0003             ///< A pre-execution check of the PPDT failed
#define ERR_BAD_PUP                 0x0004             ///< An inconsistency was found in the update package
#define ERR_SCRIPT_SYNTAX           0x0005             ///< Unknown operator or name, or invalid syntax found in script
#define ERR_UNDEFINED_FLASH_OBJECT  0x0006             ///< An unimplemented flash object was referenced
#define ERR_INVALID_LINE            0x0007             ///< A JMP, JE, JNE, JG, JGE, JL, or JLE operator has a target that is not within the script buffer (between BEGIN and END inclusive)
#define ERR_BAD_PUPC                0x0008             ///< PUPC inconsistency found
#define ERR_BAD_SVN                 0x0009             ///< PFAT module SVN is lower than required by PPDT
#define ERR_UNEXPECTED_OPCODE       0x000A             ///< An EC related opcode found in a script when the PPDT indicates there is no EC in the system
#define ERR_RANGE_VIOLATION         0x8001             ///< Buffer or flash operation exceeded object bounds
#define ERR_SFAM_VIOLATION          0x8002             ///< An unsigned script attempted to write or erase a bock of flash that overlaps with the SFAM
#define ERR_OVERFLOW                0x8003             ///< An integer overflow occurred
#define ERR_EXEC_LIMIT              0x8004             ///< Total number of script opcodes retired exceeds either platform limit, or global limit
#define ERR_INTERNAL_ERROR          0x8005             ///< An internal consistency check failed within the PFAT module
#define ERR_LAUNCH_FAIL             0xFFFF             ///< CPU detected an error and did not execute the PFAT module

#define PFAT_F0_INDEX               0
#define PFAT_B0_INDEX               0

#define EC_PRESENT                  BIT1
#define EC_PFAT_PROTECTED           BIT2
#define PEG_DMI_FIX                 BIT24;

#define MAX_SPI_COMPONENTS          8
#define PFAT_MEMORY_PAGES           64                 ///< PFAT Module needs 256KB of reserved memory (64 pages of 4KB each one)
#define ALIGNMENT_IN_PAGES          64                 ///< PFAT requires 256KB alignment (64 pages of 4KB each one)

#define PUP_BUFFER_SIZE             0x00014000         ///< 16KB Script + 64KB Flash Block
#define PPDT_MAJOR_VERSION          1
#define PPDT_MINOR_VERSION          0
#define PFAT_SVN                    0x10001
#define PUP_HDR_VERSION             1
#define PSL_MAJOR_VERSION           1
#define PSL_MINOR_VERSION           0

#define PFAT_LOG_VERSION            1
#define PFAT_LOG_OPT_STEP_TRACE     BIT0
#define PFAT_LOG_OPT_BRANCH_TRACE   BIT1
#define PFAT_LOG_OPT_FLASH_WRITE    BIT2
#define PFAT_LOG_OPT_FLASH_ERASE    BIT3
#define PFAT_LOG_OPT_FLASH_ERROR    BIT4
#define PFAT_LOG_OPT_DEBUG          BIT5

#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
///
/// GUID to locate PFAT Module
///
#define PFAT_MODULE_GUID \
  { \
    0x7934156D, 0xCFCE, 0x460E, 0x92, 0xF5, 0xA0, 0x79, 0x09, 0xA5, 0x9E, 0xCA \
  }
///
/// GUID to locate PFAT HOB
///
#define PFAT_HOB_GUID \
  { \
    0x66F0C42D, 0x0D0E, 0x4C23, 0x93, 0xC0, 0x2D, 0x52, 0x95, 0xDC, 0x5E, 0x21 \
  }
#else
///
/// GUID to locate PFAT Module
///
#define PFAT_MODULE_GUID \
  { \
    0x7934156D, 0xCFCE, 0x460E, \
    { \
      0x92, 0xF5, 0xA0, 0x79, 0x09, 0xA5, 0x9E, 0xCA \
    } \
  }
///
/// GUID to locate PFAT HOB
///
#define PFAT_HOB_GUID \
  { \
    0x66F0C42D, 0x0D0E, 0x4C23, \
    { \
      0x93, 0xC0, 0x2D, 0x52, 0x95, 0xDC, 0x5E, 0x21 \
    } \
  }
#endif

#define SPI_SIZE_BASE_512KB  0x80000

typedef enum {
  EnumSpiCompSize512KB  = 0,
  EnumSpiCompSize1MB,
  EnumSpiCompSize2MB,
  EnumSpiCompSize4MB,
  EnumSpiCompSize8MB,
  EnumSpiCompSize16MB,
  EnumSpiCompSize32MB,
  EnumSpiCompSize64MB,
  EnumSpiCompSize128MB
} SPI_COMPONENT_SIZE;

#define MIN_SFAM_COUNT  1
#define MAX_SFAM_COUNT  64

typedef struct {
  UINT32 FirstByte;  ///< Linear flash address of the first byte of the signed range, must be aligned to be first byte in the block. Ordering is little-endian
  UINT32 LastByte;   ///< Linear flash address of the last byte of the signed range, must be aligned to be last byte in the block. Ordering is little-endian
} SFAM_DATA;

///
/// PFAT Platform Data Table  (PPDT)
/// Provides platform specific data required by PFAT Module
///
typedef struct {
  UINT32    PpdtSize;                  ///< Size in bytes of PPDT including SFAM
  UINT16    PpdtMajVer;                ///< Indicates major version of PPDT
  UINT16    PpdtMinVer;                ///< Indicates minor version of PPDT
  UINT8     PlatId[16];                ///< PLAT_ID used to be compared against the one found in the PUP Header to prevent cross platform flashing
  UINT8     PkeySlot0[32];             ///< SHA256 hash for PUP verification key 0
  UINT8     PkeySlot1[32];             ///< SHA256 hash for PUP verification key 1
  UINT8     PkeySlot2[32];             ///< SHA256 hash for PUP verification key 2
  UINT32    PfatModSvn;                ///< PFAT Module SVN
  UINT32    BiosSvn;                   ///< BIOS_SVN to prevent back-flashing
  UINT32    ExecLim;                   ///< Limit the number of opcodes that can be executed on any invocation of PFAT
  ///
  /// Bitmap of platform policy attributes
  ///   BIT[0]: Reserved.  Must be 0
  ///   BIT[2:1] (EC_PRESENT)
  ///     00b = There is not an EC is the system
  ///     01b = There exists an EC in the system, PFAT does not extend any protection to the EC 
  ///     11b = There exist an EC in the system, PFAT extends protection to the EC 
  ///     10b = Reserved.  Must not be used
  ///   BIT[3]:  (DESCRIPTOR_OVERRIDE_POLICY)
  ///     0b = Do not override PFAT security policy
  ///     1b = Override PFAT security policy
  ///   BIT[23:4]: Reserved, must be 0
  ///   BIT[24]: 
  ///     0b = indicates PFAT module will not take additional steps.
  ///     1b = indicates PFAT module should take additional steps to mitigate potential interference from installed PEG device.
  ///   BIT[31:25]  Reserved, must be 0
  ///
  UINT32    PlatAttr;
  ///
  /// BIT[9:0] - 8 bit IO port used for sending EC commands (writes), and reading EC status (reads)
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:10] - Reserved.  Must be 0
  ///
  UINT32    EcCmd;
  ///
  /// BIT[9:0] - 8 bit IO port used for reading and writing data to the EC based on a command issued to EC_CMD
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:10] - Reserved.  Must be 0
  ///
  UINT32    EcData;
  ///
  /// BIT[7:0] - EC command indicating a read of the current EC firmware SVN
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:8] - Reserved.  Must be 0
  ///
  UINT32    EcCmdGetSvn;
  ///
  /// BIT[7:0] - EC command indicating flash begin of flash update session
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:8] - Reserved.  Must be 0
  ///
  UINT32    EcCmdOpen;
  ///
  /// BIT[7:0] - EC command indicating the termination of PFAT protected session
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:8] - Reserved.  Must be 0
  ///
  UINT32    EcCmdClose;
  ///
  /// BIT[7:0] - EC command used to verify connectivity between PFAT and EC
  ///   This field must be populated if PLAT_ATTR.EC_PRESENT != 0
  ///   This field must be zero if PLAT_ATTR.EC_PRESENT == 0
  /// BIT[31:8] - Reserved.  Must be 0
  ///
  UINT32    EcCmdPortTest;
  UINT8     Reserved1[4];
  ///
  /// Defines number of elements in SFAM array
  ///   BIT[5..0]: Index of the last SFAM element
  ///   BIT[7..6]: Reserved for future use.  Must be 0
  ///
  UINT8     LastSfam;
  UINT8     Reserved2[3];
  SFAM_DATA SfamData[MAX_SFAM_COUNT];  ///< Array of flash address map descriptors.  sizeof (SFAM_DESC) == 8
} PPDT;

///
/// PFAT Update Package Header
///
typedef struct {
  UINT16 Version;            ///< Version of the update package header.  Must be 0x0001
  UINT8  Reserved3[2];
  UINT8  PlatId[16];         ///< PLAT_ID used to be compared against the one found in the PPDT to prevent cross platform flashing
  ///
  /// If any bit set in this field then PUP must be signed and valid PUPC must be provided for PUP to be processed
  ///   BIT[0] - Indicates write/erase operations will be executed on protected flash area indicated in the PPDT SFAM
  ///   BIT[1] - Indicates protected EC operations included
  ///
  UINT16 PkgAttributes;
  UINT8  Reserved4[2];
  UINT16 PslMajorVer;        ///< Indicates the PSL major version. Must be 1
  UINT16 PslMinorVer;        ///< Indicates the PSL minor version. Must be 0
  UINT32 ScriptSectionSize;  ///< Size in bytes of the script
  UINT32 DataSectionSize;    ///< Size of the data region in bytes
  UINT32 BiosSvn;            ///< BIOS SVN
  UINT32 EcSvn;              ///< EC SVN
  UINT32 VendorSpecific;
} PUP_HEADER;

typedef struct {
  PUP_HEADER PupHeader;
  UINT64     PupBuffer[PUP_BUFFER_SIZE / 8];
} PUP;

///
/// Memory location for PUPC and PFAT LOG inside PFAT DPR allocated memory for Tool interface
///
#define PUPC_MEMORY_OFFSET        0x00028000                                     ///< PfatMemAddress + PfatMemSize - 160KB
#define PFAT_LOG_MEMORY_OFFSET    0x00020000                                     ///< PfatMemAddress + PfatMemSize - 128KB
#define MAX_PFAT_LOG_PAGE         ((PFAT_LOG_MEMORY_OFFSET / EFI_PAGE_SIZE) - 2) ///< 30 4KB Pages

///
/// PFAT Log
/// The logging facility is used to communicate detailed information regarding the execution of a PFAT script
/// from the SMI handler which invoked the PFAT module itself
///
typedef struct {
  UINT16 Version;            ///< Indicates the version of the log.  Must be 0x0001
  UINT16 LastPage;           ///< Last valid page index for the log
  ///
  /// Bitmap indicating what events to log
  ///   BIT[0] - Step trace - this indicates a full execution trace.  Each line is entered into the log with an EXECUTION_TRACE entry
  ///   BIT[1] - Branch trace - All taken jumps are logged with a BRANCH_TRACE entry
  ///   BIT[2] - Flash write - All flash write operations are logged with a FLASH_WRITE entry
  ///   BIT[3] - Flash erase - All flash erase operations are logged with a FLASH_ERASE entry
  ///   BIT[4] - Flash error - All error conditions from flash operations are logged with FLASH_ERROR entry
  ///   BIT[5] - Debug - Log Debug opcode execution
  ///   BIT[6] - PFAT module debug message - Log implementation specific debug messages from debug module
  ///   BIT[31:7] - Reserved.  Must be 0.  If any reserved bits are set in Header.LoggingOptions, the PFAT module must disable the logging feature
  ///
  UINT32 LoggingOptions;
  UINT8  Reserved5[8];
  UINT32 PfatModSvn;         ///< Indicates a version number of the PFAT module
  UINT32 NumOfEntriesInLog;  ///< Total number of log entries that have been written to the log
} PFAT_LOG;

///
/// HOB used to pass data through every phase of PFAT Bios
/// PFAT Bios code is executed in PEI, DXE and SMM and HOB is the only method to properly pass data
/// between every phase
///
typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  PPDT              Ppdt;                              ///< PFAT Platform Data Table
  ///
  /// PFAT update package header, this header will be appended to all flash updates along with PSL script
  ///
  PUP_HEADER        PupHeader;
  UINT8             NumSpiComponents;                  ///< Number of physical SPI flash components on platform
  UINT8             ComponentSize[MAX_SPI_COMPONENTS]; ///< Array containing size of each flash component
  UINT64            PfatToolsIntIoTrapAdd;             ///< IO Trap address required to Initialize PFAT Tools Interface
  PFAT_LOG          PfatLog;                           ///< Header for PFAT Log Buffer
} PFAT_HOB;

#endif
