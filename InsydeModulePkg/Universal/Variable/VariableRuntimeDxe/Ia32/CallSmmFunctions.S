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
##   32 bit Sent SMI to call IHISI VATS function
##

#------------------------------------------------------------------------------
#  UINT8
#  IhisiVatsCall (
#    IN     UINT8            *InPutBuff,
#    IN     UINT8            *OutPutBuff,
#    IN     UINT8            SubFunNum,
#    IN     UINT16           SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(IhisiVatsCall);
ASM_PFX(IhisiVatsCall):
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
		mov	$0xef, %al
		mov	20(%ebp), %edx		# SmiPort
		outb	%al, %dx

		popl	%esi
		popl	%edi
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%ebp
		ret
#------------------------------------------------------------------------------
#  UINT8
#  SmmSecureBootCall (
#    IN     UINT8            *InPutBuff,
#    IN     UINTN            DataSize,
#    IN     UINT8            SubFunNum,
#    IN     UINT16           SmiPort
#    )#
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SmmSecureBootCall);
ASM_PFX(SmmSecureBootCall):
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
		mov	$0xec, %al
		mov	20(%ebp), %edx		# SmiPort
		outb	%al, %dx

		popl	%esi
		popl	%edi
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%ebp
		ret
