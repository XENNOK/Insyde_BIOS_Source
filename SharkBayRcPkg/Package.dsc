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
   DEFINE   CHIPSET_REF_CODE_PKG = SharkBayRcPkg

!include $(CHIPSET_REF_CODE_PKG)/Package.env
#[-end-130301-IB05670140-add]#

[LibraryClasses]
!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  MeChipsetLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/MeKernel/Common/MeChipsetLib/MeChipsetLib.inf
  MeLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/MeKernel/Dxe/MeLib.inf
  AmtLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/Amt/Dxe/AmtLib.inf
!else
  MeLib|$(CHIPSET_PKG)/Library/DxeMeLibNull/DxeMeLibNull.inf
  AmtLib|$(CHIPSET_PKG)/Library/DxeAmtLibNull/DxeAmtLibNull.inf
!endif
  PchPlatformLib|$(CHIPSET_REF_CODE_PKG)/Pch/Library/PchPlatformLib/PchPlatformLib.inf
#[-start-120911-IB10820121-add]#
  CpuPlatformLib|$(CHIPSET_REF_CODE_PKG)/Cpu/Library/CpuPlatformLib/CpuPlatformLib.inf
#[-end-120911-IB10820121-add]#
  RcFviDxeLib|$(CHIPSET_REF_CODE_PKG)/Pch/Library/RcFviDxeLib/RcFviDxeLib.inf
  OverclockingLib|$(CHIPSET_REF_CODE_PKG)/Cpu/Library/OverclockingLib/OverclockingLib.inf
  PchPciExpressHelpersLib|$(CHIPSET_REF_CODE_PKG)/Pch/Library/PchPciExpressHelpersLib/PchPciExpressHelpersLib.inf
  SaPcieDxeLib | $(CHIPSET_REF_CODE_PKG)/SystemAgent/Library/SaPcieLib/Dxe/SaPcieDxeLib.inf
  BootGuardLib|$(CHIPSET_REF_CODE_PKG)/Cpu/Library/BootGuardLib/BootGuardLib.inf
!if $(ANTI_THEFT_SUPPORT) == YES
  AtDxeLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/AtLibrary/Dxe/AtDxeLib.inf
  AtGraphicsLite|$(CHIPSET_REF_CODE_PKG)/Me/At/Library/AtGraphicsLite/AtGraphics.inf
!endif
  TxtLib|$(CHIPSET_REF_CODE_PKG)/Cpu/Library/TxtLib/TxtLib.inf
  
  PchSmbusLibDxe|$(CHIPSET_REF_CODE_PKG)/Pch/Library/PchSmbusLib/Dxe/PchSmbusLibDxe.inf
  PchSmbusLibPei|$(CHIPSET_REF_CODE_PKG)/Pch/Library/PchSmbusLib/Pei/PchSmbusLibPei.inf
  
[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]
!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  PeiAmtLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/Amt/Pei/PeiAmtLib.inf
  PeiMeLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/MeKernel/Pei/PeiMeLib.inf
!endif

  #
  # Rapidstart reference code
  #	
!if $(RAPID_START_SUPPORT) == YES
  GfxDisplayLibPei|$(CHIPSET_REF_CODE_PKG)/RapidStart/Library/GfxDisplayLibPei/GfxDisplayLibPei.inf
!endif

#[-start-130812-IB05670190-modify]#
!if gChipsetPkgTokenSpaceGuid.PcdPttSupport
#[-end-130812-IB05670190-modify]#
  PttHciDeviceLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/Ptt/Pei/PttHciDevicePeiLib.inf
!endif

  
[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  PchDxeRuntimePciLibPciExpress|$(CHIPSET_REF_CODE_PKG)/Pch/Library/DxeRuntimePciLibPciExpress/DxeRuntimePciLibPciExpress.inf
  
[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_DRIVER]

  #
  # Rapidstart reference code
  #	
!if $(RAPID_START_SUPPORT) == YES
  GfxDisplayLibDxe|$(CHIPSET_REF_CODE_PKG)/RapidStart/Library/GfxDisplayLibDxe/GfxDisplayLibDxe.inf
!endif

#[-start-130812-IB05670190-modify]#
!if gChipsetPkgTokenSpaceGuid.PcdPttSupport
#[-end-130812-IB05670190-modify]#
  PttHciDeviceLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/Ptt/Dxe/PttHciDeviceDxeLib.inf
  PttHeciLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/PttHeci/Dxe/PttHeciDxeLib.inf
!endif
  
[LibraryClasses.common.DXE_SMM_DRIVER]
#[-start-130812-IB05670190-modify]#
!if gChipsetPkgTokenSpaceGuid.PcdPttSupport
#[-end-130812-IB05670190-modify]#
  PttHciDeviceLib|$(CHIPSET_REF_CODE_PKG)/Me/Library/Ptt/Dxe/PttHciDeviceDxeLib.inf
!endif

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]
  
[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]
!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/StartWatchDog/Pei/StartWatchDog.inf
  $(CHIPSET_REF_CODE_PKG)/Me/Heci/Pei/HeciPei.inf
  $(CHIPSET_REF_CODE_PKG)/Me/PchMeUma/PchMeUma.inf
!endif

  $(CHIPSET_REF_CODE_PKG)/Cpu/CpuS3Pei/CpuS3Pei.inf
  $(CHIPSET_REF_CODE_PKG)/Cpu/CpuInit/Pei/CpuInitPeim.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/PchInit/Pei/PchInitPeim.inf 
  $(CHIPSET_REF_CODE_PKG)/Pch/Wdt/Pei/WdtPeim.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Reset/Pei/PchResetPeim.inf
#  $(CHIPSET_REF_CODE_PKG)/Pch/SmmControl/Pei/SmmControl.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Smbus/Pei/PchSmbusArpDisabled.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Spi/Pei/PchSpiPeim.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/S3Support/S3/PchS3Peim.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Usb/Pei/PchUsb.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/SaInit/Pei/SaInitPeim.inf

  #
  # Rapidstart reference code
  #	
!if $(RAPID_START_SUPPORT) == YES
  $(CHIPSET_REF_CODE_PKG)/RapidStart/Pei/RapidStartPei.inf
  $(CHIPSET_REF_CODE_PKG)/RapidStart/GfxDisplay/FfsGfxDriver/Pei/PeiGfxDriver.inf  
!endif

  $(CHIPSET_REF_CODE_PKG)/SystemAgent/MemoryInit/Pei/MemoryInit.inf
  
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/SmmAccess/Pei/SmmAccess.inf
  
  #
  # TXT reference code
  # 
  !if $(TXT_SUPPORT) == YES
  $(CHIPSET_REF_CODE_PKG)/Cpu/Txt/TxtInit/Pei/TxtPei.inf
  $(CHIPSET_REF_CODE_PKG)/Cpu/Txt/TxtInit/Pei/Ia32/TxtPeiAp.inf  
  !endif
  
!if gChipsetPkgTokenSpaceGuid.PcdMe5MbSupported
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/AlertStandardFormat/Heci/Pei/AlertStandardFormatPei.inf
!endif  
  
[Components.X64]
!if gChipsetPkgTokenSpaceGuid.PcdMeSupported
  $(CHIPSET_REF_CODE_PKG)/Me/Heci/Dxe/HeciDxe.inf
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/AmtBootOptions/Dxe/ActiveManagement.inf
  $(CHIPSET_REF_CODE_PKG)/Me/BiosExtensionLoader/Dxe/BiosExtensionLoader.inf
  $(CHIPSET_REF_CODE_PKG)/Me/MeFwDowngrade/Dxe/MeFwDowngrade.inf
!endif

  #
  # Power Management Drivers
  #
  $(CHIPSET_REF_CODE_PKG)/Cpu/PowerManagement/Dxe/PowerMgmtDxe.inf
  $(CHIPSET_REF_CODE_PKG)/Cpu/PowerManagement/Smm/PowerMgmtS3.inf
  $(CHIPSET_REF_CODE_PKG)/Cpu/PowerManagement/AcpiTables/PowerMgmtAcpiTables.inf

  #
  # DTS SMM drivers
  #
  $(CHIPSET_REF_CODE_PKG)/Cpu/CpuInit/Dxe/CpuInitDxe.inf
  
#[-start-121204-IB06720141-remove]#
#  $(CHIPSET_REF_CODE_PKG)/Platform/Insyde/Common/SmbiosMemoryDxe/SmbiosMemoryDxe.inf
#[-end-121204-IB06720141-remove]#
  
  #
  # DTS SMM drivers
  #
  $(CHIPSET_REF_CODE_PKG)/Cpu/Dts/Smm/DigitalThermalSensorSmm.inf

  #
  # PFAT SMM drivers
  #
  $(CHIPSET_REF_CODE_PKG)/Cpu/PfatServicesSmm/PfatServicesSmm.inf
  
  #
  # DXE drivers produce PCH protocols
  #  
  $(CHIPSET_REF_CODE_PKG)/Pch/ActiveBios/Dxe/ActiveBios.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Spi/RuntimeDxe/PchSpiRuntime.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/SerialGpio/Dxe/PchSerialGpio.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/SmartTimer/Dxe/SmartTimer.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/SmmControl/RuntimeDxe/SmmControl.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Smbus/Smm/PchSmbusSmm.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Smbus/Dxe/PchSmbusDxe.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/LegacyInterrupt/Dxe/LegacyInterrupt.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Reset/RuntimeDxe/PchResetRuntime.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/S3Support/Dxe/PchS3Support.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Wdt/Dxe/WdtDxe.inf
  #
  # DXE drivers use PCH protocols to initialize PCH
  #  
  $(CHIPSET_REF_CODE_PKG)/Pch/PchInit/Dxe/PchInitDxe.inf

  #
  # PchInit Smm driver
  #
  $(CHIPSET_REF_CODE_PKG)/Pch/PchInit/Smm/PchLateInitSmm.inf

  #
  # EFI 1.1 drivers
  #  
  $(CHIPSET_REF_CODE_PKG)/Pch/SataController/Dxe/SataController.inf
  
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/BdatAccessHandler/Dxe/BdatAccessHandler.inf

  #
  # Rapidstart reference code
  #
!if $(RAPID_START_SUPPORT) == YES  
  $(CHIPSET_REF_CODE_PKG)/RapidStart/Dxe/RapidStartDxe.inf
  $(CHIPSET_REF_CODE_PKG)/RapidStart/AcpiTables/RapidStartAcpiTables.inf
  $(CHIPSET_REF_CODE_PKG)/RapidStart/Smm/RapidStartSmm.inf
!endif
  
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SaAcpiTables_Edk.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SaSsdt/SaSsdt_Edk.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/SmmAccess/Dxe/SmmAccess.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/PciHostBridge/Dxe/PciHostBridge.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/SaInit/Dxe/SaInit.inf
#[-start-130517-IB10040025-add]#
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/SmbiosMemory/Dxe/SmBiosMemory.inf
#[-end-130517-IB10040025-add]#
  
  $(CHIPSET_REF_CODE_PKG)/Pch/IoTrap/Smm/IoTrap.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/PchSmiDispatcher/Smm/PchSmiDispatcher.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Pcie/Smm/PchPcieSmm.inf
  $(CHIPSET_REF_CODE_PKG)/Pch/Spi/Smm/PchSpiSmm.inf
  
  #
  # S3 Boot Script Dispatch thunk driver
  #
  $(CHIPSET_REF_CODE_PKG)/Pch/S3BootScriptDispatchThunk/Dxe/S3BootScriptDispatchThunk.inf  {
      <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }

  #
  # TXT reference code
  # 
!if $(TXT_SUPPORT) == YES  
  $(CHIPSET_REF_CODE_PKG)/Cpu/Txt/TxtInit/Dxe/TxtDxe.inf 
!endif
  
!if gChipsetPkgTokenSpaceGuid.PcdMe5MbSupported
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/AlertStandardFormat/Heci/Dxe/AlertStandardFormatDxe.inf
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/IdeR/Dxe/IdeRController.inf
  $(CHIPSET_REF_CODE_PKG)/Me/ActiveManagement/Sol/Dxe/PciSerial.inf
  #
  # Uncomment the following line if the Mebx and MebxSetupBrowser are included by this build description
  #
  $(CHIPSET_REF_CODE_PKG)/Me/EfiMEBx/$(MEBX_VERSION)/Mebx/Mebx.inf
  $(CHIPSET_REF_CODE_PKG)/Me/EfiMEBx/$(MEBX_VERSION)/MebxSetupBrowser/MebxSetupBrowser.inf
!endif

!if $(ANTI_THEFT_SUPPORT) == YES
  $(CHIPSET_REF_CODE_PKG)/Me/At/AtAm/Dxe/AtAm.inf
  $(CHIPSET_REF_CODE_PKG)/Me/At/AtBadging/AtBadging.inf
!endif
 
 #
 # Rapidstart reference code
 #
 $(CHIPSET_REF_CODE_PKG)/RapidStart/GfxDisplay/FfsGfxDriver/Dxe/DxeFfsGfxDriver.inf
 
  #
  # Disable the following driver if PTT is not supported
  #
#[-start-130812-IB05670190-modify]#
!if gChipsetPkgTokenSpaceGuid.PcdPttSupport
#[-end-130812-IB05670190-modify]#
  $(CHIPSET_REF_CODE_PKG)/Me/Ptt/Smm/PttHciSmm.inf
  $(CHIPSET_REF_CODE_PKG)/Me/Ptt/AcpiTables/Tpm2AcpiTables.inf
  $(CHIPSET_REF_CODE_PKG)/Me/AcpiTables/MeSsdt/MeSsdt.inf
!endif

#[-start-140218-IB10920078-add]#
!if gChipsetPkgTokenSpaceGuid.PcdIntelSwitchableGraphicsSupported
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SwitchableGraphics/Pch/SgAcpiTablesPch.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SwitchableGraphics/Peg/SgAcpiTables.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SwitchableGraphics/AmdPowerXpress/AmdPowerXpressSsdt.inf
  $(CHIPSET_REF_CODE_PKG)/SystemAgent/AcpiTables/SwitchableGraphics/NvidiaOptimus/NvidiaOptimusSsdt.inf
!endif
#[-end-140218-IB10920078-add]#

[BuildOptions]
!if $(ULT_SUPPORT) == YES
  *_*_*_CC_FLAGS = -D ULT_FLAG -D LPSS_FLAG/D ADSP_FLAG -D MEMORY_DOWN_SUPPORT
!else
  *_*_*_CC_FLAGS = -D TRAD_FLAG -D PEG_FLAG -D DMI_FLAG
!endif
