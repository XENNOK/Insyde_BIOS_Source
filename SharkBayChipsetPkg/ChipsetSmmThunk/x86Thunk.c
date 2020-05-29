/** @file
   Real Mode Thunk Functions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

   Copyright (c) 2006 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.
*/

#include "x86Thunk.h"

#define IA32API __cdecl

extern CONST UINTN                  mCode16Size;

/**

 Make a far call to 16-bit code.
 
 NOTE: This function must be called on TPL_HIGH_LEVEL or with interrupts
       disabled because of GDTR and IDTR manipulations.
       This function must be placed in identity mapped pages.


 @param[in]        ThunkContext - Thunk context to use.
 @param[in,out]   RegisterSet - 
                                    TCPU registers would be set to the values contained in this
                                    structure before making the far call. Then CPU registers are
                                    copied back to this structure.
                                    CS:EIP points to the real mode code being called on input.
                                    SS:ESP points to the real mode stack if THUNK_USER_STACK is
                                    set on input, otherwise ignored.
                                    EFlages is ignored on input.
                                    On output, values of CS, EIP, SS and ESP should be ignored.
 @param[in]        ThunkFlags -
                                    2 flags have currently been defined, THUNK_SAVE_FP_STATE and
                                    THUNK_USER_STACK.
                                    THUNK_SAVE_FP_STATE - FPU state would be saved/restored
                                                                         before/after calling real mode code.
                                    THUNK_USER_STACK    - The stack specified by SS:ESP would be
                                                                         used instead of the default stack.
 
 @retval None

*/
VOID
EFIAPI
AsmThunk16SetProperties (
  OUT     THUNK_CONTEXT             *ThunkContext,
  IN      VOID                      *RealModeBuffer,
  IN      UINT32                    BufferSize
  )
{
  BufferSize &= ~3;

  ThunkContext->RealModeBuffer = RealModeBuffer;
  ThunkContext->RealModeBufferSize = BufferSize;
  //ThunkContext->DefaultStack   = (UINT32)(ThunkContext->RealModeBuffer + BufferSize);
  //CopyMem (RealModeBuffer, (VOID*)(UINTN)_Code16Addr, mCode16Size);

}

/**

 Make a far call to 16-bit code.
 
 NOTE: This function must be called on TPL_HIGH_LEVEL or with interrupts
       disabled because of GDTR and IDTR manipulations.
       This function must be placed in identity mapped pages.


 @param[in]        ThunkContext - Thunk context to use.
 @param[in,out]   RegisterSet - 
                                    TCPU registers would be set to the values contained in this
                                    structure before making the far call. Then CPU registers are
                                    copied back to this structure.
                                    CS:EIP points to the real mode code being called on input.
                                    SS:ESP points to the real mode stack if THUNK_USER_STACK is
                                    set on input, otherwise ignored.
                                    EFlages is ignored on input.
                                    On output, values of CS, EIP, SS and ESP should be ignored.
 @param[in]        ThunkFlags -
                                    2 flags have currently been defined, THUNK_SAVE_FP_STATE and
                                    THUNK_USER_STACK.
                                    THUNK_SAVE_FP_STATE - FPU state would be saved/restored
                                                                         before/after calling real mode code.
                                    THUNK_USER_STACK    - The stack specified by SS:ESP would be
                                                                         used instead of the default stack.
 
 @retval None

*/
VOID
EFIAPI
AsmThunk16FarCall86 (
  IN      THUNK_CONTEXT             *ThunkContext,
  IN OUT  IA32_REGISTER_SET         *RegisterSet,
  IN      UINT32                    Flags
  )
{
  ThunkContext->RealModeState = RegisterSet;
  AsmThunk16 (ThunkContext);
}

/**

 Invoke a 16-bit interrupt handler.
 
 NOTE: This function must be called on TPL_HIGH_LEVEL or with interrupts
       disabled because of GDTR and IDTR manipulations.
       This function must be placed in identity mapped pages.

 @param[in]        ThunkContext - Thunk context to use.
 @param[in]        IntNumber     - The ordinal of the interrupt handler ranging from 0 to 255.
 @param[in,out]   RegisterSet  - 
                                CPU registers would be set to the values contained in this
                                structure before making the far call. Then CPU registers are
                                copied back to this structure.
                                SS:ESP points to the real mode stack if THUNK_USER_STACK is
                                set on input, otherwise ignored.
                                EFlages is ignored on input.
                                On output, values of CS, EIP, SS and ESP should be ignored.
 @param[in]        ThunkFlags -
                                    2 flags have currently been defined, THUNK_SAVE_FP_STATE and
                                    THUNK_USER_STACK.
                                    THUNK_SAVE_FP_STATE - FPU state would be saved/restored
                                                                         before/after calling real mode code.
                                    THUNK_USER_STACK    - The stack specified by SS:ESP would be
                                                                        used instead of the default stack.
 
 @retval None

*/
VOID
EFIAPI
AsmThunk16Int86 (
  IN      THUNK_CONTEXT             *ThunkContext,
  IN      UINT8                     IntNumber,
  IN OUT  IA32_REGISTER_SET         *RegisterSet,
  IN      UINT32                    Flags
  )
{
  RegisterSet->E.Eip = (UINT16)((UINT32 *)NULL)[IntNumber];
  RegisterSet->E.CS  = (UINT16)(((UINT32 *)NULL)[IntNumber] >> 16);
  ThunkContext->RealModeState = RegisterSet;
  AsmThunk16 (ThunkContext);
}
