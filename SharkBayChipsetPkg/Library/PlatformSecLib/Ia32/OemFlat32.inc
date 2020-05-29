;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

DEFAULT_PCI_BUS_NUMBER_PCH        EQU        00t
PCI_DEVICE_NUMBER_PCH_LPC         EQU        31t
PCI_FUNCTION_NUMBER_PCH_LPC       EQU        00t

R_PCH_LPC_PMIR                    EQU        0ACh
B_PCH_LPC_PMIR_CF9GR              EQU        100000h  ;BIT20
;[-start-120323-IB06460379-add];
R_XCMOS_INDEX                     EQU        072h
R_XCMOS_DATA                      EQU        073h
;[-end-120323-IB06460379-add];
MEFlashReset                      EQU        2Dh
V_ME_GLOBAL_RESET                 EQU        0F1h
V_ME_NO_RESET                     EQU        0

InitPort80    MACRO
  mov     esi, PCH_RCRB_BASE + PCH_RCRB_GCS
  mov     ebx, Dword Ptr [esi]
  or      bl, BIT5

  ;
  and     bl, NOT (BIT2)      ; Port80h to LPC

  mov     Dword Ptr [esi], ebx
ENDM

ResetPatch    MACRO
  ;
  ; System need full reset when flash bios,
  ; to ensure ME status
  ;
  mov     dx, R_XCMOS_INDEX
  mov     ax, MEFlashReset
  out     dx, ax
  mov     dx, R_XCMOS_DATA
  in      al, dx
  cmp     al, V_ME_GLOBAL_RESET
  jnz     NotFlashBIOS
  ;
  ; set MEFlashReset value is no reset
  ;
  mov     dx, R_XCMOS_INDEX
  mov     ax, MEFlashReset
  out     dx, ax
  mov     dx, R_XCMOS_DATA
  mov     ax, V_ME_NO_RESET
  out     dx, ax
  ;
  ; issue global reset
  ;
  mov     eax, 80000000h  
  add     eax, (DEFAULT_PCI_BUS_NUMBER_PCH  SHL 16t) or \
               (PCI_DEVICE_NUMBER_PCH_LPC   SHL 11t) or \
               (PCI_FUNCTION_NUMBER_PCH_LPC SHL 08t) or \
               (R_PCH_LPC_PMIR              SHL 00t)
  mov     dx, 0CF8h
  out     dx, eax
  mov     dx, 0CFCh
  mov     eax,0
  or      eax,B_PCH_LPC_PMIR_CF9GR
  out     dx, eax
  mov     dx, 0CF9h
  mov     al, 0Eh
  out     dx, al   
  jmp     $

NotFlashBIOS:
  mov     eax, 80000000h  
  add     eax, (DEFAULT_PCI_BUS_NUMBER_PCH  SHL 16t) or \
               (PCI_DEVICE_NUMBER_PCH_LPC   SHL 11t) or \
               (PCI_FUNCTION_NUMBER_PCH_LPC SHL 08t) or \
               (R_PCH_LPC_PMIR              SHL 00t)  
  mov     dx, 0CF8h
  out     dx, eax
  mov     dx, 0CFCh
  in      eax, dx
  and     eax, NOT B_PCH_LPC_PMIR_CF9GR
  out     dx, eax

  ;
  ; Workaround for warm reset issue
  ;
  mov     eax, 800000DCh
  mov     dx, 0CF8h
  out     dx, eax
  mov     dx, 0CFCh
  in      eax, dx
  shr     eax, 24
  cmp     al, 0FFh
  jne     Normal
  mov     dx, 0CF9h
  mov     al, 6
  out     dx, al      

Normal:
ENDM

PEGWorkaround MACRO
  ;
  ; Workaround for PEG.
  ; Set dGPU_HOLD_RST# (GPIO50) to inactive to avoid delay time.
  ;
  ; GPIO_USE_SEL2 Register -> 1 = GPIO 0 = Native
  ;
  mov   dx,   GPIO_BASE_ADDRESS + R_GPIO_USE_SEL2
  in    eax,  dx
  or    eax,  040000h		;Enable GPIO50
  out   dx,   eax
  ;
  ; GP_IO_SEL2 Register -> 1 = Input 0 = Output (if Native Mode don't care)
  ;
  mov   dx,   GPIO_BASE_ADDRESS + R_GPIO_IO_SEL2
  in    eax,  dx
  and   eax,  0FFFBFFFFh	;Configure GPIO50 as Output
  out   dx,   eax
  
  mov   dx,   GPIO_BASE_ADDRESS + R_GPIO_LVL2
  in    eax,  dx
  or    eax,  040000h		;Configure GPIO50 as High
  out   dx,   eax
ENDM
