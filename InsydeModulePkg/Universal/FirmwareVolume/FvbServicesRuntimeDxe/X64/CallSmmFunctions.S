#/** @file
#  64 bit Sent SMI to call registered SMM callback in this driver.
#
##******************************************************************************
##* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
##*
##* You may not reproduce, distribute, publish, display, perform, modify, adapt,
##* transmit, broadcast, present, recite, release, license or otherwise exploit
##* any part of this publication in any form, by any means, without the prior
##* written permission of Insyde Software Corporation.
##*
##******************************************************************************
#*/

#------------------------------------------------------------------------------
#  UINT8
#  SmmFvbAccessCall (
#    IN     UINT8            *InPutBuff,       // rcx
#    IN     UINTN            DataSize,         // rdx
#    IN     UINT8            SubFunNum,        // r8
#    IN     UINT16           SmiPort           // r9
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SmmFvbAccessCall);
ASM_PFX(SmmFvbAccessCall):
          pushq  %rbx
          pushq  %rdi
          pushq  %rsi
          pushq  %r8

          movq   %rcx, %rsi
          movq   %rdx, %rbx
          movb   %r8b, %al
          movb   %al,  %ah
          movb   $0x16, %al
          movq   %r9, %rdx
          outb   %al, %dx

          #AL Fun ret state

          popq   %r8
          popq   %rsi
          popq   %rdi
          popq   %rbx
          ret
