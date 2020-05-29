/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the SandyBridge         *;
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

    //
    //  System Bus
    //
    Scope (\_SB.PCI0)
    {

      External(BID)
      External(\_SB.PCI0.I2C0.PS0X, MethodObj)
      External(\_SB.PCI0.I2C0.PS3X, MethodObj)
      External(\_SB.PCI0.I2C1.PS0X, MethodObj)
      External(\_SB.PCI0.SDHC.PS0X, MethodObj)

      Device(SIRC)
      {
        //
        // Serial IO devices occupy 2 address ranges each: BAR0 and BAR1.
        // Each device claims its BAR0 addresses in its own _CRS method
        // BAR1 addresses are not defined inside devices; instead, they are gathered in SIRC device
        // SIRC also contains about half of BAR0 for SDIO - upper half except 0xC bytes which are clamed by WiFi device
        //
        Name(_HID,EISAID("PNP0C02"))

        Name(_UID,4)

        Method(_STA)
        {
          If(LLess(OSYS,2012)) { Return(0x0) } // check for Win7 or older
          If(LEqual(And(CDID,0xF000), 0x8000)) { Return(0x0) } // check for LPT-H chipset
          Return (0xF)
        }

        //
        // Base address of the below memory ranges will be updated with actual addresses during BIOS execution.
        //
        Name(BUF1,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU01) })  // Serial IO SDMA BAR1
        Name(BUF2,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU02) })  // Serial IO I2C0 BAR1
        Name(BUF3,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU03) })  // Serial IO I2C1 BAR1
        Name(BUF4,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU04) })  // Serial IO SPI0 BAR1
        Name(BUF5,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU05) })  // Serial IO SPI1 BAR1
        Name(BUF6,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU06) })  // Serial IO UA00 BAR1
        Name(BUF7,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU07) })  // Serial IO UA01 BAR1
        Name(BUF8,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BU08) })  // Serial IO SDIO BAR1
        Name(BUFL,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00000008, SDLO) })  // Serial IO SDIO BAR0 lower part
        Name(BUFH,ResourceTemplate() { Memory32Fixed (ReadWrite, 0x00000000, 0x00000FEC, SDHI) })  // Serial IO SDIO BAR0 upper part

        Method (_CRS, 0x0, NotSerialized)
        {
          Store(ResourceTemplate() { }, Local0)  // placeholder for concatenated buffers
          CreateDWordField(BUF1, ^BU01._BAS, BR01)
          CreateDWordField(BUF2, ^BU02._BAS, BR02)
          CreateDWordField(BUF3, ^BU03._BAS, BR03)
          CreateDWordField(BUF4, ^BU04._BAS, BR04)
          CreateDWordField(BUF5, ^BU05._BAS, BR05)
          CreateDWordField(BUF6, ^BU06._BAS, BR06)
          CreateDWordField(BUF7, ^BU07._BAS, BR07)
          CreateDWordField(BUF8, ^BU08._BAS, BR08)
          //
          // concatenate all buffers with non-zero address into Local0
          //
          If(LNotEqual(BR01, 0)) { ConcatenateResTemplate(Local0, BUF1, Local0) }
          If(LNotEqual(BR02, 0)) { ConcatenateResTemplate(Local0, BUF2, Local0) }
          If(LNotEqual(BR03, 0)) { ConcatenateResTemplate(Local0, BUF3, Local0) }
          If(LNotEqual(BR04, 0)) { ConcatenateResTemplate(Local0, BUF4, Local0) }
          If(LNotEqual(BR05, 0)) { ConcatenateResTemplate(Local0, BUF5, Local0) }
          If(LNotEqual(BR06, 0)) { ConcatenateResTemplate(Local0, BUF6, Local0) }
          If(LNotEqual(BR07, 0)) { ConcatenateResTemplate(Local0, BUF7, Local0) }

          If(LNotEqual(BR08, 0)) {
            ConcatenateResTemplate(Local0, ^BUF8, Local0)
            //
            // Calculate regions occupied by SDIO's BAR0
            //
            OperationRegion(SDCH, SystemMemory, BR08, 0x40) // SDHC PCI Config Header
            Field(SDCH, DWordAcc, NoLock, Preserve) {
              Offset(0x10),
              BAR0, 32
            }

            CreateDWordField(^BUFL, ^SDLO._BAS, LBAS)
            CreateDWordField(^BUFH, ^SDHI._BAS, HBAS)
            Add(BAR0, 0x1000, LBAS)
            Add(BAR0, 0x1014, HBAS)

            ConcatenateResTemplate(Local0, BUFL, Local0)
            ConcatenateResTemplate(Local0, BUFH, Local0)
          }

          Return (Local0)
        } //end _CRS

        Method(CNTR,1,Serialized)
        {
          Switch (ToInteger(Arg0))
          {
            Case (1) { CreateDWordField(^BUF1,^BU01._BAS, BAR1); Return(BAR1) }
            Case (2) { CreateDWordField(^BUF2,^BU02._BAS, BAR2); Return(BAR2) }
            Case (3) { CreateDWordField(^BUF3,^BU03._BAS, BAR3); Return(BAR3) }
            Case (4) { CreateDWordField(^BUF4,^BU04._BAS, BAR4); Return(BAR4) }
            Case (5) { CreateDWordField(^BUF5,^BU05._BAS, BAR5); Return(BAR5) }
            Case (6) { CreateDWordField(^BUF6,^BU06._BAS, BAR6); Return(BAR6) }
            Case (7) { CreateDWordField(^BUF7,^BU07._BAS, BAR7); Return(BAR7) }
            Case (8) { CreateDWordField(^BUF8,^BU08._BAS, BAR8); Return(BAR8) }
            Default { Return (0xFFFFFFFF) }
          }
        } //end CNTR

      }  // end of SIRC

      //----------------------------
      //  Serial IO GPIO Controller
      //----------------------------
      Device (GPI0)
      {
        Name (_HID, "INT33C7")
        Name (_CID, "INT33C7")
        Name (_UID, 1)

        Name (RBUF, ResourceTemplate ()
        {
          //
          // BIOS specific change.
          // Provide Min & Max IO range addresses
          // BIOS to update AddressMinimum & AddressMaximum fields
          // dynamically after PCI enumeration.
          //
          DWordIo (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000, 0x0000, 0x03FF, 0x0000, 0x0400,,, BAR0)
        })


        Method (_CRS, 0x0, NotSerialized)
        {

          Return (RBUF)
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._MIN,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }
      }   //  Device (GPI0)

      //---------------------------
      //  Serial IO DMA Controller
      //---------------------------
      Device (SDMA)
      {
        Name (_HID, "INTL9C60")
        Name (_UID, 1)
        Name (_ADR, 0x00150000)

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {20}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Return (RBUF)
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        Method (PTD3, 0x0, NotSerialized) {
          //
          // put DMA hardware in D3
          //
          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(1), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(1),0x84 ,Local0)
            OperationRegion(DMB1, SystemMemory, Local0, 4)
            Field(DMB1, DWordAcc, NoLock, Preserve) { TEMP, 32 }
            Or(TEMP, 0x3, TEMP)
          }
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // OS older than Win 8? hide the device
          If(LAnd(LEqual(DOSD, 2), LEqual(OSYS,2012))) { PTD3(); Return(0x0) } // "auto" mode & Win8? hide the device
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

      }   //  Device (SDMA)


      //----------------------------
      //  Serial IO I2C0 Controller
      //----------------------------
      Device (I2C0)
      {
        Name (_HID, "INT33C2")
        Name (_CID, "INT33C2")
        Name (_UID, 1)
        Name (_ADR, 0x00150001)

        Method(_DEP){
          ADBG("I2C0 DEP Call")
          If(LEqual(S0ID, 1)){
            ADBG("I2C0 DEP")
            Return(Package() {\_SB.PEPD})
          }Else{
            ADBG("I2C0 DEP NULL")
            Return(Package() {})
          }
        }

        Method (SSCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 432, 507, 9 })
          Store(SSH0, Index(PKG,0))
          Store(SSL0, Index(PKG,1))
          Store(SSD0, Index(PKG,2))
           Return (PKG)
        }
        Method (FMCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 72, 160, 9 })
          Store(FMH0, Index(PKG,0))
          Store(FML0, Index(PKG,1))
          Store(FMD0, Index(PKG,2))
           Return (PKG)
        }
        Method (FPCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 26, 50, 5 })
          Store(FPH0, Index(PKG,0))
          Store(FPL0, Index(PKG,1))
          Store(FPD0, Index(PKG,2))
           Return (PKG)
        }
        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C0, Index(PKG,0))
           Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 2000 })
          Store(M1C0, Index(PKG,0))
           Return (PKG)
        }
        Method (M0D0, 0x0, Notserialized)
        {
          Name (PKG, Package(1) {  0 })
          Store(M2C0, Index(PKG,0))
           Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Name (DBUF, ResourceTemplate ()
          {
            //
            // Provide Serial IO DMA channels used by the I2C0 controller
            //
            FixedDMA(0x18, 4, Width32Bit, DMA1) //Tx
            FixedDMA(0x19, 5, Width32Bit, DMA2) //Rx
          })
          If(LNotEqual(\_SB_.PCI0.SDMA._STA, 0x0)) {
            Return (ConcatenateResTemplate(RBUF, DBUF))
          } Else {
            Return (RBUF)
          }
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for I2C0
        Method(_PS0,0,Serialized)
        {
          ADBG("I2C0 Ctrlr D0")
          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(2), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(2),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }
          If(CondRefOf(\_SB.PCI0.I2C0.PS0X))
          {
            \_SB.PCI0.I2C0.PS0X()
          }
        }

        // D3 Method for I2C0
        Method(_PS3,0,Serialized)
        {
          ADBG("I2C0 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(2), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(2),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
          If(CondRefOf(\_SB.PCI0.I2C0.PS3X))
          {
            \_SB.PCI0.I2C0.PS3X()
          }

        }

      }  //  Device (I2C0)

      //----------------------------
      //  Serial IO I2C1 Controller
      //----------------------------
      Device (I2C1)
      {
        Name (_HID, "INT33C3")
        Name (_CID, "INT33C3")
        Name (_UID, 2)
        Name (_ADR, 0x00150002)

        Method(_DEP){
          ADBG("I2C1 DEP Call")
          If(LEqual(S0ID, 1)){
            ADBG("I2C1 DEP")
            Return(Package() {\_SB.PEPD})
          }Else{
            ADBG("I2C1 DEP NULL")
            Return(Package() {})
          }
        }

        Method (SSCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 432, 507, 9 })
          Store(SSH1, Index(PKG,0))
          Store(SSL1, Index(PKG,1))
          Store(SSD1, Index(PKG,2))
          Return (PKG)
        }
        Method (FMCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 72, 160, 9 })
          Store(FMH1, Index(PKG,0))
          Store(FML1, Index(PKG,1))
          Store(FMD1, Index(PKG,2))
          Return (PKG)
        }
        Method (FPCN, 0x0, NotSerialized)
        {
          Name (PKG, Package(3) { 26, 50, 5 })
          Store(FPH1, Index(PKG,0))
          Store(FPL1, Index(PKG,1))
          Store(FPD1, Index(PKG,2))
          Return (PKG)
        }
        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C1, Index(PKG,0))
          Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 2000 })
          Store(M1C1, Index(PKG,0))
          Return (PKG)
        }
        Method (M0D0, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 0 })
          Store(M2C1, Index(PKG,0))
          Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Name (DBUF, ResourceTemplate ()
          {
            FixedDMA(0x1A, 6, Width32Bit, DMA1) //Tx
            FixedDMA(0x1B, 7, Width32Bit, DMA2) //Rx
          })

          If(LNotEqual(\_SB_.PCI0.SDMA._STA, 0x0)) {
            Return (ConcatenateResTemplate(RBUF, DBUF))
          } Else {
            Return (RBUF)
          }
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for I2C1
        Method(_PS0,0,Serialized)
        {
          ADBG("I2C1 Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(3), 0)) {
            If(CondRefOf(\_SB.PCI0.I2C1.PS0X))
            {
              \_SB.PCI0.I2C1.PS0X()
            }
            Add(\_SB.PCI0.SIRC.CNTR(3),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }
        }

        // D3 Method for I2C1
        Method(_PS3,0,Serialized)
        {
          ADBG("I2C1 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(3), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(3),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }
      }  //  Device (I2C1)

      //----------------------------
      //  Serial IO SPI0 Controller
      //----------------------------
      Device (SPI0)
      {
        Name (_HID, "INT33C0")
        Name (_CID, "INT33C0")
        Name (_UID, 1)
        Name (_ADR, 0x00150003)

        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C2, Index(PKG,0))
          Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 2000 })
          Store(M1C2, Index(PKG,0))
          Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Return (RBUF)
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)

        }

        // D0 Method for SPI0
        Method(_PS0,0,Serialized)
        {
          ADBG("SPI0 Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(4), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(4),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }
        }

        // D3 Method for SPI0
        Method(_PS3,0,Serialized)
        {
          ADBG("SPI0 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(4), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(4),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }
      }   //  Device (SPI0)

      //----------------------------
      //  Serial IO SPI1 Controller
      //----------------------------
      Device (SPI1)
      {
        Name (_HID, "INT33C1")
        Name (_CID, "INT33C1")
        Name (_UID, 2)
        Name (_ADR, 0x00150004)


        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C3, Index(PKG,0))
          Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 2000 })
          Store(M1C3, Index(PKG,0))
          Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Name (DBUF, ResourceTemplate ()
          {
            FixedDMA(0x10, 0, Width32Bit, DMA1) //Tx
            FixedDMA(0x11, 1, Width32Bit, DMA2) //Rx
          })

          If(LNotEqual(\_SB_.PCI0.SDMA._STA, 0x0)) {
            Return (ConcatenateResTemplate(RBUF, DBUF))
          } Else {
            Return (RBUF)
          }
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for SPI1
        Method(_PS0,0,Serialized)
        {
          ADBG("SPI1 Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(5), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(5),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }
        }

        // D3 Method for SPI1
        Method(_PS3,0,Serialized)
        {
          ADBG("SPI1 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(5), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(5),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }
      }   //  Device (SPI1)

      //-----------------------------
      //  Serial IO UART0 Controller
      //-----------------------------
      Device (UA00)
      {
        Name (_HID, "INT33C4")
        Name (_CID, "INT33C4")
        Name (_UID, 1)
        Name (_ADR, 0x00150005)

        Method(_DEP){
          ADBG("UA00 DEP Call")
          If(LEqual(S0ID, 1)){
            ADBG("UA00 DEP")
            Return(Package() {\_SB.PEPD})
          }Else{
            ADBG("UA00 DEP NULL")
            Return(Package() {})
          }
        }

        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C4, Index(PKG,0))
          Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M1C4, Index(PKG,0))
          Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x0001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Return (RBUF)
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for UAR0
        Method(_PS0,0,Serialized)
        {
          ADBG("UAR0 Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(6), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(6),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }
        }

        // D3 Method for UAR0
        Method(_PS3,0,Serialized)
        {
          ADBG("UAR0 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(6), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(6),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }


      }  //  Device (UART0)

      //-----------------------------
      //  Serial IO UART1 Controller
      //-----------------------------
      Device (UA01)
      {
        Name (_HID, "INT33C5")
        Name (_CID, "INT33C5")
        Name (_UID, 2)
        Name (_ADR, 0x00150006)

        Method(_DEP){
          ADBG("UA01 DEP Call")
          If(LEqual(S0ID, 1)){
            ADBG("UA01 DEP")
            Return(Package() {\_SB.PEPD})
          }Else{
            ADBG("UA01 DEP NULL")
            Return(Package() {})
          }
        }

        Method (M0D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M0C5, Index(PKG,0))
          Return (PKG)
        }
        Method (M1D3, 0x0, Notserialized)
        {
          Name (PKG, Package(1) { 200 })
          Store(M1C5, Index(PKG,0))
          Return (PKG)
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x0001000, BAR0)
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {21}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Name (DBUF, ResourceTemplate ()
          {
            FixedDMA(0x16, 2, Width32Bit, DMA1) //Tx
            FixedDMA(0x17, 3, Width32Bit, DMA2) //Rx
          })

          If(LNotEqual(\_SB_.PCI0.SDMA._STA, 0x0)) {
            Return (ConcatenateResTemplate(RBUF, DBUF))
          } Else {
            Return (RBUF)
          }
        }

        Method (_HRV)
        {
           //
           // Report LPC Revision Id
           //
           Return (CRID)
        }

        CreateDWordField(RBUF,BAR0._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for UAR1
        Method(_PS0,0,Serialized)
        {
          ADBG("UAR1 Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(7), 0)) {
           Add(\_SB.PCI0.SIRC.CNTR(7),0x84 ,Local0)
           OperationRegion(ICB1, SystemMemory, Local0, 4)
           Field(ICB1, DWordAcc, NoLock, Preserve) {
             Offset(0x0),
             TEMP, 32
           }
           And(TEMP, 0xFFFFFFFC, TEMP)
           Store(TEMP, Local0)
         }
        }

        // D3 Method for UAR1
        Method(_PS3,0,Serialized)
        {
          ADBG("UAR1 Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(7), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(7),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }
      }  //  Device (UART1)

      //--------------------------------
      // Serial IO SDIO Host Controller
      //--------------------------------
      Device (SDHC)
      {
        Name (_HID, "INT33C6")
        Name (_CID, "PNP0D40")
        Name (_UID, 1)
        Name (_ADR, 0x00170000)

        Method(_DEP){
          ADBG("SDHC DEP Call")
          If(LEqual(S0ID, 1)){
            ADBG("SDHC DEP")
            Return(Package() {\_SB.PEPD})
          }Else{
            ADBG("SDHC DEP NULL")
            Return(Package() {})
          }
        }

        Name (RBUF, ResourceTemplate ()
        {
          //
          // Resource settings will be overwritten after PCI enumeration
          //
          Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BARA)  // BAR0 Range
          Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {22}
        })

        Method (_CRS, 0x0, NotSerialized)
        {
          Return (RBUF)
        }

        CreateDWordField(RBUF,BARA._BAS,BVAL)

        Method (_STA, 0x0, NotSerialized)
        {
          If(LEqual(BVAL, 0))  { Return(0x0) } // has BAR for this device been programmed by the OS?
          If(LLess(OSYS,2012)) { Return(0x0) } // Win 8 or above?
          If(LEqual(S0ID, 1))  { Return(0xF) } // CS enabled in SETUP?
          Return(0x0)
        }

        // D0 Method for SDHC
        Method(_PS0,0,Serialized)
        {
          ADBG("SDHC Ctrlr D0")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(8), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(8),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            And(TEMP, 0xFFFFFFFC, TEMP)
            Store(TEMP, Local0)
          }

          If(CondRefOf(\_SB.PCI0.SDHC.PS0X))
          {
            \_SB.PCI0.SDHC.PS0X()
          }
        }

        // D3 Method for SDHC
        Method(_PS3,0,Serialized)
        {
          ADBG("SDHC Ctrlr D3")

          If(LNotEqual(\_SB.PCI0.SIRC.CNTR(8), 0)) {
            Add(\_SB.PCI0.SIRC.CNTR(8),0x84 ,Local0)
            OperationRegion(ICB1, SystemMemory, Local0, 4)
            Field(ICB1, DWordAcc, NoLock, Preserve) {
              Offset(0x0),
              TEMP, 32
            }
            Or(TEMP, 0x3, TEMP)
            Store(TEMP, Local0)
          }
        }

      }   //  Device (SDHC)

    }   //  Scope (\_SB.PCI0)
