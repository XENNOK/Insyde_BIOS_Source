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

#
# Build options for build targets
#

export WORKSPACE = $(shell dirname $$PWD)

include $(WORKSPACE)/BaseTools/Conf/GNUmakefile

project_postbuild:
	FitInfo -i $(PROJECT_PKG)/PlatformConfig/PatchFitConfig.ini
