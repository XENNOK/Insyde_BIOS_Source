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

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]

[LibraryClasses.common.PEIM]

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gSioGuid.PcdSioNct6683dluSetup|TRUE
  #
  # If you initialize UpdateAsl variable to FALSE, remembering checked 
  # mailbox struct in SioAsl\$(SioName).asl.
  #    
  gSioGuid.PcdSioNct6683dluUpdateAsl|TRUE  
  
[PcdsFixedAtBuild] 
  #
  # Device Number: Com:0x01, Floppy:0x02, LPT:0x3, KYBD:0x04, MOUSE:0x05, IR/CIR:0x07
  #                HardWare Monitor:0x10, ACPI:0x11  
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
    0xC5, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x90, 0x00, 0x00, 0x00, 0x00,    \ # HWM 
    0xC7, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    \ # ACPI    
    0xC7, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x2E, 0x02, 0x00, 0x00, 0x00,    \ # CFG
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     \ # End Entry
  }

  gSioGuid.PcdSioNct6683dluSetupStr|L"SioNct6683dluSetup00"

[Components.IA32]
  SioNct6683dluPkg/SioNct6683dluPei/SioNct6683dluPei.inf

[Components.X64]
  SioNct6683dluPkg/SioNct6683dluDxe/SioNct6683dluDxe.inf

