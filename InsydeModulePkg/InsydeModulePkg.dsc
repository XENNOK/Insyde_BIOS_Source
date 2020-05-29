## @file
#  Package dscription file for InsydeModulePkg
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

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  !include InsydeModulePkg/Package.env
  PLATFORM_NAME                  = InsydeModule
  PLATFORM_GUID                  = 2346B031-A111-4D9F-8365-C05FAA6954C4
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/InsydeModulePkg
  SUPPORTED_ARCHITECTURES        = IA32|X64|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[BuildOptions]
  GCC:*_*_*_CC_FLAGS             = -DMDEPKG_NDEBUG
  INTEL:*_*_*_CC_FLAGS           = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses.ARM]
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  CmosLib|InsydeModulePkg/Library/CmosLibNull/CmosLibNull.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf

[LibraryClasses.ARM.PEIM]
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf

[LibraryClasses.IA32, LibraryClasses.X64]
  NULL|MdeModulePkg/Override/Library/CompilerStubLib/CompilerStubLib.inf

[LibraryClasses]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdePkg/Override/Library/BaseLib
  }
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  PlatformBdsLib|InsydeModulePkg/Library/PlatformBdsLibNull/PlatformBdsLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf

  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  GenericUtilityLib|InsydeModulePkg/Library/GenericUtilityLib/GenericUtilityLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  ImageRelocationLib|InsydeModulePkg/Library/ImageRelocationLib/ImageRelocationLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  HiiExLib|InsydeModulePkg/Library/UefiHiiExLib/UefiHiiExLib.inf
  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf

  #
  # Flash Device Support
  #
  FdSupportLib|InsydeModulePkg/Library/FlashDeviceSupport/FdSupportLib/FdSupportLib.inf
  FlashDevicesLib|InsydeFlashDevicePkg/Library/FlashDevicesLib/FlashDevicesLib.inf
  SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf

  #
  # TPM 1.2
  #
  TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
  Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibTcg/Tpm12DeviceLibTcg.inf  
  #
  # TPM 2.0
  #
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tpm2DeviceLib|InsydeModulePkg/Library/Tpm2DeviceLibProtocolBased/Tpm2DeviceLibProtocolBased.inf
  Tpm2TisCommLib|InsydeModulePkg/Library/Tpm2TisCommLib/Tpm2TisCommLib.inf

  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf

  #
  # Misc
  #
!if $(EFI_DEBUG) == YES || $(USB_DEBUG_SUPPORT) == YES
  DebugLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLib/IrsiRegistrationLib.inf
  BvdtLib|InsydeModulePkg/Library/BvdtLib/BvdtLib.inf
  CmosLib|InsydeModulePkg/Library/CmosLib/CmosLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  S3IoLib|MdePkg/Library/BaseS3IoLib/BaseS3IoLib.inf
  S3PciLib|MdePkg/Library/BaseS3PciLib/BaseS3PciLib.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  HidDescriptorLib|InsydeModulePkg/Library/HidDescriptorLib/HidDescriptorLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  VariableSupportLib|InsydeModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  PostCodeLib|InsydeModulePkg/Library/BasePostCodeLib/BasePostCodeLib.inf
  HiiLib|InsydeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  ConsoleLib|InsydeModulePkg/Library/ConsoleLib/ConsoleLib.inf
  KernelConfigLib|InsydeModulePkg/Library/DxeKernelConfigLib/DxeKernelConfigLib.inf
  #CpuConfigLib|IA32FamilyCpuPkg/Library/CpuConfigLib/CpuConfigLib.inf
  BaseSetupDefaultLib|InsydeModulePkg/Library/SetupDefaultLib/SetupDefaultLib.inf
  KernelVfrLib|InsydeModulePkg/Library/SetupUtilityLib/KernelVfrLib.inf
  PlatformVfrLib|InsydeModulePkg/Library/PlatformVfrLibNull/PlatformVfrLibNull.inf
  SeamlessRecoveryLib|InsydeModulePkg/Library/SeamlessRecoveryLib/SeamlessRecoveryLib.inf
  StdLib|InsydeModulePkg/Library/StdLib/StdLib.inf

  CommonEcLib|InsydeModulePkg/Library/CommonEcLib/CommonEcLib.inf
  BaseOemSvcKernelLib|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLib.inf
  BaseOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLibDefault.inf
  VariableLib|InsydeModulePkg/Library/DxeVariableLib/DxeVariableLib.inf
  #
  # SIO Library
  #
  SioLib|InsydeModulePkg/Library/SioLib/SioLib.inf

  MultiConfigBaseLib|InsydeModulePkg/Library/MultiConfigBaseLib/MultiConfigBaseLib.inf
  Win32Lib|InsydeModulePkg/Library/Win32Lib/Win32Lib.inf
  DELib|InsydeSetupPkg/Library/H2ODisplayEngineLib/H2ODisplayEngineLib.inf
  LayoutLib|InsydeSetupPkg/Library/LayoutSupportLib/LayoutSupportLib.inf
  RectLib|InsydeModulePkg/Library/RectLib/RectLib.inf
  BitBltLib|InsydeModulePkg/Library/BitBltLib/BitBltLib.inf
  MediaPlayerLib|InsydeModulePkg/Library/MediaPlayerLibNull/MediaPlayerLibNull.inf
  
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/DxeFlashRegionLib/DxeFlashRegionLib.inf    
  

[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  H2ODebugLib|InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
  KernelConfigLib|InsydeModulePkg/Library/KernelConfigLibNull/KernelConfigLibNull.inf
  TimerLib|PerformancePkg/Library/TscTimerLib/PeiTscTimerLib.inf
  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/PeiFlashRegionLib/PeiFlashRegionLib.inf
  #
  # Required Library Classes for Flash Device Support
  #
  SpiAccessInitLib|InsydeModulePkg/Library/FlashDeviceSupport/PeiSpiAccessInitLib/PeiSpiAccessInitLib.inf
  FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/PeiFlashWriteEnableLib/PeiFlashWriteEnableLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  PeiOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLibDefault.inf
  PeiOemSvcKernelLib|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf
  PeiChipsetSvcLib|InsydeModulePkg/Library/PeiChipsetSvcLib/PeiChipsetSvcLib.inf
  SeamlessRecoveryLib|InsydeModulePkg/Library/PeiSeamlessRecoveryLib/SeamlessRecoveryLib.inf
  #
  # TPM 1.2
  #
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
  #
  # TPM 2.0
  #
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tpm2DeviceLib|InsydeModulePkg/Library/Tpm2DeviceLibPpiBased/Tpm2DeviceLibPpiBased.inf

  #
  # VariableLib
  #
  VariableLib|InsydeModulePkg/Library/PeiVariableLib/PeiVariableLib.inf
  MultiConfigBaseLib|InsydeModulePkg/Library/PeiMultiConfigLib/PeiMultiConfigLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf

[LibraryClasses.common.DXE_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  CommonSmbiosLib|InsydeModulePkg/Library/CommonPlatformLib/CommonSmbiosLib/CommonSmbiosLib.inf
  BadgingSupportLib|InsydeModulePkg/Library/CommonPlatformLib/BadgingSupportLib/BadgingSupportLib.inf
  AcpiPlatformLib|InsydeModulePkg/Library/CommonPlatformLib/AcpiPlatformLib/AcpiPlatformLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
  CapsuleUpdateCriteriaLib|InsydeModulePkg/Library/CapsuleUpdateCriteriaLib/CapsuleUpdateCriteriaLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  IhisiLib|InsydeModulePkg/Library/IhisiLib/IhisiLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  MemoryAllocationLib|InsydeModulePkg/Library/SmmDxeMemoryAllocationLib/SmmDxeMemoryAllocationLib.inf
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  SmmServicesTableLib|InsydeModulePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

#
# LibraryClasses for OS Tool Modules
#
[LibraryClasses.common.USER_DEFINED]
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
  MemoryAllocationLib|InsydeModulePkg/Tools/Library/MemoryAllocationLib/MemoryAllocationLib.inf
  ToolLib|InsydeModulePkg/Tools/Library/ToolLib/ToolLib.inf
  MultiConfigBaseLib|InsydeModulePkg/Library/PeiMultiConfigLib/PeiMultiConfigLib.inf


################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE

[PcdsFixedAtBuild]
  gEfiSecurityPkgTokenSpaceGuid.PcdRemovableMediaImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdOptionRomImageVerificationPolicy|0x04
  gEfiSecurityPkgTokenSpaceGuid.PcdFixedMediaImageVerificationPolicy|0x04
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Insyde"

  #
  # BusMaster Override PCDs
  # Entry format: VendorID, DeviceID, 0x00(disable BusMaster) 0x01(enable BusMaster)
  #
  gInsydeTokenSpaceGuid.PcdNoExternalDmaOverrideTable|{ \
    UINT16(0xffff), UINT16(0xffff), 0xff}     # EndEntry

[PcdsFixedAtBuild.IPF]

[PcdsPatchableInModule]

[PcdsDynamicDefault]
  #
  # Crisis File name definition
  #
  # New File Path Definition : //Volume_Label\\File_Path\\File_Name
  # Notice : "//" is signature that volume label start definition.
  #
  # Example path : //RECOVERY\\BIOS\\Current\\STRAWBERRYMOUNTAIN.fd
  gInsydeTokenSpaceGuid.PcdPeiRecoveryFile|L"STRAWBERRYMOUNTAIN.fd"|VOID*|0x100

  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|0x00000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L""|VOID*|64
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareReleaseDateString|L""|VOID*|64
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0xb6, 0xe5, 0x01, 0x8b, 0x19, 0x4f, 0xe8, 0x46, 0xab, 0x93, 0x1c, 0x53, 0x67, 0x1b, 0x90, 0xcc}
[PcdsDynamicExDefault]
  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0
  gInsydeTokenSpaceGuid.PcdPortNumberMapTable|{0}
  gInsydeTokenSpaceGuid.PcdH2OSdhcEmmcSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OSdhcSdCardSupported|TRUE
  gInsydeTokenSpaceGuid.PcdAhciPcieMemBaseAddress|{0}
  gInsydeTokenSpaceGuid.PcdAhciPcieBridgeAddress|{0}
  
  #
  # PXE procedure cancel hot key definition
  # Key struct definition {KeyCode, ScanCode, [Next hot key], 0x00, 0x00[END]}
  #
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyDefinition|{ \
    0x00, 0x17, \ # ESC
    0x00, 0x00  \ # list end
    }
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyString|"[ESC]"

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
  #
  # Common DXE Modules for IA32/X64/ARM
  #
  InsydeModulePkg/Universal/PcdSmmDxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  InsydeModulePkg/Universal/Acpi/Msdm/InstallMsdm.inf
  InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf
  InsydeModulePkg/Universal/UserInterface/UnicodeCollationDxe/UnicodeCollationDxe.inf

  InsydeModulePkg/Universal/Console/JpegDecoderDxe/JpegDecoderDxe.inf
  InsydeModulePkg/Universal/Console/TgaDecoderDxe/TgaDecoderDxe.inf
  InsydeModulePkg/Universal/Console/PcxDecoderDxe/PcxDecoderDxe.inf
  InsydeModulePkg/Universal/Console/GifDecoderDxe/GifDecoderDxe.inf
  InsydeModulePkg/Universal/Console/PngDecoderDxe/PngDecoderDxe.inf
  InsydeModulePkg/Universal/DashDxe/DashDxe.inf
  InsydeModulePkg/Library/BvdtLib/BvdtLib.inf
  InsydeModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  InsydeModulePkg/H2ODebug/Library/PeCoffExtraActionLib/PeCoffExtraActionLib.inf
  InsydeModulePkg/H2ODebug/Library/DebugAgentLib/DebugAgentLib.inf
  InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  InsydeModulePkg/H2ODebug/DebugEngineDxe/DebugEngineDxe.inf
  InsydeModulePkg/Universal/UserInterface/SetupMouseDxe/SetupMouseDxe.inf
  InsydeModulePkg/Universal/CommonPolicy/BootOptionPolicyDxe/BootOptionPolicyDxe.inf
!if $(GRAPHICS_SETUP_SUPPORT) == YES
  InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/GraphicsSetupBrowserDxe.inf
!else
  InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/SetupBrowserDxe.inf
!endif
  InsydeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  InsydeModulePkg/Universal/UserInterface/Fonts/TrueTypeDecoderDxe/TrueTypeDecoderDxe.inf

  InsydeModulePkg/Universal/UserInterface/MicrowindowsDxe/MicrowindowsDxe.inf {
    <LibraryClasses>
       TimerLib|InsydeModulePkg/Library/CpuTimerLib/CpuTimerLib.inf
  }


  #
  # HiiLayoutPkg
  #
  #InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf

  #
  # H2OFormBrowser and H2ODisplayEngine
  #
  #InsydeSetupPkg/Drivers/H2OFormBrowserDxe/H2OFormBrowserDxe.inf
  #InsydeSetupPkg/Drivers/H2ODisplayEngineLocalTextDxe/H2ODisplayEngineLocalTextDxe.inf
  #InsydeSetupPkg/Drivers/H2ODisplayEngineLocalMetroDxe/H2ODisplayEngineLocalMetroDxe.inf


  InsydeModulePkg/Universal/UserInterface/H2OKeyDescDxe/H2OKeyDescDxe.inf
  InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf
  InsydeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  InsydeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  InsydeModulePkg/Universal/Console/VgaDriverPolicyDxe/VgaDriverPolicyDxe.inf
  InsydeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf

  InsydeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf
  InsydeModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf

  InsydeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  InsydeModulePkg/Bus/Pci/OhciDxe/OhciDxe.inf
  InsydeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf

  InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf
  InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf
  InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf
  InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  InsydeModulePkg/Bus/Usb/UsbTouchPanelDxe/UsbTouchPanelDxe.inf

  InsydeModulePkg/Bus/I2c/I2cDxe/I2cDxe.inf
  InsydeModulePkg/Bus/I2c/I2cKbDxe/I2cKbDxe.inf
  InsydeModulePkg/Bus/I2c/I2cMouseDxe/I2cMouseDxe.inf
  InsydeModulePkg/Bus/I2c/I2cTouchPanelDxe/I2cTouchPanelDxe.inf

  #
  # TPM
  #
  InsydeModulePkg/Universal/Security/Tcg/TcgDxe/TcgDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/PhysicalPresenceDxe/PhysicalPresenceDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/MemoryOverwriteControl/TcgMor.inf
  #
  # TPM 2.0
  #
  InsydeModulePkg/Universal/Security/Tcg/TisDxe/TisDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEEDxe/TrEEDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  InsydeModulePkg/Universal/Security/Tcg/TrEEPhysicalPresenceDxe/TrEEPhysicalPresenceDxe.inf

  InsydeModulePkg/Universal/Security/Tcg/TrEEConfigDxe/TrEEConfigDxe.inf

  InsydeModulePkg/Universal/Security/RngDxe/RngDxe.inf
  InsydeModulePkg/Universal/Security/SysPasswordDxe/SysPasswordDxe.inf

  InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf {
    <LibraryClasses>
      #
      # FlashWriteEnableLib & SpiAccessLib shall be replaced according to the platform implementation
      #
      FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  }
  InsydeModulePkg/Universal/FirmwareVolume/FlashDeviceFvbRuntimeDxe/FlashDeviceFvbRuntimeDxe.inf  {
    <LibraryClasses>
      #
      # FlashWriteEnableLib & SpiAccessLib shall be replaced according to the platform implementation
      #
      FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  }
  InsydeModulePkg/Universal/Security/SecureFlash/SecurityErrorHandlingDxe/SecurityErrorHandlingDxe.inf
  InsydeModulePkg/Universal/Security/SecureFlash/SecureFlashDxe/SecureFlashDxe.inf

  InsydeModulePkg/Universal/Irsi/IrsiRegistrationRuntimeDxe/IrsiRegistrationRuntimeDxe.inf
  InsydeModulePkg/Library/Irsi/IrsiRegistrationLib/IrsiRegistrationLib.inf

  InsydeModulePkg/Universal/Irsi/IrsiRuntimeDxe/IrsiRuntimeDxe.inf {
    <LibraryClasses>
      FvRegionAccessLib|InsydeModulePkg/Library/Irsi/IrsiLib/FvRegionAccessLib/FvRegionAccessLib.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/GetVersion/GetVersion.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/GetBiosInfo/GetBiosInfo.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/GetPlatformInfo/GetPlatformInfo.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/GetBatteryInfoNull/GetBatteryInfo.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/ReadVariable/ReadVariable.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/WriteVariable/WriteVariable.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/ReadImage/ReadImage.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/WriteImage/WriteImage.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/FactoryCopyManagement/FactoryCopyManagement.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiLib/UpdateCapsule/UpdateCapsule.inf
      NULL|InsydeModulePkg/Library/Irsi/IrsiRuntimeVariableHookLib/IrsiRuntimeVariableHookLib.inf
  }
  InsydeModulePkg/Universal/FirmwareVolume/FirmwareManagementRuntimeDxe/FirmwareManagementRuntimeDxe.inf
  InsydeModulePkg/Universal/CapsuleUpdate/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  InsydeModulePkg/Universal/CapsuleUpdate/CapsuleLoaderTriggerDxe/CapsuleLoaderTriggerDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/VariableRegionInfoDxe/VariableRegionInfoDxe.inf

  InsydeModulePkg/Library/HidDescriptorLib/HidDescriptorLib.inf
  #
  # Secure boot relative drivers
  #
  InsydeModulePkg/Universal/Variable/VariableRuntimeDxe/VariableRuntimeDxe.inf
  InsydeModulePkg/Universal/Variable/EmuVariableRuntimeDxe/EmuVariableRuntimeDxe.inf
  InsydeModulePkg/Universal/Variable/VariableDefaultUpdateDxe/VariableDefaultUpdateDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FtwLiteRuntimeDxe/FtwLiteRuntimeDxe.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
      NULL|InsydeModulePkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
      NULL|SecurityPkg/Library/DxeDeferImageLoadLib/DxeDeferImageLoadLib.inf
      NULL|InsydeModulePkg/Library/DxeTpmMeasureBootLib/DxeTpmMeasureBootLib.inf
      NULL|InsydeModulePkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
  }

  InsydeModulePkg/Universal/StatusCode/DdtStatusCodeRuntimeDxe/DdtStatusCodeRuntimeDxe.inf {
    <PcdsFeatureFlag>
      gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt|TRUE
  }
  InsydeModulePkg/Universal/StatusCode/UsbStatusCodeRuntimeDxe/UsbStatusCodeRuntimeDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiSupportDxe/AcpiSupportDxe.inf {
    <PcdsFixedAtBuild>
      gInsydeTokenSpaceGuid.PcdFlashNvStorageBvdtSize|0x200
  }
  InsydeModulePkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf

  InsydeModulePkg/Universal/Network/NetworkLockerDxe/NetworkLockerDxe.inf
  InsydeModulePkg/Universal/Network/DhcpDummyDxe/DhcpDummyDxe.inf
  InsydeModulePkg/Universal/Network/PxeDummyDxe/PxeDummyDxe.inf
  InsydeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  InsydeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  InsydeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  InsydeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf
  InsydeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  InsydeModulePkg/Universal/Network/Ip4ConfigDxe/Ip4ConfigDxe.inf
  InsydeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  InsydeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  InsydeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  InsydeModulePkg/Universal/Network/Ip6Dxe/Ip6Dxe.inf
  InsydeModulePkg/Universal/Network/Udp6Dxe/Udp6Dxe.inf
  InsydeModulePkg/Universal/Network/Dhcp6Dxe/Dhcp6Dxe.inf
  InsydeModulePkg/Universal/Network/Mtftp6Dxe/Mtftp6Dxe.inf
  InsydeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  InsydeModulePkg/Universal/Network/TcpDxe/TcpDxe.inf
  InsydeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
  InsydeModulePkg/Universal/Acpi/InstallWpbtDxe/InstallWpbtDxe.inf
  InsydeModulePkg/Universal/Disk/DelayUefiRaidOprom/DelayUefiRaidOprom.inf

[Components.X64]
  #
  # X64 DXE modules
  #
  InsydeModulePkg/Library/CmosLib/CmosLib.inf
  InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
  InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
  InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
  InsydeModulePkg/Bus/Pci/UfsDxe/UfsDxe.inf 
  InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
  InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
  InsydeModulePkg/Csm/BiosThunk/KeyboardDxe/KeyboardDxe.inf
  InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm/Int15ServiceSmm.inf
  InsydeModulePkg/Csm/BiosThunk/VideoDxe/VideoDxe.inf
  InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
  InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
  InsydeModulePkg/Universal/Security/Tcg/TcgSmm/TcgSmm.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEESmm/TrEESmm.inf
  InsydeModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
      #
      # FlashWriteEnableLib & SpiAccessLib shall be replaced according to the platform implementation
      #
      FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  }
  InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf
  InsydeModulePkg/Universal/StatusCode/CmosStatusCodeRuntimeDxe/CmosStatusCodeRuntimeDxe.inf

  #
  # Common Platform
  #
  InsydeModulePkg/Universal/CommonPolicy/CommonPciPlatformDxe/CommonPciPlatformDxe.inf



  InsydeModulePkg/Universal/Security/BiosProtectDxe/BiosProtectDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  InsydeModulePkg/Bus/Smbus/ProgClkGenPeim/ProgClkGenPeim.inf
  InsydeModulePkg/Universal/CommonPolicy/PciHotPlugDxe/PciHotPlugDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiCallBacksSmm/AcpiCallBacksSmm.inf
  InsydeModulePkg/Universal/Recovery/EmuPeiGateDxe/EmuPeiGateDxe.inf
  InsydeModulePkg/Universal/IsaAcpiDxe/IsaAcpi.inf

[Components.IA32]
  #
  # IA32 PEI Modules
  #
  InsydeModulePkg/Bus/Isa/Ps2KeyboardPei/Ps2KeyboardPei.inf
  InsydeModulePkg/H2ODebug/UsbDebugIoPei/UsbDebugIoPei.inf

  InsydeModulePkg/Bus/Pci/SdhcPei/SdHostDriverPei.inf
  InsydeModulePkg/Bus/Sdhc/SdMmcPei/SdMmcDevicePei.inf
  InsydeModulePkg/Bus/Pci/IdeBusPei/IdeBusPei.inf
  InsydeModulePkg/Bus/Pci/AhciBusPei/AhciBusPei.inf

  InsydeModulePkg/Bus/Pci/UfsPei/UfsPei.inf 

  InsydeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
  InsydeModulePkg/Bus/Pci/OhciPei/OhciPei.inf
  InsydeModulePkg/Bus/Pci/UhciPei/UhciPei.inf
  InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
  InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf
  InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  InsydeModulePkg/Universal/Security/SecureFlash/SecureFlashPei/SecureFlashPei.inf

  #
  # TPM
  #
  InsydeModulePkg/Universal/Security/Tcg/TrEEConfigPei/TrEEConfigPei.inf
  InsydeModulePkg/Universal/Security/Tcg/TcgPei/TcgPei.inf
  #
  # TPM 2.0
  #
  InsydeModulePkg/Universal/Security/Tcg/TisPei/TisPei.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEEPei/TrEEPei.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }

  InsydeModulePkg/Universal/Variable/VariablePei/VariablePei.inf
  InsydeModulePkg/Universal/StatusCode/DdtStatusCodePei/DdtStatusCodePei.inf {
    <PcdsFeatureFlag>
      gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt|TRUE
  }
  InsydeModulePkg/Universal/StatusCode/UsbStatusCodePei/UsbStatusCodePei.inf
  InsydeModulePkg/Universal/Recovery/FatPei/FatPei.inf
  InsydeModulePkg/Universal/Recovery/CdExpressPei/CdExpressPei.inf
  InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/CrisisRecoveryPei.inf {
    <LibraryClasses>
      #
      # SpiAccessLib must be replaced according to the platform implementation
      #
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
      SeamlessRecoveryLib|InsydeModulePkg/Library/SeamlessRecoveryLib/SeamlessRecoveryLib.inf
  }
  InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
  InsydeModulePkg/Universal/StatusCode/CmosStatusCodePei/CmosStatusCodePei.inf

  #
  # Common Platform
  #
  InsydeModulePkg/Universal/CommonPolicy/PlatformStage1Pei/PlatformStage1Pei.inf
  InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf
  InsydeModulePkg/Universal/Recovery/EmuSecPei/EmuSecPei.inf
  InsydeModulePkg/Universal/Recovery/EmuPeiHelperPei/EmuPeiHelperPei.inf


  #
  # IA32 DXE Modules
  #
  InsydeModulePkg/Library/CmosLib/CmosLib.inf
  InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
  InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
  InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
  InsydeModulePkg/Bus/Pci/UfsDxe/UfsDxe.inf 
  InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
  InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
  InsydeModulePkg/Csm/BiosThunk/KeyboardDxe/KeyboardDxe.inf
  InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm/Int15ServiceSmm.inf
  InsydeModulePkg/Csm/BiosThunk/VideoDxe/VideoDxe.inf
  InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
  InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
  InsydeModulePkg/Universal/Security/Tcg/TcgSmm/TcgSmm.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEESmm/TrEESmm.inf
  InsydeModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
      #
      # FlashWriteEnableLib & SpiAccessLib shall be replaced according to the platform implementation
      #
      FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  }
  InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf
  InsydeModulePkg/Universal/StatusCode/CmosStatusCodeRuntimeDxe/CmosStatusCodeRuntimeDxe.inf

  #
  # Common Platform
  #
  InsydeModulePkg/Universal/CommonPolicy/CommonPciPlatformDxe/CommonPciPlatformDxe.inf

  InsydeModulePkg/Universal/Security/BiosProtectDxe/BiosProtectDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  InsydeModulePkg/Bus/Smbus/ProgClkGenPeim/ProgClkGenPeim.inf
  InsydeModulePkg/Universal/CommonPolicy/PciHotPlugDxe/PciHotPlugDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiCallBacksSmm/AcpiCallBacksSmm.inf
  InsydeModulePkg/Universal/Recovery/EmuPeiGateDxe/EmuPeiGateDxe.inf

  InsydeModulePkg/Universal/Variable/SetupDataProviderPei/SetupDataProviderPei.inf
  InsydeModulePkg/Universal/IsaAcpiDxe/IsaAcpi.inf
  InsydeModulePkg/Universal/Misc/FirmwareRevisionSyncPei/FirmwareRevisionSyncPei.inf

[Components.ARM]
  #
  # ARM PEI Modules
  #
  InsydeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
  InsydeModulePkg/Bus/Pci/OhciPei/OhciPei.inf
  InsydeModulePkg/Bus/Pci/UhciPei/UhciPei.inf
  InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
  InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf
  InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  InsydeModulePkg/Universal/Security/SecureFlash/SecureFlashPei/SecureFlashPei.inf
  #
  # TPM
  #
  InsydeModulePkg/Universal/Security/Tcg/TcgPei/TcgPei.inf
  InsydeModulePkg/Universal/Variable/VariablePei/VariablePei.inf
  InsydeModulePkg/Universal/StatusCode/DdtStatusCodePei/DdtStatusCodePei.inf {
    <PcdsFeatureFlag>
      gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt|TRUE
  }
  InsydeModulePkg/Universal/StatusCode/UsbStatusCodePei/UsbStatusCodePei.inf
  InsydeModulePkg/Universal/Recovery/FatPei/FatPei.inf
  InsydeModulePkg/Universal/Recovery/CdExpressPei/CdExpressPei.inf
  InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/CrisisRecoveryPei.inf {
    <LibraryClasses>
      #
      # SpiAccessLib must be replaced according to the platform implementation
      #
      SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
  }
  InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
  InsydeModulePkg/Universal/Misc/FirmwareRevisionSyncPei/FirmwareRevisionSyncPei.inf
