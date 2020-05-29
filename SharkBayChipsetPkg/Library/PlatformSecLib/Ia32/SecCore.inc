;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2011, Intel Corporation. All rights reserved.<BR>
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
;   SecCore.inc
;
; Abstract:
;
;   SecCore constants and macros
;
;------------------------------------------------------------------------------

;[-start-120323-IB06460379-add];
IFDEF PLATFORM_DEBUG_PORT_NUMBER
  DEBUG_PORT        EQU PLATFORM_DEBUG_PORT_NUMBER
ELSE
  DEBUG_PORT        EQU 080h
ENDIF
;[-end-120323-IB06460379-add];

;[-start-120330-IB05300303-modify];
;
; Set to 1 to enable debug
;
IFDEF H2O_PORT_80_DEBUG
NO_EVICTION_MODE_DEBUG  EQU     1
ELSE
NO_EVICTION_MODE_DEBUG  EQU     0
ENDIF

STATUS_CODE         MACRO status
IF NO_EVICTION_MODE_DEBUG
                    mov  al, status
                    out  DEBUG_PORT, al
ENDIF
ENDM
;[-end-120330-IB05300303-modify];

FVHEADER_LEN_OFF    EQU 30h
FFSHEADER_LEN       EQU 18h

IMAGE_BASE_ADDRESS  EQU 0FFFF0000h

;
; Set to 1 to enable debug support for "Deterministic BSP selection"
;
AP_ENTRY_DELAY      EQU         10h
AP_EXECUTION_DELAY  EQU         1000h

;
; Define the segment used for AP start-up
; It should be on the top of the recovery FV
; Seg  = 0100h - (BlockNumber of Recovery FV)
; Here 0FCh = 0100h - 04h
;
AP_SEG                      EQU   0FFh

;
; Commands defined in the AP SIPI code 
;
AP_SIPI_COLLECT_MAX_RATIO   EQU   001h  
AP_SIPI_PROGRAM_MAX_RATIO   EQU   002h
AP_SIPI_SWITCH_BSP          EQU   003h
