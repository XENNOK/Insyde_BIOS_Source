;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;;
;; Abstract:
;;

          .MODEL small
          .586p
          .code

        INCLUDE INT15Rom.EQU
        
CodeStart:
          ORG  0
          db   55h, 0AAh
CodeSize  db   ((((offset CodeEnd) - (offset CodeStart)) / 512) + 1)
          ORG  3
          jmp  HookInt15

OldINT15SegOff     dw  ?, ?
INT15SmiPort       dw  ?
INT15SmiData       dw  ?
INT15FnEntry       dw  ?
MaxINT15FnCount    dw  ?
CpuRegisters       dw  ?
;Initialize INT15 option rom
; Input Registers:
;     Regs.X.AX = COMMON_INT15_SMI;    
;     Regs.X.CX = MAX_OEM_INT15_FN_COUNT;
;     Regs.X.DX = SW_SMI_PORT;    
;     Regs.X.DS = FunctionListSegment;
;     Regs.X.ES = CpuRegistersSegment;
HookInt15:
          cli
          push    eax
          push    ebx
          push    ecx
          push    edx 
          ; OEM INT15 data initialize
          mov     cs:[INT15SmiPort], ax
          mov     cs:[INT15SmiData], dx
          mov     cs:[MaxINT15FnCount], cx

          mov     ax, ds
          mov     cs:[INT15FnEntry], ax
          
          mov     ax, es
          mov     cs:[CpuRegisters], ax         
          
          ; Hook INT15
          push    cs 
          pop     dx
          shl     edx, 16
          push    offset INT15CallBack
          pop     dx
          mov     eax, 15h
          call    ChangeInterruptTable
          mov     dword ptr cs:OldINT15SegOff, eax
          
          pop     edx
          pop     ecx
          pop     ebx
          pop     eax
          sti
          retf

; Oem Int15 call back function
INT15CallBack:
          pushfd
          call    CompareInt15FunList
          jc      OriginalInt15Vector
          popfd
          call    SaveCpuRegisters
          call    CheckCpuMode
          jc      EXIT_INT15
          call    TriggerSmi
          call    RestoreCpuRegisters
EXIT_INT15:          
          iret
          
OriginalInt15Vector:
          popfd
          jmp     dword ptr cs:[OldINT15SegOff]

;****************************************************************************
;* CheckCpuMode                                                             *
;*                                                                          *
;* PURPOSE:  Check Cpu Mode                                                 *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  InSmmFlag                                                       *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: CY: If InSmmFlag = 055aah ,it means the caller in smm mode      *
;*              set InSmmFlag = 0aa55h and return                           *
;*          NC: Caller in normal Cpu mode, do smi                           *
;****************************************************************************
CheckCpuMode PROC NEAR
        push   es 
        push   ax
        mov    ax, cs:[CpuRegisters]
        mov    es, ax
        cmp    es:INT15_PRIVATE_INFO.InSmmFlag, 55aah
        jne    Normal_mode
        mov    es:INT15_PRIVATE_INFO.InSmmFlag, 0aa55h
        stc
        jmp    Smm_mode
Normal_mode:
        clc
Smm_mode:        
        pop    ax
        pop    es 
        ret
CheckCpuMode ENDP

;****************************************************************************
;* CompareInt15FunList                                                      *
;*                                                                          *
;* PURPOSE:  Save CpuRegisters to CpuRegs                                   *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  AX  - Interrupt service number                                  *
;*          EDX - New Interrupt Segment:Offset                              *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: EAX - Original Interrupt Segment:offset                         *
;****************************************************************************
CompareInt15FunList PROC NEAR
        pushad
        push    ds 
        
        mov     cx, cs:[MaxINT15FnCount] 
        push    cs:[INT15FnEntry]      
        pop     ds
        xor     si, si
        
SearchNextInt15Fun:  
        cmp     word ptr ds:[si], 0
        je      NoMergeFun
        cmp     ax, word ptr ds:[si]
        je      FindMergeInt15Fun
        add     si, 2
        loop    SearchNextInt15Fun
        
NoMergeFun:
        stc
        jmp     Compare_Exit
        
FindMergeInt15Fun: 
        clc
        
Compare_Exit:
        pop     ds
        popad
        ret
CompareInt15FunList ENDP

;****************************************************************************
;* TriggerSmi                                                               *
;*                                                                          *
;* PURPOSE:  Save CpuRegisters to CpuRegs                                   *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  AX  - Interrupt service number                                  *
;*          EDX - New Interrupt Segment:Offset                              *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: EAX - Original Interrupt Segment:offset                         *
;****************************************************************************
TriggerSmi PROC NEAR
        push   dx 
        push   ax 
        
        mov    ax, cs:[INT15SmiPort]
        mov    dx, cs:[INT15SmiData]

        out    dx, al
        out    0edh, al ;IO dummy for Smi trigger synchronous

        pop    ax
        pop    dx 
        ret
TriggerSmi ENDP

;****************************************************************************
;* SaveCpuRegisters                                                         *
;*                                                                          *
;* PURPOSE:  Save CpuRegisters to CpuRegs                                   *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  All Cpu Registers                                               *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: None                                                            *
;****************************************************************************
SaveCpuRegisters PROC NEAR
        push    es  
        push    cs:[CpuRegisters]
        pop     es   
        
        mov     es:EFI_DWORD_REGS.RegEAX, eax
        mov     es:EFI_DWORD_REGS.RegEBX, ebx
        mov     es:EFI_DWORD_REGS.RegECX, ecx
        mov     es:EFI_DWORD_REGS.RegEDX, edx
        mov     es:EFI_DWORD_REGS.RegESI, esi 
        mov     es:EFI_DWORD_REGS.RegEDI, edi
        
        pushfd
        pop     es:EFI_DWORD_REGS.RegEFlags

        push    cs
        pop     es:EFI_DWORD_REGS.RegCS
        push    ss
        pop     es:EFI_DWORD_REGS.RegSS
        push    ds
        pop     es:EFI_DWORD_REGS.RegDS
        push    fs
        pop     es:EFI_DWORD_REGS.RegFS
        push    gs
        pop     es:EFI_DWORD_REGS.RegGS    
        push    ebp
        pop     es:EFI_DWORD_REGS.RegEBP
        push    esp
        pop     es:EFI_DWORD_REGS.RegESP        
        pop     es

        push    ds 
        push    cs:[CpuRegisters]
        pop     ds   
        push    es
        pop     ds:EFI_DWORD_REGS.RegES
        pop     ds 
        
        ret
SaveCpuRegisters ENDP

;****************************************************************************
;* RestoreCpuRegisters                                                      *
;*                                                                          *
;* PURPOSE:  Restore CpuRegisters from CpuRegs                              *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  ES:SI  - Point to CpuRegs                                       *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: All Cpu registers                                               *
;****************************************************************************
RestoreCpuRegisters PROC NEAR
    
        push    cs:[CpuRegisters]
        pop     es   

        mov     eax, es:EFI_DWORD_REGS.RegEAX
        mov     ebx, es:EFI_DWORD_REGS.RegEBX
        mov     ecx, es:EFI_DWORD_REGS.RegECX
        mov     edx, es:EFI_DWORD_REGS.RegEDX
        mov     esi, es:EFI_DWORD_REGS.RegESI
        mov     edi, es:EFI_DWORD_REGS.RegEDI

        push    es:EFI_DWORD_REGS.RegSS
        pop     ss
        push    es:EFI_DWORD_REGS.RegDS
        pop     ds
        push    es:EFI_DWORD_REGS.RegFS
        pop     fs
        push    es:EFI_DWORD_REGS.RegGS 
        pop     gs  
        push    es:EFI_DWORD_REGS.RegEBP
        pop     ebp      
        push    es:EFI_DWORD_REGS.RegEFlags
        popfd
;        
; Can't restore CS and ESP
;
;         push    (EFI_DWORD_REGS PTR es:[si]).RegCS
;         pop     cs        
;         push    es:EFI_DWORD_REGS.RegESP
;         pop     esp 
        push    es:EFI_DWORD_REGS.RegES
        pop     es
        
        ret
RestoreCpuRegisters ENDP

;****************************************************************************
;* ChangeInterruptTable                                                     *
;*                                                                          *
;* PURPOSE:  Change Interrupt table                                         *
;* DESCRIPTION:                                                             *
;* USES:                                                                    *
;* PASSED:  AX  - Interrupt service number                                  *
;*          EDX - New Interrupt Segment:Offset                              *
;* SPECIAL: NOTHING.                                                        *
;* RETURNS: EAX - Original Interrupt Segment:offset                         *
;****************************************************************************
ChangeInterruptTable PROC NEAR
        push    es
        push    0
        pop     es
        shl     eax, 2
        push    dword ptr es:[eax]
        mov     dword ptr es:[eax], edx
        pop     eax
        pop     es
        ret
ChangeInterruptTable ENDP

CodeEnd:
        END
