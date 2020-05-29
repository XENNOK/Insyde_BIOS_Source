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
;; Module Name:
;;
;;   CallSmmSecureBoot.asm
;;
;; Abstract:
;;
;;   64 bit Sent SMI to call Secure Boot function
;;

TITLE  CallSmmSecureBoot.asm

text  SEGMENT


;Argument 1: rcx
;Argument 2: rdx
;Argument 3: r8
;Argument 4: r9

;------------------------------------------------------------------------------
;  UINT8
;  SmmSecureBootCall (
;    IN     UINT8            *InPutBuff,       // rcx
;    IN     UINTN            DataSize,         // rdx
;    IN     UINT8            SubFunNum,        // r8
;    IN     UINT16           SmiPort           // r9
;    );
;------------------------------------------------------------------------------
SmmSecureBootCall PROC       PUBLIC
          push  rbx
          push  rdi
          push  rsi
          push  r8

          mov   rsi, rcx
          mov   rbx, rdx
          mov   al,  r8b
          mov   ah,  al
          mov   al,  0ECh
          mov   rdx, r9
          out   dx,  al

          ;AL Fun ret state

          pop   r8
          pop   rsi
          pop   rdi
          pop   rbx
          ret
SmmSecureBootCall  ENDP

text  ENDS
END
