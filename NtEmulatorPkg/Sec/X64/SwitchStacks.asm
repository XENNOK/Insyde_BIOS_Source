;------------------------------------------------------------------------------
;
; Copyright (C) 2013 Insyde Software Corp. All rights reserved.
; Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
;
;------------------------------------------------------------------------------

    .CODE

;------------------------------------------------------------------------------
; Routine Description:
;
;   Call the PEI Core entry point , which has the following typedef:
;
;     typedef
;     VOID
;     EFIAPI
;     (*EFI_PEI_CORE_ENTRY_POINT)(
;       IN CONST EFI_SEC_PEI_HAND_OFF *SecCoreData,
;       IN CONST EFI_PEI_PPI_DESCRIPTOR *PpiList
;       );
;
;   NOTE: The function call is done using the new stack.
;
;   NOTE: The entry point uses the EFIAPI processor binding, which, happens to
;   match Windows. But it does not match other OS and would require
;   parameter translation.
;
;
; Entry Parameters:
;       RCX       EntryPoint
;       RDX       SecCoreData
;       R8        PPI List
;       R9        Context3
;       [RSP]     Return address
;       [RSP+8]   Reserved Space for RCX
;       [RSP+16]  Reserved Space for RDX
;       [RSP+24]  Reserved Space for R8
;       [RSP+32]  Reserved Space for R9
;       [RSP+40]  NewStack
;
; Exit Parameters:
;       Never Returns.

PeiSwitchStacks PROC

      mov   rsp, QWORD PTR [rsp + 40] ; RSP = new stack address
      and   rsp, NOT 15               ; make sure it is 16-byte aligned.
      sub   rsp, 20h

      mov   rax, rcx                  ; RAX = entry point address
      mov   rcx, rdx                  ; RCX = SecCoreData
      mov   rdx, r8                   ; RDX = PPI List
      mov   r8, r9                    ; R8  = NULL
      call  rax
PeiSwitchStacks ENDP


        END