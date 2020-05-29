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
        OperationRegion (GPPB, PCI_Config, 0xB4, 0x14)     
        Field (GPPB, ByteAcc, NoLock, Preserve) {
            PBCI, 8,  // Caps
            PBNP, 8,  // Next Ptr
            PBLS, 8,  // Size
            PBCC, 8,  // CONTROL
            PBCM, 32, // Command
            PBDI, 32, // Data In
            PBDO, 32, // Data Out
            PBMX, 32  // Mutex
        }

        // MINT
        // Input
        // Arg0 - type of command
        //        1 - Get Current Brightness level
        //        2 - to set brightness level, 
        // Arg1 - target brightness level. percentage ranged from 0 to 100	
        Method (NINT, 2, NotSerialized)
        {
            If(LEqual(PBCC, One))
            {
                If (LEqual (Arg0, One))
                {
                    Store (One, PBMX)
                    Store (Zero, Local0)
                    While (LLess (Local0, 0x03E8))
                    {
                        Store (0xC5100000, PBCM)
                        Stall (0x64)
                        And (PBCM, 0xFF, Local1)
                        Increment (Local0)
                        If (LEqual (Local1, One))
                        {
                            Store (0x03E8, Local0)
                        }
                    }
                    Store (Zero, PBCM)
                    Store (Zero, PBMX)
                    Store (PBDO, Local2)
                    Return (Local2)
                }
                If (LEqual (Arg0, 0x02))
                {
                    Store (Arg1, Local1)
                    Store (Zero, Local0)
                    Multiply (Local1, 0x0a, Local3)
                    ShiftLeft (Local3, 0x08, Local3)
                    Store (Local3, PBDI)
                    While (LLess (Local0, 0x03E8))
                    {
                        Store (One, PBMX)
                        Store (0x86100000, PBCM)
                        Stall (0x64)
                        And (PBCM, 0xFF, Local2)
                        Increment (Local0)
                        If (LEqual (Local2, One))
                        {
                            Store(0x03E8, Local0)
                        }
                    }
                    Store (Zero, PBCM)
                    Store (Zero, PBMX)
                    Return (Zero)
                }
            }
            Return (Zero)
        }

        Method (GBUS, 0x0)
        {
            Return (^^PSBN) // Return DGPU bus number.
        }

        //
        // For WMI wrapper code.
        // Return supported MXM version.
        // Arg0=MXM version.
        //
        Method (MXMI, 1) 
        {
            If (LEqual (Arg0, 0x20)) { // 2.0?                        
                Return (Arg0)
            }
            If (LEqual (Arg0, 0x21)) { // 2.1?
                Return (Arg0)
            }
            If (LEqual (Arg0, 0x30)) { // 3.0?
                Return (Arg0)
            }
            If (LEqual (Arg0, 0x0)) { // Return highest supported version.
                Return (0x30)                       
            }
            Return (0x0) // Other versions, not supported.
        }

        //
        // For WMI wrapper code.
        // Return MXM structure.
        // Arg0=MXM version
        //
        Method (MXMS, 1)
        {
            If (LEqual (Arg0, 0x30))
            {
                If (LNotEqual (DGPU_SCOPE.SMXS, 0))
                {
                    Name (MXM3, Buffer(DGPU_SCOPE.SMXS) {0x00})
                    Store (DGPU_SCOPE.SMXB, MXM3)
                    Return (MXM3)
                }
            }
            Else 
            {
                Return (Buffer(8) {0x0}) // Return zeros.
            }
        }

        Method (_DSM, 4, SERIALIZED)
        {
            //
            // Notebook Common Interfaces
            //
            If (LEqual (Arg0, ToUUID ("D4A50B75-65C7-46F7-BFB7-41514CEA0244")))
            {
                If (LNotEqual (Arg1, 0x102))
                {
                    Return (STATUS_ERROR_UNSUPPORTED)
                }
                Switch (Arg2)
                {
                    case (NBCI_FUNC_SUPPORT) // Function 0: NBCI_FUNC_SUPPORT
                    {
                        // Supported functions: 0, 14h, 16h
                        Return (0x00500001)
                    }
                    case (NBCI_FUNC_GETBACKLIGHT) // Function 14h: NBCI_FUNC_GETBACKLIGHT
                    {
                        Return (Package() {
                            ACPI_ID_LCD, // device 0x110
                            0x00000200,  // PWM + Driver API
                            0x00000000,  // i2c address (n/a)
                            0x00000000,  // controller type (n/a)
                            0x00000000,  // backlight type
                            0x00000001,  // one entry
                            // first entry
                            200,         // 200 Hz
                            0, 1000,     // 0 - 100.0%
                            0, 0, 0      // RGB correction
                        })
                    }
                    case (NBCI_FUNC_GETLICENSE) // Function 16h: NBCI_FUNC_GETLICENSE
                    {
                        Return (Package() {TEST_LICENSE_VECTOR})
                    }
                }
                Return (STATUS_ERROR_UNSUPPORTED) //MXM_ERROR_UNSUPPORTED - FunctionCode or SubfunctionCode not supported
            }
            // MXM_DSM_GUID     {4004A400-917D-4cf2-B89C-79B62FD55665}
            If (LEqual (Arg0, ToUUID ("4004A400-917D-4cf2-B89C-79B62FD55665")))
            { 
                If (LNot (LEqual(Arg1, 0x300)))
                {
                    Return (STATUS_ERROR_UNSUPPORTED)          // Unsupported error.
                }
                Switch (ToInteger (Arg2))
                {
                    //
                    // Function 0: MXM_FUNC_MXSS
                    //
                    case (MXM_FUNC_MXSS)
                    {
                        // Sub-Functions 0,16,24 are supported
                        Return (ToBuffer (0x01010001))
                    }
                    //
                    // Function 24: MXM_FUNC_MXMI
                    //
                    case (MXM_FUNC_MXMI)
                    {
                        Return (ToBuffer (0x30))
                    }
                    //
                    // Function 24: MXM_FUNC_MXMS
                    //
                    case (MXM_FUNC_MXMS)
                    {
                        If (LNotEqual (DGPU_SCOPE.SMXS, 0))
                        {
                            Name (MXM3, Buffer(DGPU_SCOPE.SMXS) {0x00})
                            Store (DGPU_SCOPE.SMXB, MXM3)
                            Return (MXM3)
                        }
                    }       
                }
                Return (STATUS_ERROR_UNSUPPORTED) //MXM_ERROR_UNSUPPORTED - FunctionCode or SubfunctionCode not supported
            }
            Return (STATUS_ERROR_UNSPECIFIED) //MXM_ERROR_UNSPECIFIED
        }
    } // end of Device (DGPU)
}
