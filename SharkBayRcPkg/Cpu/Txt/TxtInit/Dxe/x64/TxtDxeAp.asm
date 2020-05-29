;/** @file
;  This file contains DXE AP initialization code.
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
;*/
;/*++                                                             
;  This file contains an 'Intel Peripheral Driver' and uniquely   
;  identified as "Intel Reference Module" and is                  
;  licensed for Intel CPUs and chipsets under the terms of your   
;  license agreement with Intel or your vendor.  This file may    
;  be modified by the user, subject to additional terms of the    
;  license agreement                                              
;--*/                                                             
;                                                                 
;/*++                                                             
;                                                                 
; Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
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
;   TxtDxeAp.asm
; 
; Abstract:
; 
;   This file contains DXE AP initialization code.
;
;--*/

	.xlist 
	include	txt.inc 
	include	mmx64.inc 
	.list 


	_TEXT	SEGMENT


EXTERN        mAcmBase:QWORD
EXTERN        mMcuAddr:QWORD
EXTERN        mApMtrrTab:QWORD
EXTERN        mApIdt:QWORD
EXTERN        mApCr4:QWORD
EXTERN        mApSavedIa32ThermInterruptMSR:QWORD
EXTERN        mApSavedApicThermalValue:DWORD
              
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:	DoApInit
; 
;  Input:	AcmBase - Base address of LT BIOS ACM
;               McuAddr - Address of MCU patch in flash
; 
;  Output:	None
; 
;  Registers:	All are preserved
; 
;  Description:	Initiatialize AP before GETSEC as per TXT BWG
; 
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoApInit PROC FRAME

    START_FRAME
    MAKE_LOCAL ACM_SIZE_TO_CACHE:QWORD
    MAKE_LOCAL ACM_BASE_TO_CACHE:QWORD
    MAKE_LOCAL NEXT_MTRR_INDEX:QWORD
    MAKE_LOCAL NEXT_MTRR_SIZE:QWORD
    ; jmp     $
    END_FRAME

    pushf
    cli
    PUSHA_64
    ;
    ; Check uCode was loaded or not
    ;
    mov rcx, MCU_REV_MSR
    xor rax, rax
    xor rdx, rdx
    wrmsr
    mov rax, 1
    cpuid
    mov rcx, MCU_REV_MSR
    rdmsr
    or  rdx, rdx
    jnz uCode_loaded

    ;
    ; Load uCode update
    ;
    mov	rax, 1
    cpuid

    mov	rax, mMcuAddr
    lea	rax, [rax + SIZEOF MCU]	; RAX -> MCU data (after header)
    xor	rdx, rdx						   
    mov	rcx, MCU_LOAD_MSR       ; Trigger to load MCU

    wrmsr                       ; Load MCU
		   
    mov	rax, 1
    cpuid
uCode_loaded:        
;-----------------------------------------------------------------------------
;
;  Section:	Initial RLPs cache requirements
; 
;  Description:	Ensure CR0.CD and CR0.NW are cleared
; 							 
;-----------------------------------------------------------------------------
    ;
    ; Clear CR0.CD and CR0.NW
    ;
    mov	rax, cr0 
    and	rax, NOT (CR0_CD_MASK + CR0_NW_MASK)
    mov	cr0, rax 

;-----------------------------------------------------------------------------
;
;  Section:	Clean all MCi_STATUS MSR registers
; 
;  Description: MCA registers are cleaned
; 							 
;-----------------------------------------------------------------------------

    mov	rcx, MCG_CAP 
    rdmsr 
    movzx	rbx, al                 ; ebx = MCR bank count 
    xor	rax, rax                ; Write 0 into all MCi_STATUS registers
    xor	rdx, rdx 
    mov	rcx, MC0_STATUS 
 
McaErrorCleanLoopStart:	
    wrmsr 
    dec	rbx 
    jz	@F
    add	rcx, 4                  ; ecx = number of MSRs per bank
    jmp	McaErrorCleanLoopStart 

@@:  
    
    mov rcx,IA32_APIC_BASE
    rdmsr
    and rax,BIT11+BIT10
    cmp rax,BIT11+BIT10
    jne x2ApicDisabled
    mov rcx,EFI_MSR_EXT_XAPIC_LVT_THERM
    rdmsr
    and eax, NOT (B_INTERRUPT_MASK + B_DELIVERY_MODE + B_VECTOR)
    or  eax, (B_INTERRUPT_MASK + V_MODE_SMI)
    wrmsr
    jmp @f
x2ApicDisabled:
    ; mask thermal LVT
    mov	rcx, IA32_APIC_BASE
    rdmsr				; Get APIC Base
    and	rax, BASE_ADDR_MASK	; Just need the address
    mov	edx, [rax+LOCAL_APIC_THERMAL_DEF]
    and edx, NOT (B_INTERRUPT_MASK + B_DELIVERY_MODE + B_VECTOR)
    or  edx, (B_INTERRUPT_MASK + V_MODE_SMI)
    mov	DWORD PTR [rax+LOCAL_APIC_THERMAL_DEF], edx	;disable DTS SMIs
@@:
    POPA_64 
    popf 
        
    leave
    ret 0 
DoApInit	ENDP


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:	ApSaveConfig
; 
;  Input:	None
; 
;  Output:	None
; 
;  Registers:	All are preserved
; 
;  Description:	Saves AP configuration
; 
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
ApSaveConfig PROC FRAME
    START_FRAME
    END_FRAME

    pushf
    cli
    PUSHA_64

    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0FFh
    shl     rax, 1
    mov     rcx, rax

    lea     rbx, mApMtrrTab
 
SaveNextMtrr:               
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    rdmsr  
    mov     QWORD PTR [rbx+0], rax
    mov     QWORD PTR [rbx+8], rdx            
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    add     rbx, 10h
    loop    SaveNextMtrr

    mov     rcx, IA32_MTRR_DEF_TYPE
    rdmsr  
    mov     QWORD PTR [rbx+0], rax
    mov     QWORD PTR [rbx+8], rdx            
                                       
    lea     rbx, mApIdt
    sidt    [rbx]
    lea     rbx, mApCr4
    mov     rax, cr4
    mov     QWORD PTR [rbx], rax
    
    mov rcx,IA32_APIC_BASE
    rdmsr
    and rax,BIT11+BIT10
    cmp rax,BIT11+BIT10
    jne x2ApicDisabled
    mov rcx,EFI_MSR_EXT_XAPIC_LVT_THERM
    rdmsr
    lea rbx, mApSavedApicThermalValue
    mov	DWORD PTR [rbx], eax	; read and save thermal LVT
    jmp @f
x2ApicDisabled:

    mov	rcx, IA32_APIC_BASE
    rdmsr				; Get APIC Base
    and	rax, BASE_ADDR_MASK	; Just need the address
    lea rbx, mApSavedApicThermalValue
    mov edx, DWORD PTR [rax+LOCAL_APIC_THERMAL_DEF]
    mov	DWORD PTR [rbx], edx	; read and save thermal LVT
@@:
    mov	rcx, EFI_MSR_IA32_THERM_INTERRUPT
    rdmsr				;
    lea	rbx, mApSavedIa32ThermInterruptMSR 
    mov QWORD PTR [rbx+0], rax
    mov QWORD PTR [rbx+8], rdx            
    ;disable THERMAL INT
    and rax, NOT (BIT0+BIT1+BIT2+BIT4+BIT15+BIT23)
    wrmsr

    wbinvd
                     
    POPA_64 
    popf 

    leave
    ret 0 
ApSaveConfig	ENDP


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:	ApRestoreConfig
; 
;  Input:	None
; 
;  Output:	None
; 
;  Registers:	All are preserved
; 
;  Description:	Saves AP configuration
; 
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
ApRestoreConfig PROC FRAME
    START_FRAME
    END_FRAME

    pushf
    cli
    PUSHA_64          

    ;
    ; Disable cache
    ;
    mov	rax, cr0                ; set CR0:CD and CR0:NE, clear CR0:NW
    or	rax, CR0_CD_MASK OR CR0_NE_MASK 
    and	rax, NOT CR0_NW_MASK 
    mov	cr0, rax 
    wbinvd   
        
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0FFh
    shl     rax, 1
    mov     rcx, rax

    lea     rbx, mApMtrrTab
 
RestoreNextMtrr:               
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    mov     rax, QWORD PTR [rbx+0]  
    mov     rdx, QWORD PTR [rbx+8]  
    wrmsr
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    add     rbx, 10h
    loop    RestoreNextMtrr

    mov	rax, QWORD PTR [rbx+0]
    mov	rdx, QWORD PTR [rbx+8]
    mov	rcx, IA32_MTRR_DEF_TYPE 
    wrmsr 
        
    lea     rbx, mApIdt
    lidt    FWORD PTR [rbx]
    lea     rbx, mApCr4
    mov     rax, QWORD PTR [rbx]
    mov     cr4, rax

    mov	rcx, EFI_MSR_IA32_THERM_INTERRUPT
    lea	rbx, mApSavedIa32ThermInterruptMSR 
    mov rax, QWORD PTR [rbx+0]
    mov rdx, QWORD PTR [rbx+8]            
    wrmsr
    
    mov rcx,IA32_APIC_BASE
    rdmsr
    and rax,BIT11+BIT10
    cmp rax,BIT11+BIT10
    jne x2ApicDisabled
    mov rcx,EFI_MSR_EXT_XAPIC_LVT_THERM
    lea rbx, mApSavedApicThermalValue
    mov	eax,DWORD PTR [rbx] ;restore thermal LVT
    xor rdx,rdx
    wrmsr  
    jmp @f
x2ApicDisabled:

    mov	rcx, IA32_APIC_BASE
    rdmsr				; Get APIC Base
    and	rax, BASE_ADDR_MASK	; Just need the address
    lea rbx, mApSavedApicThermalValue
    mov	edx, DWORD PTR [rbx]
    mov	DWORD PTR [rax+LOCAL_APIC_THERMAL_DEF], edx	; restore thermal LVT
@@:
    ;
    ; Enable cache
    ;
    mov	rax, cr0 
    and	rax, NOT CR0_CD_MASK 
    mov	cr0, rax 

    POPA_64 
    popf 

    leave
    ret 0 
ApRestoreConfig	ENDP

_TEXT	ENDS
        
    END 
