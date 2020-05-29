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

  TSknParticipant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

Scope(\_SB.PCI0.LPCB.H_EC)
{

  Device(TSKN)  // Skin temperature sensor
  {
    Name(_HID, EISAID("INT3403"))  // Intel DPTF Temperature Sensor Device
    Name(_UID, 0)

    Name(HITP, 0x64)
    Name(SAUX, 0x10)
    Name(_STR, Unicode ("Skin Temperature Sensor"))
    Name(PTYP, 0x03)

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(SKDE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }
    // _TMP (Temperature)
    //
    // This control method returns the thermal zone's current operating temperature.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP,0,Serialized)
    {
      If(\ECON)
      { // Get the Skin Temperature.
        Return(\_SB.IETM.CTOK(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.SKNT)))) 
      } Else {
        Return(3000)
      }
    }

    // Number of Aux Trips available
    Name(PATC, 2)         

    // PATx (Participant Programmable Auxiliary Trip) - Sets Aux Trip Point
    //
    // The PATx objects shall take a single integer parameter, in tenths of degree Kelvin, which
    // represents the temperature at which the device should notify the participant driver of
    // an auxiliary trip event. A PATx control method returns no value.
    //
    //  Arguments: (1)
    //    Arg0 - temperature in tenths of degree Kelvin
    //  Return Value:
    //    None
    //
    Method(PAT0, 1, Serialized)
    {
      If (\ECON)
      {
        Store (Acquire(\_SB.PCI0.LPCB.H_EC.PATM, 100),Local0)  // save Acquire result so we can check for Mutex acquired
        If (LEqual(Local0, Zero))  // check for Mutex acquired
        {
          Store(\_SB.IETM.KTOC(Arg0),SAUX) 
          \_SB.PCI0.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PCI0.LPCB.H_EC.TSI))
          \_SB.PCI0.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PCI0.LPCB.H_EC.HYST))
          \_SB.PCI0.LPCB.H_EC.ECWT(SAUX, RefOf(\_SB.PCI0.LPCB.H_EC.TSLT))
          \_SB.PCI0.LPCB.H_EC.ECMD(0x4A)                            // Set Trip point
          Release(\_SB.PCI0.LPCB.H_EC.PATM)
        }
      }
    }

    // PATx (Participant Programmable Auxiliary Trip) - Sets Aux Trip Point
    //
    // The PATx objects shall take a single integer parameter, in tenths of degree Kelvin, which
    // represents the temperature at which the device should notify the participant driver of
    // an auxiliary trip event. A PATx control method returns no value.
    //
    //  Arguments: (1)
    //    Arg0 - temperature in tenths of degree Kelvin
    //  Return Value:
    //    None
    //
    Method(PAT1, 1, Serialized)
    {
      If (\ECON)
      {
        Store (Acquire(\_SB.PCI0.LPCB.H_EC.PATM, 100),Local0)  // save Acquire result so we can check for Mutex acquired
        If (LEqual(Local0, Zero))  // check for Mutex acquired
        {
          Store(\_SB.IETM.KTOC(Arg0),HITP) 
          \_SB.PCI0.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PCI0.LPCB.H_EC.TSI))
          \_SB.PCI0.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PCI0.LPCB.H_EC.HYST))
          // Set High Trip to PSV or ACT 
          \_SB.PCI0.LPCB.H_EC.ECWT(HITP, RefOf(\_SB.PCI0.LPCB.H_EC.TSHT))  
          \_SB.PCI0.LPCB.H_EC.ECMD(0x4A)            //Set Trip point. 
          Release(\_SB.PCI0.LPCB.H_EC.PATM)
        }
      }
    }

    // Thermal Sensor Hysteresis, 2 degrees
    Name(GTSH, 20)

    Name(LSTM,0)  // Last temperature reported

    // _DTI (Device Temperature Indication)
    //
    // Conveys the temperature of a device's internal temperature sensor to the platform when a temperature trip point
    // is crossed or when a meaningful temperature change occurs.
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the current value of the temperature sensor (in tenths Kelvin)
    // Return Value:
    //   None
    //
    Method(_DTI, 1)
    {
      Store(Arg0,LSTM)
      Notify(TSKN, 0x91) // notify the participant of a trip point change event
    }

    // _NTT (Notification Temperature Threshold)
    //
    // Returns the temperature change threshold for devices containing native temperature sensors to cause
    // evaluation of the _DTI object 
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the temperature threshold in tenths of degrees Kelvin.
    //
    Method(_NTT, 0)
    {
      Return(2782)  // 5 degree Celcius, this could be a platform policy with setup item
    }

  } // End TSKN Device
}// end Scope(\_SB.PCI0.LPCB.H_EC)
