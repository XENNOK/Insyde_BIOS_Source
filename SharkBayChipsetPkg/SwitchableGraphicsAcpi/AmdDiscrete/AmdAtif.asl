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
    // For saving Pending System BIOS requests (these get cleared only when function 2 is called)
    Name (PSBR, Buffer(0x4) {0, 0, 0, 0}) // Pending System BIOS Requests
    Name (NCOD, 0x81) // Default General VGA Notification Code for ATIF
    Name (MSKN, 0x00) // Save Notification Mask
    Name (TSTG, 0) // Thermal State: Target Graphics Controller
    Name (TSSI, 0) // Thermal State: State Id
    Name (FPTG, 0) // Forced Power State: Target Graphics Controller
    Name (FPSI, 0) // Forced Power State: State Id
    Name (SPWS, 0) // System Power Source
    Name (SUPD, 0) // Supported Displays
    Name (CACD, 0) // Currently active displays
    Name (CCND, 0) // Currently connected displays
    Name (NACD, 0) // Next active displays
    Name (TLST, 0) // Toggle List Number
    Name (LDST, 0) // Lid Switch Toggle 
    Name (TLSN, 0) // Toggle list sequence index
    Name (MADL, 0) // Current attached display device
    Name (DSFL, 0) // Display Switch flag?
    Name (MSTE, 0) // Current actived display device
    Name (NTLE, 0) // Total number of toggle list entries
    Name (DPFL, 1) // DP or TMDS flag?
       
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

            // Supported Notifications Mask when PowerXpress is enabled
            // We support only BIT4 (System power source change request) and
            // BIT0 (Display switch request, currently limited to discrete GPU
            // only), and BIT6 (PowerXpress graphics controller switch)
            // on the AMD reference board when PowerXpress is enabled.            
            // If customer does not call the VGA.AFN4() function from the
            // AC/Battery detect _PSR method (see ac.asi sample code)
            // then this mask should be changed to 0x00000041
            // (Display switch request and PowerXpress switch only)
            Store (0x000000D1,F0SN) // Supported Notifications Mask
            Store (F0SN, MSKN) // Save the mask
            Store (0x00000007, F0SF) // Supported Functions Bit Vector, support function 1, 2, 3

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
            Store (0x81, NCOD) // Save Notify Code in Namespace variable

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
            // PSBR - Pending System BIOS Requests
            // BIT0 - If set then Display switch was requested
            // BIT1 - If set then Expansion mode change was requested
            // BIT2 - If set then Thermal state change was requested
            // BIT3 - If set then Forced power state change was requested
            // BIT4 - If set then System power sopurce change was requested
            // BIT5 - If set then Display configuration change was requested
            // BIT6 - If set then PowerXpress graphics switch toggle was requested
            // BIT7 - If set then brightness change was requested
            CreateBitField (PSBR, 0, PDSW) // Display switch request
            CreateBitField (PSBR, 1, PEXM) // Expansion mode request
            CreateBitField (PSBR, 2, PTHR) // Thermal state change request
            CreateBitField (PSBR, 3, PFPS) // Forced power state change request
            CreateBitField (PSBR, 4, PSPS) // System power state change request
            CreateBitField (PSBR, 5, PDCC) // Display configuration change request
            CreateBitField (PSBR, 6, PXPS) // PowerXpress graphics controller switch request
            CreateBitField (PSBR, 7, PBRT) // Brightness level change request

            Name (TMP2, Buffer(256) {0x00})
            CreateWordField  (TMP2,  0, F2SS)
            CreateDwordField (TMP2,  2, F2PR)
            CreateByteField  (TMP2,  6, F2EM)
            CreateByteField  (TMP2,  7, F2TG)
            CreateByteField  (TMP2,  8, F2SI)
            CreateByteField  (TMP2,  9, F2FG)
            CreateByteField  (TMP2, 10, F2FI)
            CreateByteField  (TMP2, 11, F2SP)
            CreateByteField  (TMP2, 12, F2BR) // Brightness Level
            // Note: PEG mode,Brightness level too low, AMD' ATIF range is 0-255, we are 0-100 now  			
            CreateWordField  (TMP2, 13, BKLT)

            Store (0x0D, F2SS) // Size of return structure
            Store (PSBR, F2PR) // Load current pending System BIOS request indicators (passing back)

            If (PDSW) {
                Store (0x0D, NTLE) // Total number of toggle list entries
                Store (Zero, MADL) // Clear Connected display states
                Store (Zero, PDSW) // Clear out pending indicator
            }
            If (PEXM) {
                Store (EXPM, F2EM) // Pass back panel expansion mode
                Store (Zero, EXPM)
                Store (Zero, PEXM) // Clear out pending indicator
            }
            If (PTHR) {
                Store (TSTG, F2TG) // Pass back thermal state Gfx controller
                Store (TSSI, F2SI) // Pass back thermal state ID
                Store (Zero, TSTG)
                Store (Zero, TSSI)
                Store (Zero, PTHR) // Clear out pending indicator
            }
            If (PFPS) {
                Store (FPTG, F2FG) // Pass back forced power state Gfx controller
                Store (FPSI, F2FI) // Pass back forced power state ID
                Store (Zero, FPTG)
                Store (Zero, FPSI)
                Store (Zero, PFPS) // Clear out pending indicator
            }
            If (PSPS) {
                Store (SPWS, F2SP) // Pass back system power state
                Store (Zero, PSPS) // Clear out pending indicator
            }
            If (PXPS) {            // PowerXpress GPU switch request
                Store(Zero, PXPS)  // Clear out pending indicator
            }
            If (PBRT)
            {
                Store (BRTL, F2BR) // Pass back Brightness level
                // Note: PEG mode,Brightness level too low, AMD' ATIF range is 0-255, we are 0-100 now
                Divide (Multiply (F2BR, 0xFF, BKLT), 0x64, , F2BR)
                Store (Zero, PBRT)
            }

            Return(TMP2)
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

            //
            // Check connected display
            //
            // Bit 0:   Reserved (must be zero). 
            // Bit 1:   CRT1 
            // Bit 2:   TV 
            // Bit 3:   DFP1 
            // Bit 4:   CRT2 
            // Bit 5:   Reserved (must be zero). 
            // Bit 6:   DFP6 
            // Bit 7:   DFP2 
            // Bit 8:   CV 
            // Bit 9:   DFP3 
            // Bit 10:  DFP4 
            // Bit 11:  DFP5 
            // Bits 15-12: Reserved (must be zero).
            //

            // LCD (Check Lid States)
            If (LEqual (\_SB.PCI0.LPCB.EC0.LIDP, 0))
            {
                Or (MADL, 0x08, MADL)
            }
            // CRT
            If (AND (CODS, 0x0002))
            { 
                Or (MADL, 0x02, MADL)
            }
            // HDMI (DFP3)
            If (AND (CODS, 0x0200))
            {
                Or (MADL, 0x01, MADL)
            }
            // DP (DFP1)
            If (AND (CODS, 0x0008))
            {
                Or (MADL, 0x04, MADL)
            }
//            // DP
//            If (AND (CODS, 0x0200))
//            {
//                Or (MADL, 0x04, MADL)
//            }
//            // DP
//            If (AND (CODS, 0x0400))
//            {
//                Or (MADL, 0x04, MADL)
//            }
//            // DP
//            If (AND (CODS, 0x0800))
//            {
//                Or (MADL, 0x04, MADL)
//            }

            Store (0x0004, F3SS) // Size of return structure

            If (LEqual (DSFL, Zero))
            {
                Store (SLDS, F3SD)
                return (TMP3)
            }  
            Store (MSTE, CACD)
            Store (NATL (CACD), Local1) // Get the next active display
            Store (Local1, NACD) // The next active toggle list - put it on Opregion
            Store (NACD, F3SD) // Store it in the return buffer
            Return (TMP3)
        }

        Return (Zero)
    }

    //
    // Method: NATL - Get the next active display
    // Input:
    //  (0x01) HDMI only
    //  (0x02) CRT only
    //  (0x03) HDMI + CRT
    //  (0x04) DP only
    //  (0x05) HDMI + DP
    //  (0x06) CRT + DP
    //  (0x07) HDMI + CRT + DP
    //  (0x08) LCD only
    //  (0x09) HDMI + LCD
    //  (0x0A) CRT + LCD
    //  (0x0B) HDMI + CRT + LCD
    //  (0x0C) DP + LCD
    //  (0x0D) HDMI + DP + LCD
    //  (0x0E) CRT + DP + LCD
    //
    // Output: Selected displays combinations
    // Bit Map:
    // Bit 0:  LCD1 
    // Bit 1:  CRT1 
    // Bit 2:  TV 
    // Bit 3:  DFP1 (DP)
    // Bit 4:  CRT2 
    // Bit 5:  LCD2 
    // Bit 6:  DFP6 
    // Bit 7:  DFP2 
    // Bit 8:  CV 
    // Bit 9:  DFP3 (HDMI)
    // Bit 10: DFP4 
    // Bit 11: DFP5 
    // Bits 15-12: Reserved (must be zero).
    //
    Method (NATL, 1)
    {
        Switch (ToInteger (Arg0))
        {
            case (0x01) // HDMI only
            {
                return (0x0200)
            }
            case (0x02) // CRT only
            {
                return (0x0002)
            }
            case (0x03) // HDMI + CRT
            {
                return (0x0202)
            }
            case (0x04) // DP only
            {
                return (0x0008)
            }
            case (0x05) // HDMI + DP
            {
                return (0x0208)
            }
            case (0x06) // CRT + DP
            {
                return (0x000A)
            }
            case (0x07) // HDMI + CRT + DP
            {
                return (0x020A)
            }
            case (0x08) // LCD only
            {
                return (0x0001)
            }
            case (0x09) // HDMI + LCD
            {
                return (0x0201)
            }
            case (0x0A) // CRT + LCD
            {
                return (0x0003)
            }
            case (0x0B) // HDMI + CRT + LCD
            {
                return (0x0203)
            }
            case (0x0C) // DP + LCD
            {
                return (0x0009)
            }
            case (0x0D) // HDMI + DP + LCD
            {
                return (0x0209)
            }
            case (0x0E) // CRT + DP + LCD
            {
                return (0x0000B)
            }
        } // end of Switch
    }

    Method (AFN0, 0, Serialized)	
    {
        If (And (MSKN, 0x01)) // if notification supported
        {
            CreateBitField (PSBR, 0, PDSW) // Display switch request
            Store (One, PDSW) // Set the request to pending state
            Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    }

    Method (AFN1, 0, Serialized)
    {
        If (And (MSKN, 0x02)) // if notification supported
        {
            CreateBitField (PSBR, 1, PEXM) // Expansion mode request
            Store (One, PEXM) // Set the request to pending state
            //
            // Expansion Mode toggling
            //
            If (LEqual (EXPM, 2))
            {
                Store (0, EXPM)
            }
            Else
            {
                Increment (EXPM)
            }
            Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    }

    Method (AFN3, 2, Serialized)
    {
        If (And (MSKN, 0x08)) // if notification supported
        {
            Store (Arg0, Local0) // Get the forced power state
            Store (Local0, FPSI) // 0 - exit forced power state,
                                 // non-zero - forced power state
            Store (Arg1, Local0) // Get the target Gfx controller
            Store (And (Local0, 0x03, Local0), FPTG) // save it
            CreateBitField (PSBR, 3, PFPS) // Forced power state change request
            Store (One, PFPS) // Set the request to pending state
            Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    }

    Method (AFN4, 1, Serialized)
    {
        If (And (MSKN, 0x10)) // if notification supported
        {
            Store (Arg0, Local0) // Get the current system power state
            Store (SPWS, Local1) // Get the previous system power state
            Store (Local0, SPWS) // 00 - Reserved
                                 // 01 - Power source is AC
                                 // 10 - Power source is DC
                                 // 11 - Power source is restricted AC
            // Only set pending request and notify if the power state actually changed
            if (LNotEqual (Local0, Local1))
            {
                CreateBitField (PSBR, 4, PSPS) // System power state change request
                Store (One, PSPS) // Set the request to pending state
                Notify (DGPU_SCOPE, NCOD) // Inform the display driver
            }
        }
    }

    Method (AFN5, 0, Serialized)
    {
        If (And (MSKN, 0x20)) // if notification supported (BIT5)
        {
            CreateBitField (PSBR, 5, PDCC) // Display configuration change request
            Store (One, PDCC) // Set the request to pending state
            Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    }

    Method (AFN6, 0, Serialized)
    {
        If (And (MSKN, 0x40)) // if notification supported (BIT6)
        {
           CreateBitField (PSBR, 6, PXPS) // PowerXpress graphics switch toggle request
           Store (One, PXPS) // Set the request to pending state
           Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    } 

    Method (AFN7, 1, Serialized)
    {
        If (And (MSKN, 0x80))
        {
            CreateBitField (PSBR, 7,PBRT)
            Store (One, PBRT)
            Store (Arg0, BRTL)
            Notify (DGPU_SCOPE, NCOD) // Inform the display driver
        }
    }
}
