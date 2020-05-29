/** @file

  MeFwDowngrade driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>

#include <MeLib.h>
#include <MeAccess.h>
#include <Library/Amt/Dxe/AmtLib.h>
#include <Heci/Include/HeciRegs.h>

#include <Guid/MePlatformReadyToBoot.h>

#define ERROR_HANDLING_DELAY  7000000 /// show warning msg and stay for 7 seconds.
#define FW_MSG_DELAY          1000    /// show warning msg and stay for 1 milisecond.
#define FW_MSG_DELAY_TIMEOUT  10

UINT8 HeciHmrfpoLockResult;
UINT8 HeciHmrfpoEnableResult;

/**
  Tell User that we failed to lock/unlock the flash - Do this after we have graphics initialized

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context

  @retval None
**/
VOID
EFIAPI
MeFwDowngradeMsgEvent (
  IN EFI_EVENT                    Event,
  IN VOID                         *Context
  )
{
  if (HeciHmrfpoLockResult != HMRFPO_LOCK_SUCCESS) {
    MeReportError (MSG_HMRFPO_LOCK_FAILURE);
  }

  if (HeciHmrfpoEnableResult != HMRFPO_ENABLE_SUCCESS) {
    MeReportError (MSG_HMRFPO_UNLOCK_FAILURE);
  }

  gBS->CloseEvent (Event);
  return;
}

/**
  Send the HMRFPO_DISABLE MEI message.

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context

  @retval None
**/
VOID
EFIAPI
MeHmrfpoDisableEvent (
  EFI_EVENT                       Event,
  VOID                            *Context
  )
{
  UINT32  FWstatus;
  UINT8   WriteValue;
  UINT8   StallCount;

  FWstatus    = 0;
  WriteValue  = 0;
  StallCount  = 0;

  DEBUG ((EFI_D_ERROR, "(B3) Me FW Downgrade - Send the HMRFPO_DISABLE MEI message\n"));

  WriteValue  = HeciPciRead8 (R_GEN_STS + 3);
  WriteValue  = WriteValue & BRNGUP_HMRFPO_DISABLE_CMD_MASK;
  WriteValue  = WriteValue | BRNGUP_HMRFPO_DISABLE_CMD;
  DEBUG ((EFI_D_ERROR, "Me FW Downgrade Writing %x to register %x of PCI space\n", WriteValue, (R_GEN_STS + 3)));
  ///
  /// Set the highest Byte of General Status Register (Bits 28-31)
  ///
  HeciPciWrite8 (R_GEN_STS + 3, WriteValue);
  FWstatus = HeciPciRead32 (R_FWSTATE);
  while
  (
    ((FWstatus & BRNGUP_HMRFPO_DISABLE_OVR_MASK) != BRNGUP_HMRFPO_DISABLE_OVR_RSP) &&
    (StallCount < FW_MSG_DELAY_TIMEOUT)
  ) {
    DEBUG ((EFI_D_ERROR, "Me FW Downgrade - ME HMRFPO Disable Status = 0x%x\n", FWstatus));
    FWstatus = HeciPciRead32 (R_FWSTATE);
    gBS->Stall (FW_MSG_DELAY);
    StallCount = StallCount + 1;
  }

  if ((FWstatus & BRNGUP_HMRFPO_DISABLE_OVR_MASK) == BRNGUP_HMRFPO_DISABLE_OVR_RSP) {
    DEBUG ((EFI_D_ERROR, "Me FW Downgrade Disable Msg Received Successfully\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "Me FW Downgrade Disable Msg ACK not Received\n"));

  }
  ///
  /// Hide ME devices so we don't get a yellow bang in OS with disabled devices
  ///
  DisableAllMEDevices ();

  gBS->CloseEvent (Event);

  return;
}

/**
  Entry point for the MeFwDowngrade driver

  @param[in] ImageHandle          Standard entry point parameter.
  @param[in] SystemTable          Standard entry point parameter.

  @exception EFI_UNSUPPORTED      ME policy library initization failure.
  @retval                         Status code returned by CreateEventEx.
**/
EFI_STATUS
EFIAPI
MeFwDowngradeEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS        Status;
  EFI_EVENT         MeFwDowngradeEvent;
  EFI_HECI_PROTOCOL *Heci;
  UINT32            MeMode;
  UINT32            MeStatus;
  UINT64            Nonce;
  UINT32            FactoryDefaultBase;
  UINT32            FactoryDefaultLimit;
  HECI_FWS_REGISTER MeFirmwareStatus;

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  ASSERT_EFI_ERROR (Status);

  Status = Heci->GetMeMode (&MeMode);
  ASSERT_EFI_ERROR (Status);

  Status = Heci->GetMeStatus (&MeStatus);
  ASSERT_EFI_ERROR (Status);

  ///
  /// (B1) Whcih mode ?
  ///
  if (MeMode == ME_MODE_NORMAL) {
    ///
    /// (A2) Intel Me image re-flash is requested?
    ///
    if (MeFwDowngradeSupported ()) {
      ///
      /// (A4)(A5) The BIOS sends the HMRFPO ENABLE MEI message and wiat for response.
      ///
      HeciHmrfpoEnableResult  = HMRFPO_ENABLE_UNKNOWN_FAILURE;
      Status                  = HeciHmrfpoEnable (0, &HeciHmrfpoEnableResult);
      if (Status == EFI_SUCCESS && HeciHmrfpoEnableResult == HMRFPO_ENABLE_SUCCESS) {
        ///
        /// (A6) The BIOS sends the GLOBAL RESET MEI message
        ///
        DEBUG ((EFI_D_ERROR, "Me FW Downgrade !!- Step A6\n"));

        HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
        CpuDeadLoop ();
      }
      ///
      /// (A8) Error Handling, HeciHmrfpoEnable include error handling.
      ///
      DEBUG (
        (
        EFI_D_ERROR, "Me FW Downgrade Error !!- Step A8, the Status is %r, The result is %x\n", Status,
        HeciHmrfpoEnableResult
        )
        );
      IoWrite8 (0x80, 0xA8);
    } else {
      ///
      /// (A7) The BIOS sends the HMRFPO Lock MEI message and continues the normal boot
      ///
      HeciHmrfpoLockResult = HMRFPO_LOCK_SUCCESS;
      ///
      /// The ME firmware will ignore the HMRFPO LOCK command if ME is in ME manufacturing mode
      ///
      if ((MeFirmwareStatus.r.ManufacturingMode == 0) &&
          (
            (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_IN_RECOVERY_MODE) ||
          (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY)
        )
          ) {

        DEBUG (
          (
          EFI_D_ERROR,
          "(A7) Me FW Downgrade - The BIOS sends the HMRFPO Lock MEI message and continues the normal boot\n"
          )
          );

        FactoryDefaultBase  = 0;
        FactoryDefaultLimit = 0;
        Status              = HeciHmrfpoLock (&Nonce, &FactoryDefaultBase, &FactoryDefaultLimit, &HeciHmrfpoLockResult);
        if (Status != EFI_SUCCESS) {
          HeciHmrfpoLockResult = HMRFPO_LOCK_FAILURE;
        }
      }
    }

    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    MeFwDowngradeMsgEvent,
                    (VOID *) &ImageHandle,
                    &gMePlatformReadyToBootGuid,
                    &MeFwDowngradeEvent
                    );

  }
  ///
  /// (B3) Create an event that will call the HMRFPO_DISABLE
  ///
  if ((MeFirmwareStatus.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_HECI_MSG) &&
      (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY)
      ) {
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    MeHmrfpoDisableEvent,
                    (VOID *) &ImageHandle,
                    &gMePlatformReadyToBootGuid,
                    &MeFwDowngradeEvent
                    );
  }

  return Status;
}
