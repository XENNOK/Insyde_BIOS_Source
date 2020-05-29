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
;   AsmFuncs for PeCoffExtraActionLib
;

text		segment

;==============================================================================
;  VOID AsmSendInfo(PE_COFF_LOADER_IMAGE_CONTEXT*)
;==============================================================================

AsmSendInfo	proc	public
		sub	rsp, 10h
		sidt	fword ptr [rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	@F
		mov	rdx, [rax + 38h]
		shl     rdx, 32
		mov     rdx, [rax + 34h]
		mov     dx, [rax + 30h]
		cmp	dword ptr [rdx - 16], 044656267h
		jne	@F
		mov	rax, [rdx - 32]
		mov	rdx, rcx
		call	rax
@@:
		add	rsp, 10h
		ret
AsmSendInfo	endp

		end



