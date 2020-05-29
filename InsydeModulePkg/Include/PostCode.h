/** @file
  Post code definitions

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

#ifndef _POSTCODE_H_
#define _POSTCODE_H_

#include <Library/PostCodeLib.h>

//
// Use POST_CODE(Value) to display post code
//

//
// SEC Functionality
//
#define SEC_SYSTEM_POWER_ON                   0x01        // CPU power on and switch to Protected mode
#define SEC_BEFORE_MICROCODE_PATCH            0x02        // Patching CPU microcode
#define SEC_AFTER_MICROCODE_PATCH             0x03        // Setup Cache as RAM
#define SEC_ACCESS_CSR                        0x04        // PCIE MMIO Base Address initial
#define SEC_GENERIC_MSRINIT                   0x05        // CPU Generic MSR initial
#define SEC_CPU_SPEEDCFG                      0x06        // Setup CPU speed
#define SEC_SETUP_CAR_OK                      0x07        // Cache as RAM test
#define SEC_FORCE_MAX_RATIO                   0x08        // Tune CPU frequency ratio to maximum level
#define SEC_GO_TO_SECSTARTUP                  0x09        // Setup BIOS ROM cache
#define SEC_GO_TO_PEICORE                     0x0A        // Enter Boot Firmware Volume

//
// PEI Functionality
//
#define PEI_SIO_INIT                          0x70        // Super I/O initial
#define PEI_CPU_REG_INIT                      0x71        // CPU Early Initial
#define PEI_CPU_AP_INIT                       0x72        // Multi-processor Early initial
#define PEI_CPU_HT_RESET                      0x73        // HyperTransport initial
#define PEI_PCIE_MMIO_INIT                    0x74        // PCIE MMIO BAR Initial
#define PEI_NB_REG_INIT                       0x75        // North Bridge Early Initial
#define PEI_SB_REG_INIT                       0x76        // South Bridge Early Initial
#define PEI_PCIE_TRAINING                     0x77        // PCIE Training
#define PEI_TPM_INIT                          0x78        // TPM Initial
#define PEI_SMBUS_INIT                        0x79        // SMBUS Early Initial
#define PEI_PROGRAM_CLOCK_GEN                 0x7A        // Clock Generator Initial
#define PEI_IGD_EARLY_INITIAL                 0x7B        // Internal Graphic device early initial, PEI_IGDOpRegion
#define PEI_HECI_INIT                         0x7C        // HECI Initial
#define PEI_WATCHDOG_INIT                     0x7D        // Watchdog timer initial
#define PEI_MEMORY_INIT                       0x7E        // Memory Initial for Normal boot.
#define PEI_MEMORY_INIT_FOR_CRISIS            0x7F        // Memory Initial for Crisis Recovery
#define PEI_MEMORY_INSTALL                    0x80        // Simple Memory test
#define PEI_TXTPEI                            0x81        // TXT function early initial
#define PEI_SWITCH_STACK                      0x82        // Start to use Memory
#define PEI_MEMORY_CALLBACK                   0x83        // Set cache for physical memory
#define PEI_ENTER_RECOVERY_MODE               0x84        // Recovery device initial
#define PEI_RECOVERY_MEDIA_FOUND              0x85        // Found Recovery image
#define PEI_RECOVERY_MEDIA_NOT_FOUND          0x86        // Recovery image not found
#define PEI_RECOVERY_LOAD_FILE_DONE           0x87        // Load Recovery Image complete
#define PEI_RECOVERY_START_FLASH              0x88        // Start Flash BIOS with Recovery image
#define PEI_ENTER_DXEIPL                      0x89        // Loading BIOS image to RAM
#define PEI_FINDING_DXE_CORE                  0x8A        // Loading DXE core
#define PEI_GO_TO_DXE_CORE                    0x8B        // Enter DXE core
#define PEI_IFFS_TRANSITION_START             0x8C        // iFFS Transition Start
#define PEI_IFFS_TRANSITION_END               0x8D        // iFFS Transition End

//
// DXE Functionality
//
#define DXE_TCGDXE                            0x40        // TPM initial in DXE
#define DXE_SB_SPI_INIT                       0x41        // South bridge SPI initial
#define DXE_CF9_RESET                         0x42        // Setup Reset service, DXE_CF9Reset
#define DXE_SB_SERIAL_GPIO_INIT               0x43        // South bridge Serial GPIO initial, DXE_SB_SerialGPIO_INIT
#define DXE_SMMACCESS                         0x44        // Setup SMM ACCESS service
#define DXE_NB_INIT                           0x45        // North bridge Middle initial
#define DXE_SIO_INIT                          0x46        // Super I/O DXE initial
#define DXE_LEGACY_REGION                      0x47        // Setup Legacy Region service, DXE_LegacyRegion
#define DXE_SB_INIT                           0x48        // South Bridge Middle Initial
#define DXE_IDENTIFY_FLASH_DEVICE             0x49        // Identify Flash device
#define DXE_FTW_INIT                          0x4A        // Fault Tolerant Write verification
#define DXE_VARIABLE_INIT                     0x4B        // Variable Service Initial
#define DXE_VARIABLE_INIT_FAIL                0x4C        // Fail to initial Variable Service
#define DXE_MTC_INIT                          0x4D        // MTC Initial
#define DXE_CPU_INIT                          0x4E        // CPU Middle Initial
#define DXE_MP_CPU_INIT                       0x4F        // Multi-processor Middle Initial
#define DXE_SMBUS_INIT                        0x50        // SMBUS Driver Initial
#define DXE_SMART_TIMER_INIT                  0x51        // 8259 Initial
#define DXE_PCRTC_INIT                        0x52        // RTC Initial
#define DXE_SATA_INIT                         0x53        // SATA Controller early initial
#define DXE_SMM_CONTROLER_INIT                0x54        // Setup SMM Control service, DXE_SMMControler_INIT
#define DXE_LEGACY_INTERRUPT                  0x55        // Setup Legacy Interruput service, DXE_LegacyInterrupt
#define DXE_RELOCATE_SMBASE                   0x56        // Relocate SMM BASE
#define DXE_FIRST_SMI                         0x57        // SMI test
#define DXE_VTD_INIT                          0x58        // VTD Initial
#define DXE_BEFORE_CSM16_INIT                 0x59        // Legacy BIOS initial
#define DXE_AFTER_CSM16_INIT                  0x5A        // Legacy interrupt function initial
#define DXE_LOAD_ACPI_TABLE                   0x5B        // ACPI Table Initial
#define DXE_SB_DISPATCH                       0x5C        // Setup SB SMM Dispatcher service, DXE_SB_Dispatch
#define DXE_SB_IOTRAP_INIT                    0x5D        // Setup SB IOTRAP Service
#define DXE_SUBCLASS_DRIVER                   0x5E        // Build AMT Table
#define DXE_PPM_INIT                          0x5F        // PPM Initial
#define DXE_HECIDRV_INIT                      0x60        // HECIDRV Initial
#define DXE_VARIABLE_RECLAIM                  0x61        // Variable store garbage collection and reclaim operation
#define DXE_FLASH_PART_NONSUPPORT             0x62        // Do not support flash part (which is defined in SpiDevice.c)
//
// BDS Functionality
//
#define BDS_ENTER_BDS                         0x10        // Enter BDS entry
#define BDS_INSTALL_HOTKEY                    0x11        // Install Hotkey service
#define BDS_ASF_INIT                          0x12        // ASF Initial
#define BDS_PCI_ENUMERATION_START             0x13        // PCI enumeration
#define BDS_BEFORE_PCIIO_INSTALL              0x14        // PCI resource assign complete
#define BDS_PCI_ENUMERATION_END               0x15        // PCI enumeration complete
#define BDS_CONNECT_CONSOLE_IN                0x16        // Keyboard Controller, Keyboard and Moust initial
#define BDS_CONNECT_CONSOLE_OUT               0x17        // Video device initial
#define BDS_CONNECT_STD_ERR                   0x18        // Error report device initial
#define BDS_CONNECT_USB_HC                    0x19        // USB host controller initial
#define BDS_CONNECT_USB_BUS                   0x1A        // USB BUS driver initial
#define BDS_CONNECT_USB_DEVICE                0x1B        // USB device driver initial
#define BDS_NO_CONSOLE_ACTION                 0x1C        // Console device initial fail
#define BDS_DISPLAY_LOGO_SYSTEM_INFO          0x1D        // Display logo or system information
#define BDS_START_IDE_CONTROLLER              0x1E        // IDE controller initial
#define BDS_START_SATA_CONTROLLER             0x1F        // SATA controller initial
#define BDS_START_ISA_ACPI_CONTROLLER         0x20        // SIO controller initial
#define BDS_START_ISA_BUS                     0x21        // ISA BUS driver initial
#define BDS_START_ISA_FDD                     0x22        // Floppy device initial
#define BDS_START_ISA_SEIRAL                  0x23        // Serial device initial
#define BDS_START_IDE_BUS                     0x24        // IDE device initial
#define BDS_START_AHCI_BUS                    0x25        // AHCI device initial
#define BDS_CONNECT_LEGACY_ROM                0x26        // Dispatch option ROMs
#define BDS_ENUMERATE_ALL_BOOT_OPTION         0x27        // Get boot device information
#define BDS_END_OF_BOOT_SELECTION             0x28        // End of boot selection
#define BDS_ENTER_SETUP                       0x29        // Enter Setup Menu
#define BDS_ENTER_BOOT_MANAGER                0x2A        // Enter Boot manager
#define BDS_BOOT_DEVICE_SELECT                0x2B        // Try to boot system to OS
#define BDS_EFI64_SHADOW_ALL_LEGACY_ROM       0x2C        // Shadow Misc Option ROM
#define BDS_ACPI_S3SAVE                       0x2D        // Save S3 resume required data in RAM
#define BDS_READY_TO_BOOT_EVENT               0x2E        // Last Chipset initial before boot to OS
#define BDS_GO_LEGACY_BOOT                    0x2F        // Start to boot Legacy OS
#define BDS_GO_UEFI_BOOT                      0x30        // Start to boot UEFI OS
#define BDS_LEGACY16_PREPARE_TO_BOOT          0x31        // Prepare to Boot to Legacy OS
#define BDS_EXIT_BOOT_SERVICES                0x32        // Send END of POST Message to ME via HECI
#define BDS_LEGACY_BOOT_EVENT                 0x33        // Last Chipset initial before boot to Legacy OS.
#define BDS_ENTER_LEGACY_16_BOOT              0x34        // Ready to Boot Legacy OS.
#define BDS_RECOVERY_START_FLASH              0x35        // Fast recovery start flash
#define BDS_START_SDHC_BUS                    0x36        // SDHC device initial
#define BDS_CONNECT_ATA_LEGACY                0x37        // Ata Legacy device initial
#define BDS_CONNECT_SD_LEGACY                 0x38        // SD Legacy device initial

//
// PostBDS Functionality
//
#define POST_BDS_NO_BOOT_DEVICE                0xF9        // No Boot Device, PostBDS_NO_BOOT_DEVICE

#define POST_BDS_START_IMAGE                   0xFB        // UEFI Boot Start Image, PostBDS_START_IMAGE

#define POST_BDS_ENTER_INT19                   0xFD        // Legacy 16 boot entry
#define POST_BDS_JUMP_BOOT_SECTOR              0xFE        // Try to Boot with INT 19

//
// SMM Functionality
//
#define SMM_IDENTIFY_FLASH_DEVICE             0xA0        // Identify Flash device in SMM
#define SMM_SMM_PLATFORM_INIT                 0xA2        // SMM service initial
#define SMM_ACPI_ENABLE_START                 0xA6        // OS call ACPI enable function
#define SMM_ACPI_ENABLE_END                   0xA7        // ACPI enable function complete
#define SMM_S1_SLEEP_CALLBACK                 0xA1        // Enter S1
#define SMM_S3_SLEEP_CALLBACK                 0xA3        // Enter S3
#define SMM_S4_SLEEP_CALLBACK                 0xA4        // Enter S4
#define SMM_S5_SLEEP_CALLBACK                 0xA5        // Enter S5
#define SMM_ACPI_DISABLE_START                0xA8        // OS call ACPI disable function
#define SMM_ACPI_DISABLE_END                  0xA9        // ACPI disable function complete

//
// S3 Functionality
//
#define S3_RESTORE_MEMORY_CONTROLLER          0xC0        // Memory initial for S3 resume
#define S3_INSTALL_S3_MEMORY                  0xC1        // Get S3 resume required data from memory
#define S3_SWITCH_STACK                       0xC2        // Start to use memory during S3 resume
#define S3_MEMORY_CALLBACK                    0xC3        // Set cache for physical memory during S3 resume
#define S3_ENTER_S3_RESUME_PEIM               0xC4        // Start to restore system configuration
#define S3_BEFORE_ACPI_BOOT_SCRIPT            0xC5        // Restore system configuration stage 1
#define S3_BEFORE_RUNTIME_BOOT_SCRIPT         0xC6        // Restore system configuration stage 2
#define S3_BEFORE_RELOCATE_SMM_BASE           0xC7        // Relocate SMM BASE during S3 resume
#define S3_BEFORE_MP_INIT                     0xC8        // Multi-processor initial during S3 resume
#define S3_BEFORE_RESTORE_ACPI_CALLBACK       0xC9        // Start to restore system configuration in SMM
#define S3_AFTER_RESTORE_ACPI_CALLBACK        0xCA        // Restore system configuration in SMM complete
#define S3_GO_TO_FACS_WAKING_VECTOR           0xCB        // Back to OS

//
// ASL Functionality
//
#define ASL_ENTER_S1                          0x51        // Prepare to enter S1
#define ASL_ENTER_S3                          0x53        // Prepare to enter S3
#define ASL_ENTER_S4                          0x54        // Prepare to enter S4
#define ASL_ENTER_S5                          0x55        // Prepare to enter S5
#define ASL_WAKEUP_S1                         0xE1        // System wakeup from S1
#define ASL_WAKEUP_S3                         0xE3        // System wakeup from S3
#define ASL_WAKEUP_S4                         0xE4        // System wakeup from S4
#define ASL_WAKEUP_S5                         0xE5        // System wakeup from S5

#endif