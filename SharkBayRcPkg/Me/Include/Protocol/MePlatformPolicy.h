/** @file
  Interface definition details between ME and platform drivers during DXE phase.

@copyright
  Copyright (c) 2006 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _ME_PLATFORM_POLICY_H_
#define _ME_PLATFORM_POLICY_H_

///
/// ME policy provided by platform for DXE phase
/// This protocol provides an interface to get Intel ME Configuration information
///
#define DXE_PLATFORM_ME_POLICY_GUID \
  { \
    0x69bf9e8a, 0x4ad6, 0x9a28, 0x87, 0xf3, 0x09, 0xa0, 0x71, 0x29, 0x2a, 0x00 \
  }

///
/// Initial Revision
///
#define DXE_PLATFORM_ME_POLICY_PROTOCOL_REVISION_1  1
///
/// Remove MbpSecurity
///
#define DXE_PLATFORM_ME_POLICY_PROTOCOL_REVISION_2  2
extern EFI_GUID gDxePlatformMePolicyGuid;

#pragma pack(1)

typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8     MeFwDownGrade : 1;        ///< 0: Disabled; 1: Enabled
  UINT8     MeLocalFwUpdEnabled : 1;  ///< 0: Disabled; 1: Enabled
  UINT8     Byte0ReservedBit2 : 1;
  UINT8     EndOfPostEnabled : 1;     ///< 0: Disabled; 1: Enabled
  UINT8     EndOfPostDone : 1;        ///< 0: Not yet;  1: Done
  UINT8     MdesCapability : 1;       ///< 0: Disabled; 1: Enabled
  UINT8     SvtForPchCap: 1;          ///< 0: Disabled; 1: Enabled
  UINT8     MdesForBiosState : 1;     ///< 0: Disabled; 1: Enabled
  UINT8     ByteReserved[15];         ///< Byte 1-15 Reserved for other bit definitions in future
} ME_CONFIG;

///
/// ME ERROR Message ID
///
typedef enum {
  MSG_EOP_ERROR             = 0,
  MSG_ME_FW_UPDATE_FAILED,
  MSG_ASF_BOOT_DISK_MISSING,
  MSG_KVM_TIMES_UP,
  MSG_KVM_REJECTED,
  MSG_HMRFPO_LOCK_FAILURE,
  MSG_HMRFPO_UNLOCK_FAILURE,
  MSG_ME_FW_UPDATE_WAIT,
  MSG_ILLEGAL_CPU_PLUGGED_IN,
  MSG_KVM_WAIT,
  MSG_PLAT_DISABLE_WAIT,
  MAX_ERROR_ENUM
} ME_ERROR_MSG_ID;

/**
  Show Me Error message. This is to display localized message in
  the console. This is used to display message strings in local
  language. To display the message, the routine will check the
  message ID and ConOut the message strings. For example, the
  End of Post error displayed in English will be:
  gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of
  Post message to ME\n"); It is recommended to clear the screen
  before displaying the error message and keep the message on
  the screen for several seconds.
  A sample is provided, see ShowMeReportError () to retrieve
  details.

  @param[in] MsgId                Me error message ID for displaying on screen message

  @retval None
**/
typedef
VOID
(EFIAPI *ME_REPORT_ERROR) (
  IN ME_ERROR_MSG_ID              MsgId
  );

typedef struct {
  ///
  /// This member determines the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS
  /// Type 14 - Group Associations structure - item type. FVI structure uses it as
  /// SMBIOS OEM type to provide MEBx, ME FW and reference code version information
  ///
  UINT8 FviSmbiosType;
} ME_MISC_CONFIG;

#pragma pack()
///
/// ME DXE Platform Policy
/// This protocol provides information of the current Intel ME feature selection. Information is
/// passed from the platform code to the Intel ME Reference code using this structure. There are
/// 2 types of information, BIOS setup option and ME status information.
///
typedef struct _DXE_ME_POLICY_PROTOCOL {
  ///
  /// Revision for the protocol structure
  ///
  UINT8                           Revision;
  ///
  /// Intel ME feature selection enable/disable and firmware configuration information
  ///
  ME_CONFIG                       MeConfig;
  ///
  /// Support Localization for displaying on screen message
  ///
  ME_REPORT_ERROR                 MeReportError;
  ///
  /// Miscellaneous items
  ///
  ME_MISC_CONFIG                  MeMiscConfig;
} DXE_ME_POLICY_PROTOCOL;

#endif
