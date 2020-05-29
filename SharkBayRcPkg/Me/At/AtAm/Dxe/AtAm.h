/** @file
  Header file for hangling AT and provide interface for creating UI.

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
  Copyright (c) 2012 - 2013 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef _ATAM_H_
#define _ATAM_H_

#include "At.h"
#include "AtHi.h"
#include <MeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/AtLibrary/Dxe/AtAmHelper.h>
#include <Library/AtLibrary/Dxe/AtPolicyLib.h>
///
/// Used during initialization
///
#include <Protocol/Heci.h>
#include <Protocol/At.h>
#include <Protocol/AtPlatformPolicy.h>
//[-start-120829-IB04320199-add]//
#include <Protocol/SetupUtility.h>
#include <ChipsetSetupConfig.h>
//[-end-120829-IB04320199-add]//
///
/// Driver Produced Protocols
///
#include <Protocol/AtAm.h>

#define ATAM_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('A', 'T', 'A', 'M')

#define MAX_HEX_BYTES             20
#define ATAM_ENTER_SUSPEND_STATE  1
#define ATAM_EXIT_SUSPEND_STATE   0

#define PBA_FAILED_COUNT_VARIABLE_GUID \
  { 0x7C66FFDC, 0x423C, 0xE5D4, { 0x25, 0x1B, 0x55, 0xAD, 0xBA, 0x95, 0x26, 0x98 }}

#pragma pack(1)

typedef struct {
  UINTN             Signature;
  EFI_HANDLE        Handle;
  EFI_ATAM_PROTOCOL AtAmProtocol;
} ATAM_INSTANCE;

#define ATAM_INSTANCE_FROM_ATAM_PROTOCOL(a) CR (a, ATAM_INSTANCE, AtAm, ATAM_PRIVATE_DATA_SIGNATURE)

#pragma pack()

#define NONCE_LENGTH                16
#define STR_NONCE_LENGTH            33
#define ATAM_SETUP_PASSWORD_LENGTH  49
#define ATAM_TIMER_STRING_LENGTH    10
#define ISV_PLATFORM_ID_LENGTH      16
#define SERVER_SHORTCODE_LENGTH     16
#define DEFAULT_LANGUAGE_STRING     4
#define RECOVERY_STRING_LENGTH      256
#define MX_SMS_MESSAGES             99

typedef enum _AT_AM_SELECTION
{
  AT_AM_SELECTION_ATAM = 0,
  AT_AM_SELECTION_PBAM,
  AT_AM_SELECTION_MAX
} AT_AM_SELECTION;

/**
  This function gets the ISV Strings stored by AT Server that BIOS will display.

  @param[in] This        The address of protocol
  @param[out] IsvString  Isv string pointer
  @param[out] IsvId      Intel(R) Anti-Theft service provider Id

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
AtAmGetIsvId (
  IN  EFI_AT_PROTOCOL             *This,
  OUT UINT8                       *IsvString,
  OUT UINT32                      *IsvId
  );

/**
  This function returns time left to enter password.

  @param[in] This                 The address of protocol
  @param[out] TimeLeft            Time
  @param[out] TimeInterval        Time interval

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
EFI_STATUS
AtAmGetTimer (
  IN  EFI_AT_PROTOCOL             *This,
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
EFI_STATUS
AtAmGetNonce (
  IN  EFI_AT_PROTOCOL             *This,
  OUT UINT8                       *NonceStr
  );

/**
  This function gets recovery config.

  @param[in]  This                The address of protocol
  @param[out] RecoveryConfig      Pointer to structure

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
AtAmGetRecoveryConfig (
  IN  EFI_AT_PROTOCOL             *This,
  OUT AT_BIOS_RECOVERY_CONFIG     *RecoveryConfig
  );


/**
  This routine receives the data (passphrase or SRTK) from UI and verifies it if the password (either passphrase or SRTK) is acceptable.

  @param[in] This                 The address of protocol
  @param[in] PasswordEntered      Pointer to string
  @param[in] PassType             Password type
  @param[out] IsAuthenticated     Pointer to result of verification

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
AtAmVerifyPassword (
  IN  EFI_AT_PROTOCOL             *This,
  IN  UINT8                       *PasswordEntered,
  IN  UINT32                      PassType,
  OUT UINT8                       *IsAuthenticated
  );

/**
  This routine receives the SSTK from UI and verifies it if the password is acceptable. This requests FW to enter or exit Suspend mode based on user input.

  @param[in] This                 The address of protocol
  @param[in] TransitionState      1- enter suspend state, 0 - exit suspend state
  @param[in] Token                Pointer to token

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
AtAmSetSuspendState (
  IN EFI_AT_PROTOCOL              *This,
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
EFI_STATUS
AtAmGetAtStateInfo (
  IN  EFI_AT_PROTOCOL             *This,
  OUT AT_STATE_INFO               *AtStateInfo
  );

/**
  Validate preffered AM.

  @param[in] AtState              State of AT
  @param[in] AtAmPref             Preferred AT authentication

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
EFI_STATUS
AtAmValidatePreferredAM (
  IN UINT8                        *AtState,
  IN UINT16                       *AtAmPref
  );

/**
  This routine checks if the PbaOverrideThreshold is exceeded.

  @param[out] PbaFailedExceeded   TRUE when the PbaOverrideThreshold is exceeded
  @param[out] PbaFailedAttempts   Number of failed attempts
  @param[out] PbaFailedThreshold  Pba failed count treshold

  @retval EFI_SUCCESS             The function completed
          successfully.
**/
EFI_STATUS
AtAmGetPbaCounter (
  OUT UINT8*          PbaFailedExceeded,
  OUT UINT16*         PbaFailedAttempts,
  OUT UINT16*         PbaFailedThreshold
  );

#endif // _ATAM_H_
