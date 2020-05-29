;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
  title   CpuIoAccess.asm

  .686
  .MODEL FLAT,C
  .CODE


UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD
UINT64   TYPEDEF    QWORD
UINTN    TYPEDEF    UINT32

;------------------------------------------------------------------------------
;  UINT8
;  CpuIoRead8 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
CpuIoRead8 PROC    PUBLIC Port:UINT16
    mov    dx,  Port
    in     al,  dx
    ret
CpuIoRead8  ENDP

;------------------------------------------------------------------------------
;  UINT16
;  CpuIoRead16 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
CpuIoRead16 PROC    PUBLIC Port:UINT16
    mov    dx,  Port
    in     ax,  dx
    ret
CpuIoRead16  ENDP

;------------------------------------------------------------------------------
;  UINT32
;  CpuIoRead32 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
CpuIoRead32 PROC    PUBLIC Port:UINT16
    mov   dx,  Port
    in    eax, dx
    ret
CpuIoRead32  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite8 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
CpuIoWrite8 PROC    PUBLIC Port:UINT16, Data:UINT32
    mov    eax, Data
    mov    dx,  Port
    out    dx,  al
    ret
CpuIoWrite8  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite16 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
CpuIoWrite16 PROC    PUBLIC Port:UINT16, Data:UINT32
    mov    eax, Data
    mov    dx,  Port
    out    dx,  ax
    ret
CpuIoWrite16  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite32 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
CpuIoWrite32 PROC    PUBLIC Port:UINT16, Data:UINT32
    mov    eax, Data
    mov    dx,  Port
    out    dx,  eax
    ret
CpuIoWrite32  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  EFIAPI
;  ByteRotate (
;    IN  UINTN  Data
;    );
;------------------------------------------------------------------------------
ByteRotate PROC    PUBLIC Data:UINTN
    mov    eax, Data
    ror	    al, 4
    ret
ByteRotate  ENDP

;------------------------------------------------------------------------------
;  VOID
;  Stall (
;    IN  UINTN  Count
;    )
;------------------------------------------------------------------------------
Stall PROC    PUBLIC Count:UINTN
    pushad
	  mov	al, 074h
	  out	043h, al
	  mov	al, 058h
	  out	041h, al
	  mov	al, 002h
	  out	041h, al
loc_skip_setup_timer1:
	  push	ecx
	  mov	ecx, Count
DeadLoop0:
	  in	al, 061h
	  and	al, 010h
	  jz	DeadLoop0
DeadLoop1:
	  in	al, 061h
	  and	al, 010h
	  jnz	DeadLoop1
	  loop	DeadLoop0
	  pop	ecx
	  mov	al, 074h
	  out	043h, al
	  mov	al, 012h
	  out	041h, al
	  mov	al, 000h
	  out	041h, al
loc_skip_setup_timer2:
    popad
    ret
Stall  ENDP

END
