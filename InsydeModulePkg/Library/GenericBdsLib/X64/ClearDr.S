##******************************************************************************
##* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
##*
##* You may not reproduce, distribute, publish, display, perform, modify, adapt,
##* transmit, broadcast, present, recite, release, license or otherwise exploit
##* any part of this publication in any form, by any means, without the prior
##* written permission of Insyde Software Corporation.
##*
##******************************************************************************

#------------------------------------------------------------------------------
#  VOID
#  ClearDebugRegisters (
#    VOID
#    )
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(ClearDebugRegisters);
ASM_PFX(ClearDebugRegisters):
    pushq   %rax
    xorq    %rax, %rax
    movq    %rax, %dr0
    movq    %rax, %dr1
    popq    %rax
    ret
