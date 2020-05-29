//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;

#ifndef _OEM_CLKGEN_DEFINE_H_
#define _OEM_CLKGEN_DEFINE_H_

#define MAX_SMBUS_RW_BYTE        32
#define I_CHIP_VID               0x8086
#define A_CHIP_VID               0x1022
#define N_CHIP_VID               0x10DE
#define V_CHIP_VID               0x1106
#define NO_VERFY_CHIP_VID        0xFFFF
#define NO_VERFY_CLKGEN_VID_DID  0xFF
#define R_SLOT_STATUS            0xBA

#define CLKGEN_BIT0              0x01
#define CLKGEN_BIT1              0x02
#define CLKGEN_BIT2              0x04
#define CLKGEN_BIT3              0x08
#define CLKGEN_BIT4              0x10
#define CLKGEN_BIT5              0x20
#define CLKGEN_BIT6              0x40
#define CLKGEN_BIT7              0x80

typedef enum {
  CLKGEN_BLOCK_OPERATION,
  CLKGEN_BYTE_OPERATION,
  CLKGEN_FAST_BLOCK_OPERATION,
  CLKGEN_DATA_END = 0xFF
} CLOCK_GEN_RW_TYPE;

typedef enum {
  CLOCK_GEN_VGA_UMA,
  CLOCK_GEN_VGA_DESCRIPTER,
  CLOCK_GEN_VGA_UNITED,
  NO_VERFY_VGA = 0xFF
} CLOCK_GEN_VGA_TYPE;

typedef enum {
  CLOCK_GEN_PRI_INIT,
  CLOCK_GEN_INIT
} CLOCK_GEN_INIT_TYPE;

typedef struct  {
  CLOCK_GEN_RW_TYPE          WriteType;
  UINT8                      Byte;
  UINT8                      AndSourceValue;
  UINT8                      OrSourceValue;
} CLOCK_GEN_DATA_INFO;

typedef struct  {
  UINT8                      Byte;
  UINT8                      Mask;
} CLOCK_GEN_ID_REG;

typedef struct  {
  UINT8                      VenderId;
  UINT8                      DeviceId;
  CLOCK_GEN_ID_REG           VenderIdReg;
  CLOCK_GEN_ID_REG           DeviceIdReg;
} CLOCK_GEN_DEVICE_INFO;

typedef struct  {
  UINT16                     ChipVId;
  UINT16                     PlatformId;
  CLOCK_GEN_VGA_TYPE         VgaType;
  CLOCK_GEN_INIT_TYPE        InitType;
} CLOCK_GEN_EXTRA_INFO;

typedef struct  {
  UINT16                     BigTableSize;
  UINT16                     DataInfoSize;
} STRUCTURE_SIZE_INFO;


typedef struct  {
  CLOCK_GEN_EXTRA_INFO       *ExtraInfo;
  CLOCK_GEN_DATA_INFO        *DataInfo;
} CLOCK_GEN_UNION_DATA_INFO;

typedef struct  {
  CLOCK_GEN_UNION_DATA_INFO  *UnionDataInfo;
  CLOCK_GEN_DEVICE_INFO      *DeviceInfo;
} CLOCK_GEN_UNION_INFO;

#endif
