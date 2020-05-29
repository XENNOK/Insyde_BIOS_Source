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
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.SEC]
  TimerLib|PerformancePkg/Library/TscTimerLib/PeiTscTimerLib.inf

[LibraryClasses.common.PEI_CORE]
  TimerLib|PerformancePkg/Library/TscTimerLib/PeiTscTimerLib.inf

[LibraryClasses.common.PEIM]
  TimerLib|PerformancePkg/Library/TscTimerLib/PeiTscTimerLib.inf

[LibraryClasses.common.DXE_CORE]
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.DXE_DRIVER]
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]
  TimerLib|PerformancePkg/Library/TscTimerLib/DxeTscTimerLib.inf

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]

[Components.X64]
