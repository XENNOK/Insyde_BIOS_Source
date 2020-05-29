;;******************************************************************************
;;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
;
      TITLE   AsmCopyMem.asm: Optimized memory-copy routine

;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   EfiCopyMem.asm
;
; Abstract:
;
;   This is the code that supports x64-optimized CopyMem service
;
;------------------------------------------------------------------------------
;VOID
;AsmEfiCommonLibCopyMem (
;  VOID  *Destination,  -- rcx
;  VOID  *Source,       -- rdx
;  UINTN Count          -- r8
;  );
;------------------------------------------------------------------------------
.code
;------------------------------------------------------------------------------
;
AsmEfiCommonLibCopyMem  PROC    PUBLIC
  ; Put source and destination pointers in rsi/rdi
  push  rbp
  mov   rbp, rsp
  sub   rsp, 8
  push  rsi
  push  rdi
  mov   rsi, rdx
  mov   rdi, rcx
  ;   rdx: source
  ;   rcx: destination
  ;   r8: count
  ;   First off, make sure we have no overlap. That is to say,
  ;   if (Source == Destination)           => do nothing
  ;   if (Source + Count <= Destination)   => regular copy
  ;   if (Destination + Count <= Source)   => regular copy
  ;   otherwise, do a reverse copy
  mov   rax, rsi
  add   rax, r8                       ; Source + Count
  cmp   rax, rdi
  jbe   _StartByteCopy

  mov   rax, rdi
  add   rax, r8                       ; Dest + Count
  cmp   rax, rsi
  jbe   _StartByteCopy

  cmp   rsi, rdi
  je    _CopyMemDone
  jb    _CopyOverlapped               ; too bad -- overlaps

  ; Pick up misaligned start bytes to get destination pointer 4-byte aligned
_StartByteCopy:
  cmp   r8, 0
  je    _CopyMemDone                ; Count == 0, all done
  mov   rdx, rdi
  and   dl, 3                       ; check lower 2 bits of address
  test  dl, dl
  je    SHORT _CopyBlocks           ; already aligned?

  ; Copy a byte
  mov   al, BYTE PTR [rsi]          ; get byte from Source
  mov   BYTE PTR [rdi], al          ; write byte to Destination
  dec   r8
  inc   rdi
  inc   rsi
  jmp   _StartByteCopy               ; back to top of loop

_CopyBlocks:
  ; Compute how many 64-byte blocks we can clear
  mov   rax, r8                     ; get Count in eax
  shr   rax, 6                      ; convert to 64-byte count
  shl   rax, 6                      ; convert back to bytes
  sub   r8, rax                     ; subtract from the original count
  shr   rax, 6                      ; and this is how many 64-byte blocks

  ; If no 64-byte blocks, then skip
  cmp   rax, 0
  je    _CopyRemainingDWords

  ; Save mm0
  dd 0F8457F0Fh  ;  movq  [rbp - 8], mm0
copymmx:
  dw 6F0Fh
  db 06h         ;  movq  mm0, QWORD PTR ds:[rsi]
  dw 7F0Fh
  db 07h         ;  movq  QWORD PTR ds:[rdi], mm0
  dd 08466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+8]
  dd 08477F0Fh   ;  movq  QWORD PTR ds:[rdi+8], mm0
  dd 10466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+16]
  dd 10477F0Fh   ;  movq  QWORD PTR ds:[rdi+16], mm0
  dd 18466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+24]
  dd 18477F0Fh   ;  movq  QWORD PTR ds:[rdi+24], mm0
  dd 20466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+32]
  dd 20477F0Fh   ;  movq  QWORD PTR ds:[rdi+32], mm0
  dd 28466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+40]
  dd 28477F0Fh   ;  movq  QWORD PTR ds:[rdi+40], mm0
  dd 30466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+48]
  dd 30477F0Fh   ;  movq  QWORD PTR ds:[rdi+48], mm0
  dd 38466F0Fh   ;  movq  mm0, QWORD PTR ds:[rsi+56]
  dd 38477F0Fh   ;  movq  QWORD PTR ds:[rdi+56], mm0
  add   rdi, 64
  add   rsi, 64
  dec   rax
  jnz   copymmx
  dd 0F8456F0Fh  ;  movq  mm0, [rbp - 8]
  dw 770Fh       ;  emms
  ; Exit MMX Instruction

  ; Copy as many DWORDS as possible
_CopyRemainingDWords:
  cmp   r8, 4
  jb    _CopyRemainingBytes

  mov   eax, DWORD PTR [rsi]        ; get data from Source
  mov   DWORD PTR [rdi], eax        ; write byte to Destination
  sub   r8, 4                       ; decrement Count
  add   rsi, 4                      ; advance Source pointer
  add   rdi, 4                      ; advance Destination pointer
  jmp   _CopyRemainingDWords        ; back to top

_CopyRemainingBytes:
  cmp   r8, 0
  je    _CopyMemDone
  mov   al, BYTE PTR [rsi]          ; get byte from Source
  mov   BYTE PTR [rdi], al          ; write byte to Destination
  dec   r8
  inc   rsi
  inc   rdi                         ; advance Destination pointer
  jmp   _CopyRemainingBytes         ; back to top of loop
  ;
  ; We do this block if the source and destination buffers overlap. To
  ; handle it, copy starting at the end of the source buffer and work
  ; your way back. Since this is the atypical case, this code has not
  ; been optimized, and thus simply copies bytes.
  ;
_CopyOverlapped:
 
  ; Move the source and destination pointers to the end of the range
  add   rsi, r8                      ; Source + Count
  dec   rsi
  add   rdi, r8                      ; Dest + Count
  dec   rdi

_CopyOverlappedLoop:
  cmp   r8, 0
  je    _CopyMemDone
  mov   al, BYTE PTR [rsi]          ; get byte from Source
  mov   BYTE PTR [rdi], al          ; write byte to Destination
  dec   r8
  dec   rsi
  dec   rdi
  jmp   _CopyOverlappedLoop         ; back to top of loop

_CopyMemDone:
  pop   rdi
  pop   rsi
  add   rsp, 8
  pop   rbp

  ret
 
AsmEfiCommonLibCopyMem  ENDP
  END
