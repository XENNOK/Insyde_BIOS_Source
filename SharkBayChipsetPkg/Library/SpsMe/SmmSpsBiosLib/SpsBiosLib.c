/** @file

  Server Platform Services SMM Library For BIOS

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

#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <Library/FdSupportLib.h>
#include <Protocol/SpsMeUpgrade.h>

SPS_ME_UPGRADE_PROTOCOL                *mSpsMeUpgrade = NULL;

/**
  Retrieves the Smm SPS ME upgrade protocol from the SMST handle database.
**/
EFI_STATUS
GetSpsMeUpgradeProtocol (
  VOID
  )
{
  EFI_STATUS            Status;

  if (mSpsMeUpgrade != NULL) {
    return EFI_SUCCESS;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gSpsSmmMeUpgradeProtocolGuid,
                    NULL,
                    (VOID **)&mSpsMeUpgrade
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ( (DEBUG_INFO, "[SMM SPS BIOS LIB] INFO: Locate SMM SPS ME Upgrade Protocol => (%r)\n", Status));
    return Status;
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
  EFI_STATUS                           Status;
  UINTN                                TempNumberOfProtectRegion;
  SPS_ME_PROTECT_ROM_MAP               *TempProtectRomMap;

//[-start-131206-IB10310041-modify]//
  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_UNSUPPORTED;
  }
//[-end-131206-IB10310041-modify]//

  if ((ProtectRomMap == NULL) || (NumberOfProtectRegions == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempNumberOfProtectRegion = 0;
  TempProtectRomMap         = NULL;

  ///
  /// Get ME protect ROM map from SPS_ME_UPGRADE_PROTOCOL
  ///
  Status = GetSpsMeUpgradeProtocol ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSpsMeUpgrade->GetProtectRomMap (mSpsMeUpgrade, NULL, &TempNumberOfProtectRegion);
  if ((!EFI_ERROR (Status)) && (TempNumberOfProtectRegion != 0)) {
    Status = gSmst->SmmAllocatePool (
                EfiRuntimeServicesData,
                (TempNumberOfProtectRegion * sizeof (SPS_ME_PROTECT_ROM_MAP)),
                (VOID **)&TempProtectRomMap
                );
    if (EFI_ERROR (Status)) {
      DEBUG ( (DEBUG_ERROR, "[SMM SPS BIOS LIB] ERROR: gSmst->SmmAllocatePool (%r)\n", Status));
      TempProtectRomMap = NULL;
    }
  }

  if (TempProtectRomMap != NULL) {
    Status = mSpsMeUpgrade->GetProtectRomMap (mSpsMeUpgrade, TempProtectRomMap, &TempNumberOfProtectRegion);
    if (EFI_ERROR (Status)) {
      DEBUG ( (DEBUG_ERROR, "[SMM SPS BIOS LIB] ERROR: GetProtectRomMap %r\n", Status));
      return Status;
    }
  }

  *NumberOfProtectRegions = TempNumberOfProtectRegion;
  *ProtectRomMap           = TempProtectRomMap;

  return EFI_SUCCESS;
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
  EFI_STATUS            Status;

  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_UNSUPPORTED;
  }

  Status = GetSpsMeUpgradeProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  Status = mSpsMeUpgrade->UpgradeComplete (mSpsMeUpgrade);
  if (EFI_ERROR (Status)) {
  }


  return Status;
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
  EFI_STATUS            Status;

  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_UNSUPPORTED;
  }

  Status = GetSpsMeUpgradeProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }
  ///
  /// Update procedures for SPS ME firmware
  ///
  Status = mSpsMeUpgrade->UpgradeProgress (
                            mSpsMeUpgrade,
                            RomBaseAddress,
                            WriteSize
                            );
  if (EFI_ERROR (Status)) {
  }

  return Status;
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
  EFI_STATUS            Status;

  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_UNSUPPORTED;
  }

  if ((NewImageSource == NULL) || (NewImageSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSpsMeUpgradeProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  Status = mSpsMeUpgrade->PassImage (mSpsMeUpgrade, NewImageSource, NewImageSize);
  if (EFI_ERROR (Status)) {
  }

  return Status;
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
  EFI_STATUS            Status;

  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_UNSUPPORTED;
  }

  Status = GetSpsMeUpgradeProtocol ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  ///
  /// Setting SPS ME Upgrade Mode.
  ///
  Status =  mSpsMeUpgrade->SetMode (mSpsMeUpgrade, UpgradeMode);
  if (EFI_ERROR (Status)) {
  }

  return Status;
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
  FLASH_REGION               *FlashRegionPtr;

  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return EFI_SUCCESS;
  }

  if (Table == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FlashRegionPtr = (FLASH_REGION *) Table;

  ///
  /// Override the ME region access policy to available if SPS ME Upgrade
  /// Protocol is presented
  ///
  if ((FlashRegionPtr[ME_REGION].Type == ME_REGION) &&
      (FlashRegionPtr[ME_REGION].Access == ACCESS_NOT_AVAILABLE)
      ) {
    FlashRegionPtr[ME_REGION].Access = ACCESS_AVAILABLE;
  }
  ASSERT (FlashRegionPtr[ME_REGION].Type == ME_REGION);

  return EFI_SUCCESS;
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
  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return Status;
  }

  if (IhisiStatus == NULL) {
    return Status;
  }

  *IhisiStatus = 0;
  Status = EFI_SUCCESS;

  return Status;
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
  return EFI_UNSUPPORTED;
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
  return EFI_UNSUPPORTED;
}
