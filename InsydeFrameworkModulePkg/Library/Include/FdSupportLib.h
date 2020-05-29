//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _FD_SUPPORT_LIB_H_
#define _FD_SUPPORT_LIB_H_

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)

#define FVB_MAX_RETRY_TIMES             10000000
#define STALL_TIME                      0x02

#define ACCESS_NOT_AVAILABLE            0
#define ACCESS_AVAILABLE                1
#define BIOS_REGION_READ_ACCESS         0
#define BIOS_REGION_WRITE_ACCESS        8
#define BIOS_MASTER_READ_ACCESS_GRANT   16
#define BIOS_MASTER_WRITE_ACCESS_GRANT  24

typedef enum {
  DESC_REGION              = 0,
  BIOS_REGION,
  ME_REGION,
  GBE_REGION,
  PLATFORM_DATA_REGION,
  MAX_FLASH_REGION,
  ME_SUB_REGION_IGNITION   = 0x20,
  REGION_TYPE_OF_EOS  = 0xff,
} FLASH_REGION_TYPE;

#define NUMBER_OF_SUB_REGION    1
#define REGION_EOS              1
#define TOTAL_FLASH_REGION      ( MAX_FLASH_REGION + NUMBER_OF_SUB_REGION + REGION_EOS )

#pragma pack(1)
typedef struct {
  UINT8        Type;
  UINT32       Offset;
  UINT32       Size;
  UINT8        Access;
} FLASH_REGION;
#pragma pack()

#define FLASH_SIZE_128K     0x01
#define FLASH_SIZE_256K     0x02
#define FLASH_SIZE_512K     0x03
#define FLASH_SIZE_1024K    0x04
#define FLASH_SIZE_2048K    0x05
#define FLASH_SIZE_4096K    0x06
#define FLASH_SIZE_8192K    0x07
#define FLASH_SIZE_16384K   0x08
#define MAX_STRING          0x20

#define FD_INDEX_INVALID    0xff
#define FD_ID_INVALID       0
#define COMMON_SPI_DEVICE_ID 0x12345678

#pragma pack(1)
typedef struct {
  UINT16      BlockSize;   //Unit: 1 = 256 bytes
  UINT16      Mutiple;     //flash how many times
  UINT16      EOS;         //end of struct, this word must be 0xffff
} FD_BLOCK_MAP;

typedef struct  {
  UINT64              Id;
  UINT8               Size;
  CHAR8               VendorName[MAX_STRING];
  CHAR8               DeviceName[MAX_STRING];
  FD_BLOCK_MAP        BlockMap;
} FD_INFO;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_INIT) (
  EFI_SMM_SYSTEM_TABLE          *mSmst
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_RECOGNIZE) (
  IN UINT64                     DeviceId
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_PROGRAM) (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_SPIDEVICE_SUPPORT_PROGRAM) (
  IN  UINT8                     *FlashAddress,
  IN  UINT8                     *SrcAddress,
  IN  UINTN                     *BufferSize,
  IN  UINTN                     LbaWriteAddress
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_SPIDEVICE_SUPPORT_RECOGNIZE) (
  IN VOID                       *FlashDevice
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_READ) (
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      Count
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_ERASE) (
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  );

typedef
EFI_STATUS
(EFIAPI *FLASH_DEVICE_SUPPORT_CONVERT_POINTER) (
  VOID
  );

typedef struct  {
   FD_INFO                               DeviceInfo;
   FLASH_DEVICE_SUPPORT_INIT             Init;
   FLASH_DEVICE_SUPPORT_RECOGNIZE        Recognize;
   FLASH_DEVICE_SUPPORT_ERASE            Erase;
   FLASH_DEVICE_SUPPORT_PROGRAM          Program;
   FLASH_DEVICE_SUPPORT_READ             Read;
   FLASH_DEVICE_SUPPORT_CONVERT_POINTER  ConvertPointer;
} FLASH_DEVICE;

EFI_STATUS
GetFdSupportNumber (
  OUT UINTN                              *FdSupportNumber
  );

EFI_STATUS
AutoDetectDevice (
  FLASH_DEVICE                           **FlashOperationPtr,
  EFI_SMM_SYSTEM_TABLE                   *mSmst
  );

EFI_STATUS
FdSupportConvertPointer (
  OUT FLASH_DEVICE                       **FlashOperationPtr
  );

EFI_STATUS
GetPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  );
  
EFI_STATUS
GetPlatformSpiFlashNumber (
  OUT   UINT8       *FlashNumber
  );

EFI_STATUS
ConvertToSpiAddress (
  IN UINTN          Source,
  OUT UINTN         *Destination
  );


//
// Flash config block definition for SPI
//
typedef struct
{
  UINT8   OpCodeMenu[8];
  UINT16  OpType;
  UINT8   PrefixMenu[2];
  UINT32  GlobalProtect : 1;
  UINT32  BlockProtect : 1;
  UINT32  BlockProtectDataRequired : 1;
  UINT32  ProgramGranularity : 1;
  UINT32  FlashIDSize : 3;
  UINT32  MinBytesPerOp : 3;
  UINT32  NVStatusBit : 1;
  UINT32  Reserve : 21;
  UINT8   GlobalProtectCode;
  UINT8   GlobalUnprotectCode;
  UINT8   BlockProtectCode;
  UINT8   BlockUnprotectCode;
  UINT32  DeviceSize;
  UINT32  BlockEraseSize;
  UINT32  BlockProtectSize;
} SPI_CONFIG_BLOCK;

//
// SFDP Table
//
#define SFDP_SIGNATURE                  0x50444653
#define SPI_READ_SFDP_OPCODE            0x5A
#define SPI_READ_SFDP_DUMMY_CYCLE_NUM   1

#pragma pack(1)
typedef struct
{
  UINT32  SfdpSignature;
  UINT8   SfdpMinorRevNum;
  UINT8   SfdpMajorRevNum;
  UINT8   ParameterHeaderNum;       // n: n+1 parameter header
  UINT8   Reserve0;
} SPI_SFDP_HEADER;

typedef struct
{
  UINT8  ID;
  UINT8  ParameterTableMinorRev;
  UINT8  ParameterTableMajorRev;
  UINT8  ParameterTableLen;         //in double word
  UINT8  ParameterTablePointer[3];
  UINT8  Reserve0;
} SPI_SFDP_PARAMETER_HEADER;

typedef struct
{
  //
  // 1th dword
  //
  UINT8  BlockEraseSizes : 2;
  UINT8  WriteGranularity : 1;
  UINT8  VolatitleStatusBit : 1;
  UINT8  WriteEnableOpcodeSelect : 1;
  UINT8  Reserve0 : 3;
  UINT8  EraseOpcode;
  UINT8  FastRead112Support : 1;   //112(1-1-2) (x-y-z): x: x pins used for the opcode
  UINT8  AddressBytes : 2;         //                    y: y pins used for the address
  UINT8  DTRSupport : 1;           //                    z: z pins used for the data
  UINT8  FastRead122Support : 1;
  UINT8  FastRead144Support : 1;
  UINT8  FastRead114Support : 1;
  UINT8  Reserve1 : 1;
  UINT8  Reserve2;
  //
  // 2nd dword
  //
  UINT32 FlashMemoryDensity;
  //
  // 3rd dword
  //
  UINT8  FastRead144DummyCycle : 5;
  UINT8  FastRead144ModeBit : 3;
  UINT8  FastRead144OpCode;
  UINT8  FastRead114DummyCycle : 5;
  UINT8  FastRead114ModeBit : 3;
  UINT8  FastRead114OpCode;
  //
  // 4th dword
  //
  UINT8  FastRead112DummyCycle : 5;
  UINT8  FastRead112ModeBit : 3;
  UINT8  FastRead112OpCode;
  UINT8  FastRead122DummyCycle : 5;
  UINT8  FastRead122ModeBit : 3;
  UINT8  FastRead122OpCode;
  //
  // 5th dword
  //
  UINT8  FastRead222Support : 1;
  UINT8  Reserve3 : 3;
  UINT8  FastRead444Support : 1;
  UINT8  Reserve4 : 3;
  UINT8  Reserve5[3];
  //
  // 6th dword
  //
  UINT8  Reserve6[2];
  UINT8  FastRead222DummyCycle : 5;
  UINT8  FastRead222ModeBit : 3;
  UINT8  FastRead222OpCode;
  //
  // 7th dword
  //
  UINT8  Reserve7[2];
  UINT8  FastRead444DummyCycle : 5;
  UINT8  FastRead444ModeBit : 3;
  UINT8  FastRead444OpCode;
  //
  // 8th dword
  //
  UINT8  SectorType1Size;
  UINT8  SectorType1EraseOpcode;
  UINT8  SectorType2Size;
  UINT8  SectorType2EraseOpcode;
  //
  // 9th dword
  //
  UINT8  SectorType3Size;
  UINT8  SectorType3EraseOpcode;
  UINT8  SectorType4Size;
  UINT8  SectorType4EraseOpcode;
} SPI_SFDP_JEDEC_PARAMETER;

typedef struct
{
  SPI_SFDP_HEADER                  Header;
  SPI_SFDP_PARAMETER_HEADER        JedecParameterHeader;
  SPI_SFDP_JEDEC_PARAMETER         JedecParameter;
} SPI_SFDP_TABLE;
#pragma pack()

typedef struct  {
   FD_INFO                               DeviceInfo;
   FLASH_DEVICE_SUPPORT_INIT             Init;
   FLASH_SPIDEVICE_SUPPORT_RECOGNIZE     Recognize;
   FLASH_DEVICE_SUPPORT_ERASE            Erase;
   FLASH_SPIDEVICE_SUPPORT_PROGRAM       Program;
   FLASH_DEVICE_SUPPORT_READ             Read;
   FLASH_DEVICE_SUPPORT_CONVERT_POINTER  ConvertPointer;
   SPI_CONFIG_BLOCK                      *SpiConfigBlock;
} FLASH_SPI_DEVICE;

extern FLASH_DEVICE * FdSupportTable [];
extern BOOLEAN gFwhLpcDev;
extern UINTN   gCurrentFdIndex;
extern UINT64  gCurrentFdId;

#endif

