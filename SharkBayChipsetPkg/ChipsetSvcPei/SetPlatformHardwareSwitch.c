/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcSetPlatformHardwareSwitch().
 The function PeiCsSvcSetPlatformHardwareSwitch() use chipset services to feedback 
 its proprietary settings of the hardware switches.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <ChipsetSetupConfig.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/PlatformHardwareSwitch.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

//
// helper function prototypes
//
EFI_STATUS
PlatformHardwareSwitchInitWithSetupVar (
  IN PLATFORM_HARDWARE_SWITCH   *PlatformHardwareSwitch
  );

/**
  Initialize platform hardware switch from SystemConfiguration variable.

  @param[in]  PlatformHardwareSwitch     Pointer to PLATFORM_HARDWARE_SWITCH.

  @retval EFI_SUCCESS                 Initialize platform hardware Switch value from SystemConfiguration variable success.
  @retval EFI_NOT_FOUND               The gPeiReadOnlyVariablePpiGuid not been install.
**/
EFI_STATUS
PlatformHardwareSwitchInitWithSetupVar (
  IN PLATFORM_HARDWARE_SWITCH   *PlatformHardwareSwitch
  )
{
  EFI_STATUS                       Status;
  VOID                             *SetupVariable;
  CHIPSET_CONFIGURATION            SystemConfiguration;
  UINTN                            DataSize;
  EFI_STATUS                       VariableStatus;
  EFI_GUID                         mSystemConfigurationGuid  = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Variable;
  EFI_BOOT_MODE                    BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }
 
  DataSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariable = AllocateZeroPool (DataSize);
  //
  // Get desired ratio from NVRAM variable
  //
  VariableStatus = Variable->GetVariable (
                               Variable,
                               L"Setup",
                               &mSystemConfigurationGuid,
                               NULL,
                               &DataSize,
                               SetupVariable
                               );
  
  CopyMem (&SystemConfiguration, SetupVariable, sizeof(CHIPSET_CONFIGURATION));

  //
  // If there's no variable, just report the state of the hardware
  //
  if (!EFI_ERROR (VariableStatus)) {

    //
    // USB Port  
    //
    if (SystemConfiguration.UsbPort0 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort1 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort2 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort3 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort4 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort5 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort6 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port06En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port06En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port06En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort7 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port07En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port07En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port07En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort8 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port08En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port08En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port08En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort9 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port09En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port09En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port09En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort10 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port10En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port10En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port10En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort11 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port11En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port11En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port11En = PORT_DISABLE;
    }
    if (SystemConfiguration.UsbPort12 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port12En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port12En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port12En = PORT_DISABLE;
    }    
    if (SystemConfiguration.UsbPort13 && PlatformHardwareSwitch->UsbPortSwitch.Config.Port13En) {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port13En = PORT_ENABLE;
    } else {
      PlatformHardwareSwitch->UsbPortSwitch.Config.Port13En = PORT_DISABLE;
    }
    //
    // PCIE
    //
    if (SystemConfiguration.PcieRootPortEn0 && PlatformHardwareSwitch->PciePortSwitch.Config.Port00En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port00En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn1 && PlatformHardwareSwitch->PciePortSwitch.Config.Port01En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port01En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn2 && PlatformHardwareSwitch->PciePortSwitch.Config.Port02En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port02En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn3 && PlatformHardwareSwitch->PciePortSwitch.Config.Port03En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port03En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn4 && PlatformHardwareSwitch->PciePortSwitch.Config.Port04En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port04En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn5 && PlatformHardwareSwitch->PciePortSwitch.Config.Port05En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port05En == PCIE_PORT_AUTO) {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port05En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port05En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port05En = PORT_DISABLE;
    }

    if (SystemConfiguration.PcieRootPortEn6 && PlatformHardwareSwitch->PciePortSwitch.Config.Port06En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port06En == PCIE_PORT_AUTO) {	
        PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = PORT_DISABLE;
    }
     
    if (SystemConfiguration.PcieRootPortEn7 && PlatformHardwareSwitch->PciePortSwitch.Config.Port07En) {
      if (PlatformHardwareSwitch->PciePortSwitch.Config.Port07En == PCIE_PORT_AUTO) {	
        PlatformHardwareSwitch->PciePortSwitch.Config.Port07En = PCIE_PORT_AUTO;
      } else {
        PlatformHardwareSwitch->PciePortSwitch.Config.Port07En = PORT_ENABLE;
      }
    } else {
      PlatformHardwareSwitch->PciePortSwitch.Config.Port07En = PORT_DISABLE;
    }   
  }

  return EFI_SUCCESS;
}

/**
  To feedback its proprietary settings of the hardware switches.
  if you wants it to be not auto detected while it is set to 'PORT_ENABLE'. 
  That means, you has 3 choices to a port of PCI-e Ports - 'PORT_DISABLE', 'PCIE_PORT_AUTO', 
  and 'PORT_ENABLE'. For Usb and Sata ports, only two options are available -'PORT_DISABLE' and 'PORT_ENABLE'.

  @param[out]  PlatformHardwareSwitch    A Pointer to PLATFORM_HARDWARE_SWITCH.

  @retval      EFI_SUCCESS                Set hardware switch success.
  @retval      EFI_NOT_FOUND              Locate ReadOnlyVariablePpi failed.
**/
EFI_STATUS
SetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  )
{
  EFI_STATUS                            Status;
  PLATFORM_HARDWARE_SWITCH              *PlatformHardwareSwitch;

  PlatformHardwareSwitch = *PlatformHardwareSwitchDptr;

  //
  // Default Enable all ports
  //

  PlatformHardwareSwitch->SataPortSwitch.CONFIG = 0xFFFFFFFFFFFFFFFF;
  PlatformHardwareSwitch->UsbPortSwitch.CONFIG = 0xFFFFFFFFFFFFFFFF;
//  //
//  // Default set PCIE port configure to Auto mode
//  //
//  PlatformHardwareSwitch->PciePortSwitch.CONFIG = 0xAAAAAAAAAAAAAAAA;
  //
  // because PchPlatformPolicy struct has changed (CougarPoint), a default "Auto(0x2)" will became "Disable(0x0)" when type convert
  // So default set PCIE port configure to Enable mode
  //
  PlatformHardwareSwitch->PciePortSwitch.CONFIG = 0x5555555555555555;

  /*
  Sample code to set Platform Hardware Switch by platform dessign
  All ports is enabled by default. All PCIE ports is configured to Auto
  Below sample code disable Sata PORT4, USB PORT5 and PCIE PORT6.

  PlatformHardwareSwitch->SataPortSwitch.Config.Port04En = PORT_DISABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En = PORT_DISABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = PORT_DISABLE;
  */
  
  //
  // Sata Port
  //
  PlatformHardwareSwitch->SataPortSwitch.Config.Port00En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port01En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port02En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port03En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port04En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port05En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port06En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port07En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port08En = PORT_ENABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port09En = PORT_ENABLE;
  
  //
  // USB Port
  //  
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port06En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port07En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port08En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port09En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port10En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port11En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port12En = PORT_ENABLE;
  PlatformHardwareSwitch->UsbPortSwitch.Config.Port13En = PORT_ENABLE;
  
  //
  // PCIE
  //    
  PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port05En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = PORT_ENABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port07En = PORT_ENABLE; 

  //
  // Initialize platform hardware switch from SystemConfiguration variable.
  //
  Status = PlatformHardwareSwitchInitWithSetupVar (PlatformHardwareSwitch);

  return Status;
}

