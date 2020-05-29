/** @file
  Implementation file for Heci Message functionality

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2006 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
//[-start-120808-IB10820098-modify]//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <HeciMsgLib.h>
#include <MeAccess.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>

#include <Guid/StatusCodeDataTypeId.h>
#include <Protocol/Wdt.h>
#include <Protocol/PchReset.h>


BOOLEAN
IsAfterEndOfPost (
  VOID
  );

//
// Internal function for HeciMsgLib used only
//

/**
  Convert EFI Status Code severity to Mdes severity.

  @param[in] statusToConv         EFI Status Code severity.

  @retval UINT16                  Mdes severity.
**/
STATIC
UINT16
BiosToMdesSeverity (
  IN EFI_STATUS                   statusToConv
  )
{
  UINT16  MdesSev;

  MdesSev = SEV_NO_ERROR;
  switch (statusToConv & EFI_STATUS_CODE_SEVERITY_MASK) {
  case (EFI_ERROR_MINOR):
    MdesSev = SEV_LOW_ERROR;
    break;

  case (EFI_ERROR_MAJOR):
    MdesSev = SEV_HIGH_ERROR;
    break;

  case (EFI_ERROR_UNRECOVERED):
    MdesSev = SEV_CRITICAL_ERROR;
    break;

  case (EFI_ERROR_UNCONTAINED):
    MdesSev = SEV_CRITICAL_ERROR;
    break;
  }

  return MdesSev;
}

//
// Interface functions of HeciMsgLib
//

/**
  Send the required system ChipsetInit Table to ME FW.

  @param[in] ChipsetInitTable     The required system ChipsetInit Table.
  @param[in] ChipsetInitTableLen  Length of the table in bytes

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciChipsetInitSyncMsg (
  IN  UINT8                      *ChipsetInitTable,
  IN  UINT32                      ChipsetInitTableLen
  )
{
  EFI_HECI_PROTOCOL              *Heci;
  EFI_STATUS                     Status;
  UINT32                         ReqSize;
  PCH_RESET_PROTOCOL             *PchResetProtocol;
  EFI_GUID                       PchResetProtocolGuid  = PCH_RESET_PROTOCOL_GUID;
  MPHY_WRITE_SETTINGS_REQ        *MPhyWriteSettingsReqPtr;

  DEBUG ((EFI_D_ERROR, "HeciChipsetInitSyncMsg(0x%08X, %d): Start\n", ChipsetInitTable, ChipsetInitTableLen));
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8130);
  ASSERT(ChipsetInitTableLen <= 1024);  // ChipsetInit table should not get too large

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "EfiHeciProtocol not found.\n"));
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &PchResetProtocolGuid,
                  NULL,
                  (VOID **) &PchResetProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "PchResetProtocol not found.\n"));
    return Status;
  }

  //
  // Allocate a buffer for the Request Structure and the ChipsetInit Table
  //
  ReqSize = sizeof (MPHY_WRITE_SETTINGS_REQ) + ChipsetInitTableLen;
  MPhyWriteSettingsReqPtr = AllocateZeroPool (ReqSize);
  if (MPhyWriteSettingsReqPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "(MPHY) HeciChipsetInitSyncMsg: Could not allocate Memory\n"));
    return EFI_ABORTED;
  }

  //
  // Setup the HECI message for a MPHY Write
  //
  MPhyWriteSettingsReqPtr->Header.ApiVersion   = LYNX_POINT_PLATFORM;
  MPhyWriteSettingsReqPtr->Header.IccCommand   = WRITE_MPHY_SETTINGS;
  MPhyWriteSettingsReqPtr->Header.BufferLength = ReqSize - sizeof (ICC_HEADER);
  MPhyWriteSettingsReqPtr->PostedWrite         = FALSE;
  CopyMem (MPhyWriteSettingsReqPtr+1, ChipsetInitTable, ChipsetInitTableLen);

  DEBUG ((EFI_D_ERROR, "(MPHY) mPhyChipsetInitTable[] = \n"));
  DEBUG ((EFI_D_ERROR, "    Ver=%d CRC=0x%04X NumEntries=%d\n",
          *((UINT16*)&ChipsetInitTable[2]), *((UINT16*)&ChipsetInitTable[0]), ChipsetInitTable[4]));
  #ifdef EFI_DEBUG
  {
    int ii;
    UINT8* entry_ptr = &ChipsetInitTable[5];
    for (ii = 0; ii < ChipsetInitTable[4]; ii++) {
      DEBUG ((EFI_D_ERROR, "     %d: EP=0x%02X OFFSET=0x%04X VALUE=0x%08X\n",
              ii, entry_ptr[6],*((UINT16*)&entry_ptr[0]),*((UINT32*)&entry_ptr[2])));
      entry_ptr += 7;
    }
  }
  #endif

  //
  // Send ChipsetInit Table to ME
  //
  Status = Heci->SendwACK (
                  (UINT32 *)MPhyWriteSettingsReqPtr,
                  ReqSize,
                  &ReqSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "(MPHY) Write MPHY Settings Message failed! EFI_STATUS = %r\n", Status));
  }
  else if (MPhyWriteSettingsReqPtr->Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG ((EFI_D_ERROR,"(MPHY) Write MPHY Settings failed!: FW Response=0x%x\n",MPhyWriteSettingsReqPtr->Header.IccResponse));
    Status = EFI_DEVICE_ERROR;
  }

  FreePool (MPhyWriteSettingsReqPtr);
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8140);
    return Status;
  }

  if (PchResetProtocol != NULL) {
    DEBUG ((EFI_D_ERROR, "HeciChipsetInitSyncMsg(): Reset required for ChipsetInit Settings synch\n"));
    PchResetProtocol->Reset (PchResetProtocol, ColdReset);
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8140);
  DEBUG ((EFI_D_ERROR, "HeciChipsetInitSyncMsg(): End\n"));
  return Status;
}

//
// Interface functions of HeciMsgLib
//

/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] ResetOrigin         Reset source
  @param[in] ResetType           Global or Host reset

  @exception EFI_UNSUPPORTED     Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciSendCbmResetRequest (
  IN  UINT8                      ResetOrigin,
  IN  UINT8                      ResetType
  )
{
  EFI_HECI_PROTOCOL              *Heci;
  EFI_STATUS                     Status;
  UINT32                         HeciLength;
  CBM_RESET_REQ                  CbmResetRequest;
  PLATFORM_ME_HOOK_PROTOCOL      *PlatformMeHook;
  EFI_GUID                       WdtProtocolGuid = WDT_PROTOCOL_GUID;
  WDT_PROTOCOL                   *WdtProtocol;
  UINT32                         MeMode;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gPlatformMeHookProtocolGuid,
                  NULL,
                  (VOID **) &PlatformMeHook
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Locate PlatformMeHook Protocol for Global Reset Hook, so skip instead.- %r\n", Status));
  } else {
    PlatformMeHook->PreGlobalReset ();
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  CbmResetRequest.MKHIHeader.Data               = 0;
  CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
  CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
  CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
  CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
  CbmResetRequest.MKHIHeader.Fields.Result      = 0;
  CbmResetRequest.Data.RequestOrigin            = ResetOrigin;
  CbmResetRequest.Data.ResetType                = ResetType;

  HeciLength = sizeof (CBM_RESET_REQ);

  Status = gBS->LocateProtocol (&WdtProtocolGuid, NULL, (VOID **) &WdtProtocol);
  ASSERT_EFI_ERROR (Status);
  WdtProtocol->AllowKnownReset ();

  Status = Heci->SendMsg (
                  (UINT32 *) &CbmResetRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send Reset Request - %r\n", Status));
  }

  return Status;
}

/**
  Send Hardware Asset Tables to Firmware

  @param[in] Handle               A handle for this module
  @param[in] AssetTableData       Hardware Asset Table Data
  @param[in] TableDataSize        Size of Asset table

  @retval EFI_SUCCESS             Table sent
  @retval EFI_ABORTED             Could not allocate Memory
**/
EFI_STATUS
HeciAssetUpdateFwMsg (
  IN EFI_HANDLE                   Handle,
  IN TABLE_PUSH_DATA              *AssetTableData,
  IN UINT16                       TableDataSize
  )
{
  AU_TABLE_PUSH_MSG *SendAssetTableDataMsg;
  EFI_STATUS        Status;
  EFI_HECI_PROTOCOL *Heci;

  Status = EFI_SUCCESS;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Subtract off single byte from TABLE_PUSH_DATA.TableData[1]
  ///
  SendAssetTableDataMsg = AllocateZeroPool (sizeof (AU_TABLE_PUSH_MSG) + MAX_ASSET_TABLE_ALLOCATED_SIZE - 1);
  if (SendAssetTableDataMsg == NULL) {
    DEBUG ((EFI_D_ERROR, "AssetUpdateFwMsg Error: Could not allocate Memory\n"));
    return EFI_ABORTED;
  }

  if (TableDataSize > MAX_ASSET_TABLE_ALLOCATED_SIZE) {
    TableDataSize = MAX_ASSET_TABLE_ALLOCATED_SIZE;
  }

  SendAssetTableDataMsg->Header.Data = 0;
  ///
  /// Subtract off single byte from TABLE_PUSH_DATA.TableData[1]
  ///
  SendAssetTableDataMsg->Header.Fields.MessageLength      = TableDataSize + sizeof (TABLE_PUSH_DATA) - 1;
  SendAssetTableDataMsg->Header.Fields.Command            = HWA_TABLE_PUSH_CMD;
  SendAssetTableDataMsg->Header.Fields.FRUTablePresent    = 1;
  SendAssetTableDataMsg->Header.Fields.SMBIOSTablePresent = 1;
  SendAssetTableDataMsg->Header.Fields.ASFTablePresent    = 1;
  if (AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Length == 0) {
    SendAssetTableDataMsg->Header.Fields.MediaTablePresent = 0;
  } else {
    SendAssetTableDataMsg->Header.Fields.MediaTablePresent = 1;
  }

  CopyMem (&SendAssetTableDataMsg->Data, AssetTableData, SendAssetTableDataMsg->Header.Fields.MessageLength);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8020);
  Status = Heci->SendMsg (
                  (UINT32 *) SendAssetTableDataMsg,
                  SendAssetTableDataMsg->Header.Fields.MessageLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_HWA_CLIENT_ID
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AssetUpdateFwMsg: Failed to Send SendAssetTableDataMsg\n"));

  }
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8021);

  FreePool (SendAssetTableDataMsg);

  return Status;

}

/**
  Send End of Post Request Message through HECI.

  @param[in] Handle               A handle for this module

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciSendEndOfPostMessage (
  IN EFI_HANDLE                   Handle
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  GEN_END_OF_POST_ACK             CbmEndOfPost;
  UINT32                          MeMode;
  EFI_HECI_PROTOCOL               *Heci;
  PCH_RESET_PROTOCOL              *PchResetProtocol;
  EFI_GUID                        PchResetProtocolGuid  = PCH_RESET_PROTOCOL_GUID;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&PchResetProtocolGuid, NULL, (VOID **) &PchResetProtocol);

  if (EFI_ERROR (Status)) {
    PchResetProtocol = NULL;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  CbmEndOfPost.Header.Data              = 0;
  CbmEndOfPost.Header.Fields.Command    = CBM_END_OF_POST_CMD;
  CbmEndOfPost.Header.Fields.IsResponse = 0;
  CbmEndOfPost.Header.Fields.GroupId    = MKHI_GEN_GROUP_ID;
  CbmEndOfPost.Data.RequestedActions    = 0;

  HeciSendLength                        = sizeof (MKHI_MESSAGE_HEADER);
  HeciRecvLength                        = sizeof (GEN_END_OF_POST_ACK);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8030);
  Status = Heci->SendMsg (
                  (UINT32 *) &CbmEndOfPost,
                  HeciSendLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &CbmEndOfPost,
                  &HeciRecvLength
                  );
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8031);

  if (CbmEndOfPost.Data.RequestedActions == HECI_EOP_PERFORM_GLOBAL_RESET) {
    if (PchResetProtocol != NULL) {
      DEBUG ((EFI_D_ERROR, "HeciSendEndOfPostMessage(): Reset requested by FW EOP ACK %r\n"));
      PchResetProtocol->Reset (PchResetProtocol, GlobalReset);
    }
  }

  return Status;
}

/**
  Send Get Firmware SKU Request to ME

  @param[in] MsgGenGetFwCapsSku     Return message for Get Firmware Capability SKU
  @param[in] MsgGenGetFwCapsSkuAck  Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetFwCapsSkuMsg (
  IN OUT GEN_GET_FW_CAPSKU        *MsgGenGetFwCapsSku,
  IN OUT GEN_GET_FW_CAPS_SKU_ACK  *MsgGenGetFwCapsSkuAck
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetFwCapsSku->MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetFwCapsSku->Data.RuleId                   = 0;
  Length = sizeof (GEN_GET_FW_CAPSKU);

  ///
  /// Send Get FW SKU Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8040);
  Status = Heci->SendMsg (
                  (UINT32 *) MsgGenGetFwCapsSku,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_CAPS_SKU_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) MsgGenGetFwCapsSkuAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8042);
    return Status;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8041);
  return Status;
}

/**
  Send Get Firmware Version Request to ME

  @param[in][out] MsgGenGetFwVersionAck   Return themessage of FW version

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK       *MsgGenGetFwVersionAck
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  GEN_GET_FW_VER                  *MsgGenGetFwVersion;
  GEN_GET_FW_VER                  GenGetFwVersion;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Allocate MsgGenGetFwVersion data structure
  ///
  MsgGenGetFwVersion = &GenGetFwVersion;
  MsgGenGetFwVersion->MKHIHeader.Data = 0;
  MsgGenGetFwVersion->MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  MsgGenGetFwVersion->MKHIHeader.Fields.Command = GEN_GET_FW_VERSION_CMD;
  MsgGenGetFwVersion->MKHIHeader.Fields.IsResponse = 0;
  Length = sizeof (GEN_GET_FW_VER);
  ///
  /// Send Get Firmware Version Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8050);
  Status = Heci->SendMsg (
                  (UINT32 *) MsgGenGetFwVersion,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_VER_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) MsgGenGetFwVersionAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8052);
    return Status;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8051);
  return Status;
}

/**
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in] Nonce                Nonce received in previous HMRFPO_ENABLE Response Message
  @param[in] Result               HMRFPO_ENABLE response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoEnable (
  IN  UINT64                      Nonce,
  OUT UINT8                       *Result
  )
{
  EFI_STATUS                      Status;
  EFI_HECI_PROTOCOL               *Heci;
  MKHI_HMRFPO_ENABLE              HmrfpoEnableRequest;
  MKHI_HMRFPO_ENABLE_RESPONSE     HmrfpoEnableResponse;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoEnableRequest.MkhiHeader.Data               = 0;
  HmrfpoEnableRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.Command     = HMRFPO_ENABLE_CMD_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.IsResponse  = 0;
  HmrfpoEnableRequest.Nonce                         = Nonce;

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8070);
  Status = Heci->SendMsg (
                  (UINT32 *) &HmrfpoEnableRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_ENABLE_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE_RESPONSE);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &HmrfpoEnableResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Read HMRFPO_ENABLE_CMD_ID Result - %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8072);
    return Status;
  }

  *Result = HmrfpoEnableResponse.Status;
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8071);

  return Status;
}

/**
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register "Protected Range Limit" field with this value
  @param[out] Result              Status report

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoLock (
  OUT UINT64                      *Nonce,
  OUT UINT32                      *FactoryDefaultBase,
  OUT UINT32                      *FactoryDefaultLimit,
  OUT UINT8                       *Result
  )
{
  EFI_STATUS                      Status;
  EFI_HECI_PROTOCOL               *Heci;
  MKHI_HMRFPO_LOCK                HmrfpoLockRequest;
  MKHI_HMRFPO_LOCK_RESPONSE       HmrfpoLockResponse;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoLockRequest.MkhiHeader.Data               = 0;
  HmrfpoLockRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.Command     = HMRFPO_LOCK_CMD_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.IsResponse  = 0;

  HeciLength = sizeof (MKHI_HMRFPO_LOCK);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8080);
  Status = Heci->SendMsg (
                  (UINT32 *) &HmrfpoLockRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_LOCK_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_LOCK_RESPONSE);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &HmrfpoLockResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to read HMRFPO_LOCK_CMD_ID response - %r.\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8082);
    return Status;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8081);

  *Nonce                = HmrfpoLockResponse.Nonce;
  *FactoryDefaultBase   = HmrfpoLockResponse.FactoryDefaultBase;
  *FactoryDefaultLimit  = HmrfpoLockResponse.FactoryDefaultLimit;
  *Result               = HmrfpoLockResponse.Status;

  return Status;
}

/**
  System BIOS sends this message to get status for HMRFPO_LOCK message.

  @param[out] Result              HMRFPO_GET_STATUS response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoGetStatus (
  OUT UINT8                       *Result
  )
{
  EFI_STATUS                      Status;
  EFI_HECI_PROTOCOL               *Heci;
  MKHI_HMRFPO_GET_STATUS          HmrfpoGetStatusRequest;
  MKHI_HMRFPO_GET_STATUS_RESPONSE HmrfpoGetStatusResponse;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoGetStatusRequest.MkhiHeader.Data              = 0;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.GroupId    = MKHI_SPI_GROUP_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.Command    = HMRFPO_GET_STATUS_CMD_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.IsResponse = 0;

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8090);
  Status = Heci->SendMsg (
                  (UINT32 *) &HmrfpoGetStatusRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_GET_STATUS_CMD_ID - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS_RESPONSE);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &HmrfpoGetStatusResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Read HMRFPO_GET_STATUS_CMD_ID Result - %r\n", Status));
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8091);
  *Result = HmrfpoGetStatusResponse.Status;

  return Status;
}

/**
  This is used to send KVM request message to Intel ME. When
  Bootoptions indicate that a KVM session is requested then BIOS
  will send this message before any graphical display output to
  ensure that FW is ready for KVM session.

  @param[in] QueryType            0 - Query Request
                                  1 - Cancel Request
  @param[out] ResponseCode        1h - Continue, KVM session established.
                                  2h - Continue, KVM session cancelled.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciQueryKvmRequest (
  IN  UINT32                      QueryType,
  OUT UINT32                      *ResponseCode
  )
{
  EFI_STATUS                      Status;
  EFI_HECI_PROTOCOL               *Heci;
  AMT_QUERY_KVM_REQUEST           QueryKvmRequest;
  AMT_QUERY_KVM_RESPONSE          QueryKvmResponse;
  UINT32                          HeciLength;
  UINT16                          TimeOut;
  UINT32                          MeMode;
//[-start-110830-IB06460300-add]//
  UINT32                          MeStatus;
//[-end-110830-IB06460300-add]//

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

//[-start-110830-IB06460300-add]//
  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }
//[-end-110830-IB06460300-add]//

  QueryKvmRequest.Command       = EFI_KVM_MESSAGE_COMMAND;
  QueryKvmRequest.ByteCount     = EFI_KVM_BYTE_COUNT;
  QueryKvmRequest.SubCommand    = EFI_KVM_QUERY_REQUES;
  QueryKvmRequest.VersionNumber = EFI_KVM_VERSION;
  QueryKvmRequest.QueryType     = QueryType;

  HeciLength                    = sizeof (AMT_QUERY_KVM_REQUEST);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8060);
  Status = Heci->SendMsg (
                  (UINT32 *) &QueryKvmRequest,
                  HeciLength,
                  BIOS_ASF_HOST_ADDR,
                  HECI_ASF_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Query KVM failed %r\n", Status));
  }

  TimeOut     = 0;
  HeciLength  = sizeof (AMT_QUERY_KVM_RESPONSE);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &QueryKvmResponse,
                  &HeciLength
                  );

  if (QueryType == QUERY_REQUEST) {
    while (EFI_ERROR (Status)) {
      gBS->Stall (EFI_KVM_STALL_1_SECOND);
      TimeOut++;

      if (TimeOut > EFI_KVM_MAX_WAIT_TIME) {
        break;
      }

      HeciLength = sizeof (AMT_QUERY_KVM_RESPONSE);
      Status = Heci->ReadMsg (
                      NON_BLOCKING,
                      (UINT32 *) &QueryKvmResponse,
                      &HeciLength
                      );
    }
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8061);
  *ResponseCode = QueryKvmResponse.ResponseCode;

  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to query the local firmware update interface status.

  @param[out] RuleData            1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetLocalFwUpdate (
  OUT UINT8                       *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_GET_LOCAL_FW_UPDATE         MsgGenGetLocalFwUpdate;
  GEN_GET_LOCAL_FW_UPDATE_ACK     MsgGenGetLocalFwUpdatekuAck;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetLocalFwUpdate.Data.RuleId                  = 7;
  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE);

  ///
  /// Send Get Local FW update Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80A0);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenGetLocalFwUpdate,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenGetLocalFwUpdatekuAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80A2);
    return Status;
  }
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80A1);
  *RuleData = MsgGenGetLocalFwUpdatekuAck.Data.RuleData;

  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to enable or disable the local firmware update interface.
  The firmware allows a single update once it receives the enable command

  @param[in] RuleData             1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciSetLocalFwUpdate (
  IN UINT8                        RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_SET_LOCAL_FW_UPDATE         MsgGenSetLocalFwUpdate;
  GEN_SET_LOCAL_FW_UPDATE_ACK     MsgGenSetLocalFwUpdateAck;
  UINT32                          MeMode;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenSetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_SET_RULE_CMD;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenSetLocalFwUpdate.Data.RuleId                  = 7;
  MsgGenSetLocalFwUpdate.Data.RuleDataLen             = 1;
  MsgGenSetLocalFwUpdate.Data.RuleData                = RuleData;
  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE);

  ///
  /// Send Get Local FW update Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80B0);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenSetLocalFwUpdate,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80B2);
    return Status;
  }

  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenSetLocalFwUpdateAck,
                  &Length
                  );

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80B1);
  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to enable the ME State. The firmware allows a single
  update once it receives the enable command. Once firmware receives this message,
  the firmware will be in normal mode after a global reset.

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME enabled message sent
**/
EFI_STATUS
HeciSetMeEnableMsg (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_SET_FW_CAPSKU_ACK           MsgMeStateControlAck;
  HECI_FWS_REGISTER               MeFirmwareStatus;
  UINTN                           HeciPciAddressBase;
  UINT16                          TimeOut;
  UINT32                          MeMode;

  TimeOut = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (MeMode == ME_MODE_NORMAL) {
    return EFI_SUCCESS;
  }

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (Status) || (MeMode != ME_MODE_TEMP_DISABLED)) {
    return EFI_UNSUPPORTED;
  }

  HeciPciAddressBase = PCI_LIB_ADDRESS (
                        ME_BUS,
                        ME_DEVICE_NUMBER,
                        HECI_FUNCTION_NUMBER,
                        0
                        );
  PciWrite8 (HeciPciAddressBase + R_GEN_STS + 3, 0x20);
  do {
    MeFirmwareStatus.ul = PciRead32 (HeciPciAddressBase + R_FWSTATE);
    gBS->Stall (EFI_ME_STATE_STALL_1_SECOND);
    TimeOut++;
  } while ((MeFirmwareStatus.r.FwInitComplete != ME_FIRMWARE_COMPLETED) && (TimeOut < EFI_ME_STATE_MAX_TIMEOUT));

  Length = sizeof (GEN_SET_FW_CAPSKU_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgMeStateControlAck,
                  &Length
                  );
  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to disable the ME State. The firmware allows a single
  update once it receives the disable command Once firmware receives this message,
  the firmware will work in "Soft Temporary Disable" mode (HFS[19:16] = 3) after a
  global reset. Note, this message is not allowed when AT firmware is enrolled/configured.

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME is disabled
**/
EFI_STATUS
HeciSetMeDisableMsg (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_SET_FW_CAPSKU               MsgMeStateControl;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (MeMode == ME_MODE_TEMP_DISABLED) {
    return EFI_SUCCESS;
  }

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgMeStateControl.MKHIHeader.Data               = 0;
  MsgMeStateControl.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgMeStateControl.MKHIHeader.Fields.Command     = FWCAPS_SET_RULE_CMD;
  MsgMeStateControl.MKHIHeader.Fields.IsResponse  = 0;
  MsgMeStateControl.Data.RuleId.Data              = 6;
  MsgMeStateControl.Data.RuleDataLen              = 1;
  MsgMeStateControl.Data.RuleData                 = 0;

  Length = sizeof (GEN_SET_FW_CAPSKU);

  Status = Heci->SendwACK (
                  (UINT32 *) &MsgMeStateControl,
                  Length,
                  &Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  1.5M or 5M size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_GET_PLATFORM_TYPE           MsgGenGetPlatformType;
  GEN_GET_PLATFORM_TYPE_ACK       MsgGenGetPlatformTypeAck;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetPlatformType.MKHIHeader.Data               = 0;
  MsgGenGetPlatformType.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetPlatformType.MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetPlatformType.MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetPlatformType.Data.RuleId                   = 0x1D;
  Length = sizeof (GEN_GET_PLATFORM_TYPE);

  ///
  /// Send Get Platform Type Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80C0);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenGetPlatformType,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80C2);
    return Status;
  }

  Length = sizeof (GEN_GET_PLATFORM_TYPE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenGetPlatformTypeAck,
                  &Length
                  );
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80C1);
  *RuleData = MsgGenGetPlatformTypeAck.Data.RuleData;

  return Status;
}

/**
  This message is sent by the BIOS on the boot where the host wants to get the firmware provisioning state.
  The firmware will respond to AMT BIOS SYNCH INFO message even after the End of Post.

  @param[out] RuleData            Bit [2:0] Reserved
                                  Bit [4:3] Provisioning State
                                    00 - Pre -provisioning
                                    01 - In -provisioning
                                    02 - Post !Vprovisioning
                                  Bit [31:5] Reserved

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Firmware provisioning state returned
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciAmtBiosSynchInfo (
  OUT UINT32                      *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_AMT_BIOS_SYNCH_INFO         MsgGenAmtBiosSynchInfo;
  GEN_AMT_BIOS_SYNCH_INFO_ACK     MsgGenAmtBiosSynchInfoAck;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenAmtBiosSynchInfo.MKHIHeader.Data              = 0;
  MsgGenAmtBiosSynchInfo.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenAmtBiosSynchInfo.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenAmtBiosSynchInfo.MKHIHeader.Fields.IsResponse = 0;
  MsgGenAmtBiosSynchInfo.Data.RuleId                  = 0x30005;
  Length = sizeof (GEN_AMT_BIOS_SYNCH_INFO);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8130);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenAmtBiosSynchInfo,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8132);
    return Status;
  }

  Length = sizeof (GEN_AMT_BIOS_SYNCH_INFO_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenAmtBiosSynchInfoAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8133);
    return Status;
  }

  *RuleData = MsgGenAmtBiosSynchInfoAck.RuleData;

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8131);
  return Status;
}

/**
  The firmware will respond to GET OEM TAG message even after the End of Post (EOP).

  @param[in] RuleData             Default is zero. Tool can create the OEM specific OEM TAG data.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetOemTagMsg (
  OUT UINT32                      *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_GET_OEM_TAG_MSG             MsgGenGetOemTagMsg;
  GEN_GET_OEM_TAG_MSG_ACK         MsgGenGetOemTagMsgAck;
  UINT32                          MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetOemTagMsg.MKHIHeader.Data              = 0;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetOemTagMsg.Data.RuleId                  = 0x2B;
  Length = sizeof (GEN_GET_OEM_TAG_MSG);

  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenGetOemTagMsg,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_OEM_TAG_MSG_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenGetOemTagMsgAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *RuleData = MsgGenGetOemTagMsgAck.RuleData;

  return Status;
}

/**
  Enables/disables clocks. Used to turn off clocks in unused pci/pcie slots.
  BIOS use this command when it enumerates PCI slots. When PCI slot is found unpopulated, the
  BIOS can disable its clock through this MEI message. It is the BIOS requirement to know which
  slot is controlled by which control bit.

  @param[in] Enables              each bit means corresponding clock should be turned on (1) or off (0)
  @param[in] EnablesMask          each bit means corresponding enable bit is valid (1) or should be ignored (0)
  @param[in] ResponseMode 0       wait for response, 1 - skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_INVALID_PARAMETER   ResponseMode is invalid value
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
HeciSetIccClockEnables (
  IN UINT32                       Enables,
  IN UINT32                       EnablesMask,
  IN UINT8                        ResponseMode
  )
{
  EFI_STATUS                      Status;
  ICC_SET_CLK_ENABLES_BUFFER      Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if ((ResponseMode != ICC_RESPONSE_MODE_SKIP) && (ResponseMode != ICC_RESPONSE_MODE_WAIT)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  CommandSize                         = sizeof (ICC_SET_CLK_ENABLES_MESSAGE);
  ResponseSize                        = sizeof (ICC_SET_CLK_ENABLES_RESPONSE);

  Buffer.message.Header.ApiVersion    = LYNX_POINT_PLATFORM;
  Buffer.message.Header.IccCommand    = SET_CLOCK_ENABLES;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.ClockEnables         = Enables;
  Buffer.message.ClockEnablesMask     = EnablesMask;
  Buffer.message.Params               = ResponseMode;

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D0);
  Status = Heci->SendMsg (
                  (UINT32 *) &Buffer,
                  CommandSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );

  if (ResponseMode == ICC_RESPONSE_MODE_WAIT) {
    Status = Heci->ReadMsg (
                    BLOCKING,
                    (UINT32 *) &Buffer,
                    &ResponseSize
                    );
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) IccSetClockEnables: Message failed! EFI_STATUS = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D2);
    return Status;
  }

  if (ResponseMode == ICC_RESPONSE_MODE_SKIP) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D3);
    return EFI_SUCCESS;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR,
      "(ICC) IccSetClockEnables: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D4);
    return EFI_DEVICE_ERROR;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D1);
  return Status;
}

/**
  Sets or reads Lock mask on ICC registers.
  @param[in] AccessMode           0 - set, 1 - get
  @param[in] ResponseMode         0 - firmware will answer, 1 - firmware will not answer
  @param[in][out] LockRegInfo     bundle count info and mask of registers to become (for 'set' mode) or are
                                  (for 'get' mode) locked. Each bit represents a register. 0=lock, 1=don't lock

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_INVALID_PARAMETER   ResponseMode or pointer of Mask is invalid value
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciLockIccRegisters (
  IN UINT8                        AccessMode,
  IN UINT8                        ResponseMode,
  IN OUT ICC_LOCK_REGS_INFO       *LockRegInfo
  )
{
  EFI_STATUS                      Status;
  ICC_LOCK_REGISTERS_BUFFER       Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if ((ResponseMode != ICC_RESPONSE_MODE_SKIP) && (ResponseMode != ICC_RESPONSE_MODE_WAIT)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  DEBUG ((EFI_D_INFO, "(ICC) LockIccRegisters\n"));
  if (LockRegInfo == NULL) {
    return EFI_INVALID_PARAMETER;

  }

  CommandSize   = sizeof (ICC_LOCK_REGISTERS_MESSAGE) -
                  sizeof(UINT32) * ICC_LOCK_MASK_COUNT +
                  sizeof(UINT32) * LockRegInfo->RegBundles.BundlesCnt;
  ResponseSize  = sizeof (ICC_LOCK_REGISTERS_RESPONSE);

  Buffer.message.Header.ApiVersion      = LYNX_POINT_PLATFORM;
  Buffer.message.Header.IccCommand      = LOCK_ICC_REGISTERS;
  Buffer.message.Header.IccResponse     = 0;
  Buffer.message.Header.BufferLength    = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved        = 0;
  Buffer.message.AccessMode             = AccessMode;
  Buffer.message.Parameters             = ResponseMode;
  Buffer.message.Reserved[0]            = 0;
  Buffer.message.Reserved[1]            = 0;
  Buffer.message.LockRegInfo.RegBundles = LockRegInfo->RegBundles;

//
//  Note : To avoid compoler fail (memcpy)
//
//  for (i = 0; i < LockRegInfo->RegBundles.BundlesCnt; i++) {
//    Buffer.message.LockRegInfo.RegMask[i] = LockRegInfo->RegMask[i];
//  }
  CopyMem (
    Buffer.message.LockRegInfo.RegMask,
    LockRegInfo->RegMask,
    LockRegInfo->RegBundles.BundlesCnt
    );

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80E0);
  Status = Heci->SendMsg (
                  (UINT32 *) &Buffer,
                  CommandSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );

  if (ResponseMode == ICC_RESPONSE_MODE_WAIT) {
    Status = Heci->ReadMsg (
                    BLOCKING,
                    (UINT32 *) &Buffer,
                    &ResponseSize
                    );
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) LockIccRegisters: Message failed! EFI_STATUS = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80E2);
    return Status;
  }

  if (ResponseMode == ICC_RESPONSE_MODE_SKIP) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80E3);
    return EFI_SUCCESS;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR,
      "(ICC) LockIccRegisters: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80E4);
    return EFI_DEVICE_ERROR;
  }

  if (AccessMode == ICC_LOCK_ACCESS_MODE_GET) {
    LockRegInfo->RegBundles = Buffer.response.LockRegInfo.RegBundles;
    LockRegInfo->RegMask[0] = Buffer.response.LockRegInfo.RegMask[0];
    LockRegInfo->RegMask[1] = Buffer.response.LockRegInfo.RegMask[1];
    LockRegInfo->RegMask[2] = Buffer.response.LockRegInfo.RegMask[2];
  }
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80E1);

  return Status;
}

//[-start-120315-IB06460374-modify]//
/**
  retrieves the number of currently used ICC clock profile

  @param[out] Profile             number of current ICC clock profile

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetIccProfile (
  OUT UINT8                       *Profile,
  OUT UINT8                       *ProfileSoftStrap,
  OUT UINT8                       *SupportedProfilesNumber
  )
//[-end-120315-IB06460374-modify]//
{
  EFI_STATUS                      Status;
  ICC_GET_PROFILE_BUFFER          Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  DEBUG ((EFI_D_INFO, "(ICC) GetIccProfile\n"));
  CommandSize                         = sizeof (ICC_GET_PROFILE_MESSAGE);
  ResponseSize                        = sizeof (ICC_GET_PROFILE_RESPONSE);

  Buffer.message.Header.ApiVersion    = LYNX_POINT_PLATFORM;
  Buffer.message.Header.IccCommand    = GET_ICC_PROFILE;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80F0);
  Status = Heci->SendwACK (
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "(ICC) GetIccProfile: Message failed! EFI_STATUS = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80F2);
    return Status;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR,
      "(ICC) GetIccProfile: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    Status = EFI_DEVICE_ERROR;
  } else {
    DEBUG ((EFI_D_INFO, "(ICC) GetIccProfile: Current profile = 0x%x\n", Buffer.response.IccProfileIndex));
  }

  if (Profile != NULL) {
    *Profile = Buffer.response.IccProfileIndex;
//[-start-120315-IB06460374-add]//
    *ProfileSoftStrap = Buffer.response.IccProfileSoftStrap;
    *SupportedProfilesNumber = Buffer.response.SupportedProfilesNumber;    
//[-end-120315-IB06460374-add]//
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80F1);

  return Status;
}

/**
  Sets ICC clock profile to be used on next and following boots

  @param[in] Profile              number of profile to be used

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccProfile (
  IN UINT8                        Profile
  )
{
  EFI_STATUS                      Status;
  ICC_SET_PROFILE_BUFFER          Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  DEBUG ((EFI_D_INFO, "(ICC) SetIccProfile\n"));

  CommandSize                         = sizeof (ICC_SET_PROFILE_MESSAGE);
  ResponseSize                        = sizeof (ICC_SET_PROFILE_RESPONSE);

  Buffer.message.Header.ApiVersion    = LYNX_POINT_PLATFORM;
  Buffer.message.Header.IccCommand    = SET_ICC_PROFILE;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.ProfileBIOS          = Profile;
  Buffer.message.PaddingA             = 0;
  Buffer.message.PaddingB             = 0;

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8100);
  Status = Heci->SendwACK (
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) SetIccProfile: Message failed! EFI_STATUS = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8102);
    return Status;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR,
      "(ICC) SetIccProfile: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8103);
    return EFI_DEVICE_ERROR;
  }
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8101);

  return Status;
}

/**
  Writes 1 dword of data to the icc register offset specified by RegOffset in the ICC Aux space
  @param[in] RegOffset            Register Offset in ICC Aux Space to write
  @param[in] RegData              Dword ICC register data to write
  @param[in] ResponseMode 0       Wait for response, 1 - skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_INVALID_PARAMETER   ResponseMode is invalid value
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
HeciWriteIccRegDword (
  IN UINT16                       RegOffset,
  IN UINT32                       RegData,
  IN UINT8                        ResponseMode
  )
{
  EFI_STATUS                      Status;
  ICC_WRITE_ICC_REG_BUFFER        Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  EFI_HECI_PROTOCOL               *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if ((ResponseMode != ICC_RESPONSE_MODE_SKIP) && (ResponseMode != ICC_RESPONSE_MODE_WAIT)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **)&Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  CommandSize                         = sizeof (ICC_WRITE_ICC_REG_DWORD_MESSAGE);
  ResponseSize                        = sizeof (ICC_WRITE_ICC_REG_DWORD_RESPONSE);

  Buffer.message.Header.ApiVersion    = LYNX_POINT_PLATFORM;
  Buffer.message.Header.IccCommand    = WRITE_ICC_REGISTER;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.Reserved             = 0;
  Buffer.message.Reserved1            = 0;
  Buffer.message.Params               = ResponseMode;
  Buffer.message.RecordDword.RecordFlags                  = WRITE_ICC_RECORD_FLAGS;
  Buffer.message.RecordDword.BundleCount.BundlesCnt       = WRITE_ICC_REG_BUNDLE_COUNT;
  Buffer.message.RecordDword.BundleCount.AU               = 0;
  Buffer.message.RecordDword.BundleCount.Reserved         = 0;
  Buffer.message.RecordDword.AddressMask.AddressMaskData  = RegOffset | ADDRESS_MASK_FIXED_DATA;
  Buffer.message.RecordDword.RegValue                     = RegData;

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D0);
  Status = Heci->SendMsg (
                  (UINT32 *) &Buffer,
                  CommandSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );

  if (ResponseMode == ICC_RESPONSE_MODE_WAIT) {
    Status = Heci->ReadMsg (
                    BLOCKING,
                    (UINT32 *) &Buffer,
                    &ResponseSize
                    );

  }
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) HeciWriteIccRegDword: Message failed! EFI_STATUS = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D2);
    return Status;
  }

  if (ResponseMode == ICC_RESPONSE_MODE_SKIP) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D3);
    return EFI_SUCCESS;
  }

  if (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR,
      "(ICC) HeciWriteIccRegDword: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D4);
    return EFI_DEVICE_ERROR;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x80D1);
  return Status;
}

/**
  This message is used to turn on the Intel ME firmware MDES
  capability, Intel SVT for PCH capability or both when the
  system is in a post-manufactured state. Once firmware receives
  this message, the firmware will enable selected platform debug
  capabilities . The firmware will automatically disable all
  platform debug capabilities if this message is not received
  before receiving End Of Post.

  @param[in]  Data                capabilities to be enabled
  @param[out] Result              0x00   : Enable Success
                                  Others : Enable Failure

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciPlatformDebugCapabilityMsg (
  IN PLATFORM_DEBUG_CAP           Data,
  OUT UINT8                       *Result
  )
{
  EFI_STATUS                          Status;
  UINT32                              Length;
  EFI_HECI_PROTOCOL                   *Heci;
  GEN_PLATFORM_DEBUG_CAP_MKHI_CMD_MSG PlatformDebug;
  UINT32                              MeMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  PlatformDebug.MKHIHeader.Data              = 0;
  PlatformDebug.MKHIHeader.Fields.GroupId    = MKHI_MDES_GROUP_ID;
  PlatformDebug.MKHIHeader.Fields.Command    = MDES_ENABLE_MKHI_CMD;
  PlatformDebug.MKHIHeader.Fields.IsResponse = 0;
  PlatformDebug.Capability                   = Data;
  Length = sizeof (GEN_PLATFORM_DEBUG_CAP_MKHI_CMD_MSG);

  Status = Heci->SendwACK (
                  (UINT32 *) &PlatformDebug,
                  Length,
                  &Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (Status == EFI_SUCCESS) {
    *Result = (UINT8) PlatformDebug.MKHIHeader.Fields.Result;
  }

  return Status;
}

/**
  It creates and sends Heci messages.

  Remark:
  Functionality is available only in release mode.
  Using MDES in debug mode causes recursive calling of this function
  because debug messages are sending from Heci->SendMsg function.

  @param[in] CodeType             Indicates the type of status code being reported.
  @param[in] Value                Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Opcode evaluation success.
  @retval Other                   Opcode evaluation failed.
**/
EFI_STATUS
HeciSendMdesStatusCode (
  IN EFI_STATUS_CODE_TYPE         CodeType,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN EFI_GUID                     * CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA         * Data OPTIONAL
  )
{
  CBM_BIOS_MDES_MSG_REQ           MsgData;
  EFI_STATUS                      Status;
  UINT32                          HeciLength;
  UINT32                          MeMode;
  EFI_HECI_PROTOCOL               *Heci;
  static UINT32                   sNr = 0;
  UINT32                          ExtendedDataSize;
  UINT32                          StringCnt;

  ExtendedDataSize  = 0;
  StringCnt         = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgData.MKHIHeader.Fields.GroupId         = MKHI_MDES_GROUP_ID;
  MsgData.MKHIHeader.Fields.Command         = MDES_BIOS_MSG_LOG_REQ_CMD;
  MsgData.MKHIHeader.Fields.IsResponse      = 0;
  MsgData.MKHIHeader.Fields.Result          = 0;

  MsgData.Data.MdesAttr.Severity            = BiosToMdesSeverity (CodeType);
  MsgData.Data.MdesAttr.PayLoadType         = EFI_STATUS_CODE;
  MsgData.Data.BiosAttr.CallerIdData        = (CallerId == 0) ? 0 : 1;
  MsgData.Data.BiosAttr.ExtendedDataHeader  = (Data == 0) ? 0 : 1;
  MsgData.Data.Serial                       = sNr++;
  MsgData.Data.StatusType                   = CodeType;
  MsgData.Data.StatusCode                   = Value;
  MsgData.Data.Instance                     = Instance;

  if (CallerId != 0) {
    CopyMem (&MsgData.Data.CallerId, CallerId, sizeof(EFI_GUID));
  }

  if (Data != 0) {
    ExtendedDataSize = Data->Size;
    if (ExtendedDataSize > SIZE_OF_MDES_EXTENDED_DATA) {
      ASSERT (FALSE);
      //
      // extended data too long
      //
      ExtendedDataSize = SIZE_OF_MDES_EXTENDED_DATA;
    }

    if (CompareGuid (&gEfiStatusCodeDataTypeStringGuid, &Data->Type)) {

      EFI_STATUS_CODE_STRING_DATA *str_data;

      str_data                      = (EFI_STATUS_CODE_STRING_DATA *) Data;
      MsgData.Data.ExtendedData[0]  = (UINT8) (str_data->StringType);
      MsgData.Data.ExtendedData[1]  = 0;
      MsgData.Data.ExtendedData[2]  = 0;
      MsgData.Data.ExtendedData[3]  = 0;

      if (str_data->StringType == EfiStringAscii) {
        AsciiStrnCpy ((CHAR8 *) &MsgData.Data.ExtendedData[4], str_data->String.Ascii, SIZE_OF_MDES_EXTENDED_DATA);
        StringCnt         = (UINT32) AsciiStrLen ((CHAR8 *) &MsgData.Data.ExtendedData[4]);
        ExtendedDataSize  = 4 + StringCnt;
        if (ExtendedDataSize > SIZE_OF_MDES_EXTENDED_DATA) {
          ExtendedDataSize = SIZE_OF_MDES_EXTENDED_DATA;
        }

      } else if (str_data->StringType == EfiStringUnicode) {
        StrnCpy ((CHAR16 *) &MsgData.Data.ExtendedData[4], str_data->String.Unicode, SIZE_OF_MDES_EXTENDED_DATA);
        StringCnt         = (UINT32) StrLen ((CHAR16 *) &MsgData.Data.ExtendedData[4]);
        ExtendedDataSize  = 4 + StringCnt;
        if (ExtendedDataSize > SIZE_OF_MDES_EXTENDED_DATA) {
          ExtendedDataSize = SIZE_OF_MDES_EXTENDED_DATA;
        }

      } else if (str_data->StringType == EfiStringToken) {
        CopyMem (MsgData.Data.ExtendedData, Data + Data->HeaderSize, ExtendedDataSize);
      }
    } else {
      CopyMem (MsgData.Data.ExtendedData, Data + Data->HeaderSize, ExtendedDataSize);
    }

    MsgData.Data.ExtendedDataHeader.Size        = (UINT16) ExtendedDataSize;
    MsgData.Data.ExtendedDataHeader.HeaderSize  = sizeof (MDES_EXTENDED_DATA_HEADER);
    CopyMem (&MsgData.Data.ExtendedDataHeader.Type, &Data->Type, sizeof(EFI_GUID));
  }

  if (CallerId != 0) {
    HeciLength = sizeof (CBM_BIOS_MDES_MSG_REQ) - SIZE_OF_MDES_EXTENDED_DATA - sizeof (MDES_EXTENDED_DATA_HEADER);
  } else {
    HeciLength = sizeof (CBM_BIOS_MDES_MSG_REQ) -
      SIZE_OF_MDES_EXTENDED_DATA -
      sizeof (MDES_EXTENDED_DATA_HEADER) -
      sizeof (EFI_GUID);
  }

  if (Data != 0) {
    HeciLength = sizeof (CBM_BIOS_MDES_MSG_REQ) - SIZE_OF_MDES_EXTENDED_DATA + ExtendedDataSize;
  }

  Status = Heci->SendMsg (
                  (UINT32 *) &MsgData,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  return Status;
}

/**
  Provides an interface to call function to send HECI message.

  @param[in] Flags                Indicates the status of the BIOS MDES.
  @param[in] BiosEventFilters     Indicates the status of the BIOS event filter group.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI sent with success.
**/
EFI_STATUS
HeciGetMdesConfig (
  OUT MDES_BIOS_FLAGS             *Flags,
  OUT UINT32                      *BiosEventFilters
  )
{
  EFI_STATUS                            Status;
  EFI_HECI_PROTOCOL                     *Heci;
  UINT32                                MeMode;
  UINT32                                Length;
  MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK MsgMdesAvailabilityAck;
  MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ MsgMdesAvailabilityReq;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgMdesAvailabilityReq.MKHIHeader.Fields.GroupId    = MKHI_MDES_GROUP_ID;
  MsgMdesAvailabilityReq.MKHIHeader.Fields.Command    = MDES_BIOS_MSG_GET_CONFIG_CMD;
  MsgMdesAvailabilityReq.MKHIHeader.Fields.IsResponse = 0;
  MsgMdesAvailabilityReq.MKHIHeader.Fields.Result     = 0;

  Length = sizeof (MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgMdesAvailabilityReq,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgMdesAvailabilityAck,
                  &Length
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Flags            = MsgMdesAvailabilityAck.Data.Flags;
  *BiosEventFilters = MsgMdesAvailabilityAck.Data.BiosEventFilters;

  return Status;
}


/**
  Sends the MKHI Enable/Disable manageability message.
  The message will only work if bit 2 in the bitmasks is toggled.
  To enable manageability:
    EnableState = 0x00000004, and
    DisableState = 0x00000000.
  To disable manageability:
    EnableState = 0x00000000, and
    DisableState = 0x00000004

  @param[in] EnableState          Enable Bit Mask
  @param[in] DisableState         Disable Bit Mask

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciFwFeatureStateOverride (
  IN UINT32                       EnableState,
  IN UINT32                       DisableState
  )
{
  EFI_STATUS                        Status;
  UINT32                            HeciLength;
  UINT32                            MeMode;
  FIRMWARE_CAPABILITY_OVERRIDE      MngStateCmd;
  FIRMWARE_CAPABILITY_OVERRIDE_ACK  MngStateAck;
  EFI_HECI_PROTOCOL                 *Heci;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MngStateCmd.MKHIHeader.Data               = 0;
  MngStateCmd.MKHIHeader.Fields.Command     = FIRMWARE_CAPABILITY_OVERRIDE_CMD;
  MngStateCmd.MKHIHeader.Fields.IsResponse  = 0;
  MngStateCmd.MKHIHeader.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MngStateCmd.MKHIHeader.Fields.Reserved    = 0;
  MngStateCmd.MKHIHeader.Fields.Result      = 0;
  MngStateCmd.FeatureState.EnableFeature    = EnableState;
  MngStateCmd.FeatureState.DisableFeature   = DisableState;
  HeciLength = sizeof (FIRMWARE_CAPABILITY_OVERRIDE);

  Status = Heci->SendMsg (
                  (UINT32 *) &MngStateCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  HeciLength = sizeof (FIRMWARE_CAPABILITY_OVERRIDE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MngStateAck,
                  &HeciLength
                  );

  return Status;
}

/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData            MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  GEN_GET_FW_FEATURE_STATUS       GetFwFeatureStatus;
  GEN_GET_FW_FEATURE_STATUS_ACK   GetFwFeatureStatusAck;
  UINT32                          MeMode;
  EFI_HECI_PROTOCOL               *Heci;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetFwFeatureStatus.MKHIHeader.Data              = 0;
  GetFwFeatureStatus.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  GetFwFeatureStatus.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  GetFwFeatureStatus.MKHIHeader.Fields.IsResponse = 0;
  GetFwFeatureStatus.Data.RuleId                  = 0x20;

  Length = sizeof (GEN_GET_FW_FEATURE_STATUS);
  Status = Heci->SendMsg (
                  (UINT32 *) &GetFwFeatureStatus,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_FEATURE_STATUS_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &GetFwFeatureStatusAck,
                  &Length
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  RuleData->Data = GetFwFeatureStatusAck.RuleData.Data;

  return Status;
}

/**
  This message is sent by the BIOS when it wants to query
  the independent firmware recovery (IFR).

  @param[in] RuleData             1 - local firmware update interface enable

                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetIfrUpdate (
  OUT UINT8                       *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_GET_LOCAL_FW_UPDATE         MsgGenGetLocalFwUpdate;
  GEN_GET_LOCAL_FW_UPDATE_ACK     MsgGenGetLocalFwUpdatekuAck;
  UINT32                          MeMode;

  Status = EFI_SUCCESS;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetLocalFwUpdate.Data.RuleId                  = MEFWCAPS_ME_FWU_IFR_RULE;
  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE);

  ///
  /// Send Get Local FW update Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8110);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenGetLocalFwUpdate,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8112);
    return Status;
  }

  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenGetLocalFwUpdatekuAck,
                  &Length
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8113);
    return Status;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8111);
  *RuleData = MsgGenGetLocalFwUpdatekuAck.Data.RuleData;

  return Status;
}

/**
  This message is sent by the BIOS when it wants to set
  the independent firmware recovery (IFR) state.

  @param[in] RuleData             1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciSetIfrUpdate (
  IN UINT8                        RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  GEN_SET_LOCAL_FW_UPDATE         MsgGenSetLocalFwUpdate;
  GEN_SET_LOCAL_FW_UPDATE_ACK     MsgGenSetLocalFwUpdateAck;
  UINT32                          MeMode;

  Status = EFI_SUCCESS;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenSetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_SET_RULE_CMD;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenSetLocalFwUpdate.Data.RuleId                  = MEFWCAPS_ME_FWU_IFR_RULE;
  MsgGenSetLocalFwUpdate.Data.RuleDataLen             = 1;
  MsgGenSetLocalFwUpdate.Data.RuleData                = RuleData;
  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE);

  ///
  /// Send Set Local FW update Request to ME
  ///
  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8120);
  Status = Heci->SendMsg (
                  (UINT32 *) &MsgGenSetLocalFwUpdate,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8122);
    return Status;
  }

  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgGenSetLocalFwUpdateAck,
                  &Length
                  );

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8121);
  return Status;
}

/**
  This message is sent by the BIOS if EOP-ACK not received to force ME to disable
  HECI interfaces.

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciDisableHeciBusMsg (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  EFI_HECI_PROTOCOL               *Heci;
  HECI_BUS_DISABLE_CMD_ACK        MsgHeciBusDisable;

  ZeroMem(&MsgHeciBusDisable, sizeof(HECI_BUS_DISABLE_CMD_ACK));

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  MsgHeciBusDisable.Command.Data = HECI_BUS_DISABLE_OPCODE;
  Length = sizeof (HECI_BUS_DISABLE_CMD_ACK);

  Status = Heci->SendMsg (
                  (UINT32 *) &MsgHeciBusDisable,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  ME_HECI_FIXED_ADDRESS
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &MsgHeciBusDisable,
                  &Length
                  );
  if ((MsgHeciBusDisable.Command.Data != HECI_BUS_DISABLE_ACK_OPCODE)
    || (MsgHeciBusDisable.Status != 0)) {
    Status = EFI_ABORTED;
  }

  return Status;
}

/**
  This message is sent by the BIOS to inform ME FW whether or not to take the 
  TPM 1.2 Deactivate flow

  @param[in] UINT8 TpmDeactivate  0 - ME FW should not take the
                                      deactivate flow.
                                  1 - ME FW should take the deactivate
                                      flow.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciSendTpmData (
  IN UINT8               TpmDeactivate
  )
{
  EFI_STATUS             Status;
  UINT32                 HeciSendLength;
  UINT32                 HeciRecvLength;
  EFI_HECI_PROTOCOL      *Heci;
  BIOSNV_SET_ACM_TPM     SetAcmTpmMsg;
  BIOSNV_SET_ACM_TPM_ACK SetAcmTpmMsgAck;

  DEBUG ((EFI_D_ERROR, "HeciSendTpmData Message.  TpmDeactivate Setup Data = %d\n", TpmDeactivate));

  ZeroMem(&SetAcmTpmMsg, sizeof(BIOSNV_SET_ACM_TPM));
  ZeroMem(&SetAcmTpmMsgAck, sizeof(BIOSNV_SET_ACM_TPM_ACK));

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (IsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  SetAcmTpmMsg.MKHIHeader.Data                          = ACM_TPM_DATA_MKHI_DATA;
  SetAcmTpmMsg.AcmTpmData.RuleId.Fields.RuleTypeId      = ACM_TPM_DATA_RULE_TYPE_ID;
  SetAcmTpmMsg.AcmTpmData.RuleDataLen                   = ACM_TPM_DATA_RULE_DATA_LENGTH;
  SetAcmTpmMsg.AcmTpmData.TpmState.Fields.TpmDeactivate = FALSE;

  if (TpmDeactivate == 1) {
    SetAcmTpmMsg.AcmTpmData.TpmState.Fields.TpmDeactivate = TRUE;
  }

  //
  // Send Set ACM TPM Data MKHI message
  //
  HeciSendLength = sizeof (BIOSNV_SET_ACM_TPM);
  Status = Heci->SendMsg (
                  (UINT32 *) &SetAcmTpmMsg,
                  HeciSendLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Get Set ACM TPM Data MKHI ACK message
  //
  HeciRecvLength = sizeof (BIOSNV_SET_ACM_TPM_ACK);
  Status = Heci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &SetAcmTpmMsgAck,
                  &HeciRecvLength
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
