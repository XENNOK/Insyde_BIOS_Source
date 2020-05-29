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
;;   64 bit Sent SMI to call IHISI flash ROM part
;;

TITLE   CallSmmIhisi.asm

text  SEGMENT

IHISI_SIGNATURE EQU      02448324Fh

;Argument 1: rcx
;Argument 2: rdx
;Argument 3: r8
;Argument 4: r9

;------------------------------------------------------------------------------
;  UINT8
;  FlashPartInfo (
;    IN     UINT8            *PartInfo,       // rcx
;    IN     UINT8            *PartBlock,      // rdx
;    IN     UINT8            FlashTypeSelect, // r8
;    IN     UINT16           SmiPort          // r9
;    );
;------------------------------------------------------------------------------
FlashPartInfo PROC       PUBLIC
        	push	rbx
	        push	rdi
	        push	rsi
          push  r8

          mov   cl,  r8b
      		mov		rdi, rcx
      		mov		rsi, rdx
					mov		ebx, IHISI_SIGNATURE
					mov		ax,  13EFh
					mov		rdx, r9
					out		dx,  al

          ;AL Fun ret state

          pop   r8
        	pop   rsi
        	pop   rdi
	        pop   rbx
          ret
FlashPartInfo  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  FlashWrite (
;    IN UINT8	                *Buffer,      // rcx
;    IN UINT32	              FlashSize,    // rdx
;  	 IN UINT32	              FlashAddress  // r8
;  	 IN UINT16	              SmiPort       // r9
;    );
;------------------------------------------------------------------------------
FlashWrite PROC       PUBLIC
        	push	rbx
	        push	rdi
	        push	rsi
          push  r8

      		mov		rsi, rcx    ;Arg1 rcx -> Write buffer to ESI
      		mov   rcx, rdx    ;Arg2 rdx -> Write size to ECX
      		mov   rdi, r8     ;Arg3 r8  -> Write address to EDI
					mov		ebx, IHISI_SIGNATURE
					mov		ax,  15EFh
					mov		rdx, r9
					out		dx,  al

          ;AL Fun ret state

          pop   r8
        	pop   rsi
        	pop   rdi
	        pop   rbx
          ret
FlashWrite  ENDP

;------------------------------------------------------------------------------
;  UINT8
;  FlashComplete (
;    IN     UINT16	              Command,  // rcx
;    IN     UINT16                SmiPort   // rdx
;    );
;------------------------------------------------------------------------------
FlashComplete PROC       PUBLIC
        	push	rbx
	        push	rdi
	        push	rsi
          push  r8

					mov		ebx, IHISI_SIGNATURE
					mov		ax,  16EFh
					out		dx,  al

          ;AL Fun ret state

          pop   r8
        	pop   rsi
        	pop   rdi
	        pop   rbx
          ret
FlashComplete  ENDP

text  ENDS
END
