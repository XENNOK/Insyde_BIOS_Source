;******************************************************************************
;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************


FastRecoveryDxeToPeiData  STRUC
  StackData             dq   ?
  
  ;
  ; Data address
  ;
  EmuPeiFv              dq   ?
  EmuPeiFvBase          dq   ?
  EmuPeiEntry           dq   ?
  EmuSecEntry           dq   ?
  EmuSecFfsAddress      dq   ?   ; Will mask after we execute
  RecoveryImageAddress  dq   ?
  OriginalHobList       dq   ?
  RomToRamDifference    dq   ?
                             
  ;                          
  ; Data values              
  ;                          
  EmuPeiFvSize          dq   ?  ; Refer Flashmap.h
  RecoveryImageSize     dq   ?
FastRecoveryDxeToPeiData	ENDS
