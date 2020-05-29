/** @file
  Define the structure for communication between chipset layer and common
  platform layer.

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

#ifndef _COMMON_SMBIOS_MEMORY_H_
#define _COMMON_SMBIOS_MEMORY_H_

#include <IndustryStandard/SmBios.h>

//typedef UINT16                    STRING_REF; 

#ifndef MAX_SOCKETS
#define MAX_SOCKETS 4 
#endif

//
// Maximum number of sides supported per DIMM
//
#ifndef MAX_SIDES
#define MAX_SIDES  2
#endif

//
// Maximum number of rows supported by the memory controller
//
#ifndef MAX_ROWS
#define MAX_ROWS  (MAX_SIDES * MAX_SOCKETS)
#endif

///
/// Maximum number of DIMM sockets supported by each channel
///
#ifndef MAX_SLOTS
#define MAX_SLOTS 2
#endif

//
// MAX SPD data size
//
#define MAX_SPD_DATA_SIZE    0x100 

//
// Define saving SPD data structure
//
typedef struct {
  UINT32      ModuleSN;
  UINT8       SmbusBufferData[MAX_SPD_DATA_SIZE];
} SPD_DATA;

typedef struct {
	SPD_DATA    DimmData[MAX_SOCKETS];
} SPD_SAVE_DATA;

//
// Row configuration data structure
//
typedef struct {
  EFI_PHYSICAL_ADDRESS  BaseAddress; // Base address of Row 
  UINT64                RowLength;   // Size of Row in bytes
} DDR_ROW_CONFIG;

// 
// This structure is used to set some SMBIOS Type 17 information without SPD on platform such as On-board memory.
// If SpdExist = FALSE, the other four member must fill-in the information according to On-board memory.
//
//typedef struct {
//  BOOLEAN                     SpdExist;         ///< Memory information can get from SPD.(Default is TRUE)
//  UINT8                       MemoryType;       ///< The enumeration value from MEMORY_DEVICE_TYPE.       // The type of memory used in this device.(Offest 0x12)
//  MEMORY_DEVICE_TYPE_DETAIL   TypeDetail;       ///< Additional detail on the memory device type.(Offest 0x13)
//  UINT16                      TotalWidth;       ///< The total width, in bits, of this memory device.(Offest 0x08)
//  UINT16                      DataWidth;        ///< The data width, in bits, of this memory device.(Offest 0x0A)
//} H2O_MEMORY_ON_BOARD_INFO;

typedef struct _H2O_SMBIOS_MEM_CONFIG_DATA {
  SMBIOS_TABLE_TYPE5                      MemoryControllerInfo;    // SmBios Type 5
  SMBIOS_TABLE_TYPE16                     ArrayLocationData;       // SmBios Type 16
  SMBIOS_TABLE_TYPE17                     MemoryDeviceData;        // SmBios Type 17
  SMBIOS_TABLE_TYPE18                     Memory32bitErrorInfo;    // SmBios Type 18
  BOOLEAN                                 Interleaved;             // Dual channel mode or Single channel mode
  DDR_ROW_CONFIG                          RowConfArray[MAX_ROWS];  // Row Configuration for all dimm
//  H2O_MEMORY_ON_BOARD_INFO                MemoryOnBoardInfo;       ///< MemoryOnBoardInfo is optional to fill-in.
} H2O_SMBIOS_MEM_CONFIG_DATA;

#endif
