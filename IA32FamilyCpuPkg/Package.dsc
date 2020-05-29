## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]

[LibraryClasses]
  CpuConfigLib|IA32FamilyCpuPkg/Library/CpuConfigLib/CpuConfigLib.inf
  CpuOnlyResetLib|IA32FamilyCpuPkg/Library/CpuOnlyResetLibNull/CpuOnlyResetLibNull.inf

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
  gEfiCpuTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE

[PcdsFixedAtBuild]
  gEfiCpuTokenSpaceGuid.PcdCpuIEDRamSize|0x400000
  gEfiCpuTokenSpaceGuid.PcdTemporaryRamBase|0xFEF00000
  gEfiCpuTokenSpaceGuid.PcdTemporaryRamSize|0x00010000
  gEfiCpuTokenSpaceGuid.PcdCpuSmmStackSize|0x8000

[PcdsDynamicDefault]

[Components.IA32]
  IA32FamilyCpuPkg/SecCore/SecCore.inf
  IA32FamilyCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf
!if gEfiCpuTokenSpaceGuid.PcdH2OCBTSupported
  IA32FamilyCpuPkg/Btsinit/BtsInitPei/BtsInitPei.inf
!endif

[Components.X64]
  IA32FamilyCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  IA32FamilyCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf
!if gEfiCpuTokenSpaceGuid.PcdH2OCBTSupported
  IA32FamilyCpuPkg/Btsinit/BtsInitDxe/BtsInitDxe.inf
  IA32FamilyCpuPkg/Btsinit/BtsinitSmm/BtsinitSmm.inf
!endif
