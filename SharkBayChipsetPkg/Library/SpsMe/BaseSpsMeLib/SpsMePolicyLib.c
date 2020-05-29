/** @file

  Implementation file for SPS ME Policy functionality
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

#include <SpsMe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/SpsPolicyLib.h>

/**
  Initialize SPS ME Policy library.

  @param None.

**/
EFI_STATUS
SpsMePolicyLibInit (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Check if Dynamic Fusing message is enabled in setup options.

  @param None.

  @retval FALSE                   Dynamic Fusing message is disabled.
  @retval TRUE                    Dynamic Fusing message is enabled.
**/
BOOLEAN
SpsDynamicFusingEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsDynamicFusingEnalbed;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  IsDynamicFusingEnalbed = SpsMePolicy->MsgSwitch.DynamicFusing;

  return IsDynamicFusingEnalbed;
}

/**
  Check if Me-Bios Interface Version message is enabled in setup options.

  @param None.

  @retval FALSE                   Me-Bios Interface Version message is disabled.
  @retval TRUE                    Me-Bios Interface Version message is enabled.
**/
BOOLEAN
SpsMeBiosInterfaceVersionEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsMeBiosInterfaceVersionEnalbed;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  IsMeBiosInterfaceVersionEnalbed = SpsMePolicy->MsgSwitch.MeBiosInterfaceVersion;

  return IsMeBiosInterfaceVersionEnalbed;
}

/**
  Check if Set ICC Clock Enable message is enabled in setup options.

  @param None.

  @retval FALSE                   Set ICC Clock Enable message is disabled.
  @retval TRUE                    Set ICC Clock Enable message is enabled.
**/
BOOLEAN
SpsIccClockEnablesEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsIccClockEnablesEnabled;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  IsIccClockEnablesEnabled = SpsMePolicy->MsgSwitch.IccSetClockEnables;

  return IsIccClockEnablesEnabled;
}

/**
  Check if Set ICC Spread Spectrum message is enabled in setup options.

  @param None.

  @retval FALSE                   Set ICC Spread Spectrum message is disabled.
  @retval TRUE                    Set ICC Spread Spectrum message is enabled.
**/
BOOLEAN
SpsIccSpreadSpectrumEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsIccSpreadSpectrumEnabled;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  IsIccSpreadSpectrumEnabled = SpsMePolicy->MsgSwitch.IccSetSpreadSpectrum;

  return IsIccSpreadSpectrumEnabled;
}

/**
  Check if END_OF_POST Message is enabled.

  @param None.

  @retval FALSE                   END_OF_POST Message is disabled.
  @retval TRUE                    END_OF_POST Message is enabled.
**/
BOOLEAN
SpsEndOfPostEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsEopEnabled;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MsgSwitch.EndOfPost) {
    IsEopEnabled = TRUE;
  } else {
    IsEopEnabled = FALSE;
  }
  DEBUG ((DEBUG_INFO, "[SPS ME POLICY LIB] INFO: SPS Me Policy END_OF_POST is %a\n", IsEopEnabled ? "Enabled" : "Disabled"));

  return IsEopEnabled;
}

///
/// SPS Silicon Enable Information
///
/**
  Provide ICC Clock Enable Setting from sps policy

  @param[in, out] Enables         Clock Enables
  @param[in, out] EnablesMask     Clock Enables mask
  @param[in, out] ResponseMode    Response Mode

  @retval EFI_INVALID_PARAMETER   Parameter has invaild
  @retval EFI_SUCCESS             ICC Clock Enable setting complete.
**/
EFI_STATUS
SpsIccClockEnablesSetting (
  IN OUT UINT32                        *Enables,
  IN OUT UINT32                        *EnablesMask,
  IN OUT UINT8                         *ResponseMode
  )
{
  SPS_POLICY                           *SpsMePolicy;
  EFI_STATUS                           Status;
  SPS_ICC_SET_CLOCK_ENABLES_BITS       IccClockenables;
  SPS_ICC_SET_CLOCK_ENABLES_MASK       IccClockenablesMask;

  if ((Enables == NULL) || (EnablesMask == NULL) || (ResponseMode == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  SpsMePolicy                = NULL;
  IccClockenables.UInt32     = 0x00000000;
  IccClockenablesMask.UInt32 = 0x00000000;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return EFI_NOT_STARTED;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// Step (??) : TBD. now is default vaule. table 3-30 Clock Enables
  ///
  /// Update ICC Clock enables settin
  ///
  IccClockenables.UInt32     = SpsMePolicy->IccSetting.IccClockEnables.UInt32;
  IccClockenablesMask.UInt32 = SpsMePolicy->IccSetting.IccClockEnablesMask.UInt32;

  *Enables      = IccClockenables.UInt32;
  *EnablesMask  = IccClockenablesMask.UInt32;

  /// 0: Response is expected. 1: No response is expected.
  *ResponseMode = SpsMePolicy->IccSetting.IccClockEnablesSkipResponse;

  return EFI_SUCCESS;
}

/**
  Provide ICC Spread Spectrum Setting from sps policy

  @param[in, out] IccSSMSelect    Spread Spectrum Mode Select
  @param[in, out] ResponseMode    Response Mode

  @retval EFI_INVALID_PARAMETER   Parameter has invaild
  @retval EFI_SUCCESS             ICC Spread Spectrum setting complete.
**/
EFI_STATUS
SpsIccSpreadSpectrumSetting (
  IN OUT UINT32                        *IccSSMSelect,
  IN OUT UINT8                         *ResponseMode
  )
{
  SPS_POLICY                           *SpsMePolicy;
  EFI_STATUS                           Status;
  SPS_ICC_SPREAD_SPECTRUM_SELECT       IccSpreadSpectrum;

  if ((IccSSMSelect == NULL) || (ResponseMode == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  SpsMePolicy              = NULL;
  IccSpreadSpectrum.UInt32 = 0x00000000;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return EFI_NOT_STARTED;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// Step (??) : TBD.
  ///
  /// Update ICC Spread spectrum setting
  ///
  IccSpreadSpectrum.UInt32 = SpsMePolicy->IccSetting.IccSpreadSpectrum.UInt32;

  *IccSSMSelect = IccSpreadSpectrum.UInt32;
  /// 0: Response is expected. 1: No response is expected.
  *ResponseMode = SpsMePolicy->IccSetting.IccSpreadSpectrumSkipResponse;

  return EFI_SUCCESS;
}

///
/// SPS ME FW Update
///
/**
  Check if SPS ME Policy revision is match.

  @param None.

  @retval FALSE                   SPS ME Policy revision is mismatch.
  @retval TRUE                    SPS ME Policy revision is match.
**/
BOOLEAN
SpsCheckMePolicyRevision (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsValidRevision;

  SpsMePolicy     = NULL;
  IsValidRevision = FALSE;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->Revision >= SPS_POLICY_REVISION) {
    IsValidRevision = TRUE;
  } else {
    IsValidRevision = FALSE;
  }

  return IsValidRevision;
}

/**
  Check if SPS ME-BIOS Interface version is match.

  @param[in] MajorVersion         ME-BIOS Interface Major Version from SPS ME Firmware.
  @param[in] MinorVersion         ME-BIOS Interface Minor Version from SPS ME Firmware.

  @retval FALSE                   SPS ME ME-BIOS Interface version is not match.
  @retval TRUE                    SPS ME ME-BIOS Interface version is match.
**/
BOOLEAN
SpsCheckMeBiosInterFaceVersion (
  IN UINT8                             MajorVersion,
  IN UINT8                             MinorVersion
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsVersionMatch;

  SpsMePolicy    = NULL;
  IsVersionMatch = FALSE;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if ((SpsMePolicy->MeBiosIntfVer.Major == MajorVersion) && (SpsMePolicy->MeBiosIntfVer.Minor == MinorVersion)) {
    IsVersionMatch = TRUE;
  } else {
    IsVersionMatch = FALSE;
  }

  return IsVersionMatch;
}

/**
  Check SPS Me fw upgrade in previous boot.

  @param None.

  @retval FALSE                   No upgraded.
  @retval TRUE                    FwUpgrade has upgraded.
**/
BOOLEAN
SpsUpgradedPreviousBoot (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  UINT8                      Data8;
  UINT8                      CmosIndexPort;
  UINT8                      CmosDataPort;
  UINT8                      CmosOffset;
  BOOLEAN                    IsMeFlashed;

  SpsMePolicy = NULL;
  Data8       = 0;
  IsMeFlashed = FALSE;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  CmosIndexPort = SpsMePolicy->MeFwUpgradeConfig.CmosIndexPort;
  CmosDataPort  = SpsMePolicy->MeFwUpgradeConfig.CmosDataPort;
  CmosOffset    = SpsMePolicy->MeFwUpgradeConfig.CmosOffset;
  DEBUG ((DEBUG_INFO, "[SPS ME POLICY LIB] INFO: CMOS Index Port: 0x%02x\n", CmosIndexPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Data Port : 0x%02x\n", CmosDataPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Offset    : 0x%02x\n", CmosOffset));

  if (CmosDataPort > 0x7f) {
    return FALSE;
  }

  IoWrite8 (CmosIndexPort, CmosOffset);
  Data8 = IoRead8 (CmosDataPort);

  IsMeFlashed = (Data8 == SPS_ME_FW_UPGRADED_CMOS_SIG ? TRUE : FALSE);
  DEBUG ((DEBUG_INFO,"[SPS ME POLICY LIB] INFO: ME Fimware Upgraded Previous Boot is %a\n", IsMeFlashed ? "TRUE" : "FALSE"));

  return IsMeFlashed;
}

/**
  Clear SPS Me fw upgrade signal.

  @param None.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsClearMeUpgradedSignal (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  UINT8                      CmosIndexPort;
  UINT8                      CmosDataPort;
  UINT8                      CmosOffset;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return EFI_NOT_STARTED;
  }

  CmosIndexPort = SpsMePolicy->MeFwUpgradeConfig.CmosIndexPort;
  CmosDataPort  = SpsMePolicy->MeFwUpgradeConfig.CmosDataPort;
  CmosOffset    = SpsMePolicy->MeFwUpgradeConfig.CmosOffset;

  if (CmosDataPort > 0x7F) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "[SPS ME POLICY LIB] INFO: CMOS Index Port: 0x%02x\n", CmosIndexPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Data Port : 0x%02x\n", CmosDataPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Offset    : 0x%02x\n", CmosOffset));

  IoWrite8 (CmosIndexPort, CmosOffset);
  IoWrite8 (CmosDataPort, 0x00);

  return EFI_SUCCESS;
}

/**
  Get offset of ME upgraded in CMOS Bank 1 or 2

  @param None.

  @retval Offset settings of Nonce Store Variable Guid
**/
UINT16
SpsGetMeUpgradeCmosOffset (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return 0xFFFF;
  }

  return (*(UINT16 *) &(SpsMePolicy->MeFwUpgradeConfig.CmosOffset));
}

/**
  Check if MeFwUpgrade is enabled in setup options.

  @param None.

  @retval FALSE                   MeFwUpgrade is disabled.
  @retval TRUE                    MeFwUpgrade is enabled.
**/
BOOLEAN
SpsMeFwUpgradeSupported (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsFwUpgradeSupported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.UpgradeSupported) {
    IsFwUpgradeSupported = TRUE;
  } else {
    IsFwUpgradeSupported = FALSE;
  }
  return IsFwUpgradeSupported;
}

/**
  Check if Protect Descriptor Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Descriptor Region is disabled.
  @retval TRUE                    Protect Descriptor Region is enabled.
**/
BOOLEAN
SpsMeProtectDescriptorRegion (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.ProtectDescriptorRegion) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if Protect ME Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect ME Region is disabled.
  @retval TRUE                    Protect ME Region is enabled.
**/
BOOLEAN
SpsMeProtectMeRegion (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.ProtectMeRegion) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if Protect GBE Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect GBE Region is disabled.
  @retval TRUE                    Protect GBE Region is enabled.
**/
BOOLEAN
SpsMeProtectGBE (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.ProtectGbeRegion) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if Protect Platform Data Region(PDR) is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Platform Data Region(PDR) is disabled.
  @retval TRUE                    Protect Platform Data Region(PDR) is enabled.
**/
BOOLEAN
SpsMeProtectPDR (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.ProtectPDRRegion) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if Protect Device Expansion Region(DER) is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Device Expansion Region(DER) is disabled.
  @retval TRUE                    Protect Device Expansion Region(DER) is enabled.
**/
BOOLEAN
SpsMeProtectDER (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.ProtectDERRegion) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}


/**
  Check if SPI Lock is enabled in setup options.

  @param None.

  @retval FALSE                   SPI Lock is disabled.
  @retval TRUE                    SPI Lock is enabled.
**/
BOOLEAN
SpsMeSpiLockSupported (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    Supported;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.SpiLock) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Notify SPS ME PEIM that ME has been upgraded through CMOS.

  @param None

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsNotifyMeUpgradedByCmos (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  UINT8                      CmosIndexPort;
  UINT8                      CmosDataPort;
  UINT8                      CmosOffset;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return Status;
  }

  CmosIndexPort = SpsMePolicy->MeFwUpgradeConfig.CmosIndexPort;
  CmosDataPort  = SpsMePolicy->MeFwUpgradeConfig.CmosDataPort;
  CmosOffset    = SpsMePolicy->MeFwUpgradeConfig.CmosOffset;
  DEBUG ((DEBUG_INFO, "[SPS ME POLICY LIB] INFO: CMOS Index Port: 0x%02x\n", CmosIndexPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Data  Port: 0x%02x\n", CmosDataPort));
  DEBUG ((DEBUG_INFO, "                          CMOS Offset    : 0x%02x\n", CmosOffset));

  // Check does policy settings of Nonce Store Guid Variable valid
  if (CmosDataPort > 0x7f) {
    DEBUG ((DEBUG_ERROR, "[SPS ME POLICY LIB] ERROR: Offset of SPS ME upgraded flag is invalid, unsupported!\n"));
    return EFI_INVALID_PARAMETER;
  }

  IoWrite8 (CmosIndexPort, CmosOffset);
  IoWrite8 (CmosDataPort, SPS_ME_FW_UPGRADED_CMOS_SIG);

  DEBUG ((DEBUG_INFO, "[SPS ME POLICY LIB] INFO: Store SPS ME upgraded flag to CMOS\n"));

  return EFI_SUCCESS;
}

/**
  Get Factory default area size.

  @param[out] FactoryDefaultSize  SPS ME factory default size.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsUpgradeGetFactoryDefaultSize (
  OUT UINT32                           *FactoryDefaultSize
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return Status;
  }

  *FactoryDefaultSize = SpsMePolicy->MeFwUpgradeConfig.FactoryDefaultSize;

  return EFI_SUCCESS;
}

/**
  Get Operation image size.

  @param[out] OperationImageSize  SPS ME operation imamge size.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsUpgradeGetOperationImageSize (
  OUT UINT32                           *OperationImageSize
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return Status;
  }

  *OperationImageSize = SpsMePolicy->MeFwUpgradeConfig.OperationImageSize;

  return EFI_SUCCESS;
}

/**
  Check upgrade mode is block mode.

  @param None.

  @retval TRUE                    SPS ME upgrade by multi SMI.
  @retval FALSE                   SPS ME upgrade by one SMI.
**/
BOOLEAN
SpsUpgradeIsBlockMode (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsBlockMode;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.BlockMode) {
    IsBlockMode = TRUE;
  } else {
    IsBlockMode = FALSE;
  }
  return IsBlockMode;
}

/**
  Check SPS ME downgrade is support.
  If TRUE, Old ME can be updated.

  @param None.

  @retval TRUE                    SPS ME upgrade downgrade suppoer.
  @retval FALSE                   SPS ME upgrade downgrade unsuppoer.
**/
BOOLEAN
SpsUpgradeDowngradeSupported (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsCheckMeOffset;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MeFwUpgradeConfig.Downgrade) {
    IsCheckMeOffset = TRUE;
  } else {
    IsCheckMeOffset = FALSE;
  }
  return IsCheckMeOffset;
}

///
/// SPS Node Manager
///
/**
  Check if Host Cinfiguration message is enabled in setup options.

  @param None.

  @retval FALSE                   Host Cinfiguration message is disabled.
  @retval TRUE                    Host Cinfiguration message is enabled.
**/
BOOLEAN
SpsNmHostConfigEnabled (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsNmHostConfigEnabled;

  SpsMePolicy           = NULL;
  IsNmHostConfigEnabled = FALSE;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  if (SpsMePolicy->MsgSwitch.HostConfiguration) {
    IsNmHostConfigEnabled = TRUE;
  } else {
    IsNmHostConfigEnabled = FALSE;
  }

  DEBUG ((DEBUG_INFO,"[SPS ME POLICY LIB] INFO: SPS Me Policy NM Host Configuration Message is %a\n", IsNmHostConfigEnabled ? "Enabled" : "Disabled"));

  return IsNmHostConfigEnabled;
}

/**
  Provide Host Configuration data from sps policy

  @param[out] HostConfigureData   Host Configuration

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsNmPrepareHostConfigureData (
  OUT VOID                             *HostConfigureData
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  if (HostConfigureData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return EFI_NOT_STARTED;
  }

  CopyMem (HostConfigureData, &(SpsMePolicy->HostInfo), sizeof (SPS_NM_HOST_CONFIGURATION_DATA));

  return EFI_SUCCESS;;
}

/**
  Check if mPhy Survivability message is enabled in booting flow.

  @param None.

  @retval FALSE                   Set mPhy Survivability message is disabled.
  @retval TRUE                    Set mPhy Survivability message is enabled.
**/
BOOLEAN
SpsmPhySurvivabilityProgrammingEnalbed (
  VOID
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  BOOLEAN                    IsmPhySurvivabilityEnalbed;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  IsmPhySurvivabilityEnalbed = SpsMePolicy->MsgSwitch.mPhySurvivability;

  return IsmPhySurvivabilityEnalbed;
}

/**
  Check if mPhy Survivability message is required in booting flow.

  @param[in] CurrentmPhyVersion   Current mPhy Survivability Table Version.

  @retval FALSE                   Set mPhy Survivability message is skipped.
  @retval TRUE                    Set mPhy Survivability message is required.
**/
BOOLEAN
SpsIsmPhySurvivabilityProgrammingRequired (
  IN UINT16                            CurrentmPhyVersion
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return FALSE;
  }

  return (CurrentmPhyVersion != SpsMePolicy->mPhyTableVer) && (CurrentmPhyVersion != 0);
}


/**
  Get mPhy Survivability Table Parameter.

  @param[out] ChipsetInitTable    The required system ChipsetInit Table.
  @param[out] ChipsetInitTableLen Length of the table in bytes

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsGetmPhySurvivabilityTableParameter (
  OUT UINT32                           *ChipsetInitTableLen,
  OUT UINT8                            **ChipsetInitTable
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  if ((ChipsetInitTableLen == NULL) || (ChipsetInitTable == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  SpsMePolicy = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if ((EFI_ERROR (Status)) || (SpsMePolicy == NULL)) {
    return EFI_NOT_STARTED;
  }

  *ChipsetInitTableLen  = SpsMePolicy->mPhyTableLen;
  *ChipsetInitTable     = &SpsMePolicy->mPhyTableData[0];

  return EFI_SUCCESS;
}

