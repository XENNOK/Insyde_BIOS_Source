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
  PLATFORM_NAME                  = SioNct6683dluPkg
  PLATFORM_GUID                  = 5817365E-F350-473f-BF1A-D5AEE08E7BB9
  PLATFORM_VERSION               = 0.2
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/SioNct6683dluPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  SioLib|InsydeModulePkg/Library/SioLib/SioLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  AcpiPlatformLib|InsydeModulePkg/Library/CommonPlatformLib/AcpiPlatformLib/AcpiPlatformLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf 
  BvdtLib|InsydeModulePkg/Library/BvdtLib/BvdtLib.inf    
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  HiiLib|InsydeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
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
  gSioGuid.PcdSioNct6683dluSetup|FALSE
  #
  # If you initialize UpdateAsl variable to FALSE, remembering checked 
  # mailbox struct in SioAsl\$(SioName).asl.
  #    
  gSioGuid.PcdSioNct6683dluUpdateAsl|TRUE
 
[PcdsFixedAtBuild] 
  #
  # Device Number: Com:0x01, LPT:0x3, KYBD:0x04, MOUSE:0x05, IR/CIR:0x07
  #                ACPI:0x11  
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
  gSioGuid.PcdSioNct6683dluCfg|{ \
    #SIO TYPE | SI  | D   | DI  | DE  | DBASE     | SIZE| LDN | DIRQ| DDMA
    #-----------------------------------------------------------------------------
    0xC7, 0x00, 0x00, 0x01, 0x00, 0x01, 0x03, 0xF8, 0x10, 0x00, 0x04, 0x00,    \ # ComA
    0xC7, 0x00, 0x00, 0x01, 0x01, 0x00, 0x02, 0xF8, 0x10, 0x00, 0x01, 0x00,    \ # ComB
    0xC7, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x78, 0x10, 0x00, 0x07, 0x00,    \ # Lpt
    0xC7, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x60, 0x10, 0x00, 0x01, 0x00,    \ # KYBD
    0xC7, 0x00, 0x00, 0x05, 0x01, 0x01, 0x00, 0x60, 0x10, 0x00, 0x0C, 0x00,    \ # MOUSE
    0xC7, 0x00, 0x00, 0x07, 0x00, 0x00, 0x03, 0x10, 0x10, 0x00, 0x0B, 0x00,    \ # IR    
    0xC7, 0x00, 0x00, 0x07, 0x01, 0x00, 0x03, 0x20, 0x01, 0x00, 0x0B, 0x00,    \ # CIR  
    0xC7, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x90, 0x00, 0x00, 0x00, 0x00,    \ # HWM 
    0xC7, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    \ # ACPI     
    0xC7, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x2E, 0x02, 0x00, 0x00, 0x00,    \ # CFG
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     \ # End Entry
  }

  gSioGuid.PcdSioNct6683dluSetupStr|L"SioNct6683dluSetup00"

[Components.IA32]
  SioNct6683dluPkg/SioNct6683dluPei/SioNct6683dluPei.inf {
    <LibraryClasses>
      IoLib|MdePkg/Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
      PeiOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLibDefault.inf
      PeiOemSvcKernelLib|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf
  }

[Components.X64]
  SioNct6683dluPkg/SioNct6683dluDxe/SioNct6683dluDxe.inf {
    <LibraryClasses>
      DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
      DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  }

