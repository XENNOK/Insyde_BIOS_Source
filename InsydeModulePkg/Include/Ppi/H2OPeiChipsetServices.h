/** @file
 H2O PEI Chipset Services interface definition.
 
 This file provides the H2O PEI Chipset Services definition. It is 
 consumed by PEIMs and libraries in the Kernel and Platform layers. It is
 produced by PEIMs and libraries in the Chipset layer.
 
 WARNING: This ppi should not be called directly. Instead, the library
 functions in the PeiChipsetSvcLib should be used, because they add the 
 necessary error checking.

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

#ifndef _H2O_PEI_CHIPSET_SERVICES_H_
#define _H2O_PEI_CHIPSET_SERVICES_H_

#include <Uefi.h>
#include <ChipsetSvc.h>

#include <Ppi/PlatformMemoryRange.h>
#include <Guid/PlatformHardwareSwitch.h>

#define H2O_CHIPSET_SERVICES_PPI_GUID \
  {0x544c6934, 0x1616, 0x4ae5, {0x90, 0xd0, 0xb1, 0xdc, 0x38, 0x44, 0xba, 0x27}}


//
// Function prototypes
//

/**
 Platform specific function to enable/disable flash device write access

 @param[in]         EnableWrites        TRUE - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_ENABLE_FD_WRITES) (
  IN      BOOLEAN                         EnableWrites
);

/**
 Legacy Region Access Control.
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_LEGACY_REGION_ACCESS_CTRL) (
  IN      UINT32                          Start,
  IN      UINT32                          Length,
  IN      UINT32                          Mode  
  );

/**
 To identify sleep state.

 @param[in, out]    SleepState          3 - This is an S3 restart
                                        4 - This is an S4 restart

 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_GET_SLEEP_STATE) (
  IN OUT  UINT8              *SleepState
  );

/**
 To provide a interface for chipset porting on different HDA controller.

 @param[in]         HdaBar              Base address for HDA

 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_HDA_INIT_HOOK) (
  IN UINT32          HdaBar
  );

/**
 Platform initialization in PEI phase stage 1.

 @param[in]         None

 @retval            EFI_SUCCESS         This function alway return successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_PLATFORM_STAGE_1_INIT) (
  VOID
  );

/**
 Install Firmware Volume Hob's once there is main memory

 @param[in]         BootMode            Boot mode

 @retval            EFI_SUCCESS         This function alway return successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_STAGE_1_MEMORY_DISCOVER_CALLBACK) (
  IN  EFI_BOOT_MODE        *BootMode
  );
  
/**
 Chipset initialization code in stage2 of PEI phase.

 @param[in]         None
                   
 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_PLATFORM_STAGE_2_INIT) (
  VOID
  );

/**
 Install Firmware Volume Hob's once there is main memory.

 @param [in]        BootMode            A pointer to boot mode status
                   
 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_STAGE_2_MEMORY_DISCOVER_CALLBACK) (
  IN EFI_BOOT_MODE      *BootMode
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
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_MODIFY_MEMORY_RANGE) (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  );

/**
 When variable not found, to set default variables.

 @param[in]         SystemConfiguration A pointer to setup variables

 @retval            EFI_SUCCESS         Function returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_SET_SETUP_VARIABLE_DEFAULT) (
  IN VOID               *SystemConfiguration
  );

/**
 To feedback its proprietary settings of the hardware switches.

 @param[in]         PlatformHardwareSwitch     Pointer to PLATFORM_HARDWARE_SWITCH.

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_PEI_SET_PLATFORM_HARDWARE_SWITCH) (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  );

typedef struct _H2O_CHIPSET_SERVICES_PPI {
  UINT32                      Size;///< size of this structure, in bytes.
  
  H2O_CS_SVC_PEI_ENABLE_FD_WRITES                  EnableFdWrites;
  H2O_CS_SVC_PEI_LEGACY_REGION_ACCESS_CTRL         LegacyRegionAccessCtrl;
  
  H2O_CS_SVC_PEI_GET_SLEEP_STATE                   GetSleepState;
  H2O_CS_SVC_PEI_HDA_INIT_HOOK                     HdaInitHook;
  H2O_CS_SVC_PEI_PLATFORM_STAGE_1_INIT             PlatformStage1Init;
  H2O_CS_SVC_PEI_STAGE_1_MEMORY_DISCOVER_CALLBACK  Stage1MemoryDiscoverCallback;
  H2O_CS_SVC_PEI_PLATFORM_STAGE_2_INIT             PlatformStage2Init;
  H2O_CS_SVC_PEI_STAGE_2_MEMORY_DISCOVER_CALLBACK  Stage2MemoryDiscoverCallback;
  H2O_CS_SVC_PEI_MODIFY_MEMORY_RANGE               ModifyMemoryRange;
  H2O_CS_SVC_PEI_SET_SETUP_VARIABLE_DEFAULT        SetSetupVariableDefault;
  H2O_CS_SVC_PEI_SET_PLATFORM_HARDWARE_SWITCH      SetPlatformHardwareSwitch;

} H2O_CHIPSET_SERVICES_PPI;

extern EFI_GUID gH2OChipsetServicesPpiGuid;

#endif
