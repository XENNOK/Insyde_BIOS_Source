//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//;
//;
//;
//;------------------------------------------------------------------------------
//;
//; Abstract:
//;

#ifndef _NORMAL_BOOT_SCRIPT_H_
#define _NORMAL_BOOT_SCRIPT_H_

#include "Tiano.h"
#include "Pei.h"
#include "EfiBootScript.h"
#include EFI_PPI_DEFINITION (BootScriptExecuter)

#define IO_WRITE_BOOT_SCRIPT_STRUCT(Size)       \
typedef struct _IO_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT32                Count;                \
  UINT64                Address;              \
  UINT##Size            Data##Size;           \
} IO_WRITE##Size##_BOOT_SCRIPT


IO_WRITE_BOOT_SCRIPT_STRUCT(8);
IO_WRITE_BOOT_SCRIPT_STRUCT(16);
IO_WRITE_BOOT_SCRIPT_STRUCT(32);
IO_WRITE_BOOT_SCRIPT_STRUCT(64);

#define IO_READ_WRITE_BOOT_SCRIPT_STRUCT(Size)       \
typedef struct _IO_READ_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT64                Address;              \
  UINT##Size            OrData##Size;         \
  UINT##Size            AndData##Size;        \
} IO_READ_WRITE##Size##_BOOT_SCRIPT


IO_READ_WRITE_BOOT_SCRIPT_STRUCT(8);
IO_READ_WRITE_BOOT_SCRIPT_STRUCT(16);
IO_READ_WRITE_BOOT_SCRIPT_STRUCT(32);
IO_READ_WRITE_BOOT_SCRIPT_STRUCT(64);

#define MEM_WRITE_BOOT_SCRIPT_STRUCT(Size)       \
typedef struct _MEM_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT32                Count;                \
  UINT64                Address;              \
  UINT##Size            Data##Size;           \
} MEM_WRITE##Size##_BOOT_SCRIPT


MEM_WRITE_BOOT_SCRIPT_STRUCT(8);
MEM_WRITE_BOOT_SCRIPT_STRUCT(16);
MEM_WRITE_BOOT_SCRIPT_STRUCT(32);
MEM_WRITE_BOOT_SCRIPT_STRUCT(64);

#define MEM_READ_WRITE_BOOT_SCRIPT_STRUCT(Size)       \
typedef struct _MEM_READ_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT64                Address;              \
  UINT##Size            OrData##Size;         \
  UINT##Size            AndData##Size;        \
} MEM_READ_WRITE##Size##_BOOT_SCRIPT


MEM_READ_WRITE_BOOT_SCRIPT_STRUCT(8);
MEM_READ_WRITE_BOOT_SCRIPT_STRUCT(16);
MEM_READ_WRITE_BOOT_SCRIPT_STRUCT(32);
MEM_READ_WRITE_BOOT_SCRIPT_STRUCT(64);

#define PCICFG_WRITE_BOOT_SCRIPT_STRUCT(Size) \
typedef struct _PCICFG_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT32                Count;                \
  UINT64                Address;              \
  UINT##Size            Data##Size;           \
} PCICFG_WRITE##Size##_BOOT_SCRIPT


PCICFG_WRITE_BOOT_SCRIPT_STRUCT(8);
PCICFG_WRITE_BOOT_SCRIPT_STRUCT(16);
PCICFG_WRITE_BOOT_SCRIPT_STRUCT(32);
PCICFG_WRITE_BOOT_SCRIPT_STRUCT(64);

#define PCICFG_READ_WRITE_BOOT_SCRIPT_STRUCT(Size)       \
typedef struct _PCICFG_READ_WRITE##Size##_BOOT_SCRIPT {  \
  UINT16                OpCode;               \
  UINT8                 Length;               \
  UINT32                Width;                \
  UINT64                Address;              \
  UINT##Size            OrData##Size;         \
  UINT##Size            AndData##Size;        \
} PCICFG_READ_WRITE##Size##_BOOT_SCRIPT


PCICFG_READ_WRITE_BOOT_SCRIPT_STRUCT(8);
PCICFG_READ_WRITE_BOOT_SCRIPT_STRUCT(16);
PCICFG_READ_WRITE_BOOT_SCRIPT_STRUCT(32);
PCICFG_READ_WRITE_BOOT_SCRIPT_STRUCT(64);

typedef struct {
  UINT16  OpCode;
  UINT8   Length;
  UINT16  Version;
  UINT32  TableLength;
  UINT16  Reserved[2];
} HEADER_BOOT_SCRIPT;

typedef struct {
  UINT16                OpCode;
  UINT8                 Length;
} TERMINATE_BOOT_SCRIPT;

typedef struct {
  UINT16                OpCode;
  UINT8                 Length;
  EFI_PHYSICAL_ADDRESS  EntryPoint;
} DISPATCH_BOOT_SCRIPT;

typedef struct {
  UINT16  OpCode;
  UINT8   Length;
  UINT64  Duration;
} STALL_BOOT_SCRIPT;

typedef union {
  UINT8                 Raw[32];
  HEADER_BOOT_SCRIPT    Header;
  TERMINATE_BOOT_SCRIPT Terminate;
  DISPATCH_BOOT_SCRIPT  Dispatch;
  STALL_BOOT_SCRIPT     Stall;
  
  IO_WRITE8_BOOT_SCRIPT IoWrite8;
  IO_WRITE16_BOOT_SCRIPT IoWrite16;
  IO_WRITE32_BOOT_SCRIPT IoWrite32;
  IO_WRITE64_BOOT_SCRIPT IoWrite64;

  IO_READ_WRITE8_BOOT_SCRIPT IoReadWrite8;
  IO_READ_WRITE16_BOOT_SCRIPT IoReadWrite16;
  IO_READ_WRITE32_BOOT_SCRIPT IoReadWrite32;
  IO_READ_WRITE64_BOOT_SCRIPT IoReadWrite64;

  MEM_WRITE8_BOOT_SCRIPT MemWrite8;
  MEM_WRITE16_BOOT_SCRIPT MemWrite16;
  MEM_WRITE32_BOOT_SCRIPT MemWrite32;
  MEM_WRITE64_BOOT_SCRIPT MemWrite64;

  MEM_READ_WRITE8_BOOT_SCRIPT MemReadWrite8;
  MEM_READ_WRITE16_BOOT_SCRIPT MemReadWrite16;
  MEM_READ_WRITE32_BOOT_SCRIPT MemReadWrite32;
  MEM_READ_WRITE64_BOOT_SCRIPT MemReadWrite64;

  PCICFG_WRITE8_BOOT_SCRIPT PciCfgWrite8;
  PCICFG_WRITE16_BOOT_SCRIPT PciCfgWrite16;
  PCICFG_WRITE32_BOOT_SCRIPT PciCfgWrite32;
  PCICFG_WRITE64_BOOT_SCRIPT PciCfgWrite64;

  PCICFG_READ_WRITE8_BOOT_SCRIPT PciCfgReadWrite8;
  PCICFG_READ_WRITE16_BOOT_SCRIPT PciCfgReadWrite16;
  PCICFG_READ_WRITE32_BOOT_SCRIPT PciCfgReadWrite32;
  PCICFG_READ_WRITE64_BOOT_SCRIPT PciCfgReadWrite64;
} NORMAL_BOOT_SCRIPT;

//#define NORMAL_BOOT_SCRIPT_LENGTH (sizeof(NORMAL_BOOT_SCRIPT) * NORMAL_BOOT_COUNT)

#define START_TABLE_BOOT_SCRIPT                 \
  UINT8 mNormalBootScript[] = {                 \
      EFI_BOOT_SCRIPT_TABLE_OPCODE, 0,          \
      sizeof(EFI_BOOT_SCRIPT_TABLE_HEADER),     \
      0, 0,                                     \
      0xFF,                                     \
      0xFF,                                     \
      0xFF,                                     \
      0xFF,                                     \
      0, 0, 0, 0,

#define STOP_TABLE_BOOT_SCRIPT                  \
      EFI_BOOT_SCRIPT_TERMINATE_OPCODE, 0,      \
      sizeof(EFI_BOOT_SCRIPT_TERMINATE)         \
  };

//
// I/O Script Table Macros
//
#define IO_WRITE8_TABLE_BOOT_SCRIPT(IoAddress, IoData8) \
    EFI_BOOT_SCRIPT_IO_WRITE_OPCODE, 0,     \
    sizeof(EFI_BOOT_SCRIPT_IO_WRITE) + 1,     \
    EfiBootScriptWidthUint8, 0, 0, 0,         \
    1, 0, 0, 0,                               \
    0xFF & IoAddress,                         \
    0xFF & (IoAddress >> 8),                  \
    0, 0, 0, 0, 0, 0,                         \
    IoData8,

#define IO_WRITE16_TABLE_BOOT_SCRIPT(IoAddress, IoData16) \
    EFI_BOOT_SCRIPT_IO_WRITE_OPCODE, 0,       \
    sizeof(EFI_BOOT_SCRIPT_IO_WRITE) + 2,     \
    EfiBootScriptWidthUint16, 0, 0, 0,        \
    1, 0, 0, 0,                               \
    0xFF & IoAddress,                         \
    0xFF & (IoAddress >> 8),                  \
    0, 0, 0, 0, 0, 0,                         \
    0xFF & IoData16,                          \
    0xFF & (IoData16 >> 8),

#define IO_WRITE32_TABLE_BOOT_SCRIPT(IoAddress, IoData32) \
    EFI_BOOT_SCRIPT_IO_WRITE_OPCODE, 0,       \
    sizeof(EFI_BOOT_SCRIPT_IO_WRITE) + 4,     \
    EfiBootScriptWidthUint32, 0, 0, 0,        \
    1, 0, 0, 0,                               \
    0xFF & IoAddress,                         \
    0xFF & (IoAddress >> 8),                  \
    0, 0, 0, 0, 0, 0,                         \
    0xFF & IoData32,                          \
    0xFF & (IoData32 >> 8),                   \
    0xFF & (IoData32 >> 16),                  \
    0xFF & (IoData32 >> 24),

#define IO_READ_WRITE8_TABLE_BOOT_SCRIPT(IoAddress, AndData8, OrData8) \
    EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_IO_READ_WRITE) + 2,  \
    EfiBootScriptWidthUint8, 0, 0, 0,           \
    0xFF & IoAddress,                           \
    0xFF & (IoAddress >> 8),                    \
    0, 0, 0, 0, 0, 0,                           \
    OrData8,                                    \
    AndData8,

#define IO_READ_WRITE16_TABLE_BOOT_SCRIPT(IoAddress, AndData16, OrData16) \
    EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_IO_READ_WRITE) + 4,  \
    EfiBootScriptWidthUint16, 0, 0, 0,          \
    0xFF & IoAddress,                           \
    0xFF & (IoAddress >> 8),                    \
    0, 0, 0, 0, 0, 0,                           \
    0xFF & OrData16,                            \
    0xFF & (OrData16 >> 8),                     \
    0xFF & AndData16,                           \
    0xFF & (AndData16 >> 8),

#define IO_READ_WRITE32_TABLE_BOOT_SCRIPT(IoAddress, AndData32, OrData32) \
    EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_IO_READ_WRITE) + 8,  \
    EfiBootScriptWidthUint32, 0, 0, 0,          \
    0xFF & IoAddress,                           \
    0xFF & (IoAddress >> 8),                    \
    0, 0, 0, 0, 0, 0,                           \
    0xFF & OrData32,                            \
    0xFF & (OrData32 >> 8),                     \
    0xFF & (OrData32 >> 16),                    \
    0xFF & (OrData32 >> 24),                    \
    0xFF & AndData32,                           \
    0xFF & (AndData32 >> 8),                    \
    0xFF & (AndData32 >> 16),                   \
    0xFF & (AndData32 >> 24),

//
// Memory Script Table Macros
//
#define MEM_WRITE8_TABLE_BOOT_SCRIPT(MemAddress, MemData8) \
    EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE, 0,      \
    sizeof(EFI_BOOT_SCRIPT_MEM_WRITE) + 1,    \
    EfiBootScriptWidthUint8, 0, 0, 0,         \
    1, 0, 0, 0,                               \
    0xFF & MemAddress,                        \
    0xFF & (MemAddress >> 8),                 \
    0xFF & (MemAddress >> 16),                \
    0xFF & (MemAddress >> 24),                \
    0, 0, 0, 0,                               \
    MemData8,

#define MEM_WRITE16_TABLE_BOOT_SCRIPT(MemAddress, MemData16) \
    EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE, 0,      \
    sizeof(EFI_BOOT_SCRIPT_MEM_WRITE) + 2,    \
    EfiBootScriptWidthUint16, 0, 0, 0,        \
    1, 0, 0, 0,                               \
    0xFF & MemAddress,                        \
    0xFF & (MemAddress >> 8),                 \
    0xFF & (MemAddress >> 16),                \
    0xFF & (MemAddress >> 24),                \
    0, 0, 0, 0,                               \
    0xFF & MemData16,                         \
    0xFF & (MemData16 >> 8),

#define MEM_WRITE32_TABLE_BOOT_SCRIPT(MemAddress, MemData32) \
    EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE, 0,      \
    sizeof(EFI_BOOT_SCRIPT_MEM_WRITE) + 4,    \
    EfiBootScriptWidthUint32, 0, 0, 0,        \
    1, 0, 0, 0,                               \
    0xFF & MemAddress,                        \
    0xFF & (MemAddress >> 8),                 \
    0xFF & (MemAddress >> 16),                \
    0xFF & (MemAddress >> 24),                \
    0, 0, 0, 0,                               \
    0xFF & MemData32,                         \
    0xFF & (MemData32 >> 8),                  \
    0xFF & (MemData32 >> 16),                 \
    0xFF & (MemData32 >> 24),

#define MEM_READ_WRITE8_TABLE_BOOT_SCRIPT(MemAddress, AndData8, OrData8) \
    EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_MEM_READ_WRITE) + 2,  \
    EfiBootScriptWidthUint8, 0, 0, 0,            \
    0xFF & MemAddress,                           \
    0xFF & (MemAddress >> 8),                    \
    0xFF & (MemAddress >> 16),                   \
    0xFF & (MemAddress >> 24),                   \
    0, 0, 0, 0,                                  \
    OrData8,                                     \
    AndData8,

#define MEM_READ_WRITE16_TABLE_BOOT_SCRIPT(MemAddress, AndData16, OrData16) \
    EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_MEM_READ_WRITE) + 4,  \
    EfiBootScriptWidthUint16, 0, 0, 0,           \
    0xFF & MemAddress,                           \
    0xFF & (MemAddress >> 8),                    \
    0xFF & (MemAddress >> 16),                   \
    0xFF & (MemAddress >> 24),                   \
    0, 0, 0, 0,                                  \
    0xFF & OrData16,                             \
    0xFF & (OrData16 >> 8),                      \
    0xFF & AndData16,                            \
    0xFF & (AndData16 >> 8),

#define MEM_READ_WRITE32_TABLE_BOOT_SCRIPT(MemAddress, AndData32, OrData32) \
    EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, 0,    \
    sizeof(EFI_BOOT_SCRIPT_MEM_READ_WRITE) + 8,  \
    EfiBootScriptWidthUint32, 0, 0, 0,           \
    0xFF & MemAddress,                           \
    0xFF & (MemAddress >> 8),                    \
    0xFF & (MemAddress >> 16),                   \
    0xFF & (MemAddress >> 24),                   \
    0, 0, 0, 0,                                  \
    0xFF & OrData32,                             \
    0xFF & (OrData32 >> 8),                      \
    0xFF & (OrData32 >> 16),                     \
    0xFF & (OrData32 >> 24),                     \
    0xFF & AndData32,                            \
    0xFF & (AndData32 >> 8),                     \
    0xFF & (AndData32 >> 16),                    \
    0xFF & (AndData32 >> 24),


extern UINT8 mNormalBootScript[];

EFI_STATUS
BootScriptExecute (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_BOOT_SCRIPT_EXECUTER_PPI           * This,
  IN EFI_PHYSICAL_ADDRESS                   Address,
  IN EFI_GUID                               * FvFile OPTIONAL
  );

#define NORMAL_BOOT_SCRIPT_EXECUTE                \
  Status = BootScriptExecute (                    \
            PeiServices,                          \
            NULL,                                 \
            (UINT64)&mNormalBootScript[0],        \
            0                                     \
            );                                    \
  if (EFI_ERROR (Status)) {                       \
    return Status;                                \
  }

#endif
