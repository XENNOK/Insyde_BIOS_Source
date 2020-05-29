//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/** @file
  This file is a wrapper for Intel RapidStart Platform Policy driver.
  Get Setup Value to initilize Intel DXE Platform Policy.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#include <RapidStartDxePolicyInit.h>

RAPID_START_PLATFORM_POLICY_PROTOCOL  mDxePlatformRapidStartPolicy;

//
// Function implementations
//
/**
  Initilize Intel RapidStart DXE Platform Policy

  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
RapidStartDxePolicyInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_SETUP_UTILITY_PROTOCOL       *SetupUtility;
  CHIPSET_CONFIGURATION             *SetupVariable;
//[-start-120320-IB05300301-remove]//
//  UINT16                           RapidStartS3WakeTimerMin;
//[-end-120320-IB05300301-remove]//

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nRapid Start Policy Entry\n"));

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  
  SetupVariable = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);
  
  ///
  /// Initialize the EFI Driver Library
  ///
  ZeroMem (&mDxePlatformRapidStartPolicy, sizeof (RAPID_START_PLATFORM_POLICY_PROTOCOL));

  mDxePlatformRapidStartPolicy.EnableRapidStart           = SetupVariable->EnableRapidStart;
  mDxePlatformRapidStartPolicy.EntryOnS3RtcWake           = SetupVariable->EntryOnS3RtcWake;
//[-start-120320-IB05300301-remove]//
////
//// To support 24 hours in the UINT8
//// mininute = the value of 3WakeTimerMin that <= 60
//// hour = the value of S3WakeTimerMin that > 60
////
//  if (SetupVariable->S3WakeTimerMin <= 60) {
//    RapidStartS3WakeTimerMin = SetupVariable->S3WakeTimerMin;
//  } else {
//    RapidStartS3WakeTimerMin = (SetupVariable->S3WakeTimerMin - 60 ) * 60 + (SetupVariable->S3WakeTimerMin - (SetupVariable->S3WakeTimerMin - 60 ) );
//  }
//[-end-120320-IB05300301-remove]//
  mDxePlatformRapidStartPolicy.S3WakeTimerMin             = SetupVariable->S3WakeTimerMin;
  mDxePlatformRapidStartPolicy.EntryOnS3CritBattWake      = SetupVariable->EntryOnS3CritBattWake;
  mDxePlatformRapidStartPolicy.ActivePageThresholdSupport = SetupVariable->ActivePageThresholdEnable;
  mDxePlatformRapidStartPolicy.ActivePageThresholdSize    = SetupVariable->ActivePageThresholdSize;
//[-start-121009-IB10370024-add]//
  mDxePlatformRapidStartPolicy.HybridHardDisk             = SetupVariable->HybridHardDisk;
  mDxePlatformRapidStartPolicy.DisplaySaveRestore         = SetupVariable->DisplaySaveRestore;
  mDxePlatformRapidStartPolicy.DisplayType                = SetupVariable->DisplayType;
//[-end-121009-IB10370024-add]//
  ///
  /// By default set to 255 as not used
  ///
  mDxePlatformRapidStartPolicy.RaidModeSataPortNumber = PcdGet8 ( PcdDxePlatformRapidStartPolicyRaidModeSataPortNumber );
  if (SetupVariable->EnableRapidStart) {
    if (SetupVariable->EntryOnS3CritBattWake) {
      WriteExtCmos8(R_XCMOS_INDEX, R_XCMOS_DATA, CritticalBatWakeThres, SetupVariable->CritticalBatWakeThres);
    }
  }
  ///
  /// Protocol revision number
  ///
  mDxePlatformRapidStartPolicy.Revision = DXE_RAPID_START_PLATFORM_POLICY_PROTOCOL_REVISION;

  ///
  /// FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group
  /// Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  /// version information. The default value is type 221.
  ///
  mDxePlatformRapidStartPolicy.FviSmbiosType = PcdGet8 ( PcdMeMiscConfigFviSmbiosType );
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateRapidStartPlatformPolicy (&mDxePlatformRapidStartPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateRapidStartPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install protocol to to allow access to this Policy.
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gRapidStartPlatformPolicyProtocolGuid,
                    &mDxePlatformRapidStartPolicy,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121220-IB10820206-modify]//
  //
  // Dump RapidStart 
  //
  DumpRapidStartPolicy (&mDxePlatformRapidStartPolicy);

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Rapid Start Policy Exit\n"));

  return Status;
}

VOID
DumpRapidStartPolicy (
  IN      RAPID_START_PLATFORM_POLICY_PROTOCOL        *RapidStartPlatformPolicy
  )
{
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "RapidStartPlatformPolicy ( Address : 0x%x )\n", RapidStartPlatformPolicy ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-Revision                    : %x\n", RapidStartPlatformPolicy->Revision ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EnableRapidStart            : %x\n", RapidStartPlatformPolicy->EnableRapidStart ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EntryOnS3RtcWake            : %x\n", RapidStartPlatformPolicy->EntryOnS3RtcWake ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EntryOnS3CritBattWake       : %x\n", RapidStartPlatformPolicy->EntryOnS3CritBattWake ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-ActivePageThresholdSupport  : %x\n", RapidStartPlatformPolicy->ActivePageThresholdSupport ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-Reserved                    : %x\n", RapidStartPlatformPolicy->Reserved ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-S3WakeTimerMin              : %x\n", RapidStartPlatformPolicy->S3WakeTimerMin ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-RaidModeSataPortNumber      : %x\n", RapidStartPlatformPolicy->RaidModeSataPortNumber ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-ActivePageThresholdSize     : %x\n", RapidStartPlatformPolicy->ActivePageThresholdSize ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-FviSmbiosType               : %x\n", RapidStartPlatformPolicy->FviSmbiosType ) );
//[-start-121009-IB10370024-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-HybridHardDisk              : %x\n", RapidStartPlatformPolicy->HybridHardDisk ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-DisplaySaveRestore          : %x\n", RapidStartPlatformPolicy->DisplaySaveRestore ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-DisplayType                 : %x\n", RapidStartPlatformPolicy->DisplayType ) );
//[-end-121009-IB10370024-add]//

  return;
}

