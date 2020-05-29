;; @file
;  This is the code that supports IA32 CPU architectural protocol
;
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

include MpEqu.inc

.686p
.model  flat        
.code

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

EnableMce  proc near C public

  mov     eax, cr4
  or      eax, 40h
  mov     cr4, eax

  ret

EnableMce  endp

MpMtrrSynchUpEntry  PROC    NEAR C PUBLIC
    ;
    ; Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
    ;
    mov eax, cr0
    and eax, 0DFFFFFFFh
    or  eax, 040000000h
    mov cr0, eax
    ;
    ; Flush cache
    ;
    wbinvd
    ;
    ; Clear PGE flag Bit 7
    ;
    mov eax, cr4
    mov edx, eax
    and eax, 0FFFFFF7Fh
    mov cr4, eax
    ;
    ; Flush all TLBs
    ;
    mov eax, cr3
    mov cr3, eax
    
    mov eax, edx
    
    ret
    
MpMtrrSynchUpEntry  ENDP
    
MpMtrrSynchUpExit  PROC    NEAR C  PUBLIC

    push    ebp             ; C prolog
    mov     ebp, esp
    ;
    ; Flush all TLBs the second time
    ;
    mov eax, cr3
    mov cr3, eax
    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov eax, cr0
    and eax, 09FFFFFFFh
    mov cr0, eax
    ;
    ; Set PGE Flag in CR4 if set
    ;
    mov eax, dword ptr [ebp + 8]
    mov cr4, eax
    
    pop ebp
    
    ret

MpMtrrSynchUpExit  ENDP

;-------------------------------------------------------------------------------
;  AsmAcquireMPLock (&Lock);
;-------------------------------------------------------------------------------
AsmAcquireMPLock   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        mov         al, NotVacantFlag
        mov         ebx, dword ptr [ebp+24h]
TryGetLock:
        lock xchg   al, byte ptr [ebx]
        cmp         al, VacantFlag
        jz          LockObtained

        PAUSE32
        jmp         TryGetLock       

LockObtained:
        popad
        ret
AsmAcquireMPLock   ENDP

;-------------------------------------------------------------------------------
;  AsmReleaseMPLock (&Lock);                                                    
;-------------------------------------------------------------------------------------
AsmReleaseMPLock   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        mov         al, VacantFlag
        mov         ebx, dword ptr [ebp+24h]
        lock xchg   al, byte ptr [ebx]
        
        popad
        ret
AsmReleaseMPLock   ENDP

END
