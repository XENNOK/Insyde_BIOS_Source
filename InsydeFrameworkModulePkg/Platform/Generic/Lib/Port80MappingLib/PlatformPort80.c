//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
 
  PlatformPort80.c
 
Abstract:

  Library to map status code to P80 code for the platform

--*/

#include "TianoCommon.h"
#include "EfiCommonLib.h"
#include "EfiStatusCode.h"
#include "OemStatusCode.h"

#pragma pack(1)
typedef struct {
  EFI_STATUS_CODE_TYPE    CodeType;
  EFI_STATUS_CODE_VALUE   StatusValue;
  UINT8                   Port80Value;
} EFI_STATUS_CODE_TO_PORT_80;
#pragma pack()

const EFI_STATUS_CODE_TO_PORT_80  mPort80Table[] = {

  //
  // PEI status code
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_PC_ENTRY_POINT,               0x10},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_CORE | EFI_SW_PS_PC_DISPATCH,                        0x11},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_BEGIN,                         0x12},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_SERVICE | EFI_SW_PS_PC_GET_BOOT_MODE,                0x13},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_PC_CAPSULE_LOAD,                0x14},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_PC_CAPSULE_START,               0x15},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_PC_RECOVERY_BEGIN,              0x16},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_CORE_PC_HANDOFF_TO_NEXT,         0x17},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_PC_RECOVERY_AUTO,               0x18},
  {EFI_ERROR_CODE,    EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_EC_NO_RECOVERY_CAPSULE,             0x1E},
  {EFI_ERROR_CODE,    EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_EC_INVALID_CAPSULE_DESCRIPTOR,      0x1F},
 
  //
  //Dxe status code
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_ENTRY_POINT,               0x20},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_DXE_SERVICE | EFI_SW_DS_PC_DISPATCH,                 0x21},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_BEGIN,                         0x22},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_START_DRIVER,              0x23},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_LOAD_IMAGE,              0x24},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_HANDOFF_TO_NEXT,           0x25},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_VAR_RECLAIM,             0x26},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_CORE | EFI_SW_DXE_CORE_PC_ARCH_READY,                0x27},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_EXIT_BOOT_SERVICES,      0x28},
  {EFI_ERROR_CODE,    EFI_SOFTWARE_DXE_CORE | EFI_CU_EC_NON_SPECIFIC,                           0x2F},

  //
  //BDS
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_ENTRY_POINT,                            0x30},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_RETURN_TO_LAST,                         0x31},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_SETUP_POLICY,                           0x32},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_CONNECT_CONTROLLER,                     0x33},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_FLASH_UPDATE,                           0x34},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_BDS | EFI_SW_BDS_HANDOFF_TO_NEXT,                        0x35},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_INPUT_WAIT,                    0x36},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP,                    0x37},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_OPROM_INIT,      0x38},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT,    0x39},
  {EFI_ERROR_CODE,    EFI_SOFTWARE_BDS | EFI_SW_EC_NON_SPECIFIC,                                0x3F},

  //
  //Memory
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_SPD_READ,                0x40},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_PRESENCE_DETECT,         0x41},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TIMING,                  0x42},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_CONFIGURING,             0x43},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_OPTIMIZING,              0x44},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,                    0x45},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TEST,                    0x46},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT_DONE,               0x47},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_SW_EC_NON_SPECIFIC,                       0x4F},

  //
  //host processor
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_POWER_ON_INIT,       0x50},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_CACHE_INIT,          0x51},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_BSP_SELECT,          0x52},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT,             0x53},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_SMM_INIT,            0x54},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_NON_SPECIFIC,               0x5F},

  //
  //for chipset
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_PC_INIT_BEGIN,                    0x60},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_PC_SOUTH_INIT,            0x61},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_NORTH_INIT,               0x62},
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_SIO_INIT,                 0x63},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_EC_NON_SPECIFIC,                      0x6F},

  //
  //PCI
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PC_INIT,                                     0x70},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_BUS_ENUM,                             0x71},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_HPC_INIT,                             0x72},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PC_ENABLE,                                   0x73},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_RES_ALLOC,                            0x74},
  {EFI_ERROR_CODE,    EFI_IO_BUS_PCI | EFI_IOB_EC_NON_SPECIFIC,                                 0x75},

  //
  //USB
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_INIT,                                     0x7B},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_USB_BUS_ENUM,                                0x7C},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_ENABLE,                                   0x7D},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_RESET,                                    0x7E},
  {EFI_ERROR_CODE,    EFI_IO_BUS_USB | EFI_IOB_EC_NON_SPECIFIC,                                 0x7F},

  //
  //ATA/ATAPI
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_PRESENCE_DETECT,                      0x85},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_ENUM,                          0x86},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_ENABLE,                  0x87},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_RESET,                              0x88},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_NON_SPECIFIC,                           0x89},

  //
  //SMBUS
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_RESET,                                  0x95},
  {EFI_ERROR_CODE,    EFI_IO_BUS_SMBUS | EFI_IOB_EC_NON_SPECIFIC,                               0x96},

  //
  //SCSI
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SCSI | EFI_IOB_PC_INIT,                                    0x98},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SCSI | EFI_IOB_PC_RESET,                                   0x99},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SCSI | EFI_IOB_PC_DETECT,                                  0x9A},

  //
  //LPC/ISA
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_LPC | EFI_IOB_PC_INIT,                                     0x9B},
  {EFI_ERROR_CODE,    EFI_IO_BUS_LPC | EFI_IOB_EC_NON_SPECIFIC,                                 0x9C},

  //
  //Console
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_RESET,                        0xA0},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_DISABLE,                      0xA1},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_ENABLE,                       0xA2},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NON_SPECIFIC,                     0xA3},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_PC_RESET,                       0xA4},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_PC_DISABLE,                     0xA5},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_PC_ENABLE,                      0xA6},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_NON_SPECIFIC,                    0xA7},

  //
  //keyboard: PS2 or USB
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_RESET,                             0xA8},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_DISABLE,                           0xA9},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_PRESENCE_DETECT,                   0xAA},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_ENABLE,                            0xAB},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_CLEAR_BUFFER,             0xAC},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST,                0xAD},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NON_SPECIFIC,                         0xAE},

  //
  //mouse
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_MOUSE | EFI_P_PC_RESET,                                0xAF},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_MOUSE | EFI_P_PC_DETECTED,                             0xB0},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT,                      0xB1},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE,                               0xB2},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_MOUSE | EFI_P_EC_NON_SPECIFIC,                            0xB3},

  //
  //LCD console
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LCD_DEVICE | EFI_P_PC_RESET,                           0xB4},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LCD_DEVICE | EFI_P_PC_DISABLE,                         0xB5},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LCD_DEVICE | EFI_P_PC_ENABLE,                          0xB6},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LCD_DEVICE | EFI_P_EC_NON_SPECIFIC,                        0xB7},

  //
  //serial
  //
  {EFI_PROGRESS_CODE, EFI_P_PC_ENABLE | EFI_PERIPHERAL_SERIAL_PORT,                         0xB8},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_SERIAL_PORT | EFI_P_EC_NON_SPECIFIC,                      0xB9},

  //
  //parallel
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_PC_ENABLE,                       0xBA},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_PARALLEL_PORT | EFI_P_EC_NON_SPECIFIC,                    0xBB},

  //
  //Fix Media
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_RESET,                          0xC0},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_DISABLE,                        0xC1},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_PRESENCE_DETECT,                0xC2},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_PC_ENABLE,                         0xC3},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_FIXED_MEDIA | EFI_P_EC_NON_SPECIFIC,                      0xC7},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_RESET,                      0xC8},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_DISABLE,                    0xC9},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_PRESENCE_DETECT,            0xCA},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_ENABLE,                     0xCB},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_PC_INIT,                       0xCC},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_REMOVABLE_MEDIA | EFI_P_EC_NON_SPECIFIC,                  0xCF},
 
  //
  //other
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_AUDIO_INPUT | EFI_P_PC_INIT,                           0xD0},
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_NETWORK | EFI_P_PC_INIT,                               0xD1},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_AGP | EFI_IOB_PC_INIT,                                     0xD2},
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PC_CARD | EFI_IOB_PC_INIT,                                 0xD3},
 
  //
  //runtime & EFI OS boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RT_PC_ENTER_SLEEP,          0xF0},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RT_PC_EXIT_SLEEP,           0xF1},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_RS_PC_SET_VIRTUAL_ADDRESS_MAP, 0xF2},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_RESET_SYSTEM,         0xF3},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_PC_INIT,                          0xF4},
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_GET_VARIABLE,         0xF5},
  //
  // Starting to boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT,      0xF6},
  {EFI_ERROR_CODE,    EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_NON_SPECIFIC,               0xFF}

};

BOOLEAN
EFIAPI
StatusCodeToPostCode (
  IN  EFI_STATUS_CODE_TYPE    CodeType,
  IN  EFI_STATUS_CODE_VALUE   Value,
  OUT UINT8                   *PostCode
  )
/*++

Routine Description:

  Convert code value to an 8 bit post code

Arguments:

  CodeType  - Code type
  Value     - Code value
  PostCode  - Post code as output

Returns:

  TRUE    - Successfully converted
 
  FALSE   - Convertion failed

--*/
{
  UINTN             Index;
 
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_DEBUG_CODE) {
    return FALSE;
  }
 
  for (Index = 0; Index < sizeof (mPort80Table) / sizeof (EFI_STATUS_CODE_TO_PORT_80); Index++) {
    if (CodeType == mPort80Table[Index].CodeType && mPort80Table[Index].StatusValue == Value) {
      *PostCode = mPort80Table[Index].Port80Value;
      return TRUE;
    }
  }
  return FALSE;
}
