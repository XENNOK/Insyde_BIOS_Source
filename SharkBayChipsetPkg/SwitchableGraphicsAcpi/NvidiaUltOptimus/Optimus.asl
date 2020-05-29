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

Scope (DGPU_SCOPE)
{
    OperationRegion (PCNV, SystemMemory, DGPU_SCOPE.EBAS, 0x1000)  // MXM Space
    Field (PCNV, AnyAcc, NoLock, Preserve) { 
        Offset (0x488), // Bus1 Dev0 Fun0 Reg488
            ,  25,
        MLTF,   1,  // HDA Multifunction bit Register
    }

    Name (OMPR, 2)  // Optimus DGPU Power-Control Ready
    Name (DGOS, 0)  // DGPU Power Status 0:on 1:off
    Name (CTOI, 1)  // Current Toggle Index
    Name (NTOI, 1)  // Next Toggle Index
    Name (MADL, 0)  // Current attached display device
    Name (MSTE, 0)  // Current actived display device

    Method (VSTS, 0)
    {
        // a. check attached display device
        If (LNotEqual (IGPU_SCOPE.CPDL, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPDL, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPDL, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CPL2, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL2, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL2, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CPL3, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL3, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL3, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }         
        }
        If (LNotEqual (IGPU_SCOPE.CPL4, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL4, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL4, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }

        }
        If (LNotEqual (IGPU_SCOPE.CPL5, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL5, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL5, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CPL6, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL6, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL6, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CPL7, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL7, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL7, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CPL8, Zero))
        {
            If (LEqual (IGPU_SCOPE.CPL8, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GATD, 2, GATD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CPL8, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GATD, 1, GATD)  // Bit0 - LVDS
            }
        }
        
        // b. check actived dispaly device
        If (LNotEqual (IGPU_SCOPE.CADL, Zero))
        {
            If (LEqual (IGPU_SCOPE.CADL, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CADL, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL2, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL2, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL2, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL3, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL3, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL3, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL4, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL4, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL4, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL5, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL5, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL5, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL6, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL6, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL6, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL7, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL7, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL7, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }
        If (LNotEqual (IGPU_SCOPE.CAL8, Zero))
        {
            If (LEqual (IGPU_SCOPE.CAL8, And (DID1, 0x0000FFFF)))  // DID1 - CRT
            {
                Or (GACD, 2, GACD)  // Bit1 - CRT
            }
            Elseif (LEqual (IGPU_SCOPE.CAL8, And (DID2, 0x0000FFFF)))  // DID2 - LVDS
            {
                Or (GACD, 1, GACD)  // Bit0 - LVDS
            }
        }

        // Pass curent attached display device into flag
        Store (GATD, DGPU_SCOPE.MADL)
        // Pass curent actived display device into flag
        Store (GACD, DGPU_SCOPE.MSTE)

        Store (0, GATD)
        Store (0, GACD)
    }

    Method (GETD, 2)
    {
        // Check current attached/actived display device status
        VSTS ()                                      // From Internal garphic
        Or (DGPU_SCOPE.MADL, Arg0, DGPU_SCOPE.MADL)  // From External graphic
        Or (DGPU_SCOPE.MSTE, Arg1, DGPU_SCOPE.MSTE)  
    
        // MDTL  Index
        // LCD        = 1
        // CRT        = 2
        // HDMI       = 3
        // LCD + CRT  = 4
        // LCD + HDMI = 5
        // CRT + HDMI = 6
        // BitMap
        // LCD        = 1
        // CRT        = 2
        // HDMI       = 4
         Switch (ToInteger (DGPU_SCOPE.MADL))
         {
            case (0x07) // LCD+CRT+HDMI attached
            {
                Switch (ToInteger (DGPU_SCOPE.MSTE))
                {
                    case (0x01) //LCD
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x02, DGPU_SCOPE.NTOI)  // CRT
                    }
                    case (0x02) //CRT
                    {
                        Store (0x02, DGPU_SCOPE.CTOI) 
                        Store (0x03, DGPU_SCOPE.NTOI)  // HDMI
                    }
                    case (0x04) //HDMI
                    {
                        Store (0x03, DGPU_SCOPE.CTOI) 
                        Store (0x04, DGPU_SCOPE.NTOI)  // LCD + CRT
                    }              
                    case (0x03) //LCD+CRT
                    {
                        Store (0x04, DGPU_SCOPE.CTOI) 
                        Store (0x05, DGPU_SCOPE.NTOI)  // LCD + HDMI
                    }
                    case (0x05) // LCD+HDMI
                    {
                        Store (0x05, DGPU_SCOPE.CTOI) 
                        Store (0x06, DGPU_SCOPE.NTOI)  // CRT + HDMI
                    }
                    case (0x06) // CRT+HDMI
                    {
                        Store (0x06, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                    default 
                    {
                        Store (0x01, DGPU_SCOPE.CTOI)  // LCD
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                 }
            }
            case (0x06) // CRT+HDMI attached
            {
                Switch (ToInteger (DGPU_SCOPE.MSTE))
                {
                    case (0x02) //CRT
                    {
                        Store (0x02, DGPU_SCOPE.CTOI) 
                        Store (0x03, DGPU_SCOPE.NTOI)  // HDMI
                    }
                    case (0x04) //HDMI
                    {
                        Store (0x03, DGPU_SCOPE.CTOI) 
                        Store (0x06, DGPU_SCOPE.NTOI)  // CRT+HDMI
                    }
                    case (0x06) // CRT+HDMI
                    {
                        Store (0x06, DGPU_SCOPE.CTOI) 
                        Store (0x02, DGPU_SCOPE.NTOI)  // CRT 
                    }
                    default 
                    {
                        Store (0x02, DGPU_SCOPE.CTOI) 
                        Store (0x02, DGPU_SCOPE.NTOI)  // CRT
                    }
                }
            }
            case (0x05) // LCD+HDMI attached
            {
                Switch (ToInteger (DGPU_SCOPE.MSTE))
                {
                    case (0x01) //LCD
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x03, DGPU_SCOPE.NTOI)  // HDMI
                    }
                    case (0x04) //HDMI
                    {
                        Store (0x03, DGPU_SCOPE.CTOI) 
                        Store (0x05, DGPU_SCOPE.NTOI)  // LCD+HDMI
                    }
                    case (0x05) // LCD+HDMI
                    {
                        Store (0x05, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                    default 
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                }
            }
            case (0x03) // LCD+CRT attached
            {
                Switch (ToInteger (DGPU_SCOPE.MSTE))
                {
                    case (0x01) //LCD
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x02, DGPU_SCOPE.NTOI)  // CRT
                    }
                    case (0x02) //CRT
                    {
                        Store (0x02, DGPU_SCOPE.CTOI) 
                        Store (0x04, DGPU_SCOPE.NTOI)  // LCD+CRT
                    }
                    case (0x03) //LCD+CRT
                    {
                        Store (0x04, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                    default 
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)  // LCD
                    }
                }
            }
            default 
            {
                Switch (ToInteger (DGPU_SCOPE.MSTE))
                {
                    case (0x01) //LCD
                    {
                        Store (0x01, DGPU_SCOPE.CTOI) 
                        Store (0x01, DGPU_SCOPE.NTOI)
                    }
                    case (0x02) //CRT
                    {
                        Store (0x02, DGPU_SCOPE.CTOI) 
                        Store (0x02, DGPU_SCOPE.NTOI)
                    }
                    case (0x04) //HDMI
                    {
                        Store (0x03, DGPU_SCOPE.CTOI) 
                        Store (0x03, DGPU_SCOPE.NTOI)
                    }
                }
            }
         }
    }
    
    Method (NVOP, 4, NotSerialized)  // Called from Method (_DSM)
    {
        // Only Interface Revision 0x0100 is supported
        If (LNotEqual (Arg1, 0x100))
        {
            Return (STATUS_ERROR_UNSUPPORTED)
        }

        // (Arg2) Sub-Function
        Switch (ToInteger (Arg2))
        {
            //
            // Function 0: NVOP_FUNC_SUPPORT - Bit list of supported functions.
            //
            case (NVOP_FUNC_SUPPORT)
            {
                If (LNotEqual (DGPU_SCOPE.DHPS, Zero))
                {
                    // bit 0, 5, 6, 16, 26, 27 support
                    Store (Buffer(4) {0x61, 0x00, 0x01, 0x0C}, Local0)
                }
                Else
                {
                    // bit 0, 5, 6, 16, 26 support
                    Store (Buffer(4) {0x61, 0x00, 0x01, 0x04}, Local0)
                }
                Return (Local0)
            }

            //
            // Function 5: NVOP_FUNC_DISPLAYSTATUS - Query the Display Hot-Key.
            //
            case (NVOP_FUNC_DISPLAYSTATUS)
            {
                 Name (TMP5, Buffer() {0x00, 0x00, 0x00, 0x00})
                 CreateField (TMP5,  0,  4, DAVF)  // Display ACPI event
                 CreateField (TMP5,  4,  1, LIDF)  // LID event state
                 CreateField (TMP5,  8,  6, TOGN)  // Toggle List Sequence Number
                 CreateField (Arg3, 31,  1, NCSM)  // Next Combination Sequence Mask
                 CreateField (Arg3, 25,  5, NCSN)  // Next Combination Sequence Number
                 CreateField (Arg3, 24,  1, DIMK)  // Display Mask
                 CreateField (Arg3, 12, 12, ACTD)  // Active Displays
                 CreateField (Arg3,  0, 12, ATTD)  // Attached Displays

                 If (ToInteger (NCSM))
                 {
                     // Next Combination Sequence Mask Set
                     Store (ToInteger (NCSN), TOGN)
                 }
                 Elseif (ToInteger (DIMK))
                 {
                     // Display Mask Set
                     GETD (ToInteger (ATTD), ToInteger (ACTD))

                     Store (DGPU_SCOPE.NTOI, TOGN)
                     Store (1, DAVF)
                 }
      
                 Return (TMP5)
            }
            
            //
            // Function 6: NVOP_FUNC_MDTL - Query Display Toggle List.
            //
            case (NVOP_FUNC_MDTL)
            {
                // Display Toggle List
                Name (TMP6, Package()
                {
                    ones, 0x2C,        // LVDS
                    ones, 0x2C,        // CRT
                    ones, 0x2C,        // HDMI
                    ones, ones, 0x2C,  // LVDS + CRT
                    ones, ones, 0x2C,  // LVDS + HDMI
                    ones, ones, 0x2C   // CRT  + HDMI
                })

                // Update Display Toggle List
                Store (DID2, Index (TMP6, 0))  // LVDS
                Store (DID1, Index (TMP6, 2))  // CRT 
                Store (DID4, Index (TMP6, 4))  // HDMI 

                Store (DID2, Index (TMP6, 6))  // LVDS + CRT
                Store (DID1, Index (TMP6, 7))

                Store (DID2, Index (TMP6, 9))  // LVDS + HDMI
                Store (DID4, Index (TMP6, 10))

                Store (DID1, Index (TMP6, 12)) // CRT + HDMI
                Store (DID4, Index (TMP6, 13))

                Return (TMP6)
            }

            //
            // Function 16: NVOP_FUNC_GETOBJBYTYPE - Get Data Object.
            //
            case (NVOP_FUNC_GETOBJBYTYPE)
            {
                Return (DGPU_SCOPE.GOBT (Arg3))
            }

            //
            // Function 26: NVOP_FUNC_OPTIMUSCAPS - Optimus Capabilities.
            //
            case (NVOP_FUNC_OPTIMUSCAPS)
            {
                CreateField (Arg3, 24, 2, OPCE)
                CreateField (Arg3,  0, 1, FLCH)
                
                If (ToInteger (FLCH))
                {
                    Store (ToInteger(OPCE), OMPR)  // Optimus Power Control Enable - From DD
                }

                // Definition of return buffer.
                // bit 0   - Optimus Enabled
                //           0 : Optimus Graphics Disabled
                //           1 : Optimus Graphics Enabled (default)
                // bit 4:3 - Current GPU Control Status
                //           0 : GPU is powered off
                //           3 : GPU power has stabilized (default)
                // bit 6   - Shared discrete GPU Hot-Plug Capabilities
                //           1 : There are discrete GPU Display Hot-Plug signals co-connected to the platform
                // bit 26:24 - Optimus Capabilities
                //             0 : No special platform capabilities
                //             1 : Platform has dynamic GPU power control
                // bit 27:28 - Optimus HD Audio Codec Capabilities
                //             0 : No audio codec-related capabilities
                //             1 : Platform does not use HD audio
                //             2 : Platform supports Optimus dynamic codec control
                Name (RBUF, Buffer() {0x00, 0x00, 0x00, 0x00})
                CreateField (RBUF,  0, 1, OPEN)
                CreateField (RBUF,  3, 2, CGCS)
                CreateField (RBUF,  6, 1, SHPC)
                CreateField (RBUF, 24, 3, DGPC)  // DGPC(Optimus Capabilities) - Default: No Dynamic GPU Power Control
                CreateField (RBUF, 27, 2, HDAC)  // HDAC - HD Audio Codec Cap

                Store (One, OPEN)  // Optimus Enabled

                If (LNotEqual (DGPU_SCOPE.DHPS, Zero))
                {
                    Store (One, SHPC)         // GPU Display Hotplug Supported
                    Store (0x2, HDAC)         // HDA BIOS control Supported
                }

                If (LNotEqual (DGPU_SCOPE.DPCS, Zero))
                {
                    Store (One, DGPC)         // Dynamic GPU Power Control Available
                    If (DGPU_SCOPE.GSTA)
                    {
                        Store (0x3, CGCS)     // Current GPU Control status
                    }
                    ELSE
                    {
                        Store (0x0, CGCS)     // Current GPU Control status
                    }
                }
                Else
                {
                    Store (0x3, CGCS)         // GPU is enable all the time when no power-off available
                }
                Return (RBUF)
            }

            //
            // Function 27: NVOP_FUNC_OPTIMUSFLAGS - Optimus State flags.
            //
            case (NVOP_FUNC_OPTIMUSFLAGS)
            {
                    Store (Arg3, Local0)
                    CreateField (Local0, 0, 1, OPFL)
                    CreateField (Local0, 1, 1, OPVL)
                    If (ToInteger (OPVL))
                    {
                        Store (Zero, OPTF)
                        If (ToInteger (OPFL))
                        {
                            Store(One, OPTF)
                        }
                    }
                    Store (OPTF, Local0)
                    Return (Local0)
            }

            default 
            {
                //
                // FunctionCode or SubFunctionCode not supported
                //
                Return (STATUS_ERROR_UNSUPPORTED)
            }
        }
    }

    Method (GOBT, 1)
    {
        Name (OPVK, Buffer()
        {
        // Key below is *NOT A REAL KEY*, it is for reference
        // Customer need to ask NVIDIA PM to get the key
        // Customer need to put the key in between labels "// key start -" and
        //  "// key end -". Please consult NVIDIA PM if any issues

        // Key start -
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00
        // Key end -
        })

        CreateWordField (Arg0, 2, USRG)  // Object type signature passed in by driver.

        If (LEqual (USRG, 0x564B)) {     // 'VK' for Optimus Validation Key Object.
           Return (OPVK)
        }
        Return (Zero)    
    } // End GOBT()

    Method (GSTA, 0, Serialized)
    {
        If (LEqual (SGPI (PWOK), 1)) 
        {
            Return (0x01)
        }
        Else 
        {
            Return (0x00)
        }
    }
        

    Method (_PS0, 0x0)
    {
        If (DGOS) {
            If (LNotEqual (DGPU_SCOPE.DPCS, Zero))
            {
                DGPU_SCOPE._ON ()       // Here turn on the DGPU, OEM can switch their own ON method here.
                Store (Zero, DGOS)               // Indicate DGPU power is ON
                If (LNotEqual (DGPU_SCOPE.DHPS, Zero))
                {
                    Store (Zero, MLTF)               // Disable HDA multifunction device
                }
            }
        }
    }
    Method (_PS3, 0x0)
    {
        If (LEqual (DGPU_SCOPE.OMPR, 0x3)) {
            If (LNotEqual (DGPU_SCOPE.DPCS, Zero))
            {
                DGPU_SCOPE._OFF()      // Here turn off the DGPU, OEM can switch their own OFF method here.
                Store (One, DGOS)                // Indicate DGPU power is OFF
                Store (0x2, DGPU_SCOPE.OMPR) // Reset NV GPU power down flag
            }
        }
    }

    //  method  : _ROM
    //  Arguments:
    //          Arg0:   Integer     Offset of the graphics device ROM data
    //          Arg1:   Integer     Size of the buffer to fill in (up to 4K)
    //
    //  Return Value:
    //          Returns Buffer      Buffer of requested video ROM bytes
    Method (_ROM, 2)
    {
        Store (Arg0, Local0)
        Store (Arg1, Local1)

        Name (VROM, Buffer(Local1) {0x00}) // Create 4K buffer to return to DD

        If (LGreater (Local1, 0x1000))
        {
            Store (0x1000, Local1) // Return dummy buffer if asking for more than 4K
        }

        If (LGreater (Arg0, RVBS))
        {
            Return (VROM) // Return dummy buffer if asking beyond VBIOS image
        }

        Add (Arg0, Arg1, Local2)

        If (LGreater (Local2, RVBS)) // If requested BASE+LEN > VBIOS image size
        {
            Subtract (RVBS, Local0, Local1) // Limit length to the final chunk of VBIOS image
        }

        If (LLess (Local0, 0x8000))
        {
            Mid (VBS1, Local0, Local1, VROM)
        }
        Else
        {
            Subtract (Local0, 0x8000, Local0)
            If (LLess (Local0, 0x8000))
            {
                Mid (VBS2, Local0, Local1, VROM)
            }
            Else
            {
                Subtract (Local0, 0x8000, Local0)
                If (LLess (Local0, 0x8000))
                {
                    Mid (VBS3, Local0, Local1, VROM)
                }
                Else
                {
                    Subtract (Local0, 0x8000, Local0)
                    If (LLess (Local0, 0x8000))
                    {
                        Mid (VBS4, Local0, Local1, VROM)
                    }
                }
            }
        }

        Return (VROM)
    }


    Method (MXMX, 1, Serialized) 
    {
        If (LEqual (Arg0, 0)) 
        {
            // Acquire DDC/AUX mux
            // No mutex implemented. No need to acquire mutex. 
            // Set mux to dGPU
            DGPU_SCOPE.SGPO (ESEL, One)

            P8XH (1, 0x99)
            P8XH (0, 0x00)

            Return (0x1)
        }
        If (LEqual (Arg0, 1)) 
        {
            // Release DDC/AUX mux
            // No mutex implemented. No need to release mutex.
            // 2-way mux. Hence no need to do anything
            P8XH (1, 0x99)
            P8XH (0, 0x01)

            Return (0x1)
        }
        If (LEqual (Arg0, 2))
        {
            P8XH (1, 0x99)
            P8XH (0, 0x02)

            // Get ddc/aux mux status for dGPU
            Return (DGPU_SCOPE.SGPI (ESEL))
        }

        Return (0x0)
    }

    Method (MXDS, 1, Serialized)
    {
        If (LEqual (Arg0, Zero))
        {
            Return (DGPU_SCOPE.SGPI (DSEL))
        }
        If (LEqual (Arg0, One))
        {
            DGPU_SCOPE.SGPO (DSEL, One)
        }
        Return (Zero)
    }

    Method (_DSM, 4, SERIALIZED)
    {
        //
        // Check for Nvidia Optimus GC6 _DSM  UUID
        //
        // GC6_DSM_GUID {CBECA351-067B-4924-9CBD-B46B00B86F34}
        If (LEqual (Arg0, ToUUID ("CBECA351-067B-4924-9CBD-B46B00B86F34")))
        {
            If (LNotEqual (DGPU_SCOPE.GC6S, Zero))
            {
                Return (DGPU_SCOPE.OGC6 (Arg0, Arg1, Arg2, Arg3))
            }
        }

        //
        // Check for Nvidia SPB _DSM  UUID
        //
        // SPB_DSM_GUID {95DB88FD-940A-4253-A446-70CE0504AEDF}
        If (LEqual (Arg0, ToUUID ("95DB88FD-940A-4253-A446-70CE0504AEDF")))
        {
            If (LNotEqual (DGPU_SCOPE.VENS, Zero))
            {
                Return (IGPU_SCOPE.SPB (Arg0, Arg1, Arg2, Arg3))
            }
        }

        //
        // Check for Nvidia GPS _DSM  UUID
        //
        // GPS_DSM_GUID {A3132D01-8CDA-49BA-A52E-BC9D46DF6B81}
        If (LEqual (Arg0, ToUUID ("A3132D01-8CDA-49BA-A52E-BC9D46DF6B81")))
        {
            If (LNotEqual (DGPU_SCOPE.GPSS, Zero))
            {
                Return (DGPU_SCOPE.GPS (Arg0, Arg1, Arg2, Arg3))
            }
        }

        //
        // Check for Nvidia Optimus _DSM  UUID
        //
        // NVOP_DSM_GUID {A486D8F8-0BDA-471B-A72B-6042A6B5BEE0}
        If (LEqual (Arg0, ToUUID ("A486D8F8-0BDA-471B-A72B-6042A6B5BEE0")))
        {
            Return (DGPU_SCOPE.NVOP (Arg0, Arg1, Arg2, Arg3))
        }

        If (LEqual (Arg0, ToUUID ("4004A400-917D-4cf2-B89C-79B62FD55665")))
        { 
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
                // Function 16: MXM_FUNC_MXMS
                //
                case (MXM_FUNC_MXMS)
                {
                    If(LEqual (Arg1, 0x300))
                    {
                        If (LNotEqual (MXBS, 0))
                        {
                            Name (MXM3, Buffer(MXBS) {0x00})
                            Store (MXMB, MXM3)
                            Return (MXM3)
                        }
                    }
                }       
            }
            Return (STATUS_ERROR_UNSUPPORTED)  // MXM_ERROR_UNSUPPORTED - FunctionCode or SubfunctionCode not supported
        }
        Return (STATUS_ERROR_UNSPECIFIED)  // MXM_ERROR_UNSPECIFIED
    }
}
