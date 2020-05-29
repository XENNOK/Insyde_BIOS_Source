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

  Ertm.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/

Scope(\_SB.IETM)
{
    // ETRM (Intel DPTF Participant List)
    //
    // The ETRM object evaluates to a package of packages each of which describes one participant device.
    //
    Name(ETRM, Package() 
    { // TODO: Replace with system-specific ETRM data here
      Package() {\_SB.PCI0.TMEM,            "INT3402",    0x06, "0"}, // Memory Device
      Package() {\_SB.PCI0.LPCB.H_EC.TSKN,  "INT3403",    0x06, "0"}, // CPU VR Temperature Sensor
      Package() {\_SB.PCI0.LPCB.H_EC.TAMB,  "INT3403",    0x06, "1"}, // WWAN Temperature Sensor
      Package() {\_SB.PCI0.LPCB.H_EC.TEFN,  "INT3403",    0x06, "3"}, // SSD Board Temperature Sensor
      Package() {\_SB.PCI0.LPCB.H_EC.TINL,  "INT3403",    0x06, "5"}, // Board Fan Inlet Temperature Sensor
      Package() {\_SB.PCI0.LPCB.H_EC.TPWR,  "INT3407",    0x06, "0"}, // Platform Power Device
      Package() {\_SB.PCI0.LPCB.H_EC.TFN1,  "INT3404",    0x06, "0"}, // Fan Control Device
      Package() {\_SB.PCI0.LPCB.H_EC.TFN2,  "INT3404",    0x06, "1"}, // Fan Control Device
      Package() {\_SB.PCI0.DPLY,            "INT3406",    0x06, "0"}, // Display Participant Device
      Package() {\_SB.PCI0.B0D4,            "8086_0A03",  0x00, "0x00040000"}, // HSW ULT SA Thermal Device
      Package() {\_SB.PCI0.TPCH,            "8086_9C24",  0x00, "0x001F0006"}, // LPT ULT PCH Thermal Device
      Package() {\_SB.PCI0.B0D4,            "8086_0D03",  0x00, "0x00040000"}  // CRW SA Thermal Device
    })     
}

