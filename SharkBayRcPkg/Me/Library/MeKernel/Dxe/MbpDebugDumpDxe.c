/** @file

  Dump whole DXE_MBP_DATA_PROTOCOL and serial out.

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


//[-start-120808-IB10820098-modify]//
#include <Library/DebugLib.h>
#include <Protocol/MeBiosPayloadData.h>
//[-end-120808-IB10820098-modify]//

/**
  Dump DXE_MBP_DATA_PROTOCOL

  @param [in] MbpData             Pointer to DXE_MBP_DATA_PROTOCOL

**/
VOID
DxeMbpDebugDump (
  IN DXE_MBP_DATA_PROTOCOL             *MbpPtr
  )
{
  UINTN i;
  
  if (MbpPtr == NULL) {
    return;
  }
  DEBUG ((EFI_D_INFO, "\n------------------------ MeBiosPayload Data Protocol Dump Begin -----------------\n"));
  DEBUG ((EFI_D_INFO, " Revision : 0x%x\n", MbpPtr->Revision));
  DEBUG ((EFI_D_INFO, "MeBiosPayload FwVersionName ---\n"));
  DEBUG ((EFI_D_INFO, " ME FW MajorVersion : 0x%x\n", MbpPtr->MeBiosPayload.FwVersionName.MajorVersion));
  DEBUG ((EFI_D_INFO, " ME FW MinorVersion : 0x%x\n", MbpPtr->MeBiosPayload.FwVersionName.MinorVersion));
  DEBUG ((EFI_D_INFO, " ME FW HotfixVersion : 0x%x\n", MbpPtr->MeBiosPayload.FwVersionName.HotfixVersion));
  DEBUG ((EFI_D_INFO, " ME FW BuildVersion : 0x%x\n", MbpPtr->MeBiosPayload.FwVersionName.BuildVersion));

  if (MbpPtr->MeBiosPayload.FwCapsSku.Available == TRUE) {
    DEBUG ((EFI_D_INFO, "MeBiosPayload FwCapabilities ---\n"));
    DEBUG ((EFI_D_INFO, " FullNet : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.FullNet));
    DEBUG ((EFI_D_INFO, " StdNet : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.StdNet));
    DEBUG ((EFI_D_INFO, " Manageability : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.Manageability));
    DEBUG ((EFI_D_INFO, " SmallBusiness : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.SmallBusiness));
    DEBUG ((EFI_D_INFO, " IntelAT : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntelAT));
    DEBUG ((EFI_D_INFO, " IntelCLS : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntelCLS));
    DEBUG ((EFI_D_INFO, " IntelMPC : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntelMPC));
    DEBUG ((EFI_D_INFO, " IccOverClocking : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IccOverClocking));
    DEBUG ((EFI_D_INFO, " PAVP : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.PAVP));
    DEBUG ((EFI_D_INFO, " IPV6 : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IPV6));
    DEBUG ((EFI_D_INFO, " KVM : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.KVM));
    DEBUG ((EFI_D_INFO, " OCH : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.OCH));
    DEBUG ((EFI_D_INFO, " VLAN : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.VLAN));
    DEBUG ((EFI_D_INFO, " TLS : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.TLS));
    DEBUG ((EFI_D_INFO, " WLAN : 0x%x\n", MbpPtr->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.WLAN));
  }

  DEBUG ((EFI_D_INFO, "MeBiosPayload RomBistData ---\n"));
  DEBUG ((EFI_D_INFO, " DeviceId : 0x%x\n", MbpPtr->MeBiosPayload.RomBistData.DeviceId));
  DEBUG ((EFI_D_INFO, " FuseTestFlags : 0x%x\n", MbpPtr->MeBiosPayload.RomBistData.FuseTestFlags));
  for (i = 0; i < 4; i++) {
    DEBUG ((EFI_D_INFO, " UMCHID[0x%x] : 0x%x \n", i, MbpPtr->MeBiosPayload.RomBistData.UMCHID[i]));
  }

  DEBUG ((EFI_D_INFO, "MeBiosPayload Platform Key ---\n"));
  for (i = 0; i < 8; i++) {
    DEBUG ((EFI_D_INFO, " Key[0x%x] : 0x%x \n", i, MbpPtr->MeBiosPayload.PlatformKey.Key[i]));
  }

  if (MbpPtr->MeBiosPayload.FwPlatType.Available == TRUE) {
    DEBUG ((EFI_D_INFO, "MeBiosPayload ME Platform TYpe ---\n"));
    DEBUG ((EFI_D_INFO, " PlatformTargetUsageType : 0x%x\n", MbpPtr->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformTargetUsageType));
    DEBUG ((EFI_D_INFO, " PlatformTargetMarketType : 0x%x\n", MbpPtr->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformTargetMarketType));
    DEBUG ((EFI_D_INFO, " SuperSku : 0x%x\n", MbpPtr->MeBiosPayload.FwPlatType.RuleData.Fields.SuperSku));
    DEBUG ((EFI_D_INFO, " IntelMeFwImageType : 0x%x\n", MbpPtr->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType));
    DEBUG ((EFI_D_INFO, " PlatformBrand : 0x%x\n", MbpPtr->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformBrand));
  }

  DEBUG ((EFI_D_INFO, "MeBiosPayload IccProfile ---\n"));
  DEBUG ((EFI_D_INFO, " NumIccProfiles : 0x%x\n",  MbpPtr->MeBiosPayload.IccProfile.NumIccProfiles));
  DEBUG ((EFI_D_INFO, " IccProfileSoftStrap : 0x%x\n",  MbpPtr->MeBiosPayload.IccProfile.IccProfileSoftStrap));
  DEBUG ((EFI_D_INFO, " IccProfileIndex : 0x%x\n",  MbpPtr->MeBiosPayload.IccProfile.IccProfileIndex));
  DEBUG ((EFI_D_INFO, " RegBundles : 0x%x\n",  MbpPtr->MeBiosPayload.IccProfile.IccLockRegInfo.RegBundles));
  for (i = 0; i < MbpPtr->MeBiosPayload.IccProfile.IccLockRegInfo.RegBundles.BundlesCnt; i++) {
    DEBUG ((EFI_D_INFO, " RegMask[0x%x] : 0x%x\n", i, MbpPtr->MeBiosPayload.IccProfile.IccLockRegInfo.RegMask[i]));
  }

  DEBUG ((EFI_D_INFO, "MeBiosPayload AtState ---\n"));
  DEBUG ((EFI_D_INFO, " State : 0x%x\n", MbpPtr->MeBiosPayload.AtState.State));
  DEBUG ((EFI_D_INFO, " LastTheftTrigger : 0x%x\n", MbpPtr->MeBiosPayload.AtState.LastTheftTrigger));
  DEBUG ((EFI_D_INFO, " LockState : 0x%x\n", MbpPtr->MeBiosPayload.AtState.flags.LockState));
  DEBUG ((EFI_D_INFO, " AuthenticateModule : 0x%x\n", MbpPtr->MeBiosPayload.AtState.flags.AuthenticateModule));
  DEBUG ((EFI_D_INFO, " S3Authentication : 0x%x\n", MbpPtr->MeBiosPayload.AtState.flags.S3Authentication));
  DEBUG ((EFI_D_INFO, " FlashWearOut : 0x%x\n", MbpPtr->MeBiosPayload.AtState.flags.FlashWearOut));
  DEBUG ((EFI_D_INFO, " FlashVariableSecurity : 0x%x\n", MbpPtr->MeBiosPayload.AtState.flags.FlashVariableSecurity));

  DEBUG ((EFI_D_INFO, "MeBiosPayload MFSIntegrity: 0x%x\n", MbpPtr->MeBiosPayload.MFSIntegrity));

  if (MbpPtr->MeBiosPayload.HwaRequest.Available == TRUE) {
    DEBUG ((EFI_D_INFO, "MeBiosPayload HwaRequest ---\n"));
    DEBUG ((EFI_D_INFO, " MediaTablePush : 0x%x\n", MbpPtr->MeBiosPayload.HwaRequest.Data.Fields.MediaTablePush));
  }

  DEBUG ((EFI_D_INFO, "\n------------------------ MeBiosPayload Data Protocol Dump End -------------------\n"));
}

