//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
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


External(\_SB.IETM, DeviceObj)

External(TBAB)
External(RTIP)
External(HGAS, MethodObj)
External(HPFS, MethodObj)
External(HBRT, MethodObj)
External(CHPS, MethodObj)
External(UP1D)
External(UPRW)
External(SVRF, MethodObj)
External(\_SB.PCI0.CTCU, MethodObj)
External(\_SB.PCI0.CTCN, MethodObj)
External(\_SB.PCI0.CTCD, MethodObj)
External(\_SB.PCI0.GFX0.CLID)
External(\_SB.PCI0.GFX0.IUEH, MethodObj)

External(\_SB.PCI0.GFX0.IUER, DeviceObj)

#define POWER_BUTTON         0
#define WINDOWS_BUTTON       1
#define VOLUME_UP_BUTTON     2
#define VOLUME_DOWN_BUTTON   3
#define ROTATION_LOCK_BUTTON 4
#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

Device(H_EC)  // Hitachi Embedded Controller
{
  Name(_HID, EISAID("PNP0C09"))

  Name(_UID,1)

  Name(ECAV, Zero)   // OS Bug Checks if EC OpRegion accessed before Embedded Controller Driver loaded

  Method(_CRS,0)
  {
    Name(BFFR, ResourceTemplate()
    {
      IO(Decode16,0x62,0x62,0,1)  // DIN/DOUT
      IO(Decode16,0x66,0x66,0,1)  // CMD/STS
    })

    Return(BFFR)
  }

  Method(_STA)
  {
    If (LEqual(ECON,1)){ 
      Return(0x0F)
    }
    Return(0x00)
  }

  OperationRegion(ECF2,EmbeddedControl,0,0xFF)
  Field(ECF2,ByteAcc,Lock,Preserve)
  {
    Offset(1),
    PLMX, 8,      // 1       Max Platform temprature
    LTMP, 8,      // 2       Sensor Temperature
    RPWR, 1,      // 3.0     Real AC Power (AC Present = 1)
        , 2,      // 3.2:1   Reserved
    CFAN, 1,      // 3.3     CPU Fan (Fan On = 1)
        , 2,      // 3.5:4   Reserved
    LSTE, 1,      // 3.6     Lid State (Lid Open = 1)
        , 1,      // 3.7     Reserved
    SPTR, 8,      // 4       SMBus Protocol Register
    SSTS, 8,      // 5       SMBus Status Register
    SADR, 8,      // 6       SMBus Address Register
    SCMD, 8,      // 7       SMBus Command Register
    SBFR, 256,    // 8       SMBus Block Buffer
    SCNT, 8,      // 40      SMBus Block Count
    Offset(47),
    CTMP, 8,      // 47      EC Critical Temperature
    DOCK, 1,      // 48.0    Docking state (docked = 1)
    EJET, 1,      // 48.1    Eject switch state (Eject (Q-Switch off) = 1)
        , 1,      // 48.2    Reserved
    PBNS, 1,      // 48.3    Power Button State (0 = Pressed)
    VPWR, 1,      // 48.4    Virtual AC Power (AC Present = 1)
        , 3,      // 48.7:5  Reserved
    SCAN, 8,      // 49      Hot-Key Scan Code
    B1ST, 8,      // 50      Battery 1 Status
    B1CR, 8,      // 51      Battery 1 Current Rate
    B1CC, 8,      // 52      Battery 1 Current Capacity
    B1VT, 8,      // 53      Battery 1 Voltage
    B2ST, 8,      // 54      Battery 2 Status
    B2CR, 8,      // 55      Battery 2 Current Rate
    B2CC, 8,      // 56      Battery 2 Current Capacity
    B2VT, 8,      // 57      Battery 2 Voltage
    CMDR, 8,      // 58      KSC command register
    LUXL, 8,      // 59      LUX - Low Byte
    LUXH, 8,      // 60      LUX - High Byte
    ACH0, 8,      // 61      Channel 0 RAW Value
    ACH1, 8,      // 62      Channel 1 RAW Value
    Offset(65),
    PPSL, 8,      // 65      PWM Port Select low byte
    PPSH, 8,      // 66      PWM Port Select high byte
    PINV, 8,      // 67      PWM initial value
    PENV, 8,      // 68      PWM ending value
    PSTP, 8,      // 69      PWM ms delay between steps
    Offset(71),
    CPUP, 16,     // 71      CPU Power mW
    BPWR, 16,     // 73      Brick Power cW(100ths)
    PPWR, 16,     // 75      Platform Power cW(100ths)
    CVRT, 8,      // 77      CPU VR (IMVP) Temp
    FANT, 8,      // 78      Heat exchanger fan temp
    SKNT, 8,      // 79      Skin temp
    AMBT, 8,      // 80      Ambient temp
    Offset(82),   // 82-85   DPTF fields
    TSI,  4,      //         Hysteresis selection is global and meant for all four sensors.
                  //          # 0 = CPU VR (IMVP) temperature sensor
                  //          # 1 = Heat Exchanger temperature sensor
                  //          # 2 = Skin temperature sensor
                  //          # 3 = Ambient temperature sensor
                  //          # 4 = DIMM temperature sensor
                  //          # Above 4 invalid
    HYST, 4,      // 82.4-7  Hysteresis in deg C 
    TSHT, 8,      // 83      Thermal Sensor (N) high trip point
    TSLT, 8,      // 84      Thermal Sensor (N) low trip point
    TSSR, 8,      // 85      TSSR- thermal sensor status register:
                  //         TSSR bits defined:
                  //          BIT0:   CPU VR Temp Sensor Trip Flag 
                  //          BIT1:   Fan Temp Sensor Trip Flag
                  //          BIT2:   Skin Temp Sensor Trip Flag
                  //          BIT3:   Ambient Temp Sensor Trip Flag
                  //          BIT4:   DIMM Trip Point Flag
                  //          BIT[7:5] Reserved
    Offset(86),
    B1CM, 8,      // 86      Battery 1 Command (Place holder for now)
    B1DC, 16,     // 87      Battery 1 Design Capacity (mWh)
    B1RC, 16,     // 89      Battery 1 Remaining Capacity (mWh)
    B1FC, 16,     // 91      Battery 1 Full Charge Capacity (mWh)
    B1FV, 16,     // 93      Battery 1 Full Resolution Voltage (mV)
    B1DI, 16,     // 95      Battery 1 Full Resolution Discharge Current (mA)
    B1CI, 16,     // 97      Battery 1 Full Resolution Charge Current (mA)    
    B2RC, 16,     // 99      Battery 2 Remaining Capacity (mWh)
    B2FC, 16,     // 101     Battery 2 Full Charge Capacity (mWh)
    B2FV, 16,     // 103     Battery 2 Full Resolution Voltage (mV)
    B2DI, 16,     // 105     Battery 2 Full Resolution Discharge Current (mA)
    B2CI, 16,     // 107     Battery 2 Full Resolution Charge Current (mA)
    CPAP, 16,     // 109     CPU average power in mW
    BKAP, 16,     // 111     Brick average power in cW(0.01)
    PLAP, 16,     // 113     Platform average power in cW(0.01)
    CFSP, 16,     // 115     CPU Fan speed in rpm 
    BRTV, 8,      // 117     Brightness value for San Rafael second panel 
       ,  8,      // 118     reserved
    SLPC, 8,      // 119     Select Performance Counters
    DLED, 1,      // 120.0   EC Connected Standby Debug LED (CAPS LOCK)
    PB10, 1,      // 120.1   10 Second Power Button Control 
    Offset(124),
    PTIM, 8,      // 124     CPU,GFX pwr sample timer
    DHPD, 8,      // 125     Digital display hot-plug detect
    DIM0, 8,      // 126     DIMM0 temperature
    DIM1, 8,      // 127     DIMM1 temperature     
    PMAX, 8,      // 128     CPU,MCH & PCH Max temp
    PPDT, 8,      // 129     PCH DTS reading from PCH
    PECL, 8,      // 130     CPU PECI reading fractional value (1/64 Celcius)
    PECH, 8,      // 131     CPU PECI reading integer value (unit Celicus)
    PMDT, 8,      // 132     MCH DTS reading from PCH
    TSD0, 8,      // 133     TS-on-DIMM0 temperature
    TSD1, 8,      // 134     TS-on-DIMM1 temperature
    TSD2, 8,      // 135     TS-on-DIMM2 temperature
    TSD3, 8,      // 136     TS-on-DIMM3 temperature
    PAKN, 8,      // 137     Packet sequence number
    CPUE, 32,     // 138     CPU energy 
    Offset(155),
    PRCL, 8,      // 155     Repeat cycle (LSB)
    PRC0, 8,      // 156     Repeat cycle 
    PRC1, 8,      // 157     Repeat cycle 
    PRCM, 8,      // 158     Repeat cycle (MSB)
    PRIN, 8,      // 159     Repear Intervel
    PSTE, 8,      // 160     Stop on Error 
    PCAD, 8,      // 161     Peci Clinet Addess 
    PEWL, 8,      // 162     Write Length 
    PWRL, 8,      // 163     Read lenght 
    PECD, 8,      // 164     Command Code 
    PEHI, 8,      // 165     Host ID 
    PECI, 8,      // 166     Index 
    PEPL, 8,      // 167     Parameter (LSB)
    PEPM, 8,      // 168     Parameter (MSB)
    PWFC, 8,      // 169     Write FCS 
    PECC, 8,      // 170     Completion code 
    PDT0, 8,      // 171     Data 0 
    PDT1, 8,      // 172     Data 1
    PDT2, 8,      // 173     Data 2 
    PDT3, 8,      // 174     Data 3 
    PRFC, 8,      // 175     Read FCS 
    PRS0, 8,      // 176     Reserved 
    PRS1, 8,      // 177     Reserved 
    PRS2, 8,      // 178     Reserved 
    PRS3, 8,      // 179     Reserved 
    PRS4, 8,      // 180     Reserved
    Offset(187),
    PRCS, 8,      // 187     Peci Repeat Command Status
    PEC0, 8,      // 188     Peci Error Count(LSB)
    PEC1, 8,      // 189     Peci Error Count
    PEC2, 8,      // 190     Peci Error Count
    PEC3, 8,      // 191     Peci Error Count(MSB)
    GTVR, 8,      // 192     CPU GT VR temp
    MCRT, 8,      // 193     Minicard Region Temp
    Offset(194),
    IBT1, 8,      // 194     ISCT Byte 1
                  //          194.0   Enable/Disable ISCT - SAOS bit
                  //          194.1   WLAN powered in S3 
                  //          194.2   WLAN powered in S4
                  //          194.3   WLAN powered in S5
                  //          194.4   WWAN powered in S3
                  //          194.5   WWAN powered in S4
                  //          194.6   WWAN powered in S5
                  //          194.7   Reserved
    IBT2, 8,      // 195     ISCT Byte 2
                  //          195.0   WLAN Wireless Status (0 = Disabled, 1 = Enabled)
                  //          195.1   WWAN Wireless Status (0 = Disabled, 1 = Enabled) 
                  //          195.2:7 Reserved
    WTMS, 8,      // 196     Wake timer Settings       
                  //          196.0   Timer Enabled in S3 
                  //          196.1   Timer Enabled in S4
                  //          196.2   Timer Enabled in S5
                  //          196.3:6 Reserved  
                  //          196.7   Enable/Disable EC Timer
    AWT2, 8,      // 197     ACPIWakeTmrByte2 - Wake timer value (BIT23-16)
    AWT1, 8,      // 198     ACPIWakeTmrByte1 - Wake timer value (BIT15-8)
    AWT0, 8,      // 199     ACPIWakeTmrByte0 - Wake timer value (BIT7-0)
    SPT2, 1,      // 200.0   SATA Por2 - Cable connect power control. 1=ON;0=OFF
    ,7,           // 200.7-1  Reserved
    Offset(211),
    B1DV, 16,     // 211     Battery 1 Design Voltage (mV)
    Offset(215),
    B1ML, 8,      // 215   BattAPmaxL - Battery Pack A maximum low byte
    B1MH, 8,      // 216   BattAPmaxH - Battery Pack A maximum high byte
    B2ML, 8,      // 217   BattBPmaxL - Battery Pack B maximum low byte
    B2MH, 8,      // 218   BattBPmaxH - Battery Pack B maximum high byte
    BTP1, 8,      // 219   Battery 1 Trip Point in %
  }


  // ECRD (Embedded Read Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - Object to Read
  //  Return Value:
  //    Read Value
  //    
  Method(ECRD,1,Serialized, 0, IntObj, FieldUnitObj)
  {
    If(LAnd(LEqual(\ECUP,Zero),LEqual(\_SB.RDGP(88),0)))
    {
      \_SB.WTGP(88,1)
      Sleep(16)
    }
    Store(DerefOf (Arg0), Local0)
    Return(Local0)
  }

  // ECWT (Embedded Write Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (2)
  //    Arg0 - Value to Write
  //    Arg1 - Object to Write to
  //    
  Method(ECWT,2,Serialized,,,{IntObj, FieldUnitObj})
  {
    If(LAnd(LEqual(\ECUP,Zero),LEqual(\_SB.RDGP(88),0)))
    {
      \_SB.WTGP(88,1)
      Sleep(16)
    }
    Store(Arg0,Arg1)
  }

  // ECMD (Embedded Controller Command)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - EC command
  //  Return Value:
  //    0x00 = Success
  //    0xFF = Failure
  //
  Method(ECMD,1,Serialized)
  {
    While(ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CMDR))){Stall(20)} // make sure no command is currently being processed so we don't overwrite it
    ECWT(Arg0, RefOf(\_SB.PCI0.LPCB.H_EC.CMDR))
    Return (0x00)
  }

  Include("ALS.ASL")


  // Virtual battery code
  //
  Include("Bat0Virt.asl")

  // Real battery code
  //
  Include("Bat1Real.asl")
  Include("Bat2Real.asl")

  // The _REG Method is needed because communication with the EC
  // before the driver is loaded is prohibited in WIN2000/WINXP.

  Method(_REG,2)
  {
    If(LAnd(LEqual(Arg0,3),LEqual(Arg1,1)))
    {
      // Must be running NT 5.0 OS.
      // Update ECAV Object. ASL should check for this value to be One before accessing EC OpRegion.
      Store(One, ECAV)

      // Turn off the CPU Fan if Active Cooling
      //   is disabled.

      If(LEqual(0,ACTT))
      {
        ECWT(0,RefOf(CFAN))
      }

      // Turn off pwm fan so it starts in a known state
      If(LEqual(1, THOF))
      {
        \_TZ.FN00._OFF() 
      }

      // Update the Ambiant Light Illuminence Values.

      Store(ECRD(RefOf(LUXH)),LHIH)
      Store(ECRD(RefOf(LUXL)),LLOW)

      If(LAnd(LEqual(ALSE,2),IGDS))
      {
        Store(ECRD(RefOf(LUXH)), Local0)     
        Or(ShiftLeft(Local0, 8), ECRD(RefOf(LUXL)), \_SB.PCI0.GFX0.ALSI)   
      }

      // Save the Lid State in global NVS and IGD OpRegion.
      //Store(LSTE,\_SB.PCI0.GFX0.CLID)
      If (LEqual(ECRD(RefOf(LSTE)), 0))
      {
        Store(0,\_SB.PCI0.GFX0.CLID)
      }
      If (LEqual(ECRD(RefOf(LSTE)), 1))
      {
        Store(3,\_SB.PCI0.GFX0.CLID) 
      }
      Store(ECRD(RefOf(LSTE)),LIDS)

      // Update the Dock Status
      Store(\DSTS,\_SB.PCI0.GFX0.CDCK)

      // Unconditionally fix up the Battery and Power State.

      // Initialize the Number of Present Batteries.
      //  1 = Real Battery 1 is present
      //  2 = Real Battery 2 is present
      //  3 = Real Battery 1 and 2 are present

      Store(0,BNUM)
      Or(BNUM,ShiftRight(And(ECRD(RefOf(B1ST)),0x08),3),BNUM)
      Or(BNUM,ShiftRight(And(ECRD(RefOf(B2ST)),0x08),2),BNUM)

      // Save the current Power State for later.

      Store(PWRS,Local0)

      // Initialize the Power State.
      //  BNUM = 0 = Virtual Power State
      //  BNUM > 0 = Real Power State

      If(LEqual(BNUM,0))
      {
        Store(ECRD(RefOf(VPWR)),PWRS)
      }
      Else
      {
        Store(ECRD(RefOf(RPWR)),PWRS)
      }

      // Perform needed ACPI Notifications.

      PNOT()

      //
      // Call Power button init method for 10sec PB OVR
      //
      ^VGBI.PBIN()
    }
  }

  Method(_GPE)
  {
    Store (23,  Local0)   // GPI7 for other boards

    If(LOr(LOr(LEqual(BID,BWT1), LEqual(BID,BW1P)), LOr(LEqual(BID,BW2C), LEqual(BID,BW2P)))) // BoardIdWhiteTipMountain1, BoardIdWhiteTipMountain1Ppv, BoardIdWhiteTipMountain2, BoardIdWhiteTipMountain2Ppv
    {
      Store(10, Local0)   // GPI10
    }

    If(LOr(LOr(LEqual(BID,BSPC), LEqual(BID,BSPP)), LEqual(BID,BRH))) // BoardIdSawToothPeak, BoardIdSawToothPeakPpv, BoardIdReedHarborTdv
    {
      Store(10, Local0)   // GPI10
    }

    return (Local0)
  }
    
  // For the below _Qxx Methods, The Runtime SCI has been asserted,
  // the EC Driver performed it's query, and the value returned from
  // the Query = xx.

  Method(_Q30)    // Real Battery AC Insertion Event.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x30,1)
//[-end-130405-IB04770276-modify]//

    // Set Global Power State = AC Mode.

    Store(1,PWRS)

    // Perform needed ACPI Notifications.

    PNOT()
  }

  Method(_Q31)    // Real Battery AC Removal Event.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x31,1)
//[-end-130405-IB04770276-modify]//

    // Set Global Power State = Battery Mode.

    Store(0,PWRS)

    // Perform needed ACPI Notifications.

    PNOT()
  }

  Method(_Q32)    // Real Battery Capacity Change.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x32,1)
//[-end-130405-IB04770276-modify]//

    // Perform needed ACPI Notifications.

    PNOT()
  }

  Method(_Q33)    // Real Battery Insertion/Removal Event.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x33,1)
//[-end-130405-IB04770276-modify]//

    // Initialize the Number of Present Batteries.
    //  1 = Real Battery 1 is present
    //  2 = Real Battery 2 is present
    //  3 = Real Battery 1 and 2 are present

    Store(0,BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B1ST)),0x08),3),BNUM)
    Or(BNUM,ShiftRight(And(ECRD(RefOf(B2ST)),0x08),2),BNUM)

    // Perform needed ACPI Notifications.

    PNOT()
  }
  
  Method(_Q40, 0, Serialized)   // Dock complete Event
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x40,1)
//[-end-130405-IB04770276-modify]//
    If(LEqual(\_SB.PCI0.HDEF.DCKS, 1)) 
    {
      If(LEqual(\_SB.PCI0.HDEF.DCKM, 0))
      {
        Store(1, \_SB.PCI0.HDEF.DCKA)
        Stall(50)
        While(LNotEqual(\_SB.PCI0.HDEF.DCKM, 1))
        {
          Stall(50)
        }
      }
    } 

    Sleep(1000) // Delay 1 second for hot docking stability
    Store(\PDBR, \SSMP)
    Store(1, \DSTS)
    Sleep(1000) // Delay 1 second for hot docking stability
    Notify(\_SB.PCI0.DOCK, 0x00)

    If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
    {
      Or(PB1E, 0x10, PB1E) // Set Dock status
      ^VGBI.UPBT(DOCK_INDICATOR, One)
      ADBG("Notify 0xCA")
      Notify(^VGBI, 0xCA) // Notify Virtual GPIO driver that Docking event happens.
    }

    If(IGDS)
    {
      \_SB.PCI0.GFX0.GDCK(1)
    }
  }

  Method(_Q41)    // Surprise Removal
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x41,1)
//[-end-130405-IB04770276-modify]//
    If(LEqual(\_SB.PCI0.HDEF.DCKS, 1)) 
    {
      If(LEqual(\_SB.PCI0.HDEF.DCKM, 1)) 
      {
        Store(0, \_SB.PCI0.HDEF.DCKA)
      }
    }
    Store(0, \DSTS)
    Notify(\_SB.PCI0.DOCK, 0x01)

    If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
    {
      And(PB1E, Not(0x10), PB1E) // Clear Dock Status
      ^VGBI.UPBT(DOCK_INDICATOR, Zero)
      ADBG("Notify 0xCB")
      Notify(^VGBI, 0xCB) // Notify Virtual GPIO driver that Undocking event happens.
    } Else {
      If(IGDS)
      {
        \_SB.PCI0.GFX0.GDCK(0)
      }
    }           
  }           
  
  Method(_Q42)    // Undocking request Event
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x42,1)
//[-end-130405-IB04770276-modify]//
    If(LAnd(\_SB.PCI0.HDEF.DCKS, 1)) 
    {
      Store(0, \_SB.PCI0.HDEF.DCKA)
    }
    Notify(\_SB.PCI0.DOCK, 0x01)

    If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
    {
      And(PB1E, Not(0x10), PB1E) // Clear Dock Status
      ^VGBI.UPBT(DOCK_INDICATOR, Zero)
      If(CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0) 
        And(Local0, Not(0x80), \_SB.PCI0.GFX0.IUER)
      }
      ADBG("Notify 0xCB")
      Notify(^VGBI, 0xCB) // Notify Virtual GPIO driver that Undocking event happens.
    } Else {
      If(IGDS)
      {
        \_SB.PCI0.GFX0.GDCK(0)
      }
    }
  }
  
  Method(_Q43)    // Express Card Presence Changed on Saddlestring (Docking)
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x43,1)
//[-end-130405-IB04770276-modify]//

    Notify(\_SB.PCI0.RP07, 0x0)
  }
  
  Method(_Q44)  // Virtual Dock Presence Changed
  {
    If(\DSTS)
    {
      Notify(\_SB.PCI0.DOCK, 1) //UnDocked
      Store(0, \DSTS)
    }
    Else
    {
      Notify(\_SB.PCI0.DOCK, 0) //Docked
      Store(1, \DSTS)
    }
  }

  Method(_Q51)    // Lid Switch Event.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x51,1)
//[-end-130405-IB04770276-modify]//

    // Update Lid NVS State.

    Store(ECRD(RefOf(LSTE)),LIDS)

    \_SB.PCI0.GFX0.GLID(LIDS)
    
    // If ETM is not disabled, notify IETM device
    If (LEqual(\_TZ.ETMD, 0))
    {
      If(CondRefOf(\_SB.IETM))
      {
        Notify(\_SB.IETM,0x83)
      }
    }
    Notify(\_SB.LID0,0x80)
  }


  Method(_Q52)    // Hot-Key Event.
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x52,1)
//[-end-130405-IB04770276-modify]//

    // This event will be generated whenever a
    // CRTL+ALT+SHIFT+"Next Key" is hit.  This event handler
    // will base code execution on the "Next Key" Scan Code
    // stored in the EC Memory Space.

    Store(ECRD(RefOf(SCAN)),Local2)
//[-start-130405-IB04770276-modify]//
    P8XH (1,Local2,1)
//[-end-130405-IB04770276-modify]//

    // "D" Scan Code = 0x20

    If(LEqual(SCAN,0x20))
    {
      ADBG("CAS+D")
      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        //
        // CAS+D test emulation mode
        // 1. Windows press notify 0xC2
        // 2. Volume up press notify 0xC4
        // 3. Volume up release notify 0xC5
        // 4. Windows release notify 0xC3
        //

        Sleep(1000) // 1sec delay is needed for Powerbutton and Windows Home button.

        ^VGBI.UPBT(WINDOWS_BUTTON, One) // Update VBDS
        ADBG("Notify 0xC2")
        Notify(^VGBI, 0xC2) // Notify Virtual GPIO driver that Windows Home button is pressed.

        ^VGBI.UPBT(VOLUME_UP_BUTTON, One)
        ADBG("Notify 0xC4")
        Notify(^VGBI, 0xC4) // Notify Virtual GPIO driver that Vol up button is pressed.

        ^VGBI.UPBT(VOLUME_UP_BUTTON, Zero)
        ADBG("Notify 0xC5")
        Notify(^VGBI, 0xC5) // Notify Virtual GPIO driver that Vol up button is released.

        ^VGBI.UPBT(WINDOWS_BUTTON, Zero)
        ADBG("Notify 0xC3")
        Notify(^VGBI, 0xC3) // Notify Virtual GPIO driver that Windows Home button is released.
      }
    }

    // "F" Scan Code = 0x21

    If(LEqual(SCAN,0x21))
    {
      ADBG("CAS+F")
      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        //
        // CAS+F test emulation mode
        // 1. Windows press notify 0xC2
        // 2. Volume down press notify 0xC6
        // 3. Volume down release notify 0xC7
        // 4. Windows release notify 0xC3
        //

        Sleep(1000) // 1sec delay is needed for Powerbutton and Windows Home button

        ^VGBI.UPBT(WINDOWS_BUTTON, One) // Update VBDS
        ADBG("Notify 0xC2")
        Notify(^VGBI, 0xC2) // Notify Virtual GPIO driver that Windows Home button is pressed.

        ^VGBI.UPBT(VOLUME_DOWN_BUTTON, One)
        ADBG("Notify 0xC6")
        Notify(^VGBI, 0xC6) // Notify Virtual GPIO driver that Vol down button is pressed.

        ^VGBI.UPBT(VOLUME_DOWN_BUTTON, Zero)
        ADBG("Notify 0xC7")
        Notify(^VGBI, 0xC7) // Notify Virtual GPIO driver that Vol down button is released.

        ^VGBI.UPBT(WINDOWS_BUTTON, Zero) // Update VBDS
        ADBG("Notify 0xC3")
        Notify(^VGBI, 0xC3) // Notify Virtual GPIO driver that Windows Home button is released.
      }
    }

    // Intel Ultrabook Event Handlers.

    // "G" Scan Code = 0x22

    If(LEqual(Local2,0x22))
    {
      ADBG("CAS+G")
      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        //
        // CAS+G test emulation mode
        // 1. Windows press notify 0xC2
        // 2. Powerbutton press notify 0xC0
        // 3. Powerbutton release notify 0xC1
        // 4. Windows release notify 0xC3
        //

        Sleep(1000) // 1sec delay is needed for Powerbutton and Windows Home button

        ^VGBI.UPBT(WINDOWS_BUTTON, One)
        ADBG("Notify 0xC2")
        Notify(^VGBI, 0xC2) // Notify Virtual GPIO driver that Windows Home button is pressed.
        If(And(PB1E, 1)) 
        {
          ^VGBI.UPBT(POWER_BUTTON, One)
          ADBG("Notify 0xC0")
          Notify(^VGBI, 0xC0) // Notify Virtual GPIO driver that Power button is pressed.

          ^VGBI.UPBT(POWER_BUTTON, Zero)
          ADBG("Notify 0xC1")
          Notify(^VGBI, 0xC1) // Notify Virtual GPIO driver that Power button is released.
        }

        ^VGBI.UPBT(WINDOWS_BUTTON, Zero)
        ADBG("Notify 0xC3")
        Notify(^VGBI, 0xC3) // Notify Virtual GPIO driver that Windows Home button is released.
      } Else {
        // Power Button.  If HID Event Driver loaded, use it. Otherwise, use GFX Driver.
        If(LEqual(\_SB.HIDD.HRDY, 1))
        {
          \_SB.HIDD.HPEM(9)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(0)
        }
      }
    }

    // "H" Scan Code = 0x23

    If(LEqual(Local2,0x23))
    {
      ADBG("CAS+H")
      // Windows Button.  If HID Event Driver loaded, use it.  Otherwise, use GFX Driver.
      Sleep(1000)

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        ^VGBI.UPBT(WINDOWS_BUTTON, One)
        ADBG("Notify 0xC2")
        Notify(^VGBI, 0xC2) // Notify Virtual GPIO driver that Windows Home button is pressed.
        ^VGBI.UPBT(WINDOWS_BUTTON, Zero)
        ADBG("Notify 0xC3")
        Notify(^VGBI, 0xC3) // Notify Virtual GPIO driver that Windows Home button is released.
      } Else {
        If(LEqual(\_SB.HIDD.HRDY, 1))
        {
          \_SB.HIDD.HPEM(1)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(1)
        }
      }
    }

    // "J" Scan Code = 0x24
    If(LEqual(Local2,0x24))
    {
      ADBG("CAS+J")
      // Volume Up.  If HID Event Driver loaded, use it.  Otherwise, use GFX Driver.

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        ^VGBI.UPBT(VOLUME_UP_BUTTON, One)
        ADBG("Notify 0xC4")
        Notify(^VGBI, 0xC4) // Notify Virtual GPIO driver that Volume UP button is pressed.

        ^VGBI.UPBT(VOLUME_UP_BUTTON, Zero)
        ADBG("Notify 0xC5")
        Notify(^VGBI, 0xC5) // Notify Virtual GPIO driver that Volume UP button is released.
      } Else {
        If(LEqual(\_SB.HIDD.HRDY, 1))
        {
          \_SB.HIDD.HPEM(17)
        } Else {
          ADBG("IUEH")
           \_SB.PCI0.GFX0.IUEH(2)
        }// End of If(LEqual(\_SB.HIDD.HRDY, 1))
      } // If(LAnd(CondRefOf(^VGBI._STA)),(LEqual(And(^VGBI._STA(),One),One)))
    }

    // "K" Scan Code = 0x25

    If(LEqual(Local2,0x25))
    {
      ADBG("CAS+K")
      // Volume Down.  If HID Event Driver loaded, use it. Otherwise, use GFX Driver.

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        ^VGBI.UPBT(VOLUME_DOWN_BUTTON, One)
        ADBG("Notify 0xC6")
        Notify(^VGBI, 0xC6) // Notify Virtual GPIO driver that Volue Down button is pressed.

        ^VGBI.UPBT(VOLUME_DOWN_BUTTON, Zero)
        ADBG("Notify 0xC7")
        Notify(^VGBI, 0xC7) // Notify Virtual GPIO driver that Volue Down button is released.
      } Else {
        If(LEqual(\_SB.HIDD.HRDY, 1))
        {
          \_SB.HIDD.HPEM(18)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(3)
        }
      }
    }

    // "L" Scan Code = 0x26

    If(LEqual(Local2,0x26))
    {
      ADBG("CAS+L")
       // Rotation Lock.  If HID Event Driver loaded, use it. Otherwise, use GFX Driver.
       Sleep(1000)

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        Xor(PB1E, 0x04, PB1E) // Toggle Rotation Lock Status (bit2 of PB1E)
        ^VGBI.UPBT(ROTATION_LOCK_BUTTON, One)
        ADBG("Notify 0xC8")
        Notify(^VGBI, 0xC8) // Notify Virtual GPIO driver that Rotation Lock button is pressed.

        ^VGBI.UPBT(ROTATION_LOCK_BUTTON, Zero)
        ADBG("Notify 0xC9")
        Notify(^VGBI, 0xC9) // Notify Virtual GPIO driver that Rotation Lock button is released.
      } Else {
        If(LEqual(\_SB.HIDD.HRDY, 1))
        {
           \_SB.HIDD.HPEM(2)
        } Else {
          ADBG("IUEH")
          \_SB.PCI0.GFX0.IUEH(4)
        }
      }
    }

    // ";" Scan Code = 0x27
    If(LEqual(Local2,0x27))
    {
       ADBG("CAS+;")

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        Xor(PB1E, 0x08, PB1E) // Toggle Slate/Laptop Lock Status
        If(And(PB1E, 0x08)) // Slave/Laptop Mode changed 0 -> 1 Laptop mode
        {
          ^VGBI.UPBT(CONVERTIBLE_BUTTON, One)
          ADBG("Notify 0xCD")
          Notify(^VGBI, 0xCD) // Notify Virtual GPIO driver that the system is now in Laptop mode.
        } Else { // Slave/Laptop Mode Changed 1 -> 0 Slate/Tablet Mode                              
          ^VGBI.UPBT(CONVERTIBLE_BUTTON, Zero)
          ADBG("Notify 0xCC")
          Notify(^VGBI, 0xCC) // Notify Virtual GPIO driver that the system is now in Slate/Tablet mode.
        }
      } Else {
        \_SB.PCI0.GFX0.IUEH(6) // Convertible Indicator lock
      }
    }

    // "'" Scan Code = 0x28
    If(LEqual(Local2,0x28))
    {
       ADBG("CAS+'")

      If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
      {
        Xor(PB1E, 0x10, PB1E) // Toggle Dock/Undock Status
        If(And(PB1E, 0x10)) // Dock/Undock status changed 0 -> 1 Dock mode
        {
          ^VGBI.UPBT(DOCK_INDICATOR, One)
          ADBG("Notify 0xCA")
          Notify(^VGBI, 0xCA) // Notify Virtual GPIO driver that the system is now in Dock mode.
        } Else { // Dock/Undock status Changed 1 -> 0 Undock mode                              
          ^VGBI.UPBT(DOCK_INDICATOR, Zero)
          ADBG("Notify 0xCB")
          Notify(^VGBI, 0xCB) // Notify Virtual GPIO driver that the system is now in Undock mode.
        }
      } Else {
        \_SB.PCI0.GFX0.IUEH(7) // Docking Indicator lock
      }
    }

    // F1 Scan Code = 0x3B
    If(LEqual(Local2,0x3B)) // EC passes directly to kbd buffer, Windows handles this
    {
      ADBG("FN+F1") // Sleep 
      If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
      {
        ADBG("Sleep")
      } Else {
        If(IGDS)
        {
          \_SB.PCI0.GFX0.GHDS(0) // GMCH SCI hotkey display switch, table index 0       
        }
      }
    }

    // F2 Scan Code = 0x3C
    If(LEqual(Local2,0x3C))
    {
      ADBG("FN+F2")
      If(LEqual(BID,BRH)){ // BoardIdReedHarborTdv
// TODO: Display Sleep for Reed Harbor
        ADBG("Display Sleep")
      } Else {
        If(IGDS) // all other boards
        {
          \_SB.PCI0.GFX0.GHDS(1) // GMCH SCI hotkey display switch, table index 1       
        }
      }
    }

    // F3 Scan Code = 0x3D
    If(LEqual(Local2,0x3D))
    {
      ADBG("FN+F3")
      If(LEqual(BID,BRH)){ // BoardIdReedHarborTdv
// TODO: GPIO_57 RFKILL for WiFi for Reed Harbor
        ADBG("RFKILL")
      } Else {
        If(IGDS) // all other boards
        {
          \_SB.PCI0.GFX0.GHDS(2) // GMCH SCI hotkey display switch, table index 2       
        }
      }
    }

    // F4 Scan Code = 0x3E
    If(LEqual(Local2,0x3E))
    {
      ADBG("FN+F4")
      // BoardIdReedHarborTdv
      If(LEqual(BID,BRH)) // Hot Key decrease brightness.
      {
        If(And(4,DSEN))
        {
          BRTN(0x87)
        } Else {
          Store(BRTL, Local0)
          // Current brightness is a percentage.
          // This must be done if ALS if enabled, as the driver will change the brightness based on ambient light.
          Store(\_SB.PCI0.GFX0.CBLV, BRTL)
          // 1 % will be lost in the conversion so need to make sure we account for it.
          AND(Add(BRTL, 1),0xFE, BRTL)
          If(LGreaterEqual(BRTL, 10))
          {
            Subtract(BRTL, 10, BRTL)
          }
          \_SB.PCI0.GFX0.AINT(1, BRTL)
        }
      } Else { // Hot Key Display Switch for all other boards.
        If(IGDS)
        { 
          \_SB.PCI0.GFX0.GHDS(3) // GMCH SCI hotkey display switch, table index 3       
        }
      }
    }

    If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
    {
      // F5 Scan Code = 0x3F
      If(LEqual(Local2,0x3F))
      {
        ADBG("FN+F5") // brightness up
        If(And(4,DSEN))
        {
          BRTN(0x86)
        } Else {
          Store(BRTL, Local0)
          // Current brightness is a percentage.
          // This must be done if ALS if enabled, as the driver will change the brightness based on ambient light.
          Store(\_SB.PCI0.GFX0.CBLV, BRTL)
          // 1 % will be lost in the conversion so need to make sure we account for it.
          AND(Add(BRTL, 1),0xFE, BRTL)
          If(LLessEqual(BRTL, 90))
          {
            Add(BRTL, 10, BRTL)
          }
          \_SB.PCI0.GFX0.AINT(1, BRTL)
        }
      } // end If(LEqual(Local2,0x3E))
    } Else { // all other boards
      // Virtual Battery Hot Keys(VBHK) - VBHK uses keys 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, + and F5.
      If(LEqual(BNUM,0))  // Check for Virtual Battery Mode before testing the hot-keys specific to Virtual Battery Support.
      {
        VBHK()  //  remove this call if no virtual battery is needed.
      }
    }

    // F6 Scan Code = 0x40
    If(LEqual(Local2,0x40))
    {
      ADBG("FN+F6")
      If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
      {
        If(IGDS) // toggle display
        {
          \_SB.PCI0.GFX0.GHDS(0) // GMCH SCI hotkey display switch, table index 0       
        }
      } Else {
        If(IGDS) // Cycle Brightness for all other boards
        {
          If(CondRefOf(HGAS))
          {
            HGAS()
          } Else {
            If(And(4,DSEN))
            {
              BRTN(0x85)
            }
          }
        }
      }
    }

    // Disable these Function keys for Reed Harbor.
    If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
    {
      // F7 Scan Code = 0x41
      If(LEqual(Local2,0x41)) // EC passes directly to kbd buffer
      {
        ADBG("FN+F7") // mute
      }
      // F8 Scan Code = 0x42
      If(LEqual(Local2,0x42)) // EC passes directly to kbd buffer
      {
        ADBG("FN+F8") // volume down
      }
      // F9 Scan Code = 0x43
      If(LEqual(Local2,0x43)) // EC passes directly to kbd buffer
      {
        ADBG("FN+F9") // volume up
      }
      // F10 Scan Code = 0x44
      If(LEqual(Local2,0x44)) // blank
      {
        ADBG("FN+10")
      }
    } Else {

      // F7 Scan Code = 0x41
      If(LEqual(Local2,0x41))   // Zero Brightness
      {
        If(IGDS)
        {
          If(CondRefOf(CHPS))
          {
            CHPS()      //Display the current hybrid policy on Port 80 header
          } Else {
            If(And(4,DSEN))
            {
              BRTN(0x88)
            }
          }
        }
      }

      // F8 Scan Code = 0x42
      If(LEqual(Local2,0x42))   // Panel Fitting Hot Key.
      {
        If(IGDS)
        {
          If(CondRefOf(HPFS))
          {
            HPFS()
          } Else {
            \_SB.PCI0.GFX0.AINT(2, 0)
          }
        }
      }
  
      // F9 Scan Code = 0x43
      If(LEqual(Local2,0x43))   // Decrease Brightness Level.
      {
        If(CondRefOf(HBRT))
        {
          HBRT(4)
        }
        If(IGDS)
        {
          If(And(4,DSEN))
          {
            BRTN(0x87)
          } Else {
            Store(BRTL, Local0)
            // Current brightness is a percentage.
            // This must be done if ALS if enabled, as the driver will change the brightness based on ambient light.
            Store(\_SB.PCI0.GFX0.CBLV, BRTL)
            // 1 % will be lost in the conversion so need to make sure we account for it.
            AND(Add(BRTL, 1),0xFE, BRTL)
            If(LGreaterEqual(BRTL, 10))
            {
              Subtract(BRTL, 10, BRTL)
            }
            \_SB.PCI0.GFX0.AINT(1, BRTL)
          }
        }
      }
  
      // F10 Scan Code = 0x44
      If(LEqual(Local2,0x44))   // Increase Brightness Level.
      {
        If(CondRefOf(HBRT))
        {
          HBRT(3)
        }
        If(IGDS)
        {
          If(And(4,DSEN))
          {
            BRTN(0x86)
          } Else {
            Store(BRTL, Local0)
            // Current brightness is a percentage.
            // This must be done if ALS if enabled, as the driver will change the brightness based on ambient light.
            Store(\_SB.PCI0.GFX0.CBLV, BRTL)
            // 1 % will be lost in the conversion so need to make sure we account for it.
            AND(Add(BRTL, 1),0xFE, BRTL)
            If(LLessEqual(BRTL, 90))
            {
              Add(BRTL, 10, BRTL)
            }
            \_SB.PCI0.GFX0.AINT(1, BRTL)
          }
        }
      }
    } // end  If(LEqual(BID,BRH))

    // "F12" Scan Code = 0x58
    If(LEqual(Local2,0x58))
    {
      // Airplane Mode toggle.
      If(\_SB.HIDD.HRDY)
      {
         \_SB.HIDD.HPEM(8)
      }
    }

    // Calculator (-) Key = 0x4A

    If(LEqual(Local2,0x4A))   // ACPI Debug Mode is Off.
    {
      Store(0,DBGS)
    }

    // Calculator (+) Key = 0x4E

    If(LEqual(Local2,0x4E))   // ACPI Debug Mode is On.
    {
      Store(1,DBGS)
    }

    // Calculator (Enter) Key = 0x1C
    If(LEqual(Local2,0x1C))   // Set ACPI Break Point.
    {
      BreakPoint
    }


    // "D" Scan Code = 0x20
    If(LEqual(Local2,0x20))   // Virtual Dock
    {
      \_SB.PCI0.GFX0.GDCK(1)
    }
    
    // "U" Scan Code = 0x16
    If(LEqual(Local2,0x16))   // Virtual Undock
    {
      \_SB.PCI0.GFX0.GDCK(0)
    }

    // "0" Scan Code = 0x01 
    If(LEqual(Local2,0x01))   // Toggle Wireless On/Off state.
    {

    }



    // HK: "O" Scan Code = 0x18
    If(LEqual(Local2,0x18))   // CTDP Up
    {
      If(LAnd(CondRefOf(\_SB.PCI0.CTCU),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
        \_SB.PCI0.CTCU()
      }
    }

    // HK: "[" Scan Code = 0x1A
    If(LEqual(Local2,0x1A))   // CTDP Nominal
    {
      If(LAnd(CondRefOf(\_SB.PCI0.CTCN),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
      \_SB.PCI0.CTCN()
      }
    }

    // HK: "]" Scan Code = 0x1B
    If(LEqual(Local2,0x1B))   // CTDP Down
    {
      If(LAnd(CondRefOf(\_SB.PCI0.CTCD),LEqual(CTDB,1))){ // only allow this feature if it is enabled in SETUP
      \_SB.PCI0.CTCD()
      }
    }
  
    // "Fn+F12" Scan Code = 0x62
    If(LEqual(Local2,0x62)) // Web-Cam power control
    {
//[-start-130405-IB04770276-modify]//
      P8XH(0,0x62,1)
//[-end-130405-IB04770276-modify]//
    }

  } // end Method(_Q52) hot key event

  Method(_Q54)    // Power Button Event for iSCT and Control method Power Button(10sec PB Override without V-GPIO driver)
  {
//[-start-130405-IB04770276-modify]//
    P8XH(0,0x54,1)
//[-end-130405-IB04770276-modify]//

    //
    //  iSCT and 10Sec PB Override are now mutual exclusive at least for this event.
    //  Make sure iSCT is enabled before notifying IAOE.
    //
    If(And(ICNF, 1))
    {
      Notify(IAOE,0x80)
    }
    
    //
    // Check if 10sec PB Override and EC 10sec mode.
    // If 10sec PB OVR is enabled but EC 10sec mode is not enabled, then BIOS has to handle
    // Power button event.
    // PB1E Bit0 -> Setup setting for 10sec PB OVR
    // PB10 -> EC setting for 10sec PB OVR
    //
    If(LAnd(And(PB1E, 1), LNot(ECRD(RefOf(PB10))))){

      //
      // Check UAMS(User Absent Mode State) to notify the power button event.
      //
      If(UAMS){ // UAMS has a non-Zero value, means the system is under User Absent Mode. Send Wake up event.
        ADBG("PB Wake up 0x02")
        If(CondRefOf(\_SB.PWRB)){
          Notify(\_SB.PWRB, 0x02)
        }
      } Else { // UAMS is Zero, means the system is ON. Send Sleep event.
        ADBG("PB Sleep 0x80")
        If(CondRefOf(\_SB.PWRB)){
          Notify(\_SB.PWRB, 0x80)
        }
      }
    }
  } // end Method(_Q54) Power button event.

  Method (_Q70)   // ALS "Light Intensity" event
  {
//[-start-130405-IB04770276-modify]//
    P8XH (0,0x70,1)
//[-end-130405-IB04770276-modify]//

    // Update the LUX Values.

    Store(ECRD(RefOf(LUXH)),LHIH)
    Store(ECRD(RefOf(LUXL)),LLOW)

    If(LAnd(LEqual(ALSE,2),IGDS))
    {
      // Handle the ALS event via the OpRegion method
      
      Store(ECRD(RefOf(LUXH)), Local0)     
      Or(ShiftLeft(Local0, 8), ECRD(RefOf(LUXL)), Local0)
      \_SB.PCI0.GFX0.AINT(0, Local0)
      Notify(ALSD,0x80) // notify MS driver of change
    }
  }

  Method(_QD5)    // 10 second power button press.
  {
    If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
    {
      ^VGBI.UPBT(POWER_BUTTON, One)
      ADBG("Notify 0xC0")
      ECMD (0x2D) // Wake up EC first. This is needed to get the release event immediately after press event. 
      Notify(^VGBI, 0xC0) // Notify Virtual GPIO driver that the power button is pressed.
    } Else {
      If(CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0) 
        And(Local0, 0xC0, \_SB.PCI0.GFX0.IUER)  // Clear 4:0 button events on entry.
        Store(\_SB.PCI0.GFX0.IUER, Local0) 
        Or(Local0, 0x01, \_SB.PCI0.GFX0.IUER)   // Set Power Button Status = pressed.
      }
    } // End IF
  }// End of Method

  Method(_QD6)    // 10 second power button de-press.
  {
    If(LAnd(CondRefOf(^VGBI._STA),LEqual(And(^VGBI._STA(),One),One))) // VirtualButton driver is loaded(means it is Winblue)
    {
      ^VGBI.UPBT(POWER_BUTTON, Zero)
      ADBG("Notify 0xC1")
      Notify(^VGBI, 0xC1)  // Notify Virtual GPIO driver that the power button is released.
    } Else {
      If(CondRefOf(\_SB.PCI0.GFX0.IUER))
      {
        Store(\_SB.PCI0.GFX0.IUER, Local0) 
        And(Local0, 0xC0, \_SB.PCI0.GFX0.IUER)  // Clear 4:0 button events on entry.
      }
    }
  }
//
// Reed Harbor hardware buttons.
//
  Method(_Q80)    // Volume Up
  {
    If(LEqual(BID,BRH)){ // BoardIdReedHarborTdv
ADBG("Volume Up")
      \_SB.PCI0.GFX0.IUEH(2)
    }
  }
  Method(_Q81)    // Volume Down
  {
    If(LEqual(BID,BRH)){ // BoardIdReedHarborTdv
ADBG("Volume Down")
      \_SB.PCI0.GFX0.IUEH(3)
    }
  }
  Method(_Q85)    // Windows Home button
  {
    If(LEqual(BID,BRH)){ // BoardIdReedHarborTdv
ADBG("Windows Home")
      Sleep(1000)
      \_SB.PCI0.GFX0.IUEH(1)
    }
  }

  Method(_QF0)    // Thermal Event.
  {
    If(LEqual(DBGS,0))
    {
      // Only handle the numerous Thermal Events if
      // we are NOT doing ACPI Debugging.
      Notify(\_TZ.TZ00,0x80)
      Notify(\_TZ.TZ01,0x80)
    }
  }

Device(WDT0)  // WDT Device Resource Consumption
{
  Name(_HID,EISAID("PNP0C02"))        

  Name(_UID,3)

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x6A4,0x6A4,0x1,0x1)  // 1 Byte EC Prv Intfc.
    IO(Decode16,0x6A0,0x6A0,0x1,0x1)  // 1 Byte EC Prv Intfc.
  })
}

/************************************************************************;
;*
;* Name:  CHDK
;*
;* Description: Check DOCK status, returen True if Dock status equal Arg0
;*
;************************************************************************/

  Method(CHDK, 1)
  {
    // If not Mobile Platform then skip the code and return 0
    If(LEqual(ECON,1)){ 
      If(LEqual(ECRD(RefOf(DOCK)), Arg0))
        { // If not docked then it's hot plug
          Return(1)   
        }
    }
    Return(0)
  }

  //
  // Hardware Button Array support
  //

  Device(BIND)  // Button Indicators.
  {
     Name(_HID, "INT33D2")
     Name(_CID, "PNP0C40")

     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUBE,1), LGreaterEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
     //
     // _DSM : Device Specific Method for the Windows Compatible Button Array.
     //
     // Arg0: UUID Unique function identifier
     // Arg1: Integer Revision Level
     // Arg2: Integer Function Index
     // Arg3: Package Parameters
     //
     Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
     {
       // Compare passed in UUID to supported UUID.

       If (LEqual(Arg0, ToUUID ("DFBCF3C5-E7A5-44E6-9C1F-29C76F6E059C")))
       {
         If (LEqual(0,ToInteger(Arg1)))        // Revision 0.
         {
           Switch (ToInteger(Arg2))            // Switch to Function Index.
           {
             //
             // Function 0, Query of supported functions.
             //

             Case (0)
             {
               Return (Buffer() {0x03})
             }

             //
             // Function 1, Windows Compatible Button Array Power Button Properties.
             //

             Case (1)
             {
               // Only return support if platform enabled via setup.
               // PB1E's Bit0 is set only when both CS and 10s PB OVR are enabled.
               If(LEqual(And(PB1E, 1), 1))
               {
                 Return (0x07)
               }

               Return(0x00)

             }
           } // End Switch statement
         }  // End Revision check
       }  // End UUID check

       // If the code falls through to this point, just return a buffer of 0.

       Return (Buffer() {0x00})

    }  // End _DSM Method
  }
  
  Device(CIND)  // Convertible Indicators.
  {
     Name(_HID, "INT33D3")
     Name(_CID, "PNP0C60")
    
     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUCE,1), LGreaterEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }
  
  Device(DIND)  // Docking Indicators.
  {
     Name(_HID, "INT33D4")
     Name(_CID, "PNP0C70")
     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUDE,1), LGreaterEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }

  Device(VGBI)                                  // Virtual GPIO Button "Interface."
  {
  
    Name(_HID, EISAID("INT33D6"))               // HID for Intel Virtual GPIO Button Interface.
    Name(VBDS,0)
    Name(ONTM,0) // Temporary variable to initialise VBDS only once in the boot

    Method(_STA,0,Serialized)
    {
      If(LGreaterEqual(OSYS,2013))              // Virtual Button applicable for Winblue and higher version of WinOS
      {
        ADBG("WinB VGBI STA")
        Return(0x0F)                            // Device present,enabled and should be shown in UI.
      } Else {
        Return(0x00)                            // Device NOT present
      }
    }// End of _STA

    Method(VBDL,0,Serialized) // Virtual Button Driver Load - Refer Bios Arch Spec
    {
      ADBG("VBDL")
      If(LEqual(And(PB1E,One),One)){ // 10 Sec Power Button Enabled in setup?
        ADBG("VBDL EN")

        //
        // Clear PBST so that we can hide the default power button. 
        //
        If(CondRefOf(\_SB.PWRB.PBST))
        {
          Store(0, \_SB.PWRB.PBST)
          Notify(\_SB.PWRB, 1) // Device check
        }

        // First send 0x74 to EC to disable _Q54 event for Power button.
        If(CondRefOf(ECMD)){
          ADBG("Disable _Q54")
          ECMD(0x74)
        }

        // Set EC 10s enable bit.
        ECWT(1, RefOf(PB10))

      } Else {
        // 10s power button disabled, clear EC 10s enable bit.
        ADBG("VBDL DIS")
        ECWT(0, RefOf(PB10))

        //
        // Here we know V-GPIO driver is loaded but 10s PB is disabled.
        // Set PBST so that the default power button can handle the request. 
        //
        If(CondRefOf(\_SB.PWRB.PBST))
        {
          Store(1, \_SB.PWRB.PBST)
          Notify(\_SB.PWRB, 1) // Device check
        }
      }
    }

    Method(VGBS,0,Serialized)  // Virtual GPIO Button Status - Refer Bios Arch Spec
    {
      // Bit[7] : Docking Indicatory Status
      // Bit[6] : Convertible/Slate Indicator Status
      // Bit[5] : Reserved
      // Bit[4] : Rotation Lock Button Status
      // Bit[3] : Volume Down Button Status
      // Bit[2] : Volume Up Button Status
      // Bit[1] : Windows Home Button Status
      // Bit[0] : Power Button Status

      ADBG("VGBS")
      If(LEqual(ONTM,0))
      {
        // Initial setup option on VBDS and thereafter hotkeys should be updating the VBDS
        if(LEqual(And(PB1E,0x04),0x04)) // Rotation Lock
        {
          UPBT(ROTATION_LOCK_BUTTON,One)
        }

        if(LEqual(And(PB1E,0x08),0x08)) // Slate/Laptop
        {
          UPBT(CONVERTIBLE_BUTTON,One)
        }

        if(LEqual(And(PB1E,0x10),0x10))  // Undock/Dock
        {
          UPBT(DOCK_INDICATOR,One)
        }
        Store(One,ONTM) 
      }
      Return(VBDS)
    }// End of Method

    //
    // UPBT Update Button Status
    //
    // Arg0: Bit location of the target button
    //       0: Power Button
    //       1: Windows Button
    //       2: Volume up Button
    //       3: Volume down Button
    //       4: Rotation Lock Button
    //       5: Reserved
    //       6: Convertible state 0 - Slate, 1 - Notebook
    //       7: Dock Indicator 0 - Undock, 1 - Dock
    //       
    // Arg1: On/Off state, 0 - Clear the target bit, 1 - Set the target bit.
    // 
    Method(UPBT,2,Serialized)  // Update Button Status 
    {
      ShiftLeft(One, Arg0, Local0)
      if(Arg1){                // Button Press/ON
        Or(VBDS, Local0, VBDS)
      } Else {                 // Button Press/OFF
        And(VBDS, Not(Local0),VBDS) 
      }
    } // End of UPBT

    Method(PBIN, 0, Serialized)
    {
      ADBG("VGBI PB INIT")
      //
      // Send EC 0x73 command to make 0x54 Power button notification is available.
      // This is needed to support no Virtual GPIO driver case.
      // Once the driver calls VBDL, VBDL sends 0x74(disable PB 0x54) to EC.
      //
      If(LAnd(And(PB1E, 1), LNot(ECRD(RefOf(PB10)))))
      {
        If(CondRefOf(\_SB.PWRB.PBST))
        {
          Store(1, \_SB.PWRB.PBST)
          Notify(\_SB.PWRB, 1) // Device check
        }
        If(CondRefOf(ECMD)){
          ADBG("Enable _Q54")
          ECMD(0x73)  // Enable Power Button 0x54 Notification
        }
      }
    }
  } // End of VGBI

} // End H_EC

  // System Bus

Scope(\_SB)
{

  // Define an AC Device.

  Device(ADP1)
  {
    Name(_HID,"ACPI0003")

    Method(_STA)
    {
      If (LEqual(ECON,1)){ 
        Return(0x0F)
      }
      Return(0x00)
    }
  
    // Return the value that determines if running
    // from AC or not.

    Method(_PSR,0)
    {
      Return(PWRS)
    }

    // Return that everything runs off of AC.

    Method(_PCL,0)
    {
      Return(\_SB)
    }
  }

  // Define a Lid Switch.

  Device(LID0)
  {
    Name(_HID,EISAID("PNP0C0D"))

    Method(_STA)
    {
      If(LEqual(ECON,1)){ 
        Return(0x0F)
      }
      Return(0x00)
    }

    Method(_LID,0)
    {
      // 0 = Closed, 1 = Open.

      Return(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)))
    }
  }

  // Define a (Control Method) Power Button.

  Device(PWRB)
  {
    Name(_HID,EISAID("PNP0C0C"))

    // The PRW isn't working when
    // placed in any of the logical locations ( PS2K, PS2M,
    // H_EC ), so a Power Button Device was created specifically
    // for the WAKETIME_SCI PRW.

    //
    // Power button status flag used to communicate H_EC.VBDL
    //
    Name(PBST, 1)

    Method(_PRW, 0) {
      If(LEqual(LANP,1)){
        //
        // When LAN is present, GPIO27 becomes ME GPIO(HSD 4834440).  Remove PBT.
        // 
        Return(Package(){0,0})
      }
      If(LEqual(BID, BWT1))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for WhiteTipMountain1
      }
      If(LEqual(BID, BW1P))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for WhiteTipMountain1 PPV
      }
      If(LEqual(BID, BW2C))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for WhiteTipMountain2
      }
      If(LEqual(BID, BW2P))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for WhiteTipMountain2 PPV
      }
      If(LEqual(BID, BSPC))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for SawToothPeak
      }
      If(LEqual(BID, BSPP))
      {
        Return(Package(){27,4})   // GPI027 = GPE27 for SawToothPeak PPV
      }
      
//[-start-130401-IB10920020-modify]//
      //
      // GPI14 = GPE30 = Waketime SCI for Haswell Traditional boards
      // Must send EC command to control Waketime,  otherwise return 0 for LowestSleepState
      //
      
      //Return(Package(){30,4})      
      Return(Package(){30,0})
//[-end-130401-IB10920020-modify]//
    }
    
    Method(_STA, 0)
    {
      If(LAnd(LEqual(ECON,1), PBST)){ 
        Return(0x0F)
      }
      Return(0x00)
    }

  }//end device PWRB

  Device(HIDD)                                          // HID Device.
  {
     Name(_HID,"INT33D5")                            // Intel Ultrabook HID Platform Event Driver.
     Name(_CID,"PNP0C02")                            // Generic Motherboard Resources.

     Name (HBSY, 0)   // HID Busy
     Name (HIDX, 0)   // HID Index
     Name (HMDE, 0)   // HID Mode
     Name (HRDY, 0)   // HID Ready

     Method(_STA,0,Serialized)                       // Status Method.
     {
        If(LEqual(BID, BHB))                         // Hide device for HarrisBeach
        {
          Return(0x00)
        }
        Else
        {
          Return(0x0F)
        }
     }

    //
    // HID Driver Descriptor Method - Called by HID Driver during initialization
    // to obtain HID Descriptor information.
    //
    // Input: None
    //
    // Output:  Package containing a complete HID Descriptor information.
    //
    Method(HDDM,0,Serialized)
    {
       // Placeholder.
       Name(DPKG, Package(4) {0x11111111, 0x22222222, 0x33333333, 0x44444444})
       Return(DPKG)
    }

    //
    // HID Driver Event Method - Called by HID Driver to get the specific
    // platform event.
    //
    // Input: None
    //
    // Output: Plastform HID Event.
    // Mode 0 = Index of HID Input Report, per pre-defined Table.
    // Mode 1 = Package containing a complete HID Input Report.
    //
    Method(HDEM,0,Serialized)
    {
       Store(0,HBSY)                          // Clear HID Busy.
       // Simple Mode is hardcoded for now.  Return Simple Mode HID Index Value.
       If(LEqual(HMDE,0))
       {
          Return(HIDX)
       }
       Return(HMDE)
    }

    //
    // HID Driver Mode Method - Called by HID Driver during initialization to get
    // the platform mode of operation.
    //
    // Input: None
    //
    // Output:  Mode the platform is running in.
    // 0 = Simple Mode.
    // 1 = Advanced Mode.
    //
    Method(HDMM,0,Serialized)
    {
       Return(HMDE)               // Return Mode of operation.
    }

    //
    // HID Driver Status Method - called by HID Driver to report platform readiness status.
    // Input: Driver Status.
    // 0 = Driver Unloaded.
    // 1 = Driver Loaded and Ready.
    //
    // Output: None
    //
    Method(HDSM,1,Serialized)
    {
      Store(Arg0,HRDY)              // Store HID Ready Status.
      // Eventually code will communicate to platform the Driver status (enabled/disabled).
    }

    //
    // HID Platform Event Method - called by Platform to communicate HID Event to Driver.
    // Input:
    // Mode 0 = Index of HID Event.
    // Mode 1 = Package containing a complete HID Report.
    //
    Method(HPEM,1,Serialized)                        // HID Platform Event Method.
    {
      Store(1,HBSY)                                 // Set HID Busy.
      // Simple Mode is hardcoded for now.  Simply store HID Index value.
      If(LEqual(HMDE,0))
      {
         Store(Arg0,HIDX)
      } Else {
         Store(Arg0,HIDX)
      }
      Notify(\_SB.HIDD,0xC0)                         // Notify Driver to get HID Event.
      Store(0,Local0)                                // Initialize Local0 as a timeout counter.
      While(LAnd(LLess(Local0,250),HBSY))            // Wait <= 1 second for Driver to ACK success.
      {
        Sleep(4)                                     // Delay 4 ms.
        Increment(Local0)                            // Increment Timeout.
      }
      If(LEqual(HBSY,1))                             // Failure?
      {
        Store(0,HBSY)                                // Yes.  Clear HID Busy Flag.
        Store(0,HIDX)                                // Set HID Simple Mode Index = 0 = Undefined.
        Return(1)                                    // Return Failure.
      } Else {
        Return(0)                                    // Return Success.
      }
    }
  }


}//end scope _SB


