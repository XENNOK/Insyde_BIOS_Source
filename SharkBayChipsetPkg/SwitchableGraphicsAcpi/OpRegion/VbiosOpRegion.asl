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
    OperationRegion (VBOR, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (VBOR, DWordAcc, Lock, Preserve)
    {
        RVBS,     32,  // Runtime VBIOS image size
        VBS1, 262144,  // 0x8000 bytes in bits for VBIOS
        VBS2, 262144,  // 0x8000 bytes in bits for VBIOS
        VBS3, 262144,  // 0x8000 bytes in bits for VBIOS
        VBS4, 262144,  // 0x8000 bytes in bits for VBIOS
    }
}
