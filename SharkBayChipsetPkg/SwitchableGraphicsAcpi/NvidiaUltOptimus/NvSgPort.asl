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

Name (DGPU_SCOPE.GACD, 0)
Name (DGPU_SCOPE.GATD, 0)

Name (BDFR, 0x0F804C) // Bus 0, Device 0x1F, Function 0, Register 0x4C
Name (GPLS, 0x00)     // GPIO Lockdown Status

OperationRegion (GPCR, SystemMemory, Or (PEBS, BDFR, BDFR), 0x1) // GPIO Control Register
Field (GPCR, AnyAcc, NoLock, Preserve) {
    GPCT,  1,  // bit 0: GPIO Lockdown Enable
}

OperationRegion (APMR, SystemIO, 0xB2, 0x2)
Field (APMR, WordAcc, NoLock, Preserve)
{
    APMC, 8,
    APMS, 8,
}

Name (BDF2, 0x0000DC) // Bus 0, Device 0, Function 0, Register 0xDC
OperationRegion (SKPR, SystemMemory, Or (PEBS, BDF2, BDF2), 0x4) // GPIO Control Register
Field (SKPR, AnyAcc, NoLock, Preserve) {
    SKPD,  32,
}

