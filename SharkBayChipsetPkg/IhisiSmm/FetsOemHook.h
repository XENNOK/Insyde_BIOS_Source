/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IHISI_FETS_OEM_HOOK_H_
#define _IHISI_FETS_OEM_HOOK_H_

#include "IhisiSmm.h"
#include "FbtsOemHook.h"
#include "Fets.h"

#define EC_PART_SIZE                 EC256K

#define RST_CNT                      0xCF9
#define V_RST_CNT_FULLRESET          0x0E

typedef struct {
  UINT8         SizeIndex;
  UINT32        Size;
} FLASH_DEVICE_SIZE_MAP_TABLE;
//[-start-130125-IB10820224-remove]//
#if 0

/**
  Get EC part information hook function.

  @retval EFI_SUCCESS    Successfully returns.
**/
EFI_STATUS
FetsGetPartInfoHook (
  VOID
  );
#endif
//[-end-130125-IB10820224-remove]//

/**
  Hook function before flashing EC part.

  @param[in, out] FlashingDataBuffer Double pointer to data buffer.
  @param[in, out] SizeToFlash        Data size by bytes want to flash.
  @param[in, out] DestBlockNo        Dsstination block number.

  @retval EFI_SUCCESS                Successfully returns.
**/
EFI_STATUS
FetsDoBeforeFlashing (
  IN OUT  UINT8        **FlashingDataBuffer,
  IN OUT  UINTN        *SizeToFlash,
  IN OUT  UINT8        *DestBlockNo
  );

/**
  Hook function after flashing EC part.

  @param[in] ActionAfterFlashing Input action flag.

  @retval EFI_SUCCESS            Successfully returns.
**/
EFI_STATUS
FetsDoAfterFlashing (
  IN      UINT8        ActionAfterFlashing
  );

/**
  Fets reset system function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsReboot (
  VOID
  );

/**
  Fets shutdown function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsShutdown (
  VOID
  );

/**
  Fets EC idle function

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsEcIdle (
  IN      BOOLEAN        Idle
  );

/**
  Function uses to flash EC part.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
FetsEcFlash (
  IN      UINT8        *FlashingDataBuffer,
  IN      UINTN        SizeToFlash,
  IN      UINT8        DestBlockNo
  );

#endif
