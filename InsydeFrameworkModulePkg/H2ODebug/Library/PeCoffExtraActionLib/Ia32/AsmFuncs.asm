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

.686p
.model  flat
.stack
.code

;==============================================================================
;  VOID AsmSendInfo(PE_COFF_LOADER_IMAGE_CONTEXT*)
;==============================================================================

_AsmSendInfo	proc	near	public
		sub	esp, 10h
		sidt	fword ptr [esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	@F
		mov	edx, [eax + 28]
		mov	dx, [eax + 24]
		cmp	dword ptr [edx - 16], 044656267h
		jne	@F
		push	dword ptr [esp + 14h]
		push	dword ptr [eax]
		mov	edx, [edx - 32]
		call	edx
		add	esp, 8
@@:
		add	esp, 10h
		ret
_AsmSendInfo	endp

		end



