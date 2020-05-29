## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]
  EDK_GLOBAL  INSYDE_APEI_PKG          = InsydeApeiPkg

[LibraryClasses]
  WheaLib|$(INSYDE_APEI_PKG)/Library/WheaLib/WheaLib.inf

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
  ## APEI Feature Switch
!if $(APEI_SUPPORT) == YES
  gEfiApeiPkgTokenSpaceGuid.PcdApeiSupport|TRUE
!else
  gEfiApeiPkgTokenSpaceGuid.PcdApeiSupport|FALSE
!endif

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]

[Components.X64]
!if   gEfiApeiPkgTokenSpaceGuid.PcdApeiSupport
  #
  # WHEA/APEI
  #
  $(INSYDE_APEI_PKG)/Whea/IsPlatformSupportWhea/IsPlatformSupportWhea.inf
  $(INSYDE_APEI_PKG)/Whea/WheaSupport/WheaSupportDxe.inf
  $(INSYDE_APEI_PKG)/Whea/WheaPlatform/WheaPlatformSmm.inf
  $(INSYDE_APEI_PKG)/Whea/WheaErrorInject/WheaErrorInjectSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  $(INSYDE_APEI_PKG)/Whea/WheaErrorLog/WheaErrorLogSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
!endif

