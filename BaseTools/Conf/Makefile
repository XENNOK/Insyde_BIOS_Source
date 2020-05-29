## @file
#  Common Makefile for project build
#
#******************************************************************************
#* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

#
# Build options for build targets
#
DDT_BUILD      = "-D INSYDE_DEBUGGER=YES"
EFI_DEBUG      = "-D EFI_DEBUG=YES"
DDT_DEBUG      = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
EFI_DEBUG_DDT  = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
PERF_BUILD     = "-D SYMBOLIC_DEBUG=YES -D INSYDE_DEBUGGER=NO -D EFI_DEBUG=NO -D FIRMWARE_PERFORMANCE=YES"
EFI_DEBUG_USB  = "-D EFI_DEBUG=YES -D USB_DEBUG_SUPPORT=YES"
CBT_DEBUG      = "-D SYMBOLIC_DEBUG=YES -D H2OCBT_SUPPORT=YES"

!ifndef BUILD_MODE
#
# Available build modes
# -q: Quiet mode - disable all messages except fatal errors, module based messages
# -s: Silent mode - file based messages
# -v: Turn on verbose output with informational messages printed
#
BUILD_MODE     = -q
!endif

!ifndef BUILD_REPORT
BUILD_REPORT = -Y PCD -Y DEPEX -Y LIBRARY -y $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildReport.txt
!endif


## @BuildTarget "IA32 X64", RELEASE, Release build for x86 64-bit CPU Arch.
uefi64:
  $(MAKE) release BUILD_TARGET=$@

## @BuildTarget "IA32", RELEASE, Release build for x86 32-bit CPU Arch.
uefi32:
  $(MAKE) release BUILD_TARGET=$@

## @BuildTarget "IA32 X64", DEBUG, Debug build for Insyde Debugger
uefi64ddt:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_BUILD)

## @BuildTarget "IA32", DEBUG, Debug build for Insyde Debugger
uefi32ddt:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_BUILD)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port
efidebug:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via USB port
efidebugusb:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_USB)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
efidebugddt:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_DDT)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_DEBUG)

## @BuildTarget "Common", DEBUG, Performance-log enabled build for x86 64-bit CPU Arch.
uefi64perf:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(PERF_BUILD)

## @BuildTarget "Common", DEBUG, Performance-log enabled build for x86 32-bit CPU Arch.
uefi32perf:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(PERF_BUILD)

## @BuildTarget "Common", DEBUG, CBT debug.
uefi64cbt:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(CBT_DEBUG)

## @BuildTarget "Common", RELEASE, Release package build for InsydeModulePkg package
kernel: cleancache
  set PROJECT_PKG=InsydeModulePkg
  nmake genfont
  nmake genlayout
  set PROJECT_PKG=
  @build -p InsydeModulePkg\InsydeModulePkg.dsc -N -s -b RELEASE

## @BuildTarget "IA32", DEBUG, Build NT32 emulation package
## Before buliding DSC file, temporarily set PROJECT_PKG as NtEmulator32 for output folder of genfont and genlayout
nt32: cleancache gen_debug_target cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) /nologo $@ %i
!ELSE
  nmake /nologo genfont genlayout
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
  build -N -p Build/$(PROJECT_PKG)/Project.dsc -s -a IA32 -b DEBUG $(PROJECT_BUILD_OPTIONS)
!ENDIF

## @BuildTarget "X64", DEBUG, Build NT64 emulation package
## Before buliding DSC file, temporarily set PROJECT_PKG as NtEmulator64 for output folder of genfont and genlayout
nt64: cleancache gen_debug_target cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) /nologo $@ %i
!ELSE
  nmake /nologo genfont genlayout
  call <<build64.bat
    set LIB=%VC_PATH%\VC\Lib\amd64;%TOOL_DRV%\%SDK_DIR%\Lib\x64;%TOOL_DRV%\%SDK_DIR%\Lib\win8\um\x86
    if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
    build -N -p Build/$(PROJECT_PKG)/Project.dsc -s -a X64 -b DEBUG $(PROJECT_BUILD_OPTIONS)
<<
  del build64.bat
!ENDIF

## @BuildTarget "IA32", DEBUG, Run NT32 emulation
run: cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  cd %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32 & %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\SecMain.exe
!ENDIF


## @BuildTarget "X64", DEBUG, Run NT64 emulation
run64: cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  cd %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64 & %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\SecMain.exe
!ENDIF

cleancache:
  @if exist $(WORKSPACE)\Conf\.cache rmdir $(WORKSPACE)\Conf\.cache /s /q

clean: cleancache
  @if exist $(WORKSPACE)\Build rmdir $(WORKSPACE)\Build /s /q

release: cleancache gen_release_target genfont genlayout
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
  build -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat $(BUILD_TARGET)
!ENDIF

debug: cleancache gen_debug_target genfont genlayout
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
  build -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat $(BUILD_TARGET)
!ENDIF

genfont:
!IF "$(PROJECT_PKG)" != ""
  @create_temporary_font_hpk.bat
!ENDIF

genlayout:
!IF "$(PROJECT_PKG)" != ""
  @create_temporary_layout_hpk.bat
!ENDIF

gen_release_target:
  @echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)\Conf\target.txt
  @echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)\Conf\target.txt
  @echo TARGET                       = RELEASE                          >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)\Conf\target.txt
  @echo MAX_CONCURRENT_THREAD_NUMBER = $(NUMBER_OF_PROCESSORS)          >> $(WORKSPACE)\Conf\target.txt
  @echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)\Conf\target.txt

gen_debug_target:
  @echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)\Conf\target.txt
  @echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)\Conf\target.txt
  @echo TARGET                       = DEBUG                            >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)\Conf\target.txt
  @echo MAX_CONCURRENT_THREAD_NUMBER = $(NUMBER_OF_PROCESSORS)          >> $(WORKSPACE)\Conf\target.txt
  @echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)\Conf\target.txt

