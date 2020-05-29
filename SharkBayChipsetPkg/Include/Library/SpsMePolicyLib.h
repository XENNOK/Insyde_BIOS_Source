/** @file

  Header file for SPS Me Policy functionality
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

#ifndef _SPS_ME_POLICY_LIB_H_
#define _SPS_ME_POLICY_LIB_H_

/**
  Check if Me is enabled.

  @param None.

**/
EFI_STATUS
SpsMePolicyLibInit (
  VOID
  );

///
/// SPS Silicon Enable Switch
///
/**
  Check if Dynamic Fusing message is enabled in setup options.

  @param None.

  @retval FALSE                   Dynamic Fusing message is disabled.
  @retval TRUE                    Dynamic Fusing message is enabled.
**/
BOOLEAN
SpsDynamicFusingEnabled (
  VOID
  );

/**
  Check if Me-Bios Interface Version message is enabled in setup options.

  @param None.

  @retval FALSE                   Me-Bios Interface Version message is disabled.
  @retval TRUE                    Me-Bios Interface Version message is enabled.
**/
BOOLEAN
SpsMeBiosInterfaceVersionEnabled (
  VOID
  );

/**
  Check if Set ICC Clock Enable message is enabled in setup options.

  @param None.

  @retval FALSE                   Set ICC Clock Enable message is disabled.
  @retval TRUE                    Set ICC Clock Enable message is enabled.
**/
BOOLEAN
SpsIccClockEnablesEnabled (
  VOID
  );

/**
  Check if Set ICC Spread Spectrum message is enabled in setup options.

  @param None.

  @retval FALSE                   Set ICC Spread Spectrum message is disabled.
  @retval TRUE                    Set ICC Spread Spectrum message is enabled.
**/
BOOLEAN
SpsIccSpreadSpectrumEnabled (
  VOID
  );

/**
  Check if END_OF_POST Message is enabled.

  @param None.

  @retval FALSE                   END_OF_POST Message is disabled.
  @retval TRUE                    END_OF_POST Message is enabled.
**/
BOOLEAN
SpsEndOfPostEnabled (
  VOID
  );

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
  );

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
  );

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
  );

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
  );

/**
  Check SPS Me fw upgrade in previous boot.

  @param None.

  @retval FALSE                   No upgraded.
  @retval TRUE                    FwUpgrade has upgraded.
**/
BOOLEAN
SpsUpgradedPreviousBoot (
  VOID
  );

/**
  Clear SPS Me fw upgrade signal.

  @param None.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsClearMeUpgradedSignal (
  VOID
  );

/**
  Get offset of ME upgraded in CMOS Bank 1 or 2

  @param None.

  @retval Offset settings of Nonce Store Variable Guid
**/
UINT16
SpsGetMeUpgradeCmosOffset (
  VOID
  );

/**
  Check if MeFwUpgrade is enabled in setup options.

  @param None.

  @retval FALSE                   MeFwUpgrade is disabled.
  @retval TRUE                    MeFwUpgrade is enabled.
**/
BOOLEAN
SpsMeFwUpgradeSupported (
  VOID
  );

/**
  Check if Protect Descriptor Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Descriptor Region is disabled.
  @retval TRUE                    Protect Descriptor Region is enabled.
**/
BOOLEAN
SpsMeProtectDescriptorRegion (
  VOID
  );

/**
  Check if Protect ME Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect ME Region is disabled.
  @retval TRUE                    Protect ME Region is enabled.
**/
BOOLEAN
SpsMeProtectMeRegion (
  VOID
  );

/**
  Check if Protect GBE Region is enabled in setup options.

  @param None.

  @retval FALSE                   Protect GBE Region is disabled.
  @retval TRUE                    Protect GBE Region is enabled.
**/
BOOLEAN
SpsMeProtectGBE (
  VOID
  );

/**
  Check if Protect Platform Data Region(PDR) is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Platform Data Region(PDR) is disabled.
  @retval TRUE                    Protect Platform Data Region(PDR) is enabled.
**/
BOOLEAN
SpsMeProtectPDR (
  VOID
  );

/**
  Check if Protect Device Expansion Region(DER) is enabled in setup options.

  @param None.

  @retval FALSE                   Protect Device Expansion Region(DER) is disabled.
  @retval TRUE                    Protect Device Expansion Region(DER) is enabled.
**/
BOOLEAN
SpsMeProtectDER (
  VOID
  );

/**
  Check if SPI Lock is enabled in setup options.

  @param None.

  @retval FALSE                   SPI Lock is disabled.
  @retval TRUE                    SPI Lock is enabled.
**/
BOOLEAN
SpsMeSpiLockSupported (
  VOID
  );

/**
  Notify SPS ME PEIM that ME has been upgraded through CMOS.

  @param None

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsNotifyMeUpgradedByCmos (
  VOID
  );

/**
  Get Factory default area size.

  @param[out] FactoryDefaultSize  SPS ME factory default size.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsUpgradeGetFactoryDefaultSize (
  OUT UINT32                           *FactoryDefaultSize
  );

/**
  Get Operation image size.

  @param[out] OperationImageSize  SPS ME operation imamge size.

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsUpgradeGetOperationImageSize (
  OUT UINT32                           *OperationImageSize
  );

/**
  Check upgrade mode is block mode.

  @param None.

  @retval TRUE                    SPS ME upgrade by multi SMI.
  @retval FALSE                   SPS ME upgrade by one SMI.
**/
BOOLEAN
SpsUpgradeIsBlockMode (
  VOID
  );

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
  );

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
  );

/**
  Provide Host Configuration data from sps policy

  @param[out] HostConfigureData   Host Configuration

  @retval EFI_SUCCESS             Function normally.
  @retval EFI_NOT_STARTED         SPS Policy not ready.
**/
EFI_STATUS
SpsNmPrepareHostConfigureData (
  OUT VOID                             *HostConfigureData
  );

/**
  Check if mPhy Survivability message is enabled in booting flow.

  @param None.

  @retval FALSE                   Set mPhy Survivability message is disabled.
  @retval TRUE                    Set mPhy Survivability message is enabled.
**/
BOOLEAN
SpsmPhySurvivabilityProgrammingEnalbed (
  VOID
  );

/**
  Check if mPhy Survivability message is required in booting flow.

  @param[in] CurrentmPhyVersion   Current mPhy Survivability Table Version.

  @retval FALSE                   Set mPhy Survivability message is skipped.
  @retval TRUE                    Set mPhy Survivability message is required.
**/
BOOLEAN
SpsIsmPhySurvivabilityProgrammingRequired (
  IN UINT16                            CurrentmPhyVersion
  );


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
  );

#endif
