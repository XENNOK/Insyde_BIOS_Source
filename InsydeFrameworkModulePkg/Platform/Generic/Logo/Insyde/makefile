#;******************************************************************************
#;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
#;*
#;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#;* transmit, broadcast, present, recite, release, license or otherwise exploit
#;* any part of this publication in any form, by any means, without the prior
#;* written permission of Insyde Software Corporation.
#;*
#;******************************************************************************
#/*++
#
#  Copyright (c) 2001-2002 Intel Corporation.  All rights reserved.
#
#  This software and associated documentation (if any) is furnished under
#  a license and may only be used or copied in accordance with the terms
#  of the license.  Except as permitted by such license, no part of this
#  software or documentation may be reproduced, stored in a retrieval system,
#  or transmitted in any form or by any means without the express written
#  consent of Intel Corporation.
#
#  File Name:
#
#    makefile
#
#  Abstract:
#
#    This is the makefile for creating the Logo FFS file. It requires its own
#    makefile because it's a special build case that does not fit in well
#    with the standard build of drivers.
#
#--*/

#
# Define some useful macros, then include the master Efi toolchain setup
# file.
#
BIN_DIR     = $(BUILD_DIR)\$(PROCESSOR)
TOOLCHAIN   = TOOLCHAIN_$(PROCESSOR)

!INCLUDE $(BUILD_DIR)\PlatformTools.env

#
# We simply copy the Pcx file from the source directory to the build directory
#
$(BIN_DIR)\$(LOGO_NAME) : $(SOURCE_DIR)\$(LOGO_NAME)
  copy $(SOURCE_DIR)\$(LOGO_NAME) $(BIN_DIR)\$(LOGO_NAME) /y

#
# Use GenFFSFile to convert it to an FFS file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs : $(BIN_DIR)\$(LOGO_NAME)
  $(GENSECTION) -I $(BIN_DIR)\$(LOGO_NAME) -O $(BIN_DIR)\$(BASE_NAME).sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(BIN_DIR) -P1 $(DEST_DIR)\$(BASE_NAME).pkg -V

all: $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs


