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

Method (HGAS, 0, Serialized)
{
    //
    // HG Notification to do the GFX adapter switch
    //
//    Notify (\_SB.PCI0.GFX0,0xD0)
}

Method (HBRT, 1 , Serialized)
{
    //
    // Send backlight notifications to the DGPU LFP device. 
    // This is required for Win7 and is backward compatible with Vista.
    //
    If (LEqual (Arg0, 4))
    {  
        // Note:EDP OS Fn+F4, Fn+F5 brightness up and down	 
        If (Lequal (EDPE, One)) 
        {
            Notify (DGPU_SCOPE.DSP1, 0x87) // bugbug: hardcoding the EDP device as DSP1
        }
        Else
        {
            Notify (DGPU_SCOPE.LCD0, 0x87) // bugbug: hardcoding the LFP device as LCD0
        }
        If (CondRefOf (DGPU2_SCOPE.LCD0))
        {
            If (LEqual (DGPU_SCOPE.SLVS, 1))
            {
                Notify (DGPU2_SCOPE.LCD0, 0x87) // bugbug: hardcoding the LFP device as LCD0
            }
        }
    }
    If (LEqual (Arg0, 3))
    {
        // Note:EDP OS Fn+F4, Fn+F5 brightness up and down	
        If (Lequal (EDPE, One)) 
        {
            Notify (DGPU_SCOPE.DSP1, 0x86)
        }
        Else
        {
            Notify (DGPU_SCOPE.LCD0, 0x86)
        }
        If (CondRefOf (DGPU2_SCOPE.LCD0))
        {
            If (LEqual (DGPU_SCOPE.SLVS, 1))
            {
                Notify (DGPU2_SCOPE.LCD0, 0x86) // bugbug: hardcoding the LFP device as LCD0
            }
        }
    }
}

Method (HLSE, 0, Serialized)
{
    //
    // HG Handling of Lid Switch Event
    //
}

Method (HDSE, 1, Serialized)
{
    //
    // HG Handling of Display Switch Event
    //
    If (LEqual (Arg0, 0)) 
    {
        Notify (DGPU_SCOPE, 0xF0) // Mirror Notify on DGPU
        Return (DGPU_SCOPE.MADL)
    }
    Else
    {
        Store (1, DGPU_SCOPE.DSFL)
        Store (Arg0, DGPU_SCOPE.CHOI)
        Notify (DGPU_SCOPE, 0x80) // Mirror Notify on DGPU
    }
}

Method (HPFS, 0, Serialized) //Panel Fitting Hot Key
{
    //
    // HG Handling of Panel Fitting Switch
    //
}

Method (HGLS, 0, Serialized)
{
}

Method (FPED)
{
}

Method (PDCC) //Display Configuration(Adapter) Change
{
    //Stateless button/Hotkey supporting 3 states - Power Saver, Adaptive and Perf
}

Method (WMAB, 1, Serialized)
{
}
