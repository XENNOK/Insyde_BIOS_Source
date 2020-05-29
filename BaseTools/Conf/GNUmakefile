## @file
#  Common Makefile for project build
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

export EDK_TOOLS_PATH = $(WORKSPACE)/BaseTools
export PATH = $(EDK_TOOLS_PATH)/Bin/Linux:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PROJECT_PKG = $(shell basename $$PWD)

#
# Set-up the execution authority of all the build tools.
#
$(shell chmod -R a+x $(EDK_TOOLS_PATH)/Bin/Linux)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/CertToPublicKey/CertToPublicKey)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenBvdt/GenBvdt)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenFactoryCopy/GenFactoryCopy)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenVariableFvs/GenVariableFvs)

#
# Set-up library path for tool modules
#
ifeq ($(LIB32_PATH),)
ifeq ("$(shell uname -m)", "x86_64")
export LIB32_PATH=/usr/lib32
else
export LIB32_PATH=/usr/lib/i386-linux-gnu
endif
endif

#
# Build options for build targets
#
DDT_BUILD      = "-D INSYDE_DEBUGGER=YES"
EFI_DEBUG      = "-D EFI_DEBUG=YES"
DDT_DEBUG      = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
EFI_DEBUG_DDT  = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
PERF_BUILD     = "-D SYMBOLIC_DEBUG=YES -D INSYDE_DEBUGGER=NO -D EFI_DEBUG=NO -D FIRMWARE_PERFORMANCE=YES"
EFI_DEBUG_USB  = "-D EFI_DEBUG=YES -D USB_DEBUG_SUPPORT=YES"

ifeq ($(BUILD_MODE),)
#
# Available build modes
# -q: Quiet mode - disable all messages except fatal errors, module based messages
# -s: Silent mode - file based messages
# -v: Turn on verbose output with informational messages printed
#
BUILD_MODE     = -s
endif

ifeq ($(ARCH),)
ARCH = IA32 X64
endif

ifeq ($(TOOL_CHAIN),)
TOOL_CHAIN = GCC48
endif

ifeq ($(BUILD_REPORT),)
BUILD_REPORT = -Y PCD -Y DEPEX -Y LIBRARY -y $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildReport.txt
endif

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
uefiperf:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(PERF_BUILD)

## @BuildTarget "Common", RELEASE, Release package build for InsydeModulePkg package
kernel: cleancache gen_release_target
	@build -D GCC=Gcc -p InsydeModulePkg/InsydeModulePkg.dsc -N -s -b RELEASE

kerneldebug: cleancache gen_debug_target
	@build -D GCC=Gcc -p InsydeModulePkg/InsydeModulePkg.dsc -N -s -b DEBUG

cleancache:
	@rm $(WORKSPACE)/Conf/.cache -rf

clean: cleancache
	@rm $(WORKSPACE)/Build/$(PROJECT_PKG) -rf

release: gen_release_target prebuild build postbuild
  
debug: gen_debug_target prebuild build postbuild

build:
	build -D GCC=Gcc -D LIB32_PATH=$(LIB32_PATH) -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)

gen_release_target:
	@echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)/Conf/target.txt
	@echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)/Conf/target.txt
	@echo TARGET                       = RELEASE                          >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)/Conf/target.txt
	@echo MAX_CONCURRENT_THREAD_NUMBER = $(shell nproc)                   >> $(WORKSPACE)/Conf/target.txt
	@echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)/Conf/target.txt
	@mkdir -p $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/Font.hpk
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/Layout.hpk
	@GenFlashLayout Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN) Build/$(PROJECT_PKG)/AutoGen/FlashLayout

gen_debug_target:
	@echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)/Conf/target.txt
	@echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)/Conf/target.txt
	@echo TARGET                       = DEBUG                            >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)/Conf/target.txt
	@echo MAX_CONCURRENT_THREAD_NUMBER = $(shell nproc)                   >> $(WORKSPACE)/Conf/target.txt
	@echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)/Conf/target.txt
	@mkdir -p $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/Font.hpk
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/Layout.hpk
	@GenFlashLayout Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN) Build/$(PROJECT_PKG)/AutoGen/FlashLayout
	
#
# Pre-build processes
#
kernel_prebuild:
	@echo Processing Kernel Pre-build
	PkgPreBuild $(WORKSPACE)/$(PROJECT_PKG)/Project.dsc $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.dsc
	PkgPreBuild $(WORKSPACE)/$(PROJECT_PKG)/Project.fdf $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.fdf
	GenFlashSetting Build/$(PROJECT_PKG)/AutoGen

chipset_prebuild:
	@echo Dummy Chipset PreBuild

project_prebuild:
	@echo Dummy Project PreBuild


#
# Post-build processes
#
kernel_postbuild:
	@echo Dummy Kernel PostBuild

chipset_postbuild:
	@echo Dummy Chipset PostBuild

project_postbuild:
	@echo Dummy Project PostBuild

prebuild:  kernel_prebuild  chipset_prebuild  project_prebuild
postbuild: kernel_postbuild  chipset_postbuild  project_postbuild
