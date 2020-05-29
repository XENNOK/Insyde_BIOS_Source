## @file
# H2O IPMI package project build description file.
#
#******************************************************************************
#* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
#


################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]


################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  #
  # H2O IPMI
  #
  IpmiCommonLib|InsydeIpmiPkg/Library/BaseIpmiCommonLib/BaseIpmiCommonLib.inf
  IpmiHobLib|InsydeIpmiPkg/Library/BaseIpmiHobLib/BaseIpmiHobLib.inf
  IpmiInterfaceLib|InsydeIpmiPkg/Library/DxeIpmiInterfaceLib/DxeIpmiInterfaceLib.inf
  IpmiSdrLib|InsydeIpmiPkg/Library/DxeIpmiSdrLib/DxeIpmiSdrLib.inf
  IpmiFruLib|InsydeIpmiPkg/Library/DxeIpmiFruLib/DxeIpmiFruLib.inf
  IpmiSelDataLib|InsydeIpmiPkg/Library/DxeIpmiSelDataLib/DxeIpmiSelDataLib.inf
  IpmiSelInfoLib|InsydeIpmiPkg/Library/DxeIpmiSelInfoLib/DxeIpmiSelInfoLib.inf
!endif

[LibraryClasses.common.PEIM]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  #
  # H2O IPMI
  #
  IpmiInterfaceLib|InsydeIpmiPkg/Library/PeiIpmiInterfaceLib/PeiIpmiInterfaceLib.inf
  PeiIpmiPackageLib|InsydeIpmiPkg/Library/PeiIpmiPackageLib/PeiIpmiPackageLib.inf
!if gH2OIpmiPkgTokenSpaceGuid.PcdIpmiLpcDecode
  PeiIpmiLpcLib|InsydeIpmiPkg/Library/PeiIpmiLpcLib/PeiIpmiLpcLib.inf
!else
  PeiIpmiLpcLib|InsydeIpmiPkg/Library/PeiIpmiLpcLibNull/PeiIpmiLpcLibNull.inf
!endif
!if gH2OIpmiPkgTokenSpaceGuid.PcdIpmiSioConfig
  PeiIpmiSioLib|InsydeIpmiPkg/Library/PeiIpmiSioLib/PeiIpmiSioLib.inf
!else
  PeiIpmiSioLib|InsydeIpmiPkg/Library/PeiIpmiSioLibNull/PeiIpmiSioLibNull.inf
!endif
  PeiOemIpmiPackageLibDefault|InsydeIpmiPkg/Library/PeiOemIpmiPackageLib/PeiOemIpmiPackageLibDefault.inf
  PeiOemIpmiPackageLib|InsydeIpmiPkg/Library/PeiOemIpmiPackageLib/PeiOemIpmiPackageLib.inf
!endif

[LibraryClasses.common.DXE_DRIVER]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  #
  # H2O IPMI
  #
  DxeIpmiPackageLib|InsydeIpmiPkg/Library/DxeIpmiPackageLib/DxeIpmiPackageLib.inf
  DxeOemIpmiPackageLibDefault|InsydeIpmiPkg/Library/DxeOemIpmiPackageLib/DxeOemIpmiPackageLibDefault.inf
  DxeOemIpmiPackageLib|InsydeIpmiPkg/Library/DxeOemIpmiPackageLib/DxeOemIpmiPackageLib.inf
  #
  # IPMI Config Menu in Device Manager or Setup Utility
  #
!if gH2OIpmiPkgTokenSpaceGuid.PcdIpmiConfigInDeviceManager
  DxeIpmiDmConfigVfrLib|InsydeIpmiPkg/Library/DxeIpmiDmConfigVfrLib/DxeIpmiDmConfigVfrLib.inf
  DxeOemIpmiDmConfigLibDefault|InsydeIpmiPkg/Library/DxeOemIpmiDmConfigLib/DxeOemIpmiDmConfigLibDefault.inf
  DxeOemIpmiDmConfigLib|InsydeIpmiPkg/Library/DxeOemIpmiDmConfigLib/DxeOemIpmiDmConfigLib.inf
!else
  DxeIpmiSetupUtilityLib|InsydeIpmiPkg/Library/DxeIpmiSetupUtilityLib/DxeIpmiSetupUtilityLib.inf
!endif
  #
  # Library for BDS
  #
  DxeIpmiBdsLib|InsydeIpmiPkg/Library/DxeIpmiBdsLib/DxeIpmiBdsLib.inf
!endif

[LibraryClasses.common.DXE_SMM_DRIVER]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  #
  # H2O IPMI
  #
  IpmiInterfaceLib|InsydeIpmiPkg/Library/SmmIpmiInterfaceLib/SmmIpmiInterfaceLib.inf
  IpmiSdrLib|InsydeIpmiPkg/Library/SmmIpmiSdrLib/SmmIpmiSdrLib.inf
  IpmiFruLib|InsydeIpmiPkg/Library/SmmIpmiFruLib/SmmIpmiFruLib.inf
  IpmiSelDataLib|InsydeIpmiPkg/Library/SmmIpmiSelDataLib/SmmIpmiSelDataLib.inf
  IpmiSelInfoLib|InsydeIpmiPkg/Library/SmmIpmiSelInfoLib/SmmIpmiSelInfoLib.inf
  SmmOemIpmiPackageLibDefault|InsydeIpmiPkg/Library/SmmOemIpmiPackageLib/SmmOemIpmiPackageLibDefault.inf
  SmmOemIpmiPackageLib|InsydeIpmiPkg/Library/SmmOemIpmiPackageLib/SmmOemIpmiPackageLib.inf
!endif

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.UEFI_APPLICATION]

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiLpcDecode|FALSE
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiSioConfig|FALSE
  #
  # Because IPMI Config Menu currently most displayed on Setup Utility
  #
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiConfigInDeviceManager|FALSE

[PcdsFixedAtBuild]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiBootOptionEventGuid|{ \
    0x9B, 0x3B, 0xC8, 0xC9, 0xE7, 0x81, 0x76, 0x4D, 0xA3, 0x5F, 0x06, 0x8B, 0xC0, 0xCB, 0x97, 0xAE  \ #gEndOfBdsBootSelectionProtocolGuid
  }
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiWdtStartEventGuidList|{ \
    0x31, 0xD6, 0xDA, 0x72, 0x6E, 0x37, 0xFA, 0x4C, 0xB5, 0x73, 0x6A, 0x16, 0x52, 0xA8, 0x41, 0x10, \ #gEndOfShadowRomProtocolGuid
    0x74, 0xA7, 0xDA, 0x34, 0x17, 0x14, 0xA9, 0x46, 0x92, 0x40, 0xC1, 0xB6, 0xA3, 0xB9, 0xE3, 0x46, \ #gEndOfFrontPageProtocolGuid
    0x2D, 0xBC, 0xE0, 0xDD, 0x91, 0x01, 0xE4, 0x46, 0x86, 0xCD, 0x1A, 0xC2, 0x42, 0xDE, 0xA3, 0xD1  \ #gEndOfSetupUtilityProtocolGuid
  }
  gH2OIpmiPkgTokenSpaceGuid.PcdIpmiWdtStopEventGuidList|{ \
    0xC5, 0xA9, 0x5E, 0xE8, 0x25, 0x85, 0x63, 0x43, 0x84, 0x70, 0xB1, 0x75, 0x63, 0x01, 0x53, 0x8E, \ #gStartOfShadowRomProtocolGuid
    0xEF, 0x29, 0xD3, 0xF5, 0x5B, 0x35, 0xE9, 0x42, 0x8F, 0x0F, 0xFF, 0xBD, 0xC2, 0x94, 0xE9, 0x25, \ #gStartOfFrontPageProtocolGuid
    0xE2, 0x70, 0xD4, 0x88, 0x73, 0xD1, 0xE7, 0x4E, 0x81, 0x23, 0x70, 0x57, 0x26, 0xDA, 0x08, 0x32  \ #gStartOfSetupUtilityProtocolGuid
  }
!endif

[PcdsFixedAtBuild.IPF]

[PcdsPatchableInModule]

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components.IA32]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  InsydeIpmiPkg/IpmiInterfacePei/IpmiInterfacePei.inf
!endif

[Components.X64]
!if gChipsetPkgTokenSpaceGuid.PcdH2OIpmiSupport
  InsydeIpmiPkg/IpmiInterfaceDxeSmm/IpmiInterfaceDxe.inf
  InsydeIpmiPkg/IpmiInterfaceDxeSmm/IpmiInterfaceSmm.inf
  InsydeIpmiPkg/IpmiFruDxeSmm/IpmiFruDxe.inf
  InsydeIpmiPkg/IpmiFruDxeSmm/IpmiFruSmm.inf
  InsydeIpmiPkg/IpmiSelDataDxeSmm/IpmiSelDataDxe.inf
  InsydeIpmiPkg/IpmiSelDataDxeSmm/IpmiSelDataSmm.inf
  InsydeIpmiPkg/IpmiSelInfoDxeSmm/IpmiSelInfoDxe.inf
  InsydeIpmiPkg/IpmiSelInfoDxeSmm/IpmiSelInfoSmm.inf
  InsydeIpmiPkg/IpmiMiscDxe/IpmiMiscDxe.inf
  InsydeIpmiPkg/IpmiSdrDxeSmm/IpmiSdrDxe.inf
  InsydeIpmiPkg/IpmiSdrDxeSmm/IpmiSdrSmm.inf
  InsydeIpmiPkg/IpmiUtilityDxe/IpmiUtilityDxe.inf
!if gH2OIpmiPkgTokenSpaceGuid.PcdIpmiConfigInDeviceManager
  InsydeIpmiPkg/IpmiDmConfigDxe/IpmiDmConfigDxe.inf {
    <LibraryClasses>
      HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  }
!endif
!endif

