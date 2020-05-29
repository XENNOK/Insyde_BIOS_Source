/** @file
  IA32 Exception Includes.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _IA32EXCEPTION_H
#define _IA32EXCEPTION_H

///
/// Driver Consumed Protocol Prototypes
///

#define INTERRUPT_HANDLER_DIVIDE_ZERO           0x00
#define INTERRUPT_HANDLER_DEBUG                 0x01
#define INTERRUPT_HANDLER_NMI                   0x02
#define INTERRUPT_HANDLER_BREAKPOINT            0x03
#define INTERRUPT_HANDLER_OVERFLOW              0x04
#define INTERRUPT_HANDLER_BOUND                 0x05
#define INTERRUPT_HANDLER_INVALID_OPCODE        0x06
#define INTERRUPT_HANDLER_DEVICE_NOT_AVAILABLE  0x07
#define INTERRUPT_HANDLER_DOUBLE_FAULT          0x08
#define INTERRUPT_HANDLER_COPROCESSOR_OVERRUN   0x09
#define INTERRUPT_HANDLER_INVALID_TSS           0x0A
#define INTERRUPT_HANDLER_SEGMENT_NOT_PRESENT   0x0B
#define INTERRUPT_HANDLER_STACK_SEGMENT_FAULT   0x0C
#define INTERRUPT_HANDLER_GP_FAULT              0x0D
#define INTERRUPT_HANDLER_PAGE_FAULT            0x0E
#define INTERRUPT_HANDLER_RESERVED              0x0F
#define INTERRUPT_HANDLER_MATH_FAULT            0x10
#define INTERRUPT_HANDLER_ALIGNMENT_FAULT       0x11
#define INTERRUPT_HANDLER_MACHINE_CHECK         0x12
#define INTERRUPT_HANDLER_STREAMING_SIMD        0x13

typedef struct {
  EFI_STATUS_CODE_DATA Header;
  union {
    EFI_SYSTEM_CONTEXT_IA32 SystemContextIa32;
    EFI_SYSTEM_CONTEXT_X64  SystemContextX64;
  } SystemContext;
} CPU_STATUS_CODE_TEMPLATE;

/**
  Common exception handler

  @param[in] InterruptType - Exception type
  @param[in] SystemContext - EFI_SYSTEM_CONTEXT
**/
VOID
EFIAPI
CommonExceptionHandler (
  IN EFI_EXCEPTION_TYPE InterruptType,
  IN EFI_SYSTEM_CONTEXT SystemContext
  );

/**
  Install the IA-32 EM64T Exception Handler.
  The current operation (which likely will change) will uninstall all the
  pertinent exception handlers (0-7, 10-14, 16-19) except for Int8 which the timer
  is currently sitting on (or soon will be).

  It then installs all the appropriate handlers for each exception.

  The handler then calls gRT->ReportStatusCode with a specific progress code.  The
  progress codes for now start at 0x200 for IA-32 processors. See Status Code
  Specification for details. The Status code Specification uses the enumeration from
  the EFI 1.1 Debug Support Protocol.

  @param[in] CpuProtocol - Instance of CPU Arch Protocol

  @retval EFI_SUCCESS - This function always return success after registering handlers.
**/
EFI_STATUS
InitializeException (
  IN EFI_CPU_ARCH_PROTOCOL *CpuProtocol
  );

#endif
