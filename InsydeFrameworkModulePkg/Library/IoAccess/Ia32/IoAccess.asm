;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
;
  title   IoAccess.asm
;------------------------------------------------------------------------------
;
; Copyright (c) 2006 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   IoAccess.asm
;
; Abstract:
;
;   Supports IA32 CPU IO operation
;
;------------------------------------------------------------------------------

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
;  IoInput8 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
IoInput8 PROC    PUBLIC Port:UINT16
    push   edx
    mov    dx,  Port
    in     al,  dx
    pop    edx
    ret
IoInput8  ENDP

;------------------------------------------------------------------------------
;  UINT16
;  IoInput16 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
IoInput16 PROC    PUBLIC Port:UINT16
    push   edx
    mov    dx,  Port
    in     ax,  dx
    pop    edx
    ret
IoInput16  ENDP

;------------------------------------------------------------------------------
;  UINT32
;  IoInput32 (
;    IN  UINT16  Port
;    )
;------------------------------------------------------------------------------
IoInput32 PROC    PUBLIC Port:UINT16
    push  edx
    mov   dx,  Port
    in    eax, dx
    pop   edx
    ret
IoInput32  ENDP



;------------------------------------------------------------------------------
;  VOID
;  IoOutput8 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
IoOutput8 PROC    PUBLIC Port:UINT16, Data:UINT8
    pushad
    mov    al, Data
    mov    dx,  Port
    out    dx,  al
    popad
    ret
IoOutput8  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoOutput16 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
IoOutput16 PROC    PUBLIC Port:UINT16, Data:UINT16
    pushad
    mov    ax, Data
    mov    dx,  Port
    out    dx,  ax
    popad
    ret
IoOutput16  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoOutput32 (
;    IN  UINT16  Port,
;    IN  UINT32  Data
;    )
;------------------------------------------------------------------------------
IoOutput32 PROC    PUBLIC Port:UINT16, Data:UINT32
    pushad
    mov    eax, Data
    mov    dx,  Port
    out    dx,  eax
    popad
    ret
IoOutput32  ENDP


END
