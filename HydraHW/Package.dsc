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

[LibraryClasses]
  HydraLib|_HomeWorkPkg/Library/HydraLib/HydraLib.inf
  
[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.X64]
  ##_HomeWorkPkg/_Dialog_HW/Dialog_HW.inf
  ##_HomeWorkPkg/_SMM_HW/SMM_HW.inf
  ##_HomeWorkPkg/_SMMAP_HW/SMMAP_HW.inf
  
[Components]
  ##_HomeWorkPkg/Test_Lib_Use/Test_Lib_Use.inf
  _HomeWorkPkg/_PCI_Utility_HW/PCI_Utility_HW.inf
  _HomeWorkPkg/_BDA_CMOS_HW/BDA_CMOS_HW.inf
  _HomeWorkPkg/_Memory_HW/Memory_HW.inf
  _HomeWorkPkg/_ImageHandle_HW/ImageHandle_HW.inf
  _HomeWorkPkg/_KBC_HW/KBC_HW.inf
  _HomeWorkPkg/_SIO_HW/SIO_HW.inf
  _HomeWorkPkg/_CPU_HW/CPU_HW.inf
  _HomeWorkPkg/_FileSystem_HW/FS_HW.inf
  _HomeWorkPkg/_Variable_HW/Variable_HW.inf
  _HomeWorkPkg/_NonDriver_HW/NonDriver_HW.inf
  _HomeWorkPkg/_NonDriver_HW/AppNonDriver_HW.inf
  ##_HomeWorkPkg/_DriverModelDriver_HW/DriverModelDriver_HW.inf
  ##_HomeWorkPkg/_TimerEvent_HW/TimerEvent_HW.inf
  ##_HomeWorkPkg/_SMBUS_HW/SMBUS_HW.inf
  ##_HomeWorkPkg\_IRQ_HW\IRQ_HW.inf
  _HomeWorkPkg\_ACPI_HW\ACPI_HW.inf
  _HomeWorkPkg\_NvmeData\NvmeData.inf
  





