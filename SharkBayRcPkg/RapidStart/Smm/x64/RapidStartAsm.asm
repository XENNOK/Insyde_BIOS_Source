;/** @file
;  This driver is isa acpi driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;*/

;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
  page    ,132
  title   RapidStart ARCHITECTURAL SMM ASSEMBLY HOOKS
;------------------------------------------------------------------------------
;
; Copyright (c) 2005 -2012 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;   RapidStartAsm.asm
;
; Abstract:
;
;   Assembly code of the implementation of X64 RapidStart
;
;------------------------------------------------------------------------------

text  SEGMENT

;------------------------------------------------------------------------------
;  UINT64
;  ScanValueMem64 (
;    VOID*   BaseAddress,
;    UINT64  PageSize,
;    UINT64  Index
;    )
;------------------------------------------------------------------------------
ScanValueMem64    PROC    USES    rbx rsi rdi
   xor     rsi, rsi
    dec     rsi
    mov     rdi, rcx
    mov     rax, r8
    mov     rcx, rdx
    repe    scasq
    mov    rax, rsi
    lea     rbx, [rdi - 8]
    cmovne  rax, rbx
    ret
ScanValueMem64    ENDP

text  ENDS
END

