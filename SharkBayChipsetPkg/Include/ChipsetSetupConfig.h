/** @file
  Chipset Setup Configuration Definitions

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SETUP_CONFIG_H_
#define _CHIPSET_SETUP_CONFIG_H_
#ifndef VFRCOMPILE
#include <Uefi/UefiInternalFormRepresentation.h>
#endif
#include <KernelSetupConfig.h>

#define FORMSET_ID_GUID_ADVANCE   {0xc6d4769e, 0x7f48, 0x4d2a, 0x98, 0xe9, 0x87, 0xad, 0xcc, 0xf3, 0x5c, 0xcc}
#define FORMSET_ID_GUID_POWER     {0xa6712873, 0x925f, 0x46c6, 0x90, 0xb4, 0xa4, 0x0f, 0x86, 0xa0, 0x91, 0x7b}

#define DVMT_PREALLOCATE_LABEL                  0x1060
#define ICC_PROFILE_LABEL                       0x1080

#define KEY_TXT                                 0x20B4
#define KEY_SCAN_ESC                            0xF0D1
#define KEY_SCAN_F9                             0xF0D2
#define KEY_SCAN_F10                            0xF0D3

//
// Setup Formset label
//
#define IDE_UPDATE_LABEL                        0x1005
#define AZALIA_LABEL                            0x1006
#define RATIO_LABEL                             0x1050
#define CORE_RATIO_LIMIT_LABEL                  0x1051
#define SYSTEM_HEALTH_LABEL                     0x1013
#define ICC_CLOCK_GEN1_LABEL                    0x1cc1
#define ICC_CLOCK_GEN2_LABEL                    0x1cc2
#define ICC_CLOCK_GEN3_LABEL                    0x1cc3
#define ICC_CLOCK_GEN4_LABEL                    0x1cc4
#define ICC_CLOCK_GEN5_LABEL                    0x1cc5
#define ICC_CLOCK_GEN6_LABEL                    0x1cc6
#define DUAL_VGA_SUPPORT_START_LABEL            0x1cfe
#define DUAL_VGA_SUPPORT_END_LABEL              0x1cff
#define PLUG_IN_DISPLAY_SELECTION_START_LABEL   0x1d00
#define PLUG_IN_DISPLAY_SELECTION_END_LABEL     0x1d01
#define IGD_DISPLAY_SELECTION_START_LABEL       0x1d02
#define IGD_DISPLAY_SELECTION_END_LABEL         0x1d03
#define APCI_DEBUG_ADDRESS_LABEL                0x1052
#define KEY_SERIAL_PORTA                        0x2030
#define KEY_SERIAL_PORTA_BASE_IO                0x2031
#define KEY_SERIAL_PORTA_INTERRUPT              0x2032
#define KEY_SERIAL_PORTB                        0x2033
#define KEY_SERIAL_PORTB_BASE_IO                0x2034
#define KEY_SERIAL_PORTB_INTERRUPT              0x2035
#define KEY_PCI_SLOT3_IRQ_SET                   0x2040
#define KEY_PCI_SLOT4_IRQ_SET                   0x2041
#define KEY_PEG_FORCE_X1                        0x2050
#define KEY_PCIE_COMPLIANCE_MODE                0x2051
#define KEY_CHIPSET_EXTENDED_CONFIG             0x2060
#define KEY_CHIPSET_SDRAM_TIME_CTRL             0x2061
#define KEY_SETUP_REFRESH                       0x2062
#define KEY_PCI_IRQ_SET                         0x2063
#define KEY_VT_CONFIG                           0x2064
#define KEY_AHCI_OPROM_CONFIG                   0x2065
#define KEY_SATA_CNFIGURE_MODE                  0x2090
#define KEY_DUAL_VGA_SUPPORT                    0x2066
#define KEY_PLUG_IN_DISPLAY_SELECTION1      0x21d0
#define KEY_PLUG_IN_DISPLAY_SELECTION2      0x21d1
#define KEY_IGD_PRIMARY_DISPLAY_SELECTION   0x21e0
#define KEY_IGD_SECONDARY_DISPLAY_SELECTION 0x21e1
#define KEY_XTU_ITEM                            0x500
#define KEY_XTU_ITEM_01                         0x501
#define KEY_XTU_ITEM_02                         0x502
#define KEY_XTU_ITEM_03                         0x503
#define KEY_XTU_ITEM_04                         0x504
#define KEY_XTU_ITEM_05                         0x505
#define KEY_XTU_ITEM_06                         0x506
#define KEY_XTU_ITEM_07                         0x507
#define KEY_XTU_ITEM_08                         0x508
#define KEY_XTU_ITEM_09                         0x509
#define KEY_XTU_ITEM_0A                         0x50A
#define KEY_XTU_ITEM_0B                         0x50B
#define KEY_XTU_ITEM_0C                         0x50C
#define KEY_XTU_ITEM_0D                         0x50D
#define KEY_XTU_ITEM_0E                         0x50E
#define KEY_XTU_TURN_ON                         0x520
#define FW_IMAGE_TYPE_1_5MB                     3
#define FW_IMAGE_TYPE_5MB                       4

#define KEY_DVMT_PREALLOCATE                    0xA00
#define KEY_GTT                                 0xA01
#define KEY_IGD_CONTROL                         0xA02
#define KEY_PRIMARY_DISPLAY                     0xA03
#define KEY_APERTURE_SIZE                       0xA04
#define KEY_RAPID_START_ENABLE                  0x550
#define KEY_WAKE_ON_PME                         0x402
#define KEY_ISCT_ENABLE                         0x950
#define KEY_FULL_RESET                          0xCF9
#define KEY_PEG_FULL_RESET_0                    0xA05
#define KEY_PEG_FULL_RESET_1                    0xA06
#define KEY_PEG_FULL_RESET_2                    0xA07
#define KEY_PEG_FULL_RESET_3                    0xA08
#define KEY_COREACTIVE_FULL_RESET               0xA09
#define KEY_LAN_SELECT                          0xA10

#define KEY_ICC_CLOCK_1                         0x1cc1
#define KEY_ICC_CLOCK_2                         0x1cc2
#define KEY_ICC_CLOCK_3                         0x1cc3
#define KEY_ICC_CLOCK_4                         0x1cc4
#define KEY_ICC_CLOCK_5                         0x1cc5
#define KEY_ICC_CLOCK_6                         0x1cc6
#define KEY_ICC_APPLY                           0x1cca

#define KEY_DPTF                                0x3610
#define KEY_XHCI_PREBOOT_SUPPORT                0x600
#define KEY_IFR_UPDATE                          0x610
#define KEY_DDR3LV_OPTION                       0x620
#define KEY_MAX_TOLUD_OPTION                    0x630
//[-start-130207-IB02950497-add]//
#define KEY_XHCI_MANUAL_OPTION                  0x640
//[-end-130207-IB02950497-add]//
//[-start-130403-IB02950501-add]//
#define KEY_XHCI_PB_OPTION                      0x641
#define KEY_XHCI_OPTION                         0x642
#define KEY_USBRS_OPTION                        0x643
#define KEY_EHCI1_OPTION                        0x644
#define KEY_EHCI2_OPTION                        0x645
//[-end-130403-IB02950501-add]//
#define IDE_MODE                                0
#define AHCI_MODE                               1
#define RAID_MODE                               2
#define KEY_PCI_EXPRESS_CONFIGURATION_CHANGE    0x61

//[-start-130131-IB03780481-add]//
#define KEY_TBT_SECURITY_OPTION                 0x650
#define KEY_TBT_SX_OPTION                       0x651
//[-end-130131-IB03780481-add]//

//
// Form ID
//
#define SERIAL_ATA_PORT0_FORM_ID                0x230
#define SERIAL_ATA_PORT1_FORM_ID                0x231
#define SERIAL_ATA_PORT2_FORM_ID                0x232
#define SERIAL_ATA_PORT3_FORM_ID                0x233
#define SERIAL_ATA_PORT4_FORM_ID                0x234
#define SERIAL_ATA_PORT5_FORM_ID                0x235
#define SERIAL_ATA_PORT6_FORM_ID                0x236
#define SERIAL_ATA_PORT7_FORM_ID                0x237
#define HDD_SOFTWARE_FEATURE_MASK_FORM_ID       0x1c20
#define EXTENDED_ICC_FORM_ID                    0x1cc0
#define ICC_CLOCK_GEN1_FORM_ID                  0x1cc1
#define ICC_CLOCK_GEN2_FORM_ID                  0x1cc2
#define ICC_CLOCK_GEN3_FORM_ID                  0x1cc3
#define ICC_CLOCK_GEN4_FORM_ID                  0x1cc4
#define ICC_CLOCK_GEN5_FORM_ID                  0x1cc5
#define ICC_CLOCK_GEN6_FORM_ID                  0x1cc6
#define ISCT_FORM_ID                            0x1E00
#define HARDWARE_MONITOR_FORM_ID                0x245
#define TEMPERATURE_LABEL                       0x2450
#define VOLTAGE_LABEL                           0x2451
#define FAN_SPEED_LABEL                         0x2452

#define HW_MONITOR_CALLBACK_ID                  0x2423
#define DPTF_FORM_ID                            0x3610
#define DPTF_POLICCY_FORM_ID                    0x3611
#define SERIAL_IO_FORM_ID                            0x3700
#define ADSP_FORM_ID                            0x3710
#define KEY_DSP_CHANGE                          0x3701
#define KEY_AZALIA_CHANGE                       0x3702

#define MAX_PCI_EXPRESS_ROOT_PORTS              6
#define MAX_PCI_ROOT_BRIDGES                    1

//
// Express Card Support
//
#define EXPRESS_CARD_ROOT_PORT_BUS              0x0
#define EXPRESS_CARD_ROOT_PORT_DEV              PCI_DEVICE_NUMBER_ICH_PCIEXP
#define EXPRESS_CARD_ROOT_PORT_FUN              PCI_FUNCTION_NUMBER_ICH_PCIEXP2

//
// Local Flat Panel Backlight Control Mode , refer to VBIOS Int15 sub:5F49h
//
#define BKLT_SEL_PWM_INVERTED                   0
#define BKLT_SEL_PWM_NORMAL                     2
#define BKLT_SEL_GMBUS_INVERTED                 1
#define BKLT_SEL_GMBUS_NORMAL                   3

#define ICC_SOFT_STRAP_SUPPORT                  0
#define ICC_BIOS_SUPPORT                        1
#define ICC_LOCK_ALL_REG                        0
#define ICC_LOCK_STATIC_REG                     1
#define ICC_LOCK_ALL_UNLOCKED_REG               2

#define B_C1_AUTO_DEMOTION                      BIT0
#define B_C3_AUTO_DEMOTION                      BIT1

#define B_C1_UNDEMOTION                         BIT0
#define B_C3_UNDEMOTION                         BIT1

#define B_TS_ON_DIMM_SLOT0                      BIT0
#define B_TS_ON_DIMM_SLOT1                      BIT1

#define DUAL_VGA_CONTROLLER_ENABLE              1
#define DUAL_VGA_CONTROLLER_DISABLE             0
//
// DisplayMode
//
#define DISPLAY_MODE_IGPU               0x0
#define DISPLAY_MODE_DGPU               0x1
#define DISPLAY_MODE_PCI                0x2
#define DISPLAY_MODE_AUTO               0x3
#define DISPLAY_MODE_HYBRID             0x4
#define DUAL_VGA_CONTROLLER_ENABLE      1
#define DUAL_VGA_CONTROLLER_DISABLE     0
#define SCU_IGD_BOOT_TYPE_DISABLE       0x00
#define SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT 0x00
#define SCU_IGD_BOOT_TYPE_CRT           0x01
#define SCU_IGD_BOOT_TYPE_LFP           0x08
#define SCU_IGD_BOOT_TYPE_EFP           0x04
#define SCU_IGD_BOOT_TYPE_EFP3          0x20
#define SCU_IGD_BOOT_TYPE_EFP2          0x40
#define SCU_IGD_BOOT_TYPE_LFP2          0x80


#define IGD_ENABLE                              1
#define IGD_DISABLE                             0
#define IGD_AUTO                                2
#define ALWAYS_ENABLE_PEG                       1
#define DONT_ALWAYS_ENABLE_PEG                  1
#define SG_ENABLE                               1
#define SG_DISABLE                              0
#define MUXED_FIXED                             1
#define MUXLESS_FIXED                           5
#define MUXLESS_DYNAMIC                         6
#define MUXLESS_FIXED_DYNAMIC                   7
#define SCU_IGD_BOOT_TYPE_DISABLE               0x00
#define SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT         0x00
#define SCU_IGD_BOOT_TYPE_CRT                   0x01
#define SCU_IGD_BOOT_TYPE_LFP                   0x08
#define SCU_IGD_BOOT_TYPE_EFP                   0x04
#define SCU_IGD_BOOT_TYPE_EFP3                  0x20
#define SCU_IGD_BOOT_TYPE_EFP2                  0x40
#define SCU_IGD_BOOT_TYPE_LFP2                  0x80
//
// The order is for SCU 
//
#define SCU_IGD_INDEX_DISABLE           0x00
#define SCU_IGD_INDEX_VBIOS_DEFAULT     0x00
#define SCU_IGD_INDEX_CRT               0x01
#define SCU_IGD_INDEX_LFP               0x02
#define SCU_IGD_INDEX_EFP               0x03
#define SCU_IGD_INDEX_EFP2              0x04
#define SCU_IGD_INDEX_EFP3              0x05
#define SCU_IGD_INDEX_LFP2              0x06
#define FAST_RECLAIM_COUNT                      35
#define CONFIGURATION_VARSTORE_ID               0x1234

#define MAX_HII_HANDLES                         0x10

//
// Console Redirection Text Mode Resolution (CRTextModeResolution?
//
#define CR_TEXT_MODE_AUTO               0
#define CR_TEXT_MODE_80X25              1
#define CR_TEXT_MODE_80X24DF            2
#define CR_TEXT_MODE_80X24DL            3

#define TPM2_DISABLE           0
#define TPM2_ENABLE            1

//[-start-130611-IB11120016-add]//
//
// IPMI definition
//
#define IPMI_CONFIG_FORM_ID                     0x2D00
#define BMC_CONFIG_FORM_ID                      0x2D01
#define IPMI_SDR_LIST_FORM_ID                   0x2D02

#define IPMI_CONFIG_LABEL                       0x2D00
#define IPMI_SDR_LIST_LABEL                     0x2D01
#define IPMI_SDR_LIST_END_LABEL                 0x2D02

#define KEY_IPMI_CONFIG                         0x2D80
#define KEY_EXECUTE_IPMI_UTILITY                0x2D81
#define KEY_SET_BMC_LAN_CHANNEL_NUM             0x2D82
#define KEY_SET_BMC_IPV4_IP_ADDRESS             0x2D83
#define KEY_SET_BMC_IPV4_SUBNET_MASK            0x2D84
#define KEY_SET_BMC_IPV4_GATEWAY_ADDRESS        0x2D85
#define KEY_SET_BMC_IPV6_IP_ADDRESS             0x2D86
#define KEY_SET_BMC_IPV6_GATEWAY_ADDRESS        0x2D87
#define KEY_IPMI_SDR_LIST_SUPPORT               0x2D88
//[-end-130611-IB11120016-add]//

//[-start-130617-IB04560405-add]//
//[-start-131122-IB08400219-modify]//
//
// Event Log
//
#define VFR_FORMID_BIOS_EVENT_LOG       0x2F
//[-start-140318-IB08400253-modify]//
#ifdef H2O_POST_MESSAGE
#define VFR_FORMID_POST_MESSAGE         0x31
#endif
//[-end-140318-IB08400253-modify]//

#define KEY_SHOW_EVENT_LOG_MODE         0x0901
#define KEY_CLEAR_BIOS_EVENT_LOG        0x0902
#define KEY_EVENT_LOG_VIEWER            0x0903
#define KEY_BIOS_LOG_EVENT_TO           0x0904
#define KEY_BIOS_EVENT_LOG_FULL_OPTION  0x0905
//[-end-131122-IB08400219-modify]//
//[-end-130617-IB04560405-add]//

//[-start-131225-IB08620313-add]//
#ifdef APEI_SUPPORT
#define APEI_FORM_ID                    0x2C
#endif
//[-end-131225-IB08620313-add]//

#pragma pack(1)

//
//  Setup Utility Structure
//
//-----------------------------------------------------------------------------------------------------------------
// Important!!! The following setup utility structure should be syncronize with OperationRegion MBOX in mailbox.asi.
// If you do NOT follow it, you may face on unexpected issue. The total size are 1200bytes.
// (Common 146bytes + Kernel 84bytes + Chipset 300bytes + OEM 70bytes + ODM 100bytes + Platform 500bytes)
//-----------------------------------------------------------------------------------------------------------------

//[-start-130709-IB05160465-modify]//
typedef struct {
  //
  // Kernel system configuration (offset 0~229, total 230 bytes)
  //
  #define _IMPORT_KERNEL_SETUP_
  #include <KernelSetupData.h>
  #undef _IMPORT_KERNEL_SETUP_

  //
  // Chipset system configuration (offset 230~529, total 300 bytes)
  //
  #define _IMPORT_CHIPSET_COMMON_SETUP_
  #include <ChipsetCommonSetupData.h>
  #undef _IMPORT_CHIPSET_COMMON_SETUP_

  //
  // Chipset system configuration (offset 530~1099, total 570 bytes)
  //
  #define _IMPORT_CHIPSET_SPECIFIC_SETUP_
  #include <ChipsetSpecificSetupData.h>
  #undef _IMPORT_CHIPSET_SPECIFIC_SETUP_

} CHIPSET_CONFIGURATION;
//[-end-130709-IB05160465-modify]//
#pragma pack()

#endif

