#;******************************************************************************
#;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
#;*
#;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#;* transmit, broadcast, present, recite, release, license or otherwise exploit
#;* any part of this publication in any form, by any means, without the prior
#;* written permission of Insyde Software Corporation.
#;*
#;******************************************************************************
#
	TITLE	$LegacyMode.asm: 0B32it Legacy mode routine switch to Emu Sec

.686P		
.MODEL	FLAT,	$C
.CODE		


FastRecoveryDxeToPeiData	STRUC	
	StackData	$dq   ?

#
# Data address
#
	EmuPeiFv	$dq   ?
	EmuPeiFvBase	$dq   ?
	EmuPeiEntry	$dq   ?
	EmuSecEntry	$dq   ?
	EmuSecFfsAddress	$dq   ?	# Will mask after we execute
	RecoveryImageAddress	$dq   ?
	OriginalHobList	$dq   ?
	RomToRamDifference	$dq   ?

#
# Data values
#
	EmuPeiFvSize	$dq   ?	# Refer Flashmap.h
	RecoveryImageSize	$dq   ?
FastRecoveryDxeToPeiData	ENDS	

#----------------------------------------------------------------------------
# Prototype:    EFI_STATUS
#               GoLegacyModePei (
#                 IN FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData,
#                 IN UINTN                              RamSize
#                 );
#
# Arguments:
#   PhaseData    - Pinter to structure of phase transition data
#   RamSize      - cash as ram size
#
# Return Value:
#   EFI_SUCCESS  - Should never return
#
# Description:  
#   Switch current execution mode to 32 bit legacy protected mode and transfer
#   control to EmuSec               
#               
#               
#;
#----------------------------------------------------------------------------
GoLegacyModePei	PROC	$RamSize: DWORD,$C PhaseData: DWORD
#
# Save PhaseData to edi
#
	movl	$PhaseData,%edi

#
# Switch stack
#
	movl	FastRecoveryDxeToPeiData PTR (%edi),%ecx
	addl	$RamSize,%ecx
	addl	$0x01F,%ecx
	andl	$0x0FFFFFFE0,%ecx
	movl	%ecx,%esp

#
# Transfer control to EmuSec 
#
	pushl	%edi
	pushl	%edi

	calll	FastRecoveryDxeToPeiData PTR (%edi)

#
# Should new go here          
#      
	jmp	$

	xorl	%eax,%eax
	ret	
GoLegacyModePei	ENDP	



