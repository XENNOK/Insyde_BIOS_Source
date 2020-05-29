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
;   Supports X64 CPU IO operation
;
;------------------------------------------------------------------------------

text  SEGMENT

;------------------------------------------------------------------------------
;  UINT8
;  IoInput8 (
;    IN  UINT16  Port  // rcx
;    )
;------------------------------------------------------------------------------
IoInput8 PROC    PUBLIC
    xor   eax, eax
    mov    dx, cx
    in     al, dx
    ret
IoInput8  ENDP

;------------------------------------------------------------------------------
;  UINT16
;  IoInput16 (
;    IN  UINT16  Port  // rcx
;    )
;------------------------------------------------------------------------------
IoInput16 PROC    PUBLIC
    xor   eax, eax
    mov    dx, cx
    in     ax, dx
    ret
IoInput16  ENDP

;------------------------------------------------------------------------------
;  UINT32
;  IoInput32 (
;    IN  UINT16  Port  // rcx
;    )
;------------------------------------------------------------------------------
IoInput32 PROC    PUBLIC
    mov    dx, cx
    in    eax, dx
    ret
IoInput32  ENDP



;------------------------------------------------------------------------------
;  VOID
;  IoOutput8 (
;    IN  UINT16  Port, // rcx
;    IN  UINT32  Data  // rdx
;    )
;------------------------------------------------------------------------------
IoOutput8 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, al
    ret
IoOutput8  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoOutput16 (
;    IN  UINT16  Port, // rcx
;    IN  UINT32  Data  // rdx
;    )
;------------------------------------------------------------------------------
IoOutput16 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, ax
    ret
IoOutput16  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoOutput32 (
;    IN  UINT16  Port, // rcx
;    IN  UINT32  Data  // rdx
;    )
;------------------------------------------------------------------------------
IoOutput32 PROC    PUBLIC
    mov   eax, edx
    mov    dx, cx
    out    dx, eax
    ret
IoOutput32  ENDP

text  ENDS
END
