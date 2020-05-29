/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved    *;
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

      Name (_HID, "INT33C8")
      Name (_CID, "INT33C8")
      Name (_DDN, "Intel(R) Smart Sound Technology Host Controller - INT33C8" )
      Name (_UID, 1)

      Method(_DEP){
        ADBG("ADSP DEP Call")
        If(LEqual(S0ID, 1)){
          ADBG("ADSP DEP")
          Return(Package() {\_SB.PEPD})
        }Else{
          ADBG("ADSP DEP NULL")
          Return(Package() {})
        }
      }

      // Default parameters values for Realtek codec
      Name (MCLK, Zero)
      Name (SCLK, 0x9)
      Name (SSPM, Zero)

      // Bluetooth support
      Name (ABTH, Zero)

      Name (RBUF, ResourceTemplate ()
      {
        Memory32Fixed (ReadWrite, 0x00000000, 0x00100000, BAR0)  // MMIO 1 - Audio DSP MMIO
        Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR1)  // MMIO 2 - Shadowed PCI Config Space
        Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {3}  //  Audio DSP IRQ
      }) 

      Name (EOD, 1)
      Method (_CRS, 0x0, NotSerialized)
      {
        Switch (ToInteger(CODS))
        {
          Case (0) {
            // Realtek param values
            Store (\_SB.PCI0.I2C0.ACD0.MCLK, \_SB.PCI0.ADSP.MCLK)
            Store (\_SB.PCI0.I2C0.ACD0.SCLK, \_SB.PCI0.ADSP.SCLK)
            Store (\_SB.PCI0.I2C0.ACD0.SSPM, \_SB.PCI0.ADSP.SSPM)
          }
          Case (1) {
            // Cirrus param values
            Store (\_SB.PCI0.I2C0.ACD1.MCLK, \_SB.PCI0.ADSP.MCLK)
            Store (\_SB.PCI0.I2C0.ACD1.SCLK, \_SB.PCI0.ADSP.SCLK)
            Store (\_SB.PCI0.I2C0.ACD1.SSPM, \_SB.PCI0.ADSP.SSPM)
          }
          Case (2) {
            // IDT param values
            Store (\_SB.PCI0.I2C0.ACD2.MCLK, \_SB.PCI0.ADSP.MCLK)
            Store (\_SB.PCI0.I2C0.ACD2.SCLK, \_SB.PCI0.ADSP.SCLK)
            Store (\_SB.PCI0.I2C0.ACD2.SSPM, \_SB.PCI0.ADSP.SSPM)
          }
          Default {
            // Realtek params as default
            Store (\_SB.PCI0.I2C0.ACD0.MCLK, \_SB.PCI0.ADSP.MCLK)
            Store (\_SB.PCI0.I2C0.ACD0.SCLK, \_SB.PCI0.ADSP.SCLK)
            Store (\_SB.PCI0.I2C0.ACD0.SSPM, \_SB.PCI0.ADSP.SSPM)
          }
        }
        Return (RBUF)
      }

      Method (_SRS, 0x1, Serialized)
      {
        Store (1, EOD)
      }

      Method (_STA, 0x0, NotSerialized)
      {
        If(LGreaterEqual(OSYS,2012))
        {
          If(LEqual(S0ID, 1)) 
          {
            CreateDWordField (^RBUF,^BAR0._BAS,BVAL)
            If (LEqual(BVAL, 0))
            {
              Return (0x0)
            }
            If (And (EOD, 0x1, EOD))
            {
              Return (0xf)  // Enabled 1111
            }
            Else
            {
              Return (0xd)  // Disabled 1101
            }
          }
        }
        Return (0x0)
      }

      Method (_DIS, 0x0, NotSerialized)
      {
        Store (0, EOD)        
      }

      Device (I2S0)
      { // I2S Port 0
        Name (_ADR, 0)
      }
      Device (I2S1)
      {  // I2S Port 1
        Name (_ADR, 1)
      }