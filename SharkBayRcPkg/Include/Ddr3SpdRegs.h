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
//; Module Name:
//;
//;   Ddr3SpdRegs.h
//;
//; Abstract:
//;
//;   Define DDR3 SPD register offest. (Accroading to JEDEC SPD Revision 1.0)
//;
//;-----------------------------------------------------------------------------*
#ifndef _DDR3_SPD_REGS_H_
#define _DDR3_SPD_REGS_H_

//
// SPD register offsets
//
#define SPD_DDR3_DEV_TYPE       2       // DRAM Device Type (3:0)
 #define SPD_DDR3_SDRAM          0x0B    // DDR3 Memory type DDR3 SDRAM
 
#define SPD_DDR3_MODULE         3       // Module type (3:0)
 #define SPD_DDR3_MTYPE_MASK     0x0F    // DDR3 Memory Type mask
 #define SPD_DDR3_UNBUFFERED     0x02    // DDR3 Unbuffered Memory Type value
//[-start-111129-IB06720067-modify]//
 #define SPD_DDR3_RDIMM          0x01    // Registered DIMM Memory
 #define SPD_DDR3_UDIMM          0x02    // Unbuffered DIMM Memory
 #define SPD_DDR3_SODIMM         0x03    // Small Outline DIMM Memory
//[-end-111129-IB06720067-modify]//
//[-start-111129-IB06720067-add]//
 #define SPD_DDR3_MICRO_DIMM     0x04    // Micro-DIMM Memory
 #define SPD_DDR3_MINI_RDIMM     0x05    // Mini Registered DIMM Memory
 #define SPD_DDR3_MINI_UDIMM     0x06    // Mini Unbuffered DIMM Memory
//[-end-111129-IB06720067-add]//
 
#define SPD_DDR3_DENBANK        4       // Banks (6:4), Density (3:0)
 #define SPD_DDR3_CAP_MASK       0x07    // Total SDRAM capacity Mask
 
#define SPD_DDR3_ADDRESS        5       // Row (5:3), Column (2:0) address

#define SPD_DDR3_ORG            7       // Ranks (5:3),device width (2:0)
 #define SPD_DDR3_DEVW_MASK      0x07    // DDR3 Device Width
 #define SPD_DDR3_RANKNUM_MASK   0x38    // DDR3 Number of Ranks
 
#define SPD_DDR3_WIDTH          8       // Bus width ext (4:3), Bus width (2:0)
  #define SPD_DDR3_PBW_MASK      0x07    // DDR3 Primary bus width mask 
  #define SPD_DDR3_BWE_MASK      0x18    // DDR3 Bus Width Extension mask
  
#define SPD_DDR3_MTBDD          10      // Medium Timebase (MTB) Dividend
#define SPD_DDR3_MTBDS          11      // Medium Timebase (MTB) Divisor
#define SPD_DDR3_TCLK           12      // Minimum cycle time (tCKmin)
#define SPD_DDR3_CLL            14      // CAS latency supported, low byte
#define SPD_DDR3_CLH            15      // CAS latency supported, high byte
#define SPD_DDR3_TAA            16      // Minimum CAS latency time (tAA)
#define SPD_DDR3_TWR            17      // Minimum write recovery time (tWR)
#define SPD_DDR3_TRCD           18      // Minimum RAS to CAS time (tRCD)
#define SPD_DDR3_TRRD           19      // Minimum RA to RA time (tRRD)
#define SPD_DDR3_TRP            20      // Minimum precharge time (tRP)
#define SPD_DDR3_TRASRC         21      // Upper nibbles for tRAS (7:4), tRC (3:0)
#define SPD_DDR3_TRAS           22      // Minimum active to precharge (tRAS)
#define SPD_DDR3_TRC            23      // Minimum active to active/refresh (tRC)
#define SPD_DDR3_TRFCL          24      // Minimum refresh recovery (tRFC), low byte
#define SPD_DDR3_TRFCH          25      // Minimum refresh recovery (tRFC), high byte
#define SPD_DDR3_TWTR           26      // Minimum internal wr to rd cmd (tWTR)
#define SPD_DDR3_TRTP           27      // Minimum internal rd to pc cmd (tRTP)
#define SPD_DDR3_TFAWH          28      // Upper Nibble for tFAW
#define SPD_DDR3_TFAWL          29      // Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
#define SPD_DDR3_STROPT         31      // SDRAM Thermal and Refresh Options
#define SPD_DDR3_THERMAL        32      // Thermal Related
#define SPD_DDR3_RAW_CARD       62      // Reference Raw Card Used
#define SPD_DDR3_ADD_MAPPING    63      // Address Mapping (Odd Rank Mirror)

#endif 
