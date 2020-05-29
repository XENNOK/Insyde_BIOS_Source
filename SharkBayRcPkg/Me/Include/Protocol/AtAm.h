/** @file
  Header file for ATAM

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2013 Intel Corporation. All rights reserved
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
#ifndef _DXE_ATAM_PROTOCOL_H_
#define _DXE_ATAM_PROTOCOL_H_

#include <MkhiMsgs.h>

///
/// Define the  protocol GUID
///
#define EFI_ATAM_PROTOCOL_GUID \
  { 0x8AEBAA5A, 0x556F, 0x404B, { 0x96, 0x05, 0x38, 0xE0, 0xDE, 0x29, 0x76, 0x2F }}

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiAtProtocolGuid;

#define ATAM_PROTOCOL_REVISION  3

//
// Forward reference for ANSI C compatibility
//

#define ISV_PLATFORM_ID_LENGTH  16
#define SERVER_SHORTCODE_LENGTH 16
#define DEFAULT_LANGUAGE_STRING 4

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_ATAM_PROTOCOL  EFI_ATAM_PROTOCOL;

#pragma pack(1)
typedef struct {

  UINT8   BiosLanguageSupport[DEFAULT_LANGUAGE_STRING];
  UINT8   Reserved[2];

  ///
  /// Defines the number which will be used by BIOS before making BIOS AM as a
  /// default recovery mechanism
  ///
  UINT16  PbaOverRideThreshold;

  ///
  /// The value used by BIOS to try sending GPS location when AT stolen
  ///
  UINT8   BiosLocationBeconing;

  ///
  /// This will be used for Notebook transfer authorization request
  ///
  UINT8   PlatformAuthReq;

  ///
  /// This will be used for Notebook transfer authorization acknowledgement
  ///
  UINT8   PlatformAuthAck;

  ///
  /// This will be used for AT Server short code
  ///
  UINT8   ServerShortCode[SERVER_SHORTCODE_LENGTH];

  ///
  /// This allow ISV to set unique platform ID and will be used for displayed on need basis
  ///
  UINT16  IsvPlatformId[ISV_PLATFORM_ID_LENGTH];

} AT_BIOS_RECOVERY_CONFIG;
#pragma pack()

///
/// Protocol definitions
///

/**
  This function gets the ISV Strings stored by AT Server that BIOS will display.

  @param[in] This        The address of protocol
  @param[out] IsvString  Isv string pointer
  @param[out] IsvId      Intel(R) Anti-Theft service provider Id

  @retval EFI_SUCCESS   The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_ISV_ID) (
  IN  EFI_ATAM_PROTOCOL *This,
  OUT UINT8             *IsvString,
  OUT UINT32            *IsvId
  );

/**
  This function gets recovery config.

  @param[in]  This                The address of protocol
  @param[out] RecoveryConfig      Pointer to structure

  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_RECOVERY_CONFIG) (
  IN  EFI_ATAM_PROTOCOL           *This,
  OUT AT_BIOS_RECOVERY_CONFIG     *RecoveryConfig
  );

/**
  This function returns time left to enter password.

  @param[in] This                 The address of protocol
  @param[out] TimeLeft            Time
  @param[out] TimeInterval        Time interval

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_TIMER) (
  IN  EFI_ATAM_PROTOCOL           *This,
  OUT UINT32                      *TimeLeft,
  OUT UINT32                      *TimeInterval
  );

/**
  This function gets 16 bytes nonce from firmware and also converts it to string according to format "XXXX-XXXX-XXXX-XXXX-XXXX-XXXX-XX".

  @param[in] This                 The address of protocol
  @param[out] NonceStr            Pointer to Nonce string

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_NONCE) (
  IN  EFI_ATAM_PROTOCOL           *This,
  OUT UINT8                       *NonceStr
  );

/**
  This routine receives the data (passphrase or SRTK) from UI and verifies it if the password (either passphrase or SRTK) is acceptable.

  @param[in] This                 The address of protocol
  @param[in] PasswordEntered      Pointer to string
  @param[in] PassType             Password type
  @param[out] IsAuthenticated     Pointer to result of verification

  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_VERIFY_PASSWORD) (
  IN  EFI_ATAM_PROTOCOL           *This,
  IN  UINT8                       *PasswordEntered,
  IN  UINT32                      PassType,
  OUT UINT8                       *IsAuthenticated
  );

/**
  This routine receives the SRTK from UI and verifies it if the password is acceptable. This requests FW to enter or exit Suspend mode based on user input.

  @param[in] This                 The address of protocol
  @param[in] TransitionState      1- enter suspend state, 0 - exit suspend state
  @param[in] Token                Pointer to token

  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_SET_SUSPEND_STATE) (
  IN EFI_ATAM_PROTOCOL            *This,
  IN UINT32                       TransitionState,
  IN UINT8                        *Token
  );

/**
  This routine gets AT state.

  @param[in]  This                The address of protocol
  @param[out] AtStateInfo         State of AT

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_AT_STATE_INFO) (
  IN  EFI_ATAM_PROTOCOL           *This,
  OUT AT_STATE_INFO               *AtStateInfo
  );

/**
  This routine gets PBA failed count value.

  @param[out] PbaFailedExceeded   TRUE when the PbaOverrideThreshold is exceeded
  @param[out] PbaFailedAttempts   Number of failed attempts
  @param[out] PbaFailedThreshold  Pba failed count treshold

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ATAM_GET_PBA_COUNTER) (
  OUT UINT8*                      PbaFailedExceeded,
  OUT UINT16*                     PbaFailedAttempts,
  OUT UINT16*                     PbaFailedThreshold
  );

///
/// Protocol definition
///
struct _EFI_ATAM_PROTOCOL {
  UINT8                           Revision;
  EFI_ATAM_GET_ISV_ID             AtAmGetIsvId;
  EFI_ATAM_GET_RECOVERY_CONFIG    AtAmGetRecoveryConfig;
  EFI_ATAM_GET_TIMER              AtAmGetTimer;
  EFI_ATAM_GET_NONCE              AtAmGetNonce;
  EFI_ATAM_VERIFY_PASSWORD        AtAmVerifyPassword;
  EFI_ATAM_SET_SUSPEND_STATE      AtAmSetSuspendState;
  EFI_ATAM_GET_AT_STATE_INFO      AtAmGetAtStateInfo;
  EFI_ATAM_GET_PBA_COUNTER        AtAmGetPbaCounter;
};

#endif
