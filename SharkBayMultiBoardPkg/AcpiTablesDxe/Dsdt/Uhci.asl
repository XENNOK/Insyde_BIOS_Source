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


      OperationRegion(U1CS,PCI_Config,0xC4,0x04)

      Field(U1CS,DWordAcc,NoLock,Preserve)
      {
        U1EN, 2 // USB 1 Wake Enable
      }

      Method(_PSW,1)
      {
        If(Arg0)
        {
          Store(3,U1EN)
        }
        Else
        {
          Store(0,U1EN)
        }
      }

      // The CRB leaves the USB ports on in S3/S4 to allow
      // the ability to Wake from USB.  Therefore, define
      // the below control methods to state D2 entry during
      // the given S-State.

      Method(_S3D,0)
      {
        Return(2)
      }

      Method(_S4D,0)
      {
        Return(2)
      }

      Device(HUBN)
      {
        Name(_ADR,0x00000000)

        Device(PRT1)
        {
          Name(_ADR,0x0000001)  // Port 0
        }

        Device(PRT2)
        {
          Name(_ADR,0x0000002)  // Port 1
        }
      }
