#/** @file
#  32 bit Sent SMI to call registered SMM callback in this driver.
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
#    IN     UINT8            *InPutBuff,
#    IN     UINTN            DataSize,
#    IN     UINT8            SubFunNum,
#    IN     UINT16           SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SmmFvbAccessCall);
ASM_PFX(SmmFvbAccessCall):
		pushl   %ebp
		movl	%esp, %ebp	    
		pushl	%ebx
		pushl	%ecx
		pushl	%edx
		pushl	%edi
		pushl	%esi

		movl	8(%ebp), %esi		# InputBuff
		movl	12(%ebp), %ebx		# DataSize
		movl	16(%ebp), %eax		# SubFunNum
		movb	%al, %ah
		mov	$0x16, %al
		mov	20(%ebp), %edx		# SmiPort
		outb	%al, %dx

		popl	%esi
		popl	%edi
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%ebp
		ret
