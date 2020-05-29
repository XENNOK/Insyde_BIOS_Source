## @file
#  Package dscription file for InsydeFlashDevicePkg
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

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = InsydeFlashDevice
  PLATFORM_GUID                  = A77ED161-409C-4702-A5EB-9EB0CBD6947C
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/InsydeFlashDevice
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT



################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ImageRelocationLib|InsydeModulePkg/Library/ImageRelocationLib/ImageRelocationLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  SpiAccessInitLib|MahoBayChipsetPkg/Library/FlashDeviceSupport/DxeSpiAccessInitLib/DxeSpiAccessInitLib.inf
  SmbusLib|MahoBayChipsetPkg/Library/SmbusLib/SmbusLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf

[LibraryClasses.common.PEIM]
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  SpiAccessInitLib|InsydeModulePkg/Library/FlashDeviceSupport/PeiSpiAccessInitLib/PeiSpiAccessInitLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf

[LibraryClasses.common.DXE_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsFixedAtBuild.IPF]

[PcdsPatchableInModule]

[PcdsDynamicExDefault]
  gH2OFlashDeviceMfrNameGuid.PcdSst25lf040SpiMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst25lf040SpiPartName|"25LF040"
  gH2OFlashDeviceGuid.PcdSst25lf040Spi|{ \ # SST  25LF040
    0x03, 0x00, 0x00, 0x00, 0xbf, 0x40, 0x00, 0x00,  \  # DeviceType = 03  Id = 000040BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdSst25lf040SpiConfig|{ \ # SST  25LF040
    0x28, 0x00, 0x00, 0x00, 0x90, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 90  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7e, 0xf2, 0x06, 0x50,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27E  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 50
    0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,  \  # GlobalProtectCode = 0C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00080000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst25lf040aSpiMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst25lf040aSpiPartName|"25LF040A"
  gH2OFlashDeviceGuid.PcdSst25lf040aSpi|{ \ # SST  25LF040A
    0x03, 0x00, 0x00, 0x00, 0xbf, 0x44, 0x00, 0x00,  \  # DeviceType = 03  Id = 000044BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdSst25lf040aSpiConfig|{ \ # SST  25LF040A
    0x28, 0x00, 0x00, 0x00, 0x90, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 90  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7e, 0xf2, 0x06, 0x50,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27E  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 50
    0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,  \  # GlobalProtectCode = 0C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00080000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst25lf080aSpiMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst25lf080aSpiPartName|"25LF080A"
  gH2OFlashDeviceGuid.PcdSst25lf080aSpi|{ \ # SST  25LF080A
    0x03, 0x00, 0x00, 0x00, 0xbf, 0x80, 0x00, 0x00,  \  # DeviceType = 03  Id = 000080BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdSst25lf080aSpiConfig|{ \ # SST  25LF080A
    0x28, 0x00, 0x00, 0x00, 0x90, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 90  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7e, 0xf2, 0x06, 0x50,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27E  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 50
    0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00100000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst25vf080bSpiMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst25vf080bSpiPartName|"25VF080B"
  gH2OFlashDeviceGuid.PcdSst25vf080bSpi|{ \ # SST  25VF080B
    0x03, 0x00, 0x00, 0x00, 0xbf, 0x25, 0x8e, 0x00,  \  # DeviceType = 03  Id = 008E25BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdSst25vf080bSpiConfig|{ \ # SST  25VF080B
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x50,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 50
    0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00100000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst25vf016bSpiMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst25vf016bSpiPartName|"25VF016B"
  gH2OFlashDeviceGuid.PcdSst25vf016bSpi|{ \ # SST  25VF016B
    0x03, 0x00, 0x00, 0x00, 0xbf, 0x41, 0x00, 0x00,  \  # DeviceType = 03  Id = 000041BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0200
  }

  gH2OFlashDeviceConfigGuid.PcdSst25vf016bSpiConfig|{ \ # SST  25VF016B
    0x28, 0x00, 0x00, 0x00, 0x90, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 90  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7e, 0xf2, 0x06, 0x50,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27E  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 50
    0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00200000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdAtmel25df041SpiMfrName|"ATMEL"
  gH2OFlashDevicePartNameGuid.PcdAtmel25df041SpiPartName|"25DF041"
  gH2OFlashDeviceGuid.PcdAtmel25df041Spi|{ \ # ATMEL  25DF041
    0x03, 0x00, 0x00, 0x00, 0x1f, 0x44, 0x00, 0x00,  \  # DeviceType = 03  Id = 0000441F
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdAtmel25df041SpiConfig|{ \ # ATMEL  25DF041
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x00,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 00
    0x03, 0x00, 0x00, 0x00, 0x7c, 0xf2, 0x00, 0x00,  \  # ReadOp = 03  ReadStatusOp = 00  OpType = F27C  WriteEnablePrefix = 00  WriteStatusEnablePrefix = 00
    0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00080000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdAtmel25df081aSpiMfrName|"ATMEL"
  gH2OFlashDevicePartNameGuid.PcdAtmel25df081aSpiPartName|"25DF081A"
  gH2OFlashDeviceGuid.PcdAtmel25df081aSpi|{ \ # ATMEL  25DF081A
    0x03, 0x00, 0x00, 0x00, 0x1f, 0x45, 0x00, 0x00,  \  # DeviceType = 03  Id = 0000451F
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdAtmel25df081aSpiConfig|{ \ # ATMEL  25DF081A
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00100000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdAtmel26df161SpiMfrName|"ATMEL"
  gH2OFlashDevicePartNameGuid.PcdAtmel26df161SpiPartName|"26DF161"
  gH2OFlashDeviceGuid.PcdAtmel26df161Spi|{ \ # ATMEL  26DF161
    0x03, 0x00, 0x00, 0x00, 0x1f, 0x46, 0x00, 0x00,  \  # DeviceType = 03  Id = 0000461F
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0200
  }

  gH2OFlashDeviceConfigGuid.PcdAtmel26df161SpiConfig|{ \ # ATMEL  26DF161
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00200000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdAtmel26df321SpiMfrName|"ATMEL"
  gH2OFlashDevicePartNameGuid.PcdAtmel26df321SpiPartName|"26DF321"
  gH2OFlashDeviceGuid.PcdAtmel26df321Spi|{ \ # ATMEL  26DF321
    0x03, 0x00, 0x00, 0x00, 0x1f, 0x47, 0x00, 0x00,  \  # DeviceType = 03  Id = 0000471F
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x04   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0400
  }

  gH2OFlashDeviceConfigGuid.PcdAtmel26df321SpiConfig|{ \ # ATMEL  26DF321
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00400000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdAtmel25df641SpiMfrName|"ATMEL"
  gH2OFlashDevicePartNameGuid.PcdAtmel25df641SpiPartName|"25DF641"
  gH2OFlashDeviceGuid.PcdAtmel25df641Spi|{ \ # ATMEL  25DF641
    0x03, 0x00, 0x00, 0x00, 0x1f, 0x48, 0x00, 0x00,  \  # DeviceType = 03  Id = 0000481F
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdAtmel25df641SpiConfig|{ \ # ATMEL  25DF641
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdWinbondW25q64bvSpiMfrName|"WINBOND"
  gH2OFlashDevicePartNameGuid.PcdWinbondW25q64bvSpiPartName|"W25Q64BV"
  gH2OFlashDeviceGuid.PcdWinbondW25q64bvSpi|{ \ # WINBOND  W25Q64BV
    0x03, 0x00, 0x00, 0x00, 0xef, 0x40, 0x17, 0x00,  \  # DeviceType = 03  Id = 001740EF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdWinbondW25q64bvSpiConfig|{ \ # WINBOND  W25Q64BV
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdWinbondW25q128bvSpiMfrName|"WINBOND"
  gH2OFlashDevicePartNameGuid.PcdWinbondW25q128bvSpiPartName|"W25Q128BV"
  gH2OFlashDeviceGuid.PcdWinbondW25q128bvSpi|{ \ # WINBOND  W25Q128BV
    0x03, 0x00, 0x00, 0x00, 0xef, 0x40, 0x18, 0x00,  \  # DeviceType = 03  Id = 001840EF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10   \  # ExtId = 00000000  BlockSize = 0010  Multiple = 1000
  }

  gH2OFlashDeviceConfigGuid.PcdWinbondW25q128bvSpiConfig|{ \ # WINBOND  W25Q128BV
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BLockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 01000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25l8005SpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25l8005SpiPartName|"25L8005"
  gH2OFlashDeviceGuid.PcdMxic25l8005Spi|{ \ # MXIC  25L8005
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x14, 0x00,  \  # DeviceType = 03  Id = 001420C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l8005SpiConfig|{ \ # MXIC  25L8005
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00100000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25l1605aSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25l1605aSpiPartName|"25L1605A"
  gH2OFlashDeviceGuid.PcdMxic25l1605aSpi|{ \ # MXIC  25L1605A
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x15, 0x00,  \  # DeviceType = 03  Id = 001520C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0200
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l1605aSpiConfig|{ \ # MXIC  25L1605A
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00200000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25l6405dSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25l6405dSpiPartName|"25L6405D"
  gH2OFlashDeviceGuid.PcdMxic25l6405dSpi|{ \ # MXIC  25L6405D  
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x17, 0x00,  \  # DeviceType = 03  Id = 001720C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l6405dSpiConfig|{ \ # Mxic  25L6405D
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00  Reserved = 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25l12805dSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25l12805dSpiPartName|"25L12805D"
  gH2OFlashDeviceGuid.PcdMxic25l12805dSpi|{ \ # MXIC  25L12805D  
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x18, 0x00,  \  # DeviceType = 03  Id = 001820C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 1000
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l12805dSpiConfig|{ \ # Mxic  25L12805D
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00  Reserved = 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 01000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25l25635eSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25l25635eSpiPartName|"25L25635E"
  gH2OFlashDeviceGuid.PcdMxic25l25635eSpi|{ \ # MXIC  25L25635E  
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x19, 0x00,  \  # DeviceType = 03  Id = 001920C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x20   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 2000
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l25635eSpiConfig|{ \ # Mxic  25L25635E
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00  Reserved = 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 02000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25u6435fSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25u6435fSpiPartName|"25U6435F"
  gH2OFlashDeviceGuid.PcdMxic25u6435fSpi|{ \ # MXIC  25U6435F
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x25, 0x37, 0x00,  \  # DeviceType = 03  Id = 003725C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25u6435fSpiConfig|{ \ # MXIC  25U6435F
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00  Reserved = 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdStM25pe80SpiMfrName|"ST"
  gH2OFlashDevicePartNameGuid.PcdStM25pe80SpiPartName|"M25PE80"
  gH2OFlashDeviceGuid.PcdStM25pe80Spi|{ \ # ST  M25PE80
    0x03, 0x00, 0x00, 0x00, 0x20, 0x80, 0x14, 0x00,  \  # DeviceType = 03  Id = 00148020
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdStM25pe80SpiConfig|{ \ # ST  M25PE80
    0x28, 0x00, 0x00, 0x00, 0x9f, 0xdb, 0x02, 0x00,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = DB  WriteOp = 02  WriteStatusOp = 00
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x00,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 00
    0x00, 0x01, 0x01, 0x01, 0x03, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 01  BlockProtectCodeRequired = 01  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 01  BlockUnprotectCode = 00  DeviceSize = 00100000
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00   \  # BlockEraseSize = 00000100  BlockProtectSize = 00010000
  }

  gH2OFlashDeviceMfrNameGuid.PcdPmc25lv040SpiMfrName|"PMC"
  gH2OFlashDevicePartNameGuid.PcdPmc25lv040SpiPartName|"25LV040"
  gH2OFlashDeviceGuid.PcdPmc25lv040Spi|{ \ # PMC  25LV040
    0x03, 0x00, 0x00, 0x00, 0x9d, 0x7e, 0x00, 0x00,  \  # DeviceType = 03  Id = 00007E9D
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdPmc25lv040SpiConfig|{ \ # PMC  25LV040
    0x28, 0x00, 0x00, 0x00, 0xab, 0xd7, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = AB  EraseOp = D7  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 02  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00080000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdGd25lq16SpiMfrName|"GD"
  gH2OFlashDevicePartNameGuid.PcdGd25lq16SpiPartName|"25LQ16"
  gH2OFlashDeviceGuid.PcdGd25lq16Spi|{ \ # GD  25LQ16
    0x03, 0x00, 0x00, 0x00, 0xC8, 0x60, 0x15, 0x00,  \  # DeviceType = 03  Id = 0x001560C8
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0200
  }

  gH2OFlashDeviceConfigGuid.PcdGd25lq16SpiConfig|{ \ # GD  25LQ16
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BLockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = C3  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00200000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdGd25lq64cSpiMfrName|"GD"
  gH2OFlashDevicePartNameGuid.PcdGd25lq64cSpiPartName|"25LQ64C"
  gH2OFlashDeviceGuid.PcdGd25lq64cSpi|{ \ # GD  25LQ64C
    0x03, 0x00, 0x00, 0x00, 0xC8, 0x60, 0x17, 0x00,  \  # DeviceType = 03  Id = 001760C8
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  Multiple = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdGd25lq64cSpiConfig|{ \ # GD  25LQ64C
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BLockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 1C  GlobalUnprotectCode = C3  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdCommonvidCommondidSpiMfrName|"COMMONVID"
  gH2OFlashDevicePartNameGuid.PcdCommonvidCommondidSpiPartName|"COMMONDID"
  gH2OFlashDeviceGuid.PcdCommonvidCommondidSpi|{ \ # COMMONVID  COMMONDID
    0x03, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12,  \  # DeviceType = 03  Id = 12345678
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xff, 0xff   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = FFFF
  }

  gH2OFlashDeviceConfigGuid.PcdCommonvidCommondidSpiConfig|{ \ # COMMONVID  COMMONDID
    0x28, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 00  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,  \  # GlobalProtectAvailable = 01  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 00  BytesOfId = 00  MinBytePerProgRead = 01  NoVolStatusAvailable = 00
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = FFFFFFFF
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst49lf008aLpcMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst49lf008aLpcPartName|"49LF008A"
  gH2OFlashDeviceGuid.PcdSst49lf008aLpc|{ \ # SST  49LF008A
    0x00, 0x00, 0x00, 0x00, 0xbf, 0x5a, 0x00, 0x00,  \  # DeviceType = 00  Id = 00005ABF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdSst49lf008aLpcConfig|{ \ # SST  49LF008A
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0xff,  \  # Size = 00000010  IdAddress = FFBC0000
    0x55, 0x55, 0xf0, 0xff, 0xaa, 0x2a, 0xf0, 0xff   \  # CmdPort55 = FFF05555  CmdPort2A = FFF02AAA
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst39vf080LpcMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst39vf080LpcPartName|"39VF080"
  gH2OFlashDeviceGuid.PcdSst39vf080Lpc|{ \ # SST  39VF080
    0x00, 0x00, 0x00, 0x00, 0xbf, 0xd8, 0x00, 0x00,  \  # DeviceType = 00  Id = 0000D8BF
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0100
  }

  gH2OFlashDeviceConfigGuid.PcdSst39vf080LpcConfig|{ \ # SST  39VF080
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  \  # Size = 00000010  IdAddress = FFFF0000
    0x55, 0x55, 0xf0, 0xff, 0xaa, 0x2a, 0xf0, 0xff   \  # CmdPort55 = FFF05555  CmdPort2A = FFF02AAA
  }

  gH2OFlashDeviceMfrNameGuid.PcdIntel82802acLpcMfrName|"INTEL"
  gH2OFlashDevicePartNameGuid.PcdIntel82802acLpcPartName|"82802AC"
  gH2OFlashDeviceGuid.PcdIntel82802acLpc|{ \ # INTEL  82802AC
    0x01, 0x00, 0x00, 0x00, 0x89, 0xac, 0x00, 0x00,  \  # DeviceType = 01  Id = 0000AC89
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00   \  # ExtId = 00000000  BlockSize = 0100  BlockCount = 0010
  }

  gH2OFlashDeviceConfigGuid.PcdIntel82802acLpcConfig|{ \ # INTEL  82802AC
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  \  # Size = 00000010  IdAddress = FFFF0000
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # CmdPort55 = 00000000  CmdPort2A = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdSst49lf004aLpcMfrName|"SST"
  gH2OFlashDevicePartNameGuid.PcdSst49lf004aLpcPartName|"49LF004A"
  gH2OFlashDeviceGuid.PcdSst49lf004aLpc|{ \ # SST  49LF004A
    0x00, 0x00, 0x00, 0x00, 0xbf, 0x60, 0x00, 0x00,  \  # DeviceType = 00  Id = 000060BF
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00   \  # ExtId = 00000000  BlockSize = 0100  BlockCount = 0008
  }

  gH2OFlashDeviceConfigGuid.PcdSst49lf004aLpcConfig|{ \ # SST  49LF004A
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0xff,  \  # Size = 00000010  IdAddress = FFBC0000
    0x55, 0x55, 0xf0, 0xff, 0xaa, 0x2a, 0xf0, 0xff   \  # CmdPort55 = FFF05555  CmdPort2A = FFF02AAA
  }

  gH2OFlashDeviceMfrNameGuid.PcdPmc49fl004tLpcMfrName|"PMC"
  gH2OFlashDevicePartNameGuid.PcdPmc49fl004tLpcPartName|"49FL004T"
  gH2OFlashDeviceGuid.PcdPmc49fl004tLpc|{ \ # PMC  49FL004T
    0x00, 0x00, 0x00, 0x00, 0x9d, 0x6e, 0x00, 0x00,  \  # DeviceType = 00  Id = 00006E9D
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdPmc49fl004tLpcConfig|{ \ # PMC  49FL004T
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0xff,  \  # Size = 00000010  IdAddress = FFBC0000
    0x55, 0x55, 0xf0, 0xff, 0xaa, 0x2a, 0xf0, 0xff   \  # CmdPort55 = FFF05555  CmdPort2A = FFF02AAA
  }

  gH2OFlashDeviceMfrNameGuid.PcdWinbond39v040fpaLpcMfrName|"WINBOND"
  gH2OFlashDevicePartNameGuid.PcdWinbond39v040fpaLpcPartName|"39V040FPA"
  gH2OFlashDeviceGuid.PcdWinbond39v040fpaLpc|{ \ # WINBOND  39V040FPA
    0x00, 0x00, 0x00, 0x00, 0xda, 0x34, 0x00, 0x00,  \  # DeviceType = 00  Id = 000034DA
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00   \  # ExtId = 00000000  BlockSize = 0100  BlockCount = 0008
  }

  gH2OFlashDeviceConfigGuid.PcdWinbond39v040fpaLpcConfig|{ \ # WINBOND  39V040FPA
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0xff,  \  # Size = 00000010  IdAddress = FFBC0000
    0x55, 0x55, 0xf0, 0xff, 0xaa, 0x2a, 0xf0, 0xff   \  # CmdPort55 = FFF05555  CmdPort2A = FFF02AAA
  }

  gH2OFlashDeviceMfrNameGuid.PcdSt50flw040aLpcMfrName|"ST"
  gH2OFlashDevicePartNameGuid.PcdSt50flw040aLpcPartName|"50FLW040A"
  gH2OFlashDeviceGuid.PcdSt50flw040aLpc|{ \ # ST  50FLW040A
    0x01, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00,  \  # DeviceType = 01  Id = 00000820
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 0080
  }

  gH2OFlashDeviceConfigGuid.PcdSt50flw040aLpcConfig|{ \ # ST  50FLW040A
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  \  # Size = 00000010  IdAddress = FFFF0000
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # CmdPort55 = 00000000  CmdPort2A = 00000000
  }



###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components.IA32]

[Components.X64]

[Components.EBC]

[Components.IPF]

