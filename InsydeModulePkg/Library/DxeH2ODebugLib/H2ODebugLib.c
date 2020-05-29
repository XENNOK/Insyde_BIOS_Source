/** @file
  H2O Debug Library

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Protocol/DebugComm.h>

VOID*
DDTGetDebugComm (
  )
{
#if defined(MDE_CPU_IA32)
#ifdef __GNUC__
  __asm__ ("subl   $0x10, %esp\n\t"
           "sidt   (%esp)\n\t"
           "movl   0x02(%esp), %eax\n\t"
           "addl   $0x10, %esp\n\t"
           "movw   0x1e(%eax), %dx\n\t"
           "shll   $16, %edx\n\t"
           "movw   0x18(%eax), %dx\n\t"
           "xorl   %eax, %eax\n\t"
           "cmpl   $0x44656267, -0x10(%edx)\n\t"
           "jne    quit\n\t"
           "movl   -0x18(%edx), %eax\n"
           "quit:");
  return NULL;
#else
  __asm
  {
    sub    esp, 0x10
    sidt   fword ptr [esp]
    mov    eax, [esp + 2]
    add    esp, 0x10
    mov    dx, [eax + 0x1e]
    shl    edx, 16
    mov    dx, [eax + 0x18]
    xor    eax, eax
    cmp    dword ptr [edx - 0x10], 0x44656267
    jne    quit
    mov    eax, [edx - 0x18]
quit:
  };
#endif
#elif defined(MDE_CPU_X64)
  UINT8 OpCode[] = { 0x48, 0x83, 0xEC, 0x10, 0x0F, 0x01, 0x0C, 0x24, 0x48, 0x8B, 0x44, 0x24, 0x02, 0x48, 0x83, 0xC4,
                     0x10, 0x8B, 0x50, 0x38, 0x48, 0xC1, 0xE2, 0x10, 0x66, 0x8B, 0x50, 0x36, 0x48, 0xC1, 0xE2, 0x10,
                     0x66, 0x8B, 0x50, 0x30, 0x48, 0x33, 0xC0, 0x81, 0x7A, 0xF0, 0x67, 0x62, 0x65, 0x44, 0x75, 0x04,
                     0x48, 0x8B, 0x42, 0xE8, 0xC3 };
  VOID* (*DDTGetDebugCommX64)();
  *(UINT8**)&DDTGetDebugCommX64 = OpCode;
  return DDTGetDebugCommX64();
#endif
}

VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  )
{
  H2O_DEBUG_COMM_PROTOCOL       *DebugComm;
  VA_LIST                       Args;

  DebugComm = DDTGetDebugComm();
  if (DebugComm) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}
