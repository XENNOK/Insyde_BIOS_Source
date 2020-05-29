//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Platform.h

Abstract:

  Header file for Platform Initialization Driver.

Revision History

++*/

#ifndef _PLATFORM_DRIVER_H
#define _PLATFORM_DRIVER_H

///===============================================
///  MdePkg/Include/
///===============================================
#include <PiDxe.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>

#include <Guid/HobList.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/WatchdogTimer.h>
///===============================================
///  InsydeModulePkg/Include/
///===============================================
#include <Library/CmosLib.h>
#include <Guid/SetupDefaultGuid.h>
///===============================================
///  $(CHIPSET_PKG)/Include/
///===============================================
#include <ChipsetCmos.h>
#include <Protocol/ICCPlatformPolicy.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Include/
///===============================================
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/Include
///===============================================
#include <PchAccess.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/Me/Library/MeKernel/Dxe/
///===============================================
#include <MeLib.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/Me/
///===============================================
#include <Protocol/Heci.h>
#include <Protocol/MeBiosPayloadData.h>

#include <ChipsetSetupConfig.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/PlatformPhaseNotify.h>

#define ICC_LOCK_ALL_MASK               0x0
#define ICC_LOCK_STATIC_MASK            0xF0F0013
                                      
#define R_PCH_LPC_PM_INIT               0xAC
#define B_PCH_LPC_GLOBAL_RESET_CF9      0x00100000

#define CLEAR_WATCHDOG_TIMER_SUCCESS    0x00
#define NOT_SET_WATCHDOG_TIMER          0x01
#define NOT_CLEAR_WATCHDOG_TIMER        0x02
#define WATCHDOG_TIMER_TIME             5000000

//
// Prototypes
//
/**
  This is the standard EFI driver point for the Platform Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param [in] ImageHandle         Standard entry point parameter.
  @param [in] SystemTable         Standard entry point parameter.

  @retval EFI_SUCCESS             Policy decisions set

**/
EFI_STATUS
EFIAPI
IccInitEntryPoint (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  );

/**
  This function will set the ICC profile

  @param [in] IccProfile          Standard entry point parameter.
  @param [in] SetupVariable       NV variable.

  @retval EFI_SUCCESS             Policy decisions set

**/
EFI_STATUS
SetIccProfile (
  IN UINT8                             IccProfile,
  IN CHIPSET_CONFIGURATION              *SetupVariable
  );

/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param [out] BootMode            Boot mode from PEI handoff HOB.

  @retval EFI_SUCCESS             Successfully get boot mode.
  @retval EFI_NOT_FOUND           Can not find the current system boot mode.

**/
EFI_STATUS
GetBootMode (
  OUT EFI_BOOT_MODE                    *BootMode
  );
  
/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param [out] SetupVariable      NV variable.

  @retval EFI_SUCCESS             Policy decisions set.

**/
EFI_STATUS
SetIccWdt (
  IN CHIPSET_CONFIGURATION              *SetupVariable
  );  
  
/**
  Lock ICC registers.

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,
                                  which is implementation-dependent.

  @retval EFI_SUCCESS             disable complete..

**/
VOID
EFIAPI
SetIccLock (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  );  
  
/**
  Signal handlers for WatchDog event

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,
                                  which is implementation-dependent.

  @retval none

**/
VOID
EFIAPI
WatchDogNotifyFunction (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  );  
  
/**
  A function of this type is called when the watchdog timer fires if a
  handler has been registered.

  @param [in] Time                The time in 100 ns units that has passed since the watchdog
                                  timer was armed.  For the notify function to be called, this
                                  must be greater than TimerPeriod.

  @retval none

**/
VOID
EFIAPI
WatchDogCallback (
  IN UINT64                            Time
  );

//
// Move this function from HeciMsgLib.c to prevent override the file.
//
/**
  retrieves the number of currently used ICC clock profile

  @param [out] Profile            number of current ICC clock profile
  @param [out] ProfileSoftStrap
  

  @retval EFI_UNSUPPORTED

**/
EFI_STATUS
HeciGetIccProfileFunc (
  OUT UINT8                            *Profile,
  OUT UINT8                            *ProfileSoftStrap
  );

/**

  @param none

  @retval none

**/
VOID
ICCSetClockEnables (
  VOID
  );

/**  
  Enables/disables clocks. Used to turn off clocks in unused pci/pcie slots.

  @param [in] Event               Event whose notification function is being invoked.
  @param [in] Context             The pointer to the notification function's context,

  @retval    None
**/
VOID
EFIAPI
ICCSetClockEnablesCallback (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  );

#endif

