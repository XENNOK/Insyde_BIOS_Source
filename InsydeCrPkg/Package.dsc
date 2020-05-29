## @file
# H2O Console Redirection package project build description file.
#
#******************************************************************************
#* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
#

[Defines]

[LibraryClasses]
!if gInsydeTokenSpaceGuid.PcdH2OConsoleRedirectionSupported == 1
 CrVfrConfigLib|InsydeCrPkg/Library/CrVfrConfigLib/CrVfrConfigLib.inf
 CrBdsLib|InsydeCrPkg/Library/CrBdsLib/CrBdsLib.inf
 CrConfigDefaultLib|InsydeCrPkg/Library/CrConfigDefaultLib/CrConfigDefaultLib.inf
!endif 
[LibraryClasses.common.PEIM]

[LibraryClasses.common.DXE_DRIVER]

[LibraryClasses.common.DXE_SMM_DRIVER]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]
  gInsydeCrTokenSpaceGuid.PcdCROverUsbSupported|FALSE
  gInsydeCrTokenSpaceGuid.PcdCRSrvManagerSupported|FALSE
  gInsydeCrTokenSpaceGuid.PcdCRPciSerialSupported|TRUE
  
[PcdsFixedAtBuild]

[PcdsFixedAtBuild.IPF]

[PcdsPatchableInModule]

[Components.IA32] 

[Components.X64]
!if gInsydeTokenSpaceGuid.PcdH2OConsoleRedirectionSupported == 1 
  InsydeCrPkg/CrHookDxe/CrHookDxe.inf
  InsydeCrPkg/CrPolicyDxe/CrPolicyDxe.inf
  InsydeCrPkg/IsaSerialDxe/IsaSerialDxe.inf
  InsydeCrPkg/TerminalDxe/TerminalDxe.inf
!if gInsydeCrTokenSpaceGuid.PcdCRPciSerialSupported == 1 
  InsydeCrPkg/PciSerialDxe/PciSerialDxe.inf
!endif
!if gInsydeCrTokenSpaceGuid.PcdCRSrvManagerSupported == 1
  InsydeCrPkg/CrSrvManagerDxe/CrSrvManagerDxe.inf
!if gInsydeTokenSpaceGuid.PcdSecureFlashSupported == 0    
  InsydeCrPkg/CrBiosFlashDxe/CrBiosFlashDxe.inf
!endif  
  InsydeCrPkg/CrFileTransferDxe/CrFileTransferDxe.inf
  InsydeCrPkg/FileSelectUIDxe/FileSelectUIDxe.inf
!endif
!if gInsydeCrTokenSpaceGuid.PcdCROverUsbSupported == 1   
  InsydeCrPkg/UsbSerialDxe/UsbSerialDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported != 1
  InsydeCrPkg/CrDdtCableDxe/CrDdtCableDxe.inf
!endif
  InsydeCrPkg/CrPl2303Dxe/CrPl2303Dxe.inf
!endif
!endif


