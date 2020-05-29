/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  RapidStart.asl

Abstract:

  RapidStart ACPI ASL code to support OS control

--*/


DefinitionBlock (
  "RapidStart.aml",
  "SSDT",
  0x01,
  "Iffs",
  "IffsAsl",
  0x3000
  )
{

  Scope (\_SB) {
    Device(IFFS) {
      OperationRegion(FFSN,SystemMemory,0xFFFF0000,0xAA55)
      Field(FFSN,AnyAcc,Lock,Preserve)
      {
        FFSA,	8,      // (0) iFFS available events
        FFSS,	8,      // (1) iFFS enabled events
        FFST,	16,     // (2) iFFS wake timer in minutes
        FFSP,	32      // (4) iFFS performance monitoring data
      }

      Name(_HID,EISAID("INT3392"))
      Name(_CID,EISAID("PNP0C02"))

      Method(GFFS,0,Serialized) {
        Return(FFSS)
      }
      Method(SFFS,1,Serialized) {
        And(Arg0, FFSA, FFSS)
        Return(FFSS)
      }
      Method(GFTV,0,Serialized) {
        Return(FFST)
      }
      Method(SFTV,1,Serialized) {
        //
        //  24*60=1440 minutes (24 Hours)
        //
        If (LLessEqual(Arg0,1440)) {
          Store(Arg0,FFST)
        } Else {
          And(FFSS,0xFFFE,FFSS)
          Store(10,FFST)
        }
        Return(FFST)
      }
    }
  }
}
