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
;;   32 bit Sent SMI to call IHISI function 48h for secure flash feature
;;

TITLE   SecureFlashSmm.asm

  .686
  .MODEL FLAT,C
  .CODE

IHISI_SIGNATURE EQU      02448324Fh


;------------------------------------------------------------------------------
;  UINT8
;  SecureFlashReadyToBootSmi (
;    IN     UINT32	          Command,
;    IN     UINT16                SmiPort
;    );
;------------------------------------------------------------------------------
SecureFlashReadyToBootSmi PROC       PUBLIC Command:DWORD, SmiPort:WORD
			push	ebx
			push	ecx
			push	edx
			push	edi
			push	esi

      mov               ecx, Command  
      mov		ebx, IHISI_SIGNATURE
      mov		ax,  48EFh
      mov		dx,  SmiPort
      out		dx,  al

      ;AL Fun ret state

			pop		esi
			pop		edi
			pop		edx
			pop		ecx
			pop		ebx
			ret
SecureFlashReadyToBootSmi  ENDP

END
