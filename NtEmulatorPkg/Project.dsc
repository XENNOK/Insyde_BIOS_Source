## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2015, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
!import NtEmulatorPkg/Package.dsc

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = NtEmulatorPkg
  PLATFORM_GUID                  = 7f48118c-2f97-4d39-972a-e836358b7433
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PROJECT_PKG)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Build/$(PROJECT_PKG)/Project.fdf

  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  !include $(PROJECT_PKG)/Project.env
  RFC_LANGUAGES                  = "en-US;fr-FR"


################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf {
    <SOURCE_OVERRIDE_PATH>
      NtEmulatorPkg/Override/MdeModulePkg/Library/UefiHiiLib
  }

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  FileHandleLib|ShellPkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  SortLib|ShellPkg/Library/UefiSortLib/UefiSortLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  PathLib|ShellPkg/Library/BasePathLib/BasePathLib.inf

[PcdsDynamicDefault.Ia32]
  gEfiNtEmulatorPkgTokenSpaceGuid.PcdWinNtFileSystem|L"."

[PcdsDynamicDefault.x64]
  gEfiNtEmulatorPkgTokenSpaceGuid.PcdWinNtFileSystem|L"."

[PcdsFixedAtBuild]
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
!ifdef $(NO_SHELL_PROFILES)
  gEfiShellPkgTokenSpaceGuid.PcdShellProfileMask|0x00
!endif #$(NO_SHELL_PROFILES)

[Components]
  #
  # Disable unused drivers
  #
  !disable MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  !disable MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  !disable MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  !disable MdeModulePkg/Universal/Network/Ip4ConfigDxe/Ip4ConfigDxe.inf
  !disable MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  !disable MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  !disable MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  !disable MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  !disable MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  !disable MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  !disable MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  !disable NtEmulatorPkg/SnpNtEmulatorDxe/SnpNtEmulatorDxe.inf {

  !disable MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      NtEmulatorPkg/Override/MdeModulePkg/Universal/HiiDatabaseDxe
  }

  ShellPkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
  ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf

  ShellPkg/Library/UefiDpLib/UefiDpLib.inf {
    <LibraryClasses>
      TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
      PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
      DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  }

  ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf {
    <SOURCE_OVERRIDE_PATH>
      NtEmulatorPkg/Override/ShellPkg/Library/UefiShellDebug1CommandsLib
  }
!if $(BUILD_NEW_SHELL)
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
!ifndef $(NO_SHELL_PROFILES)
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
!ifdef $(INCLUDE_DP)
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf
!endif #$(INCLUDE_DP)
!endif #$(NO_SHELL_PROFILES)
  }
!endif

[BuildOptions]
  MSFT:RELEASE_*_*_CC_FLAGS      = -D MDEPKG_NDEBUG
!if $(EFI_DEBUG) == NO
  MSFT:DEBUG_*_*_CC_FLAGS        = -D MDEPKG_NDEBUG
!endif
