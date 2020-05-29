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
    OperationRegion (SGOP, SystemMemory, 0x55AA55AA, 0x55AA55AA)
    Field (SGOP, AnyAcc, Lock, Preserve)
    {
        XBAS,  32,  // Any Device's PCIe Config Space Base Address
        EBAS,  32,  // PEG Endpoint PCIe Base Address
        EECP,  32,  // PEG Endpoint PCIe Capability Structure Base Address
        DBPA,  32,  // dGPU bridge's PCIe Config Space Address
        SGGP,   8,  // SG GPIO Support
        SGMD,   8,  // SG Mode (0 = Disabled, 1 = SG Muxed, 2 = SG Muxless, 3 = DGPU Only)
        PWOK,   8,  // dGPU PWROK GPIO assigned
        HLRS,   8,  // dGPU HLD RST GPIO assigned
        DSEL,   8,  // dGPU Display Select GPIO assigned
        ESEL,   8,  // dGPU EDID Select GPIO assigned
        PSEL,   8,  // dGPU PWM Select GPIO assigned
        PWEN,   8,  // dGPU PWR Enable GPIO assigned
        PRST,   8,  // dGPU Present Detect GPIO assigned
        GBAS,  16,  // GPIO Base Address
    }
}
