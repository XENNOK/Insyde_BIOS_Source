## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]
  EDK_GLOBAL  INSYDE_EVENTLOG_PKG          = InsydeEventLogPkg

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
#[-start-140307-IB08400249-add]#
  # FOR DXE
  gH2OEventLogPkgTokenSpaceGuid.PcdStoreDebugMsgEn|FALSE
  # FOR PEI
  gH2OEventLogPkgTokenSpaceGuid.PcdStoreDebugMsgEnPei|FALSE
#[-end-140307-IB08400249-add]#
#[-start-140325-IB08400258-add]#
  # For logging Event to BIOS GPNV Region.
  gH2OEventLogPkgTokenSpaceGuid.PcdBiosEventStorageSupport|TRUE
  # For logging Event to BMC SEL.
  gH2OEventLogPkgTokenSpaceGuid.PcdBmcSelEventStorageSupport|TRUE
#[-end-140325-IB08400258-add]#

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.IA32]

#[-start-140325-IB08400258-modify]#
!if gH2OEventLogPkgTokenSpaceGuid.PcdBiosEventStorageSupport
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BiosStorage/BiosStoragePei/BiosStoragePei.inf
!endif
#[-end-140325-IB08400258-modify]#

!if gH2OEventLogPkgTokenSpaceGuid.PcdH2OEventLogSupport
  $(INSYDE_EVENTLOG_PKG)/EventLog/EventLogPei/EventLogPei.inf
!endif  

!if gH2OEventLogPkgTokenSpaceGuid.PcdH2OPostMessageSupport
  $(INSYDE_EVENTLOG_PKG)/PostMessage/PostMessagePei/PostMessagePei.inf
!endif

#[-start-140325-IB08400258-modify]#
!if gH2OEventLogPkgTokenSpaceGuid.PcdBmcSelEventStorageSupport
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BmcSelStorage/BmcSelStoragePei/BmcSelStoragePei.inf
!endif
#[-end-140325-IB08400258-modify]#

#[-start-140306-IB08400249-add]#
  $(INSYDE_EVENTLOG_PKG)/DebugMessage/StoreDebugMessagePei/StoreDebugMessagePei.inf
#[-end-140306-IB08400249-add]#

[Components.X64]

#[-start-140325-IB08400258-modify]#
!if gH2OEventLogPkgTokenSpaceGuid.PcdBiosEventStorageSupport
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BiosStorage/BiosStorageSmm/BiosStorageChipsetSmm.inf 
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BiosStorage/BiosStorageSmm/BiosStorageKernelSmm.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  }
  
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BiosStorage/BiosStorageDxe/BiosStorageChipsetDxe.inf 
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BiosStorage/BiosStorageDxe/BiosStorageKernelDxe.inf {
    <LibraryClasses>
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  }
!endif
#[-end-140325-IB08400258-modify]#

!if gH2OEventLogPkgTokenSpaceGuid.PcdH2OEventLogSupport
  $(INSYDE_EVENTLOG_PKG)/EventLog/EventLogDxe/EventLogDxe.inf
  $(INSYDE_EVENTLOG_PKG)/EventLog/EventLogSmm/EventLogSmm.inf
  
  $(INSYDE_EVENTLOG_PKG)/EventStorage/MemoryStorage/MemoryStorageDxe/MemoryStorageKernelDxe.inf 
  $(INSYDE_EVENTLOG_PKG)/EventStorage/MemoryStorage/MemoryStorageDxe/MemoryStorageChipsetDxe.inf 

  $(INSYDE_EVENTLOG_PKG)/EventStorage/MemoryStorage/MemoryStorageSmm/MemoryStorageKernelSmm.inf 
  $(INSYDE_EVENTLOG_PKG)/EventStorage/MemoryStorage/MemoryStorageSmm/MemoryStorageChipsetSmm.inf 
  
#[-start-140325-IB08400258-modify]#
!if gH2OEventLogPkgTokenSpaceGuid.PcdBmcSelEventStorageSupport  
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BmcSelStorage/BmcSelEventStorageDxe/BmcSelEventStorageDxe.inf
  $(INSYDE_EVENTLOG_PKG)/EventStorage/BmcSelStorage/BmcSelEventStorageSmm/BmcSelEventStorageSmm.inf 
  $(INSYDE_EVENTLOG_PKG)/BmcSelStringTranslationDxe/BmcSelStringTranslationDxe.inf
!endif  
#[-end-140325-IB08400258-modify]#

!if $(GCC)!=Gcc    #BugBug: gcc fails to build this module
  $(INSYDE_EVENTLOG_PKG)/EventLogViewerDxe/EventLogViewerDxe.inf
!endif
  
!endif  

!if gH2OEventLogPkgTokenSpaceGuid.PcdH2OPostMessageSupport
  $(INSYDE_EVENTLOG_PKG)/PostMessage/PostMessageDxe/PostMessageDxe.inf
  $(INSYDE_EVENTLOG_PKG)/PostMessage/PostMessageSmm/PostMessageSmm.inf
!endif

#[-start-140306-IB08400249-add]#
  #
  # Store the DEBUG() in memory space, user can dump it by tool(Ex:H2OELV (shell and DOS tool).).
  #
  $(INSYDE_EVENTLOG_PKG)/DebugMessage/StoreDebugMessageDxe/StoreDebugMessageDxe.inf
#[-start-140311-IB08400251-add]#
  $(INSYDE_EVENTLOG_PKG)/DebugMessage/StoreDebugMessageSmm/StoreDebugMessageSmm.inf
#[-end-140311-IB08400251-add]#
#[-end-140306-IB08400249-add]#
