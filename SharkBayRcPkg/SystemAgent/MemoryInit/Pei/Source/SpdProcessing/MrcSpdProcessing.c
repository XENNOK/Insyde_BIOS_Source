/** @file
  By passing in a SPD data structure and platform support values, an output
  structure is populated with DIMM configuration information.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.

**/

#include "MrcSpdProcessing.h"

#ifdef MRC_DEBUG_PRINT
const char        UnknownString[]  = "unknown";
const char        Ddr3String[]     = "DDR3";
const char        Ddr4String[]     = "DDR4";
const char        RdimmString[]    = "RDIMM";
const char        UdimmString[]    = "UDIMM";
const char        SodimmString[]   = "SO-DIMM";
const char        Sodimm72String[] = "72 bit SO-DIMM";
const char        StdString[]      = "Standard";
#if (SUPPORT_XMP == SUPPORT)
const char        Xmp1String[]     = "XMP1";
const char        Xmp2String[]     = "XMP2";
const char        XpString[]       = "  XMP profile %u is %sabled and recommended channel config: %u DIMM per channel\n";
#endif // SUPPORT_XMP
const char        ErrorString[]    = "ERROR: Unsupported ";
const char        SpdValString[]   = "SPD value: ";
const char        IsSupString[]    = " is supported";
const char        NotSupString[]   = " is not supported";
const char        TimeBaseString[] = "Timebase (MTB/FTB)";
const char        tAAString[]      = "CAS Latency Time (tAAmin)";
const char        tCKString[]      = "SDRAM Cycle Time (tCKmin)";
const char        tWRString[]      = "Write recovery time (tWRmin)";
const char        tRCDString[]     = "RAS# to CAS# delay time (tRCDmin)";
const char        tRRDString[]     = "Row active to row active delay time (tRRDmin)";
const char        tRPString[]      = "Row precharge delay time (tRPmin)";
#if (SUPPORT_LPDDR3 == SUPPORT)
const char        Lpddr3String[]   = "LPDDR3";
const char        tRPabString[]    = "Row precharge delay time for all banks (tRPab)";
#endif // SUPPORT_LPDDRn
const char        tRASString[]     = "Active to precharge delay time (tRASmin)";
const char        tRCString[]      = "Active to active/refresh delay time (tRCmin)";
const char        tRFCString[]     = "Refresh recovery delay time (tRFCmin)";
const char        tWTRString[]     = "Internal write to read command delay time (tWTRmin)";
const char        tRTPString[]     = "Internal read to precharge delay time (tRTPmin)";
const char        tFAWString[]     = "Active to active/refresh delay time (tFAWmin)";
const char        tREFIString[]    = "Average Periodic Refresh Interval (tREFImin)";
const char        tCWLString[]     = "CAS Write Latency (tCWLmin)";
const char        NmodeString[]    = "Command rate mode (Nmode)";
const char        VddString[]      = "Module voltage VDD (mVolts)";
const char        BestCaseString[] = "Best case value for profiles 0-";
const char        ProfileString[]  = "Profile";
const char        HeaderString[]   = "Profile Controller Channel Dimm Value";
const char        RrcString[][3]   = {
                                     " A", " B", " C", " D", " E", " F", " G", " H", " J", " K",
                                     " L", " M", " N", " P", " R", " T", " U", " V", " W", " Y",
                                     "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AJ", "AK",
                                     "AL", "ZZ", "AM", "AN", "AP", "AR", "AT", "AU", "AV", "AW",
                                     "AY", "BA", "BB", "BC", "BD", "BE", "BF", "BG", "BH", "BJ",
                                     "BK", "BL", "BM", "BN", "BP", "BR", "BT", "BU", "BV", "BW",
                                     "BY", "CA", "CB", "ZZ"};
#endif // MRC_DEBUG_PRINT

const TRangeTable Range[] = {
  { 0xFFFFFFFF,            fUnSupport, (0 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_800_TCK_MIN,  f800,       (1 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1000_TCK_MIN, f1000,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1067_TCK_MIN, f1067,      (1 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1200_TCK_MIN, f1200,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1333_TCK_MIN, f1333,      (1 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1400_TCK_MIN, f1400,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1600_TCK_MIN, f1600,      (1 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1800_TCK_MIN, f1800,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_1867_TCK_MIN, f1867,      (1 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2000_TCK_MIN, f2000,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2133_TCK_MIN, f2133,      (1 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2200_TCK_MIN, f2200,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2400_TCK_MIN, f2400,      (1 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2600_TCK_MIN, f2600,      (0 << MRC_REF_CLOCK_133) | (1 << MRC_REF_CLOCK_100) },
  { MRC_DDR3_2667_TCK_MIN, f2667,      (1 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) },
  { 0,                     fNoInit,    (0 << MRC_REF_CLOCK_133) | (0 << MRC_REF_CLOCK_100) }
};

const SupportTable PlatformSupport = {
  {TRAD_SUPPORT_LPDDR3,    ULT_SUPPORT_LPDDR3   },
  {TRAD_SUPPORT_COLUMN_10, ULT_SUPPORT_COLUMN_10},
  {TRAD_SUPPORT_COLUMN_11, ULT_SUPPORT_COLUMN_11},
  {TRAD_SUPPORT_COLUMN_12, ULT_SUPPORT_COLUMN_12},
  {TRAD_VDDMINPOSSIBLE,    ULT_VDDMINPOSSIBLE   },
  {TRAD_VDDMAXPOSSIBLE,    ULT_VDDMAXPOSSIBLE   }
};

/**
  @brief
    Calculate the memory clock value from the current memory frequency.

    @param[in, out] MrcData     - Pointer to MrcData data structure.
    @param[in]      Frequency   - Memory frequency to convert.
    @param[out]     tCKminIndex - Pointer to the chosen table index.

    @retval Returns the tCK value for the given frequency.
**/
U32
ConvertFreq2Clock (
  IN OUT MrcParameters *const MrcData,
  IN     const MrcFrequency   Frequency,
  OUT    S32           *const tCKminIndex
  )
{
  U32 tCKminActual;
  S32 Index;

  tCKminActual = MRC_DDR3_800_TCK_MIN;
  for (Index = 0; (U32) Index < (sizeof (Range) / sizeof (TRangeTable)); Index++) {
    if (Frequency == Range[Index].Frequency) {
      tCKminActual = Range[Index].tCK;
      break;
    }
  }
  if (tCKminIndex != NULL) {
    *tCKminIndex = Index;
  }
  return (tCKminActual);
}

/**
  @brief
  Calculate the memory frequency from the memory clock value.

    @param[in, out] MrcData     - Pointer to MrcData data structure.
    @param[in]      RefClk      - The memory reference clock.
    @param[in]      tCKmin      - The tCKmin value to convert.
    @param[out]     tCKminIndex - Pointer to the chosen table index.

    @retval Returns the tCK value for the given frequency.
**/
static
U32
ConvertClock2Freq (
  IN OUT MrcParameters *const  MrcData,
  IN     const MrcRefClkSelect RefClk,
  IN     const U32             tCKmin,
  OUT    S32           *const  tCKminIndex
  )
{
  MrcOutput     *Outputs;
  MrcFrequency  Frequency;
  U32           Index;

  Outputs = &MrcData->SysOut.Outputs;

  //
  // Convert tCK value to the nearest frequency value.
  // Then find slowest valid frequency for the given reference clock.
  //
  Frequency = fNoInit;
  for (Index = 0; Index < (sizeof (Range) / sizeof (TRangeTable)) - 1; Index++) {
    if ((tCKmin <= Range[Index].tCK) && (tCKmin > Range[Index + 1].tCK)) {
      Frequency = Range[Index].Frequency;
      break;
    }
  }

  while (Index) {
    if ((Range[Index].RefClkFlag & (1 << RefClk)) == 0) {
      Frequency = Range[--Index].Frequency;
    } else {
      break;
    }
  }
  if (tCKminIndex != NULL) {
    *tCKminIndex = Index;
  }
  return (Frequency);
}

/**
  @brief
  Determine if the DIMM slot is filled.
    If the SPD structure is all zero's, then DIMM is not present.

    @param[in] MrcData - Pointer to MrcData data structure.
    @param[in] Spd     - Pointer to Spd data structure.
    @param[in] Size    - Amount of data, in bytes.

    @retval TRUE on valid value, otherwise FALSE and the value is set to zero.
**/
static
MrcDimmSts
DimmPresence (
  IN const MrcParameters *const MrcData,
  IN const MrcSpd        *const Spd,
  IN const U32                  Size
  )
{

  const U8        *p;
  U32             count;
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

  p     = (const U8 *) Spd;
  count = Size;
  while (count--) {
    if (0 != *p++) {
      return DIMM_PRESENT;
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "  Warning: No DIMM detected in slot\n");
  return DIMM_NOT_PRESENT;
}

/**
  @brief
  Determine if the DIMM is valid and supported.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE.
**/
static
BOOL
ValidDimm (
  IN OUT MrcParameters *const MrcData,
  IN     const MrcSpd  *const Spd,
  IN OUT MrcDimmOut    *const DimmOut
  )
{
  const SPD_EXTREME_MEMORY_PROFILE  *Xmp;
  BOOL                              Status;
  U8                                DeviceType;
  MrcCpuModel                       CpuModel;
  SpdVddFlag                        VddFlag;
#ifdef MRC_DEBUG_PRINT
  const U16                         BytesUsedConst[] = {0, 128, 176, 256};
  const MrcDebug                    *Debug;
  const char                        *DramTypeString;
  const char                        *ModuleTypeString;
  const char                        *ProfileString;
  SPD_REVISION_STRUCT               Revision;
  U16                               BytesUsed;
  U16                               BytesTotal;
  U16                               CrcCoverage;

  Debug    = &MrcData->SysIn.Inputs.Debug;
#endif // MRC_DEBUG_PRINT

  Status   = TRUE;
  CpuModel = MrcData->SysIn.Inputs.CpuModel;

  DeviceType = Spd->Ddr3.General.DramDeviceType.Bits.Type;

  VddFlag.Bits.Vdd1_35 = Spd->Ddr3.General.ModuleNominalVoltage.Bits.OperationAt1_35;

  switch (DeviceType) {
#if (SUPPORT_DDR3 == SUPPORT)
  case MRC_SPD_DDR3_SDRAM_TYPE_NUMBER:
    DimmOut->DdrType    = MRC_DDR_TYPE_DDR3;
    DimmOut->ModuleType = Spd->Ddr3.General.ModuleType.Bits.ModuleType;
    Xmp                 = &Spd->Ddr3.Xmp;
    if ((CpuModel == cmHSW_ULT) && (VddFlag.Bits.Vdd1_35 == 0)) {
      Status = FALSE;
#ifdef MRC_DEBUG_PRINT
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "  DDR3 memory does not support 1.35V operation\n");
#endif // MRC_DEBUG_PRINT
    }
    break;
#endif // SUPPORT_DDR3

#if (SUPPORT_LPDDR3 == SUPPORT)
  case MRC_SPD_LPDDR3_SDRAM_TYPE_NUMBER:
    if ((CpuModel == cmHSW_ULT && PlatformSupport.Lpddr3.UltSupport) ||
        ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.Lpddr3.TradSupport)) {
      DimmOut->DdrType    = MRC_DDR_TYPE_LPDDR3;
      DimmOut->ModuleType = Spd->Ddr3.General.ModuleType.Bits.ModuleType;
      Xmp                 = &Spd->Ddr3.Xmp;
      break;
    }
    // no break;
#endif // SUPPORT_LPDDR3


  default:
    DimmOut->DdrType    = MRC_DDR_TYPE_UNKNOWN;
    DimmOut->ModuleType = 0;
    Xmp                 = NULL;
    Status              = FALSE;
    break;
  }

  if (Status) {
    switch (DimmOut->ModuleType) {
#if (SUPPORT_RDIMM == SUPPORT)
      case MRC_MODULE_TYPE_RDIMM:
        break;
#endif

#if (SUPPORT_UDIMM == SUPPORT)
      case MRC_MODULE_TYPE_UDIMM:
        break;
#endif

#if (SUPPORT_SODIMM == SUPPORT)
      case MRC_MODULE_TYPE_SODIMM:
      case MRC_MODULE_72B_SO_UDIMM:
        break;
#endif

      default:
        Status = FALSE;
        break;
    }
  }

#if (SUPPORT_XMP == SUPPORT)
  DimmOut->XmpSupport = 0;
  if (Status) {
    if ((XMP_ID_STRING != Xmp->Header.XmpId) ||
      (0x12 != (Xmp->Header.XmpRevision.Data & 0xFE)) ||
      ((MrcData->SysIn.Inputs.MemoryProfile == XMP_PROFILE1) && (Xmp->Header.XmpOrgConf.Bits.ProfileEnable1 == 0)) ||
      ((MrcData->SysIn.Inputs.MemoryProfile == XMP_PROFILE2) && (Xmp->Header.XmpOrgConf.Bits.ProfileEnable2 == 0))) {
        if ((MrcData->SysIn.Inputs.MemoryProfile == XMP_PROFILE1) || (MrcData->SysIn.Inputs.MemoryProfile == XMP_PROFILE2)) {
          Status = FALSE;
        }
    } else {
      MrcData->SysOut.Outputs.XmpProfileEnable |= 1;
    }
    if (XMP_ID_STRING == Xmp->Header.XmpId) {
      if (0x12 == (Xmp->Header.XmpRevision.Data & 0xFE)) {
        DimmOut->XmpRevision = Xmp->Header.XmpRevision.Data;
      }
      if (Xmp->Header.XmpOrgConf.Bits.ProfileEnable1 != 0) {
        DimmOut->XmpSupport |= 1;
      }
      if (Xmp->Header.XmpOrgConf.Bits.ProfileEnable2 != 0) {
        DimmOut->XmpSupport |= 2;
      }
    }
  }
#endif // SUPPORT_XMP

#ifdef MRC_DEBUG_PRINT
  switch (MrcData->SysIn.Inputs.MemoryProfile) {
      case STD_PROFILE:
      case USER_PROFILE:
      default:
        ProfileString = StdString;
        break;
#if (SUPPORT_XMP == SUPPORT)
      case XMP_PROFILE1:
        ProfileString = Xmp1String;
        break;
      case XMP_PROFILE2:
        ProfileString = Xmp2String;
        break;
#endif // SUPPORT_XMP
  }

  switch (DeviceType) {
#if (SUPPORT_DDR3 == SUPPORT)
  case MRC_SPD_DDR3_SDRAM_TYPE_NUMBER:
    DramTypeString = Ddr3String;
    BytesTotal    = 256 * Spd->Ddr3.General.Description.Bits.BytesTotal;
    BytesUsed     = BytesUsedConst[Spd->Ddr3.General.Description.Bits.BytesUsed & 3];
    CrcCoverage   = 125 - (9 * Spd->Ddr3.General.Description.Bits.CrcCoverage);
    Revision.Data = Spd->Ddr3.General.Revision.Data;
    break;
#endif // SUPPORT_DDR3

#if (SUPPORT_LPDDR3 == SUPPORT)
  case MRC_SPD_LPDDR3_SDRAM_TYPE_NUMBER:
    if ((CpuModel == cmHSW_ULT && PlatformSupport.Lpddr3.UltSupport) ||
        ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.Lpddr3.TradSupport)) {
      DramTypeString = Lpddr3String;
      BytesTotal    = 256 * Spd->Ddr3.General.Description.Bits.BytesTotal;
      BytesUsed     = BytesUsedConst[Spd->Ddr3.General.Description.Bits.BytesUsed & 3];
      CrcCoverage   = 125 - (9 * Spd->Ddr3.General.Description.Bits.CrcCoverage);
      Revision.Data = Spd->Ddr3.General.Revision.Data;
      break;
    }
    // no break;
#endif // SUPPORT_LPDDR3


  default:
    DramTypeString = UnknownString;
    BytesTotal     = 0;
    BytesUsed      = 0;
    CrcCoverage    = 0;
    Revision.Data  = 0;
    break;
  }

  switch (DimmOut->ModuleType) {
#if (SUPPORT_RDIMM == SUPPORT)
    case MRC_MODULE_TYPE_RDIMM:
      ModuleTypeString = RdimmString;
      break;
#endif // SUPPORT_RDIMM

#if (SUPPORT_UDIMM == SUPPORT)
    case MRC_MODULE_TYPE_UDIMM:
      ModuleTypeString = UdimmString;
      break;
#endif // SUPPORT_UDIMM

#if (SUPPORT_SODIMM == SUPPORT)
    case MRC_MODULE_TYPE_SODIMM:
      ModuleTypeString = SodimmString;
      break;

    case MRC_MODULE_72B_SO_UDIMM:
      ModuleTypeString = SodimmString;
      break;
#endif // SUPPORT_SODIMM

    default:
      ModuleTypeString = UnknownString;
      break;
  }

  if (Status) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "  %s %s detected, Rev: %u.%u, Size: %u used/%u total, CRC coverage: 0 - %u\n",
      DramTypeString,
      ModuleTypeString,
      Revision.Bits.Major,
      Revision.Bits.Minor,
      BytesUsed,
      BytesTotal,
      CrcCoverage
      );
  } else {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "  %s %s detected, SPD Dram type %Xh, module type %Xh\n",
      DramTypeString,
      ModuleTypeString,
      DeviceType,
      DimmOut->ModuleType
      );
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  DIMM profile %s selected\n", ProfileString);
#if (SUPPORT_XMP == SUPPORT)
  if (Xmp == NULL) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Xmp structure is NULL!\n\n");
  } else {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "  XMP String: %Xh, Rev: %u.%u\n",
      Xmp->Header.XmpId,
      Xmp->Header.XmpRevision.Bits.Major,
      Xmp->Header.XmpRevision.Bits.Minor
    );
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      XpString,
      1,
      Xmp->Header.XmpOrgConf.Bits.ProfileEnable1 ? "en" : "dis",
      Xmp->Header.XmpOrgConf.Bits.ProfileConfig1 + 1
    );
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      XpString,
      2,
      Xmp->Header.XmpOrgConf.Bits.ProfileEnable2 ? "en" : "dis",
      Xmp->Header.XmpOrgConf.Bits.ProfileConfig2 + 1
    );
  }
#endif // SUPPORT_XMP

#endif // MRC_DEBUG_PRINT

  return Status;
}

/**
  @brief
  Determine if the DIMM SDRAM device width is valid and return the value.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE and the value is set to zero.
**/
static
BOOL
ValidSdramDeviceWidth (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

    DimmOut->SdramWidthIndex = Spd->Ddr3.General.ModuleOrganization.Bits.SdramDeviceWidth;

  switch (DimmOut->SdramWidthIndex) {
#if (SUPPORT_DEVWIDTH_4 == SUPPORT)

  case MRC_SPD_SDRAM_DEVICE_WIDTH_4:
    DimmOut->SdramWidth = 4;
    break;
#endif
#if (SUPPORT_DEVWIDTH_8 == SUPPORT)

  case MRC_SPD_SDRAM_DEVICE_WIDTH_8:
    DimmOut->SdramWidth = 8;
    break;
#endif
#if (SUPPORT_DEVWIDTH_16 == SUPPORT)

  case MRC_SPD_SDRAM_DEVICE_WIDTH_16:
    DimmOut->SdramWidth = 16;
    break;
#endif
#if (SUPPORT_DEVWIDTH_32 == SUPPORT)

  case MRC_SPD_SDRAM_DEVICE_WIDTH_32:
    DimmOut->SdramWidth = 32;
    break;
#endif

  default:
    DimmOut->SdramWidth = 0;
    MRC_DEBUG_MSG (Debug,
      MSG_LEVEL_ERROR,
      "%sSDRAM device width, %s%Xh\n",
      ErrorString,
      SpdValString,
      DimmOut->SdramWidthIndex
      );
    return FALSE;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  SDRAM device width: %u\n", DimmOut->SdramWidth);
  return TRUE;
}

/**
  @brief
  Determine if the DIMM SDRAM row address size is valid and return the value.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE if the row address size is valid, otherwise FALSE and the value is set to zero.
**/
static
BOOL
ValidRowSize (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  U8              RowBits;
  U8              RowAddress;
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

    RowAddress = Spd->Ddr3.General.SdramAddressing.Bits.RowAddress;

  switch (RowAddress) {
#if (SUPPORT_ROW_12 == SUPPORT)

  case MRC_SPD_SDRAM_ROW_12:
    DimmOut->RowSize = MRC_BIT12;
    RowBits          = 12;
    break;
#endif
#if (SUPPORT_ROW_13 == SUPPORT)

  case MRC_SPD_SDRAM_ROW_13:
    DimmOut->RowSize = MRC_BIT13;
    RowBits          = 13;
    break;
#endif
#if (SUPPORT_ROW_14 == SUPPORT)

  case MRC_SPD_SDRAM_ROW_14:
    DimmOut->RowSize = MRC_BIT14;
    RowBits          = 14;
    break;
#endif
#if (SUPPORT_ROW_15 == SUPPORT)

  case MRC_SPD_SDRAM_ROW_15:
    DimmOut->RowSize = MRC_BIT15;
    RowBits          = 15;
    break;
#endif
#if (SUPPORT_ROW_16 == SUPPORT)

  case MRC_SPD_SDRAM_ROW_16:
    DimmOut->RowSize = MRC_BIT16;
    RowBits          = 16;
    break;
#endif
  default:
    DimmOut->RowSize = 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "%sSDRAM row size, %s%Xh\n", ErrorString, SpdValString, RowAddress);
    return FALSE;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Row bits: %u\n", RowBits);
  return TRUE;
}

/**
  @brief
  Determine if the DIMM SDRAM column address size is valid and return the value.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE if the column address size is valid, otherwise FALSE and the value is set to zero.
**/
static
BOOL
ValidColumnSize (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;
  U8              ColumnBits;
  U8              ColumnAddress;
  MrcCpuModel     CpuModel;

  Debug    = &MrcData->SysIn.Inputs.Debug;
  CpuModel = MrcData->SysIn.Inputs.CpuModel;

    ColumnAddress = Spd->Ddr3.General.SdramAddressing.Bits.ColumnAddress;

  switch (ColumnAddress) {
#if (SUPPORT_COLUMN_9 == SUPPORT)
  case MRC_SPD_SDRAM_COLUMN_9:
    DimmOut->ColumnSize = MRC_BIT9;
    ColumnBits          = 9;
    break;
#endif

#if (SUPPORT_COLUMN_10 == SUPPORT)
  case MRC_SPD_SDRAM_COLUMN_10:
    if ((CpuModel == cmHSW_ULT && PlatformSupport.Column10.UltSupport) ||
        ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.Column10.TradSupport)) {
      DimmOut->ColumnSize = MRC_BIT10;
      ColumnBits          = 10;
      break;
    }
    // no break;
#endif

#if (SUPPORT_COLUMN_11 == SUPPORT)
  case MRC_SPD_SDRAM_COLUMN_11:
    if ((CpuModel == cmHSW_ULT && PlatformSupport.Column11.UltSupport) ||
        ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.Column11.TradSupport)) {
      DimmOut->ColumnSize = MRC_BIT11;
      ColumnBits          = 11;
      break;
    }
    // no break;
#endif

#if (SUPPORT_COLUMN_12 == SUPPORT)
  case MRC_SPD_SDRAM_COLUMN_12:
    if ((CpuModel == cmHSW_ULT && PlatformSupport.Column12.UltSupport) ||
        ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.Column12.TradSupport)) {
      DimmOut->ColumnSize = MRC_BIT12;
      ColumnBits          = 12;
      break;
    }
    // no break;
#endif

  default:
    DimmOut->ColumnSize = 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "%sSDRAM column size, %s%Xh\n", ErrorString, SpdValString, ColumnAddress);
    return FALSE;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Column bits: %u\n", ColumnBits);
  return TRUE;
}

/**
  @brief
  Determine if the DIMM SDRAM primary bus width is valid and return the value.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE and the value is set to zero.
**/
static
BOOL
ValidPrimaryWidth (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  U8              Width;
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

    Width = Spd->Ddr3.General.ModuleMemoryBusWidth.Bits.PrimaryBusWidth;

  switch (Width) {
#if (SUPPORT_PRIWIDTH_8 == SUPPORT)

  case MRC_SPD_PRIMARY_BUS_WIDTH_8:
    DimmOut->PrimaryBusWidth = 8;
    break;
#endif
#if (SUPPORT_PRIWIDTH_16 == SUPPORT)

  case MRC_SPD_PRIMARY_BUS_WIDTH_16:
    DimmOut->PrimaryBusWidth = 16;
    break;
#endif
#if (SUPPORT_PRIWIDTH_32 == SUPPORT)

  case MRC_SPD_PRIMARY_BUS_WIDTH_32:
    DimmOut->PrimaryBusWidth = 32;
    break;
#endif
#if (SUPPORT_PRIWIDTH_64 == SUPPORT)

  case MRC_SPD_PRIMARY_BUS_WIDTH_64:
    DimmOut->PrimaryBusWidth = 64;
    break;
#endif

  default:
    DimmOut->PrimaryBusWidth = 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "%sSDRAM primary bus width, %s%Xh\n", ErrorString, SpdValString, Width);
    return FALSE;
    break;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Primary bus width: %u\n", DimmOut->PrimaryBusWidth);
  return TRUE;
}

/**
  Determines if the number of Bank are valid.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE.
**/
static
BOOL
ValidBank (
  IN OUT MrcParameters  *const MrcData,
  IN const MrcSpd       *const Spd,
  IN OUT MrcDimmOut     *const DimmOut
  )
{
  const MrcDebug  *Debug;
  U8              BankAddress;
  U8              BankGroup;
  U8              ValidCheck;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  ValidCheck  = TRUE;
      DimmOut->DensityIndex = Spd->Ddr3.General.SdramDensityAndBanks.Bits.Density;
      BankAddress           = Spd->Ddr3.General.SdramDensityAndBanks.Bits.BankAddress;
      BankGroup             = 0;
      switch (BankAddress) {
#if (SUPPORT_BANK_8 == SUPPORT)
        case MRC_SPD_DDR3_SDRAM_BANK_8:
#endif
#if (SUPPORT_BANK_16 == SUPPORT)
        case MRC_SPD_DDR3_SDRAM_BANK_16:
#endif
#if (SUPPORT_BANK_32 == SUPPORT)
        case MRC_SPD_DDR3_SDRAM_BANK_32:
#endif
#if (SUPPORT_BANK_64 == SUPPORT)
        case MRC_SPD_DDR3_SDRAM_BANK_64:
#endif
#if ((SUPPORT_BANK_8 == SUPPORT) || (SUPPORT_BANK_16 == SUPPORT) || (SUPPORT_BANK_32 == SUPPORT) || (SUPPORT_BANK_64 == SUPPORT))
          DimmOut->Banks      = MRC_BIT3 << BankAddress;;
          DimmOut->BankGroups = BankGroup;
          break;
#endif

        default:
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "%sSDRAM number of banks, %s%Xh\n",
            ErrorString,
            SpdValString,
            BankAddress
            );
          ValidCheck = FALSE;
      }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    (TRUE == ValidCheck) ? "  %u Banks in %u groups\n" : "",
    DimmOut->Banks,
    DimmOut->BankGroups
    );

  return ValidCheck;
}

/**
  @brief
  Determine if the number of ranks in the DIMM is valid and return the value.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE and the value is set to zero.
**/
static
BOOL
GetRankCount (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;
  U8              RankCount;

  Debug       = &MrcData->SysIn.Inputs.Debug;

    RankCount = Spd->Ddr3.General.ModuleOrganization.Bits.RankCount;

  DimmOut->RankInDIMM = RankCount + 1;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Ranks: %u\n", DimmOut->RankInDIMM);
  if (DimmOut->RankInDIMM > MAX_RANK_IN_DIMM) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "%snumber of ranks, %s%Xh\n", ErrorString, SpdValString, RankCount);
    DimmOut->RankInDIMM = 0;
    return FALSE;
  }

  return TRUE;
}

/**
  @brief
  Calculate the size of the DIMM, in MBytes.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval TRUE on valid value, otherwise FALSE and the value is set to zero.
**/
static
BOOL
GetDimmSize (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const U32       SdramCapacityTable[] = {
                  (256 / 8), (512 / 8), (1024 / 8), (2048 / 8),
                  (4096 / 8), (8192 / 8), (16384 / 8), (32768 / 8)
                  };
  U32             DimmSize;
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

  if ((DimmOut->SdramWidth > 0) && (DimmOut->DensityIndex < (sizeof (SdramCapacityTable) / sizeof (SdramCapacityTable[0])))) {
    DimmSize = (((SdramCapacityTable[DimmOut->DensityIndex] * DimmOut->PrimaryBusWidth) / DimmOut->SdramWidth) * DimmOut->RankInDIMM);
    if ((DimmSize >= DIMMSIZEMIN) && (DimmSize <= DIMMSIZEMAX)) {
      DimmOut->DimmCapacity = DimmSize;
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "  DIMM size: %u MByte\n",
        DimmSize
        );
      return TRUE;
    }
  }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_ERROR,
    "%sDIMM size, valid range %u - %u. ",
    ErrorString,
    DIMMSIZEMIN,
    DIMMSIZEMAX
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_ERROR,
    "SDRAM capacity %s%Xh\n",
    SpdValString,
    DimmOut->DensityIndex
    );
  DimmOut->DimmCapacity = 0;
  return FALSE;
}

/**
  @brief
  Obtain ECC support Status for this DIMM.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval Returns TRUE.
**/
static
BOOL
ValidEccSupport (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
#if (SUPPORT_ECC == SUPPORT)
  U8              BusWidthExtension;
#endif // SUPPORT_ECC
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

#if (SUPPORT_ECC == SUPPORT)
    BusWidthExtension = Spd->Ddr3.General.ModuleMemoryBusWidth.Bits.BusWidthExtension;

  if (MRC_SPD_BUS_WIDTH_EXTENSION_8 == BusWidthExtension) {
    DimmOut->EccSupport = TRUE;
  } else
#endif // SUPPORT_ECC
  {
    DimmOut->EccSupport = FALSE;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  ECC is %ssupported\n", (DimmOut->EccSupport == FALSE) ? "not " : "");
  return TRUE;
}

/**
  @brief
  Obtain address mirroring Status for this DIMM.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval Returns TRUE.
**/
static
BOOL
GetAddressMirror (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;
  U8              MappingRank1;

  Debug             = &MrcData->SysIn.Inputs.Debug;

    MappingRank1 = Spd->Ddr3.Module.Unbuffered.AddressMappingEdgeConn.Bits.MappingRank1;
  DimmOut->AddressMirrored = (MappingRank1 != 0) ? TRUE : FALSE;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  DIMM has %saddress mirroring\n", (DimmOut->AddressMirrored == FALSE) ? "no " : "");
  return TRUE;
}

/**
  @brief
  Obtain thermal and refresh support for this DIMM.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval Returns TRUE.
**/
static
BOOL
GetThermalRefreshSupport (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;
  const MrcInput  *Inputs;

  Inputs = &MrcData->SysIn.Inputs;
  Debug  = &Inputs->Debug;
    DimmOut->PartialSelfRefresh    = Spd->Ddr3.General.ThermalAndRefreshOptions.Bits.PartialArraySelfRefresh;
    DimmOut->OnDieThermalSensor    = Spd->Ddr3.General.ThermalAndRefreshOptions.Bits.OnDieThermalSensor;
    DimmOut->AutoSelfRefresh       = Spd->Ddr3.General.ThermalAndRefreshOptions.Bits.AutoSelfRefresh && Inputs->AutoSelfRefreshSupport;
    DimmOut->ExtendedTemperRefresh = Spd->Ddr3.General.ThermalAndRefreshOptions.Bits.ExtendedTemperatureRefreshRate;
    DimmOut->ExtendedTemperRange   = Spd->Ddr3.General.ThermalAndRefreshOptions.Bits.ExtendedTemperatureRange;

  DimmOut->SelfRefreshTemp = ((!DimmOut->AutoSelfRefresh) && (DimmOut->ExtendedTemperRange) && (Inputs->ExtTemperatureSupport)) ? TRUE : FALSE;

  MRC_DEBUG_MSG (Debug,
    MSG_LEVEL_NOTE,
    "  Partial Array Self Refresh%s\n",
    DimmOut->PartialSelfRefresh ? IsSupString : NotSupString);
  MRC_DEBUG_MSG (Debug,
    MSG_LEVEL_NOTE,
    "  On-Die Thermal Sensor Readout%s\n",
    DimmOut->OnDieThermalSensor ? IsSupString : NotSupString);
  MRC_DEBUG_MSG (Debug,
    MSG_LEVEL_NOTE,
    "  Auto Self Refresh%s\n",
    DimmOut->AutoSelfRefresh ? IsSupString : NotSupString);
  MRC_DEBUG_MSG (Debug,
    MSG_LEVEL_NOTE,
    "  Extended Temperature Refresh Rate%s\n",
    DimmOut->ExtendedTemperRefresh ? IsSupString : NotSupString);
  MRC_DEBUG_MSG (Debug,
    MSG_LEVEL_NOTE,
    "  Extended Temperature Range%s\n",
    DimmOut->ExtendedTemperRange ? IsSupString : NotSupString);
  return TRUE;
}

/**
  @brief
  Obtain which JEDEC reference design raw card was used as the basis for the DIMM assembly.

    @param[in, out] MrcData - Pointer to MrcData data structure.
    @param[in]      Spd     - Pointer to Spd data structure.
    @param[in, out] DimmOut - Pointer to structure containing DIMM information.

    @retval Returns TRUE.
**/
static
BOOL
GetReferenceRawCardSupport (
  IN OUT MrcParameters   *const MrcData,
  IN const MrcSpd        *const Spd,
  IN OUT MrcDimmOut      *const DimmOut
  )
{
  const MrcDebug  *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

  DimmOut->ReferenceRawCard = (Spd->Ddr3.Module.Unbuffered.ReferenceRawCardUsed.Bits.Extension << MRC_SPD_REF_RAW_CARD_SIZE) |
    Spd->Ddr3.Module.Unbuffered.ReferenceRawCardUsed.Bits.Card;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "  Reference raw card: %u '%s'\n",
    DimmOut->ReferenceRawCard,
    (DimmOut->ReferenceRawCard < (sizeof (RrcString) / sizeof (RrcString[0][0]))) ?
    RrcString[DimmOut->ReferenceRawCard] : UnknownString
  );
  return TRUE;
}

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
BOOL
GetDimmCrc (
  IN  const U8  *const Buffer,
  IN  const U32        Size,
  OUT U16       *const Crc
  )
{
  const U8  *Data;
  U32       Value;
  U32       Byte;
  U8        Bit;

  Data  = Buffer;
  Value = CRC_SEED;
  for (Byte = 0; Byte < Size; Byte++) {
    Value ^= (U32) *Data++ << 8;
    for (Bit = 0; Bit < 8; Bit++) {
      Value = (Value & MRC_BIT15) ? (Value << 1) ^ CRC_XOR_MASK : Value << 1;
    }
  }

  *Crc = (U16) Value;
  return TRUE;
}

/**
  @brief
  Calculate the medium and fine timebases, using integer math.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if medium timebase is valid, otherwise FALSE.
**/
static
BOOL
GetChannelDimmTimeBase (
  IN OUT MrcParameters  *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
#if ((SUPPORT_XMP == SUPPORT) || (SUPPORT_DDR3 == SUPPORT) || (SUPPORT_LPDDR3 == SUPPORT))
  U8                        SpdMtbDividend;
  U8                        SpdMtbDivisor;
  U8                        SpdFtbDividend;
  U8                        SpdFtbDivisor;
#endif
#if (SUPPORT_XMP == SUPPORT)
  const SPD_MEDIUM_TIMEBASE *XmpMtb;
  U32                       Index;

#endif // SUPPORT_XMP
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", TimeBaseString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd      = &DimmIn->Spd;
            TimeBase = &ChannelOut->TimeBase[Dimm][Profile];
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    SpdFtbDividend = Spd->Ddr3.Xmp.Header.FineTimeBase.Bits.Dividend;
                    SpdFtbDivisor  = Spd->Ddr3.Xmp.Header.FineTimeBase.Bits.Divisor;
                    XmpMtb         = &Spd->Ddr3.Xmp.Header.MediumTimeBase[Index];
                  SpdMtbDividend = XmpMtb->Dividend.Bits.Dividend;
                  SpdMtbDivisor  = XmpMtb->Divisor.Bits.Divisor;
                  TimeBase->Ftb  = ((DimmOut->XmpRevision == 0x12) || (SpdFtbDivisor == 0)) ? 0 : (SpdFtbDividend * MRC_FREQUENCY_FTB_OFFSET) / SpdFtbDivisor;
                  TimeBase->Mtb  = (SpdMtbDivisor == 0) ? 0 : (SpdMtbDividend * MRC_FREQUENCY_MTB_OFFSET) / SpdMtbDivisor;
                } else {
                  TimeBase->Ftb = 0;
                  TimeBase->Mtb = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
              case STD_PROFILE:
              default:
                  SpdFtbDividend  = Spd->Ddr3.General.FineTimebase.Bits.Dividend;
                  SpdFtbDivisor   = Spd->Ddr3.General.FineTimebase.Bits.Divisor;
                  SpdMtbDividend  = Spd->Ddr3.General.MediumTimebase.Dividend.Bits.Dividend;
                  SpdMtbDivisor   = Spd->Ddr3.General.MediumTimebase.Divisor.Bits.Divisor;
                  TimeBase->Ftb = (SpdFtbDivisor == 0) ? 0 : (SpdFtbDividend * MRC_FREQUENCY_FTB_OFFSET) / SpdFtbDivisor;
                  TimeBase->Mtb = (SpdMtbDivisor == 0) ? 0 : (SpdMtbDividend * MRC_FREQUENCY_MTB_OFFSET) / SpdMtbDivisor;
                break;
            } //switch

            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u % 6u %u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              TimeBase->Mtb,
              TimeBase->Ftb
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  return TRUE;
}

/**
  @brief
  Calculate the SDRAM minimum cycle time (tCKmin) that this DIMM supports.
    Then use the lookup table to obtain the frequency closest to the clock multiple.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if the DIMM frequency is supported, otherwise FALSE and the frequency is set to fUnSupport.
**/
static
BOOL
GetChannelDimmtCK (
  IN OUT MrcParameters  *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  S32                   tCKminMtb;
  S32                   tCKminFine;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s (fs)\n", tCKString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = fNoInit;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            Calculated     = 0;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                    Data = &Spd->Ddr3.Xmp.Data[Profile - XMP_PROFILE1];

                  tCKminMtb  = Data->tCKmin.Bits.tCKmin;
                  tCKminFine = (DimmOut->XmpRevision == 0x13) ? Data->tCKminFine.Bits.tCKminFine : 0;
                  Calculated = (MediumTimebase * tCKminMtb) + (FineTimebase * tCKminFine);
                  Calculated = MAX (Outputs->MemoryClockMax, Calculated);
                } else {
                  Calculated = 0;
                }
#endif // SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (Inputs->Ratio > 0) {
                  Calculated  = MrcRatioToClock (Inputs->Ratio, Outputs->RefClk, Inputs->BClkFrequency);
                  Calculated = MAX (Outputs->MemoryClockMax, Calculated);
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                    tCKminMtb   = Spd->Ddr3.General.tCKmin.Bits.tCKmin;
                    tCKminFine  = Spd->Ddr3.General.tCKminFine.Bits.tCKminFine;

                  Calculated = (MediumTimebase * tCKminMtb) + (FineTimebase * tCKminFine);
                  Calculated = MAX (Outputs->MemoryClockMax, Calculated);
                break;
            } //switch
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u % 6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tCK    = Actual[Profile];
          ChannelOut->Timing[Profile].tCK = Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  Outputs->MemoryClock = Actual[Inputs->MemoryProfile];

  return TRUE;
}

/**
  @brief
  Calculate the Minimum CAS Latency Time (tAAmin) for the given DIMMs.
      Step 1: Determine the common set of supported CAS Latency values for all modules
              on the memory channel using the CAS Latencies Supported in SPD.
      Step 2: Determine tAAmin(all) which is the largest tAAmin value for all modules on the memory channel.
      Step 3: Determine tCKmin(all) which is the largest tCKmin value for all
              the modules on the memory channel (Done in function GetChannelDimmtCK).
      Step 4: For a proposed tCK value between tCKmin and tCKmax, determine the desired CAS Latency.
              If tCKproposed is not a standard JEDEC value then tCKproposed must be adjusted to the
              next lower standard tCK value for calculating CLdesired.
      Step 5: Chose an actual CAS Latency that is greater than or equal to CLdesired and is
              supported by all modules on the memory channel as determined in step 1. If no such value exists,
              choose a higher tCKproposed value and repeat steps 4 and 5 until a solution is found.
      Step 6: Once the calculation of CLactual is completed, the BIOS must also verify that this CAS
              Latency value does not exceed tAAmax, which is 20 ns for all DDR3 speed grades.
              If not, choose a lower CL value and repeat steps 5 and 6 until a solution is found.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if the CAS latency has been calculated, otherwise FALSE and the returned value is set to zero.
**/
static
BOOL
GetChannelDimmtAA (
  IN OUT MrcParameters  *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcProfile            Profile;
  MrcBool               Found[MAX_PROFILE];
  MrcBool               UserProfile;
  MrcBool               tCLOverride;
  BOOL                  Status;
  S32                   MediumTimeBase;
  S32                   FineTimeBase;
  S32                   tCKminIndex;
  S32                   tCKmin100;
  S32                   tCKminIndexSave;
  S32                   TimingFTB;
  U32                   TimingMTB;
  U32                   tCKmin;
  U32                   CommonCasMask[MAX_PROFILE];
  U32                   CasMask;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
  U32                   tCLLimitMin;
  U32                   tCLLimitMax;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
  U32                                   Index;
#endif

  Inputs         = &MrcData->SysIn.Inputs;
  Outputs        = &MrcData->SysOut.Outputs;
  Debug          = &Inputs->Debug;
  tCKmin         = 0;
  Calculated     = 0;
  Status         = FALSE;
  tCLOverride    = FALSE;
  MediumTimeBase = 0;
  FineTimeBase   = 0;
  TimingMTB      = 0;
  TimingFTB      = 0;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s tCL Mask\n", tAAString, HeaderString);

  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    UserProfile            = (Profile == USER_PROFILE) && (Inputs->MemoryProfile == USER_PROFILE);
    CommonCasMask[Profile] = ~(0UL);
    Actual[Profile]        = 0;
    tCLLimitMin            = 4;
    tCLLimitMax            = 18;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            tCKmin         = ChannelOut->Dimm[Dimm].Timing[Profile].tCK;
            MediumTimeBase = ChannelOut->TimeBase[Dimm][Profile].Mtb;
            FineTimeBase   = ChannelOut->TimeBase[Dimm][Profile].Ftb;
            CasMask        = 0;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Data        = &Spd->Ddr3.Xmp.Data[Index];
                    tCLLimitMin = 4;
                    tCLLimitMax = 18;
                  TimingMTB       = Data->tAAmin.Bits.tAAmin;
                  TimingFTB       = (DimmOut->XmpRevision == 0x13) ? Data->tAAminFine.Bits.tAAminFine : 0;
                  CasMask         = Data->CasLatencies.Data & MRC_SPD_CL_SUPPORTED_MASK;
                  Calculated      = (tCKmin == 0) ? 0 : ((MediumTimeBase * TimingMTB) + (FineTimeBase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                  Actual[Profile] = MAX (Actual[Profile], Calculated);
                } else {
                  Calculated      = 0;
                }
#endif // SUPPORT_XMP
                break;

              case USER_PROFILE:
                if (DimmIn->Timing.tCL > 0) {
                  CasMask         = ~(0UL);
                  Calculated      = DimmIn->Timing.tCL;
                  Actual[Profile] = MAX (Actual[Profile], Calculated);
                  tCLOverride     = TRUE;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }

              case STD_PROFILE:
              default:
                  tCLLimitMin = 4;
                  tCLLimitMax = 18;
                  TimingMTB   = Spd->Ddr3.General.tAAmin.Bits.tAAmin;
                  TimingFTB   = Spd->Ddr3.General.tAAminFine.Bits.tAAminFine;
                  CasMask     = Spd->Ddr3.General.CasLatencies.Data & MRC_SPD_CL_SUPPORTED_MASK;
                Calculated      = (tCKmin == 0) ? 0 : ((MediumTimeBase * TimingMTB) + (FineTimeBase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                Actual[Profile] = MAX (Actual[Profile], Calculated);
                break;
            } //end switch

            CommonCasMask[Profile] &= CasMask;
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u % 8Xh\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated,
              CasMask
              );
          } //if DimmOut->Status
        } //for Dimm
      } //for Channel
    } //for Controller

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Profile %u common set of supported CAS Latency values = %Xh\n", Profile, CommonCasMask[Profile]);

    if ((Profile >= XMP_PROFILE1) && (tCKmin == 0)) {
      continue;
    }

    Found[Profile] = FALSE;
    ConvertClock2Freq (MrcData, Outputs->RefClk, tCKmin, &tCKminIndex);
    if ((Profile >= XMP_PROFILE1) && (Outputs->RefClk == MRC_REF_CLOCK_133) && (Outputs->Capable100)) {
      ConvertClock2Freq (MrcData, MRC_REF_CLOCK_100, tCKmin, &tCKmin100);
      if (tCKmin100 > tCKminIndex) {
        tCKminIndex = tCKmin100;
        if (Inputs->MemoryProfile == Profile) {
          Outputs->RefClk = MRC_REF_CLOCK_100;
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  Profile%u is RefClk 100 capable, switching to RefClk 100\n", Profile);
      }
    }
    do {
      for (; !Found[Profile] && (Actual[Profile] <= tCLLimitMax); Actual[Profile]++) {
        if ((UserProfile) ||
          ((MRC_BIT0 == ((CommonCasMask[Profile] >> (Actual[Profile] - tCLLimitMin)) & MRC_BIT0)) &&
          ((Actual[Profile] * tCKmin) <= MRC_TaaMAX))) {
            Found[Profile] = TRUE;
            if (Profile == Inputs->MemoryProfile) {
              Outputs->MemoryClock = tCKmin;
              Status = TRUE;
            }
            for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
              ControllerOut = &Outputs->Controller[Controller];
              for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
                ChannelOut = &ControllerOut->Channel[Channel];
                for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
                  DimmOut = &ChannelOut->Dimm[Dimm];
                  if (DIMM_PRESENT == DimmOut->Status) {
                    DimmOut->Timing[Profile].tCL    = (U16) Actual[Profile];
                    ChannelOut->Timing[Profile].tCL = (U16) Actual[Profile];
                    DimmOut->Timing[Profile].tCK    = tCKmin;
                    ChannelOut->Timing[Profile].tCK = tCKmin;
                  } //if
                } //for Dimm
              } //for Channel
            } //for Controller
            break;
        } //if
      } //for Actual[Profile]
      if (!Found[Profile]) {
        if (UserProfile && ((Inputs->Ratio > 0) || (tCLOverride == TRUE))) {
          break;
        } else {
          tCKminIndexSave = tCKminIndex;
          while (--tCKminIndex > 0) {
            if ((Range[tCKminIndex].RefClkFlag == 3) ||
               ((Range[tCKminIndex].RefClkFlag == 1) && (Outputs->RefClk == MRC_REF_CLOCK_133)) ||
               ((Range[tCKminIndex].RefClkFlag == 2) && (Outputs->RefClk == MRC_REF_CLOCK_100))) {
              tCKmin = Range[tCKminIndex].tCK;
              ConvertClock2Freq (MrcData, Outputs->RefClk, tCKmin, &tCKminIndex);
              Actual[Profile] = (tCKmin == 0) ? 0 : ((MediumTimeBase * TimingMTB) + (FineTimeBase * TimingFTB) + (tCKmin - 1)) / tCKmin;
              MRC_DEBUG_MSG (Debug,
                MSG_LEVEL_WARNING,
                "Warning: The memory frequency is being downgraded on profile %u, from %u to %u and the new tCL is %u\n",
                Profile,
                Range[tCKminIndexSave].Frequency,
                Range[tCKminIndex].Frequency,
                Actual[Profile]);
              break;
            }
          }
        }
      }
    } while (!Found[Profile] && (tCKminIndex > 0));
  } //for Profile

  Outputs->Frequency = ConvertClock2Freq (MrcData, Outputs->RefClk, Outputs->MemoryClock, NULL);

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Found[Profile] ? Actual[Profile] : 0);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n  Memory clock = %ufs\n", Outputs->MemoryClock);
#endif

  return (Status);
}

/**
  @brief
  Calculate the minimum tCWL timing value for the given memory frequency.
    We calculate timings for all profiles so that this information can be passed out of MRC.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtCWL (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
  U32                   MaxPossible;
  U32                   TimingMTB;

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &Inputs->Debug;
  MaxPossible = TCWLMAXPOSSIBLE;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tCWLString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            Calculated     = 0;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                      TimingMTB = Spd->Ddr3.Xmp.Data[Profile - XMP_PROFILE1].tCWLmin.Bits.tCWLmin;
                    Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif // SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tCWL > 0) {
                  Calculated = DimmIn->Timing.tCWL;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                TimingMTB = 0;
#if (SUPPORT_LPDDR3 == SUPPORT)
                if (DimmOut->DdrType == MRC_DDR_TYPE_LPDDR3) {
                  //
                  // WL Set A from MR2 spec, adding 1 to take tDQSS into account.
                  // We will subtract this 1 when programming TC_BANK_RANK_D.tWCL
                  //
                  if (tCKmin <= MRC_DDR3_1333_TCK_MIN) {
                    Calculated = 7;
                  } else if (tCKmin <= MRC_DDR3_1067_TCK_MIN) {
                    Calculated = 5;
                  } else if (tCKmin <= MRC_DDR3_800_TCK_MIN) {
                    Calculated = 4;
                  }
                } else
#endif // SUPPORT_LPDDR3
                {
                  if (tCKmin <= MRC_DDR3_2133_TCK_MIN) {
                    Calculated = 10;
                  } else if (tCKmin <= MRC_DDR3_1867_TCK_MIN) {
                    Calculated = 9;
                  } else if (tCKmin <= MRC_DDR3_1600_TCK_MIN) {
                    Calculated = 8;
                  } else if (tCKmin <= MRC_DDR3_1333_TCK_MIN) {
                    Calculated = 7;
                  } else if (tCKmin <= MRC_DDR3_1067_TCK_MIN) {
                    Calculated = 6;
                  } else if (tCKmin <= MRC_DDR3_800_TCK_MIN) {
                    Calculated = 5;
                  }
                }
                break;
            } //switch

            Calculated = MIN (Calculated, MaxPossible);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tCWL    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tCWL = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

#if (MRC_DDR3_LPDDR_SUPPORTED)
/**
  @brief
  Calculate the minimum tFAW timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtFAW (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tFAWString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Data       = &Spd->Ddr3.Xmp.Data[Profile - XMP_PROFILE1];
                  TimingMTB  = ((U32) (Data->tFAWMinUpper.Bits.tFAWminUpper) << 8) | (U32) (Data->tFAWmin.Bits.tFAWmin);
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tFAW > 0) {
                  Calculated = DimmIn->Timing.tFAW;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = ((U32) (Spd->Ddr3.General.tFAWMinUpper.Bits.tFAWminUpper) << 8) | (U32) (Spd->Ddr3.General.tFAWmin.Bits.tFAWmin);
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TFAWMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tFAW    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tFAW = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}
#endif

/**
  @brief
  Calculate the minimum tRAS timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRAS (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
  U32                                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRASString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Data = &Spd->Ddr3.Xmp.Data[Index];
                  TimingMTB = ((U32) (Data->tRASMintRCMinUpper.Bits.tRASminUpper) << 8) | (U32) (Data->tRASmin.Bits.tRASmin);
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRAS > 0) {
                  Calculated = DimmIn->Timing.tRAS;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = ((U32) (Spd->Ddr3.General.tRASMintRCMinUpper.Bits.tRASminUpper) << 8) | (U32) (Spd->Ddr3.General.tRASmin.Bits.tRASmin);
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TRASMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRAS    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRAS = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Calculate the minimum tRC timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRC (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   TimingFTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
  U32                                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRCString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Data = &Spd->Ddr3.Xmp.Data[Index];
                  TimingMTB = ((U32) (Data->tRASMintRCMinUpper.Bits.tRCminUpper) << 8) | (U32) (Data->tRCmin.Bits.tRCmin);
                  TimingFTB = (DimmOut->XmpRevision == 0x13) ? Data->tRCminFine.Bits.tRCminFine : 0;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRC > 0) {
                  Calculated = DimmIn->Timing.tRC;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = ((U32) (Spd->Ddr3.General.tRASMintRCMinUpper.Bits.tRCminUpper) << 8) | (U32) (Spd->Ddr3.General.tRCmin.Bits.tRCmin);
                    TimingFTB = Spd->Ddr3.General.tRCminFine.Bits.tRCminFine;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TRCMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRC    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRC = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Calculate the minimum tRCD timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRCD (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   TimingFTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
  U32                                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRCDString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Data = &Spd->Ddr3.Xmp.Data[Index];
                  TimingMTB = Data->tRCDmin.Bits.tRCDmin;
                  TimingFTB = (DimmOut->XmpRevision == 0x13) ? Data->tRCDminFine.Bits.tRCDminFine : 0;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRCD > 0) {
                  Calculated = DimmIn->Timing.tRCD;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = Spd->Ddr3.General.tRCDmin.Bits.tRCDmin;
                    TimingFTB = Spd->Ddr3.General.tRCDminFine.Bits.tRCDminFine;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TRCDMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRCD    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRCD = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Calculate the minimum tREFI timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtREFI (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
  U32                   TimingMTB;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tREFIString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    TimingMTB = Spd->Ddr3.Xmp.Data[Index].tREFImin.Bits.tREFImin;
                  Calculated = (tCKmin == 0) ? 0 : (U32) (MrcOemMemoryDivideU64ByU64 (
                    ((MrcOemMemoryMultiplyU64ByU32 (MediumTimebase, TimingMTB * TREFIMULTIPLIER)) + (tCKmin - 1)),
                    tCKmin));
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tREFI > 0) {
                  Calculated = DimmIn->Timing.tREFI;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                  switch (DimmOut->DdrType)
                  {
#if (MRC_DDR3_LPDDR_SUPPORTED)
              case MRC_DDR_TYPE_DDR3:
                TimingMTB = TREFIMIN_DDR3;
                break;
#endif
#if (SUPPORT_LPDDR3 == SUPPORT)
              case MRC_DDR_TYPE_LPDDR3:
                TimingMTB = TREFIMIN_LPDDR3;
                break;
#endif
              default:
                TimingMTB = TREFIMIN_DDR3;
                break;
                  }

                  Calculated = (tCKmin == 0) ? 0 : ((TimingMTB + ((tCKmin / 1000) - 1)) / (tCKmin / 1000));
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TREFIMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tREFI    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tREFI = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Calculate the minimum tRFC timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRFC (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRFCString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    TimingMTB = Spd->Ddr3.Xmp.Data[Index].tRFCmin.Bits.tRFCmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRFC > 0) {
                  Calculated = DimmIn->Timing.tRFC;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = Spd->Ddr3.General.tRFCmin.Bits.tRFCmin;
                    //
                    // @todo: Temp w/a for 8GB dimms
                    // if ((DimmOut->DimmCapacity == 8192) && (TimingMTB != 2400)) {
                    //   MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s %u/%u/%u tRFC MTB = %u .., W/A - changing it to 2400\n", CcdString, Controller, Channel, Dimm, TimingMTB);
                    //   TimingMTB = 2400;
                    // }
                    //

                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TRFCMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRFC    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRFC = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}


/**
  @brief
  Calculate the minimum tRP timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRP (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   TimingMTB;
  S32                   TimingFTB;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  U32                                   Index;
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Data;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRPString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Data = &Spd->Ddr3.Xmp.Data[Index];
                  TimingMTB = Data->tRPmin.Bits.tRPmin;
                  TimingFTB = (DimmOut->XmpRevision == 0x13) ? Data->tRPminFine.Bits.tRPminFine : 0;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRP > 0) {
                  Calculated = DimmIn->Timing.tRP;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                    TimingMTB = Spd->Ddr3.General.tRPmin.Bits.tRPmin;
                    TimingFTB = Spd->Ddr3.General.tRPminFine.Bits.tRPminFine;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TRPMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRP    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRP = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

#if (SUPPORT_LPDDR3 == SUPPORT)
/**
  @brief
  Calculate the minimum tRPab timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRPab (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  MrcBool               Flag;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   TimingMTB;
  S32                   TimingFTB;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;
  Flag    = FALSE;

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if ((DIMM_PRESENT == DimmOut->Status) && (DimmOut->DdrType == MRC_DDR_TYPE_LPDDR3)) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case USER_PROFILE:
                if (DimmIn->Timing.tRPab > 0) {
                  Calculated = DimmIn->Timing.tRPab;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              case XMP_PROFILE1:
              case XMP_PROFILE2:
              default:
                if (tCKmin > 0) {
                  TimingMTB  = Spd->Ddr3.General.tRPab.Bits.tRPab;
                  TimingFTB  = Spd->Ddr3.General.tRPabFine.Bits.tRPabFine;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (FineTimebase * TimingFTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            if ((Calculated >= TRPABMINPOSSIBLE) && ((Calculated - DimmOut->Timing[Profile].tRP) <= 3)) {
              Actual[Profile] = MAX (Actual[Profile], Calculated);
            }
            if (!Flag) {
              Flag = TRUE;
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRPabString, HeaderString);
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                "  % 7u % 10u % 8u % 5u %6u\n",
                Profile,
                Controller,
                Channel,
                Dimm,
                Calculated
                );
            } //Flag
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  if (Flag ) {
    //
    // Set the best case timing for all controllers/channels/dimms, for each profile.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
    for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
      if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
        continue;
      }
      for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
        ControllerOut = &Outputs->Controller[Controller];
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          ChannelOut = &ControllerOut->Channel[Channel];
          for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
            DimmOut = &ChannelOut->Dimm[Dimm];
            DimmOut->Timing[Profile].tRPab    = (U16) Actual[Profile];
            ChannelOut->Timing[Profile].tRPab = (U16) Actual[Profile];
          }
        }
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  }

  return TRUE;

}
#endif

#if (MRC_DDR3_LPDDR_SUPPORTED)
/**
  @brief
  Calculate the minimum tRRD timing value for the given memory frequency.
    MRC should not set tRRD below 4nCK for all frequencies.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE
**/
static
BOOL
GetChannelDimmtRRD (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRRDString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                  TimingMTB = Spd->Ddr3.Xmp.Data[Index].tRRDmin.Bits.tRRDmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRRD > 0) {
                  Calculated = DimmIn->Timing.tRRD;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                  TimingMTB = Spd->Ddr3.General.tRRDmin.Bits.tRRDmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MAX (Calculated, TRRDMINPOSSIBLE);  // Make sure tRRD is at least 4 tCK
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRRD    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRRD = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}
#endif


#if (MRC_DDR3_LPDDR_SUPPORTED)
/**
  @brief
  Calculate the minimum tRTP timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtRTP (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
  U32                   MaxPossible;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &Inputs->Debug;
  MaxPossible = TRTPMAXPOSSIBLE;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tRTPString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                  TimingMTB = Spd->Ddr3.Xmp.Data[Index].tRTPmin.Bits.tRTPmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tRTP > 0) {
                  Calculated = DimmIn->Timing.tRTP;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                  TimingMTB = Spd->Ddr3.General.tRTPmin.Bits.tRTPmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, MaxPossible);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tRTP    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tRTP = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}
#endif

#if (MRC_DDR3_LPDDR_SUPPORTED)
/**
  @brief
  Calculate the minimum tWR timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtWR (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
  U32                   MaxPossible;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &Inputs->Debug;
  MaxPossible = TWRMAXPOSSIBLE;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tWRString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                  TimingMTB = Spd->Ddr3.Xmp.Data[Index].tWRmin.Bits.tWRmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tWR > 0) {
                  Calculated = DimmIn->Timing.tWR;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                  TimingMTB = Spd->Ddr3.General.tWRmin.Bits.tWRmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            //
            // Special case, tWRmin values of 9, 11, 13, and 15 are not supported by DDR3 Mode Register 0 (MR0).
            // If we see one of these values, then add one clock to it in order to make it valid.
            //
            if ((9 == Calculated) || (11 == Calculated) || (13 == Calculated) || (15 == Calculated)) {
              Calculated++;
            } else {
              Calculated = MIN (Calculated, MaxPossible);
            }
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tWR    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tWR = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}
#endif

#if (MRC_DDR3_LPDDR_SUPPORTED)
/**
  @brief
  Calculate the minimum tWTR timing value for the given memory frequency.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmtWTR (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  U32                   TimingMTB;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", tWTRString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 0;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                  TimingMTB = Spd->Ddr3.Xmp.Data[Index].tWTRmin.Bits.tWTRmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.tWTR > 0) {
                  Calculated = DimmIn->Timing.tWTR;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                if (tCKmin > 0) {
                  TimingMTB = Spd->Ddr3.General.tWTRmin.Bits.tWTRmin;
                  Calculated = (tCKmin == 0) ? 0 : ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                }
                break;
            } //switch

            Calculated = MIN (Calculated, TWTRMAXPOSSIBLE);
            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].tWTR    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].tWTR = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}
#endif

/**
  @brief
  Calculate the minimum command rate mode value for the given channel.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmNmode (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  const MrcSpd          *Spd;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcTimeBase           *TimeBase;
  MrcProfile            Profile;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   tCKmin;
  S32                   MediumTimebase;
  S32                   FineTimebase;
  U32                   Actual[MAX_PROFILE];
  U32                   Calculated;
#if (SUPPORT_XMP == SUPPORT)
  U32                   Index;
  U32                   TimingMTB;
#endif

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", NmodeString, HeaderString);

  //
  // Find the smallest timing value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = (Profile < XMP_PROFILE1) ? NMODEMINPOSSIBLE : 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd            = &DimmIn->Spd;
            Calculated     = 2;
            tCKmin         = DimmOut->Timing[Profile].tCK;
            TimeBase       = &ChannelOut->TimeBase[Dimm][Profile];
            MediumTimebase = TimeBase->Mtb;
            FineTimebase   = TimeBase->Ftb;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    TimingMTB = Spd->Ddr3.Xmp.Data[Index].SystemCmdRate.Bits.NMode;
                  if (tCKmin == 0) {
                    Calculated = 0;
                  } else {
                    Calculated = ((MediumTimebase * TimingMTB) + (tCKmin - 1)) / tCKmin;
                    if (Calculated == 0) {
                      Calculated = 2;
                    }
                  }
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (DimmIn->Timing.NMode > 0) {
                  Calculated = DimmIn->Timing.NMode;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                Calculated = NMODEMINPOSSIBLE;
                break;
            } //switch

            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %6u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case timing for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          DimmOut->Timing[Profile].NMode    = (U16) Actual[Profile];
          ChannelOut->Timing[Profile].NMode = (U16) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Calculate the VDD voltage value for the given channel.

    @param[in, out] MrcData - Pointer to MrcData data structure.

    @retval TRUE if there are DIMMs present, otherwise FALSE.
**/
static
BOOL
GetChannelDimmVdd (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput                        *Inputs;
  const MrcControllerIn                 *ControllerIn;
  const MrcChannelIn                    *ChannelIn;
  const MrcDimmIn                       *DimmIn;
  const MrcSpd                          *Spd;
  const MrcDebug                        *Debug;
  const SPD_EXTREME_MEMORY_PROFILE_DATA *Xmp;
  MrcOutput                             *Outputs;
  MrcControllerOut                      *ControllerOut;
  MrcChannelOut                         *ChannelOut;
  MrcDimmOut                            *DimmOut;
  MrcProfile                            Profile;
  SpdVddFlag                            VddFlag;
  MrcCpuModel                           CpuModel;
  U32                                   Actual[MAX_PROFILE];
  U32                                   Calculated;
  U8                                    Controller;
  U8                                    Channel;
  U8                                    Dimm;
#if (SUPPORT_XMP == SUPPORT)
  U32                                   Index;
#endif

  Inputs   = &MrcData->SysIn.Inputs;
  Outputs  = &MrcData->SysOut.Outputs;
  Debug    = &Inputs->Debug;
  CpuModel = MrcData->SysIn.Inputs.CpuModel;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %s:\n  %s\n", VddString, HeaderString);

  //
  // Find the best case voltage value for all the given DIMMs, for all the profiles.
  //
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    Actual[Profile] = (Profile < XMP_PROFILE1) ? VDD_1_20 : 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn  = &Inputs->Controller[Controller];
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn   = &ControllerIn->Channel[Channel];
        ChannelOut  = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn  = &ChannelIn->Dimm[Dimm];
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Spd        = &DimmIn->Spd;
            Calculated = VDD_1_50;
            switch (Profile) {
              case XMP_PROFILE1:
              case XMP_PROFILE2:
#if (SUPPORT_XMP == SUPPORT)
                if (((Profile == XMP_PROFILE1) && ((DimmOut->XmpSupport & 1) != 0)) ||
                    ((Profile == XMP_PROFILE2) && ((DimmOut->XmpSupport & 2) != 0))) {
                  Index = Profile - XMP_PROFILE1;
                    Xmp = &Spd->Ddr3.Xmp.Data[Index];
                  Calculated  = XMP_VDD_INCREMENT * Xmp->Vdd.Bits.Decimal;
                  Calculated  = MIN (Calculated, XMP_VDD_INTEGER - 1);
                  Calculated += (XMP_VDD_INTEGER * Xmp->Vdd.Bits.Integer);
                  Calculated  = MAX (Calculated, XMP_VDD_MIN_POSSIBLE);
                  Calculated  = MIN (Calculated, XMP_VDD_MAX_POSSIBLE);
                } else {
                  Calculated = 0;
                }
#endif //SUPPORT_XMP
                break;
              case USER_PROFILE:
                if (Inputs->VddVoltage > 0) {
                  Calculated = Inputs->VddVoltage;
                  break;
                } else {
                  // In AUTO mode, so no break.
                }
              case STD_PROFILE:
              default:
                  VddFlag.Bits.Vdd1_50 = ~(Spd->Ddr3.General.ModuleNominalVoltage.Bits.OperationAt1_50);
                  VddFlag.Bits.Vdd1_35 = Spd->Ddr3.General.ModuleNominalVoltage.Bits.OperationAt1_35;
                  VddFlag.Bits.Vdd1_25 = Spd->Ddr3.General.ModuleNominalVoltage.Bits.OperationAt1_25;
#if (VDDMINPOSSIBLE <= 1350)
                  if ((CpuModel == cmHSW_ULT && PlatformSupport.VddMin.UltSupport <= 1350) ||
                    ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.VddMin.TradSupport <= 1350 )) {
                      if (VddFlag.Bits.Vdd1_35) {
                        Calculated = VDD_1_35;
                      }
                  }
#endif // VDDMINPOSSIBLE
#if (VDDMINPOSSIBLE <= 1200)
                  if ((CpuModel == cmHSW_ULT && PlatformSupport.VddMin.UltSupport <= 1200) ||
                    ((CpuModel == cmHSW || CpuModel == cmCRW) && PlatformSupport.VddMin.TradSupport <= 1200 )) {
                      if (VddFlag.Bits.Vdd1_25) {
                        Calculated = VDD_1_20;
                      }
                  }
#endif // VDDMINPOSSIBLE
                  if ((Profile == STD_PROFILE) && (Inputs->BoardType == btCRBDT)) {
                    Calculated = VDD_1_50;
                  }
                break;
            } //switch

            Actual[Profile] = MAX (Actual[Profile], Calculated);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  % 7u % 10u % 8u % 5u %4u\n",
              Profile,
              Controller,
              Channel,
              Dimm,
              Calculated
            );
          } //DimmOut->Status
        } //Dimm
      } //Channel
    } //Controller
  } //Profile

  //
  // Set the best case voltage for all controllers/channels/dimms, for each profile.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    %s%u:", BestCaseString, MAX_PROFILE - 1);
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    if (((Profile == XMP_PROFILE1) || (Profile == XMP_PROFILE2)) && (!(Outputs->XmpProfileEnable))) {
      continue;
    }
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          Outputs->VddVoltage[Profile] = (MrcVddSelect) Actual[Profile];
          DimmOut->VddVoltage[Profile] = (MrcVddSelect) Actual[Profile];
        }
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %u", Actual[Profile]);
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  return TRUE;
}

/**
  @brief
  Analyze the given DIMM SPD data to determine DIMM presence and configuration.

    @param[in, out] MrcData - Pointer to MRC global data structure.
    @param[in] Controller   - Current controller number.
    @param[in] Channel      - Current channel number.
    @param[in] Dimm         - Current DIMM number.

    @retval mrcSuccess if DIMM is present otherwise mrcDimmNotExist.
**/
static
MrcStatus
SpdDimmRecognition (
  IN OUT MrcParameters *const MrcData,
  IN  const U8                Controller,
  IN  U8                      Channel,
  IN  U8                      Dimm
  )
{
    const SpdRecogCallTable CallTable[] = {
    {ValidDimm},
    {ValidSdramDeviceWidth},
    {ValidPrimaryWidth},
    {GetRankCount},
    {ValidBank},
    {GetDimmSize},
    {ValidRowSize},
    {ValidColumnSize},
    {ValidEccSupport},
    {GetAddressMirror},
    {GetThermalRefreshSupport},
    {GetReferenceRawCardSupport}
  };
  const MrcSpd    *Spd;
  const U8        *CrcStart;
  MrcDimmOut      *DimmOut;
  MrcDimmIn       *DimmIn;
  BOOL            Status;
  U32             CrcSize;
  U8              Index;

  Spd     = &MrcData->SysIn.Inputs.Controller[Controller].Channel[Channel].Dimm[Dimm].Spd;
  DimmIn  = &MrcData->SysIn.Inputs.Controller[Controller].Channel[Channel].Dimm[Dimm];
  DimmOut = &MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm];
  DimmOut->Status = DIMM_NOT_PRESENT;
  if (DIMM_PRESENT == DimmPresence (MrcData, Spd, sizeof (MrcSpd))) {
    Status = TRUE;
    for (Index = 0; (Status == TRUE) && (Index < (sizeof (CallTable) / sizeof (CallTable[0]))); Index++) {
      Status &= CallTable[Index].mrc_task (MrcData, Spd, DimmOut);
    }
    if (Status == FALSE) {
      DimmOut->Status = DIMM_DISABLED;
      return mrcDimmNotExist;
    }
    DimmOut->Status = DIMM_PRESENT;
    CrcStart = MrcSpdCrcArea (MrcData, Controller, Channel, Dimm, &CrcSize);
    GetDimmCrc ((const U8 *const) CrcStart, CrcSize, &DimmOut->Crc);
  } else {
    return mrcDimmNotExist;
  }

  if (DIMM_DISABLED == DimmIn->Status) {
    DimmOut->Status = DIMM_DISABLED;
  }

  return mrcSuccess;
}

/**
  @brief
  Calculate the timing of all DIMMs on all channels.

    @param[in, out] MrcData - The MRC "global data".

    @retval mrcSuccess on success, mrcDimmNotExist if no DIMMs found.
**/
static
MrcStatus
SpdTimingCalculation (
  IN OUT MrcParameters *const MrcData
  )
{
  const SpdTimeCallTable CallTable[] = {
    {GetChannelDimmTimeBase}, // Note: This must be done first as all other calculations are based on this.
    {GetChannelDimmtCK},      // Note: This must be done second as all other calculations are based on this.
    {GetChannelDimmtAA},
    {GetChannelDimmtCWL},
    {GetChannelDimmtRAS},
    {GetChannelDimmtRC},
    {GetChannelDimmtRCD},
    {GetChannelDimmtREFI},
    {GetChannelDimmtRFC},
    {GetChannelDimmtRP},
#if (SUPPORT_LPDDR3 == SUPPORT)
    {GetChannelDimmtRPab},    // Note: This must be done after GetChannelDimmtRP
#endif
#if (MRC_DDR3_LPDDR_SUPPORTED)
    {GetChannelDimmtFAW},
    {GetChannelDimmtRRD},
    {GetChannelDimmtRTP},
    {GetChannelDimmtWR},
    {GetChannelDimmtWTR},
#endif
    {GetChannelDimmNmode},
    {GetChannelDimmVdd}
  };
  BOOL          Status;
  U8            Index;
#if (SUPPORT_FORCE == SUPPORT)
  MrcChannelOut *ChannelOut;
  MrcDimmOut    *DimmOut;
  U16           Value;
  U8            Controller;
  U8            Channel;
  U8            Dimm;
#endif

  //
  // Find the "least common denominator" timing across the DIMMs.
  // tAA must be done first before any other timings are calculated.
  //
  Status = TRUE;
  for (Index = 0; (Status == TRUE) && (Index < (sizeof (CallTable) / sizeof (SpdTimeCallTable))); Index++) {
    Status &= CallTable[Index].mrc_task (MrcData);
  }

#if (SUPPORT_FORCE == SUPPORT)
  if (Status == TRUE) {
    //
    // Force tCLmin, tRCDmin, tRPmin to be the same "least common denominator" value.
    //
    Value = 0;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &Outputs->Controller[Controller].Channel[Channel].Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            Value = MAX (Value, DimmOut->Timing.tRCD);
            Value = MAX (Value, DimmOut->Timing.tRP);
            Value = MAX (Value, DimmOut->Timing.tCL);
          }
        }
      }
    }

    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &Outputs->Controller[Controller].Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DIMM_PRESENT == DimmOut->Status) {
            ChannelOut->Timing.tRCD = Value;
            ChannelOut->Timing.tRP  = Value;
            ChannelOut->Timing.tCL  = Value;
            DimmOut->Timing.tRCD    = Value;
            DimmOut->Timing.tRP     = Value;
            DimmOut->Timing.tCL     = Value;
          }
        }
      }
    }
  }
#endif
  return (Status == FALSE) ? mrcDimmNotExist : mrcSuccess;
}

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
  )
{
  const MrcDimmIn *DimmIn;
  MrcDimmOut      *DimmOut;
  const U8        *CrcStart;

  DimmIn   = &MrcData->SysIn.Inputs.Controller[Controller].Channel[Channel].Dimm[Dimm];
  DimmOut  = &MrcData->SysOut.Outputs.Controller[Controller].Channel[Channel].Dimm[Dimm];
  CrcStart = NULL;

    CrcStart = (const U8 *) &DimmIn->Spd.Ddr3.ModuleId;
    *CrcSize = SPD3_MANUF_SIZE;
  return (CrcStart);
}

/**
  @brief
  Process the SPD information for all DIMMs on all channels.

    @param[in, out] MrcData - The MRC "global data".

    @retval mrcSuccess on success, mrcDimmNotExist if no DIMMs found.
**/
MrcStatus
MrcSpdProcessing (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug        *Debug;
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcDimmIn       *DimmIn;
  MrcStatus             Status;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcDimmOut            *DimmOut;
  MrcModuleType         ModuleType;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U32                   DimmCount;
  U8                    ValidRankBitMask;

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;
  Status  = mrcDimmNotExist;

  //
  // Scan thru each DIMM to see if it is a valid DIMM and to get its configuration.
  //
  ModuleType = MRC_MODULE_TYPE_UNKNOWN;
  DimmCount  = 0;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn  = &Inputs->Controller[Controller];
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn   = &ControllerIn->Channel[Channel];
      ChannelOut  = &ControllerOut->Channel[Channel];
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmIn  = &ChannelIn->Dimm[Dimm];
        DimmOut = &ChannelOut->Dimm[Dimm];
        if (DimmIn->Status == DIMM_ENABLED || DimmIn->Status == DIMM_DISABLED) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "SPD Dimm recognition, %s %u/%u/%u\n",
            CcdString,
            Controller,
            Channel,
            Dimm
            );
          if (mrcSuccess == SpdDimmRecognition (MrcData, Controller, Channel, Dimm)) {
            DimmCount++;
            if (MRC_DDR_TYPE_UNKNOWN == Outputs->DdrType) {
              Outputs->DdrType = DimmOut->DdrType;
            } else if (Outputs->DdrType != DimmOut->DdrType) {
              Status = mrcMixedDimmSystem;
            }
            if (MRC_MODULE_TYPE_UNKNOWN == ModuleType) {
              ModuleType = DimmOut->ModuleType;
            } else if (ModuleType != DimmOut->ModuleType) {
              Status = mrcMixedDimmSystem;
            }
            if (Status == mrcMixedDimmSystem) {
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_ERROR,
                "%s configuration, system contains a mix of memory types\n",
                ErrorString
                );
              return (Status);
            }
          }
        }
      }
    }
  }

  if (DimmCount > 0) {
    //
    // Scan thru each channel to see if it is a valid channel and to get its configuration.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SPD Dimm timing calculation\n");
    if (mrcSuccess == SpdTimingCalculation (MrcData)) {
      Outputs->EccSupport = TRUE;

      //
      // Count up the number of valid DIMMs.
      //
      ControllerOut = &Outputs->Controller[0];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut              = &ControllerOut->Channel[Channel];
        ChannelOut->EccSupport  = TRUE;
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          if ((DIMM_PRESENT == DimmOut->Status) || (DIMM_DISABLED == DimmOut->Status)) {
            ChannelOut->DimmCount++;
          }
          if (DIMM_PRESENT == DimmOut->Status) {
#if (MAX_RANK_IN_CHANNEL > 8)
#error The next switch statement and ValidRankBitMask needs updated to support additional ranks.
#endif
            switch (DimmOut->RankInDIMM) {
              case 1:
                ValidRankBitMask = 1;
                break;
#if (MAX_RANK_IN_DIMM > 1)

              case 2:
                ValidRankBitMask = 3;
                break;
#endif
#if (MAX_RANK_IN_DIMM > 2)

              case 3:
                ValidRankBitMask = 7;
                break;
#endif
#if (MAX_RANK_IN_DIMM > 3)

              case 4:
                ValidRankBitMask = 15;
                break;
#endif

              default:
                ValidRankBitMask = 0;
                break;
            }

            ChannelOut->ValidRankBitMask |= ValidRankBitMask << (Dimm * MAX_RANK_IN_DIMM);

            ChannelOut->EccSupport &= DimmOut->EccSupport;
            Outputs->EccSupport &= DimmOut->EccSupport;
          }
        }

        if ((ChannelOut->DimmCount > 0) && (ChannelOut->ValidRankBitMask > 0)) {
          ControllerOut->ChannelCount++;
          ControllerOut->Channel[Channel].Status = CHANNEL_PRESENT;
        }
      }

      for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
        ControllerOut = &Outputs->Controller[Controller];
        if (ControllerOut->ChannelCount > 0) {
          ControllerOut->Status = CONTROLLER_PRESENT;
          Status                = mrcSuccess;
        }
      }
    }
  }

  return Status;
}
