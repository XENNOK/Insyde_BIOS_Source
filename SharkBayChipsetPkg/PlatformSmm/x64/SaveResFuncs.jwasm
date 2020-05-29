;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

text    SEGMENT

;------------------------------------------------------------------------------
;  VOID
;  SaveCR0 (
;    OUT   UINT32             *CR0Reg   // rcx
;    )
;------------------------------------------------------------------------------
SaveCR0  PROC    PUBLIC

    mov   rax, CR0
    mov   [rcx], eax

    ret
SaveCR0  ENDP

;------------------------------------------------------------------------------
;  VOID
;  ResCR0 (
;    IN   UINT32              CR0Reg    // rcx
;    )
;------------------------------------------------------------------------------
ResCR0  PROC    PUBLIC

    mov   rax, CR0
    and   eax, 9fffffffh
    or    eax, ecx
    mov   CR0, rax

    ret
ResCR0  ENDP

text  ENDS
END
