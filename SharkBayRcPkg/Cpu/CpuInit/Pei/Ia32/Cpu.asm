;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license 
; agreement with Intel or your vendor.  This file may be      
; modified by the user, subject to additional terms of the    
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2012 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   Cpu.asm
;
; Abstract:
;
;   Assembly code of Cpu
;
;------------------------------------------------------------------------------

    .686p
    .model  flat
    .xmm

IA32_CR4_OSFXSR           equ        200h
IA32_CR4_OSXMMEXCPT       equ        400h
IA32_CR0_MP               equ        2h

IA32_CPUID_SSE2           equ        02000000h
IA32_CPUID_SSE2_B         equ        26

    .code

;------------------------------------------------------------------------------
;   Set up flags in CR4 for XMM instruction enabling
;------------------------------------------------------------------------------
XmmInit  PROC C    PUBLIC
    push    ebx

    ; Check whether SSE2 is supported
    mov     eax, 1
    cpuid
    bt      edx, IA32_CPUID_SSE2_B
    jnc     @F

    ; Enable XMM
    mov     eax, cr0
    or      eax, IA32_CR0_MP
    mov     cr0, eax
    mov     eax, cr4
    or      eax, IA32_CR4_OSFXSR OR IA32_CR4_OSXMMEXCPT
    mov     cr4, eax

@@:
    pop     ebx
    ret
XmmInit  ENDP


;------------------------------------------------------------------------------
;   Invalidate cache
;------------------------------------------------------------------------------
CacheInvd  PROC C    PUBLIC
    invd
    ret
CacheInvd  ENDP

    END
