;; @file
;;
;;  64 bit Sent SMI to call BiosStorageKernelSmm.c EventLogToSmm() function
;;
;;******************************************************************************
;;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;;

TITLE   CallSmmEventLog.asm

text  SEGMENT

EVENT_LOG_SIGNATURE EQU      024454C47h
SMM_EVENT_LOG_SMI   EQU      099h

;Argument 1: rcx
;Argument 2: rdx
;Argument 3: r8
;Argument 4: r9

;------------------------------------------------------------------------------
;  UINT8
;  SmmEventLogCall (
;    IN     UINT8            *InPutBuff,       // rcx
;    IN     UINTN            DataSize,         // rdx
;    IN     UINT8            SubFunNum,        // r8
;    IN     UINT16           SmiPort           // r9
;    );
;------------------------------------------------------------------------------
SmmEventLogCall PROC       PUBLIC
          push  rbx
          push  rdi
          push  rsi
          push  r8

          mov   rsi, rcx
          mov   rdi, rdx
          mov   rbx, EVENT_LOG_SIGNATURE
          mov   al,  r8b
          mov   ah,  al
          mov   al,  SMM_EVENT_LOG_SMI
          mov   rdx, r9
          out   dx,  al

          ;AL Fun ret state

          pop   r8
          pop   rsi
          pop   rdi
          pop   rbx
          ret
SmmEventLogCall  ENDP


text  ENDS
END
