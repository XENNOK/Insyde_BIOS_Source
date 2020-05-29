/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
Copyright (c)  2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  PcieTrainingEqSettings.c

@brief
  This file adds equalization setting support.

--*/

#include "PcieTraining.h"
#include "PciExpressInit.h"

#ifdef PEG_FLAG

VOID
GetLinkPartnerFullSwing (
  IN  UINT8 Lane,
  OUT UINT8 *FullSwing
  )
{
  UINT32  Data32;

  Data32 = BIT25 | BIT23 | (Lane << 19) | BIT18;
  McD1PciCfg32(R_SA_PEG_EQPH3_OFFSET) = Data32;
  Data32 = McD1PciCfg32(R_SA_PEG_EQPH3_OFFSET);
  McD1PciCfg32(R_SA_PEG_EQPH3_OFFSET) = 0;

  *FullSwing = (Data32 >> 6) & 0x3F;

  return;
}

VOID
GetCoefficientsFromPreset (
  IN  UINT8 Preset,
  IN  UINT8 FullSwing,
  OUT UINT8 *PreCursor,
  OUT UINT8 *Cursor,
  OUT UINT8 *PostCursor
  )
{
  INT32   PreCursorMilli;
  INT32   PostCursorMilli;

  PreCursorMilli  = 0;
  PostCursorMilli = 0;

  ///
  /// Get starting values from Table 4-16 of the PCIe Base Spec v3.0
  ///
  switch (Preset) {
    case  0:
      PreCursorMilli  =    0;
      PostCursorMilli = -250;
      break;

    case  1:
      PreCursorMilli  =    0;
      PostCursorMilli = -167;
      break;

    case  2:
      PreCursorMilli  =    0;
      PostCursorMilli = -200;
      break;

    case  3:
      PreCursorMilli  =    0;
      PostCursorMilli = -125;
      break;

    case  4:
      PreCursorMilli  =    0;
      PostCursorMilli =    0;
      break;

    case  5:
      PreCursorMilli  = -100;
      PostCursorMilli =    0;
      break;

    case  6:
      PreCursorMilli  = -125;
      PostCursorMilli =    0;
      break;

    case  7:
      PreCursorMilli  = -100;
      PostCursorMilli = -200;
      break;

    case  8:
      PreCursorMilli  = -125;
      PostCursorMilli = -125;
      break;

    case  9:
      PreCursorMilli  = -166;
      PostCursorMilli =    0;
      break;

    case 10:  ///< P10 is unsupported
    default:
      PreCursorMilli  = -100;
      PostCursorMilli = -200;
      DEBUG ((EFI_D_WARN, "GetCoefficientsFromPreset(): Unsupported Preset Requested: P%d. Using P7.\n", Preset));
      break;
  }

  ///
  /// Convert to absolute values
  ///
  if (PreCursorMilli < 0) {
    PreCursorMilli *= -1;
  }
  if (PostCursorMilli < 0) {
    PostCursorMilli *= -1;
  }

  ///
  /// Apply FullSwing
  ///
  PreCursorMilli  *= FullSwing;
  PostCursorMilli *= FullSwing;

  ///
  /// Convert to integers
  ///
  *PreCursor  = (( PreCursorMilli % 1000) >= 500) ? (UINT8) (( PreCursorMilli / 1000) + 1) : (UINT8) ( PreCursorMilli / 1000);
  *PostCursor = ((PostCursorMilli % 1000) >= 500) ? (UINT8) ((PostCursorMilli / 1000) + 1) : (UINT8) (PostCursorMilli / 1000);
  *Cursor     = FullSwing - (*PreCursor) - (*PostCursor);

  return;
}

VOID
SetPartnerTxCoefficients (
  IN  UINT8 Lane,
  IN  UINT8 *PreCursor,
  IN  UINT8 *Cursor,
  IN  UINT8 *PostCursor
  )
{
  UINT32  Data32;

  Data32 = (Lane << 19) | BIT18 | (*Cursor << 12) | (*PreCursor << 6) | (*PostCursor);
  McD1PciCfg32(R_SA_PEG_EQPH3_OFFSET) = Data32;
  McD1PciCfg32(R_SA_PEG_EQPH3_OFFSET) = 0;

  return;
}

#endif // PEG_FLAG
