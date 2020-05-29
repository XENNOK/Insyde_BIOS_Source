## @file
#
#  SIO Package Description file
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


################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = SioDummyPkg
  PLATFORM_GUID                  = AE583EED-A42A-42c7-BED8-B955C5ACEB4B
  PLATFORM_VERSION               = 0.2
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/SioDummyPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  #//[-start-131009-IB13150002-add]//
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf
  #//[-end-131009-IB13150002-add]//

[LibraryClasses.common.PEIM]

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
 
[PcdsFixedAtBuild] 
  #
  # Device Number: Com:0x01, Floppy:0x02, LPT:0x3, KYBD:0x04, MOUSE:0x05
  #
  # TYPEH: SIO ID High Byte 
  # TYPEL: SIO ID Low Byte
  # SI:    SIO Instance
  # D:     SIO Device
  # DI:    SIO Device Instance
  # DE:    SIO Device Enable
  # DBASE: SIO Device Base Address
  # SiIZE: SIO Device Size
  # LDN:   SIO Device LDN
  # DIRQ:  SIO Device IRQ
  # DDMA:  SIO Device DMA
  #
  gSioGuid.PcdSioDummyCfg|{ \
    #SIO TYPE | SI  | D   | DI  | DE  | DBASE     | SIZE| LDN | DIRQ| DDMA
    #-----------------------------------------------------------------------------
    0xFF, 0x00, 0x00, 0x01, 0x00, 0x01, 0x03, 0xF8, 0x10, 0x00, 0x04, 0x00,    \ # Com
    0xFF, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0xF8, 0x10, 0x00, 0x03, 0x00,    \ # Com	
    0xFF, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x60, 0x10, 0x00, 0x01, 0x00,    \ # KYBD
    0xFF, 0x00, 0x00, 0x05, 0x01, 0x01, 0x00, 0x60, 0x10, 0x00, 0x0C, 0x00,    \ # MOUSE
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     \ # End Entry
  }

[Components.IA32]
  SioDummyPkg/SioDummyPei/SioDummyPei.inf

[Components.X64]
  SioDummyPkg/SioDummyDxe/SioDummyDxe.inf 

