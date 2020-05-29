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
//
// Switchable Graphics ACPI Control
// Methods and Notifications
// Rev. 0.27
//
// ATI PowerXpress (PX) Contrl Method: ATPX
//
//   This is PowerXpress specific Control Method used by integrated graphics or discrete
//   graphics driver on PowerXpress enabled platforms. The existence of this method indicates
//   support for PowerXpress. This method provides multiple functions that can be
//   invoked by the display driver. The general format of this method is as follows.
//
// Arguments:
//
//     Arg0 (ACPI_INTEGER): Function code
//     Arg1 (ACPI_BUFFER): Parameter buffer, 256 bytes
//
// Output:
//
//     (ACPI_BUFFER): 256 bytes.
//
// Arg0 parameter may define the following values (only lower 32bit used):
//
    Method (ATPX, 2, Serialized)
    {
//
// Function 0 (Verify PowerXpress Interface)
//
//   This function provides interface version and functions supported by PowerXpress System BIOS.
//   It is a required function, if any other ATPX functions are supported by System BIOS.
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
// || Supported         || DWORD  || Bit vector providing supported functions information. Each bit ||
// || Functions Bit     ||        || marks support for one specific function of the ATPX method.    ||
// || Vector            ||        || Bit n, if set, indicates that Function n+1 is supported.       ||
// ===================================================================================================
//
        If (LEqual (Arg0, PX_VERIFY_POWERXPRESS_INTERFACE))
        {
            Name (TMP1, Buffer(256) {0x00})
            CreateWordField  (TMP1, 0, F0SS)
            CreateWordField  (TMP1, 2, F0IV)
            CreateDwordField (TMP1, 4, F0SF)

            Store (0x08, F0SS)
            Store (One, F0IV)
            Store (0x000000BF, F0SF)
    
            If (LEqual (And (DGPU_SCOPE.SGMD, 0x0F), Zero)) // SG Mode (0 = Disabled)
            {
                Store (Zero, F0SF)
                Return (TMP1)
            }
            If (LEqual (And (DGPU_SCOPE.SGMD, 0x0F), 0x03)) // SG Mode (3 = DGPU Only)
            {
                Store (Zero, F0SF)
                Return (TMP1)
            }
            If (LEqual (And (DGPU_SCOPE.SGMD, 0x0F), 0x02)) // SG Mode (2 = SG Muxless)
            {
                Store (0x00000033, F0SF)
            }
            //
            // PX Dynamic Mode Switch Enabled
            // Don't support ATPX function 2
            //
            If (LEqual (DGPU_SCOPE.PXDY, One))
            {
                And (F0SF, 0xFFFFFFFD, F0SF)
            }
            //
            // Support both Dynamic and Fixed PX switch
            // Support ATPX function 2
            //
            If (LEqual (DGPU_SCOPE.PXFD, One))
            {
                Or (F0SF, 0x00000002, F0SF)
            }
            //
            // Support Full dGPU Poweroff Dynamic mode
            // Support ATPX function 2
            //
            If (LEqual (DGPU_SCOPE.FDPD, One))
            {
                Or (F0SF, 0x00000002, F0SF)
            }
            Return (TMP1)
        }
//
// Function 1 (Get PowerXpress Parameters)
//
//   This function retrieves various PowerXpress related platform parameters. It is assumed
//   that LCD1 display output signals are always multiplexed.
//   It is a required function if any display multiplexers are supported on a given platform.
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
// || Valid Flags Mask  || DWORD  || Indicates which of the "Flags" bits are valid.                 ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Flags             || DWORD  || Bit 0: LVDS I2C is accessible to both graphics controllers.    ||
// ||                   ||        || Bit 1: CRT1 I2C is accessible to both graphics controllers.    ||
// ||                   ||        || Bit 2: DVI1 I2C is accessible to both graphics controllers.    ||
// ||                   ||        || Bit 3: CRT1 RGB signals are multiplexed.                       ||
// ||                   ||        || Bit 4: TV1 signals are multiplexed.                            ||
// ||                   ||        || Bit 5: DFP1 signals are multiplexed.                           ||
// ||                   ||        || Bit 6: Indicates that a separate multiplexer control           ||
// ||                   ||        ||        for I2C/Aux/HPD exists and is controlled by             ||
// ||                   ||        ||        function 4 (Monitor I2C Control).                       ||
// ||                   ||        || Bit 7: Indicates that a "dynamic" PX scheme is supported.      ||
// ||                   ||        || Bit 8: Indicates that Dual Graphics (Asymmetric CrossFire)     ||
// ||                   ||        ||        is not supported, if set to one.                        ||
// ||                   ||        || Bit 9: Indicates that fixed scheme is not supported,           ||
// ||                   ||        ||        if set to one.                                          ||
// ||                   ||        || Bit 10: Indicates that full dGPU power off in "dynamic" scheme ||
// ||                   ||        ||         is supported, if set to one.                           ||
// ||                   ||        || Bit 11: Indicates that discrete graphics must be powered on    ||
// ||                   ||        ||         while a monitor is connected to discrete graphics      ||
// ||                   ||        ||         connector, if set to one.                              ||
// ||                   ||        || Bits 31-12: Reserved (must be zero).                           ||
// ===================================================================================================
//
        If (LEqual (Arg0, PX_GET_POWERXPRESS_PARAMETERS))
        {
            Name (TMP2, Buffer(256) {0x00})
            CreateWordField  (TMP2, 0, F1SS)
            CreateDwordField (TMP2, 2, F1VM)
            CreateDwordField (TMP2, 6, F1FG)

            Store (0x000A, F1SS)  //Structure size of return package
            Store (0x0000007F, F1VM)
    
            If (LEqual (And (DGPU_SCOPE.SGMD, 0x0F), 0x02))
            {
                //
                // Muxless-Based Power Xpress Support
                //
                Store (0x00000000, F1FG)
                Store (0x00000000, F1VM)
                If (LEqual (DGPU_SCOPE.PXDY, 0x01))
                {
                    Or (F1FG, 0x00000080, F1FG) // Dynamic scheme
                    Or (F1VM, 0x00000080, F1VM)
                }
                If (LEqual (DGPU_SCOPE.PXFD, 0x00))
                {
                    Or (F1FG, 0x00000200, F1FG) // Fixed scheme is not supported
                    Or (F1VM, 0x00000200, F1VM)
                }
                If (LEqual (DGPU_SCOPE.FDPD, 0x01))
                {
                    Or (F1FG, 0x00000400, F1FG) // Full dGPU power off in "dynamic" scheme is supported
                    Or (F1VM, 0x00000400, F1VM)
                    //
                    // PX 5.6: discrete graphics must be powered on while a monitor is connected
                    //
                    Or (F1FG, 0x00000800, F1FG)
                    Or (F1VM, 0x00000800, F1VM)
                }
            }
            Else
            {
                //
                // Mux-Based Power Xpress Support
                //
                Store (0x00000040, F1FG)
                Store (0x00000040, F1VM)
            }
            Return (TMP2)
        }
//
// Function 2 (Power Control)
//
//   This function powers on/off the discrete graphics.
//   It is a required function.
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
// || Power State       || BYTE   || Indicates which of the "Flags" bits are valid.                 ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Flags             || DWORD  || Bit 0:                                                         ||
// ||                   ||        || 0 - Power off discrete graphics.                               ||
// ||                   ||        || 1 - Power on discrete graphics.                                ||
// ||                   ||        || Bits 7-1: Reserved (must be zero).                             ||
// ===================================================================================================
//
// Output:
//
//   None.
//
        If (LEqual (Arg0, PX_POWER_CONTROL))
        {
            CreateWordField (Arg1, 0, FN2S)
            CreateByteField (Arg1, 2, DGPR)

            If (LEqual (DGPR, Zero))  // Powers off discrete graphics
            {
                DGPU_SCOPE._OFF ()
            }
            If (LEqual (DGPR, One))  // Powers on discrete graphics
            {
                DGPU_SCOPE._ON ()
            }
            Return (Zero)
        }
//
// Function 3 (Display Multiplexer Control)
//
//   This function controls display multiplexers.
//   It is a required function only, if display output multiplexers are supported on
//   a given platform. This function must not be exposed by System BIOS on platforms
//   where display output multiplexers are not supported.
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
// || Display           || WORD   || Bit 0:                                                         ||
// || Multiplexer       ||        || 0 - Switch display multiplexers to integrated graphics.        ||
// || Control           ||        || 1 - Switch display multiplexers to discrete graphics.          ||
// ||                   ||        || Bits 15-1: Reserved (must be zero).                            ||
// ===================================================================================================
//
// Output:
//
//   None.
//
        If (LEqual (Arg0, PX_DISPLAY_MULTIPLEXER_CONTROL))
        {
            CreateWordField (Arg1, 0, FN3S)
            CreateWordField (Arg1, 2, SDMG)

            If (LEqual (SDMG, Zero))
            {
                DGPU_SCOPE.SGPO (DGPU_SCOPE.DSEL, Zero)
            }
            If (LEqual (SDMG, One))
            {
                DGPU_SCOPE.SGPO (DGPU_SCOPE.DSEL, One)
            }
            Return (Zero)
        }
//
// Function 4 (Monitor I2C Control)
//
//   This function controls monitor I2C/Aux and HPD (Hot Plug Detect) lines.
//   It is optional and does not need to be supported, if no monitor I2C/Aux/HPD
//   lines need to be switched between two graphics devices.
//   It is a required function only, if display output multiplexers are supported on a given platform.
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
// || Monitor(s) I2C    || WORD   || Bit 0:                                                         ||
// || Control           ||        || 0 - Switch I2C/Aux/HPD lines for all monitors                  ||
// ||                   ||        ||     to integrated graphics.                                    ||
// ||                   ||        || 1 - Switch I2C/Aux/HPD lines for all monitors                  ||
// ||                   ||        ||     to discrete graphics.                                      ||
// ||                   ||        || Bits 15-1: Reserved (must be zero).                            ||
// ===================================================================================================
//
// Output:
//
//   None.
//
        If (LEqual (Arg0, PX_MONITOR_I2C_CONTROL))
        {
            CreateWordField (Arg1, 0, FN4S)
            CreateWordField (Arg1, 2, SIMG)

            If (LEqual (SIMG, Zero))
            {
                DGPU_SCOPE.SGPO (DGPU_SCOPE.ESEL, Zero)
            }
            If (LEqual (SIMG, One))
            {
                DGPU_SCOPE.SGPO (DGPU_SCOPE.ESEL, One)
            }
            Return (Zero)
        }
//
// Function 5 (Graphics Device Switch Start Notification)
//
//   This function notifies System BIOS that graphics device switch process has been started.
//   It is an optional function. It is only required if System BIOS needs to know
//   which graphics device is active.
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
// || Target Graphics   || WORD   || Bit 0:                                                         ||
// || Device            ||        || 0 - Target graphics device is integrated graphics.             ||
// ||                   ||        || 1 - Target graphics device is discrete graphics.               ||
// ||                   ||        || Bits 15-1: Reserved (must be zero).                            ||
// ===================================================================================================
//
// Output:
//
//   None.
//
        If (LEqual (Arg0, PX_GFX_SWITCH_START_NOTIFICATION))
        {
            CreateWordField (Arg1, 0, FN5S)
            CreateWordField (Arg1, 2, TGFX)
 
            Store (TGFX, DGPU_SCOPE.TGXA)
            Store (One,  DGPU_SCOPE.GSTP)
 
            Return (Zero)
        }
//
// Function 6 (Graphics Device Switch End Notification)
//
//   This function notifies System BIOS that graphics device switch process has been completed.
//   It is an optional function. It is only required if System BIOS needs to know
//   which graphics device is active.
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
// || Active Graphics   || WORD   || Bit 0:                                                         ||
// || Device            ||        || 0 - Active graphics device is integrated graphics.             ||
// ||                   ||        || 1 - Active graphics device is discrete graphics.               ||
// ||                   ||        || Bits 15-1: Reserved (must be zero).                            ||
// ===================================================================================================
//
// Output:
//
//   None.
//
        If (LEqual (Arg0, PX_GFX_SWITCH_END_NOTIFICATION))
        {
            CreateWordField (Arg1, 0, FN6S)
            CreateWordField (Arg1, 2, AGFX)
 
            Store (AGFX, DGPU_SCOPE.AGXA)
            Store (Zero, DGPU_SCOPE.GSTP)
 
            Return (Zero)
        }
//
// Function 8 (Get Display Connectors Mapping)
//
//   This function reports all display connectors on a given platform.Where display outputs from
//   two ports on different adapters are multiplexed, two connector entries will be reported with
//   the same ATIF ID and a different Adapter ID (ACPI ID can be the same or different). Where DP
//   display outputs from two different ports on different adapters are multiplexed, two connector
//   entries will be reported for integrated graphics adapter and two connector entries will be
//   reported for discrete graphics adapter: one corresponds to the DP signal type and the other 
//   is for HDMI signal type. Where display outputs from a single port on an adapter are multiplexed
//   then multiple connector entries will be reported with different ATIF IDs and the same Adapter ID
//   (ACPI IDs will be different).
//   It is a required function on platforms where a separate control for I2C/aux multiplexers is
//   implemented and on platforms where external digital display I2C/aux signals and/or external 
//   digital display outputs are multiplexed.
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
// || Num Connectors    || WORD   || Number of reported display connectors.                         ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Connector         || DWORD  || Connector structure size in bytes excluding the Connector      ||
// || Structure Size    ||        || Structure Size field.                                          ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Flags             || BYTE   || Bit 0: display output supported by the graphics device         ||
// ||                   ||        ||        identified by Adapter ID.                               ||
// ||                   ||        || Bit 1: display detectable through HPD by the graphics device   ||
// ||                   ||        ||        identified by Adapter ID.                               ||
// ||                   ||        || Bit 2: display I2C/Aux lines available to the graphics device  ||
// ||                   ||        ||        identified by Adapter ID.                               ||
// ||                   ||        || Bits 7-3: Reserved (must be zero).                             ||
// ||-------------------||--------||----------------------------------------------------------------||
// || ATIF ID           || BYTE   || Bit position in a global (per platform) ATIF display vector:   ||
// ||                   ||        ||   0 = LCD1      5 = LCD2         10 = DFP4                     ||
// ||                   ||        ||   1 = CRT1      6 = Reserved     11 = DFP5                     ||
// ||                   ||        ||   2 = TV        7 = DFP2         12 = DFP6                     ||
// ||                   ||        ||   3 = DFP1      8 = CV                                         ||
// ||                   ||        ||   4 = CRT2      9 = DFP3                                       ||
// ||                   ||        || ATIF display vector is defined as:                             ||
// ||                   ||        ||   Bit 0: LCD1     Bit 7: DFP2                                  ||
// ||                   ||        ||   Bit 1: CRT1     Bit 8: CV                                    ||    
// ||                   ||        ||   Bit 2: TV       Bit 9: DFP3                                  ||
// ||                   ||        ||   Bit 3: DFP1     Bit 10: DFP4                                 ||
// ||                   ||        ||   Bit 4: CRT2     Bit 11: DFP5                                 ||
// ||                   ||        ||   Bit 5: LCD2     Bit 12: DFP6                                 ||
// ||                   ||        ||   Bit 6: Reserved (must be zero)                               ||
// ||                   ||        ||   Bits 15-13: Reserved (must be zero).                         ||
// ||-------------------||--------||----------------------------------------------------------------||
// || Adapter ID        || BYTE   || Adapter ID: 0 = integrated graphics device,                    ||
// ||                   ||        ||             1 = discrete graphics device on the lowest         ||
// ||                   ||        ||             numbered PCIe bus, increments per PCIe bus number. ||
// ||-------------------||--------||----------------------------------------------------------------||
// || ACPI ID           || WORD   || Connector ACPI ID (local, per adapter),                        ||
// ||                   ||        || can be different for the same connector                        ||
// ||                   ||        || where output is multiplexed between two adapters.              ||
// ===================================================================================================
// ============================================================================================
// ||       Flags                      || ATIF ID (Global,  || Adapter ID || ACPI ID (Local, ||
// ||                                  ||  per Platform)    ||            ||  per Adapter)   ||
// ============================================================================================
// || display output supported         ||       LCD1        ||     0      ||     0x0400      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       CRT1        ||     0      ||     0x0100      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       DFP1        ||     0      ||     0x0300      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       DFP2        ||     0      ||     0x0301      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display I2C/Aux lines available  ||       DFP3        ||     0      ||     0x0302      ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       LCD1        ||     1      ||     0x0110      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       CRT1        ||     1      ||     0x0100      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       DFP1        ||     1      ||     0x0210      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       DFP2        ||     1      ||     0x0210      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ||----------------------------------||-------------------||------------||-----------------||
// || display output supported         ||       DFP3        ||     1      ||     0x0220      ||
// || display I2C/Aux lines available  ||                   ||            ||                 ||
// || display detectable through HPD   ||                   ||            ||                 ||
// ============================================================================================
//
// Note that _DOD method in "Discrete Only" boot-up mode will reflect these IDs.
// =================================================================
// ||  ACPI ID   ||             Connector                         ||
// =================================================================
// || 0x00000100 || VGA #1                                        ||
// || 0x00000110 || LVDS or Embedded DP                           ||
// || 0x00000120 || VGA #2                                        ||
// || 0x000001F0 || Component                                     ||
// || 0x00000200 || Composite or S Video                          ||
// || 0x00000210 || External digital connector #1 (DP, HDMI, DVI) ||
// || 0x00000220 || External digital connector #2 (DP, HDMI, DVI) ||
// || 0x00000230 || External digital connector #3 (DP, HDMI, DVI) ||
// || 0x00000240 || External digital connector #4 (DP, HDMI, DVI) ||
// || 0x00000250 || External digital connector #5 (DP, HDMI, DVI) ||
// || 0x00000260 || External digital connector #6 (DP, HDMI, DVI) ||
// =================================================================
//
// Note that _DOD method in Switchable Graphics boot-up mode will reflect these IDs.
// =================================================================
// ||  ACPI ID   ||             Connector                         ||
// =================================================================
// || 0x80000100 || VGA #1                                        ||
// || 0x80000400 || LVDS or Embedded DP                           ||
// || 0x80000101 || VGA #2                                        ||
// || 0x8000020F || Component                                     ||
// || 0x80000200 || Composite or S Video                          ||
// || 0x80000300 || External digital connector #1 (DP, HDMI, DVI) ||
// || 0x80000301 || External digital connector #2 (DP, HDMI, DVI) ||
// || 0x80000302 || External digital connector #3 (DP, HDMI, DVI) ||
// || 0x80000303 || External digital connector #4 (DP, HDMI, DVI) ||
// || 0x80000304 || External digital connector #5 (DP, HDMI, DVI) ||
// || 0x80000306 || External digital connector #6 (DP, HDMI, DVI) ||
// ================================================================= 
//
        If (LEqual (Arg0, PX_GET_DISPLAY_CONNECTORS_MAPPING))
        {
            Name (TMP3, Buffer(0x0100) {0x0E, 0x00, 0x46, 0x00,
                                        0x07, 0x01, 0x00, 0x00, 0x01,
                                        0x07, 0x01, 0x01, 0x00, 0x01,
                                        0x05, 0x00, 0x00, 0x00, 0x04,
                                        0x05, 0x00, 0x01, 0x10, 0x01,
                                        0x07, 0x03, 0x00, 0x00, 0x03,
                                        0x07, 0x03, 0x01, 0x10, 0x02,
                                        0x07, 0x07, 0x00, 0x01, 0x03,
                                        0x07, 0x07, 0x01, 0x10, 0x02,
                                        0x07, 0x09, 0x00, 0x02, 0x03,
                                        0x07, 0x09, 0x01, 0x20, 0x02,
                                        0x07, 0x0A, 0x00, 0x03, 0x03,
                                        0x07, 0x0A, 0x01, 0x30, 0x02,
                                        0x07, 0x0B, 0x00, 0x04, 0x03,
                                        0x07, 0x0B, 0x01, 0x30, 0x02,
                                        0x07, 0x0C, 0x00, 0x06, 0x03})
            Name (SGFL, One)
            Name (MXD1, SG_MUX_DID_1)
            Name (MXD2, SG_MUX_DID_2)
            Name (MXD3, SG_MUX_DID_3)
            Name (MXD4, SG_MUX_DID_4)
            Name (MXD5, SG_MUX_DID_5)
            Name (MXD6, SG_MUX_DID_6)
            Name (MXD7, SG_MUX_DID_7)
            Name (MXD8, SG_MUX_DID_8)
            Name (MXD9, SG_MUX_DID_9)
            CreateWordField (TMP3, 0, ATNO)
            CreateWordField (TMP3, 2, ATSZ)
            Store (ADPM (MXD2, 1), Index (TMP3, 14))
            Store (ADPM (MXD2, 1), Index (TMP3, 19))
            Store (ADPM (MXD3, 1), Index (TMP3, 24))
            Store (ADPM (MXD3, 2), Index (TMP3, 29))
            Store (ADPM (MXD4, 1), Index (TMP3, 34))
            Store (ADPM (MXD4, 2), Index (TMP3, 39))
            Store (ADPM (MXD5, 1), Index (TMP3, 44))
            Store (ADPM (MXD5, 2), Index (TMP3, 49))
            Store (ADPM (MXD6, 1), Index (TMP3, 54))
            Store (ADPM (MXD6, 2), Index (TMP3, 59))
            Store (ADPM (MXD7, 1), Index (TMP3, 64))
            Store (ADPM (MXD7, 2), Index (TMP3, 69))
            If (And (SGFL, One))
            {
                Store (Add (ATNO,  One), ATNO)
                Store (Add (ATSZ, 0x05), ATSZ)
            }
            Return (TMP3)
        }
        Return (Zero) //End of ATPX
    }

    Method (ADPM, 2, Serialized)
    {
        Store (Zero, Local1)
        ShiftRight (Arg0, 0x10, Local0)
        If (LEqual (Arg1, One))
        {
            Or (And (Local0, One), Local1, Local1)
        }
        Else
        {
            Or (ShiftRight (And (Local0, 0x02), One), Local1, Local1)
        }
        ShiftLeft (Local1, One, Local1)
        ShiftRight (Arg0, 0x18, Local0)
        If (LEqual (Arg1, One))
        {
            Or (And (Local0, One), Local1, Local1)
        }
        Else
        {
            Or (ShiftRight (And (Local0, 0x02), One), Local1, Local1)
        }
        ShiftLeft (Local1, One, Local1)
        ShiftRight (Arg0, 0x08, Local0)
        If (LEqual (Arg1, One))
        {
            Or (And (Local0, One), Local1, Local1)
        }
        Else
        {
            Or (ShiftRight (And (Local0, 0x02), One), Local1, Local1)
        }
        Return (Local1)
    }

//
// ATI PowerXpress (PX) get ROM Method: ATRM
//
//   This is PowerXpress specific Control Method used by the discrete graphics driver on
//   PowerXpress enabled platforms to get a runtime modified copy of the discrete graphics
//   device ROM data (Video BIOS). The ATRM method definition is identical to the standard
//   ACPI _ROM method except that the ATRM method is defined in the _VGA namespace of the
//   integrated graphics device.
//
//   This function is required unless another method of exposing non-POSTed Video BIOS
//   image to the driver is supported by System BIOS: copying Video BIOS image to Video
//   Memory after Video BIOS obtained run-time parameters from System BIOS or when there
//   are no run-time parameters (Video BIOS image has hard-coded platform dependent data)
//   and Video BIOS ROM image can be accessed directly by the driver.
//
// Arguments:
//
//     Arg0: Offset of the graphics device ROM data.
//     Arg1: Size of the buffer to fill in (up to 4K).
//
// Output:
//
//     Buffer of bytes.
//
    Method (ATRM, 2, Serialized)
    {
        Store (Arg0, Local0)
        Store (Arg1, Local1)

        Name (VROM, Buffer(Local1) {0x00}) // Create 4K buffer to return to DD

        If (LGreater (Local1, 0x1000))
        {
            Store (0x1000, Local1) // Return dummy buffer if asking for more than 4K
        }

        If (LGreater (Arg0, DGPU_SCOPE.RVBS))
        {
            Return (VROM) // Return dummy buffer if asking beyond VBIOS image
        }

        Add (Arg0, Arg1, Local2)

        If (LGreater (Local2, DGPU_SCOPE.RVBS)) // If requested BASE+LEN > VBIOS image size
        {
            Subtract (DGPU_SCOPE.RVBS, Local0, Local1) // Limit length to the final chunk of VBIOS image
        }

        If (LLess (Local0, 0x8000))
        {
            Mid (DGPU_SCOPE.VBS1, Local0, Local1, VROM)
        }
        Else
        {
            Subtract (Local0, 0x8000, Local0)
            If (LLess (Local0, 0x8000))
            {
                Mid (DGPU_SCOPE.VBS2, Local0, Local1, VROM)
            }
        }

        Return (VROM)
    }
}
