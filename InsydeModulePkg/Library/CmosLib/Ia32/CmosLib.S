## @file
# Supports IA32 CPU operation for CMOS library
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
#------------------------------------------------------------------------------
#  VOID
#    CmosDisableInterrupt (
#    )
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(CmosDisableInterrupt);
ASM_PFX(CmosDisableInterrupt):
    cli
    ret

#------------------------------------------------------------------------------
#  VOID
#  CmosEableInterrupt (
#    )
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(CmosEableInterrupt);
ASM_PFX(CmosEableInterrupt):
    sti
    ret

ASM_GLOBAL ASM_PFX(CmosGetCpuFlags);
ASM_PFX(CmosGetCpuFlags):
    pushfl
    popl    %eax
    ret
