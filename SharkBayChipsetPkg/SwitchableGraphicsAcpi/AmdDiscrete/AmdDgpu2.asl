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

Scope (DGPU2_BRIDGE_SCOPE)
{
    //
    // Get bus number for this DGPU
    //     
    OperationRegion (PE19, PCI_Config, 0x19, 0x1)
    Field (PE19, ByteAcc, NoLock, Preserve){
        PSBN,8
    }

    Device (DGPU2_DEVICE)
    {       
        Name (_ADR, 0x00000000)
        Name (_PRW, Package() {0x09,4})	// can wakeup from S4 state

        Method (_INI) 
        {
        Store (Zero, IGDS) // Integated Graphics Device Disabled
        }

        Method (GBUS, 0x0)
        {
            Return (^^PSBN) // Return DGPU bus number.
        }
    } // end of Device (DGPU)
}
