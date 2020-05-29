//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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


  // General Purpose Events.  This Scope handles the Run-time and
  // Wake-time SCIs.  The specific method called will be determined by
  // the _Lxx value, where xx equals the bit location in the General
  // Purpose Event register(s).

  
  External(D1F0)
  External(D1F1)
  External(D1F2)
  External(\_SB.PCI0.PEG0.HPME, MethodObj)
  External(\_SB.PCI0.PEG1.HPME, MethodObj)
  External(\_SB.PCI0.PEG2.HPME, MethodObj)

  Scope(\_GPE)
  {
  // Note:
  // Originally, the two GPE methods below are automatically generated, but, for ASL code restructuring,
  // disabled the automatic generation and declare the ASL code here.
  //

  Method(_L6B, 0) {
    Notify(\_SB.PCI0.P0P1, 0x02)
  }


  //
  // This PME event (PCH's GPE #9) is received on one or more of the PCI Express* ports or
  // an assert PMEGPE message received via DMI
  //
  Method(_L69, 0) {
    //
    // If the Root Port is enabled, run PCI_EXP_STS handler
    //
    If(LEqual(RP1D,0))
    {
      \_SB.PCI0.RP01.HPME()
      Notify(\_SB.PCI0.RP01, 0x02)
    }

    If(LEqual(RP2D,0))
    {
      \_SB.PCI0.RP02.HPME()
      Notify(\_SB.PCI0.RP02, 0x02)
    }

    If(LEqual(RP3D,0))
    {
      \_SB.PCI0.RP03.HPME()
      Notify(\_SB.PCI0.RP03, 0x02)
    }

    If(LEqual(RP4D,0))
    {
      \_SB.PCI0.RP04.HPME()
      Notify(\_SB.PCI0.RP04, 0x02)
    }

    If(LEqual(RP5D,0))
    {
      \_SB.PCI0.RP05.HPME()
      Notify(\_SB.PCI0.RP05, 0x02)
    }

    If(LEqual(RP6D,0))
    {
      \_SB.PCI0.RP06.HPME()
      Notify(\_SB.PCI0.RP06, 0x02)
    }

    If(LEqual(RP7D,0))
    {
      \_SB.PCI0.RP07.HPME()
      Notify(\_SB.PCI0.RP07, 0x02)
    }

    If(LEqual(RP8D,0))
    {
      \_SB.PCI0.RP08.HPME()
      Notify(\_SB.PCI0.RP08, 0x02)
    }
    If(LEqual(D1F0,1))
    {
      \_SB.PCI0.PEG0.HPME()
      Notify(\_SB.PCI0.PEG0, 0x02)
      Notify(\_SB.PCI0.PEG0.PEGP, 0x02)
    }

    If(LEqual(D1F1,1))
    {
      \_SB.PCI0.PEG1.HPME()
      Notify(\_SB.PCI0.PEG1, 0x02)
    }

    If(LEqual(D1F2,1))
    {
      \_SB.PCI0.PEG2.HPME()
      Notify(\_SB.PCI0.PEG2, 0x02)
    }
  }

  //
  // This PME event (PCH's GPE #13) is received when any PCH internal device with PCI Power Management capabilities
  // on bus 0 asserts the equivalent of the PME# signal.
  //
  Method(_L6D, 0) {
    If(LAnd(\_SB.PCI0.EHC1.PMES, \_SB.PCI0.EHC1.PMES)){
      Store(1, \_SB.PCI0.EHC1.PMES) //Clear PME status
      Store(0, \_SB.PCI0.EHC1.PMEE) //Disable PME
      Notify(\_SB.PCI0.EHC1, 0x02)
    }
    If(LAnd(\_SB.PCI0.EHC2.PMES, \_SB.PCI0.EHC2.PMES)){
      Store(1, \_SB.PCI0.EHC2.PMES) //Clear PME status
      Store(0, \_SB.PCI0.EHC2.PMEE) //Disable PME
      Notify(\_SB.PCI0.EHC2, 0x02)
    }
    If(LAnd(\_SB.PCI0.XHC.PMES, \_SB.PCI0.XHC.PMES)){
      Store(1, \_SB.PCI0.XHC.PMES) //Clear PME status
      Store(0, \_SB.PCI0.XHC.PMEE) //Disable PME
      Notify(\_SB.PCI0.XHC, 0x02)
    }
    Notify(\_SB.PCI0.HDEF, 0x02)
    Notify(\_SB.PCI0.GLAN, 0x02)
  }

  // PCI Express Hot-Plug caused the wake event.

  Method(_L61)
  {
    Add(L01C,1,L01C)  // Increment L01 Entry Count.

//    P8XH(0,0x01)      // Output information to Port 80h.
//    P8XH(1,L01C)


    // Check Root Port 1 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP1D,0),\_SB.PCI0.RP01.HPSX))
    {
      // Delay for 100ms to meet the timing requirements
      // of the PCI Express Base Specification, Revision
      // 1.0A, Section 6.6 ("...software must wait at
      // least 100ms from the end of reset of one or more
      // device before it is permitted to issue
      // Configuration Requests to those devices").

      P8XH(1,0x01,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count

//[-start-130709-IB05400426-modify]//
//[-start-120503-IB03780443-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 0)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-120503-IB03780443-modify]//
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP01.PDCX)
      {
        // Clear all status bits first.

        Store(1,\_SB.PCI0.RP01.PDCX)
        Store(1,\_SB.PCI0.RP01.HPSX)

        //
        // PCH BIOS Spec Update Rev 1.03, Section 8.9 PCI Express* Hot-Plug BIOS Support
        // In addition, BIOS should intercept Presence Detect Changed interrupt, enable L0s on
        // hot plug and disable L0s on hot unplug. BIOS should also make sure the L0s is
        // disabled on empty slots prior booting to OS.
        //
        If(LNot(\_SB.PCI0.RP01.PDSX)) {
          // The PCI Express slot is empty, so disable L0s on hot unplug
          //
          Store(0,\_SB.PCI0.RP01.L0SE)

        }

//[-start-130709-IB05400426-modify]//
//[-start-120423-IB03780443-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 0)) {
          Notify(\_SB.PCI0.RP01,0)
        }
#else
        Notify(\_SB.PCI0.RP01,0)
#endif
//[-end-120423-IB03780443-modify]//
//[-end-130709-IB05400426-modify]//

      }
      Else
      {
        // False event.  Clear Hot-Plug Status
        // then exit.

        Store(1,\_SB.PCI0.RP01.HPSX)
      }
    }

    // Check Root Port 2 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP2D,0),\_SB.PCI0.RP02.HPSX))
    {
      P8XH(1,0x02,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count
    
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 1)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP02.PDCX)
      {
        Store(1,\_SB.PCI0.RP02.PDCX)
        Store(1,\_SB.PCI0.RP02.HPSX)

        If(LNot(\_SB.PCI0.RP02.PDSX)) {
          Store(0,\_SB.PCI0.RP02.L0SE)
        }
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 1)) {
          Notify(\_SB.PCI0.RP02,0)
        }
#else
        Notify(\_SB.PCI0.RP02,0)
#endif
//[-end-130709-IB05400426-modify]//
      }
      Else
      {
        Store(1,\_SB.PCI0.RP02.HPSX)
      }
    }

    // Check Root Port 3 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP3D,0),\_SB.PCI0.RP03.HPSX))
    {
      P8XH(1,0x03,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count

//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 2)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP03.PDCX)
      {
        Store(1,\_SB.PCI0.RP03.PDCX)
        Store(1,\_SB.PCI0.RP03.HPSX)

        If(LNot(\_SB.PCI0.RP03.PDSX)) {
          Store(0,\_SB.PCI0.RP03.L0SE)
        }
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 2)) {
          Notify(\_SB.PCI0.RP03,0)
        }
#else
        Notify(\_SB.PCI0.RP03,0)
#endif
//[-end-130709-IB05400426-modify]//
      }
      Else
      {
        Store(1,\_SB.PCI0.RP03.HPSX)
      }
    }

    // Check Root Port 4 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP4D,0),\_SB.PCI0.RP04.HPSX))
    {
      P8XH(1,0x04,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count
    
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 3)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP04.PDCX)
      {
        Store(1,\_SB.PCI0.RP04.PDCX)
        Store(1,\_SB.PCI0.RP04.HPSX)

        If(LNot(\_SB.PCI0.RP04.PDSX)) {
          Store(0,\_SB.PCI0.RP04.L0SE)
        }
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 3)) {
          Notify(\_SB.PCI0.RP04,0)
        }
#else
        Notify(\_SB.PCI0.RP04,0)
#endif
//[-end-130709-IB05400426-modify]//
      }
      Else
      {
        Store(1,\_SB.PCI0.RP04.HPSX)
      }
    }

    // Check Root Port 5 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP5D,0),\_SB.PCI0.RP05.HPSX))
    {
      P8XH(1,0x05,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count
    
//[-start-130709-IB05400426-modify]//
//[-start-120523-IB03780443-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 4)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-120523-IB03780443-modify]//
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP05.PDCX)
      {
        Store(1,\_SB.PCI0.RP05.PDCX)
        Store(1,\_SB.PCI0.RP05.HPSX)

        If(LNot(\_SB.PCI0.RP05.PDSX)) {
          Store(0,\_SB.PCI0.RP05.L0SE)
        }
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 4)) {
          Notify(\_SB.PCI0.RP05,0)
        }
#else
        Notify(\_SB.PCI0.RP05,0)
#endif
//[-end-130709-IB05400426-modify]//
      }
      Else
      {
        Store(1,\_SB.PCI0.RP05.HPSX)
      }
    }

    // Check Root Port 6 for a Hot Plug Event if the Port is
    // enabled.
    If(LAnd(LEqual(RP6D,0),\_SB.PCI0.RP06.HPSX))
    {
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 5)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP06.PDCX)
      {
        Store(1,\_SB.PCI0.RP06.PDCX)
        Store(1,\_SB.PCI0.RP06.HPSX)

        If(LNot(\_SB.PCI0.RP06.PDSX)) {
          Store(0,\_SB.PCI0.RP06.L0SE)
        }
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
        If (LNotEqual (TRPN, 5)) {
          Notify(\_SB.PCI0.RP06,0)
        }
#else
        Notify(\_SB.PCI0.RP06,0)
#endif
//[-end-130709-IB05400426-modify]//
      }
      Else
      {
        Store(1,\_SB.PCI0.RP06.HPSX)
      }
    }

    // Check Root Port 7 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP7D,0),\_SB.PCI0.RP07.HPSX))
    {
      P8XH(1,0x07,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count
    
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 6)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP07.PDCX)
      {
        Store(1,\_SB.PCI0.RP07.PDCX)
        Store(1,\_SB.PCI0.RP07.HPSX)

        If(LNot(\_SB.PCI0.RP07.PDSX)) {
          Store(0,\_SB.PCI0.RP07.L0SE)
        }
        If (\ECON)
        {
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
          { // If not docked then it's hot plug
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
            If (LNotEqual (TRPN, 6)) {
              Notify(\_SB.PCI0.RP07,0)
            }
#else
            Notify(\_SB.PCI0.RP07,0)
#endif
//[-end-130709-IB05400426-modify]//
          }
        }
      }
      Else
      {
        Store(1,\_SB.PCI0.RP07.HPSX)
      }
    }

    // Check Root Port 8 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LEqual(RP8D,0),\_SB.PCI0.RP08.HPSX))
    {
      P8XH(1,0x08,1)       // Port 80h : show plug port
      P8XH(0,L01C,1)       // Port 80h : show Entry count
    
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 7)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130709-IB05400426-modify]//

      If(\_SB.PCI0.RP08.PDCX)
      {
        Store(1,\_SB.PCI0.RP08.PDCX)
        Store(1,\_SB.PCI0.RP08.HPSX)

        If(LNot(\_SB.PCI0.RP08.PDSX)) {
          Store(0,\_SB.PCI0.RP08.L0SE)
        }

        If (\ECON)
        {
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
          { // If not docked then it's hot plug
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
            If (LNotEqual (TRPN, 7)) {
              Notify(\_SB.PCI0.RP08,0)
            }
#else
            Notify(\_SB.PCI0.RP08,0)
#endif
//[-end-130709-IB05400426-modify]//
          }
        }
      }
      Else
      {
        Store(1,\_SB.PCI0.RP08.HPSX)
      }
    }
  }

  //
  // Software GPE caused the event.
  //
  Method(_L62)
  {
    // Clear GPE status bit.
    Store(0,GPEC)

    //
    // Handle DTS Thermal Events.
    //
    External(DTSE, IntObj)
    If(CondRefOf(DTSE)){
      If(LGreaterEqual(DTSE, 0x01)){
        Notify(\_TZ.TZ00,0x80)
        Notify(\_TZ.TZ01,0x80)
      }
    }
    //
    // CPPC Begin
    //
    // If GPE was asserted on doorbell ring, notify CPPC driver. If GPE was asserted by CPPC to signal
    // command completion, clear internal flag. This method is required for CPPC driver to function.
    //
    External(\_SB.PCCD.PENB, IntObj)
    If(CondRefOf(\_SB.PCCD.PENB)){
      If(LEqual(\_SB.PCCD.PENB, 1))  // If CPPC is enabled in BIOS setup, then process command.
      {
        Notify(\_SB.PCCD, 0x80)
      }
    }
    //
    // CPPC End
    //
  }

  // IGD OpRegion SCI event (see IGD OpRegion/Software SCI BIOS SPEC).

  Method(_L66)
  {
    If(LAnd(\_SB.PCI0.GFX0.GSSE, LNot(GSMI))) // Graphics software SCI event?
    {
      \_SB.PCI0.GFX0.GSCI() // Handle the SWSCI
    }
  }

  // SMBus Alert caused the wake event

  Method(_L67)
  {
    Store(0x20,\_SB.PCI0.SBUS.HSTS)
  }

  // RI# caused the wake event (COMA).

//[-start-121128-IB06460471-modify]//
//[-start-120809-IB10370015-remove]//
		Method(_L68)
		{
			Notify(\_SB.PCI0.LPCB.SM17.UAR1, 0x02)
		}
//[-end-120809-IB10370015-remove]//
//[-end-121128-IB06460471-modify]//

//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
  Method(_L0B, 0)
  {
    Store(0xF1, \SSMP)               // Generate SMI for the Thunderbolt Bus Check
//[-start-130709-IB05400426-modify]//
    If (LEqual (TRPN, 0)) {
      Notify (\_SB.PCI0.RP01, 0x00)
    } ElseIf (LEqual (TRPN, 1)) {
      Notify (\_SB.PCI0.RP02, 0x00)
    } ElseIf (LEqual (TRPN, 2)) {
      Notify (\_SB.PCI0.RP03, 0x00)
    } ElseIf (LEqual (TRPN, 3)) {
      Notify (\_SB.PCI0.RP04, 0x00)
    } ElseIf (LEqual (TRPN, 4)) {
      Notify (\_SB.PCI0.RP05, 0x00)
    } ElseIf (LEqual (TRPN, 5)) {
      Notify (\_SB.PCI0.RP06, 0x00)
    } ElseIf (LEqual (TRPN, 6)) {
      Notify (\_SB.PCI0.RP07, 0x00)
    } ElseIf (LEqual (TRPN, 7)) {
      Notify (\_SB.PCI0.RP08, 0x00)
    }   
//[-end-130709-IB05400426-modify]//
  }
#endif

  //
  // GPI027 = EC WAKETIME SCI
  //
  Method(_L1B)
  {
    // Do nothing if Desktop platform
    If (LEqual(\ECON,0))
    {
      Return ()
    }

    // If the wake event is not a Virtual Lid or Battery, then
    // it must be a Wake from either the Keyboard or Mouse. A
    // Power Button Event will be sent for both events such
    // that the video turns back on for these "attended wake"
    // events.

    // Check for Virtual Lid Event.

    If(LNotEqual(LIDS,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE))))
    {
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)),LIDS)

      If(IGDS)
      {
        // Upon waking a lid event may fail if driver is not ready.
        // If it does, set the high bit of the variable to indicate that
        // another notification is required during system callbacks.
        If(\_SB.PCI0.GFX0.GLID(LIDS))
        {
          Or(0x80000000,\_SB.PCI0.GFX0.CLID, \_SB.PCI0.GFX0.CLID)
        }
      }
      Notify(\_SB.LID0,0x80)
    }
    Else
    {
      // Check for Virtual Power Event.

      If(LEqual(BNUM,0))
      {
        If(LNotEqual(PWRS,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR))))
        {
          // Update NVS Power State.
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS)

          // Perform needed ACPI Notifications.
          PNOT()
        }
      }

      // Make it an attended wake event, no matter what.
      Notify(\_SB.PWRB,0x02)
    }
    Return ()
  }


}
