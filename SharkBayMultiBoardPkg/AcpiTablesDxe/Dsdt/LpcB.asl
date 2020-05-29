/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved   *;
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


// LPC Bridge - Device 31, Function 0
// Define the needed LPC registers used by ASL.

scope(\_SB){
      OperationRegion(\_SB.PCI0.LPCB.LPC1, PCI_Config, 0x00, 0x100)
      Field(\_SB.PCI0.LPCB.LPC1, AnyAcc, NoLock, Preserve)
      {
        Offset(0x02),
        CDID,16,
        Offset(0x08),
        CRID, 8,
        Offset(0x60),
        PARC, 8,
        PBRC, 8,
        PCRC, 8,
        PDRC, 8,
        Offset(0x68),
        PERC, 8,
        PFRC, 8,
        PGRC, 8,
        PHRC, 8,
        Offset(0xAC),
        , 8,
        , 8,
        XUSB,   1
      }

        Include ("98_LINK.ASL")
}
      OperationRegion(LPC0, PCI_Config, 0x40, 0xC0)
      Field(LPC0, AnyAcc, NoLock, Preserve)
      {
        Offset(0x40), // 0x80h
        IOD0, 8,
        IOD1, 8,
        Offset(0x78),   // 0xB8h
        , 6,
        GR03,   2,
        , 8,
        GR08,   2,
        GR09,   2,
        GR0A,   2,
        GR0B,   2,
        , 8,
        Offset(0x7C),   // 0xBCh
        , 2,
        GR19,   2,
        , 28,
        Offset(0x0B0),  // 0xF0h
        RAEN,   1,
        ,   13,
        RCBA, 18
      }

      Include ("EC.ASL")
      Include ("LPC_DEV.ASL")
//[-start-120605-IB09330033-add]//
#ifdef H2OIPMI_SUPPORT
      Include ("Ipmi.ASL")
#endif
//[-end-120605-IB09330033-add]//
      Include ("NAT_SIO.ASL")
      Include ("SMSC1007_SIO.ASL")
      Include ("WPCN381U_SIO.asl")
      Include ("SMSC1000_SIO.asl")
      Include ("H8S2113_SIO.ASL")

      // Define the KBC_COMMAND_REG-64, KBC_DATA_REG-60 Registers as an ACPI Operating
      // Region.  These registers will be used to skip kbd mouse 
      // resource settings if not present.
      OperationRegion(PKBS, SystemIO, 0x60, 0x05)
      Field(PKBS, ByteAcc, Lock, Preserve)
      {
        PKBD, 8,
        , 8,
        , 8,
        , 8,
        PKBC, 8
      }
      Device(PS2K)    // PS2 Keyboard
      {
        Name(_HID,"MSFT0001")
        Name(_CID,EISAID("PNP0303"))       

        Method(_STA)
        {
          If(LEqual(BID, BHB))
          {
            Return(0x000F)
          }
          // Only report resources to the OS if the Keyboard is present
          If(LOr(And(LEqual(PKBD,0xFF),    LEqual(PKBC, 0xFF)), LEqual(BID, BFCC)))
          {
            Return(0x0000)
          }
          Return(0x000F)
        }

        Name(_CRS,ResourceTemplate()
        {
          IO(Decode16,0x60,0x60,0x01,0x01)
          IO(Decode16,0x64,0x64,0x01,0x01)
          IRQ(Edge,ActiveHigh,Exclusive){0x01}
        })

        Name(_PRS, ResourceTemplate(){
          StartDependentFn(0, 0) {  
          FixedIO(0x60,0x01)
          FixedIO(0x64,0x01)
          IRQNoFlags(){1}
          }
          EndDependentFn()
        })

      }

      Device(PS2M)    // PS/2 Mouse
      {
        Name(_HID,"MSFT0003")
        Name(_CID,EISAID("PNP0F03")) 

        Method(_STA)
        {
          If(LOr(LEqual(BID, BHB), LEqual(BID, BFCC)))
          {
            Return(0x0000)
          }

          If (P2ME)
          {          
            Return(0x000F)
          }

          Return(0x0000)
        }

        Name(_CRS,ResourceTemplate()
        {
          IRQ(Edge,ActiveHigh,Exclusive){0x0C}
        })

        Name(_PRS, ResourceTemplate(){
          StartDependentFn(0, 0) { 
          IRQNoFlags(){12} 
          }
          EndDependentFn()
        })
      }


