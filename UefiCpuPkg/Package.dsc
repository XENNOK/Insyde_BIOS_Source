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

[LibraryClasses]
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf

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
  UefiCpuPkg/CpuIoPei/CpuIoPei.inf

[Components.X64]
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
