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
##   CallSmmSecureBoot.asm
##
## Abstract:
##
##   64 bit Sent SMI to call Secure Boot function
##

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
