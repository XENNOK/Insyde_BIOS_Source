## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

!import MdePkg/Package.dsc
!import MdeModulePkg/Package.dsc
!import UefiCpuPkg/Package.dsc
!import PerformancePkg/Package.dsc
!import CryptoPkg/Package.dsc
!import SecurityPkg/Package.dsc
!import PcAtChipsetPkg/Package.dsc
!import IA32FamilyCpuPkg/Package.dsc
!import IntelFrameworkModulePkg/Package.dsc
!import EdkCompatibilityPkg/Package.dsc
#[-start-131202-IB09740048-add]#
!import InsydeCrPkg/Package.dsc
#[-end-131202-IB09740048-add]#
!import InsydeOemServicesPkg/Package.dsc
#!import SioDummyPkg/Package.dsc
!import SioNct6683dluPkg/Package.dsc
!import InsydeModulePkg/Package.dsc
#[start-130916-IB05670200-add]#
!import InsydeFlashDevicePkg/Package.dsc
#[end-130916-IB05670200-add]#
!import InsydeFrameworkModulePkg/Package.dsc
!import SharkBayRcPkg/Package.dsc
!import SharkBayChipsetPkg/Package.dsc
#[-start-130625-IB11120016-add]#
!import InsydeIpmiPkg/Package.dsc
#[-end-130625-IB11120016-add]#
#[-start-131225-IB08620313-add]#
!import InsydeApeiPkg/Package.dsc
#[-end-131225-IB08620313-add]#
#[-start-130617-IB04560405-add]#
!import InsydeEventLogPkg/Package.dsc
#[-end-130617-IB04560405-add]#
#[-start-140609-IB13080004-add]#
#!import InsydeMediaPlayerPkg/Package.dsc
#[-end-140609-IB13080004-add]#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = SharkBay
  PLATFORM_GUID                  = C197CED3-B91A-4544-8F97-A458CA0AAFDC
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PROJECT_PKG)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Build/$(PROJECT_PKG)/Project.fdf

  !include $(PROJECT_PKG)/Project.env
  !include $(PROJECT_PKG)/Platform.env
  DEFINE SECURE_FLASH_CERTIFICATE_FILE_PATH = $(PROJECT_PKG)/FactoryCopyInfo/SecureFlash.cer

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform.
#
################################################################################
[PcdsFeatureFlag]
  gInsydeTokenSpaceGuid.PcdH2ODdtSupported|$(INSYDE_DEBUGGER)
!if $(EFI_DEBUG) == YES
  gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt|$(INSYDE_DEBUGGER)
  gInsydeTokenSpaceGuid.PcdStatusCodeUseUsb|$(USB_DEBUG_SUPPORT)
!if $(USB_DEBUG_SUPPORT) == NO
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!endif
!endif
  gInsydeTokenSpaceGuid.PcdFrontPageSupported|$(FRONTPAGE_SUPPORT)
  gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported|$(CRISIS_RECOVERY_SUPPORT)
  gInsydeTokenSpaceGuid.PcdUseFastCrisisRecovery|$(USE_FAST_CRISIS_RECOVERY)
  gInsydeTokenSpaceGuid.PcdSecureFlashSupported|$(SECURE_FLASH_SUPPORT)
#[-start-130812-IB05670190-add]#
  gInsydeTokenSpaceGuid.PcdBackupSecureBootSettingsSupported|$(BACKUP_SECURE_BOOT_SETTINGS_SUPPORT)
#[-end-130812-IB05670190-add]#
  gInsydeTokenSpaceGuid.PcdUnsignedFvSupported|$(UNSIGNED_FV_SUPPORT)
  gInsydeTokenSpaceGuid.PcdH2OConsoleRedirectionSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2ONetworkSupported|TRUE
  gInsydeTokenSpaceGuid.PcdH2ONetworkIpv6Supported|TRUE
  gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OTpmSupported|TRUE
  gInsydeTokenSpaceGuid.PcdH2OTpm2Supported|TRUE
  gInsydeTokenSpaceGuid.PcdSysPasswordInCmos|$(SYS_PASSWORD_IN_CMOS)
  gInsydeTokenSpaceGuid.PcdSysPasswordSupportUserPswd|$(SUPPORT_USER_PASSWORD)
  gInsydeTokenSpaceGuid.PcdReturnDialogCycle|$(RETURN_DIALOG_CYCLE)
  gInsydeTokenSpaceGuid.PcdH2OUsbSupported|TRUE
  gInsydeTokenSpaceGuid.PcdSeamlessAhciSupported|$(SEAMLESS_AHCI_SUPPORT)
  gInsydeTokenSpaceGuid.PcdSeamlessIdeSupported|$(SEAMLESS_IDE_SUPPORT)
  gInsydeTokenSpaceGuid.PcdH2OSdhcSupported|FALSE
#[-start-130814-IB09740037-remove]#
#  gInsydeTokenSpaceGuid.PcdScreenShotSupported|$(SCREENSHOT_SUPPORT)
#[-end-130814-IB09740037-remove]#
  gInsydeTokenSpaceGuid.PcdSnapScreenSupported|$(SNAPSCREEN_SUPPORT)
  gInsydeTokenSpaceGuid.PcdGraphicsSetupSupported|$(GRAPHICS_SETUP_SUPPORT)
  gInsydeTokenSpaceGuid.PcdUefiPauseKeyFunctionSupport|$(UEFI_PAUSE_KEY_FUNCTION_SUPPORT)
  gInsydeTokenSpaceGuid.PcdTextModeFullScreenSupport|$(TEXT_MODE_FULL_SCREEN_SUPPORT)
  gInsydeTokenSpaceGuid.PcdOnlyUsePrimaryMonitorToDisplay|$(ONLY_USE_PRIMARY_MONITOR_TO_DISPLAY)
  gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported|TRUE
  gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalTextDESupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalMetroDESupported|TRUE
!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported == TRUE
  gInsydeTokenSpaceGuid.PcdGraphicsSetupSupported|FALSE
!endif  
  gInsydeTokenSpaceGuid.PcdBvdtGenBiosBuildTimeSupported|$(BUILD_TIME_FLAG)

  gChipsetPkgTokenSpaceGuid.PcdSIOSupported|FALSE
  gChipsetPkgTokenSpaceGuid.PcdMeSupported|$(ME_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdAMTSupported|$(AMT_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdMe5MbSupported|$(ME_5MB_SUPPORT)
#[-start-130321-IB10310017-add]#
  gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported|$(SPS_ME_SUPPORT)
#[-end-130321-IB10310017-add]#
  gChipsetPkgTokenSpaceGuid.PcdThunderBoltSupported|$(THUNDERBOLT_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdGraniteCitySupported|$(GRANITE_CITY_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdMemSpdProtectionSupported|$(MEMORY_SPD_PROTECTION)
  gChipsetPkgTokenSpaceGuid.PcdMrcRmt|$(MRC_RMT)
  gChipsetPkgTokenSpaceGuid.PcdXtuSupported|$(XTU_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdTXTSupported|$(TXT_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdRapidStartSupported|$(RAPID_START_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdSusWellRestore|$(SUS_WELL_RESTORE)
  gChipsetPkgTokenSpaceGuid.PcdDdr3lvSupported|$(DDR3LV_SUPPORT)

!if gChipsetPkgTokenSpaceGuid.PcdTXTSupported
  gInsydeTokenSpaceGuid.PcdH2OTpmSupported|TRUE
!endif

!if $(ULT_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdUltFlag|TRUE
  gChipsetPkgTokenSpaceGuid.PcdLpassFlag|TRUE
  gChipsetPkgTokenSpaceGuid.PcdAdspFlag|TRUE
  gChipsetPkgTokenSpaceGuid.PcdTradFlag|FALSE
  gChipsetPkgTokenSpaceGuid.PcdPegFlag|FALSE
  gChipsetPkgTokenSpaceGuid.PcdDmiFlag|FALSE
!else
  gChipsetPkgTokenSpaceGuid.PcdUltFlag|FALSE
  gChipsetPkgTokenSpaceGuid.PcdLpassFlag|FALSE
  gChipsetPkgTokenSpaceGuid.PcdAdspFlag|FALSE
  gChipsetPkgTokenSpaceGuid.PcdTradFlag|TRUE
  gChipsetPkgTokenSpaceGuid.PcdPegFlag|TRUE
  gChipsetPkgTokenSpaceGuid.PcdDmiFlag|TRUE
!endif

#[-start-140224-IB10920078-add]#
!if $(SWITCHABLE_GRAPHICS_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdIntelSwitchableGraphicsSupported|FALSE
!else
  gChipsetPkgTokenSpaceGuid.PcdIntelSwitchableGraphicsSupported|$(SG_SUPPORT)
!endif  
#[-end-140224-IB10920078-add]#
  gChipsetPkgTokenSpaceGuid.PcdSwitchableGraphicsSupported|$(SWITCHABLE_GRAPHICS_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdNvidiaOptimusSupported|$(NVIDIA_OPTIMUS_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdAmdPowerXpressSupported|$(AMD_POWERXPRESS_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdSmmInt10Enable|$(SMM_INT10_ENABLE)
  gChipsetPkgTokenSpaceGuid.PcdEcSpiInterfaceSupported|$(EC_SPI_INTERFACE)
  gChipsetPkgTokenSpaceGuid.PcdEcSharedFlashSupported|$(EC_SHARED_FLASH_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdEcIdlePerWriteBlockSupported|$(EC_IDLE_PER_WRITE_BLOCK)
  gChipsetPkgTokenSpaceGuid.PcdAntiTheftSupported|$(ANTI_THEFT_SUPPORT)

!if $(PFAT_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdPfatSupport|TRUE
  gChipsetPkgTokenSpaceGuid.PcdPfatEcFlag|TRUE
  gInsydeTokenSpaceGuid.PcdFvbAccessThroughSmi|TRUE
!else
  gChipsetPkgTokenSpaceGuid.PcdPfatSupport|FALSE
  gChipsetPkgTokenSpaceGuid.PcdPfatEcFlag|FALSE
!endif

  gChipsetPkgTokenSpaceGuid.PcdAncSupportFlag|$(ANC_SUPPORT_FLAG)
  gChipsetPkgTokenSpaceGuid.PcdQ2LServiceSupport|$(Q2LSERVICE_SUPPORT)
#[-start-130812-IB05670190-modify]#
  gChipsetPkgTokenSpaceGuid.PcdPttSupport|$(PTT_SUPPORT)
#[-end-130812-IB05670190-modify]#

#
# for Denlow board
#
  gChipsetPkgTokenSpaceGuid.PcdDenlowServerSupported|$(DENLOW_SERVER_SUPPORT)

#[-start-130611-IB11120016-add]#
#
# H2OIPMI_SUPPORT
#
!if $(DENLOW_SERVER_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport|TRUE
!else
  gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport|$(H2OIPMI_SUPPORT)
!endif
#[-end-130611-IB11120016-add]#

#[-start-130724-IB10920034-add]#
  gChipsetPkgTokenSpaceGuid.PcdRapidStartWithIsctWakeSupport|$(RAPID_START_WITH_ISCT_WAKE_FLAG)
#[-end-130724-IB10920034-add]#
#[-start-140318-IB08400253-modify]#
#[-start-130928-IB10930061-add]#
#[-start-140423-IB08400263-modify]#
  gChipsetPkgTokenSpaceGuid.PcdBiosStorageSupport|$(EVENT_LOG_SUPPORT)
#[-end-140423-IB08400263-modify]#
  gH2OEventLogPkgTokenSpaceGuid.PcdH2OEventLogSupport|$(H2O_EVENT_LOG_SUPPORT)
  gH2OEventLogPkgTokenSpaceGuid.PcdH2OPostMessageSupport|$(H2O_POST_MESSAGE)
  gH2OEventLogPkgTokenSpaceGuid.PcdH2OPostBeepSupport|$(H2O_POST_BEEP)
#[-end-130928-IB10930061-add]#
#[-end-140318-IB08400253-modify]#
#[-start-140325-IB08400258-add]#
  gH2OEventLogPkgTokenSpaceGuid.PcdBiosEventStorageSupport|$(EVENT_LOG_SUPPORT)
  gH2OEventLogPkgTokenSpaceGuid.PcdBmcSelEventStorageSupport|$(H2OIPMI_SUPPORT)
#[-end-140325-IB08400258-add]#
#[-start-140320-IB06040445-add]#
  gInsydeTokenSpaceGuid.PcdMultiConfigSupported|$(MULTI_CONFIG_SUPPORT)
#[-end-140320-IB06040445-add]#
#[-start-140410-IB13080003-add]#
  gInsydeTokenSpaceGuid.PcdDynamicHotKeySupported|$(DYNAMIC_HOTKEY_SUPPORT)
#[-end-140410-IB13080003-add]#

#[-start-140321-IB10300105-add]#
  gInsydeTokenSpaceGuid.PcdHotKeyFlashSupported|$(HOTKEY_FLASH_SUPPORT)
#[-end-140321-IB10300105-add]#
#[-start-140521-IB05400527-add]#
  gInsydeTokenSpaceGuid.Pcd64BitAmlSupported|$(SUPPORT_64BITS_AML)
#[-end-140521-IB05400527-add]#

#[-start-140722-IB10130142-add]#
  gEfiCpuTokenSpaceGuid.PcdH2OCBTSupported|$(H2OCBT_SUPPORT)
#[-end-140722-IB10130142-add]#

[PcdsFixedAtBuild]
#[-start-131204-IB05670214-remove]#
#gEfiSioTokenSpaceGuid.PcdSerialRegisterBase|0x3F8
#  gEfiSioTokenSpaceGuid.PcdSerialLineControl|0x07
#  gEfiSioTokenSpaceGuid.PcdSerialBoudRate|115200
#[-end-131204-IB05670214-remove]#
  gEfiCpuTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x8000

!if $(FIRMWARE_PERFORMANCE) == YES
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|64
!else
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
!endif

  gPerformancePkgTokenSpaceGuid.PcdPerfPkgAcpiIoPortBaseAddress|0x1800

  #
  # Renesas USB 3.0 PCD setting for XhciSmiDispatcher
  #
  gChipsetPkgTokenSpaceGuid.PcdXhciAddonCardSetting|0x1c060601

#[-start-130107-IB10820214-modify]#
#[-start-130315-IB06690220-modify]#
# This setting need to syn with SharkBayecpPkg memoryinit.c AcpiVariableSet->AcpiReservedMemorySize, Otherwise s3 will become reboot
#  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdS3AcpiReservedMemorySize|0x10000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdS3AcpiReservedMemorySize|0x200000
#[-end-130315-IB06690220-modify]#
#[-end-130107-IB10820214-modify]#

!if $(PCI_EXPRESS_SIZE) == 256
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xE0000000
  gChipsetPkgTokenSpaceGuid.PcdPciExpressSize|256
!endif
!if $(PCI_EXPRESS_SIZE) == 128
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xF0000000
  gChipsetPkgTokenSpaceGuid.PcdPciExpressSize|128
!endif
!if $(PCI_EXPRESS_SIZE) == 64
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xF0000000
  gChipsetPkgTokenSpaceGuid.PcdPciExpressSize|64
!endif

  #
  # Hot key Configuration
  # Platform Hot key Define
  # ScanCode, ShiftKey, AltKey, CtrlKey
  # ex:
  #    0x54, 0x0, 0x1, 0x0      F1(Combination Key ScanCode) + ShiftKey
  #    0x68, 0x0, 0x2, 0x0      F1(Combination Key ScanCode) + AltKey
  #    0x5f, 0x0, 0x4, 0x0      F1(Combination Key ScanCode) + CtrlKey
  #
  gInsydeTokenSpaceGuid.PcdPlatformKeyList|{ \
    0x3b, 0x0, 0x0, 0x0,                     \ # F1_KEY
    0x3c, 0x0, 0x0, 0x0,                     \ # F2_KEY
    0x53, 0x0, 0x0, 0x0,                     \ # DEL_KEY
    0x44, 0x0, 0x0, 0x0,                     \ # F10_KEY
    0x86, 0x0, 0x0, 0x0,                     \ # F12_KEY
    0x01, 0x0, 0x0, 0x0,                     \ # ESC_KEY
    0x40, 0x0, 0x0, 0x0,                     \ # UP_ARROW_KEY_BIT
    0x3d, 0x0, 0x0, 0x0,                     \ # F3_KEY
    0x43, 0x0, 0x0, 0x0,                     \ # F9_KEY
    0x00, 0x0, 0x0, 0x0}                       # EndEntry

#
# for Denlow board
#
!if $(DENLOW_SERVER_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdGlobalNvsAreaPlatformFlavor|0x03
  gInsydeTokenSpaceGuid.PcdDefaultHorizontalResolution|640
  gInsydeTokenSpaceGuid.PcdDefaultVerticalResolution|480
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins0|0x00000000
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins1|0x00000000
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins2|0x00000003
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins3|0x00000003
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins4|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins5|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins6|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins7|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins8|0x00000006
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins9|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins10|0x00000005
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins11|0x00000005
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins12|0x00000006
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb20OverCurrentPins13|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins0|0x00000000
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins1|0x00000000
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins2|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins3|0x00000008
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins4|0x00000003
  gChipsetPkgTokenSpaceGuid.PcdPchUsbConfigUsb30OverCurrentPins5|0x00000003
#[-start-130510-IB10310019-add]#
  gChipsetPkgTokenSpaceGuid.PcdPchPchPlatformDataEcPresent|0x00000000
#[-end-130510-IB10310019-add]#
!else
  gChipsetPkgTokenSpaceGuid.PcdGlobalNvsAreaPlatformFlavor|0x02
!endif

#[-start-130410-IB05160433-add]#
  #
  # Board ID for ASL code, please refer to token.asl
  #  Flathead Creek Board ID for ASL code    : 0x40
  #  Flathead Creek CRB Board ID for ASL code: 0x43
  #
  gChipsetPkgTokenSpaceGuid.PcdBoardIdForAslCode|0x43
#[-end-130410-IB05160433-add]#

#[-start-130425-IB05160441-add]#
#
# ThunderBolt chip: CACTUS_RIDGE (CR) = 0 or REDWOOD_RIDGE (RR) = 1
#
  gChipsetPkgTokenSpaceGuid.PcdThunderBoltChip|0x00
#[-end-130425-IB05160441-add]#

#[-start-130611-IB11120016-add]#
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  #
  # Because FHC PMBASE is 0x1800, ACPI Timer Address is 0x1808
  #
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiAcpiTimerAddress|0x1808
!if gChipsetPkgTokenSpaceGuid.PcdDenlowServerSupported
  #
  # Config PliotII/PliotIII by Super I/O interface to active KCS Logical Devices for Denlow board
  #
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiSioConfig|TRUE
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiLpcDecode|TRUE
!else
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiSioConfig|FALSE
!endif
!endif
#[-end-130611-IB11120016-add]#

#[-start-130709-IB05160465-add]#
  #
  # Note: If change sizeof(SYSTEM_CONFIGURATION) in SetupConfig.h, must update really structure size in here!!!
  #
  gChipsetPkgTokenSpaceGuid.PcdSetupConfigSize|1300
#[-end-130709-IB05160465-add]#

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamicDefault]
#[-start-130911-IB08620310-add]#
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMrIovSupport|FALSE
#[-end-130911-IB08620310-add]#
#[-start-130201-IB05280044-modify]#
#  2-Chip
#   GRAYS_REEF                              = PcdPeiGpioTable1
#   BASKING_RIDGE                           = PcdPeiGpioTable2
#  Ult
#   WHITE_TIP_MOUNTAIN                      = PcdPeiGpioTable3
#   SAW_TOOTH_PEAK                          = PcdPeiGpioTable4
#   HARRIS_BEACH                            = PcdPeiGpioTable5
#  For OEM MB
#   if PcdDefaultBoardId = 0xFF, alway load PcdPeiGpioTable1.
#
# GPIO_SETTING_TABLE field name is reference to PCH EDS mnemonic
# Name       | 2-Chip Spec  | ULT Spec
# GpioUseSel | GPIO_USE_SEL | GPIO_USE_SEL
# GpioIoSel  | GP_IO_SEL    | GPIO_IO_SEL
# GpoLvl     | GP_LVL       | GPO_LVL
# GpioBlink  | GPO_BLINK    | GPO_BLINK
# GpiInv     | GPI_INV      | GPI_INV
# GpioOwn    | None         | GPIO_OWN
# GpiRout    | None         | GPI_ROUT
# GpiIe      | None         | GPI_IE
# GpiLxEb    | None         | GPI_LxEB
# GpiWp      | None         | GPIWP
# GpinDis    | None         | GPINDIS
# None value set 0
#
# Data size is 2 bytes for each Pin.
# Byte 0:
#  [7]    [6]     [5]    [4]      [3]     [2]      [1]       [0]
# GpiIe GpiRout GpioOwn GpiInv GpioBlink GpoLvl GpioIoSel GpioUseSel
#
# Byte 1:
#  [7:4]     [3]   [2:1]   [0]
# Reserved GpinDis GpiWp GpiLxEb
#
# For FHC or Default
  gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable1|{ \
#  Pin0      Pin1      Pin2      Pin3
#[0]  [1]  [0]  [1]  [0]  [1]  [0]  [1]
0x07,0x00,0x17,0x00,0x17,0x00,0x04,0x00, \  #Pin0~Pin3
0x07,0x00,0x07,0x00,0x07,0x00,0x17,0x00, \  #Pin4~Pin7
0x17,0x00,0x06,0x00,0x06,0x00,0x01,0x00, \  #Pin8~Pin11
0x06,0x00,0x01,0x00,0x13,0x00,0x01,0x00, \  #Pin12~Pin15
0x07,0x00,0x13,0x00,0x07,0x00,0x03,0x00, \  #Pin16~Pin19
0x06,0x00,0x03,0x00,0x07,0x00,0x06,0x00, \  #Pin20~Pin23
0x07,0x00,0x07,0x00,0x07,0x00,0x07,0x00, \  #Pin24~Pin27
0x07,0x00,0x04,0x00,0x04,0x00,0x03,0x00, \  #Pin28~Pin31
0x06,0x00,0x07,0x00,0x07,0x00,0x03,0x00, \  #Pin32~Pin35
0x07,0x00,0x07,0x00,0x07,0x00,0x03,0x00, \  #Pin36~Pin39
0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00, \  #Pin40~Pin43
0x07,0x00,0x01,0x00,0x05,0x00,0x01,0x00, \  #Pin44~Pin47
0x07,0x00,0x07,0x00,0x07,0x00,0x03,0x00, \  #Pin48~Pin51
0x07,0x00,0x03,0x00,0x07,0x00,0x03,0x00, \  #Pin52~Pin55
0x06,0x00,0x03,0x00,0x06,0x00,0x06,0x00, \  #Pin56~Pin59
0x05,0x00,0x04,0x00,0x04,0x00,0x04,0x00, \  #Pin60~Pin63
0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00, \  #Pin64~Pin67
0x07,0x00,0x03,0x00,0x07,0x00,0x07,0x00, \  #Pin68~Pin71
0x04,0x00,0x06,0x00,0x07,0x00,0x06,0x00, \  #Pin72~Pin75
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin76~Pin79
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin80~Pin83
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin84~Pin87
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin88~Pin91
0x00,0x00,0x00,0x00,0x00,0x00}              #Pin92~Pin94
#[-end-130201-IB05280044-modify]#

# For DELOW or Default
#[-start-130322-IB10130037-add]#
gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable2|{ \
#  Pin0      Pin1      Pin2      Pin3
#[0]  [1]  [0]  [1]  [0]  [1]  [0]  [1]
0x07,0x00,0x06,0x00,0x04,0x00,0x04,0x00, \  #Pin0~Pin3
0x04,0x00,0x01,0x00,0x06,0x00,0x06,0x00, \  #Pin4~Pin7
0x04,0x00,0x06,0x00,0x06,0x00,0x06,0x00, \  #Pin8~Pin11
0x04,0x00,0x07,0x00,0x07,0x00,0x07,0x00, \  #Pin12~Pin15
0x05,0x00,0x06,0x00,0x01,0x00,0x02,0x00, \  #Pin16~Pin19
0x06,0x00,0x03,0x00,0x04,0x00,0x06,0x00, \  #Pin20~Pin23
0x03,0x00,0x01,0x00,0x03,0x00,0x01,0x00, \  #Pin24~Pin27
0x05,0x00,0x04,0x00,0x00,0x00,0x06,0x00, \  #Pin28~Pin31
0x01,0x00,0x03,0x00,0x05,0x00,0x01,0x00, \  #Pin32~Pin35
0x03,0x00,0x07,0x00,0x04,0x00,0x04,0x00, \  #Pin36~Pin39
0x02,0x00,0x07,0x00,0x06,0x00,0x07,0x00, \  #Pin40~Pin43
0x07,0x00,0x07,0x00,0x07,0x00,0x02,0x00, \  #Pin44~Pin47
0x04,0x00,0x01,0x00,0x07,0x00,0x03,0x00, \  #Pin48~Pin51
0x07,0x00,0x03,0x00,0x07,0x00,0x07,0x00, \  #Pin52~Pin55
0x02,0x00,0x06,0x00,0x06,0x00,0x06,0x00, \  #Pin56~Pin59
0x06,0x00,0x04,0x00,0x03,0x00,0x03,0x00, \  #Pin60~Pin63
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00, \  #Pin64~Pin67
0x06,0x00,0x03,0x00,0x07,0x00,0x07,0x00, \  #Pin68~Pin71
0x05,0x00,0x01,0x00,0x00,0x00,0x06,0x00, \  #Pin72~Pin75
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin76~Pin79
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin80~Pin83
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin84~Pin87
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \  #Pin88~Pin91
0x00,0x00,0x00,0x00,0x00,0x00}              #Pin92~Pin94
#[-end-130322-IB10130037-add]#

#[-start-130531-IB05160452-remove]#
##[-start-130201-IB11410050-modify]#
#gChipsetPkgTokenSpaceGuid.PcdPeiSioTable|{ \
#  0x02, 0x88, \   #Power On UARTs
#  0x24, 0xFE, \
#  0x25, 0xBE, \
#  0x28, 0x43, \   #IRQ of UARTs, UART2 IRQ=3,UART1 IRQ=4,
#  0x29, 0x80, \   # SIRQ_CLKRUN_EN
#  0x2A, 0x00, \
#  0x2B, 0xDE, \
#  0x2C, 0x02, \
#  0x30, 0x60, \
#  0x3B, 0x06, \
#  0x3C, 0x10, \
#  0x3A, 0x0A, \   # LPC Docking Enabling
#  0x31, 0x1F, \
#  0x32, 0x00, \
#  0x33, 0x04, \
#  0x38, 0xFB, \
#  0x35, 0xFE, \
#  0x36, 0x00, \
#  0x37, 0xFE, \
#  0x3A, 0x0B, \
#  0x3C, 0x90, \
#  0x39, 0x00, \
#  0x34, 0x01, \
#  0x12, 0x2E, \        # Relocate configuration ports base address to 0x2E
#  0x13, 0x00}          # to ensure SIO config address can be accessed in OS
##[-end-130201-IB11410050-modify]#
##[-end-130110-IB11410040-modify]#
#[-end-130531-IB05160452-remove]#

#[-start-121207-IB10820186-modify]#
#
# UINT32 BassAddress  Bit 15:2 Decode Range Base Address
# UINT32 RangeAddress Bit 23:18 Decode Range address [7:2]Mask
#
  gChipsetPkgTokenSpaceGuid.PcdPchLpcIoDecodeTable|{ \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \     # LPC offset 84h-87h
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \     # LPC offset 88h-8Bh
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \     # LPC offset 8Ch-8Fh
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  \     # LPC offset 90h-93h
  }
#[-end-121207-IB10820186-modify]#


#[-start-130531-IB05160452-remove]#
##[-start-130508-IB10310018-modify]#
##[-start-130417-IB02960526-add]#
#   gInsydeTokenSpaceGuid.PcdPreserveMemoryTable|{    \
#     0x09, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, \ # Preserve 256K(0x40 pages) for ASL
#     0x0a, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, \ # Preserve 4M(0x400 pages) for S3, SMM, etc
#     0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, \ # Preserve 4.2M(0x420 pages) for BIOS reserved memory
#     0x06, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, \ # Preserve 3M(0x300 pages) for UEFI OS runtime data to make S4 memory consistency
#     0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, \ # Preserve 2M(0x200 pages) for UEFI OS runtime drivers to make S4 memory consistency
#     0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, \ # Preserve 16M(0x1000 pages) for boot service drivers to reduce memory fragmental
#     0x04, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, \ # Preserve 64M(0x4000 pages) for boot service data to reduce memory fragmental
#     0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, \ # Preserve 2M(0x200 pages) for UEFI OS boot loader to keep on same address
#     0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
#     0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  \ #EndEntry
#   }
##[-end-130417-IB02960526-add]#
##[-end-130508-IB10310018-modify]#
#[-end-130531-IB05160452-remove]#
  #
  # Crisis File name definition
  #
  # New File Path Definition : //Volume_Label\\File_Path\\File_Name
  # Notice : "//" is signature that volume label start definition.
  #
  # Example path : //RECOVERY\\BIOS\\Current\\SHARKBAY.fd
#[-start-130814-IB08340128-modify]#
  gInsydeTokenSpaceGuid.PcdPeiRecoveryFile|L"SHARKBAY.fd"|VOID*|0x100
#[-end-130814-IB08340128-modify]#


#[-start-131203-IB07390113-add]#
################################################################################
#
# SMBIOS Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicExDefault]
#[-start-140121-IB07390126-modify]#
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType000|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType001|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType002|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType003|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType008|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType009|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType011|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType012|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType013|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType015|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType021|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType022|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType026|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType027|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType028|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType032|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType039|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType040|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType041|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType128|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType129|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType130|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType136|TRUE

  gSmbiosTokenSpaceGuid.PcdSmbiosMaxMultiRecords |32
  gSmbiosTokenSpaceGuid.PcdSmbiosMultiRecordsType|{2, 3, 4, 7, 8, 9, 17, 21, 22, 26, 27, 28, 29, 39, 41}

  gSmbiosTokenSpaceGuid.PcdType000Record | { \
      0x00,                       \ # Type
      0x00,                       \ # Length
      UINT16(0x0000),             \ # Handle
      0xFF,                       \ # Vendor
      0x02,                       \ # BIOS Version
      UINT16(0xE000),             \ # BIOS Starting Address Segment
      0xFF,                       \ # BIOS Release Date
      0xFF,                       \ # BIOS ROM Size
      UINT64(0x000000004BF99880), \ # BIOS Characteristics
      UINT16(0x0D03),             \ # BIOS Characteristics Extension Bytes
      0xFF,                       \ # System BIOS Major Release
      0xFF,                       \ # System BIOS Minor Release
      0xFF,                       \ # Embedded Controller Firmware Major Release
      0xFF                        \ # Embedded Controller Firmware Minor Release
  }
  
  gSmbiosTokenSpaceGuid.PcdType000Strings|"Insyde;05.04.34;2014/08/27;"
  gSmbiosTokenSpaceGuid.PcdType001Record |{0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x90, 0xab, 0xcd, 0xde, 0xef, 0xaa, 0xbb, 0xcc, 0x06, 0x05, 0x06}
  gSmbiosTokenSpaceGuid.PcdType001Strings|"Insyde;SharkBay;TBD by OEM;123456789;Type1Sku0;Type1Family;"

  gSmbiosTokenSpaceGuid.PcdType002Record000 |{0x02, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x09, 0x06, 0xFF, 0xFF, 0x0A, 0x00}
  gSmbiosTokenSpaceGuid.PcdType002Strings000|"Type2 - Board Vendor Name1;Type2 - Board Product Name1;Type2 - Board Version;Type2 - Board Serial Number;Type2 - Board Asset Tag;Type2 - Board Chassis Location;"

  gSmbiosTokenSpaceGuid.PcdType003Record000 |{0x03, 0x00, 0x00, 0x00, 0x01, 0x0A, 0x02, 0x03, 0x04, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05}
  gSmbiosTokenSpaceGuid.PcdType003Strings000|"Chassis Manufacturer;Chassis Version;Chassis Serial Number;Chassis Asset Tag;SKU Number;"

  gSmbiosTokenSpaceGuid.PcdType008Record000|{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0F, 0x0D}
  gSmbiosTokenSpaceGuid.PcdType008Strings000|"J1A1;Keyboard;"
  gSmbiosTokenSpaceGuid.PcdType008Record001 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0F, 0x0E}
  gSmbiosTokenSpaceGuid.PcdType008Strings001|"J1A1;Mouse;"
  gSmbiosTokenSpaceGuid.PcdType008Record002 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0D, 0x1C}
  gSmbiosTokenSpaceGuid.PcdType008Strings002|"J2A1;TV OUT;"
  gSmbiosTokenSpaceGuid.PcdType008Record003 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x07, 0x1C}
  gSmbiosTokenSpaceGuid.PcdType008Strings003|"J2A2;CRT;"
  gSmbiosTokenSpaceGuid.PcdType008Record004 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x08, 0x09}
  gSmbiosTokenSpaceGuid.PcdType008Strings004|"J2A2;COM 1;"
  gSmbiosTokenSpaceGuid.PcdType008Record005 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings005|"J3A1;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record006 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings006|"J3A1;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record007 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings007|"J3A1;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record008 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings008|"J5A1;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record009 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings009|"J5A1;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record010 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings010|"J5A2;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record011 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0B, 0x1F}
  gSmbiosTokenSpaceGuid.PcdType008Strings011|"J5A1;Network;"
  gSmbiosTokenSpaceGuid.PcdType008Record012 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x17, 0x02, 0x00, 0xFF}
  gSmbiosTokenSpaceGuid.PcdType008Strings012|"J9G2;OnBoard Floppy Type;"
  gSmbiosTokenSpaceGuid.PcdType008Record013 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x16, 0x02, 0x00, 0xFF}
  gSmbiosTokenSpaceGuid.PcdType008Strings013|"J7J1;OnBoard Primary IDE;"
  gSmbiosTokenSpaceGuid.PcdType008Record014 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x1F, 0x1D}
  gSmbiosTokenSpaceGuid.PcdType008Strings014|"J30;Microphone In;"
  gSmbiosTokenSpaceGuid.PcdType008Record015 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x1F, 0x1D}
  gSmbiosTokenSpaceGuid.PcdType008Strings015|"J30;Line In;"
  gSmbiosTokenSpaceGuid.PcdType008Record016 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x1F, 0x1D}
  gSmbiosTokenSpaceGuid.PcdType008Strings016|"J30;Speaker Out;"

#[-start-131231-IB07390121-modify]#
  gSmbiosTokenSpaceGuid.PcdType009Record000 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xAA, 0x0D, 0x00, 0x01, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08}
  gSmbiosTokenSpaceGuid.PcdType009Strings000|"J6UB;"
  gSmbiosTokenSpaceGuid.PcdType009Record001 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE7}
  gSmbiosTokenSpaceGuid.PcdType009Strings001|"J4BU;"
  gSmbiosTokenSpaceGuid.PcdType009Record002 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE6}
  gSmbiosTokenSpaceGuid.PcdType009Strings002|"J17BU;"
  gSmbiosTokenSpaceGuid.PcdType009Record003 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x04, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE5}
  gSmbiosTokenSpaceGuid.PcdType009Strings003|"J13BU;"
  #gSmbiosTokenSpaceGuid.PcdType009Record004 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE3}
  #gSmbiosTokenSpaceGuid.PcdType009Strings004|"J7C1;"
  #gSmbiosTokenSpaceGuid.PcdType009Record005 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE2}
  #gSmbiosTokenSpaceGuid.PcdType009Strings005|"J7D2;"
  #gSmbiosTokenSpaceGuid.PcdType009Record006 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x00, 0x01, 0x07, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE1}
  #gSmbiosTokenSpaceGuid.PcdType009Strings006|"J8C1;"
  #gSmbiosTokenSpaceGuid.PcdType009Record007 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xAA, 0x0D, 0x00, 0x01, 0x08, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xE0}
  #gSmbiosTokenSpaceGuid.PcdType009Strings007|"J8C2;"
  #gSmbiosTokenSpaceGuid.PcdType009Record008 |{0x09, 0x00, 0x00, 0x00, 0x01, 0x06, 0x05, 0x00, 0x01, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}
  #gSmbiosTokenSpaceGuid.PcdType009Strings008|"J9C1;"
#[-end-131231-IB07390121-modify]#

  gSmbiosTokenSpaceGuid.PcdType011Record |{0x0B, 0x00, 0x00, 0x00, 0x03}
  gSmbiosTokenSpaceGuid.PcdType011Strings|"OemString1;OemString2;OemString3;"
  gSmbiosTokenSpaceGuid.PcdType012Record |{0x0C, 0x00, 0x00, 0x00, 0x03}
  gSmbiosTokenSpaceGuid.PcdType012Strings|"ConfigOptions1;ConfigOptions2;ConfigOptions3;"
  gSmbiosTokenSpaceGuid.PcdType013Record |{0x0D, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
  gSmbiosTokenSpaceGuid.PcdType013Strings|"en|US|iso8859-1,0;fr|CA|iso8859-1,0;zh|TW|unicode,0;ja|JP|unicode,0;it|IT|iso8859-1,0;es|ES|iso8859-1,0;de|DE|iso8859-1,0;pt|PT|iso8859-1,0;"
  #gSmbiosTokenSpaceGuid.PcdType013Record |{0x0D, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
  #gSmbiosTokenSpaceGuid.PcdType013Strings|"enUS,0;frCA,0;zhTW,0;jaJP,0;itIT,0;esES,0;deDE,0;ptPT,0;"
  gSmbiosTokenSpaceGuid.PcdType015Record |{0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x01, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x02, 0x07, 0x00, 0x08, 0x04, 0x16, 0x00}

  gSmbiosTokenSpaceGuid.PcdType021Record000 |{0x15, 0x00, 0x00, 0x00, 0x07, 0x04, 0x04}

  gSmbiosTokenSpaceGuid.PcdType022Record000 |{0x16, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}
  gSmbiosTokenSpaceGuid.PcdType022Strings000|"Fake;-Virtual Battery 0-;08/08/2010;Battery 0;CRB Battery 0;LithiumPolymer;"

  gSmbiosTokenSpaceGuid.PcdType026Record000 |{0x1A, 0x00, 0x00, 0x00, 0x01, 0x42, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80}
  gSmbiosTokenSpaceGuid.PcdType026Strings000|"Voltage Probe Description;"

  gSmbiosTokenSpaceGuid.PcdType027Record000 |{0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01}
  gSmbiosTokenSpaceGuid.PcdType027Strings000|"Cooling Device Description;"

  gSmbiosTokenSpaceGuid.PcdType028Record000 |{0x1C, 0x00, 0x00, 0x00, 0x01, 0x42, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80}
  gSmbiosTokenSpaceGuid.PcdType028Strings000|"Temperature Probe Description;"

  gSmbiosTokenSpaceGuid.PcdType032Record |{0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

  gSmbiosTokenSpaceGuid.PcdType039Record000 |{0x27, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x4B, 0x00, 0xA4, 0x21, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
  gSmbiosTokenSpaceGuid.PcdType039Strings000|"OEM Define 0;OEM Define 1;OEM Define 2;OEM Define 3;OEM Define 4;OEM Define 5;OEM Define 6;"

  gSmbiosTokenSpaceGuid.PcdType040Record |{0x28, 0x00, 0x00, 0x00, 0x02, 0x06, 0x09, 0x00, 0x05, 0x01, 0xAA, 0x06, 0x00, 0x00, 0x05, 0x02, 0x00}
  gSmbiosTokenSpaceGuid.PcdType040Strings|"PCIExpressx16;Compiler Version: VC 9.0;"

  gSmbiosTokenSpaceGuid.PcdType041Record000 |{0x29, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x10}
  gSmbiosTokenSpaceGuid.PcdType041Strings000|"IGD;"

  gSmbiosTokenSpaceGuid.PcdType128Record |{0x80, 0x00, 0x00, 0x00, 0x55, 0xAA, 0x55, 0xAA}
  gSmbiosTokenSpaceGuid.PcdType128Strings|"Oem Type 128 Test 1;Oem Type 128 Test 2;"
  gSmbiosTokenSpaceGuid.PcdType129Record |{0x81, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x01}
  gSmbiosTokenSpaceGuid.PcdType129Strings|"Insyde_ASF_001;Insyde_ASF_002;"
  gSmbiosTokenSpaceGuid.PcdType130Record |{0x82, 0x00, 0x00, 0x00, 0x24, 0x41, 0x4D, 0x54, 0x01, 0x01, 0x01, 0x01, 0x01, 0xA5, 0x1F, 0x02, 0x00, 0x00, 0x00, 0x00}

  gSmbiosTokenSpaceGuid.PcdType136Record |{0x88, 0x00, 0x00, 0x00, 0xFF, 0xFF}
#[-end-131203-IB07390113-add]#
#[-end-140121-IB07390126-modify]#

[Libraries]
  InsydeFrameworkModulePkg/Library/BaseCryptLib/BaseCryptLib.inf

[LibraryClasses]
#[-start-130814-IB08340127-remove]#
#  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
#[-end-130814-IB08340127-remove]#
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
#[-start-130813-IB06720232-add]#
  BaseOemSvcKernelLib|$(PROJECT_PKG)/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLib.inf
  BaseOemSvcChipsetLib|$(PROJECT_PKG)/Library/BaseOemSvcChipsetLib/BaseOemSvcChipsetLib.inf
#[-end-130813-IB06720232-add]#

#[-start-130129-IB05670132-remove]#
#  BaseSetupDefaultLib|InsydeModulePkg/Library/CommonPlatformLib/BaseSetupDefaultLib/BaseSetupDefaultLib.inf {
#  BaseSetupDefaultLib|$(PROJECT_PKG)/Library/CommonPlatformLib/BaseSetupDefaultLib/BaseSetupDefaultLib.inf {
#  <SOURCE_OVERRIDE_PATH>
  #
  # You should put the *.vfr and *.uni file path here.
  #
#  InsydeModulePkg/Library/SetupUtilityLib
#  InsydeModulePkg/Library/SetupUtilityLib/Main
#  InsydeModulePkg/Library/SetupUtilityLib/Security
#  InsydeModulePkg/Library/SetupUtilityLib/boot
#  InsydeModulePkg/Library/SetupUtilityLib/Exit
#[-start-120816-IB10820105-modify]#
#    $(CHIPSET_PKG)/UefiSetupUtilityDxe
#    $(CHIPSET_PKG)/UefiSetupUtilityDxe/Power
#    $(CHIPSET_PKG)/UefiSetupUtilityDxe/Advance
#[-end-120816-IB10820105-modify]#
#}
#[-end-130129-IB05670132-remove]#

#[-start-130531-IB05160452-remove]#
##[-start-130408-IB05160430-add]#
#  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Library/SetupUtilityLib
#  }
##[-end-130408-IB05160430-add]#
#[-end-130531-IB05160452-remove]#

[LibraryClasses.common.SEC]
#[-start-120621-IB03530373-add]#
  H2ODebugLib|InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
#[-end-120621-IB03530373-add]#

[LibraryClasses.common.PEI_CORE]
  PeiOemSvcKernelLib|$(PROJECT_PKG)/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf

[LibraryClasses.common.PEIM]
  PeiOemSvcKernelLib|$(PROJECT_PKG)/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf
#[-start-120726-IB10820086-add]#
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
#[-end-120726-IB10820086-add]#
#[-start-130419-IB05400398-add]#
  PeiOemSvcChipsetLib|$(PROJECT_PKG)/Library/PeiOemSvcChipsetLib/PeiOemSvcChipsetLib.inf
#[-end-130419-IB05400398-add]#

[LibraryClasses.common.DXE_CORE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DxeOemSvcChipsetLib|$(PROJECT_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DxeOemSvcChipsetLib|$(PROJECT_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf

[LibraryClasses.common.DXE_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DxeOemSvcChipsetLib|$(PROJECT_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
#[-start-130618-IB05160458-add]#
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
#[-end-130618-IB05160458-add]#

[LibraryClasses.common.COMBINED_SMM_DXE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf

[LibraryClasses.common.SMM_CORE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
#[-start-130618-IB05160458-add]#
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
#[-end-130618-IB05160458-add]#

[LibraryClasses.common.UEFI_APPLICATION]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
################################################################################
#
# Platform related components
#
################################################################################
[Components.IA32]

#[-start-120222-IB05300283-modify]#
#[-start-130531-IB05160452-remove]#
#  !disable InsydeModulePkg/Bus/Isa/Ps2KeyboardPei/Ps2KeyboardPei.inf
#  InsydeModulePkg/Bus/Isa/Ps2KeyboardPei/Ps2KeyboardPei.inf{
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Isa/Ps2KeyboardPei
#  }
#[-end-130531-IB05160452-remove]#
#[-end-120222-IB05300283-modify]#
#[-start-130322-IB05670149-modify]#
#[-start-130129-IB11410048-add]#
#[-start-130529-IB12360011-remove]#
#  !disable InsydeModulePkg/Universal/CommonPolicy/PlatformStage1Pei/PlatformStage1Pei.inf
#  InsydeModulePkg/Universal/CommonPolicy/PlatformStage1Pei/PlatformStage1Pei.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/CommonPolicy/PlatformStage1Pei
#  }
#[-end-130529-IB12360011-remove]#
#[-end-130129-IB11410048-add]#
#[-start-120907-IB10820120-add]#
#[-start-130531-IB05160452-remove]#
#  !disable InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf
#  InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei
#  }
#[-end-130531-IB05160452-remove]#
#[-end-120907-IB10820120-add]#
#[-end-130322-IB05670149-modify]#

#[-start-130531-IB05160452-remove]#
##[-start-130326-IB06690221-add]#
#  !disable InsydeModulePkg/Universal/Acpi/S3RestoreAcpiPei/S3RestoreAcpiPei.inf
#  InsydeModulePkg/Universal/Acpi/S3RestoreAcpiPei/S3RestoreAcpiPei.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Acpi/S3RestoreAcpiPei
#  }
##[-end-130326-IB06690221-add]#
#
#  !disable InsydeModulePkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf
#  InsydeModulePkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Acpi/S3Resume2Pei
#  }
#[-end-130531-IB05160452-remove]#


[Components.X64]

#[-start-130617-IB04560405-remove]#
##[-start-120209-IB10820003-add]#
#!if $(EVENT_LOG_SUPPORT)== YES
#  InsydeFrameworkModulePkg/Platform/Generic/EventLog/Pei/PeiEventLog.inf{
#    <SOURCE_OVERRIDE_PATH>
#      $(COMMON_PLATFORM_DIR)/RuntimeDxe/FvbServices
#  }
#!endif
##[-end-120209-IB10820003-add]#
#[-end-130617-IB04560405-remove]#
#[-start-130322-IB05670149-modify]#
#[-start-130531-IB05160452-remove]#
#  !disable InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
#  InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#     $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe
#  }
#[-end-130531-IB05160452-remove]#
#[-end-130322-IB05670149-modify]#

  !disable InsydeModulePkg/Universal/FirmwareVolume/FlashDeviceFvbRuntimeDxe/FlashDeviceFvbRuntimeDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FlashDeviceFvbRuntimeDxe/FlashDeviceFvbRuntimeDxe.inf {
    <LibraryClasses>
      FdSupportLib|InsydeModulePkg/Library/FlashDeviceSupport/FdSupportLib/FdSupportLib.inf
#[-start-130403-IB09720138-remove]#
#       ChipsetLib|InsydeModulePkg/Library/ChipsetLib/DxeChipsetLib/DxeChipsetLib.inf
#[-end-130403-IB09720138-remove]#
      IrsrRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLib/IrsiRegistrationLib.inf
  }
#[-end-120630-IB03530377-modify]#

#[-start-130125-IB05160404-add]#
#[-start-130531-IB05160452-remove]#
#  !disable InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
#  InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf {
#   <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe
#  }
#[-end-130531-IB05160452-remove]#
#[-end-130125-IB05160404-add]#

#[-start-130403-IB05160429-add]#
#[-start-130531-IB05160452-remove]#
#  !disable InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
#  InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf {
#   <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Csm/LegacyBiosDxe
#  }
#[-end-130531-IB05160452-remove]#
#[-end-130403-IB05160429-add]#

#[-start-130516-12360009-remove]#
#[-start-130322-IB05670149-modify]#
#[-start-130227-IB10930024-add]#
#  !disable InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe/LegacyBiosPlatformDxe.inf
#  InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe/LegacyBiosPlatformDxe.inf {
#   <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe
#  }
#[-end-130227-IB10930024-add]#
#[-end-130322-IB05670149-modify]#
#[-end-130516-12360009-remove]#

#[-start-130401-IB06690222-add]#
# Implement this funtion at BdsPlatform driver ...
  !disable EdkCompatibilityPkg/Compatibility/DxeSmmReadyToLockOnExitPmAuthThunk/DxeSmmReadyToLockOnExitPmAuthThunk.inf
#[-end-130401-IB06690222-add]#

  #
  # AcpiTable
  #
  $(PROJECT_PKG)/AcpiTablesDxe/Dptf/DptfDxe/DptfDxe.inf
  $(PROJECT_PKG)/AcpiTablesDxe/Dptf/DptfAcpiTables/DptfAcpiTables.inf
  $(PROJECT_PKG)/AcpiTablesDxe/Cppc/CppcDxe/CppcDxe.inf
  $(PROJECT_PKG)/AcpiTablesDxe/Cppc/CppcAcpiTables/CppcAcpiTables.inf
  $(PROJECT_PKG)/AcpiTablesDxe/AcpiTablesDxe.inf
  $(PROJECT_PKG)/AcpiFviDxe/AcpiFviDxe.inf

###################################################################################################
#
# BuildOptions Section - Define the module specific tool chain flags that should be used as
#                        the default flags for a module. These flags are appended to any
#                        standard flags that are defined by the build process. They can be
#                        applied for any modules or only those modules with the specific
#                        module style (EDK or EDKII) specified in [Components] section.
#
###################################################################################################
[BuildOptions.Common.EDK]
!if $(INSYDE_DEBUGGER) == YES
  *_*_IA32_CC_FLAGS    = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
!else
  *_*_IA32_CC_FLAGS    = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
!endif
  *_*_IA32_ASM_FLAGS   = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(ASM_FLAGS)
  *_*_IA32_VFRPP_FLAGS = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_APP_FLAGS   = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_PP_FLAGS    = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_ASLPP_FLAGS = -D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)

!if $(INSYDE_DEBUGGER) == YES
  *_*_X64_CC_FLAGS     = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS) /wd4028 /wd4031 /wd4334 /wd4718 /wd4090 /wd4213 -D EFI_FIRMWARE_VENDOR="L\"INSYDE Corp.\""  -D MDE_CPU_X64 -D PCH_DEBUG_INFO=1 /Od
!else
  *_*_X64_CC_FLAGS     = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS) /wd4028 /wd4031 /wd4334 /wd4718 /wd4090 /wd4213 -D EFI_FIRMWARE_VENDOR="L\"INSYDE Corp.\""  -D MDE_CPU_X64 -D PCH_DEBUG_INFO=1
!endif
  *_*_X64_ASM_FLAGS    = -DEFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(ASM_FLAGS)
  *_*_X64_VFRPP_FLAGS  = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_APP_FLAGS    = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_PP_FLAGS     = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_ASLPP_FLAGS  = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_ASLCC_FLAGS  = -D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)

  *_*_*_VFR_FLAGS   =
  *_*_*_BUILD_FLAGS = -s

[BuildOptions.common.EDKII]
  GCC:*_*_IA32_CC_FLAGS          = -Wno-error -Wno-unused-local-typedefs -Wno-pointer-to-int-cast -Wno-unused-function -Wno-parentheses -DMDEPKG_NDEBUG $(CC_FLAGS)
  GCC:*_*_X64_CC_FLAGS           = -Wno-error -DMDEPKG_NDEBUG $(CC_FLAGS)
  GCC:*_*_IA32_JWASM_FLAGS       =
  GCC:*_*_X64_JWASM_FLAGS        =
  INTEL:*_*_*_CC_FLAGS           = -D MDEPKG_NDEBUG $(CC_FLAGS)
  MSFT:RELEASE_*_*_CC_FLAGS      = -D MDEPKG_NDEBUG $(CC_FLAGS)
  MSFT:DEBUG_*_*_CC_FLAGS        = $(CC_FLAGS)
!if $(EFI_DEBUG) == NO
  MSFT:DEBUG_*_*_CC_FLAGS        = -D MDEPKG_NDEBUG $(CC_FLAGS)
!endif
  *_*_*_ASLPP_FLAGS              =  $(CC_FLAGS)

#[-start-120903-IB11410001-add]#
  *_*_*_VFRPP_FLAGS         = $(CC_FLAGS)
#[-end-120903-IB11410001-add]#

#[-start-120525-IB03530367-add]#
   *_*_IA32_ASM_FLAGS            = $(EDKII_IA32_ASM_BUILD_OPTIONS)
#[-end-120525-IB03530367-add]#

