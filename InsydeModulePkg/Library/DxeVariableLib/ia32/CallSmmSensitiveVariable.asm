;;******************************************************************************
;;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;;
;; Module Name:
;;
;;   CallSmmSensitiveVariable.asm
;;
;; Abstract:
;;
;;   32 bit Sent SMI to call update sensitive variable function
;;

TITLE   CallSmmSensitiveVariable.asm

  .686
  .MODEL FLAT,C
  .CODE

;------------------------------------------------------------------------------
;  UINT8
;  SmmSensitiveVariableCall (
;    IN     UINT8            *InPutBuff,
;    IN     UINTN            DataSize,
;    IN     UINT8            SubFunNum,
;    IN     UINT16           SmiPort
;    );
;------------------------------------------------------------------------------
SmmSensitiveVariableCall PROC       PUBLIC InPutBuff:DWORD, DataSize:DWORD, \
                                    SubFunNum:BYTE, SmiPort:WORD
			    push	ebx
			    push	ecx
			    push	edx
			    push	edi
			    push	esi

			    mov		ebx, DataSize
			    mov		esi, InPutBuff
			    mov   al,  SubFunNum
			    mov   ah,  al
			    mov   al,  0ECh
			    mov		dx,  SmiPort
			    out		dx,  al

			    pop		esi
			    pop		edi
			    pop		edx
			    pop		ecx
			    pop		ebx
			    ret
SmmSensitiveVariableCall  ENDP

END