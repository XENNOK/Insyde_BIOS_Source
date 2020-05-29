/** @file

  Server Platform Services(SPS) ME Firmware Upgrade SMM driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_UPGRADE_SMM_H_
#define _SPS_ME_UPGRADE_SMM_H_

#include <SpsMe.h>

#include <SpsMeUpgradeCommon.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/SpsMeLib.h>
#include <Library/SpsMeFwUpgradeLib.h>

#include <Protocol/SmmBase.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmFwBlockService.h>

#pragma pack()

#define SPS_ME_UPGRADE_SPI_ERASE_SIZE_MAX        0x1000
#define SPS_ME_UPGRADE_SPI_WRITE_SIZE_MAX        0x1000

typedef
EFI_STATUS
( *SPS_ME_UPGRADE_PROGRESS_FUNCTION ) (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

typedef struct _SPS_ME_UPGRADE_PROGRESS_TABLE {
  UINT32                               TableIndex;
  SPS_ME_UPGRADE_PROGRESS_FUNCTION     Function;
} SPS_ME_UPGRADE_PROGRESS_TABLE;

/**
  Send HMRFPO_ENABLE message to ME to open ME region

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval EFI_ACCESS_DENIED       Enable ME fail or ME in disable state.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEnableMeRegion (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Backup ME factory default data.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressInitialize (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Check New image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressCheckNewImage (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase FPT sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseFPT (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase MFSB sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseMFSB (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase SCA sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseSCA (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase FTPR sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseFTPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase MFS sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseMFS (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase OPR1 sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase OPR2 sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New FTPR to OPR1.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPRtoOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New FTPR to OPR2.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPRtoOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New FPT.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFPT (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New MFSB.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewMFSB (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New SCA.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewSCA (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New FTPR.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New MFS.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewMFS (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New OPR1.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New OPR2.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Verify New image by HMRFPO_LOCK message.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressVerifyNewImage (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Nodify Upgrade is done by CMOS.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressNotifyByCmos (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Check which OPR is inactice operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressCheckInavtiveOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Erase inactive Operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseInavtiveOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Write New OPR to inactive Operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  );

/**
  Get SPS ME protect ROM map

  @param[in]      This            Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[out]     ProtectRomMap   Pointer of protect ROM map output buffer.
  @param[in, out] NumberOfProtectRegions  # of protect ROM map ouput buffer as input parameter;
                                          physical # of protect ROM map as output parameter.

  @retval EFI_INVALID_PARAMETER   This or NumberOfProtectRegions parameter is NULL pointer
  @retval EFI_BUFFER_TOO_SMALL    Size of protect ROM map buffer less than physical size
  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GetProtectRomMap (
  IN     SPS_ME_UPGRADE_PROTOCOL       *This,
  OUT    SPS_ME_PROTECT_ROM_MAP        *ProtectRomMap, OPTIONAL
  IN OUT UINTN                         *NumberOfProtectRegions
  );

/**
  Distinguish does the target linear address within ME region. If true, then perform the
  SPS ME firmware updating procedures.
  This function must be invoked before writing data into ME region of flash device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] WriteTargetAddr      Target linear address for writing flash device.
  @param[in] WriteTargetSize      Target size for writing flash device.

  @retval others                  Perform SPS ME firmware updating procedures failed.
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
UpgradeProgress (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             WriteTargetAddr,
  IN UINTN                             WriteTargetSize
  );

/**
  Perform the SPS ME firmware updating procedures after writing data to ME region of flash
  device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer.
  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Perform SPS ME firmware updating procedures failed.
**/
EFI_STATUS
EFIAPI
UpgradeComplete (
  IN SPS_ME_UPGRADE_PROTOCOL           *This
  );

#pragma pack()

#endif // _SPS_ME_UPGRADE_H_
