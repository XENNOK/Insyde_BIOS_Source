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
    OperationRegion (NOPR, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (NOPR, AnyAcc, Lock, Preserve)
    {
        DHPS,       8,  // Optimus dGPU HotPlug Support
        DPCS,       8,  // Optimus dGPU Power Control Support
        GPSS,       8,  // GPS Feature Support
        VENS,       8,  // Ventura Feature Support
        GC6S,       8,  // Optimus GC6 Feature Support
        SLVS,       8,  // Slave dGPU Support
        PBCM,       8,  // SpbConfig
        GPSP,       8,  // Expansion Mode
        MXBS,      32,  // MXM bin file Size
        MXMB,   32768,  // 0x1000 bytes in bits
        SMXS,      32,  // Slave MXM bin file Size
        SMXB,   32768,  // Slave MXM binary 0x1000 bytes in bits
    }
}
