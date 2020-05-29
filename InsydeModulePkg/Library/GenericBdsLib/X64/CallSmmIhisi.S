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
##   CallSmmIhisi.S
##
## Abstract:
##
##   64 bit Sent SMI to call IHISI flash ROM part
##


#------------------------------------------------------------------------------
#  UINT8
#  FlashPartInfo (
#    IN     UINT8            *PartInfo,       // rcx
#    IN     UINT8            *PartBlock,      // rdx
#    IN     UINT8            FlashTypeSelect, // r8
#    IN     UINT16           SmiPort          // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashPartInfo);
ASM_PFX(FlashPartInfo):
        pushq   %rbx
        pushq   %rdi
        pushq	%rsi
        pushq   %r8

        movb    %r8b, %cl
        movq    %rcx, %rdi
        movq    %rdx, %rsi
        movl    $0x2448324f, %ebx
        movw    $0x13ef, %ax
        movq    %r9, %rdx
        outb    %al, %dx

        popq    %r8
        popq    %rsi
        popq    %rdi
        popq    %rbx
        ret

#------------------------------------------------------------------------------
#  UINT8
#  FlashWrite (
#    IN UINT8	                *Buffer,      // rcx
#    IN UINT32	              FlashSize,    // rdx
#  	 IN UINT32	              FlashAddress  // r8
#  	 IN UINT16	              SmiPort       // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashWrite);
ASM_PFX(FlashWrite):
        pushq   %rbx
        pushq   %rdi
        pushq   %rsi
        pushq   %r8

        movq    %rcx, %rsi
        movq    %rdx, %rcx
        movq    %r8, %rdi
        movl    $0x2448324f, %ebx
        movw    $0x15ef, %ax
        movq    %r9, %rdx
        outb    %al, %dx


        popq    %r8
        popq    %rsi
        popq    %rdi
	popq    %rbx
        ret

#------------------------------------------------------------------------------
#  UINT8
#  FlashComplete (
#    IN     UINT16	              Command,  // rcx
#    IN     UINT16                SmiPort   // rdx
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(FlashComplete);
ASM_PFX(FlashComplete):
        pushq   %rbx
        pushq   %rdi
        pushq   %rsi
        pushq   %r8

        movl    $0x2448324f, %ebx
        movw    $0x16ef, %ax
        outb    %al, %dx

        popq    %r8
        popq    %rsi
        popq    %rdi
	popq    %rbx
        ret
