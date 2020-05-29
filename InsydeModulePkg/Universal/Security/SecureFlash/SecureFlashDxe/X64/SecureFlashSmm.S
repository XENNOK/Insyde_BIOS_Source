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
##   SecureFlashSmm.S
##
## Abstract:
##
##   64 bit Sent SMI to call IHISI function 48h for secure flash feature
##

#------------------------------------------------------------------------------
#  UINT8
#  SecureFlashReadyToBootSmi (
#    IN     UINT32	          Command,  // rcx, not used
#    IN     UINT16                SmiPort   // rdx
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SecureFlashReadyToBootSmi);
ASM_PFX(SecureFlashReadyToBootSmi):

                pushq   %rbx
		movl    $0x2448324f, %ebx
		movw	$0x48EF, %ax
		outb	%al, %dx
                popq    %rbx

          #AL Fun ret state
                ret
