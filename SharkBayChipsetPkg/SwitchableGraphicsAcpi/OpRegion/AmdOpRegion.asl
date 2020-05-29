/** @file
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

Scope (DGPU_SCOPE) {
    OperationRegion (AOPR, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (AOPR, AnyAcc, Lock, Preserve)
    {
        SLVS,       8,  // Slave dGPU Support
        EXPM,       8,  // Expansion Mode
        PXDY,       8,  // Dynamic Support State 1: dynamic, 0: Non-dynamic
        PXFD,       8,  // Fixed   Support State 1: Fixed,   0: Non-fixed
        FDPD,       8,  // Full dGPU PowerOff Dynamic mode
    }
}
