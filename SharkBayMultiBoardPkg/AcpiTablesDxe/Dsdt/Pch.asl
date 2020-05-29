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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

//[-start-130813-IB06720229-add]//
External(AR08, PkgObj)
External(PR08, PkgObj)
//[-end-130813-IB06720229-add]//
  //
  // RPAx is root port addresses, which are updated when post to reflect 
  // the root port function number swapping.
  //
  Name (RPA0, 0x001C0000)
  Name (RPA1, 0x001C0001)
  Name (RPA2, 0x001C0002)
  Name (RPA3, 0x001C0003)
  Name (RPA4, 0x001C0004)
  Name (RPA5, 0x001C0005)
  Name (RPA6, 0x001C0006)
  Name (RPA7, 0x001C0007)

  //
  // PCHS stands for PCH series. This will be updated when post.
  // 1: PchH
  // 2: PchLp
  // others: unknown
  //
//[-start-130703-IB10540053-modify]//
  Name (PCHS, 0x55AA55AA)
//[-end-130703-IB10540053-modify]//

  //
  // Reserved MEM range for S3 Save/restore with 64K in size.
  // This will be updated when post.
  //
//[-start-130703-IB10540053-modify]//
  Name (SRMB, 0x55AA55AA)
//[-end-130703-IB10540053-modify]//

  //
  // Maximum Snoop Latency and Maximum No-Snoop Latency values for PCIE.
  // This will be updated when post.
  //
//[-start-130711-IB08050222-modify]//
  Name (PML1, 0x55AA55AA)
  Name (PML2, 0x55AA55AA)
  Name (PML3, 0x55AA55AA)
  Name (PML4, 0x55AA55AA)
  Name (PML5, 0x55AA55AA)
  Name (PML6, 0x55AA55AA)
  Name (PML7, 0x55AA55AA)
  Name (PML8, 0x55AA55AA)
  Name (PNL1, 0x55AA55AA)
  Name (PNL2, 0x55AA55AA)
  Name (PNL3, 0x55AA55AA)
  Name (PNL4, 0x55AA55AA)
  Name (PNL5, 0x55AA55AA)
  Name (PNL6, 0x55AA55AA)
  Name (PNL7, 0x55AA55AA)
  Name (PNL8, 0x55AA55AA)
//[-end-130711-IB08050222-modify]//

Scope(\)
{
  //
  // Define the IO Address 0810h-0813h as an ACPI Operating Region first, and
  // then it will be dynamically updated during initialization of DTS code.
  // The address presented here may not be the actual address used.
  // This address range is used as a DTS I/O Trap SMI so that ASL and SMI
  // can communicate when needed.
  //
  OperationRegion(IO_D,SystemIO,0x810,0x4)
  Field(IO_D,ByteAcc,NoLock,Preserve) {
    TRPD,  8  // 0x810 = DTS I/O Trap 
  }
  //
  // The IO address in this ACPI Operating Region will be updated during POST.
  // This address range is used as a HotKey I/O Trap SMI so that ASL and SMI can
  // communicate when needed.
  //
  OperationRegion(IO_H,SystemIO,0x1000,0x4)
  Field(IO_H,ByteAcc,NoLock,Preserve) {
    TRPH,  8
  }
  //
  // Define PCH PMBASE I/O as an ACPI operating region.  The base address
  // can be found in Device 31, Registers 40-43h.
  //
  OperationRegion(PMIO, SystemIo, \PMBS, 0x80)
  Field(PMIO, ByteAcc, NoLock, Preserve) {
    Offset(0x28),     // GPE0 Enable
      ,     16,
      ,      3,       // GPE for 0-2 GPIO's
      GPE3,  1,
      ,      7,       // GPE for 4-0xA GPIO's
      GPEB,  1,
    Offset(0x3c),     // UPRWC - USB Per-Port registers write control
      ,      1,
      UPRW,  1,       // USB Per-Port registers write enable
    Offset(0x42),     // General Purpose Control
      ,      1,
      GPEC,  1
  }
  Field(PMIO, ByteAcc, NoLock, WriteAsZeros) {
    Offset(0x20),     // GPE0 Status
      ,     16,
      ,      3,       // GPS for 0-2 GPIO's
      GPS3,  1,
      ,      7,       // GPS for 4-0xa GPIO's
      GPSB,  1,
    Offset(0x64),     // TCO status register
      ,      9,
      SCIS,  1,       // TCO DMI SCI status
      ,      6
  }

  //
  // Define PCH PMBASE I/O as an ACPI operating region.  The base address
  // can be found in Device 31, Registers 40-43h.
  //
  OperationRegion(PMLP, SystemIo, Add(\PMBS,0x80), 0x20)
  Field(PMLP, ByteAcc, NoLock, Preserve) {
    Offset(0x10),     // GPE0 Enable
      ,      8,
      GE08,  1,
      ,      8,
      GE17,  1,
      ,     27,
      GE45,  1,
      ,      5,
      GE51,  1,
      ,     76,
  }
  Field(PMLP, ByteAcc, NoLock, WriteAsZeros) {
      Offset(0x00),     // GPE0 Status
      ,      8,
      GS08,  1,
      ,      8,
      GS17,  1,
      ,     27,
      GS45,  1,
      ,      5,
      GS51,  1,
      ,     76,
  }

  //
  // Define PCH GPIO I/O as an ACPI operating region.
  // The base address can be found in Device 31, Registers 48-4Bh.
  //

  OperationRegion(GPR, SystemIo, \GPBS, 0x400)
  Field(GPR, ByteAcc, NoLock, Preserve) {
    Offset(0x00),   // GPIO, Use Select, Bank 0
      GU00,  8,
      GU01,  8,
      GU02,  8,
      GU03,  8,
    Offset(0x04),   // GPIO, I/O Select, Bank 0
      GIO0,  8,
      GIO1,  8,
      GIO2,  8,
      GIO3,  8,
    Offset(0x0C),   // GPIO, Level, Bank 0
      GL00,  8,
      GL01,  8,
      GL02,  8,
      GP24,  1,
      , 2,
      GP27,  1,     // SATA_PWR_EN#0
      GP28,  1,     // SATA_PWR_EN#1 (SATA Ports 1 and 2)
      , 3,
    Offset(0x18),
      GB00,  8,     // GPIO, Blink, Bank 0
      GB01,  8,
      GB02,  8,
      GB03,  8,
    Offset(0x2C),
      GIV0,  8,     // GPIO, Invert, Bank 0
      GIV1,  8,
      GIV2,  8,
      GIV3,  8,
    Offset(0x30),   // GPIO, Use Select, Bank 1
      GU04,  8,
      GU05,  8,
      GU06,  8,
      GU07,  8,
    Offset(0x34),   // GPIO, I/O Select, Bank 1
      GIO4,  8,
      GIO5,  8,
      GIO6,  8,
      GIO7,  8,
    Offset(0x38),   // GPIO, Level, Bank 1
      GL04,  8,
      GL05,  8,
      GL06,  8,
      GL07,  8,
    Offset(0x40),   // GPIO, Use Select, Bank 2
      GU08,  8,
      GU09,  8,
      GU0A,  8,
      GU0B,  8,
    Offset(0x44),   // GPIO, I/O Select, Bank 2
      GIO8,  8,
      GIO9,  8,
      GIOA,  8,
      GIOB,  8,
    Offset(0x48),   // GPIO, Level, Bank 2
      GL08,  8,
      GL09,  8,
      GL0A,  8,
      GL0B,  8
  }

  //
  // Define PCH GPIO I/O as an ACPI operating region.
  // The base address can be found in Device 31, Registers 48-4Bh.
  //
  OperationRegion(GPRL, SystemIo, \GPBS, 0x40)
  Field(GPRL, ByteAcc, NoLock, Preserve) {
    Offset(0x00),   // GPI_OWN, 0 => ACPI driver owned
      ,      8,
      GO08,  1,
      GO09,  1,
      ,      3,
      GO13,  1,
      GO14,  1,
      ,      2,
      GO17,  1,
      ,     27,
      GO45,  1,
      ,      5,
      GO51,  1,
      ,     76,
    Offset(0x30),   // GPI_ROUT, 0 => SCI
      GR00,  32,
      GR01,  32,
      GR02,  32
  }

  //
  // Define a Memory Region that will allow access to the Root Complex
  // Register Block.  Note that in the Intel Reference Solution, the RCBA
  // will get fixed up dynamically during POST.
  //
  OperationRegion(RCRB,SystemMemory,\SRCB,0x4000)
  Field(RCRB,DWordAcc,Lock,Preserve) {
    Offset(0x0000),   // Backbone Related Registers
    Offset(0x1000),   // Other Chipset Registers
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdTradFlag)
//[-end-130812-IB05670190-modify]//
    Offset(0x2330),   // SBI AFE Address
      AFEA, 32,
    Offset(0x2334),   // SBI AFE Data
      AFED, 32,
    Offset(0x2338),   // SBI AFE Status
      AFES, 16,
    Offset(0x233A),   // SBI AFE Routing Id
      AFER, 16,
#endif // FeaturePcdGet(PcdTradFlag)
    Offset(0x3000),   // Legacy\Other Chipset Configuration Registers
    Offset(0x331c),
      ,     24,
      PMFS,  1,       // PCIe Source Clock Request Status
    Offset(0x3320),
      CKEN, 32,       // PCIe Source Clock Enable Register
    Offset(0x3404),   // High Performance Timer Configuration
      HPAS,  2,       //  (1:0) High Performance Address Select
      ,      5,       //  (6:2) Reserved
      HPAE,  1,       //  (7) High Performance Address Enable
    Offset(0x3418),   // Function Disable Register
      ,      1,       //  (0) Reserved
      ADSD,  1,       //  (1) Audio DSP Disable
      SATD,  1,       //  (2) Serial ATA Disable
      SMBD,  1,       //  (3) SMBus Disable
      HDAD,  1,       //  (4) High Definition Audio Disable
      ,     11,       //  (15:5) Skip for now
      RP1D,  1,       //  (16) Root Port 1 Disable
      RP2D,  1,       //  (17) Root Port 2 Disable
      RP3D,  1,       //  (18) Root Port 3 Disable
      RP4D,  1,       //  (19) Root Port 4 Disable
      RP5D,  1,       //  (20) Root Port 5 Disable
      RP6D,  1,       //  (21) Root Port 6 Disable
      RP7D,  1,       //  (22) Root Port 7 Disable
      RP8D,  1,       //  (23) Root Port 8 Disable
    Offset(0x359c),   // Usb Port Disable Override Register
      UP0D,  1,       //  (0) Usb Port 0 disable
      UP1D,  1,       //  (1) Usb Port 1 disable
      UP2D,  1,       //  (2) Usb Port 2 disable
      UP3D,  1,       //  (3) Usb Port 3 disable
      UP4D,  1,       //  (4) Usb Port 4 disable
      UP5D,  1,       //  (5) Usb Port 5 disable
      UP6D,  1,       //  (6) Usb Port 6 disable
      UP7D,  1,       //  (7) Usb Port 7 disable
      UP8D,  1,       //  (8) Usb Port 8 disable
      UP9D,  1,       //  (9) Usb Port 9 disable
      UPAD,  1,       //  (10) Usb Port 10 disable
      UPBD,  1,       //  (11) Usb Port 11 disable
      UPCD,  1,       //  (12) Usb Port 12 disable
      UPDD,  1,       //  (13) Usb Port 13 disable
      ,      1,       //  (14) Reserved
      ,      1        //  (15) Reserved
  }
  //
  // Support S0, S3, S4, and S5.  The proper bits to be set when
  // entering a given sleep state are found in the Power Management
  // 1 Control ( PM1_CNT ) register, located at ACPIBASE + 04h,
  // bits 10d - 12d.
  //

  //
  // Define the IO Address 1000h-1003h as an ACPI Operating Region first, and
  // then it will be dynamically updated during initialization of PFAT code.
  // The address presented here may not be the actual address used.
  // This address range is used as a PFAT Tools Interface I/O Trap so that the
  // update tool can trigger bios code to update the flash using the pfat flow.
  //
  OperationRegion (IO_P, SystemIO, 0x1000, 0x4)
  Field (IO_P, ByteAcc, NoLock, Preserve) {
    TRPF, 8  // 0x1000 = PFAT I/O Trap
  }

} //end Scope(\)

  Scope (\_SB)
  {

    Method(RDGI,1,Serialized) //Read the value of Input GPIO Line
    {
      // Function to Read GPIO
      //
      // Arg0 : GPIn : GPIO Pin number to be read(Zero based)
      //
      If(LLessEqual(Arg0, 94)){
        // GPBS - GPIO Base Address - 0x800
        // Local0 = GPIOBASE + 100h + (GPIn * 0x08)
        Store( Add(Add(GPBS,0x100) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4) 
        Field(LGPI, AnyAcc, NoLock, Preserve) {
          Offset(0x0),
          ,30,
          TEMP, 1
        }
        Return(TEMP)
      }
    } // End of Method(RDGI,1)

    Method(RDGP,1,Serialized)
    {
      // Function to Read GPIO
      //
      // Arg0 : GPIn : GPIO Pin number to be read(Zero based)
      //
      If(LLessEqual(Arg0, 94)){
        // GPBS - GPIO Base Address - 0x800
        // Local0 = GPIOBASE + 100h + (GPIn * 0x08)
        Store( Add(Add(GPBS,0x100) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4) 
        Field(LGPI, AnyAcc, NoLock, Preserve) {
          Offset(0x0),
          ,31,
          TEMP, 1
        }
        Return(TEMP)
      }
    } // End of Method(RDGP,1)
  
    Method(WTGP,2,Serialized)
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  Value to be written
      //
      If(LLessEqual(Arg0, 94)){
        Store( Add(Add(GPBS,0x100) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4)
        Field(LGPI, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
        , 31,
        TEMP, 1
        }
        Store(Arg1,TEMP)
      }
    }
    Method(WTIN,2,Serialized)
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  Value to be written
      //
      If(LLessEqual(Arg0, 94)){
        Store( Add(Add(GPBS,0x100) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4)
        Field(LGPI, ByteAcc, NoLock, Preserve) {
        Offset(0x0),
        , 3,
        TEMP, 1
        }
        Store(Arg1,TEMP)
      }
    }

    Method(WPGP,2,Serialized) //GP Weak pull
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  00 = none 01 = down 10 = up
      //
      If(LLessEqual(Arg0, 94)){
        Store( Add(Add(GPBS,0x104) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4)
        Field(LGPI, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
          TEMP, 2
        }
        Store(Arg1,TEMP)
      }
    }

    Method(GP2N,2,Serialized) //GPIO to Native
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  0 = Native  1 = GPIO
      //
      If(LLessEqual(Arg0, 94)){
        Store( Add(Add(GPBS,0x100) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGPI, SystemIo, Local0, 4)
        Field(LGPI, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
          TEMP, 1
        }
        Store(Arg1,TEMP)
      }
    }

    Method(GP2A,2,Serialized) //GP to APIC
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  00 = mask 01 = route to IOxAPIC and also Pull UP/NONE the GPIO mode Sensing weak pull
      //
        If(LLessEqual(Arg0, 94)){
        Store( Add(Add(GPBS,0x104) , Multiply(Arg0,0x08)),Local0)
        OperationRegion(LGP2, SystemIo, Local0, 4)
        Field(LGP2, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
          GPWP, 2,
          GPIS, 1
        }
        if(LEqual(Arg1,1))
        {
          Store(0,GPIS)
          Store(0,GPWP)
        }Else{
          Store(2,GPWP)
          Store(1,GPIS)
        }    

        Store(Add(GPBS, 0x10), Local0)
        OperationRegion(LGPI, SystemIo, Local0, 2)
        Field(LGPI, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
          TEMP, 16
        }
        
        // GPI PIRQ to IOxAPIC enable bit map
        // GPI[55:45] to IOxAPIC enable bit[15:5], Subtract 40 from GPI to get ACPI bit
        // GPI[14:13] to IOxAPIC enable bit[4:3], Subtract 10 from GPI to get ACPI bit
        // GPI[10:8]  to IOxAPIC enable bit[2:0], Subtract 8 from GPI to get ACPI bit
        If(LGreaterEqual(Arg0, 45))
        {
          Subtract(Arg0, 40, Local1)   // GPI[55:45] map to APIC[15:5]
        }Else{
          If(LLessEqual(Arg0, 10))
          {
            Subtract(Arg0, 8, Local1)  // GPI[10:8] map to APIC[2:0]
          }Else{
            Subtract(Arg0, 10, Local1) // GPI[14:13] map to APIC[4:3]
          }
        }
        
        Store(ShiftLeft(1, Local1), Local2)
        If(Arg1){                 //Enable GP to IOAPIC
          Or(TEMP, Local2, TEMP)
        } Else{                   //mask
          And(TEMP, Not(Local2), TEMP)
        }
      }
    }

    Method(GP2B,2,Serialized) //GP to APIC
    {
      // Function to write GPIO 
      // Arg0 - GPIn : GPIO Pin number to write
      // Arg1 -  00 = mask 01 = route to IOxAPIC
      //
      If(LLessEqual(Arg0, 94)){
        Store(Add(GPBS, 0x10), Local0)
        OperationRegion(LGPI, SystemIo, Local0, 2)
        Field(LGPI, AnyAcc, NoLock, Preserve) {
        Offset(0x0),
          TEMP, 16
        }

        // GPI PIRQ to IOxAPIC enable bit map
        // GPI[55:45] to IOxAPIC enable bit[15:5], Subtract 40 from GPI to get ACPI bit
        // GPI[14:13] to IOxAPIC enable bit[4:3], Subtract 10 from GPI to get ACPI bit
        // GPI[10:8]  to IOxAPIC enable bit[2:0], Subtract 8 from GPI to get ACPI bit
        If(LGreaterEqual(Arg0, 45))
        {
          Subtract(Arg0, 40, Local1)   // GPI[55:45] map to APIC[15:5]
        }Else{
          If(LLessEqual(Arg0, 10))
          {
            Subtract(Arg0, 8, Local1)  // GPI[10:8] map to APIC[2:0]
          }Else{
            Subtract(Arg0, 10, Local1) // GPI[14:13] map to APIC[4:3]
          }
        }

        Store(ShiftLeft(1, Local1), Local2)
        If(Arg1){                 //Enable GP to IOAPIC
          Or(TEMP, Local2, TEMP)
        } Else{                   //mask
          And(TEMP, Not(Local2), TEMP)
        }
      }
    }

  } // End of Scope SB

scope (\_SB.PCI0) {
  Name(LTRE, 0)
  Name(OBFF, 0)

  Name(LMSL, 0)
  Name(LNSL, 0)
  //
  // LAN Controller - Device 25, Function 0
  //
  Device(GLAN) { // GbE Controller
    Name(_ADR, 0x00190000)
    Method(_PRW, 0) { Return(GPRW(0x0D, 4)) }  // can wakeup from S4 state
  } // end "GbE Controller"

  //
  // EHCI Controller #1 - Device 29, Function 0
  //
  Device(EHC1) {
    Name(_ADR, 0x001D0000)
    include("PchEhci1.asl")
    Method(_PRW, 0) { Return(GPRW(0x0D, 4)) }  // can wakeup from S4 state
  } // end "EHCI Controller #1"

  //
  // EHCI Controller #2 - Device 26, Function 0
  //
  Device(EHC2) {
    Name(_ADR, 0x001A0000)
    include("PchEhci2.asl")
    Method(_PRW, 0) { Return(GPRW(0x0D, 4)) }  // can wakeup from S4 state
  } // end "EHCI Controller #2"

  //
  // xHCI Controller - Device 20, Function 0
  //
  Device(XHC) {
    Name(_ADR, 0x00140000)
    include("PchXhci.asl")
    Method(_PRW, 0) { Return(GPRW(0x0D, 4)) }  // can wakeup from S4 state
  } // end "xHCI Controller"

  //
  // High Definition Audio Controller - Device 27, Function 0
  //
  Device(HDEF) {
    Name(_ADR, 0x001B0000)
    include("PchAudio.asl")
    Method(_PRW, 0) { Return(GPRW(0x0D, 4)) }  // can wakeup from S4 state
  } // end "High Definition Audio Controller"

#ifdef SERIAL_IO_FLAG
    //
    //  Serial IO Controllers definitions
    //
    include ("PchSerialIo.asl")
    include ("SerialIoDevices.asl")
#endif // SERIAL_IO_FLAG 
  
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdAdspFlag)  
//[-end-130812-IB05670190-modify]//
  //
  //  Audio DSP Device definition - Device 19, Function 0
  //
  Device(ADSP) {
    Name(_ADR, 0)
    include("PchAudioDsp.asl")
  }
#endif // FeaturePcdGet(PcdAdspFlag)
 
  //
  // PCIE Root Port #1
  //
  Device(RP01) {
    Method (_ADR, 0) { Return (RPA0) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR1, LTRE)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      Store (OBF1, OBFF)
//[-start-130225-IB03780481-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 0))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130225-IB03780481-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR04) }// APIC mode
//      Return (PR04) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x04 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B04PRT.asl")
    Method(_PRT,0) {
		If(PICM) { Return(AR04) }// APIC mode
		Return (PR04) // PIC mode
    }
// _PRT - BUS 0x04 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//

//[-start-130307-IB03780481-modify]//
//[-start-120730-IB03780456-modify]//
//[-start-120523-IB03780443-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU4) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 0)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-120523-IB03780443-add]//
//[-end-120730-IB03780456-modify]//
//[-end-130307-IB03780481-modify]//
  } // end "PCIE Root Port #1"

  //
  // PCIE Root Port #2
  //
  Device(RP02) {
    Method (_ADR, 0) { Return (RPA1) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR2, LTRE)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      Store (OBF2, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 1))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR05) }// APIC mode
//      Return (PR05) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x05 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B05PRT.asl")
    Method(_PRT, 0) {
		If(PICM) { Return(AR05) } // APIC mode
		Return (PR05)             // PIC mode
    }
// _PRT - BUS 0x05 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU2) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 1)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//
  } // end "PCIE Root Port #2"

  //
  // PCIE Root Port #3
  //
  Device(RP03) {
    Method (_ADR, 0) { Return (RPA2) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR3, LTRE)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      Store (OBF3, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 2))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR06) }// APIC mode
//      Return (PR06) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x06 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B06PRT.asl")
    Method(_PRT, 0) {
    	If(PICM) { Return(AR06) } // APIC mode
      	Return (PR06)             // PIC mode
    }
// _PRT - BUS 0x06 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU3) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 2)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//

  } // end "PCIE Root Port #3"

  //
  // PCIE Root Port #4
  //
  Device(RP04) {
    Method (_ADR, 0) { Return (RPA3) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR4, LTRE)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      Store (OBF4, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 3))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR07) }// APIC mode
//      Return (PR07) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x07 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B07PRT.asl")
    Method(_PRT, 0) {
    	If(PICM) { Return(AR07) } // APIC mode
    	Return (PR07)             // PIC mode
    }
// _PRT - BUS 0x07 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU4) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 3)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//
  } // end "PCIE Root Port #4"

  //
  // PCIE Root Port #5
  //
  Device(RP05) {
    Method (_ADR, 0) { Return (RPA4) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR5, LTRE)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      Store (OBF5, OBFF)
//[-start-130225-IB03780481-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 4))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130225-IB03780481-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR08) }// APIC mode
//      Return (PR08) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x08 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B08PRT.asl")
    Method(_PRT, 0) {
  		If(PICM) { Return(AR08) } // APIC mode
  		Return (PR08)             // PIC mode
    }
// _PRT - BUS 0x08 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//

//[-start-120523-IB03780443-remove]//
//#if FeaturePcdGet(PcdThunderBoltSupported)
//    Device(HRUP)
//    {
//      Name(_ADR, 0x00)
//      Name(_PRW, Package () {0x09, 0x04})
//      Device(HRDN)
//      {
//        Name(_ADR, 0x00040000)
//        Name(_PRW, Package () {0x09, 0x04})
//        Device(EPUP)
//        {
//          Name(_ADR, 0x00)
//          Method(_RMV)
//          {
//            Return (0x01)
//          }
//        }
//      }
//    }
//#endif
//[-end-120523-IB03780443-remove]//
//[-start-130307-IB03780481-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU4) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 4)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130307-IB03780481-modify]//
  } // end "PCIE Root Port #5"

  //
  // PCIE Root Port #6
  //
  Device(RP06) {
    Method (_ADR, 0) { Return (RPA5) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR6, LTRE)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
      Store (OBF6, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 5))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR09) }// APIC mode
//      Return (PR09) // PIC Mode
//    } // end _PRT
//[-start-130726-IB06720229-modify]//
//[-start-130508-IB06720225-modify]//
// _PRT - BUS 0x09 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B09PRT.asl")
    Method(_PRT, 0) {
      If (LEqual (And (CDID, 0xF000), 0x8000)) { // LPT-H
        If (PICM) { Return(AR09) } // APIC mode
         Return (PR09)             // PIC mode
      } Else { // ULT
        If (PICM) { Return (AR08) }// APIC mode
         Return (PR08) // PIC mode
      }
    }
// _PRT - BUS 0x09 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU6) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 5)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//
  } // end "PCIE Root Port #6"

  //
  // PCIE Root Port #7
  //
  Device(RP07) {
    Method (_ADR, 0) { Return (RPA6) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR7, LTRE)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
      Store (OBF7, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 6))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR0E) } // APIC mode
//      Return (PR0E) // PIC Mode
//    } // end _PRT
//[-start-130726-IB06720229-modify]//
//[-start-130604-IB06720225-modify]//
// _PRT - BUS 0x14 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B14PRT.asl")
    Method(_PRT, 0) {
    	If(PICM) { Return(AR0E) } // APIC mode
  		Return (PR0E)             // PIC mode
    }
// _PRT - BUS 0x14 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130604-IB06720225-modify]//
//[-end-130726-IB06720229-modify]//
//[-end-120326-IB05330332-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU7) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 6)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//
  } // end "PCIE Root Port #7"

  //
  // PCIE Root Port #8
  //
  Device(RP08) {
    Method (_ADR, 0) { Return (RPA7) }
    //
    // Pass LTRx to LTRE so PchPcie.asl can be reused for PCIes.
    //
    Method(_INI)
    {
      Store (LTR8, LTRE)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
      Store (OBF8, OBFF)
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      //
      // Call OSUP() when Thuderbolt Chip is RR.
      //
      If (LAnd (LEqual (TBTC, 1), LEqual (TRPN, 7))) {
        // Get memory mapped IO address of RR host router upstream port
        Store (\_GPE.MMTB (), Local3)
        // Call OSUP
        \_GPE.OSUP (Local3)
      }
#endif
//[-end-130709-IB05400426-add]//
    }
    include("PchPcie.asl")
    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) { Return(AR0F) }// APIC mode
//      Return (PR0F) // PIC Mode
//    } // end _PRT
//[-start-130723-IB06720229-modify]//
//[-start-130604-IB06720225-modify]//
// _PRT - BUS 0x15 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B15PRT.asl")
    Method(_PRT, 0) {
    	If(PICM) { Return(AR0F) } // APIC mode
  		Return (PR0F)             // PIC mode
    }
// _PRT - BUS 0x15 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130604-IB06720225-modify]//
//[-end-130723-IB06720229-modify]//
//[-start-130709-IB05400426-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
    Device(HRU8) // Host router Upstream port
    {
      Name (_ADR, 0x00000000)
      Method (_RMV)
      {
        If (LEqual (TRPN, 7)) {
          Return (1)
        }
      } // end _RMV
    }
#endif
//[-end-130709-IB05400426-add]//
//[-end-120326-IB05330332-modify]//
  } // end "PCIE Root Port #8"

  //
  // Serial ATA Host Controller - Device 31, Function 2
  //

  // PCH SATA Controller
  Device (SAT0)
  {
    //Bus 0x00, Device 0x1F, Function 0x02
    Name(_ADR, 0x001F0002)
    Name(FDEV, Zero)
    Name(FDRP, Zero)

    Method(_DEP){
      ADBG("SAT0 DEP Call")

      If(LGreaterEqual(OSYS,2013)) {                               // PEP SATA Constraint for WinBlue and newer version of WinOS
        If(LAnd (LEqual(S0ID, 1), LNotEqual(And(PEPC, 0x03), 0))){ // PEPC Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
          // SATA PEP not set to No Constraint
          ADBG("SAT0 DEP")
          Return(Package() {\_SB.PEPD})
        }
      }

      ADBG("SAT0 DEP NULL")
      Return(Package() {})                                         // No dependency for other OS (non-WinBlue)
    }

    Device(PRT0)
    {
      Name(_ADR,0x0000FFFF)  // Port 0
      Method(_SDD,1, Serialized)
      {
        CreateByteField(Arg0, 157, BFDS)
        ToInteger(BFDS, FDEV)
        CreateByteField(Arg0, 154, BFRP)
        ToInteger(BFRP, FDRP)
      }
      //Get Task File
      Method(_GTF,0,Serialized)
      {
        //Set Feature Command to enable DevSlp
        If (LAnd(LAnd(LEqual(DVS0, 1), LEqual(And(FDEV, 0x01), 0x01)), LEqual(And(FDRP, 0x80), 0x80))) {
          Name(PIB1, Buffer(7)
           {0x10, 0x09, 0x00, 0x00, 0x00, 0xB0, 0xEF })
          Return (PIB1)
        }
        Name(PIB2, Buffer(7)
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 })
        Return (PIB2)
      }
    }
    Device(PRT1)
    {
      Name(_ADR,0x0001FFFF)  // Port 1
      Name(FDEV, Zero)
      Name(FDRP, Zero)
      Method(_SDD,1, Serialized)
      {
        CreateByteField(Arg0, 157, BFDS)
        ToInteger(BFDS, FDEV)
        CreateByteField(Arg0, 154, BFRP)
        ToInteger(BFRP, FDRP)
      }
      // Get Task File
//[-start-130617-IB08050212-modify]//
      Method(_GTF,0,Serialized)
//[-end-130617-IB08050212-modify]//
      {
        //Set Feature Command to enable DevSlp
        If (LAnd(LAnd(LEqual(DVS1, 1), LEqual(And(FDEV, 0x01), 0x01)), LEqual(And(FDRP, 0x80), 0x80))) {
          Name(PIB1, Buffer(7)
           {0x10, 0x09, 0x00, 0x00, 0x00, 0xB0, 0xEF })
          Return (PIB1)
        }
        Name(PIB2, Buffer(7)
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 })
        Return (PIB2)
      }
    }
    Device(PRT2)
    {
      Name(_ADR,0x0002FFFF)  // Port 1
      Name(FDEV, Zero)
      Name(FDRP, Zero)
      Method(_SDD,1, Serialized)
      {
        CreateByteField(Arg0, 157, BFDS)
        ToInteger(BFDS, FDEV)
        CreateByteField(Arg0, 154, BFRP)
        ToInteger(BFRP, FDRP)
      }
      // Get Task File
//[-start-130617-IB08050212-modify]//
      Method(_GTF,0,Serialized)
//[-end-130617-IB08050212-modify]//
      {
        //Set Feature Command to enable DevSlp
        If (LAnd(LAnd(LEqual(DVS2, 1), LEqual(And(FDEV, 0x01), 0x01)), LEqual(And(FDRP, 0x80), 0x80))) {
          Name(PIB1, Buffer(7)
           {0x10, 0x09, 0x00, 0x00, 0x00, 0xB0, 0xEF })
          Return (PIB1)
        }
        Name(PIB2, Buffer(7)
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 })
        Return (PIB2)
      }
    }
    Device(PRT3)
    {
      Name(_ADR,0x0003FFFF)  // Port 3
      Name(FDEV, Zero)
      Name(FDRP, Zero)
      Method(_SDD,1, Serialized)
      {
        CreateByteField(Arg0, 157, BFDS)
        ToInteger(BFDS, FDEV)
        CreateByteField(Arg0, 154, BFRP)
        ToInteger(BFRP, FDRP)
      }
      // Get Task File
      Method(_GTF,0,Serialized)
      {
        //Set Feature Command to enable DevSlp
        If (LAnd(LAnd(LEqual(DVS3, 1), LEqual(And(FDEV, 0x01), 0x01)), LEqual(And(FDRP, 0x80), 0x80))) {
          Name(PIB1, Buffer(7)
           {0x10, 0x09, 0x00, 0x00, 0x00, 0xB0, 0xEF })
          Return (PIB1)
        }
        Name(PIB2, Buffer(7)
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 })
        Return (PIB2)
      }
    }
  }

  Device(SAT1) {
    Name(_ADR,0x001F0005)
    //
    // SATA Methods pulled in via SSDT.
    //
  }

  //
  // SMBus Controller - Device 31, Function 3
  //
  Device(SBUS) {
    Name(_ADR,0x001F0003)
    Include("PchSmb.asl")
  }
}
