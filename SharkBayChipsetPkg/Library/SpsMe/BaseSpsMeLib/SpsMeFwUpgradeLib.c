/** @file

  Implementation file for SPS ME Firmware upgrade functionality
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

#include <Uefi.h>
#include <Include/SpsMeUpgradeCommon.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/SpsMeChipsetLib.h>

#define SPS_DEBUG_NAME                 "[SPS UPDATE LIB] "

/**
  Enable HECI device

  @param None

  @retval EFI_SUCCESS             Always return Successfully.
**/
EFI_STATUS
SpsUpgradeEnableHeciDevice (
  VOID
  )
{
  // Enable HECI device if it is disabled
  SpsMeChipsetEnableHeciDevice ();

  return EFI_SUCCESS;
}

/**
  Get ROM linear base address of system from descriptor region

  @param[out] RomBaseAddress      The pointer of ROM linear base address

  @retval EFI_SUCCESS             Get Rom Base Address Successfully.
**/
EFI_STATUS
SpsUpgradeGetRomBaseAddress (
  OUT UINTN                            *RomBaseAddress
  )
{
  EFI_STATUS            Status;
  UINTN                 TempRomBaseAddress;

  if (RomBaseAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsMeChipsetGetRomBaseAddress (&TempRomBaseAddress);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get Rom Base Address => (%r)\n", Status));
    return Status;
  }
  *RomBaseAddress = TempRomBaseAddress;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 TempBase;
  UINTN                 TempLimit;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempBase  = 0;
  TempLimit = 0;

  Status = SpsMeChipsetGetDescRegion (&TempBase, &TempLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get Descriptor region => (%r)\n", Status));
    return Status;
  }

  *Base  = TempBase;
  *Limit = TempLimit;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 TempBase;
  UINTN                 TempLimit;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempBase  = 0;
  TempLimit = 0;

  Status = SpsMeChipsetGetMeRegion (&TempBase, &TempLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get ME region => (%r)\n", Status));
  }

  *Base  = TempBase;
  *Limit = TempLimit;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 TempBase;
  UINTN                 TempLimit;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempBase  = 0;
  TempLimit = 0;

  Status = SpsMeChipsetGetGBERegion (&TempBase, &TempLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get GBE region => (%r)\n", Status));
    return Status;
  }

  *Base  = TempBase;
  *Limit = TempLimit;

  return EFI_SUCCESS;
};

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
  )
{
  EFI_STATUS            Status;
  UINTN                 TempBase;
  UINTN                 TempLimit;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempBase  = 0;
  TempLimit = 0;

  Status = SpsMeChipsetGetPDRRegion (&TempBase, &TempLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get Platform Data Regione(PDR) => (%r)\n", Status));
    return Status;
  }

  *Base  = TempBase;
  *Limit = TempLimit;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 TempBase;
  UINTN                 TempLimit;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempBase  = 0;
  TempLimit = 0;

  Status = SpsMeChipsetGetDERRegion (&TempBase, &TempLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Get Device Expansion Region(DER) => (%r)\n", Status));
    return Status;
  }

  *Base  = TempBase;
  *Limit = TempLimit;

  return EFI_SUCCESS;
}

/**
  Check Region read access for specific region.

  @param[in] FlashRegionIndex     Flash Region index.

  @retval TRUE                    BIOS has read access.
  @retval FALSE                   BIOS has not read access.
**/
BOOLEAN
SpsUpgradeCheckBRRA (
  IN UINT8                             FlashRegionIndex
  )
{
  BOOLEAN     IsReadAccessEnable;

  IsReadAccessEnable  = FALSE;

  switch (FlashRegionIndex) {
  case SPS_DESC_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckDescBRRA ();
    break;

  case SPS_BIOS_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckBiosBRRA ();
    break;

  case SPS_ME_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckMeBRRA ();
    break;

  case SPS_GBE_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckGbeBRRA ();
    break;

  case SPS_PDR_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckPDRBRRA ();
    break;

  case SPS_DER_REGION:
    IsReadAccessEnable  = SpsMeChipsetCheckDERBRRA ();
    break;

  default:
    break;
  }

  return IsReadAccessEnable;
}

/**
  Check Region write access for Region0 Flash Descriptor.

  @param[in] FlashRegionIndex     Flash Region index.

  @retval TRUE                    BIOS has write access.
  @retval FALSE                   BIOS has not write access.
**/
BOOLEAN
SpsUpgradeCheckBRWA (
  IN UINT8                             FlashRegionIndex
  )
{
  BOOLEAN     IsWriteAccessEnable;

  IsWriteAccessEnable = FALSE;

  switch (FlashRegionIndex) {
  case SPS_DESC_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckDescBRWA ();
    break;

  case SPS_BIOS_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckBiosBRWA ();
    break;

  case SPS_ME_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckMeBRWA ();
    break;

  case SPS_GBE_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckGbeBRWA ();
    break;

  case SPS_PDR_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckPDRBRWA ();
    break;

  case SPS_DER_REGION:
    IsWriteAccessEnable = SpsMeChipsetCheckDERBRWA ();
    break;

  default:
    break;
  }

  return IsWriteAccessEnable;
}

/**
  Protect Descriptor region if Bios write access was not permission.

  @param None.

  @retval EFI_NOT_FOUND           No Protected Region Register can be used.
  @retval EFI_SUCCESS             Completed succussfully.
**/
EFI_STATUS
SpsUpgradeProtectDescriptorRegion (
  VOID
  )
{
  EFI_STATUS            Status;

  Status = SpsMeChipsetProtectDescRegion ();
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Protect Descriptor region => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;

  Status = SpsMeChipsetProtectMeRegion (FactoryDefaultBase, FactoryDefaultLimit);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Protect ME region => (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

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
  )
{
  SPS_ME_SUB_REGION_INFO     *TempMeSubRegionInfo;
  EFI_STATUS                 Status;
  UINTN                      NumberOfSubRegion;
  UINTN                      Index;

  Status = EFI_NOT_FOUND;

  if ((SubRegionInfo == NULL) || (MeRegionMap == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempMeSubRegionInfo = MeRegionMap->SubRegionInfo;
  NumberOfSubRegion   = MeRegionMap->NumberOfSubRegion;

  for (Index = 0; Index < NumberOfSubRegion; Index++) {
    if (Signature == TempMeSubRegionInfo[Index].Signature) {
      CopyMem (SubRegionInfo, &TempMeSubRegionInfo[Index], sizeof (SPS_ME_SUB_REGION_INFO));
      return EFI_SUCCESS;
    }
  }

  return Status;
}

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
  )
{
  UINTN                 Index;
  SPS_ME_BINARY_FPT     *TempSpsMeFpt;

  if ((FptPoint == NULL) || (SourceData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *FptPoint  = NULL;

  for (Index = 0; Index < SourceSize; Index++) {
    TempSpsMeFpt = (SPS_ME_BINARY_FPT *)&SourceData[Index];
    if (TempSpsMeFpt->Signature == SPS_ME_SUB_REGION_SIGNATURE_FPT) {
      *FptPoint = (VOID *)(UINTN)TempSpsMeFpt;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

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
  )
{
  EFI_STATUS                           Status;
  SPS_ME_BINARY_FPT_ENTRY              *FptEntries;
  SPS_ME_BINARY_FPT                    *SpsMeFpt;
  SPS_ME_BINARY_FPT                    *TempSpsMeFpt;
  SPS_ME_SUB_REGION_INFO               *TempSubRegionInfo;
  UINTN                                TempNumberOfSubRegion;
  UINTN                                Index;

  FptEntries            = NULL;
  TempSpsMeFpt          = NULL;
  SpsMeFpt              = NULL;
  Index                 = 0;
  TempSubRegionInfo     = NULL;
  TempNumberOfSubRegion = 0;

  if ((SourceData == NULL) || (SourceSize == 0) || (SpsMeRegionMap == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Search Flash Partition Table($FPT) region
  ///
  Status = SpsUpgradeSearchFpt (SourceData, SourceSize, &SpsMeFpt);
  if (EFI_ERROR (Status) || (SpsMeFpt == NULL)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Flash Partition Table Not Found\n"));
    SpsMeRegionMap->NumberOfSubRegion = 0;
    return EFI_NOT_FOUND;
  }
#if SPS_DEBUG_FULL_MESSAGE
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Flash Partition Table Found\n"));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Number of Flash Partition Table Entries : %x\n", SpsMeFpt->NumberOfEntries));
#endif

  ///
  /// Allocate Buffer for SPS ME sub region.
  ///
  TempSubRegionInfo = AllocateZeroPool ((SpsMeFpt->NumberOfEntries + 1) * sizeof (SPS_ME_SUB_REGION_INFO));
  if (TempSubRegionInfo == NULL) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Allocate buffer for SPS ME sub region failed!\n"));
    if (TempSubRegionInfo != NULL) {
      FreePool (TempSubRegionInfo);
      TempSubRegionInfo = NULL;
    }
    SpsMeRegionMap->NumberOfSubRegion = 0;
    return EFI_BUFFER_TOO_SMALL;
  }

  ///
  /// Update SPS ME sub region information.
  ///
  ///
  /// Update FPT region information.
  ///   FPT region Signature = $FPT
  ///   FPT region Size      = Next region start.
  ///   FPT region Offset    = Entire image base;
  ///   FPT region Start     = ME Region start.
  ///   FPT region End       = FPT region Start + FPT region Size - 1;
  ///
  TempSubRegionInfo[TempNumberOfSubRegion].Signature = SpsMeFpt->Signature;
  TempSubRegionInfo[TempNumberOfSubRegion].Size      = SpsMeFpt->Entry[0].Offset;
  TempSubRegionInfo[TempNumberOfSubRegion].Offset    = 0;
  TempSubRegionInfo[TempNumberOfSubRegion].Valid     = TRUE;
  TempSubRegionInfo[TempNumberOfSubRegion].Start     = SpsMeRegionMap->MeRegionBase;
  TempSubRegionInfo[TempNumberOfSubRegion].End       = TempSubRegionInfo[TempNumberOfSubRegion].Start + TempSubRegionInfo[TempNumberOfSubRegion].Size - 1;
  if ((TempSubRegionInfo[TempNumberOfSubRegion].Offset + TempSubRegionInfo[TempNumberOfSubRegion].Size) > SourceSize) {
    TempSubRegionInfo[TempNumberOfSubRegion].DataValid = FALSE;
    TempSubRegionInfo[TempNumberOfSubRegion].Data      = NULL;
  } else {
    TempSubRegionInfo[TempNumberOfSubRegion].DataValid = TRUE;
    TempSubRegionInfo[TempNumberOfSubRegion].Data      = SourceData + TempSubRegionInfo[TempNumberOfSubRegion].Offset;
  }
  TempNumberOfSubRegion++;

  for (Index = 0; Index < SpsMeFpt->NumberOfEntries; Index++) {
    FptEntries = (SPS_ME_BINARY_FPT_ENTRY *) &SpsMeFpt->Entry[Index];
    TempSubRegionInfo[TempNumberOfSubRegion].Signature = FptEntries->Signature;
    TempSubRegionInfo[TempNumberOfSubRegion].Size      = FptEntries->Size;
    TempSubRegionInfo[TempNumberOfSubRegion].Offset    = FptEntries->Offset;
    TempSubRegionInfo[TempNumberOfSubRegion].Valid     = TRUE;
    TempSubRegionInfo[TempNumberOfSubRegion].Start     = SpsMeRegionMap->MeRegionBase + FptEntries->Offset;
    TempSubRegionInfo[TempNumberOfSubRegion].End       = TempSubRegionInfo[TempNumberOfSubRegion].Start + TempSubRegionInfo[TempNumberOfSubRegion].Size - 1;

    if ((TempSubRegionInfo[TempNumberOfSubRegion].Offset + TempSubRegionInfo[TempNumberOfSubRegion].Size) > SourceSize) {
      TempSubRegionInfo[TempNumberOfSubRegion].DataValid = FALSE;
      TempSubRegionInfo[TempNumberOfSubRegion].Data      = NULL;
    } else {
      TempSubRegionInfo[TempNumberOfSubRegion].DataValid = TRUE;
      TempSubRegionInfo[TempNumberOfSubRegion].Data      = SourceData + TempSubRegionInfo[TempNumberOfSubRegion].Offset;
    }
    TempNumberOfSubRegion++;
  }

  SpsMeRegionMap->NumberOfSubRegion = TempNumberOfSubRegion;
  SpsMeRegionMap->SubRegionInfo     = TempSubRegionInfo;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                           Status;
  SPS_ME_SUB_REGION_INFO               TempOldSubRegionInfo;
  SPS_ME_SUB_REGION_INFO               TempNewSubRegionInfo;
  UINTN                                TempOldOffset;
  UINTN                                TempNewOffset;
  UINTN                                TempOldSize;
  UINTN                                TempNewSize;

  if ((OldMeMapInfo == NULL) || (NewMeMapInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *IsOffsetMatch        = TRUE;
  *IsSizeMatch          = TRUE;
  TempOldOffset         = 0;
  TempNewOffset         = 0;
  TempOldSize           = 0;
  TempNewSize           = 0;

  ///
  /// Check Sub region has no change.
  ///
  Status = SpsUpgradeGetSubRegionInfo (Signature, NewMeMapInfo, &TempNewSubRegionInfo);
  if ((!EFI_ERROR (Status)) && TempNewSubRegionInfo.Valid) {
    Status = SpsUpgradeGetSubRegionInfo (Signature, OldMeMapInfo, &TempOldSubRegionInfo);
    if ((!EFI_ERROR (Status)) && TempOldSubRegionInfo.Valid) {
      TempOldOffset = TempOldSubRegionInfo.Offset;
      TempNewOffset = TempNewSubRegionInfo.Offset;
      if (TempOldOffset != TempNewSubRegionInfo.Offset) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match. Old(0x%08x):New(0x%08x)\n", Signature, TempOldOffset, TempNewOffset));
        if (IsOffsetMatch != NULL) {
          *IsOffsetMatch = FALSE;
        }
      }

      TempOldSize = TempOldSubRegionInfo.Size;
      TempNewSize = TempNewSubRegionInfo.Size;
      if (TempOldSize != TempNewSize) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match. Old(0x%08x):New(0x%08x)\n", Signature, TempOldSize, TempNewSize));
        if (IsSizeMatch != NULL) {
          *IsSizeMatch = FALSE;
        }
      }
    }
  }

  return Status;
}

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
  )
{
  EFI_STATUS       Status;

  if (SourceData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  ///
  /// Search "$MN2"
  ///
  if ((*(UINT16 *)(SourceData + 0x10) == 0x8086) && \
      (*(UINT32 *)(SourceData + 0x1C) == 0x324E4D24)) {
  
    if (MajorNumber != NULL) {
      *MajorNumber = *(UINT16 *)(SourceData + 0x24);
    }
    if (MinorNumber != NULL) {
      *MinorNumber = *(UINT16 *)(SourceData + 0x26);
    }
    if (PatchNumber != NULL) {
      *PatchNumber = *(UINT16 *)(SourceData + 0x28);
    }
    if (BuildNumber != NULL) {
      *BuildNumber = *(UINT16 *)(SourceData + 0x2A);
    }
    return EFI_SUCCESS;
  }

  return Status;
}

/**
  Show SPS ME map info.

  @param[in]      SpsMeRegionMap  Point to SPS ME Me Map Information.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS.
**/
EFI_STATUS
SpsUpgradeShowMeMapInfo (
  IN SPS_ME_UPGRADE_MAP_INFO       *SpsMeRegionMap
  )
{
#if SPS_DEBUG_FULL_MESSAGE
  UINTN  Index;
  UINT32 Signature;

  if (SpsMeRegionMap == NULL) {
    return EFI_SUCCESS;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME region info:\n"));
  SPS_DEBUG ((DEBUG_INFO, "  MeRegionBase      = 0x%08x\n", SpsMeRegionMap->MeRegionBase));
  SPS_DEBUG ((DEBUG_INFO, "  MeRegionLimit     = 0x%08x\n", SpsMeRegionMap->MeRegionLimit));
  SPS_DEBUG ((DEBUG_INFO, "  MeRegionSize      = 0x%08x\n", SpsMeRegionMap->MeRegionSize));
  SPS_DEBUG ((DEBUG_INFO, "  Valid             = %x\n", SpsMeRegionMap->Valid));
  SPS_DEBUG ((DEBUG_INFO, "  NumberOfSubRegion = 0x%x\n", SpsMeRegionMap->NumberOfSubRegion));

  for (Index = 0; Index < SpsMeRegionMap->NumberOfSubRegion; Index++) {
    Signature = SpsMeRegionMap->SubRegionInfo[Index].Signature;
    SPS_DEBUG ((DEBUG_INFO, "SPS ME Sub region...\n"));
    SPS_DEBUG ((DEBUG_INFO, "  Signature = %a\n", &Signature));
    SPS_DEBUG ((DEBUG_INFO, "  Offset    = 0x%08x\n", SpsMeRegionMap->SubRegionInfo[Index].Offset));
    SPS_DEBUG ((DEBUG_INFO, "  Start     = 0x%08x\n", SpsMeRegionMap->SubRegionInfo[Index].Start));
    SPS_DEBUG ((DEBUG_INFO, "  End       = 0x%08x\n", SpsMeRegionMap->SubRegionInfo[Index].End));
    SPS_DEBUG ((DEBUG_INFO, "  Size      = 0x%08x\n", SpsMeRegionMap->SubRegionInfo[Index].Size));
    SPS_DEBUG ((DEBUG_INFO, "  Valid     = %x\n", SpsMeRegionMap->SubRegionInfo[Index].Valid));
    SPS_DEBUG ((DEBUG_INFO, "  DataValid = %x\n", SpsMeRegionMap->SubRegionInfo[Index].DataValid));
    if (SpsMeRegionMap->SubRegionInfo[Index].DataValid) {
      SPS_DEBUG ((DEBUG_INFO, "  Data      = 0x%08x\n", SpsMeRegionMap->SubRegionInfo[Index].Data));
    }
    SPS_DEBUG ((DEBUG_INFO, "\n"));
  }
#endif
  
  return EFI_SUCCESS;
}

