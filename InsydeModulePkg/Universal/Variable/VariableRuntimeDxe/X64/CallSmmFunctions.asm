;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
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
;;   CallSmmIhisi.asm
;;
;; Abstract:
;;
;;   64 bit Sent SMI to call IHISI VATS function
;;

TITLE   CallSmmIhisi.asm

text  SEGMENT

IHISI_SIGNATURE EQU      02448324Fh

;Argument 1: rcx
;Argument 2: rdx
;Argument 3: r8
;Argument 4: r9

;------------------------------------------------------------------------------
;  UINT8
;  IhisiVatsCall (
;    IN     UINT8            *InPutBuff,       // rcx
;    IN     UINT8            *OutPutBuff,      // rdx
;    IN     UINT8            SubFunNum,        // r8
;    IN     UINT16           SmiPort           // r9
;    );
;------------------------------------------------------------------------------
IhisiVatsCall PROC       PUBLIC
          push  rbx
          push  rdi
          push  rsi
          push  r8

          mov   rsi, rcx
          mov   rdi, rdx
          mov   ebx, IHISI_SIGNATURE
          mov   al,  r8b
          mov   ah,  al
          mov   al,  0EFh
          mov   rdx, r9
          out   dx,  al

          ;AL Fun ret state

          pop   r8
          pop   rsi
          pop   rdi
          pop   rbx
          ret
IhisiVatsCall  ENDP


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
