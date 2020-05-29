/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  Trt.asl

Abstract:

  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

--*/

Scope(\_SB.IETM)
{

    Name(TRT0, Package()
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           20,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.B0D4,           10,    100,   0, 0, 0, 0},

      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.TPCH,           30,     50,   0, 0, 0, 0},

      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TSKN, 20,    300,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TSKN,  1,    300,   0, 0, 0, 0},

      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TAMB, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.TMEM,       \_SB.PCI0.LPCB.H_EC.TAMB,  1,    200,   0, 0, 0, 0},

      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.TINL,  20,   200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.DPLY,       \_SB.PCI0.LPCB.H_EC.TINL,  20,   200,   0, 0, 0, 0}
    })     

    // TRTR (Thermal Relationship Table Revision)
    //
    // This object evaluates to an integer value that defines the revision of the _TRT object. 
    //
    // Arguments: (0)
    //   None
    // Return Value:
    // 0: Traditional TRT as defined by the ACPI Specification.
    // 1: Priority based TRT
    //
    Method(TRTR)
    {
      Return(TRTV)
    }

    // _TRT (Thermal Relationship Table)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Thermal Relationship Packages as described below.
    //
    // Return Value Information
    //   Package {
    //   ThermalRelationship[0] // Package
    //    ...
    //   ThermalRelationship[n] // Package
    //   }
    //
    Method(_TRT,,,,PkgObj)
    {
      Return(TRT0)
    } // End _TRT 
}

