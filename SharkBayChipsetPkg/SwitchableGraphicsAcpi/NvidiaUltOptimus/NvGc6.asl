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
    Name (TGPC, Buffer(0x04) {0x00})

    Method (GC6I, 0, Serialized)
    {
        Store(One, LNKD) // PCIE link disabling.
        CreateField (TGPC, 0x06,0x2, ECOC) // NOC: Notify on complete: Reserve
        CreateField (TGPC, 0x0A,0x2, PRGE) // PRGE : PCIe Root Power GC6 Enter Sequence
        If (Lor (LEqual (ToInteger (PRGE), 0x3), LEqual (ToInteger (PRGE), 0x1)))
        { 
            // DBGS : Link Disable before GC6 Entry starts (E0) 
            // DBGC : Link Disable before GC6 Entry complete (E1)
            Store(One, LNKD) // PCIE link disabling.
        }
        Store (1 , EC_SCOPE.EC6I) // notify EC to prepare GC6 entry.
        Store (0 , EC_SCOPE.EC6O) // Clean GC6 exit bit in EC ram
    }

    Method (GC6O, 0, Serialized)
    {
        CreateField (TGPC, 0x06, 0x2, ECOC) // NOC: Notify on complete: Reserve
        CreateField (TGPC, 0x08, 0x2, PRGX) // PRGX : PCIe Root Power GC6 Exit Sequence
        If (LEqual (ToInteger (PRGX), Zero)) // EBPG : Link Enable before GPU Power-On & GC6 Exit begins (X0)
        {
            Store(Zero, LNKD) // PCIE link enabling
        }
        SGPO (HLRS, 1)
        SGPO (PWEN, 1)
        Store (Zero, Local0) // Delay by Stall(0x32) *30 times.= 1.5ms
        While (LLess (Local0, 0x78))
        {
            Add (Local0, One, Local0)
            Stall (0x32)
        }
        SGPO (HLRS, 0)
        Sleep (0x16) // Delay time
        IF (LEqual (ToInteger (PRGX), 0x3)) // EAPG : Link Enable after GPU Power-On Reset, but before GC6 Exit begins
        {
            Store(Zero, LNKD) // PCIE link enabling
        }
        While (LLESS (LNKS, 7)) // Wait for dGPU PWR OK                        
        {
            Sleep (One)
        } 
        Store (1 , EC_SCOPE.EC6O) // notify EC to prepare GC6 exit.
        Store (0 , EC_SCOPE.EC6I) // Clean GC6 entry bit in EC ram
    }

    Method (GETS, 0, SERIALIZED)
    {
        //
        // Get the current GPU GCx Sleep Status
        //
        If (LEqual (SGPI (PWOK), 1))
        {
            Return (1) // GC0/D0
        }
        Else
        {
            If (LEqual (EC_SCOPE.FBST, 0)) // FB_CLAMP deasserts
            {
                Return (2) // dGPU & FB Powered Off
            }
            Else
            {
                Return (3) // GC6 (dGPU Off, FB On, w/FB_CLAMP asserted)
            }
        }
    }

    Method (OGC6, 4, SERIALIZED)
    {
        If (LLess (ToInteger (Arg1), JT_REVISION_ID))
        {
            Return (STATUS_ERROR_UNSPECIFIED)
        }
        
        Switch (ToInteger (Arg2))
        {
            case (JT_FUNC_SUPPORT)
            {
                //
                // Sub-Functions 0-6 are supported
                //
                Return (Buffer(4) {0x7F, 0x00, 0x00, 0x00})
            }
            case (JT_FUNC_CAPS)
            {
                Name (JTB1, Buffer(4) {0x00})
                CreateField (JTB1, 0,  1,  JTEN) // JT Enable
                CreateField (JTB1, 1,  2,  SREN) // NVSR Enable
                CreateField (JTB1, 3,  3,  PLPR) // Panel Power Rail
                CreateField (JTB1, 6,  2,  FBPR) // FB Power Rail
                CreateField (JTB1, 8,  2,  GUPR) // GPU Power Rail
                CreateField (JTB1, 10, 1,  GC6R) // GC6 ROM
                CreateField (JTB1, 11, 1,  PTRH) // Panic Trap Handler
                CreateField (JTB1, 20, 12, JTRV) // JT Revision

                Store (1, JTEN) // JT Enable
                Store (1, GC6R) // GC6 integrated ROM
                Store (1, PTRH) // No SMI Handler
                Store (1, SREN) // Disable NVSR
                Store (0x103, JTRV) // JT Rev

                Return (JTB1)
            }
            case (JT_FUNC_POLICYSELECT)
            {
                Return (STATUS_ERROR_UNSUPPORTED)
            }
            case (JT_FUNC_POWERCONTROL)
            {
                CreateField (Arg3, 0, 3, GUPC) // GPU Power Control
                CreateField (Arg3, 4, 1, PLPC) // Panel Power Control
                CreateField (Arg3, 7, 1, ECOC) // Notify on complete

                Name (JTB3, Buffer(4) {0x00})
                CreateField (JTB3, 0, 3, GUPS) // dGPU Power Status
                CreateField (JTB3, 3, 1, GPGS) // dGPU Power status 
                CreateField (JTB3, 7, 1, PLST) // Panel State

                If (LEqual (ToInteger (GUPC), 1))
                {
                    Store (Arg3, TGPC) // Store GC6 control input for GC6I GC6O
                    GC6I ()
                    //
                    // Power Off the Panel here
                    //
                    Store (1, PLST) // Panel is powered off
                }
                ElseIf (LEqual (ToInteger (GUPC), 2))
                {
                    Store (Arg3, TGPC) // Store GC6 control input for GC6I GC6O
                    GC6I ()
                    If (LEqual (ToInteger (PLPC), 0)) // Panel remains powered while GPU is in GC6
                    {
                        //
                        // Power On the Panel here
                        //
                        Store (0, PLST) // Panel is powered on
                    }
                }
                ElseIf (LEqual (ToInteger (GUPC), 3))
                {
                    Store (Arg3, TGPC) // Store GC6 control input for GC6I GC6O
                    GC6O ()
                    //
                    // The panel always powered on in Optimus platform
                    //
                    If (LNotEqual (ToInteger (PLPC), 0)) // Panel remains powered while GPU is in GC6
                    {
                        //
                        // Power On the Panel here
                        //
                        Store (0, PLST) // Panel is powered on
                    }
                }
                ElseIf (LEqual (ToInteger (GUPC), 4))
                {
                    Store (Arg3, TGPC) // Store GC6 control input for GC6I GC6O
                    GC6O ()
                    //
                    // The panel always powered on in Optimus platform
                    //
                    If (LNotEqual (ToInteger (PLPC), 0)) // Panel remains powered while GPU is in GC6
                    {
                        Store (0, PLST) // Panel is powered on
                    }
                }
                ElseIf (LEqual (ToInteger (GUPC), 0))
                {
                       Store (GETS (), GUPS)
                       If (LEqual (ToInteger (GUPS), 0x01)) 
                       {
                           Store (One, GPGS) // dGPU power status is Power OK
                       }
                       Else
                       {
                           Store (Zero, GPGS) // dGPU power status is Power off
                       } 

                }

                Return (JTB3)
            }
            case (JT_FUNC_PLATPOLICY)
            {
                Return (STATUS_ERROR_UNSUPPORTED)
            }
            case (JT_FUNC_DISPLAYSTATUS)
            {
                CreateField (Arg3, 0,  12, ATCD) // Attached Display
                CreateField (Arg3, 12, 12, ACTD) // Active Display
                CreateField (Arg3, 24, 1,  DMSK) // Display Mask
                CreateField (Arg3, 25, 5,  NCSN) // Next Combination Sequence Number
                CreateField (Arg3, 30, 1,  NCRT) // Next Combination Requires GPU Transition
                CreateField (Arg3, 31, 1,  NCSM) // Next Combination Sequence Mask

                Name (JTB5, Buffer(4) {0x00})
                CreateField (JTB5, 0,  4,  DPAE) // Display ACPI Event
                CreateField (JTB5, 4,  1,  LIDE) // LID Event State
                CreateField (JTB5, 5,  1,  DOCS) // Dock State
                CreateField (JTB5, 8,  6,  TLSN) // Toggle List Sequence Number
                CreateField (JTB5, 20, 1,  DHPS) // Display Hot-Plug Status
                CreateField (JTB5, 21, 28, DHPE) // Display Hot-Plug Event

                If (LNotEqual (ToInteger (NCSM), 0))
                {
                    // Next Combination Sequence Mask Set
                    Store (ToInteger (NCSN), TLSN)
                }
                Elseif (ToInteger (DMSK))
                {
                    // Display Mask Set
                    GETD (ToInteger (ATCD), ToInteger (ACTD))

                    Store (DGPU_SCOPE.NTOI, TLSN)
                    Store (1, DPAE)
                }

                Return (JTB5)
            }
            case (JT_FUNC_MDTL)
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
        }
        Return (STATUS_ERROR_UNSUPPORTED)
    }
}

//
// Here's a sample for GC6 EC related Q-event code for enter/exit GC6, please add into Q-event related file.
//
//#include <NvidiaOptimus.asi>
//External (DGPU_ROOT_PORT_SCOPE.LNKD, IntObj)
//External (DGPU_SCOPE.TGPC, IntObj)
//External (DGPU_SCOPE.SGPO, MethodObj)
//External (DGPU_SCOPE.HLRS, IntObj)
//External (DGPU_SCOPE.PWEN, IntObj)
//
//Method (_Q60) // for GC6 entry Q-event
//{
//    CreateField (DGPU_SCOPE.TGPC, 0x0A, 0x2, PRGE) // PRGE : PCIe Root Power GC6 Enter Sequence
//    IF (LEqual (ToInteger (PRGE), 0x0)) // DAGC : Link Disable after GC6 Entry complete & before GPU Power Down
//    {
//        Store (One, DGPU_ROOT_PORT_SCOPE.LNKD) // PCIE link disabling.
//    }
//    DGPU_SCOPE.SGPO (DGPU_SCOPE.HLRS, 1) // Assert dGPU_HOLD_RST#
//    DGPU_SCOPE.SGPO (DGPU_SCOPE.PWEN, 1) // Assert dGPU_PWR_EN#
//    IF (LEqual (ToInteger (PRGE), 0x2)) // DAGP : Link Disable after GC6 Entry & GPU Power down is complete
//    {
//        Store (One, DGPU_ROOT_PORT_SCOPE.LNKD) // PCIE link disabling.
//    }
//    Store (0x00, EC_SCOPE.EC6I)
//    Store (0x00, EC_SCOPE.EC6O)
//}
//
//Method (_Q61) // for GC6 exit Q-event
//{
//    Store (0x00, EC_SCOPE.EC6O)
//    Store (0x00, EC_SCOPE.EC6I)
//}