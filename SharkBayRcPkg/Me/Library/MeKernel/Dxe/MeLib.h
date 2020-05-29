/** @file

  Header file for functions to get Intel ME information

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

#ifndef _ME_LIB_H_
#define _ME_LIB_H_
//[-start-120808-IB10820098-modify]//
#include <MePolicyLib.h>
#include <HeciMsgLib.h>
#include <MeChipset.h>
#include <HeciRegs.h>

#include <Protocol/MeBiosPayloadData.h>
//[-end-120808-IB10820098-modify]//
#include <Guid/MeBiosExtensionSetup.h>
#define MBP_PRESENT_MASK  0x00000020
#define FIVE_MS_TIMEOUT   5000

extern EFI_GUID gExitPmAuthProtocolGuid;
extern CHAR16  gEfiMeBiosExtensionSetupName[];

#pragma pack(push, 1)

typedef union _MBP_HEADER {
  UINT32  Data;
  struct {
    UINT32  MbpSize : 8;    ///< Byte 0 - MBP Size in DW including Header
    UINT32  NumEntries : 8; ///< Byte 1 - Number of Entries (Data Items) in MBP
    UINT32  Rsvd : 16;      ///< Byte 3:2 - Reserved
  } Fields;
} MBP_HEADER;

typedef union _MBP_ITEM_HEADER {
  UINT32  Data;
  struct {
    UINT32  AppId : 8;      ///< Byte 0 - Application ID
    UINT32  ItemId : 8;     ///< Byte 1 - Item ID
    UINT32  Length : 8;     ///< Byte 2 - Length in DW
    UINT32  Rsvd : 8;       ///< Byte 3 - Reserved
  } Fields;
} MBP_ITEM_HEADER;

#pragma pack(pop)

/**
  Check if Me is enabled

  @param[in] None.

  @retval None
**/
EFI_STATUS
MeLibInit (
  VOID
  );

/**
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku            Return Data from Get Firmware Capabilities MKHI Request

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwCapsSku (
  MEFWCAPS_SKU                    *FwCapsSku
  );

/**
  This message is sent by the BIOS or Intel(R) MEBX prior to the End of Post (EOP) on the boot
  where host wants to get Lynx Point platform type. One of usages is to utilize this command to
  determine if the platform runs in 1.5M or 5M size firmware.

  @param[out] RuleData            PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetPlatformType (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  );

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
  );

/**
  This routine returns ME-BIOS Payload information from HECI buffer

  @param[out] MbpPtr              ME-BIOS Payload information.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        Failed to consume MBP
**/
EFI_STATUS
PrepareMeBiosPayload (
  OUT ME_BIOS_PAYLOAD             *MbpPtr
  );

/**
  Send Get Firmware Version Request to ME

  @param[in] MsgGenGetFwVersionAckData  Return themessage of FW version

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwVersion (
  IN OUT GEN_GET_FW_VER_ACK_DATA  *MsgGenGetFwVersionAckData
  );

/**
  Host client gets Firmware update info from ME client for SMBIOS Table 131

  @param[in][out] MECapability    Structure of FirmwareUpdateInfo

  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
HeciGetMeFwInfo (
  IN OUT ME_CAP                   *MECapability
  );


/**
  This is a dummy event to be hooked to provide ME or Platform
  code before EOP Heci message is sent

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context
**/
VOID
EFIAPI
MeEmptyEvent (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  );

/**
  Get AT State Information From ME FW

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
  );

/**
  Dump DXE_MBP_DATA_PROTOCOL

  @param[in] MbpData              Pointer to DXE_MBP_DATA_PROTOCOL

  @retval None
**/
VOID
DxeMbpDebugDump (
  IN DXE_MBP_DATA_PROTOCOL        *MbpPtr
  );

#endif
