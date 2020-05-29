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
    pushl  %eax
    xorl   %eax, %eax
    movl   %eax, %dr0
    movl   %eax, %dr1
    popl    eax
    ret

