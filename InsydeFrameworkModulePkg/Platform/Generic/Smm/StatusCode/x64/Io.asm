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
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
  title   Io.asm
;------------------------------------------------------------------------------
;
; Copyright (c)  2007 Intel Corporation. All rights reserved
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
;   Supports X64 CPU IO operation
;
;------------------------------------------------------------------------------

  .CODE

;------------------------------------------------------------------------------
;  UINT8
;  IoRead8 (
;    IN  UINT64  Address  ; rcx
;    )
;------------------------------------------------------------------------------
IoRead8 PROC    PUBLIC
    mov    rdx, rcx
    in     al,  dx
    ret
IoRead8  ENDP


;------------------------------------------------------------------------------
;  VOID
;  IoWrite8 (
;    IN  UINT64  Address, ; rcx
;    IN  UINT8   Data     ; rdx
;    )
;------------------------------------------------------------------------------
IoWrite8 PROC    PUBLIC
    mov    al,  dl
    mov    rdx, rcx
    out    dx,  al
    ret
IoWrite8  ENDP

END