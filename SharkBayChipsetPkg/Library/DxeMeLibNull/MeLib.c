/** @file

  A emptry template implementation of ME Library.

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

#include <Uefi.h>
#include <MeLib.h>

/**
  Check if Me is enabled

  @param[in] None.

  @retval None
**/
EFI_STATUS
MeLibInit (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku            Return Data from Get Firmware Capabilities MKHI Request

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwCapsSku (
  MEFWCAPS_SKU                    *FwCapsSku
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  4M or 8M size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetPlatformType (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
{
  return EFI_UNSUPPORTED;
}

VOID
MbpGiveUp (
  IN  VOID
  )
{
  return;
}

/**
  Routine checks whether MBP buffer has been cleared form HECI buffer or not.
  BIOS must check this before executing any 3rd paty code or Oprom

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context
**/
VOID
EFIAPI
MbpCleared (
  IN EFI_EVENT                    Event,
  IN VOID                         *Context
  )
{
  return;
}

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval UINT8                   Number of filled slots.
**/
UINT8
FilledSlots2 (
  IN UINT32                       ReadPointer,
  IN UINT32                       WritePointer
  )
{
  return 0;
}

/**
  This routine returns ME-BIOS Payload information.

  @param[out] MbpPtr              ME-BIOS Payload information.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        Failed to consume MBP
**/
EFI_STATUS
PrepareMeBiosPayload (
  OUT ME_BIOS_PAYLOAD             *MbpPtr
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Send Get Firmware Version Request to ME

  @param[in] MsgGenGetFwVersionAckData  Return themessage of FW version

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwVersion (
  IN OUT GEN_GET_FW_VER_ACK_DATA  *MsgGenGetFwVersionAckData
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Host client gets Firmware update info from ME client

  @param[in] MECapability         Structure of FirmwareUpdateInfo

  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
HeciGetMeFwInfo (
  IN OUT ME_CAP                   *MECapability
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Dummy return for Me signal event use

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context
**/
VOID
EFIAPI
MeEmptyEvent (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  return;
}

/**
  Get AT State Information From Stored ME platform policy

  @param[in] AtState              Pointer to AT State Information
  @param[in] AtLastTheftTrigger   Pointer to Variable holding the cause of last AT Stolen Stae
  @param[in] AtLockState          Pointer to variable indicating whether AT is locked or not
  @param[in] AtAmPref             Pointer to variable indicating whether ATAM or PBA should be used

  @retval EFI_SUCCESS             The function completed successfully
  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
GetAtStateInfo (
  IN AT_STATE_INFO                *AtStateInfo
  )
{
  return EFI_UNSUPPORTED;
}
