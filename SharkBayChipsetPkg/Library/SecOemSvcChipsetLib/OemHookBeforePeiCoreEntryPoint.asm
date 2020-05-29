;; @file
;;  This Function offers an interface for chipset code to call before "CallPeiCoreEntryPoint".

;;******************************************************************************
;;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
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

.const

.code

;
; This Function offers an interface for chipset code to call before "CallPeiCoreEntryPoint".
;
; This code needs to be written by Assembler. and call by CALL_MMX (save ReturnAddress into MM7)
; @param        None
;
; @retval           al = 3  (EFI_UNSUPPORTED)     Returns unsupported by default.
; @retval           al = 13 (EFI_MEDIA_CHANGED)   Alter the Configuration Parameter or hook code
; @retval           al = 0  (EFI_SUCCESS)         The function performs the same operation as caller.
;                                                 The caller will skip the specified behavior and assuming
;                                                 that it has been handled completely by this function.
OemHookBeforePeiCoreEntryPoint  PROC  NEAR  PUBLIC
  ;
  ; Add SEC customization code at here
  ;

  ;
  mov      al, 3                        ; Return value
  movd    esi, mm7                      ; Restore ESI from MM7
  jmp     esi
OemHookBeforePeiCoreEntryPoint  ENDP


END
