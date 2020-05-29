;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
;
;------------------------------------------------------------------------------
;
; Copyright (c)  2006 Intel Corporation. All rights reserved
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
;   Thunk.asm
;
; Abstract:
;
;   Real mode thunk
;
;------------------------------------------------------------------------------

EXTERNDEF   mCode16Size:QWORD

    .const

mCode16Size     DQ      _Code16End - _Code16Addr

    .data

NullSegSel      DQ      0
_16CsSegSel     LABEL   QWORD
                DW      -1
                DW      0
                DB      0
                DB      9bh
                DB      8fh             ; 16-bit segment
                DB      0
_16DsSegSel     LABEL   QWORD
                DW      -1
                DW      0
                DB      0
                DB      93h
                DB      8fh             ; 16-bit segment
                DB      0

_16Gdtr         LABEL   FWORD
                DW      $ - offset NullSegSel - 1
                DQ      offset NullSegSel

    .code

IA32_REGS   STRUC   4t
_EDI        DD      ?
_ESI        DD      ?
_EBP        DD      ?
_ESP        DD      ?
_EBX        DD      ?
_EDX        DD      ?
_ECX        DD      ?
_EAX        DD      ?
_DS         DW      ?
_ES         DW      ?
_FS         DW      ?
_GS         DW      ?
_RFLAGS     DQ      ?
_EIP        DD      ?
_CS         DW      ?
_SS         DW      ?
IA32_REGS   ENDS

_STK16      STRUC   1t
RetEip      DD      ?
RetCs       DW      ?
ThunkFlags  DW      ?
SavedGdtr   FWORD   ?
Resvd1      DW      ?
SavedCr0    DD      ?
SavedCr4    DD      ?
_STK16      ENDS

; RCX - IA32_REGISTER_SET         *RegisterSet,
; RDX - (UINT16)(ThunkFlags >> 16)
; R8  - ThunkContext->RealModeBuffer >> 4

_Thunk16    PROC    USES    rbp rbx rsi rdi r12 r13 r14 r15

    push    fs
    push    gs

    mov     r12d, ds
    mov     r13d, es
    mov     r14d, ss
    mov     r15, rsp
    mov     rsi, rcx
    movzx   r10, (IA32_REGS ptr [rsi])._SS
    xor     rdi, rdi
    mov     edi, (IA32_REGS ptr [rsi])._ESP
    add     rdi, - sizeof (IA32_REGS) - sizeof (_STK16)
    push    rdi
    imul    rax, r10, 16
    add     rdi, rax
    push    sizeof (IA32_REGS) / 4
    pop     rcx
    rep     movsd
    pop     rbx                         ; rbx <- 16-bit stack offset
    lea     eax, @F                     ; return offset
    stosd
    mov     eax, cs                     ; return segment
    stosw
    mov     eax, edx                    ; THUNK Flags
    stosw
    sgdt    fword ptr [rsp + 58h]       ; save GDTR
    mov     rax, [rsp + 58h]
    stosq
    mov     rax, cr0                    ; save CR0
    mov     esi, eax                    ; esi <- CR0 to set
    stosd
    mov     rax, cr4                    ; save CR4
    stosd
    sidt    fword ptr [rsp + 58h]       ; save IDTR
    and     esi, 07ffffffeh             ; clear PE & PG bits
    mov     rdi, r10                    ; rdi <- 16-bit stack segment

    shl     r8, 16
IFDEF      SMM_INT10_ENABLE
    mov     qword ptr [PatchRealModeOffset], r8
ELSE
    push    r8                          ; far jmp address
ENDIF
    lea     eax, @16Bit
    push    rax
    mov     word ptr [rsp + 4], 8
    lgdt    _16Gdtr
    retf

@16Bit:
IFDEF      SMM_INT10_ENABLE
;    mov     ax, 16
     DB      0b8h
     DB      010h
     DB      000h
;    mov     ds, ax
     DB      08eh
     DB      0d8h
;    mov     es, ax
     DB      08eh
     DB      0c0h
;    mov     fs, ax
     DB      08eh
     DB      0e0h
;    mov     gs, ax
     DB      08eh
     DB      0e8h
;    mov     ss, ax
     DB      08eh
     DB      0d0h
ENDIF
    DB      66h
    mov     ecx, 0c0000080h
    mov     cr0, rsi                    ; disable PE & PG
    rdmsr
    and     ah, NOT 1
    wrmsr                               ; clear LME bit
    mov     rax, cr4
    and     al, NOT 30h                 ; clear PAE & PSE
    mov     cr4, rax
IFDEF      SMM_INT10_ENABLE
    DB      0EAh                        ; 'jmp far' opcode (absolute, address given in operand)
PatchRealModeOffset:
    DQ      00h
ELSE
    retf
ENDIF
@@:
    xor     rax, rax
    mov     eax, ss
    shl     eax, 4
    add     eax, esp                    ; rax <- address of 16-bit stack
    mov     rsp, r15
IFDEF      SMM_INT10_ENABLE
    mov     ss, r14d
ENDIF
    lidt    fword ptr [rsp + 58h]       ; restore IDTR
    mov     ds, r12d
    mov     es, r13d
IFNDEF      SMM_INT10_ENABLE
     mov     ss, r14d
ENDIF
    pop     gs
    pop     fs
    ret
_Thunk16    ENDP

    ALIGN   10h

_Code16Addr PROC
_Code16Addr ENDP

RealMode    PROC
    mov     ss, edi
    mov     sp, bx                      ; set up 16-bit stack
    DB      2eh, 0fh, 1, 1eh
    DW      _16Idtr - _Code16Addr       ; lidt _16Idtr
    DB      66h, 61h                    ; popad
    DB      1fh                         ; pop ds
    DB      7                           ; pop es
    pop     fs
    pop     gs

    add     esp, 8                      ; skip RFLAGS
    DB      67h, 0f7h, 44h, 24h, 0eh, 1, 0  ; test [esp + 0eh], 1
    jz      @F
    pushfq                              ; pushf, actually
@@:
    DB      0eh                         ; push cs
    DB      68h                         ; push /iw
    DW      @FarCallRet - _Code16Addr
    jz      @F
    DB      66h
    jmp     fword ptr [esp + 6]
@@:
    DB      66h
    jmp     fword ptr [esp + 4]
@FarCallRet:
    DB      66h
    push    0                           ; push a dword of zero
    pushf                               ; pushfd, actually
    push    gs
    push    fs
    DB      6                           ; push es
    DB      1eh                         ; push ds
    DB      66h, 60h                    ; pushad
    cli

    DB      66h
    lgdt    (_STK16 ptr [esp + sizeof(IA32_REGS)]).SavedGdtr
    DB      66h
    mov     eax, (_STK16 ptr [esp + sizeof(IA32_REGS)]).SavedCr4
    mov     cr4, rax
    DB      66h
    mov     ecx, 0c0000080h
    rdmsr
    or      ah, 1
    wrmsr                               ; set LME
    DB      66h
    mov     eax, (_STK16 ptr [esp + sizeof(IA32_REGS)]).SavedCr0
    mov     cr0, rax
    DB      66h
    jmp     fword ptr (_STK16 ptr [esp + sizeof(IA32_REGS)]).RetEip

RealMode    ENDP

_16Idtr     FWORD   (1 SHL 10) - 1

_Code16End:

    END
