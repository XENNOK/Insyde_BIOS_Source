;; @file
; Interrupt Redirection Template
;
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

.686P
.MODEL FLAT, C
.CODE

;----------------------------------------------------------------------------
; Procedure:    InterruptRedirectionTemplate: Redirects interrupts 0x68-0x6F
;
; Input:        None
;
; Output:       None
;
; Prototype:    VOID
;               InterruptRedirectionTemplate (
;                                VOID
;                                );
;
; Saves:        None
;
; Modified:     None
;
; Description:  Contains the code that is copied into low memory (below 640K).
;               This code reflects interrupts 0x68-0x6f to interrupts 0x08-0x0f.
;               This template must be copied into low memory, and the IDT entries
;               0x68-0x6F must be point to the low memory copy of this code.  Each
;               entry is 4 bytes long, so IDT entries 0x68-0x6F can be easily
;               computed.
;
;----------------------------------------------------------------------------

InterruptRedirectionTemplate PROC  C
  int     08h
  DB      0cfh          ; IRET
  nop
  int     09h
  DB      0cfh          ; IRET
  nop
  int     0ah
  DB      0cfh          ; IRET
  nop
  int     0bh
  DB      0cfh          ; IRET
  nop
  int     0ch
  DB      0cfh          ; IRET
  nop
  int     0dh
  DB      0cfh          ; IRET
  nop
  int     0eh
  DB      0cfh          ; IRET
  nop
  int     0fh
  DB      0cfh          ; IRET
  nop
InterruptRedirectionTemplate ENDP

DummyInterruptTemplate PROC    PUBLIC
  DB      0cfh          ; IRET
DummyInterruptTemplate ENDP

END
