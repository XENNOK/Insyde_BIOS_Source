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


  OperationRegion(PXCS,PCI_Config,0x00,0x380)
  Field(PXCS,AnyAcc, NoLock, Preserve)
  {      
    Offset(0),
    VDID, 32,
    Offset(0x50), // LCTL - Link Control Register
    L0SE, 1,      // 0, L0s Entry Enabled
    , 3,
    LDIS, 1,
    , 3,
    Offset(0x52), // LSTS - Link Status Register
    , 13,
    LASX, 1,      // 0, Link Active Status
    Offset(0x5A), // SLSTS[7:0] - Slot Status Register
    ABPX, 1,      // 0, Attention Button Pressed
    , 2,
    PDCX, 1,      // 3, Presence Detect Changed
    , 2,
    PDSX, 1,      // 6, Presence Detect State       
    , 1,
    Offset(0x60), // RSTS - Root Status Register
    , 16,
    PSPX, 1,      // 16,  PME Status
    Offset(0xA4),
    D3HT, 2,      // Power State
    Offset(0xD8), // MPC - Miscellaneous Port Configuration Register
    , 30,
    HPEX, 1,      // 30,  Hot Plug SCI Enable
    PMEX, 1,      // 31,  Power Management SCI Enable 
    Offset(0xE2), // RPPGEN - Root Port Power Gating Enable
    , 2,
    L23E, 1,      // 2,   L23_Rdy Entry Request (L23ER)
    L23R, 1,       // 3,   L23_Rdy to Detect Transition (L23R2DT)
    Offset(0x324),
    , 3,
    LEDM, 1        // PCIEDBG.DMIL1EDM
  }
  Field(PXCS,AnyAcc, NoLock, WriteAsZeros)
  {      
    Offset(0xDC), // SMSCS - SMI/SCI Status Register
    , 30,
    HPSX, 1,      // 30,  Hot Plug SCI Status
    PMSX, 1       // 31,  Power Management SCI Status 
  }

  Method(_STA, 0x0, NotSerialized)
  {
    If(LEqual(VDID, 0xFFFFFFFF)){
      Return(0x00)
    } Else {
      Return(0x0F)
    }
  }

  Name(LTRV, Package(){0,0,0,0})
  Name(OPTS, 0) // PCH SETUP options for LTR and OBFF

  //
  // _DSM Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index (0 = Return Supported Functions)
  // Arg3: Package Parameters
  Method(_DSM, 4, Serialized) {
    //
    // Switch based on which unique function identifier was passed in
    //
    Switch(ToInteger(Arg0)) {
      //
      // _DSM Definitions for Latency Tolerance Reporting
      //
      // Arguments:
      // Arg0: UUID: E5C937D0-3553-4d7a-9117-EA4D19C3434D
      // Arg1: Revision ID: 2
      // Arg2: Function Index: 1, 4 or 6
      // Arg3: Empty Package
      //
      // Return:
      // A Package of four integers corresponding with the LTR encoding defined
      // in the PCI Express Base Specification, as follows:
      // Integer 0: Maximum Snoop Latency Scale
      // Integer 1: Maximum Snoop Latency Value
      // Integer 2: Maximum No-Snoop Latency Scale
      // Integer 3: Maximum No-Snoop Latency Value
      // These values correspond directly to the LTR Extended Capability Structure
      // fields described in the PCI Express Base Specification.
      //
      Case(ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D")) {
        //
        // Switch by function index
        //
        Switch(ToInteger(Arg2)) {
          //
          // Function Index:0
          // Standard query - A bitmask of functions supported
          //
          Case (0)
          {
            if (LEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
              Store(1, OPTS) // function 0
              if (LTRE){
                Or(OPTS,0x40,OPTS) // function 6
              }
              if (OBFF){
                Or(OPTS,0x10,OPTS) // function 4
              }
              Return (OPTS) // bitmask of supported functions: 6, 4, 0.
            } else {
              Return (0)
            }
          }
          //
          // Function Index: 4
          //
          Case(4) {
            if (LEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
              if (OBFF){
                Return (Buffer () {0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0}) // OBFF capable, offset 4[08h]
              } else {
                Return (Buffer () {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0})
              }
            }
          }
          //
          // Function Index: 6
          // LTR Extended Capability Structure
          //
          Case(6) {
            if (LEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
              if (LTRE){
                if (LOr(LEqual(LMSL, 0xFFFFFFFF),LEqual(LNSL, 0xFFFFFFFF)))
                {
                  if (LEqual (PCHS, 1)) {
                    //PCH-H
                    Store (0x0846, LMSL)
                    Store (0x0846, LNSL)
                  } elseif (LEqual (PCHS, 2)) {
                    //PCH-LP
                    Store (0x1003, LMSL)
                    Store (0x1003, LNSL)
                  }
                }
                Store(And(ShiftRight(LMSL,10),7), Index(LTRV, 0))
                Store(And(LMSL,0x3FF), Index(LTRV, 1))
                Store(And(ShiftRight(LNSL,10),7), Index(LTRV, 2))
                Store(And(LNSL,0x3FF), Index(LTRV, 3))

                Return (LTRV)
              } else {
                Return (0)
              }
            }
          }
        } // End of switch(Arg2)
      } // End of case(ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))
    } // End of switch(Arg0)
    return (Buffer() {0x00})
  } // End of _DSM

  Device(PXSX)
  {
    Name(_ADR, 0x00000000)

    // NOTE:  Any PCIE Hot-Plug dependency for this port is
    // specific to the CRB.  Please modify the code based on
    // your platform requirements.

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
  }

  //
  // PCI_EXP_STS Handler for PCIE Root Port
  //
  Method(HPME,0,Serialized)
  {
    If(PMSX) {
      //
      // Clear the PME SCI status bit with timout
      //
      Store(200,Local0)
      While(Local0) {
        //
        // Clear PME SCI Status
        //
        Store(1, PMSX)
        //
        // If PME SCI Status is still set, keep clearing it.
        // Otherwise, break the while loop.
        //
        If(PMSX) {
          Decrement(Local0)
        } else {
          Store(0,Local0)
        }
      }
      //
      // Notify PCIE Endpoint Devices
      //
      Notify(PXSX, 0x02)
    }
  }
