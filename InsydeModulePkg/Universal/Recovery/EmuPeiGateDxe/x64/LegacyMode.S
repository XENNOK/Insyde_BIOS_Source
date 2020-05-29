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
#

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
#----------------------------------------------------------------------------
# RCX - FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData
# RDX - UINTN                              RamSize
#
# RAX - Returned EFI_STATUS
#

ASM_GLOBAL ASM_PFX (GoLegacyModePei);
ASM_PFX (GoLegacyModePei):
#
# Save phase data pointer to memory
#
	mov	%rcx, %rdi

#
# Save GDT base address
# Gdt destination address save to rsi for convenience in legacy mode
#
	mov	$GDT_BASE, %rax
	lea	$GdtDesc, %rsi
	movl	%eax, rsi+2

#
# Save mode changing jump pointers into memory for long jump
#
	mov	$CompatibilityMode,$rax
	movl	%eax,rsi+24

	mov	$ProtectedModeAfterGdt,$rbx
	movl	%ebx,rsi+32

#
# Reset stack
#      
	mov	FastRecoveryDxeToPeiData PTR (%edi),$rax
	addl	%edx,%eax
	addl	$0x01F,%eax
	andl	$0x0FFFFFFE0,%eax
	movl	%eax,%esp

#
# jump to cmpatibility mode
#
	cli	
	lea	$QWORD PTR CompatibilityModeVector,$rax
	jmp	FWORD PTR+rax

CompatibilityMode:		
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#     now in 64-bit compatibility mode           ;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movw	$0x080, %dx
	movb	$0x001, %al
	outb	%al, %dx

#
# disable paging
#

	movl	%cr0, $rax
	btrl	$31, %eax
	movl	$rax, %cr0

#
#  set EFER.LME = 0 to leave long mode
#
	movl	$0x0c0000080,%ecx	# EFER MSR number.
	rdmsr		# Read EFER.
	btrl	$8,%eax	# Set LME=0.
	wrmsr		# Write EFER.

	jmp	Legacy32bitProtectedMode

Legacy32bitProtectedMode:		
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#     now in 32-bit legacy mode                  ;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#
# Set IDTR for 32 bit legacy mode
# lidt [esi+8]
#
	.byte	0x0f
	.byte	0x01
	.byte	0x05e
	.byte	0x08

#
# Set GDTR
# lgdt   [esi]
#
	.byte	0x0f
	.byte	0x01
	.byte	0x016

#
# Change CS by long jump
# jmp    FWORD PTR  [esi+32]  
#
	.byte	0x0ff
	.byte	0x06e
	.byte	0x020

ProtectedModeAfterGdt:		

#
# Load selectors and stack pointer
#
	movl	$0x018,%eax
	movw	%ax,$gs
	movw	%ax,$fs
	movw	%ax,$es
	movw	%ax,$ds
	movw	%ax,$ss


#
# PEI Execution environment setup OK. We can transfer control to EmuSec
# by callubg SecEntry
#
	.byte	0x057	# push  edi
	.byte	0x057	# push  edi

	.byte	0x0ff	# call  DWORD PTR [edi+PEI_STACK_SIZE + 8 * 4]
	.byte	0x057
	.byte	0x020
#
# Should new go here          
#      
	jmp	$

	xor	$rax,$rax
	ret	

#
# Data
#    
.align	10h	

#
# Legacy Mode GDT
#

#
# GDT[0]: 0x00: Null entry, never used.
#
.equ	NULL_SEL,	$$ - GDT_BASE	# Selector [0]
GDT_BASE:		
BootGDTtable:	.int	0
	.int	0
#
# Linear data segment descriptor
#
.equ	LINEAR_SEL,	$$ - GDT_BASE	# Selector [0x8]
	.short	0x0FFFF	# limit 0xFFFFF
	.short	0	# base 0
	.byte	0
	.byte	0x092	# present, ring 0, data, expand-up, writable
	.byte	0x0CF	# page-granular, 32-bit
	.byte	0
#
# Linear code segment descriptor
#
.equ	LINEAR_CODE_SEL,	$$ - GDT_BASE	# Selector [0x10]
	.short	0x0FFFF	# limit 0xFFFFF
	.short	0	# base 0
	.byte	0
	.byte	0x09B	# present, ring 0, data, expand-up, not-writable
	.byte	0x0CF	# page-granular, 32-bit
	.byte	0
#
# System data segment descriptor
#
.equ	SYS_DATA_SEL,	$$ - GDT_BASE	# Selector [0x18]
	.short	0x0FFFF	# limit 0xFFFFF
	.short	0	# base 0
	.byte	0
	.byte	0x093	# present, ring 0, data, expand-up, not-writable
	.byte	0x0CF	# page-granular, 32-bit
	.byte	0

#
# System code segment descriptor
#
.equ	SYS_CODE_SEL,	$$ - GDT_BASE	# Selector [0x20]
	.short	0x0FFFF	# limit 0xFFFFF
	.short	0	# base 0
	.byte	0
	.byte	0x09A	# present, ring 0, data, expand-up, writable
	.byte	0x0CF	# page-granular, 32-bit
	.byte	0
#
# Spare segment descriptor
#
.equ	SYS16_CODE_SEL,	$$ - GDT_BASE	# Selector [0x28]
	.short	0x0FFFF	# limit 0xFFFF
	.short	0	# base 0xF000
	.byte	0x0F
	.byte	0x09B	# present, ring 0, code, expand-up, writable
	.byte	0x00	# byte-granular, 16-bit
	.byte	0
#
# Spare segment descriptor
#
.equ	SYS16_DATA_SEL,	$$ - GDT_BASE	# Selector [0x30]
	.short	0x0FFFF	# limit 0xFFFF
	.short	0	# base 0
	.byte	0
	.byte	0x093	# present, ring 0, data, expand-up, not-writable
	.byte	0x00	# byte-granular, 16-bit
	.byte	0

#
# Spare segment descriptor
#
.equ	SPARE5_SEL,	$$ - GDT_BASE	# Selector [0x38]
	.short	0	# limit 0
	.short	0	# base 0
	.byte	0
	.byte	0	# present, ring 0, data, expand-up, writable
	.byte	0	# page-granular, 32-bit
	.byte	0
.equ	GDT_SIZE,	$$ - BootGDTtable	# Size, in bytes


GdtDesc:			# GDT descriptor
	.short	GDT_SIZE - 1	# GDT limit
	.int	?	# GDT base address
	.short	?	# Reserved
ProtectedModeIdt:		
	.short	0x3FF	# Limit
	.int	0x0	# Base
	.short	?	# Reserved
PhaseDataPointer:		
	.quad	?	# Point to Phase data

CompatibilityModeVector:		
	.int	?	# Compatible Mode entry
	.short	0x20	# 32bit code segment (Follow LongMode.asm)
	.short	?	# Reserved

LegacyModeVector:		
	.int	?	# Legacy Mode entry
	.short	0x10	# Code selector
	.short	?	# Reserved







