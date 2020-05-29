;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
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
;;   CallSmmIhisi.asm
;;
;; Abstract:
;;
;;   32 bit Sent SMI to call IHISI VATS function
;;

TITLE   CallSmmIhis.asm

  .686
  .MODEL FLAT,C
  .CODE

IHISI_SIGNATURE EQU      02448324Fh

;------------------------------------------------------------------------------
;  UINT8
;  IhisiVatsCall (
;    IN     UINT8            *InPutBuff,
;    IN     UINT8            *OutPutBuff,
;    IN     UINT8            SubFunNum,
;    IN     UINT16           SmiPort
;    );
;------------------------------------------------------------------------------
IhisiVatsCall PROC       PUBLIC InPutBuff:DWORD, OutPutBuff:DWORD, \
                                SubFunNum:BYTE, SmiPort:WORD
			    push	ebx
			    push	ecx
			    push	edx
			    push	edi
			    push	esi

			    mov		edi, OutPutBuff
			    mov		esi, InPutBuff
			    mov		ebx, IHISI_SIGNATURE
			    mov   al,  SubFunNum
			    mov   ah,  al
			    mov   al,  0EFh
			    mov		dx,  SmiPort
			    out		dx,  al

			    pop		esi
			    pop		edi
			    pop		edx
			    pop		ecx
			    pop		ebx
			    ret
IhisiVatsCall  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  SmmSecureBootCall (
;    IN     UINT8            *InPutBuff,
;    IN     UINTN            DataSize,
;    IN     UINT8            SubFunNum,
;    IN     UINT16           SmiPort
;    );
;------------------------------------------------------------------------------
SmmSecureBootCall PROC       PUBLIC InPutBuff:DWORD, DataSize:DWORD, \
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
SmmSecureBootCall  ENDP

END
