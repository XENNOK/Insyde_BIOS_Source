## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]
!include InsydeFrameworkModulePkg/Package.env

[Libraries]
  InsydeFrameworkModulePkg/Guid/GuidLib.inf
  InsydeFrameworkModulePkg/Library/BvdtLib/BvdtLib.inf
  InsydeFrameworkModulePkg/Library/CmosLib/CmosLib.inf
  InsydeFrameworkModulePkg/Library/Dxe/ConsoleLib/ConsoleLib.inf  
  InsydeFrameworkModulePkg/Library/Dxe/EfiRegTableLib/EfiRegTableLib.inf
  InsydeFrameworkModulePkg/Library/Dxe/OemGraphicsLite/Graphics.inf {
    <SOURCE_OVERRIDE_PATH>
      InsydeFrameworkModulePkg/Override/Library/Dxe/OemGraphicsLite
  }
  InsydeFrameworkModulePkg/Library/Dxe/UefiSetupUtilityLib/UefiSetupUtilityLib.inf 
  InsydeFrameworkModulePkg/Library/IoAccess/IoAccess.inf
  InsydeFrameworkModulePkg/Library/RuntimeDxe/FdSupportLib/FdSupportLib.inf
  InsydeFrameworkModulePkg/Library/Smm/EfiSmmDriverLib.inf
  InsydeFrameworkModulePkg/Library/Smm/SmmIo/SmmIoLib.inf
  InsydeFrameworkModulePkg/Library/Smm/SmmKscLib/SmmKscLib.inf
  InsydeFrameworkModulePkg/Platform/Generic/Guid/GenericGuidLib.inf
  InsydeFrameworkModulePkg/Platform/Generic/MonoStatusCode/Library/Pei/SerialStatusCode/SerialStatusCode.inf
  InsydeFrameworkModulePkg/Platform/Generic/MonoStatusCode/Library/Pei/SimpleCpuIo/SimpleCpuIo.inf
  InsydeFrameworkModulePkg/Platform/Generic/Protocol/GenericProtocolLib.inf
  InsydeFrameworkModulePkg/Platform/Generic/Lib/IhisiLib/IhisiLib.inf
  InsydeFrameworkModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  InsydeFrameworkModulePkg/Ppi/PpiLib.inf
  InsydeFrameworkModulePkg/Protocol/ProtocolLib.inf
!if $(USE_FAST_CRISIS_RECOVERY) == YES
  InsydeFrameworkModulePkg/$(FAST_CRISIS_RECOVERY_DIR)/Guid/FastRecoveryEmuGuid.inf
  InsydeFrameworkModulePkg/$(FAST_CRISIS_RECOVERY_DIR)/Ppi/FastRecoveryPpiLib.inf
!endif

  InsydeFrameworkModulePkg/Compatibility/Library/PeiDebugLibReportStatusCode/PeiDebugLibReportStatusCode.inf
  InsydeFrameworkModulePkg/Compatibility//Library/DxeDebugLibReportStatusCode/DxeDebugLibReportStatusCode.inf
  InsydeFrameworkModulePkg/Compatibility//Library/SetupDefaultLib/SetupDefaultLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(PLATFORM_CRB)/$(DEMOBOARD_FAMILY)/$(DEMOBOARD_NAME)/Dxe/UefiSetupUtility
      $(PLATFORM_CRB)/$(PROJECT_FAMILY)/$(PROJECT_NAME)/Dxe/UefiSetupUtility
      $(PLATFORM_CRB)/Override/$(PLATFORM_CRB)/$(DEMOBOARD_FAMILY)/$(DEMOBOARD_NAME)/Dxe/UefiSetupUtility
      $(PROJECT_PKG)/Override/$(PLATFORM_CRB)/$(DEMOBOARD_FAMILY)/$(DEMOBOARD_NAME)/Dxe/UefiSetupUtility
      InsydeFrameworkModulePkg/Library/Dxe/UefiSetupUtilityLib
  }
  

[LibraryClasses]

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
  #
  # The purpose of the FlashLayout tool module is to generate FdLayout.h automatically
  # The FlashLayout tool module is intentionally to put here instead of project package
  # to ensure that this module is built before all the ECP modules
  #
  $(OUTPUT_DIRECTORY)/AutoGen/FlashLayout/FlashLayout.inf
  InsydeFrameworkModulePkg/Compatibility/PeiSmbus2OnPeiSmbusThunk/PeiSmbus2OnPeiSmbusThunk.inf
  InsydeFrameworkModulePkg/Compatibility/FvFileLoaderOnLoadFileThunk/FvFileLoaderOnLoadFileThunk.inf
  InsydeFrameworkModulePkg/Compatibility/PeiOemServicesThunk/PeiOemServicesThunk.inf

[Components.X64]
  InsydeFrameworkModulePkg/Compatibility/ChipsetLibServices/Dxe/ChipsetLibServicesDxe.inf
  InsydeFrameworkModulePkg/Compatibility/ChipsetLibServices/Smm/ChipsetLibServicesSmm.inf

  InsydeFrameworkModulePkg/Compatibility/SmmRuntime/SmmRuntime.inf
  InsydeFrameworkModulePkg/Compatibility/SmmUsbDispatch2OnSmmUsbDispatchThunk/SmmUsbDispatch2OnSmmUsbDispatchThunk.inf
  InsydeFrameworkModulePkg/Compatibility/PiSmbiosRecordOnDataHubRecordThunk/PiSmbiosRecordOnDataHubRecordThunk.inf
  InsydeFrameworkModulePkg/Compatibility/SmmSwDispatch2OnSmmSwDispatchThunk/SmmSwDispatch2OnSmmSwDispatchThunk.inf
  InsydeFrameworkModulePkg/Compatibility/PiSmmStatusCodeThunk/PiSmmStatusCodeThunk.inf
  InsydeFrameworkModulePkg/Compatibility/DxeOemServicesThunk/DxeOemServicesThunk.inf
  InsydeFrameworkModulePkg/Compatibility/SmmOemServicesThunk/SmmOemServicesThunk.inf

