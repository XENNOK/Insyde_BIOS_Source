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
!include InsydeOemServicesPkg/Package.env

[LibraryClasses]
  BaseOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLibDefault.inf
  BaseOemSvcKernelLib|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLib.inf
  
[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]
  PeiOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLibDefault.inf

[LibraryClasses.common.PEIM]
  PeiOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLibDefault.inf

[LibraryClasses.common.DXE_CORE]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf

[LibraryClasses.common.UEFI_DRIVER]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf

[LibraryClasses.common.DXE_DRIVER]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf

[LibraryClasses.common.SMM_CORE]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf

[LibraryClasses.common.UEFI_APPLICATION]
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]

[Components.X64]

