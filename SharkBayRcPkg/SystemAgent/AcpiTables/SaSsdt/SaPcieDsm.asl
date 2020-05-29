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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/   

  Name(LTRV, Package(){0,0,0,0})
  Name(OPTS, 0) // SA SETUP options for LTR and OBFF

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
      // Arg2: Function Index: 6
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
              if (LTRS){
                Or(OPTS,0x40,OPTS) // function 6
              }
              if (OBFS){
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
              if (OBFS){
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
              if (LTRS){
                Store(And(ShiftRight(SMSL,10),7), Index(LTRV, 0))
                Store(And(SMSL,0x3FF), Index(LTRV, 1))
                Store(And(ShiftRight(SNSL,10),7), Index(LTRV, 2))
                Store(And(SNSL,0x3FF), Index(LTRV, 3))
                return (LTRV)
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
