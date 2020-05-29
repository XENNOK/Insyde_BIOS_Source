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
  //
  // High Definition Audio - Device 27, Function 0
  //
  OperationRegion(HDAR, PCI_Config, 0x4C,0x10)
  Field(HDAR,WordAcc,NoLock,Preserve) {
    Offset(0),  // 0x4C, Dock Control Register
    DCKA,1,    // Dock Attach
    ,7,
    Offset(1),  // 04Dh, Dock Status Register
    DCKM,1,    // Dock Mated
    ,6,
    DCKS,1,    // Docking Supported
    Offset(8),  // 0x54, Power Management Control and Status Register
    , 8,
    PMEE,1,
    , 6,
    PMES,1     // PME Status
  }
