//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  TcpuDppmPolicy.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

Scope(\_SB.TCPU)
{
    Name(CTYP,0)  // Device-specific cooling policy preference
    
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
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local1) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local1) // Active Cooling Policy
      }
      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,Multiply(2, 10)))
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
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local0) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local0) // Active Cooling Policy
      }
      Subtract(Local0, 50, Local0)

      If(LGreaterEqual(LSTM,Local0))
      {
        Return (Subtract(Local0, Multiply(2, 10)))
      }
      Else
      {
        Return(Local0)
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
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local0) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local0) // Active Cooling Policy
      }
      Subtract(Local0, 100, Local0)
      If(LGreaterEqual(LSTM,Local0))
      {
        Return (Subtract(Local0, Multiply(2, 10)))
      }
      Else
      {
        Return(Local0)
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
    Method(_AC3)
    {
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local0) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local0) // Active Cooling Policy
      }
      Subtract(Local0, 150, Local0)
      If(LGreaterEqual(LSTM,Local0))
      {
        Return (Subtract(Local0, Multiply(2, 10)))
      }
      Else
      {
        Return(Local0)
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
    Method(_AC4)
    {
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local0) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local0) // Active Cooling Policy
      }
      Subtract(Local0, 200, Local0)
      If(LGreaterEqual(LSTM,Local0))
      {
        Return (Subtract(Local0, Multiply(2, 10)))
      }
      Else
      {
        Return(Local0)
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
    Method(_AC5)
    {
      If(CTYP)
      { 
        Store(\_SB.IETM.CTOK(PSVT),Local0) // Passive Cooling Policy
      }
      Else
      {
        Store(\_SB.IETM.CTOK(ACTT),Local0) // Active Cooling Policy
      }
      Subtract(Local0, 250, Local0)
      If(LGreaterEqual(LSTM,Local0))
      {
        Return (Subtract(Local0, Multiply(2, 10)))
      }
      Else
      {
        Return(Local0)
      }
    }
    
    // _CRT (Critical Temperature)
    //
    // This object, when defined under a thermal zone, returns the critical temperature at which OSPM must shutdown the system.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CRT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(CRTT)) 
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
      Return(\_SB.IETM.CTOK(Subtract(CRTT, 3))) 
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
    Method(_PSV,0,Serialized)
    {
      If(CTYP)
      {  
        Return(\_SB.IETM.CTOK(ACTT))  // Passive Cooling Policy
      } Else {
        Return(\_SB.IETM.CTOK(PSVT))  // Active Cooling Policy
      }
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
//[-start-130618-IB08050213-modify]//
        P8XH(0, Arg1, 1)
        P8XH(1, Arg2, 1)
//[-end-130618-IB08050213-modify]//
        Notify(\_SB.TCPU, 0x91)  
      }
    }

} // End Scope(\_SB.TCPU)

