/**
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Reference Module" and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement

Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  MrcSsaServices.h

@brief
  This file contains the SSA services PPI.
**/
#ifndef _MrcSsaServices_h_
#define _MrcSsaServices_h_

#include <Library/DebugLib.h>

#pragma pack (push, 1)

#define INT32_MIN                       (0x80000000)
#define INT32_MAX                       (0x7FFFFFFF)
#define INT16_MIN                       (0x8000)
#define INT16_MAX                       (0x7FFF)
#define MAX_CHUNK_SIZE                  (8)
#ifndef MAX_CHANNEL
#define MAX_CHANNEL                     (2)
#endif

typedef enum {
  Success,                                      ///< The function completed successfully.
  NotYetAvailable,                              ///< The function is not yet available.
  NotAvailable,                                 ///< The function is not available or selected DIMM is invalid.
  UnsupportedValue,                             ///< A function parameter is incorrect.
  SocketNotSupported,                           ///< The desired CPU is not supported or not available in the system.
  ControllerNotSupported,                       ///< The desired memory controller is not supported or not available in the CPU.
  ChannelNotSupported,                          ///< The desired memory channel is not supported or not available on the controller.
  LogicalRankNotSupported,                      ///< The desired memory rank is not supported or not available in the channel.
  IoLevelNotSupported,                          ///< The desired I/O level is not supported or not available.
  MarginGroupNotSupported,                      ///< The desired margin group is not supported or not available.
  SsaStatusMax                                  ///< SSA_STATUS structure maximum value.
} SSA_STATUS;

typedef enum {
  RegWidth8,                                    ///< An 8-bit register width is selected.
  RegWidth16,                                   ///< A 16-bit register width is selected.
  RegWidth32,                                   ///< A 32-bit register width is selected.
  RegWidth64,                                   ///< A 64-bit register width is selected.
  RegWidthMax                                   ///< REG_WIDTH structure maximum value.
} REG_WIDTH;

typedef enum {
  RecEnDelay,                                   ///< Receive enable delay margin group.
  RxDqsDelay,                                   ///< Receive DQS delay margin group.
  RxDqBitDelay,                                 ///< Receive DS bit delay margin group.
  WrLvlDelay,                                   ///< Write leveling delay margin group.
  TxDqsDelay,                                   ///< Transmit DQS delay margin group.
  TxDqDelay,                                    ///< Transmit DQ delay margin group.
  TxDqBitDelay,                                 ///< Transmit DQ bit delay margin group.
  RxVref,                                       ///< Receive voltage reference margin group.
  TxVref,                                       ///< Transmit voltage reference margin group.
  CmdAll,                                       ///< All command margin group.
  CmdGrp0,                                      ///< Command 0 margin group.
  CmdGrp1,                                      ///< Command 1 margin group.
  CmdGrp2,                                      ///< Command 2 margin group.
  CtlAll,                                       ///< All control margin group.
  CtlGrp0,                                      ///< Control 1 margin group.
  CtlGrp1,                                      ///< Control 2 margin group.
  CtlGrp2,                                      ///< Control 3 margin group.
  CtlGrp3,                                      ///< Control 4 margin group.
  CtlGrp4,                                      ///< Control 5 margin group.
  CtlGrp5,                                      ///< Control 5 margin group.
  CkAll,                                        ///< All CK margin group.
  CmdCtlAll,                                    ///< All command/control margin group.
  CmdVref,                                      ///< Command voltage reference margin group.
  GsmGtMax                                      ///< GSM_GT structure maximum value.
} GSM_GT;

typedef enum {
  VmseLevel,                                    ///< VMSE I/O level.
  DdrLevel,                                     ///< DDR I/O level.
  LrbufLevel,                                   ///< LRBUF I/O level.
  GsmLtMax                                      ///< GSM_LT structure maximum value.
} GSM_LT;

typedef enum {
  AllLanes,                                     ///< All lanes counter mode.
  ParticularLane,                               ///< A particular lane counter mode.
  ParticularByteGroup,                          ///< A particular byte lane counter mode.
  ParticularChunk,                              ///< A particular chunk counter mode.
  CounterModeMax                                ///< COUNTER_MODE structure maximum value.
} COUNTER_MODE;

typedef enum {
  PatternModeFixed,                             ///< Fixed pattern mode.
  PatternModeLsfr,                              ///< LFSR pattern mode.
  PatternModeMax                                ///< PATTERN_MODE structure maximum value.
} PATTERN_MODE;

typedef enum {
  TrafficModeWrRd,                              ///< Write/Read traffic mode.
  TrafficModeWrite,                             ///< Write traffic mode.
  TrafficModeRead,                              ///< Read traffic mode.
  TrafficModeMax                                ///< TRAFFIC_MODE structure maximum value.
} TRAFFIC_MODE;

typedef enum {
  MCH_BAR,                                      ///< MCHBAR base address selection.
  BaseAddrTypeMax                               ///< BASE_ADDR_TYPE structure maximum value.
} BASE_ADDR_TYPE;

typedef enum {
  SSA_D_WARN  = EFI_D_WARN,                     ///< Warnings
  SSA_D_LOAD  = EFI_D_LOAD,                     ///< Load events
  SSA_D_INFO  = EFI_D_INFO,                     ///< Informational debug messages
  SSA_D_EVENT = EFI_D_EVENT,                    ///< Event messages
  SSA_D_ERROR = EFI_D_ERROR,                    ///< Error
} PRINT_LEVEL;

typedef union {
  UINT64 Data64;                                ///< 64-bit MMIO buffer.
  UINT32 Data32;                                ///< 32-bit MMIO buffer.
  UINT16 Data16;                                ///< 16-bit MMIO buffer.
  UINT8  Data8;                                 ///< 8-bit MMIO buffer.
} MMIO_BUFFER;

typedef union {
  UINT32 Data32;                                ///< 32-bit I/O buffer.
  UINT16 Data16;                                ///< 16-bit I/O buffer.
  UINT8  Data8;                                 ///< 8-bit I/O buffer.
} IO_BUFFER;

typedef union {
  UINT32 Data32;                                ///< 32-bit PCI buffer.
  UINT16 Data16;                                ///< 16-bit PCI buffer.
  UINT8  Data8;                                 ///< 8-bit PCI buffer.
} PCI_BUFFER;

typedef union {
  struct {
    UINT32 Reserved0 : 2;                       ///< PCI address pointer reserved value, range 0 to 3.
    UINT32 Offset    : 6;                       ///< PCI address pointer offset value, range 0 to 63.
    UINT32 Function  : 3;                       ///< PCI address pointer function value, range 0 to 7.
    UINT32 Device    : 5;                       ///< PCI address pointer device value, range 0 to 31.
    UINT32 Bus       : 8;                       ///< PCI address pointer bus value, range 0 to 255.
    UINT32 Reserved1 : 7;                       ///< PCI address pointer reserved value, range 0 to 127.
    UINT32 Enable    : 1;                       ///< PCI address pointer enable flag, 0 = disable, 1 = enable.
  } Bits;
  UINT32 Value;
} PCI_CONFIG_SPACE;

#ifndef _MrcSpdData_h_
typedef union {
  struct {
    UINT16 ContinuationCount  :  7;             ///< Bits 6:0
    UINT16 ContinuationParity :  1;             ///< Bits 7:7
    UINT16 LastNonZeroByte    :  8;             ///< Bits 15:8
  } Bits;
  UINT16 Data;
  UINT8  Data8;
} SPD_MANUFACTURER_ID_CODE;

typedef struct {
  UINT8 Location;                               ///< Module Manufacturing Location
} SPD_MANUFACTURING_LOCATION;

typedef struct {
  UINT8  Year;                                  ///< Year represented in BCD (00h = 2000)
  UINT8  Week;                                  ///< Year represented in BCD (47h = week 47)
} SPD_MANUFACTURING_DATE;

typedef union {
  UINT32 Data;
  UINT16 SerialNumber16[2];
  UINT8  SerialNumber8[4];
} SPD_MANUFACTURER_SERIAL_NUMBER;

typedef struct {
  SPD_MANUFACTURER_ID_CODE       IdCode;        ///< Module Manufacturer ID Code
  SPD_MANUFACTURING_LOCATION     Location;      ///< Module Manufacturing Location
  SPD_MANUFACTURING_DATE         Date;          ///< Module Manufacturing Year, in BCD (range: 2000-2255)
  SPD_MANUFACTURER_SERIAL_NUMBER SerialNumber;  ///< Module Serial Number
} SPD_UNIQUE_MODULE_ID;
#endif //_MrcSpdData_h_

typedef struct {
  BOOLEAN               EccSupport;             ///< TRUE if the DIMM supports ECC, otherwise FALSE.
  UINT32                DimmCapacity;           ///< The DIMM's capacity, in megabytes.
  UINT32                RowSize;                ///< The DIMM's row address size.
  UINT16                ColumnSize;             ///< The DIMM's column address size.
  SPD_UNIQUE_MODULE_ID  SerialNumber;           ///< The DIMM's serial number, retrieved from the SPD.
} MrcDimmInfo;

typedef struct {
  UINT8                 MaxNumberSockets;       ///< The maximum number of CPU sockets in a system.
  UINT8                 MaxNumberControllers;   ///< The maximum number of memory controllers in a CPU socket.
  UINT8                 MaxNumberChannels;      ///< The maximum number of channels in a memory controller.
  UINT8                 MaxNumberLogicalRanks;  ///< The maximum number of ranks in a memory channel.
  UINT32                SocketsBitMask;         ///< The bit mask of available CPU sockets.
} MrcSystemInfo;

typedef union {
  struct {
    UINT16 PatternMode : 1;                     ///< Pattern types. 0 = Fixed, 1 = LSFR.
    UINT16             : 15;                    ///< Reserved.
  } Bits;
  UINT16 Data;
} PATTERN_MODES;

typedef union {
  struct {
    UINT8 TrafficMode : 2;                      ///< Traffic modes. 0 = Write/Read, 1 = Write, 2 = Read, 3 = Reserved.
    UINT8             : 6;                      ///< Reserved.
  } Bits;
  UINT8 Data;
} TRAFFIC_MODES;

typedef union {
  struct {
    UINT32 Count    : 31;                       ///< Error count, range is 0 to 2^31 - 1.
    UINT32 Overflow : 1;                        ///< Error count overflow, 0 = no overflow, 1 = overflow.
  } Bits;
  UINT32 Data;
} ERROR_COUNT_32BITS;

typedef struct {
  UINT16        Ranks[MAX_CHANNEL];             ///< The bit mask of the ranks in a channel to test.
  TRAFFIC_MODES TrafficModes[MAX_CHANNEL];      ///< Traffic modes. 0 = write/read, 1 = write only, 2 = read only, all other values reserved.
  PATTERN_MODES Modes[MAX_CHANNEL];             ///< Pattern types. 0 = Fixed, 1 = LSFR.
  UINT8         WdbIncRates[MAX_CHANNEL];       ///< WDB increment rates.
  UINT8         WdbEnds[MAX_CHANNEL];           ///< WDB pattern stop.
  UINT8         EnCadb[MAX_CHANNEL];            ///< Enable CADB.
  UINT8         LoopCount;                      ///< Sequence repeat count.
  UINT16        BurstLength;                    ///< Burst length.
  UINT8         StopOnErr;                      ///< Stop on error.
} POINT_TEST_PARAMETERS;

typedef 
VOID
(EFIAPI * WRITE_MEM) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  );

typedef 
VOID
(EFIAPI * READ_MEM) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  REG_WIDTH             Width,
  EFI_PHYSICAL_ADDRESS  Address,
  VOID                  *Buffer
  );

typedef 
VOID
(EFIAPI * WRITE_IO) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  );

typedef 
VOID
(EFIAPI * READ_IO) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  REG_WIDTH             Width,
  UINT32                Address,
  VOID                  *Buffer
  );

typedef 
VOID
(EFIAPI * WRITE_PCI) (
  EFI_PEI_SERVICES                **PeiServices,
//  SSA_BIOS_SERVICES_PPI           *This,
  VOID                            *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer,
  BOOLEAN                         CachedData
  );

typedef 
VOID
(EFIAPI * READ_PCI) (
  EFI_PEI_SERVICES                **PeiServices,
//  SSA_BIOS_SERVICES_PPI           *This,
  VOID                            *This,
  REG_WIDTH                       Width,
  EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS *Address,
  VOID                            *Buffer,
  BOOLEAN                         CachedData
  );

typedef 
SSA_STATUS
(EFIAPI * GET_BASE_ADDRESS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Index,
  BASE_ADDR_TYPE        BaseAddressType,
  EFI_PHYSICAL_ADDRESS  *BaseAddress
  );

typedef 
SSA_STATUS
(EFIAPI * GET_MEM_VOLTAGE) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT32                *Voltage
  );

typedef 
SSA_STATUS
(EFIAPI * SET_MEM_VOLTAGE) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT32                *Voltage
  );

typedef 
SSA_STATUS
(EFIAPI * GET_MEM_TEMP) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  INT32                 *Temperature
  );

typedef 
VOID
(EFIAPI * RESTORE_MRS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Address
  );

typedef 
VOID
(EFIAPI * WRITE_MRS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Bank,
  UINT16                Data
  );

typedef 
VOID
(EFIAPI * READ_MRS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 Address,
  UINT16                *Data
  );

typedef 
VOID *
(EFIAPI * MALLOC) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT32                Size
  );

typedef 
VOID
(EFIAPI * FREE) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  VOID                  *Buffer
  );

typedef
VOID
(EFIAPI * DEBUG_PRINT) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  PRINT_LEVEL           PrintLevel,
  UINT8                 *FormatString,
  ...
  );

typedef 
UINT8
(EFIAPI * GET_DIMM_FROM_LOGICAL_RANK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Rank
  );

typedef 
SSA_STATUS
(EFIAPI * GET_DIMM_INFO) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  MrcDimmInfo           *DimmInfoBuffer
  );

typedef 
UINT8
(EFIAPI * GET_RANK_IN_DIMM) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm
  );

typedef 
UINT8
(EFIAPI * GET_LOGICAL_RANK_BIT_MASK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  );

typedef 
UINT8
(EFIAPI * GET_CHANNEL_BIT_MASK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller
  );

typedef 
SSA_STATUS
(EFIAPI * GET_SYSTEM_INFO) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  MrcSystemInfo         *SystemInfoBuffer
  );


typedef 
UINT8
(EFIAPI * GET_CONTROLLER_BIT_MASK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket
  );

typedef 
VOID
(EFIAPI * JEDEC_RESET) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller
  );

typedef 
SSA_STATUS
(EFIAPI * GET_MARGIN_PARAM_LIMITS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 LogicRank,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  INT16                 *MinOffset,
  INT16                 *MaxOffset,
  UINT16                *Delay
  );

typedef 
VOID
(EFIAPI * OFFSET_MARGIN_PARAM) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 LogicRank,
  GSM_LT                IoLevel,
  GSM_GT                MarginGroup,
  UINT16                Offset
  );

typedef 
VOID
(EFIAPI * SET_WDB_PATTERN) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                *Pattern,
  UINT8                 CachelineCount,
  UINT8                 StartCachelineIndex
  );

typedef 
VOID
(EFIAPI * SET_CADB_PATTERN) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                *Pattern,
  UINT8                 CachelineCount,
  UINT8                 StartCachelineIndex
  );

typedef 
VOID
(EFIAPI * CLEAR_ERROR_STATUS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelMask
  );

typedef 
SSA_STATUS
(EFIAPI * CLEAR_ERROR_COUNTER) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter
  );

typedef 
UINT64
(EFIAPI * GET_DQ_ERROR_STATUS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  );

typedef 
UINT8
(EFIAPI * GET_ECC_ERROR_STATUS) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel
  );

typedef 
SSA_STATUS
(EFIAPI * SET_ERROR_COUNTER_MODE) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter,
  COUNTER_MODE          CounterMode,
  UINT32                ModeIndex
  );

typedef 
ERROR_COUNT_32BITS
(EFIAPI * GET_ERROR_COUNT) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter
  );

typedef 
VOID
(EFIAPI * SET_VALIDATION_BIT_MASK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT64                DqMask,
  UINT8                 EccMask
  );

typedef 
VOID
(EFIAPI * SET_VALIDATION_PHASE_MASK) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 CachelineMask,
  UINT8                 PhaseMask
  );

typedef 
VOID
(EFIAPI * RUN_POINT_TEST) (
  EFI_PEI_SERVICES      **PeiServices,
//  SSA_BIOS_SERVICES_PPI *This,
  VOID                  *This,
  UINT8                 Socket,
  UINT8                 Controller,
  VOID                  *TestParameters,
  BOOLEAN               SkipSetup
  );

//
// SSA BIOS Common Configuration
//
typedef struct _SSA_COMMON_CONFIG {
  UINT32                        Revision;               ///< Incremented when a backwards compatible binary change is made to the PPI.
  VOID                          *BiosData;              ///< Pointer to the BIOS data buffer.
  READ_MEM                      ReadMem;                ///< Reads a variable-sized value from a memory mapped register using an absolute address. This function takes advantage of any caching implemented by BIOS.
  WRITE_MEM                     WriteMem;               ///< Writes a variable-sized value to a memory mapped register using an absolute address. This function takes advantage of any caching implemented by BIOS.
  READ_IO                       ReadIo;                 ///< Reads a variable-sized value from IO. This function takes advantage of any caching implemented by BIOS.
  WRITE_IO                      WriteIo;                ///< Writes a variable-sized value to IO.
  READ_PCI                      ReadPci;                ///< Reads a variable-sized value from a PCI config space register. This function takes advantage of any caching implemented by BIOS.
  WRITE_PCI                     WritePci;               ///< Writes a variable-sized value to a PCI config space register. This function takes advantage of any caching implemented by BIOS.
  GET_BASE_ADDRESS              GetBaseAddress;         ///< Gets a base address to be used in the different memory map or IO register access functions.
  MALLOC                        Malloc;                 ///< Function used to dynamically allocate memory.
  FREE                          Free;                   ///< Function used to release memory allocated using Malloc.
  DEBUG_PRINT                   DebugPrint;             ///< Function used to output debug messages to the output logging device.
} SSA_COMMON_CONFIG;

//
// SSA BIOS Memory Configuration
//
typedef struct _SSA_MEMORY_CONFIG {
  UINT32                        Revision;               ///< Incremented when a backwards compatible binary change is made to the PPI.
  VOID                          *MrcData;               ///< Pointer to the BIOS data buffer.
  GET_SYSTEM_INFO               GetSystemInfo;          ///< Returns system information.
  GET_MEM_VOLTAGE               GetMemVoltage;          ///< Returns the platform's memory voltage.
  SET_MEM_VOLTAGE               SetMemVoltage;          ///< Sets the platform's memory voltage.
  GET_MEM_TEMP                  GetMemTemp;             ///< Returns the DIMM's temperature.
  RESTORE_MRS                   RestoreMrs;             ///< Restore BIOS default DRAM mode register value.
  WRITE_MRS                     WriteMrs;               ///< Writes DRAM mode register.
  READ_MRS                      ReadMrs;                ///< Reads DRAM mode register.
  GET_DIMM_FROM_LOGICAL_RANK    GetDimmFromLogicalRank; ///< Return the DIMM number according to the logical rank number.
  GET_DIMM_INFO                 GetDimmInfo;            ///< Returns DIMM information.
  GET_RANK_IN_DIMM        	    GetRankInDimm;          ///< Return the number of ranks in a specific DIMM on a given controller.
  GET_LOGICAL_RANK_BIT_MASK     GetLogicalRankBitMask;  ///< Return the logical rank bit mask of the channel.
  GET_CHANNEL_BIT_MASK          GetChannelBitMask;      ///< Return the channel bit mask of the populated channels.
  GET_CONTROLLER_BIT_MASK       GetControllerBitMask;   ///< Returns bitmask of available controllers on a given socket.
  JEDEC_RESET                   JedecReset;             ///< Function used to reset a DIMM.
  GET_MARGIN_PARAM_LIMITS       GetMarginParamLimits;   ///< Function returns the low side, high side range and required delay of a margin parameter.
  OFFSET_MARGIN_PARAM           OffsetMarginParam;      ///< Function used to adjust a margin parameter.
  SET_WDB_PATTERN               SetWdbPattern;          ///< Function used to set up WDB pattern.
  SET_CADB_PATTERN              SetCadbPattern;         ///< Function used to set up CADB pattern.
  CLEAR_ERROR_STATUS            ClearErrorStatus;       ///< Function used to clear the lane error status registers.
  CLEAR_ERROR_COUNTER           ClearErrorCounter;      ///< Function used to clear the error counter register.
  GET_DQ_ERROR_STATUS           GetDqErrorStatus;       ///< Function used to get the DQ lane error status.
  GET_ECC_ERROR_STATUS          GetEccErrorStatus;      ///< Function used to get the ECC lane error status.
  SET_ERROR_COUNTER_MODE        SetErrorCounterMode;    ///< Function used to clear the error counter register.
  GET_ERROR_COUNT               GetErrorCount;          ///< Function used to get the error count value for a give channel and counter on a given controller.
  SET_VALIDATION_BIT_MASK       SetValidationBitMask;   ///< Function used to set the lane validation mask for a give channel on a given controller. Only the lanes with the mask bit set will be checked for errors.
  SET_VALIDATION_PHASE_MASK     SetValidationPhaseMask; ///< Function used to set the phase mask for a give channel on a given controller. Only the phases with the mask bit set will be checked for errors.
  RUN_POINT_TEST                RunPointTest;           ///< Function used to run a point test.
} SSA_MEMORY_CONFIG;

///
/// SSA BIOS Services Header
///
typedef struct _SSA_BIOS_HEADER {
  UINT32                        Revision;               ///< Incremented when a backwards compatible binary change is made to the PPI.
  SSA_COMMON_CONFIG             *SsaCommonConfig;       ///< Pointer to the SSA BIOS common functions.
  SSA_MEMORY_CONFIG             *SsaMemoryConfig;       ///< Pointer to the SSA BIOS memory related functions.
} SSA_BIOS_HEADER;

///
/// SSA BIOS Services PPI
///
typedef struct _SSA_BIOS_SERVICES_PPI {
  SSA_BIOS_HEADER               SsaHeader;              ///< SSA BIOS Services Header.
  SSA_COMMON_CONFIG             SsaCommonConfig;        ///< SSA BIOS common functions.
  SSA_MEMORY_CONFIG             SsaMemoryConfig;        ///< SSA BIOS memory related functions.
} SSA_BIOS_SERVICES_PPI;

#pragma pack (pop)
#endif // _MrcSsaServices_h_
