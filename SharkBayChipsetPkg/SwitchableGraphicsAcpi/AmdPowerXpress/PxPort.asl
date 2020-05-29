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

Name (DGPU_SCOPE.GSTP, 0) // GPU switch transition in progress
Name (DGPU_SCOPE.TGXA, 0) // Target GFX adapter as notified by ATPX function 5
Name (DGPU_SCOPE.AGXA, 0) // Active GFX adapter as notified by ATPX function 6
Name (DGPU_SCOPE.DSWR, 0) // Display switch request
Name (DGPU_SCOPE.EMDR, 0) // Expansion mode change request
Name (DGPU_SCOPE.PXGS, 0) // PowerXpress graphics switch toggle request state
Name (DGPU_SCOPE.CACD, 0) // Currently active displays
Name (DGPU_SCOPE.CCND, 0) // Currently connected displays
Name (DGPU_SCOPE.NACD, 0) // Next active displays
Name (DGPU_SCOPE.TLSN, 0) // Toggle list sequence index

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

Method (HGAS, 0, NotSerialized)
{
    Store (One, DGPU_SCOPE.PXGS)
    // Notify 0x81 - Video Controller Reenumerate Video Devices (_DOD)
    Notify (IGPU_SCOPE, 0x81)
}

Method (HBRT, 1, Serialized)
{
    //
    // Send backlight notifications to the DGPU LFP device. 
    // This is required for Win7 and is backward compatible with Vista.
    //
    If (And (0x04, DSEN))
    {
        If (LEqual (Arg0, 0x04))
        {
            // Notify 0x87
            Notify (DGPU_SCOPE.DD02, 0x87)
        }
        If (LEqual (Arg0, 0x03))
        {
            // Notify 0x86
            Notify (DGPU_SCOPE.DD02, 0x86)
        }
    }
}

Method (HPFS, 0, Serialized)
{
    If (LNot (LEqual (DGPU_SCOPE.GSTP, One)))
    {
        //
        // HG Handling of Panel Fitting Switch
        //
        If (LEqual (DGPU_SCOPE.AGXA, Zero))
        {
            IGPU_SCOPE.AINT (0x02, Zero)
        }
        Else
        {
            Store (One, DGPU_SCOPE.EMDR)
            //
            // Expansion Mode toggling
            //
            If (LEqual (DGPU_SCOPE.EXPM, 0x02))
            {
                Store (Zero, DGPU_SCOPE.EXPM)
            }
            Else
            {
                Increment (DGPU_SCOPE.EXPM)
            }
            // Notify 0x81 - Video Controller Reenumerate Video Devices (_DOD)
            Notify (IGPU_SCOPE, 0x81)
        }
    }
}

Method (HNOT, 1, Serialized)
{
    Name (TMP0, Zero)
    While (One)
    {
        Store (Arg0, TMP0)
        If (LEqual (TMP0, 0x01))
        {
            If (LNot (LEqual (DGPU_SCOPE.GSTP, One)))
            {
                If (And (DGPU_SCOPE.SGMD, 0x02))
                {
                    If (LEqual (DGPU_SCOPE.AGXA, Zero))
                    {
                        // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                        Notify (IGPU_SCOPE, 0x80)
                    }
                    If (LEqual (DGPU_SCOPE.AGXA, One))
                    {
                        // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                        Notify (DGPU_SCOPE, 0x80)
                    }
                }
                Else
                {
                    Store (TMP0, DGPU_SCOPE.DSWR)
                    // Notify 0x81 - Video Controller Reenumerate Video Devices (_DOD)
                    Notify (IGPU_SCOPE, 0x81)
                }
            }
        }
        Else
        {
            If (LEqual (TMP0, 0x02))
            {
                If (And (DGPU_SCOPE.SGMD, 0x02))
                {
                    If (LEqual (DGPU_SCOPE.AGXA, Zero))
                    {
                        // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                        Notify (IGPU_SCOPE, 0x80)
                    }
                    If (LEqual (DGPU_SCOPE.AGXA, One))
                    {
                        // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                        Notify (DGPU_SCOPE, 0x80)
                    }
                }
                Else
                {
                    // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                    Notify (IGPU_SCOPE, 0x80)
                }
            }
            Else
            {
                If (LEqual (TMP0, 0x03))
                {
                    If (And (DGPU_SCOPE.SGMD, 0x02))
                    {
                        If (LEqual (DGPU_SCOPE.AGXA, Zero))
                        {
                            // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                            Notify (IGPU_SCOPE, 0x80)
                        }
                        If (LEqual (DGPU_SCOPE.AGXA, One))
                        {
                            // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                            Notify (DGPU_SCOPE, 0x80)
                        }
                    }
                    Else
                    {
                        // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                        Notify (IGPU_SCOPE, 0x80)
                    }
                }
                Else
                {
                    // Notify 0x80 - Video Controller Switch Current Video Device(_DOS)
                    Notify (IGPU_SCOPE, 0x80)
                }
            }
        }
        Break
    }
}
