//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
#------------------------------------------------------------------------------
#
# Copyright (c) 2009 - 2012, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#   SmiException.S
#
# Abstract:
#
#   Exception handlers used in SM mode
#
#------------------------------------------------------------------------------

ASM_GLOBAL  ASM_PFX(gSmiMtrrs)
ASM_GLOBAL  ASM_PFX(gcSmiIdtr)
ASM_GLOBAL  ASM_PFX(gcSmiGdtr)
ASM_GLOBAL  ASM_PFX(gcPsd)
ASM_GLOBAL  ASM_PFX(FeaturePcdGet (PcdCpuSmmStackGuard))
ASM_GLOBAL  ASM_PFX(gSavedDebugExceptionIdtEntry)
ASM_GLOBAL  ASM_PFX(gSmiExceptionHandlers)
ASM_GLOBAL  ASM_PFX(FeaturePcdGet (PcdCpuSmmProfileEnable))

    .data 

NullSeg:    .quad 0
            .quad 0                     # reserved for future use
CodeSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x9b
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
DataSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x93
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
            .quad 0                     # reserved for future use
CodeSeg16:
            .word -1
            .word 0
            .byte 0
            .byte 0x9b
            .byte 0x8f
            .byte 0
DataSeg16:
            .word -1
            .word 0
            .byte 0
            .byte 0x93
            .byte 0x8f
            .byte 0
CodeSeg64:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x9b
            .byte 0xaf                  # LimitHigh
            .byte 0                     # BaseHigh
.equ  GDT_SIZE, .- NullSeg

TssSeg:
            .word      TSS_DESC_SIZE       # LimitLow
            .word      0                   # BaseLow
            .byte      0                   # BaseMid
            .byte      0x89
            .byte      0x80                # LimitHigh
            .byte      0                   # BaseHigh
ExceptionTssSeg:
            .word      TSS_DESC_SIZE       # LimitLow
            .word      0                   # BaseLow
            .byte      0                   # BaseMid
            .byte      0x89
            .byte      0x80                # LimitHigh
            .byte      0                   # BaseHigh

.equ  CODE_SEL,          CodeSeg32 - NullSeg
.equ  DATA_SEL,          DataSeg32 - NullSeg
.equ  TSS_SEL,           TssSeg - NullSeg
.equ  EXCEPTION_TSS_SEL, ExceptionTssSeg - NullSeg

# Create 2 TSS segments just after GDT
TssDescriptor:
            .word      0                   # PreviousTaskLink
            .word      0                   # Reserved
            .long      0                   # ESP0
            .word      0                   # SS0
            .word      0                   # Reserved
            .long      0                   # ESP1
            .word      0                   # SS1
            .word      0                   # Reserved
            .long      0                   # ESP2
            .word      0                   # SS2
            .word      0                   # Reserved
ExceptionCr3:
            .long      0                   # CR3
ExceptionEip:
            .long      0                   # EIP
ExceptionEflags:
            .long      0                   # EFLAGS
            .long      0                   # EAX
            .long      0                   # ECX
            .long      0                   # EDX
            .long      0                   # EBX
            .long      0                   # ESP
            .long      0                   # EBP
            .long      0                   # ESI
            .long      0                   # EDI
            .word      0                   # ES
            .word      0                   # Reserved
            .word      0                   # CS
            .word      0                   # Reserved
            .word      0                   # SS
            .word      0                   # Reserved
            .word      0                   # DS
            .word      0                   # Reserved
            .word      0                   # FS
            .word      0                   # Reserved
            .word      0                   # GS
            .word      0                   # Reserved
            .word      0                   # LDT Selector
            .word      0                   # Reserved
            .word      0                   # T
            .word      0                   # I/O Map Base
.equ TSS_DESC_SIZE, . - TssDescriptor
.equ  EXCEPTION_EIP_OFFSET, ExceptionEip - TssDescriptor
.equ  EXCEPTION_EFLAGS_OFFSET, ExceptionEflags - TssDescriptor
.equ  EXCEPTION_CR3_OFFSET, ExceptionCr3 - TssDescriptor

ExceptionTssDescriptor:
            .word      0                   # PreviousTaskLink
            .word      0                   # Reserved
            .long      0                   # ESP0
            .word      0                   # SS0
            .word      0                   # Reserved
            .long      0                   # ESP1
            .word      0                   # SS1
            .word      0                   # Reserved
            .long      0                   # ESP2
            .word      0                   # SS2
            .word      0                   # Reserved
            .long      0                   # CR3
            .long      PFHandlerEntry      # EIP
            .long      00000002            # EFLAGS
            .long      0                   # EAX
            .long      0                   # ECX
            .long      0                   # EDX
            .long      0                   # EBX
            .long      0                   # ESP
            .long      0                   # EBP
            .long      0                   # ESI
            .long      0                   # EDI
            .word      DATA_SEL            # ES
            .word      0                   # Reserved
            .word      CODE_SEL            # CS
            .word      0                   # Reserved
            .word      DATA_SEL            # SS
            .word      0                   # Reserved
            .word      DATA_SEL            # DS
            .word      0                   # Reserved
            .word      DATA_SEL            # FS
            .word      0                   # Reserved
            .word      DATA_SEL            # GS
            .word      0                   # Reserved
            .word      0                   # LDT Selector
            .word      0                   # Reserved
            .word      0                   # T
            .word      0                   # I/O Map Base

ASM_PFX(gcPsd):
            .ascii  "PSDSIG  "
            .word      PSD_SIZE
            .word 2
            .word      1 << 2
            .word      CODE_SEL
            .word      DATA_SEL
            .word      DATA_SEL
            .word      DATA_SEL
            .word 0
            .long 0
            .long 0
            .long 0
            .long 0
            .quad 0
            .long      NullSeg
            .long 0
            .long      GDT_SIZE
            .long 0
            .space 24, 0
            .long      ASM_PFX(gSmiMtrrs)
            .long 0
.equ  PSD_SIZE,  . - ASM_PFX(gcPsd)

ASM_PFX(gcSmiGdtr):  .word      GDT_SIZE - 1
                     .long      NullSeg

ASM_PFX(gcSmiIdtr):  .word      IDT_SIZE - 1
                     .long      _SmiIDT

_SmiIDT:
# The following segment repeats 32 times:
# No. 1
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 2
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 3
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 4
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 5
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 6
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 7
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 8
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 9
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 10
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 11
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 12
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 13
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 14
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 15
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 16
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 17
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 18
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 19
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 20
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 21
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 22
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 23
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 24
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 25
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 26
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 27
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 28
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 29
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 30
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 31
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
# No. 32
    .word 0                             # Offset 0:15
    .word      CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31

.equ  IDT_SIZE, . - _SmiIDT

TaskGateDescriptor:
    .word      0                        # Reserved
    .word      EXCEPTION_TSS_SEL        # TSS Segment selector
    .byte      0                        # Reserved
    .byte      0x85                     # Task Gate, present, DPL = 0
    .word      0                        # Reserved

#
# Saved IDT Entry for INT 1
#
ASM_PFX(gSavedDebugExceptionIdtEntry):
    .long      0                        
    .long      0        
    
ASM_PFX(gSmiExceptionHandlers):
# 32 Entries
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(SmiPFHandler)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)
    .long  ASM_PFX(CpuSleep)

    .text

#------------------------------------------------------------------------------
# Exception handlers
#------------------------------------------------------------------------------
_SmiExceptionHandlers:
.equ  IHDLRIDX, 0
# The following segment repeats 8 times:
# No. 1
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 2
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 3
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 4
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 5
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 6
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 7
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 8
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1

    pushl    $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1

    pushl   %eax                        # dummy error code
    pushl    $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1

# The following segment repeats 5 times:
# No. 1
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 2
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 3
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 4
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 5
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1

# The following segment repeats 2 times:
# No. 1
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 2
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1

    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
    int     $3
.equ  IHDLRIDX, IHDLRIDX + 1

# The following segment repeats 14 times:
# No. 1
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 2
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 3
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 4
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 5
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 6
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 7
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 8
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 9
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 10
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 11
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 12
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 13
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1
# No. 14
    pushl   %eax                        # dummy error code
    pushl   $IHDLRIDX
    .byte 0xe9                          # jmp disp32
    .long      _SmiExceptionEntryPoint - (. + 4)
.equ  IHDLRIDX, IHDLRIDX + 1


#------------------------------------------------------------------------------
# _SmiExceptionEntryPoint is the entry point for all exceptions
#
# Stack frame would be as follows as specified in IA32 manuals:
#   RFLAGS  +1ch
#   CS      +18h
#   RIP     +14h
#   ErrCode +10h
#   INT#    +0ch
#   EAX     +8
#   ECX     +4
#   EDX     +0
#
# RSP set to odd multiple of 8 means ErrCode PRESENT
#------------------------------------------------------------------------------
_SmiExceptionEntryPoint:
    pushl   %eax
    pushl   %ecx
    pushl   %edx
    pushl   $0                         # TSS base = 0 indicating SMM Stack Guard not enabled 

    movl    0x18(%esp), %edx           # EIP

_SmiExceptionEntryPoint2:
    movl    0x10(%esp), %ecx           # INT#
    movl    0x14(%esp), %eax           # Errcode

# Push EIP
    push    %edx
# Push ErrCode    
    push    %eax
# Push INT#
    push    %ecx
# UEFI calling convention for IA32 requires that Direction flag in EFLAGs is clear
    cld

    lea     ASM_PFX(gSmiExceptionHandlers), %eax
    call    *(%eax, %ecx, 4)
    popl     %ecx
    popl     %ecx
    popl     %ecx
    popl     %ecx                     # TSS Base

# Set single step DB# if SMM profile is enabled and page fault exception happens
    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmProfileEnable))
    jz      L_Done

# Check if this is page fault exception
    movl    0xc(%esp), %eax
    cmp     $0xe,%eax
    jnz     L_INT1

# Enable TF bit after page fault handler runs
    jecxz  L_INT2

    btsl     $8, EXCEPTION_EFLAGS_OFFSET(%ecx)
    jmp     L_Done

L_INT2:
    movl    0x1c(%esp), %eax
    bts     $8, %eax
    movl    %eax, 0x1c(%esp)
    jmp     L_Done

L_INT1:
# Check if this is INT 1 exception
    movl    0xc(%esp), %eax
    cmp     $0x1,%eax
    jnz     L_Done

# Clear TF bit after INT1 handler runs
    jecxz  L_INT3

    btcl     $8, EXCEPTION_EFLAGS_OFFSET(%ecx)
    jmp     L_Done

L_INT3:
    movl    0x1c(%esp), %eax
    btc     $8, %eax
    movl    %eax, 0x1c(%esp)

L_Done:
    popl    %edx
    popl    %ecx
    popl    %eax
    addl    $8,%esp                     # skip INT# & ErrCode
    iretl                               # iretl(32-bit iret)
#
# Page Fault Exception Handler entry when SMM Stack Guard is enabled
#
PFHandlerEntry:
    pushl   $14                         # INT#
    pushl   %eax
    pushl   %ecx
    pushl   %edx

#
# Get this processor's TSS
#
    subl     $8, %esp
    sgdt     2(%esp)
    movl     4(%esp), %eax             # GDT base
    addl     $8, %esp
    movl     (TSS_SEL + 2)(%eax), %ecx
    shll     $8, %ecx
    movb     (TSS_SEL + 7)(%eax), %cl
    rorl     $8, %ecx                  # ecx = TSS base
    pushl    %ecx                      # Save TSS base

# Workaround: processor does not save CR3 in task switch?
    movl     %cr3, %eax
    movl     %eax, EXCEPTION_CR3_OFFSET(%ecx)
    
    movl     EXCEPTION_EIP_OFFSET(%ecx), %edx  # EIP

    jmp     _SmiExceptionEntryPoint2

ASM_GLOBAL ASM_PFX(InitializeIDT)
ASM_PFX(InitializeIDT):
    pushl   %ebx
    lea     _SmiIDT - 8, %edx
#    push    IDT_SIZE / 8
    .byte 0x68                          # push /id
    .long      IDT_SIZE / 8
    lea      _SmiExceptionHandlers - 8, %ebx
    popl    %ecx
L1: 
    leal    (%ebx,%ecx,8),%eax
    movw    %ax,(%edx,%ecx,8)
    shrl    $16,%eax
    movw    %ax, 6(%edx, %ecx, 8)
    loop    L1

    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmStackGuard))
    jz      L2

#
# If SMM Stack Guard feature is enabled, the Page Fault Exception entry in IDT
# is a Task Gate Descriptor so that when a Page Fault Exception occurrs,
# the processors can use a known good stack in case stack ran out.
#
    leal    _SmiIDT + 14 * 8, %ebx
    leal    TaskGateDescriptor, %edx
    movl    (%edx), %eax
    movl    %eax, (%ebx)
    movl    4(%edx), %eax
    movl    %eax, 4(%ebx)

L2:
    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmProfileEnable))
    jz      L3
    
#
# Save INT 1 IDT entry in gSavedDebugExceptionIdtEntry
#    
    leal    _SmiIDT + 1 * 8, %ebx
    leal    ASM_PFX(gSavedDebugExceptionIdtEntry), %edx
    movl    (%ebx), %eax
    movl    %eax, (%edx)
    movl    4(%ebx), %eax
    movl    %eax, 4(%edx)

L3:
    popl    %ebx
    ret

