;; @file
; Supports IA32 CPU operation for CMOS library
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
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
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
  title   CmosLib.asm

  .686
  .MODEL FLAT,C
  .CODE

;------------------------------------------------------------------------------
;  VOID
;    CmosDisableInterrupt (
;    )
;------------------------------------------------------------------------------
CmosDisableInterrupt PROC    PUBLIC
    cli
    ret
CmosDisableInterrupt  ENDP

;------------------------------------------------------------------------------
;  VOID
;  CmosEableInterrupt (
;    )
;------------------------------------------------------------------------------
CmosEableInterrupt PROC    PUBLIC
    sti
    ret
CmosEableInterrupt  ENDP

;------------------------------------------------------------------------------
;  UINT16
;  CmosGetCpuFlags (
;    )
;------------------------------------------------------------------------------
CmosGetCpuFlags PROC    PUBLIC
    pushf
    pop    ax
    ret
CmosGetCpuFlags  ENDP


END
