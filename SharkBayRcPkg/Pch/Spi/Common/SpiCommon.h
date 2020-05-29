/** @file
  Header file for the PCH SPI Common Driver.

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
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SpiCommon.h

@brief
  Header file for the PCH SPI Common Driver.

**/
#ifndef _SPI_COMMON_H_
#define _SPI_COMMON_H_

#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
//
// Maximum time allowed while waiting the SPI cycle to complete
//  Wait Time = 6 seconds = 6000000 microseconds
//  Wait Period = 10 microseconds
//
#define WAIT_TIME   6000000     ///< Wait Time = 6 seconds = 6000000 microseconds
#define WAIT_PERIOD 10          ///< Wait Period = 10 microseconds
//
// PCH Required SPI Commands -------- COMMAND SET I ------------
// SPI flash device must support in order to be compatible with PCH
//
#define PCH_SPI_COMMAND_WRITE_STATUS          0x01
#define PCH_SPI_COMMAND_PROGRAM_BYTE          0x02
#define PCH_SPI_COMMAND_READ_DATA             0x03
#define PCH_SPI_COMMAND_WRITE_DISABLE         0x04
#define PCH_SPI_COMMAND_READ_STATUS           0x05
#define PCH_SPI_COMMAND_WRITE_ENABLE          0x06
#define PCH_SPI_COMMAND_FAST_READ             0x0B
#define PCH_SPI_COMMAND_DUAL_FAST_READ        0x3B  ///< Dual Output Fast Read
#define PCH_SPI_COMMAND_ENABLE_WRITE_STATUS   0x50  ///< Enable Write Status Register
#define PCH_SPI_COMMAND_DISCOVERY_PARAMETERS  0x5A  ///< Serial Flash Discovery Parameters
#define PCH_SPI_COMMAND_READ_ID               0x9F  ///< JEDEC Read ID
#define PCH_SPI_COMMAND_FULL_CHIP_ERASE       0xC7  ///< Full Chip Erase
//
// Need to support at least one of the following three kinds of size of sector for erasing
//
#define PCH_SPI_COMMAND_4KB_ERASE   0x20
#define PCH_SPI_COMMAND_64KB_ERASE  0xD8
#define PCH_SPI_COMMAND_256B_ERASE  0xDB
//
// ICH8M Recommended SPI Commands -------- COMMAND SET II ------------
// SPI flash device best to support
//
#define PCH_SPI_COMMAND_WRITE_STATUS    0x01
#define PCH_SPI_COMMAND_WRITE_STATUS_EN 0x50
#define PCH_SPI_COMMAND_FULL_CHIP_ERASE 0xC7

#define SIZE_OF_SPI_VTBA_ENTRY          (S_PCH_SPI_VTBA_JID0 + S_PCH_SPI_VTBA_VSCC0)

///
/// Private data structure definitions for the driver
///
#define PCH_SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('P', 'S', 'P', 'I')

///
/// Initialization data table loaded to the SPI host controller
///    PrefixOpcode    Prefix opcodes which are loaded into the SPI host controller
///    SpiCmdConfig    Determines Opcode Type, Menu and Frequency of the SPI commands
///    BiosStartOffset The offset of the start of the BIOS image relative to the flash device.
///                    Please note this is a Flash Linear Address, NOT a memory space address.
///                    This value is platform specific and depends on the system flash map.
///                    This value is only used on non Descriptor mode.
///    BiosSize        The the BIOS Image size in flash. This value is platform specific
///                    and depends on the system flash map. Please note BIOS Image size may
///                    be smaller than BIOS Region size (in Descriptor Mode) or the flash size
///                    (in Non Descriptor Mode), and in this case, BIOS Image is supposed to be
///                    placed at the top end of the BIOS Region (in Descriptor Mode) or the flash
///                    (in Non Descriptor Mode)
///
typedef struct _SPI_INIT_TABLE {
  UINT8               PrefixOpcode[SPI_NUM_PREFIX_OPCODE];
  SPI_COMMAND_CONFIG  SpiCmdConfig[SPI_NUM_OPCODE];
  UINTN               BiosStartOffset;
  UINTN               BiosSize;
} SPI_INIT_TABLE;

typedef struct _SPI_ID_TABLE {
  UINT8       VendorId;
  UINT8       DeviceId0;
  UINT8       DeviceId1;
} SPI_ID_TABLE;

typedef struct _SPI_DESCRIPTOR_TABLE {
  UINT8       NumberComponents    :2;
  UINT8       Comp1Density        :4;
  UINT8       Rsvd                :2;
} SPI_DESCRIPTOR_TABLE;

typedef struct {
  UINTN                 Signature;
  EFI_HANDLE            Handle;
  EFI_SPI_PROTOCOL      SpiProtocol;
  SPI_INIT_TABLE        SpiInitTable;
  SPI_ID_TABLE          SpiIdTable[2];
  UINTN                 PchRootComplexBar;
  SPI_DESCRIPTOR_TABLE  SpiDescriptor;
  BOOLEAN               DescriptorMode;
} SPI_INSTANCE;

#define SPI_INSTANCE_FROM_SPIPROTOCOL(a)  CR (a, SPI_INSTANCE, SpiProtocol, PCH_SPI_PRIVATE_DATA_SIGNATURE)

//
// Function prototypes used by the SPI protocol.
//

/**
  Initialize an SPI protocol instance.
  The function will assert in debug if PCH RCBA has not been initialized

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @exception EFI_UNSUPPORTED      The PCH is not supported by this module
**/
EFI_STATUS
SpiProtocolConstructor (
  SPI_INSTANCE          *SpiInstance
  );

/**
  JEDEC Read IDs from SPI flash part, this function will return 1-byte Vendor ID and 2-byte Device ID

  @param[in] This                 Pointer to the EFI_SPI_PROTOCOL instance.
  @param[in] Address              This value is for determines the command is sent to SPI Component 1 or 2
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the dada received or sent during the SPI cycle.

  @retval EFI_SUCCESS             Read Jedec Id completed.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @exception EFI_UNSUPPORTED      This function is unsupport after SpiProtocolInit is called
**/
EFI_STATUS
EFIAPI
SpiProtocolReadId (
  IN EFI_SPI_PROTOCOL       *This,
  IN     UINTN              Address,
  IN OUT UINT8              *Buffer
  );

/**
  Initialize the host controller to execute SPI command.

  @param[in] This                 Pointer to the EFI_SPI_PROTOCOL instance.
  @param[in] InitData             Initialization data to be programmed into the SPI host controller.

  @retval EFI_SUCCESS             Initialization completed.
  @retval EFI_ACCESS_DENIED       The SPI static configuration interface has been locked-down.
  @retval EFI_INVALID_PARAMETER   Bad input parameters.  
  @exception EFI_UNSUPPORTED      Can't get Descriptor mode VSCC values
**/
EFI_STATUS
EFIAPI
SpiProtocolInit (
  IN EFI_SPI_PROTOCOL       *This,
  IN SPI_INIT_DATA          *InitData
  );

/**
  Execute SPI commands from the host controller.
  This function would be called by runtime driver, please do not use any MMIO marco here

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
  @param[in] DataByteCount        Number of bytes in the data portion of the SPI cycle. This function may break the 
                                  data transfer into multiple operations. This function ensures each operation does 
                                  not cross 256 byte flash address boundary. 
                                  *NOTE: if there is some SPI chip that has a stricter address boundary requirement
                                  (e.g., its write page size is < 256 byte), then the caller cannot rely on this
                                  function to cut the data transfer at proper address boundaries, and it's the 
                                  caller's reponsibility to pass in a properly cut DataByteCount parameter.
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the dada received or sent during the
                                  SPI cycle.
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
EFI_STATUS
EFIAPI
SpiProtocolExecute (
  IN     EFI_SPI_PROTOCOL   *This,
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

/**
  This function sends the programmed SPI command to the slave device.

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
  @param[in] DataByteCount        Number of bytes in the data portion of the SPI cycle. This function may break the 
                                  data transfer into multiple operations. This function ensures each operation does 
                                  not cross 256 byte flash address boundary. 
                                  *NOTE: if there is some SPI chip that has a stricter address boundary requirement
                                  (e.g., its write page size is < 256 byte), then the caller cannot rely on this
                                  function to cut the data transfer at proper address boundaries, and it's the 
                                  caller's reponsibility to pass in a properly cut DataByteCount parameter.
  @param[in, out] Buffer          Data received or sent during the SPI cycle.
  @param[in] SpiRegionType        SPI Region type. Values EnumSpiRegionBios, EnumSpiRegionGbE, EnumSpiRegionMe, 
                                  EnumSpiRegionDescriptor, and EnumSpiRegionPlatformData are only applicable in 
                                  Descriptor mode. Value EnumSpiRegionAll is applicable to both Descriptor Mode
                                  and Non Descriptor Mode, which indicates "SpiRegionOffset" is actually relative
                                  to base of the 1st flash device (i.e., it is a Flash Linear Address).

  @retval EFI_SUCCESS             SPI command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
  @retval EFI_ACCESS_DENIED       Some unrecognized command encountered in hardware sequencing mode
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
**/
EFI_STATUS
SendSpiCmd (
  IN     EFI_SPI_PROTOCOL   *This,
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

/**
  Wait execution cycle to complete on the SPI interface. Check both Hardware
  and Software Sequencing status registers

  @param[in] This                 The SPI protocol instance
  @param[in] UseSoftwareSequence  TRUE if this is a Hardware Sequencing operation
  @param[in] ErrorCheck           TRUE if the SpiCycle needs to do the error check

  @retval TRUE                    SPI cycle completed on the interface. 
  @retval FALSE                   Time out while waiting the SPI cycle to complete. 
                                  It's not safe to program the next command on the SPI interface.
**/
BOOLEAN
WaitForSpiCycleComplete (
  IN     EFI_SPI_PROTOCOL   *This,
  IN     BOOLEAN            UseSoftwareSequence,
  IN     BOOLEAN            ErrorCheck
  );

#endif
