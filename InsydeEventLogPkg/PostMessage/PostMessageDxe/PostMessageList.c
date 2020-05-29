/** @file

  POST Message List implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/ 

#include <PostMessageHookDxe.h>

POST_MESSAGE_LIST                    gPostMessageList[] = {

  //
  // Memory
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,                      BEHAVIOR_DEFAULT_SETTING_FOR_PEI_MSG},  
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,             BEHAVIOR_DEFAULT_SETTING_FOR_PEI_MSG},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,               BEHAVIOR_DEFAULT_SETTING_FOR_PEI_MSG},
  //
  // SMBUS
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,                                     BEHAVIOR_DEFAULT_SETTING_FOR_PEI_MSG},  
  //
  // Legacy Boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT,        BEHAVIOR_DEFAULT_SETTING},  
  //
  // UEFI boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_PC_LOAD,                            BEHAVIOR_DEFAULT_SETTING},
  //
  // ATA/IDE, ~\Bus\Pci\IdeBus\Dxe\idebus.inf
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_INIT,                                 BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_RESET,                                BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_PRESENCE_DETECT,                        BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_ENABLE,                                 BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_DISABLE,                                BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_ENABLE,                    BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_UNDERTHRESHOLD,            BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD,             BEHAVIOR_DEFAULT_SETTING},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR,                      BEHAVIOR_DEFAULT_SETTING},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR,                     BEHAVIOR_DEFAULT_SETTING},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_NOTSUPPORTED,              BEHAVIOR_DEFAULT_SETTING},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_DISABLED,                  BEHAVIOR_DEFAULT_SETTING},
  //
  // PCI
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_RES_ALLOC,                                 BEHAVIOR_DEFAULT_SETTING},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | OEM_IOB_PCI_PC_OPTION_ROM_INIT,                        BEHAVIOR_DEFAULT_SETTING},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,                          BEHAVIOR_DEFAULT_SETTING},
  //
  // Keyboard
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST,                  BEHAVIOR_DEFAULT_SETTING},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR,                    BEHAVIOR_DEFAULT_SETTING},
  //
  // USB
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_RESET,                                      BEHAVIOR_DEFAULT_SETTING},
  //
  // CPU
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT,               BEHAVIOR_DEFAULT_SETTING},  
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN,               BEHAVIOR_DEFAULT_SETTING},  
  //
  //  Software
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_PC_USER_SETUP,                   BEHAVIOR_DEFAULT_SETTING},    
  //
  // VGA
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_INIT,                           BEHAVIOR_DEFAULT_SETTING},  
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_DETECTED,                       BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NON_SPECIFIC,                   BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_DISABLED,                       BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_SUPPORTED,                  BEHAVIOR_DEFAULT_SETTING},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED,                   BEHAVIOR_DEFAULT_SETTING}, 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_CONFIGURED,                 BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INTERFACE_ERROR,                BEHAVIOR_DEFAULT_SETTING},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,               BEHAVIOR_DEFAULT_SETTING} 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INPUT_ERROR,                    BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,                   BEHAVIOR_DEFAULT_SETTING},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,              BEHAVIOR_DEFAULT_SETTING},  

  };

OEM_POST_MESSAGE_LOG                    gOemPostMessageLog[] = {

  //
  // Memory
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,                       {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x01, OEM_SEL_UNDEFINED}, L"Memory initialization"},  
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,              {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x01, OEM_SEL_UNDEFINED}, L"No system memory is physically installed in the system"},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x02, OEM_SEL_UNDEFINED}, L"No usable system memory"},
  //
  // SMBUS
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,                                      {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x0B, OEM_SEL_UNDEFINED}, L"SM Bus initialization"},  
  //
  // Legacy Boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT,         {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x13, OEM_SEL_UNDEFINED}, L"Starting operating system boot process"},  
  //
  // UEFI boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_PC_LOAD,                             {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x13, OEM_SEL_UNDEFINED}, L"Starting operating system boot process"},
  //
  // ATA/IDE, ~\Bus\Pci\IdeBus\Dxe\idebus.inf
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_INIT,                                  {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x02, OEM_SEL_UNDEFINED}, L"ATA Initialization"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_RESET,                               {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA initialization"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_PRESENCE_DETECT,                       {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"To detect ATA drive"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_ENABLE,                                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA device eanbled!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_DISABLE,                               {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA device disable!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_ENABLE,                   {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA SMART enabled!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_UNDERTHRESHOLD,           {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"The threshold exceeded condition is not detected by the ATA device"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD,            {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"The threshold exceeded condition is detected by the ATA device"},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR,                       {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x03, OEM_SEL_UNDEFINED}, L"ATA device error"},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR,                      {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x06, OEM_SEL_UNDEFINED}, L"ATA controller error"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_NOTSUPPORTED,             {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA SMART nonsupport"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_DISABLED,                 {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"ATA SMART disabled!"},
  //
  // PCI
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_RES_ALLOC,                                  {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x07, OEM_SEL_UNDEFINED}, L"PCI resource configuration"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | OEM_IOB_PCI_PC_OPTION_ROM_INIT,                       {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x08, OEM_SEL_UNDEFINED}, L"Option ROM initialization"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,                         {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x00, OEM_SEL_UNDEFINED}, L"PCI resource conflict"},
  //
  // Keyboard
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST,                   {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x17, OEM_SEL_UNDEFINED}, L"Keyboard test"},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR,                     {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x07, OEM_SEL_UNDEFINED}, L"Keyboard controller error"},
  //
  // USB
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_RESET,                                       {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x06, OEM_SEL_UNDEFINED}, L"USB resource configuration"},
  //
  // CPU
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT,                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x03, OEM_SEL_UNDEFINED}, L"Secondary processor(s) initialization"},  
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN,                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x19, OEM_SEL_UNDEFINED}, L"Primary processor initialization"},  
  //
  //  Software
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_PC_USER_SETUP,                    {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x05, OEM_SEL_UNDEFINED}, L"User-initiated system setup"},    
  //
  // VGA
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_INIT,                            {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x09, OEM_SEL_UNDEFINED}, L"Local Console initialization"},  
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_DETECTED,                        {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x02, 0x0A, OEM_SEL_UNDEFINED}, L"Local Console Detected"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NON_SPECIFIC,                  {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device non specific"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_DISABLED,                      {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device disabled"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_SUPPORTED,                 {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device not supported"},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED,                    {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device controller detected"}, 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_CONFIGURED,                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device not configured"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INTERFACE_ERROR,               {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device interface error"},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,                {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x09, OEM_SEL_UNDEFINED}, L"Local Console device controller error"} 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INPUT_ERROR,                   {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device input error"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,                  {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device ourtput error"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,             {0x0F,OEM_SEL_SENSOR_NUM, OEM_SEL_EVENT_TYPE}, {0x00, 0x0A, OEM_SEL_UNDEFINED}, L"No Local Console device resource conflict"},  

  };

OEM_POST_MESSAGE_STRING                    gOemPostMessageStr[] = {

  //
  // Memory
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,                      L"Memory initialization"},  
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,             L"No system memory is physically installed in the system"},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,               L"No usable system memory"},
  //
  // SMBUS
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,                                     L"SM Bus initialization"},  
  //
  // Legacy Boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT,        L"Starting operating system boot process"},  
  //
  // UEFI boot
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_PC_LOAD,                            L"Starting operating system boot process"},
  //
  // ATA/IDE, ~\Bus\Pci\IdeBus\Dxe\idebus.inf
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_INIT,                                 L"ATA Initialization"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_RESET,                                L"ATA initialization"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_PRESENCE_DETECT,                        L"To detect ATA drive"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_ENABLE,                                 L"ATA device eanbled!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_P_PC_DISABLE,                                L"ATA device disable!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_ENABLE,                    L"ATA SMART enabled!"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_UNDERTHRESHOLD,            L"The threshold exceeded condition is not detected by the ATA device"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD,             L"The threshold exceeded condition is detected by the ATA device"},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR,                      L"ATA device error"},
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR,                     L"ATA controller error"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_NOTSUPPORTED,              L"ATA SMART nonsupport"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_DISABLED,                  L"ATA SMART disabled!"},
  //
  // PCI
  //  
  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | EFI_IOB_PCI_RES_ALLOC,                                 L"PCI resource configuration"},
//  {EFI_PROGRESS_CODE, EFI_IO_BUS_PCI | OEM_IOB_PCI_PC_OPTION_ROM_INIT,                        L"Option ROM initialization"},
//  {EFI_ERROR_CODE,    EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,                          L"PCI resource conflict"},
  //
  // Keyboard
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST,                  L"Keyboard test"},
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR,                    L"Keyboard controller error"},
  //
  // USB
  //
  {EFI_PROGRESS_CODE, EFI_IO_BUS_USB | EFI_IOB_PC_RESET,                                      L"USB resource configuration"},
  //
  // CPU
  //
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT,               L"Secondary processor(s) initialization"},  
  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN,               L"Primary processor initialization"},  
  //
  //  Software
  //
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_PC_USER_SETUP,                   L"User-initiated system setup"},    
  //
  // VGA
  //
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_INIT,                           L"Local Console initialization"},  
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_DETECTED,                       L"Local Console Detected"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NON_SPECIFIC,                   L"No Local Console device non specific"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_DISABLED,                       L"No Local Console device disabled"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_SUPPORTED,                  L"No Local Console device not supported"},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED,                   L"No Local Console device controller detected"}, 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_CONFIGURED,                 L"No Local Console device not configured"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INTERFACE_ERROR,                L"No Local Console device interface error"},  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,               L"Local Console device controller error"} 
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_INPUT_ERROR,                    L"No Local Console device input error"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,                   L"No Local Console device ourtput error"},  
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,              L"No Local Console device resource conflict"},  

  };

OEM_POST_MESSAGE_BEEP                gOemPostMessageBeep[] = {
  // VGA
  {EFI_PROGRESS_CODE, EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_PC_DETECTED,                   1,  { 250000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //single short
  
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_NOT_DETECTED,               1,  { 250000,        250000, 
                                                                                               1000000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //a short and a long beep

  // Keyboard
  {EFI_ERROR_CODE,    EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR,                1,  { 250000,        250000, 
                                                                                                1000000,      1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //a short and a long beep
  // ATA/IDE
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_INTERFACE_ERROR,                  1,  {1000000,        750000, 
                                                                                                250000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //a long beep a short and
  // ATA/IDE
  {EFI_ERROR_CODE,    EFI_IO_BUS_ATA_ATAPI | EFI_IOB_EC_CONTROLLER_ERROR,                 1,  {1000000,        750000, 
                                                                                                250000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //a long beep a short and 

  // Legacy Boot
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT,    3,  { 250000,        250000, 
                                                                                                250000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}, //two short
  // UEFI Boot
  {EFI_PROGRESS_CODE, EFI_SOFTWARE_EFI_OS_LOADER | EFI_SW_PC_LOAD,                         3, { 250000,        250000, 
                                                                                                250000,       1500000, 
                                                                                                   0x0,           0x0, 
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0,
                                                                                                   0x0,           0x0}}  //two short
  };

UINTN                                 gPostMessageListSize    = (sizeof(gPostMessageList)/sizeof(POST_MESSAGE_LIST));
UINTN                                 gOemPostMessageStrSize  = (sizeof(gOemPostMessageStr)/sizeof(OEM_POST_MESSAGE_STRING));
UINTN                                 gOemPostMessageLogSize  = (sizeof(gOemPostMessageLog)/sizeof(OEM_POST_MESSAGE_LOG));
UINTN                                 gOemPostMessageBeepSize = (sizeof(gOemPostMessageBeep)/sizeof(OEM_POST_MESSAGE_BEEP));
