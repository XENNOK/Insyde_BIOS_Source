/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2006 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ThunkLib.h

@brief
  Real Mode Thunk Functions for IA32 and X64
**/
#ifndef __THUNK_LIB__
#define __THUNK_LIB__

///
/// Byte packed structure for an 16-bit real mode thunks
///
typedef struct {
  UINT32 RealModeBuffer;
  UINT32 DefaultStack;
} THUNK16_CONTEXT;

//
// 16-bit thunking services
//
#define THUNK_SAVE_FP_STATE 0x1
#define THUNK_USER_STACK    0x2
#define THUNK_INTERRUPT     0x10000

//
// Function Prototypes
//
/**
  Returns the properties of this real mode thunk implementation. Currently
  there are 2 properties has been defined, the minimum real mode buffer size
  and the minimum stack size.

  @param[in] MinimumStackSize  - The minimum size required for a 16-bit stack.

  @retval The minimum size of the real mode buffer needed by this thunk implementation
  @retval is returned.
**/
UINTN
EFIAPI
R8AsmThunk16GetProperties (
  OUT UINTN *MinimumStackSize OPTIONAL
  );

/**
  Tell this real mode thunk implementation the address and size of the real
  mode buffer needed.

  @param[in] ThunkContext    - The thunk context whose properties to set.
  @param[in] RealModeBuffer  - The address of the buffer allocated by caller. It should be
                    aligned on a 16-byte boundary.
                    This buffer must be in identity mapped pages.
  @param[in] BufferSize      - The size of RealModeBuffer. Must be larger than the minimum
                    size required as returned by R8AsmThunk16GetProperties().
**/
THUNK16_CONTEXT *
EFIAPI
R8AsmThunk16SetProperties (
  OUT THUNK16_CONTEXT *ThunkContext,
  IN VOID             *RealModeBuffer,
  IN UINTN            BufferSize
  );

/**
  Reset all internal states to their initial values. The caller should not
  release the real mode buffer until after a call to this function.

  @param[in] ThunkContext  - The thunk context to destroy.
**/
VOID
EFIAPI
R8AsmThunk16Destroy (
  IN OUT THUNK16_CONTEXT *ThunkContext
  );

/**
  Make a far call to 16-bit code.

  NOTE: This function must be called on TPL_HIGH_LEVEL or with interrupts
        disabled because of GDTR and IDTR manipulations.
  This function must be placed in identity mapped pages.

  @param[in] ThunkContext  - Thunk context to use.
  @param[in] RegisterSet   - CPU registers would be set to the values contained in this
                  structure before making the far call. Then CPU registers are
                  copied back to this structure.
                  CS:EIP points to the real mode code being called on input.
                  SS:ESP points to the real mode stack if THUNK_USER_STACK is
                  set on input, otherwise ignored.
                  EFlages is ignored on input.
                  On output, values of CS, EIP, SS and ESP should be ignored.
  @param[in] ThunkFlags    - 2 flags have currently been defined, THUNK_SAVE_FP_STATE and
                  THUNK_USER_STACK.
                  THUNK_SAVE_FP_STATE - FPU state would be saved/restored
                                        before/after calling real mode code.
                  THUNK_USER_STACK    - The stack specified by SS:ESP would be
                                        used instead of the default stack.

  @retval RegisterSet is returned.
**/
IA32_REGISTER_SET *
EFIAPI
R8AsmThunk16FarCall86 (
  IN THUNK16_CONTEXT       *ThunkContext,
  IN OUT IA32_REGISTER_SET *RegisterSet,
  IN UINT32                ThunkFlags
  );

/**
  Invoke a 16-bit interrupt handler.

  NOTE: This function must be called on TPL_HIGH_LEVEL or with interrupts
        disabled because of GDTR and IDTR manipulations.
  This function must be placed in identity mapped pages.

  @param[in] ThunkContext  - Thunk context to use.
  @param[in] IntNumber     - The ordinal of the interrupt handler ranging from 0 to 255.
  @param[in] RegisterSet   - CPU registers would be set to the values contained in this
                  structure before making the far call. Then CPU registers are
                  copied back to this structure.
                  SS:ESP points to the real mode stack if THUNK_USER_STACK is
                  set on input, otherwise ignored.
                  EFlages is ignored on input.
                  On output, values of CS, EIP, SS and ESP should be ignored.
  @param[in] ThunkFlags    - 2 flags have currently been defined, THUNK_SAVE_FP_STATE and
                  THUNK_USER_STACK.
                  THUNK_SAVE_FP_STATE - FPU state would be saved/restored
                                        before/after calling real mode code.
                  THUNK_USER_STACK    - The stack specified by SS:ESP would be
                                        used instead of the default stack.

  @retval RegisterSet is returned.
**/
IA32_REGISTER_SET *
EFIAPI
R8AsmThunk16Int86 (
  IN THUNK16_CONTEXT       *ThunkContext,
  IN UINT8                 IntNumber,
  IN OUT IA32_REGISTER_SET *RegisterSet,
  IN UINT32                ThunkFlags
  );

#endif
