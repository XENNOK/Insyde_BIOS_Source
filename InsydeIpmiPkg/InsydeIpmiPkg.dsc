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
  PLATFORM_NAME                  = InsydeIpmiPkg
  PLATFORM_GUID                  = 6D1B6F07-53D7-4754-88AB-20CAA09CF464
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/InsydeIpmiPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  #
  # Entry point
  #
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  #
  # Misc
  #
  DebugLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
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

[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  #
  # H2O IPMI
  #
  IpmiInterfaceLib|InsydeIpmiPkg/Library/PeiIpmiInterfaceLib/PeiIpmiInterfaceLib.inf
  PeiIpmiPackageLib|InsydeIpmiPkg/Library/PeiIpmiPackageLibNull/PeiIpmiPackageLibNull.inf
  PeiIpmiLpcLib|InsydeIpmiPkg/Library/PeiIpmiLpcLibNull/PeiIpmiLpcLibNull.inf
  PeiIpmiSioLib|InsydeIpmiPkg/Library/PeiIpmiSioLibNull/PeiIpmiSioLibNull.inf
  PeiOemIpmiPackageLibDefault|InsydeIpmiPkg/Library/PeiOemIpmiPackageLib/PeiOemIpmiPackageLibDefault.inf
  PeiOemIpmiPackageLib|InsydeIpmiPkg/Library/PeiOemIpmiPackageLib/PeiOemIpmiPackageLib.inf

[LibraryClasses.common.DXE_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  #
  # H2O IPMI
  #
  DxeIpmiPackageLib|InsydeIpmiPkg/Library/DxeIpmiPackageLibNull/DxeIpmiPackageLibNull.inf
  DxeOemIpmiPackageLibDefault|InsydeIpmiPkg/Library/DxeOemIpmiPackageLib/DxeOemIpmiPackageLibDefault.inf
  DxeOemIpmiPackageLib|InsydeIpmiPkg/Library/DxeOemIpmiPackageLib/DxeOemIpmiPackageLib.inf
  DxeIpmiDmConfigVfrLib|InsydeIpmiPkg/Library/DxeIpmiDmConfigVfrLib/DxeIpmiDmConfigVfrLib.inf
  DxeOemIpmiDmConfigLibDefault|InsydeIpmiPkg/Library/DxeOemIpmiDmConfigLib/DxeOemIpmiDmConfigLibDefault.inf
  DxeOemIpmiDmConfigLib|InsydeIpmiPkg/Library/DxeOemIpmiDmConfigLib/DxeOemIpmiDmConfigLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
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

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.UEFI_APPLICATION]

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]

[PcdsFixedAtBuild]

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

[Components]
  InsydeIpmiPkg/IpmiInterfacePei/IpmiInterfacePei.inf
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
  InsydeIpmiPkg/IpmiDmConfigDxe/IpmiDmConfigDxe.inf

