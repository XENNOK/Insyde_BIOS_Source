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
;

      TITLE   LegacyMode.asm: x64 Long mode to 32bit Legacy mode routine
      
INCLUDE FastRecoveryData.inc

.CODE
;----------------------------------------------------------------------------
; Prototype:    EFI_STATUS
;               GoLegacyModePei (
;                 IN FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData,
;                 IN UINTN                              RamSize
;                 );
;
; Arguments:
;   PhaseData    - Pinter to structure of phase transition data
;   RamSize      - cash as ram size
;
; Return Value:
;   EFI_SUCCESS  - Should never return
;
; Description:  
;   Switch current execution mode to 32 bit legacy protected mode and transfer
;   control to EmuSec               
;               
;               
;
;----------------------------------------------------------------------------
; RCX - FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData
; RDX - UINTN                              RamSize
;
; RAX - Returned EFI_STATUS
;

GoLegacyModePei PROC    PUBLIC
      ;
      ; Save phase data pointer to memory
      ;
      mov     QWORD PTR PhaseDataPointer, rcx
      mov     rdi,  rcx
      
      ;
      ; Save GDT base address
      ; Gdt destination address save to rsi for convenience in legacy mode
      ;
      mov     rax,  OFFSET GDT_BASE
      lea     rsi,  QWORD PTR GdtDesc
      mov     DWORD PTR [rsi+2], eax
      
      ;
      ; Save mode changing jump pointers into memory for long jump
      ;
      mov     rax,  OFFSET CompatibilityMode
      mov     DWORD PTR [rsi+24], eax
      
      mov     rbx,  OFFSET ProtectedModeAfterGdt
      mov     DWORD PTR [rsi+32], ebx

      ;
      ; Reset stack
      ;      
      mov     rax, (FastRecoveryDxeToPeiData PTR [edi]).StackData
      add     eax, edx
      add     eax, 01Fh
      and     eax, 0FFFFFFE0h
      mov     esp,  eax
      
      ;
      ; jump to cmpatibility mode
      ;
      cli
      lea     rax,  QWORD PTR CompatibilityModeVector
      jmp     FWORD PTR[rax]

CompatibilityMode:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;     now in 64-bit compatibility mode           ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

      mov     dx,   080h
      mov     al,   001h
      out     dx,   al
      
      ;
      ; disable paging
      ;
      
      mov     rax,  cr0
      btr     eax,  31
      mov     cr0,  rax

      ;
      ;  set EFER.LME = 0 to leave long mode
      ;
      mov     ecx,  0c0000080h ; EFER MSR number.
      rdmsr                    ; Read EFER.
      btr     eax,  8          ; Set LME=0.
      wrmsr                    ; Write EFER.
  
      jmp     Legacy32bitProtectedMode
      
Legacy32bitProtectedMode:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;     now in 32-bit legacy mode                  ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      ;
      ; Set IDTR for 32 bit legacy mode
      ; lidt [esi+8]
      ;
      DB      0fh
      DB      01h
      DB      05eh
      DB      08h
      
      ;
      ; Set GDTR
      ; lgdt   [esi]
      ;
      DB      0fh
      DB      01h
      DB      016h

      ;
      ; Change CS by long jump
      ; jmp    FWORD PTR  [esi+32]  
      ;
      DB      0ffh
      DB      06eh
      DB      020h
      
ProtectedModeAfterGdt:
    
      ;
      ; Load selectors and stack pointer
      ;
      mov     eax,  018h
      mov     gs,   ax
      mov     fs,   ax
      mov     es,   ax              
      mov     ds,   ax 
      mov     ss,   ax


      ;
      ; PEI Execution environment setup OK. We can transfer control to EmuSec
      ; by callubg SecEntry
      ;
      DB      057h                 ; push  edi
      DB      057h                 ; push  edi

      DB      0ffh                 ; call  DWORD PTR [edi+PEI_STACK_SIZE + 8 * 4]
      DB      057h
      DB      020h
      ;
      ; Should new go here          
      ;      
      jmp     $ 

      xor     rax,  rax
      ret    

GoLegacyModePei ENDP



;
; Data
;    
align 10h
       
;
; Legacy Mode GDT
;

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL            equ     $ - GDT_BASE               ; Selector [0]
GDT_BASE:
BootGDTtable        DD      0
                    DD      0
;
; Linear data segment descriptor
;
LINEAR_SEL          equ     $ - GDT_BASE               ; Selector [0x8]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      092h                       ; present, ring 0, data, expand-up, writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL     equ     $ - GDT_BASE               ; Selector [0x10]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      09Bh                       ; present, ring 0, data, expand-up, not-writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; System data segment descriptor
;
SYS_DATA_SEL        equ     $ - GDT_BASE               ; Selector [0x18]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      093h                       ; present, ring 0, data, expand-up, not-writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0

;
; System code segment descriptor
;
SYS_CODE_SEL        equ     $ - GDT_BASE               ; Selector [0x20]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      09Ah                       ; present, ring 0, data, expand-up, writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; Spare segment descriptor
;
SYS16_CODE_SEL      equ     $ - GDT_BASE               ; Selector [0x28]
                    DW      0FFFFh                     ; limit 0xFFFF
                    DW      0                          ; base 0xF000
                    DB      0Fh
                    DB      09Bh                       ; present, ring 0, code, expand-up, writable
                    DB      00h                       ; byte-granular, 16-bit
                    DB      0
;
; Spare segment descriptor
;
SYS16_DATA_SEL      equ     $ - GDT_BASE               ; Selector [0x30]
                    DW      0FFFFh                     ; limit 0xFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      093h                       ; present, ring 0, data, expand-up, not-writable
                    DB      00h                        ; byte-granular, 16-bit
                    DB      0

;
; Spare segment descriptor
;
SPARE5_SEL          equ     $ - GDT_BASE               ; Selector [0x38]
                    DW      0                          ; limit 0
                    DW      0                          ; base 0
                    DB      0
                    DB      0                          ; present, ring 0, data, expand-up, writable
                    DB      0                          ; page-granular, 32-bit
                    DB      0
GDT_SIZE            EQU     $ - BootGDTtable           ; Size, in bytes


GdtDesc:                                               ; GDT descriptor
                    DW      GDT_SIZE - 1               ; GDT limit
                    DD      ?                          ; GDT base address
                    DW      ?                          ; Reserved
ProtectedModeIdt:
                    DW      3FFh                       ; Limit
                    DD      0h                         ; Base
                    DW      ?                          ; Reserved
PhaseDataPointer:
                    DQ      ?                          ; Point to Phase data

CompatibilityModeVector:
                    DD      ?                          ; Compatible Mode entry
                    DW      20h                        ; 32bit code segment (Follow LongMode.asm)
                    DW      ?                          ; Reserved

LegacyModeVector:
                    DD      ?                          ; Legacy Mode entry
                    DW      10h                        ; Code selector
                    DW      ?                          ; Reserved






END
