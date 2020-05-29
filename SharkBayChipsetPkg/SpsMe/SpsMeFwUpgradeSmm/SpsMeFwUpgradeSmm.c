/** @file

  Server Platform Services(SPS) ME Firmware Upgrade SMM driver

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

#include "SpsMeFwUpgradeSmm.h"

#define SPS_DEBUG_NAME                 "[SMM SPS ME FW UPGRADE] "

SPS_ME_UPGRADE_PROGRESS_TABLE
mSpsUpgradeTableOnlyOPR[] = {
  { 0x00000001, SpsUpgradeProgressEnableMeRegion },
  { 0x00000002, SpsUpgradeProgressInitialize },
  { 0x00000003, SpsUpgradeProgressCheckNewImage },
  { 0x00000004, SpsUpgradeProgressCheckInavtiveOPR },
  { 0x00000004, SpsUpgradeProgressEraseInavtiveOPR },
  { 0x00000004, SpsUpgradeProgressWriteNewOPR },
  { 0x0000000E, SpsUpgradeProgressVerifyNewImage },
  { 0x0000000E, SpsUpgradeProgressNotifyByCmos },
  { 0xFFFFFFFF, NULL }
};


//
// 0.  Check caller's target address.
// 1.  To do.
// 2.  To do.
// 3.  Enable write access to ME region using HMRFPO_ENABLE message.
// 4.  Erase ME region partitions OPR1 (and OPR2 in dual image configuration).
// 5.  Write a new recovery (FTPR) image to the beginning of the OPR1 partition as a recovery backup. 
//     In dual image configuration the same recovery image must be 
//     written to the beginning or both OPR1 and OPR2 partitions of ME region.
// 7.  Verify the operational partition content using HMRFPO_LOCK message. If status 
//     field in lock response is not equal to 0 (new recovery boot-loader image in 
//     operational partition is not valid) abort the update procedure. Restart update 
//     procedure from step 1 using valid ME recovery firmware image. After reset 
//     ME will boot using old recovery image still present in FTPR partition. 
// 8.  Enable write access to ME region using HMRFPO_ENABLE message.
// 9.  Erase factory data partitions in ME region that need to be updated (FPT, MFSB, SCA, FTPR, MFS).
//     Operational partitions OPRx must not be erased.
//     They contain the only valid ME firmware image at this step.
// 10. Write factory data partitions in ME region that need to be updated (FPT, MFSB, FTPR, MFS) with their new images.
// 11. Erase ME region operational partition OPR1 (and OPR2 if dual image configuration).
// 12. Write new operational ME firmware image in operational partition OPR1(and OPR2 in dual image configuration).
// 13. Verify the new operational image using HMRFPO_LOCK message.
//
SPS_ME_UPGRADE_PROGRESS_TABLE
mSpsUpgradeTableEntireMe[] = {
  { 0x00000001, SpsUpgradeProgressEnableMeRegion },
  { 0x00000002, SpsUpgradeProgressInitialize },
  { 0x00000003, SpsUpgradeProgressCheckNewImage },
  { 0x00000004, SpsUpgradeProgressEraseOPR1 },
  { 0x00000005, SpsUpgradeProgressEraseOPR2 },
  { 0x00000005, SpsUpgradeProgressWriteNewFTPRtoOPR1 },
  { 0x00000005, SpsUpgradeProgressWriteNewFTPRtoOPR2 },
  { 0x00000006, SpsUpgradeProgressVerifyNewImage },
  { 0x00000007, SpsUpgradeProgressEnableMeRegion },
  { 0x00000008, SpsUpgradeProgressEraseFPT },
  { 0x00000008, SpsUpgradeProgressEraseMFSB },
  { 0x00000008, SpsUpgradeProgressEraseSCA },
  { 0x00000008, SpsUpgradeProgressEraseFTPR },
  { 0x00000008, SpsUpgradeProgressEraseMFS },
  { 0x00000009, SpsUpgradeProgressWriteNewFPT },
  { 0x00000009, SpsUpgradeProgressWriteNewMFSB },
  { 0x00000009, SpsUpgradeProgressWriteNewSCA },
  { 0x00000009, SpsUpgradeProgressWriteNewFTPR },
  { 0x00000009, SpsUpgradeProgressWriteNewMFS },

  { 0x0000000A, SpsUpgradeProgressEraseOPR1 },
  { 0x0000000B, SpsUpgradeProgressEraseOPR2 },
  { 0x0000000C, SpsUpgradeProgressWriteNewOPR1 },
  { 0x0000000D, SpsUpgradeProgressWriteNewOPR2 },
  { 0x0000000E, SpsUpgradeProgressVerifyNewImage },
  { 0x000000F0, SpsUpgradeProgressNotifyByCmos },
  { 0xFFFFFFFF, NULL }
};

SPS_ME_NONCE_INFO                      mSpsMeNonceInfo;
SPS_ME_UPGRADE_MAP_INFO                mSpsMeMapInfo;
SPS_ME_UPGRADE_MAP_INFO                mNewSpsMeMapInfo;
UINT8                                  *mSpsMeFactoryBackupBuffer = NULL;
UINTN                                  mSpsMeFactoryBackupSize    = 0;
UINTN                                  mSpsRomBaseAddress         = 0;
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL      *mSmmFwBlockService        = NULL;

SPS_ME_UPGRADE_CONTEXT                 mSmmSpsMeUpgradeCtx;

VOID                                   *mNewImageData             = NULL;
UINTN                                  mNewImageDataSize          = 0;
///
/// Function definitions
///
/**
  SPI erase.

  @param[in]      LbaWriteAddress Start address to be erased.
  @param[in, out] NumBytes        IN:  The requested erase size.

  @retval EFI_INVALID_PARAMETER   NumBytes parameter is NULL pointer.
  @retval EFI_SUCCESS             SPI write success.
**/
EFI_STATUS
EFIAPI
SpsUpgradeSpiErase (
  IN     UINTN                         LbaWriteAddress,
  IN OUT UINTN                         *NumBytes
  )
{
  EFI_STATUS       Status;
  UINT8            RetryCount;

  Status     = EFI_SUCCESS;
  RetryCount = 0;

  if ((NumBytes == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwBlockService);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Locate Protocol Smm Fw Block Service Protocol => (%r)!\n", Status));
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiErase...Start!\n"));
  SPS_DEBUG ((DEBUG_INFO, "   Address = 0x%08x\n", LbaWriteAddress));
  SPS_DEBUG ((DEBUG_INFO, "   Size    = 0x%08x\n", *NumBytes));
  do {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   LbaWriteAddress,
                                   NumBytes
                                   );
    RetryCount++;
  } while ((EFI_ERROR (Status)) && (RetryCount < 100));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiErase...End! => (%r)\n", Status));

  return Status;
}

/**
  SPI read.

  @param[in]      WriteAddress    Start LBA address which want to read.
  @param[in]      Offset          Offset in the block at which to begin read
  @param[in, out] NumBytes        IN:  The requested read size.
                                  OUT: The data size by bytes has been read
  @param[out]     Buffer          Data buffer in which to place data read.

  @retval EFI_INVALID_PARAMETER   Buffer parameter is NULL pointer.
  @retval EFI_SUCCESS             SPI read success.
**/
EFI_STATUS
EFIAPI
SpsUpgradeSpiRead (
  IN     UINTN                         WriteAddress,
  IN     UINTN                         Offset,
  IN OUT UINTN                         *NumBytes,
  OUT    UINT8                         *Buffer
  )
{
  EFI_STATUS       Status;
  UINT8            RetryCount;

  Status     = EFI_SUCCESS;
  RetryCount = 0;

  if ((NumBytes == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwBlockService);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Locate Protocol Smm Fw Block Service Protocol => (%r)!\n", Status));
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiRead...Start!\n"));
  SPS_DEBUG ((DEBUG_INFO, "   Address = 0x%08x\n", WriteAddress));
  SPS_DEBUG ((DEBUG_INFO, "   Offset  = 0x%08x\n", Offset));
  SPS_DEBUG ((DEBUG_INFO, "   Size    = 0x%08x\n", *NumBytes));
  SPS_DEBUG ((DEBUG_INFO, "   Buffer  = 0x%08x\n", Buffer));
  do {
    Status = mSmmFwBlockService->Read (
                                   mSmmFwBlockService,
                                   WriteAddress,
                                   Offset,
                                   NumBytes,
                                   Buffer
                                   );
    RetryCount++;
  } while ((EFI_ERROR (Status)) && (RetryCount < 100));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiRead...End! => (%r)\n", Status));

  return Status;
}

/**
  SPI write.

  @param[in]      Offset          Offset in the block at which to begin read
  @param[in, out] NumBytes        IN:  The requested read size.
                                  OUT: The data size by bytes has been read
  @param[out]     Buffer          Data buffer in which to place data read.

  @retval EFI_INVALID_PARAMETER   Buffer parameter is NULL pointer.
  @retval EFI_SUCCESS             SPI write success.
**/
EFI_STATUS
EFIAPI
SpsUpgradeSpiWrite (
  IN     UINTN                         Offset,
  IN OUT UINTN                         *NumBytes,
  IN     UINT8                         *Buffer
  )
{
  EFI_STATUS       Status;
  UINT8            RetryCount;

  Status     = EFI_SUCCESS;
  RetryCount = 0;

  if ((NumBytes == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwBlockService);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Locate Protocol Smm Fw Block Service Protocol => (%r)!\n", Status));
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiWrite...Start!\n"));
  SPS_DEBUG ((DEBUG_INFO, "   Address = 0x%08x\n", Offset));
  SPS_DEBUG ((DEBUG_INFO, "   Size    = 0x%08x\n", *NumBytes));
  SPS_DEBUG ((DEBUG_INFO, "   Buffer  = 0x%08x\n", Buffer));
  do {
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   Offset,
                                   NumBytes,
                                   Buffer
                                   );
    RetryCount++;
  } while ((EFI_ERROR (Status)) && (RetryCount < 100));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeSpiWrite...End! => (%r)\n", Status));

  return Status;
}

/**
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] NonceInfo           Nonce, Factory Default base and limit and message result.

  @retval EFI_INVALID_PARAMETER   NonceInfo parameter is NULL pointer
  @retval EFI_SUCCESS
**/
EFI_STATUS
SpsUpgradeHmrfpoLock (
  OUT SPS_ME_NONCE_INFO                *NonceInfo
  )
{
  SPS_ME_NONCE_INFO     TempNonceInfo;
  EFI_STATUS            Status;
  UINT8                 RetryCount;

  Status     = EFI_SUCCESS;
  RetryCount = 0;

  if (NonceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  do {
    TempNonceInfo.Result = 0xff;
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_LOCK Message : (Attempts = %x)\n", RetryCount));
    Status = SpsHeciHmrfpoLock (
               &TempNonceInfo.Nonce,
               &TempNonceInfo.FactoryDefaultBase,
               &TempNonceInfo.FactoryDefaultLimit,
               &TempNonceInfo.Result
               );

    SPS_DEBUG ((DEBUG_INFO, "  Nonce               : 0x%lx\n", TempNonceInfo.Nonce));
    SPS_DEBUG ((DEBUG_INFO, "  FactoryDefaultBase  : 0x%x\n", TempNonceInfo.FactoryDefaultBase));
    SPS_DEBUG ((DEBUG_INFO, "  FactoryDefaultLimit : 0x%x\n", TempNonceInfo.FactoryDefaultLimit));
    SPS_DEBUG ((DEBUG_INFO, "  Result              : 0x%x\n", TempNonceInfo.Result));

    if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_LOCK_SUCCESS)) {
      mSmmSpsMeUpgradeCtx.MeRegionEnabled = FALSE;
      break;
    }
    RetryCount++;
    MicroSecondDelay (5000);
  } while (RetryCount < 3);

  NonceInfo->Nonce               = TempNonceInfo.Nonce;
  NonceInfo->FactoryDefaultBase  = TempNonceInfo.FactoryDefaultBase;
  NonceInfo->FactoryDefaultLimit = TempNonceInfo.FactoryDefaultLimit;
  NonceInfo->Result              = TempNonceInfo.Result;
  
  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_LOCK_SUCCESS)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: HMRFPO Lock failed (result : %x), could not perform update procedure!\n", TempNonceInfo.Result));
    Status = EFI_NOT_READY;
  }

  return Status;
}

/**
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in]  Nonce               Nonce received in previous HMRFPO_ENABLE Response Message.
  @param[out] NonceInfo           Nonce, Factory Default base and limit and message result.

  @retval EFI_INVALID_PARAMETER   Nonce or NonceInfo parameter is NULL pointer.
  @retval EFI_SUCCESS
**/
EFI_STATUS
SpsUpgradeHmrfpoEnable (
  IN  UINT64                           Nonce,
  OUT SPS_ME_NONCE_INFO                *NonceInfo
  )
{
  SPS_ME_NONCE_INFO     TempNonceInfo;
  EFI_STATUS            Status;
  UINT8                 RetryCount;

  Status     = EFI_SUCCESS;
  RetryCount = 0;
  ZeroMem (&TempNonceInfo, sizeof (SPS_ME_NONCE_INFO));

  if ((Nonce == 0) && (NonceInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.1.2  HMRFPO_ENABLE message 
  ///
  ///  /* HMRFPO_ENABLE message  */
  ///     If response to this message is not received within 2 seconds, BIOS must repeat the 
  ///     request, up to 3 times, before communication failure can be reported. If ME is 
  ///     running in normal operational mode, it does not respond to the first HMRFPO_ENABLE 
  ///     request but restarts to recovery mode upon receiving the first request.
  ///
  do {
    TempNonceInfo.Result = 0xff;
    TempNonceInfo.Nonce  = Nonce;
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_ENABLE Message : (Attempts = %x)\n", RetryCount));
    Status = SpsHeciHmrfpoEnable (
               TempNonceInfo.Nonce,
               &TempNonceInfo.FactoryDefaultBase,
               &TempNonceInfo.FactoryDefaultLimit,
               &TempNonceInfo.Result
               );

    SPS_DEBUG ((DEBUG_INFO, "  Nonce               : 0x%lx\n", TempNonceInfo.Nonce));
    SPS_DEBUG ((DEBUG_INFO, "  FactoryDefaultBase  : 0x%lx\n", TempNonceInfo.FactoryDefaultBase));
    SPS_DEBUG ((DEBUG_INFO, "  FactoryDefaultLimit : 0x%lx\n", TempNonceInfo.FactoryDefaultLimit));
    SPS_DEBUG ((DEBUG_INFO, "  Result              : 0x%x\n", TempNonceInfo.Result));
    SPS_DEBUG ((DEBUG_INFO, "  Status              : %r\n", Status));
    if (TempNonceInfo.FactoryDefaultLimit != mSpsMeNonceInfo.FactoryDefaultLimit) {
      SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: FactoryDefaultLimit does not match (0x%08x)(0x%08x)\n", TempNonceInfo.FactoryDefaultLimit, mSpsMeNonceInfo.FactoryDefaultLimit));
    }

    if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_ENABLE_SUCCESS)) {
      mSmmSpsMeUpgradeCtx.MeRegionEnabled = TRUE;
      break;
    }
    RetryCount++;
    MicroSecondDelay (500);
  } while (RetryCount < 3);

  NonceInfo->FactoryDefaultBase  = TempNonceInfo.FactoryDefaultBase;
  NonceInfo->FactoryDefaultLimit = TempNonceInfo.FactoryDefaultLimit;
  NonceInfo->Result              = TempNonceInfo.Result;

  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_ENABLE_SUCCESS)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: HMRFPO Enable failed (result : %x), Can not access SPS ME region!\n", TempNonceInfo.Result));
    Status = EFI_WRITE_PROTECTED;
  }

  return Status;
}

EFI_STATUS
SpsUpgradeBackupFactoryData (
  IN SPS_ME_NONCE_INFO                 *NonceInfo
  )
{
  EFI_STATUS       Status;

  Status = EFI_NOT_FOUND;

  if (NonceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSpsMeMapInfo.MeRegionBase == 0)  {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME region Base invaild (0x%08x)\n", mSpsMeMapInfo.MeRegionBase));
    ///
    /// SPS ME region base address is zero.
    /// We don't know how to access ME region.
    ///
    return EFI_NOT_FOUND;
  }

  mSpsMeFactoryBackupSize = NonceInfo->FactoryDefaultLimit;
  if (mSpsMeFactoryBackupSize == 0) {
    ///
    /// SPS ME Factory Default size is zero.
    /// We do not backup.
    ///
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME Factory Default Size is zero!\n"));
    if (mSpsMeFactoryBackupBuffer != NULL) {
      FreePool (mSpsMeFactoryBackupBuffer);
      mSpsMeFactoryBackupBuffer = NULL;
    }
    return EFI_NOT_FOUND;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Data Size = 0x%08x\n", mSpsMeFactoryBackupSize));

  if (mSpsMeFactoryBackupBuffer == NULL) {
    ///
    /// Allocate Buffer for SPS ME Factory Data buffer.
    ///
    mSpsMeFactoryBackupBuffer = AllocateZeroPool (mSpsMeFactoryBackupSize);
    if (mSpsMeFactoryBackupBuffer == NULL) {
      SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Allocate buffer for SPS ME Factory Data buffer failed!\n"));
    }
  }
  if (mSpsMeFactoryBackupBuffer != NULL) {
    Status = SpsUpgradeSpiRead (
               mSpsMeMapInfo.MeRegionBase,
               0,
               &mSpsMeFactoryBackupSize,
               mSpsMeFactoryBackupBuffer
               );
    if (EFI_ERROR (Status)) {
      SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Read SPS ME Factory Data(0x%016x) failed => (%r)\n", mSpsMeMapInfo.MeRegionBase, Status));
      if (mSpsMeFactoryBackupBuffer != NULL) {
        FreePool (mSpsMeFactoryBackupBuffer);
        mSpsMeFactoryBackupBuffer = NULL;
      }
    }
  }

  return Status;
}

EFI_STATUS
SpsUpgradeGetCurrentFpt (
  OUT UINT8                            **CurrentFptPtr,
  OUT UINTN                            *CurrentFptSize
  )
{
  EFI_STATUS                           Status;
  SPS_ME_BINARY_FPT                    *TempFptPtr;
  SPS_ME_BINARY_FPT                    TempFpt;
  UINTN                                TempFptSize;
  UINT8                                Result;

  TempFptPtr  = NULL;
  TempFptSize = 0;
  Result      = SPS_HMRFPO_STS_DISABLED;
  ZeroMem (&TempFpt, sizeof (SPS_ME_BINARY_FPT));

  if ((CurrentFptPtr == NULL) || (CurrentFptSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsHeciHmrfpoGetStatus (&Result);
  if (Result != SPS_HMRFPO_STS_ENABLED) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO not enable! Can not access to ME (Result : %x)\n", Result));
    return EFI_ACCESS_DENIED;
  }

  if (mSpsMeFactoryBackupBuffer != NULL) {
    ///
    /// if SPS ME Factory Default data exist.
    /// Search SPS ME Flash Partition Table in Factory Default data.
    ///
    TempFptPtr  = (SPS_ME_BINARY_FPT *) mSpsMeFactoryBackupBuffer;
    TempFptSize = mSpsMeFactoryBackupSize;
    if (TempFptPtr->Signature == SPS_ME_SUB_REGION_SIGNATURE_FPT) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Flash Partition Table Found in Factory Default data\n"));
      SPS_DEBUG ((DEBUG_INFO, "  Flash Partition Table address : 0x%08x\n", TempFptPtr));
      SPS_DEBUG ((DEBUG_INFO, "  Flash Partition Table siez    : 0x%08x\n", TempFptSize));
      SPS_DEBUG ((DEBUG_INFO, "  Number of Entries             : 0x%x\n", TempFptPtr->NumberOfEntries));
      *CurrentFptPtr   = (UINT8 *)TempFptPtr;
      *CurrentFptSize = TempFptSize;
      return EFI_SUCCESS;
    }
  }

  ///
  /// if no SPS ME Factory Default data or FPT can not find in Factory Default data
  /// try to get current SPS ME FPT from ME Region in SPI.
  ///
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Default data not exist.\n"));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Try Get Flash Partition Table in SPI ME region.\n"));
  if (mSpsMeMapInfo.MeRegionBase == 0) {
    ///
    /// SPS ME region in SPI is unknow.
    /// Can not access ME region.
    ///
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME region Base address unknow\n"));
    return EFI_NOT_FOUND;
  }
  ///
  /// Get number of FPT entries.
  ///
  TempFptSize = sizeof (SPS_ME_BINARY_FPT);
  ZeroMem (&TempFpt, TempFptSize);
  Status = SpsUpgradeSpiRead (mSpsMeMapInfo.MeRegionBase, 0, &TempFptSize, (UINT8 *)&TempFpt);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Read Flash Partition Table(0x%016x) failed => (%r)\n", mSpsMeMapInfo.MeRegionBase, Status));
    return Status;
  }
  if (TempFpt.Signature != SPS_ME_SUB_REGION_SIGNATURE_FPT) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Flash Partition Table Not Found\n"));
    return EFI_NOT_FOUND;
  }

  ///
  /// Get number of FPT entries successfully.
  /// Update FptSize to entire FPT size.
  /// Free pool and allocate new buffer for entire FPT.
  ///
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Flash Partition Table Found\n"));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Number of Flash Partition Table Entries : 0x%x\n", TempFpt.NumberOfEntries));
  TempFptSize = sizeof (SPS_ME_BINARY_FPT) + (TempFpt.NumberOfEntries * sizeof (SPS_ME_BINARY_FPT_ENTRY));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Entire Flash Partition Table Size : 0x%x\n", TempFptSize));

  ///
  /// Get FPT header and entries.
  ///
  TempFptPtr = AllocateZeroPool (TempFptSize);
  if (TempFptPtr == NULL) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Allocate buffer for entire Flash Partition Table Entries failed!\n"));
    return Status;
  }
  ZeroMem (TempFptPtr, TempFptSize);
  Status = SpsUpgradeSpiRead (mSpsMeMapInfo.MeRegionBase, 0, &TempFptSize, (UINT8 *)TempFptPtr);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Read entire Flash Partition Table(0x%016x) failed => (%r)\n", mSpsMeMapInfo.MeRegionBase, Status));
    if (TempFptPtr != NULL) {
      FreePool (TempFptPtr);
      TempFptPtr = NULL;
    }
    return EFI_NOT_FOUND;
  }
  if (TempFptPtr->Signature == SPS_ME_SUB_REGION_SIGNATURE_FPT) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Flash Partition Table is SPI ME region\n"));
    SPS_DEBUG ((DEBUG_INFO, "  Flash Partition Table address : 0x%08x\n", TempFptPtr));
    SPS_DEBUG ((DEBUG_INFO, "  Flash Partition Table siez    : 0x%08x\n", TempFptSize));
    SPS_DEBUG ((DEBUG_INFO, "  Number of Entries             : 0x%x\n", TempFptPtr->NumberOfEntries));
    *CurrentFptPtr   = (UINT8 *)TempFptPtr;
    *CurrentFptSize = TempFptSize;
    return EFI_SUCCESS;
  }

  ///
  /// Check FPT data is vaild.
  ///
  if ((*CurrentFptPtr == NULL) || CurrentFptSize == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

#if 0
EFI_STATUS
SpsUpgradeCheckUpgradePhase (
  IN     UINTN                         WriteTargetAddr,
  IN     UINTN                         WriteTargetSize,
  IN OUT UINT32                        *UpgradePhase
  )
{
  EFI_STATUS                 Status;
  SPS_ME_NONCE_INFO          TempNonceInfo;

  Status                            = EFI_SUCCESS;
  TempNonceInfo.Revision            = 0;
  TempNonceInfo.Nonce               = 0;
  TempNonceInfo.FactoryDefaultBase  = 0;
  TempNonceInfo.FactoryDefaultLimit = 0;
  TempNonceInfo.Result              = 0xff;

  if (UpgradePhase == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((mSpsMeMapInfo.MeRegionBase == 0) || (mSpsMeMapInfo.MeRegionLimit == 0)) {
    ///
    /// SPS ME region base address in SPI is unknow.
    ///
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME region Base address unknow\n"));
    return EFI_NOT_FOUND;
  }

  if (mSpsMeNonceInfo.FactoryDefaultLimit != 0) {
    TempNonceInfo.FactoryDefaultBase  = mSpsMeNonceInfo.FactoryDefaultBase;
    TempNonceInfo.FactoryDefaultLimit = mSpsMeNonceInfo.FactoryDefaultLimit;
  }

  // Check does the write target address within ME region
  if (((WriteTargetAddr + WriteTargetSize) > mSpsMeMapInfo.MeRegionBase) && (WriteTargetAddr <= mSpsMeMapInfo.MeRegionLimit)) {
    if (!mSmmSpsMeUpgradeCtx.MeRegionEnabled) {
      //
      // Check SPS ME HMRFPO in which states.
      //
      Status = SpsHeciHmrfpoGetStatus (&TempNonceInfo.Result);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (TempNonceInfo.Result == SPS_HMRFPO_STS_LOCKED) {
        Status = SpsUpgradeHmrfpoEnable (mSmmSpsMeUpgradeCtx.NonceInfo->Nonce, &TempNonceInfo);
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoEnable => (%r)(%x)\n", Status, TempNonceInfo.Result));
        if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_ENABLE_SUCCESS)) {
          SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO_ENEABLE fail. ME region is not open.\n"));
          *UpgradePhase = SPS_ME_UPGRADE_PHASE_DO_NOTHING;
          return Status;
        } else {
          mSmmSpsMeUpgradeCtx.MeRegionEnabled = TRUE;
        }
      } else if (TempNonceInfo.Result == SPS_HMRFPO_STS_DISABLED) {
        SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO in Disabled state.\n"));
        *UpgradePhase = SPS_ME_UPGRADE_PHASE_DO_NOTHING;
        return EFI_WRITE_PROTECTED;
      }
    }

    ///
    /// Server Platform Services(SPS) Firmware ME-BIOS Interface
    ///  for Denlow Revision 1.0.1(#503664) - 3.10.1.2  HMRFPO_ENABLE message 
    ///
    ///  /* HMRFPO_ENABLE message */
    ///     This message indirectly provides the address of the ME SPS operational
    ///     firmware to update. The Factory Base and Factory Limit variables in this message
    ///     provide the offset and length of ME factory data (including recovery boot-
    ///     loader) and the operational firmware area starts just behind this factory data area in
    ///     the ME region on the flash. After HMRFPO_ENABLE response is received with
    ///     Status equal to HMRFPO_ENABLE_SUCCESS all ME region is open for writing.
    ///     BIOS should protect the factory data area and may immediately write the operational
    ///     area just behind the factory data.
    ///
    ///     In dual-image configuration, the currently inactive image should be updated. If first
    ///     image is active then the factory data area is extended to include the first operational
    ///     image, so that Factory Base + Factory Limit points to the second operational image. If
    ///     second operational image is currently active Factory Base + Factory Limit points to the
    ///     first operational image and it is crucial to not erase more flash sectors than necessary
    ///     for the update image, because the currently active image in the second operational
    ///     area could be damaged and would not be valid backup any longer.
    ///
    if (TempNonceInfo.FactoryDefaultLimit != 0) {
      // If target address within ME region and protected region, in other words update whole ME region
      if (WriteTargetAddr < (mSpsMeMapInfo.MeRegionBase + TempNonceInfo.FactoryDefaultLimit)) {
        *UpgradePhase = SPS_ME_UPGRADE_PHASE_ENTIRE_ME_REGION;
      }

      // If target address within ME region but not within protected region, only update operational region
      if (WriteTargetAddr > (mSpsMeMapInfo.MeRegionBase + TempNonceInfo.FactoryDefaultLimit - 1)) {
        *UpgradePhase = SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION;
      }
    }
  } else {
    //
    // Target address out of ME region.
    //
    Status = EFI_SUCCESS;
    *UpgradePhase = SPS_ME_UPGRADE_PHASE_OUT_OF_ME_REGION;
  }

  return Status;
}
#endif

#if 0
/**
  Performs "Safe Update of Entire SPS ME" procedures

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This parameter is NULL pointer.
  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PerformEntireMeRegionPhase (
  IN SPS_ME_UPGRADE_PROTOCOL           *This
  )
{
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  EFI_STATUS                 Status;
  SPS_ME_NONCE_INFO          TempNonceInfo;
  SPS_ME_SUB_REGION_INFO     FPT;
  SPS_ME_SUB_REGION_INFO     MFSB;
  SPS_ME_SUB_REGION_INFO     FTPR;
  SPS_ME_SUB_REGION_INFO     MFS;
  SPS_ME_SUB_REGION_INFO     OPR1;
  SPS_ME_SUB_REGION_INFO     OPR2;
  SPS_ME_SUB_REGION_INFO     NewFPT;
  SPS_ME_SUB_REGION_INFO     NewMFSB;
  SPS_ME_SUB_REGION_INFO     NewFTPR;
  SPS_ME_SUB_REGION_INFO     NewMFS;
  SPS_ME_SUB_REGION_INFO     NewOPR1;
  SPS_ME_SUB_REGION_INFO     NewOPR2;
  UINT8                      *CurrentMeFpt;
  UINTN                      CurrentMeFptSize;

  Ctx = NULL;

  TempNonceInfo.Revision            = 0;
  TempNonceInfo.Nonce               = 0;
  TempNonceInfo.FactoryDefaultBase  = 0;
  TempNonceInfo.FactoryDefaultLimit = 0;
  TempNonceInfo.Result              = 0xff;

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  ///TODO: 3.10.2.5  Safe update of entire ME region after EOP
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.5  Safe update of entire ME region after EOP
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 1. On each power flow when nonce can be retrieved by sending HMRFPO_LOCK
  ///             command, BIOS should retrieve it and store in a secure way in the SMI handler
  ///             memory so that it is available for later use after EOP.
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 2. BIOS should send EOP message and boot EFI shell or OS. Note that this step
  ///             blocks the possibility to retrieve the nonce from ME firmware.
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 3. Each time when (re)trying ME region update and ME runs in recovery
  ///             mode prior to starting update procedure ME reset should be performed. This
  ///             action is required to make sure ME firmware status registers MEFS1 and
  ///             MEFS2 reflect the current content of ME region when retrying ME
  ///             region update at some unknown point. After reset ME firmware status
  ///             should be verified in order to assure that ME firmware is initialized
  ///             (MEFS1.InitComplete is set).
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 4. Bit 5 and bit 10 of ME firmware status register must be verified to handle
  ///             update retry case properly. If bit 5 (FPT or Factory Presets Bad) or bit 10
  ///             (Recovery BUP Loader Fault) is set then jump to step 9, skipping steps 5 to 8. This
  ///             situation means that previous update attempt was interrupted, but new recovery
  ///             (FTPR) image was successfully loaded from OPR1 (or OPR2) partition. In case the
  ///             old FTPR image is erased (during previous update attempt) the new recovery
  ///             image in the operational partition will be the only valid image present in the
  ///             ME region, so OPRx partitions cannot be written until FTPR is updated with
  ///             the new recovery image.
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 5. Enable write access to ME region using HMRFPO_ENABLE message. It is
  ///             expected that the update application will trigger SMI interrupt and BIOS, when
  ///             handling the SMI, will send the HMRFPO_ENABLE message to ME using the
  ///             nonce word stored in SMI handler memory. Since the whole ME region is
  ///             going to be written, Factory Base and Factory Limit values provided in the
  ///             HMRFPO_ENABLE response can be ignored.
  ///
  ///  (Done in SpsUpgradeCheckUpgradePhase())
  ///

  ///
  /// Backup SPS ME Factory Default data.
  ///
  if ((Ctx->NonceInfo != NULL) && (Ctx->NonceInfo->Result == SPS_HMRFPO_LOCK_SUCCESS)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Default Data Backup...Start\n"));
    Status = SpsUpgradeBackupFactoryData (Ctx->NonceInfo);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Default Data Backup...End => (%r)\n\n", Status));
    if (EFI_ERROR (Status)) {
    }
  }

  ///
  /// Create SPS ME FW Flash Partition Table and partitions information.
  /// To learn the distribution of ME.
  ///
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI...Start\n"));
  Status = SpsUpgradeGetCurrentFpt (&CurrentMeFpt, &CurrentMeFptSize);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI...End => (%r)\n\n", Status));
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI => (%r)\n", Status));
    ///
    /// If we can not get current SPS's Flash Partition Table(FPT)
    /// it mean we cannot know SPS ME Partition information.
    /// So, we cannot simulate upgrade progress and also cannot do real upgrade Program.
    /// just Enable ME region. Let AP do any thing...
    ///
    return Status;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with Current SPS Data...Start\n"));
  Status = SpsUpgradeCreateRegionInfo (
             CurrentMeFpt,
             CurrentMeFptSize,
             &mSpsMeMapInfo
             );
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with Current SPS Data...End => (%r)\n\n", Status));
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with Current SPS Data => (%r)\n", Status));
    return Status;
  }

  if (mNewSpsMeMapInfo.Valid) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image...Start\n"));
    Status = SpsUpgradeCreateRegionInfo (
               (UINT8 *)mNewSpsMeMapInfo.MeRegionBase,
               mNewSpsMeMapInfo.MeRegionSize,
               &mNewSpsMeMapInfo
               );
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image...End => (%r)\n\n", Status));
    if (EFI_ERROR (Status)) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image => (%r)\n", Status));
      return Status;
    }
  }

  ///
  /// Get Current SPS ME partition info.
  ///
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FPT, &mSpsMeMapInfo, &FPT);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Flash Partition Table Not Found => (%r)\n", Status));
    FPT.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_MFSB, &mSpsMeMapInfo, &MFSB);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME File System Backup Not Found => (%r)\n", Status));
    MFSB.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FTPR, &mSpsMeMapInfo, &FTPR);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Fault Tolerant Partition Not Found => (%r)\n", Status));
    FTPR.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_MFS, &mSpsMeMapInfo, &MFS);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME File System Not Found => (%r)\n", Status));
    MFS.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR1, &mSpsMeMapInfo, &OPR1);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Operational Image 1 Not Found => (%r)\n", Status));
    OPR1.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR2, &mSpsMeMapInfo, &OPR2);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Operational Image 2 Not Found => (%r)\n", Status));
    OPR2.Valid = FALSE;
  }

  ///
  /// Get New SPS ME partition info.
  ///
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FPT, &mNewSpsMeMapInfo, &NewFPT);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS Flash Partition Table Not Found => (%r)\n", Status));
    NewFPT.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_MFSB, &mNewSpsMeMapInfo, &NewMFSB);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS ME File System Backup Not Found => (%r)\n", Status));
    NewMFSB.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FTPR, &mNewSpsMeMapInfo, &NewFTPR);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS Fault Tolerant Partition Not Found => (%r)\n", Status));
    NewFTPR.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_MFS, &mNewSpsMeMapInfo, &NewMFS);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS ME File System Not Found => (%r)\n", Status));
    NewMFS.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR1, &mNewSpsMeMapInfo, &NewOPR1);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS Operational Image 1 Not Found => (%r)\n", Status));
    NewOPR1.Valid = FALSE;
  }
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR2, &mNewSpsMeMapInfo, &NewOPR2);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New SPS Operational Image 2 Not Found => (%r)\n", Status));
    NewOPR2.Valid = FALSE;
  }

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 6. Erase ME region partitions OPR1 (and OPR2 in dual-image configuration).
  ///
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 7. Write new recovery boot-loader (FTPR) image to OPR1 partition as a recovery
  ///             backup. In dual-image configuration, the same recovery image must be written to
  ///             both OPR1 and OPR2 partitions of ME region.
  ///
  ///
  /// Do Step 6 and Step 7, If New FTPR exist and data is valid.
  ///
  ///
  if ((NewFTPR.Valid) && (NewFTPR.DataValid)) {
    if (OPR1.Valid) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Erase ME region partitions OPR1...Start\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: OPR1 Start = 0x%08x, Size = 0x%08x\n", OPR1.Start, OPR1.Size));
      Status = SpsUpgradeSpiErase (OPR1.Start, &OPR1.Size);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Erase ME region partitions OPR1...End => (%r)\n\n", Status));
      if (EFI_ERROR (Status)) {

      }
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new recovery boot-loader (FTPR) image to OPR1...Start\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: NewFTPR Start = 0x%08x, Size = 0x%08x\n", NewFTPR.Data, NewFTPR.Size));
      Status = SpsUpgradeSpiWrite (OPR1.Start, &NewFTPR.Size, NewFTPR.Data);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new Recovery boot-loader (FTPR) image to OPR1...End => (%r)\n\n", Status));
      if (EFI_ERROR (Status)) {

      }
    } else {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: ME region partitions OPR1...Not found.\n"));
    }
    if (OPR2.Valid) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Erase ME region partitions OPR2...Start\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: OPR2 Start = 0x%08x, Size = 0x%08x\n", OPR2.Start, OPR2.Size));
      Status = SpsUpgradeSpiErase (OPR2.Start, &OPR2.Size);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Erase ME region partitions OPR1...End => (%r)\n\n", Status));
      if (EFI_ERROR (Status)) {

      }
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new recovery boot-loader (FTPR) image to OPR2...Start\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: NewFTPR Start = 0x%08x, Size = 0x%08x\n", NewFTPR.Data, NewFTPR.Size));
      Status = SpsUpgradeSpiWrite (OPR2.Start, &NewFTPR.Size, NewFTPR.Data);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new recovery boot-loader (FTPR) image to OPR2...End => (%r)\n\n", Status));
      if (EFI_ERROR (Status)) {

      }
    } else {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: ME region partitions OPR2...Not found.\n"));
    }
  } else {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New recovery boot-loader (FTPR) image...Not found.\n"));
    return EFI_SUCCESS;
  }

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 8. Verify the operational partition content using HMRFPO_LOCK message. If status
  ///             field in lock response is not equal to 0 (new recovery image in operational partition
  ///             is not valid), abort the update procedure. Restart update procedure from step 2
  ///             using valid ME recovery image. After reset, ME will boot using old
  ///             recovery image still present in FTPR partition.
  ///
  TempNonceInfo.Result = SPS_HMRFPO_LOCK_FAIL;
  Status = SpsUpgradeHmrfpoLock (&TempNonceInfo);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoLock => (%r)(%x)\n", Status, TempNonceInfo.Result));
  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_LOCK_SUCCESS)) {
    ///
    /// if HMRFPO lock fail, will set PhaseInitDownBis[2] to 1 to identify
    /// the new image are invaild and abort this SPS ME upgrade progress.
    ///
    Ctx->PhaseInitDownBits = (Ctx->PhaseInitDownBits | BIT2);
    ///
    /// abort the update procedure.
    ///
    return EFI_ABORTED;
  }

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 9. Enable write access to ME region using HMRFPO_ENABLE message.
  ///
  Status = SpsUpgradeHmrfpoEnable (Ctx->NonceInfo->Nonce, &TempNonceInfo);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoEnable => (%r)(%x)\n", Status, TempNonceInfo.Result));
  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_ENABLE_SUCCESS)) {
    return Status;
  }

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 10. Erase factory data partitions in ME region that need to be updated (FPT,
  ///             MFSB, FTPR, MFS). Operational partitions OPRx must not be erased. They contain
  ///             the only valid ME firmware image at this step.
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 11. Write factory data partitions in ME region that need to be updated (FPT,
  ///             MFSB, FTPR, MFS) with their new images.
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 12. Erase ME region operational partition OPR1 (and OPR2 in dual-image configuration).
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 13. Write new operational ME firmware image in operational partition OPR1
  ///             (and OPR2 in dual-image configuration).
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 14. Verify the new operational image using HMRFPO_LOCK message. If status field in
  ///             the response is not equal to 0 (new ME operational image is not valid) abort
  ///             update procedure. Restart update procedure from step 2 using valid ME
  ///             operational firmware image. After reset, ME will boot using recovery boot-
  ///             loader image in FTPR partition.
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 15. After successful, an information for the BIOS POST phase should be passed to the
  ///             BIOS to perform joint ME and host reset at next POST. Such a reset cannot
  ///             be done immediately because of its locking required in section 0. It is expected
  ///             that the SMI handler will store such information in nonvolatile memory for POST
  ///             phase.
  ///

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///    Step 16. When the request for BIOS POST phase to perform global reset is stored, the
  ///             update application may request normal host system reset to enter BIOS POST
  ///             phase.
  ///

  return Status;
}
#endif

#if 0
/**
  Performs "Direct Firmware Update of SPS ME" procedures (operational regions only)

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This parameter is NULL pointer.
  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PerformMeOperationalRegionPhase (
  IN SPS_ME_UPGRADE_PROTOCOL           *This
  )
{
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  Ctx = NULL;

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  ///TODO: 3.10.2.2  Direct firmware update after EOP
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.2  Direct firmware update after EOP
  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 1. On each power flow when nonce can be retrieved by sending HMRFPO_LOCK
  ///             command, BIOS should retrieve it and store in a secure way in the SMI handler
  ///             memory so that it is available for later use after EOP.
  ///

  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 2. BIOS should send END_OF_POST message and boot the EFI shell or OS. This step
  ///             blocks the possibility to retrieve the nonce word from ME firmware. 
  ///

  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 3. To initiate update procedure, HMRFPO_ENABLE command with proper nonce must
  ///             be sent to ME firmware to enable access to ME Region. It is expected
  ///             that the update application will trigger an SMI interrupt and BIOS, when handling
  ///             the SMI, will send the HMRFPO_ENABLE message to ME using the nonce
  ///             word saved in SMI memory.
  ///
  ///  (Done in SpsUpgradeCheckUpgradePhase())
  ///

  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 4. BIOS should protect the factory data range provided in the HMRFPO_ENABLE
  ///             response. Writing to the factory data area may corrupt recovery boot-loader and
  ///             board with damaged recovery boot-loader could be recovered only with the use of
  ///             Security Strap Override jumper, so physical access to the platform hardware is
  ///             required.
  ///
  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 5. After flashing the new ME firmware to the ME region, the
  ///             HMRFPO_LOCK command should be sent to verify if the updated image is correct
  ///             (see step A15 in Figure 3-2). It is expected that the update application will trigger
  ///             SMI interrupt and BIOS, when handling the SMI, will send the HMRFPO_LOCK.
  ///
  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 6. After positive verification of the new image, a host reset should be performed,
  ///             either cold or warm. ME will promote this reset to global platform reset
  ///             (joint ME and host reset) to restart ME to normal, operational mode,
  ///             using the new firmware image.
  ///

  return EFI_SUCCESS;
}
#endif

/**
  Get SPS ME protect ROM map

  @param[in]      This            Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[out]     ProtectRomMap   Pointer of protect ROM map output buffer.
  @param[in, out] NumberOfProtectRegions  # of protect ROM map ouput buffer as input parameter;
                                          physical # of protect ROM map as output parameter.

  @retval EFI_INVALID_PARAMETER   This or NumberOfProtectRegions parameter is NULL pointer
  @retval EFI_BUFFER_TOO_SMALL    Size of protect ROM map buffer less than physical size
  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GetProtectRomMap (
  IN     SPS_ME_UPGRADE_PROTOCOL       *This,
  OUT    SPS_ME_PROTECT_ROM_MAP        *ProtectRomMap, OPTIONAL
  IN OUT UINTN                         *NumberOfProtectRegions
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  Status = EFI_SUCCESS;

  if (!SpsMeFwUpgradeSupported ()) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  if ((This == NULL) || (NumberOfProtectRegions == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  if ((*NumberOfProtectRegions) < Ctx->NumberOfProtectRomMap) {
    (*NumberOfProtectRegions) = Ctx->NumberOfProtectRomMap;

    if (ProtectRomMap != NULL) {
      Status = EFI_BUFFER_TOO_SMALL;
    }
  }

  if (ProtectRomMap != NULL) {
    CopyMem (
      ProtectRomMap,
      Ctx->ProtectRomMap,
      (Ctx->NumberOfProtectRomMap * sizeof (SPS_ME_PROTECT_ROM_MAP))
      );
  }

  return Status;
}

#if 0
/**
  Distinguish does the target linear address within ME region. If true, then perform the
  SPS ME firmware updating procedures.
  This function must be invoked before writing data into ME region of flash device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] WriteTargetAddr      Target linear address for writing flash device.
  @param[in] WriteTargetSize      Target size for writing flash device.

  @retval others                  Perform SPS ME firmware updating procedures failed.
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
UpgradeProgress (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             WriteTargetAddr,
  IN UINTN                             WriteTargetSize
  )
{
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  EFI_STATUS                 Status;
  UINT32                     UpgradePhase;

  Status                 = EFI_SUCCESS;
  Ctx                    = NULL;
  UpgradePhase           = SPS_ME_UPGRADE_PHASE_MAX;

  if (!SpsMeFwUpgradeSupported ()) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// if HMRFPO lock fail, will set PhaseInitDownBis[2] to 1 to identify
  /// the new image are invaild and abort this SPS ME upgrade progress.
  ///
  if ((Ctx->PhaseInitDownBits & BIT2) == BIT2) {
    return EFI_WRITE_PROTECTED;
  }

  Status = SpsUpgradeEnableHeciDevice ();
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ( (DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeEnableHeciDevice() (%r)\n", Status));
  }

  Status = SpsUpgradeCheckUpgradePhase (WriteTargetAddr, WriteTargetSize, &UpgradePhase);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Check Upgrade phase => (%r)(%x)\n\n", Status, UpgradePhase));
  }

  switch (UpgradePhase) {
  case SPS_ME_UPGRADE_PHASE_ENTIRE_ME_REGION:
    if (Ctx->PhaseInitDownBits == 0) {
      SPS_DEBUG ((DEBUG_INFO, "\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Upgate entire ME region...Start\n"));
      Status = PerformEntireMeRegionPhase (This);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Upgate entire ME region...End => (%r)\n\n", Status));
      if (!EFI_ERROR (Status)) {
        Ctx->PhaseInitDownBits = (Ctx->PhaseInitDownBits | BIT0);
      }
    }
    break;

  case SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION:
    if (Ctx->PhaseInitDownBits == 0) {
      SPS_DEBUG ((DEBUG_INFO, "\n"));
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Upgate ME Operational region...Start\n"));
      Status = PerformMeOperationalRegionPhase (This);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Upgate ME Operational region...End => (%r)\n\n", Status));
      if (!EFI_ERROR (Status)) {
        Ctx->PhaseInitDownBits = (Ctx->PhaseInitDownBits | BIT1);
      }
    }
    break;

  case SPS_ME_UPGRADE_PHASE_DO_NOTHING:
  case SPS_ME_UPGRADE_PHASE_OUT_OF_ME_REGION:
  case SPS_ME_UPGRADE_PHASE_MAX:
  default:
    break;
  }

  if (mSpsMeMapInfo.SubRegionInfo != NULL) {
    FreePool (mSpsMeMapInfo.SubRegionInfo);
    mSpsMeMapInfo.SubRegionInfo = NULL;
  }

  if (mNewSpsMeMapInfo.SubRegionInfo != NULL) {
    FreePool (mNewSpsMeMapInfo.SubRegionInfo);
    mNewSpsMeMapInfo.SubRegionInfo = NULL;
  }

  return Status;
}
#endif

/**
  Perform the SPS ME firmware updating procedures after writing data to ME region of flash
  device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer.
  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Perform SPS ME firmware updating procedures failed.
**/
EFI_STATUS
EFIAPI
UpgradeComplete (
  IN SPS_ME_UPGRADE_PROTOCOL           *This
  )
{
  EFI_STATUS                 Status;
  SPS_ME_NONCE_INFO          NonceInfo;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  Status = EFI_UNSUPPORTED;

  if (!SpsMeFwUpgradeSupported ()) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeEnableHeciDevice ();
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ( (DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeEnableHeciDevice() (%r)\n", Status));
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_GET_STATUS to get HMRFPO status...Start\n"));
  Status = SpsHeciHmrfpoGetStatus (&NonceInfo.Result);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_GET_STATUS to get HMRFPO status...End => (%r)(%x)\n", Status, NonceInfo.Result));
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get HMRFPO Status => (%r)(%x)\n", Status, NonceInfo.Result));
    NonceInfo.Result = SPS_HMRFPO_STS_DISABLED;
  }
  if (NonceInfo.Result != SPS_HMRFPO_STS_LOCKED) {
    ///
    /// Server Platform Services(SPS) Firmware ME-BIOS Interface
    ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.2  Direct firmware update after EOP
    ///
    ///  /* Direct firmware update after EOP */
    ///     Step 5. After flashing the new ME firmware to the ME region, the
    ///             HMRFPO_LOCK command should be sent to verify if the updated image is correct
    ///             (see step A15 in Figure 3-2). It is expected that the update application will trigger
    ///             SMI interrupt and BIOS, when handling the SMI, will send the HMRFPO_LOCK.
    ///
    /// Server Platform Services(SPS) Firmware ME-BIOS Interface
    ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.5  Safe update of entire ME region after EOP
    ///
    ///  /* Safe update of entire ME region after EOP */
    ///     Step 14. Verify the new operational image using HMRFPO_LOCK message. If status field in
    ///              the response is not equal to 0 (new ME operational image is not valid) abort
    ///              update procedure. Restart update procedure from step 2 using valid ME
    ///              operational firmware image. After reset, ME will boot using recovery boot-
    ///              loader image in FTPR partition.
    ///
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_LOCK to verify New image...Start\n"));
    Status = SpsUpgradeHmrfpoLock (&NonceInfo);
    if (!EFI_ERROR (Status) && (NonceInfo.Result == SPS_HMRFPO_LOCK_SUCCESS)) {
      Ctx->FlowControl.UpgradeDone = TRUE;
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New image is valid(%x). HMRFPO Lock Success. => (%r)\n", NonceInfo.Result, Status));
    } else {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: HMRFPO_LOCK message => (%r) result => (%x)\n", Status, NonceInfo.Result));
    }
  } else {
    ///
    /// HMRFPO is locked. ME region not update.
    ///
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: HMRFPO is locked\n"));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.2  Direct firmware update after EOP
  ///
  ///  /* Direct firmware update after EOP */
  ///     Step 6.  After positive verification of the new image, a host reset should be performed,
  ///              either cold or warm. ME will promote this reset to global platform reset
  ///              (joint ME and host reset) to restart ME to normal, operational mode,
  ///              using the new firmware image.
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3.10.2.5  Safe update of entire ME region after EOP
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 15. After successful, an information for the BIOS POST phase should be passed to the
  ///              BIOS to perform joint ME and host reset at next POST. Such a reset cannot
  ///              be done immediately because of its locking required in section 0. It is expected
  ///              that the SMI handler will store such information in nonvolatile memory for POST
  ///              phase.
  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 16. When the request for BIOS POST phase to perform global reset is stored, the
  ///              update application may request normal host system reset to enter BIOS POST
  ///              phase.
  ///
  if (Ctx->FlowControl.UpgradeDone) {
    ///
    /// Notfiy SPS PEIM that SPS ME FW has been upgraded and then perform a global reset.
    ///
    if (SpsCheckMePolicyRevision ()) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Notify SPS PEI Module that SPS ME FW has been upgraded!\n"));
      SpsNotifyMeUpgradedByCmos ();
    }
  }

  ///
  /// Set All config to initial value.
  /// Free all memory that allocate by this driver.
  ///
  Ctx->FlowControl.UpgradeMode         = SPS_ME_UPGRADE_MODE_DEFAULT;
  Ctx->FlowControl.UpgradeModeLimit    = SPS_ME_UPGRADE_MODE_FULL_UPGRADE;
  Ctx->FlowControl.UpgradePhase        = SPS_ME_UPGRADE_PHASE_FIRST_TIME;
  Ctx->FlowControl.UpgradeStatus       = SPS_ME_UPGRADE_STATUS_DEFAULT;
  Ctx->FlowControl.UpgradeTableIndex   = 0;
  Ctx->FlowControl.SpiBlockIndex       = 0;
  Ctx->FlowControl.InactiveOPR         = SPS_ME_SUB_REGION_SIGNATURE_OPR1;
  Ctx->FlowControl.UpgradeDone         = FALSE;
  Ctx->FlowControl.ImageInvalid        = FALSE;

  if (mSpsMeFactoryBackupBuffer != NULL) {
    FreePool (mSpsMeFactoryBackupBuffer);
    mSpsMeFactoryBackupBuffer = NULL;
  }

  if (mSpsMeMapInfo.SubRegionInfo != NULL) {
    FreePool (mSpsMeMapInfo.SubRegionInfo);
    mSpsMeMapInfo.SubRegionInfo = NULL;
  }

  if (mNewImageData != NULL) {
    FreePool (mNewImageData);
    mNewImageData = NULL;
  }

  if (mNewSpsMeMapInfo.SubRegionInfo != NULL) {
    FreePool (mNewSpsMeMapInfo.SubRegionInfo);
    mNewSpsMeMapInfo.SubRegionInfo = NULL;
  }

  if (mNewSpsMeMapInfo.Valid) {
    ZeroMem (&mNewSpsMeMapInfo, sizeof(SPS_ME_UPGRADE_MAP_INFO));
  }

  return Status;
}

/**
  Pass new image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] NewImageSource       Pointer of New image.
  @param[in] NewImageSize         New image size.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer
  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Pass SPS ME firmware to driver failed.
**/
EFI_STATUS
PassImage (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN VOID                              *NewImageSource,
  IN UINTN                             NewImageSize
  )
{
  EFI_STATUS                 Status;
  SPS_ME_BINARY_FPT          *NewSpsMeFpt;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if (!SpsMeFwUpgradeSupported ()) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if ((This == NULL) || (NewImageSource == NULL) || (NewImageSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Image Source : (0x%x)\n", (UINTN*)NewImageSource));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Image Size   : (0x%x)\n", (UINTN)NewImageSize));

  ///
  ///TODO: Here mean a new upgrade progress is starting.
  /// Clean all status to initial state
  ///
  Ctx->FlowControl.UpgradeMode         = SPS_ME_UPGRADE_MODE_DEFAULT;
  Ctx->FlowControl.UpgradeModeLimit    = SPS_ME_UPGRADE_MODE_FULL_UPGRADE;
  Ctx->FlowControl.UpgradePhase        = SPS_ME_UPGRADE_PHASE_FIRST_TIME;
  Ctx->FlowControl.UpgradeStatus       = SPS_ME_UPGRADE_STATUS_DEFAULT;
  Ctx->FlowControl.UpgradeTableIndex   = 0;
  Ctx->FlowControl.SpiBlockIndex       = 0;
  Ctx->FlowControl.InactiveOPR         = SPS_ME_SUB_REGION_SIGNATURE_OPR1;
  Ctx->FlowControl.UpgradeDone         = FALSE;
  Ctx->FlowControl.ImageInvalid        = FALSE;

  mNewSpsMeMapInfo.Valid = FALSE;
  mNewImageDataSize      = 0;

//[-start-140506-IB10310055-remove]//
//  if (mNewImageData != NULL) {
//    FreePool (mNewImageData);
//    mNewImageData = NULL;
//  }
//[-end-140506-IB10310055-remove]//
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Search $FPT in new Image Source... Start\n"));
  Status = SpsUpgradeSearchFpt (NewImageSource, NewImageSize, &NewSpsMeFpt);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Search $FPT in new Image Source... End => (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Allocate memory for new image data.
  ///
  mNewImageDataSize = NewImageSize;
//[-start-140506-IB10310055-modify]//
  mNewImageData     = NewImageSource;
//  mNewImageData = AllocatePool (mNewImageDataSize);
//[-end-140506-IB10310055-modify]//
  if (mNewImageData == NULL) {
    mNewImageDataSize      = 0;
    mNewSpsMeMapInfo.Valid = FALSE;
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: AllocatePool for New Image Data fail.\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  ///
  /// Keep new image data for internal use.
  ///
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Save New image for internal use... Start\n"));
  if (mNewImageData != NULL) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: CopyMem(0x%08x, 0x%08x, 0x%08x)\n", mNewImageData, NewImageSource, mNewImageDataSize));
//    CopyMem (mNewImageData, NewImageSource, mNewImageDataSize);
  }
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Save New image for internal use... End\n"));

  ///
  /// Search $FPT and setting New image me information.
  ///
  Status = SpsUpgradeSearchFpt (mNewImageData, mNewImageDataSize, &NewSpsMeFpt);
  if (!EFI_ERROR (Status)) {
    ///
    ///TODO: New ME region size is not real size.
    ///
    mNewSpsMeMapInfo.Valid         = TRUE;
    mNewSpsMeMapInfo.MeRegionBase  = (UINTN)NewSpsMeFpt;
    mNewSpsMeMapInfo.MeRegionSize  = NewImageSize;
    mNewSpsMeMapInfo.MeRegionLimit = mNewSpsMeMapInfo.MeRegionBase + mNewSpsMeMapInfo.MeRegionSize - 1;
  } else {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Search $FPT => (%r)\n", Status));
  }

  if (mNewSpsMeMapInfo.Valid) {
    Status = SpsUpgradeCreateRegionInfo (
               (UINT8 *)mNewSpsMeMapInfo.MeRegionBase,
               mNewSpsMeMapInfo.MeRegionSize,
               &mNewSpsMeMapInfo
               );
    if (EFI_ERROR (Status)) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image => (%r)\n", Status));
    } else {
      SpsUpgradeShowMeMapInfo (&mNewSpsMeMapInfo);
    }
  }

  return Status;
}

/**
  Setting SPS ME Upgrade Mode.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] UpgradeMode          SPS ME Upgrade mode.

  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Pass SPS ME firmware to driver failed.
**/
EFI_STATUS
SetMode (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             UpgradeMode
  )
{
  SPS_ME_UPGRADE_CONTEXT     *Ctx;


  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (UpgradeMode >= SPS_ME_UPGRADE_MODE_MAX) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Ctx->FlowControl.UpgradeMode = UpgradeMode;
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Upgrade Mode => %x\n", UpgradeMode));

  return EFI_SUCCESS;
}

/**
  Erase SPI by Sub region signature.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] Signature            Which sub region will erase.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This Erase progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressSpiErase (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  IN  UINT32                           Signature,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     SubRegionInfo;
  UINTN                      TempSpiBlockIndex;
  UINTN                      EraseSize;
  UINTN                      EraseAddress;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;
  EraseSize        = SPS_ME_UPGRADE_SPI_ERASE_SIZE_MAX;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  TempSpiBlockIndex = Ctx->FlowControl.SpiBlockIndex;

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Easer (%a)\n", &Signature));
  Status = SpsUpgradeGetSubRegionInfo (Signature, &mSpsMeMapInfo, &SubRegionInfo);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get (%a) info => (%r)\n", &Signature, Status));
    return Status;
  }
  if (!SubRegionInfo.Valid) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: (%a) info is invalid.\n", &Signature));
    return EFI_NOT_FOUND;
  }

  EraseAddress = SubRegionInfo.Start + (TempSpiBlockIndex * EraseSize);
  Status = SpsUpgradeSpiErase (EraseAddress, &EraseSize);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeSpiErase (0x%08x, 0x%08x) => (%r)\n", EraseAddress, EraseSize, Status));
    return Status;
  }
  ///
  /// Check is any more block need to erase.
  /// if yes, SPI block index add 1 and set IsCompleted to FALSE.
  ///
  if ((EraseAddress + EraseSize) < SubRegionInfo.End) {
    Ctx->FlowControl.SpiBlockIndex ++;
    *IsCompleted = FALSE;
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Erase next address => (0x%08x)\n", EraseAddress + EraseSize));
    return Status;
  }

  ///
  /// Erase done.
  ///
  Ctx->FlowControl.SpiBlockIndex = 0;
  switch (Signature) {
  case SPS_ME_SUB_REGION_SIGNATURE_FPT:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_FPT_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_MFSB:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_MFSB_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_FTPR:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_FTPR_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_MFS:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_MFS_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_OPR1:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR1_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_OPR2:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR2_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_SCA:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_SCA_DONE;
    break;

  case SPS_ME_SUB_REGION_SIGNATURE_ASAP:
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_ASAP_DONE;
    break;

  default:
    break;
  }
  
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Easer (%a) Done.\n", &Signature));
  return Status;
}

/**
  Write data to SPI by Sub region signature.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] Signature            Which sub region will erase.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This write progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressSpiWrite (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  IN  UINT32                           Signature,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     NewSubRegionInfo;
  SPS_ME_SUB_REGION_INFO     SubRegionInfo;
  UINTN                      TempSpiBlockIndex;
  UINTN                      WriteSize;
  UINTN                      WriteAddress;
  UINT8                      *WriteSource;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;
  WriteAddress     = 0;
  WriteSize        = SPS_ME_UPGRADE_SPI_WRITE_SIZE_MAX;
  WriteSource      = NULL;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);
  TempSpiBlockIndex = Ctx->FlowControl.SpiBlockIndex;

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write (%a)\n", &Signature));

  Status = SpsUpgradeGetSubRegionInfo (Signature, &mNewSpsMeMapInfo, &NewSubRegionInfo);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get New (%a) info => (%r)\n\n", &Signature, Status));
    return Status;
  }
  if ((!NewSubRegionInfo.Valid) || (!NewSubRegionInfo.DataValid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New (%a) info is invalid.\n\n", &Signature));
    return EFI_NOT_FOUND;
  }

  Status = SpsUpgradeGetSubRegionInfo (Signature, &mSpsMeMapInfo, &SubRegionInfo);
  if (EFI_ERROR (Status) || (!SubRegionInfo.Valid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Old(%a) info => (%r) Use New image info.\n", &Signature, Status));
    WriteAddress      = (mSpsMeMapInfo.MeRegionBase + NewSubRegionInfo.Offset + (TempSpiBlockIndex * WriteSize));
    SubRegionInfo.End = (mSpsMeMapInfo.MeRegionBase + NewSubRegionInfo.Offset + NewSubRegionInfo.Size);
  } else {
    WriteAddress = SubRegionInfo.Start + (TempSpiBlockIndex * WriteSize);
  }

  WriteSource  = NewSubRegionInfo.Data + (TempSpiBlockIndex * WriteSize);
  Status = SpsUpgradeSpiWrite (WriteAddress, &WriteSize, WriteSource);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeSpiWrite (0x%08x, 0x%08x, 0x%08x) => (%r)\n", WriteAddress, WriteSize, WriteSource, Status));
    return Status;
  }

  ///
  /// Check is any more block need to wrire.
  /// if yes, SPI block index add 1 and set IsCompleted to FALSE.
  ///
  if ((WriteAddress + WriteSize) < SubRegionInfo.End) {
    Ctx->FlowControl.SpiBlockIndex ++;
    *IsCompleted = FALSE;
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write next address => (0x%08x)\n", WriteAddress + WriteSize));
    return Status;
  }

  ///
  /// Write done.
  ///
  Ctx->FlowControl.SpiBlockIndex = 0;

  return Status;
}

/**
  Send HMRFPO_ENABLE message to ME to open ME region

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval EFI_ACCESS_DENIED       Enable ME fail or ME in disable state.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEnableMeRegion (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_NONCE_INFO          TempNonceInfo;

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempNonceInfo.Revision            = 0;
  TempNonceInfo.Nonce               = 0;
  TempNonceInfo.FactoryDefaultBase  = 0;
  TempNonceInfo.FactoryDefaultLimit = 0;
  TempNonceInfo.Result              = 0xff;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Check SPS ME HMRFPO in which states.
  ///
  Status = SpsHeciHmrfpoGetStatus (&TempNonceInfo.Result);
  if (TempNonceInfo.Result == SPS_HMRFPO_STS_LOCKED) {
    Status = SpsUpgradeHmrfpoEnable (Ctx->NonceInfo->Nonce, &TempNonceInfo);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoEnable => (%r)(%x)\n", Status, TempNonceInfo.Result));
    if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_ENABLE_SUCCESS)) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME HMRFPO_ENEABLE success. ME region is open.\n"));
    } else {
      SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO_ENEABLE fail. ME region is not open.\n"));
      return EFI_ACCESS_DENIED;
    }
  } else if (TempNonceInfo.Result == SPS_HMRFPO_STS_DISABLED) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO in Disabled state.\n"));
    return EFI_ACCESS_DENIED;
  }

  return Status;
}

/**
  Backup ME factory default data.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressInitialize (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  UINT8                      *CurrentMeFpt;
  UINTN                      CurrentMeFptSize;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Backup SPS ME Factory Default data.
  ///
  if ((Ctx->NonceInfo != NULL) && (Ctx->NonceInfo->Result == SPS_HMRFPO_LOCK_SUCCESS)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Default Data Backup...Start\n"));
    Status = SpsUpgradeBackupFactoryData (Ctx->NonceInfo);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Factory Default Data Backup...End => (%r)\n\n", Status));
    if (EFI_ERROR (Status)) {

    }
  }

  ///
  /// Create SPS ME FW Flash Partition Table and partitions information.
  /// To learn the distribution of ME.
  ///
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI...Start\n"));
  Status = SpsUpgradeGetCurrentFpt (&CurrentMeFpt, &CurrentMeFptSize);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI...End => (%r)\n\n", Status));
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get Current SPS ME FPT form SPI => (%r)\n", Status));
    ///
    /// If we can not get current SPS's Flash Partition Table(FPT)
    /// it mean we cannot know SPS ME Partition information.
    /// So, we cannot simulate upgrade progress and also cannot do real upgrade Program.
    /// just Enable ME region. Let AP do any thing...
    ///
    return EFI_SUCCESS;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with Current SPS Data...Start\n"));
  Status = SpsUpgradeCreateRegionInfo (
             CurrentMeFpt,
             CurrentMeFptSize,
             &mSpsMeMapInfo
             );
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with Current SPS Data...End => (%r)\n\n", Status));
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Create Region Info with Current SPS Data => (%r)\n", Status));
    return EFI_WRITE_PROTECTED;
  }

  if (mNewSpsMeMapInfo.Valid) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image...Start\n"));
    Status = SpsUpgradeCreateRegionInfo (
               (UINT8 *)mNewSpsMeMapInfo.MeRegionBase,
               mNewSpsMeMapInfo.MeRegionSize,
               &mNewSpsMeMapInfo
               );
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Create Region Info with New Image...End => (%r)\n\n", Status));
    if (EFI_ERROR (Status)) {
      SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Create Region Info with New Image => (%r)\n", Status));
    }
  }

  ///
  /// SMM FW Block services is necessary in full upgrade mode.
  ///
  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwBlockService);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Locate Protocol Smm Fw Block Service Protocol => (%r)!\n", Status));
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return EFI_UNSUPPORTED;
    }
  }

  return Status;
}

/**
  Check New image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressCheckNewImage (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     NewSubRegionInfo;
  UINT32                     Signature;
  UINT16           MajorNumber;
  UINT16           MinorNumber;
  UINT16           PatchNumber;
  UINT16           BuildNumber;
  BOOLEAN          IsOffsetMatch;
  BOOLEAN          IsSizeMatch;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Get New Recovery Image version.
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_FTPR;
  Status = SpsUpgradeGetSubRegionInfo (Signature, &mNewSpsMeMapInfo, &NewSubRegionInfo);
  if ((!EFI_ERROR (Status)) && NewSubRegionInfo.Valid && NewSubRegionInfo.DataValid) {
    Status = SpsUpgradeGetFwVersionFromImage (
               NewSubRegionInfo.Data,
               NewSubRegionInfo.Size,
               &MajorNumber,
               &MinorNumber,
               &PatchNumber,
               &BuildNumber
               );
    if (!EFI_ERROR (Status)) {
      mNewSpsMeMapInfo.FwVersion.RcvFw.MajorNumber = (UINT8)MajorNumber;
      mNewSpsMeMapInfo.FwVersion.RcvFw.MinorNumber = MinorNumber;
      mNewSpsMeMapInfo.FwVersion.RcvFw.PatchNumber = PatchNumber;
      mNewSpsMeMapInfo.FwVersion.RcvFw.BuildNumber = BuildNumber;
    }

    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Recovery Image Version: %d.%d.%d.%d\n", MajorNumber, MinorNumber, PatchNumber, BuildNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MajorNumber => (0x%04x)\n", MajorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MinorNumber => (0x%04x)\n", MinorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   PatchNumber => (0x%04x)\n", PatchNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   BuildNumber => (0x%04x)\n", BuildNumber));
    if (!SpsUpgradeDowngradeSupported ()) {
      if ((mNewSpsMeMapInfo.FwVersion.RcvFw.MajorNumber < mSpsMeMapInfo.FwVersion.RcvFw.MajorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.RcvFw.MinorNumber < mSpsMeMapInfo.FwVersion.RcvFw.MinorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.RcvFw.PatchNumber < mSpsMeMapInfo.FwVersion.RcvFw.PatchNumber) || \
          (mNewSpsMeMapInfo.FwVersion.RcvFw.BuildNumber < mSpsMeMapInfo.FwVersion.RcvFw.BuildNumber)){
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Current Recovery Image Version: %d.%d.%d.%d\n", mSpsMeMapInfo.FwVersion.RcvFw.MajorNumber, mSpsMeMapInfo.FwVersion.RcvFw.MinorNumber, mSpsMeMapInfo.FwVersion.RcvFw.PatchNumber, mSpsMeMapInfo.FwVersion.RcvFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:     New Recovery Image Version: %d.%d.%d.%d\n", mNewSpsMeMapInfo.FwVersion.RcvFw.MajorNumber, mNewSpsMeMapInfo.FwVersion.RcvFw.MinorNumber, mNewSpsMeMapInfo.FwVersion.RcvFw.PatchNumber, mNewSpsMeMapInfo.FwVersion.RcvFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Downupgrade Unsupported\n"));
        return EFI_ABORTED;
      }
    }

    ///
    /// Check Sub region has no change.
    ///TODO: if change, Maybe do something.
    ///
    Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
    if (!EFI_ERROR (Status)) {
      if (!IsOffsetMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
      if (!IsSizeMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
    }
  }

  ///
  /// Get New Operation Image 1 version.
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_OPR1;
  Status = SpsUpgradeGetSubRegionInfo (Signature, &mNewSpsMeMapInfo, &NewSubRegionInfo);
  if ((!EFI_ERROR (Status)) && NewSubRegionInfo.Valid && NewSubRegionInfo.DataValid) {
    Status = SpsUpgradeGetFwVersionFromImage (
               NewSubRegionInfo.Data,
               NewSubRegionInfo.Size,
               &MajorNumber,
               &MinorNumber,
               &PatchNumber,
               &BuildNumber
               );
    if (!EFI_ERROR (Status)) {
      mNewSpsMeMapInfo.FwVersion.ActFw.MajorNumber = (UINT8)MajorNumber;
      mNewSpsMeMapInfo.FwVersion.ActFw.MinorNumber = MinorNumber;
      mNewSpsMeMapInfo.FwVersion.ActFw.PatchNumber = PatchNumber;
      mNewSpsMeMapInfo.FwVersion.ActFw.BuildNumber = BuildNumber;
    }

    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Operation Image 1 Version: %d.%d.%d.%d\n", MajorNumber, MinorNumber, PatchNumber, BuildNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MajorNumber => (0x%04x)\n", MajorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MinorNumber => (0x%04x)\n", MinorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   PatchNumber => (0x%04x)\n", PatchNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   BuildNumber => (0x%04x)\n", BuildNumber));
    if (!SpsUpgradeDowngradeSupported ()) {
      if ((mNewSpsMeMapInfo.FwVersion.ActFw.MajorNumber < mSpsMeMapInfo.FwVersion.ActFw.MajorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.ActFw.MinorNumber < mSpsMeMapInfo.FwVersion.ActFw.MinorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.ActFw.PatchNumber < mSpsMeMapInfo.FwVersion.ActFw.PatchNumber) || \
          (mNewSpsMeMapInfo.FwVersion.ActFw.BuildNumber < mSpsMeMapInfo.FwVersion.ActFw.BuildNumber)){
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Current Operation Image 1 Version: %d.%d.%d.%d\n", mSpsMeMapInfo.FwVersion.ActFw.MajorNumber, mSpsMeMapInfo.FwVersion.ActFw.MinorNumber, mSpsMeMapInfo.FwVersion.ActFw.PatchNumber, mSpsMeMapInfo.FwVersion.ActFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:     New Operation Image 1 Version: %d.%d.%d.%d\n", mNewSpsMeMapInfo.FwVersion.ActFw.MajorNumber, mNewSpsMeMapInfo.FwVersion.ActFw.MinorNumber, mNewSpsMeMapInfo.FwVersion.ActFw.PatchNumber, mNewSpsMeMapInfo.FwVersion.ActFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Downupgrade Unsupported\n"));
        return EFI_ABORTED;
      }
    }

    ///
    /// Check Sub region has no change.
    ///TODO: if change, Maybe do something.
    ///
    Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
    if (!EFI_ERROR (Status)) {
      if (!IsOffsetMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
      if (!IsSizeMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
    }
  }

  ///
  /// Get New Operation Image 2 version.
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_OPR2;
  Status = SpsUpgradeGetSubRegionInfo (Signature, &mNewSpsMeMapInfo, &NewSubRegionInfo);
  if ((!EFI_ERROR (Status)) && NewSubRegionInfo.Valid && NewSubRegionInfo.DataValid) {
    Status = SpsUpgradeGetFwVersionFromImage (
               NewSubRegionInfo.Data,
               NewSubRegionInfo.Size,
               &MajorNumber,
               &MinorNumber,
               &PatchNumber,
               &BuildNumber
               );
    if (!EFI_ERROR (Status)) {
      mNewSpsMeMapInfo.FwVersion.BkpFw.MajorNumber = (UINT8)MajorNumber;
      mNewSpsMeMapInfo.FwVersion.BkpFw.MinorNumber = MinorNumber;
      mNewSpsMeMapInfo.FwVersion.BkpFw.PatchNumber = PatchNumber;
      mNewSpsMeMapInfo.FwVersion.BkpFw.BuildNumber = BuildNumber;
    }
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Operation Image 2 Version: %d.%d.%d.%d\n", MajorNumber, MinorNumber, PatchNumber, BuildNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MajorNumber => (0x%04x)\n", MajorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   MinorNumber => (0x%04x)\n", MinorNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   PatchNumber => (0x%04x)\n", PatchNumber));
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:   BuildNumber => (0x%04x)\n", BuildNumber));
    if (!SpsUpgradeDowngradeSupported ()) {
      if ((mNewSpsMeMapInfo.FwVersion.BkpFw.MajorNumber < mSpsMeMapInfo.FwVersion.BkpFw.MajorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.BkpFw.MinorNumber < mSpsMeMapInfo.FwVersion.BkpFw.MinorNumber) || \
          (mNewSpsMeMapInfo.FwVersion.BkpFw.PatchNumber < mSpsMeMapInfo.FwVersion.BkpFw.PatchNumber) || \
          (mNewSpsMeMapInfo.FwVersion.BkpFw.BuildNumber < mSpsMeMapInfo.FwVersion.BkpFw.BuildNumber)){
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Current Operation Image 2 Version: %d.%d.%d.%d\n", mSpsMeMapInfo.FwVersion.BkpFw.MajorNumber, mSpsMeMapInfo.FwVersion.BkpFw.MinorNumber, mSpsMeMapInfo.FwVersion.BkpFw.PatchNumber, mSpsMeMapInfo.FwVersion.BkpFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO:     New Operation Image 2 Version: %d.%d.%d.%d\n", mNewSpsMeMapInfo.FwVersion.BkpFw.MajorNumber, mNewSpsMeMapInfo.FwVersion.BkpFw.MinorNumber, mNewSpsMeMapInfo.FwVersion.BkpFw.PatchNumber, mNewSpsMeMapInfo.FwVersion.BkpFw.BuildNumber));
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Downupgrade Unsupported\n"));
        return EFI_ABORTED;
      }
    }

    ///
    /// Check Sub region has no change.
    ///TODO: if change, Maybe do something.
    ///
    Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
    if (!EFI_ERROR (Status)) {
      if (!IsOffsetMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
      if (!IsSizeMatch) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
        ///
        ///TODO: Aborted Progress?
        ///
      }
    }
  }


  ///
  /// Check $FPT
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_FPT;
  Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
  if (!EFI_ERROR (Status)) {
    if (!IsOffsetMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
    if (!IsSizeMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
  }

  ///
  /// Check MFSB
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_MFSB;
  Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
  if (!EFI_ERROR (Status)) {
    if (!IsOffsetMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
    if (!IsSizeMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
  }

  ///
  /// Check MFS
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_MFS;
  Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
  if (!EFI_ERROR (Status)) {
    if (!IsOffsetMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
    if (!IsSizeMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
  }


  ///
  /// Check SCA
  ///
  Signature = SPS_ME_SUB_REGION_SIGNATURE_SCA;
  Status = SpsUpgradeCheckSubRegion (Signature, &mSpsMeMapInfo, &mNewSpsMeMapInfo, &IsOffsetMatch, &IsSizeMatch);
  if (!EFI_ERROR (Status)) {
    if (!IsOffsetMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Offset not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
    if (!IsSizeMatch) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Sub Region (%a) => Size not match.\n", Signature));
      ///
      ///TODO: Aborted Progress?
      ///
    }
  }

  return EFI_SUCCESS;
}

/**
  Erase FPT sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseFPT (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_FPT, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase FPT done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_FPT_DONE;

  return Status;
}

/**
  Erase MFSB sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseMFSB (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_MFSB, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase MFSB done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_MFSB_DONE;

  return Status;
}

/**
  Erase SCA sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseSCA (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_SCA, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
//[-start-140506-IB10310055-add]//
    if (Status == EFI_NOT_FOUND) {
      ///
      /// If Not found, Skip this region.
      ///
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Skip this region.\n"));
      Status = EFI_SUCCESS;
    }
//[-end-140506-IB10310055-add]//
    return Status;
  }

  ///
  /// Erase SCA done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_SCA_DONE;

  return Status;
}

/**
  Erase FTPR sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseFTPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_FTPR, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase FTPR done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_FTPR_DONE;

  return Status;
}

/**
  Erase MFS sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseMFS (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_MFS, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase MFS done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_MFS_DONE;

  return Status;
}

/**
  Erase OPR1 sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  if (Ctx->FlowControl.UpgradePhase == SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION) {
  }

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_OPR1, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase OPR1 done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR1_DONE;

  return Status;
}

/**
  Erase OPR2 sub region.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiErase (This, SPS_ME_SUB_REGION_SIGNATURE_OPR2, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    if (Status == EFI_NOT_FOUND) {
      ///
      /// If Not found, Skip this region.
      ///
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Skip this region.\n"));
      Status = EFI_SUCCESS;
    }
    return Status;
  }

  ///
  /// Erase OPR2 done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR2_DONE;

  return Status;
}

/**
  Write New FTPR to OPR1.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPRtoOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     OPR1;
  SPS_ME_SUB_REGION_INFO     NewFTPR;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR1, &mSpsMeMapInfo, &OPR1);
  if (EFI_ERROR (Status) || (!OPR1.Valid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get SPS Operational Image 1 => (%r)\n\n", Status));
    return Status;
  }

  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FTPR, &mNewSpsMeMapInfo, &NewFTPR);
  if (EFI_ERROR (Status) || (!NewFTPR.Valid) || (!NewFTPR.DataValid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get new SPS Fault Tolerant Partition => (%r)\n\n", Status));
    return Status;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new Recovery boot-loader (FTPR) image to OPR1...Start\n"));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: NewFTPR Start = 0x%08x, Size = 0x%08x\n", NewFTPR.Data, NewFTPR.Size));
  Status = SpsUpgradeSpiWrite (OPR1.Start, &NewFTPR.Size, NewFTPR.Data);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new Recovery boot-loader (FTPR) image to OPR1...End => (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    ///
    ///
    ///
  }

  return Status;
}

/**
  Write New FTPR to OPR2.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPRtoOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     OPR2;
  SPS_ME_SUB_REGION_INFO     NewFTPR;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Check OPR2 is exist.
  /// if not, rerutn EFI_SUCCESS; Skip this region.
  ///
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR2, &mSpsMeMapInfo, &OPR2);
  if (EFI_ERROR (Status) || (!OPR2.Valid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get SPS Operational Image 2 => (%r) SKip this region!\n\n", Status));
    return EFI_SUCCESS;
  }

  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_FTPR, &mNewSpsMeMapInfo, &NewFTPR);
  if (EFI_ERROR (Status) || (!NewFTPR.Valid) || (!NewFTPR.DataValid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get new SPS Fault Tolerant Partition => (%r)\n\n", Status));
    return Status;
  }

  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new Recovery boot-loader (FTPR) image to OPR2...Start\n"));
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: NewFTPR Start = 0x%08x, Size = 0x%08x\n", NewFTPR.Data, NewFTPR.Size));
  Status = SpsUpgradeSpiWrite (OPR2.Start, &NewFTPR.Size, NewFTPR.Data);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write new Recovery boot-loader (FTPR) image to OPR2...End => (%r)\n\n", Status));
  if (EFI_ERROR (Status)) {
    ///
    ///
    ///
  }

  return Status;
}

/**
  Write New FPT.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFPT (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_FPT, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase FPT done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_FPT_DONE;

  return Status;
}

/**
  Write New MFSB.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewMFSB (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_MFSB, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase MFSB done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_MFSB_DONE;

  return Status;
}

/**
  Write New SCA.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewSCA (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_SCA, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
//[-start-140506-IB10310055-add]//
    if (Status == EFI_NOT_FOUND) {
      ///
      /// If Not found, Skip this region.
      ///
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Skip this region.\n"));
      Status = EFI_SUCCESS;
    }
//[-end-140506-IB10310055-add]//
    return Status;
  }

  ///
  /// Erase SCA done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_SCA_DONE;

  return Status;
}

/**
  Write New FTPR.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewFTPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_FTPR, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase FTPR done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_FTPR_DONE;

  return Status;
}

/**
  Write New MFS.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewMFS (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_MFS, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase MFS done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_MFS_DONE;

  return Status;
}

/**
  Write New OPR1.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR1 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_OPR1, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase OPR1 done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_OPR1_DONE;

  return Status;
}

/**
  Write New OPR2.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR2 (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  Status = SpsUpgradeProgressSpiWrite (This, SPS_ME_SUB_REGION_SIGNATURE_OPR2, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    if (Status == EFI_NOT_FOUND) {
      ///
      /// If Not found, Skip this region.
      ///
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Skip this region.\n"));
      Status = EFI_SUCCESS;
    }
    return Status;
  }

  ///
  /// Erase OPR2 done.
  ///
  Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_WRITE_OPR2_DONE;

  return Status;
}

/**
  Verify New image by HMRFPO_LOCK message.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressVerifyNewImage (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_NONCE_INFO          TempNonceInfo;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  ///  /* Safe update of entire ME region after EOP */
  ///     Step 8. Verify the operational partition content using HMRFPO_LOCK message. If status
  ///             field in lock response is not equal to 0 (new recovery image in operational partition
  ///             is not valid), abort the update procedure. Restart update procedure from step 2
  ///             using valid ME recovery image. After reset, ME will boot using old
  ///             recovery image still present in FTPR partition.
  ///
  TempNonceInfo.Result = SPS_HMRFPO_LOCK_FAIL;
  Status = SpsUpgradeHmrfpoLock (&TempNonceInfo);
  SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoLock => (%r)(%x)\n", Status, TempNonceInfo.Result));
  if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_LOCK_SUCCESS)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: New Image is valid.\n"));
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_VERIFY_SUCCESS;
  }
  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_LOCK_SUCCESS)) {
    ///
    /// if HMRFPO lock fail, will set PhaseInitDownBis[2] to 1 to identify
    /// the new image are invaild and abort this SPS ME upgrade progress.
    ///
    Ctx->FlowControl.ImageInvalid  = TRUE;
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_VERIFY_FAIL;
    ///
    /// abort the update procedure.
    ///
    return EFI_ABORTED;
  }

  return Status;
}

/**
  Nodify Upgrade is done by CMOS.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressNotifyByCmos (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  if (Ctx->FlowControl.UpgradeStatus == SPS_ME_UPGRADE_STATUS_VERIFY_SUCCESS) {
    ///
    /// Notfiy SPS PEIM that SPS ME FW has been upgraded and then perform a global reset.
    ///
    if (SpsCheckMePolicyRevision ()) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Notify SPS PEI Module that SPS ME FW has been upgraded!\n"));
      SpsNotifyMeUpgradedByCmos ();
    }
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ENTIRE_ME_FULL_UPGRADE_DONE;
    Ctx->FlowControl.UpgradeDone   = TRUE;
  }

  return Status;
}

/**
  Check which OPR is inactice operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressCheckInavtiveOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     OPR1;
  SPS_ME_SUB_REGION_INFO     OPR2;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Check inactive image.
  ///
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR1, &mSpsMeMapInfo, &OPR1);
  if (!EFI_ERROR (Status) && (OPR1.Valid)) {
    if (Ctx->NonceInfo->FactoryDefaultLimit == OPR1.Offset) {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Operational Image 1 is inactive\n"));
      Ctx->FlowControl.InactiveOPR = SPS_ME_SUB_REGION_SIGNATURE_OPR1;
      return EFI_SUCCESS;
    }
  } else {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get SPS Operational Image 1 region info => (%r)\n\n", Status));
  }
  if (Ctx->FlowControl.DualImageMode) {
    Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR2, &mSpsMeMapInfo, &OPR2);
    if (!EFI_ERROR (Status) && (OPR2.Valid)) {
      if (Ctx->NonceInfo->FactoryDefaultLimit == OPR2.Offset) {
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS Operational Image 2 is inactive\n"));
        Ctx->FlowControl.InactiveOPR = SPS_ME_SUB_REGION_SIGNATURE_OPR2;
        return EFI_SUCCESS;
      }
    } else {
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get SPS Operational Image 2 region info => (%r)\n\n", Status));
    }
  }

  return Status;
}

/**
  Erase inactive Operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressEraseInavtiveOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  if ((Ctx->FlowControl.InactiveOPR != SPS_ME_SUB_REGION_SIGNATURE_OPR1) && \
      (Ctx->FlowControl.InactiveOPR != SPS_ME_SUB_REGION_SIGNATURE_OPR2)) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Erase inactive OPR.
  ///
  Status = SpsUpgradeProgressSpiErase (This, Ctx->FlowControl.InactiveOPR, IsCompleted);
  if (EFI_ERROR (Status) || (!*IsCompleted)) {
    return Status;
  }

  ///
  /// Erase inactive OPR done.
  ///
  if (Ctx->FlowControl.InactiveOPR == SPS_ME_SUB_REGION_SIGNATURE_OPR1) {
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR1_DONE;
  } else if (Ctx->FlowControl.InactiveOPR == SPS_ME_SUB_REGION_SIGNATURE_OPR2) {
    Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_ERASE_OPR2_DONE;
  }

  return Status;
}

/**
  Write New OPR to inactive Operation image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] IsCompleted          Is done.

  @retval EFI_SUCCESS             This progress successfully.
  @retval others                  This progress fail.
**/
EFI_STATUS
SpsUpgradeProgressWriteNewOPR (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This,
  OUT BOOLEAN                          *IsCompleted
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_SUB_REGION_INFO     InactiveOPR;
  SPS_ME_SUB_REGION_INFO     NewOPR;
  UINTN                      WriteAddress;
  UINTN                      WriteSize;
  UINT8                      *WriteSource;
  UINTN                      TempSpiBlockIndex;

  if ((This == NULL) || (IsCompleted == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status           = EFI_SUCCESS;
  Ctx              = NULL;
  *IsCompleted     = TRUE;
  WriteAddress     = 0;
  WriteSize        = SPS_ME_UPGRADE_SPI_WRITE_SIZE_MAX;
  WriteSource      = NULL;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  TempSpiBlockIndex = Ctx->FlowControl.SpiBlockIndex;

  if ((Ctx->FlowControl.InactiveOPR != SPS_ME_SUB_REGION_SIGNATURE_OPR1) && \
      (Ctx->FlowControl.InactiveOPR != SPS_ME_SUB_REGION_SIGNATURE_OPR2)) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Get New OPR.
  ///
  Status = SpsUpgradeGetSubRegionInfo (SPS_ME_SUB_REGION_SIGNATURE_OPR1, &mNewSpsMeMapInfo, &NewOPR);
  if (EFI_ERROR (Status) || (!NewOPR.Valid) || (!NewOPR.DataValid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get new SPS Operational Image region info => (%r)\n\n", Status));
    return Status;
  }

  ///
  /// Get inactive OPR.
  ///
  Status = SpsUpgradeGetSubRegionInfo (Ctx->FlowControl.InactiveOPR, &mSpsMeMapInfo, &InactiveOPR);
  if (EFI_ERROR (Status) || (!InactiveOPR.Valid)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Get SPS Inactive OPR region info => (%r)\n\n", Status));
    return Status;
  }

  ///
  /// Write New OPR to inactive OPR.
  ///
  WriteAddress = InactiveOPR.Start + (TempSpiBlockIndex * WriteSize);
  WriteSource  = NewOPR.Data + (TempSpiBlockIndex * WriteSize);
  Status = SpsUpgradeSpiWrite (WriteAddress, &WriteSize, WriteSource);
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeSpiWrite (0x%08x, 0x%08x, 0x%08x) => (%r)\n", WriteAddress, WriteSize, WriteSource, Status));
    return Status;
  }

  ///
  /// Update New OPR information.
  ///
  NewOPR.End = InactiveOPR.Start + NewOPR.Size - 1;

  ///
  /// Check is any more block need to wrire.
  /// if yes, SPI block index add 1 and set IsCompleted to FALSE.
  ///
  if ((WriteAddress + WriteSize) < NewOPR.End) {
    Ctx->FlowControl.SpiBlockIndex ++;
    *IsCompleted = FALSE;
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Write next address => (0x%08x)\n", WriteAddress + WriteSize));
    return Status;
  }

  ///
  /// Write done.
  ///
  Ctx->FlowControl.SpiBlockIndex = 0;

  return Status;
}

/**
  Check Target address in ME region.

  @param[in] WriteTargetAddr      Targer address.
  @param[in] WriteTargetSize      Size of targer address.

  @retval TRUE                    Targer address in ME resgion.
  @retval FALSE                   Targer address out of ME resgion.
**/
BOOLEAN
SpsUpgradeIsInMeRegion (
  IN UINTN                             WriteTargetAddr,
  IN UINTN                             WriteTargetSize
  )
{
  if (((WriteTargetAddr + WriteTargetSize) > mSpsMeMapInfo.MeRegionBase) && \
       (WriteTargetAddr <= mSpsMeMapInfo.MeRegionLimit)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Performs "Safe Update of Entire SPS ME" procedures

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This parameter is NULL pointer.
  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
SpsUpgradeHandler (
  IN  SPS_ME_UPGRADE_PROTOCOL          *This
  )
{
  SPS_ME_UPGRADE_PROGRESS_TABLE   *UpgradeTable;
  SPS_ME_UPGRADE_CONTEXT          *Ctx;
  EFI_STATUS                 Status;
  BOOLEAN                    IsCompleted;

  Status           = EFI_SUCCESS;
  IsCompleted      = FALSE;

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// Upgrage mode setting.
  ///
  switch (Ctx->FlowControl.UpgradeMode) {
  case SPS_ME_UPGRADE_MODE_FULL_UPGRADE:
    UpgradeTable = mSpsUpgradeTableEntireMe;
    if (Ctx->FlowControl.UpgradePhase == SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION) {
      UpgradeTable = mSpsUpgradeTableOnlyOPR;
    }
    break;

  case SPS_ME_UPGRADE_MODE_ONLY_ENABLE_HMRFPO:
  case SPS_ME_UPGRADE_MODE_DEFAULT:
  default:
    return Status;
  }

  while (UpgradeTable[Ctx->FlowControl.UpgradeTableIndex].Function != NULL) {
    Status = UpgradeTable[Ctx->FlowControl.UpgradeTableIndex].Function (This, &IsCompleted);
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: UpgradeTable[%d] => (%r)\n", Ctx->FlowControl.UpgradeTableIndex, Status));
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsCompleted) {
      ///
      /// Function completed, go next.
      ///
      Ctx->FlowControl.UpgradeTableIndex++;
    }

    ///
    /// Check Upgrade is in block mode.
    /// if not, run all.
    ///
    if (Ctx->FlowControl.BlockMode) {
      ///
      /// Return EFI_NOT_READY and expect Caller call again.
      ///
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: AP waiting...\n"));
      return EFI_NOT_READY;
    }
  };

  return Status;
}

/**
  Distinguish does the target linear address within ME region. If true, then perform the
  SPS ME firmware updating procedures.
  This function must be invoked before writing data into ME region of flash device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] WriteTargetAddr      Target linear address for writing flash device.
  @param[in] WriteTargetSize      Target size for writing flash device.

  @retval others                  Perform SPS ME firmware updating procedures failed.
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
UpgradeProgress2 (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             WriteTargetAddr,
  IN UINTN                             WriteTargetSize
  )
{
  EFI_STATUS                 Status;
  SPS_ME_UPGRADE_CONTEXT     *Ctx;
  SPS_ME_NONCE_INFO          TempNonceInfo;

  Status           = EFI_SUCCESS;
  Ctx              = NULL;


  if (!SpsMeFwUpgradeSupported ()) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  if (gSmst == NULL) {
    ASSERT (gSmst != NULL);
    return EFI_UNSUPPORTED;
  }

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ctx = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (This);

  ///
  /// if HMRFPO lock fail, will set PhaseInitDownBis[2] to 1 to identify
  /// the new image are invaild and abort this SPS ME upgrade progress.
  ///
  if (Ctx->FlowControl.ImageInvalid) {
    return EFI_WRITE_PROTECTED;
  }

  Status = SpsUpgradeEnableHeciDevice ();
  if (EFI_ERROR (Status)) {
    SPS_DEBUG ( (DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeEnableHeciDevice() (%r)\n", Status));
  }

  ///
  /// Target Address out of ME region, SPS upgrade do nothing.
  ///
  if (!SpsUpgradeIsInMeRegion (WriteTargetAddr, WriteTargetSize)) {
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Not in ME region... Skip!\n"));
    return EFI_SUCCESS;
  }

  ///
  /// Check upgrade phase (Entire ME or direct firmware update).
  /// 
  ///
  if (Ctx->FlowControl.UpgradePhase == SPS_ME_UPGRADE_PHASE_FIRST_TIME) {
    ///
    /// Enable SPS ME region.
    ///
    if (!Ctx->MeRegionEnabled) {
      Status = SpsUpgradeHmrfpoEnable (Ctx->NonceInfo->Nonce, &TempNonceInfo);
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SpsUpgradeHmrfpoEnable => (%r)(%x)\n", Status, TempNonceInfo.Result));
      if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_ENABLE_SUCCESS)) {
        Ctx->NonceInfo->FactoryDefaultBase  = TempNonceInfo.FactoryDefaultBase;
        Ctx->NonceInfo->FactoryDefaultLimit = TempNonceInfo.FactoryDefaultLimit;
        SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME HMRFPO_ENEABLE success. ME region is open.\n"));
        Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_HMRFPO_ENABLE_SUCCESS;
      } else {
        SPS_DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SPS ME HMRFPO_ENEABLE fail. ME region is not open.\n"));
        Ctx->FlowControl.UpgradeStatus = SPS_ME_UPGRADE_STATUS_HMRFPO_ENABLE_FAILURE;
        return Status;
      }
    }

    if (WriteTargetAddr < (mSpsMeMapInfo.MeRegionBase + Ctx->NonceInfo->FactoryDefaultLimit)) {
      ///
      /// If target address within ME region and protected region,
      /// in other words update whole ME region.
      ///
      Ctx->FlowControl.UpgradePhase = SPS_ME_UPGRADE_PHASE_ENTIRE_ME_REGION;
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Update whole ME region\n"));
    }

    if (WriteTargetAddr > (mSpsMeMapInfo.MeRegionBase + Ctx->NonceInfo->FactoryDefaultLimit - 1)) {
      ///
      /// If target address within ME region but not within protected region,
      /// only update operational region.
      ///
      Ctx->FlowControl.UpgradePhase = SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION;
      SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Only update operational region\n"));
    }
  }

  ///
  /// Upgrage mode setting.
  ///
  if (!mNewSpsMeMapInfo.Valid) {
    Ctx->FlowControl.UpgradeMode = SPS_ME_UPGRADE_MODE_DEFAULT;
  }

  switch (Ctx->FlowControl.UpgradeMode) {
  case SPS_ME_UPGRADE_MODE_DEFAULT:
  case SPS_ME_UPGRADE_MODE_ONLY_ENABLE_HMRFPO:
    SPS_DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Only enable HMRFPO. => (%r)\n\n", Status));
    break;

  case SPS_ME_UPGRADE_MODE_FULL_UPGRADE:
    break;

  default:
    break;
  }

  Status = SpsUpgradeHandler (This);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Ctx->FlowControl.UpgradeDone) {
    if (mSpsMeMapInfo.SubRegionInfo != NULL) {
      FreePool (mSpsMeMapInfo.SubRegionInfo);
      mSpsMeMapInfo.SubRegionInfo = NULL;
    }
    if (mNewSpsMeMapInfo.SubRegionInfo != NULL) {
      FreePool (mNewSpsMeMapInfo.SubRegionInfo);
      mNewSpsMeMapInfo.SubRegionInfo = NULL;
    }

    Status = EFI_WRITE_PROTECTED;
  }

  ///
  /// If reach hear, upgrade progress is doen.
  ///
  return Status;
}

/**
  Initialize SPS ME Upgrade Support Context

  @param[in, out] SpsMeUpgradeCtx Point to SPS upgrade context.

  @retval EFI_SUCCESS             Initialize SPS ME Upgrade Support Context successfully.
**/
EFI_STATUS
EFIAPI
InitializeContext (
  IN OUT SPS_ME_UPGRADE_CONTEXT        *SpsMeUpgradeCtx
  )
{
  SPS_ME_UPGRADE_CONTEXT     *SpsMeUpgradeCtxDxe;
  SPS_ME_UPGRADE_PROTOCOL    *SpsMeUpgrade;
  EFI_STATUS                 Status;
  UINTN                      NumberOfProtectRomMap;
  UINTN                      TempMeRegionBase;
  UINTN                      TempMeRegionLimit;
  UINTN                      TempRegionBase;
  UINTN                      TempRegionLimit;
  SPS_ME_NONCE_INFO          TempNonceInfo;
  UINT32                     SpsMeStatus;
  UINT32                     TempOperationImageSize;

  SpsMeUpgradeCtxDxe    = NULL;
  SpsMeUpgrade          = NULL;
  Status                = EFI_UNSUPPORTED;
  NumberOfProtectRomMap = 0;
  TempMeRegionBase      = 0;
  TempMeRegionLimit     = 0;
  TempRegionBase        = 0;
  TempRegionLimit       = 0;

  if (SpsMeUpgradeCtx == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&mSpsMeNonceInfo, sizeof (SPS_ME_NONCE_INFO));
  ZeroMem (&mSpsMeMapInfo, sizeof (SPS_ME_UPGRADE_MAP_INFO));
  ZeroMem (&mNewSpsMeMapInfo, sizeof (SPS_ME_UPGRADE_MAP_INFO));
  ZeroMem (SpsMeUpgradeCtx, sizeof (SPS_ME_UPGRADE_CONTEXT));

  SpsMeUpgradeCtx->Signature                     = SPS_ME_UPGRADE_CONTEXT_SIGNATURE;
  SpsMeUpgradeCtx->NonceInfo                     = &mSpsMeNonceInfo;

  SpsMeUpgradeCtx->NumberOfProtectRomMap         = 0;
  SpsMeUpgradeCtx->ProtectRomMap                 = NULL;

  SpsMeUpgradeCtx->MeRegionEnabled               = FALSE;

  SpsMeUpgradeCtx->SpsMeUpgrade.Revision         = SPS_ME_UPGRADE_PROTOCOL_REVISION_1;
  SpsMeUpgradeCtx->SpsMeUpgrade.GetProtectRomMap = GetProtectRomMap;
//  SpsMeUpgradeCtx->SpsMeUpgrade.UpgradeProgress  = UpgradeProgress;
  SpsMeUpgradeCtx->SpsMeUpgrade.UpgradeProgress  = UpgradeProgress2;
  SpsMeUpgradeCtx->SpsMeUpgrade.UpgradeComplete  = UpgradeComplete;

  SpsMeUpgradeCtx->SpsMeUpgrade.PassImage        = PassImage;
  SpsMeUpgradeCtx->SpsMeUpgrade.SetMode          = SetMode;
  SpsMeUpgradeCtx->FlowControl.UpgradeMode       = SPS_ME_UPGRADE_MODE_DEFAULT;
  SpsMeUpgradeCtx->FlowControl.UpgradeModeLimit  = SPS_ME_UPGRADE_MODE_DEFAULT;
  SpsMeUpgradeCtx->FlowControl.UpgradePhase      = SPS_ME_UPGRADE_PHASE_FIRST_TIME;
  SpsMeUpgradeCtx->FlowControl.UpgradeStatus     = SPS_ME_UPGRADE_STATUS_DEFAULT;
  SpsMeUpgradeCtx->FlowControl.UpgradeTableIndex = 0;
  SpsMeUpgradeCtx->FlowControl.SpiBlockIndex     = 0;
  SpsMeUpgradeCtx->FlowControl.DualImageMode     = FALSE;
  SpsMeUpgradeCtx->FlowControl.BlockMode         = SpsUpgradeIsBlockMode ();
  SpsMeUpgradeCtx->FlowControl.InactiveOPR       = SPS_ME_SUB_REGION_SIGNATURE_OPR1;
  SpsMeUpgradeCtx->FlowControl.UpgradeDone       = FALSE;
  SpsMeUpgradeCtx->FlowControl.ImageInvalid      = FALSE;

  // Get Flash ROM Base address.
  Status = SpsUpgradeGetRomBaseAddress (&mSpsRomBaseAddress);
  if (EFI_ERROR (Status)) {
    mSpsRomBaseAddress = 0;
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeGetRomBaseAddress() => (%r)\n", Status));
  }
  // Get ME region base address and limit address.
  Status = SpsUpgradeGetMeRegion (&TempMeRegionBase, &TempMeRegionLimit);
  if (EFI_ERROR (Status)) {
    TempMeRegionBase  = 0;
    TempMeRegionLimit = 0;
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: SpsUpgradeGetMeRegion() => (%r)\n", Status));
  }
  if ((mSpsRomBaseAddress == 0) || (TempMeRegionBase == 0) || (TempMeRegionLimit == 0)) {
    mSpsMeMapInfo.MeRegionBase  = 0;
    mSpsMeMapInfo.MeRegionLimit = 0;
    mSpsMeMapInfo.Valid         = FALSE;
  } else {
    mSpsMeMapInfo.MeRegionBase  = mSpsRomBaseAddress + TempMeRegionBase;
    mSpsMeMapInfo.MeRegionLimit = mSpsRomBaseAddress + TempMeRegionLimit;
    mSpsMeMapInfo.MeRegionSize  = mSpsMeMapInfo.MeRegionLimit - mSpsMeMapInfo.MeRegionBase + 1;
    mSpsMeMapInfo.Valid         = TRUE;
  }

  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPI Base Address       : 0x%lXh\n", mSpsRomBaseAddress));
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: ME Region Base Address : 0x%lXh\n", mSpsMeMapInfo.MeRegionBase));
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: ME Region Limit        : 0x%lXh\n", mSpsMeMapInfo.MeRegionLimit));

  Status = gBS->LocateProtocol (
                  &gSpsDxeMeUpgradeProtocolGuid,
                  NULL,
                  (VOID **)&SpsMeUpgrade
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Could not find SPS ME upgrade protocol => (%r)!\n", Status));
    SpsMeUpgrade = NULL;
  }
  if (SpsMeUpgrade != NULL) {
    SpsMeUpgradeCtxDxe = SPS_ME_UPGRADE_CONTEXT_FROM_THIS (SpsMeUpgrade);
    if (SpsMeUpgradeCtxDxe->NonceInfo != NULL) {
      CopyMem (&mSpsMeNonceInfo, (SpsMeUpgradeCtxDxe->NonceInfo), sizeof (SPS_ME_NONCE_INFO));
    }
  }

  ///
  /// Check Image mode.
  ///
  SpsHeciGetFwVersion (&mSpsMeMapInfo.FwVersion);
  if ((mSpsMeMapInfo.FwVersion.BkpFw.MajorNumber == 0) && \
      (mSpsMeMapInfo.FwVersion.BkpFw.MinorNumber == 0) && \
      (mSpsMeMapInfo.FwVersion.BkpFw.PatchNumber == 0) && \
      (mSpsMeMapInfo.FwVersion.BkpFw.BuildNumber == 0) && \
      (mSpsMeMapInfo.FwVersion.BkpFw.ServerSegmentCode == 0)) {
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Single image mode.\n"));
    SpsMeUpgradeCtx->FlowControl.DualImageMode = FALSE;
  } else {
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Dual image mode.\n"));
    SpsMeUpgradeCtx->FlowControl.DualImageMode = TRUE;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///    Step (A13) : If Direct ME Firmware Update is supported BIOS sends HMRFPO_LOCK
  ///                 message to retrieve the nonce word from ME firmware, and protects the
  ///                 ME region area with PRx registers. Nonce could then be used to perform
  ///                 the Direct Firmware Update and should be preserved in the memory region
  ///                 hidden and protected from the OS (SMM memory region).
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Grantley Revision 1.0.1(#516145) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///    Step (A12) : If Direct ME Firmware Update is supported BIOS sends HMRFPO_LOCK message 
  ///                 to retrieve the nonce word from ME firmware, and protects the ME region area 
  ///                 with SPI Protected Range Register (PRx). Nonce could be then used to perform 
  ///                 the Direct ME Firmware Update and should be preserved in the memory region 
  ///                 hidden and protected from the OS (SMM memory region). 
  ///
  SpsMeUpgradeCtx->NonceInfo->Revision = SPS_ME_NONCE_STORE_REVISION_1;
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_GET_STATUS to get HMRFPO status...Start\n"));
  Status = SpsHeciHmrfpoGetStatus (&TempNonceInfo.Result);
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_GET_STATUS to get HMRFPO status...End => (%r)(%x)\n", Status, TempNonceInfo.Result));
  if (EFI_ERROR (Status) || (TempNonceInfo.Result != SPS_HMRFPO_STS_LOCKED)) {
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Send HMRFPO_LOCK Message...\n"));
    Status = SpsUpgradeHmrfpoLock (&TempNonceInfo);
    if (!EFI_ERROR (Status) && (TempNonceInfo.Result == SPS_HMRFPO_LOCK_SUCCESS)) {
      if (SpsMeUpgradeCtx->NonceInfo->Nonce == 0) {
        SpsMeUpgradeCtx->NonceInfo->Nonce             = TempNonceInfo.Nonce;
      }
      SpsMeUpgradeCtx->NonceInfo->FactoryDefaultBase  = TempNonceInfo.FactoryDefaultBase;
      SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit = TempNonceInfo.FactoryDefaultLimit;
      SpsMeUpgradeCtx->NonceInfo->Result              = TempNonceInfo.Result;
    } else {
      SpsMeUpgradeCtx->NonceInfo->Result              = SPS_HMRFPO_LOCK_FAIL;
    }
  }

  ///
  /// In Recovery mode, Factory default limit is 0,
  ///   Protect Factory default by SPS policy.
  ///   if Dual image, Add one Operation image size.
  ///
  SpsGetMeStatus (&SpsMeStatus);
  if (IS_SPS_ME_CURSTATE_RECOVERY (SpsMeStatus)) {
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME firmware in Recovery mode.\n"));
    if (SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit == 0) {
      SpsUpgradeGetFactoryDefaultSize (&TempNonceInfo.FactoryDefaultLimit);
      if (SpsMeUpgradeCtx->FlowControl.DualImageMode) {
        SpsUpgradeGetOperationImageSize (&TempOperationImageSize);
        TempNonceInfo.FactoryDefaultLimit += TempOperationImageSize;
      }
      SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit = TempNonceInfo.FactoryDefaultLimit;
    }
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect Factory Default area. (0x%08x)\n", SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit));
  }

  DEBUG ((DEBUG_INFO, "\n"));
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Nonce info\n"));
  DEBUG ((DEBUG_INFO, "  Nonce               : 0x%lx\n", SpsMeUpgradeCtx->NonceInfo->Nonce));
  DEBUG ((DEBUG_INFO, "  FactoryDefaultBase  : 0x%x\n", SpsMeUpgradeCtx->NonceInfo->FactoryDefaultBase));
  DEBUG ((DEBUG_INFO, "  FactoryDefaultLimit : 0x%x\n", SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit));
  DEBUG ((DEBUG_INFO, "  Result              : 0x%x\n", SpsMeUpgradeCtx->NonceInfo->Result));

  ///
  /// Count number of protect map.
  ///   1. Descriptor region.
  ///   2. Factory Default data.
  ///   3. GBE.
  ///   4. Platform Data Regione(PDR).
  ///   5. Device Expansion Region(DER).
  ///
  /// Protect Descriptor region. if need.
  ///
  if (SpsMeProtectDescriptorRegion ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_DESC_REGION)) || (!SpsUpgradeCheckBRRA (SPS_DESC_REGION))) {
      NumberOfProtectRomMap++;
    }
  }

  ///
  /// Protect GBE Region. if need.
  ///
  if (SpsMeProtectGBE ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_GBE_REGION)) || (!SpsUpgradeCheckBRRA (SPS_GBE_REGION))) {
      NumberOfProtectRomMap++;
    }
  }

  ///
  /// Protect Platform Data Region(PDR). if need.
  ///
  if (SpsMeProtectPDR ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_PDR_REGION)) || (!SpsUpgradeCheckBRRA (SPS_PDR_REGION))) {
      NumberOfProtectRomMap++;
    }
  }

  ///
  /// Protect Device Expansion Region(DER). if need.
  ///
  if (SpsMeProtectDER ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_DER_REGION)) || (!SpsUpgradeCheckBRRA (SPS_DER_REGION))) {
      NumberOfProtectRomMap++;
    }
  }

  ///
  /// Protect Factory Default data. if exist.
  ///
  if (SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit != 0) {
    NumberOfProtectRomMap++;
  }

  ///
  /// Allocate memory for Protect map.
  ///
  if (NumberOfProtectRomMap == 0) {
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Not Protect any region\n", Status));
    return EFI_SUCCESS;
  } else {
    SpsMeUpgradeCtx->ProtectRomMap = (SPS_ME_PROTECT_ROM_MAP *) AllocateZeroPool ((NumberOfProtectRomMap * sizeof (SPS_ME_PROTECT_ROM_MAP)));
  }

  if (SpsMeUpgradeCtx->ProtectRomMap != NULL) {
    SpsMeUpgradeCtx->NumberOfProtectRomMap = NumberOfProtectRomMap;
  } else {
    ///
    /// Allocate memory fail.
    ///
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Allocate memory for Protect map fail.\n"));
    return EFI_SUCCESS;
  }

  ///
  /// Check Region write access for Region 0 Flash Descriptor.
  /// If FALSE, Adding it in protect map.
  ///
  if (SpsMeProtectDescriptorRegion ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_DESC_REGION)) || (!SpsUpgradeCheckBRRA (SPS_DESC_REGION))) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect Descriptor Regione by SPS.\n"));
      Status = SpsUpgradeGetDescRegion (&TempRegionBase, &TempRegionLimit);
      if (!EFI_ERROR (Status)) {
        NumberOfProtectRomMap--;
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase  = (mSpsRomBaseAddress + TempRegionBase);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit = (mSpsRomBaseAddress + TempRegionLimit);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute    = SPS_ME_PROTECT_ROM_NON_OVERRIDABLE;
      } else {

      }
    }
  }

  ///
  /// Check Region 3 GBE Read access and Write access .
  /// If FALSE, Adding it in protect map.
  ///
  if (SpsMeProtectGBE ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_GBE_REGION)) || (!SpsUpgradeCheckBRRA (SPS_GBE_REGION))) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect GBE by SPS.\n"));
      Status = SpsUpgradeGetGBERegion (&TempRegionBase, &TempRegionLimit);
      if (!EFI_ERROR (Status)) {
        NumberOfProtectRomMap--;
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase  = (mSpsRomBaseAddress + TempRegionBase);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit = (mSpsRomBaseAddress + TempRegionLimit);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute    = SPS_ME_PROTECT_ROM_OVERRIDABLE;
      } else {

      }
    }
  }
  ///
  /// Check Region 4 Platform Data Region(PDR) Read access and Write access .
  /// If FALSE, Adding it in protect map.
  ///
  if (SpsMeProtectPDR ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_PDR_REGION)) || (!SpsUpgradeCheckBRRA (SPS_PDR_REGION))) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect Platform Data Regione(PDR) by SPS.\n"));
      Status = SpsUpgradeGetPDRRegion (&TempRegionBase, &TempRegionLimit);
      if (!EFI_ERROR (Status)) {
        NumberOfProtectRomMap--;
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase  = (mSpsRomBaseAddress + TempRegionBase);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit = (mSpsRomBaseAddress + TempRegionLimit);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute    = SPS_ME_PROTECT_ROM_OVERRIDABLE;
      } else {

      }
    }
  }
  ///
  /// Check Region 5 Device Expansion Region(DER) Read access and Write access.
  /// If FALSE, Adding it in protect map.
  ///
  if (SpsMeProtectDER ()) {
    if ((!SpsUpgradeCheckBRWA (SPS_DER_REGION)) || (!SpsUpgradeCheckBRRA (SPS_DER_REGION))) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect Device Expansion Region(DER) by SPS.\n"));
      Status = SpsUpgradeGetDERRegion (&TempRegionBase, &TempRegionLimit);
      if (!EFI_ERROR (Status)) {
        NumberOfProtectRomMap--;
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase  = (mSpsRomBaseAddress + TempRegionBase);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit = (mSpsRomBaseAddress + TempRegionLimit);
        SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute    = SPS_ME_PROTECT_ROM_OVERRIDABLE;
      } else {

      }
    }
  }

  if (SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit != 0) {
    NumberOfProtectRomMap--;
    SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase  = (mSpsMeMapInfo.MeRegionBase + SpsMeUpgradeCtx->NonceInfo->FactoryDefaultBase);
    SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit = (mSpsMeMapInfo.MeRegionBase + SpsMeUpgradeCtx->NonceInfo->FactoryDefaultLimit - 1);
    SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute    = SPS_ME_PROTECT_ROM_OVERRIDABLE;
  } else {
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Factory Default Limit is zero!\n", Status));
  }

  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPI Protect MAP\n"));
  for (NumberOfProtectRomMap = 0; NumberOfProtectRomMap < SpsMeUpgradeCtx->NumberOfProtectRomMap; NumberOfProtectRomMap++) {
    DEBUG ((DEBUG_INFO, "           %d |- Base       : 0x%lXh\n", NumberOfProtectRomMap, SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectBase));
    DEBUG ((DEBUG_INFO, "             |- Limit      : 0x%lXh\n", SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].ProtectLimit));
    DEBUG ((DEBUG_INFO, "             |- Attribute  : 0x%xh\n\n", SpsMeUpgradeCtx->ProtectRomMap[NumberOfProtectRomMap].Attribute));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SpsMeFwUpgradeSmmEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_SMM_RUNTIME_PROTOCOL    *SmmRt;
  EFI_HANDLE                  SpsMeImageHandle;

  Status = EFI_UNSUPPORTED;

  if (!SpsMeFwUpgradeSupported ()) {
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: SPS ME Firmware Upgrade => Not Supported\n"));
    return EFI_UNSUPPORTED;
  }

  Status = InitializeContext (&mSmmSpsMeUpgradeCtx);
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Initialize SPS ME FW Upgrade Support context => (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SpsMeImageHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &SpsMeImageHandle,
                    &gSpsSmmMeUpgradeProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(mSmmSpsMeUpgradeCtx.SpsMeUpgrade)
                    );
  DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: (gSmst)Install SMM SPS ME Upgrade Support protocol => (%r)\n", Status));

  ///
  /// For ECP module compatibility, skip Status check
  ///
  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRt);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Locate Protocol Smm Runtime Protocol Failure!\n"));
    SmmRt = NULL;
  } else {
    Status = SmmRt->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRt);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Locate Protocol Smm Runtime Protocol Failure!\n"));
      SmmRt = NULL;
    }
  }
  if (SmmRt != NULL) {
    Status = SmmRt->InstallProtocolInterface (
                      &ImageHandle,
                      &gSpsSmmMeUpgradeProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &(mSmmSpsMeUpgradeCtx.SpsMeUpgrade)
                      );
    DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: (SmmRt)Install SMM SPS ME Upgrade Support protocol => (%r)\n", Status));
  }

  ///
  /// Protect Descriptor Region in Protected Range x Register, if need.
  ///
  if (SpsMeProtectDescriptorRegion ()) {
    Status = SpsUpgradeProtectDescriptorRegion ();
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect Descriptor Region => (%r)\n", Status));
    } else {
      DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Protect Descriptor Region => (%r)\n", Status));
    }
  }

  ///
  /// Protect ME Region in Protected Range x Register, if need.
  ///
  if (SpsMeProtectMeRegion ()) {
    Status = SpsUpgradeProtectMeRegion (mSpsMeNonceInfo.FactoryDefaultBase, mSpsMeNonceInfo.FactoryDefaultLimit);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, SPS_DEBUG_NAME"INFO: Protect ME Region => (%r)\n", Status));
    } else {
      DEBUG ((DEBUG_ERROR, SPS_DEBUG_NAME"ERROR: Protect ME Region => (%r)\n", Status));
    }
  }

  return EFI_SUCCESS;
}
