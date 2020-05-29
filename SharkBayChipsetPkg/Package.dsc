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

[Defines]
#[-start-130301-IB05670140-add]#
  DEFINE  CHIPSET_PKG          = SharkBayChipsetPkg

!include $(CHIPSET_PKG)/Package.env
#[-end-130301-IB05670140-add]#

[PcdsFeatureFlag]

[LibraryClasses]
  ChipsetCpuLib|$(CHIPSET_PKG)/Library/ChipsetCpuLib/ChipsetCpuLib.inf
  SpiAccessLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/SpiAccessLib/SpiAccessLib.inf
  FlashWriteEnableLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/FlashWriteEnableLib/FlashWriteEnableLib.inf
  SpiAccessInitLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/DxeSpiAccessInitLib/DxeSpiAccessInitLib.inf
#[start-130916-IB05670200-remove]#
#  FlashDevicesLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/FlashDevicesLib/FlashDevicesLib.inf
#[end-130916-IB05670200-remove]#
  SmmLib|$(CHIPSET_PKG)/Library/PchSmmLib/PchSmmLib.inf
  PlatformBdsLib|$(CHIPSET_PKG)/Library/PlatformBdsLib/PlatformBdsLib.inf
  ResetSystemLib|$(CHIPSET_PKG)/Library/ResetSystemLib/ResetSystemLib.inf
#[-start-130808-IB06720232-remove]#
#  EcLib|$(CHIPSET_PKG)/Library/EcLib/$(EC_NAME)/EcLib.inf
#[-end-130808-IB06720232-remove]#
  SmbusLib|$(CHIPSET_PKG)/Library/SmbusLib/SmbusLib.inf
  PlatformHookLib|$(CHIPSET_PKG)/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  PlatformVfrLib|$(CHIPSET_PKG)/UefiSetupUtilityDxe/PlatformVfrLib.inf
  BaseOemSvcChipsetLib|$(CHIPSET_PKG)/Library/BaseOemSvcChipsetLib/BaseOemSvcChipsetLib.inf
  BaseOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/BaseOemSvcChipsetLib/BaseOemSvcChipsetLibDefault.inf
#[-start-130227-IB05280051-add]#
  TbtLib|$(CHIPSET_PKG)/Library/TbtLib/TbtLib.inf
#[-end-130227-IB05280051-add]#
  StallLib|$(CHIPSET_PKG)/Library/StallLib/SmmStall.inf
  DxeNullLib|$(CHIPSET_PKG)/Library/DxeNullLib/DxeNullLib.inf
#[-start-130416-IB10820284-remove]#
#  AslUpdateLib|$(CHIPSET_PKG)/Library/AslUpdateLib/DxeAslUpdateLib.inf
#[-end-130416-IB10820284-remove]#
  PchAslUpdateLib|$(CHIPSET_PKG)/Library/PchAslUpdateLib/PchAslUpdateLib.inf
  RapidStartCommonLib|$(CHIPSET_PKG)/Library/RapidStartCommonLib/RapidStartCommonLib.inf
  BootGuardRevocationLib|$(CHIPSET_PKG)/Library/BootGuardRevocationLib/Dxe/BootGuardRevocationLib.inf
  AcpiPlatformLib|$(CHIPSET_PKG)/Library/AcpiPlatformLib/AcpiPlatformLib.inf
#[-start-130116-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
  SpsMeChipsetLib|$(CHIPSET_PKG)/Library/SpsMe/SpsMeChipsetLib/SpsMeChipsetLib.inf
#[-start-131203-IB10310040-add]#
  SpsMeLib|$(CHIPSET_PKG)/Library/SpsMe/BaseSpsMeLib/BaseSpsMeLib.inf
#[-end-131203-IB10310040-add]#
!else
  SpsBiosLib|$(CHIPSET_PKG)/Library/SpsBiosLibNull/SpsBiosLibNull.inf
!endif
#[-end-130116-IB10310017-add]#

#[-start-130613-IB05400416-add]#
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
#[-end-130613-IB05400416-add]#

#[-start-140512-IB10300110-add]#
  WatchDogLib|$(CHIPSET_PKG)/Library/WatchDogLib/WatchDogLib.inf
#[-end-140512-IB10300110-add]#

[LibraryClasses.common.SEC]
  PlatformSecLib|$(CHIPSET_PKG)/Library/PlatformSecLib/PlatformSecLib.inf
  BiosIdLib|$(CHIPSET_PKG)/Library/BiosIdLib/BiosIdLib.inf
  SecOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLib.inf
  SecOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLibDefault.inf

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]
  KernelConfigLib|$(CHIPSET_PKG)/Library/PeiKernelConfigLib/PeiKernelConfigLib.inf
#[-start-130114-IB10040020-remove]#
#  PeiChipsetPlatformLib|$(CHIPSET_PKG)/Library/PeiChipsetPlatformLib/PeiChipsetPlatformLib.inf {
#    <BuildOptions>
#    !if $(iFFS_FLAG) == YES
#      *_*_*_CC_FLAGS = -D iFFS_FLAG
#    !endif
#    !if $(XTU_SUPPORT) == YES
#      *_*_*_CC_FLAGS = -D XTU_SUPPORT
#    !endif
#    !if $(USE_FAST_CRISIS_RECOVERY) == YES
#      *_*_*_CC_FLAGS = -D USE_FAST_CRISIS_RECOVERY
#    !endif
#    !if gChipsetPkgTokenSpaceGuid.PcdAntiTheftSupported
#      *_*_*_CC_FLAGS = -D ANTI_THEFT_SUPPORT
#    !endif
#    !if gChipsetPkgTokenSpaceGuid.PcdAntiTheftTestMenuSupported
#      *_*_*_CC_FLAGS = -D ANTI_THEFT_TESTMENU_SUPPORT
#    !endif
#    !if gChipsetPkgTokenSpaceGuid.PcdNvidiaSgSupported
#      *_*_*_CC_FLAGS = -D NVIDIA_SG_SUPPORT
#    !endif
#  }
#  PeiChipsetPlatformLib|$(CHIPSET_PKG)/Library/PeiChipsetPlatformLib/PeiChipsetPlatformLib.inf
#[-end-130114-IB10040020-remove]#
#[-start-130308-IB10820257-remove]#
#  ChipsetLib|$(CHIPSET_PKG)/Library/ChipsetLib/PeiChipsetLib.inf
#[-end-130308-IB10820257-remove]#
  PeiOemSvcChipsetLib|$(CHIPSET_PKG)/Library/PeiOemSvcChipsetLib/PeiOemSvcChipsetLib.inf
  PeiOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/PeiOemSvcChipsetLib/PeiOemSvcChipsetLibDefault.inf
  RapidStartPeiLib|$(CHIPSET_PKG)/Library/RapidStartPlatformLib/Pei/RapidStartPeiLib.inf
  PeiKscLib|$(CHIPSET_PKG)/Library/PeiKscLib/PeiKscLib.inf
#[-start-130322-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
#[-start-131203-IB10310040-remove]#
#  SpsMeLib|$(CHIPSET_PKG)/Library/SpsMe/PeiMeLib/PeiSpsMeLib.inf
#[-end-131203-IB10310040-remove]#
  SpsBiosLib|$(CHIPSET_PKG)/Library/SpsMe/PeiSpsBiosLib/PeiSpsBiosLib.inf
#[-start-131203-IB10310040-add]#
  SpsPolicyLib|$(CHIPSET_PKG)/Library/SpsMe/PeiSpsPolicyLib/PeiSpsPolicyLib.inf
  SpsHeciLib|$(CHIPSET_PKG)/Library/SpsMe/PeiSpsHeciLib/PeiSpsHeciLib.inf
#[-end-131203-IB10310040-add]#
!endif
#[-end-130322-IB10310017-add]#

[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
#[-start-130308-IB10820257-remove]#
#  ChipsetLib|$(CHIPSET_PKG)/Library/ChipsetLib/DxeChipsetLib.inf
#[-end-130308-IB10820257-remove]#
  DxeOemSvcChipsetLib|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf
  DxeOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLibDefault.inf
  RapidStartDxeLib|$(CHIPSET_PKG)/Library/RapidStartPlatformLib/Dxe/RapidStartDxeLib.inf

[LibraryClasses.common.UEFI_DRIVER]
#[-start-130308-IB10820257-remove]#
#  ChipsetPlatformLib|$(CHIPSET_PKG)/Library/DxeChipsetPlatformLib/DxeChipsetPlatformLib.inf
#  ChipsetLib|$(CHIPSET_PKG)/Library/ChipsetLib/DxeChipsetLib.inf
#[-end-130308-IB10820257-remove]#
  DxeOemSvcChipsetLib|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf
  DxeOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLibDefault.inf

[LibraryClasses.common.DXE_DRIVER]
#[-start-131203-IB07390113-remove]#
#  SmbiosMiscLib|MahoBayChipsetPkg/Library/SmbiosMiscLib/SmbiosMiscLib.inf
#[-end-131203-IB07390113-remove]#
#[-start-130115-IB10040020-remove]#
#  DxeChipsetPlatformLib|$(CHIPSET_PKG)/Library/DxeChipsetPlatformLib/DxeChipsetPlatformLib.inf {
#    <BuildOptions>
#    !if $(SWITCHABLE_GRAPHICS_SUPPORT) == YES
#      *_*_*_CC_FLAGS = -D SWITCHABLE_GRAPHICS_SUPPORT
#    !endif
#    !if $(XTU_SUPPORT) == YES
#      *_*_*_CC_FLAGS = -D XTU_SUPPORT
#    !endif
#  }
#  DxeChipsetPlatformLib|$(CHIP SET_PKG)/Library/DxeChipsetPlatformLib/DxeChipsetPlatformLib.inf
#[-end-130115-IB10040020-remove]#
  EfiRegTableLib|$(CHIPSET_PKG)/Library/EfiRegTableLib/EfiRegTableLib.inf
  DxeOemSvcChipsetLib|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf
  DxeOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLibDefault.inf
  RapidStartDxeLib|$(CHIPSET_PKG)/Library/RapidStartPlatformLib/Dxe/RapidStartDxeLib.inf
  DxeKscLib|$(CHIPSET_PKG)/Library/DxeKscLib/DxeKscLib.inf
#[-start-130611-IB11120016-add]#
  DxeIpmiSetupUtilityLib|$(CHIPSET_PKG)/Library/DxeIpmiSetupUtilityLibNull/DxeIpmiSetupUtilityLibNull.inf
#[-end-130611-IB11120016-add]#
#[-start-131011-IB05400449-add]#
  DxeIpmiBdsLib|$(CHIPSET_PKG)/Library/DxeIpmiBdsLibNull/DxeIpmiBdsLibNull.inf
#[-end-131011-IB05400449-add]#
#[-start-130701-IB05160463-add]#
  AslUpdateLib|$(CHIPSET_PKG)/Library/AslUpdateLib/DxeAslUpdateLib.inf
#[-end-130701-IB05160463-add]#
#[-start-130322-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
#[-start-131203-IB10310040-remove]#
#  SpsMeLib|$(CHIPSET_PKG)/Library/SpsMe/DxeMeLib/DxeSpsMeLib.inf
#[-end-131203-IB10310040-remove]#
  SpsBiosLib|$(CHIPSET_PKG)/Library/SpsMe/DxeSpsBiosLib/DxeSpsBiosLib.inf
#[-start-131203-IB10310040-add]#
  SpsPolicyLib|$(CHIPSET_PKG)/Library/SpsMe/DxeSpsPolicyLib/DxeSpsPolicyLib.inf
  SpsHeciLib|$(CHIPSET_PKG)/Library/SpsMe/DxeSpsHeciLib/DxeSpsHeciLib.inf
#[-end-131203-IB10310040-add]#
!endif
#[-end-130322-IB10310017-add]#
#[-start-131202-IB09740048-add]#
!if gInsydeTokenSpaceGuid.PcdH2OConsoleRedirectionSupported != 1
   CrConfigDefaultLib|$(CHIPSET_PKG)/Library/CrConfigDefaultLibNull/CrConfigDefaultLibNull.inf
   CrVfrConfigLib|$(CHIPSET_PKG)/Library/CrVfrConfigLibNull/CrVfrConfigLibNull.inf
!endif 
#[-end-131202-IB09740048-add]#
[LibraryClasses.common.DXE_SMM_DRIVER]
#[-start-130308-IB10820257-remove]#
#  ChipsetLib|$(CHIPSET_PKG)/Library/ChipsetLib/DxeChipsetLib.inf
#[-end-130308-IB10820257-remove]#
  SmmOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLib.inf
  SmmOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLibDefault.inf
  SmmDTSHookLib|$(CHIPSET_PKG)/Library/SmmDTSHookLib/SmmDTSHookLib.inf
  SmmKscLib|$(CHIPSET_PKG)/Library/SmmKsc/SmmKscLib.inf
#[-start-130416-IB10820284-add]#
  AslUpdateLib|$(CHIPSET_PKG)/Library/AslUpdateLib/DxeAslUpdateLib.inf
#[-end-130416-IB10820284-add]#
#[-start-130321-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
#[-start-131203-IB10310040-remove]#
#  SpsMeLib|$(CHIPSET_PKG)/Library/SpsMe/SmmMeLib/SmmSpsMeLib.inf
#[-end-131203-IB10310040-remove]#
  SpsBiosLib|$(CHIPSET_PKG)/Library/SpsMe/SmmSpsBiosLib/SmmSpsBiosLib.inf
#[-start-131203-IB10310040-add]#
  SpsPolicyLib|$(CHIPSET_PKG)/Library/SpsMe/SmmSpsPolicyLib/SmmSpsPolicyLib.inf
  SpsHeciLib|$(CHIPSET_PKG)/Library/SpsMe/SmmSpsHeciLib/SmmSpsHeciLib.inf
#[-end-131203-IB10310040-add]#
!endif
#[-end-130321-IB10310017-add]#

[LibraryClasses.common.COMBINED_SMM_DXE]
  SmmOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLib.inf
  SmmOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLibDefault.inf
#[-start-130227-IB05280051-add]#
  TbtLib|$(CHIPSET_PKG)/Library/TbtLib/TbtLib.inf
#[-end-130227-IB05280051-add]#

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]
#[-start-140714-IB02040415-remove]#
#[-start-130129-IB05280043-modify]#
  #
  # The PCD for Microsoft HID over I2C devices, the bits definition below
  #
  # Bit  0 ~  6 : Slave address
  # Bit  7      : Interrupt GPIO pin active level, 0 = low active, 1 = high active
  # Bit  8 ~ 15 : Interrupt GPIO pin number
  # Bit 16 ~ 31 : HID descriptor register number
  # Bit 32 ~ 47 : Device type,
  #               0x0000 -> (AutoDetect)
  #               0x0d00 -> Touch panel
  #               0x0102 -> Mouse
  #               0x0106 -> Keyboard
  # Bit 48 ~ 51 : Host controller number
  #               0x00      -> (AutoDetect)
  #               0x01~0x0f -> I2C host controller 0~14 (One based)
  # Bit 52 ~ 55 : Device Speed Override
  #               0x01      -> V_SPEED_STANDARD
  #               0x02      -> V_SPEED_FAST
  #               0x03      -> V_SPEED_HIGH
  #
  #gInsydeTokenSpaceGuid.PcdI2cHidDevice01|0x0003010200013715
  #gInsydeTokenSpaceGuid.PcdI2cHidDevice02|0x00030d0000013210
#[-end-130129-IB05280043-modify]#
#[-end-140714-IB02040415-remove]#

#[-start-130716-IB07390107-add]#
[PcdsFixedAtBuild, PcdsPatchableInModule]
  ## Stack size in the temporary RAM.
  #   0 means half of PcdTemporaryRamSizeWhenRecovery.
  gInsydeTokenSpaceGuid.PcdPeiTemporaryRamStackSizeWhenRecovery|0x8000
#[-end-130716-IB07390107-add]#

[PcdsFixedAtBuild]

#[-start-140512-IB10300110-add]#
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported != 1
  gChipsetPkgTokenSpaceGuid.PcdH2OWatchDogSupported|FALSE
!else
  gChipsetPkgTokenSpaceGuid.PcdH2OWatchDogSupported|FALSE
!endif
#[-end-140512-IB10300110-add]#

#[-start-130716-IB07390107-add]#
#[-start-131029-IB06720238-modify]#
  gInsydeTokenSpaceGuid.PcdTemporaryRamSizeWhenRecovery|0x40000
#[-end-131029-IB06720238-modify]#
#[-end-130716-IB07390107-add]#

  gInsydeTokenSpaceGuid.PcdUmaDeviceNumber|0x02                 # 0xFF no Uma port.
  gInsydeTokenSpaceGuid.PcdPegDeviceNumber|0x01                 # 0xFF no Peg port.

#[-start-130122-IB05280037-modify]#
# PcdDefaultBoardId
#  2-Chip
#   BASKING_RIDGE                           = 0x03
#   GRAYS_REEF                              = 0x01
#  Ult
#   WHITE_TIP_MOUNTAIN                      = 0x20
#   SAW_TOOTH_PEAK                          = 0x24
#  For OEM MB
#   PcdDefaultBoardId                       =  0xFF
#   Then Chipse program GPIO code will run PcdPeiGpioTable1

!if $(ULT_SUPPORT) == YES
  gChipsetPkgTokenSpaceGuid.PcdDefaultBoardId|0x20
!else
  gChipsetPkgTokenSpaceGuid.PcdDefaultBoardId|0x01
!endif
#[-end-130122-IB05280037-modify]#

#
#  PEI RC Policy default value override
#
!if gChipsetPkgTokenSpaceGuid.PcdAntiTheftSupported
  gChipsetPkgTokenSpaceGuid.PcdAMTPlatformPolicyPpiManageabilityMode|0x00000000
!endif
!if gChipsetPkgTokenSpaceGuid.PcdUltFlag
  gChipsetPkgTokenSpaceGuid.PcdPegGpioDataGpioSupport|FALSE
  gChipsetPkgTokenSpaceGuid.PcdDevice26InterruptPinRegisterValue|0x00000000
!endif
!if gChipsetPkgTokenSpaceGuid.PcdMrcRmt
  gChipsetPkgTokenSpaceGuid.PcdMemConfigSerialDebug|0x00000003
  gChipsetPkgTokenSpaceGuid.PcdMemConfigRMT|0x00000001
  gChipsetPkgTokenSpaceGuid.PcdMemConfigMEMTST|0x00000001
  gChipsetPkgTokenSpaceGuid.PcdMemConfigRmtBdatEnable|TRUE
  gChipsetPkgTokenSpaceGuid.PcdMemConfigMrcFastBoot|FALSE
!endif
#[-start-121113-IB06150257-modify]#
  gChipsetPkgTokenSpaceGuid.PcdNemDataStackSize|262144     #0x40000
#[-end-121113-IB06150257-modify]#
!if gChipsetPkgTokenSpaceGuid.PcdMe5MbSupported
  gChipsetPkgTokenSpaceGuid.PcdGlobalNvsAreaIdeREnable|0x01
!endif
  #
  # CMOS Debug Code Table
  #
  gInsydeTokenSpaceGuid.PcdPeiChipsetDebugCodeTable|{ \
0xE9, 0x71, 0x59, 0x14, 0x52, 0xAD, 0x94, 0x40, 0xA8, 0xC8, 0xBE, 0x5B, 0x3F, 0xEC, 0xC8, 0x2D, 0x40, 0x02, \ #CpuPeim
0x71, 0xBD, 0x66, 0xC8, 0x79, 0x7C, 0xF1, 0x4B, 0xA9, 0x3B, 0x06, 0x6B, 0x83, 0x0D, 0x8F, 0x9A, 0x40, 0x06, \ #Init MP for S3
0x50, 0x4F, 0xB7, 0xAB, 0x2D, 0xFD, 0x72, 0x40, 0xA3, 0x21, 0xCA, 0xFC, 0x72, 0x97, 0x7E, 0xFA, 0x40, 0x07, \ #PeiSmmRelocate
0xE0, 0xC8, 0x9E, 0xA8, 0xA1, 0x0B, 0xaa, 0x40, 0xA0, 0x3E, 0xAB, 0xDD, 0xA5, 0x29, 0x5C, 0xDE, 0x43, 0x02, \ #PciExpress
0xEA, 0x25, 0xEE, 0xD4, 0x48, 0x0B, 0xae, 0x43, 0xA0, 0x16, 0x4D, 0x6E, 0x8B, 0x6C, 0x43, 0xB3, 0x43, 0x04, \ #CantigaMemoryInit
0xBF, 0x35, 0x1B, 0x92, 0x55, 0x02, 0x22, 0x47, 0xBF, 0x5A, 0x5B, 0x8B, 0x69, 0x09, 0x35, 0x93, 0x46, 0x02, \ #IchInit
0x77, 0xF7, 0x3D, 0x64, 0x12, 0xF3, 0xed, 0x42, 0x81, 0xCC, 0x1B, 0x1F, 0x57, 0xE1, 0x8A, 0xD6, 0x46, 0x03, \ #IchSmbusArpDisabled
0x7B, 0x2B, 0x44, 0xC5, 0xCF, 0x58, 0x75, 0x43, 0xB1, 0x52, 0x43, 0x83, 0x13, 0x26, 0x07, 0xFC, 0x46, 0x05, \ #For recover PEIM
0x22, 0x97, 0x61, 0xC3, 0x37, 0xEF, 0x05, 0x46, 0x8C, 0x35, 0xC7, 0xA3, 0x7B, 0x51, 0xED, 0xD2, 0x49, 0x02, \ #ClockGen and ck505
0x4D, 0x66, 0xEE, 0x50, 0x03, 0x77, 0xc3, 0x42, 0x9E, 0x69, 0x8C, 0x89, 0xDE, 0x70, 0xD1, 0xD5, 0x4C, 0x02, \ #SioInit
0x07, 0xB1, 0x78, 0x8A, 0xDD, 0x0F, 0xc8, 0x4c, 0xB7, 0xBA, 0xDC, 0x3E, 0x13, 0xCB, 0x85, 0x24, 0x4C, 0x03, \ #PeiCpuIoPciCfg
0xFC, 0x27, 0x50, 0xA8, 0x09, 0x0E, 0xA9, 0x4F, 0xA4, 0x07, 0xCA, 0xD2, 0x06, 0xFB, 0x4F, 0x1D, 0x4C, 0x04, \ #PlatformStage1
0xF5, 0xBB, 0x91, 0x11, 0xB9, 0xDC, 0xF4, 0x44, 0x82, 0x7E, 0x95, 0x35, 0x97, 0x44, 0xC9, 0x87, 0x4C, 0x05, \ #PlatformStage2
0x9C, 0xE0, 0x79, 0x54, 0x74, 0x2E, 0x1b, 0x48, 0x89, 0xF8, 0xB0, 0x17, 0x2E, 0x38, 0x8D, 0x1F, 0x4F, 0x01, \ #Start Watch Dog PEIM driver
0xf3, 0xaf, 0x62, 0x48, 0x7c, 0x66, 0x58, 0x54, 0xb2, 0x74, 0xa1, 0xc6, 0x2d, 0xf8, 0xba, 0x80, 0x4F, 0x02, \ #Framework PEIM to HECI
0x17, 0x86, 0x9D, 0xCA, 0x52, 0xD6, 0x3b, 0x40, 0xB6, 0xC5, 0xBA, 0x47, 0x57, 0x01, 0x16, 0xAD, 0x50, 0x01, \ #Txt PEIM
#[-start-130115-IB10040020-remove]#
#0x06, 0x9D, 0x8D, 0xDA, 0x37, 0x1F, 0xB5, 0x4B, 0xBE, 0xA0, 0xCF, 0x8B, 0x4F, 0xE3, 0x81, 0x38, 0x51, 0x08, \ #ChipsetPlatformLibServicesPei
#[-end-130115-IB10040020-remove]#
0x64, 0x58, 0x26, 0xAE, 0x5D, 0xCF, 0xa8, 0x41, 0x91, 0x3D, 0x71, 0xC1, 0x55, 0xE7, 0x64, 0x42, 0x51, 0x0D, \ #CpuIoPei
0x38, 0x58, 0xFC, 0xE2, 0xA9, 0x16, 0xD7, 0x4E, 0x96, 0xE0, 0x9A, 0x75, 0xF5, 0xAF, 0x71, 0x1D, 0x51, 0x0F, \ #EmuPeiHelperPei
0x91, 0x63, 0xA9, 0xEA, 0xE3, 0x9B, 0x88, 0x44, 0x8A, 0xF3, 0xB3, 0xE6, 0xEF, 0xD1, 0x57, 0xD5, 0x51, 0x10, \ #EmuSecPei
0x72, 0xAD, 0x6F, 0xA5, 0x64, 0xA2, 0x70, 0x43, 0x85, 0xC5, 0x00, 0x58, 0x46, 0x54, 0xDC, 0xE2, 0x51, 0x11, \ #InstallVerbTablePei
0x21, 0x10, 0x3d, 0x6d, 0xfc, 0xe7, 0xc2, 0x42, 0xb3, 0x01, 0xec, 0xe9, 0xdb, 0x19, 0x82, 0x87, 0x51, 0x12, \ #LegacySpeakerPei
0x78, 0x7F, 0x8B, 0x1C, 0x99, 0x16, 0xe6, 0x40, 0xAF, 0x33, 0x9B, 0x99, 0x5D, 0x16, 0xB0, 0x43, 0x51, 0x15, \ #PiSmmCommunicationPei
0xA7, 0xB9, 0xB3, 0xFD, 0x82, 0x1E, 0x77, 0x4C, 0x9C, 0x6C, 0x43, 0x05, 0xC8, 0x51, 0xF2, 0x53, 0x51, 0x1A, \ #ProgClkGenPeim
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} #EndEntry

  gInsydeTokenSpaceGuid.PcdDxeChipsetDebugCodeTable|{ \
0xCB, 0x71, 0xD1, 0x62, 0xCD, 0x78, 0x80, 0x44, 0x86, 0x78, 0xC6, 0xA2, 0xA7, 0x97, 0xA8, 0xDE, 0x41, 0x01, \ #MpCpu
0x9F, 0x59, 0xE7, 0xBA, 0x6B, 0x3C, 0xb7, 0x43, 0xBD, 0xF0, 0x9C, 0xE0, 0x7A, 0xA9, 0x1A, 0xA6, 0x41, 0x02, \ #CpuIo
0x5A, 0x57, 0x52, 0x55, 0x00, 0x7E, 0x61, 0x4D, 0xA3, 0xA4, 0xF7, 0x54, 0x73, 0x51, 0xB4, 0x9E, 0x42, 0x02, \ #SmmBase
0x7D, 0x5D, 0xC5, 0x9C, 0xFF, 0xFB, 0x1c, 0x43, 0xBC, 0x14, 0x33, 0x4E, 0xAE, 0xA6, 0x05, 0x2B, 0x42, 0x04, \ #SmmCoreDispatcher
0xEE, 0x72, 0xED, 0x7F, 0x70, 0x01, 0x14, 0x48, 0x98, 0x78, 0xA8, 0xFB, 0x18, 0x64, 0xDF, 0xAF, 0x42, 0x06, \ #SmmRelocate
0x15, 0xE2, 0x3B, 0x8D, 0xF6, 0xD6, 0x64, 0x42, 0xBE, 0xA6, 0x28, 0x07, 0x3F, 0xB1, 0x3A, 0xEA, 0x42, 0x07, \ #SmmThunk
0x03, 0xF2, 0x51, 0x31, 0x6B, 0x54, 0x83, 0x46, 0xAD, 0x72, 0xD8, 0xB1, 0x6B, 0xC7, 0xD7, 0x5E, 0x44, 0x02, \ #DxeMchInit
0xB9, 0x56, 0x67, 0x8D, 0x5E, 0xE5, 0x6a, 0x4d, 0xA3, 0xA5, 0x5E, 0x4D, 0x72, 0xDD, 0xF7, 0x72, 0x44, 0x03, \ #PciHostBridge
0xA4, 0x91, 0xA7, 0xED, 0x4B, 0x38, 0x06, 0x49, 0x89, 0xD1, 0xEE, 0x4D, 0xC9, 0x72, 0xFE, 0x4A, 0x44, 0x04, \ #SmbiosMemory
0xA8, 0x04, 0xC2, 0xCD, 0xDB, 0xF5, 0xf6, 0x44, 0xBD, 0xC6, 0x44, 0x6E, 0xEE, 0x54, 0x31, 0x6F, 0x44, 0x05, \ #IgdOpRegion
0x05, 0x8F, 0xFC, 0xEF, 0x26, 0xB5, 0xb5, 0x4e, 0xB3, 0x6B, 0x8C, 0xD8, 0x89, 0x92, 0x3C, 0x0C, 0x44, 0x06, \ #LegacyRegion
0x7C, 0xB8, 0x9C, 0xB0, 0xD8, 0x67, 0x2b, 0x41, 0xBB, 0x9D, 0x9F, 0x4B, 0x21, 0x4D, 0x72, 0x0A, 0x44, 0x07, \ #VTd
0x7A, 0xE3, 0xF1, 0x90, 0x6B, 0xA2, 0x50, 0x4e, 0x9A, 0x19, 0xF9, 0xDD, 0x65, 0xF9, 0xF1, 0x73, 0x44, 0x08, \ #HDCP
0xDA, 0x51, 0x02, 0xF5, 0x08, 0xB6, 0x77, 0x41, 0x99, 0x1A, 0x7D, 0xF4, 0x27, 0x8C, 0x97, 0x53, 0x44, 0x09, \ #SmmAccess
0x81, 0x17, 0xA0, 0x3A, 0x40, 0x5E, 0x24, 0x45, 0xB6, 0xCA, 0x1A, 0xCB, 0x3B, 0x45, 0x57, 0x8C, 0x45, 0x02, \ #HDCPSMI
0xCB, 0x4E, 0x09, 0x21, 0x20, 0x9F, 0x81, 0x47, 0xAE, 0x4B, 0x50, 0x72, 0x8B, 0x38, 0x9A, 0x6E, 0x47, 0x02, \ #DxeIchInit
0x2B, 0x94, 0x65, 0xBB, 0x1F, 0x52, 0xc3, 0x4e, 0xBA, 0xF9, 0xA9, 0x25, 0x40, 0xCF, 0x60, 0xD2, 0x47, 0x04, \ #SataController
0xA6, 0xD8, 0x52, 0xE0, 0x4A, 0x22, 0x32, 0x4c, 0x8D, 0x37, 0x2E, 0x0A, 0xE1, 0x62, 0x36, 0x4D, 0x47, 0x05, \ #DxeIchSmbus
0xF7, 0xD9, 0xBA, 0xA0, 0x78, 0xAB, 0x1b, 0x49, 0xB5, 0x83, 0xC5, 0x2B, 0x7F, 0x84, 0xB9, 0xE0, 0x47, 0x06, \ #SmmControl
0xAA, 0x98, 0x01, 0x6F, 0x1D, 0x1F, 0x6D, 0x42, 0xAE, 0x3E, 0x39, 0xAB, 0x63, 0x3F, 0xCC, 0x28, 0x47, 0x07, \ #Cf9Reset
0xF9, 0x18, 0xC4, 0xC1, 0x1D, 0x59, 0x1c, 0x46, 0x82, 0xA2, 0xB9, 0xCD, 0x96, 0xDF, 0xEA, 0x86, 0x47, 0x08, \ #IntelIchLegacyInterrupt
0xDB, 0x75, 0xCB, 0x90, 0xFC, 0x71, 0x9d, 0x48, 0xAA, 0xCF, 0x94, 0x34, 0x77, 0xEC, 0x72, 0x12, 0x47, 0x09, \ #SmartTimer
0xEA, 0xC6, 0x94, 0xC1, 0x8C, 0xB6, 0x81, 0x49, 0xB6, 0x4B, 0x9B, 0xD2, 0x71, 0x47, 0x4B, 0x20, 0x47, 0x0A, \ #RuntimeDxeIchSpi
0x40, 0x76, 0x1B, 0xFC, 0x66, 0x34, 0x06, 0x4c, 0xB1, 0xCC, 0x1C, 0x93, 0x53, 0x94, 0xB5, 0xC2, 0x47, 0x0B, \ #IchSerialGpio
0xD9, 0x1D, 0x52, 0x75, 0xF3, 0xDB, 0x4d, 0x4f, 0x80, 0xB7, 0xF5, 0x14, 0x4F, 0xD7, 0x4B, 0xF8, 0x47, 0x0C, \ #AhciBus
0x53, 0xED, 0xD6, 0xB0, 0x44, 0xB8, 0xf5, 0x43, 0xBD, 0x2F, 0x61, 0x09, 0x52, 0x64, 0xE7, 0x7E, 0x48, 0x01, \ #IchSmmDispatcher
0xDF, 0xED, 0x74, 0x23, 0x03, 0xF2, 0xc0, 0x4f, 0xA2, 0x0E, 0x61, 0xBA, 0xD7, 0x30, 0x89, 0xD6, 0x48, 0x03, \ #IoTrap
0x13, 0x1E, 0x5B, 0xDE, 0x27, 0x14, 0x3f, 0x45, 0xAC, 0xC4, 0xCD, 0xEB, 0x0E, 0x15, 0x79, 0x7E, 0x48, 0x04, \ #IchS3Save
0x0A, 0x33, 0xA9, 0x07, 0x47, 0xF3, 0xd4, 0x11, 0x9A, 0x49, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D, 0x4A, 0x02, \ #LegacyMetronome
0x65, 0x7B, 0x8D, 0x37, 0xA9, 0x8D, 0x73, 0x47, 0xB6, 0xE4, 0xA4, 0x78, 0x26, 0xA8, 0x33, 0xE1, 0x4A, 0x03, \ #PcRtc
0x08, 0x42, 0xCA, 0x79, 0xA1, 0xBB, 0x9a, 0x4a, 0x84, 0x56, 0xE1, 0xE6, 0x6A, 0x81, 0x48, 0x4E, 0x4A, 0x04, \ #Legacy8259
0x90, 0x56, 0x83, 0x27, 0xBE, 0xBE, 0x29, 0x49, 0xBC, 0x60, 0x94, 0xD7, 0x7C, 0x68, 0x62, 0xF7, 0x4A, 0x05, \ #IsaAcpiDriver (Sio1007)
0xAC, 0x91, 0xF6, 0xA6, 0xC8, 0x31, 0x44, 0x44, 0x85, 0x4C, 0xE2, 0xC1, 0xA6, 0x95, 0x0F, 0x92, 0x4D, 0x01, \ #Bds
0x9B, 0xDD, 0x67, 0x19, 0x2C, 0xB7, 0x28, 0x43, 0x8C, 0x80, 0xD4, 0xAC, 0xFC, 0x83, 0xFD, 0xF8, 0x4D, 0x02, \ #PciHotPlug
0xFE, 0x42, 0x35, 0xFE, 0xD3, 0xC1, 0xF8, 0x4E, 0x65, 0x7C, 0x80, 0x48, 0x60, 0x6F, 0xF6, 0x70, 0x4D, 0x03, \ #SetupUtility
0x2A, 0xD1, 0xED, 0x94, 0x9B, 0x41, 0x7F, 0x44, 0x94, 0x34, 0x9B, 0x3B, 0x70, 0x78, 0x39, 0x03, 0x4D, 0x04, \ #DxePlatform
0xA3, 0x4F, 0xA1, 0xFD, 0xFC, 0xAF, 0x9a, 0x46, 0xB7, 0xBB, 0x34, 0xBC, 0xDD, 0x4A, 0xC0, 0x96, 0x4D, 0x05, \ #PlatformIde
0xB6, 0x99, 0x0C, 0xEF, 0xD3, 0xB1, 0x25, 0x40, 0x94, 0x05, 0xBF, 0x6A, 0x56, 0x0F, 0xE0, 0xE0, 0x4D, 0x07, \ #MiscSubclass
0x99, 0x40, 0xC0, 0xAF, 0x39, 0x0D, 0x5d, 0x40, 0xBE, 0x46, 0x84, 0x6F, 0x08, 0xC5, 0x1A, 0x31, 0x4D, 0x08, \ #AcpiPlatform
0x34, 0x2A, 0x93, 0xE3, 0x29, 0x57, 0x24, 0x4F, 0x9F, 0xB1, 0xD7, 0x40, 0x9B, 0x45, 0x6A, 0x15, 0x4D, 0x09, \ #OemBadgingSupport
0xF4, 0xFF, 0x4C, 0xF8, 0x1E, 0x51, 0xC8, 0x41, 0xB8, 0x29, 0x51, 0x9F, 0x51, 0x52, 0xF4, 0x44, 0x4D, 0x0A, \ #LegacyBiosPlatform
0x64, 0x1B, 0x44, 0xE2, 0xF4, 0x7E, 0xfe, 0x41, 0xB3, 0xA3, 0x8C, 0xAA, 0x7F, 0x8D, 0x30, 0x17, 0x4D, 0x0B, \ #PciPlatform
0x37, 0x0A, 0xC2, 0x99, 0x2A, 0x04, 0xe2, 0x46, 0x80, 0xF4, 0xE4, 0x02, 0x7F, 0xDB, 0xC8, 0x6F, 0x4E, 0x01, \ #SmmPlatform
0x9E, 0x00, 0xA6, 0x77, 0x6E, 0x11, 0x4D, 0x46, 0x8E, 0xF8, 0xB3, 0x52, 0x01, 0xA0, 0x22, 0xDD, 0x4E, 0x05, \ #DigitalThermalSensor
0x4c, 0x1b, 0x73, 0xf7, 0xa2, 0x58, 0xf4, 0x4d, 0x89, 0x80, 0x56, 0x45, 0xd3, 0x9e, 0xce, 0x58, 0x4E, 0x06, \ #ProcessorPowerManagement
0x27, 0x6E, 0xAD, 0x76, 0x81, 0x61, 0x69, 0x47, 0x91, 0x88, 0x23, 0x3C, 0xEA, 0x0A, 0x81, 0xF3, 0x4F, 0x03, \ #Create a dataHub for AMT
0x63, 0xC1, 0x45, 0x79, 0xD4, 0x58, 0xa8, 0x4d, 0xA5, 0x7B, 0x08, 0x11, 0x30, 0xAF, 0x3D, 0x21, 0x4F, 0x04, \ #Heci driver core
0x69, 0xF9, 0x39, 0xD7, 0x2D, 0xFB, 0xc2, 0x4b, 0xAF, 0xE7, 0x08, 0x13, 0x27, 0xD3, 0xFE, 0xDE, 0x4F, 0x05, \ #AMT driver
0x44, 0x66, 0xE7, 0x55, 0xA5, 0x78, 0x82, 0x4a, 0xA9, 0x00, 0x71, 0x26, 0xA5, 0x79, 0x88, 0x92, 0x4F, 0x06, \ #Heci driver
0xF8, 0xC9, 0xC1, 0x32, 0x3F, 0xD5, 0xc8, 0x41, 0x94, 0xD0, 0xF6, 0x73, 0x9F, 0x23, 0x10, 0x11, 0x4F, 0x07, \ #AMT Bios Extensions
0x07, 0xD0, 0xF2, 0xC4, 0xFD, 0x37, 0x2d, 0x42, 0xB6, 0x3D, 0x7E, 0xD7, 0x38, 0x86, 0xE6, 0xCA, 0x4F, 0x08, \ #IDE_CONTROLLER_INIT protocol
0x6d, 0x40, 0xc6, 0x33, 0x6b, 0x2f, 0xb5, 0x41, 0x87, 0x05, 0x52, 0xba, 0xfb, 0x63, 0x3c, 0x09, 0x4F, 0x09, \ #ASF messages
0xF2, 0x30, 0xFA, 0xA7, 0x70, 0x2D, 0xE6, 0x4E, 0xB1, 0xF2, 0x44, 0xEC, 0xD5, 0x05, 0x60, 0x11, 0x4F, 0x0A, \ #AMT module on BDS
0x99, 0x2B, 0x14, 0xFB, 0x57, 0xDF, 0xcb, 0x46, 0xBC, 0x69, 0x0B, 0xF8, 0x58, 0xA7, 0x34, 0xF9, 0x4F, 0x0B, \ #AMT PCI serial
0x7E, 0xCE, 0x78, 0xEB, 0x07, 0x41, 0xf5, 0x4e, 0x86, 0xCB, 0x22, 0xE8, 0xD8, 0xAC, 0x49, 0x50, 0x4F, 0x0C, \ #SmmAsfInit
0xBC, 0x78, 0xEF, 0x69, 0x71, 0x3B, 0xcc, 0x4e, 0x83, 0x4F, 0x3B, 0x74, 0xF9, 0x14, 0x84, 0x30, 0x4F, 0x0D, \ #SmmFlashWriteProtect
0x22, 0x7E, 0x91, 0xFF, 0x28, 0xA2, 0x8d, 0x44, 0xBD, 0xAA, 0x68, 0xEF, 0xCC, 0xDD, 0xA5, 0xD3, 0x50, 0x02, \ #TXT dxe driver
0xec, 0x5d, 0x76, 0xf2, 0x41, 0x6b, 0xd5, 0x11, 0x8e, 0x71, 0x00, 0x90, 0x27, 0x07, 0xb3, 0x5e, 0x51, 0x01, \ #8254Timer
0x1F, 0xCE, 0xB2, 0x4F, 0x3A, 0x1A, 0xe3, 0x42, 0xBD, 0x0C, 0x7B, 0x84, 0xF9, 0x54, 0x18, 0x9A, 0x51, 0x03, \ #AcpiCallBacksSmm
0x12, 0x39, 0x93, 0xcb, 0x8f, 0xdf, 0x05, 0x43, 0xb1, 0xf9, 0x7b, 0x44, 0xfa, 0x11, 0x39, 0x5c, 0x51, 0x04, \ #AcpiPlatformDxe
0xC1, 0x3E, 0x91, 0xA8, 0x0C, 0xC0, 0x3a, 0x4c, 0xA2, 0x45, 0x07, 0x7C, 0x0C, 0xA3, 0x57, 0x38, 0x51, 0x05, \ #AspmOverrideDxe
0xE3, 0xF4, 0xD4, 0xC7, 0xEA, 0xDA, 0xb0, 0x40, 0x88, 0x46, 0xF4, 0xCA, 0xF3, 0x13, 0x5C, 0xE8, 0x51, 0x06, \ #BiosProtectDxe
#[-start-130115-IB10040020-remove]#
#0x1D, 0x3A, 0x3F, 0xD0, 0x8E, 0x08, 0xC8, 0x46, 0xA9, 0xFB, 0x82, 0x09, 0x77, 0x0F, 0x2C, 0xE2, 0x51, 0x07, \ #ChipsetPlatformLibServicesDxe
#[-end-130115-IB10040020-remove]#
0xEB, 0x8B, 0x35, 0xB7, 0x52, 0x6A, 0x50, 0x4D, 0x98, 0xF9, 0x7E, 0xDD, 0x70, 0xB4, 0xB3, 0x20, 0x51, 0x09, \ #CommonPciPlatformDxe
0xE7, 0x1F, 0x9B, 0xA1, 0xBC, 0xC1, 0xF8, 0x49, 0x87, 0x5F, 0x54, 0xA5, 0xD5, 0x42, 0x44, 0x3F, 0x51, 0x0A, \ #CpuIo2Dxe
0xD8, 0xE2, 0x7E, 0xA4, 0x0E, 0xF6, 0xfd, 0x42, 0x8E, 0x58, 0x7B, 0xD6, 0x5E, 0xE4, 0xC2, 0x9B, 0x51, 0x0B, \ #CpuIo2Smm
0x27, 0x2F, 0x45, 0x26, 0xDE, 0x45, 0x94, 0x4A, 0x80, 0x7A, 0x0E, 0x6F, 0xDC, 0x1C, 0xB9, 0x62, 0x51, 0x0E, \ #EmuPeiGateDxe
0x2A, 0x9E, 0x73, 0x51, 0x22, 0xA0, 0x73, 0x4D, 0xAD, 0xB9, 0x91, 0xF0, 0xC9, 0xBC, 0x71, 0x42, 0x51, 0x13, \ #MpServicesOnFrameworkMpServicesThunk
0xA8, 0x35, 0x1F, 0xE2, 0xFF, 0x42, 0x50, 0x40, 0x82, 0xD6, 0x93, 0xF7, 0xCD, 0xFA, 0x70, 0x73, 0x51, 0x16, \ #PiSmmCommunicationSmm
0xF5, 0x0E, 0xFF, 0xA3, 0x28, 0x0C, 0xf5, 0x42, 0xB5, 0x44, 0x8C, 0x7D, 0xE1, 0xE8, 0x00, 0x14, 0x51, 0x17, \ #PiSmmCpuDxeSmm
0x61, 0xEA, 0xBD, 0x96, 0x64, 0xC3, 0x13, 0x45, 0xB6, 0xB3, 0x03, 0x7E, 0x9A, 0xD5, 0x4C, 0xE4, 0x51, 0x1B, \ #SetSsidSvidDxe
0xB6, 0x99, 0x0C, 0xEF, 0xD3, 0xB1, 0x25, 0x40, 0x94, 0x05, 0xBF, 0x6A, 0x56, 0x0F, 0xE0, 0xE0, 0x51, 0x1C, \ #SmbiosMiscDxe
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} #EndEntry

[PcdsDynamicDefault]
#[-start-130313-IB05280055-modify]#
## SharkBay White Tip Mountain GPIO Table Data
  gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable3|{ \
#  Pin0      Pin1      Pin2      Pin3
#[0]  [1]  [0]  [1]  [0]  [1]  [0]  [1]
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin0~Pin3
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin4~Pin7
0x33,0x01,0x23,0x01,0x13,0x00,0x24,0x01, \  #Pin8~Pin11
0x24,0x00,0x25,0x08,0x33,0x02,0x21,0x08, \  #Pin12~Pin15
0x25,0x08,0x33,0x01,0x24,0x01,0x24,0x00, \  #Pin16~Pin19
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin20~Pin23
0x23,0x00,0x25,0x08,0x23,0x00,0x13,0x01, \  #Pin24~Pin27
0x21,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin28~Pin31
0x24,0x00,0x24,0x00,0x25,0x08,0x24,0x00, \  #Pin32~Pin35
0x33,0x01,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin36~Pin39
0x24,0x00,0x24,0x00,0x24,0x00,0x25,0x08, \  #Pin40~Pin43
0x25,0x08,0x33,0x01,0x25,0x08,0x33,0x00, \  #Pin44~Pin47
0x25,0x08,0x25,0x01,0x23,0x01,0x23,0x04, \  #Pin48~Pin51
0x25,0x08,0x23,0x04,0x23,0x01,0x23,0x01, \  #Pin52~Pin55
0x25,0x08,0x21,0x08,0x23,0x01,0x25,0x08, \  #Pin56~Pin59
0x21,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin60~Pin63
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin64~Pin67
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin68~Pin71
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin72~Pin75
0x25,0x08,0x21,0x00,0x23,0x01,0x33,0x01, \  #Pin76~Pin79
0x23,0x01,0x24,0x00,0x24,0x00,0x21,0x08, \  #Pin80~Pin83
0x21,0x08,0x21,0x08,0x21,0x08,0x21,0x08, \  #Pin84~Pin87
0x25,0x08,0x25,0x08,0x21,0x08,0x24,0x00, \  #Pin88~Pin91
0x24,0x00,0x24,0x00,0x24,0x00}              #Pin92~Pin94
## SharkBay Sawtooth Peak GPIO Table Data
  gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable4|{ \
#  Pin0      Pin1      Pin2      Pin3
#[0]  [1]  [0]  [1]  [0]  [1]  [0]  [1]
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin0~Pin3
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin4~Pin7
0x33,0x01,0x23,0x01,0x13,0x00,0x24,0x01, \  #Pin8~Pin11
0x24,0x00,0x25,0x08,0x33,0x02,0x21,0x08, \  #Pin12~Pin15
0x25,0x08,0x33,0x01,0x24,0x01,0x24,0x00, \  #Pin16~Pin19
0x24,0x00,0x24,0x00,0x24,0x08,0x24,0x00, \  #Pin20~Pin23
0x23,0x00,0x25,0x08,0x23,0x00,0x13,0x01, \  #Pin24~Pin27
0x21,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin28~Pin31
0x24,0x00,0x24,0x00,0x33,0x00,0x24,0x00, \  #Pin32~Pin35
0x33,0x01,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin36~Pin39
0x24,0x00,0x24,0x00,0x24,0x00,0x25,0x08, \  #Pin40~Pin43
0x25,0x08,0x33,0x01,0x25,0x08,0x33,0x00, \  #Pin44~Pin47
0x25,0x08,0x21,0x01,0x23,0x01,0x33,0x01, \  #Pin48~Pin51
0x25,0x08,0x23,0x00,0x33,0x01,0x23,0x01, \  #Pin52~Pin55
0x25,0x08,0x21,0x08,0x23,0x01,0x25,0x08, \  #Pin56~Pin59
0x21,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin60~Pin63
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin64~Pin67
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin68~Pin71
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin72~Pin75
0x25,0x08,0x25,0x08,0x23,0x01,0x33,0x01, \  #Pin76~Pin79
0x23,0x01,0x24,0x00,0x24,0x00,0x21,0x08, \  #Pin80~Pin83
0x21,0x08,0x21,0x08,0x21,0x08,0x21,0x08, \  #Pin84~Pin87
0x25,0x08,0x25,0x08,0x21,0x08,0x24,0x00, \  #Pin88~Pin91
0x24,0x00,0x24,0x00,0x24,0x00}              #Pin92~Pin94
## SharkBay Harris Beach GPIO Table Data
  gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable5|{ \
#  Pin0      Pin1      Pin2      Pin3
#[0]  [1]  [0]  [1]  [0]  [1]  [0]  [1]
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin0~Pin3
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin4~Pin7
0x24,0x00,0x23,0x01,0x13,0x01,0x24,0x00, \  #Pin8~Pin11
0x24,0x00,0x25,0x08,0x23,0x04,0x24,0x00, \  #Pin12~Pin15
0x24,0x00,0x23,0x01,0x24,0x01,0x24,0x00, \  #Pin16~Pin19
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin20~Pin23
0x23,0x01,0x25,0x08,0x23,0x01,0x13,0x01, \  #Pin24~Pin27
0x25,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin28~Pin31
0x24,0x00,0x24,0x00,0x25,0x08,0x24,0x00, \  #Pin32~Pin35
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin36~Pin39
0x24,0x00,0x24,0x00,0x24,0x00,0x25,0x08, \  #Pin40~Pin43
0x25,0x08,0x24,0x00,0x25,0x08,0x24,0x00, \  #Pin44~Pin47
0x24,0x00,0x25,0x00,0x23,0x04,0x24,0x00, \  #Pin48~Pin51
0x25,0x08,0x23,0x01,0x33,0x01,0x23,0x04, \  #Pin52~Pin55
0x24,0x00,0x25,0x08,0x23,0x01,0x24,0x08, \  #Pin56~Pin59
0x25,0x08,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin60~Pin63
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin64~Pin67
0x24,0x00,0x24,0x00,0x23,0x01,0x24,0x00, \  #Pin68~Pin71
0x24,0x00,0x24,0x00,0x24,0x00,0x24,0x00, \  #Pin72~Pin75
0x25,0x08,0x33,0x01,0x22,0x01,0x32,0x01, \  #Pin76~Pin79
0x22,0x01,0x24,0x00,0x24,0x00,0x25,0x08, \  #Pin80~Pin83
0x25,0x08,0x20,0x08,0x21,0x08,0x25,0x08, \  #Pin84~Pin87
0x25,0x08,0x25,0x08,0x21,0x08,0x24,0x00, \  #Pin88~Pin91
0x24,0x00,0x24,0x00,0x24,0x00}              #Pin92~Pin94
#[-end-130313-IB05280055-modify]#

#[-start-130531-IB05160452-add]#
#[-start-130201-IB11410050-modify]#
## SIO Table
gChipsetPkgTokenSpaceGuid.PcdPeiSioTable|{ \
  0x02, 0x88, \   #Power On UARTs
  0x24, 0xFE, \
  0x25, 0xBE, \
  0x28, 0x43, \   #IRQ of UARTs, UART2 IRQ=3,UART1 IRQ=4,
  0x29, 0x80, \   # SIRQ_CLKRUN_EN
  0x2A, 0x00, \
  0x2B, 0xDE, \
  0x2C, 0x02, \
  0x30, 0x60, \
  0x3B, 0x06, \
  0x3C, 0x10, \
  0x3A, 0x0A, \   # LPC Docking Enabling
  0x31, 0x1F, \
  0x32, 0x00, \
  0x33, 0x04, \
  0x38, 0xFB, \
  0x35, 0xFE, \
  0x36, 0x00, \
  0x37, 0xFE, \
  0x3A, 0x0B, \
  0x3C, 0x90, \
  0x39, 0x00, \
  0x34, 0x01, \
  0x12, 0x2E, \        # Relocate configuration ports base address to 0x2E
  0x13, 0x00}          # to ensure SIO config address can be accessed in OS
#[-end-130201-IB11410050-modify]#
#[-end-130110-IB11410040-modify]#


#[-start-140612-IB05400534-modify]#
#[-start-130508-IB10310018-modify]#
#[-start-130417-IB02960526-add]#
  gInsydeTokenSpaceGuid.PcdPreserveMemoryTable|{ \
    UINT32(0x09), UINT32(0x40),   \ # Preserve 256K(0x40 pages) for ASL
    UINT32(0x0a), UINT32(0x400),  \ # Preserve 4M(0x400 pages) for S3, SMM, etc
    UINT32(0x00), UINT32(0x1A00), \ # Preserve 26M(0x1A00 pages) for BIOS reserved memory
    UINT32(0x06), UINT32(0x300),  \ # Preserve 3M(0x300 pages) for UEFI OS runtime data to make S4 memory consistency
    UINT32(0x05), UINT32(0x200),  \ # Preserve 2M(0x200 pages) for UEFI OS runtime drivers to make S4 memory consistency
    UINT32(0x03), UINT32(0x1000), \ # Preserve 16M(0x1000 pages) for boot service drivers to reduce memory fragmental
    UINT32(0x04), UINT32(0x8000), \ # Preserve 128M(0x8000 pages) for boot service data to reduce memory fragmental
    UINT32(0x01), UINT32(0x200),  \ # Preserve 2M(0x200 pages) for UEFI OS boot loader to keep on same address
    UINT32(0x02), UINT32(0x00),   \
    UINT32(0x0e), UINT32(0x00)    \ #EndEntry
  }
#[-end-130417-IB02960526-add]#
#[-end-130508-IB10310018-modify]#
#[-end-140612-IB05400534-modify]#
#[-end-130531-IB05160452-add]#
[PcdsDynamicDefault]
#[-start-140402-IB10300106-add]#
  gChipsetPkgTokenSpaceGuid.PcdHotKeyFlashFileName|L"FlashUpdate.efi"|VOID*|64
#[-end-140402-IB10300106-add]#

[PcdsDynamicExDefault]
  #
  # Port number mapping table Define
  #
  gInsydeTokenSpaceGuid.PcdPortNumberMapTable|{ \
0x00, 0x1F, 0x02, 0, 0, 0x00, \
0x00, 0x1F, 0x02, 0, 1, 0x02, \
0x00, 0x1F, 0x02, 1, 0, 0x01, \
0x00, 0x1F, 0x02, 1, 1, 0x03, \
0x00, 0x1F, 0x05, 0, 0, 0x04, \
0x00, 0x1F, 0x05, 1, 0, 0x05, \
0x00, 0x16, 0x02, 0, 0, 0x06, \
0x00, 0x16, 0x02, 0, 1, 0x07, \
0x00, 0x16, 0x02, 1, 0, 0x08, \
0x00, 0x16, 0x02, 1, 1, 0x09, \
0x00, 0x00, 0x00, 0, 0, 0x00} #EndEntry

#[-start-130430-IB06720225-add]#
#[-start-130628-IB06720227-remove]#
#  #
#  # Device Interrupt Route Register
#  # RCBA Offset: 3140h-3161h
#  #
#  # Bit 2:0         : Interrupt A Pin Route
#  # Bit 6:4         : Interrupt B Pin Route
#  # Bit 10:8        : Interrupt C Pin Route
#  # Bit 14:12       : Interrupt D Pin Route
#  # Bit 3,7,11,15   : Reserved
#  # Value           : 0h = PIRQA        1h= PIRQB
#  #                 : 2h = PIRQC        3h= PIRQD
#  #                 : 4h = PIRQE        5h= PIRQF
#  #                 : 6h = PIRQG        7h= PIRQH
#  #                 : 8h = No PIRQ
#  #
#  gChipsetPkgTokenSpaceGuid.PcdDevice19InterruptRouteRegisterValue|0x8887
#  gChipsetPkgTokenSpaceGuid.PcdDevice20InterruptRouteRegisterValue|0x3210
#  gChipsetPkgTokenSpaceGuid.PcdDevice21InterruptRouteRegisterValue|0x5554
#  gChipsetPkgTokenSpaceGuid.PcdDevice22InterruptRouteRegisterValue|0x3210
#  gChipsetPkgTokenSpaceGuid.PcdDevice23InterruptRouteRegisterValue|0x0006
#  gChipsetPkgTokenSpaceGuid.PcdDevice25InterruptRouteRegisterValue|0x7654
#  gChipsetPkgTokenSpaceGuid.PcdDevice26InterruptRouteRegisterValue|0x3250
#  gChipsetPkgTokenSpaceGuid.PcdDevice27InterruptRouteRegisterValue|0x3216
#  gChipsetPkgTokenSpaceGuid.PcdDevice28InterruptRouteRegisterValue|0x3210
#  gChipsetPkgTokenSpaceGuid.PcdDevice29InterruptRouteRegisterValue|0x2037
#  gChipsetPkgTokenSpaceGuid.PcdDevice31InterruptRouteRegisterValue|0x0230
#[-end-130628-IB06720227-remove]#
#[-start-140612-IB06720257-remove]#
#  #
#  # Device Interrupt Pin Register
#  # RCBA Offset: 3100h-312Bh
#  #
#  # Bit  3:0        : Function 0 Interrupt Pin
#  # Bit  7:4        : Function 1 Interrupt Pin
#  # Bit 11:8        : Function 2 Interrupt Pin
#  # Bit 15:12       : Function 3 Interrupt Pin
#  # Bit 19:16       : Function 4 Interrupt Pin
#  # Bit 23:20       : Function 5 Interrupt Pin
#  # Bit 27:24       : Function 6 Interrupt Pin
#  # Bit 31:28       : Function 7 Interrupt Pin
#  #
#  # One component use 4 bit.
#  # Value           : 0h = No Interrupt
#  #                 : 1h = INTA         2h = INTB
#  #                 : 3h = INTC         4h = INTD
#  gChipsetPkgTokenSpaceGuid.PcdDevice20InterruptPinRegisterValue|0x00000021
#  gChipsetPkgTokenSpaceGuid.PcdDevice22InterruptPinRegisterValue|0x00004321
#  gChipsetPkgTokenSpaceGuid.PcdDevice25InterruptPinRegisterValue|0x00000001
#  gChipsetPkgTokenSpaceGuid.PcdDevice26InterruptPinRegisterValue|0x00000001
#  gChipsetPkgTokenSpaceGuid.PcdDevice27InterruptPinRegisterValue|0x00000001
#  gChipsetPkgTokenSpaceGuid.PcdDevice28InterruptPinRegisterValue|0x43214321
#  gChipsetPkgTokenSpaceGuid.PcdDevice29InterruptPinRegisterValue|0x00000001
#  gChipsetPkgTokenSpaceGuid.PcdDevice31InterruptPinRegisterValue|0x03203200
#[-end-140612-IB06720257-remove]#

  # PIRQ link value                                PIRQA, PIRQB, PIRQC, PIRQD, PIRQE, PIRQF, PIRQG, PIRQH
  gChipsetPkgTokenSpaceGuid.PcdPirqLinkValueArray|{0x60,  0x61,  0x62,  0x63,  0x68,  0x69,  0x6A,  0x6B} #EndEntry                                                 0x00, 0x01, 0x02, 0x03, \

#[-start-130723-IB06720229-modify]#
  gChipsetPkgTokenSpaceGuid.PcdVirtualBusTable|{  0x00, 0x01, 0x01, 0x0A, \
                                                  0x00, 0x01, 0x02, 0x0B, \
                                                  0x00, 0x1e, 0x00, 0x01, \
                                                  0x00, 0x1c, 0x00, 0x04, \
                                                  0x00, 0x1c, 0x01, 0x05, \
                                                  0x00, 0x1c, 0x02, 0x06, \
                                                  0x00, 0x1c, 0x03, 0x07, \
                                                  0x00, 0x1c, 0x04, 0x08, \
                                                  0x00, 0x01, 0x00, 0x02, \
                                                  0x00, 0x1c, 0x05, 0x09, \
                                                  0x00, 0x1c, 0x06, 0x0E, \
                                                  0x00, 0x1c, 0x07, 0x0F} #EndEntry
#[-end-130723-IB06720229-modify]#

#[-start-130723-IB06720229-modify]#
#0x98 => Dev 0x13
#0xA0 => Dev 0x14
#0xA8 => Dev 0x15
#0xB0 => Dev 0x16
#0xB8 => Dev 0x17
#0xC0 => Dev 0x18
#0xC8 => Dev 0x19
#0xD0 => Dev 0x1A
#0xD8 => Dev 0x1B
#0xE0 => Dev 0x1C
#0xE8 => Dev 0x1D
#0xF8 => Dev 0x1F
#0x08 => Dev 0x01 PEG
#0x10 => Dev 0x02 IGD
#0x18 => Dev 0x03 SA Audio Device
#0x20 => Dev 0x04 SA Thermal Device

#[-start-140612-IB06720257-modify]#
  #
  #Bus, Dev,  INT#A,IrqMask,        INT#B,IrqMask,        INT#C,IrqMask,        INT#D,IrqMask,        Slot, Reserved, DevIpRegValue, ProgrammableIrq.
  #
  gChipsetPkgTokenSpaceGuid.PcdControllerDeviceIrqRoutingEntry| { \
  0x04, 0x00, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x05, 0x00, 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x06, 0x00, 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x07, 0x00, 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x08, 0x00, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x09, 0x00, 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x0E, 0x00, 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x0F, 0x00, 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x00, 0x98, 0x6B, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0xA0, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000021), 0x00,  \
  0x00, 0xA8, 0x68, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0xB0, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00004321), 0x00,  \
  0x00, 0xB8, 0x6A, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0xC0, 0x68, UINT16(0xDEB8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0xC8, 0x68, UINT16(0xDEB8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, 0x00, UINT32(0x00000001), 0x00,  \
  0x00, 0xD0, 0x60, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000001), 0x00,  \
  0x00, 0xD8, 0x6A, UINT16(0xDEB8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, 0x00, UINT32(0x00000001), 0x00,  \
  0x00, 0xE0, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x43214321), 0x00,  \
  0x00, 0xE8, 0x6B, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000001), 0x00,  \
  0x00, 0xF8, 0x60, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x03203200), 0x00,  \
  0x0A, 0x00, 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x0B, 0x00, 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x02, 0x00, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x0B, 0x00, 0x63, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x00, 0xFF, UINT32(0x00000000), 0x00,  \
  0x00, 0x08, 0x60, UINT16(0xDEB8), 0x61, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x63, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0x10, 0x60, UINT16(0xDEB8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, UINT16(0xDEF8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0x18, 0x60, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x00, 0x20, 0x60, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, UINT16(0xDEB8), 0x00, 0x00, UINT32(0x00000000), 0x00,  \
  0x01, 0x00, 0x69, UINT16(0xDEB8), 0x6A, UINT16(0xDEB8), 0x6B, UINT16(0xDEB8), 0x68, UINT16(0xDEB8), 0x01, 0xFF, UINT32(0x00000000), 0x00,  \
  0x01, 0x08, 0x6A, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x68, UINT16(0xDEB8), 0x6B, UINT16(0xDEB8), 0x02, 0xFF, UINT32(0x00000000), 0x00,  \
  0x01, 0x10, 0x6B, UINT16(0xDEB8), 0x68, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x6A, UINT16(0xDEB8), 0x03, 0xFF, UINT32(0x00000000), 0x00,  \
  0x01, 0x18, 0x63, UINT16(0xDEB8), 0x62, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x6A, UINT16(0xDEB8), 0x04, 0xFF, UINT32(0x00000000), 0x00,  \
  0x01, 0x20, 0x62, UINT16(0xDEB8), 0x6B, UINT16(0xDEB8), 0x60, UINT16(0xDEB8), 0x68, UINT16(0xDEB8), 0x05, 0xFF, UINT32(0x00000000), 0x00,  \
  0x01, 0x28, 0x62, UINT16(0xDEB8), 0x68, UINT16(0xDEB8), 0x6A, UINT16(0xDEB8), 0x69, UINT16(0xDEB8), 0x06, 0xFF, UINT32(0x00000000), 0x00 }#EndEntry
#[-end-130723-IB06720229-modify]#
#[-end-140612-IB06720257-modify]#

  gChipsetPkgTokenSpaceGuid.PcdIrqPoolTable|{07, 0x00, \ #IRQ7
                                             10, 0x00, \ #IRQ10
                                             11, 0x00, \ #IRQ11
                                             11, 0x00, \ #IRQ11
                                             07, 0x00, \ #IRQ07
                                             10, 0x00, \ #IRQ10
                                             11, 0x00}   #IRQ11

  gChipsetPkgTokenSpaceGuid.PcdPirqPriorityTable|{7,  \# PIRQ A
                                                  0,  \# PIRQ B
                                                  0,  \# PIRQ C
                                                  10, \# PIRQ D
                                                  0,  \# PIRQ E
                                                  0,  \# PIRQ F
                                                  0,  \# PIRQ G
                                                  0}  #EndEntry

#[-end-130430-IB06720225-add]#

#[-start-140604-IB0508-SPI-add]#
  gH2OFlashDeviceMfrNameGuid.PcdNumonyxN25q064SpiMfrName|"Numonyx"
  gH2OFlashDevicePartNameGuid.PcdNumonyxN25q064SpiPartName|"N25Q064"
  gH2OFlashDeviceGuid.PcdNumonyxN25q064Spi|{ \ # Numonyx  N25Q064
    0x03, 0x00, 0x00, 0x00, 0x20, 0xBA, 0x17, 0x00,  \  # DeviceType = 03  Id = 0017BA20
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  Multiple = 0800
  }
  
  gH2OFlashDeviceConfigGuid.PcdNumonyxN25q064SpiConfig|{ \ # Numonyx  N25Q064
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x01, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BLockProtectAvailable = 01  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x3C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = 00  BlockProtectCode = 01  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00001000
  }
#[-end-140604-IB0508-SPI-add]#

[Components.IA32]

#[-start-140522-IB10300110-add]#
!if gChipsetPkgTokenSpaceGuid.PcdH2OWatchDogSupported
  $(CHIPSET_PKG)/WatchDogReportStatusCodePei/WatchDogReportStatusCodePei.inf
!endif
#[-end-140522-IB10300110-add]#

#[-start-130628-IB08400164-remove]#
##[-start-130617-IB04560405-add]#
#  !disable MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
#  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf{
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/MdeModulePkg/Universal/StatusCodeHandler/Pei
#  }
##[-end-130617-IB04560405-add]#
#[-end-130628-IB08400164-remove]#

#[-start-130115-IB10040020-remove]#
#  $(CHIPSET_PKG)/ChipsetPlatformLibServicesPei/ChipsetPlatformLibServicesPei.inf
#[-end-130115-IB10040020-remove]#
#[-start-130419-IB05400398-add]#
  $(CHIPSET_PKG)/GetBoardInfoPei/GetBoardInfoPei.inf
#[-end-130419-IB05400398-add]#
  $(CHIPSET_PKG)/ProgramGpioPei/ProgramGpioPei.inf
  $(CHIPSET_PKG)/MemoryTestNullPei/MemoryTestNullPei.inf
  $(CHIPSET_PKG)/PlatformPolicyPei/PolicyClass1Pei/PolicyClass1Pei.inf
!if gChipsetPkgTokenSpaceGuid.PcdThunderBoltSupported
  $(CHIPSET_PKG)/ThunderboltPei/ThunderboltPei.inf
!endif
  $(CHIPSET_PKG)/AzaliaControllerPei/AzaliaInitPei.inf
  $(CHIPSET_PKG)/SioInitPei/SioInitPei.inf
#[-start-130417-IB02960526-remove]#
#  $(CHIPSET_PKG)/PlatformMemorySizeHookPei/PlatformMemorySizeHookPei.inf
#[-end-130417-IB02960526-remove]#
  $(CHIPSET_PKG)/SmmControlPei/SmmControlPei.inf
#[-start-131226-IB04560452-add]#
  $(CHIPSET_PKG)/CommonChipset/SpeakerPei/LegacySpeakerPei.inf
#[-end-131226-IB04560452-add]#
!if gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported
#[-start-131226-IB04560452-remove]#
#  $(CHIPSET_PKG)/CommonChipset/SpeakerPei/LegacySpeakerPei.inf
#[-end-131226-IB04560452-remove]#
  $(CHIPSET_PKG)/PchAhciPei/PchAhciPei.inf
#[-start-140812-IB14090001-remove]#
#  $(CHIPSET_PKG)/PchXhciPei/PchXhciPei.inf
#[-end-140812-IB14090001-remove]#
  $(CHIPSET_PKG)/PchUfsPei/PchUfsPei.inf   
!endif

#[-start-140812-IB14090001-add]#
!if gInsydeTokenSpaceGuid.PcdH2OUsbPeiSupported
  $(CHIPSET_PKG)/PchXhciPei/PchXhciPei.inf
!endif
#[-end-140812-IB14090001-add]#

  $(CHIPSET_PKG)/CommonChipset/InstallVerbTablePei/InstallVerbTablePei.inf

  $(CHIPSET_PKG)/ChipsetSvcPei/ChipsetSvcPei.inf

#[-start-130218-IB07250301-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSwitchableGraphicsSupported
  $(CHIPSET_PKG)/SwitchableGraphicsPei/SwitchableGraphicsPei.inf
!endif
#[-end-130218-IB07250301-add]#

  #
  # ISCT PEI
  #
  $(CHIPSET_PKG)/IsctPei/IsctPei.inf

#[-start-130321-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
  $(CHIPSET_PKG)/SpsMe/Sample/SpsPolicyPei/SpsPolicyPei.inf
  $(CHIPSET_PKG)/SpsMe/SpsPei/SpsPei.inf
  $(CHIPSET_PKG)/SpsHeci/SpsHeciPei/SpsHeciPei.inf
!endif
#[-end-130321-IB10310017-add]#

!if $(XTU_SUPPORT) == YES
  $(CHIPSET_PKG)/Performancetuning/AcpiWatchDog/Pei/WdttPei.inf
!endif

#[-start-140318-IB08400253-add]#
  $(CHIPSET_PKG)/InsydeEventLogPolicyPei/InsydeEventLogPolicyPei.inf
#[-end-140318-IB08400253-add]#

################################################################################
#
# Components.IA32 Override
#
################################################################################

#[-start-130531-IB05160452-add]#
  !disable InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf
  InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei
  }

  !disable InsydeModulePkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf
  InsydeModulePkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Acpi/S3Resume2Pei
  }
#[-end-130531-IB05160452-add]#

#[-start-140205-IB05670227-add]#
  !disable InsydeFrameworkModulePkg/Compatibility/PeiOemServicesThunk/PeiOemServicesThunk.inf
  !disable InsydeFrameworkModulePkg/Compatibility/FvFileLoaderOnLoadFileThunk/FvFileLoaderOnLoadFileThunk.inf
#[-end-140205-IB05670227-add]#
[Components.X64]
#[-start-140522-IB10300110-add]#
!if gChipsetPkgTokenSpaceGuid.PcdH2OWatchDogSupported
  $(CHIPSET_PKG)/WatchDogHook/WatchDogHook.inf
  $(CHIPSET_PKG)/WatchDogLegacyHook/WatchDogLegacyHook.inf
  $(CHIPSET_PKG)/WatchDogReportStatusCodeDxe/WatchDogReportStatusCodeDxe.inf
  $(CHIPSET_PKG)/WatchDogReportStatusCodeSmm/WatchDogReportStatusCodeSmm.inf
!endif
#[-end-140522-IB10300110-add]#
#[-start-130830-IB05160485-add]#
  $(CHIPSET_PKG)/SmramSaveInfoHandlerSmm/SmramSaveInfoHandlerSmm.inf
#[-end-130830-IB05160485-add]#
#[-start-130709-IB05400426-add]#
  $(CHIPSET_PKG)/ChipsetGcdHookDxe/ChipsetGcdHookDxe.inf
#[-end-130709-IB05400426-add]#
#[-start-130628-IB08400164-remove]#
##[-start-130617-IB04560405-add]#
#  !disable MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
#  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe
#  }
#
#  !disable MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
#  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/MdeModulePkg/Universal/StatusCodeHandler/Smm
#  }
##[-end-130617-IB04560405-add]#
#[-end-130628-IB08400164-remove]#

#[-start-130709-IB05400426-add]#
  !disable InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/PciBusDxe
  }
#[-end-130709-IB05400426-add]#

#[-start-130912-IB08340139-remove]#
#[-start-130617-IB04560405-add]#
#  !disable InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf
#  InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf{
#    <SOURCE_OVERRIDE_PATH>
#     $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Smbios/PnpSmm
#  }
#[-end-130617-IB04560405-add]#
#[-end-130912-IB08340139-remove]#
#[-start-140714-IB12740058-remove]#
#!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OTpm2Supported == 1
#  $(CHIPSET_PKG)/Tpm/PhysicalPresence/TpmDriverDxe.inf
#!endif
#[-end-140714-IB12740058-remove]#
#[-start-130115-IB10040020-remove]#
# $(CHIPSET_PKG)/ChipsetPlatformLibServicesDxe/ChipsetPlatformLibServicesDxe.inf
#[-end-130115-IB10040020-remove]#
  $(CHIPSET_PKG)/RestoreMtrrDxe/RestoreMtrrDxe.inf
  $(CHIPSET_PKG)/SaveMemoryConfigDxe/SaveMemoryConfigDxe.inf
  $(CHIPSET_PKG)/PlatformInfoCollectDxe/PlatformInfoCollectDxe.inf
  $(CHIPSET_PKG)/UsbLegacyControlSmm/UsbLegacyControlSmm.inf
  $(CHIPSET_PKG)/UefiSetupUtilityDxe/SetupUtilityDxe.inf
  $(CHIPSET_PKG)/PlatformDxe/PlatformDxe.inf
  $(CHIPSET_PKG)/PciPlatformDxe/PciPlatformDxe.inf
  $(CHIPSET_PKG)/PlatformSmm/PlatformSmm.inf
  $(CHIPSET_PKG)/Int15MicrocodeSmm/Int15MicrocodeSmm.inf
  $(CHIPSET_PKG)/BbstableHookDxe/BbstableHookDxe.inf
#
# Accelerator for Windows 7 warm booting.
#
  $(CHIPSET_PKG)/CsmInt10BlockDxe/CsmInt10BlockDxe.inf
  $(CHIPSET_PKG)/BiosWriteProtectSmm/PchBiosWriteProtectSmm.inf
  $(CHIPSET_PKG)/BiosRegionLockDxe/BiosRegionLockDxe.inf
!if gChipsetPkgTokenSpaceGuid.PcdUltFlag
  $(CHIPSET_PKG)/I2cBusDxe/I2cBusDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/I2cPolicyDxe/I2cPolicyDxe.inf
!endif
#[-start-120918-IB10820123-add]#
#
#  Policy
#
  $(CHIPSET_PKG)/PlatformPolicyDxe/GopPolicyDxe/GopPolicyDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/SaPolicyDxe/SaPolicyDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/PchPolicyDxe/PchPolicyDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/CpuPolicyDxe/CpuPolicyDxe.inf
!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  $(CHIPSET_PKG)/PlatformPolicyDxe/MePolicyDxe/MePolicyDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/AmtPolicyDxe/AmtPolicyDxe.inf
!endif
  $(CHIPSET_PKG)/PlatformPolicyDxe/IccPolicyDxe/IccPolicyDxe.inf
  $(CHIPSET_PKG)/PlatformPolicyDxe/AcpiPolicyInitDxe/AcpiPolicyInitDxe.inf
!if gChipsetPkgTokenSpaceGuid.PcdAntiTheftSupported
  $(CHIPSET_PKG)/PlatformPolicyDxe/AtPolicyDxe/AtPolicyDxe.inf
!endif
!if gChipsetPkgTokenSpaceGuid.PcdXtuSupported
  $(CHIPSET_PKG)/PlatformPolicyDxe/XtuPolicyDxe/XtuPolicyDxe.inf
!endif
!if gChipsetPkgTokenSpaceGuid.PcdThunderBoltSupported
  $(CHIPSET_PKG)/ThunderboltDxe/ThunderboltDxe.inf
  $(CHIPSET_PKG)/ThunderboltSmm/ThunderboltSmm.inf
!endif
!if gChipsetPkgTokenSpaceGuid.PcdRapidStartSupported
  $(CHIPSET_PKG)/PlatformPolicyDxe/RapidStartPolicyInitDxe/RapidStartPolicyInitDxe.inf
!endif
#[-end-120918-IB10820123-add]#
#[-start-130911-IB08620310-add]#
  $(CHIPSET_PKG)/PlatformPolicyDxe/SrIovPolicyDxe/SrIovPolicyDxe.inf
#[-end-130911-IB08620310-add]#


  $(CHIPSET_PKG)/VbiosHookSmm/VbiosHookSmm.inf
#[-start-130225-IB08520070-remove]#
#  $(CHIPSET_PKG)/CheckRaidDxe/CheckRaidDxe.inf
#[-end-130225-IB08520070-remove]#
  $(CHIPSET_PKG)/SavePegConfigDxe/SavePegConfigDxe.inf
  $(CHIPSET_PKG)/OemBadgingSupportDxe/OEMBadgingSupportDxe.inf


  $(CHIPSET_PKG)/Binary/UefiRaid/$(RAID_UEFI_DRIVER_VERSION)/UefiRaid.inf
#[-start-130905-IB08620307-add]#
  $(CHIPSET_PKG)/Binary/UefiRaid/$(RAID_SV_UEFI_DRIVER_VERSION)/UefiRaid.inf
#[-end-130905-IB08620307-add]#
  $(CHIPSET_PKG)/Binary/MicrocodeUpdates/MicrocodeUpdates.inf

  $(CHIPSET_PKG)/ChipsetSvcDxe/ChipsetSvcDxe.inf
  $(CHIPSET_PKG)/ChipsetSvcSmm/ChipsetSvcSmm.inf


#
# On-board USB 3.0 SMI dispatcher driver
#
  $(CHIPSET_PKG)/XhciSmiDispatcher/XhciSmiDispatcher.inf

  $(CHIPSET_PKG)/CommonChipset/SetSsidSvidDxe/SetSsidSvidDxe.inf
  $(CHIPSET_PKG)/CommonChipset/AspmOverrideDxe/AspmOverrideDxe.inf
#[-start-130227-IB06720162-add]#
  $(CHIPSET_PKG)/CommonChipset/SpeakerDxe/LegacySpeakerDxe.inf
#[-end-130227-IB06720162-add]#
  $(CHIPSET_PKG)/Binary/UefiGop/$(VIDEO_UEFI_DRIVER_VERSION)/IntelGopDriver.inf
!if $(DENLOW_SERVER_SUPPORT) == YES
  $(CHIPSET_PKG)/Binary/UefiBmcGop/$(BMC_VIDEO_UEFI_DRIVER_VERSION)/BmcGopDriver.inf
!endif
  $(CHIPSET_PKG)/Binary/UefiLan/$(LAN_UEFI_DRIVER_VERSION)/UefiLan.inf
  $(CHIPSET_PKG)/Binary/Icc/$(ICC_SDK_VERSION)/IccOverClocking.inf
  $(CHIPSET_PKG)/IhisiSmm/IhisiSmm.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  }
  $(CHIPSET_PKG)/OemModifyOpRegionDxe/OemModifyOpRegionDxe.inf
  $(CHIPSET_PKG)/OemAcpiPlatformDxe/OemAcpiPlatformDxe.inf
  $(CHIPSET_PKG)/MemInfoDxe/MemInfoDxe.inf
  $(CHIPSET_PKG)/PlatformResetRunTimeDxe/PlatformResetRunTimeDxe.inf

  #[-start-130218-IB07250301-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSwitchableGraphicsSupported
  $(CHIPSET_PKG)/SwitchableGraphicsDxe/SwitchableGraphicsDxe.inf
  $(CHIPSET_PKG)/SwitchableGraphicsSmm/SwitchableGraphicsSmm.inf
!if gChipsetPkgTokenSpaceGuid.PcdAmdPowerXpressSupported
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/AmdDiscreteSsdt.inf
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/AmdPowerXpressSsdt.inf
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/AmdUltPowerXpressSsdt.inf
!endif
!if gChipsetPkgTokenSpaceGuid.PcdNvidiaOptimusSupported
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/NvidiaDiscreteSsdt.inf
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/NvidiaOptimusSsdt.inf
  $(CHIPSET_PKG)/SwitchableGraphicsAcpi/NvidiaUltOptimusSsdt.inf
!endif
!endif
#[-end-130218-IB07250301-add]#

!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  $(CHIPSET_PKG)/IccDxe/IccDxe.inf
  $(CHIPSET_PKG)/MdesStatusCodeDxe/MdesStatusCodeDxe.inf
!endif
  $(CHIPSET_PKG)/ChipsetSmmThunk/ChipsetSmmThunk.inf
  $(CHIPSET_PKG)/TxtOneTouchDxe/TxtOneTouchDxe.inf

!if $(XTU_SUPPORT) == YES
  $(CHIPSET_PKG)/Performancetuning/AcpiWatchDog/Smm/AcpiWatchDogSmm.inf
  $(CHIPSET_PKG)/Performancetuning/AcpiWatchDog/Dxe/AcpiWatchDogDxe.inf
  $(CHIPSET_PKG)/Performancetuning/Dxe/PerfTuneCore/PerfTuneCoredrv.inf
  $(CHIPSET_PKG)/Performancetuning/Dxe/BiosSettingDevices/BiosSpttDrv.inf
  $(CHIPSET_PKG)/Performancetuning/Dxe/RealtimeControlDevices/RealtimeSpttDrv.inf
  $(CHIPSET_PKG)/Performancetuning/Smm/PerfTuneSmm/PerfTuneSmm.inf
  $(CHIPSET_PKG)/Performancetuning/Dxe/PerfTuneDxe/PerfTuneDxe.inf
!endif

 #
 # ISCT Driver
 #
 $(CHIPSET_PKG)/IsctAcpiDxe/IsctAcpiDxe.inf
 $(CHIPSET_PKG)/IsctSmm/IsctSmm.inf
 $(CHIPSET_PKG)/IsctAcpiTables/IsctAcpiTables.inf

 #
 # Hardware Monitor Driver by EC
 #
 $(CHIPSET_PKG)/HwMonitorEcDxe/HwMonitorEcDxe.inf

#[-start-130403-IB05330413-remove]#
##[-start-130426-IB10930028-add]#
# #
# # Memory thermal initialization driver
# #
# $(CHIPSET_PKG)/MemoryThermal/MemoryThermal.inf
##[-end-130426-IB10930028-add]#
#[-end-130403-IB05330413-remove]#

#[-start-130321-IB10310017-add]#
!if gChipsetPkgTokenSpaceGuid.PcdSpsMeSupported
  $(CHIPSET_PKG)/SpsMe/Sample/SpsPolicyDxe/SpsPolicyDxe.inf
  $(CHIPSET_PKG)/SpsMe/SpsDxe/SpsDxe.inf

  $(CHIPSET_PKG)/SpsHeci/SpsHeciDxe/SpsHeciDxe.inf
  $(CHIPSET_PKG)/SpsHeci/SpsHeciSmm/SpsHeciSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }

  $(CHIPSET_PKG)/SpsMe/SpsMeFwUpgradeDxe/SpsMeFwUpgradeDxe.inf
  $(CHIPSET_PKG)/SpsMe/SpsMeFwUpgradeSmm/SpsMeFwUpgradeSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
!endif
#[-end-130321-IB10310017-add]#

#[-start-130531-IB10930032-add]#
 #
 # Variable Edit
 #
 $(CHIPSET_PKG)/VariableEditDxe/VariableEditDxe.inf
 $(CHIPSET_PKG)/VariableEditSmm/VariableEditSmm.inf
#[-end-130531-IB10930032-add]#

#[-start-140325-IB13080003-add]#
!if gInsydeTokenSpaceGuid.PcdDynamicHotKeySupported
 $(CHIPSET_PKG)/DynamicHotKeyDxe/DynamicHotKeyDxe.inf
!endif
#[-end-140325-IB13080003-add]#
#[-start-140321-IB10300105-add]#
  $(CHIPSET_PKG)/FileSelectUIDxe/FileSelectUIDxe.inf
  $(CHIPSET_PKG)/Ramdisk/Ramdisk.inf
  $(CHIPSET_PKG)/InternalFlashBios/InternalFlashBios.inf
  $(CHIPSET_PKG)/BiosFlashUI/BiosFlashUI.inf
#[-end-140321-IB10300105-add]#
################################################################################
#
# Components.X64 Override
#
################################################################################

#[-start-130531-IB05160452-add]#
  !disable InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf {
   <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe
  }
#[-end-130531-IB05160452-add]#

#[-start-130830-IB05160485-add]#
  !disable InsydeFrameworkModulePkg/Compatibility/ChipsetLibServices/Dxe/ChipsetLibServicesDxe.inf
  !disable InsydeFrameworkModulePkg/Compatibility/ChipsetLibServices/Smm/ChipsetLibServicesSmm.inf

#[-end-130830-IB05160485-add]#

#[-start-140205-IB05670227-add]#
  !disable InsydeFrameworkModulePkg/Compatibility/SmmRuntime/SmmRuntime.inf
  !disable InsydeFrameworkModulePkg/Compatibility/PiSmmStatusCodeThunk/PiSmmStatusCodeThunk.inf
  !disable InsydeFrameworkModulePkg/Compatibility/DxeOemServicesThunk/DxeOemServicesThunk.inf
  !disable InsydeFrameworkModulePkg/Compatibility/SmmOemServicesThunk/SmmOemServicesThunk.inf
#[-end-140205-IB05670227-add]#
#[-start-140811-IB05400545-add]#
  !disable EdkCompatibilityPkg/Compatibility/MpServicesOnFrameworkMpServicesThunk/MpServicesOnFrameworkMpServicesThunk.inf
#[-end-140811-IB05400545-add]#

###################################################################################################
#
# BuildOptions Section - Define the module specific tool chain flags that should be used as
#                        the default flags for a module. These flags are appended to any
#                        standard flags that are defined by the build process. They can be
#                        applied for any modules or only those modules with the specific
#                        module style (EDK or EDKII) specified in [Components] section.
#
###################################################################################################

[BuildOptions.X64.EDKII]

#[-start-130402-IB10920016-add]#
!if $(HARDWARE_MONITOR_SUPPORT) == YES
  *_*_*_CC_FLAGS    = -D HARDWARE_MONITOR_SUPPORT
  *_*_*_VFRPP_FLAGS = -D HARDWARE_MONITOR_SUPPORT
!endif
#[-end-130402-IB10920016-add]#

#[-start-140610-IB05400533-add]#
[BuildOptions]
!IF $(PCI_EXPRESS_SIZE) == 128
  GCC:*_*_IA32_JWASM_FLAGS           = -D PLATFORM_PCIEX_BASE_ADDRESS
  GCC:*_*_X64_JWASM_FLAGS            = -D PLATFORM_PCIEX_BASE_ADDRESS
!ENDIF
!IF $(PCI_EXPRESS_SIZE) == 64
  GCC:*_*_IA32_JWASM_FLAGS           = -D PLATFORM_PCIEX_BASE_ADDRESS -D PLATFORM_PCIEX_BASE_ADDRESS_64MB
  GCC:*_*_X64_JWASM_FLAGS            = -D PLATFORM_PCIEX_BASE_ADDRESS -D PLATFORM_PCIEX_BASE_ADDRESS_64MB
!ENDIF
#[-end-140610-IB05400533-add]#