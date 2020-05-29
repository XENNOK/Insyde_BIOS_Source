/** @file
  Implements Platform Trust Technology (FTPM) HECI SkuMgr Interface Library.

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
  license agreement

**/

#ifdef PTT_FLAG
#include "PttHeciLib.h"
#include <HeciMsgLib.h>

#define PTT_BITMASK   0x20000000 //BIT29

#define CLEAR_PTT_BIT 0x00000000

/**
  Checks whether ME FW has the Platform Trust Technology capability.

  @param[out] PttCapability      TRUE if PTT is supported, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciGetCapability(
  OUT BOOLEAN   *PttCapability
  )
{
  EFI_STATUS                Status;
  GEN_GET_FW_CAPSKU         MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK   MsgGenGetFwCapsSkuAck;

  *PttCapability = FALSE;

  Status = HeciGetFwCapsSkuMsg (&MsgGenGetFwCapsSku, &MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Command) == FWCAPS_GET_RULE_CMD) &&
      ((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.IsResponse) == 1) &&
      (MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Result == 0)
      ) {

      if (MsgGenGetFwCapsSkuAck.Data.FWCapSku.Fields.PTT) {
        *PttCapability = TRUE;
      }
  }
  DEBUG ((EFI_D_INFO, "PTT SkuMgr: PttCapability = %d\n", *PttCapability));

  return EFI_SUCCESS;
}

/**
  Checks Platform Trust Technology enablement state.

  @param[out] IsPttEnabledState  TRUE if PTT is enabled, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciGetState(
  OUT BOOLEAN   *IsPttEnabledState
  )
{
  EFI_STATUS      Status;
  MEFWCAPS_SKU    CurrentFeatures;

  *IsPttEnabledState = FALSE;

  Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CurrentFeatures.Fields.PTT) {
    *IsPttEnabledState = TRUE;
  }

  DEBUG ((EFI_D_INFO, "PTT SkuMgr: PttState = %d\n", *IsPttEnabledState));

  return EFI_SUCCESS;
}

/**
  Changes current Platform Trust Technology state.

  @param[in] PttEnabledState     TRUE to enable, FALSE to disable.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciSetState(
  IN  BOOLEAN   PttEnabledState
  )
{
  EFI_STATUS      Status;
  UINT32          EnableBitmap;
  UINT32          DisableBitmap;

  if (PttEnabledState) {
    //
    // Enable PTT
    //
    DEBUG ((EFI_D_INFO, "PTT SkuMgr: Enable PTT\n"));
    EnableBitmap  = PTT_BITMASK;
    DisableBitmap = CLEAR_PTT_BIT;
  } else {
    //
    // Disable PTT
    //
    DEBUG ((EFI_D_INFO, "PTT SkuMgr: Disable PTT\n"));
    EnableBitmap  = CLEAR_PTT_BIT;
    DisableBitmap = PTT_BITMASK;
  }
  Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);

  return Status;
}

#endif // PTT_FLAG
