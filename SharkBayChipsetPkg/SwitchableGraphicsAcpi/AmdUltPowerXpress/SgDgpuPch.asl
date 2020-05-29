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

Scope (DGPU_BRIDGE_SCOPE)
{
    //
    // Define a Memory Region that will allow access to the PEG root port
    // Register Block.
    //
    OperationRegion (RPCI, PCI_Config, 0x00, 0xF0)
    Field (RPCI, DWordAcc, Lock, Preserve)
    {
        Offset (0x50),  // LCTL - Link Control Register
        ASPM,  2,       // 1:0, ASPM //Not referenced in code
            ,  2,
        LNKD,  1,       // Link Disable
    }

    OperationRegion (RPCX, SystemMemory, DGPU_SCOPE.DBPA, 0x1000)
    Field (RPCX, DWordAcc, NoLock, Preserve)
    {
        Offset (0x328), // PCIESTS1 - PCI Express Status 1
        ,       19,
        LNKS,   4,     // Link Status (LNKSTAT) {22:19}
    }

    Device (DGPU_AUDIO_DEVICE) { // PEG Audio Device 0 Function 1
        Name (_ADR, 0x00000001)
        Name (_PRW, Package() {0x09,4}) // can wakeup from S4 state
        OperationRegion (ACAP, PCI_Config, DGPU_SCOPE.EECP, 0x14)
        Field (ACAP, DWordAcc, NoLock, Preserve)
        {
            Offset (0x10),
            LCT1,  16,  // Link Control register
        }
    }
}

Scope (DGPU_SCOPE)
{       
    Name (ELCT, 0x00000000)
    Name (HVID, 0x0000)
    Name (HDID, 0x0000)

    OperationRegion (PCIS, PCI_Config, 0x00, 0x100)
    Field (PCIS, DWordAcc, Lock, Preserve)
    {
        Offset (0x00),
        DVID,  16,
        Offset (0xB),
        CBCC, 8,
        Offset (0x2C),
        SVID,  16,
        SDID,  16,
        Offset (0x4C),
        WVID,  16,
        WDID,  16,
    }

    OperationRegion (PCAP, PCI_Config, EECP, 0x14)
    Field (PCAP, DWordAcc, NoLock, Preserve)
    {
        Offset (0x10),                    
        LCTL,  16,      // Link Control register
    }

    Method (_INI)
    {
        Store (0x0, DGPU_SCOPE._ADR)
    }

    Method (_ON, 0, Serialized)
    {
        // ACTION TODO:
        //........................................................................................
        //  While powering up the slot again, the only requirement is that the Reset# should be 
        //  de-asserted 100ms after the power to slot is up (Standard requirement as per PCIe spec).
        // Note:
        //  Before power enable, and for 100ms after power enable, the reset should be in hold condition.
        //  The 100 ms time is given for power rails and clocks to become stable.
        //  So during this period, reset must not be released.
        //........................................................................................

        // Power on the ElkCreek card
        SGPO (HLRS, 1)  // Assert dGPU_HOLD_RST#
        SGPO (PWEN, 1)  // Assert dGPU_PWR_EN#
//        While (LNotEqual (SGPI (PWOK), 1))  // Wait until dGPU_PWROK = 1
//        {
//            Sleep(1)                
//        }
        Sleep (APDT)    // Wait for 300ms if dGPU_PWROK has h/w issues
        SGPO (HLRS, 0)  // Deassert dGPU_HOLD_RST#
        Sleep (IHDT)    // Wait for 100ms

        // Enable x4 Link
        // This bit should already be set to 0 in the _Off method. But do it anyway.
        Store (0, LNKD)

        // wait until link has trained to x4. Verify        
        While (LLess (LNKS, 7))
        {
            Sleep(1)
        }

        // Re-store the DGPU SSID
        Store (HVID, WVID)
        Store (HDID, WDID)

        // Re-store the Link Control register - Common Clock Control and ASPM
        Or (And (ELCT, 0x0043), And (LCTL, 0xFFBC), LCTL)
        Or (And (ELCT, 0x0043), And (DGPU_AUDIO_SCOPE.LCT1, 0xFFBC), DGPU_AUDIO_SCOPE.LCT1)

        // Ask OS to do a PnP rescan
        Notify (DGPU_BRIDGE_SCOPE, 0)

        Return (1)                      
    }

    Method (_OFF, 0, Serialized)
    {
        // ACTION TODO:
        //........................................................................................
        // To turn off the power to the slot, all you would need to do is assert the RESET# 
        // and then take off the power using the power enable GPIO.
        // Once the power goes off, the clock request from the slot to the PCH is also turned off, 
        // so no clocks will be going to the PCIe slot anymore.
        //........................................................................................

        // Save the Link Control register
        Store (LCTL, ELCT)

        // Save the DGPU SSID
        Store (SVID, HVID)
        Store (SDID, HDID)

        // Force disable the x4 link
        Store (1, LNKD)

        // Wait till link is actually in disabled state
        While (LNotEqual (LNKS, 0))
        {
            Sleep (1)
        }

        // Power-off ElkCreek
        SGPO (HLRS, 1)  // Assert dGPU_HOLD_RST# (PERST#)
        SGPO (PWEN, 0)  // Deassert dGPU_PWR_EN#

        // Ask OS to do a PnP rescan
        Notify (DGPU_BRIDGE_SCOPE, 0)

        Return (0)
    }
        
    Method (_STA, 0, Serialized)
    {
        //
        // Check SGMode and dGPU Present Detect GPIO for SG system
        //
        If (And (SGMD, 0x0F))
        {
            If (LNotEqual (SGGP, 0x01))
            {
               Return (0x0F)
            }
            // Check dGPU_PWROK to detect dGPU.
            If (LEqual (SGPI (PWOK), 1))  
            {
                Return (0x0F)
            }
            Return (0x00)
        }
  
        //
        // For non-SG system check for valid Vendor Id
        //
        If (LNotEqual (DVID, 0xFFFF))
        {
            Return (0x0F)
        }
        Return (0x00)
    }

    Method (_DOD, 0)
    {
        Return (IGPU_SCOPE._DOD ())
    }

    Device (DD01)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD01._ADR ())
        }

        // Device Current State.
        Method (_DCS,0)
        {

        }

        // Device Get State.
        Method (_DGS,0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD01._DGS ())
        }

        // Device Set State.
        // _DSS Table:
        //
        //  BIT31   BIT30   Execution
        //  0       0       Don't implement.
        //  0       1       Cache change.  Nothing to Implement.
        //  1       0       Don't Implement.
        //  1       1       Display Switch Complete.  Implement.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD02)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD02._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
            Return (IGPU_SCOPE.DD02._DCS ())
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD02._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
/*
        Method (_DDC, 1)
        {
            If (Lor (LEqual (IGPU_SCOPE.PHED, 1), LEqual (IGPU_SCOPE.PHED, 2)))
            {
                Name (DDC2, Buffer(256) {0x0})
                Store (IGPU_SCOPE.BDDC, DDC2)
                Return (DDC2)
            }
            Return (Buffer(256) {0x0})
        }
*/
        Method (_BCL, 0)
        {
            Return (IGPU_SCOPE.DD02._BCL ())               
        }

        Method (_BQC, 0)
        {
            Return (IGPU_SCOPE.DD02._BQC ())
        }
        
        Method (_BCM, 1)
        {
            Return (IGPU_SCOPE.DD02._BCM (Arg0))
        }
    }

    Device (DD03)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD03._ADR ())
        }

        // Device Current State.

        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.

        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD03._DGS ())
        }

        // Device Set State.

        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD04)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD04._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD04._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD05)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD05._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD05._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD06)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD06._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD06._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD07)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD07._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD07._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    Device (DD08)
    {
        Method (_ADR, 0, Serialized)
        {
            Return (IGPU_SCOPE.DD08._ADR ())
        }

        // Device Current State.
        Method (_DCS, 0)
        {
            // Get the Current Display State.
        }

        // Device Get State.
        Method (_DGS, 0)
        {
            // Return the Next State.
            Return (IGPU_SCOPE.DD08._DGS ())
        }

        // Device Set State.
        Method (_DSS, 1)
        {
            // Do nothing here in the OpRegion model.  OEMs may choose to
            // update internal state if necessary.
        }
    }

    // GPIO Read
    // Arg0 = GPIO No + GPIO active info. {BIT7 => (1:Active, 0: Not active), BIT6:0 => GPIO No}
    Method (SGPI, 1, Serialized)
    {
        ShiftRight (Arg0, 7, Local1) // GPIO active info
        And (Arg0, 0x7F, Arg0) // GPIO No
        // Read the GPIO [GPI_LVL]
        // Arg0 - GPIO Pin number to read
        If (LLessEqual (Arg0, 94))
        {
            Store (Add (Add (DGPU_SCOPE.GBAS, 0x100) , Multiply (Arg0, 0x08)), Local0)
            OperationRegion (LGPI, SystemIo, Local0, 8)
            Field (LGPI, ByteAcc, NoLock, Preserve) {
                Offset(0x0),
                , 30,
                TEMP, 1
            }
            Store (TEMP, Local2)
        }
        // 
        // Check if Active Low
        //
        If (LEqual (Local1, 0))
        {
            Not (Local2, Local2)
        }
        Return (And (Local2, 0x01))
    }
  

    // GPIO Write
    // Arg0 = GPIO No + GPIO active info. {BIT7 => (1:Active, 0: Not active), BIT6:0 => GPIO No}
    // Arg1 = Value (0/1)
    Method (SGPO, 2, Serialized)
    {
        ShiftRight (Arg0, 7, Local1) // GPIO active info
        And (Arg0, 0x7F, Arg0) // GPIO No
        // 
        // Check if Active Low
        //
        If (LEqual (Local1, 0))
        {
            Not (Arg1, Arg1)
        }
        And (Arg1, 0x01, Arg1)
        // Program the GPIO [GPO_LVL]
        // Arg0 - GPIO Pin number to write
        // Arg1 - Value to be written
        If (LLessEqual (Arg0, 94))
        {
            Store (Add (Add (DGPU_SCOPE.GBAS, 0x100), Multiply (Arg0, 0x08)), Local0)
            OperationRegion (LGPI, SystemIo, Local0, 8)
            Field (LGPI, ByteAcc, NoLock, Preserve) {
                Offset (0x0),
                , 31,
                TEMP, 1
            }
            Store (Arg1, TEMP)
        }
    }
}
