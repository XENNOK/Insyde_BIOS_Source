/** @file
  This code fills in standard CMOS values and updates the standard CMOS
  checksum. The Legacy16 code or LegacyBiosPlatform.c is responsible for
  non-standard CMOS locations and non-standard checksums.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosInterface.h"

/**
  Read CMOS register through index/data port.

  @param[in]  Index   The index of the CMOS register to read.

  @return  The data value from the CMOS register specified by Index.

**/
UINT8
LegacyReadStandardCmos (
  IN UINT8  Index
  )
{
  return  ReadCmos8 (Index);
}

/**
  Write CMOS register through index/data port.

  @param[in]  Index  The index of the CMOS register to write.
  @param[in]  Value  The value of CMOS register to write.

  @return  The value written to the CMOS register specified by Index.

**/
VOID
LegacyWriteStandardCmos (
  IN UINT8  Index,
  IN UINT8  Value
  )
{
  WriteCmos8 (Index, Value);
}

/**
  Calculate the new standard CMOS checksum and write it.

  @param  Private      Legacy BIOS Instance data

  @retval EFI_SUCCESS  Calculate 16-bit checksum successfully

**/
EFI_STATUS
LegacyCalculateWriteStandardCmosChecksum (
  VOID
  )
{
  UINT8   Register;
  UINT16  Checksum;

  for (Checksum = 0, Register = 0x10; Register < 0x2e; Register++) {
    Checksum = (UINT16)(Checksum + LegacyReadStandardCmos (Register));
  }
  LegacyWriteStandardCmos (CMOS_2E, (UINT8)(Checksum >> 8));
  LegacyWriteStandardCmos (CMOS_2F, (UINT8)(Checksum & 0xff));
  return EFI_SUCCESS;
}


/**
  Fill in the standard CMOS stuff before Legacy16 load

  @param  Private      Legacy BIOS Instance data

  @retval EFI_SUCCESS  It should always work.

**/
EFI_STATUS
LegacyBiosInitCmos (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
{
  UINT32  Size;
  UINT8   Temp;
  UINT8   Temp1;

  //
  //  Clear all errors except RTC lost power
  //
  LegacyWriteStandardCmos (CMOS_0E, (UINT8)(LegacyReadStandardCmos (CMOS_0E) & BIT7));

  //
  // Update CMOS locations 15,16,17,18,30,31 and 32
  // CMOS 16,15 = 640Kb = 0x280
  // CMOS 18,17 = 31,30 = 15Mb max in 1Kb increments =0x3C00 max
  // CMOS 32 = 0x20
  //
  LegacyWriteStandardCmos (CMOS_15, 0x80);
  LegacyWriteStandardCmos (CMOS_16, 0x02);

  Size = 63 * SIZE_1MB;
  Temp  = 0x00;
  Temp1 = 0xFC;
  if (Private->IntThunk->EfiToLegacy16InitTable.OsMemoryAbove1Mb < (63 * SIZE_1MB)) {
    Size  = Private->IntThunk->EfiToLegacy16InitTable.OsMemoryAbove1Mb >> 10;
    Temp      = (UINT8) (Size & 0xFF);
    Temp1     = (UINT8) (Size >> 8);
  }

  LegacyWriteStandardCmos (CMOS_17, Temp);
  LegacyWriteStandardCmos (CMOS_30, Temp);
  LegacyWriteStandardCmos (CMOS_18, Temp1);
  LegacyWriteStandardCmos (CMOS_31, Temp1);

  LegacyCalculateWriteStandardCmosChecksum ();

  return EFI_SUCCESS;
}
