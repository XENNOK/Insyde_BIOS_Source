/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Wdt.h

@brief
  Watchdog Timer protocol

**/
#ifndef _DXE_WDT_H_
#define _DXE_WDT_H_

///
/// GUID for the WDT Protocol
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define WDT_PROTOCOL_GUID \
  { \
    0xB42B8D12, 0x2ACB, 0x499a, 0xA9, 0x20, 0xDD, 0x5B, 0xE6, 0xCF, 0x09, 0xB1 \
  }

#else

#define WDT_PROTOCOL_GUID \
  { \
    0xB42B8D12, 0x2ACB, 0x499a, \
    { \
      0xA9, 0x20, 0xDD, 0x5B, 0xE6, 0xCF, 0x09, 0xB1 \
    } \
  }

#endif
//
// Extern the GUID for protocol users.
//
extern EFI_GUID               gWdtProtocolGuid;
//
// Forward reference for ANSI C compatibility
//
typedef struct _WDT_PROTOCOL  WDT_PROTOCOL;

/**
  Reloads WDT with new timeout value and starts it. Also sets Unexpected Reset bit, which
  causes the next reset to be treated as watchdog expiration - unless AllowKnownReset()
  function was called too.

  @param[in] TimeoutValue         Time in seconds before WDT times out. Supported range = 1 - 1024.

  @retval EFI_SUCCESS             if everything's OK
  @retval EFI_INVALID_PARAMETER   if TimeoutValue parameter is wrong
**/
typedef
EFI_STATUS
(EFIAPI *WDT_RELOAD_AND_START) (
  UINT32 TimeoutValue
  );

/**
  Returns WDT failure status.

  @param[in] None

  @retval V_PCH_OC_WDT_CTL_STATUS_FAILURE   If there was WDT expiration or unexpected reset
  @retval V_PCH_OC_WDT_CTL_STATUS_OK        Otherwise
**/
typedef
UINT8
(EFIAPI *WDT_CHECK_STATUS) (
  VOID
  );

/**
  Returns information if WDT coverage for the duration of BIOS execution
  was requested by an OS application.

  @param[in] None

  @retval TRUE                    if WDT was requested
  @retval FALSE                   if WDT was not requested
**/
typedef
UINT8
(EFIAPI *IS_WDT_REQUIRED) (
  VOID
  );

/**
  Returns WDT enabled/disabled status.

  @param[in] None

  @retval TRUE                    if WDT is enabled
  @retval FALSE                   if WDT is disabled
**/
typedef
UINT8
(EFIAPI *IS_WDT_ENABLED) (
  VOID
  );

/**
  Disables WDT timer.

  @param[in] None

  @retval None
**/
typedef
VOID
(EFIAPI *WDT_DISABLE) (
  VOID
  );

/**
  Normally, each reboot performed while watchdog runs is considered a failure.
  This function allows platform to perform expected reboots with WDT running,
  without being interpreted as failures.
  In DXE phase, it is enough to call this function any time before reset.
  In PEI phase, between calling this function and performing reset, ReloadAndStart()
  must not be called.

  @param[in] None

  @retval None
**/
typedef
VOID
(EFIAPI *WDT_ALLOW_KNOWN_RESET) (
  VOID
  );

///
/// These protocols and PPI allow a platform module to perform watch dog timer operations
/// through the Intel PCH LPC Host Controller Interface. The WDT protocol and WDT PPI
/// implement the Intel (R) Watch Dog timer for DXE, and PEI environments, respectively.
/// WDT_PROTOCOL referenced hereafter represents both WDT_PROTOCOL and WDT_PPI, as they
/// share the identical data structure.
///
struct _WDT_PROTOCOL {
  WDT_RELOAD_AND_START  ReloadAndStart;   ///< Reloads WDT with new timeout value and starts it.
  WDT_CHECK_STATUS      CheckStatus;      ///< Returns WDT failure status.
  WDT_DISABLE           Disable;          ///< Disables WDT timer.
  WDT_ALLOW_KNOWN_RESET AllowKnownReset;  ///< Perform expected reboots with WDT running, without being interpreted as failures.
  IS_WDT_REQUIRED       IsWdtRequired;    ///< Returns information if WDT coverage for the duration of BIOS execution was requested by an OS application.
  IS_WDT_ENABLED        IsWdtEnabled;     ///< Returns WDT enabled/disabled status.
};

#endif
