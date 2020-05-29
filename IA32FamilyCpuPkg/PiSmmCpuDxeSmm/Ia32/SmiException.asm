;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2009 - 2012, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;   SmiException.asm
;
; Abstract:
;
;   Exception handlers used in SM mode
;
;------------------------------------------------------------------------------

    .686p
    .model  flat,C

CpuSleep             PROTO   C
SmiPFHandler         PROTO   C

EXTERNDEF   gSmiMtrrs:QWORD
EXTERNDEF   gcSmiIdtr:FWORD
EXTERNDEF   gcSmiGdtr:FWORD
EXTERNDEF   gcPsd:BYTE
EXTERNDEF   FeaturePcdGet (PcdCpuSmmStackGuard):BYTE
EXTERNDEF   gSavedDebugExceptionIdtEntry:DWORD
EXTERNDEF   gSmiExceptionHandlers:QWORD
EXTERNDEF   FeaturePcdGet (PcdCpuSmmProfileEnable):BYTE


    .data

NullSeg     DQ      0                   ; reserved by architecture
            DQ      0                   ; reserved for future use
CodeSeg32   LABEL   QWORD
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      9bh
            DB      0cfh                ; LimitHigh
            DB      0                   ; BaseHigh
DataSeg32   LABEL   QWORD
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      93h
            DB      0cfh                ; LimitHigh
            DB      0                   ; BaseHigh
            DQ      0                   ; reserved for future use
CodeSeg16   LABEL   QWORD
            DW      -1
            DW      0
            DB      0
            DB      9bh
            DB      8fh
            DB      0
DataSeg16   LABEL   QWORD
            DW      -1
            DW      0
            DB      0
            DB      93h
            DB      8fh
            DB      0
CodeSeg64   LABEL   QWORD
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      9bh
            DB      0afh                ; LimitHigh
            DB      0                   ; BaseHigh
GDT_SIZE = $ - offset NullSeg

TssSeg      LABEL   QWORD
            DW      TSS_DESC_SIZE       ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      89h
            DB      080h                ; LimitHigh
            DB      0                   ; BaseHigh
ExceptionTssSeg     LABEL   QWORD
            DW      TSS_DESC_SIZE       ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      89h
            DB      080h                ; LimitHigh
            DB      0                   ; BaseHigh

CODE_SEL          = offset CodeSeg32 - offset NullSeg
DATA_SEL          = offset DataSeg32 - offset NullSeg
TSS_SEL           = offset TssSeg - offset NullSeg
EXCEPTION_TSS_SEL = offset ExceptionTssSeg - offset NullSeg

; Create 2 TSS segments just after GDT
TssDescriptor LABEL BYTE
            DW      0                   ; PreviousTaskLink
            DW      0                   ; Reserved
            DD      0                   ; ESP0
            DW      0                   ; SS0
            DW      0                   ; Reserved
            DD      0                   ; ESP1
            DW      0                   ; SS1
            DW      0                   ; Reserved
            DD      0                   ; ESP2
            DW      0                   ; SS2
            DW      0                   ; Reserved
ExceptionCr3 LABEL DWORD
            DD      0                   ; CR3
ExceptionEip LABEL DWORD
            DD      0                   ; EIP
ExceptionEflags LABEL DWORD
            DD      0                   ; EFLAGS
            DD      0                   ; EAX
            DD      0                   ; ECX
            DD      0                   ; EDX
            DD      0                   ; EBX
            DD      0                   ; ESP
            DD      0                   ; EBP
            DD      0                   ; ESI
            DD      0                   ; EDI
            DW      0                   ; ES
            DW      0                   ; Reserved
            DW      0                   ; CS
            DW      0                   ; Reserved
            DW      0                   ; SS
            DW      0                   ; Reserved
            DW      0                   ; DS
            DW      0                   ; Reserved
            DW      0                   ; FS
            DW      0                   ; Reserved
            DW      0                   ; GS
            DW      0                   ; Reserved
            DW      0                   ; LDT Selector
            DW      0                   ; Reserved
            DW      0                   ; T
            DW      0                   ; I/O Map Base
TSS_DESC_SIZE = $ - offset TssDescriptor

EXCEPTION_EIP_OFFSET    = offset ExceptionEip - offset  TssDescriptor
EXCEPTION_EFLAGS_OFFSET = offset ExceptionEflags - offset  TssDescriptor
EXCEPTION_CR3_OFFSET    = offset ExceptionCr3 - offset  TssDescriptor

ExceptionTssDescriptor LABEL BYTE
            DW      0                   ; PreviousTaskLink
            DW      0                   ; Reserved
            DD      0                   ; ESP0
            DW      0                   ; SS0
            DW      0                   ; Reserved
            DD      0                   ; ESP1
            DW      0                   ; SS1
            DW      0                   ; Reserved
            DD      0                   ; ESP2
            DW      0                   ; SS2
            DW      0                   ; Reserved
            DD      0                   ; CR3
            DD      offset PFHandlerEntry ; EIP
            DD      00000002            ; EFLAGS
            DD      0                   ; EAX
            DD      0                   ; ECX
            DD      0                   ; EDX
            DD      0                   ; EBX
            DD      0                   ; ESP
            DD      0                   ; EBP
            DD      0                   ; ESI
            DD      0                   ; EDI
            DW      DATA_SEL            ; ES
            DW      0                   ; Reserved
            DW      CODE_SEL            ; CS
            DW      0                   ; Reserved
            DW      DATA_SEL            ; SS
            DW      0                   ; Reserved
            DW      DATA_SEL            ; DS
            DW      0                   ; Reserved
            DW      DATA_SEL            ; FS
            DW      0                   ; Reserved
            DW      DATA_SEL            ; GS
            DW      0                   ; Reserved
            DW      0                   ; LDT Selector
            DW      0                   ; Reserved
            DW      0                   ; T
            DW      0                   ; I/O Map Base

gcPsd     LABEL   BYTE
            DB      'PSDSIG  '
            DW      PSD_SIZE
            DW      2
            DW      1 SHL 2
            DW      CODE_SEL
            DW      DATA_SEL
            DW      DATA_SEL
            DW      DATA_SEL
            DW      0
            DQ      0
            DQ      0
            DQ      0
            DQ      offset NullSeg
            DD      GDT_SIZE
            DD      0
            DB      24 dup (0)
            DQ      offset gSmiMtrrs
PSD_SIZE  = $ - offset gcPsd

gcSmiGdtr   LABEL   FWORD
    DW      GDT_SIZE - 1
    DD      offset NullSeg

gcSmiIdtr   LABEL   FWORD
    DW      IDT_SIZE - 1
    DD      offset _SmiIDT

_SmiIDT     LABEL   QWORD
REPEAT      32
    DW      0                           ; Offset 0:15
    DW      CODE_SEL                    ; Segment selector
    DB      0                           ; Unused
    DB      8eh                         ; Interrupt Gate, Present
    DW      0                           ; Offset 16:31
            ENDM
IDT_SIZE = $ - offset _SmiIDT

TaskGateDescriptor LABEL DWORD
    DW      0                           ; Reserved
    DW      EXCEPTION_TSS_SEL           ; TSS Segment selector
    DB      0                           ; Reserved
    DB      85h                         ; Task Gate, present, DPL = 0
    DW      0                           ; Reserved

;
; Saved IDT Entry for INT 1
;
gSavedDebugExceptionIdtEntry LABEL   DWORD
    DD      0
    DD      0
    
gSmiExceptionHandlers   LABEL   QWORD
    DD      14 dup (CpuSleep)
    DD      SmiPFHandler
    DD      17 dup (CpuSleep)

    .code

;------------------------------------------------------------------------------
; Exception handlers
;------------------------------------------------------------------------------
_SmiExceptionHandlers   PROC
IHDLRIDX = 0
REPEAT      8                           ; INT0 ~ INT7
    push    eax                         ; dummy error code
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      1                           ; INT8
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
    int     3
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      1                           ; INT9
    push    eax                         ; dummy error code
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      5                           ; INT10 ~ INT14
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
    int     3
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      2                           ; INT15 ~ INT16
    push    eax                         ; dummy error code
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      1                           ; INT17
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
    int     3
IHDLRIDX = IHDLRIDX + 1
            ENDM
REPEAT      14                          ; INT18 ~ INT31
    push    eax                         ; dummy error code
    push    IHDLRIDX
    DB      0e9h                        ; jmp disp32
    DD      _SmiExceptionEntryPoint - ($ + 4)
IHDLRIDX = IHDLRIDX + 1
            ENDM
_SmiExceptionHandlers   ENDP

;------------------------------------------------------------------------------
; _SmiExceptionEntryPoint is the entry point for all exceptions
;
; Stack frame would be as follows as specified in IA32 manuals:
;   RFLAGS  +1ch
;   CS      +18h
;   RIP     +14h
;   ErrCode +10h
;   INT#    +0ch
;   EAX     +8
;   ECX     +4
;   EDX     +0
;
; RSP set to odd multiple of 8 means ErrCode PRESENT
;------------------------------------------------------------------------------
_SmiExceptionEntryPoint PROC
    push    eax
    push    ecx
    push    edx
    push    0                          ; TSS base = 0 indicating SMM Stack Guard not enabled 

    mov     edx, [esp + 18h]           ; EIP

_SmiExceptionEntryPoint2:
    mov     ecx, [esp + 10h]           ; INT#
    mov     eax, [esp + 14h]           ; Errcode

; Push EIP
    push    edx
; Push ErrCode    
    push    eax
; Push INT#
    push    ecx
; UEFI calling convention for IA32 requires that Direction flag in EFLAGs is clear
    cld

    lea     eax, gSmiExceptionHandlers
    call    dword ptr [eax + ecx*4]
    pop     ecx
    pop     ecx
    pop     ecx
    pop     ecx                        ; TSS Base

; Set single step DB# if SMM profile is enabled and page fault exception happens
    cmp     FeaturePcdGet (PcdCpuSmmProfileEnable), 0
    jz      @Done

; Check if this is page fault exception
    mov     eax, [esp + 0ch]
    cmp     eax, 0eh
    jnz     @F

; Enable TF bit after page fault handler runs
    jecxz   @1

    bts     dword ptr [ecx + EXCEPTION_EFLAGS_OFFSET], 8
    jmp     @Done

@1:
    mov     eax, [esp + 1ch] ; EFLAGS
    bts     eax, 8 
    mov     [esp + 1ch], eax
    jmp     @Done

@@:
; Check if this is INT 1 exception
    mov     eax, [esp + 0ch]
    cmp     eax, 1h
    jnz     @Done

; Clear TF bit after INT1 handler runs
    jecxz   @2

    btc     dword ptr [ecx + EXCEPTION_EFLAGS_OFFSET], 8
    jmp     @Done

@2:
    mov     eax, [esp + 1ch] ; EFLAGS
    btc     eax, 8 
    mov     [esp + 1ch], eax

@Done:
    pop     edx
    pop     ecx
    pop     eax
    add     esp, 8                      ; skip INT# & ErrCode
    iretd
;
; Page Fault Exception Handler entry when SMM Stack Guard is enabled
;
PFHandlerEntry::
    push    14                          ; INT#
    push    eax
    push    ecx
    push    edx

;
; Get this processor's TSS
;
    sub     esp, 8
    sgdt    [esp + 2]
    mov     eax, [esp + 4]               ; GDT base
    add     esp, 8
    mov     ecx, [eax + TSS_SEL + 2]
    shl     ecx, 8
    mov     cl, [eax + TSS_SEL + 7]
    ror     ecx, 8                       ; ecx = TSS base
    push    ecx                          ; Save TSS base

; Workaround: processor does not save CR3 in task switch?
    mov     eax, cr3
    mov     [ecx + EXCEPTION_CR3_OFFSET], eax
    
    mov     edx, [ecx + EXCEPTION_EIP_OFFSET]  ; EIP

    jmp     _SmiExceptionEntryPoint2
_SmiExceptionEntryPoint ENDP

InitializeIDT   PROC    USES    ebx
    lea     edx, _SmiIDT - 8
;    push    IDT_SIZE / 8
    DB      68h                         ; push /id
    DD      IDT_SIZE / 8
    lea     ebx, _SmiExceptionHandlers - 8
    pop     ecx
@@:
    lea     eax, [ebx + ecx*8]
    mov     [edx + ecx*8], ax
    shr     eax, 16
    mov     [edx + ecx*8 + 6], ax
    loop    @B

    cmp     FeaturePcdGet (PcdCpuSmmStackGuard), 0
    jz      @F

;
; If SMM Stack Guard feature is enabled, the Page Fault Exception entry in IDT
; is a Task Gate Descriptor so that when a Page Fault Exception occurrs,
; the processors can use a known good stack in case stack is ran out.
;
    lea     ebx, _SmiIDT + 14 * 8
    lea     edx, TaskGateDescriptor
    mov     eax, [edx]
    mov     [ebx], eax
    mov     eax, [edx + 4]
    mov     [ebx + 4], eax

@@:
    cmp     FeaturePcdGet (PcdCpuSmmProfileEnable), 0
    jz      @F

;
; Save INT 1 IDT entry in gSavedDebugExceptionIdtEntry
;
    lea     ebx, _SmiIDT + 1 * 8
    lea     edx, gSavedDebugExceptionIdtEntry
    mov     eax, [ebx]
    mov     [edx], eax
    mov     eax, [ebx + 4]
    mov     [edx + 4], eax

@@:
    ret
InitializeIDT   ENDP

    END
