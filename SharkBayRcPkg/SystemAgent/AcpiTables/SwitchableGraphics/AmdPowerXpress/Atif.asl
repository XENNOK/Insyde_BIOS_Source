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

Scope (IGPU_SCOPE)
{
    Name (DGPU_SCOPE.TLE1, TOGGLE_LIST_ENTRIES_01)
    Name (DGPU_SCOPE.TLE2, TOGGLE_LIST_ENTRIES_02)
    Name (DGPU_SCOPE.TLE3, TOGGLE_LIST_ENTRIES_03)
    Name (DGPU_SCOPE.TLE4, TOGGLE_LIST_ENTRIES_04)
    Name (DGPU_SCOPE.TLE5, TOGGLE_LIST_ENTRIES_05)
    Name (DGPU_SCOPE.TLE6, TOGGLE_LIST_ENTRIES_06)
    Name (DGPU_SCOPE.TLE7, TOGGLE_LIST_ENTRIES_07)
    Name (DGPU_SCOPE.TLE8, TOGGLE_LIST_ENTRIES_08)
    Name (DGPU_SCOPE.TLE9, TOGGLE_LIST_ENTRIES_09)
    Name (DGPU_SCOPE.TL10, TOGGLE_LIST_ENTRIES_10)
    Name (DGPU_SCOPE.TL11, TOGGLE_LIST_ENTRIES_11)
    Name (DGPU_SCOPE.TL12, TOGGLE_LIST_ENTRIES_12)
    Name (DGPU_SCOPE.TL13, TOGGLE_LIST_ENTRIES_13)
    Name (DGPU_SCOPE.TL14, TOGGLE_LIST_ENTRIES_14)
    Name (DGPU_SCOPE.TL15, TOGGLE_LIST_ENTRIES_15)

//
// ACPI Control Methods and Notifications
// Rev. 0.30
//
// ATI GFX Interface: ATIF
//
//   This is the major entry point used by the driver for ATI specific functionality
//   in the System BIOS. This method provides multiple functions that can be invoked
//   by the display driver. The only required function is "Verify Interface". All other
//   functions are optional. The general format of this method is as follows.
//
// Arguments:
//
//   Arg0 (ACPI_INTEGER): Function code
//   Arg1 (ACPI_BUFFER): Parameter buffer, 256 bytes
//
// Output:
//
//   (ACPI_BUFFER): 256 bytes.
//
    Method (ATIF, 2, Serialized)
    {
//
// Function 0 (Verify Interface)
//
//   This function provides interface version and bit vectors indicating functions and
//   notifications supported by System BIOS. It is a required function, if any other 
//   ATIF functions or any ATIF notifications are supported by System BIOS.
//
// Arguments:
//
//   Arg0: Function code
//   Arg1: None
//
// Output:
//
//   Structure data containing the following info:
//
// ===================================================================================================
// ||       Field       || Format ||                        Description                             ||
// ===================================================================================================
// || Structure Size    || WORD   || Structure size in bytes including the Structure Size field.    ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Interface         || WORD   || 16-bit interface version. Must be set to 1.                    ||
// || Version           ||        ||                                                                ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Supported         || DWORD  || Bit 0: Display switch request is supported                     ||
// || Notifications     ||        || Bit 1: Expansion mode change request is supported              ||
// || Mask              ||        || Bit 2: Thermal state change request is supported               ||
// ||                   ||        || Bit 3: Forced power state change request is supported          ||
// ||                   ||        || Bit 4: System power source change request is supported         ||
// ||                   ||        || Bit 5: Display configuration change request is supported       ||
// ||                   ||        || Bit 6: PowerXpress graphics switch toggle request is supported ||
// ||                   ||        || Bit 7: Panel brightness change request is supported            ||
// ||                   ||        || Bit 8: Discrete GPU display connect/disconnect event is        ||
// ||                   ||        ||        supported.                                              ||
// ||                   ||        || Bits 31-9: Reserved (must be zero)                             ||
// ||                   ||        || If supported notifications mask indicates that a notification  ||
// ||                   ||        || is not supported, it will be ignored when function             ||
// ||                   ||        || "Get System BIOS Requests" is executed.                        ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Supported         || DWORD  || Bit vector providing supported functions information. Each bit ||
// || Functions Bit     ||        || marks support for one specific function of the ATIF method.    ||
// || Vector            ||        || Bit n, if set, indicates that Function n+1 is supported.       ||
// ===================================================================================================
//
        If (LEqual (Arg0, ATIF_VERIFY_INTERFACE))
        {
            Name (TMP0, Buffer(256) {0x00})
            CreateWordField  (TMP0, 0, F0SS)
            CreateWordField  (TMP0, 2, F0IV)
            CreateDwordField (TMP0, 4, F0SN)
            CreateDwordField (TMP0, 8, F0SF)

            Store (0x000C, F0SS) // Size of return structure
            Store (0x0001, F0IV) // Interface version
            Store (0x00000141, F0SN) // Supported Notifications Mask
            Store (0x00000007, F0SF) // Supported Functions Bit Vector, support function 1, 2, 3

            If (LEqual (And (DGPU_SCOPE.SGMD, 0x0F), 0x02))
            {
                Store (Zero, F0SN)
                Store (Zero, F0SF)
            }

            Return (TMP0)
        }
//
// Function 1 (Get System Parameters)
//
//   This function retrieves various System parameters.
//   It is a required function only, if any ATIF notifications are supported by System BIOS.
//
// Arguments:
//
//   Arg0: Function code
//   Arg1: None
//
// Output:
//
//   Structure data containing the following info:
//
// ===================================================================================================
// ||       Field       || Format ||                        Description                             ||
// ===================================================================================================
// || Structure Size    || WORD   || Structure size in bytes including the Structure Size field.    ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Valid Flags Mask  || DWORD  || Indicates which of the Flags field bits are valid.             ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Flags             || DWORD  || Bits 1-0:                                                      ||
// ||                   ||        || 00 - Indicates that Notify (VGA, 0x81) is not used as a        ||
// ||                   ||        ||      general purpose notification.                             ||
// ||                   ||        || 01 - Indicates that Notify (VGA, 0x81) is used as a            ||
// ||                   ||        ||      general purpose notification.                             ||
// ||                   ||        || 10 - Indicates that Notify (VGA, n) is used as a general       ||
// ||                   ||        ||      purpose notification, where n is defined by Notify        ||
// ||                   ||        ||      Command Code field.                                       ||
// ||                   ||        || Bit 2: When set to 1 indicates that lid state changes are not  ||
// ||                   ||        ||        reported by System BIOS through int10 VBIOS interface.  ||
// ||                   ||        || Bits 31-3: Reserved (must be zero).                            ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Notify Command    || BYTE   || Range: 0xd0 - oxd9.                                           ||
// || Code              ||        || Indicates that Notify (VGA, n) is used as a general purpose    ||
// ||                   ||        || notification, where n is one of: 0xd0 - 0xd9. This field is   ||
// ||                   ||        || valid only when the Flags field bits 1-0 is are set to 10.     ||
// ===================================================================================================
//
        If (LEqual (Arg0, ATIF_GET_SYSTEM_PARAMETERS))
        {
            Name (TMP1, Buffer(256) {0x00})
            CreateWordField  (TMP1, 0, F1SS)
            CreateDwordField (TMP1, 2, F1VF) 
            CreateDwordField (TMP1, 6, F1FG)

            Store (0x000A, F1SS) // Size of return structure
            Store (0x00000003, F1VF) // Valid Fields Mask
            Store (0x00000001, F1FG) // Notify (VGA, 0x81) is used as a general purpose notification

            Return (TMP1)
        }
//
// Function 2 (Get System BIOS Requests)
//
//   This function reports pending System BIOS requests. It is invoked whenever driver
//   receives a general purpose notification as defined in the function "Get System Parameters".
//   It is valid for the driver to receive multiple notifications where the number of
//   notifications exceeds the number of pending System BIOS requests as reported by this function.
//   System BIOS must clear pending requests indicators when this function is executed. A new
//   request of the same type as a pending request replaces the pending request.
//   The output structure contains parameters associated with each request, where applicable.
//   When pending System BIOS request bit is not set to one,
//   the corresponding request parameterfield is ignored.
//   It is a required function only, if any ATIF notifications are supported by System BIOS.
//
// Arguments:
//
//   Arg0: Function code
//   Arg1: None
//
// Output:
//
//   Structure data containing the following info:
//
// ===================================================================================================
// ||       Field       || Format ||                        Description                             ||
// ===================================================================================================
// || Structure Size    || WORD   || Structure size in bytes including the Structure Size field.    ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Pending System    || DWORD  || Bit 0: Display switch request.                                 ||
// || BIOS Requests     ||        || Bit 1: Expansion mode change request.                          ||
// ||                   ||        || Bit 2: Thermal state change request.                           ||
// ||                   ||        || Bit 3: Forced power state change request.                      ||
// ||                   ||        || Bit 4: System power source change request.                     ||
// ||                   ||        || Bit 5: Display configuration change request.                   ||
// ||                   ||        || Bit 6: PowerXpress graphics switch toggle request.             ||
// ||                   ||        || Bit 7: Panel brightness change request.                        ||
// ||                   ||        || Bit 8: Discrete GPU display connect state change request.      ||
// ||                   ||        || Bits 31-9: Reserved (must be zero).                            ||
// ||                   ||        || System BIOS requests that are not supported as indicated       ||
// ||                   ||        || in the "Supported Notifications Mask" field in the output      ||
// ||                   ||        || structure of the "Verify Interface" function will be ignored.  ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Expansion Mode    || BYTE   || Bits 1-0:                                                      ||
// ||                   ||        || 00 - Disable panel expansion.                                  ||
// ||                   ||        || 01 - Enable panel expansion to full screen size.               ||
// ||                   ||        || 10 - Enable panel expansion maintaining source aspect ratio.   ||
// ||                   ||        || Bits 7-2: Reserved (must be zero).                             ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Thermal State:    || BYTE   || This field indicates target graphics controller.               ||
// || Target Graphics   ||        || 0 - Undefined, used on single graphics controller platforms.   ||
// || Controller        ||        || 1 - Integrated graphics controller, used on PowerXpress        ||
// ||                   ||        ||     enabled platforms.                                         ||
// ||                   ||        || 2 - Discrete graphics controller, used on PowerXpress          ||
// ||                   ||        ||     enabled platforms.                                         ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Thermal State:    || BYTE   || 0 - Exit thermal state, non zero defines a thermal state.      ||
// || State Id.         ||        ||                                                                ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Forced Power      || BYTE   || This field indicates target graphics controller.               ||
// || State:            ||        || 0 - Undefined, used on single graphics controller platforms.   ||
// || Target Graphics   ||        || 1 - Integrated graphics controller, used on PowerXpress        ||
// || Controller        ||        ||     enabled platforms.                                         ||
// ||                   ||        || 2 - Discrete graphics controller, used on PowerXpress          ||
// ||                   ||        ||     enabled platforms.                                         ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Forced Power      || BYTE   || 0 - Exit forced power state; non zero defines a forced power   ||
// || State:            ||        ||     state.                                                     ||
// ||-------------------||--------||----------------------------------------------------------------||
// || System Power      || BYTE   || Bits 2- 0:                                                     ||
// || Source            ||        || 000 - Reserved.                                                ||
// ||                   ||        || 001 - Power source is AC.                                      ||
// ||                   ||        || 010 - Power source is DC.                                      ||
// ||                   ||        || 011 - Power source is "Restricted AC 1".                       ||
// ||                   ||        || 100 - Power source is "Restricted AC 2"                        ||
// ||                   ||        || (lower power adapter than "Restricted AC 1").                  ||
// ||                   ||        || Bits 7-3: Reserved (must be zero).                             ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Panel Brightness: || BYTE   || Backlight level: 0 - 255:                                      ||
// || Backlight Level   ||        ||                                                                ||
// ===================================================================================================
//
        If (LEqual (Arg0, ATIF_GET_SBIOS_REQUESTS))
        {
            Name (TMP2, Buffer(256) {0x00})
            CreateWordField  (TMP2,  0, F2SS)
            CreateDwordField (TMP2,  2, F2PR)
            CreateByteField  (TMP2,  6, F2EM)
            CreateByteField  (TMP2,  7, F2TG)
            CreateByteField  (TMP2,  8, F2SI)
            CreateByteField  (TMP2,  9, F2FG)
            CreateByteField  (TMP2, 10, F2FI)
            CreateByteField  (TMP2, 11, F2SP)

            Store (0x000A, F2SS)   // Size of return structure
            Name (STS2, Zero)      // Initialize Pending System BIOS Requests field
            Or (DGPU_SCOPE.DSWR, STS2, STS2)  // Display switch request
            Or (ShiftLeft (DGPU_SCOPE.PXGS, 0x06), STS2, STS2)
            Store (STS2, F2PR)
            Store (Zero, DGPU_SCOPE.DSWR)  // Clear Display switch request
            Store (Zero, DGPU_SCOPE.PXGS)
            Or (F2PR, 0x00000100, F2PR) // Discrete GPU display connect state change request

            Return (TMP2)
        }
//
// Function 3 (Select Active Displays)
//
//   This function returns displays to be selected in response to display switch request notification.
//   Currently active and currently connected displays are passed to System BIOS. It is a required
//   function only, if display switch request notification is supported by System BIOS.
//
// Arguments:
//
//   Arg0: Function code
//   Arg1: Structure data containing the following info:
//
// ===================================================================================================
// ||       Field       || Format ||                        Description                             ||
// ===================================================================================================
// || Structure Size    || WORD   || Structure size in bytes including the Structure Size field.    ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Selected Displays || WORD   || IThis field indicates currently selected displays.             ||
// ||                   ||        || Bit 0: LCD1     Bit 7: DFP2                                    ||
// ||                   ||        || Bit 1: CRT1     Bit 8: CV                                      ||
// ||                   ||        || Bit 2: TV       Bit 9: DFP3                                    ||
// ||                   ||        || Bit 3: DFP1     Bit 10: DFP4                                   ||
// ||                   ||        || Bit 4: CRT2     Bit 11: DFP5                                   ||
// ||                   ||        || Bit 5: LCD2     Bit 12: DFP6                                   ||
// ||                   ||        || Bit 6: Reserved (must be zero)                                 ||
// ||                   ||        || Bits 15-13: Reserved (must be zero).                           ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Connected         || WORD   || This field indicates currently connected displays.             ||
// || Displays          ||        || Bit 0: Reserved (must be zero).   Bit 7: DFP2                  ||
// ||                   ||        || Bit 1: CRT1                       Bit 8: CV                    ||
// ||                   ||        || Bit 2: TV                         Bit 9: DFP3                  ||
// ||                   ||        || Bit 3: DFP1                       Bit 10: DFP4                 ||
// ||                   ||        || Bit 4: CRT2                       Bit 11: DFP5                 ||
// ||                   ||        || Bit 5: Reserved (must be zero).   Bit 12: DFP6                 ||
// ||                   ||        || Bit 6: Reserved (must be zero).                                ||
// ||                   ||        || Bits 15-13: Reserved (must be zero).                           ||
// ===================================================================================================
//
// Output:
//
//   Structure data containing the following info:
//
// ===================================================================================================
// || Structure Size    || WORD   || Structure size in bytes including the Structure Size field.    ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Selected Displays || BYTE   || Bit 0: LCD1       Bit 7: DFP2                                  ||
// ||                   ||        || Bit 1: CRT1       Bit 8: CV                                    ||
// ||                   ||        || Bit 2: TV         Bit 9: DFP3                                  ||
// ||                   ||        || Bit 3: DFP1       Bit 10: DFP4                                 ||
// ||                   ||        || Bit 4: CRT2       Bit 11: DFP5                                 ||
// ||                   ||        || Bit 5: LCD2       Bit 12: DFP6                                 ||
// ||                   ||        || Bit 6: Reserved (must be zero)                                 ||
// ||                   ||        || Bits 15-13: Reserved (must be zero).                           ||
// ===================================================================================================
//
        If (LEqual (Arg0, ATIF_SELECT_ACTIVE_DISPLAY))
        {
            Name (TMP3, Buffer(256) {0x00})
            CreateWordField (TMP3, 0, F3SS)
            CreateWordField (TMP3, 2, F3SD)
            CreateWordField (Arg1, 0, AI3S) // Input - Function 3 Structure Size
            CreateWordField (Arg1, 2, SLDS) // Input - Function 3 Selected Displays
            CreateWordField (Arg1, 4, CODS) // Input - Function 3 Connected Displays

            Store (SLDS, DGPU_SCOPE.CACD)             // Selected Displays(SLDS) save to Currently Active Displays(CACD)
            Store (CODS, DGPU_SCOPE.CCND)             // Connected Displays(CODS) save to Currently Connected Displays(CCND)
            If (\ECON)
            {
                If (LEqual (\_SB.PCI0.LPCB.H_EC.LSTE, One))
                {
                    //
                    // Built-in LCD1 panels are always attached. So or the value 0x0001
                    //
                    Or (DGPU_SCOPE.CCND, One, DGPU_SCOPE.CCND)
                }
            }

            Store (0x0004, F3SS) // Size of return structure
            Store (CTOI (DGPU_SCOPE.CACD), DGPU_SCOPE.TLSN)
            Store (DGPU_SCOPE.CACD, Local1)
            Store (DGPU_SCOPE.NTLE, Local0)
 
            While (Local0)
            {
                Store (NATL (DGPU_SCOPE.TLSN), Local1)
                If (LNot (LEqual (Local1, Zero)))
                {
                    If (LEqual (And (Local1, DGPU_SCOPE.CCND), Local1))
                    {
                        Store (One, Local0)
                    }
                }
                Decrement (Local0)
                Increment (DGPU_SCOPE.TLSN)
                If (LGreater (DGPU_SCOPE.TLSN, DGPU_SCOPE.NTLE))
                {
                    Store (One, DGPU_SCOPE.TLSN)
                }
            }
            SNXD (Local1)
            Store (Local1, DGPU_SCOPE.NACD)
            Store (DGPU_SCOPE.NACD, F3SD)
            Return (TMP3)
        }

        Return (Zero)
    }

    //
    // Method:
    //
    //   COTA - Get the toggle list sequence number for Currently Active display list
    //
    // Input:
    //
    //   Selected displays combinations
    //
    // Output:
    //
    //   1  - LCD1 (LCD)
    //   2  - CRT1 (CRT)
    //   3  - DFP1 (DVI)
    //   4  - DFP2 (HDMI)
    //   5  - DFP3
    //   6  - DFP4
    //   7  - DFP5
    //   8  - CRT1 + LCD1
    //   9  - CRT1 + DFP1
    //   10 - CRT1 + DFP2
    //   11 - CRT1 + DFP3
    //   12 - CRT1 + DFP4
    //   13 - CRT1 + DFP5
    //
    Method (CTOI, 1)
    {
        //
        // If the Total number of toggle list entries is not equal zero, return it.
        //
        If (LNotEqual (DGPU_SCOPE.NTLE, Zero))
        {
            If (LEqual (DGPU_SCOPE.TLE1, Arg0))
            {
                Return (1)
            }
            If (LEqual (DGPU_SCOPE.TLE2, Arg0))
            {
                Return (2)
            }
            If (LEqual (DGPU_SCOPE.TLE3, Arg0))
            {
                Return (3)
            }
            If (LEqual (DGPU_SCOPE.TLE4, Arg0))
            {
                Return (4)
            }
            If (LEqual (DGPU_SCOPE.TLE5, Arg0))
            {
                Return (5)
            }
            If (LEqual (DGPU_SCOPE.TLE6, Arg0))
            {
                Return (6)
            }
            If (LEqual (DGPU_SCOPE.TLE7, Arg0))
            {
                Return (7)
            }
            If (LEqual (DGPU_SCOPE.TLE8, Arg0))
            {
                Return (8)
            }
            If (LEqual (DGPU_SCOPE.TLE9, Arg0))
            {
                Return (9)
            }
            If (LEqual (DGPU_SCOPE.TL10, Arg0))
            {
                Return (10)
            }
            If (LEqual (DGPU_SCOPE.TL11, Arg0))
            {
                Return (11)
            }
            If (LEqual (DGPU_SCOPE.TL12, Arg0))
            {
                Return (12)
            }
            If (LEqual (DGPU_SCOPE.TL13, Arg0))
            {
                Return (13)
            }
            If (LEqual (DGPU_SCOPE.TL14, Arg0))
            {
                Return (14)
            }
            If (LEqual (DGPU_SCOPE.TL15, Arg0))
            {
                Return (15)
            }
        }
        Return (Zero)
    }

    //
    // Method:
    //
    //   NATL - Get the next active display list
    //
    // Input:
    //
    //   1  - LCD1 (LCD)
    //   2  - CRT1 (CRT)
    //   3  - DFP1 (DVI)
    //   4  - DFP2 (HDMI)
    //   5  - DFP3
    //   6  - DFP4
    //   7  - DFP5
    //   8  - CRT1 + LCD1
    //   9  - CRT1 + DFP1
    //   10 - CRT1 + DFP2
    //   11 - CRT1 + DFP3
    //   12 - CRT1 + DFP4
    //   13 - CRT1 + DFP5
    //
    // Output:
    //
    //   Selected displays combinations
    //
    Method (NATL, 1)
    {
        If (LNotEqual (DGPU_SCOPE.NTLE, Zero))
        {
            If (LEqual (Arg0, 1))
            {
                Return (DGPU_SCOPE.TLE2)
            }
            If (LEqual (Arg0, 2))
            {
                Return (DGPU_SCOPE.TLE3)
            }
            If (LEqual (Arg0, 3))
            {
                Return (DGPU_SCOPE.TLE4)
            }
            If (LEqual (Arg0, 4))
            {
                Return (DGPU_SCOPE.TLE5)
            }
            If (LEqual (Arg0, 5))
            {
                Return (DGPU_SCOPE.TLE6)
            }
            If (LEqual (Arg0, 6))
            {
                Return (DGPU_SCOPE.TLE7)
            }
            If (LEqual (Arg0, 7))
            {
                Return (DGPU_SCOPE.TLE8)
            }
            If (LEqual (Arg0, 8))
            {
                Return (DGPU_SCOPE.TLE9)
            }
            If (LEqual (Arg0, 9))
            {
                Return (DGPU_SCOPE.TL10)
            }
            If (LEqual (Arg0, 10))
            {
                Return (DGPU_SCOPE.TL11)
            }
            If (LEqual (Arg0, 11))
            {
                Return (DGPU_SCOPE.TL12)
            }
            If (LEqual (Arg0, 12))
            {
                Return (DGPU_SCOPE.TL13)
            }
            If (LEqual (Arg0, 13))
            {
                Return (DGPU_SCOPE.TL14)
            }
            If (LEqual (Arg0, 14))
            {
                Return (DGPU_SCOPE.TL15)
            }
            If (LEqual (Arg0, 15))
            {
                Return (DGPU_SCOPE.TLE1)
            }
        }
        Return (Zero)
    }
    Method (INDL, 0, Serialized)
    {
        Store (Zero, NXD1)
        Store (Zero, NXD2)
        Store (Zero, NXD3)
        Store (Zero, NXD4)
        Store (Zero, NXD5)
        Store (Zero, NXD6)
        Store (Zero, NXD7)
        Store (Zero, NXD8)
    }
    Method (SNXD, 1, Serialized)
    {
        INDL ()
        Store (Arg0, Local0)
        If (And (Local0, 0x02))
        {
            Store (One, NXD1)
        }
        If (And (Local0, One))
        {
            Store (One, NXD2)
        }
        If (And (Local0, 0x08))
        {
            Store (One, NXD3)
        }
        If (And (Local0, 0x80))
        {
            Store (One, NXD4)
        }
        If (And (Local0, 0x0200))
        {
            Store (One, NXD5)
        }
        If (And (Local0, 0x0400))
        {
            Store (One, NXD6)
        }
        If (And (Local0, 0x0800))
        {
            Store (One, NXD7)
        }
    }
}
