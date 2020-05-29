## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]

[Libraries]
  EdkCompatibilityPkg/Compatibility/Library/UefiLanguageLib/UefiLanguageLib.inf
  EdkCompatibilityPkg/Foundation/Core/Dxe/ArchProtocol/ArchProtocolLib.inf
  EdkCompatibilityPkg/Foundation/Cpu/Pentium/CpuIA32Lib/CpuIA32Lib.inf
  EdkCompatibilityPkg/Foundation/Efi/Guid/EfiGuidLib.inf
  EdkCompatibilityPkg/Foundation/Efi/Protocol/EfiProtocolLib.inf
  EdkCompatibilityPkg/Foundation/Framework/Guid/EdkFrameworkGuidLib.inf
  EdkCompatibilityPkg/Foundation/Framework/Ppi/EdkFrameworkPpiLib.inf
  EdkCompatibilityPkg/Foundation/Framework/Protocol/EdkFrameworkProtocolLib.inf
  EdkCompatibilityPkg/Foundation/Guid/EdkGuidLib.inf
  EdkCompatibilityPkg/Foundation/Library/CompilerStub/CompilerStubLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiDriverLib/EfiDriverLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiIfrSupportLib/EfiIfrSupportLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiScriptLib/EfiScriptLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/EfiUiLib/EfiUiLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/Graphics/Graphics.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/GraphicsLite/Graphics.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/Hob/HobLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/Print/PrintLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/PrintLite/PrintLib.inf
  EdkCompatibilityPkg/Foundation/Library/Dxe/UefiEfiIfrSupportLib/UefiEfiIfrSupportLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseLib/BaseLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseMemoryLib/BaseMemoryLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePciExpressLib/BasePciExpressLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePciLibCf8/BasePciLibCf8.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePeCoffLib/BasePeCoffLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePostCodeLibDebug/BasePostCodeLibDebug.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BasePrintLib/BasePrintLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseTimerLibLocalApic/BaseTimerLibLocalApic.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeHobLib/DxeHobLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxePerformanceLib/DxePerformanceLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeSmbusLib/DxeSmbusLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/EdkDxeRuntimeDriverLib/EdkDxeRuntimeDriverLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/EdkDxeSalLib/EdkDxeSalLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/HiiLib/HiiLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiDxePostCodeLibReportStatusCode/PeiDxePostCodeLibReportStatusCode.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiHobLib/PeiHobLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiServicesLib/PeiServicesLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiServicesTablePointerLibMm7/PeiServicesTablePointerLibMm7.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/PeiSmbusLib/PeiSmbusLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/SmmRuntimeDxeReportStatusCodeLib/SmmRuntimeDxeReportStatusCodeLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/UefiDriverModelLib/UefiDriverModelLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/UefiLib/UefiLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  EdkCompatibilityPkg/Foundation/Library/EfiCommonLib/EfiCommonLib.inf
  EdkCompatibilityPkg/Foundation/Library/Pei/Hob/PeiHobLib.inf
  EdkCompatibilityPkg/Foundation/Library/Pei/PeiLib/PeiLib.inf
  EdkCompatibilityPkg/Foundation/Library/RuntimeDxe/EfiRuntimeLib/EfiRuntimeLib.inf
  EdkCompatibilityPkg/Foundation/Library/Thunk16/Thunk16Lib.inf
  EdkCompatibilityPkg/Foundation/Ppi/EdkPpiLib.inf
  EdkCompatibilityPkg/Foundation/Protocol/EdkProtocolLib.inf
  EdkCompatibilityPkg/Foundation/Library/EdkIIGlueLib/Library/DxeRuntimePciExpressLib/DxeRuntimePciExpressLib.inf

[Libraries.IA32]

[Libraries.X64]

[LibraryClasses]
  LanguageLib|EdkCompatibilityPkg/Compatibility/Library/UefiLanguageLib/UefiLanguageLib.inf

[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]

[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_DRIVER]

[LibraryClasses.common.DXE_SMM_DRIVER]

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]
  EdkCompatibilityPkg/Compatibility/PciCfgOnPciCfg2Thunk/PciCfgOnPciCfg2Thunk.inf
  EdkCompatibilityPkg/Compatibility/ReadOnlyVariableOnReadOnlyVariable2Thunk/ReadOnlyVariableOnReadOnlyVariable2Thunk.inf
  EdkCompatibilityPkg/Compatibility/AcpiVariableHobOnSmramReserveHobThunk/AcpiVariableHobOnSmramReserveHobThunk.inf

[Components.X64]
  EdkCompatibilityPkg/Compatibility/SmmBaseOnSmmBase2Thunk/SmmBaseOnSmmBase2Thunk.inf
  EdkCompatibilityPkg/Compatibility/SmmBaseHelper/SmmBaseHelper.inf
  EdkCompatibilityPkg/Compatibility/SmmAccess2OnSmmAccessThunk/SmmAccess2OnSmmAccessThunk.inf
  EdkCompatibilityPkg/Compatibility/SmmControl2OnSmmControlThunk/SmmControl2OnSmmControlThunk.inf
  EdkCompatibilityPkg/Compatibility/DxeSmmReadyToLockOnExitPmAuthThunk/DxeSmmReadyToLockOnExitPmAuthThunk.inf
  EdkCompatibilityPkg/Compatibility/BootScriptSaveOnS3SaveStateThunk/BootScriptSaveOnS3SaveStateThunk.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
  EdkCompatibilityPkg/Compatibility/FrameworkHiiOnUefiHiiThunk/FrameworkHiiOnUefiHiiThunk.inf
  EdkCompatibilityPkg/Compatibility/PrintThunk/PrintThunk.inf
  EdkCompatibilityPkg/Compatibility/FvOnFv2Thunk/FvOnFv2Thunk.inf
  EdkCompatibilityPkg/Compatibility/LegacyRegion2OnLegacyRegionThunk/LegacyRegion2OnLegacyRegionThunk.inf {
    <SOURCE_OVERRIDE_PATH>
      EdkCompatibilityPkg/Override/Compatibility/LegacyRegion2OnLegacyRegionThunk
  }
  EdkCompatibilityPkg/Compatibility/MpServicesOnFrameworkMpServicesThunk/MpServicesOnFrameworkMpServicesThunk.inf
