;;******************************************************************************
;;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
      TITLE   LegacyMode.asm: 32bit Legacy mode routine switch to Emu Sec


.686P
.MODEL FLAT, C
.CODE


;----------------------------------------------------------------------------
; Prototype:    EFI_STATUS
;               GoLegacyModePei (
;                 IN FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData
;                 );
;
; Arguments:
;   PhaseData    - Pinter to structure of phase transition data
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
GoLegacyModePei PROC C PhaseData: DWORD
    ;
    ; Save PhaseData to edi
    ;
    mov edi, PhaseData
    
    ;
    ; Switch stack
    ;
    lea   ecx, DWORD PTR [edi + PEI_STACK_SIZE] 
    mov   esp, ecx
    
    ;
    ; Transfer control to EmuSec 
    ;
    push  edi
    push  edi

    call  DWORD PTR [edi+PEI_STACK_SIZE + 8 * 3]

    ;
    ; Should new go here          
    ;      
    jmp   $
     
    xor   eax, eax
    ret     
GoLegacyModePei ENDP

IFDEF SIZE_OF_CAR
PEI_STACK_SIZE      EQU     (SIZE_OF_CAR*1024)
ELSE
PEI_STACK_SIZE      EQU     (64*1024)
ENDIF

END
