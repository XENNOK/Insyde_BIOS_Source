##******************************************************************************
##* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
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
##   CallSmmIhisi.asm
##
## Abstract:
##
##   32 bit Sent SMI to call IHISI flash ROM part
##


#------------------------------------------------------------------------------
#  UINT8
#  FlashPartInfo (
#    IN     UINT8            *PartInfo,
#    IN     UINT8            *PartBlock,
#    IN     UINT8            FlashTypeSelect,
#    IN     UINT16           SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashPartInfo);
ASM_PFX(FlashPartInfo):
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %edi
        pushl   %esi

        movl    $0x2448324f, %ebx        
        movl    8(%ebp), %edi
        movl    12(%ebp), %esi
        movl    16(%ebp), %ecx
        movl    20(%ebp), %edx
        movl    $0x13ef, %eax
        outb    %al, %dx

        #AL Fun ret state
        popl    %esi
        popl    %edi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret

#------------------------------------------------------------------------------
#  UINT8
#  FlashWrite (
#    IN UINT8	                *Buffer,
#    IN UINT32	              FlashSize,
#  	 IN UINT32	              FlashAddress,
#  	 IN UINT16	              SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashWrite);
ASM_PFX(FlashWrite):
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %edi
        pushl   %esi

        movl    $0x2448324f, %ebx        
        movl    8(%ebp), %esi
        movl    12(%ebp), %ecx
        movl    16(%ebp), %edi
        movl    20(%ebp), %edx
        movl    $0x15ef, %eax
        outb    %al, %dx

        #AL Fun ret state
        popl    %esi
        popl    %edi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret
#------------------------------------------------------------------------------
#  UINT8
#  FlashComplete (
#    IN     UINT16	          Command,
#    IN     UINT16                SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashComplete);
ASM_PFX(FlashComplete):
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %edi
        pushl   %esi

        movl    $0x2448324f, %ebx        
        movl    8(%ebp), %ecx
        movl    12(%ebp), %edx
        movl    $0x16ef, %eax
        outb    %al, %dx

        popl    %esi
        popl    %edi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret
