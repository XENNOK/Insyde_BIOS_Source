##******************************************************************************
##* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
##*
##* You may not reproduce, distribute, publish, display, perform, modify, adapt,
##* transmit, broadcast, present, recite, release, license or otherwise exploit
##* any part of this publication in any form, by any means, without the prior
##* written permission of Insyde Software Corporation.
##*
##******************************************************************************
##
## Module Name:
##
##   CallSmmSensitiveVariable.S
##
## Abstract:
##
##   32 bit Sent SMI to call update sensitive variable function
##

#------------------------------------------------------------------------------
#  UINT8
#  SmmSensitiveVariableCall (
#    IN     UINT8            *InPutBuff,       // rcx
#    IN     UINTN            DataSize,         // rdx
#    IN     UINT8            SubFunNum,        // r8
#    IN     UINT16           SmiPort           // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SmmSensitiveVariableCall);
ASM_PFX(SmmSensitiveVariableCall):
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %edi
        pushl   %esi
        popl    %esi
        popl    %edi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp

        movl    8(%ebp), %esi
        movl    12(%ebp), %ebx
        movl    16(%ebp), %eax
        movb    %al, %ah
        movb    $0xec, %al
        movl    20(%ebp), %edx
        outb    %al, %dx

        #AL Fun ret state
        popl    %esi
        popl    %edi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp

        ret
