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
;;   SecureFlashSmm.asm
;;
;; Abstract:
;;
;;   64 bit Sent SMI to call IHISI function 48h for secure flash feature
;;

TITLE   SecureFlashSmm.asm

text  SEGMENT

IHISI_SIGNATURE EQU      02448324Fh


;------------------------------------------------------------------------------
;  UINT8
;  SecureFlashReadyToBootSmi (
;    IN     UINT32	          Command,  // rcx
;    IN     UINT16                SmiPort   // rdx
;    );
;------------------------------------------------------------------------------
SecureFlashReadyToBootSmi PROC       PUBLIC
        	push	rbx
	        push	rdi
	        push	rsi
                push    r8

		mov		ebx, IHISI_SIGNATURE
		mov		ax,  48EFh
		out		dx,  al

          ;AL Fun ret state

                pop   r8
        	pop   rsi
        	pop   rdi
	        pop   rbx
                ret
SecureFlashReadyToBootSmi  ENDP

text  ENDS
END
