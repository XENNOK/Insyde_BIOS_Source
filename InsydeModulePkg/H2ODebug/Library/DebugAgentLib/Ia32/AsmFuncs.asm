;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;
; Module Name:
;
;   AsmFuncs.asm
;
; Abstract:
;
;   AsmFuncs for DebugAgentLib
;

.686p
.model  flat
.stack
.code

Int1Save	dd	2 dup(0)
Int3Save	dd	2 dup(0)
SmmSetupProc	dd	0

;==============================================================================
;  VOID AsmSaveIdt()
;==============================================================================

_AsmSaveIdt	proc	near	public
		sub	esp, 10h
		sidt	fword ptr [esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	@F
		mov	edx, [eax + 8]
		mov	Int1Save, edx
		mov	edx, [eax + 12]
		mov	Int1Save + 4, edx
		mov	edx, [eax + 24]
		mov	Int3Save, edx
		mov	edx, [eax + 28]
		mov	Int3Save + 4, edx
		mov	dx, [eax + 24]
		cmp	dword ptr [edx - 16], 044656267h
		jne	@F
		mov	edx, [edx - 8]
		mov	SmmSetupProc, edx
@@:
		add	esp, 10h
		ret
_AsmSaveIdt	endp

;==============================================================================
;  VOID AsmRestoreIdt()
;==============================================================================

_AsmRestoreIdt	proc	near	public
		sub	esp, 10h
		sidt	fword ptr [esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	@F
		mov	edx, Int1Save
		mov	[eax + 8], edx
		mov	edx, Int1Save + 4
		mov	[eax + 12], edx
		mov	edx, Int3Save
		mov	[eax + 24], edx
		mov	edx, Int3Save + 4
		mov	[eax + 28], edx
@@:
		add	esp, 10h
		ret
_AsmRestoreIdt	endp

;==============================================================================
;  VOID AsmSmmSetup(BOOLEAN)
;==============================================================================

_AsmSmmSetup	proc	near	public
		sub	esp, 10h
		xor	eax, eax
		mov	al, [esp + 14h]
		or	eax, eax
		jz	@F
		sidt	fword ptr [esp]
		mov	eax, esp
@@:
		mov	edx, SmmSetupProc
		or	edx, edx
		jz	@F
		call	edx
@@:
		add	esp, 10h
		ret
_AsmSmmSetup	endp

;==============================================================================
;  VOID AsmThunkX64Setup()
;==============================================================================

_AsmSetupDxeIdt	proc	public
		mov	eax, dword ptr ds:[03fch]
		or	eax, eax
		jz	quit
		cmp	dword ptr [eax], 044656267h
		jne	quit
		sub	esp, 10h
		mov	word ptr [esp], 3fh
		mov	eax, [eax - 08h]
		sub	eax, 048h
		mov	[esp + 2], eax
		cmp	dword ptr [eax - 4], 0
		jne	@F
		push	ebx
		sidt	fword ptr [eax - 10h]
		mov	ebx, [eax - 0eh]
		mov	ebx, [ebx - 4]
		mov	[eax - 4], ebx
		pop	ebx
@@:		
		lidt	fword ptr [esp]
		add	esp, 10h
quit:
		ret
_AsmSetupDxeIdt	endp

		end



