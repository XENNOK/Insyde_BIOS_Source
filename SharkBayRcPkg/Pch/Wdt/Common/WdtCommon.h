/** @file
  Library that contains common parts of WdtPei and WdtDxe. Not a standalone module.

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

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
  WdtCommon.h 

@brief
  Library that contains common parts of WdtPei and WdtDxe. Not a standalone module.

**/
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

extern UINT8    mAllowExpectedReset;
extern EFI_GUID mWdtHobGuid;

#define WDT_HOB_GUID \
  { \
    0x65675786, 0xacca, 0x4b11, \
    { \
      0x8a, 0xb7, 0xf8, 0x43, 0xaa, 0x2a, 0x8b, 0xea \
    } \
  }
//
// HOB definitions duplicated from HOB.h which couldn't be included directly,
// because it requires either EdkIIGluePeim.h or EdkIIGlueDxe.h,
// and WdtCommon must not include any of those.
//
#ifndef _PEI_HOB_H_
#ifndef __PI_HOB_H__
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct {
  EFI_HOB_GUID_TYPE Header;
  UINT16            TimeoutValue;
  UINT8             Active;
} WDT_HOB;

/**
  Reads LPC bridge to get Watchdog Timer address

  @param[in] none

  @retval UINT32                  Watchdog's address
**/
UINT32
WdtGetAddress (
  VOID
  );

/**
  Reloads WDT with new timeout value and starts it. Also sets Unexpected Reset bit, which
  causes the next reset to be treated as watchdog expiration - unless AllowKnownReset()
  function was called too.

  @param[in] TimeoutValue         Time in seconds before WDT times out. Supported range = 1 - 1024.

  @retval EFI_SUCCESS             if everything's OK
  @retval EFI_INVALID_PARAMETER   if TimeoutValue parameter is wrong
**/
EFI_STATUS
EFIAPI
WdtReloadAndStart (
  IN  UINT32  TimeoutValue
  );

/**
  Disables WDT timer.

  @param[in] None

  @retval None
**/
VOID
EFIAPI
WdtDisable (
  VOID
  );

/**
  Returns WDT failure status.

  @param[in] None

  @retval V_PCH_OC_WDT_CTL_STATUS_FAILURE   If there was WDT expiration or unexpected reset
  @retval V_PCH_OC_WDT_CTL_STATUS_OK        Otherwise
**/
UINT8
EFIAPI
WdtCheckStatus (
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
VOID
EFIAPI
WdtAllowKnownReset (
  VOID
  );

/**
  Returns information if WDT coverage for the duration of BIOS execution
  was requested by an OS application

  @param[in] None

  @retval TRUE                    if WDT was requested
  @retval FALSE                   if WDT was not requested
**/
UINT8
EFIAPI
IsWdtRequired (
  VOID
  );

/**
  Returns WDT enabled/disabled status.

  @param[in] None

  @retval TRUE                    if WDT is enabled
  @retval FALSE                   if WDT is disabled
**/
UINT8
EFIAPI
IsWdtEnabled (
  VOID
  );
