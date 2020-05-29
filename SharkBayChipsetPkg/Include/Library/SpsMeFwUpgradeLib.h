/** @file

  Header file for SPS ME Firmware upgrade functionality
  Note: Only for SPS.

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

#ifndef _SPS_ME_UPGRADE_LIB_H_
#define _SPS_ME_UPGRADE_LIB_H_

///
/// SPS ME Firmware upgrade LIB
///
/**
  Enable HECI device

  @param None

  @retval EFI_SUCCESS             Always return Successfully.
**/
EFI_STATUS
SpsUpgradeEnableHeciDevice (
  VOID
  );

/**
  Get ROM linear base address of system from descriptor region

  @param[out] RomBaseAddress      The pointer of ROM linear base address

  @retval EFI_SUCCESS             Get Rom Base Address Successfully.
**/
EFI_STATUS
SpsUpgradeGetRomBaseAddress (
  OUT UINTN             *RomBaseAddress
  );

/**
  Get descriptor region base address and limit from SPI.

  @param[out] Base                The descriptor region base address.
  @param[out] Limit               The descriptor region limit.

  @retval EFI_SUCCESS             Get descriptor region Successfully.
**/
EFI_STATUS
SpsUpgradeGetDescRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get ME region base address and limit from SPI.

  @param[out] Base                The ME region base address.
  @param[out] Limit               The ME region limit.

  @retval EFI_SUCCESS             Get ME region Successfully.
**/
EFI_STATUS
SpsUpgradeGetMeRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get GBE region base address and limit from SPI.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region limit.

  @retval EFI_SUCCESS             Get GBE region Successfully.
**/
EFI_STATUS
SpsUpgradeGetGBERegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get Platform Data Regione(PDR) region base address and limit from SPI.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region limit.

  @retval EFI_SUCCESS             Get Platform Data Regione(PDR) region Successfully.
**/
EFI_STATUS
SpsUpgradeGetPDRRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Get Device Expansion Region(DER) region base address and limit from SPI.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region limit.

  @retval EFI_SUCCESS             Get Device Expansion Region(DER) region Successfully.
**/
EFI_STATUS
SpsUpgradeGetDERRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  );

/**
  Check Region read access for specific region.

  @param[in] FlashRegionIndex     Flash Region index.

  @retval TRUE                    BIOS has read access.
  @retval FALSE                   BIOS has not read access.
**/
BOOLEAN
SpsUpgradeCheckBRRA (
  IN UINT8                             FlashRegionIndex
  );

/**
  Check Region write access for specific region.

  @param[in] FlashRegionIndex     Flash Region index.

  @retval TRUE                    BIOS has write access.
  @retval FALSE                   BIOS has not write access.
**/
BOOLEAN
SpsUpgradeCheckBRWA (
  IN UINT8                             FlashRegionIndex
  );

/**
  Protect Descriptor region if Bios write access was not permission.

  @param None.

  @retval EFI_NOT_FOUND           No Protected Region Register can be used.
  @retval EFI_SUCCESS             Completed succussfully.
**/
EFI_STATUS
SpsUpgradeProtectDescriptorRegion (
  VOID
  );

/**
  Protect ME region if Bios write access was not permission.

  @param[in] FactoryDefaultBase  Factory default base from ME fw.
  @param[in] FactoryDefaultLimit Factory default limit from ME fw.

  @retval EFI_NOT_FOUND           No Protected Region Register can be used.
  @retval EFI_SUCCESS             Completed succussfully.
**/
EFI_STATUS
SpsUpgradeProtectMeRegion (
  IN UINT32                            FactoryDefaultBase,
  IN UINT32                            FactoryDefaultLimit
  );

/**
  Get SPS ME sub region information. if Me Region Man is valid.

  @param[in]  Signature           Target subregion;'s signature.
  @param[in]  MeRegionMap         Source of SPS ME map information.
  @param[out] SubRegionInfo       point to Sub region information buffer.

  @retval EFI_NOT_FOUND           Sub region not found in MeRegionMap.
  @retval EFI_SUCCESS             Sub region found.
**/
EFI_STATUS
SpsUpgradeGetSubRegionInfo (
  IN  UINT32                           Signature,
  IN  SPS_ME_UPGRADE_MAP_INFO          *MeRegionMap,
  OUT SPS_ME_SUB_REGION_INFO           *SubRegionInfo
  );

/**
  Find SPS FPT data.

  @param[in]  SourceData          Source data.
  @param[in]  SourceSize          Size of Source.
  @param[out] FptPoint            point to FPT address in SourceData.

  @retval EFI_NOT_FOUND           FPT not found in SourceData.
  @retval EFI_SUCCESS             FPT found.
**/
EFI_STATUS
SpsUpgradeSearchFpt (
  IN  UINT8                            *SourceData,
  IN  UINTN                            SourceSize,
  OUT VOID                             **FptPoint
  );

/**
  Create SPS ME sub region information.

  @param[in]      SourceData      Source data.
  @param[in]      SourceSize      Size of Source.
  @param[in, out] SpsMeRegionMap  point to ME region Map buffer.

  @retval EFI_NOT_FOUND           FPT not found inf SourceData.
  @retval EFI_SUCCESS             ME region created in SpsMeRegionMap.
**/
EFI_STATUS
SpsUpgradeCreateRegionInfo (
  IN     UINT8                         *SourceData,
  IN     UINTN                         SourceSize,
  IN OUT SPS_ME_UPGRADE_MAP_INFO       *SpsMeRegionMap
  );

/**
  Check SPS ME Sub region.

  @param[in]  Signature           Target subregion;'s signature.
  @param[in]  OldMeMapInfo        Source of Current SPS ME map information.
  @param[in]  NewMeMapInfo        Source of New SPS ME map information.
  @param[out] IsOffsetMatch       Is Sub region offset match.
  @param[out] IsSizeMatch         Is Sub region Size match.

  @retval EFI_NOT_FOUND           FPT not found inf SourceData.
  @retval EFI_SUCCESS             Sub region check done.
**/
EFI_STATUS
SpsUpgradeCheckSubRegion (
  IN  UINT32                           Signature,
  IN  SPS_ME_UPGRADE_MAP_INFO          *OldMeMapInfo,
  IN  SPS_ME_UPGRADE_MAP_INFO          *NewMeMapInfo,
  OUT BOOLEAN                          *IsOffsetMatch,
  OUT BOOLEAN                          *IsSizeMatch
  );

/**
  Get SPS ME Firmware version from Recovery Image (or Operation Image).

  @param[in]  SourceData          Source data.
  @param[in]  SourceSize          Size of Source.
  @param[out] MajorNumber         point to Minor Number.
  @param[out] MinorNumber         point to Minor Number.
  @param[out] PatchNumber         point to Patch Number.
  @param[out] BuildNumber         point to Build Number.

  @retval EFI_NOT_FOUND           Version not found inf SourceData.
  @retval EFI_SUCCESS             Get Version successfully.
**/
EFI_STATUS
SpsUpgradeGetFwVersionFromImage (
  IN  UINT8                         *SourceData,
  IN  UINTN                         SourceSize,
  OUT UINT16                        *MajorNumber,
  OUT UINT16                        *MinorNumber,
  OUT UINT16                        *PatchNumber,
  OUT UINT16                        *BuildNumber
  );

/**
  Show SPS ME map info.

  @param[in]      SpsMeRegionMap  Point to SPS ME Me Map Information.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS.
**/
EFI_STATUS
SpsUpgradeShowMeMapInfo (
  IN SPS_ME_UPGRADE_MAP_INFO       *SpsMeRegionMap
  );

#endif
