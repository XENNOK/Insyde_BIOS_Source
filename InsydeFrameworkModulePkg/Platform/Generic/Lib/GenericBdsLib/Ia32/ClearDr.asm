;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
  title   ClearDr.asm
;------------------------------------------------------------------------------
;
; Copyright (c) 2004, Intel Corporation                                                         
; All rights reserved. This program and the accompanying materials                          
; are licensed and made available under the terms and conditions of the BSD License         
; which accompanies this distribution.  The full text of the license may be found at        
; http://opensource.org/licenses/bsd-license.php                                            
;                                                                                           
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
; 
; Module Name:
;
;   ClearDr.asm
; 
; Abstract:
; 
;   Clear dr0 dr1 register
;
;------------------------------------------------------------------------------

  .686
  .MODEL FLAT,C
  .CODE

;------------------------------------------------------------------------------
;  VOID
;  ClearDebugRegisters (
;    VOID
;    )
;------------------------------------------------------------------------------
ClearDebugRegisters PROC    PUBLIC
    push   eax
    xor    eax, eax
    mov    dr0, eax
    mov    dr1, eax
    pop    eax
    ret
ClearDebugRegisters ENDP

END

