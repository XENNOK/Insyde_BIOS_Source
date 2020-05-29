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
      TITLE   LegacyMode.asm: 32bit Legacy mode routine switch to Emu Sec

INCLUDE FastRecoveryData.inc

.686P
.MODEL FLAT, C
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
;;
;----------------------------------------------------------------------------
GoLegacyModePei PROC C PhaseData: DWORD, RamSize: DWORD
    ;
    ; Save PhaseData to edi
    ;
    mov edi, PhaseData
    
    ;
    ; Switch stack
    ;
    mov   ecx, DWORD PTR (FastRecoveryDxeToPeiData PTR [edi]).StackData
    add   ecx, RamSize
    add   ecx, 01Fh
    and   ecx, 0FFFFFFE0h
    mov   esp, ecx
    
    ;
    ; Transfer control to EmuSec 
    ;
    push  edi
    push  edi

    call  DWORD PTR (FastRecoveryDxeToPeiData PTR [edi]).EmuSecEntry

    ;
    ; Should new go here          
    ;      
    jmp   $
     
    xor   eax, eax
    ret     
GoLegacyModePei ENDP


END
