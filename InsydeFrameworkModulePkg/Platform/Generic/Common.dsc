#;******************************************************************************
#;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
#;*
#;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#;* transmit, broadcast, present, recite, release, license or otherwise exploit
#;* any part of this publication in any form, by any means, without the prior
#;* written permission of Insyde Software Corporation.
#;*
#;******************************************************************************
#
# This file contains a 'Sample Driver' and is licensed as such
# under the terms of your license agreement with Intel or your
# vendor.  This file may be modified by the user, subject to
# the additional terms of the license agreement
#
#/*++
#
# Copyright (c)  2000 - 2006 Intel Corporation. All rights reserved
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
#  Module Name:
#
#    Common.dsc
#
#  Abstract:
#
#    This is the build description file containing the platform
#    independent build instructions.  Platform specific instructions will
#    be prepended to produce the final build DSC file.
#
#
#  Notes:
#
#    The info in this file is broken down into sections. The start of a section
#    is designated by a "[" in the first column. So the [=====] separater ends
#    a section.
#
#--*/

[=============================================================================]
#
# These get emitted at the top of the generated master makefile.
#
[=============================================================================]
[Makefile.out]

#
# From the [makefile.out] section of the DSC file
#
TOOLCHAIN =
MAKE      = nmake -nologo

!INCLUDE $(BUILD_DIR)\PlatformTools.env

all : libraries fvs fv_null_components

#
# summary target for components with FV=NULL to handle the case
# when no components exist with FV=NULL
#
fv_null_components ::
  @echo.

[=============================================================================]
#
# These get expanded and dumped out to each component makefile after the
# component INF [defines] section gets parsed.
#
[=============================================================================]
[Makefile.Common]
#
# From the [Makefile.Common] section of the description file.
#
PROCESSOR        = $(PROCESSOR)
BASE_NAME        = $(BASE_NAME)
BUILD_NUMBER     = $(BUILD_NUMBER)
VERSION_STRING   = $(VERSION_STRING)
TOOLCHAIN        = TOOLCHAIN_$(PROCESSOR)
FILE_GUID        = $(FILE_GUID)
COMPONENT_TYPE   = $(COMPONENT_TYPE)
BUILD_TYPE       = $(BUILD_TYPE)
INF_FILENAME     = $(INF_FILENAME)
PACKAGE_FILENAME = $(PACKAGE_FILENAME)
FV_DIR           = $(BUILD_DIR)\FV
PLATFORM         = $(PROJECT_NAME)
BINARY_VERSION   = $(BINARY_VERSION)
STRGATHER_RESERVE_SIZE = $(STRGATHER_RESERVE_SIZE)

!IF "$(LANGUAGE)" != ""
LANGUAGE_FLAGS    = -lang $(LANGUAGE)
!ENDIF

!INCLUDE $(BUILD_DIR)\PlatformTools.env

!IF "$(COMPONENT_TYPE)" == "PIC_PEIM" || "$(COMPONENT_TYPE)" == "PE32_PEIM" || "$(COMPONENT_TYPE)" == "RELOCATABLE_PEIM" || "$(COMPONENT_TYPE)" == "COMBINED_PEIM_DRIVER"
DEPEX_TYPE = EFI_SECTION_PEI_DEPEX
!ELSE
DEPEX_TYPE = EFI_SECTION_DXE_DEPEX
!ENDIF


[=============================================================================]
#
# These are the commands to compile source files. One of these blocks gets
# emitted to the component's makefile for each source file. The section
# name is encoded as [Compile.$(PROCESSOR).source_filename_extension], where
# the source filename comes from the sources section of the component INF file.
#
[=============================================================================]
[Compile.Ia32.asm,Compile.x64.asm]

$(DEST_DIR)\$(FILE).obj : $(SOURCE_FILE_NAME)
  $(ASM) $(ASM_FLAGS) $**

[=============================================================================]
[Compile.Ipf.s]

$(DEST_DIR)\$(FILE).pro : $(SOURCE_FILE_NAME) $(INF_FILENAME)
 $(CC) $(C_FLAGS_PRO) $(SOURCE_FILE_NAME) > $@

$(DEST_DIR)\$(FILE).obj : $(DEST_DIR)\$(FILE).pro
 $(ASM) $(ASM_FLAGS) $(DEST_DIR)\$(FILE).pro

[=============================================================================]
[Compile.Ia32.c,Compile.Ipf.c,Compile.x64.c]

#
# If it already exists, then include the dependency list file for this
# source file. If it doesn't exist, then this is a clean build and the
# dependency file will get created below and the source file will get
# compiled. Don't do any of this if NO_MAKEDEPS is defined.
#
!IF ("$(NO_MAKEDEPS)" == "")

!IF EXIST($(DEST_DIR)\$(FILE).dep)
!INCLUDE $(DEST_DIR)\$(FILE).dep
!ENDIF

#
# This is how to create the dependency file.
#
DEP_FILE = $(DEST_DIR)\$(FILE).dep

$(DEP_FILE) : $(SOURCE_FILE_NAME)
  $(MAKEDEPS) -ignorenotfound -f $(SOURCE_FILE_NAME) -q -target \
    $(DEST_DIR)\$(FILE).obj \
    -o $(DEP_FILE) $(INC)

!ENDIF

#
# Compile the file
#
$(DEST_DIR)\$(FILE).obj : $(SOURCE_FILE_NAME) $(INC_DEPS) $(DEP_FILE)
  $(CC) $(C_FLAGS) $(SOURCE_FILE_NAME)

[=============================================================================]
[Compile.Ebc.c]

#
# If it already exists, then include the dependency list file for this
# source file. If it doesn't exist, then this is a clean build and the
# dependency file will get created below and the source file will get
# compiled. Don't do any of this if NO_MAKEDEPS is defined.
#
!IF ("$(NO_MAKEDEPS)" == "")

!IF EXIST($(DEST_DIR)\$(FILE).dep)
!INCLUDE $(DEST_DIR)\$(FILE).dep
!ENDIF

#
# This is how to create the dependency file.
#
DEP_FILE = $(DEST_DIR)\$(FILE).dep

$(DEP_FILE) : $(SOURCE_FILE_NAME)
  $(MAKEDEPS) -ignorenotfound -f $(SOURCE_FILE_NAME) -q -target \
    $(DEST_DIR)\$(FILE).obj \
    -o $(DEP_FILE) $(INC)

!ENDIF

#
# Redefine the entry point function if an entry point has been defined at all.
# This is required because all EBC entry point functions must be called
# EfiMain.
#
!IF DEFINED(IMAGE_ENTRY_POINT)
!IF "$(IMAGE_ENTRY_POINT)" != "EfiMain"
EBC_C_FLAGS = $(EBC_C_FLAGS) /D $(IMAGE_ENTRY_POINT)=EfiMain
!ENDIF
!ENDIF

#
# This is how to compile the source .c file
# Use -P to get preprocessor output file (.i)
#
$(DEST_DIR)\$(FILE).obj : $(SOURCE_FILE_NAME) $(INF_FILENAME) $(DEP_FILE)
  $(EBC_CC) $(EBC_C_FLAGS) -X $(INC) -Fa$(DEST_DIR)\$(FILE).cod \
    $(SOURCE_FILE_NAME) -Fo$(DEST_DIR)\$(FILE).obj

[=============================================================================]
#
# Commands for compiling a ".apr" Apriori source file.
#
[=============================================================================]
[Compile.Ia32.Apr,Compile.Ipf.Apr,Compile.Ebc.Apr,Compile.x64.Apr]

#
# Create the raw binary file. If you get an error on the build saying it doesn't
# know how to create the .apr file, then you're missing (or mispelled) the
# "APRIORI=" on the component lines in components section in the DSC file.
#
$(DEST_DIR)\$(BASE_NAME).bin : $(BUILD_DIR)\$(DSC_FILENAME)
  $(GENAPRIORI) -v -f $(FILE).apr -o $(DEST_DIR)\$(BASE_NAME).bin -i

$(DEST_DIR)\$(BASE_NAME).sec : $(DEST_DIR)\$(BASE_NAME).bin
  $(GENSECTION) -I $(DEST_DIR)\$(BASE_NAME).bin -O $(DEST_DIR)\$(BASE_NAME).sec -S EFI_SECTION_RAW

[=============================================================================]
[Build.Ia32.Apriori,Build.Ipf.Apriori,Build.Ebc.Apriori,Build.x64.Apriori]

all : $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).FFS

#
# Run GenFfsFile on the package file and .raw file to create the firmware file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).FFS : $(DEST_DIR)\$(BASE_NAME).sec
  $(GENFFSFILE) -B $(DEST_DIR) -P1 $(PACKAGE_FILENAME) -V

[=============================================================================]
[Build.Ia32.Makefile,Build.Ipf.Makefile,Build.Ebc.Makefile,Build.x64.Makefile]

!IF "$(MAKEFILE_OVERRIDE_DIR)" == ""
MAKEFILE_DIR = $(SOURCE_DIR)
!ELSE
MAKEFILE_DIR = $(MAKEFILE_OVERRIDE_DIR)
!ENDIF
#
# Copy the makefile directly from the source directory, then make it
# writable so we can copy over it later if we try to.
#
$(DEST_DIR)\makefile.new : $(MAKEFILE_DIR)\makefile.new
  copy $(MAKEFILE_DIR)\makefile.new $(DEST_DIR)\makefile.new
  attrib -r $(DEST_DIR)\makefile.new

#
# Make the all target, set some required macros.
#
call_makefile :
  $(MAKE) -f $(DEST_DIR)\makefile.new all   \
          SOURCE_DIR=$(SOURCE_DIR)          \
          BUILD_DIR=$(BUILD_DIR)            \
          FILE_GUID=$(FILE_GUID)            \
          DEST_DIR=$(DEST_DIR)              \
          PROCESSOR=$(PROCESSOR)            \
          TOOLCHAIN=TOOLCHAIN_$(PROCESSOR)  \
          BASE_NAME=$(BASE_NAME)            \
          PACKAGE_FILENAME=$(PACKAGE_FILENAME)

all : $(DEST_DIR)\makefile.new call_makefile

[=============================================================================]
#
# Instructions for building a component that uses a custom makefile. Encoding
# is [build.$(PROCESSOR).$(BUILD_TYPE)].
#
# To build these components, simply call the makefile from the source
# directory.
#
[Build.Ia32.Custom_Makefile,Build.Ipf.Custom_Makefile,Build.Ebc.Custom_Makefile,Build.x64.Custom_Makefile]
!IF "$(MAKEFILE_OVERRIDE_DIR)" == ""
MAKEFILE_DIR = $(SOURCE_DIR)
!ELSE
MAKEFILE_DIR = $(MAKEFILE_OVERRIDE_DIR)
!ENDIF
#
# Just call the makefile from the source directory, passing in some
# useful info.
#
all :
  $(MAKE) -f $(MAKEFILE_DIR)\makefile all  \
          SOURCE_DIR=$(SOURCE_DIR)         \
          BUILD_DIR=$(BUILD_DIR)           \
          DEST_DIR=$(DEST_DIR)             \
          FILE_GUID=$(FILE_GUID)           \
          PROCESSOR=$(PROCESSOR)           \
          TOOLCHAIN=TOOLCHAIN_$(PROCESSOR) \
          BASE_NAME=$(BASE_NAME)           \
          PLATFORM=$(PLATFORM)             \
          SOURCE_FV=$(SOURCE_FV)           \
          PACKAGE_FILENAME=$(PACKAGE_FILENAME)

[=============================================================================]
#
# These commands are used to build libraries
#
[=============================================================================]
[Build.Ia32.LIBRARY|LIBRARY_OBJ|LIBRARY_LIB,Build.Ipf.LIBRARY|LIBRARY_OBJ|LIBRARY_LIB,Build.x64.LIBRARY|LIBRARY_OBJ|LIBRARY_LIB]

#
# LIB all the object files into to our target lib file. Put
# a dependency on the component's INF file in case it changes.
#
!IF "$(BUILD_TYPE)" == "LIBRARY_LIB"
BUILD_METHOD = FROM_LIB
!ELSEIF "$(BUILD_TYPE)" == "LIBRARY_OBJ"
BUILD_METHOD = FROM_OBJ
!ELSE
BUILD_METHOD = FROM_SRC
!ENDIF

!IF "$(LOCALIZE)" == "YES" && ("$(BUILD_FROM_BINARY)" == "NO" || "$(BUILD_METHOD)" == "FROM_SRC") && "$(SOURCE_FILES)" != ""
$(DEST_DIR)\$(BASE_NAME).sdb : $(SDB_FILES) $(SOURCE_FILES)
  $(STRGATHER) -scan -vdbr $(STRGATHER_FLAGS) -od $(DEST_DIR)\$(BASE_NAME).sdb \
    -skipext .uni -skipext .h -skipext .ani -skipext .gif @<<
    $(SOURCE_FILES)
<<

!IF "$(STRGATHER_RESERVE_SIZE)" == ""
STRGATHER_RESERVE_SIZE = 0
!ENDIF

$(DEST_DIR)\$(BASE_NAME)Strings.c : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
   -reserve-size $(STRGATHER_RESERVE_SIZE) -oc $(DEST_DIR)\$(BASE_NAME)Strings.c

$(DEST_DIR)\$(BASE_NAME)StrDefs.h : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -oh $(DEST_DIR)\$(BASE_NAME)StrDefs.h

OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Strings.obj

$(DEST_DIR)\$(BASE_NAME)Strings.obj : $(DEST_DIR)\$(BASE_NAME)Strings.c
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Strings.c

LOCALIZE_TARGETS = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h
!ENDIF

!IF "$(GENERATE_ANI)" == "YES" && ("$(BUILD_FROM_BINARY)" == "NO" || "$(BUILD_METHOD)" == "FROM_SRC") && "$(SOURCE_FILES)" != ""
OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Animations.obj

$(DEST_DIR)\$(BASE_NAME)Animations.i : $(ANI_FILES) $(GIF_FILES)
  type $(ANI_FILES) > $(DEST_DIR)\$(BASE_NAME)Animations.i

$(DEST_DIR)\$(BASE_NAME)AniDefs.h : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oh $(DEST_DIR)\$(BASE_NAME)AniDefs.h

$(DEST_DIR)\$(BASE_NAME)Animations.c : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oc $(DEST_DIR)\$(BASE_NAME)Animations.c

$(DEST_DIR)\$(BASE_NAME)Animations.obj : $(DEST_DIR)\$(BASE_NAME)Animations.c $(INF_FILENAME) $(ALL_DEPS)
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Animations.c

LOCALIZE_TARGETS = $(DEST_DIR)\$(BASE_NAME)AniDefs.h $(LOCALIZE_TARGETS)
!ENDIF

!IF "$(OBJECTS)" == "" && "$(BUILD_METHOD)" == "FROM_OBJ" && "$(BUILD_TO_BINARY)" == "YES"
!ERROR Can't build objedt when the source files section is empty
!ENDIF

LIB_NAME         = $(LIB_DIR)\$(BASE_NAME).lib


!IF "$(OBJECTS)" != ""
TARGET_LOCAL_LIB = $(DEST_DIR)\$(BASE_NAME)Local.lib
BIN_TARGETS      = $(BIN_TARGETS) $(TARGET_LOCAL_LIB)


!IF "$(BUILD_FROM_BINARY)" == "YES" && "$(BUILD_METHOD)" == "FROM_OBJ"

!IF "$(SOURCE_OVERRIDE_PATH2)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSEIF "$(SOURCE_OVERRIDE_PATH)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSEIF EXIST ("$(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSE
!ERROR Pre-existing $(BASE_NAME)Local.lib file not found in $(SOURCE_DIR)\$(PROCESSOR) nor override folder
!ENDIF

!ELSE
$(TARGET_LOCAL_LIB) : $(OBJECTS) $(INF_FILENAME)
  $(LIB) $(LIB_FLAGS) $(OBJECTS) /OUT:$@
!ENDIF
!ENDIF

!IF ("$(BUILD_FROM_BINARY)" == "YES" || "$(SOURCE_FILES)" == "") && ("$(BUILD_METHOD)" == "FROM_LIB")
BIN_TARGETS =

!IF "$(SOURCE_OVERRIDE_PATH2)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME).lib")
BINARY_TARGET_LIB = $(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME).lib
!ELSEIF "$(SOURCE_OVERRIDE_PATH)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME).lib")
BINARY_TARGET_LIB = $(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME).lib
!ELSEIF EXIST ("$(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME).lib")
BINARY_TARGET_LIB = $(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME).lib
!ELSE
!ERROR Pre-existing $(BASE_NAME).lib file not found in $(SOURCE_DIR)\$(PROCESSOR) or override folder
!ENDIF

COPY_LIB:
  copy /Y $(BINARY_TARGET_LIB) $(LIB_NAME)

$(LIB_NAME) : COPY_LIB
!ELSE
$(LIB_NAME) : $(TARGET_LOCAL_LIB) $(LIBS) $(INF_FILENAME)
  $(LIB) $(LIB_FLAGS) $(LIBS) $(TARGET_LOCAL_LIB) /OUT:$@
!ENDIF

SET_BINARY:
!IF "$(BUILD_METHOD)" != "FROM_SRC" && "$(BUILD_TO_BINARY)" == "YES"
  if not exist $(SOURCE_DIR)\$(PROCESSOR) mkdir $(SOURCE_DIR)\$(PROCESSOR)
!IF "$(BUILD_METHOD)" == "FROM_OBJ"
  copy  $(TARGET_LOCAL_LIB) $(SOURCE_DIR)\$(PROCESSOR)
!ELSEIF "$(BUILD_METHOD)" == "FROM_LIB"
  copy  $(LIB_NAME) $(SOURCE_DIR)\$(PROCESSOR)
!ENDIF
!ENDIF

DEL_SOURCE:
!IF "$(BUILD_METHOD)" != "FROM_SRC" && "$(REMOVE_SOURCE_WHEN_BUILD_BINARY)" == "YES" && EXIST ("$(DEST_DIR)\$(BASE_NAME)RemoveFile.bat")
  $(DEST_DIR)\$(BASE_NAME)RemoveFile.bat
!ENDIF


all: $(LOCALIZE_TARGETS) $(BIN_TARGETS) $(LIB_NAME) SET_BINARY DEL_SOURCE


[=============================================================================]

[=============================================================================]
[Build.Ebc.Library]

$(LIB_DIR)\$(BASE_NAME).lib : $(OBJECTS) $(LIBS)
   $(EBC_LIB) $(EBC_LIB_FLAGS) $(OBJECTS) $(LIBS) /OUT:$(LIB_DIR)\$(BASE_NAME).lib

all : $(LIB_DIR)\$(BASE_NAME).lib

[=============================================================================]
#
# This is the Build.$(PROCESSOR).$(COMPONENT_TYPE) section that tells how to
# convert a firmware volume into an FV FFS file. Simply run it through
# GenFfsFile with the appropriate package file. SOURCE_FV must be defined
# in the component INF file Defines section.
#
[Build.Ia32.FvImageFile,Build.x64.FvImageFile]

all : $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).Fvi

#
# Run GenFfsFile on the package file and FV file to create the firmware
# volume FFS file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).Fvi : $(DEST_DIR)\$(SOURCE_FV)Fv.sec
  $(GENFFSFILE) -B $(DEST_DIR) -P1 $(PACKAGE_FILENAME) -V

[=============================================================================]
#
# Since many of the steps are the same for the different component types, we
# share this section for BS_DRIVER, RT_DRIVER, .... and IFDEF the parts that
# differ.  The entire section gets dumped to the output makefile.
#
[=============================================================================]
[Build.Ia32.BS_DRIVER|RT_DRIVER|SAL_RT_DRIVER|TE_PEIM|DODPX_TE_PEIM|NODPX_TE_PEIM|PE32_PEIM|NODPX_PE32_PEIM|PEI_CORE|PIC_PEIM|RELOCATABLE_PEIM|DXE_CORE|APPLICATION|COMBINED_PEIM_DRIVER, Build.Ipf.BS_DRIVER|RT_DRIVER|SAL_RT_DRIVER|SECURITY_CORE|TE_PEIM|DODPX_TE_PEIM|NODPX_TE_PEIM|PEI_CORE|PE32_PEIM|PIC_PEIM|DXE_CORE|APPLICATION|COMBINED_PEIM_DRIVER,Build.x64.BS_DRIVER|RT_DRIVER|SAL_RT_DRIVER|TE_PEIM|DODPX_TE_PEIM|NODPX_TE_PEIM|PE32_PEIM|NODPX_PE32_PEIM|PEI_CORE|PIC_PEIM|RELOCATABLE_PEIM|DXE_CORE|APPLICATION|COMBINED_PEIM_DRIVER]

!IF "$(LOCALIZE)" == "YES"

!IF "$(EFI_GENERATE_HII_EXPORT)" == "YES"
STRGATHER_FLAGS   = $(STRGATHER_FLAGS) -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

#
# There will be one HII pack containing all the strings. Add that file
# to the list of HII pack files we'll use to create our final HII export file.
#
HII_PACK_FILES    = $(HII_PACK_FILES) $(DEST_DIR)\$(BASE_NAME)Strings.hpk
LOCALIZE_TARGETS  = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME).hii
!ENDIF

!IF "$(STRGATHER_RESERVE_SIZE)" == ""
STRGATHER_RESERVE_SIZE = 0
!ENDIF

$(DEST_DIR)\$(BASE_NAME).sdb : $(SDB_FILES) $(SOURCE_FILES)
  $(STRGATHER) -scan -vdbr $(STRGATHER_FLAGS) -od $(DEST_DIR)\$(BASE_NAME).sdb \
    -skipext .uni -skipext .h -skipext .ani -skipext .gif @<<
    $(SOURCE_FILES)
<<

$(DEST_DIR)\$(BASE_NAME)Strings.c : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -reserve-size $(STRGATHER_RESERVE_SIZE) -oc $(DEST_DIR)\$(BASE_NAME)Strings.c
    $(GENSETUPFONT) -g $(FILE_GUID) -n $(DEST_DIR)\$(BASE_NAME)Strings.c -d $(BIN_DIR)\SetupFont.bin

$(DEST_DIR)\$(BASE_NAME)StrDefs.h : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -oh $(DEST_DIR)\$(BASE_NAME)StrDefs.h

$(DEST_DIR)\$(BASE_NAME)Strings.hpk : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Strings.obj

$(DEST_DIR)\$(BASE_NAME)Strings.obj : $(DEST_DIR)\$(BASE_NAME)Strings.c
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Strings.c

LOCALIZE_TARGETS = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h

!ENDIF
!IF "$(GENERATE_ANI)" == "YES"
OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Animations.obj

$(DEST_DIR)\$(BASE_NAME)Animations.i : $(ANI_FILES) $(GIF_FILES)
  type $(ANI_FILES) > $(DEST_DIR)\$(BASE_NAME)Animations.i

$(DEST_DIR)\$(BASE_NAME)AniDefs.h : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oh $(DEST_DIR)\$(BASE_NAME)AniDefs.h

$(DEST_DIR)\$(BASE_NAME)Animations.c : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oc $(DEST_DIR)\$(BASE_NAME)Animations.c

$(DEST_DIR)\$(BASE_NAME)Animations.obj : $(DEST_DIR)\$(BASE_NAME)Animations.c $(INF_FILENAME) $(ALL_DEPS)
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Animations.c

LOCALIZE_TARGETS = $(DEST_DIR)\$(BASE_NAME)AniDefs.h $(LOCALIZE_TARGETS)
!ENDIF

#
# If we have any objects associated with this component, then we're
# going to build a local library from them.
#
!IFNDEF OBJECTS
!ERROR No source files to build were defined in the INF file
!ENDIF

TARGET_LOCAL_LIB  = $(DEST_DIR)\$(BASE_NAME)Local.lib
BIN_TARGETS       = $(BIN_TARGETS) $(TARGET_LOCAL_LIB)

#
# LIB all the object files into our (local) target lib file. Put
# a dependency on the component's INF file in case it changes.
#
$(TARGET_LOCAL_LIB) : $(OBJECTS)  $(INF_FILENAME)
  $(LIB) $(LIB_FLAGS) $(OBJECTS) /OUT:$@

#
# Defines for standard intermediate files and build targets
#
TARGET_DLL      = $(BIN_DIR)\$(BASE_NAME).dll
TARGET_EFI      = $(BIN_DIR)\$(BASE_NAME).efi
TARGET_DPX      = $(DEST_DIR)\$(BASE_NAME).dpx
TARGET_UI       = $(DEST_DIR)\$(BASE_NAME).ui
TARGET_VER      = $(DEST_DIR)\$(BASE_NAME).ver
TARGET_MAP      = $(DEST_DIR)\$(BASE_NAME).map
TARGET_PDB      = $(EFI_SYMBOL_PATH)\$(BASE_NAME).pdb
TARGET_SYM      = $(BIN_DIR)\$(BASE_NAME).sym
TARGET_TE       = $(BIN_DIR)\$(BASE_NAME).te
TARGET_TES      = $(DEST_DIR)\$(BASE_NAME).tes

#
# Target executable section extension depends on the component type.
# Only define "TARGET_DXE_DPX" if it's a combined peim driver.
#
!IF "$(COMPONENT_TYPE)" == "PIC_PEIM"
TARGET_PE32 = $(DEST_DIR)\$(BASE_NAME).pic
!ELSE
TARGET_PE32 = $(DEST_DIR)\$(BASE_NAME).pe32
!ENDIF

#
# Target FFS file extension depends on the component type
# Also define "TARGET_DXE_DPX" if it's a combined PEIM driver.
#
SUBSYSTEM = EFI_BOOT_SERVICE_DRIVER

!IF "$(COMPONENT_TYPE)" == "APPLICATION"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).app
SUBSYSTEM       = EFI_APPLICATION
!ELSE IF "$(COMPONENT_TYPE)" == "SECURITY_CORE"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).sec
!ELSE IF "$(COMPONENT_TYPE)" == "PEI_CORE"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
!ELSE IF "$(COMPONENT_TYPE)" == "PE32_PEIM"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
!ELSE IF "$(COMPONENT_TYPE)" == "RELOCATABLE_PEIM"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
!ELSE IF "$(COMPONENT_TYPE)" == "PIC_PEIM"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
!ELSE IF "$(COMPONENT_TYPE)" == "COMBINED_PEIM_DRIVER"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
TARGET_DXE_DPX  = $(DEST_DIR)\$(BASE_NAME).dpxd
!ELSE
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).dxe
!ENDIF

#
# Build a FFS file from the sections and package
#
!IF "$(BUILD_TYPE)" == "TE_PEIM" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM"
$(TARGET_FFS_FILE) : $(TARGET_TES) $(TARGET_DPX) $(TARGET_UI) $(TARGET_VER)
!ELSE
$(TARGET_FFS_FILE) : $(TARGET_PE32) $(TARGET_DPX) $(TARGET_UI) $(TARGET_VER) $(TARGET_DXE_DPX)
!ENDIF
#
# Some of our components require padding to align code
#
!IF "$(PROCESSOR)" == "IPF"
!IF "$(COMPONENT_TYPE)" == "PIC_PEIM" || "$(COMPONENT_TYPE)" == "PE32_PEIM" || "$(COMPONENT_TYPE)" == "RELOCATABLE_PEIM" || "$(COMPONENT_TYPE)" == "SECURITY_CORE" || "$(COMPONENT_TYPE)" == "PEI_CORE" || "$(COMPONENT_TYPE)" == "COMBINED_PEIM_DRIVER"
  copy $(BIN_DIR)\Blank.pad $(DEST_DIR)
!ENDIF
!ENDIF
  $(GENFFSFILE) -B $(DEST_DIR) -P1 $(PACKAGE_FILENAME) -V

#
# Different methods to build section based on if PIC_PEIM
#
!IF "$(COMPONENT_TYPE)" == "PIC_PEIM"

$(TARGET_PE32) : $(TARGET_DLL)
  $(PE2BIN) $(TARGET_DLL) $(DEST_DIR)\$(BASE_NAME).TMP
#
# BUGBUG: Build PEIM header, needs to go away with new PEI.
#
  $(TEMPGENSECTION) -P $(SOURCE_DIR)\$(BASE_NAME).INF -I $(DEST_DIR)\$(BASE_NAME).TMP -O $(TARGET_PIC_PEI).tmp -M $(TARGET_MAP) -S EFI_SECTION_TYPE_NO_HEADER
  $(GENSECTION) -I $(TARGET_PIC_PEI).tmp -O $(TARGET_PE32) -S EFI_SECTION_PIC
  del $(DEST_DIR)\$(BASE_NAME).TMP

!ELSE

!IF  "$(BUILD_TYPE)" == "TE_PEIM" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM"
#
# Create our TE section from our TE file
#
$(TARGET_TES) : $(TARGET_TE) $(INF_FILENAME)
  $(GENSECTION) -I $(TARGET_TE) -O $(TARGET_TES) -S EFI_SECTION_TE

#
# Run GenTEImage on the built .efi file to create our TE file.
#
$(TARGET_TE) : $(TARGET_EFI)
  $(GENTEIMAGE) -o $(TARGET_TE) $(TARGET_EFI)

!ELSE
$(TARGET_PE32) : $(TARGET_EFI) $(INF_FILENAME)
  $(GENSECTION) -I $(TARGET_EFI) -O $(TARGET_PE32) -S EFI_SECTION_PE32
!ENDIF
#
# BUGBUG: This step is obsolete when a linker is released that supports EFI.
#
$(TARGET_EFI) : $(TARGET_DLL)
  $(FWIMAGE) -t 0 $(COMPONENT_TYPE) $(TARGET_DLL) $(TARGET_EFI)

!ENDIF

#
# generate info file and convert it to object file, then combine to dll file
#
!IF "$(BINARY_VERSION)" != ""
TARGET_INFO     = $(DEST_DIR)\$(BASE_NAME).info
TARGET_INFO_OBJ = $(DEST_DIR)\$(BASE_NAME).info.obj

$(TARGET_INFO_OBJ):
  echo 0>$(TARGET_INFO) >nul
  echo.$(BASE_NAME)>> $(TARGET_INFO)
  echo.$(BINARY_VERSION)>> $(TARGET_INFO)
  $(BIN2OBJ) -t $(PROCESSOR) -i $(TARGET_INFO) -o $(TARGET_INFO_OBJ) -s INFO
!ENDIF


#
# Link all objects and libs to create the executable
#
$(TARGET_DLL) : $(TARGET_LOCAL_LIB) $(LIBS) $(TARGET_INFO_OBJ)
  $(LINK) $(LINK_FLAGS_DLL) $(LIBS) $(TARGET_INFO_OBJ) /ENTRY:$(IMAGE_ENTRY_POINT) \
     $(TARGET_LOCAL_LIB) /OUT:$(TARGET_DLL) /MAP:$(TARGET_MAP) \
     /PDB:$(TARGET_PDB)
  $(SETSTAMP) $(TARGET_DLL) $(BUILD_DIR)\GenStamp.txt
!IF "$(EFI_GENERATE_SYM_FILE)" == "YES"
  if exist $(TARGET_PDB) $(PE2SYM) $(TARGET_PDB) $(TARGET_SYM)
!ENDIF

!IF "$(EFI_ZERO_DEBUG_DATA)" == "YES"
  $(ZERODEBUGDATA) $(TARGET_DLL)
!ENDIF

#
# Create the user interface section
#
$(TARGET_UI) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_UI) -S EFI_SECTION_USER_INTERFACE -A "$(BASE_NAME)"

#
# Create the version section
#
!IF "$(BUILD_NUMBER)" != ""
!IF "$(VERSION_STRING)" != ""
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER) -A "$(VERSION_STRING)"
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER)
!ENDIF
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  echo.>$(TARGET_VER)
  type $(TARGET_VER)>$(TARGET_VER)
!ENDIF

#
# Makefile entries to create the dependency expression section.
# Use the DPX file from the source directory unless an override file
# was specified.
# If no DPX source file was specified, then create an empty file to
# be used.
#
!IF "$(DPX_SOURCE)" != ""
DPX_SOURCE_FILE = $(SOURCE_DIR)\$(DPX_SOURCE)
!ENDIF

!IF "$(DPX_SOURCE_OVERRIDE)" != ""
DPX_SOURCE_FILE = $(DPX_SOURCE_OVERRIDE)
!ENDIF

!IF "$(DPX_SOURCE_FILE)" != "" && "$(BUILD_TYPE)" != "NODPX_PE32_PEIM" && "$(BUILD_TYPE)" != "NODPX_TE_PEIM"
!IF EXIST ($(DPX_SOURCE_FILE))
$(TARGET_DPX) : $(DPX_SOURCE_FILE) $(INF_FILENAME)
  $(CC) $(INC) $(C_FLAGS) /EP $(DPX_SOURCE_FILE) > $*.tmp1
  $(GENDEPEX) -I $*.tmp1 -O $*.tmp2
  $(GENSECTION) -I $*.tmp2 -O $@ -S $(DEPEX_TYPE)
  del $*.tmp1 > NUL
  del $*.tmp2 > NUL
!ELSE
!ERROR Dependency expression source file "$(DPX_SOURCE_FILE)" does not exist.
!ENDIF
!ELSE
$(TARGET_DPX) : $(INF_FILENAME)
  echo. > $(TARGET_DPX)
  type $(TARGET_DPX) > $(TARGET_DPX)
!ENDIF

#
# Makefile entries for DXE DPX for combined PEIM drivers.
# If a DXE_DPX_SOURCE file was specified in the INF file, use it. Otherwise
# create an empty file and use it as a DPX file.
#
!IF "$(COMPONENT_TYPE)" == "COMBINED_PEIM_DRIVER"
!IF "$(DXE_DPX_SOURCE)" != ""
!IF EXIST ($(SOURCE_DIR)\$(DPX_SOURCE))
$(TARGET_DXE_DPX) : $(SOURCE_DIR)\$(DXE_DPX_SOURCE) $(INF_FILENAME)
  $(CC) $(INC) /EP $(SOURCE_DIR)\$(DXE_DPX_SOURCE) > $*.tmp1
  $(GENDEPEX) -I $*.tmp1 -O $*.tmp2
  $(GENSECTION) -I $*.tmp2 -O $@ -S EFI_SECTION_DXE_DEPEX
  del $*.tmp1 > NUL
  del $*.tmp2 > NUL
!ELSE
!ERROR Dependency expression source file "$(SOURCE_DIR)\$(DXE_DPX_SOURCE)" does not exist.
!ENDIF
!ELSE
$(TARGET_DXE_DPX) : $(INF_FILENAME)
  echo. > $(TARGET_DXE_DPX)
  type $(TARGET_DXE_DPX) > $(TARGET_DXE_DPX)
!ENDIF
!ENDIF

#
# Describe how to build the HII export file from all the input HII pack files.
# Use the FFS file GUID for the package GUID in the export file. Only used
# when multiple VFR share strings.
#
$(DEST_DIR)\$(BASE_NAME).hii : $(HII_PACK_FILES)
  $(HIIPACK) create -g $(FILE_GUID) -p $(HII_PACK_FILES) -o $(DEST_DIR)\$(BASE_NAME).hii

#
# If the build calls for creating an FFS file with the IFR included as
# a separate binary (not compiled into the driver), then build the binary
# section now. Note that the PACKAGE must be set correctly to actually get
# this IFR section pulled into the FFS file.
#
!IF ("$(HII_IFR_PACK_FILES)" != "")

$(DEST_DIR)\$(BASE_NAME)IfrBin.sec : $(HII_IFR_PACK_FILES)
  $(HIIPACK) create -novarpacks -p $(HII_IFR_PACK_FILES) -o $(DEST_DIR)\$(BASE_NAME)IfrBin.hii
  $(GENSECTION) -I $(DEST_DIR)\$(BASE_NAME)IfrBin.hii -O $(DEST_DIR)\$(BASE_NAME)IfrBin.sec -S EFI_SECTION_RAW

BIN_TARGETS = $(BIN_TARGETS) $(DEST_DIR)\$(BASE_NAME)IfrBin.sec

!ENDIF

all: $(LOCALIZE_TARGETS) $(BIN_TARGETS) $(TARGET_FFS_FILE)
[=============================================================================]

#
# These are the commands to build EBC EFI targets
#
[=============================================================================]
[Build.Ebc.BS_DRIVER|APPLICATION]

#
# Add the EBC library to our list of libs
#
LIBS = $(LIBS) $(EBC_TOOLS_PATH)\lib\EbcLib.lib

!IF "$(LOCALIZE)" == "YES"

!IF "$(EFI_GENERATE_HII_EXPORT)" == "YES"
STRGATHER_FLAGS   = $(STRGATHER_FLAGS) -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

#
# There will be one HII pack containing all the strings. Add that file
# to the list of HII pack files we'll use to create our final HII export file.
#
HII_PACK_FILES = $(HII_PACK_FILES) $(DEST_DIR)\$(BASE_NAME)Strings.hpk

LOCALIZE_TARGETS  = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME).hii
!ENDIF

!IF "$(STRGATHER_RESERVE_SIZE)" == ""
STRGATHER_RESERVE_SIZE = 0
!ENDIF

$(DEST_DIR)\$(BASE_NAME).sdb : $(SDB_FILES) $(SOURCE_FILES)
  $(STRGATHER) -scan -vdbr $(STRGATHER_FLAGS) -od $(DEST_DIR)\$(BASE_NAME).sdb \
    -skipext .uni -skipext .h -skipext .ani -skipext .gif @<<
    $(SOURCE_FILES)
<<

$(DEST_DIR)\$(BASE_NAME)Strings.c : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -reserve-size $(STRGATHER_RESERVE_SIZE) -oc $(DEST_DIR)\$(BASE_NAME)Strings.c

$(DEST_DIR)\$(BASE_NAME)StrDefs.h : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -oh $(DEST_DIR)\$(BASE_NAME)StrDefs.h

$(DEST_DIR)\$(BASE_NAME)Strings.hpk : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Strings.obj

$(DEST_DIR)\$(BASE_NAME)Strings.obj : $(DEST_DIR)\$(BASE_NAME)Strings.c
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Strings.c

LOCALIZE_TARGETS = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h

!ENDIF

#
# If building an application, then the target is a .app, not .dxe
#
!IF "$(COMPONENT_TYPE)" == "APPLICATION"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).app
SUBSYSTEM       = EFI_APPLICATION
!ELSE
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).dxe
SUBSYSTEM       = EFI_BOOT_SERVICE_DRIVER
!ENDIF

#
# Defines for standard intermediate files and build targets
#
TARGET_EFI  = $(BIN_DIR)\$(BASE_NAME).efi
TARGET_DPX  = $(DEST_DIR)\$(BASE_NAME).dpx
TARGET_UI   = $(DEST_DIR)\$(BASE_NAME).ui
TARGET_VER  = $(DEST_DIR)\$(BASE_NAME).ver
TARGET_MAP  = $(DEST_DIR)\$(BASE_NAME).map
TARGET_PDB  = $(EFI_SYMBOL_PATH)\$(BASE_NAME).pdb
TARGET_PE32 = $(DEST_DIR)\$(BASE_NAME).pe32
TARGET_DLL  = $(BIN_DIR)\$(BASE_NAME).dll

#
# First link all the objects and libs together to make a .dll file
#
$(TARGET_DLL) : $(OBJECTS) $(LIBS)
  $(EBC_LINK) $(EBC_LINK_FLAGS) /SUBSYSTEM:$(SUBSYSTEM) /ENTRY:EfiStart \
    $(OBJECTS) $(LIBS) /OUT:$(TARGET_DLL)
  $(SETSTAMP) $(TARGET_DLL) $(BUILD_DIR)\GenStamp.txt
!IF "$(EFI_ZERO_DEBUG_DATA)" == "YES"
  $(ZERODEBUGDATA) $(TARGET_DLL)
!ENDIF

#
# Now take the .dll file and make a .efi file
#
$(TARGET_EFI) : $(TARGET_DLL)
  $(FWIMAGE) -t 0 $(COMPONENT_TYPE) $(TARGET_DLL) $(TARGET_EFI)

#
# Now take the .efi file and make a .pe32 section
#
$(TARGET_PE32) : $(TARGET_EFI) $(INF_FILENAME)
  $(GENSECTION) -I $(TARGET_EFI) -O $(TARGET_PE32) -S EFI_SECTION_PE32

#
# Create the user interface section
#
$(TARGET_UI) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_UI) -S EFI_SECTION_USER_INTERFACE -A "$(BASE_NAME)"

#
# Create the version section
#
!IF "$(BUILD_NUMBER)" != ""
!IF "$(VERSION_STRING)" != ""
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER) -A "$(VERSION_STRING)"
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER)
!ENDIF
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  echo.>$(TARGET_VER)
  type $(TARGET_VER) > $(TARGET_VER)
!ENDIF

#
# Makefile entries to create the dependency expression section.
# Use the DPX file from the source directory unless an override file
# was specified.
# If no DPX source file was specified, then create an empty file to
# be used.
#
!IF "$(DPX_SOURCE)" != ""
DPX_SOURCE_FILE = $(SOURCE_DIR)\$(DPX_SOURCE)
!ENDIF

!IF "$(DPX_SOURCE_OVERRIDE)" != ""
DPX_SOURCE_FILE = $(DPX_SOURCE_OVERRIDE)
!ENDIF

!IF "$(DPX_SOURCE_FILE)" != ""
!IF EXIST ($(DPX_SOURCE_FILE))
$(TARGET_DPX) : $(DPX_SOURCE_FILE) $(INF_FILENAME)
  $(CC) $(INC) $(C_FLAGS) /EP $(DPX_SOURCE_FILE) > $*.tmp1
  $(GENDEPEX) -I $*.tmp1 -O $*.tmp2
  $(GENSECTION) -I $*.tmp2 -O $@ -S $(DEPEX_TYPE)
  del $*.tmp1 > NUL
  del $*.tmp2 > NUL
!ELSE
!ERROR Dependency expression source file "$(DPX_SOURCE_FILE)" does not exist.
!ENDIF
!ELSE
$(TARGET_DPX) : $(INF_FILENAME)
  echo. > $(TARGET_DPX)
  type $(TARGET_DPX) > $(TARGET_DPX)
!ENDIF

#
# Build an FFS file from the sections and package
#
$(TARGET_FFS_FILE) : $(TARGET_PE32) $(TARGET_DPX) $(TARGET_UI) $(TARGET_VER)
  $(GENFFSFILE) -B $(DEST_DIR) -P1 $(PACKAGE_FILENAME) -V

all: $(LOCALIZE_TARGETS) $(TARGET_FFS_FILE)

[=============================================================================]
#
# These are the commands to build vendor-provided *.EFI files into an FV.
# To use them, create an INF file with BUILD_TYPE=BS_DRIVER_EFI.
# This section, as it now exists, only supports boot service drivers.
#

[=============================================================================]
[Build.Ia32.TE_PEIM_OBJ|DODPX_TE_PEIM_OBJ|NODPX_TE_PEIM_OBJ|BS_DRIVER_OBJ|RT_DRIVER_OBJ|APPLICATION_OBJ|PE32_PEIM_OBJ|NODPX_PE32_PEIM_OBJ|PEI_CORE_OBJ|TE_PEIM_EFI|DODPX_TE_PEIM_EFI|NODPX_TE_PEIM_EFI|BS_DRIVER_EFI|RT_DRIVER_EFI|APPLICATION_EFI|PE32_PEIM_EFI|NODPX_PE32_PEIM_EFI|PEI_CORE_EFI,Build.Ipf.Ia32.TE_PEIM_OBJ|DODPX_TE_PEIM_OBJ|NODPX_TE_PEIM_OBJ|BS_DRIVER_OBJ|RT_DRIVER_OBJ|APPLICATION_OBJ|PE32_PEIM_OBJ|NODPX_PE32_PEIM_OBJ|PEI_CORE_OBJ|TE_PEIM_EFI|DODPX_TE_PEIM_EFI|NODPX_TE_PEIM_EFI|BS_DRIVER_EFI|RT_DRIVER_EFI|APPLICATION_EFI|PE32_PEIM_EFI|NODPX_PE32_PEIM_EFI|PEI_CORE_EFI,Build.Ebc.BS_DRIVER_OBJ|RT_DRIVER_OBJ|APPLICATION_OBJ|BS_DRIVER_EFI|RT_DRIVER_EFI|APPLICATION_EFI,Build.x64.BS_DRIVER_OBJ|RT_DRIVER_OBJ|APPLICATION_OBJ|PE32_PEIM_OBJ|NODPX_PE32_PEIM_OBJ|PEI_CORE_OBJ|BS_DRIVER_EFI|RT_DRIVER_EFI|APPLICATION_EFI|PE32_PEIM_EFI|NODPX_PE32_PEIM_EFI|PEI_CORE_EFI]

!IF "$(BUILD_FROM_BINARY)" == "NO" && "$(SOURCE_FILES)" != ""

!IF "$(LOCALIZE)" == "YES"
!IF "$(EFI_GENERATE_HII_EXPORT)" == "YES"
STRGATHER_FLAGS   = $(STRGATHER_FLAGS) -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

#
# There will be one HII pack containing all the strings. Add that file
# to the list of HII pack files we'll use to create our final HII export file.
#
HII_PACK_FILES    = $(HII_PACK_FILES) $(DEST_DIR)\$(BASE_NAME)Strings.hpk
LOCALIZE_TARGETS  = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME).hii
!ENDIF

!IF "$(STRGATHER_RESERVE_SIZE)" == ""
STRGATHER_RESERVE_SIZE = 0
!ENDIF

$(DEST_DIR)\$(BASE_NAME).sdb : $(SDB_FILES) $(SOURCE_FILES)
  $(STRGATHER) -scan -vdbr $(STRGATHER_FLAGS) -od $(DEST_DIR)\$(BASE_NAME).sdb \
    -skipext .uni -skipext .h -skipext .ani -skipext .gif @<<
    $(SOURCE_FILES)
<<

$(DEST_DIR)\$(BASE_NAME)Strings.c : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -reserve-size $(STRGATHER_RESERVE_SIZE) -oc $(DEST_DIR)\$(BASE_NAME)Strings.c
    $(GENSETUPFONT) -g $(FILE_GUID) -n $(DEST_DIR)\$(BASE_NAME)Strings.c -d $(BIN_DIR)\SetupFont.bin

$(DEST_DIR)\$(BASE_NAME)StrDefs.h : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -oh $(DEST_DIR)\$(BASE_NAME)StrDefs.h

$(DEST_DIR)\$(BASE_NAME)Strings.hpk : $(DEST_DIR)\$(BASE_NAME).sdb
  $(STRGATHER) -dump $(LANGUAGE_FLAGS) -bn $(BASE_NAME)Strings -db $(DEST_DIR)\$(BASE_NAME).sdb \
    -hpk $(DEST_DIR)\$(BASE_NAME)Strings.hpk

OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Strings.obj

$(DEST_DIR)\$(BASE_NAME)Strings.obj : $(DEST_DIR)\$(BASE_NAME)Strings.c
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Strings.c

LOCALIZE_TARGETS = $(LOCALIZE_TARGETS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h

!ENDIF

#
# Describe how to build the HII export file from all the input HII pack files.
# Use the FFS file GUID for the package GUID in the export file. Only used
# when multiple VFR share strings.
#
$(DEST_DIR)\$(BASE_NAME).hii : $(HII_PACK_FILES)
  $(HIIPACK) create -g $(FILE_GUID) -p $(HII_PACK_FILES) -o $(DEST_DIR)\$(BASE_NAME).hii

#
# If the build calls for creating an FFS file with the IFR included as
# a separate binary (not compiled into the driver), then build the binary
# section now. Note that the PACKAGE must be set correctly to actually get
# this IFR section pulled into the FFS file.
#
!IF ("$(HII_IFR_PACK_FILES)" != "")

$(DEST_DIR)\$(BASE_NAME)IfrBin.sec : $(HII_IFR_PACK_FILES)
  $(HIIPACK) create -novarpacks -p $(HII_IFR_PACK_FILES) -o $(DEST_DIR)\$(BASE_NAME)IfrBin.hii
  $(GENSECTION) -I $(DEST_DIR)\$(BASE_NAME)IfrBin.hii -O $(DEST_DIR)\$(BASE_NAME)IfrBin.sec -S EFI_SECTION_RAW

BIN_TARGETS = $(BIN_TARGETS) $(DEST_DIR)\$(BASE_NAME)IfrBin.sec
!ENDIF
!ENDIF

#
# Defines for standard intermediate files and build targets. For the source
# .efi file, take the one in the source directory if it exists. If there's not
# one there, look for one in the processor-specfic subdirectory.
#
TARGET_DPX        = $(DEST_DIR)\$(BASE_NAME).dpx
TARGET_UI         = $(DEST_DIR)\$(BASE_NAME).ui
TARGET_VER        = $(DEST_DIR)\$(BASE_NAME).ver
TARGET_MAP        = $(DEST_DIR)\$(BASE_NAME).map
TARGET_PDB        = $(EFI_SYMBOL_PATH)\$(BASE_NAME).pdb
TARGET_PE32       = $(DEST_DIR)\$(BASE_NAME).pe32
TARGET_DLL        = $(BIN_DIR)\$(BASE_NAME).dll
TARGET_SYM        = $(BIN_DIR)\$(BASE_NAME).sym
TARGET_EFI        = $(BIN_DIR)\$(BASE_NAME).efi
TARGET_TE         = $(BIN_DIR)\$(BASE_NAME).te
TARGET_TES        = $(DEST_DIR)\$(BASE_NAME).tes

#
# If building an application, then the target is a .app, not .dxe
#
!IF "$(COMPONENT_TYPE)" == "APPLICATION"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).app
!ELSEIF "$(COMPONENT_TYPE)" == "PE32_PEIM" || "$(COMPONENT_TYPE)" == "PEI_CORE"
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).pei
!ELSE
TARGET_FFS_FILE = $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).dxe
!ENDIF

!IF "$(BUILD_TYPE)" == "TE_PEIM_EFI" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "BS_DRIVER_EFI" || "$(BUILD_TYPE)" == "RT_DRIVER_EFI" || "$(BUILD_TYPE)" == "TE_PEIM_EFI" || "$(BUILD_TYPE)" == "APPLICATION_EFI" || "$(BUILD_TYPE)" == "PE32_PEIM_EFI" || "$(BUILD_TYPE)" == "NODPX_PE32_PEIM_EFI" || "$(BUILD_TYPE)" == "PEI_CORE_EFI"
BUILD_METHOD = FROM_EFI
!ELSE
BUILD_METHOD = FROM_OBJ
!ENDIF

!IF "$(OBJECTS)" == "" && "$(BUILD_METHOD)" == "FROM_OBJ" && "$(BUILD_TO_BINARY)" == "YES"
!ERROR Can't build objedt when the source files section is empty
!ENDIF


#
# LIB all the object files into our (local) target lib file. Put
# a dependency on the component's INF file in case it changes.
#
!IF "$(OBJECTS)" != ""
TARGET_LOCAL_LIB  = $(DEST_DIR)\$(BASE_NAME)Local.lib
BIN_TARGETS       = $(BIN_TARGETS) $(TARGET_LOCAL_LIB)
!IF "$(BUILD_FROM_BINARY)" == "YES" && "$(BUILD_METHOD)" == "FROM_OBJ"

!IF "$(SOURCE_OVERRIDE_PATH2)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSEIF "$(SOURCE_OVERRIDE_PATH)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSEIF EXIST ("$(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME)Local.lib")
TARGET_LOCAL_LIB = $(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME)Local.lib
!ELSE
!ERROR Pre-existing $(BASE_NAME)Local.lib file not found in $(SOURCE_DIR)\$(PROCESSOR) nor override folder
!ENDIF

!ELSE
$(TARGET_LOCAL_LIB) : $(OBJECTS)  $(INF_FILENAME)
  $(LIB) $(LIB_FLAGS) $(OBJECTS) /OUT:$@
!ENDIF
!IF "$(GENERATE_ANI)" == "YES"
OBJECTS = $(OBJECTS) $(DEST_DIR)\$(BASE_NAME)Animations.obj

$(DEST_DIR)\$(BASE_NAME)Animations.i : $(ANI_FILES) $(INF_FILENAME) $(GIF_FILES)
  type $(ANI_FILES) > $(DEST_DIR)\$(BASE_NAME)Animations.i

$(DEST_DIR)\$(BASE_NAME)AniDefs.h : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oh $(DEST_DIR)\$(BASE_NAME)AniDefs.h

$(DEST_DIR)\$(BASE_NAME)Animations.c : $(DEST_DIR)\$(BASE_NAME)Animations.i
  $(ANIGATHER) -i $(DEST_DIR)\$(BASE_NAME)Animations.i -path $(DEST_DIR) -bn $(BASE_NAME)Animations -oc $(DEST_DIR)\$(BASE_NAME)Animations.c

$(DEST_DIR)\$(BASE_NAME)Animations.obj : $(ANI_FILES) $(DEST_DIR)\$(BASE_NAME)Animations.c $(INF_FILENAME) $(ALL_DEPS)
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(BASE_NAME)Animations.c

LOCALIZE_TARGETS = $(DEST_DIR)\$(BASE_NAME)AniDefs.h $(LOCALIZE_TARGETS)
!ENDIF
!ENDIF


!IF ("$(BUILD_FROM_BINARY)" == "YES" || "$(SOURCE_FILES)" == "") && ("$(BUILD_METHOD)" == "FROM_EFI")
BIN_TARGETS =

!IF "$(SOURCE_OVERRIDE_PATH2)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH2)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_OVERRIDE_PATH2)\$(BASE_NAME).efi
!ELSEIF "$(SOURCE_OVERRIDE_PATH2)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_OVERRIDE_PATH2)\$(PROCESSOR)\$(BASE_NAME).efi
!ELSEIF "$(SOURCE_OVERRIDE_PATH)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_OVERRIDE_PATH)\$(BASE_NAME).efi
!ELSEIF "$(SOURCE_OVERRIDE_PATH)" != "" && EXIST ("$(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_OVERRIDE_PATH)\$(PROCESSOR)\$(BASE_NAME).efi
!ELSEIF EXIST ("$(SOURCE_DIR)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_DIR)\$(BASE_NAME).efi
!ELSEIF EXIST ("$(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME).efi")
BINARY_TARGET_EFI = $(SOURCE_DIR)\$(PROCESSOR)\$(BASE_NAME).efi
!ELSE
!ERROR Pre-existing $(BASE_NAME).efi file not found in $(SOURCE_DIR)\$(PROCESSOR) nor override folder
!ENDIF

COPY_EFI:
  copy /Y $(BINARY_TARGET_EFI) $(TARGET_EFI)


$(TARGET_EFI): COPY_EFI

!ELSE
#
# BUGBUG: This step is obsolete when a linker is released that supports EFI.
#
$(TARGET_EFI) : $(TARGET_DLL)
  $(FWIMAGE) -t 0 $(COMPONENT_TYPE) $(TARGET_DLL) $(TARGET_EFI)
!ENDIF

#
# generate info file and convert it to object file, then combine to dll file
#
!IF "$(BINARY_VERSION)" != ""
TARGET_INFO     = $(DEST_DIR)\$(BASE_NAME).info
TARGET_INFO_OBJ = $(DEST_DIR)\$(BASE_NAME).info.obj

$(TARGET_INFO_OBJ):
  echo 0>$(TARGET_INFO) >nul
  echo.$(BASE_NAME)>> $(TARGET_INFO)
  echo.$(BINARY_VERSION)>> $(TARGET_INFO)
  $(BIN2OBJ) -t $(PROCESSOR) -i $(TARGET_INFO) -o $(TARGET_INFO_OBJ) -s INFO
!ENDIF

#
# Link all objects and libs to create the executable
#
$(TARGET_DLL) : $(TARGET_LOCAL_LIB) $(LIBS) $(TARGET_INFO_OBJ)
  $(LINK) $(LINK_FLAGS_DLL) $(LIBS) $(TARGET_INFO_OBJ) /ENTRY:$(IMAGE_ENTRY_POINT) \
     $(TARGET_LOCAL_LIB) /OUT:$(TARGET_DLL) /MAP:$(TARGET_MAP) \
     /PDB:$(TARGET_PDB)
  $(SETSTAMP) $(TARGET_DLL) $(BUILD_DIR)\GenStamp.txt
!IF "$(EFI_GENERATE_SYM_FILE)" == "YES"
  if exist $(TARGET_PDB) $(PE2SYM) $(TARGET_PDB) $(TARGET_SYM)
!ENDIF

!IF "$(EFI_ZERO_DEBUG_DATA)" == "YES"
  $(ZERODEBUGDATA) $(TARGET_DLL)
!ENDIF


!IF  "$(BUILD_TYPE)" == "TE_PEIM_EFI" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "TE_PEIM_OBJ" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM_OBJ" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM_OBJ"
#
# Create our TE section from our TE file
#
$(TARGET_TES) : $(TARGET_TE) $(INF_FILENAME)
  $(GENSECTION) -I $(TARGET_TE) -O $(TARGET_TES) -S EFI_SECTION_TE

#
# Run GenTEImage on the built .efi file to create our TE file.
#
$(TARGET_TE) : $(TARGET_EFI) SET_BINARY
  $(GENTEIMAGE) -o $(TARGET_TE) $(TARGET_EFI)

!ELSE
#
# Take the .efi file and make a .pe32 file
#
$(TARGET_PE32) : $(TARGET_EFI) $(INF_FILENAME) SET_BINARY
  $(GENSECTION) -I $(TARGET_EFI) -O $(TARGET_PE32) -S EFI_SECTION_PE32
!ENDIF

#
# Create the user interface section
#
$(TARGET_UI) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_UI) -S EFI_SECTION_USER_INTERFACE -A "$(BASE_NAME)"

#
# Create the version section
#
!IF "$(BUILD_NUMBER)" != ""
!IF "$(VERSION_STRING)" != ""
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER) -A "$(VERSION_STRING)"
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  $(GENSECTION) -O $(TARGET_VER) -S EFI_SECTION_VERSION -V $(BUILD_NUMBER)
!ENDIF
!ELSE
$(TARGET_VER) : $(INF_FILENAME)
  echo.>$(TARGET_VER)
  type $(TARGET_VER) > $(TARGET_VER)
!ENDIF

#
# Makefile entries to create the dependency expression section.
# Use the DPX file from the source directory unless an override file
# was specified.
# If no DPX source file was specified, then create an empty file to
# be used.
#
!IF "$(DPX_SOURCE)" != ""
DPX_SOURCE_FILE = $(SOURCE_DIR)\$(DPX_SOURCE)
!ENDIF

!IF "$(DPX_SOURCE_OVERRIDE)" != ""
DPX_SOURCE_FILE = $(DPX_SOURCE_OVERRIDE)
!ENDIF

!IF "$(DPX_SOURCE_FILE)" != "" && "$(BUILD_TYPE)" != "NODPX_PE32_PEIM_EFI" && "$(BUILD_TYPE)" != "NODPX_TE_PEIM_EFI" && "$(BUILD_TYPE)" != "NODPX_PE32_PEIM_OBJ" && "$(BUILD_TYPE)" != "NODPX_TE_PEIM_OBJ"
!IF EXIST ($(DPX_SOURCE_FILE))
$(TARGET_DPX) : $(DPX_SOURCE_FILE) $(INF_FILENAME)
  $(CC) $(INC) $(C_FLAGS) /EP $(DPX_SOURCE_FILE) > $*.tmp1
  $(GENDEPEX) -I $*.tmp1 -O $*.tmp2
  $(GENSECTION) -I $*.tmp2 -O $@ -S $(DEPEX_TYPE)
  del $*.tmp1 > NUL
  del $*.tmp2 > NUL
!ELSE
!ERROR Dependency expression source file "$(DPX_SOURCE_FILE)" does not exist.
!ENDIF
!ELSE
$(TARGET_DPX) : $(INF_FILENAME)
  echo. > $(TARGET_DPX)
  type $(TARGET_DPX) > $(TARGET_DPX)
!ENDIF

#
# Build a FFS file from the sections and package
#
!IF "$(BUILD_TYPE)" == "TE_PEIM_EFI" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM_EFI" || "$(BUILD_TYPE)" == "TE_PEIM_OBJ" || "$(BUILD_TYPE)" == "DODPX_TE_PEIM_OBJ" || "$(BUILD_TYPE)" == "NODPX_TE_PEIM_OBJ"
$(TARGET_FFS_FILE) : $(TARGET_TES) $(TARGET_DPX) $(TARGET_UI) $(TARGET_VER)
!ELSE
$(TARGET_FFS_FILE) : $(TARGET_PE32) $(TARGET_DPX) $(TARGET_UI) $(TARGET_VER)
!ENDIF
  $(GENFFSFILE) -B $(DEST_DIR) -P1 $(PACKAGE_FILENAME) -V

SET_BINARY:
!IF "$(BUILD_TO_BINARY)" == "YES" && "$(SOURCE_FILES)" != ""
  if not exist $(SOURCE_DIR)\$(PROCESSOR) mkdir $(SOURCE_DIR)\$(PROCESSOR)
!IF "$(BUILD_METHOD)" == "FROM_OBJ"
  copy /y $(TARGET_LOCAL_LIB) $(SOURCE_DIR)\$(PROCESSOR)
!ELSE
!IF "$(BINARY_VERSION)" == ""
  $(SETGUID) -s $(TARGET_EFI) $(BUILD_DIR)\guid.txt > $(BUILD_DIR)\Log\guid\$(BASE_NAME)_$(PROCESSOR).txt
!ENDIF
  copy /y $(TARGET_EFI) $(SOURCE_DIR)\$(PROCESSOR)
!ENDIF
!ENDIF

DEL_SOURCE:
!IF "$(REMOVE_SOURCE_WHEN_BUILD_BINARY)" == "YES" && EXIST ("$(DEST_DIR)\$(BASE_NAME)RemoveFile.bat")
  $(DEST_DIR)\$(BASE_NAME)RemoveFile.bat
!ENDIF
all: $(LOCALIZE_TARGETS) $(BIN_TARGETS) $(TARGET_FFS_FILE) DEL_SOURCE
[=============================================================================]

#
# These commands are used to build EFI shell applications. That's all
# we need to do for these is link the objects and libraries together
# to create a .EFI file. See the example INF file in the shell\ls
# directory for a template INF file. You may also need to build the
# shell library below via shell\lib\shelllib.inf.
#
[=============================================================================]
[Build.Ia32.SHELLAPP,Build.Ipf.SHELLAPP,Build.x64.SHELLAPP]

TARGET_DLL = $(BIN_DIR)\$(BASE_NAME).dll
TARGET_EFI = $(BIN_DIR)\$(BASE_NAME).efi
TARGET_PDB = $(EFI_SYMBOL_PATH)\$(BASE_NAME).pdb
TARGET_MAP = $(BIN_DIR)\$(BASE_NAME).map
SUBSYSTEM  = EFI_APPLICATION

#
# Link all the object files and library files together to create our
# final target.
#
$(TARGET_DLL) : $(OBJECTS) $(LIBS)
  $(LINK) $(LINK_FLAGS_DLL) $(OBJECTS) $(LIBS) /ENTRY:$(IMAGE_ENTRY_POINT) \
     /OUT:$(TARGET_DLL) /MAP:$(TARGET_MAP) /PDB:$(TARGET_PDB) /SUBSYSTEM:EFI_APPLICATION

$(TARGET_EFI) : $(TARGET_DLL)
  $(FWIMAGE) -t 0 $(COMPONENT_TYPE) $(TARGET_DLL) $(TARGET_EFI)
  $(SETSTAMP) $(TARGET_EFI) $(BUILD_DIR)\GenStamp.txt
!IF "$(EFI_ZERO_DEBUG_DATA)" == "YES"
  $(ZERODEBUGDATA) $(TARGET_DLL)
!ENDIF

all: $(TARGET_EFI)

[=============================================================================]
[Compile.Ia32.Bin,Compile.x64.Bin,Compile.Ipf.Bin]
#
# We simply copy the 16 bit binary file from the source directory to the destination directory
#
$(DEST_DIR)\$(BASE_NAME).bin : $(SOURCE_DIR)\$(BASE_NAME).bin
  copy $** $@

[=============================================================================]
[Compile.Ia32.Cer,Compile.x64.Cer,Compile.Ipf.Cer]
#
# We simply copy the 16 bit binary file from the source directory to the destination directory
#
$(DEST_DIR)\$(BASE_NAME).cer : $(SOURCE_DIR)\$(BASE_NAME).cer
  copy $** $@

[=============================================================================]
[Compile.Ia32.Bmp,Compile.x64.Bmp,Compile.Ipf.Bmp]
#
# We simply copy the BMP file from the source directory to the destination directory and change the extension to bin.
# This is so that we can build BINARY types the same way, with the same default package, etc.
#
$(DEST_DIR)\$(BASE_NAME).bin : $(SOURCE_DIR)\$(BASE_NAME).bmp
  copy $** $@

[=============================================================================]
[Build.Ia32.BINARY,Build.Ipf.BINARY,Build.x64.BINARY]
#
#
# Use GenFfsFile to convert it to an FFS file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs : $(DEST_DIR)\$(BASE_NAME).bin
  $(GENSECTION) -I $(DEST_DIR)\$(BASE_NAME).bin -O $(DEST_DIR)\$(BASE_NAME).sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(BIN_DIR) -P1 $(DEST_DIR)\$(BASE_NAME).pkg -V

all: $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs

[=============================================================================]
[Build.Ia32.CERTIFICATE,Build.Ipf.CERTIFICATE,Build.x64.CERTIFICATE]
#
# Use GenFfsFile to convert it to an FFS file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs : $(DEST_DIR)\$(BASE_NAME).cer
  $(GENSECTION) -I $(DEST_DIR)\$(BASE_NAME).cer -O $(DEST_DIR)\$(BASE_NAME).sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(BIN_DIR) -P1 $(DEST_DIR)\$(BASE_NAME).pkg -V

all: $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME).ffs

[=============================================================================]
[Build.Ia32.LOGO,Build.Ipf.LOGO,Build.x64.LOGO]
#
# We simply copy the logo image file from the source directory to the build directory
#
$(BIN_DIR)\$(LOGO_NAME) : $(SOURCE_DIR)\$(LOGO_NAME)
  copy $(SOURCE_DIR)\$(LOGO_NAME) $(BIN_DIR)\$(LOGO_NAME) /y
  if exist $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME)$(FFS_EXT) del $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME)$(FFS_EXT)

#
# Use GenFFSFile to convert it to an FFS file
#
$(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME)$(FFS_EXT) : $(BIN_DIR)\$(LOGO_NAME)
  $(GENSECTION) -I $(BIN_DIR)\$(LOGO_NAME) -O $(BIN_DIR)\$(BASE_NAME).sec -S EFI_SECTION_RAW
  $(GENFFSFILE) -B $(BIN_DIR) -P1 $(DEST_DIR)\$(BASE_NAME).pkg -V

all: $(BIN_DIR)\$(FILE_GUID)-$(BASE_NAME)$(FFS_EXT)

[=============================================================================]
[Compile.Ia32.gif,Compile.x64.gif,Compile.Ipf.gif]
#
# We simply copy the gif file from the source directory to the destination directory
#
$(DEST_DIR)\$(SOURCE_BASE_NAME).gif : $(SOURCE_FILE_NAME)
  copy $** $@

GIF_FILES         = $(GIF_FILES) $(DEST_DIR)\$(SOURCE_BASE_NAME).gif
[=============================================================================]
#
# These are commands to compile animation .ani files.
#
[Compile.Ia32.Ani,Compile.Ipf.Ani,Compile.Ebc.Ani,Compile.x64.Ani]
#
# Emit an error message if the file's base name is the same as the
# component base name. This causes build issues.
#
!IF "$(FILE)" == "$(BASE_NAME)"
!ERROR Component animtion file name cannot be the same as the component BASE_NAME.
!ENDIF

#!IF EXIST($(DEP_FILE))
#!INCLUDE $(DEP_FILE)
#!ENDIF

GENERATE_ANI      = YES
ANI_FILES         = $(ANI_FILES) $(SOURCE_FILE_NAME)
[=============================================================================]

#
# These are commands to compile unicode .uni files.
# Emit an error message if the file's base name is the same as the
# component base name. This causes build issues.
#
[Compile.Ia32.Uni,Compile.Ipf.Uni,Compile.Ebc.Uni,Compile.x64.Uni]

!IF "$(FILE)" == "$(BASE_NAME)"
!ERROR Component Unicode string file name cannot be the same as the component BASE_NAME.
!ENDIF

!IF ("$(NO_MAKEDEPS)" == "")

DEP_FILE = $(DEST_DIR)\$(FILE)Uni.dep

!IF EXIST($(DEP_FILE))
!INCLUDE $(DEP_FILE)
!ENDIF

$(DEST_DIR)\$(FILE).sdb : $(SOURCE_FILE_NAME)
  $(STRGATHER) -parse -newdb -db $(DEST_DIR)\$(FILE).sdb -dep $(DEP_FILE) $(INC) $(SOURCE_FILE_NAME)

!ELSE

$(DEST_DIR)\$(FILE).sdb : $(SOURCE_FILE_NAME)
  $(STRGATHER) -parse -newdb -db $(DEST_DIR)\$(FILE).sdb $(INC) $(SOURCE_FILE_NAME)

!ENDIF

SDB_FILES       = $(SDB_FILES) $(DEST_DIR)\$(FILE).sdb
STRGATHER_FLAGS = $(STRGATHER_FLAGS) -db $(DEST_DIR)\$(FILE).sdb
LOCALIZE        = YES

[=============================================================================]
[Compile.Ia32.hfr,Compile.Ipf.hfr,Compile.Ebc.hfr,Compile.x64.hfr]

[=============================================================================]
[Compile.Ia32.Vfr,Compile.Ipf.Vfr,Compile.Ebc.Vfr,Compile.x64.Vfr]

HII_PACK_FILES  = $(HII_PACK_FILES) $(DEST_DIR)\$(FILE).hpk

#
# Add a dummy command for building the HII pack file. In reality, it's built
# below, but the C_FLAGS macro reference the target as $@, so you can't specify
# the obj and hpk files as dual targets of the same command.
#
$(DEST_DIR)\$(FILE).hpk : $(DEST_DIR)\$(FILE).obj
  @echo.

$(DEST_DIR)\$(FILE).obj : $(SOURCE_FILE_NAME) $(INC_DEPS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h
  $(VFRCOMPILE) $(VFRCOMPILE_FLAGS) $(INC) -ibin -od $(DEST_DIR)\$(SOURCE_RELATIVE_PATH) \
    -l $(VFR_FLAGS) $(SOURCE_FILE_NAME)
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(FILE).c

[=============================================================================]
#
# Commands for building IFR as uncompressed binary into the FFS file. To
# use it, set COMPILE_SELECT=.vfr=Ifr_Bin for the component in the DSC file.
#
[Compile.Ia32.Ifr_Bin,Compile.Ipf.Ifr_Bin;Compile.Ebc.Ifr_Bin,Compile.x64.Ifr_Bin]

HII_PACK_FILES  = $(HII_PACK_FILES) $(DEST_DIR)\$(FILE).hpk

#
# Add a dummy command for building the HII pack file. In reality, it's built
# below, but the C_FLAGS macro reference the target as $@, so you can't specify
# the obj and hpk files as dual targets of the same command.
#
$(DEST_DIR)\$(FILE).hpk : $(DEST_DIR)\$(FILE).obj
  @echo.

$(DEST_DIR)\$(FILE).obj : $(SOURCE_FILE_NAME) $(INC_DEPS) $(DEST_DIR)\$(BASE_NAME)StrDefs.h
  $(VFRCOMPILE) $(VFRCOMPILE_FLAGS) $(INC) -ibin -od $(DEST_DIR)\$(SOURCE_RELATIVE_PATH) \
    -l $(VFR_FLAGS) $(SOURCE_FILE_NAME)
  $(CC) $(C_FLAGS) $(DEST_DIR)\$(FILE).c

#
# Add to the variable that contains the list of VFR binary files we're going
# to merge together at the end of the build.
#
HII_IFR_PACK_FILES = $(HII_IFR_PACK_FILES) $(DEST_DIR)\$(FILE).hpk

[=============================================================================]
[Compile.Ia32.Fv,Compile.Ipf.Fv,Compile.x64.Fv]
#
# Run GenSection on the firmware volume image.
#
$(DEST_DIR)\$(SOURCE_FV)Fv.sec : $(FV_DIR)\$(SOURCE_FV).fv
  copy $(FV_DIR)\$(SOURCE_FV).fv $(DEST_DIR)\$(SOURCE_FV).fv /y
  $(GENSECTION) -I $(DEST_DIR)\$(SOURCE_FV).fv -O $(DEST_DIR)\$(SOURCE_FV)Fv.sec -S EFI_SECTION_FIRMWARE_VOLUME_IMAGE

[=============================================================================]
#
# These are the package descriptions. They are tagged as
# [Package.$(COMPONENT_TYPE).$(PACKAGE)], where COMPONENT_TYPE is typically
# defined in the component INF file, and PACKAGE is typically specified
# in the components section in the main DSC file.
#

[=============================================================================]
[Package.APPLICATION.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_APPLICATION
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (Dummy) {
    Tool (
      $(OEMTOOLPATH)\GenCRC32Section
      ARGS= -i $(DEST_DIR)\$(BASE_NAME).pe32
               $(DEST_DIR)\$(BASE_NAME).ui
               $(DEST_DIR)\$(BASE_NAME).ver
            -o $(DEST_DIR)\$(BASE_NAME).crc32
      OUTPUT = $(DEST_DIR)\$(BASE_NAME).crc32
    )
  }
}

[=============================================================================]
[Package.FILE.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  $(BASE_NAME).sec
}
[=============================================================================]
[Package.Apriori.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  $(DEST_DIR)\$(BASE_NAME).sec
}

[=============================================================================]
[Package.Logo.Logo,Package.Logo.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (Dummy) {
    Tool ( $(OEMTOOLPATH)\GenCRC32Section
      ARGS = -i $(BIN_DIR)\$(BASE_NAME).sec
             -o $(BIN_DIR)\$(BASE_NAME).crc32
      OUTPUT = $(BIN_DIR)\$(BASE_NAME).crc32
    )
  }
}

[=============================================================================]
[Package.RAWFILE.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_RAW
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  $(DEST_DIR)\$(BASE_NAME).FV
}

[=============================================================================]
[Package.Legacy16.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (Dummy) {
    Tool ( $(OEMTOOLPATH)\GenCRC32Section
      ARGS = -i $(BIN_DIR)\$(BASE_NAME).sec
             -o $(BIN_DIR)\$(BASE_NAME).crc32
      OUTPUT = $(BIN_DIR)\$(BASE_NAME).crc32
    )
  }
}

[=============================================================================]
[Package.BINARY.Default]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (Dummy) {
    Tool ( $(OEMTOOLPATH)\GenCRC32Section
      ARGS = -i $(DEST_DIR)\$(BASE_NAME).sec
             -o $(DEST_DIR)\$(BASE_NAME).crc32
      OUTPUT = $(DEST_DIR)\$(BASE_NAME).crc32
    )
  }
}

[=============================================================================]
#
# Package definition for TE files
#
[Package.PE32_PEIM.TE_PEIM]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_PEIM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  $(BASE_NAME).dpx
  $(BASE_NAME).tes
  $(BASE_NAME).ui
  $(BASE_NAME).ver
}

[=============================================================================]
[Package.Config.Config]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_RAW
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  $(BASE_NAME).ini
}

[=============================================================================]
#
# Package definition to put the IFR data in a separate section in the
# FFS file.
#
[Package.BS_DRIVER.Ifr_Bin]
PACKAGE.INF
\[.]
BASE_NAME                   = $(BASE_NAME)
FFS_FILEGUID                = $(FILE_GUID)
FFS_FILETYPE                = EFI_FV_FILETYPE_DRIVER
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (Dummy) {
    Tool (
      $(OEMTOOLPATH)\GenCRC32Section
      ARGS= -i $(DEST_DIR)\$(BASE_NAME).dpx
               $(DEST_DIR)\$(BASE_NAME).pe32
               $(DEST_DIR)\$(BASE_NAME).ui
               $(DEST_DIR)\$(BASE_NAME).ver
               $(DEST_DIR)\$(BASE_NAME)IfrBin.sec
            -o $(DEST_DIR)\$(BASE_NAME).crc32
      OUTPUT = $(DEST_DIR)\$(BASE_NAME).crc32
    )
  }
}
[=============================================================================]
#
# These are the libraries that will be built by the master makefile
#
[=============================================================================]
[Libraries]
DEFINE PLATFORM=$(PLATFORM)

#
# SEC libraries
#
# None required
!include "$(EFI_SOURCE)\Platform\Insyde\Common\Include\CommonPlatformLib.dsc"
!include "$(EFI_SOURCE)\$(COMMON_CHIPSET_DIR)\Include\CommonChipsetLib.dsc"
DEFINE PROCESSOR=$(BUILD_TARGET_PROCESSOR)
#
# Libraries common to PEI and DXE
#
Guid\GuidLib.inf
Edk\Foundation\Guid\EdkGuidLib.inf
Edk\Foundation\Framework\Guid\EdkFrameworkGuidLib.inf
Edk\Foundation\Efi\Guid\EfiGuidLib.inf
Platform\Generic\Guid\GenericGuidLib.inf
Edk\Foundation\Library\EfiCommonLib\EfiCommonLib.inf
Chipset\Guid\ChipsetGuidLib.inf
Library\IoAccess\IoAccess.inf
Library\BvdtLib\BvdtLib.inf

#
# PEI libraries
#
Ppi\PpiLib.inf
Edk\Foundation\Ppi\EdkPpiLib.inf
Edk\Foundation\Framework\Ppi\EdkFrameworkPpiLib.inf
Edk\Foundation\Library\Pei\PeiLib\PeiLib.inf
Edk\Foundation\Library\Pei\Hob\PeiHobLib.inf
#Edk\Foundation\Library\CustomizedDecompress\CustomizedDecompress.inf
$(CUSTOMIZEDDECOMPRESS_INF)
H2ODebug\Ppi\H2ODDTPpiLib.inf
Chipset\Ppi\ChipsetPpiLib.inf


#
# DXE libraries
#
Edk\Foundation\Protocol\EdkProtocolLib.inf
Edk\Foundation\Framework\Protocol\EdkFrameworkProtocolLib.inf
Edk\Foundation\Efi\Protocol\EfiProtocolLib.inf
Edk\Foundation\Core\Dxe\ArchProtocol\ArchProtocolLib.inf
Protocol\ProtocolLib.inf
Platform\Generic\Protocol\GenericProtocolLib.inf
Platform\Generic\Lib\Port80MappingLib\PlatformPort80.inf
Edk\Foundation\Library\Dxe\Print\PrintLib.inf
Edk\Foundation\Library\Dxe\EfiDriverLib\EfiDriverLib.inf
Edk\Foundation\Library\RuntimeDxe\EfiRuntimeLib\EfiRuntimeLib.inf
Library\RuntimeDxe\EfiMgmtModeRuntimeLib\EfiMgmtModeRuntimeLib.inf
Library\Dxe\OemGraphics\Graphics.inf
Edk\Foundation\Library\Dxe\EfiIfrSupportLib\EfiIfrSupportLib.inf
Library\Dxe\UefiEfiIfrSupportLib\UefiEfiIfrSupportLib.inf
Library\BaseCryptLib\BaseCryptLib.inf
Application\Shell\Library\EfiShellLib.inf
Edk\Foundation\Library\Dxe\EfiScriptLib\EfiScriptLib.inf
Library\Dxe\EfiRegTableLib\EfiRegTableLib.inf
Edk\Sample\Bus\Usb\UsbLib\Dxe\UsbDxeLib.inf
Library\CmosLib\CmosLib.inf
Platform\Generic\RuntimeDxe\StatusCode\Lib\BsSerialStatusCode\BsSerialStatusCode.inf
$(EC_LIB_PATH)\EcLib.inf
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\OemServices\ROM\FWH_LPC\FwhLpcLib.inf
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\OemServices\ROM\SpiDevice\SpiDeviceLib.inf
$(DEMOBOARD_FAMILY)\OemServices\Dxe\OemServicesExLib\OemServicesExLib.inf
$(SPI_LIB_PATH)\SpiLib.inf
Chipset\Library\$(DEMOBOARD_NAME)\ChipsetLib.inf
Library\RuntimeDxe\FdSupportLib\FdSupportLib.inf
Platform\Generic\MonoStatusCode\Library\Pei\Port80StatusCode\Port80StatusCode.inf
Platform\Generic\MonoStatusCode\Library\Pei\CmosStatusCode\CmosStatusCode.inf
Platform\Generic\RuntimeDxe\StatusCode\Lib\RtPort80StatusCode\RtPort80StatusCode.inf
Platform\Generic\RuntimeDxe\StatusCode\Lib\RtCmosStatusCode\RtCmosStatusCode.inf
Edk\Sample\Platform\Generic\RuntimeDxe\StatusCode\Lib\BsDataHubStatusCode\BsDataHubStatusCode.inf
Platform\Generic\RuntimeDxe\StatusCode\Lib\BsUsbStatusCode\BsUsbStatusCode.inf
Platform\Generic\RuntimeDxe\StatusCode\Lib\RtPlatformStatusCode\RtPlatformStatusCode.inf
Library\Thunk16\Thunk16Lib.inf
!include "$(EFI_SOURCE)\Platform\Generic\EdkIIGlueLibAll.dsc"
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\Protocol\PlatformProtocolLib.inf
H2ODebug\Protocol\H2ODDTProtocolLib.inf
Library\Dxe\KscLib\KscLib.inf
Chipset\Protocol\ChipsetProtocolLib.inf
Universal\PpisNeededByDxeCore\Library\PreDxeCmosReportStatusLib\PreDxeCmosReportStatusLib.inf
Library\Dxe\ConsoleLib\ConsoleLib.inf
Library\Dxe\HidDescriptorLib\HidDescriptorLib.inf

#
# PEI and DXE combo libraries
#
Library\EfiCombinationLib\EfiCombinationLib.inf
Edk\Sample\Universal\Network\Library\NetLib.inf
Edk\Foundation\Library\CompilerStub\CompilerStubLib.inf

#
# BDS libraries
#
Platform\Generic\Lib\GenericBdsLib\GenericBdsLib.inf
#
# SetupUtility Library
#
!IF "$(EFI_SPECIFICATION_VERSION)" >= "0x0002000A"
Library\Dxe\UefiSetupUtilityLib\UefiSetupUtilityLib.inf SOURCE_OVERRIDE_PATH = $(EFI_SOURCE)\$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\ModFW\Library\Dxe\UefiSetupUtilityLib
!ELSE
!IF "$(Q2LSERVICE_SUPPORT)" == "NO"
Library\Dxe\SetupUtilityLib\SetupUtilityLib.inf
!ELSE
Library\Dxe\SetupUtilityLib\SetupUtilityLib.inf  SOURCE_OVERRIDE_PATH = $(EFI_SOURCE)\$(DEMOBOARD_FAMILY)\Common\Samples\Q2LOem\Library\Dxe\SetupUtilityLib
!ENDIF
!ENDIF
Library\Dxe\SensitiveVariableLib\SensitiveVariableLib.inf
#
# Print/Graphics Library consume SetupBrowser Print Protocol
#
Edk\Foundation\Library\Dxe\PrintLite\PrintLib.inf
Library\Dxe\OemGraphicsLite\Graphics.inf

#
# SMM libraries
#
Library\Smm\EfiSmmDriverLib.inf
Library\Smm\SmmIo\SmmIoLib.inf
Library\Smm\SmmKscLib\SmmKscLib.inf
$(DEMOBOARD_FAMILY)\OemServices\Smm\OemServicesExLib\OemServicesExLib.inf
Platform\Generic\Lib\IhisiLib\IhisiLib.inf
Edk\Foundation\Library\Dxe\Hob\HobLib.inf
Library\VariableSupportLib\VariableSupportLib.inf

#[-start-130517-IB07010131-add]#
!IF "$(GRAPHICS_SETUP_SUPPORT)" == "YES"
Library\TrueTypeLib\TrueTypeLib.inf
Library\StdLib\StdLib.inf
Universal\UserInterface\UefiSetupBrowser\Dxe\Microwindows\MicrowindowsLib.inf
!ENDIF
#[-end-130517-IB07010131-add]#

#
# EBC libraries required by drivers
#
DEFINE PROCESSOR=IA32
$(DEMOBOARD_FAMILY)\OemServices\Pei\OemServicesExLib\OemServicesExLib.inf
Platform\Generic\Ppi\GenericPpiLib.inf
Platform\Generic\MonoStatusCode\Library\Pei\SimpleCpuIo\SimpleCpuIo.inf
Platform\Generic\MonoStatusCode\Library\Pei\SerialStatusCode\SerialStatusCode.inf
Platform\Generic\MonoStatusCode\Library\Pei\UsbStatusCode\UsbStatusCode.inf
Chipset\PcatCompatible\SingleSegmentPciCfg\Pei\PciCfgLib.inf
!IF "$(EFI_BUILD_TARGET_X64)" == "YES"
Chipset\Guid\ChipsetGuidLib.inf
Guid\GuidLib.inf
Edk\Foundation\Guid\EdkGuidLib.inf
Edk\Foundation\Framework\Guid\EdkFrameworkGuidLib.inf
Edk\Foundation\Efi\Guid\EfiGuidLib.inf
Edk\Foundation\Library\EfiCommonLib\EfiCommonLib.inf
Ppi\PpiLib.inf
Chipset\Ppi\ChipsetPpiLib.inf
Edk\Foundation\Ppi\EdkPpiLib.inf
Edk\Foundation\Framework\Ppi\EdkFrameworkPpiLib.inf
Edk\Foundation\Library\Pei\Hob\PeiHobLib.inf
$(CUSTOMIZEDDECOMPRESS_INF)
Edk\Foundation\Library\Pei\PeiLib\PeiLib.inf
Edk\Foundation\Protocol\EdkProtocolLib.inf
Edk\Foundation\Framework\Protocol\EdkFrameworkProtocolLib.inf
Edk\Foundation\Efi\Protocol\EfiProtocolLib.inf
Edk\Foundation\Core\Dxe\ArchProtocol\ArchProtocolLib.inf
!ENDIF
Protocol\ProtocolLib.inf
Library\PeiCryptLib\PeiBaseCryptLib.inf

!IF "$(EFI_BUILD_TARGET_X64)" == "YES"
Edk\Foundation\Library\Dxe\Print\PrintLib.inf
Edk\Foundation\Library\Dxe\EfiDriverLib\EfiDriverLib.inf
Edk\Foundation\Library\RuntimeDxe\EfiRuntimeLib\EfiRuntimeLib.inf
Edk\Foundation\Library\Dxe\EfiScriptLib\EfiScriptLib.inf
!ENDIF
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\Protocol\PlatformProtocolLib.inf
!IF "$(EFI_BUILD_TARGET_X64)" == "YES"
Platform\Generic\Lib\Port80MappingLib\PlatformPort80.inf
Platform\Generic\Guid\GenericGuidLib.inf
Library\IoAccess\IoAccess.inf
Library\CmosLib\CmosLib.inf

Platform\Generic\MonoStatusCode\Library\Pei\Port80StatusCode\Port80StatusCode.inf
Platform\Generic\MonoStatusCode\Library\Pei\CmosStatusCode\CmosStatusCode.inf
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\OemServices\ROM\FWH_LPC\FwhLpcLib.inf
$(DEMOBOARD_FAMILY)\$(DEMOBOARD_NAME)\OemServices\ROM\SpiDevice\SpiDeviceLib.inf
$(EC_LIB_PATH)\EcLib.inf
$(SPI_LIB_PATH)\SpiLib.inf
Library\EfiCombinationLib\EfiCombinationLib.inf
Chipset\Library\$(DEMOBOARD_NAME)\ChipsetLib.inf
Library\RuntimeDxe\FdSupportLib\FdSupportLib.inf
Edk\Foundation\Library\Dxe\Hob\HobLib.inf
#
# 16Bit Thunk Library
#
Library\Thunk16\Thunk16Lib.inf
Edk\Foundation\Library\CompilerStub\CompilerStubLib.inf
H2ODebug\Ppi\H2ODDTPpiLib.inf
Library\VariableSupportLib\VariableSupportLib.inf
!ENDIF
#
# EdkIIGlueLib Glue Libraries for PEI usage (32bit)
#
!include "$(EFI_SOURCE)\Platform\Generic\EdkIIGlueLib32.dsc"
[=============================================================================]
[=============================================================================]
