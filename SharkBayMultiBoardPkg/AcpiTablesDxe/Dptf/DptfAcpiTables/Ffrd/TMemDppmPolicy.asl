/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  TMemDppmPolicy.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

Scope(\_SB.PCI0.TMEM)
{ 
    Name(CTYP,0)  // Device-specific cooling policy preference
    Name(AHYS,50) // Active Cooling Hysteresis configuration. Use this in conjuction with GTSH to define _ACx hysteresis points. (in 1/10 of K)
    Name(AC1D,30) // _AC1 Delta from _AC0  (in 1/10 of K)
    Name(AC2D,60) // _AC2 Delta from _AC0  (in 1/10 of K)

    // _CRT (Critical Temperature)
    //
    // This object, when defined under a thermal zone, returns the critical temperature at which OSPM must shutdown the system.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CRT)
    { 
      Return(\_SB.IETM.CTOK(MEMC)) 
    }
    
    // _HOT (Hot Temperature)
    //
    // This optional object, when defined under a thermal zone, returns the critical temperature 
    //  at which OSPM may choose to transition the system into the S4 sleeping state.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    The return value is an integer that represents the critical sleep threshold tenths of degrees Kelvin.
    //
    Method(_HOT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(MEMH)) 
    }
    
    // _PSV (Passive)
    //
    // This optional object, if present under a thermal zone, evaluates to the temperature 
    //  at which OSPM must activate passive cooling policy.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the passive cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_PSV)
    {
      Return(\_SB.IETM.CTOK(PTRA))
    }
    
    // _SCP (Set Cooling Policy)
    //
    //  Arguments: (3)
    //    Arg0 - Mode An Integer containing the cooling mode policy code
    //    Arg1 - AcousticLimit An Integer containing the acoustic limit
    //    Arg2 - PowerLimit An Integer containing the power limit
    //  Return Value:
    //    None
    //
    //  Argument Information:
    //    Mode - 0 = Active, 1 = Passive
    //    Acoustic Limit - Specifies the maximum acceptable acoustic level that active cooling devices may generate. 
    //    Values are 1 to 5 where 1 means no acoustic tolerance and 5 means maximum acoustic tolerance.
    //    Power Limit - Specifies the maximum acceptable power level that active cooling devices may consume. 
    //    Values are from 1 to 5 where 1 means no power may be used to cool and 5 means maximum power may be used to cool.
    //
    Method(_SCP, 3, Serialized)
    {
      If(LOr(LEqual(Arg0,0),LEqual(Arg0,1)))
      {
        Store(Arg0, CTYP)
        P8XH(0, Arg1)
        P8XH(1, Arg2)
      }
    }

    // _ACx (Active Cooling)
    //
    // This optional object, if present under a thermal zone, returns the 
    //  temperature trip point at which OSPM must start or stop Active cooling, 
    //  where x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC0)
    {
      Store(\_SB.IETM.CTOK(ATRA),Local1)
      
      If(LGreaterEqual(LSTM,Local1))
      {
        Subtract(AHYS, GTSH, Local2)
        Subtract(Local1, Local2, Local3)    // Include Hysteresis
        Return(Local3)
      }
      Else
      {
        Return(Local1)
      }
    }
    
    // _ACx (Active Cooling)
    //
    // This optional object, if present under a thermal zone, returns the 
    //  temperature trip point at which OSPM must start or stop Active cooling, 
    //  where x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC1)
    {
      Store(\_SB.IETM.CTOK(ATRA),Local0) // Active Cooling Policy
      Subtract(Local0, AC1D, Local1)
      If(LGreaterEqual(LSTM,Local1))
      {
        Subtract(AHYS, GTSH, Local2)
        Subtract(Local1, Local2, Local3)    // Include Hysteresis
        Return(Local3)
      }
      Else
      {
        Return(Local1)
      }
    }

    // _ACx (Active Cooling)
    //
    // This optional object, if present under a thermal zone, returns the 
    //  temperature trip point at which OSPM must start or stop Active cooling, 
    //  where x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC2)
    {
      Store(\_SB.IETM.CTOK(ATRA),Local0) // Active Cooling Policy
      Subtract(Local0, AC2D, Local1)
      If(LGreaterEqual(LSTM,Local1))
      {
        Subtract(AHYS, GTSH, Local2)
        Subtract(Local1, Local2, Local3)    // Include Hysteresis
        Return(Local3)
      }
      Else
      {
        Return(Local1)
      }
    }
} // End Scope(\_SB.PCI0.TMEM)
