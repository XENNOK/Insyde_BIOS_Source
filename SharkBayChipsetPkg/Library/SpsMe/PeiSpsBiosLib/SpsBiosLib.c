/** @file

  Server Platform Services PEI Library For BIOS

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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>

#include <Guid/SpsInfoHob.h>

/**
  Retrieves the Pei HECI Ppi for internal ues.
**/
EFI_STATUS
SpsInternalGetSpsInfoHob (
  IN OUT SPS_INFO_HOB                  **SpsInfoHob
  )
{
  EFI_STATUS            Status;
  EFI_HOB_GUID_TYPE     *GuidHob;
  SPS_INFO_HOB          *TempSpsInfoHob;

  Status = EFI_UNSUPPORTED;
  if (SpsInfoHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  TempSpsInfoHob = NULL;

  GuidHob = GetFirstGuidHob (&gSpsInfoHobGuid);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_INFO, "[PEI SPS BIOS LIB] INFO: SPS INFO HOB not found.\n"));
    return EFI_NOT_FOUND;
  }

  TempSpsInfoHob = (SPS_INFO_HOB *) GET_GUID_HOB_DATA (GuidHob);

  *SpsInfoHob = TempSpsInfoHob;
  if ((TempSpsInfoHob == NULL) || (*SpsInfoHob == NULL)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS HECI LIB] ERROR: Get SpsInfoHob Success but address invaild (%x)\n", *SpsInfoHob));
    return EFI_NOT_STARTED;
  }
  return EFI_SUCCESS;
}

/**
  Get SPS ME Firmware version.

  @param[out] FwVerMinor          Active firmware version minor number.
  @param[out] FwVerMajor          Active firmware version major number.
  @param[out] FwVerBuild          Active firmware version build number.
  @param[out] FwVerPatch          Active firmware version patch number.
  @param[out] FwVerSku            Active firmware SKU.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsBiosGetFwVersion (
  OUT UINT16                           *FwVerMinor,
  OUT UINT16                           *FwVerMajor,
  OUT UINT16                           *FwVerBuild,
  OUT UINT16                           *FwVerPatch,
  OUT UINT16                           *FwVerSku
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get protect rom map override by SPS ME deriver.

  @param[out] ProtectRomMap          Protect Rom Map Buffer.
  @param[out] NumberOfProtectRegions Number of Protect Region.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsGetProtectRomMap (
  OUT VOID                             **ProtectRomMap,
  OUT UINTN                            *NumberOfProtectRegions
  )
{
  return EFI_UNSUPPORTED;
}

/**
  SPS Upgrade Complete.

  @param None

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsUpgradeComplete (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Perform SPS Upgrade progress function.

  @param[in] RomBaseAddress       Target linear address for writing flash device.
  @param[in] WriteSize            Target size for writing flash device.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_WRITE_PROTECTED     Skip this block.
  @retval EFI_ABORTED             Upgrade fail.
  @retval EFI_NOT_READY           Upgrade is running and not complete.
  @retval others                  Perform SPS ME firmware updating procedures failed.
**/
EFI_STATUS
EFIAPI
SpsUpgradeProgress (
  IN UINTN                             RomBaseAddress,
  IN UINTN                             WriteSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
  SPS Upgrade pass image to upgrade driver.

  @param[in] NewImageSource       Pointer of New image.
  @param[in] NewImageSize         New image size.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsUpgradePassImage (
  IN VOID                              *NewImageSource,
  IN UINTN                             NewImageSize
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Setting SPS ME Upgrade Mode.

  @param[in] UpgradeMode          SPS ME Upgrade mode.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval others                  Setting SPS ME firmware updating mode failed.
**/
EFI_STATUS
EFIAPI
SpsUpgradeSetMode (
  IN UINTN                             UpgradeMode
  )
{
  return EFI_UNSUPPORTED;
}

/**
  SPS Upgrade Platform table Hook.

  @param[in] Table                Flash protect region map.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsPlatformTableHook (
  IN OUT UINT8                         *Table
  )
{
  return EFI_UNSUPPORTED;
}

/**
  SPS Upgrade FBTS Read hook.

  @param[in] Status               Flash protect region map.
  @param[in] IhisiStatus          Flash protect region map.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsUpgradeFbtsReadHook (
  IN     UINTN                         Status,
  IN OUT UINT32                        *IhisiStatus
  )
{
  return EFI_UNSUPPORTED;
}

///
/// SPS Node Manager
///
/**
  SPS Get booting mode from NM.

  @param[out] CoreDisable         Number of Core disabled.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsGetBootingMode (
  OUT UINT8                            *BootingMode
  )
{
  EFI_STATUS            Status;
  SPS_INFO_HOB          *SpsInfoHob;

  Status = EFI_UNSUPPORTED;
  if (BootingMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsInternalGetSpsInfoHob (&SpsInfoHob);
  if ((EFI_ERROR (Status)) || (SpsInfoHob == NULL)) {
    return EFI_NOT_STARTED;
  }

  if (SpsInfoHob->SpsMeSku != SPS_SKU_NM) {
    return EFI_UNSUPPORTED;
  }

  *BootingMode = SpsInfoHob->BootingMode;

  return Status;
}

/**
  SPS Get number of Cores disabled from NM.

  @param[out] CoreDisable         Number of Core disabled.

  @retval EFI_SUCCESS             Function normally.
**/
EFI_STATUS
EFIAPI
SpsGetCoresDisable (
  OUT UINT8                            *CoreDisable
  )
{
  EFI_STATUS            Status;
  SPS_INFO_HOB          *SpsInfoHob;

  Status = EFI_UNSUPPORTED;
  if (CoreDisable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsInternalGetSpsInfoHob (&SpsInfoHob);
  if ((EFI_ERROR (Status)) || (SpsInfoHob == NULL)) {
    return EFI_NOT_STARTED;
  }

  if (SpsInfoHob->SpsMeSku != SPS_SKU_NM) {
    return EFI_UNSUPPORTED;
  }

  *CoreDisable = SpsInfoHob->CoreDisable;

  return Status;
}
