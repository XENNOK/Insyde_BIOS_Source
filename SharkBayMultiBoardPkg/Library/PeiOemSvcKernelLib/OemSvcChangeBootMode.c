/** @file
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPolicy to control normal boot mode identification. 
  If SkipPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SetupConfig.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Library/PcdLib.h>
#include <Library/CmosLib.h>
#include <Library/PeiServicesLib.h>
#include <Guid/DebugMask.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <ChipsetCmos.h>
#include <EfiPrePostHotkey.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>


//
//
// There are 2 example to control the boot mode:
//    1. Simple boot flag
//    2. Cold/Warm boot flag : this is a Arrandale sample, not for Clarksfield
//
// Change below compiling switch to choose your control
//
#define SIMPLE_BOOT_FLAG_CONTROL          0


/**
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPriorityPolicy to control normal boot mode identification. 
  If SkipPriorityPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

  @param[in, out]  *BootMode             Assign default boot mode.
  @param[in, out]  *SkipPriorityPolicy   If SkipPriorityPolicy == TRUE, then normal boot mode identification will be skipped.

  @retval          EFI_SUCCESS           Similar to set *SkipPriorityPolicy = TRUE.
  @retval          EFI_UNSUPPORTED       Kernel should go through boot mode policy code below. 
**/
EFI_STATUS
OemSvcChangeBootMode (
  IN OUT EFI_BOOT_MODE                  *BootMode,
  OUT    BOOLEAN                        *SkipPriorityPolicy
  )
{
  EFI_STATUS                            Status;
  SYSTEM_CONFIGURATION                  SystemConfiguration;
  EFI_PRE_POST_HOTKEY                   PrePostHotkey;
  UINTN                                 VariableSize;
  EFI_GUID                              SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_READ_ONLY_VARIABLE_PPI        *Variable;
//[-start-120912-IB06460440-remove]//
//  UINT16                                BootPrevious;  
//[-end-120912-IB06460440-remove]//

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariablePpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Change boot mode only if Quick boot is enabled.
  //
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = Variable->PeiGetVariable (
                       (EFI_PEI_SERVICES **)GetPeiServicesTablePointer (),
                       L"Setup",
                       &SystemConfigurationGuid,
                       NULL,
                       &VariableSize,
                       &SystemConfiguration
                       );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  if (SystemConfiguration.QuickBoot == 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Change boot mode if no Pre-POST hotkey pressed
  //
  VariableSize = sizeof (EFI_PRE_POST_HOTKEY);
  Status = Variable->PeiGetVariable (
                       (EFI_PEI_SERVICES **)GetPeiServicesTablePointer (),
                       EFI_PRE_POST_HOTKEY_NAME,
                       &gEfiGenericVariableGuid, 
                       NULL,
                       &VariableSize,
                       &PrePostHotkey
                       );
  if ((Status == EFI_SUCCESS) && (PrePostHotkey.KeyBit != PRE_POST_HOTKEY_NOT_EXIST)) {
    return EFI_UNSUPPORTED;
  }

  if (SystemConfiguration.SetupVariableInvalid == 1) {
    //
    // Setup Variable is invalid, force run in BOOT_WITH_FULL_CONFIGURATION.
    //
    return EFI_UNSUPPORTED;    
  }
//[-start-120912-IB06460440-remove]//
//  VariableSize = sizeof (UINT16);
//  Status = Variable->PeiGetVariable (
//                               PeiServices,
//                               L"BootPrevious",
////[-start-120326-IB02960435-modify]//
//                               &gEfiGenericVariableGuid,
////[-end-120326-IB02960435-modify]//
//                               NULL,
//                               &VariableSize,
//                               &BootPrevious
//                               );
//  if (Status == EFI_NOT_FOUND) {
//    //
//    // BootPrevious Variable is invalid, force run in BOOT_WITH_FULL_CONFIGURATION.
//    //
//    return EFI_SUCCESS;        
//  }
//  //
//  // Change boot mode according to Simple Boot Flag
//  //
////[-start-120605-IB03672099-modify]//
//  if (SystemConfiguration.BootState2 == 0) {
//    *BootMode = BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
//  }
////[-end-120605-IB03672099-modify]//
//[-end-120912-IB06460440-remove]//
  //
  // return EFI_SUCCESS only when setting *SkipPriorityPolicy = TRUE
  //

  return EFI_UNSUPPORTED;
}
