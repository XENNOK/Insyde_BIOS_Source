//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++
 
Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  MiscDevicePath.h

Abstract:

  Misc class required EFI Device Path definitions (Ports, slots & 
  onboard devices)

--*/

#ifndef _MISC_DEVICE_PATH_H
#define _MISC_DEVICE_PATH_H

#include "TianoDevicePath.h"

#pragma pack(1)
//
// USB
//

/* For reference:
#define USB1_1_STR  "ACPI(PNP0A03,0)/PCI(1D,0)."
#define USB1_2_STR  "ACPI(PNP0A03,0)/PCI(1D,1)."
#define USB1_3_STR  "ACPI(PNP0A03,0)/PCI(1D,2)."
#define USB2_1_STR  "ACPI(PNP0A03,0)/PCI(1D,7)." 
*/

//
// #define acpi { 0x02, 0x01, 0x00, 0x0C, 0x0a0341d0, 0x00000000 }
// #define pci( device,function)  { 0x01, 0x01, 0x00, 0x06, device, function }
// #define end  { 0xFF, 0xFF, 0x00, 0x04 }
//
#define ACPI \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (0x0A03), 0 \
  }
#define PCI(device, function) \
  { \
    HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8) (sizeof (PCI_DEVICE_PATH)), (UINT8) \
      ((sizeof (PCI_DEVICE_PATH)) >> 8), function, device \
  }
#define END \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 \
  }

#define LPC(eisaid, function) \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (eisaid), function \
  }

#pragma pack()

#endif
