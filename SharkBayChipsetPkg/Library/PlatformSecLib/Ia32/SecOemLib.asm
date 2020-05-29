;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

   .listall
   .686p
   .xmm
   .model   flat,c

;[-start-120330-IB05300303-add];
CALL_MMX macro   RoutineLabel

  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:

endm

RET_ESI  macro

  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi

endm
;[-end-120330-IB05300303-add];

.const

.code

;[-start-120330-IB05300303-modify];
SecOemFunc  PROC  NEAR  PUBLIC
  ;
  ; Add SEC customization code at here
  ;
  RET_ESI
SecOemFunc  ENDP
;[-end-120330-IB05300303-modify];

END
