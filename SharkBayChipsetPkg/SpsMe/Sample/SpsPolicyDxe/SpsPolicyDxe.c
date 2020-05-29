/** @file



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

#include "SpsPolicyDxe.h"

SPS_POLICY_PROTOCOL                    mSpsPolicyInstance;

VOID
DxeDumpSpsPolicy (
  VOID
  )
{
  UINT8                 Index;
  UINTN                 MaxPStatesRatioNumber;

  Index = 0;
  MaxPStatesRatioNumber = sizeof(mSpsPolicyInstance.HostInfo.PStatesRatio)/sizeof(mSpsPolicyInstance.HostInfo.PStatesRatio[0]);

  DEBUG ((DEBUG_INFO, "\n"));
  DEBUG ((DEBUG_INFO, "[DXE SPS ME POLICY] INFO: mSpsPolicyInstance (Address : 0x%x)\n", &mSpsPolicyInstance));
  DEBUG ((DEBUG_INFO, " |-Revision                  : 0x%x\n", mSpsPolicyInstance.Revision));
  DEBUG ((DEBUG_INFO, " |-ME-BIOS Interface Version\n"));
  DEBUG ((DEBUG_INFO, " | |-Major Version           : 0x%x\n", mSpsPolicyInstance.MeBiosIntfVer.Major));
  DEBUG ((DEBUG_INFO, " | |-Minor Version           : 0x%x\n", mSpsPolicyInstance.MeBiosIntfVer.Minor));
  DEBUG ((DEBUG_INFO, " |-SkuSwitch\n"));
  DEBUG ((DEBUG_INFO, " | |-Silicon Enabled         : 0x%x\n", mSpsPolicyInstance.SkuSwitch.SiliconEnabled));
  DEBUG ((DEBUG_INFO, " | |-NodeManager Enabled     : 0x%x\n", mSpsPolicyInstance.SkuSwitch.NodeManagerEnabled));
  DEBUG ((DEBUG_INFO, " |-Message Switch\n"));
  DEBUG ((DEBUG_INFO, " | |-MeBiosInterfaceVersion  : 0x%x\n", mSpsPolicyInstance.MsgSwitch.MeBiosInterfaceVersion));
  DEBUG ((DEBUG_INFO, " | |-Dynamic Fusing          : 0x%x\n", mSpsPolicyInstance.MsgSwitch.DynamicFusing));
  DEBUG ((DEBUG_INFO, " | |-ICC Set Clock Enables   : 0x%x\n", mSpsPolicyInstance.MsgSwitch.IccSetClockEnables));
  DEBUG ((DEBUG_INFO, " | |-ICC Set Spread Spectrum : 0x%x\n", mSpsPolicyInstance.MsgSwitch.IccSetSpreadSpectrum));
  DEBUG ((DEBUG_INFO, " | |-Host Configuration      : 0x%x\n", mSpsPolicyInstance.MsgSwitch.HostConfiguration));
  DEBUG ((DEBUG_INFO, " | |-HMRFPO GET STATUS       : 0x%x\n", mSpsPolicyInstance.MsgSwitch.HmrfpoGetStatus));
  DEBUG ((DEBUG_INFO, " | |-HMRFPO LOCK             : 0x%x\n", mSpsPolicyInstance.MsgSwitch.HmrfpoLock));
  DEBUG ((DEBUG_INFO, " | |-HMRFPO ENABLE           : 0x%x\n", mSpsPolicyInstance.MsgSwitch.HmrfpoEnable));
  DEBUG ((DEBUG_INFO, " | |-End Of Post             : 0x%x\n", mSpsPolicyInstance.MsgSwitch.EndOfPost));

  DEBUG ((DEBUG_INFO, " |-ICC Setting\n"));
  if (mSpsPolicyInstance.MsgSwitch.IccSetClockEnables) {
    DEBUG ((DEBUG_INFO, " | |-ICC Clock Enables (SKip Response: 0x%x)\n", mSpsPolicyInstance.IccSetting.IccClockEnablesSkipResponse));
    DEBUG ((DEBUG_INFO, " | | |-Data                  : 0x%08x\n", mSpsPolicyInstance.IccSetting.IccClockEnables.UInt32));
  }

  if (mSpsPolicyInstance.MsgSwitch.IccSetClockEnables) {
    DEBUG ((DEBUG_INFO, " | |-ICC Spread spectrum select (SKip Response: 0x%x)\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrumSkipResponse));
    DEBUG ((DEBUG_INFO, " | | |-SSC1ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC1ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC2ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC2ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC3ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC3ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC4ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC4ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC5ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC5ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC6ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC6ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC7ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC7ModeSelect));
    DEBUG ((DEBUG_INFO, " | | |-SSC8ModeSelect        : 0x%x\n", mSpsPolicyInstance.IccSetting.IccSpreadSpectrum.Field.SSC8ModeSelect));
  }

  if (mSpsPolicyInstance.MsgSwitch.HostConfiguration) {
    DEBUG ((DEBUG_INFO, " |-Host Configuration Information\n"));
    DEBUG ((DEBUG_INFO, " | |-Capabilities            : 0x%x\n", mSpsPolicyInstance.HostInfo.Capabilities));
    DEBUG ((DEBUG_INFO, " | |-PStatesNumber           : 0x%x\n", mSpsPolicyInstance.HostInfo.PStatesNumber));
    DEBUG ((DEBUG_INFO, " | |-TStatesNumber           : 0x%x\n", mSpsPolicyInstance.HostInfo.TStatesNumber));
    DEBUG ((DEBUG_INFO, " | |-MaxPower                : 0x%x\n", mSpsPolicyInstance.HostInfo.MaxPower));
    DEBUG ((DEBUG_INFO, " | |-MinPower                : 0x%x\n", mSpsPolicyInstance.HostInfo.MinPower));
    DEBUG ((DEBUG_INFO, " | |-ProcNumber              : 0x%x\n", mSpsPolicyInstance.HostInfo.ProcNumber));
    DEBUG ((DEBUG_INFO, " | |-ProcCoresNumber         : 0x%x\n", mSpsPolicyInstance.HostInfo.ProcCoresNumber));
    DEBUG ((DEBUG_INFO, " | |-ProcCoresEnabled        : 0x%x\n", mSpsPolicyInstance.HostInfo.ProcCoresEnabled));
    DEBUG ((DEBUG_INFO, " | |-ProcThreadsEnabled      : 0x%x\n", mSpsPolicyInstance.HostInfo.ProcThreadsEnabled));
    DEBUG ((DEBUG_INFO, " | |-TurboRatioLimit         : 0x%lx\n", mSpsPolicyInstance.HostInfo.TurboRatioLimit));
    DEBUG ((DEBUG_INFO, " | |-PlatformInfo            : 0x%lx\n", mSpsPolicyInstance.HostInfo.PlatformInfo));
    DEBUG ((DEBUG_INFO, " | |-Altitude                : 0x%x\n", mSpsPolicyInstance.HostInfo.Altitude));
    for (Index = 0; Index < MaxPStatesRatioNumber; Index++) {
      DEBUG ((DEBUG_INFO, " | |-PStatesRatio[%02d]        : 0x%02x\n", Index, mSpsPolicyInstance.HostInfo.PStatesRatio[Index]));
    }
  }

  if (mSpsPolicyInstance.MeFwUpgradeConfig.UpgradeSupported) {
    DEBUG ((DEBUG_INFO, " |-MeFwUpgradeConfig\n"));
    DEBUG ((DEBUG_INFO, " | |-UpgradeSupported        : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.UpgradeSupported));
    DEBUG ((DEBUG_INFO, " | |-ProtectDescriptorRegion : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.ProtectDescriptorRegion));
    DEBUG ((DEBUG_INFO, " | |-ProtectMeRegion         : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.ProtectMeRegion));
    DEBUG ((DEBUG_INFO, " | |-ProtectGbeRegion        : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.ProtectGbeRegion));
    DEBUG ((DEBUG_INFO, " | |-ProtectPDRRegion        : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.ProtectPDRRegion));
    DEBUG ((DEBUG_INFO, " | |-ProtectDERRegion        : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.ProtectDERRegion));
    DEBUG ((DEBUG_INFO, " | |-SpiLock                 : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.SpiLock));
    DEBUG ((DEBUG_INFO, " | |-CmosIndexPort           : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.CmosIndexPort));
    DEBUG ((DEBUG_INFO, " | |-CmosDataPort            : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.CmosDataPort));
    DEBUG ((DEBUG_INFO, " | |-CmosOffset              : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.CmosOffset));
    DEBUG ((DEBUG_INFO, " | |-FactoryDefaultSize      : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.FactoryDefaultSize));
    DEBUG ((DEBUG_INFO, " | |-OperationImageSize      : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.OperationImageSize));
    DEBUG ((DEBUG_INFO, " | |-BlockMode               : 0x%x\n", mSpsPolicyInstance.MeFwUpgradeConfig.BlockMode));
  }

  DEBUG ((DEBUG_INFO, "\n"));

  return;
}

EFI_STATUS
SpsPolicyGetSpsPolicyHob (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_HOB_GUID_TYPE          *GuidHob;
  SPS_POLICY_HOB             *SpsPolicyHob;

  Status = EFI_UNSUPPORTED;
  GuidHob = GetFirstGuidHob (&gSpsPolicyHobGuid);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_INFO, "[DXE SPS ME POLICY] INFO: SPS POLICY HOB Not found\n"));
    return EFI_NOT_FOUND;
  }

  SpsPolicyHob = (SPS_POLICY_HOB *) GET_GUID_HOB_DATA (GuidHob);

  CopyMem (&mSpsPolicyInstance, SpsPolicyHob, sizeof (SPS_POLICY_HOB));

  return EFI_SUCCESS;
}

EFI_STATUS
SpsPolicyDxeEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      Handle;
  SYSTEM_CONFIGURATION            *SetupVariable;

  DEBUG ((DEBUG_INFO, "[DXE SPS ME POLICY] INFO: SPS ME Policy Entry\n"));

  ///
  /// Initial variables
  ///
  ZeroMem (&mSpsPolicyInstance, sizeof (SPS_POLICY_PROTOCOL));

  Handle           = NULL;
  SetupVariable    = NULL;

  SetupVariable = (SYSTEM_CONFIGURATION *) GetVariable (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (SetupVariable == NULL) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS ME POLICY] ERROR: Get Setup variable fail.\n"));
  }

  Status = SpsPolicyGetSpsPolicyHob ();
  if (EFI_ERROR (Status)) {
  }

  DxeDumpSpsPolicy ();

  ///
  /// Install ME Policy Protocol.
  ///
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gSpsPolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSpsPolicyInstance
                  );
  DEBUG ((DEBUG_INFO, "[DXE SPS ME POLICY] INFO: Install SPS ME Policy Protocol => (%r)\n", Status));

  DEBUG ((DEBUG_INFO, "[DXE SPS ME POLICY] INFO: SPS ME Policy Exit\n"));

  return Status;
}
