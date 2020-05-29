/** @file
  Definitions for SPI access Library Instance

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

#ifndef _SPI_ACCESS_H_
#define _SPI_ACCESS_H_

//[-start-120418-IB05300312-add]//
#include <Protocol/Pfat.h>
//[-end-120418-IB05300312-add]//
//
// Operation index
//
#define SPI_OPCODE_INDEX_READ_ID        0
#define SPI_OPCODE_INDEX_ERASE          1
#define SPI_OPCODE_INDEX_WRITE          2
#define SPI_OPCODE_INDEX_WRITE_S        3
#define SPI_OPCODE_INDEX_READ           4
#define SPI_OPCODE_INDEX_READ_S         5
#define SPI_OPCODE_INDEX_LOCK           6
#define SPI_OPCODE_INDEX_UNLOCK         7
#define SPI_PREFIX_INDEX_WRITE_EN       0
#define SPI_PREFIX_INDEX_WRITE_S_EN     1

//
// Block lock bit definitions
//
#define SPI_WRITE_LOCK                  0x01
#define SPI_FULL_ACCESS                 0x00


#define SPI_SOFTWARE                                 0
#define SPI_HARDWARE                                 1
#define ME_SIZE_IGNITION_BASIC                       0x10000
#define ME_SIZE_IGNITION_RUNTIME                     0x10000
#define B_PCH_LPC_BIOS_CNTL_SRC                      0x0C


#define MmSpi32(Register)                            Mmio32 (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE)
#define MmSpi32Or(Register, OrData)                  Mmio32Or (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, OrData)
#define MmSpi32And(Register, AndData)                Mmio32And (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData)
#define MmSpi32AndThenOr(Register, AndData, OrData)  Mmio32AndThenOr (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData, OrData)
#define MmSpi16(Register)                            Mmio16 (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE)
#define MmSpi16Or(Register, OrData)                  Mmio16Or (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, OrData)
#define MmSpi16And(Register, AndData)                Mmio16And (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData)
#define MmSpi16AndThenOr(Register, AndData, OrData)  Mmio16AndThenOr (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData, OrData)
#define MmSpi8(Register)                             Mmio8 (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE)
#define MmSpi8Or(Register, OrData)                   Mmio8Or (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, OrData)
#define MmSpi8And(Register, AndData)                 Mmio8And (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData)
#define MmSpi8AndThenOr(Register, AndData, OrData)   Mmio8AndThenOr (mRcrbSpiBase, Register - R_PCH_RCRB_SPI_BASE, AndData, OrData)

//[-start-120524-IB06710088-add]//
#define SPI_READ_BUFFER_SIZE (64) //According SPI controller FIFO buffer

typedef struct {
  UINTN Src;
  UINTN Dst;
  UINTN Size;
} SPI_READ_REGION;
//[-end-120524-IB06710088-add]//
#endif