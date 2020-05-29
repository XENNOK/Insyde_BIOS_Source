/** @file
  PlatformBdsLib

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

/** 
  Defined the platform specific device path which will be used by
  platform Bbd to perform the platform policy connect.

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BdsPlatform.h"
//[-start-130204-IB10820231-modify]//
//#ifdef ME_5MB_SUPPORT
#include "MeChipset.h"
//#endif
//[-end-130204-IB10820231-modify]//


//
// Predefined platform root bridge
//
PLATFORM_ROOT_BRIDGE_DEVICE_PATH gPlatformRootBridge0 = {
  gPciRootBridge,
  gEndEntire
};

EFI_DEVICE_PATH_PROTOCOL* gPlatformRootBridges [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformRootBridge0,
  NULL
};

//
// Platform specific keyboard device path
//
PLATFORM_KEYBOARD_DEVICE_PATH gKeyboardDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0303),
    0
  },
  gEndEntire
};
//
// Platform specific PS2 Mouse device path
//
PLATFORM_MOUSE_DEVICE_PATH gMouseDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0303),
    0x01
  },
  gEndEntire
};

//
// Platform specific on chip internal graphics device path
//
PLATFORM_ONBOARD_VGA_DEVICE_PATH gOnChipIgfxDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x2
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};

//
// Platform specific plug in PEG device path
//
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath10 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};

PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath11 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0x1,
    0x1    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath12 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0x2,
    0x1    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath60 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x6    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};

//
// Platform specific plug in PCIE VGA device path
//
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath0 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath1 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    1,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath2 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    2,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath3 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    3,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath4 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    4,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath5 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    5,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath6 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    6,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPcieDevicePath7 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    7,
    0x1C    
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};

//
// Platform specific plug in PCI VGA device path
//
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath0 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1E   
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath1 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1E   
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    1
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath2 = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    0x1E   
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0,
    5
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    (UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8),
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
  },
  gEndEntire
};

//
// Platform specific SATA device path
//
PLATFORM_SATA_DEVICE_PATH gSataPort0DevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x1F
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_SATA_DP,
    (UINT8) (sizeof (SATA_DEVICE_PATH)),
    (UINT8) ((sizeof (SATA_DEVICE_PATH)) >> 8),
    0,
    0,
    0
  },
  gEndEntire
};

PLATFORM_SATA_DEVICE_PATH gSataPort1DevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x1F
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_SATA_DP,
    (UINT8) (sizeof (SATA_DEVICE_PATH)),
    (UINT8) ((sizeof (SATA_DEVICE_PATH)) >> 8),
    1,
    1,
    0
  },
  gEndEntire
};

//
// Platform specific serial device path
//
PLATFORM_ISA_SERIAL_DEVICE_PATH gSerialDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x1f
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),
    0
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8)(sizeof(UART_DEVICE_PATH)),
    (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
    (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};

// Platform specific serial over lan device path
//
//[-start-130204-IB10820231-modify]//
// Current gSerialOverLANDevicePath only call by BdsLibUpdateConsoleVariable When PcdMe5MbSupported = TRUE
// So remove ifndef ME_5MB_SUPPORT code 
PLATFORM_ISA_SERIAL_OVER_LAN_DEVICE_PATH gSerialOverLANDevicePath = {
  gPciRootBridge,
//#ifndef ME_5MB_SUPPORT
//  {
//    HARDWARE_DEVICE_PATH,
//    HW_PCI_DP,
//    (UINT8)(sizeof(PCI_DEVICE_PATH)),
//    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
//    0x0,
//    0x1c
//  },
//#else
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
    SOL_FUNCTION_NUMBER,
    ME_DEVICE_NUMBER
  },
//#endif
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8)(sizeof(UART_DEVICE_PATH)),
    (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
    (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
//#ifdef ME_5MB_SUPPORT
    DEVICE_PATH_MESSAGING_VT_100
//#else
//    DEVICE_PATH_MESSAGING_PC_ANSI
//#endif
  },
  gEndEntire
};
//[-end-130204-IB10820231-modify]//
//
// Platform last boot device path
//
PLATFORM_SATA_DEVICE_PATH gLastBootDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x1F
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_SATA_DP,
    (UINT8) (sizeof (SATA_DEVICE_PATH)),
    (UINT8) ((sizeof (SATA_DEVICE_PATH)) >> 8),
    0,
    0,
    0
  },
  gEndEntire
};

//
// Predefined platform default console device path
//
BDS_CONSOLE_CONNECT_ENTRY gPlatformConsole [] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gKeyboardDevicePath  , CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gMouseDevicePath     , CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gOnChipIgfxDevicePath, CONSOLE_OUT},
  {NULL, 0}
};

//
// All the possible platform on chip internal graphics device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossibleIgfxConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gOnChipIgfxDevicePath, // IGD (D2/F0)
  NULL
};

//
// All the possible platform PEG device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePegConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPegDevicePath10, // PEG bridge (0x00/0x01/0x00) VGA device (D0/F0), Highest priority
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPegDevicePath11, // PEG bridge (0x00/0x01/0x01) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPegDevicePath12, // PEG bridge (0x00/0x01/0x02) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPegDevicePath60, // PEG bridge (0x00/0x06/0x00) VGA device (D0/F0), Lowest priority
  NULL
};

//
// All the possible platform PCIE graphics device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePcieConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath0, // SB PCIE bridge (0x00/0x1C/0x00) VGA device (D0/F0), Highest priority
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath1, // SB PCIE bridge (0x00/0x1C/0x01) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath2, // SB PCIE bridge (0x00/0x1C/0x02) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath3, // SB PCIE bridge (0x00/0x1C/0x03) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath4, // SB PCIE bridge (0x00/0x1C/0x04) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath5, // SB PCIE bridge (0x00/0x1C/0x05) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath6, // SB PCIE bridge (0x00/0x1C/0x06) VGA device (D0/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPcieDevicePath7, // SB PCIE bridge (0x00/0x1C/0x07) VGA device (D0/F0), Lowest priority
  NULL
};

//
// All the possible platform PCI graphics device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePciConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPciDevicePath0, // SB PCI bridge (0x00/0x1E/0x00) VGA device (D0/F0), Highest priority
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPciDevicePath1, // SB PCI bridge (0x00/0x1E/0x00) VGA device (D1/F0)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlugInPciDevicePath2, // SB PCI bridge (0x00/0x1E/0x00) VGA device (D5/F0), Lowest priority
  NULL
};

//
// Legacy hard disk boot option
//
LEGACY_HD_DEVICE_PATH gLegacyHd = {
  {
    BBS_DEVICE_PATH,
    BBS_BBS_DP,
    (UINT8)(sizeof(BBS_BBS_DEVICE_PATH)),
    (UINT8)((sizeof(BBS_BBS_DEVICE_PATH)) >> 8),
    BBS_TYPE_HARDDRIVE,
    0,
    0
  },
  gEndEntire
};

//
// Legacy cdrom boot option
//
LEGACY_HD_DEVICE_PATH gLegacyCdrom = {
  {
    BBS_DEVICE_PATH,
    BBS_BBS_DP,
    (UINT8)(sizeof(BBS_BBS_DEVICE_PATH)),
    (UINT8)((sizeof(BBS_BBS_DEVICE_PATH)) >> 8),
    BBS_TYPE_CDROM,
    0,
    0
  },
  gEndEntire
};

//
// Predefined platform specific perdict boot option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformBootOption [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gLegacyHd,
  (EFI_DEVICE_PATH_PROTOCOL*)&gLegacyCdrom,
  NULL
};

//
// Predefined platform specific driver option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformDriverOption [] = {
  NULL
};

//
// Predefined platform connect sequence
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectSequence [] = {
  NULL
};

//
// Predefined platform connect SATA device
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectSata [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gSataPort0DevicePath,
  NULL
};
//
// Runtime Updated platform connect Last Boot SATA device
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectLastBoot [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gLastBootDevicePath,
  NULL
};

