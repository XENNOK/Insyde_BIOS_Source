/**
 PEI Chipset Services Library.
 
 This file provides the prototype of all PEI Chipset Services Library function.

***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _PEI_CHIPSET_SVC_LIB_H_
#define _PEI_CHIPSET_SVC_LIB_H_
#include <Uefi.h>
#include <ChipsetSvc.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Guid/PlatformHardwareSwitch.h>

/**
 Platform specific function to enable/disable flash device write access. 

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/                 
EFI_STATUS
EFIAPI
PeiCsSvcEnableFdWrites (
  IN  BOOLEAN           EnableWrites
  );

/**
 Legacy Region Access Control.
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_UNSUPPORTED       If the function is not implemented.
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
EFI_STATUS
PeiCsSvcLegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  );

/**
 To identify sleep state.

 @param[in, out]    SleepState          3 - This is an S3 restart
                                        4 - This is an S4 restart
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcGetSleepState (
  IN OUT  UINT8              *SleepState
  );
  
/**
 To provide a interface for chipset porting on different HDA controller.

 @param[in]         HdaBar              Base address for HDA
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcHdaInitHook (
  IN UINT32          HdaBar
  );
  
/**
 To modify memory range setting.

 @param[in, out]    OptionRomMask       A pointer to PEI_MEMORY_RANGE_OPTION_ROM
 @param[in, out]    SmramMask           A pointer to PEI_MEMORY_RANGE_SMRAM
 @param[in, out]    GraphicsMemoryMask  A pointer to PEI_MEMORY_RANGE_GRAPHICS_MEMORY
 @param[in, out]    PciMemoryMask       A pointer to PEI_MEMORY_RANGE_PCI_MEMORY
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  );
  
/**
 Platform initialization in PEI phase stage 1.

 @param[in]         None

 @retval            EFI_SUCCESS         This function alway return successfully.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcPlatformStage1Init (
  VOID
  );
  
/**
 Chipset initialization code in stage2 of PEI phase.

 @param[in]         None
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcPlatformStage2Init (
  VOID
  );
  
/**
 When variable not found, to set default variables.

 @param[in]         SystemConfiguration A pointer to setup variables

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcSetSetupVariableDefault (
  IN VOID                 *SystemConfiguration
  );
  
/**
 Install Firmware Volume Hob's once there is main memory

 @param[in]         BootMode            Boot mode

 @retval            EFI_SUCCESS         This function alway return successfully.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcStage1MemoryDiscoverCallback (
  IN  EFI_BOOT_MODE        *BootMode
  );
  
/**
 Install Firmware Volume Hob's once there is main memory.

 @param [in]        BootMode            A pointer to boot mode status
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcStage2MemoryDiscoverCallback (
  IN EFI_BOOT_MODE       *BootMode
  );

/**
 To feedback its proprietary settings of the hardware switches.

 @param[in]         PlatformHardwareSwitch     Pointer to PLATFORM_HARDWARE_SWITCH.

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcSetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  );

#endif
