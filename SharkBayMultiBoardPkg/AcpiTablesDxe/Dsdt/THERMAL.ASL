/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/
External(\_PR.APSV)
External(\_PR.ACRT)
External(\_PR.AAC0)

// THERMAL.ASL represents a Thermal Zone to be used for testing on the
// Customer Reference Boards.
Scope(\_TZ)
{

  // Notes:
  //  1) WIN2K strictly uses interrupt driven thermal events.
  //  2) Temperature values are stored in tenths of Kelvin to
  //    eliminate the decimal place.
  //  3) Kelvin = Celsius + 273.2.
  //  4) All temperature must be >= 289K.

  Name(ETMD, 1)

  // Temprature Threshold in Celsius, used to guard against hardware returning bad temprature readings
  Name(THLD, 120) 

  //
  // YBTx --  _ACx Thermal Trip Points for YellowBluff platform
  //
  // Matrix of Thermal Trip points & Fan Control (YellowBluff Platform)
  //        T('C)   PWM Duty Cycle
  Name(YBT4, 55)     //  35
  Name(YBT3, 64)     //  55
  Name(YBT2, 73)     //  75
  Name(YBT1, 80)     //  90
  Name(YBT0, 91)     // 100

  // Fan 0 = Package Processor Fan - Maximum speed
  PowerResource(FN00, 0, 0)
  {
    Method(_STA, 0, Serialized)
    { 
        // Return Virtual Fan 0 status.
        Return(VFN0)
    }

    // This method is called when the temprature goes above _AC0.
    // Regardless of other FAN states, set to ACF0 since this is max cooling state: temp > _AC0
    Method(_ON, 0, Serialized)
    {
        // Set Virtual Fan 0 On.
        Store(1,VFN0)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            \_SB.PCI0.LPCB.H_EC.ECWT(AC0F, RefOf(\_SB.PCI0.LPCB.H_EC.PENV))
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }

    // This method is called when the temprature goes below _AC0.
    // If FAN1 is on, use its value (AC1F): _AC0 > temp > _AC1
	// If FAN1 is off, use FAN2 value (AC2F): _AC0 > _AC1 > temp 
    Method(_OFF, 0, Serialized)
    {
        // Set Virtual Fan 0 Off.
        Store(0,VFN0)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LNotEqual(VFN1,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(AC1F, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC1F
            } 
            Else
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC2F = 0
            }
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }
  }

  // Associate Virtual Fan 0 Power Resource with the FAN0 Device.
  Device(FAN0)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID, 0)
    Name(_PR0, Package(1){FN00})
    Method(_DEP) // Fan method depends on EC Wrapper Op Region to be available
    {
      Return(Package() {\_SB.PCI0.LPCB.H_EC})
    }
  }

  // Fan 1 = Package Processor Fan.
  PowerResource(FN01,0,0)
  {
    Method(_STA,0,Serialized)
    { 
        // Return Virtual Fan 1 status.
        Return(VFN1)
    }

    // This method is called when the temprature goes above _AC1.
    // If FAN0 is on, do nothing since we're already at AC0F: temp > _AC0 > _AC1
    // If FAN0 is off, use FAN1 value (AC1F): _AC0 > temp > _AC1
    Method(_ON,0,Serialized)
    {
        // Set Virtual Fan 1 On.
        Store(1,VFN1)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LEqual(VFN0,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(AC1F, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC1F
                \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
            }
        }
    }

    // This method is called when the temprature goes below _AC1.
    // If FAN2 is on, use its value (AC2F): _AC1 > temp > _AC2
	// If FAN2 is off, use FAN3 value (AC3F): _AC1 > _AC2 > temp 
    Method(_OFF,0,Serialized)
    {
        // Set Virtual Fan 1 Off.
        Store(0,VFN1)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LNotEqual(VFN2,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC2F = 0
            } 
            Else
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC3F = 0
            }
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }
  }

  // Associate Virtual Fan 1 Power Resource with the FAN1 Device.
  Device(FAN1)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID, 1)
    Name(_PR0, Package(1){FN01})
    Method(_DEP) // Fan method depends on EC Wrapper Op Region to be available
    {
      Return(Package() {\_SB.PCI0.LPCB.H_EC})
    }
  }


  // Fan 2 = Package Processor Fan.
  PowerResource(FN02,0,0)
  {
    Method(_STA,0,Serialized)
    { 
        // Return Virtual Fan 2 status.
        Return(VFN2)
    }

    // This method is called when the temprature goes above _AC2.
    // If FAN1 is on, do nothing since we're already at AC1F or greater: temp > _AC1 > _AC2
	// If FAN1 is off, use FAN2 value (AC2F): _AC1 > temp > _AC2
    Method(_ON,0,Serialized)
    {
        // Set Virtual Fan 2 On.
        Store(1,VFN2)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LEqual(VFN1,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC2F = 0
                \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
            }
        }
    }

    // This method is called when the temprature goes below _AC2.
    // If FAN3 is on, use its value (AC3F): _AC2 > temp > _AC3
	// If FAN3 is off, use FAN4 value (AC4F): _AC2 > _AC3 > temp 
    Method(_OFF,0,Serialized)
    {
        // Set Virtual Fan 2 Off.
        Store(0,VFN2)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LNotEqual(VFN3,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC3F = 0
            } 
            Else
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC4F = 0
            }
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }
  }

  // Associate Virtual Fan 2 Power Resource with the FAN0 Device.
  Device(FAN2)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID, 2)
    Name(_PR0, Package(1){FN02})
    Method(_DEP) // Fan method depends on EC Wrapper Op Region to be available
    {
      Return(Package() {\_SB.PCI0.LPCB.H_EC})
    }
  }

  // Fan 3 = Package Processor Fan.
  PowerResource(FN03,0,0)
  {
    Method(_STA,0,Serialized)
    { 
        // Return Virtual Fan 3 status.
        Return(VFN3)
    }

    // This method is called when the temprature goes above _AC3.
    // If FAN2 is on, do nothing since we're already at AC2F or greater: temp > _AC2 > _AC3
	// If FAN2 is off, use FAN3 value (AC3F): _AC2 > temp > _AC3
    Method(_ON,0,Serialized)
    {
        // Set Virtual Fan 3 On.
        Store(1,VFN3)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LEqual(VFN2,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC3F = 0
                \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
            }
        }
    }

    // This method is called when the temprature goes below _AC3.
    // If FAN4 is on, use its value (AC4F): _AC3 > temp > _AC4
	// If FAN4 is off, use FAN5 value (AC5F): _AC3 > _AC4 > temp 
    Method(_OFF,0,Serialized)
    {
        // Set Virtual Fan 3 Off.
        Store(0,VFN3)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LNotEqual(VFN4,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC4F = 0
            } 
            Else
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC5F = 0
            }
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }
  }

  // Associate Virtual Fan 3 Power Resource with the FAN3 Device.
  Device(FAN3)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID, 3)
    Name(_PR0, Package(1){FN03})
    Method(_DEP) // Fan method depends on EC Wrapper Op Region to be available
    {
      Return(Package() {\_SB.PCI0.LPCB.H_EC})
    }
  }

  // Fan 4 = Package Processor Fan - Lowest Fan Speed
  PowerResource(FN04,0,0)
  {
    Method(_STA,0,Serialized)
    { 
        // Return Virtual Fan 4 status.
        Return(VFN4)
    }

    // This method is called when the temprature goes above _AC4.
    // If FAN3 is on, do nothing since we're already at AC3F or greater: temp > _AC3 > _AC4
	// If FAN3 is off, use FAN4 value (AC4F): _AC3 > temp > _AC4
    Method(_ON,0,Serialized)
    {
        // Set Virtual Fan 4 On.
        Store(1,VFN4)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            If(LEqual(VFN3,0))
            {
                \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC4F = 0
                \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
            }
        }
    }

    // This method is called when the temprature goes below _AC4.
    // FAN4 is the lowest FAN state defined, so we simply go to AC4F
    Method(_OFF,0,Serialized)
    {
        // Set Virtual Fan 4 Off.
        Store(0,VFN4)

        // If EC access is enabled.
        If(LAnd(ECON, ETMD))
        {
            \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.PENV)) // AC4F = 0
            \_SB.PCI0.LPCB.H_EC.ECMD(0x1a)
        }
    }
  }

  // Associate Virtual Fan 4 Power Resource with the FAN4 Device.
  Device(FAN4)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID, 4)
    Name(_PR0, Package(1){FN04})
    Method(_DEP) // Fan method depends on EC Wrapper Op Region to be available
    {
      Return(Package() {\_SB.PCI0.LPCB.H_EC})
    }
  }

  // Thermal Zone 0 = Package Thermal Zone.
  // Package Thermal Zone is used for Active and Critical Policy Control
  // Package Thermal Zone returns the maximum temperature 
  // of all components within the package
  ThermalZone(TZ00)
  {
    // Temporary variable for holding the current temprature reading
    Name(PTMP,3000)

    // Notifies ASL Code the current cooling mode.
    //  0 - Active cooling
    //  1 - Passive cooling
    Method(_SCP, 1, Serialized)
    {
        Store(Arg0, CTYP)
    }

    // Return the temperature at which the OS performs Critical Shutdown
    Method(_CRT, 0, Serialized)
    {         
        // Returns automatic thermal reporting temperature for CPU throttling if available and valid.
        If(CondRefOf(\_PR.ACRT))
        {
            If(LNotEqual(\_PR.ACRT,0))
            {
                Return(Add(2732, Multiply(\_PR.ACRT, 10)))
            }
        }

        Return(Add(2732, Multiply(CRTT, 10)))
    }

    // Return the temperature(s) at which the OS initiates Active Cooling.
    Method(_AC0, 0, Serialized)
    {
        // Returns automatic thermal reporting temperature for CPU throttling if available and valid.
        If(CondRefOf(\_PR.AAC0))
        {
            If(LNotEqual(\_PR.AAC0,0))
            {
                Return(Add(2732, Multiply(\_PR.AAC0, 10)))
            }
        }
        Return(Add(2732, Multiply(ACTT, 10)))
        
    }

    Method(_AC1, 0, Serialized)
    {
        Return(Add(2732, Multiply(ACT1, 10)))
    }

    Method(_AC2, 0, Serialized)
    {
        Return(Add(2732, Multiply(0, 10)))  
       
    }

//[-start-130729-IB11980003-remove]//
//    Method(_AC3, 0, Serialized)
//    {
//        Return(Add(2732, Multiply(0, 10)))  
//        
//    }
//
//    Method(_AC4, 0, Serialized)
//    {
//        Return(Add(2732, Multiply(0, 10)))  
//        
//    }
//[-end-130729-IB11980003-remove]//

    // Return the device(s) to turn on when _ACx is exceeded.
    Name(_AL0, Package(){FAN0})
    Name(_AL1, Package(){FAN1})
    Name(_AL2, Package(){FAN2})
//[-start-130730-IB11980003-remove]//
//    Name(_AL3, Package(){FAN3})
//    Name(_AL4, Package(){FAN4})
//[-end-130730-IB11980003-remove]//

    // Return the Package Temperature.
    //    Source 1: CPU DTS temperature  
    //    Source 2: Max Platform temprature returned by EC 
    Method(_TMP, 0, Serialized)
    {

        If (LNot(ETMD))    // If Legacy TM is disabled, return static value
        {
            Return (3000)
        }

        If (LEqual(DTSE, 0x3)) //Out Of Spec Condition occurred. Return critical temperature for OS shutdown
        {
            Return(Add(2832,Multiply(CRTT,10)))
        }

        // Source 1: If DTS SMM enabled 
        // 
        // If Package Temp MSR is available, then send Package DTS Temp
        // otherwise send Core DTS Temp  
        If(LEqual(DTSE, 0x01))    
        {
            If(LEqual(PKGA, 0x01))
            {
            Store(PDTS, Local0)
            Return(Add(2732, Multiply(Local0, 10)))
            } 
            //
            // Package DTS not support. Report MAX temperature between all cores to OS.
            //
            Store(DTS1,Local0)
            If(LGreater(DTS2, Local0))
            {
                Store(DTS2,Local0)
            } 
            If(LGreater(DTS3, Local0))
            {
                Store(DTS3,Local0)
            } 
            If(LGreater(DTS4, Local0))
            {
                Store(DTS4,Local0)
            } 
            Return(Add(2732, Multiply(Local0, 10)))
        }

        // Source 2: Max Platform temprature returned by EC         
        // If EC enabled/avaialable 
        If(ECON)
        {  
          // Store current reading in temporary variable  
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PLMX)), Local0) // Max Platform temprature                                   
          Add(2732, Multiply(Local0, 10), Local0 )
          Store(Local0, PTMP) 
          Return(Local0)
        }
        

        // Return a static value if no source is available.
        Return(3010)
    }
  }

  // Thermal Zone 1 = CPU Thermal Zone.
  // CPU Thermal Zone is used for Passive Policy Control
  // Since Passive Control implies reducing CPU IA core performance states,
  // Only CPU's temperature must be used for that purpose.
  ThermalZone(TZ01)
  {

    // Temporary variable for holding the current temprature reading
    Name(PTMP,3000)

    // Notifies ASL Code the current cooling mode.
    //  0 - Active cooling
    //  1 - Passive cooling
    Method(_SCP, 1, Serialized)
    {
        Store(Arg0,CTYP)
    }

    // Return the temperature at which the OS performs Critical Shutdown
    Method(_CRT, 0, Serialized)
    {          
        // Returns automatic thermal reporting temperature for CPU throttling if available and valid.
        If(CondRefOf(\_PR.ACRT))
        {
            If(LNotEqual(\_PR.ACRT,0))
            {
                Return(Add(2732, Multiply(\_PR.ACRT, 10)))
            }
        }

        Return(Add(2732, Multiply(CRTT, 10)))
    }

    // Return the CPU Temperature to the OS.
    // CPU Temperature is the maximum temperature of all CPU IA cores
    // Source 1: CPU temprature reported by DTS SMM
    // Source 2: CPU temprature reported by EC via PECI
    Method(_TMP,0,Serialized)
    {

        If (LNot(ETMD))    // If Legacy TM is disabled, return static value
        {
            Return (3020)
        }

        If (LEqual(DTSE, 0x3)) //Out Of Spec Condition occurred. Return critical temperature for OS shutdown
        {
            Return(Add(2832,Multiply(CRTT,10)))
        }

        // Source 1: If DTS SMM enabled
        // If Package Temp MSR is available, then send Package DTS Temp
        // otherwise send Core DTS Temp  
        If(LEqual(DTSE, 0x01))    
        {
            If(LEqual(PKGA, 0x01))
            {
            Store(PDTS, Local0)
            Return(Add(2732, Multiply(Local0, 10)))
            } 
            //
            // Package DTS not support. Report MAX temperature between all cores to OS.
            //
            Store(DTS1,Local0)
            If(LGreater(DTS2, Local0))
            {
                Store(DTS2,Local0)
            } 
            If(LGreater(DTS3, Local0))
            {
                Store(DTS3,Local0)
            } 
            If(LGreater(DTS4, Local0))
            {
                Store(DTS4,Local0)
            } 
            Return(Add(2732, Multiply(Local0, 10)))
         }
        
        // If EC avialable/enabled
        If(ECON)
        {
 
          // Source 2: CPU temprature reported by EC via PECI
          // Store current Integer part of reading in temporary variable  
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECH)), Local0)
          Multiply(Local0, 10, Local0) 

          // Fractional part of temprature 
          // Fractional part is in 1/64C, Fractional value = (PECL >> 2 ) * 1/64  
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECL)), Local1)
          ShiftRight(Local1, 2, Local1)
          Store(Divide( Multiply(Local1, 10), 64), Local1)
          
          // Add fractional part to decimal part 
          Add( Local0, Local1, Local0)
          
          Add(2732, Local0, Local0 )
          Store(Local0, PTMP) 
          Return(Local0)
        }


        // Return a static value if no source is available.
        Return(3030)
    }

    // Return the Processor(s) used for Passive Cooling.
    Method(_PSL, 0, Serialized)
    {
        If(LEqual(TCNT, 8))
        {
            //  CMP - Throttling controls eight logical CPUs.
            Return(Package(){\_PR.CPU0,\_PR.CPU1,\_PR.CPU2,\_PR.CPU3,\_PR.CPU4,\_PR.CPU5,\_PR.CPU6,\_PR.CPU7})
        }
        
        If(LEqual(TCNT, 4))
        {
            //  CMP - Throttling controls four logical CPUs.
            Return(Package(){\_PR.CPU0,\_PR.CPU1,\_PR.CPU2,\_PR.CPU3})
        }

        If(LEqual(TCNT, 2))
        {
            //  CMP - Throttling controls both CPUs.
            Return(Package(){\_PR.CPU0,\_PR.CPU1})
        }

        Return(Package(){\_PR.CPU0})
    }
        
    // Returns the temperature at which the OS initiates CPU throttling.
    Method(_PSV, 0, Serialized)
    {
        // Returns automatic thermal reporting temperature for CPU throttling if available and valid.
        If(CondRefOf(\_PR.APSV))
        {
            If(LNotEqual(\_PR.APSV,0))
            {
                Return(Add(2732, Multiply(\_PR.APSV, 10)))
            }
        }

        Return(Add(2732, Multiply(PSVT, 10)))
    }

    // Returns TC1 value used in the passive cooling formula.
    Method(_TC1, 0, Serialized)
    {
        Return(TC1V)
    }
    
    // Returns TC2 value used in the passive cooling formula.
    Method(_TC2, 0, Serialized)
    {
        Return(TC2V)
    } 

    // Returns the sampling period used in the passive cooling formula.
    Method(_TSP, 0, Serialized)
    {
        Return(TSPV)
    }
  }
}
