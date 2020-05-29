;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
;;   32 bit Sent SMI to call IHISI flash ROM part
;;

TITLE   CallSmmIhis.asm

  .686
  .MODEL FLAT,C
  .CODE

IHISI_SIGNATURE EQU      02448324Fh

;------------------------------------------------------------------------------
;  UINT8
;  FlashPartInfo (
;    IN     UINT8            *PartInfo,
;    IN     UINT8            *PartBlock,
;    IN     UINT8            FlashTypeSelect,
;    IN     UINT16           SmiPort
;    );
;------------------------------------------------------------------------------
FlashPartInfo PROC       PUBLIC PartInfo:DWORD, PartBlock:DWORD, \
                                FlashTypeSelect:BYTE, SmiPort:WORD
			push	ebx
			push	ecx
			push	edx
			push	edi
			push	esi

			mov 	cl,  FlashTypeSelect
			mov		edi, PartInfo
			mov		esi, PartBlock
			mov		ebx, IHISI_SIGNATURE
			mov		ax,  13EFh
			mov		dx,  SmiPort
			out		dx,  al

			;AL Fun ret state

			pop		esi
			pop		edi
			pop		edx
			pop		ecx
			pop		ebx
			ret
FlashPartInfo  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  FlashWrite (
;    IN UINT8	                *Buffer,
;    IN UINT32	              FlashSize,
;  	 IN UINT32	              FlashAddress,
;  	 IN UINT16	              SmiPort
;    );
;------------------------------------------------------------------------------
FlashWrite PROC       PUBLIC Buffer:DWORD, FlashSize:DWORD, \
                             FlashAddress:DWORD, SmiPort:WORD
			push	ebx
			push	ecx
			push	edx
			push	edi
			push	esi

      mov		esi, Buffer          ;Arg1  Write buffer to ESI
      mov   ecx, FlashSize    	 ;Arg2  Write Size to ECX
	    mov   edi, FlashAddress    ;Arg3  Write FlashAddress to EDI
			mov		ebx, IHISI_SIGNATURE
			mov		ax,  15EFh
			mov		dx, SmiPort
			out		dx, al

			;AL Fun ret state

			pop		esi
			pop		edi
			pop		edx
			pop		ecx
			pop		ebx
			ret
FlashWrite  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  FlashComplete (
;    IN     UINT16	              Command,
;    IN     UINT16                SmiPort
;    );
;------------------------------------------------------------------------------
FlashComplete PROC       PUBLIC Command:DWORD, SmiPort:WORD
			push	ebx
			push	ecx
			push	edx
			push	edi
			push	esi

      mov   ecx, Command          ;Arg1  Flash complete command
      mov		ebx, IHISI_SIGNATURE
      mov		ax,  16EFh
      mov		dx,  SmiPort
      out		dx,  al

      ;AL Fun ret state

			pop		esi
			pop		edi
			pop		edx
			pop		ecx
			pop		ebx
			ret
FlashComplete  ENDP

END
