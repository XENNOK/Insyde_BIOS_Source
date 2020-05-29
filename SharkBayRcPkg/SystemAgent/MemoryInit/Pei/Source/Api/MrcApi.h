/** @file

  Mrc definition of supported features.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MRC_API_H_
#define _MRC_API_H_

#include <MrcTypes.h>

#define HPET_MIN          0x0001  ///< Minimum is one HPET tick = 69.841279ns
#define HPET_1US          0x000F
#define HPET_1MS          0x37EF

#define START_TEST_DELAY  (2 * HPET_MIN)
#define IO_RESET_DELAY    (2 * HPET_MIN)

#define COMP_INT          0x0A    ///< For 10ms
#define MAX_POSSIBLE_VREF 54      ///< Maximum possible margin for Vref
#define MAX_POSSIBLE_TIME 31      ///< Maximum possible margin for time
#define TXEQFULLDRV     (0x30)    ///< 12 Emphasize legs (not trained)
#define DIMMRON         (ODIC_RZQ_6)
#define BCLK_DEFAULT    (100 * 1000 * 1000)

///
/// Define the frequencies that may be possible in the memory controller.
/// Note that not all these values may be supported.
///
#define fNoInit     (0)
#define f800        (800)
#define f1000       (1000)
#define f1067       (1067)
#define f1200       (1200)
#define f1333       (1333)
#define f1400       (1400)
#define f1600       (1600)
#define f1800       (1800)
#define f1867       (1867)
#define f2000       (2000)
#define f2133       (2133)
#define f2200       (2200)
#define f2400       (2400)
#define f2600       (2600)
#define f2667       (2667)
#define fUnSupport  (0x7FFFFFFF)
typedef U32 MrcFrequency;

///
/// Define the memory nominal voltage (VDD).
/// Note that not all these values may be supported.
///
typedef enum {
  VDD_INVALID,
  VDD_1_00    = 1000,
  VDD_1_05    = 1050,
  VDD_1_10    = 1100,
  VDD_1_15    = 1150,
  VDD_1_20    = 1200,
  VDD_1_25    = 1250,
  VDD_1_30    = 1300,
  VDD_1_35    = 1350,
  VDD_1_40    = 1400,
  VDD_1_45    = 1450,
  VDD_1_50    = 1500,
  VDD_1_55    = 1550,
  VDD_1_60    = 1600,
  VDD_1_65    = 1650,
  VDD_1_70    = 1700,
  VDD_1_75    = 1750,
  VDD_1_80    = 1800,
  VDD_1_85    = 1850,
  VDD_1_90    = 1900,
  VDD_1_95    = 1950,
  VDD_2_00    = 2000,
  VDD_2_05    = 2050,
  VDD_2_10    = 2100,
  VDD_2_15    = 2150,
  VDD_2_20    = 2200,
  VDD_2_25    = 2250,
  VDD_2_30    = 2300,
  VDD_2_35    = 2350,
  VDD_2_40    = 2400,
  VDD_2_45    = 2450,
  VDD_2_50    = 2500,
  VDD_2_55    = 2550,
  VDD_2_60    = 2600,
  VDD_2_65    = 2650,
  VDD_2_70    = 2700,
  VDD_2_75    = 2750,
  VDD_2_80    = 2800,
  VDD_2_85    = 2850,
  VDD_2_90    = 2900,
  VDD_2_95    = 2950,
  VDD_MAXIMUM = 0x7FFFFFFF
} MrcVddSelect;

///
/// Compile time configuration parameters - START.
/// The user must set these values for the system.
///
#define MAX_EDGES              2 ///< Maximum number of edges.
#define MAX_BITS               8 ///< BITS per byte.
#define MAX_MR_IN_DIMM         4 ///< Maximum number of mode registers in a DIMM.
#define MAX_CPU_SOCKETS        1 ///< The maximum number of CPUs per system.
#define MAX_CONTROLLERS        1 ///< The maximum number of memory controllers per CPU socket.
#define MAX_CHANNEL            2 ///< The maximum number of channels per memory controller.

#define MAX_DIMMS_IN_CHANNEL   2 ///< The maximum number of DIMMs per channel.

#define MAX_RANK_IN_DIMM       2 ///< The maximum number of ranks per DIMM.
#define MAX_RANK_IN_CHANNEL    (MAX_DIMMS_IN_CHANNEL * MAX_RANK_IN_DIMM) ///< The maximum number of ranks per channel.
#define MAX_SDRAM_IN_DIMM      9           ///< The maximum number of SDRAMs per DIMM when ECC is enabled.
#define MAX_STROBE             18          ///< Number of strobe groups.
#define MAX_DQ                 72          ///< Number of Dq bits used by the rank.
#define CHAR_BITS              8           ///< Number of bits in a char.
#define DIMMSIZEMIN            512         ///< The minimum size of DIMM, in MBytes.
#define DIMMSIZEMAX            (16 * 1024) ///< The maximum size of DIMM, in MBytes.
#define FREQMIN                f1067       ///< The minimum valid frequency.

#define SUPPORT_DDR3           SUPPORT     ///< SUPPORT means that DDR3 is supported by the MRC.
#define ULT_SUPPORT_LPDDR3     SUPPORT     ///< SUPPORT means that LPDDR3 is supported by the MRC.
#define TRAD_SUPPORT_LPDDR3    UNSUPPORT   ///< SUPPORT means that LPDDR3 is supported by the MRC.

#define SUPPORT_SPD_CRC        UNSUPPORT   ///< SUPPORT means that the CRC of the DIMMs SPD must match.
#define SUPPORT_FORCE          UNSUPPORT   ///< SUPPORT means to force tAA, tRCD, tRP to the same value.
#define SUPPORT_ALLDIMMS       UNSUPPORT   ///< SUPPORT means all timings across all DIMMs in the system.
                                           ///< UNSUPPORT means all timings across each memory channel's DIMMs.
#define SUPPORT_XMP            SUPPORT     ///< SUPPORT means Extreme Memory Profiles are supported, else UNSUPPORT.
#define SUPPORT_ECC            SUPPORT     ///< SUPPORT means ECC is suppported, else UNSUPPORT.
#define SUPPORT_UDIMM          SUPPORT     ///< SUPPORT means that unbuffered DIMMs are supported, else UNSUPPORT.
#define SUPPORT_SODIMM         SUPPORT     ///< SUPPORT means that SO-DIMMs are supported, else UNSUPPORT.
#define SUPPORT_RDIMM          UNSUPPORT   ///< SUPPORT means that registered DIMMs are supported, else UNSUPPORT.
#define SUPPORT_PRIWIDTH_8     UNSUPPORT   ///< SUPPORT means that SDRAM primary bus width of 8 is supported by the system.
#define SUPPORT_PRIWIDTH_16    UNSUPPORT   ///< SUPPORT means that SDRAM primary bus width of 16 is supported by the system.
#define SUPPORT_PRIWIDTH_32    UNSUPPORT   ///< SUPPORT means that SDRAM primary bus width of 32 is supported by the system.
#define SUPPORT_PRIWIDTH_64    SUPPORT     ///< SUPPORT means that SDRAM primary bus width of 64 is supported by the system.
#define SUPPORT_DEVWIDTH_4     UNSUPPORT   ///< SUPPORT means that SDRAM device width of 4 is supported by the system.
#define SUPPORT_DEVWIDTH_8     SUPPORT     ///< SUPPORT means that SDRAM device width of 8 is supported by the system.
#define SUPPORT_DEVWIDTH_16    SUPPORT     ///< SUPPORT means that SDRAM device width of 16 is supported by the system.
#define SUPPORT_DEVWIDTH_32    SUPPORT     ///< SUPPORT means that SDRAM device width of 32 is supported by the system.
#define SUPPORT_COLUMN_9       UNSUPPORT   ///< SUPPORT means that 9 bit size is supported by the system.

#define ULT_SUPPORT_COLUMN_10  SUPPORT     ///< SUPPORT means that 10 bit size is supported by the system.
#define ULT_SUPPORT_COLUMN_11  SUPPORT     ///< SUPPORT means that 11 bit size is supported by the system.
#define ULT_SUPPORT_COLUMN_12  SUPPORT     ///< SUPPORT means that 12 bit size is supported by the system.
#define TRAD_SUPPORT_COLUMN_10 SUPPORT     ///< SUPPORT means that 10 bit size is supported by the system.
#define TRAD_SUPPORT_COLUMN_11 UNSUPPORT   ///< SUPPORT means that 11 bit size is supported by the system.
#define TRAD_SUPPORT_COLUMN_12 UNSUPPORT   ///< SUPPORT means that 12 bit size is supported by the system.

#define SUPPORT_ROW_12         SUPPORT     ///< SUPPORT means that 12 bit size is supported by the system.
#define SUPPORT_ROW_13         SUPPORT     ///< SUPPORT means that 13 bit size is supported by the system.
#define SUPPORT_ROW_14         SUPPORT     ///< SUPPORT means that 14 bit size is supported by the system.
#define SUPPORT_ROW_15         SUPPORT     ///< SUPPORT means that 15 bit size is supported by the system.
#define SUPPORT_ROW_16         SUPPORT     ///< SUPPORT means that 16 bit size is supported by the system.
#define SUPPORT_BANK_8         SUPPORT     ///< SUPPORT means that 8 banks is supported by the system.
#define SUPPORT_BANK_16        UNSUPPORT   ///< SUPPORT means that 16 banks is supported by the system.
#define SUPPORT_BANK_32        UNSUPPORT   ///< SUPPORT means that 32 banks is supported by the system.
#define SUPPORT_BANK_64        UNSUPPORT   ///< SUPPORT means that 64 banks is supported by the system.

#define TAAMINPOSSIBLE         4           ///< tAAmin possible range, in number of tCK cycles.
#define TAAMAXPOSSIBLE         24
#define TWRMINPOSSIBLE         5           ///< tWRmin possible range, in number of tCK cycles.
#define TWRMAXPOSSIBLE         16          ///< tWRmin values of 9, 11, 13 ,15 are not valid for DDR3.
#define TRCDMINPOSSIBLE        4           ///< tRCDmin possible range, in number of tCK cycles.
#define TRCDMAXPOSSIBLE        20
#define TRRDMINPOSSIBLE        4           ///< tRRDmin possible range, in number of tCK cycles.
#define TRRDSMINPOSSIBLE       4           ///< tRRD_Smin possible range, in number of tCK cycles.
#define TRRDSMAXPOSSIBLE       65535
#define TRRDLMINPOSSIBLE       4           ///< tRRD_Lmin possible range, in number of tCK cycles.
#define TRRDLMAXPOSSIBLE       65535
#define TRPMINPOSSIBLE         4           ///< tRPmin possible range, in number of tCK cycles.
#define TRPMAXPOSSIBLE         15
#define TRPABMINPOSSIBLE       4           ///< tRPabmin possible range, in number of tCK cycles.
#define TRPABMAXPOSSIBLE       18
#define TRASMINPOSSIBLE        10          ///< tRASmin possible range, in number of tCK cycles.
#define TRASMAXPOSSIBLE        40
#define TRCMINPOSSIBLE         1           ///< tRCmin possible range, in number of tCK cycles.
#define TRCMAXPOSSIBLE         4095
#define TRFCMINPOSSIBLE        1           ///< tRFCmin possible range, in number of tCK cycles.
#define TRFCMAXPOSSIBLE        511
#define TWTRMINPOSSIBLE        4           ///< tWTRmin possible range, in number of tCK cycles.
#define TWTRMAXPOSSIBLE        10
#define TRTPMINPOSSIBLE        4           ///< tRTPmin possible range, in number of tCK cycles.
#define TRTPMAXPOSSIBLE        15
#define TFAWMINPOSSIBLE        10          ///< tFAWmin possible range, in number of tCK cycles.
#define TFAWMAXPOSSIBLE        54
#define TCWLMINPOSSIBLE        5           ///< tCWLmin possible range, in number of tCK cycles.
#define TCWLMAXPOSSIBLE        12
#define TREFIMINPOSSIBLE       1           ///< tREFImin possible range, in number of tCK cycles.
#define TREFIMAXPOSSIBLE       65535
#define NMODEMINPOSSIBLE       1           ///< Command rate mode min possible range, in number of tCK cycles.
#define NMODEMAXPOSSIBLE       3

#define ULT_VDDMINPOSSIBLE     1200        ///< Vdd possible range, in milliVolts.
#define ULT_VDDMAXPOSSIBLE     1350
#define TRAD_VDDMINPOSSIBLE    1350        ///< Vdd possible range, in milliVolts.
#define TRAD_VDDMAXPOSSIBLE    1500

#define SPD3_MANUF_START       117         ///< The starting point for the SPD manufacturing data.
#define SPD3_MANUF_END         127         ///< The ending point for the SPD manufacturing data.
#define HOST_BRIDGE_BUS        0           ///< The host bridge bus number.
#define HOST_BRIDGE_DEVICE     0           ///< The host bridge device number.
#define HOST_BRIDGE_FUNCTION   0           ///< The host bridge function number.
#define HOST_BRIDGE_DEVID      0           ///< The host bridge device id offset.
#define HOST_BRIDGE_REVID      8           ///< The host bridge revision id offset.

#define MEMORY_RATIO_MIN       3           ///< The minimum DDR ratio value that the hardware supports.
#define MEMORY_RATIO_MAX       15          ///< The maximum DDR ratio value that the hardware supports.

///
/// Compile time configuration parameters - END.
///

#if (defined ULT_FLAG && defined TRAD_FLAG)
#define SUPPORT_LPDDR3         (ULT_SUPPORT_LPDDR3 || TRAD_SUPPORT_LPDDR3)
#define SUPPORT_COLUMN_10      (ULT_SUPPORT_COLUMN_10 || TRAD_SUPPORT_COLUMN_10)
#define SUPPORT_COLUMN_11      (ULT_SUPPORT_COLUMN_11 || TRAD_SUPPORT_COLUMN_11)
#define SUPPORT_COLUMN_12      (ULT_SUPPORT_COLUMN_12 || TRAD_SUPPORT_COLUMN_12)
#define VDDMINPOSSIBLE         MIN (ULT_VDDMINPOSSIBLE, TRAD_VDDMINPOSSIBLE)
#define VDDMAXPOSSIBLE         MAX (ULT_VDDMAXPOSSIBLE, TRAD_VDDMAXPOSSIBLE)
#elif (defined ULT_FLAG)
#define SUPPORT_LPDDR3         ULT_SUPPORT_LPDDR3
#define SUPPORT_COLUMN_10      ULT_SUPPORT_COLUMN_10
#define SUPPORT_COLUMN_11      ULT_SUPPORT_COLUMN_11
#define SUPPORT_COLUMN_12      ULT_SUPPORT_COLUMN_12
#define VDDMINPOSSIBLE         ULT_VDDMINPOSSIBLE
#define VDDMAXPOSSIBLE         ULT_VDDMAXPOSSIBLE
#elif (defined TRAD_FLAG)
#define SUPPORT_LPDDR3         TRAD_SUPPORT_LPDDR3
#define SUPPORT_COLUMN_10      TRAD_SUPPORT_COLUMN_10
#define SUPPORT_COLUMN_11      TRAD_SUPPORT_COLUMN_11
#define SUPPORT_COLUMN_12      TRAD_SUPPORT_COLUMN_12
#define VDDMINPOSSIBLE         TRAD_VDDMINPOSSIBLE
#define VDDMAXPOSSIBLE         TRAD_VDDMAXPOSSIBLE
#endif // ULT_FLAG && TRAD_FLAG

#define MRC_ALL_DDR_SUPPORTED    ((SUPPORT_DDR4 == SUPPORT) && ((SUPPORT_DDR3 == SUPPORT) || (SUPPORT_LPDDR3 == SUPPORT)))
#define MRC_DDR3_LPDDR_SUPPORTED ((SUPPORT_DDR3 == SUPPORT) || (SUPPORT_LPDDR3 == SUPPORT))

///
/// Exit mode
///
typedef enum {
  emSlow      = 0,
  emFast      = 1,
  emAuto      = 0xFF,
} MrcExitMode;

///
/// System definitions
///
#define MRC_SYSTEM_BCLK (100)

///
/// Register default values
///
#define MRC_DIMM_RANK_INTERLEAVE      (1)
#define MRC_ENHANCED_INTERLEAVE_MODE  (1)
#define MRC_HORI_MODE                 (1)

///
/// Training definitions
///
#endif
