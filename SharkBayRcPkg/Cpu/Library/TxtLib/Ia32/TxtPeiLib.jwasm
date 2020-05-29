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
;  Copyright (c) 2012 Intel Corporation. All rights reserved
;  This software and associated documentation (if any) is furnished
;  under a license and may only be used or copied in accordance
;  with the terms of the license. Except as permitted by such
;  license, no part of this software or documentation may be
;  reproduced, stored in a retrieval system, or transmitted in any
;  form or by any means without the express written consent of
;  Intel Corporation.
; 
; 
; Module Name:
; 
;   TxtPeiLib.asm
; 
; Abstract:
; 
;   This file contains the code to determine Processor/Chipset TXT capaiblity
;
;--*/

    .XLIST
    include Txt.inc
    .LIST

    .686P
    .MMX
    .XMM
    .MODEL FLAT,C
    .CODE

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:	CheckSmxCapabilities
; 
;  Input:	None
; 
;  Output:	None
; 
;  Registers:	None are preserved
; 
;  Description:	Execute GETSEC[CAPABILITIES] to report the SMX capabilities
; 
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

CheckSmxCapabilities PROC PUBLIC USES ebx

    ;
    ; Check whether SMX is supported
    ;
    mov     eax, 1
    cpuid
    bt      ecx, IA32_CPUID_SMX_B

    .IF !CARRY?
        mov     eax, 0
        jmp     @F
    .ENDIF

    ;
    ; Save cr4 
    ;
    mov     edx, CR4

    ;
    ; Enable SMXE
    ;
    mov     eax, CR4
    or      eax, CR4_SMXE
    mov     CR4, eax

    ;
    ; Call GETSEC[CAPABILITIES]
    ;
    mov     eax, CAPABILITIES        ; eax = CAPABILITIES
    mov     ebx, 0
    _GETSEC

    ;
    ; Restore cr4 
    ;
    mov     CR4, edx

@@:
    ret
CheckSmxCapabilities ENDP

END
