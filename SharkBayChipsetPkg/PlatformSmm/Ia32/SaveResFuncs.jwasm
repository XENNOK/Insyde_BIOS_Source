;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

  .686
  .MODEL FLAT,C
  .CODE

;------------------------------------------------------------------------------
;  VOID
;  SaveCR0 (
;    OUT   UINT32             *CR0Reg   // ecx
;    )
;------------------------------------------------------------------------------
SaveCR0  PROC NEAR  PUBLIC

    mov   eax, CR0
    mov   [ecx], eax

    ret
SaveCR0  ENDP

;------------------------------------------------------------------------------
;  VOID
;  ResCR0 (
;    IN   UINT32              CR0Reg    // ecx
;    )
;------------------------------------------------------------------------------
ResCR0  PROC NEAR  PUBLIC

    mov   eax, CR0
    and   eax, 9fffffffh
    or    eax, ecx
    mov   CR0, eax

    ret
ResCR0  ENDP

END
