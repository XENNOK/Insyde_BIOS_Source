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
    //
    // GPS_EVENT_STATUS_CHANGE (0xC0) Notify GPS software of a change in system power/thermal budget
    //
    Name (GESC, 0)

    Method (GPS, 4, NotSerialized)
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
            // Function 0: GPS_FUNC_SUPPORT - Bit list of supported functions.
            //
            Case (GPS_FUNC_SUPPORT)
            {
                // Sub-Functions 0, 32, 33, 34, 35, 42 support
                Return (Buffer(0x08) {0x01, 0x00, 0x00, 0x00, 0x0F, 0x04, 0x00, 0x00})
            }
            //
            // Function 32: GPS_FUNC_PSHARESTATUS - Get system requested Power Steering settings.
            //
            Case (GPS_FUNC_PSHARESTATUS)
            {
                CreateBitField (Arg3, 24, NRIT) // New Requested IGP turbo state (vaild if bits 25 is set)
                CreateBitField (Arg3, 25, RITS) // Request new IGP turbo state (bit 24 is valid)
                CreateField (Arg3, 26, 3, PSTS) // Power Steering status (vaild if bits 29 is set)
                CreateBitField (Arg3, 29, NPSS) // Notify Power Steering status (bits 28:26 vaild)
                CreateBitField (Arg3, 30, PSES) // Power Steering enable status

                Name (GB32, Buffer(4) {0x00})
                CreateBitField (GB32,  0, SPLU) // system parameter limit update
                CreateBitField (GB32,  1, PSLS) // Power supply limit support
                CreateBitField (GB32, 24, CITS) // Current IGP turbo state
                CreateBitField (GB32, 30, PSER) // Power Steering enable request
                //
                // Request new IGP turbo state
                //
                If (RITS)
                {
                    If (NRIT)
                    {
                        //
                        // todo: IGP turbo enable, IGP turbo boost not ready yet
                        //
                        Store (1, CITS)
                    }
                    else
                    {
                        //
                        // todo: IGP turbo disable, IGP turbo boost not ready yet
                        //
                        Store (0, CITS)
                    }
                }
                //
                // Workaround: Always return IGP turbo boost enabled,
                // it because can't enable/disable or get IGP turbo boost state.
                //
                Store (1, CITS)
                Store (1, PSER) // if this machine support GPS
                If (GESC)
                {
                    //
                    // system requesting new limits (Power Steering will call GPS_FUNC_PSHAREPARAMS)
                    //
                    Store (1, SPLU)
                }
                
                Return (GB32)
            }
            //
            // Function 33: GPS_FUNC_GETPSS - Get _PPS object.
            //
            Case (GPS_FUNC_GETPSS)
            {
                Return (CPU0_SCOPE._PSS)
            }
            //
            // Function 34: GPS_FUNC_SETPPC - Set _PPS object.
            //
            Case (GPS_FUNC_SETPPC)
            {
                CreateByteField (Arg3, 0, NPPC) // New GPS _PPC limit
                Store (NPPC, CPU0_SCOPE._PPC)
                If (LEqual (TCNT, 8))
                {
                    Notify (CPU0_SCOPE, 0x80)
                    Notify (CPU1_SCOPE, 0x80)
                    Notify (CPU2_SCOPE, 0x80)
                    Notify (CPU3_SCOPE, 0x80)
                    Notify (CPU4_SCOPE, 0x80)
                    Notify (CPU5_SCOPE, 0x80)
                    Notify (CPU6_SCOPE, 0x80)
                    Notify (CPU7_SCOPE, 0x80)
                }
                If (LEqual (TCNT, 4))
                {
                    Notify (CPU0_SCOPE, 0x80)
                    Notify (CPU1_SCOPE, 0x80)
                    Notify (CPU2_SCOPE, 0x80)
                    Notify (CPU3_SCOPE, 0x80)
                }
                If (LEqual (TCNT, 2))
                {
                    Notify (CPU0_SCOPE, 0x80)
                    Notify (CPU1_SCOPE, 0x80)
                }
                Else
                {
                    Notify (CPU0_SCOPE, 0x80)
                }
                Return (STATUS_SUCCESS)
            }
            //
            // Function 35: GPS_FUNC_GETPPC - Get _PPC object.
            //
            Case (GPS_FUNC_GETPPC)
            {
                Return (CPU0_SCOPE._PPC) // Current GPS _PPC limit
            }
            //
            // Function 42: GPS_FUNC_PSHAREPARAMS - Get sensor information and capabilities.
            //
            Case (GPS_FUNC_PSHAREPARAMS)
            {
                CreateByteField (Arg3, 0, QUTP) // Query Type
                CreateBitField (Arg3,  8, GPUT) // GPU Temperature status bit
                CreateBitField (Arg3,  9, CPUT) // CPU Temperature (PDTS) status bit
                CreateBitField (Arg3, 10, FANS) // System fan speed status bit
                CreateBitField (Arg3, 11, SKIN) // System Skin Temperature status bit
                CreateBitField (Arg3, 12, CENG) // CPU energy conter status bit
                CreateBitField (Arg3, 13, SEN1) // Additional Temperature Sentor 1
                CreateBitField (Arg3, 14, SEN2) // Additional Temperature Sentor 2

                Name (GB42, Buffer(36) {0x00})
                CreateDWordField (GB42,  0, STSV) // Status value
                CreateDWordField (GB42,  4, VERS) // Structure version
                CreateDWordField (GB42,  8, TGPU) // GPU Temperature (in degrees Celsius)
                CreateDWordField (GB42, 12, PDTS) // CPU package Temperature (in degrees Celsius)
                CreateDWordField (GB42, 16, SFAN) // System fan speed (as a percentage of maximum fan speed)
                CreateDWordField (GB42, 20, SKNT) // Skin Temperature (in degrees Celsius)
                CreateDWordField (GB42, 24, CPUE) // CPU energy conter
                CreateDWordField (GB42, 28, TMP1) // Additional Temperature Sentor 1
                CreateDWordField (GB42, 32, TMP2) // Additional Temperature Sentor 2

                switch (QUTP)
                {
                    Case (0) // Query Type 0: Get current status
                    {
                        If (CPUT)
                        {
                            Store (0x00000200, STSV)
                            Or (STSV, QUTP, STSV)
                            Store (\_TZ.TZ01._TMP, PDTS)
                        }
                        Return (GB42)
                    }
                    Case (1) // Query Type 1: Get supported fields
                    {
                        Store (0x00000200, STSV)
                        Or (STSV, QUTP, STSV)
                        Store (1000, PDTS) // minimum supported sample interval: 1000ms
                        Return (GB42)
                    }
                    Case (2) // Query Type 2: Get Current opreting limits
                    {
                        Or (STSV, QUTP, STSV)
                        If (GPUT)
                        {
                            Or (STSV, 0x00000100, STSV)
                        }
                        Store (0x00000000, VERS)
                        Store (0x00000000, TGPU)
                        Store (0x00000000, PDTS)
                        Store (0x00000000, SFAN)
                        Store (0x00000000, SKNT)
                        Store (0x00000000, CPUE)
                        Store (0x00000000, TMP1)
                        Store (0x00000000, TMP2) 
                        //
                        // If Return bit is clear, the system doesn't currently wish to control this parameter,
                        // Power Steering will use its default behavior for this parameter
                        //
                        Return (GB42)          
                    }
                }
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
}
