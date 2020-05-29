/** @file
  This file defines the EFI SPI Protocol which implements the Intel(R) PCH SPI Host Controller Compatibility Interface.

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

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2006 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Spi.h

@brief
  This file defines the EFI SPI Protocol which implements the
  Intel(R) PCH SPI Host Controller Compatibility Interface.

**/
#ifndef _EFI_SPI_H_
#define _EFI_SPI_H_

///
/// Define the SPI protocol GUID
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define EFI_SPI_PROTOCOL_GUID \
  { \
    0xf8b84ae6, 0x8465, 0x4f95, 0x9f, 0xb, 0xea, 0xaa, 0x37, 0xc6, 0x15, 0x5a \
  }
#define EFI_SMM_SPI_PROTOCOL_GUID \
  { \
    0xbd75fe35, 0xfdce, 0x49d7, 0xa9, 0xdd, 0xb2, 0x6f, 0x1f, 0xc6, 0xb4, 0x37 \
  }
#else
#define EFI_SPI_PROTOCOL_GUID \
  { \
    0xf8b84ae6, 0x8465, 0x4f95, \
    { \
      0x9f, 0xb, 0xea, 0xaa, 0x37, 0xc6, 0x15, 0x5a \
    } \
  }
#define EFI_SMM_SPI_PROTOCOL_GUID \
  { \
    0xbd75fe35, 0xfdce, 0x49d7, \
    { \
      0xa9, 0xdd, 0xb2, 0x6f, 0x1f, 0xc6, 0xb4, 0x37 \
    } \
  }
#endif
//
// Extern the GUID for protocol users.
//
extern EFI_GUID                   gEfiSpiProtocolGuid;
extern EFI_GUID                   gEfiSmmSpiProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SPI_PROTOCOL  EFI_SPI_PROTOCOL;

//
// SPI protocol data structures and definitions
//
///
/// Number of Prefix Opcodes allowed on the SPI interface
///
#define SPI_NUM_PREFIX_OPCODE 2

///
/// Number of Opcodes in the Opcode Menu
///
#define SPI_NUM_OPCODE  8

///
/// Opcode Type
///    EnumSpiOpcodeCommand: Command without address
///    EnumSpiOpcodeRead: Read with address
///    EnumSpiOpcodeWrite: Write with address
///
typedef enum {
  EnumSpiOpcodeReadNoAddr,
  EnumSpiOpcodeWriteNoAddr,
  EnumSpiOpcodeRead,
  EnumSpiOpcodeWrite,
  EnumSpiOpcodeMax
} SPI_OPCODE_TYPE;

typedef enum {
  EnumSpiCycle20MHz,
  EnumSpiCycle33MHz,
  EnumSpiCycle66MHz,  ///< not supported by PCH
  EnumSpiCycle50MHz,
  EnumSpiCycleMax
} SPI_CYCLE_FREQUENCY;

typedef enum {
  EnumSpiRegionAll,
  EnumSpiRegionBios,
  EnumSpiRegionMe,
  EnumSpiRegionGbE,
  EnumSpiRegionDescriptor,
  EnumSpiRegionPlatformData,
  EnumSpiRegionMax
} SPI_REGION_TYPE;

///
/// Hardware Sequencing required operations (as listed in PCH EDS "Hardware
/// Sequencing Commands and Opcode Requirements"
///
typedef enum {
  EnumSpiOperationWriteStatus,
  EnumSpiOperationProgramData_1_Byte,
  EnumSpiOperationProgramData_64_Byte,
  EnumSpiOperationReadData,
  EnumSpiOperationWriteDisable,
  EnumSpiOperationReadStatus,
  EnumSpiOperationWriteEnable,
  EnumSpiOperationFastRead,
  EnumSpiOperationEnableWriteStatus,
  EnumSpiOperationErase_256_Byte,
  EnumSpiOperationErase_4K_Byte,
  EnumSpiOperationErase_8K_Byte,
  EnumSpiOperationErase_64K_Byte,
  EnumSpiOperationFullChipErase,
  EnumSpiOperationJedecId,
  EnumSpiOperationDualOutputFastRead,
  EnumSpiOperationDiscoveryParameters,
  EnumSpiOperationOther,
  EnumSpiOperationMax
} SPI_OPERATION;

///
/// SPI Command Configuration
///
typedef struct _SPI_COMMAND_CONFIG {
  ///
  /// The expected frequency to be used (value to be programmed to the SSFC Register)
  ///
  SPI_CYCLE_FREQUENCY Frequency;
  ///
  /// Which Hardware Sequencing required operation this opcode respoinds to.
  /// The required operations are listed in EDS Table 5-55: "Hardware
  /// Sequencing Commands and Opcode Requirements"
  /// If the opcode does not corresponds to any operation listed, use
  /// EnumSpiOperationOther, and provides TYPE and Code for it in
  /// SpecialOpcodeEntry.
  ///
  SPI_OPERATION Operation;
} SPI_COMMAND_CONFIG;

///
/// Special Opcode entries
///
typedef struct _SPI_SPECIAL_OPCODE_ENTRY {
  ///
  /// Opcode Menu Index whose Opcode Type/Menu Configuration Register need to be
  /// overrided or programmed per "Type" and "Code". Filled this field with 0xFF
  /// as the end tag of SpecialOpcodeEntry.
  ///
  UINT8 OpcodeIndex;
  ///
  /// Operation Type (value to be programmed to the OPTYPE register)
  ///
  SPI_OPCODE_TYPE Type; 
  ///
  /// The opcode (value to be programmed to the OPMENU register)
  ///
  UINT8 Code; 
} SPI_SPECIAL_OPCODE_ENTRY;

///
/// Initialization data table loaded to the SPI host controller
///
/// Note:  Most of time, the SPI flash parts with the same vendor would have the same
///        Prefix Opcode, Opcode menu, so you can provide one table for the SPI flash parts with
///        the same vendor.
///
typedef struct _SPI_INIT_DATA {
  ///
  /// Prefix opcodes which are loaded into the SPI host controller
  ///
  UINT8 PrefixOpcode[SPI_NUM_PREFIX_OPCODE];
  ///
  /// Determines Opcode Type, Menu and Frequency of the SPI commands
  ///
  SPI_COMMAND_CONFIG SpiCmdConfig[SPI_NUM_OPCODE];
  ///
  /// Special Opcode entry for the special operations.
  ///
  SPI_SPECIAL_OPCODE_ENTRY  *SpecialOpcodeEntry;
  ///
  /// The offset of the start of the BIOS image relative to the flash device.
  /// Please note this is a Flash Linear Address, NOT a memory space address.
  /// This value is platform specific and depends on the system flash map.
  /// This value is only used on non Descriptor mode.
  ///
  UINTN BiosStartOffset;
  ///
  /// The the BIOS Image size in flash. This value is platform specific
  /// and depends on the system flash map. Please note BIOS Image size may
  /// be smaller than BIOS Region size (in Descriptor Mode) or the flash size
  /// (in Non Descriptor Mode), and in this case, BIOS Image is supposed to be
  /// placed at the top end of the BIOS Region (in Descriptor Mode) or the flash
  /// (in Non Descriptor Mode)
  ///
  UINTN BiosSize;
} SPI_INIT_DATA;

//
// Protocol member functions
//

/**
  JEDEC Read IDs from SPI flash part, this function will return 1-byte Vendor ID and 2-byte Device ID

  @param[in] This                 Pointer to the EFI_SPI_PROTOCOL instance.
  @param[in] Address              This value is to determine the command is sent to SPI Component 1 or 2
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the data received or sent during the SPI cycle.

  @retval EFI_SUCCESS             Read Jedec Id completed.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @exception EFI_UNSUPPORTED      This function is unsupported after SpiProtocolInit is called
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_READ_ID) (
  IN EFI_SPI_PROTOCOL     * This,
  IN     UINTN            Address,
  IN OUT UINT8            * Buffer
  );

/**
  Initializes the host controller to execute SPI commands.

  @param[in] This                 Pointer to the EFI_SPI_PROTOCOL instance.
  @param[in] InitData             Pointer to caller-allocated buffer containing the SPI
                                  interface initialization table.

  @retval EFI_SUCCESS             Opcode initialization on the SPI host controller completed.
  @retval EFI_ACCESS_DENIED       The SPI configuration interface is locked.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource available to initialize the device.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_INIT) (
  IN EFI_SPI_PROTOCOL     * This,
  IN SPI_INIT_DATA        * InitData
  );

/**
  Execute SPI commands from the host controller.

  @param[in] This                 Pointer to the EFI_SPI_PROTOCOL instance.
  @param[in] OpcodeIndex          Index of the command in the OpCode Menu.
  @param[in] PrefixOpcodeIndex    Index of the first command to run when in an atomic cycle sequence.
  @param[in] DataCycle            TRUE if the SPI cycle contains data
  @param[in] Atomic               TRUE if the SPI cycle is atomic and interleave cycles are not allowed.
  @param[in] ShiftOut             If DataByteCount is not zero, TRUE to shift data out and FALSE to shift data in.
  @param[in] Address              In Descriptor Mode, for Descriptor Region, GbE Region, ME Region and Platform
                                  Region, this value specifies the offset from the Region Base; for BIOS Region,
                                  this value specifies the offset from the start of the BIOS Image. In Non
                                  Descriptor Mode, this value specifies the offset from the start of the BIOS Image.
                                  Please note BIOS Image size may be smaller than BIOS Region size (in Descriptor
                                  Mode) or the flash size (in Non Descriptor Mode), and in this case, BIOS Image is
                                  supposed to be placed at the top end of the BIOS Region (in Descriptor Mode) or
                                  the flash (in Non Descriptor Mode)
  @param[in] DataByteCount        Number of bytes in the data portion of the SPI cycle.
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the dada received or sent during the SPI cycle.
  @param[in] SpiRegionType        SPI Region type. Values EnumSpiRegionBios, EnumSpiRegionGbE, EnumSpiRegionMe,
                                  EnumSpiRegionDescriptor, and EnumSpiRegionPlatformData are only applicable in
                                  Descriptor mode. Value EnumSpiRegionAll is applicable to both Descriptor Mode
                                  and Non Descriptor Mode, which indicates "SpiRegionOffset" is actually relative
                                  to base of the 1st flash device (i.e., it is a Flash Linear Address).

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @exception EFI_UNSUPPORTED      Command not supported.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SPI_EXECUTE) (
  IN     EFI_SPI_PROTOCOL   * This,
  IN     UINT8              OpcodeIndex,
  IN     UINT8              PrefixOpcodeIndex,
  IN     BOOLEAN            DataCycle,
  IN     BOOLEAN            Atomic,
  IN     BOOLEAN            ShiftOut,
  IN     UINTN              Address,
  IN     UINT32             DataByteCount,
  IN OUT UINT8              *Buffer,
  IN     SPI_REGION_TYPE    SpiRegionType
  );

///
/// EFI SPI Protocol definition
///
/// These protocols/PPI allows a platform module to perform SPI operations through the
/// Intel PCH SPI Host Controller Interface.
///
struct _EFI_SPI_PROTOCOL {
  EFI_SPI_READ_ID ReadId;   ///< JEDEC Read IDs from SPI flash part, this function will return 1-byte Vendor ID and 2-byte Device ID.
  EFI_SPI_INIT    Init;     ///< Initialize the host controller to execute SPI commands.
  EFI_SPI_EXECUTE Execute;  ///< Execute SPI commands from the host controller.
};

#endif
