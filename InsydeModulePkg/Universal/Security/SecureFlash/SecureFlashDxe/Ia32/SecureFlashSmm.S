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
##   32 bit Sent SMI to call IHISI function 48h for secure flash feature
##

#------------------------------------------------------------------------------
#  UINT8
#  SecureFlashReadyToBootSmi (
#    IN     UINT32	          Command, 
#    IN     UINT16                SmiPort  
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SecureFlashReadyToBootSmi);
ASM_PFX(SecureFlashReadyToBootSmi):
                pushl   %ebp
                movl    %esp, %ebp

		pushl	%ebx
		pushl	%ecx
		pushl	%edx
		pushl	%edi
		pushl	%esi

		movl    $0x2448324f, %ebx
		movw	$0x48EF, %ax
                movl    12(%ebp), %edx
		outb	%al, %dx
                 
                popl    %esi
		popl 	%edi
		popl	%edx
		popl	%ecx
		popl	%ebx
                popl    %ebp
 
          #AL Fun ret state
                ret
