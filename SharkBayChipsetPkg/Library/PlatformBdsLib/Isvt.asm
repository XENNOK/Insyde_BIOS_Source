;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
;;   Isvt.asm
;;
;; Abstract:
;;
;;   Intel Silicon View Technology (ISVT) support
;;

TITLE  Isvt.asm

text  SEGMENT

;------------------------------------------------------------------------------
;UINT8
;IsvtCall (
;  IN UINT8 CheckPoint       // rcx
;  );
;
;Routine Description:
; 
;   Intel Silicon View Technology (ISVT) IO Reading port 0x84 with different AH values.
;
;Arguments:
;
;   CheckPoint - Put into AH.
;                AH = 1:  End of MRC State
;                AH = 2:  End of DXE State
;                AH = 3:  Before INT 19h boot (Ready to boot)
;  
;Returns:
;
;   Data in IO port 0x84.
;
;------------------------------------------------------------------------------
IsvtCall PROC       PUBLIC
    xor   eax, eax
    mov   ah,  cl
    mov   dx,  84h
    in    al,  dx
    ret
IsvtCall ENDP

text  ENDS

END
