## @file
#  Package dscription file for OemServicesPkg
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

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = _HomeWorkPkg
  PLATFORM_GUID                  = d72460eb-ccf7-4a00-adb1-de122a0cd83c
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/_HomeWorkPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  
################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

[LibraryClasses]
  HydraLib|_HomeWorkPkg/Library/HydraLib/HydraLib.inf
  
  #
  # Entry Point Libraries
  #
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf

[Components]
  _HomeWorkPkg/Library/HydraLib/HydraLib.inf


