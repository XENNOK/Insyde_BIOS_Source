;******************************************************************************
;* Copyright (c) 1983-2010, Insyde Software Corporation. All Rights Reserved.
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

text		segment

Int1Save	dq	2 dup(0)
Int3Save	dq	2 dup(0)
SmmSetupProc	dq	0

;==============================================================================
;  VOID AsmSaveIdt()
;==============================================================================

AsmSaveIdt	proc	public
		sub	rsp, 10h
		sidt	fword ptr [rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	@F
		mov	rdx, [rax + 10h]
		mov	Int1Save, rdx
		mov	rdx, [rax + 18h]
		mov	Int1Save + 8, rdx
		mov	rdx, [rax + 30h]
		mov	Int3Save, rdx
		mov	rdx, [rax + 38h]
		mov	Int3Save + 8, rdx
		shl     rdx, 32
		mov     rdx, [rax + 34h]
		mov     dx, [rax + 30h]
		cmp	dword ptr [rdx - 16], 044656267h
		jne	@F
		mov	rdx, [rdx - 8]
		mov	SmmSetupProc, rdx
@@:
		add	rsp, 10h
		ret
AsmSaveIdt	endp

;==============================================================================
;  VOID AsmRestoreIdt()
;==============================================================================

AsmRestoreIdt	proc	public
		sub	rsp, 10h
		sidt	fword ptr [rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	@F
		mov	rdx, Int1Save
		mov	[rax + 10h], rdx
		mov	rdx, Int1Save + 8
		mov	[rax + 18h], rdx
		mov	rdx, Int3Save
		mov	[rax + 30h], rdx
		mov	rdx, Int3Save + 8
		mov	[rax + 38h], rdx
@@:
		add	rsp, 10h
		ret
AsmRestoreIdt	endp

;==============================================================================
;  VOID AsmSmmSetup(BOOLEAN)
;==============================================================================

AsmSmmSetup	proc	public
		sub	rsp, 10h
		xor	rax, rax
		mov	al, cl
		or	rax, rax
		jz	@F
		sidt	fword ptr [rsp]
		mov	rax, rsp
@@:
		mov	rdx, SmmSetupProc
		or	rdx, rdx
		jz	@F
		call	rdx
@@:
		add	rsp, 10h
		ret
AsmSmmSetup	endp

		end



