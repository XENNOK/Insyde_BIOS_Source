/** @file

  Server Platform Services Firmware ME module.

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

#include "SpsPei.h"

static EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyList = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiMemoryDiscoveredPpiGuid,
  SpsInitAfterMemoryInit
};

/**
  Save SPS Information in Hob

  @param[in] SpsInfoHobType       SPS Info Type.
  @param[in] SpsInfoData          SpsInfoData.

  @retval EFI_SUCCESS             Create Me Info Hob successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsSaveInfoHob (
  IN SPS_HOB_TYPE                      SpsInfoHobType,
  IN SPS_INFO_HOB                      *SpsInfoData
  )
{
  EFI_PEI_HOB_POINTERS       Hob;
  SPS_INFO_HOB               *SpsInfoHob;

  SpsInfoHob = NULL;

  ///
  /// Create ME Info HOB
  ///
  Hob.Raw = GetFirstGuidHob (&gSpsInfoHobGuid);
  if (Hob.Raw != NULL) {
    SpsInfoHob = (SPS_INFO_HOB *) GET_GUID_HOB_DATA (Hob);
  } else {
    SpsInfoHob = (SPS_INFO_HOB *) BuildGuidHob (&gSpsInfoHobGuid, sizeof (SPS_INFO_HOB));
    if (SpsInfoHob == NULL) {
      return EFI_INVALID_PARAMETER;
    } else {
      ZeroMem (SpsInfoHob, sizeof (SPS_INFO_HOB));
    }
  }

  switch (SpsInfoHobType) {

    case SpsInfomPhyVersion:
      SpsInfoHob->SpsmPhyVersion = SpsInfoData->SpsmPhyVersion;
      break;

    case SpsInfoMeSku:
      SpsInfoHob->SpsMeSku = SpsInfoData->SpsMeSku;
      break;

    case SpsInfoFwVersion:
      SpsInfoHob->SpsFwVersion.FwVerMajor = SpsInfoData->SpsFwVersion.FwVerMajor;
      SpsInfoHob->SpsFwVersion.FwVerMinor = SpsInfoData->SpsFwVersion.FwVerMinor;
      SpsInfoHob->SpsFwVersion.FwVerBuild = SpsInfoData->SpsFwVersion.FwVerBuild;
      SpsInfoHob->SpsFwVersion.FwVerPatch = SpsInfoData->SpsFwVersion.FwVerPatch;
      break;

    case SpsInfoMeBiosIntVer:
      SpsInfoHob->SpsMeBiosIntfVer.Major = SpsInfoData->SpsMeBiosIntfVer.Major;
      SpsInfoHob->SpsMeBiosIntfVer.Minor = SpsInfoData->SpsMeBiosIntfVer.Minor;
      break;

    case SpsInfoMeDisable:
      SpsInfoHob->SpsMeDisabled = SpsInfoData->SpsMeDisabled;
      break;

    case SpsInfoMeTimeout:
      SpsInfoHob->SpsMeTimeout = SpsInfoData->SpsMeTimeout;
      break;

    case SpsInfoMeCompatible:
      SpsInfoHob->SpsMeBiosCompat = SpsInfoData->SpsMeBiosCompat;
      break;

    case SpsInfoMeInRecoveru:
      SpsInfoHob->SpsMeInRecovery = SpsInfoData->SpsMeInRecovery;
      break;

    case SpsInfoBootingMode:
      SpsInfoHob->BootingMode = SpsInfoData->BootingMode;
      break;

    case SpsInfoCoreDisable:
      SpsInfoHob->CoreDisable = SpsInfoData->CoreDisable;
      break;

    case SpsInfoMax:
      break;

    default:
      break;
  }

  return EFI_SUCCESS;
}

/**
  Get SPS Information from SSP Info Hob

  @param[out] SpsInfoData         SpsInfoData.

  @retval EFI_SUCCESS             Create Me Info Hob successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetInfoHob (
  OUT SPS_INFO_HOB                     *SpsInfoData
  )
{
  EFI_PEI_HOB_POINTERS       Hob;
  SPS_INFO_HOB               *SpsInfoHob;

  SpsInfoHob = NULL;

  if (SpsInfoData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Create ME Info HOB
  ///
  Hob.Raw = GetFirstGuidHob (&gSpsInfoHobGuid);
  if (Hob.Raw == NULL) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SPS Info Hob not found.\n"));
    return EFI_NOT_FOUND;
  }

  SpsInfoHob = (SPS_INFO_HOB *) GET_GUID_HOB_DATA (Hob);
  if (SpsInfoHob == NULL) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SPS Info Hob found but data null.\n"));
    return EFI_NOT_FOUND;
  }

  CopyMem (SpsInfoData, SpsInfoHob, sizeof (SPS_INFO_HOB));

  return EFI_SUCCESS;
}

/**
  Check SPS ME is upgraded in previous boot.
  if TRUE, Do global reset.
  else do nothing.

  @param None.

  @retval EFI_SUCCESS             Create Me Info Hob successfully.
**/
EFI_STATUS
PeiMeUpgrade (
  VOID
  )
{
  EFI_STATUS                 Status;
  UINT32                     MeStatus;

  Status         = EFI_SUCCESS;
  MeStatus       = 0;

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.5  Safe update of entire ME region after EOP
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 16. When the request for BIOS POST phase to perform global reset is stored, the
  ///             update application may request normal host system reset to enter BIOS POST
  ///             phase.
  ///
  Status = SpsGetMeStatus (&MeStatus);
  if (IS_SPS_ME_CURSTATE_RECOVERY (MeStatus)) {
    if ((SpsUpgradedPreviousBoot ()) && SpsCheckMePolicyRevision ()) {
      ///
      /// SPS ME in recovery mode and upgraded in previous boot.
      /// Perform a global reset letting SPS ME back to normal mode.
      ///

      ///
      /// Clear ME upgraded signal in CMOS
      ///
      Status = SpsClearMeUpgradedSignal ();

      ///
      /// Perform a global reset
      ///
      DEBUG ((DEBUG_INFO, "[PEI SPS UPDATE] INFO: ME FW has been upgraded at previous boot, perform a global reset.\n\n"));
      SpsMeCF9Configure (GlobalReset);

    } else {
      DEBUG ((DEBUG_INFO, "[PEI SPS UPDATE] INFO: SPS ME is in recovery, but not caused by upgrade procedures.\n"));
      Status = EFI_DEVICE_ERROR;
    }
    DEBUG ((DEBUG_INFO, "[PEI SPS UPDATE] INFO: Continue POST with SPS ME recovery mode.\n"));

  }

  return Status;
}

/**
  Get Node Manager booting mode request. and save it to SpsInfoHob.

  @param None.

  @retval EFI_SUCCESS            Get Node Manager booting mode successfully
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmBootingMode (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 BiosBootingMode;
  SPS_INFO_HOB          SpsInfoHob;

  ZeroMem (&SpsInfoHob, sizeof (SPS_INFO_HOB));

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// Step (??) : Get NM Booting mode that request BIOS boot path.
  ///
  Status = SpsNmGetBootingMode (&BiosBootingMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// If Get Booting Mode Successfully,
  /// it mean SPS ME SKU is Node Manager.
  ///
  SpsInfoHob.SpsMeSku = SPS_SKU_NM;
  Status = SpsSaveInfoHob (SpsInfoMeSku, &SpsInfoHob);

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///   Step (??) : Save NM Booting Mode for BIOS perform.
  ///
  ///
  /// Save SPS ME INFO.
  ///
  SpsInfoHob.BootingMode = BiosBootingMode;
  Status = SpsSaveInfoHob (SpsInfoBootingMode, &SpsInfoHob);

  return EFI_SUCCESS;
}

/**
  Get Node Manager Cores disable request. and save it to SpsInfoHob.

  @param None.

  @retval EFI_SUCCESS            Get Node Manager booting mode successfully
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmCoresDisable (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 NumberOfCoreDisable;
  SPS_INFO_HOB          SpsInfoHob;

  ///
  /// Get Cores Disable from NMFS register in HECI-2.
  /// B0:D22:F1 40h [7:1] Number of Cores Disabled.
  ///
  Status = SpsNmGetCoresDisableNumber (&NumberOfCoreDisable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// If Get Cores Disable number Successfully,
  /// it mean SPS ME SKU is Node Manager.
  ///
  SpsInfoHob.SpsMeSku = SPS_SKU_NM;
  Status = SpsSaveInfoHob (SpsInfoMeSku, &SpsInfoHob);

  ///
  /// Save the number of physical processor cores
  /// that should be disabled on each processor pacakge.
  ///
  SpsInfoHob.CoreDisable = NumberOfCoreDisable;
  Status = SpsSaveInfoHob (SpsInfoCoreDisable, &SpsInfoHob);

  return EFI_SUCCESS;
}

/**
  Server Platform Services ME initial before memory initialization.

  @param[in] PeiServices         General purpose services available to every PEIM.

  @retval EFI_SUCCESS            Spsinit successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsInitBeforeMemoryInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                           Status;
  SPS_INFO_HOB                         SpsInfoHob;
  UINT16                               mPhyVersion;

  ZeroMem (&SpsInfoHob, sizeof (SPS_INFO_HOB));

  ///
  /// Check SPS ME Firmware Upgrade status.
  ///
  Status = PeiMeUpgrade ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: PEI ME Upgeade => (%r)\n", Status));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A1)  BIOS must initialize PCH power management by clearing the PCH wake reason 
  ///                in ME_WAKE_STS in the PRSTS configuration register see [EDS]. This bit must 
  ///                be cleared to allow platform shutdown.  
  ///

  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Wake event clear...\n"));
  Status = SpsMeClearWakeStatus ();

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A2) : BIOS reads the mode of booting from the NM Firmware Status register
  ///                 in HECI-2 interface and sets proper BIOS booting mode.
  ///                 Two modes are supported Performance and Power Optimized.
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: NM Booting Mode Request...\n"));
  Status = SpsNmBootingMode ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: NM Booting Mode Request (%r)\n", Status));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A3) : BIOS checks the version of mPhy survivability table stored by ME using
  ///                 communication over HECI registers as described in section 3.12.
  ///
  if (SpsmPhySurvivabilityProgrammingEnalbed ()) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Check mPhy table version...\n"));
    Status = SpsMeGetmPhySurvivabilityTablePreDID (&mPhyVersion);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SPS ME-BIOS mPhy Survivability Table Version: 0x%04x\n", mPhyVersion));
    ///
    /// Save mPhy Version to SPS ME INFO.
    ///
    SpsInfoHob.SpsmPhyVersion = mPhyVersion;
    Status = SpsSaveInfoHob (SpsInfomPhyVersion, &SpsInfoHob);
  } else {
    mPhyVersion = 0;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A6) : If NM is enabled BIOS reads from the NMFS the number of processor
  ///                 cores that should be disabled in each processor package
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: NM Cores Disable request...\n"));
  Status = SpsNmCoresDisable ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: SPS Get NM Cores Disable (%r)\n", Status));
  }

  SpsMeDeviceConfigure (SpsPeiBeforeMemoryInit);

  return Status;
}


/**
  Chcek ME Firmware Status #1 register to learn the state of the firmware running in me.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Heci initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsCheckMeFwInitState (
  VOID
  )
{
  EFI_STATUS            Status;
  BOOLEAN               IsInitComplete;
  UINT8                 MeFs1ErrorCode;
  SPS_INFO_HOB          SpsInfoHob;

  IsInitComplete = FALSE;
  MeFs1ErrorCode = 0x0;
  ZeroMem (&SpsInfoHob, sizeof (SPS_INFO_HOB));

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E1) : If the register indicates that ME firmware is initializing, BIOS returns to step (A4).
  ///                 The timeout to exit this loop is 2 seconds.
  ///
  Status = SpsCheckMeFwInit (&IsInitComplete, &MeFs1ErrorCode);
  if ((IsInitComplete) && (MeFs1ErrorCode == SPS_ME_ERRCODE_NOERROR)) {
    return EFI_SUCCESS;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E2) : If the register indicates that ME firmware is in one of the states:
  ///                 1. Hard ME-Disabled
  ///                 2. Non zero Error Code is reported
  ///                 3. 2 seconds timeout
  ///               BIOS continues boot procedure without communication with ME.
  ///               At the end BIOS needs to jump to step (E4) and disable ME functions on PCI.
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Check MEFS1...\n"));
  if (Status == EFI_TIMEOUT) {
    ///
    /// Save SPS ME INFO.
    ///
    SpsInfoHob.SpsMeTimeout = TRUE;
    Status = SpsSaveInfoHob (SpsInfoMeTimeout, &SpsInfoHob);
  }
  if ((MeFs1ErrorCode == SPS_ME_ERRCODE_DISABLED)) {
    ///
    /// Save SPS ME INFO.
    ///
    SpsInfoHob.SpsMeDisabled = TRUE;
    Status = SpsSaveInfoHob (SpsInfoMeDisable, &SpsInfoHob);
  }

  return EFI_TIMEOUT;
}

/**
  Send Dynamic Fusing Request to ME and wait for response, issue global reset if needed

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Dynamic Fusing Successfully.
  @retval All other error conditions encountered result in an ASSERT.
*/
EFI_STATUS
SpsDynamicFusing (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                 Status;
  SPS_DYNAMIC_FUSING_RSP     DynamicFusingMsg;
  UINT32                     SpsMeStatus;

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1 - 3.13.1  Dynamic fusing procedure
  ///  /* Dynamic fusing HECI messages */
  ///     The message is ignored by the ME firmware after the END_OF_POST HECI message is received.
  ///
  if (SpsIsAlreadyReceivedEndOfPost ()) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] INFO: SPS ME-BIOS already received EOP, Skip Dynamic Fusing command.\n"));
    return EFI_SUCCESS;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1 - 3.13.1  Dynamic fusing procedure
  ///  /* Dynamic fusing procedure */
  ///     Note: If ME runs in recovery mode, it does not handle the dynamic fusing HECI
  ///           message but triggers global platform reset on its own after it detects that reset is
  ///           necessary.
  ///
  Status = SpsGetMeStatus (&SpsMeStatus);
  if (EFI_ERROR (Status) || (IS_SPS_ME_CURSTATE_RECOVERY(SpsMeStatus))) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: SPS ME runs in recovery mode.", SpsMeStatus));
    DEBUG ((DEBUG_ERROR, "Do not send Dynamic fusing messang\n"));
    return EFI_NOT_READY;
  }

  Status = SpsHeciSendDynamicFusing (&DynamicFusingMsg.Result);

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Dynamic Fusing response result (0x%02x)\n", DynamicFusingMsg.Result));
    switch (DynamicFusingMsg.Result) {
    case CPU_DYNAMIC_FUSING_SUCCESS:
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Send Dynamic Fusing Request successed\n"));
      break;

    case CPU_DYNAMIC_FUSING_EOP_RECEIVED:
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: ME already received EOP!!\n"));
      break;

    case CPU_DYNAMIC_FUSING_GRST_REQUIRED:

      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: ME ask to do platform global reset!!\n"));
      SpsMeCF9Configure (GlobalReset);
      ///
      /// Sould not be here!!
      ///
      ASSERT (FALSE);
      CpuDeadLoop ();
      break;

    case CPU_DYNAMIC_FUSING_ROL_NOT_SUPPORTED_BY_PCH:
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Dynamic Fusing not support by PCH!!\n"));
      break;

    case CPU_DYNAMIC_FUSING_ROL_SOFT_DISABLED:
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Send Dynamic Fusing Request failed!!\n"));
      break;

    default:
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Unknowen ME response!!\n"));
      break;
    }
  }

  return Status;
}

/**
  Send Get Me-Bios Interface Version message to ME.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             Get Me-Bios Interface Version message Successfully.
  @retval EFI_INCOMPATIBLE_VERSION ME-BIOS Interface version incompatible.
*/
EFI_STATUS
SpsGetMeBiosInterfaceVersion (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                           Status;
  SPS_GET_ME_BIOS_INTERFACE_RSP        MeBiosInterfVer;
  SPS_INFO_HOB                         SpsInfoHob;
  
  ZeroMem (&MeBiosInterfVer, sizeof (SPS_GET_ME_BIOS_INTERFACE_RSP));
  Status = SpsHeciGetMeBiosInterfaceVersion ((VOID *)&MeBiosInterfVer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Get ME-BIOS Interface Version Fail! (%r).\n", Status));
    SpsInfoHob.SpsMeBiosCompat = FALSE;
    Status = SpsSaveInfoHob (SpsInfoMeCompatible, &SpsInfoHob);
    return EFI_INCOMPATIBLE_VERSION;
  } else {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SPS ME-BIOS interface Version is "));
    DEBUG ((DEBUG_INFO, "%d.%d\n", MeBiosInterfVer.Data.MajorVersion, MeBiosInterfVer.Data.MinorVersion));
    DEBUG ((DEBUG_INFO, "  Active Feature Set : 0x%04x\n", MeBiosInterfVer.Data.BitMask));
    DEBUG ((DEBUG_INFO, "    [0] - Node Manager support = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.NM));
    DEBUG ((DEBUG_INFO, "    [1] - PECI Proxy support   = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.PeciProxy));
    DEBUG ((DEBUG_INFO, "    [2] - ICC support          = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.ICC));
    DEBUG ((DEBUG_INFO, "    [3] - Reserved1            = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.Reserved1));
    DEBUG ((DEBUG_INFO, "    [4] - IDER Enabled         = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.IDER));
    DEBUG ((DEBUG_INFO, "    [5] - SOL Enabled          = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.SOL));
    DEBUG ((DEBUG_INFO, "    [6] - Network Enabled      = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.Network));
    DEBUG ((DEBUG_INFO, "    [7] - KVM Enabled          = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.KVM));
    DEBUG ((DEBUG_INFO, "    [8] - PMBus Proxy support  = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.PMBus));
    DEBUG ((DEBUG_INFO, " [9:15] - Reserved2            = 0x%x\n", MeBiosInterfVer.Data.BitMask.Fields.Reserved2));
  }
  
  ///
  /// Save SPS ME INFO.
  ///
  SpsInfoHob.SpsMeBiosIntfVer.Major = MeBiosInterfVer.Data.MajorVersion;
  SpsInfoHob.SpsMeBiosIntfVer.Minor = MeBiosInterfVer.Data.MinorVersion;
  Status = SpsSaveInfoHob (SpsInfoMeBiosIntVer, &SpsInfoHob);
  
  SpsInfoHob.SpsMeSku = MeBiosInterfVer.Data.BitMask.Fields.NM;
  Status = SpsSaveInfoHob (SpsInfoMeSku, &SpsInfoHob);

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E3) : If BIOS and ME firmware are not compatible, BIOS logs error for user,
  ///                 disables all HECI functions and jumps to step (E4) without any more communication with ME.
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Check ME-BIOS Interface Version compatible...\n"));
  if (SpsCheckMeBiosInterFaceVersion (MeBiosInterfVer.Data.MajorVersion, MeBiosInterfVer.Data.MinorVersion)) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: ME firmware is compatible.\n"));
    ///
    /// Save SPS ME INFO.
    ///
    SpsInfoHob.SpsMeBiosCompat = TRUE;
    Status = SpsSaveInfoHob (SpsInfoMeCompatible, &SpsInfoHob);
  } else {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: ME firmware is incompatible.\n"));

    ///
    /// Server Platform Services(SPS) Firmware ME-BIOS Interface
    ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
    ///
    ///  /* BIOS POST Requirements */
    ///     Step (E4) : It is expected that BIOS POST should inform the user about any problem
    ///                 with ME communication. At minimum MEFS1 and MEFS2 should be logged
    ///                 so user is warned about any problem with ME firmware initialization.
    ///
    ///
    /// If ME-BIOS is incompatible
    /// Hide All ME Device 22 F:0/1/2/3
    ///
    SpsMeDeviceConfigure (SpsPeiDisableAll);
    ///
    /// Save SPS ME INFO.
    ///
    SpsInfoHob.SpsMeBiosCompat = FALSE;
    Status = SpsSaveInfoHob (SpsInfoMeCompatible, &SpsInfoHob);
    return EFI_INCOMPATIBLE_VERSION;
  }

  return EFI_SUCCESS;
}

/**
  SPS ME mPhy Survivability table programming.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             mPhy Survivability table programming Successfully.
  @retval All other error conditions encountered result in an ASSERT.
*/
EFI_STATUS
SpsmPhySurvivabilityProgram (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                           Status;
  SPS_INFO_HOB                         SpsInfoData;
  UINT16                               mPhyVersion;
  UINT32                               ChipsetInitTableLen;
  UINT8                                *ChipsetInitTable;

  mPhyVersion         = 0;
  ChipsetInitTableLen = 0;
  ChipsetInitTable    = NULL;
  ZeroMem (&SpsInfoData, sizeof (SPS_INFO_HOB));

  Status = SpsGetInfoHob (&SpsInfoData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SpsGetInfoHob => (%r)\n"));
  } else {
    mPhyVersion = SpsInfoData.SpsmPhyVersion;
  }

  DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: SPS ME-BIOS mPhy Survivability Table Version: 0x%04x\n", mPhyVersion));

  if (SpsIsmPhySurvivabilityProgrammingRequired (mPhyVersion) ) {

    Status = SpsGetmPhySurvivabilityTableParameter (&ChipsetInitTableLen, &ChipsetInitTable);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: Get mPhy Survivability Table Parameter Status => (%r).\n", Status));
      return Status;
    } else {
      Status = SpsMemPhySurvivabilityProgramming (ChipsetInitTableLen, ChipsetInitTable);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: mPhy survivability table programming Status => (%r).\n", Status));
        return Status;
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: mPhy survivability table version match or version is nil.\n"));
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

/**
  SPS ME ICC programming.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             ICC programming Successfully.
  @retval All other error conditions encountered result in an ASSERT.
*/
EFI_STATUS
SpsIccProgramming (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS            Status;
  UINT32                IccEnables;
  UINT32                IccEnablesMask;
  UINT32                IccSSMSelect;
  UINT8                 ResponseMode;

  Status       = EFI_UNSUPPORTED;
  IccEnables   = 0;
  IccSSMSelect = 0;
  ResponseMode = 0;

  if (SpsIsAlreadyReceivedEndOfPost ()) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] INFO: SPS ME-BIOS already received EOP, Skip ICC command.\n"));
    return EFI_SUCCESS;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// Step (??) : TBD. Set Clock Enables
  ///
  if (SpsIccClockEnablesEnabled ()) {
    Status = SpsIccClockEnablesSetting (&IccEnables, &IccEnablesMask, &ResponseMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Get ICC Set Clock Enables Fail! (%r)\n", Status));
    }
    Status = SpsHeciSetIccClockEnables (IccEnables, IccEnablesMask, ResponseMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Send ICC Set Clock Enables Message Fail! (%r)\n", Status));
    }
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// Step (??) : TBD. Set Spread Spectrum
  ///
  if (SpsIccSpreadSpectrumEnabled ()) {
    Status = SpsIccSpreadSpectrumSetting (&IccSSMSelect, &ResponseMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Get ICC Set Spread Spectrum Configuration Fail! (%r).\n", Status));
    }
    Status = SpsHeciSetIccSpreadSpectrum (IccSSMSelect, ResponseMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Send ICC Set Spread Spectrum Configuration Message Fail! (%r).\n", Status));
    }
  }
  return Status;
}

/**
  Server Platform Services ME initial after memory initialization.

  @param[in] PeiServices         General purpose services available to every PEIM.

  @retval EFI_SUCCESS            Spsinit successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsInitAfterMemoryInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN       VOID                        *Ppi
  )
{
  EFI_STATUS                           Status;
  SPS_INFO_HOB                         SpsInfoHob;
  UINT32                               MeStatus;

  ZeroMem (&SpsInfoHob, sizeof (SPS_INFO_HOB));

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A4) : After BIOS configures QPI, memory, and MCTP it sends the
  ///                 DRAM_INIT_DONE (DID) message to ME.
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Send DRAM_INIT_DONE...\n"));
  Status = SpsMeConfigDidReg (SPS_DRAM_INIT_DONE_STS_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A6) : BIOS reads ME Firmware Status #1 register in HECI-1 interface to learn
  ///                 the state of the firmware running in ME (see Table 3-4).
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS(A6)(E1)(E2) Check ME firmware status...\n"));
  Status = SpsCheckMeFwInitState ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A7) : BIOS initializes HECI interfaces.
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: HECI initialization...\n"));
  Status = SpsHeciInit();

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A8) : If the ME Firmware Status #1 says that ME runs SPS firmware in
  ///                 operational or recovery mode (see Table 3-4 Current State) BIOS sends to
  ///                 ME "Get ME-BIOS Interface Version" message to check ME-BIOS
  ///                 interface definition compatibility and supported firmware features. If ME
  ///                 runs in recovery mode, all ME features are disabled.
  ///
  if (SpsMeBiosInterfaceVersionEnabled ()) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Get ME-BIOS Interface Version from ME...\n"));
    Status = SpsGetMeBiosInterfaceVersion (PeiServices);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Get ME-BIOS Interface Version Status => (%r).\n", Status));
      return Status;
    }
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E5) : BIOS sends dynamic fusing information to ME, see section 3.12.
  ///                 Dynamic fusing should be performed right after Memory Initialization exit 
  ///                 (this message may cause CPU reset).
  ///
  if (SpsDynamicFusingEnabled ()) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Check Dynamic Fusing Status...\n"));
    Status = SpsDynamicFusing (PeiServices);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Dynamic Fusing Status => (%r).\n", Status));
    }
  }
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E6) : If MEFS1.CurrentState indicates that ME firmware runs in recovery
  ///                 mode BIOS jumps to step (A13).
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: Check MEFS1...\n"));
  Status = SpsGetMeStatus (&MeStatus);
  if (IS_SPS_ME_CURSTATE_RECOVERY (MeStatus)) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] INFO ME in Recovery.\n"));
    ///
    /// Save SPS ME INFO.
    ///
    SpsInfoHob.SpsMeInRecovery = TRUE;
    Status = SpsSaveInfoHob (SpsInfoMeInRecoveru, &SpsInfoHob);
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "[PEI SPS] INFO: ME is operational.\n"));

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (E7) : If mPhy survivability table returned in step (A3) differs from the one programmed in BIOS,
  ///                 the new table is programmed and BIOS generates a Power Cycle Reset,
  ///                 which is promoted by ME to the Global Platform Reset.
  ///
  if (SpsmPhySurvivabilityProgrammingEnalbed ()) {
    DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: mPhy Survivability table programming...\n"));
    Status = SpsmPhySurvivabilityProgram (PeiServices);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: mPhy Survivability table programming => (%r).\n", Status));
    }
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A10) : BIOS can configure selected Integrated Clock parameters. (See section 3.11.) 
  ///                  This step is optional. It is not required when the ICC settings provided into 
  ///                   FITc during ME region image creation do not require any adjustments. 
  ///
  DEBUG ((DEBUG_INFO, "[PEI SPS] PROGRESS: ICC programming...\n"));
  Status = SpsIccProgramming (PeiServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: ICC programming Status => (%r).\n", Status));
  }

  return EFI_SUCCESS;
}

/**
  Initialize SPS ME.

  @param [in] FileHandle          FileHandle  Handle of the file being invoked.
  @param [in] PeiServices         General purpose services available to every PEIM.

  @retval EFI_SUCCESS             SPS module initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMePeimEntry (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS            Status;
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// SPS init before Memory Init.
  ///
  Status = SpsInitBeforeMemoryInit (PeiServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: SPS initialization before memory Init (%r)\n", Status));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  /// SPS init after Memory Init.
  ///
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS] ERROR: Create PPI notify (%r)\n", Status));
  }

  return Status;
}
