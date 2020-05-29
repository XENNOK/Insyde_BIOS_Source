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
; This file contains a 'Sample Driver' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may be modified by the user, subject to
; the additional terms of the license agreement
;
  title   Io.asm
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
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
;   Io.asm
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
;  IoRead8 (
;    IN  UINT64  Address
;    )
;------------------------------------------------------------------------------
IoRead8 PROC    PUBLIC Address:UINT64
    push   edx
    mov    dx,  WORD PTR Address
    in     al,  dx
    pop    edx
    ret
IoRead8  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoWrite8 (
;    IN  UINT64  Address,
;    IN  UINT8   Data
;    )
;------------------------------------------------------------------------------
IoWrite8 PROC    PUBLIC Address:UINT64, Data:UINT8
    pushad
    mov    al,  Data
    mov    dx,  WORD PTR Address
    out    dx,  al
    popad
    ret
IoWrite8  ENDP




END