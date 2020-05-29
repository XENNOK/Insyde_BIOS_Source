/** @file

  SPD processing header file. 

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

#ifndef _MRC_SPD_PROCESSING_H_
#define _MRC_SPD_PROCESSING_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcGlobal.h>
#include <MrcCommon.h>
#include <MrcOemDebugPrint.h>

#pragma pack (push, 1)

///
/// Local definitions
///
#define CRC_SEED                  0
#define CRC_XOR_MASK              0x1021
#define TREFIMIN_DDR3             7800000   /// Average periodic refresh interval, in picoseconds (7.8 us for DDR3)
#define TREFIMIN_LPDDR3           3900000   /// Average periodic refresh interval, in picoseconds (3.9 us for LPDDR3)
#define TREFIMULTIPLIER           1000      /// tREFI value defined in XMP 1.3 spec is actually in thousands of MTB units.
#define MRC_TaaMAX                20000000  /// TaaMax is 20ns
#define MRC_FREQUENCY_MTB_OFFSET  1000000
#define MRC_FREQUENCY_FTB_OFFSET  1000
#define MRC_DDR3_800_TCK_MIN      2500000   /// 1/(800/2) femtoseconds
#define MRC_DDR3_1000_TCK_MIN     2000000   /// 1/(1000/2) femtoseconds
#define MRC_DDR3_1067_TCK_MIN     1875000   /// 1/(1067/2) femtoseconds
#define MRC_DDR3_1200_TCK_MIN     1666666   /// 1/(1200/2) femtoseconds
#define MRC_DDR3_1333_TCK_MIN     1500000   /// 1/(1333/2) femtoseconds
#define MRC_DDR3_1400_TCK_MIN     1428571   /// 1/(1400/2) femtoseconds
#define MRC_DDR3_1600_TCK_MIN     1250000   /// 1/(1600/2) femtoseconds
#define MRC_DDR3_1800_TCK_MIN     1111111   /// 1/(1800/2) femtoseconds
#define MRC_DDR3_1867_TCK_MIN     1071428   /// 1/(1867/2) femtoseconds
#define MRC_DDR3_2000_TCK_MIN     1000000   /// 1/(2000/2) femtoseconds
#define MRC_DDR3_2133_TCK_MIN     937500    /// 1/(2133/2) femtoseconds
#define MRC_DDR3_2200_TCK_MIN     909090    /// 1/(2200/2) femtoseconds
#define MRC_DDR3_2400_TCK_MIN     833333    /// 1/(2400/2) femtoseconds
#define MRC_DDR3_2600_TCK_MIN     769230    /// 1/(2600/2) femtoseconds
#define MRC_DDR3_2667_TCK_MIN     750000    /// 1/(2667/2) femtoseconds
#define MRC_DDR3_2800_TCK_MIN     714285    /// 1/(2800/2) femtoseconds

///
/// SPD field definitions
///
#define MRC_SPD_DDR3_SDRAM_TYPE_NUMBER      (0x0B)
#define MRC_SPD_LPDDR3_SDRAM_TYPE_NUMBER    (0xF1)

#define MRC_SPD_RDIMM_TYPE_NUMBER           (0x01)
#define MRC_SPD_UDIMM_TYPE_NUMBER           (0x02)
#define MRC_SPD_SODIMM_TYPE_NUMBER          (0x03)

#define MRC_SPD_DDR3_SDRAM_BANK_8           (0x00)
#define MRC_SPD_DDR3_SDRAM_BANK_16          (0x01)
#define MRC_SPD_DDR3_SDRAM_BANK_32          (0x02)
#define MRC_SPD_DDR3_SDRAM_BANK_64          (0x03)

#define MRC_SPD_SDRAM_DENSITY_12            (0x00)
#define MRC_SPD_SDRAM_DENSITY_13            (0x01)
#define MRC_SPD_SDRAM_DENSITY_14            (0x02)
#define MRC_SPD_SDRAM_DENSITY_15            (0x03)
#define MRC_SPD_SDRAM_DENSITY_16            (0x04)

#define MRC_SPD_SDRAM_ROW_12                (0x00)
#define MRC_SPD_SDRAM_ROW_13                (0x01)
#define MRC_SPD_SDRAM_ROW_14                (0x02)
#define MRC_SPD_SDRAM_ROW_15                (0x03)
#define MRC_SPD_SDRAM_ROW_16                (0x04)

#define MRC_SPD_SDRAM_COLUMN_9              (0x00)
#define MRC_SPD_SDRAM_COLUMN_10             (0x01)
#define MRC_SPD_SDRAM_COLUMN_11             (0x02)
#define MRC_SPD_SDRAM_COLUMN_12             (0x03)

#define MRC_SPD_SDRAM_DEVICE_WIDTH_4        (0x00)
#define MRC_SPD_SDRAM_DEVICE_WIDTH_8        (0x01)
#define MRC_SPD_SDRAM_DEVICE_WIDTH_16       (0x02)
#define MRC_SPD_SDRAM_DEVICE_WIDTH_32       (0x03)

#define MRC_SPD_PRIMARY_BUS_WIDTH_8         (0x00)
#define MRC_SPD_PRIMARY_BUS_WIDTH_16        (0x01)
#define MRC_SPD_PRIMARY_BUS_WIDTH_32        (0x02)
#define MRC_SPD_PRIMARY_BUS_WIDTH_64        (0x03)

#define MRC_SPD_BUS_WIDTH_EXTENSION_0       (0x00)
#define MRC_SPD_BUS_WIDTH_EXTENSION_8       (0x01)

#define MRC_SPD_CL_SUPPORTED_MASK           (0x7FFF)

#define XMP_VDD_INTEGER                     (1000)
#define XMP_VDD_INCREMENT                   (50)
#define XMP_VDD_MIN_POSSIBLE                (1200)
#define XMP_VDD_MAX_POSSIBLE                (1650)

#define MRC_SPD_REF_RAW_CARD_SIZE           (5)
#define MRC_SPD_REF_RAW_CARD_EXT_OFFSET     (7)

#define XMP_ID_STRING                       (0x4A0C)

typedef struct {
  U32           tCK;
  MrcFrequency  Frequency;
  U8            RefClkFlag;  // 0 = invalid freq. 1 = valid only at 133 RefClk, 2 = valid only at 100 RefClk, 3 = valid at both.
} TRangeTable;

typedef struct {
  BOOL (*mrc_task) (MrcParameters * const MrcData);
} SpdTimeCallTable;

typedef struct {
  BOOL (*mrc_task) (MrcParameters * const MrcData, const MrcSpd * const Spd, MrcDimmOut * const DimmOut);
} SpdRecogCallTable;

typedef union {
  struct {
    U8  Vdd1_20 : 1;
    U8  Vdd1_25 : 1;
    U8  Vdd1_35 : 1;
    U8  Vdd1_50 : 1;
    U8          : 4;
  } Bits;
  U8 Data;
} SpdVddFlag;

  typedef struct {
    BOOL TradSupport;
    BOOL UltSupport;
  } SupportEntry;

  typedef struct {
    SupportEntry Lpddr3;
    SupportEntry Column10;
    SupportEntry Column11;
    SupportEntry Column12;
    SupportEntry VddMin;
    SupportEntry VddMax;
  } SupportTable;

/**
  @brief
    Calculate the memory clock value from the current memory frequency.

    @param[in, out] MrcData     - Pointer to MrcData data structure.
    @param[in]      Frequency   - Memory frequency to convert.
    @param[out]     tCKminIndex - Pointer to the chosen table index.

    @retval Returns the tCK value for the given frequency.
**/
extern
U32
ConvertFreq2Clock (
  IN OUT MrcParameters *const MrcData,
  IN     const MrcFrequency   Frequency,
  OUT    S32           *const tCKminIndex
);

/**
@brief
  Calculate the CRC16 of the provided SPD data. CRC16 formula is the same
  one that is used for calculating the CRC16 stored at SPD bytes 126-127.
  This can be used to detect DIMM change.

  @param[in]  Buffer - Pointer to the start of the data.
  @param[in]  Size   - Amount of data in the buffer, in bytes.
  @param[out] Crc    - Pointer to location to write the calculated CRC16 value.

  @retval Returns TRUE.
**/
extern
BOOL
GetDimmCrc (
  IN  const U8  *const Buffer,
  IN  const U32        Size,
  OUT U16       *const Crc
  );

/**
  @brief
  Determine the starting address and size of the SPD area to generate a CRC.

    @param[in, out] MrcData    - The MRC "global data".
    @param[in]      Controller - Controller index.
    @param[in]      Channel    - Channel index.
    @param[in]      Dimm       - Dimm index.
    @param[out]     CrcSize    - Location to write CRC block size.

    @retval The starting address of the CRC block.
**/
const U8 *
MrcSpdCrcArea (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Controller,
  IN     U8                   Channel,
  IN     U8                   Dimm,
  OUT    U32           *const CrcSize
  );

/**
@brief
  Process the SPD information for all DIMMs on all channels.

  @param[in, out] MrcData - The MRC "global data".

  @retval mrcSuccess on success, mrcDimmNotExist if no DIMMs found.
**/
extern
MrcStatus
MrcSpdProcessing (
  IN OUT MrcParameters *const MrcData
  );

#pragma pack (pop)
#endif // _MrcSpdProcessing_h_
