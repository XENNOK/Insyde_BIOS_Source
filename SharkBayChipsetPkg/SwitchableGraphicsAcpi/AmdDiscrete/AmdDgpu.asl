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
    // Get bus number for this DGPU
    //     
    OperationRegion (PE19, PCI_Config, 0x19, 0x1)
    Field (PE19, ByteAcc, NoLock, Preserve) {
        PSBN, 8
    }
}

Scope (DGPU_SCOPE)
{
    Method (_INI) 
    {
        Store (Zero, IGDS) // Integated Graphics Device Disabled
    }

    Method (GBUS, 0x0)
    {
        Return (^^PSBN) // Return DGPU bus number.
    }

    Method (_DOD, 0) // standard VGA device methods
    {
        Return (Package() {
            ACPI_ID_LCD,  // LCD
            ACPI_ID_CRT,  // CRT
            ACPI_ID_HDMI, // DPA HDMI
            ACPI_ID_DP_B, // DPB
            ACPI_ID_TMDS,  // DPC HDMI
//            ACPI_ID_EDP,  // EDPD
        })
    }

    // display devices
    Device (LCD0)
    {
        Name (_ADR, ACPI_ID_LCD)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0) // Device Get State.
        {
            And (MSTE, 0x8, Local0)
            If (Local0)
            {
                Return (1)
            }
            Else
            {
                Return (0)
            }
        }
        Method (_BCL, 0)
        {
            // Note: According to the OS type to change the base brightness levels, Win8 = 10, Win7 = 7.
            // need modify \_SB.PCI0._INI for update the OS type.
            If (LEqual (OSYS, 2012)) 
            {
                // Note: Follow XPS to modify brightness level to 101 levels.
                Return (Package() {
                    100, 60, 0,
                     1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
                    61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
                    81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
                })
            }
            Else
            {
                Return (Package() {100, 60, 12, 24, 36, 48, 60, 72, 84, 100}) // taylor
            }
        }
        Method (_BCM, 1)
        {		  
          If (LAnd (LGreaterEqual (Arg0, 0), LLessEqual (Arg0, 100)))
          {
              Store (Arg0, BRTL)
              DGPU_SCOPE.AFN7 (Arg0)
              If (LEqual (Arg0, 100))
              {
                  Subtract (Arg0, 4, Arg0)
              }
              Divide (Arg0, 12, Local0, Local1)
              Decrement (Local1)
              Store (Local1, EC_SCOPE.BRTS)	
          }
        }
        Method (_BQC, 0) // Brightness Query Current level.
        {
            Return (BRTL)
        }
    } // end of Devive (LCD0)

    Device (CRT0)
    {
        Name (_ADR, ACPI_ID_CRT)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0)
        {
            And (MSTE, 0x2, Local0)
            If (Local0)
            {
                Return (1)
            }
            Else
            {
                Return (0)
            }
        }
    } // end of Devive (CRT0)

    Device (HDM0)
    {
        Name (_ADR, ACPI_ID_HDMI)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0)
        {
            And (MSTE, 0x1, Local0)
            If (Local0)
            {
                Return (1)
            }
            Else
            {
                Return (0)
            }
        }
    } // end of Devive (HDM0)

    Device (DSP0) // DisplayPort
    {
        Name (_ADR, ACPI_ID_DP_B)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0) 
        {
            If (Lequal (DPFL, 0x1)) // DP connected?
            {
                And (MSTE, 0x4, Local0)	
                If (Local0)
                {
                    Return (0x1)
                }
                Else
                {
                    Return (0x0)
                }
            } Else {
                Return (0x0)
            }
        }
    }

    Device (TMDS) // TMDS
    {
        Name (_ADR, ACPI_ID_TMDS)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0)
        {
            If (Lequal (DPFL, 0x02)) // TMDS connected?
            {
                And (MSTE, 0x4, Local0)	
                If (Local0)
                {
                    Return (0x1)
                }
                Else
                {
                    Return (0x0)
                }
            } Else {
                Return (0x0)
            }
        }
    }
/*
    Device (DSP1) // e-DisplayPort
    {
        Name (_ADR, ACPI_ID_EDP)
        Method (MXMX, 1)
        {
            Return (0x1) // No mutex needed, returns success
        }
        Method (_DGS, 0) 
        {
            If (Lequal (DPFL, 0x1)) // DP connected?
            {
                And (SWIT, 0x8, Local0)	
                If (Local0)
                {
                    Return (0x1)
                }
                Else
                {
                    Return (0x0)
                }
            } Else {
                Return (0x0)
            }
        }
    }
*/
}
