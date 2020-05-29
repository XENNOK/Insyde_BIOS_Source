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



  // NOTE:  The _PDC Implementation is out of the scope of this
  // reference code.  Please see the latest Hyper-Threading Technology
  // Reference Code for complete implementation details.

  Scope(\_PR)
  {
  Processor(CPU0,   // Unique name for Processor 0.
    1,              // Unique ID for Processor 0.
    0x1810,         // CPU0 ACPI P_BLK address = ACPIBASE + 10h.
    6)              // CPU0 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU0 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU0 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU0 DEP NULL")
        Return(Package() {})
      }
    }
  }

  Processor(CPU1,   // Unique name for Processor 1.
    2,              // Unique ID for Processor 1.
    0x1810,
    6)              // CPU1 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU1 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU1 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU1 DEP NULL")
        Return(Package() {})
      }
    }
  }

  Processor(CPU2,   // Unique name for Processor 2.
    3,              // Unique ID for Processor 2.
    0x1810,
    6)              // CPU2 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU2 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU2 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU2 DEP NULL")
        Return(Package() {})
      }
    }
  }

  Processor(CPU3,   // Unique name for Processor 3.
    4,              // Unique ID for Processor 3.
    0x1810,
    6)              // CPU3 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU3 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU3 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU3 DEP NULL")
        Return(Package() {})
      }
    }
  }
  Processor(CPU4,   // Unique name for Processor 4.
    5,              // Unique ID for Processor 4.
    0x1810,
    6)              // CPU4 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU4 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU4 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU4 DEP NULL")
        Return(Package() {})
      }
    }
  }

  Processor(CPU5,   // Unique name for Processor 5.
    6,              // Unique ID for Processor 5.
    0x1810,
    6)              // CPU5 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU5 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU5 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU5 DEP NULL")
        Return(Package() {})
      }
    }
  }
  Processor(CPU6,   // Unique name for Processor 6.
    7,              // Unique ID for Processor 6.
    0x1810,
    6)              // CPU6 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU6 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU6 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU6 DEP NULL")
        Return(Package() {})
      }
    }
  }

  Processor(CPU7,   // Unique name for Processor 7.
    8,              // Unique ID for Processor 7.
    0x1810,
    6)              // CPU7 P_BLK length = 6 bytes.
  {
    Method(_DEP){
      ADBG("CPU7 DEP Call")
      If(LEqual(S0ID, 1)){
        ADBG("CPU7 DEP")
        Return(Package() {\_SB.PEPD})
      }Else{
        ADBG("CPU7 DEP NULL")
        Return(Package() {})
      }
    }
  }

  } // End _PR 


