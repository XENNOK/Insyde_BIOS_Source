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


Scope(\_SB) {
//[-start-120326-IB05330332-remove]//
//  Name(PR00, Package(){
//// PCI Bridge
//// LPC Bridge
//    Package(){0x001FFFFF, 0, LNKF, 0 },
//    Package(){0x001FFFFF, 1, LNKD, 0 },
//    Package(){0x001FFFFF, 2, LNKC, 0 },
//    Package(){0x001FFFFF, 3, LNKA, 0 },
//// SATA Host Controller1
//// SATA RAID Controller MB
//// SATA RAID Controller DT
//// SMBus Controller
//// CHAP
//// Thermal Controller
//// EHCI Controller #1
//    Package(){0x001DFFFF, 0, LNKH, 0 },
//// EHCI Controller #2
//    Package(){0x001AFFFF, 0, LNKA, 0 },
//// Audio DSP
//    Package(){0x0013FFFF, 0, LNKH, 0 },
//// High Definition Audio Controller
//    Package(){0x001BFFFF, 0, LNKG, 0 },
//// NAND Controller
//    Package(){0x0018FFFF, 0, LNKE, 0 },
//// GbE Controller VPRO
//    Package(){0x0019FFFF, 0, LNKE, 0 },
//// GbE Controller Non-VPRO
//// GbE Controller MB
//// GbE Controller DT
//// ME
//    Package(){0x0016FFFF, 0, LNKA, 0 },
//    Package(){0x0016FFFF, 1, LNKB, 0 },
//    Package(){0x0016FFFF, 2, LNKC, 0 },
//    Package(){0x0016FFFF, 3, LNKD, 0 },
//// PCIE Root Port #1
//    Package(){0x001CFFFF, 0, LNKA, 0 },
//    Package(){0x001CFFFF, 1, LNKB, 0 },
//    Package(){0x001CFFFF, 2, LNKC, 0 },
//    Package(){0x001CFFFF, 3, LNKD, 0 },
//// PCIE Root Port #2
//// PCIE Root Port #3
//// PCIE Root Port #4
//// PCIE Root Port #5
//// PCIE Root Port #6
//// PCIE Root Port #7
//// PCIE Root Port #8
//// Host Bridge
//// P.E.G. Root Port D1F0
//    Package(){0x0001FFFF, 0, LNKA, 0 },
//    Package(){0x0001FFFF, 1, LNKB, 0 },
//    Package(){0x0001FFFF, 2, LNKC, 0 },
//    Package(){0x0001FFFF, 3, LNKD, 0 },
//// P.E.G. Root Port D1F1
//// P.E.G. Root Port D1F2
//// Mobile IGFX
//    Package(){0x0002FFFF, 0, LNKA, 0 },
//// DT IGFX
//// SA Audio Device
//    Package(){0x0003FFFF, 0, LNKA, 0 },
//// SA Thermal Device
//    Package(){0x0004FFFF, 0, LNKA, 0 },
//  })
//  Name(AR00, Package(){
//// PCI Bridge
//// LPC Bridge
//    Package(){0x001FFFFF, 0, 0, 21 },
//    Package(){0x001FFFFF, 1, 0, 19 },
//    Package(){0x001FFFFF, 2, 0, 18 },
//    Package(){0x001FFFFF, 3, 0, 16 },
//// SATA Host Controller1
//// SATA RAID Controller MB
//// SATA RAID Controller DT
//// SMBus Controller
//// CHAP
//// Thermal Controller
//// EHCI Controller #1
//    Package(){0x001DFFFF, 0, 0, 23 },
//// EHCI Controller #2
//    Package(){0x001AFFFF, 0, 0, 16 },
//// Audio DSP
//    Package(){0x0013FFFF, 0, 0, 23 },
//// High Definition Audio Controller
//    Package(){0x001BFFFF, 0, 0, 22 },
//// NAND Controller
//    Package(){0x0018FFFF, 0, 0, 20 },
//// GbE Controller VPRO
//    Package(){0x0019FFFF, 0, 0, 20 },
//// GbE Controller Non-VPRO
//// GbE Controller MB
//// GbE Controller DT
//// ME
//    Package(){0x0016FFFF, 0, 0, 16 },
//    Package(){0x0016FFFF, 1, 0, 17 },
//    Package(){0x0016FFFF, 2, 0, 18 },
//    Package(){0x0016FFFF, 3, 0, 19 },
//// PCIE Root Port #1
//    Package(){0x001CFFFF, 0, 0, 16 },
//    Package(){0x001CFFFF, 1, 0, 17 },
//    Package(){0x001CFFFF, 2, 0, 18 },
//    Package(){0x001CFFFF, 3, 0, 19 },
//// PCIE Root Port #2
//// PCIE Root Port #3
//// PCIE Root Port #4
//// PCIE Root Port #5
//// PCIE Root Port #6
//// PCIE Root Port #7
//// PCIE Root Port #8
//// Host Bridge
//// P.E.G. Root Port D1F0
//    Package(){0x0001FFFF, 0, 0, 16 },
//    Package(){0x0001FFFF, 1, 0, 17 },
//    Package(){0x0001FFFF, 2, 0, 18 },
//    Package(){0x0001FFFF, 3, 0, 19 },
//// P.E.G. Root Port D1F1
//// P.E.G. Root Port D1F2
//// Mobile IGFX
//    Package(){0x0002FFFF, 0, 0, 16 },
//// DT IGFX
//// SA Audio Device
//    Package(){0x0003FFFF, 0, 0, 16 },
//// SA Thermal Device
//    Package(){0x0004FFFF, 0, 0, 16 },
//  })
//  Name(PR04, Package(){
//// PCIE Port #1 Slot
//    Package(){0x0000FFFF, 0, LNKA, 0 },
//    Package(){0x0000FFFF, 1, LNKB, 0 },
//    Package(){0x0000FFFF, 2, LNKC, 0 },
//    Package(){0x0000FFFF, 3, LNKD, 0 },
//  })
//  Name(AR04, Package(){
//// PCIE Port #1 Slot
//    Package(){0x0000FFFF, 0, 0, 16 },
//    Package(){0x0000FFFF, 1, 0, 17 },
//    Package(){0x0000FFFF, 2, 0, 18 },
//    Package(){0x0000FFFF, 3, 0, 19 },
//  })
//  Name(PR05, Package(){
//// PCIE Port #2 Slot
//    Package(){0x0000FFFF, 0, LNKB, 0 },
//    Package(){0x0000FFFF, 1, LNKC, 0 },
//    Package(){0x0000FFFF, 2, LNKD, 0 },
//    Package(){0x0000FFFF, 3, LNKA, 0 },
//  })
//  Name(AR05, Package(){
//// PCIE Port #2 Slot
//    Package(){0x0000FFFF, 0, 0, 17 },
//    Package(){0x0000FFFF, 1, 0, 18 },
//    Package(){0x0000FFFF, 2, 0, 19 },
//    Package(){0x0000FFFF, 3, 0, 16 },
//  })
//  Name(PR06, Package(){
//// PCIE Port #3 Slot
//    Package(){0x0000FFFF, 0, LNKC, 0 },
//    Package(){0x0000FFFF, 1, LNKD, 0 },
//    Package(){0x0000FFFF, 2, LNKA, 0 },
//    Package(){0x0000FFFF, 3, LNKB, 0 },
//  })
//  Name(AR06, Package(){
//// PCIE Port #3 Slot
//    Package(){0x0000FFFF, 0, 0, 18 },
//    Package(){0x0000FFFF, 1, 0, 19 },
//    Package(){0x0000FFFF, 2, 0, 16 },
//    Package(){0x0000FFFF, 3, 0, 17 },
//  })
//  Name(PR07, Package(){
//// PCIE Port #4 Slot
//    Package(){0x0000FFFF, 0, LNKD, 0 },
//    Package(){0x0000FFFF, 1, LNKA, 0 },
//    Package(){0x0000FFFF, 2, LNKB, 0 },
//    Package(){0x0000FFFF, 3, LNKC, 0 },
//  })
//  Name(AR07, Package(){
//// PCIE Port #4 Slot
//    Package(){0x0000FFFF, 0, 0, 19 },
//    Package(){0x0000FFFF, 1, 0, 16 },
//    Package(){0x0000FFFF, 2, 0, 17 },
//    Package(){0x0000FFFF, 3, 0, 18 },
//  })
//  Name(PR08, Package(){
//// PCIE Port #5 Slot
//    Package(){0x0000FFFF, 0, LNKA, 0 },
//    Package(){0x0000FFFF, 1, LNKB, 0 },
//    Package(){0x0000FFFF, 2, LNKC, 0 },
//    Package(){0x0000FFFF, 3, LNKD, 0 },
//  })
//  Name(AR08, Package(){
//// PCIE Port #5 Slot
//    Package(){0x0000FFFF, 0, 0, 16 },
//    Package(){0x0000FFFF, 1, 0, 17 },
//    Package(){0x0000FFFF, 2, 0, 18 },
//    Package(){0x0000FFFF, 3, 0, 19 },
//  })
//  Name(PR09, Package(){
//// PCIE Port #6 Slot
//    Package(){0x0000FFFF, 0, LNKB, 0 },
//    Package(){0x0000FFFF, 1, LNKC, 0 },
//    Package(){0x0000FFFF, 2, LNKD, 0 },
//    Package(){0x0000FFFF, 3, LNKA, 0 },
//  })
//  Name(AR09, Package(){
//// PCIE Port #6 Slot
//    Package(){0x0000FFFF, 0, 0, 17 },
//    Package(){0x0000FFFF, 1, 0, 18 },
//    Package(){0x0000FFFF, 2, 0, 19 },
//    Package(){0x0000FFFF, 3, 0, 16 },
//  })
//  Name(PR0E, Package(){
//// PCIE Port #7 Slot
//    Package(){0x0000FFFF, 0, LNKC, 0 },
//    Package(){0x0000FFFF, 1, LNKD, 0 },
//    Package(){0x0000FFFF, 2, LNKA, 0 },
//    Package(){0x0000FFFF, 3, LNKB, 0 },
//  })
//  Name(AR0E, Package(){
//// PCIE Port #7 Slot
//    Package(){0x0000FFFF, 0, 0, 18 },
//    Package(){0x0000FFFF, 1, 0, 19 },
//    Package(){0x0000FFFF, 2, 0, 16 },
//    Package(){0x0000FFFF, 3, 0, 17 },
//  })
//  Name(PR0F, Package(){
//// PCIE Port #8 Slot
//    Package(){0x0000FFFF, 0, LNKD, 0 },
//    Package(){0x0000FFFF, 1, LNKA, 0 },
//    Package(){0x0000FFFF, 2, LNKB, 0 },
//    Package(){0x0000FFFF, 3, LNKC, 0 },
//  })
//  Name(AR0F, Package(){
//// PCIE Port #8 Slot
//    Package(){0x0000FFFF, 0, 0, 19 },
//    Package(){0x0000FFFF, 1, 0, 16 },
//    Package(){0x0000FFFF, 2, 0, 17 },
//    Package(){0x0000FFFF, 3, 0, 18 },
//  })
//  Name(PR02, Package(){
//// P.E.G. Port Slot x16
//    Package(){0x0000FFFF, 0, LNKA, 0 },
//    Package(){0x0000FFFF, 1, LNKB, 0 },
//    Package(){0x0000FFFF, 2, LNKC, 0 },
//    Package(){0x0000FFFF, 3, LNKD, 0 },
//// Marvel SATA
//  })
//  Name(AR02, Package(){
//// P.E.G. Port Slot x16
//    Package(){0x0000FFFF, 0, 0, 16 },
//    Package(){0x0000FFFF, 1, 0, 17 },
//    Package(){0x0000FFFF, 2, 0, 18 },
//    Package(){0x0000FFFF, 3, 0, 19 },
//// Marvel SATA
//  })
//  Name(PR0A, Package(){
//// P.E.G. Port Slot x8
//    Package(){0x0000FFFF, 0, LNKB, 0 },
//    Package(){0x0000FFFF, 1, LNKC, 0 },
//    Package(){0x0000FFFF, 2, LNKD, 0 },
//    Package(){0x0000FFFF, 3, LNKA, 0 },
//  })
//  Name(AR0A, Package(){
//// P.E.G. Port Slot x8
//    Package(){0x0000FFFF, 0, 0, 17 },
//    Package(){0x0000FFFF, 1, 0, 18 },
//    Package(){0x0000FFFF, 2, 0, 19 },
//    Package(){0x0000FFFF, 3, 0, 16 },
//  })
//  Name(PR0B, Package(){
//// P.E.G. Port Slot x4
//    Package(){0x0000FFFF, 0, LNKC, 0 },
//    Package(){0x0000FFFF, 1, LNKD, 0 },
//    Package(){0x0000FFFF, 2, LNKA, 0 },
//    Package(){0x0000FFFF, 3, LNKB, 0 },
//  })
//  Name(AR0B, Package(){
//// P.E.G. Port Slot x4
//    Package(){0x0000FFFF, 0, 0, 18 },
//    Package(){0x0000FFFF, 1, 0, 19 },
//    Package(){0x0000FFFF, 2, 0, 16 },
//    Package(){0x0000FFFF, 3, 0, 17 },
//  })
//  Name(PR01, Package(){
//// ThimblePeak 1
//    Package(){0x0000FFFF, 0, LNKF, 0 },
//    Package(){0x0000FFFF, 1, LNKG, 0 },
//    Package(){0x0000FFFF, 2, LNKH, 0 },
//    Package(){0x0000FFFF, 3, LNKE, 0 },
//// ThimblePeak 2
//    Package(){0x0001FFFF, 0, LNKG, 0 },
//    Package(){0x0001FFFF, 1, LNKF, 0 },
//    Package(){0x0001FFFF, 2, LNKE, 0 },
//    Package(){0x0001FFFF, 3, LNKH, 0 },
//// ThimblePeak 3
//    Package(){0x0002FFFF, 0, LNKH, 0 },
//    Package(){0x0002FFFF, 1, LNKE, 0 },
//    Package(){0x0002FFFF, 2, LNKF, 0 },
//    Package(){0x0002FFFF, 3, LNKG, 0 },
//// ThimblePeak 4
//    Package(){0x0003FFFF, 0, LNKD, 0 },
//    Package(){0x0003FFFF, 1, LNKC, 0 },
//    Package(){0x0003FFFF, 2, LNKF, 0 },
//    Package(){0x0003FFFF, 3, LNKG, 0 },
//// ThimblePeak 5
//    Package(){0x0004FFFF, 0, LNKC, 0 },
//    Package(){0x0004FFFF, 1, LNKH, 0 },
//    Package(){0x0004FFFF, 2, LNKA, 0 },
//    Package(){0x0004FFFF, 3, LNKE, 0 },
//// ThimblePeak 6
//    Package(){0x0005FFFF, 0, LNKC, 0 },
//    Package(){0x0005FFFF, 1, LNKE, 0 },
//    Package(){0x0005FFFF, 2, LNKG, 0 },
//    Package(){0x0005FFFF, 3, LNKF, 0 },
//  })
//  Name(AR01, Package(){
//// ThimblePeak 1
//    Package(){0x0000FFFF, 0, 0, 21 },
//    Package(){0x0000FFFF, 1, 0, 22 },
//    Package(){0x0000FFFF, 2, 0, 23 },
//    Package(){0x0000FFFF, 3, 0, 20 },
//// ThimblePeak 2
//    Package(){0x0001FFFF, 0, 0, 22 },
//    Package(){0x0001FFFF, 1, 0, 21 },
//    Package(){0x0001FFFF, 2, 0, 20 },
//    Package(){0x0001FFFF, 3, 0, 23 },
//// ThimblePeak 3
//    Package(){0x0002FFFF, 0, 0, 23 },
//    Package(){0x0002FFFF, 1, 0, 20 },
//    Package(){0x0002FFFF, 2, 0, 21 },
//    Package(){0x0002FFFF, 3, 0, 22 },
//// ThimblePeak 4
//    Package(){0x0003FFFF, 0, 0, 19 },
//    Package(){0x0003FFFF, 1, 0, 18 },
//    Package(){0x0003FFFF, 2, 0, 21 },
//    Package(){0x0003FFFF, 3, 0, 22 },
//// ThimblePeak 5
//    Package(){0x0004FFFF, 0, 0, 18 },
//    Package(){0x0004FFFF, 1, 0, 23 },
//    Package(){0x0004FFFF, 2, 0, 16 },
//    Package(){0x0004FFFF, 3, 0, 20 },
//// ThimblePeak 6
//    Package(){0x0005FFFF, 0, 0, 18 },
//    Package(){0x0005FFFF, 1, 0, 20 },
//    Package(){0x0005FFFF, 2, 0, 22 },
//    Package(){0x0005FFFF, 3, 0, 21 },
//  })
//[-end-120326-IB05330332-remove]//
//---------------------------------------------------------------------------
// List of IRQ resource buffers compatible with _PRS return format.
//---------------------------------------------------------------------------
// Naming legend:
// RSxy, PRSy - name of the IRQ resource buffer to be returned by _PRS, "xy" - last two characters of IRQ Link name.
// Note. PRSy name is generated if IRQ Link name starts from "LNK".
// HLxy , LLxy - reference names, can be used to access bit mask of available IRQs. HL and LL stand for active High(Low) Level triggered Irq model.
//---------------------------------------------------------------------------
  Name(PRSA, ResourceTemplate(){  // Link name: LNKA
    IRQ(Level, ActiveLow, Shared, LLKA) {3,4,5,6,10,11,12,14,15}
  })
  Alias(PRSA,PRSB)  // Link name: LNKB
  Alias(PRSA,PRSC)  // Link name: LNKC
  Alias(PRSA,PRSD)  // Link name: LNKD
  Alias(PRSA,PRSE)  // Link name: LNKE
  Alias(PRSA,PRSF)  // Link name: LNKF
  Alias(PRSA,PRSG)  // Link name: LNKG
  Alias(PRSA,PRSH)  // Link name: LNKH
//---------------------------------------------------------------------------
// Begin PCI tree object scope
//---------------------------------------------------------------------------
  Device(PCI0) { // PCI Bridge "Host Bridge"
    Name(_HID, EISAID("PNP0A08")) // Indicates PCI Express/PCI-X Mode2 host hierarchy
    Name(_CID, EISAID("PNP0A03")) // To support legacy OS that doesn't understand the new HID
    Name(_ADR, 0x00000000)
    Method(^BN00, 0){ return(0x0000) }  // Returns default Bus number for Peer PCI busses. Name can be overriden with control method placed directly under Device scope
    Method(_BBN, 0){ return(BN00()) } // Bus number, optional for the Root PCI Bus
    Name(_UID, 0x0000)  // Unique Bus ID, optional
//[-start-120326-IB05330332-modify]//
//    Method(_PRT,0) {
//      If(PICM) {Return(AR00)} // APIC mode
//     Return (PR00) // PIC Mode
//    } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x00 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B00PRT.asl")
    Method(_PRT, 0) {
    	If(PICM) { Return(AR00) } // APIC mode
    	Return (PR00)             // PIC mode
    }
// _PRT - BUS 0x00 - END - TOOL GENERATED. DO NOT MODIFY.    
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//

//[-start-130508-IB06720225-modify]//
// _PRT - BUS 0x02 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B02PRT.asl")
// _PRT - BUS 0x02 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130508-IB06720225-modify]//
//[-start-130725-IB06720229-add]//
// _PRT - BUS 0x0A - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B10PRT.asl")
// _PRT - BUS 0x0A - END - TOOL GENERATED. DO NOT MODIFY.
// _PRT - BUS 0x0B - BEGIN - TOOL GENERATED. DO NOT MODIFY.
    include ("BusPRT/B11PRT.asl")
// _PRT - BUS 0x0B - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130725-IB06720229-add]//

  include("HOST_BUS.ASL")
    Device(P0P1) { // PCI Bridge
      Name(_ADR, 0x001E0000)
      Method(_PRW, 0) {
        If(PMEE) {
          Return (GPRW(0x0B, 4)) // can wakeup from S4 state
        } Else {
          Return (GPRW(0x0B, 0))
        }
      }
//[-start-120326-IB05330332-modify]//
//      Method(_PRT,0) {
//        If(PICM) { Return(AR01) }// APIC mode
//        Return (PR01) // PIC Mode
//      } // end _PRT
//[-start-130508-IB06720225-modify]//
//[-start-130726-IB06720229-modify]//
// _PRT - BUS 0x01 - BEGIN - TOOL GENERATED. DO NOT MODIFY.
      include ("BusPRT/B01PRT.asl")
      Method(_PRT,0) {
 		If(PICM) { Return(AR01) }// APIC mode
 		Return (PR01) // PIC mode
      }
// _PRT - BUS 0x01 - END - TOOL GENERATED. DO NOT MODIFY.
//[-end-130726-IB06720229-modify]//
//[-end-130508-IB06720225-modify]//
//[-end-120326-IB05330332-modify]//

    } // end "PCI Bridge"

    Device(LPCB) { // LPC Bridge
      Name(_ADR, 0x001F0000)
      include("LpcB.asl")
    } // end "LPC Bridge"
    //Include ("EC.ASL")

  } // end PCI0 Bridge "Host Bridge"

//[-start-131225-IB08620313-add]//
#ifdef APEI_SUPPORT
  include ("APEI.asl")
#endif
//[-end-131225-IB08620313-add]//

} // end _SB scope
