/** @file

  Header file for Heci Message functionality

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

#ifndef _PEI_HECI_MESSAGE_LIB_H_
#define _PEI_HECI_MESSAGE_LIB_H_

#include <Uefi.h>
#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <CoreBiosMsg.h>
#include <Ppi/Heci.h>
#include <Ppi/PlatformMeHook.h>
#include <Ppi/Wdt.h>

#define START_WDT_BYTE_COUNT                      0x0F
#define START_WDT_VERSION_NUMBER                  0x10
#define START_WDT_EVENT_SENSOR_TYPE               0x23
#define START_WDT_EVENT_TYPE                      0x6F
#define START_WDT_EVENT_OFFSET                    0x00
#define START_WDT_EVENT_SOURCE_TYPE_OS            0x48
#define START_WDT_EVENT_SOURCE_TYPE_BIOS          0x00
#define START_WDT_EVENT_SOURCE_TYPE               0x00  ///< 0x00 - BIOs, 0x48 - OS
#define START_WDT_EVENT_SEVERITY                  0x10  ///< critical
#define START_WDT_SENSOR_DEVICE                   0xFF  ///< unspecified
#define START_WDT_SENSOR_NUMBER                   0xFF  ///< unspecified
#define START_WDT_ENTITY                          0x00  ///< unspecified
#define START_WDT_ENTITY_INSTANCE                 0x00  ///< unspecified
#define START_WDT_EVENT_DATA_BYTE_0               0x40
#define START_WDT_EVENT_DATA_BYTE_1               0x02  ///< 0x02 BIOS POST WDT Timeout, 0x04 OS WDT timeout
#define START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT  0x02
#define START_WDT_EVENT_DATA_BYTE_1_OS_TIMEOUT    0x04

/**
  Start Watch Dog Timer HECI message

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] HeciPpi              The pointer to HECI PPI
  @param[in] HeciMemBar           HECI Memory BAR
  @param[in] WaitTimerBios        The value of waiting limit

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciAsfStartWatchDog (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       PEI_HECI_PPI                *HeciPpi,
  IN       UINT32                      HeciMemBar,
  IN       UINT16                      WaitTimerBios
  );

/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] ResetOrigin          Reset source
  @param[in] ResetType            Global or Host reset

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendCbmResetRequest (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT8                       ResetOrigin,
  IN       UINT8                       ResetType
  );

#endif
