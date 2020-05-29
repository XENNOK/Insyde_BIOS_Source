;/** @file
;  This file contains code to launch BIOS ACM functions in DXE phase
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
;   TxtDxeBsp.asm
; 
; Abstract:
; 
;   This file contains code to launch BIOS ACM functions in DXE phase
;
;--*/
	.xlist 
	include	txt.inc 
	include	mmx64.inc 
	.list 


	_TEXT	SEGMENT
        
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:	LaunchBiosAcm
; 
;  Input:	AcmBase - Base address of LT BIOS ACM
;               Function - function number to execute
; 
;  Output:	None
; 
;  Registers:	None are preserved
; 
;  Description:	Setup GETSEC environment (protected mode, mtrrs, etc) and
;		  invoke GETSEC:ENTERACCS with requested BIOS ACM entry point.
; 
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

LaunchBiosAcm PROC FRAME

        START_FRAME
        MAKE_LOCAL AcmBase:QWORD
        MAKE_LOCAL Funct:QWORD
        MAKE_LOCAL BIOS_GDT[2]:QWORD
        MAKE_LOCAL CompModeSel:QWORD
        MAKE_LOCAL CompModeOff:QWORD
        MAKE_LOCAL LongModeSel:QWORD
        MAKE_LOCAL LongModeOff:QWORD
        MAKE_LOCAL SavedCR3:QWORD
        MAKE_LOCAL SavedCR4:QWORD
        MAKE_LOCAL SavedDS:QWORD
        MAKE_LOCAL SavedES:QWORD
        MAKE_LOCAL SavedGS:QWORD
        MAKE_LOCAL SavedFS:QWORD
        MAKE_LOCAL SavedMiscEnablesRax:QWORD
        MAKE_LOCAL SavedMiscEnablesRdx:QWORD
        MAKE_LOCAL MtrrTab[2*(IA32_MTRR_PHYSMASK9 - IA32_MTRR_PHYSBASE0 + 1)]:QWORD
        MAKE_LOCAL ACM_SIZE_TO_CACHE:QWORD
        MAKE_LOCAL ACM_BASE_TO_CACHE:QWORD
        MAKE_LOCAL NEXT_MTRR_INDEX:QWORD
        MAKE_LOCAL NEXT_MTRR_SIZE:QWORD
        MAKE_LOCAL MTRR_COUNT:QWORD
        MAKE_LOCAL MtrrTypeRax:QWORD
        MAKE_LOCAL MtrrTypeRdx:QWORD
        END_FRAME

        ;
        ; Save input parameters
        ;
        mov     AcmBase, rcx
        
        mov     Funct, rdx                                                               

	sgdt	BIOS_GDT                ; save gdtr
 
        ;
        ; Save the general purpose register state
        ;
        pushf
        cli
        
	PUSHA_64
        ;
        ; Tell where we are
        ;
        in      ax, 80h 
        mov     ah, BYTE PTR Funct 
        or      ah, PORT80_CODE_PREFIX
        out     80h, ax
        ;
        ; Save segment registers.
        ;
        mov     ax, ds
        mov     SavedDS, rax
        mov     ax, es
        mov     SavedES, rax
        mov     ax, gs
        mov     SavedGS, rax
        mov     ax, fs
        mov     SavedFS, rax

        ; Save cr4 
        ;
        mov     rax, cr4                
        mov     SavedCR4, rax

        ;
        ; Save IA32_MISC_ENABLES MSR
        ;      
	mov	rcx, IA32_MISC_ENABLE_MSR
        rdmsr 
        mov     SavedMiscEnablesRax, rax
        mov     SavedMiscEnablesRdx, rdx
        
;-----------------------------------------------------------------------------
;
;  Section:	Save variable MTRRs
; 
;  Description:	All variable MTRRs are saved in local variables.
;               They will be restored after runnong of BIOS ACM
; 							 
;-----------------------------------------------------------------------------
        mov     rcx, IA32_MTRR_CAP
	rdmsr
        and     rax, 0FFh
        shl     rax, 1
        mov     MTRR_COUNT, rax       
        mov     rcx, rax
        lea     rbx, MtrrTab
 
SaveNextMtrr:               
        add     rcx, IA32_MTRR_PHYSBASE0 - 1
	rdmsr  
        mov     [rbx+0], rax
        mov     [rbx+8], rdx            
        sub     rcx, IA32_MTRR_PHYSBASE0 - 1
        add     rbx, 10h
        loop    SaveNextMtrr

        ;
        ; Save IA32_MTRR_DEF_TYPE MSR
        ;   
        mov     rcx, IA32_MTRR_DEF_TYPE
        rdmsr
        mov     MtrrTypeRax, rax
        mov     MtrrTypeRdx, rdx
        
;-----------------------------------------------------------------------------
;
;  Section:	Program MTRRs
; 
;  Description:	Variable MTRRs are programmed to cache ACM as WB
; 							 
;-----------------------------------------------------------------------------
        ;
        ; Enable SMXE, SSE and debug extensions 
        ;
	mov	rax, cr4 
	or	rax, CR4_OSFXSR + CR4_DE + CR4_SMXE 
	mov	cr4, rax 
 
        ;
        ; Disable cache
        ;
	mov	rax, cr0                ; set CR0:CD and CR0:NE, clear CR0:NW
	or	rax, CR0_CD_MASK OR CR0_NE_MASK 
	and	rax, NOT CR0_NW_MASK 
	mov	cr0, rax 
	wbinvd   
        ;
        ; Disable MTRRs, set Default Type to UC
        ;
	mov	rcx, IA32_MTRR_DEF_TYPE 
	xor	rax, rax 
	xor	rdx, rdx 
	wrmsr 

        ;
        ; Clear all of the Variable MTRRs
        ;                      
        mov     rcx, MTRR_COUNT

ClearNextMttr:        
        add     rcx, IA32_MTRR_PHYSBASE0 - 1
        wrmsr
        sub     rcx, IA32_MTRR_PHYSBASE0 - 1
        loop    ClearNextMttr

        ;
        ; Determine size of AC module
        ;
        mov     rsi, AcmBase
        xor     rax, rax
        mov     eax, DWORD PTR [rsi+ACM_HEADER.AcmSize]
        shl     rax, 2                  ;  ...in bytes (ACM header has size in dwords)
        ;
        ; Round up to page size
        ;
        add     rax, 0FFFh              ; 
        and     rax, 0FFFFF000h         ; Aligned to a page (4KB)

        ;
        ; Program MTRRs to cover BIOS ACM
        ;
        sub	rcx, rcx
	mov	NEXT_MTRR_INDEX, rcx	; Start from MTRR0

	;
	; Save remaining size to cache
	;
	mov	ACM_SIZE_TO_CACHE, rax	; Size of ACM that must be cached
	mov	ACM_BASE_TO_CACHE, rsi	; Base ACM address
        
nextMtrr:
	;
	; Get remaining size to cache
	;
	mov	rax, ACM_SIZE_TO_CACHE
        and     rax, rax
	jz	done			; If no left size - we are done
        ;
        ; Determine next size to cache.
        ; We start from bottom up. Use the following algorythm:
        ; 1. Get our own alignment. Max size we can cache equals to our alignment
        ; 2. Determine what is bigger - alignment or remaining size to cache.
        ;    If aligment is bigger - cache it.
        ;      Adjust remaing size to cache and base address
        ;      Loop to 1.
        ;    If remaining size to cache is bigger
        ;      Determine the biggest 2^N part of it and cache it.
        ;      Adjust remaing size to cache and base address
        ;      Loop to 1.
        ; 3. End when there is no left size to cache or no left MTRRs
        ;
        mov     rsi, ACM_BASE_TO_CACHE
        bsf     rcx, rsi                ; Get index of lowest bit set in base address
	;
	; Convert index into size to be cached by next MTRR
	;
	mov	rdx, 1h
	shl	rdx, cl			; Alignment is in rdx
        cmp     rdx, rax                ; What is bigger, alignment or remaining size?
        jbe     gotSize                 ; JIf aligment is less
        ;
        ; Remaining size is bigger. Get the biggest part of it, 2^N in size
        ;
	bsr	rcx, rax		; Get index of highest set bit
	;
	; Convert index into size to be cached by next MTRR
	;
	mov	rdx, 1
	shl	rdx, cl			; Size to cache
        
gotSize:
        mov     rax, rdx
	mov	NEXT_MTRR_SIZE, rax	; Save

	;
	; Compute MTRR mask value:  Mask = NOT (Size - 1)
	;						      
	dec	rax			; eax - size to cache less one byte
	not	rax			; eax contains low 32 bits of mask
	or	rax, MTRR_VALID		; Set valid bit
	;
	; Program mask register
	;
	mov	rcx, IA32_MTRR_PHYSMASK0 ; setup variable mtrr
	mov	rbx, NEXT_MTRR_INDEX
	add	rcx, rbx
	
	mov	rdx, 0Fh                
	wrmsr
	;
	; Program base register
	;
	sub	rdx, rdx
	mov	rcx, IA32_MTRR_PHYSBASE0 ; setup variable mtrr
	add	rcx, rbx		; ebx is still NEXT_MTRR_INDEX
	
	mov	rax, ACM_BASE_TO_CACHE
	or	rax, WB			; set type to write back
	wrmsr	
	;
	; Advance and loop
	; Reduce remaining size to cache
	;
	mov	rbx, ACM_SIZE_TO_CACHE
	mov	rax, NEXT_MTRR_SIZE
	sub	rbx, rax
	mov	ACM_SIZE_TO_CACHE, rbx

	;
	; Increment MTRR index
	;
	mov	rbx, NEXT_MTRR_INDEX
	add	rbx, 2
	mov	NEXT_MTRR_INDEX, rbx
	;
	; Increment base address to cache
	;
	mov	rbx, ACM_BASE_TO_CACHE 
	mov	rax, NEXT_MTRR_SIZE
	add	rbx, rax
	mov	ACM_BASE_TO_CACHE, rbx 

	jmp	nextMtrr

done:	
        ;
        ; Enable Variable MTRRs
        ;
	xor	rdx, rdx 
	mov	rax, MTRR_ENABLE 
	mov	rcx, IA32_MTRR_DEF_TYPE 
	wrmsr 
        ;
        ; Enable cache
        ;
	mov	rax, cr0 
	and	rax, NOT CR0_CD_MASK 
	mov	cr0, rax 

        ;
        ; Clean all MCi_STATUS MSR registers
        ; SCLEAN will generate GPF otherwise
        ;
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
;-----------------------------------------------------------------------------
;
;  Section:	Find Compatible Segment Descriptor in GDT
; 
;  Description:	GDT is scanned until code descriptor with L bit = 0 is found
; 							 
;-----------------------------------------------------------------------------
                 
        lea     rax, BIOS_GDT
        add     rax, 2                  ; Point to base
        mov     rax, [rax]              ; Get base of GDT
        mov     rbx, rax                ; Save GDT base 

        mov     cx, WORD PTR BIOS_GDT
        movzx   rcx, cx
        inc     rcx                     ; rcx - total size of GDT in bytes
        add     rcx, rax                ; rcx - end of GDT
         
loopStart:                                                           
        cmp     QWORD PTR [rax], 0      ; Reserved?
        je      ApplicationDescriptor
        test    BYTE PTR [rax].SEG_DESCRIPTOR.AR0_7, MASK D_T
        jz      nextDescriptor          ; JIf system descriptor
        ;                   
        ; Application descriptor found
        ;                             
        test    BYTE PTR [rax].SEG_DESCRIPTOR.AR0_7, 8 ; Bit 3 of sType - code segment if set
        jz      nextDescriptor          ; JIf data descriptor
        ;
        ; Code descriptor is found
        ;                         
        test    byte ptr [rax].SEG_DESCRIPTOR.LAR16_23, MASK L
        jnz     nextDescriptor
        ;
        ; Compatibility mode code descriptor is found
        ;       
        sub     rax, rbx                ; rax is compatibility segment selector
        jmp     CompatibilityModeJump
        
nextDescriptor:
        cmp     rax, rcx
        jb      @F
        jmp     $                       ; Nothing found - impossible situation

@@:                                                                   
        test    BYTE PTR [rax].SEG_DESCRIPTOR.AR0_7, MASK D_T
        jz      @F

ApplicationDescriptor:        
        ;
        ; Application descriptor - 8 bytes
        ;                                 
        add     rax, 8
        jmp     loopStart
        
@@:
        add     rax, 16
        jmp     loopStart
        
;-----------------------------------------------------------------------------
;
;  Section:	Jump to compatibility mode
; 
;  Description:	Found selector and known label offset
;               are used to transfer control to compatibility mode.
;
;               NOTE!
;               Code programmed from this point on till the return to long mode
;               looks differently than code actually executed by CPU
;               This is because assembler is in x64 mode whereas CPU is not.
;               Whereever differences are present between written and executed code,
;               actual instructions are shown in comments.
;
;               Example1:       Programmed:     mov     rax, cr4
;                               Executed:       mov     eax, cr4
;               Assembler fails to assemble     "mov    eax, cr4" since CR4 is
;               64 bits wide in x64 mode. Generated opcodes are nevertheless
;               correct.                
;
;               Example2:       Programmed:     mov     eax, DWORD PTR [rbx+0]
;                               Executed:       mov     eax, DWORD PTR [ebx+0]
;               Default addressing in x64 mode is 64 bit. If ebx were coded
;               in this example, assembler would generate unneeded REX prefix.
;               By programming rbx this prefix is not generated and 
;               opcode corresponds to ebx addressing in compatibility mode
; 		        				 
;-----------------------------------------------------------------------------
        
CompatibilityModeJump:
        ;
        ; Save Long mode and Compatibility mode selectors and offsets before transition
        ;
        mov     CompModeSel, rax        ; Save Compatibility Mode selector
        mov     rcx, OFFSET ProtectedMode2
        mov     CompModeOff, rcx
        mov     cx, cs
        movzx   rcx, cx
        mov     LongModeSel, rcx
        mov     rcx, OFFSET LongMode2
        mov     LongModeOff, rcx
                
        shl     rax, 32
        mov     rcx, OFFSET CompatibilityMode
        or      rax, rcx
        push    rax
        retf

CompatibilityMode:
;-----------------------------------------------------------------------------
;
;  Section:	Jump to protected mode
; 
;  Description:	Compatibility mode is disabled since BIOS ACM must run 
;               in protected 32 bit mode.
; 							 
;-----------------------------------------------------------------------------

        ;
        ; Disable paging
        ;
	mov	rcx, cr0 
	and	ecx, DWORD PTR (NOT BIT31) 
	mov	cr0, rcx 
        ;
        ; Clear EFER.LME
        ;
	mov	ecx, IA32_EFER_MSR 
	rdmsr 
	and     eax, NOT LME 
	wrmsr 
	jmp	ProtectedMode

ProtectedMode:

;-----------------------------------------------------------------------------
;
;  Section:	Launch BIOS ACM
; 
;  Description:	Prepare and execute GETSEC[ENTERACCS]
; 							 
;-----------------------------------------------------------------------------

        ;
        ; Save return values in MMX registers
        ;     
        
        mov     ax, ss
        movzx   eax,ax
%       _movd   SAVED_SS, eax
%       _movd   SAVED_EBP, ebp
%       _movd   SAVED_ESP, esp
        lea     eax, BIOS_GDT
%       _movd   SAVED_GDT, eax
        mov     rax, cr3                ; mov eax, cr3    - in 32 bit mode
        mov     DWORD PTR SavedCR3, eax
        ;                         
        ; Call GETSEC[ENTERACCS]
        ;
	mov	esi, DWORD PTR Funct    ; esi = ACM function
	mov	eax, DWORD PTR AcmBase
	mov	ebx, eax                ; ebx = AcmBase
	mov	ecx, DWORD PTR [rbx+ACM_HEADER.AcmSize] ; mov DWORD PTR [ebx+ACM_HEADER.AcmSize] - in 32 bit mode
                                                        ; ecx = size of ACM in dwords
	shl	ecx, 2                  ; ecx = size of ACM in bytes
	xor	edx, edx 
	xor	edi, edi 
	mov	eax, ENTERACCS          ; eax = ENTERACCS

	 _GETSEC

;for debugging only
;	mov	ax, 055AAh
;	out	80h, ax
;;    jmp     $
;-----------------------------------------------------------------------------
;
;  Section:	Restore protected mode environment
; 
;  Description:	Since BIOS ACM changes GDT,
;               BIOS GDT, stack and and CS selector are restored.
;               
; 							 
;-----------------------------------------------------------------------------

        ;
        ; Reload the GDTR. Upon return CPU is loaded with selector from ACM GDT
        ; The following instruction works simply because whatever CS selector is
        ; currently, it is flat selector.
        ;
%       _movd eax, SAVED_GDT           
	lgdt	FWORD ptr [rax]         ; lgdt	FWORD ptr [eax] - in 32 bit mode

        ;
        ; Restore the stack
        ;
%	_movd   eax, SAVED_EBP
	mov	ebp, eax                ; restore ebp
%	_movd   eax, SAVED_ESP 
	mov	esp, eax                ; restore esp
%	_movd   eax, SAVED_SS
	mov	ss, ax                  ; restore ss

        ;
        ; Reload cs register 
        ;       
        mov     eax, DWORD PTR CompModeSel
        push    rax                     ; push eax - in 32 bit mode

        mov     eax, DWORD PTR CompModeOff
        push    rax                     ; push eax - in 32 bit mode
        retf                            ; will jump to Protected label below

ProtectedMode2:

;-----------------------------------------------------------------------------
;
;  Section:	Restore MTRRs
; 
;  Description:	BIOS MTRR values are restored.
; 							 
;-----------------------------------------------------------------------------
        ;                                    
        ; Disable paging
        ;
        mov     rax, cr0
        and     eax, DWORD PTR (NOT CR0_PG_MASK)
        mov     cr0, rax

        mov     rcx, MTRR_COUNT
        lea     ebx, MtrrTab
 
RestoreNextMtrr:               
        add     ecx, IA32_MTRR_PHYSBASE0 - 1
        mov     eax, DWORD PTR [rbx+0]  ; mov eax, DWORD PTR [ebx+0] - in 32 bit mode
        mov     edx, DWORD PTR [rbx+8]  ; mov eax, DWORD PTR [ebx+8] - in 32 bit mode
        wrmsr
        sub     ecx, IA32_MTRR_PHYSBASE0 - 1
        add     ebx, 10h
        loop    RestoreNextMtrr

        mov     rcx, IA32_MTRR_DEF_TYPE
        mov     rdx, MtrrTypeRdx
        mov     rax, MtrrTypeRax 
        wrmsr 
;-----------------------------------------------------------------------------
;
;  Section:	Switch to compatibility mode
; 
;  Description:	Compatibility mode i srestored by enabling of paging -
;               this is done by restoring CR4 contenxt, and setting of LME bit.
; 							 
;-----------------------------------------------------------------------------
        
        ;               
        ; Enable PAE in CR4
        ;
        mov     eax, DWORD PTR SavedCR4
        or      eax, CR4_PAE
        mov     cr4, rax 

        ;
        ; Reload CR3
        ;
        mov     eax, DWORD PTR SavedCR3
        mov     cr3, rax 

        ;
        ; Set EFER.LME to re-enable ia32-e
        ;
        mov     ecx, IA32_EFER_MSR
        rdmsr
	or	eax, LME 
	wrmsr 
        ;
        ; Enable paging
        ;
	mov	rax, cr0 
	or	eax, CR0_PG_MASK
        mov     cr0, rax

	jmp	CompatibilityMode2

CompatibilityMode2:
	wbinvd                          ; Flush and invalidate the cache
        ;
        ; Now we're in Compatibility mode - restore segment registers.
        ;
        mov     rax, SavedDS
        mov     ds, ax
        mov     rax, SavedES
        mov     es, ax
        mov     rax, SavedGS
        mov     gs, ax
        mov     rax, SavedFS
        mov     fs, ax      
        ;
        ; Reastore IA32_MISC_ENABLES MSR
        ;      
        mov     rcx, IA32_MISC_ENABLE_MSR
        mov     rdx, SavedMiscEnablesRdx
        mov     rax, SavedMiscEnablesRax
        wrmsr 
        
        
;-----------------------------------------------------------------------------
;
;  Section:	Switch to long mode
; 
;  Description:	Previously saved selector and offset are used to return 
;               CPU to long mode.
; 							 
;-----------------------------------------------------------------------------
        ;
        ; Reload cs register 
        ;
        mov     eax, DWORD PTR LongModeSel        
        push    rax                     ; push eax - in 32 bit mode

        mov     eax, DWORD PTR LongModeOff
        push    rax                     ; push eax - in 32 bit mode
	retf 

LongMode2:
;-----------------------------------------------------------------------------
;
;  Section:	Resore registers, stack and exit.
; 
;  Description:	Previously saved registers are restored. Stack is restored
;               by execution leave instruction and control is returned to
;               caller.
;
;               NOTE!
;               This section ends differences between programmed and
;               executed code.
; 							 
;-----------------------------------------------------------------------------
        ;
        ; Now we're in Long Mode
        ; Restore control registers
        ;
	mov	rax, SavedCR4 
	mov	cr4, rax 
 
	POPA_64
        popf
        
        leave
	ret 0 

LaunchBiosAcm	ENDP

_TEXT	ENDS
        
	END 
