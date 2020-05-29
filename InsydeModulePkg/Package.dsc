## @file
# Platform Package Description file
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
#

[Defines]
  !include InsydeModulePkg/Package.env
  RFC_LANGUAGES                  = "en-US;fr-FR;zh-TW;ja-JP"

[PcdsFeatureFlag]
  gInsydeTokenSpaceGuid.PcdStatusCodeUseCmos|FALSE
  
!if gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported == TRUE
  gInsydeTokenSpaceGuid.PcdH2OUsbPeiSupported|TRUE
!endif

[LibraryClasses]
  H2ODebugLib|InsydeModulePkg/Library/H2ODebugLibNull/H2ODebugLib.inf
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported
  PeCoffExtraActionLib|InsydeModulePkg/H2ODebug/Library/PeCoffExtraActionLib/PeCoffExtraActionLib.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported
  DebugAgentLib|InsydeModulePkg/H2ODebug/Library/DebugAgentLib/DebugAgentLib.inf
!endif
  PostCodeLib|InsydeModulePkg/Library/BasePostCodeLib/BasePostCodeLib.inf
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  GenericUtilityLib|InsydeModulePkg/Library/GenericUtilityLib/GenericUtilityLib.inf
  BvdtLib|InsydeModulePkg/Library/BvdtLib/BvdtLib.inf
  CmosLib|InsydeModulePkg/Library/CmosLib/CmosLib.inf
  FvRegionAccessLib|InsydeModulePkg/Library/Irsi/IrsiLib/FvRegionAccessLib/FvRegionAccessLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLib/IrsiRegistrationLib.inf
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OTpm2Supported == 1
  Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibTcg/Tpm12DeviceLibTcg.inf  
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
  Tpm2TisCommLib|InsydeModulePkg/Library/Tpm2TisCommLib/Tpm2TisCommLib.inf
  Tpm2DeviceLib|InsydeModulePkg/Library/Tpm2DeviceLibProtocolBased/Tpm2DeviceLibProtocolBased.inf
!endif

  VariableSupportLib|InsydeModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  ImageRelocationLib|InsydeModulePkg/Library/ImageRelocationLib/ImageRelocationLib.inf
  HiiLib|InsydeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  HiiExLib|InsydeModulePkg/Library/UefiHiiExLib/UefiHiiExLib.inf
  ConsoleLib|InsydeModulePkg/Library/ConsoleLib/ConsoleLib.inf

  FdSupportLib|InsydeModulePkg/Library/FlashDeviceSupport/FdSupportLib/FdSupportLib.inf

  KernelConfigLib|InsydeModulePkg/Library/DxeKernelConfigLib/DxeKernelConfigLib.inf
  BaseSetupDefaultLib|InsydeModulePkg/Library/SetupDefaultLib/SetupDefaultLib.inf
  KernelVfrLib|InsydeModulePkg/Library/SetupUtilityLib/KernelVfrLib.inf

  SeamlessRecoveryLib|InsydeModulePkg/Library/SeamlessRecoveryLib/SeamlessRecoveryLib.inf

  CommonEcLib|InsydeModulePkg/Library/CommonEcLib/CommonEcLib.inf
  VariableLib|InsydeModulePkg/Library/DxeVariableLib/DxeVariableLib.inf
  #
  # SIO Library
  #
  SioLib|InsydeModulePkg/Library/SioLib/SioLib.inf
  SerialPortLib|InsydeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf

  MultiConfigBaseLib|InsydeModulePkg/Library/MultiConfigBaseLib/MultiConfigBaseLib.inf
  StdLib|InsydeModulePkg/Library/StdLib/StdLib.inf
  Win32Lib|InsydeModulePkg/Library/Win32Lib/Win32Lib.inf

!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported
  DELib|InsydeSetupPkg/Library/H2ODisplayEngineLib/H2ODisplayEngineLib.inf
  LayoutLib|InsydeSetupPkg/Library/LayoutSupportLib/LayoutSupportLib.inf
!endif
  RectLib|InsydeModulePkg/Library/RectLib/RectLib.inf
  BitBltLib|InsydeModulePkg/Library/BitBltLib/BitBltLib.inf
  MediaPlayerLib|InsydeModulePkg/Library/MediaPlayerLibNull/MediaPlayerLibNull.inf

  HidDescriptorLib|InsydeModulePkg/Library/HidDescriptorLib/HidDescriptorLib.inf
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/DxeFlashRegionLib/DxeFlashRegionLib.inf    

[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]
  H2ODebugLib|InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/PeiDebugPrintErrorLevelLib/PeiDebugPrintErrorLevelLib.inf
  VariableLib|InsydeModulePkg/Library/PeiVariableLib/PeiVariableLib.inf
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/PeiFlashRegionLib/PeiFlashRegionLib.inf     

[LibraryClasses.common.PEIM]
  H2ODebugLib|InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/PeiDebugPrintErrorLevelLib/PeiDebugPrintErrorLevelLib.inf
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OTpm2Supported == 1
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
  Tpm2DeviceLib|InsydeModulePkg/Library/Tpm2DeviceLibPpiBased/Tpm2DeviceLibPpiBased.inf
!endif

!if gInsydeTokenSpaceGuid.PcdSecureFlashSupported
  FirmwareAuthenticationLib|InsydeModulePkg/Library/PeiFirmwareAuthenticationLib/PeiFirmwareAuthenticationLib.inf
!else
  FirmwareAuthenticationLib|InsydeModulePkg/Library/FirmwareAuthenticationLibNull/FirmwareAuthenticationLibNull.inf
!endif
  SpiAccessInitLib|InsydeModulePkg/Library/FlashDeviceSupport/PeiSpiAccessInitLib/PeiSpiAccessInitLib.inf
  FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/PeiFlashWriteEnableLib/PeiFlashWriteEnableLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  PeiChipsetSvcLib|InsydeModulePkg/Library/PeiChipsetSvcLib/PeiChipsetSvcLib.inf
  SeamlessRecoveryLib|InsydeModulePkg/Library/PeiSeamlessRecoveryLib/SeamlessRecoveryLib.inf
  VariableLib|InsydeModulePkg/Library/PeiVariableLib/PeiVariableLib.inf
  MultiConfigBaseLib|InsydeModulePkg/Library/PeiMultiConfigLib/PeiMultiConfigLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/PeiFlashRegionLib/PeiFlashRegionLib.inf    

[LibraryClasses.common.DXE_CORE]
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  BvdtLib|InsydeModulePkg/Library/BvdtLib/BvdtLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf
  CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
  CapsuleUpdateCriteriaLib|InsydeModulePkg/Library/CapsuleUpdateCriteriaLib/CapsuleUpdateCriteriaLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf

[LibraryClasses.common.DXE_DRIVER]
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf
  CommonSmbiosLib|InsydeModulePkg/Library/CommonPlatformLib/CommonSmbiosLib/CommonSmbiosLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  BadgingSupportLib|InsydeModulePkg/Library/CommonPlatformLib/BadgingSupportLib/BadgingSupportLib.inf
  AcpiPlatformLib|InsydeModulePkg/Library/CommonPlatformLib/AcpiPlatformLib/AcpiPlatformLib.inf
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  IhisiLib|InsydeModulePkg/Library/IhisiLib/IhisiLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
  SmmServicesTableLib|InsydeModulePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  MemoryAllocationLib|InsydeModulePkg/Library/SmmDxeMemoryAllocationLib/SmmDxeMemoryAllocationLib.inf
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  CommonPciLib|InsydeModulePkg/Library/CommonPlatformLib/CommonPciLib/CommonPciLib.inf

#
# LibraryClasses for OS Tool Modules
#
[LibraryClasses.common.USER_DEFINED]
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/PeiCryptLib.inf
  MemoryAllocationLib|InsydeModulePkg/Tools/Library/MemoryAllocationLib/MemoryAllocationLib.inf
  ToolLib|InsydeModulePkg/Tools/Library/ToolLib/ToolLib.inf
  MultiConfigBaseLib|InsydeModulePkg/Library/PeiMultiConfigLib/PeiMultiConfigLib.inf

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

  gInsydeTokenSpaceGuid.PcdLowestSupportedFirmwareVersion|0x52430000
  #
  # Post Code Debug Port
  #
  gInsydeTokenSpaceGuid.PcdH2ODebugPort|0x80

  #
  # CMOS Debug Code Table
  #
  gInsydeTokenSpaceGuid.PcdPeiKernelDebugCodeTable|{ \
    GUID("4AA65902-3ED3-43b3-A52B-FC1D6C077AE4"), 0x13, 0x09, \   # AhciBusPei
    GUID("B7A5041A-78BA-49e3-B73B-54C757811FB6"), 0x13, 0x0A, \   # AtapiPeim
    GUID("31e147a6-d39a-4147-9da3-befd4d523243"), 0x1F, 0x03, \   # CdExpressPei
    GUID("689C7998-7DC9-4350-B19D-0EECA6CA64AB"), 0x28, 0x02, \   # ChipsetLibServicesPei(reserved)
    GUID("9D225237-FA01-464C-A949-BAABC02D31D0"), 0x01, 0x01, \   # StatusCodeHandlerPei
    GUID("3E963A2F-E728-4218-98D8-CB9E22215320"), 0x29, 0x01, \   # CrisisRecoveryPei
    GUID("D8E26C9E-CB12-469C-AD6C-FBC03864D360"), 0x01, 0x03, \   # DdtStatusCodePei
    GUID("9A5D7D47-CF0C-451c-8E4B-B051EFC3CEDB"), 0x14, 0x0D, \   # EhciPei
    GUID("3D17205B-4C49-47e2-8157-864CD3D80DBD"), 0x1F, 0x02, \   # FatPei
    GUID("F04D6DBD-1200-4C74-9EC9-B306C9EB0F38"), 0x27, 0x04, \   # FirmwareAuthenticationPei
    GUID("B0CC7B9B-189D-4c20-98B1-9035F3520107"), 0x08, 0x01, \   # OemServicesPei(reserved)
    GUID("4FE772E8-FE3E-4086-B638-8C493C490488"), 0x04, 0x01, \   # OpPresence
    GUID("7726CDF8-737F-4b09-A254-FA396C6225C1"), 0x2C, 0x01, \   # PeiSmbus2OnPeiSmbusThunk
    GUID("67C63A11-F89D-4500-8270-D9DB251EB2AF"), 0x13, 0x04, \   # Ps2KeyboardPei
    GUID("4DE01DE6-7CBB-4786-9F2A-9B9C7C97AC06"), 0x09, 0x04, \   # S3RestoreAcpiPei
    GUID("CD75BEB3-6BBD-42f0-AEA7-D8B98CB26A1C"), 0x14, 0x11, \   # SdhcPei
    GUID("EAA006CD-3256-789B-BD20-EBABCD02583F"), 0x27, 0x02, \   # SecureFlashPei
    GUID("2BE1E4A6-6505-43b3-9FFC-A3C8330E0432"), 0x04, 0x02, \   # TcgPei
    GUID("8401A045-6F70-4505-8471-7015B40355E3"), 0x14, 0x03, \   # UsbBusPei
    GUID("BA25E07A-E253-442c-90FF-B4C92FAD6990"), 0x14, 0x07, \   # UsbMassStoragePei
    GUID("34C8C28F-B61C-45a2-8F2E-89E46BECC63B"), 0x03, 0x01, \   # VariablePei
    GUID("091AAEA3-09A9-40f6-9864-0139388A9F8C"), 0x14, 0x0F, \   # XhciPei
    GUID("49B7F3E1-6C08-4a5b-911C-E9E397ED4178"), 0x2E, 0x01, \   # AcpiVariableHobOnSmramReserveHobThunk
    GUID("C779F6D8-7113-4AA1-9648-EB1633C7D53B"), 0x22, 0x02, \   # CapsulePei
    GUID("86D70125-BAA3-4296-A62F-602BEBBB9081"), 0x06, 0x01, \   # DxeIpl
    GUID("ADF01BF6-47D6-495d-B95B-687777807214"), 0x10, 0x05, \   # FirmwarePerformancePei
    GUID("6CDDBF28-89AC-4e01-9692-616B8A1009C8"), 0x05, 0x09, \   # FvFileLoaderOnLoadFileThunk
    GUID("4F1F379F-2A62-48bb-AC34-D3F135C6E2B7"), 0x16, 0x01, \   # PcatSingleSegmentPciCfg2Pei
    GUID("717886AB-C40A-44cf-9114-4119E84B0DC7"), 0x16, 0x02, \   # PciCfgOnPciCfg2Thunk
    GUID("0FDB764B-E669-4c69-83AC-5EDD99A2711E"), 0x03, 0x05, \   # ReadOnlyVariableOnReadOnlyVariable2Thunk
    GUID("89E549B0-7CFE-449d-9BA3-10D8B2312D71"), 0x07, 0x01, \   # S3Resume2Pei
    GUID("00000000-0000-0000-0000-000000000000"), 0x00, 0x00}     # EndEntry

  gInsydeTokenSpaceGuid.PcdDxeKernelDebugCodeTable|{ \
    GUID("2BDED685-F733-455f-A840-43A22B791FB3"), 0x09, 0x03, \   # AcpiS3SaveDxe
    GUID("506533a6-e626-4500-b14f-17939c0e5b60"), 0x10, 0x01, \   # AcpiSupport
    GUID("3ACC966D-8E33-45c6-B4FE-62724BCD15A9"), 0x13, 0x08, \   # AhciBusDxe
    GUID("529D3F93-E8E9-4e73-B1E1-BDF6A9D50113"), 0x2B, 0x05, \   # ArpDxe
    GUID("FC5C7020-1A48-4198-9BE2-EAD5ABC8CF2F"), 0xFF, 0xFF, \   # BdsDxe
    GUID("4495E47E-42A9-4007-8c17-B6664F909D04"), 0x20, 0x05, \   # BlockIoDxe
    GUID("A9F634A5-29F1-4456-A9D5-6E24B88BDB65"), 0x0B, 0x05, \   # BmpDecoderDxe
    GUID("B8E62775-BB0A-43f0-A843-5BE8B14F8CCD"), 0x10, 0x02, \   # BootGraphicsResourceTableDxe
    GUID("2D6BB83D-84A2-404a-B4CA-3E7AC0EFB3D2"), 0x17, 0x01, \   # BootOptionPolicyDxe
    GUID("C62CEB80-FB40-4A46-A5E5-C1D997C36DFC"), 0x22, 0x03, \   # CapsuleLoaderTriggerDxe
    GUID("42857F0A-13F2-4B21-8A23-53D3F714B840"), 0x22, 0x01, \   # CapsuleRuntimeDxe
    GUID("E2EAE962-C492-4ca4-A11F-1A7CBB050A40"), 0x1B, 0x06, \   # ChineseDxe
    GUID("EA6D974D-AD75-40ed-BCDD-FDA297AA8F8A"), 0x28, 0x01, \   # ChipsetLibServicesDxe(reserved)
    GUID("78270D5E-9450-4FAE-8818-B91EA39864CD"), 0x28, 0x03, \   # ChipsetLibServicesSmm(reserved)
    GUID("6C2004EF-4E0E-4BE4-B14C-340EB4AA5891"), 0x01, 0x02, \   # StatusCodeHandlerRuntimeDxe
    GUID("6069277B-246B-45d5-BD6D-81820E10C11F"), 0x25, 0x01, \   # CommonSmiCallBackSmm
    GUID("51ccf399-4fdf-4e55-a45b-e123f84d456a"), 0x1C, 0x01, \   # ConPlatformDxe
    GUID("9E62300C-E94F-41a6-A70C-D7E47577B38C"), 0x1C, 0x08, \   # ConsoleRedirectionHook
    GUID("4AC99A7C-1DCF-4a51-8F06-5EEDFE8A8864"), 0x1A, 0x03, \   # CryptoServiceRuntimeDxe
    GUID("F797737A-C48B-4FB7-8D15-32AAC4AC965A"), 0x01, 0x04, \   # DdtStatusCodeRuntimeDxe
    GUID("4803B88E-9E66-45dc-8709-FC75D39CAF1D"), 0xFF, 0xFF, \   # DebugEngineDxe
    GUID("94734718-0BBC-47fb-96A5-EE7A5AE6A2AD"), 0x2B, 0x0A, \   # Dhcp4Dxe
    GUID("94E24C26-80FA-427d-80FB-E374E9E9BF85"), 0x2B, 0x0E, \   # Dhcp6Dxe
    GUID("9C7CCB02-9154-4864-9E4B-DC0487E37660"), 0x2B, 0x02, \   # DhcpDummyDxe
    GUID("A210F973-229D-4f4d-AA37-9895E6C9EABA"), 0x2B, 0x03, \   # DpcDxe
    GUID("2818256A-6BDB-4871-993C-95315854012E"), 0x1C, 0x07, \   # EdidOverrideDxe
    GUID("BDFE430E-8F2A-4db0-9991-6F856594777E"), 0x14, 0x0C, \   # EhciDxe
    GUID("02B01AD5-7E59-43e8-A6D8-238180613A5A"), 0x03, 0x03, \   # EmuVariableRuntimeDxe
    GUID("E2EAE962-C492-4ca4-A11F-1A7CBB050A41"), 0x1B, 0x04, \   # EnglishDxe
    GUID("B795675E-7583-4F98-AC6A-F9DDBBCC32E8"), 0x27, 0x05, \   # FirmwareManagementRuntimeDxe
    GUID("B0C2372B-9393-4CBC-9CAF-53913C9DCE29"), 0x05, 0x07, \   # FlashDeviceFvbRuntimeDxe
    GUID("E2EAE962-C492-4ca4-A11F-1A7CBB050A42"), 0x1B, 0x05, \   # FrenchDxe
    GUID("4C862FC6-0E54-4e36-8C8F-FF6F3167951F"), 0x05, 0x02, \   # FtwLite
    GUID("4C862FC6-0E54-4e36-8C8F-FF6F3167951F"), 0x05, 0x03, \   # FtwLiteRuntimeDxe
    GUID("BDFE5FAA-2A35-44bb-B17A-8084D4E2B9E9"), 0x05, 0x06, \   # FvbServicesRuntimeDxe
    GUID("EEC9BC9A-2907-4d59-8B86-D1EC67DE62E1"), 0x1E, 0x01, \   # GenericMemoryTestDxe
    GUID("1353DE63-B74A-4bef-80FD-2C5CFA83040B"), 0x0B, 0x04, \   # GifDecoderDxe
    GUID("E8571188-00C1-4ed4-B14E-E38451351EC4"), 0x1A, 0x04, \   # HddPassword
    GUID("69FD8E47-A161-4550-B01A-5594CEB2B2B2"), 0x13, 0x05, \   # IdeBusDxe
    GUID("6903A447-CB4F-45f6-89A2-7E7E2F9EE14C"), 0x10, 0x04, \   # InstallMsdm
    GUID("26841BDE-920A-4e7a-9FBE-637F477143A6"), 0x2B, 0x08, \   # Ip4ConfigDxe
    GUID("9FB1A1F3-3B71-4324-B39A-745CBB015FFF"), 0x2B, 0x07, \   # Ip4Dxe
    GUID("5BEDB5CC-D830-4eb2-8742-2D4CC9B54F2C"), 0x2B, 0x0C, \   # Ip6Dxe
    GUID("E8DDEB8B-82D7-4B6E-A2B4-D5EAEC2B8976"), 0x2A, 0x01, \   # IrsiRegistrationRuntimeDxe
    GUID("BA118F88-5B65-4584-9C5A-C2D087F6ED1E"), 0x2A, 0x02, \   # IrsiRuntimeDxe
    GUID("E82DA059-E213-46a3-A743-21A4296DACB4"), 0x1B, 0x07, \   # JapaneseDxe
    GUID("2707E46D-DBD7-41c2-9C04-C9FDB8BAD86C"), 0x0B, 0x01, \   # JpegDecoderDxe
    GUID("5479662B-6AE4-49e8-A6BD-6DE4B625811F"), 0x20, 0x01, \   # KeyboardDxe
    GUID("F122A15C-C10B-4d54-8F48-60F4F06DD1AD"), 0x20, 0x04, \   # LegacyBios
    GUID("025BBFC7-E6A9-4b8b-82AD-6815A1AEAF4A"), 0x2B, 0x04, \   # MnpDxe
    GUID("27BEDA18-AE2B-43c2-AF6B-74952441DE28"), 0x20, 0x03, \   # MonitorKeyDxe
    GUID("0BB51CDD-A750-442f-A15E-7DEADFBA30FF"), 0x2B, 0x0B, \   # Mtftp4Dxe
    GUID("2EC499F9-0337-4da1-91CA-6BC4E8C03DA2"), 0x2B, 0x0F, \   # Mtftp6Dxe
    GUID("C613EC78-63ED-4b93-ACD4-54B3D4CCC268"), 0x2B, 0x01, \   # NetworkLocker
    GUID("C95E6A28-FB95-49f2-AE01-F38166FD4524"), 0x08, 0x02, \   # OemServicesDxe(reserved)
    GUID("981A25E0-0D83-436d-9184-C1AA53BB143A"), 0x08, 0x03, \   # OemServicesSmm(reserved)
    GUID("1FA1F39E-FEFF-4aae-BD7B-38A070A3B609"), 0x1F, 0x04, \   # PartitionDxe
    GUID("93B80004-9FB3-11d4-9A3A-0090273FC14D"), 0x13, 0x06, \   # PciBusDxe
    GUID("FB142B99-DF57-46cb-BC69-0BF858A734F9"), 0x13, 0x07, \   # PciSerialDxe
    GUID("A8F634A5-28F1-4456-A9D5-7E24B99BDB65"), 0x0B, 0x03, \   # PcxDecoderDxe
    GUID("D85A4A0C-2E73-4491-92E1-DCEFC3882A68"), 0x04, 0x04, \   # PhysicalPresence
    GUID("C2F6A841-5F17-4ea7-90E4-5B83969869E0"), 0x04, 0x06, \   # PhysicalPresenceReadyToBoot
    GUID("A368D636-4C77-4b50-AAE8-F99E2DA40440"), 0x11, 0x04, \   # PnpRuntimeDxe
    GUID("3DC82376-637B-40a6-A8FC-A565417F2C38"), 0x13, 0x02, \   # Ps2keyboardDxe
    GUID("202A2B0E-9A31-4812-B291-8747DF152439"), 0x13, 0x03, \   # Ps2MouseDxe
    GUID("FFACF59A-4198-4aa7-9060-E4ECFE9CFF6C"), 0x14, 0x10, \   # SdhcBusDxe
    GUID("61A72ECF-F7BF-444e-BE85-221339D0F00B"), 0x14, 0x12, \   # SdLegacySmm
    GUID("6B1AB225-2E47-4a61-8FF5-B8EA42EE3EA8"), 0x27, 0x01, \   # SecureFlashDxe
    GUID("0541150C-E33B-4daf-A263-02E4BB4BF1CF"), 0x27, 0x03, \   # SecurityErrorHandlingDxe
    GUID("4F921013-4F71-4c6c-BCF8-419B2B801932"), 0x1B, 0x03, \   # SetupBrowserDxe
    GUID("06B5947E-FF53-457d-98BC-C5ABC777FD5A"), 0x1B, 0x02, \   # SetupMouseDxe
    GUID("33C6B455-87A9-4c8c-A4F6-6DB508A6260E"), 0x2D, 0x01, \   # SkipScanRemovableDev
    GUID("F9D88642-0737-49bc-81B5-6889CD57D9EA"), 0x11, 0x02, \   # SmbiosDxe
    GUID("396E583B-D2DD-45f6-86E8-119885777CDE"), 0x14, 0x0B, \   # SmmAtaLegacy
    GUID("8c3ada4e-3605-42a8-aa7d-4e4984f8bbf7"), 0x05, 0x04, \   # SmmFtw
    GUID("74D936FA-D8BD-4633-B64D-6424BDD23D24"), 0x05, 0x05, \   # SmmFwh
    GUID("E6DB4007-113B-4605-8F5F-668D7364C807"), 0x20, 0x06, \   # SmmInt15Service
    GUID("CC1BAA36-11EB-45cc-9ADC-7565E273AC70"), 0x11, 0x03, \   # SmmPnp
    GUID("7C79AC8C-5E6C-4e3d-BA6F-C260EE7C172E"), 0x15, 0x01, \   # SmmRuntime
    GUID("E1CEE6E3-6C42-4a4f-916E-38385290A556"), 0x2B, 0x06, \   # SnpDxe
    GUID("70D57D67-7F05-494d-A014-B75D7345B700"), 0x1A, 0x02, \   # StorageSecurityCommandDxe
    GUID("42FCB444-B881-49d8-A6E9-5A1695FC9F3E"), 0x1A, 0x01, \   # SysPasswordDxe
    GUID("A5683620-7998-4bb2-A377-1C1E31E1E215"), 0x04, 0x03, \   # TcgDxe
    GUID("A39D5D20-7470-4bc3-B7DA-D27FD15F9478"), 0x04, 0x05, \   # TcgSmm
    GUID("ADCCA887-5330-414a-81A1-5B578146A397"), 0x0B, 0x02, \   # TgaDecoderDxe
    GUID("6d6963ab-906d-4a65-a7ca-bd40e5d6af2b"), 0x2B, 0x09, \   # Udp4Dxe
    GUID("40008162-93D0-48f2-80DA-7E32F4C98F8B"), 0x2B, 0x0D, \   # Udp6Dxe
    GUID("AE724F8D-62C6-4638-A065-0138F47B6D23"), 0x2B, 0x10, \   # UefiPxeBcDxe
    GUID("29CF55F8-B675-4f5d-8F2F-B87A3ECFD063"), 0x20, 0x02, \   # UefiBiosVideo
    GUID("B273CC44-E62A-41dc-9CAD-BDB4235459D8"), 0x1B, 0x08, \   # UnicodeCollationDxe
    GUID("B40612B9-A063-11d4-9A3A-0090273FC14D"), 0x14, 0x04, \   # UsbBotDxe
    GUID("240612B7-A063-11d4-9A3A-0090273FC14D"), 0x14, 0x02, \   # UsbBusDxe
    GUID("A3527D16-E6CC-42f5-BADB-BF3DE177742B"), 0x14, 0x05, \   # UsbCbiDxe
    GUID("31FD7EAF-80A7-435e-8E0A-3F185F8667DD"), 0x14, 0x01, \   # UsbCoreDxe
    GUID("2D2E62CF-9ECF-43b7-8219-94E7FC713DFE"), 0x14, 0x08, \   # UsbKbDxe
    GUID("EF33C296-F64C-4146-AD04-347899702C84"), 0x14, 0x0A, \   # UsbLegacySmm
    GUID("9FB4B4A7-42C0-4bcd-8540-9BCC6711F83E"), 0x14, 0x06, \   # UsbMassStorageDxe
    GUID("2D2E62AA-9ECF-43b7-8219-94E7FC713DFE"), 0x14, 0x09, \   # UsbMouseDxe
    GUID("760F874E-B8CB-405e-AA32-A46AE2F3D680"), 0x03, 0x04, \   # VariableDefaultUpdateDxe
    GUID("78D19E50-2B62-407F-9F49-C652A4124002"), 0x26, 0x01, \   # VariableRegionInfoDxe
    GUID("CBD2E4D5-7068-4FF5-B462-9822B4AD8D60"), 0x03, 0x02, \   # VariableRuntimeDxe
    GUID("B5F33FB5-66D5-4901-BAF1-F0C774FC6588"), 0x1C, 0x09, \   # VgaDriverPolicy
    GUID("ED92EAE0-C31C-4824-8802-35595EFA8287"), 0x10, 0x03, \   # WpbtDxe
    GUID("356C2B12-3124-4451-BF66-B502D88A0074"), 0x14, 0x0E, \   # XhciDxe
    GUID("FA20568B-548B-4b2b-81EF-1BA08D4A3CEC"), 0x09, 0x01, \   # BootScriptExecutorDxe
    GUID("062ACC82-1D1E-4f61-AA94-8B0C47236A3D"), 0x09, 0x02, \   # BootScriptSaveOnS3SaveStateThunk
    GUID("408edcec-cf6d-477c-a5a8-b4844e3de281"), 0x1C, 0x02, \   # ConSplitter
    GUID("53BCC14F-C24F-434C-B294-8ED2D4CC1860"), 0x11, 0x05, \   # DataHubDxe
    GUID("CA515306-00CE-4032-874E-11B755FF6866"), 0x11, 0x06, \   # DataHubStdErrDxe
    GUID("9B680FCE-AD6B-4F3A-B60B-F59899003443"), 0x23, 0x01, \   # DevicePathDxe
    GUID("6B38F7B4-AD98-40e9-9093-ACA2B5A253C4"), 0x1F, 0x01, \   # DiskIoDxe
    GUID("82ECEE48-9571-4427-8485-85A5A45A0F39"), 0x30, 0x06, \   # DxeSmmReadyToLockOnExitPmAuthThunk
    GUID("13AC6DD0-73D0-11D4-B06B-00AA00BD6DE7"), 0x12, 0x01, \   # EbcDxe
    GUID("00160F8D-2B35-4df2-BBE0-B272A8D631F0"), 0x10, 0x06, \   # FirmwarePerformanceDxe
    GUID("044310AB-77FD-402a-AF1A-87D4120E7329"), 0x10, 0x07, \   # FirmwarePerformanceSmm
    GUID("AC3435BB-B1D3-4EF8-957C-8048606FF671"), 0x1B, 0x09, \   # FrameworkHiiOnUefiHiiThunk
    GUID("6CDDBF28-89AC-4e01-9692-616B8A1009C8"), 0x05, 0x09, \   # FvFileLoaderOnLoadFileThunk
    GUID("5007A40E-A5E0-44f7-86AE-662F9A91DA26"), 0x05, 0x08, \   # FvOnFv2Thunk
    GUID("CCCB0C28-4B24-11d5-9A5A-0090273FC14D"), 0x1C, 0x03, \   # GraphicsConsole
    GUID("348C4D62-BFBD-4882-9ECE-C80BB1C4783B"), 0x1B, 0x01, \   # HiiDatabaseDxe
    GUID("240612B5-A063-11d4-9A3A-0090273FC14D"), 0x13, 0x01, \   # IsaBusDxe
    GUID("5167FD5D-AAA2-4FE1-9D0D-5CFCAB36C14C"), 0x20, 0x07, \   # LegacyRegion2OnLegacyRegionThunk
    GUID("C8339973-A563-4561-B858-D8476F9DEFC4"), 0x2F, 0x01, \   # Metronome
    GUID("AD608272-D07F-4964-801E-7BD3B7888652"), 0x0D, 0x01, \   # MonotonicCounterRuntimeDxe
    GUID("51739E2A-A022-4D73-ADB9-91F0C9BC7142"), 0x31, 0x01, \   # MpServicesOnFrameworkMpServicesThunk
    GUID("9F8B12CF-E796-408f-9D59-3ACDDC0AFBE3"), 0x11, 0x07, \   # PiSmbiosRecordOnDataHubSmbiosRecordThunk
    GUID("14FF38A8-ACBA-4228-A7D7-A73260C7559B"), 0x01, 0x05, \   # PiSmmStatusCodeOnFrameworkSmmStatusCodeThunk
    GUID("2FA2A6DA-11D5-4dc3-999A-749648B03C56"), 0x15, 0x02, \   # PiSmmIpl
    GUID("79E4A61C-ED73-4312-94FE-E3E7563362A9"), 0x1C, 0x0A, \   # PrintDxe
    GUID("3792FF94-8614-45ed-902B-1207BF1490A8"), 0x1C, 0x0B, \   # PrintThunk
    GUID("950216A2-A621-479c-A13D-2990617BDFE7"), 0x03, 0x05, \   # ReadOnlyVariable2OnReadOnlyVariableThunk
    GUID("B601F8C4-43B7-4784-95B1-F4226CB40CEE"), 0x0C, 0x01, \   # RuntimeDxe
    GUID("BDCE85BB-FBAA-4f4e-9264-501A2C249581"), 0x09, 0x05, \   # S3SaveStateDxe
    GUID("801ADCA0-815E-46a4-84F7-657F53621A57"), 0x05, 0x01, \   # SectionExtractionDxe
    GUID("F80697E9-7FD6-4665-8646-88E33EF71DFC"), 0x0F, 0x01, \   # SecurityStubDxe
    GUID("98BBCDA4-18B4-46d3-BD1F-6A3A52D44CF8"), 0x30, 0x04, \   # SmmAccess2OnSmmAccessThunk
    GUID("8C87E0A0-B390-4be3-819C-7C6C83CAE4EB"), 0x30, 0x03, \   # SmmBaseHelper
    GUID("21CCF0B7-246B-412c-A334-0B65A07B28DF"), 0x30, 0x02, \   # SmmBaseOnSmmBase2Thunk
    GUID("B55A4515-5895-4ea8-845B-75B7480F6502"), 0x30, 0x05, \   # SmmControl2OnSmmControlThunk
    GUID("EE1BB93E-54C5-4b17-9496-A20085950561"), 0x14, 0x13, \   # SmmUsbDispatch2OnSmmUsbDispatchThunk
    GUID("33FB3535-F15E-4c17-B303-5EB94595ECB6"), 0x30, 0x01, \   # SmmLockBox
    GUID("2D59F041-53A4-40d0-A6CD-844DC0DFEF17"), 0x09, 0x06, \   # SmmS3SaveState
    GUID("9E863906-A40F-4875-977F-5B93FF237FC6"), 0x1C, 0x04, \   # TerminalDxe
    GUID("BF89F10D-B205-474f-96E3-7A7BB1B4A407"), 0x1C, 0x05, \   # VgaClassDxe
    GUID("15C5E761-58D8-461a-9173-CAB020916264"), 0x1C, 0x06, \   # VgaMiniPortDxe
    GUID("F099D67F-71AE-4c36-B2A3-DCEB0EB2B7D8"), 0x0E, 0x01, \   # WatchdogTimer
    GUID("00000000-0000-0000-0000-000000000000"), 0x00, 0x00}     # EndEntry

  #
  # LegacyBios Configuration
  #
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdLegacyBiosCacheLegacyRegion|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdEbdaReservedMemorySize|0x18000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdLowPmmMemorySize|0x30000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdHighPmmMemorySize|0x400000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdOpromReservedMemoryBase|0x60000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdOpromReservedMemorySize|0x28000

  #
  # Language supported, if want change it, please override the RFC_LANGUAGES definition in Project.dsc
  #
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|$(RFC_LANGUAGES)

  gInsydeTokenSpaceGuid.PcdCapsuleImageFolder|L"EFI\\Insyde"

  #
  # BusMaster Override PCDs
  # Entry format: VendorID, DeviceID, 0x00(disable BusMaster) 0x01(enable BusMaster)
  #
  gInsydeTokenSpaceGuid.PcdNoExternalDmaOverrideTable|{ \ 
    UINT16(0xffff), UINT16(0xffff), 0xff}     # EndEntry

[PcdsDynamicDefault]
   gInsydeTokenSpaceGuid.PcdSetupDataInvalid|FALSE
   gInsydeTokenSpaceGuid.PcdPreserveMemoryTable|{ \
     UINT32(0x09), UINT32(0x40),   \ # Preserve 256K(0x40 pages) for ASL
     UINT32(0x0a), UINT32(0x100),  \ # Preserve 1M(0x100 pages) for S3, SMM, etc
     UINT32(0x00), UINT32(0x400),  \ # Preserve 4M(0x400 pages) for BIOS reserved memory
     UINT32(0x06), UINT32(0x300),  \ # Preserve 3M(0x300 pages) for UEFI OS runtime data to make S4 memory consistency
     UINT32(0x05), UINT32(0x200),  \ # Preserve 2M(0x200 pages) for UEFI OS runtime drivers to make S4 memory consistency
     UINT32(0x03), UINT32(0x1000), \ # Preserve 16M(0x1000 pages) for boot service drivers to reduce memory fragmental
     UINT32(0x04), UINT32(0x4000), \ # Preserve 64M(0x4000 pages) for boot service data to reduce memory fragmental
     UINT32(0x01), UINT32(0x200),  \ # Preserve 2M(0x200 pages) for UEFI OS boot loader to keep on same address
     UINT32(0x02), UINT32(0x00),   \
     UINT32(0x0e), UINT32(0x00)}     #EndEntry

[PcdsDynamicExDefault]
  ##                                                 Type  Number_String Offset
  ##                                                 ====  ============= ==========================================
  gSmbiosTokenSpaceGuid.PcdSmbiosStringFieldOffset|{ 0x00, 0x03,         0x04, 0x05, 0x08,                         \
                                                     0x01, 0x06,         0x04, 0x05, 0x06, 0x07, 0x19, 0x1A,       \
                                                     0x02, 0x06,         0x04, 0x05, 0x06, 0x07, 0x08, 0x0A,       \
                                                     0x03, 0x05,         0x04, 0x06, 0x07, 0x08, 0x15,             \
                                                     0x04, 0x06,         0x04, 0x07, 0x10, 0x20, 0x21, 0x22,       \
                                                     0x06, 0x01,         0x04,                                     \
                                                     0x07, 0x01,         0x04,                                     \
                                                     0x08, 0x02,         0x04, 0x06,                               \
                                                     0x09, 0x01,         0x04,                                     \
                                                     0x0E, 0x01,         0x04,                                     \
                                                     0x11, 0x06,         0x10, 0x11, 0x17, 0x18, 0x19, 0x1A,       \
                                                     0x16, 0x07,         0x04, 0x05, 0x06, 0x07, 0x08, 0x0E, 0x14, \
                                                     0x1A, 0x01,         0x04,                                     \
                                                     0x1B, 0x01,         0x0E,                                     \
                                                     0x1C, 0x01,         0x04,                                     \
                                                     0x1D, 0x01,         0x04,                                     \
                                                     0x1E, 0x01,         0x04,                                     \
                                                     0x22, 0x01,         0x04,                                     \
                                                     0x23, 0x01,         0x04,                                     \
                                                     0x27, 0x07,         0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, \
                                                     0x29, 0x01,         0x04 }

  gInsydeTokenSpaceGuid.PcdH2OSdhcEmmcSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OSdhcSdCardSupported|TRUE
  gSetupConfigNameGuid.PcdBootTypeName|L"BootType"
  gSetupConfigOffsetSizeGuid.PcdBootTypeOffsetSize|0x00790001  # Offset = 0x79  Size = 1
  #
  # AHCI recovery for PCIe SSD
  #
  gInsydeTokenSpaceGuid.PcdAhciPcieMemBaseAddress|{UINT32(0xC0100000), UINT32(0x00)}
  gInsydeTokenSpaceGuid.PcdAhciPcieBridgeAddress|{UINT32(0x00), UINT32(0x00)}
  
  #
  # PXE procedure cancel hot key definition
  # Key struct definition {KeyCode, ScanCode, [Next hot key], 0x00, 0x00[END]}
  #
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyDefinition|{ \
    0x00, 0x17, \ # ESC
    0x00, 0x00  \ # List end
    }
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyString|"[ESC]"

[Components.IA32]
#
# Helper tool modules
#
  InsydeModulePkg/Tools/GenBvdt/GenBvdt.inf
  InsydeModulePkg/Tools/GenVariableFvs/GenVariableFvs.inf
  InsydeModulePkg/Tools/GenFactoryCopy/GenFactoryCopy.inf
  InsydeModulePkg/Tools/CertToPublicKey/CertToPublicKey.inf
!if gInsydeTokenSpaceGuid.PcdMultiConfigSupported  
  InsydeModulePkg/Tools/GenMultiConfigRegion/GenMultiConfigRegion.inf
!endif  
  InsydeModulePkg/Tools/GenUnsignedFv/GenUnsignedFv.inf

!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported
  InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoPei/$(H2O_DDT_DEBUG_IO)DebugIoPei.inf
  InsydeModulePkg/H2ODebug/DebugEnginePei/DebugEnginePei.inf
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt
  InsydeModulePkg/Universal/StatusCode/DdtStatusCodePei/DdtStatusCodePei.inf
!endif
!endif
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseUsb
  InsydeModulePkg/Universal/StatusCode/UsbStatusCodePei/UsbStatusCodePei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseCmos
  InsydeModulePkg/Universal/StatusCode/CmosStatusCodePei/CmosStatusCodePei.inf
!endif
  InsydeModulePkg/Universal/StatusCode/DebugMaskPei/DebugMaskPei.inf
  InsydeModulePkg/Universal/Variable/VariablePei/VariablePei.inf
!if gInsydeTokenSpaceGuid.PcdSecureFlashSupported
  InsydeModulePkg/Universal/Security/SecureFlash/SecureFlashPei/SecureFlashPei.inf
!endif
  InsydeModulePkg/Universal/Misc/FirmwareRevisionSyncPei/FirmwareRevisionSyncPei.inf
  InsydeModulePkg/Universal/Variable/SetupDataProviderPei/SetupDataProviderPei.inf
  InsydeModulePkg/Universal/CommonPolicy/PlatformStage1Pei/PlatformStage1Pei.inf
  InsydeModulePkg/Universal/CommonPolicy/PlatformStage2Pei/PlatformStage2Pei.inf
  InsydeModulePkg/Bus/Smbus/ProgClkGenPeim/ProgClkGenPeim.inf
  InsydeModulePkg/Bus/Isa/Ps2KeyboardPei/Ps2KeyboardPei.inf
  InsydeModulePkg/Universal/Acpi/S3RestoreAcpiPei/S3RestoreAcpiPei.inf
!if gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported
!if gInsydeTokenSpaceGuid.PcdUseFastCrisisRecovery
  InsydeModulePkg/Universal/Recovery/EmuSecPei/EmuSecPei.inf
  InsydeModulePkg/Universal/Recovery/EmuPeiHelperPei/EmuPeiHelperPei.inf
!else
  InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/CrisisRecoveryPei.inf  {
    <LibraryClasses>
      SeamlessRecoveryLib|InsydeModulePkg/Library/SeamlessRecoveryLib/SeamlessRecoveryLib.inf
  }
!endif
  InsydeModulePkg/Bus/Pci/IdeBusPei/IdeBusPei.inf
  InsydeModulePkg/Bus/Pci/AhciBusPei/AhciBusPei.inf

!if gInsydeTokenSpaceGuid.PcdH2OSdhcSupported
  InsydeModulePkg/Bus/Pci/SdhcPei/SdHostDriverPei.inf
  InsydeModulePkg/Bus/Sdhc/SdMmcPei/SdMmcDevicePei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OUfsSupported
  InsydeModulePkg/Bus/Pci/UfsPei/UfsPei.inf 
!endif
  InsydeModulePkg/Universal/Recovery/FatPei/FatPei.inf
  InsydeModulePkg/Universal/Recovery/CdExpressPei/CdExpressPei.inf
  InsydeModulePkg/Universal/Security/SecureFlash/FirmwareAuthenticationPei/FirmwareAuthenticationPei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OUsbPeiSupported
  InsydeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
  InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
  InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OTpm2Supported == 1
  InsydeModulePkg/Universal/Security/Tcg/TrEEConfigPei/TrEEConfigPei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported
  SecurityPkg/Tcg/PhysicalPresencePei/PhysicalPresencePei.inf
  InsydeModulePkg/Universal/Security/Tcg/TcgPei/TcgPei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
  InsydeModulePkg/Universal/Security/Tcg/TisPei/TisPei.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEEPei/TrEEPei.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }
!endif
  InsydeModulePkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf
  InsydeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }


  InsydeModulePkg/Universal/FileAccess/FileAccessPei/FileAccessPei.inf


[Components.X64]
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported
  InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoDxe/$(H2O_DDT_DEBUG_IO)DebugIoDxe.inf
  InsydeModulePkg/H2ODebug/DebugEngineDxe/DebugEngineDxe.inf
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt
  InsydeModulePkg/Universal/StatusCode/DdtStatusCodeRuntimeDxe/DdtStatusCodeRuntimeDxe.inf
!endif
!endif
  InsydeModulePkg/Universal/StatusCode/DebugDriverDxe/DebugDriverDxe.inf
  InsydeModulePkg/Universal/PcdSmmDxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseUsb
  InsydeModulePkg/Universal/StatusCode/UsbStatusCodeRuntimeDxe/UsbStatusCodeRuntimeDxe.inf
!endif
!if gInsydeTokenSpaceGuid.PcdStatusCodeUseCmos
  InsydeModulePkg/Universal/StatusCode/CmosStatusCodeRuntimeDxe/CmosStatusCodeRuntimeDxe.inf
!endif
  InsydeModulePkg/Universal/StatusCode/ReportStatusCodeRouterSmm/ReportStatusCodeRouterSmm.inf
  InsydeModulePkg/Bus/Isa/PcRtcRuntimeDxe/PcRtcRuntimeDxe.inf
!if gInsydeTokenSpaceGuid.PcdUseFastCrisisRecovery
  InsydeModulePkg/Universal/Recovery/EmuPeiGateDxe/EmuPeiGateDxe.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OTpm2Supported == 1
  InsydeModulePkg/Universal/Security/Tcg/MemoryOverwriteControl/TcgMor.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEEConfigDxe/TrEEConfigDxe.inf  
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported
  InsydeModulePkg/Universal/Security/Tcg/TcgDxe/TcgDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/PhysicalPresenceDxe/PhysicalPresenceDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/TcgSmm/TcgSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
  InsydeModulePkg/Universal/Security/Tcg/TisDxe/TisDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEEDxe/TrEEDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  InsydeModulePkg/Universal/Security/Tcg/TrEEPhysicalPresenceDxe/TrEEPhysicalPresenceDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/TrEESmm/TrEESmm.inf
!endif
  InsydeModulePkg/Universal/Console/JpegDecoderDxe/JpegDecoderDxe.inf
  InsydeModulePkg/Universal/Console/TgaDecoderDxe/TgaDecoderDxe.inf
  InsydeModulePkg/Universal/Console/PcxDecoderDxe/PcxDecoderDxe.inf
  InsydeModulePkg/Universal/Console/GifDecoderDxe/GifDecoderDxe.inf
  InsydeModulePkg/Universal/Console/BmpDecoderDxe/BmpDecoderDxe.inf
  InsydeModulePkg/Universal/Console/PngDecoderDxe/PngDecoderDxe.inf  
  InsydeModulePkg/Universal/FirmwareVolume/FtwLiteRuntimeDxe/FtwLiteRuntimeDxe.inf
  InsydeModulePkg/Universal/Variable/VariableRuntimeDxe/VariableRuntimeDxe.inf
!if gInsydeTokenSpaceGuid.PcdUseFastCrisisRecovery
  InsydeModulePkg/Universal/Variable/EmuVariableRuntimeDxe/EmuVariableRuntimeDxe.inf
!endif
  InsydeModulePkg/Universal/Variable/VariableDefaultUpdateDxe/VariableDefaultUpdateDxe.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
      NULL|InsydeModulePkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
      NULL|SecurityPkg/Library/DxeDeferImageLoadLib/DxeDeferImageLoadLib.inf
!if gInsydeTokenSpaceGuid.PcdH2OTpmSupported
      NULL|InsydeModulePkg/Library/DxeTpmMeasureBootLib/DxeTpmMeasureBootLib.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
      NULL|InsydeModulePkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
!endif
  }

  InsydeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiSupportDxe/AcpiSupportDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OAcpiBgrtSupported
  InsydeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf
!endif
  InsydeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }

  InsydeModulePkg/Universal/Acpi/WpbtDxe/WpbtDxe.inf
  InsydeModulePkg/Universal/Acpi/Msdm/InstallMsdm.inf
  InsydeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2keyboardDxe.inf
  InsydeModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OConsoleRedirectionSupported != 1
  InsydeModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
  InsydeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
!endif
  InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
  InsydeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf
  InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf
  InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf
  InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
  InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
!endif
!endif
!if gInsydeTokenSpaceGuid.PcdH2OSdhcSupported
  InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
  InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported  
  InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
!endif
!endif
!if gInsydeTokenSpaceGuid.PcdH2OUfsSupported
  InsydeModulePkg/Bus/Pci/UfsDxe/UfsDxe.inf 
!endif
!if gInsydeTokenSpaceGuid.PcdH2OUfsSupported == 1 || gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported == 1
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  InsydeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
!endif
  InsydeModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
  InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  }
  InsydeModulePkg/Universal/FirmwareVolume/FlashDeviceFvbRuntimeDxe/FlashDeviceFvbRuntimeDxe.inf
  InsydeModulePkg/Universal/CommonPolicy/PciHotPlugDxe/PciHotPlugDxe.inf
  InsydeModulePkg/Universal/Acpi/AcpiCallBacksSmm/AcpiCallBacksSmm.inf
  InsydeModulePkg/Universal/UserInterface/SetupMouseDxe/SetupMouseDxe.inf
  InsydeModulePkg/Universal/Smbios/PnpRuntimeDxe/PnpRuntimeDxe.inf
  InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf
  InsydeModulePkg/Universal/CommonPolicy/CommonSmiCallBackSmm/CommonSmiCallBackSmm.inf
  InsydeModulePkg/Universal/Smbios/SmbiosDxe/SmbiosDxe.inf

  InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf
  InsydeModulePkg/Universal/Security/SysPasswordDxe/SysPasswordDxe.inf
  InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf
  InsydeModulePkg/Universal/Security/CryptoServiceRuntimeDxe/CryptoServiceRuntimeDxe.inf
  InsydeModulePkg/Universal/Security/RngDxe/RngDxe.inf

!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported
  InsydeModulePkg/Universal/UserInterface/H2OKeyDescDxe/H2OKeyDescDxe.inf
  InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf
  InsydeSetupPkg/Drivers/H2OFormBrowserDxe/H2OFormBrowserDxe.inf

!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalTextDESupported
  InsydeSetupPkg/Drivers/H2ODisplayEngineLocalTextDxe/H2ODisplayEngineLocalTextDxe.inf
!endif

!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalMetroDESupported
  InsydeModulePkg/Universal/UserInterface/MicrowindowsDxe/MicrowindowsDxe.inf {
    <LibraryClasses>
       TimerLib|InsydeModulePkg/Library/CpuTimerLib/CpuTimerLib.inf
  }
  InsydeSetupPkg/Drivers/H2ODisplayEngineLocalMetroDxe/H2ODisplayEngineLocalMetroDxe.inf
!endif

!else
!if gInsydeTokenSpaceGuid.PcdGraphicsSetupSupported
  InsydeModulePkg/Universal/UserInterface/MicrowindowsDxe/MicrowindowsDxe.inf {
    <LibraryClasses>
       TimerLib|InsydeModulePkg/Library/CpuTimerLib/CpuTimerLib.inf
  }
  InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/GraphicsSetupBrowserDxe.inf
!else
  InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/SetupBrowserDxe.inf
!endif
!endif

  InsydeModulePkg/Universal/UserInterface/Fonts/TrueTypeDecoderDxe/TrueTypeDecoderDxe.inf {
    <LibraryClasses>
       NULL|MdeModulePkg/Override/Library/CompilerStubLib/CompilerStubLib.inf
       StdLib|InsydeModulePkg/Library/StdLib/StdLib.inf
  }
  InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf
  InsydeModulePkg/Universal/Console/MonitorKeyDxe/MonitorKeyDxe.inf
  InsydeModulePkg/Universal/UserInterface/UnicodeCollationDxe/UnicodeCollationDxe.inf
  InsydeModulePkg/Universal/CommonPolicy/BootOptionPolicyDxe/BootOptionPolicyDxe.inf
  InsydeModulePkg/Universal/IsaAcpiDxe/IsaAcpi.inf
  InsydeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  InsydeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  InsydeModulePkg/Universal/Console/VgaDriverPolicyDxe/VgaDriverPolicyDxe.inf
  InsydeModulePkg/Universal/Console/EdidOverrideDxe/EdidOverrideDxe.inf
  InsydeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  InsydeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf
  InsydeModulePkg/Universal/CommonPolicy/CommonPciPlatformDxe/CommonPciPlatformDxe.inf
!if gInsydeTokenSpaceGuid.PcdSnapScreenSupported
  InsydeModulePkg/Universal/Console/SnapScreenDxe/SnapScreenDxe.inf
!endif

!if gInsydeTokenSpaceGuid.PcdH2ONetworkSupported
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
!if gInsydeTokenSpaceGuid.PcdH2ONetworkIpv6Supported
  InsydeModulePkg/Universal/Network/Ip6Dxe/Ip6Dxe.inf
  InsydeModulePkg/Universal/Network/Udp6Dxe/Udp6Dxe.inf
  InsydeModulePkg/Universal/Network/Dhcp6Dxe/Dhcp6Dxe.inf
  InsydeModulePkg/Universal/Network/Mtftp6Dxe/Mtftp6Dxe.inf
!endif
  InsydeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf

!if gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported
  InsydeModulePkg/Universal/Network/TcpDxe/TcpDxe.inf
  InsydeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
!endif

!endif
  InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
  InsydeModulePkg/Csm/BiosThunk/KeyboardDxe/KeyboardDxe.inf
  InsydeModulePkg/Csm/BiosThunk/BlockIoDxe/BlockIoDxe.inf
  InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm/Int15ServiceSmm.inf
  InsydeModulePkg/Csm/BiosThunk/VideoDxe/VideoDxe.inf
!endif
  InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe/LegacyBiosPlatformDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
!if gInsydeTokenSpaceGuid.PcdSeamlessAhciSupported == 1 || gInsydeTokenSpaceGuid.PcdSeamlessIdeSupported == 1
  InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
!endif
!endif
  InsydeModulePkg/Universal/Security/BiosProtectDxe/BiosProtectDxe.inf
!if gInsydeTokenSpaceGuid.PcdSecureFlashSupported
  InsydeModulePkg/Universal/Misc/TianoCompressSmm/TianoCompressSmm.inf
  InsydeModulePkg/Universal/Security/SecureFlash/SecurityErrorHandlingDxe/SecurityErrorHandlingDxe.inf
  InsydeModulePkg/Universal/Security/SecureFlash/SecureFlashDxe/SecureFlashDxe.inf
!endif
  InsydeModulePkg/Universal/Irsi/IrsiRegistrationRuntimeDxe/IrsiRegistrationRuntimeDxe.inf
  InsydeModulePkg/Universal/Irsi/IrsiRuntimeDxe/IrsiRuntimeDxe.inf {
    <LibraryClasses>
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
!if gInsydeTokenSpaceGuid.PcdH2OCapsuleUpdateSupported
  InsydeModulePkg/Universal/CapsuleUpdate/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  InsydeModulePkg/Universal/CapsuleUpdate/CapsuleLoaderTriggerDxe/CapsuleLoaderTriggerDxe.inf
!else
!if gInsydeTokenSpaceGuid.PcdUefiCapsuleUpdateSupported
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
!endif
!endif
  InsydeModulePkg/Universal/FirmwareVolume/VariableRegionInfoDxe/VariableRegionInfoDxe.inf
  InsydeModulePkg/Universal/Disk/DelayUefiRaidOprom/DelayUefiRaidOprom.inf

!if gInsydeTokenSpaceGuid.PcdDynamicHotKeySupported
  InsydeModulePkg/Universal/StringTransformDxe/StringTransformDxe.inf
!endif
  InsydeModulePkg/Universal/UserInterface/SetupUtilityApp/SetupUtilityApp.inf

!if gInsydeTokenSpaceGuid.PcdH2OI2cSupported
  InsydeModulePkg/Bus/I2c/I2cDxe/I2cDxe.inf
  InsydeModulePkg/Bus/I2c/I2cKbDxe/I2cKbDxe.inf
  InsydeModulePkg/Bus/I2c/I2cMouseDxe/I2cMouseDxe.inf
  InsydeModulePkg/Bus/I2c/I2cTouchPanelDxe/I2cTouchPanelDxe.inf
!endif
