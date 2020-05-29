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

text  SEGMENT
;------------------------------------------------------------------------------
;  UINT8
;  CpuIoRead8 (
;    UINT16  Port   // rcx
;    )
;------------------------------------------------------------------------------
CpuIoRead8 PROC    PUBLIC
    xor   eax, eax
    mov    dx, cx
    in     al, dx
    ret
CpuIoRead8  ENDP

;------------------------------------------------------------------------------
;  UINT16
;  CpuIoRead16 (
;    UINT16  Port   // rcx
;    )
;------------------------------------------------------------------------------
CpuIoRead16 PROC    PUBLIC
    xor   eax, eax
    mov    dx, cx
    in     ax, dx
    ret
CpuIoRead16  ENDP

;------------------------------------------------------------------------------
;  UINT32
;  CpuIoRead32 (
;    UINT16  Port   // rcx
;    )
;------------------------------------------------------------------------------
CpuIoRead32 PROC    PUBLIC
    mov    dx, cx
    in    eax, dx
    ret
CpuIoRead32  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite8 (
;    UINT16  Port,    // rcx
;    UINT32  Data     // rdx
;    )
;------------------------------------------------------------------------------
CpuIoWrite8 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, al
    ret
CpuIoWrite8  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite16 (
;    UINT16  Port,    // rcx
;    UINT32  Data     // rdx
;    )
;------------------------------------------------------------------------------
CpuIoWrite16 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, ax
    ret
CpuIoWrite16  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CpuIoWrite32 (
;    UINT16  Port,    // rcx
;    UINT32  Data     // rdx
;    )
;------------------------------------------------------------------------------
CpuIoWrite32 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, eax
    ret
CpuIoWrite32  ENDP

text  ENDS
END
