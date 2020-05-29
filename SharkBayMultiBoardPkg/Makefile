## @file
#  Project Makefile
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

#
# Build mode customization
# -q: Quiet mode   - disable all messages except fatal errors, module based messages.
#                    this is default build mode if BUILD_MODE is not specified
# -s: Silent mode  - file based messages
# -v: Verbose mode - Turn on verbose output with informational messages printed
#
#BUILD_MODE     = -s

#
# Build report customization
# Below is the default build report setting if BUILD_REPORT is not specified
#BUILD_REPORT = -Y PCD -Y DEPEX -Y LIBRARY -y $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildReport.txt

#
# Add additional project build options here
#
#PROJECT_BUILD_OPTIONS =
RMT_BUILD      = "-D INSYDE_DEBUGGER=NO -D MRC_RMT=YES"

!include $(WORKSPACE)\BaseTools\Conf\Makefile

#[-start-130624-IB10930033-modify]#
#[-start-120607-IB03530370-add]#
#
# USB_DEBUG_SUPPORT function is not work in the current kernel code.If this function is ready, will set USB_DEBUG_SUPPORT=YES
#
## @BuildTarget "IA32 X64", DEBUG, Debug build to enable MRC RMT function for x86 64-bit CPU Arch.
uefi64rmt:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG) PROJECT_BUILD_OPTIONS=$(RMT_BUILD)
#[-end-120607-IB03530370-add]#
#[-end-130624-IB10930033-modify]#


