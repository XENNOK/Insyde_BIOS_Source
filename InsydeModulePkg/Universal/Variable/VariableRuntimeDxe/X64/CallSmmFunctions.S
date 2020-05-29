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
##   CallSmmIhisi.asm
##
## Abstract:
##
##   64 bit Sent SMI to call IHISI VATS function
##

#------------------------------------------------------------------------------
#  UINT8
#  IhisiVatsCall (
#    IN     UINT8            *InPutBuff,       // rcx
#    IN     UINT8            *OutPutBuff,      // rdx
#    IN     UINT8            SubFunNum,        // r8
#    IN     UINT16           SmiPort           // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(IhisiVatsCall);
ASM_PFX(IhisiVatsCall):
          pushq  %rbx
          pushq  %rdi
          pushq  %rsi
          pushq  %r8

          movq	%rcx, %rsi
          movq  %rdx, %rdi
          movl  $0x2448324f, %ebx
          movb  %r8b, %al
          movb  %al, %ah
          movb  $0xef, %al
          movq  %r9, %rdx
          outb  %al, %dx

          #AL Fun ret state

          popq  %r8
          popq  %rsi
          popq  %rdi
          popq  %rbx
          ret

#------------------------------------------------------------------------------
#  UINT8
#  SmmSecureBootCall (
#    IN     UINT8            *InPutBuff,       // rcx
#    IN     UINTN            DataSize,         // rdx
#    IN     UINT8            SubFunNum,        // r8
#    IN     UINT16           SmiPort           // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SmmSecureBootCall);
ASM_PFX(SmmSecureBootCall):
          pushq  %rbx
          pushq  %rdi
          pushq  %rsi
          pushq  %r8

          movq   %rcx, %rsi
          movq   %rdx, %rbx
          movb   %r8b, %al
          movb   %al,  %ah
          movb   $0xec, %al
          movq   %r9, %rdx
          outb   %al, %dx

          #AL Fun ret state

          popq   %r8
          popq   %rsi
          popq   %rdi
          popq   %rbx
          ret
