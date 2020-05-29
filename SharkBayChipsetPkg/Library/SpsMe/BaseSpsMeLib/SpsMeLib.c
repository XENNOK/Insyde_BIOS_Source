/** @file

  Implementation file for SPS ME functionality
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

#include <Library/DebugLib.h>
#include <Library/TimerLib.h>

#include <Library/SpsMeChipsetLib.h>
#include <Library/SpsMeLib.h>

/**
  Get SPS ME HECI#1 BAR.

  @param[out] Heci1Bar            SPS ME HECI#1 BAR.

  @retval EFI_SUCCESS             Get SPS ME HECI#1 BAR successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetHeci1Bar (
  OUT UINT64                           *Heci1Bar
  )
{
  EFI_STATUS            Status;
  UINT64                TempHeci1Bar;

  if (Heci1Bar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsMeChipsetGetHeci1Bar (&TempHeci1Bar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Heci1Bar = TempHeci1Bar;

  return EFI_SUCCESS;
}

/**
  Get SPS ME HECI#2 BAR.

  @param[out] Heci2Bar            SPS ME HECI#2 BAR.

  @retval EFI_SUCCESS             Get SPS ME HECI#2 BAR successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetHeci2Bar (
  OUT UINT64                           *Heci2Bar
  )
{
  EFI_STATUS            Status;
  UINT64                TempHeci2Bar;

  if (Heci2Bar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsMeChipsetGetHeci2Bar (&TempHeci2Bar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Heci2Bar = TempHeci2Bar;

  return EFI_SUCCESS;
}

/**
  Get SPS ME mode.

  @param[out] MeMode              SPS ME mode.

  @retval EFI_SUCCESS             Get SPS ME mode successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetMeMode (
  OUT UINT32                           *MeMode
  )
{
  SPS_MEFS1             MeFs1;


  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SpsGetMeFs1 (&MeFs1.UInt32);

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: SPS ME Mode: "));
  switch (MeFs1.Bits.OperatingMode) {
    case SPS_ME_OPMODE_NORMAL:
      *MeMode = SPS_ME_OPMODE_NORMAL;
      DEBUG ((DEBUG_INFO, "Client Firmware(%x)\n", MeFs1.Bits.OperatingMode));
      break;

    case SPS_ME_OPMODE_SPS:
      *MeMode = SPS_ME_OPMODE_SPS;
      DEBUG ((DEBUG_INFO, "SPS Firmware(%x)\n", MeFs1.Bits.OperatingMode));
      break;

    case SPS_ME_OPMODE_DEBUG:
      *MeMode = SPS_ME_OPMODE_DEBUG;
      DEBUG ((DEBUG_INFO, "Debug Mode(%x)\n", MeFs1.Bits.OperatingMode));
      break;

    case SPS_ME_OPMODE_IGNITION:
//    case SPS_ME_OPMODE_ALT_DISABLED:
    case SPS_ME_OPMODE_TEMP_DISABLE:
    case SPS_ME_OPMODE_SECOVR_JMPR:
    case SPS_ME_OPMODE_SECOVR_MSG:
    default:
      *MeMode = SPS_ME_OPMODE_NOT_SPS;
      DEBUG ((DEBUG_INFO, "Not SPS firmware(%x)\n", MeFs1.Bits.OperatingMode));
      break;
  }

  return EFI_SUCCESS;
}

/**
  Get SPS ME status

  @param[out] MeStatus            SPS ME Status.

  @retval EFI_SUCCESS             Get SPS ME Status successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetMeStatus (
  OUT UINT32                           *MeStatus
  )
{
  SPS_MEFS1                  MeFs1;
  SPS_MEFS2                  MeFs2;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SpsGetMeFs1 (&MeFs1.UInt32);

  if (IS_SPS_ME_CURSTATE_RECOVERY (MeFs1.UInt32)) {
    SpsGetMeFs2 (&MeFs2.UInt32);
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME in Recovery mode. (Cause : %x)\n", MeFs2.Bits.RecoveryCause));
  }

  *MeStatus = MeFs1.UInt32;

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME Firmware Status : %x\n", *MeStatus));

 return EFI_SUCCESS;
}

/**
  Clear SPS ME wake status.

  @param None.

  @retval EFI_SUCCESS             Allways return success.
**/
EFI_STATUS
SpsMeClearWakeStatus (
  VOID
  )
{
  SpsMeChipsetClearWakeStatus ();

  return EFI_SUCCESS;
}

/**
  Check SPS ME firmware is init done.

  @param[out] IsInitComplete      SPS ME init done.
  @param[out] ErrorCode           Error code.

  @retval EFI_SUCCESS             Get version successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsCheckMeFwInit (
  OUT BOOLEAN                          *IsInitComplete,
  OUT UINT8                            *ErrorCode
  )
{
  SPS_MEFS1                  MeFs1;
  BOOLEAN                    InitCompleteFlag;
  UINT32                     Delay;

  if ((IsInitComplete == NULL) || (ErrorCode == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  InitCompleteFlag = FALSE;

  Delay = 40; /// 50ms * 40 = 2sec
  do {
    SpsGetMeFs1 (&MeFs1.UInt32);
    if (MeFs1.Bits.InitComplete) {
      InitCompleteFlag = TRUE;
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] ME Firmware init complete\n"));
      break;
    }
    ///
    /// Server Platform Services(SPS) Firmware ME-BIOS Interface
    ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
    ///
    ///  /* BIOS POST Requirements */
    ///     Step (E1) : If the register indicates that ME firmware is initializing, BIOS returns to step (A4).
    ///                 The timeout to exit this loop is 2 seconds.
    ///
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] PROGRESS(E1): Initialization < 2 Seconds wait (%x)\n", Delay));
    ///
    /// Delay 50ms.
    ///
    MicroSecondDelay (5000);
    Delay--;
  } while (Delay != 0);

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME Firmware Status #1 (0x%08x)\n", MeFs1.UInt32));
  DEBUG ((DEBUG_INFO, "    [3:0] Current State               = %x\n", MeFs1.Bits.CurrentState));
  DEBUG ((DEBUG_INFO, "      [4] Manufacturing Mode          = %x\n", MeFs1.Bits.ManufacturingMode));
  DEBUG ((DEBUG_INFO, "      [5] FPT or Factory Defaults Bad = %x\n", MeFs1.Bits.FptBad));
  DEBUG ((DEBUG_INFO, "    [8:6] Operating State             = %x\n", MeFs1.Bits.OperatingState));
  DEBUG ((DEBUG_INFO, "      [9] Init Complete               = %x\n", MeFs1.Bits.InitComplete));
  DEBUG ((DEBUG_INFO, "     [10] Recovery BUP Load Fault     = %x\n", MeFs1.Bits.FtBupLdFlr));
  DEBUG ((DEBUG_INFO, "     [11] Update in Progress          = %x\n", MeFs1.Bits.UpdateInprogress));
  DEBUG ((DEBUG_INFO, "  [15:12] Error Code                  = %x\n", MeFs1.Bits.ErrorCode));
  DEBUG ((DEBUG_INFO, "  [19:16] Operating Mode              = %x\n", MeFs1.Bits.OperatingMode));
  DEBUG ((DEBUG_INFO, "  [24:20] Reserved                    = %x\n", MeFs1.Bits.Reserved));
  DEBUG ((DEBUG_INFO, "  [27:25] BIOS MSG ACK Data           = %x\n", MeFs1.Bits.MsgAckData));
  DEBUG ((DEBUG_INFO, "  [31:28] BIOS MSG ACK                = %x\n", MeFs1.Bits.MsgAck));

  if (MeFs1.Bits.ErrorCode != 0) {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: MEFS1 = 0x%04x\n", MeFs1.Bits.ErrorCode));

    switch (MeFs1.Bits.ErrorCode) {

      case SPS_ME_ERRCODE_NOERROR:
        DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME firmware no Error.\n"));
        *ErrorCode = SPS_ME_ERRCODE_NOERROR;
        break;

      case SPS_ME_ERRCODE_UNKNOWN:
        DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME firmware has experienced an uncategorized error.\n"));
        *ErrorCode = SPS_ME_ERRCODE_UNKNOWN;
        break;

      case SPS_ME_ERRCODE_DISABLED:
        DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME Firmware was disabled on this platform.\n"));
        *ErrorCode = SPS_ME_ERRCODE_DISABLED;
        break;

      case SPS_ME_ERRCODE_IMAGE_FAIL:
        DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME  ME firmware stored in the system flash is not valid.\n"));
        *ErrorCode = SPS_ME_ERRCODE_IMAGE_FAIL;
        break;

      default:
        *ErrorCode = (UINT8) MeFs1.Bits.ErrorCode;
        break;
    }
  }

  if (InitCompleteFlag) {
    *IsInitComplete = InitCompleteFlag;
    return EFI_SUCCESS;
  }
  return EFI_TIMEOUT;
}


//[-start-131206-IB10310041-modify]//
/**
  Configure ME Devices when needed

  @param[in] Phase                Phase of POST

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
SpsMeDeviceConfigure (
  IN UINTN                             Phase                   
  )
{
  UINT32                     MeMode;
  UINT32                     MeStatus;
  SPS_NMFS                   NmFwStatus; // HECI-2 Node Manager Firmware Status

  switch (Phase) {

    case SpsPeiBeforeMemoryInit:
      ///
      /// Server Platform Services(SPS) Firmware ME-BIOS Interface
      ///  for Denlow Revision 1.0.1(#503664) - 3.2 ME PCI Functions Hiding and Disabling
      ///
      ///  /* ME PCI Functions Hiding and Disabling */
      ///    If NM feature is enabled IDER and KT(SOL) functions are not used
      ///    and should be disabled using FD2 register in PCH.
      ///
      SpsSolDisable ();
      SpsIderDisable ();
      break;

    case SpsPeiAfterMemoryInit:
      break;

    case SpsPeiDisableAll:
      ///
      /// Server Platform Services(SPS) Firmware ME-BIOS Interface
      ///  for Denlow Revision 1.0.1(#503664) - 3.2 ME PCI Functions Hiding and Disabling
      ///
      ///  /* ME PCI Functions Hiding and Disabling */
      ///    If NM feature is disabled all ME PCI functions should be disabled
      ///
      SpsDisableAllMEDevices ();
      break;

    case SpsDxeEntry:
      break;

    case SpsDxeReadyToBoot:
      SpsGetMeMode (&MeMode);
      SpsGetMeStatus (&MeStatus);
      if (MeMode == SPS_ME_OPMODE_SPS) {
        SpsGetNmFs (&NmFwStatus.UInt32);
        if (!IS_SPS_ME_CURSTATE_RECOVERY (MeStatus) && NmFwStatus.Bits.NmEnabled) {
          DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Init HECI#2 device\n"));
          SpsMeChipsetHeci2Init ();
        } else {
          DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Disable HECI#1 and HECI#2 device\n"));
          SpsHeciDisable ();
          SpsHeci2Disable ();
        }

        DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Disable SOL and IDE-R device\n"));
        SpsIderDisable ();
        SpsSolDisable ();

        ///
        /// Store the current value of DEVEN for S3 resume path
        ///
        SpsDeviceStatusSave ();
      }
      ///
      /// If ME Mode is running in ME Temp Disable state, disable Heci1, HECI2, Ider and Sol
      ///
      if (MeMode == SPS_ME_OPMODE_TEMP_DISABLE) {
        SpsDisableAllMEDevices ();
      }
      break;

    default:
      break;
  }

  return EFI_SUCCESS;
}
//[-end-131206-IB10310041-modify]//

/**
  Config CF9 setting.

  @param[in] Action               Lock, Disable or do global reset.

  @retval None.
**/
VOID
SpsMeCF9Configure (
  IN UINTN                             Action
  )
{

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Action : %x\n", Action));
  switch (Action) {

    case Lock:
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Lock CF9\n"));
      SpsMeChipsetCF9Lock ();
      break;

    case Disable:
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Disable CF9\n"));
      SpsMeChipsetCF9Disable ();
      break;

    case GlobalReset:
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Global Reset via CF9\n"));
      DEBUG ((DEBUG_INFO, ".....\n")); /// IO delay
      SpsMeChipsetCF9Reset ();
      break;

    default:
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Do Nothing\n"));
      break;
  }

  return;
}

/**
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] InitStat             H_GS[27:24] Status

  @retval EFI_SUCCESS
**/
EFI_STATUS
SpsMeConfigDidReg (
  IN UINT8                             InitStat
  )
{
  SPS_MEFS1                       MeFs1;
  SPS_DRAM_INIT_DONE_MESSAGE      MeHgs;

  SpsGetMeFs1 (&MeFs1.UInt32);

  if (MeFs1.Bits.OperatingMode == SPS_ME_OPMODE_DEBUG) {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME in debug mode, do not check for ME UMA.\n"));
    return EFI_SUCCESS;
  }

  if (MeFs1.Bits.ErrorCode != 0) {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME error, do not check for ME UMA.\n"));
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Entered ME DRAM Init Done procedure.\n"));

  ///
  /// Set H_GS[31:28] = 0x1 indicating DRAM Init Done
  ///
  MeHgs.UInt32       = 0x0;
  MeHgs.Bits.Command = SPS_DRAM_INIT_DONE_COMMAND;

  ///
  /// Set H_GS[27:24] = Status
  ///   0x0 = Success
  ///   0x1 = No Memory in channels
  ///   0x2 = Memory Init Error
  ///   0x3 = Memory not preserved across reset
  ///   0x4-0xF = Reserved
  ///
  MeHgs.Bits.Status = InitStat;

  ///
  /// Read the ME H_FS Register to look for DID ACK.
  ///
  SpsGetMeFs1 (&MeFs1.UInt32);
  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: HFS read before DID ACK: %x\n", MeFs1.UInt32));

  SpsMeEarlyBiosMessage (MeHgs.UInt32);

  SpsGetMeFs1 (&MeFs1.UInt32);
  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: HFS read after DID ACK: %x\n", MeFs1.UInt32));

  ///
  /// By sending this message, the BIOS lets Intel(R) ME firmware know that
  /// it can start building the DRAM Memory Map and initiate traffic via Intel(R) QPI.
  /// The DRAM_INIT_DONE message does not have any response.
  /// BIOS shall send the message and continue without any delay.
  ///

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: BiosAction = %x\n", MeFs1.Bits.MsgAck));

  return EFI_SUCCESS;
}

///
/// Sps Me Get mPhy Version Pre DID
///
/**
  Get mPhy survivaility table version before DID.

  @param[out] mPhyVersion         mPhy version.

  @retval EFI_SUCCESS             Get version successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMeGetmPhySurvivabilityTablePreDID (
  OUT UINT16                           *mPhyVersion
  )
{
  SPS_MEFS1                     MeFs1;
  SPS_MPHY_PRE_DID_MESSAGE      Msg;
  UINT16                        mPhyTableVersion;
  UINT16                        mPhyTableCrc16;

  if (mPhyVersion == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SpsGetMeFs1 (&MeFs1.UInt32);

  if (MeFs1.Bits.OperatingMode == SPS_ME_OPMODE_DEBUG) {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME in debug mode, do not check for mPhy version request.\n"));
    return EFI_SUCCESS;
  }

  if (MeFs1.Bits.ErrorCode != 0) {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME error, do not check for mPhy version request.\n"));
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Entered ME mPhy version request procedure.\n"));

  Msg.UInt32        = 0;
  Msg.Bits.Data     = SPS_MPHY_PRE_DID_REPORT_VERSION;
  Msg.Bits.Command  = SPS_MPHY_PRE_DID_COMMAND;
  SpsMeEarlyBiosMessage (Msg.UInt32);

  SpsGetMeFs1 (&MeFs1.UInt32);
  if (MeFs1.Bits.MsgAck == SPS_MPHY_PRE_DID_COMMAND) {
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME acknowledge for mPhy version request. (Report mPhy version)\n"));
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: mPhy version request ACK received. (Report mPhy version)\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME didn't acknowledge for mPhy version request. (Report mPhy version)\n"));
  }

  SpsGetmPhySurvivabilityTableVesrion (&mPhyTableVersion, &mPhyTableCrc16);
  *mPhyVersion = (UINT16) mPhyTableVersion;

  Msg.UInt32        = 0;
  Msg.Bits.Data     = SPS_MPHY_PRE_DID_CLOSE_INTERFACE;
  Msg.Bits.Command  = SPS_MPHY_PRE_DID_COMMAND;
  SpsMeEarlyBiosMessage (Msg.UInt32);

  SpsGetMeFs1 (&MeFs1.UInt32);
  if (MeFs1.Bits.MsgAck == SPS_MPHY_PRE_DID_COMMAND) {
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME acknowledge for mPhy version request. (Close Interface)\n"));
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: mPhy version request ACK received. (Close Interface)\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "[SPS ME LIB] ERROR: ME didn't acknowledge for mPhy version request. (Close Interface)\n"));
  }

  return EFI_SUCCESS;
}

/**
  Send mPhy survivaility table to SPS ME firmware.

  @param[in] ChipsetInitTableLen  Length of table.
  @param[in] ChipsetInitTable     Point to table.

  @retval EFI_SUCCESS             Send message successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMemPhySurvivabilityProgramming (
  IN UINT32                            ChipsetInitTableLen,
  IN UINT8                             *ChipsetInitTable
  )
{
  EFI_STATUS                    Status;

  if ((ChipsetInitTable == NULL) || (ChipsetInitTableLen == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsHeciWritemPhySurvivabilityTable (ChipsetInitTableLen, ChipsetInitTable);
  if (!EFI_ERROR (Status)) {
    SpsMeCF9Configure (GlobalReset);
  }

  return Status;
}

/**
  Check SPS ME is already received END_OF_POST message.

  @param None.

  @retval TRUE                    SPS ME is already received END_OF_POST message.
  @retval TRUE                    SPS ME is not receive END_OF_POST message.
**/
BOOLEAN
SpsIsAlreadyReceivedEndOfPost (
  VOID
  )
{
  SPS_MEFS2                  MeFs2;

  SpsGetMeFs2 (&MeFs2.UInt32);
  return (MeFs2.Bits.EopStatus != 0);
}

///
/// SPS Node Manager
///
/**
  Get SPS NM booting mode setting.

  @param[out] BiosBootingMode     Booting mode setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetBootingMode (
  OUT UINT8                            *BiosBootingMode
  )
{
  EFI_STATUS            Status;
  SPS_NMFS              NmFs;

  NmFs.UInt32 = 0;

  ///
  /// Get NM Firmware Status.
  ///
  Status = SpsGetNmFs (&(NmFs.UInt32));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// If NMFS Bit[31] set indicate NM Enabled.
  ///
  if (NmFs.Bits.NmEnabled == SPS_NM_DISABLE) {
    DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: Node Manager is disable\n"));
    return EFI_UNSUPPORTED;
  }

  switch (NmFs.Bits.BiosBootingMode) {

    case SPS_NM_BOOTING_MODE_POWER:
      *BiosBootingMode = SPS_NM_BOOTING_MODE_POWER;
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: NM Booting Mode : Power-Optimized mode\n"));
      break;

    case SPS_NM_BOOTING_MODE_PERF:
      *BiosBootingMode = SPS_NM_BOOTING_MODE_PERF;
      DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: NM Booting Mode : Performance-Optimized mode\n"));
      break;

    default:
      *BiosBootingMode = SPS_NM_BOOTING_MODE_POWER;
      break;
  }

  return EFI_SUCCESS;
}

/**
  Get SPS NM cores disable setting.

  @param[out] NumberOfCoreDisable Cores disable setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetCoresDisableNumber (
  OUT UINT8                            *NumberOfCoreDisable
  )
{
  EFI_STATUS            Status;
  SPS_NMFS              NmFs;

  if (NumberOfCoreDisable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsGetNmFs (&(NmFs.UInt32));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *NumberOfCoreDisable = (UINT8)NmFs.Bits.CoresDisabled;
  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: %x Cores Dislabed.\n", *NumberOfCoreDisable));

  return EFI_SUCCESS;
}

/**
  Get SPS NM power limiting setting.

  @param[out] IsPowerLimiting     Power limiting setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetPowerLimiting (
  OUT UINT32                           *IsPowerLimiting
  )
{
  EFI_STATUS            Status;
  SPS_NMFS              NmFs;

  if (IsPowerLimiting == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsGetNmFs (&(NmFs.UInt32));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *IsPowerLimiting = NmFs.Bits.PowerLimiting;
  DEBUG ((DEBUG_INFO, "[SPS ME LIB] INFO: ME is actively limiting platform power consumption.\n"));

  return EFI_SUCCESS;
}

