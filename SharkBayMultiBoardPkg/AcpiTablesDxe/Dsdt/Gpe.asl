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

  Method(_L0B, 0) {
    Notify(\_SB.PCI0.P0P1, 0x02)
  }

  //
  // This PME event (PCH's GPE #9) is received on one or more of the PCI Express* ports or
  // an assert PMEGPE message received via DMI
  //
  Method(_L09, 0) {
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
  Method(_L0D, 0) {
    If(LAnd(\_SB.PCI0.EHC1.PMEE, \_SB.PCI0.EHC1.PMES)){
      Notify(\_SB.PCI0.EHC1, 0x02)
    }
    If(LAnd(\_SB.PCI0.EHC2.PMEE, \_SB.PCI0.EHC2.PMES)){
      Notify(\_SB.PCI0.EHC2, 0x02)
    }
    If(LAnd(\_SB.PCI0.XHC.PMEE, \_SB.PCI0.XHC.PMES)){
      Notify(\_SB.PCI0.XHC, 0x02)
    }
    ElseIf(LEqual(\_SB.PCI0.XHC.PMEE, 0)) {
      Store(1, \_SB.PCI0.XHC.PMES) // Clear PMES Bit because an SCI is occurring when PMEE bit is not set
    }
    If(LAnd(\_SB.PCI0.HDEF.PMEE, \_SB.PCI0.HDEF.PMES)){
      Notify(\_SB.PCI0.HDEF, 0x02)
    }
    Notify(\_SB.PCI0.GLAN, 0x02)
  }

  // PCI Express Hot-Plug caused the wake event.

  Method(_L01)
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

//[-start-130222-IB03780481-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 0)) {
        Sleep (100)
      }
#else
      Sleep (100)
#endif
//[-end-130222-IB03780481-modify]//

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

//[-start-130222-IB03780481-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 0)) {
        // Perform proper notification
        // to the OS.
        Notify (\_SB.PCI0.RP01, 0)
      }
#else
      // Perform proper notification
      // to the OS.
      Notify (\_SB.PCI0.RP01, 0)
#endif
//[-end-130222-IB03780481-modify]//
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
          Notify (\_SB.PCI0.RP02, 0)
        }
#else
        Notify (\_SB.PCI0.RP02, 0)
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
      If(LAnd(LNotEqual(BID, BICO),LNotEqual(BID, BICC))) {
        Sleep (100)
      }
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
          Notify (\_SB.PCI0.RP03, 0)
        }
#else
        If(LAnd(LNotEqual(BID, BICO),LNotEqual(BID, BICC))) {
          Notify(\_SB.PCI0.RP03,0)
        }
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
          Notify (\_SB.PCI0.RP04, 0)
        }
#else
        Notify (\_SB.PCI0.RP04, 0)
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
    
//[-start-130222-IB03780481-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 4)) {
        Sleep (100)
      }
#else
       If(LAnd(LNotEqual(BID, BICO),LNotEqual(BID, BICC))) {
         Sleep(100)
       }
#endif
//[-end-130222-IB03780481-modify]//

      If(\_SB.PCI0.RP05.PDCX)
      {
        Store(1,\_SB.PCI0.RP05.PDCX)
        Store(1,\_SB.PCI0.RP05.HPSX)

        If(LNot(\_SB.PCI0.RP05.PDSX)) {
          Store(0,\_SB.PCI0.RP05.L0SE)
        }

//[-start-130222-IB03780481-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
      If (LNotEqual (TRPN, 4)) {
        // Perform proper notification
        // to the OS.
        Notify (\_SB.PCI0.RP05, 0)
      }
#else
        If(LAnd(LNotEqual(BID, BICO),LNotEqual(BID, BICC))) {
          Notify(\_SB.PCI0.RP05,0)
        }
#endif
//[-end-130222-IB03780481-modify]//
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
          Notify (\_SB.PCI0.RP06, 0)
        }
#else
        Notify (\_SB.PCI0.RP06, 0)
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

        If(LEqual(PFLV,FDTP))
        {
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
          If (LNotEqual (TRPN, 6)) {
            Notify (\_SB.PCI0.RP07, 0)
          }
#else
          Notify (\_SB.PCI0.RP07, 0)
#endif
//[-end-130709-IB05400426-modify]//
        }
        Else
        {
          If (\ECON)
          {
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
            { // If not docked then it's hot plug
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
              If (LNotEqual (TRPN, 6)) {
                Notify (\_SB.PCI0.RP07, 0)
              }
#else
              Notify (\_SB.PCI0.RP07, 0)
#endif
//[-end-130709-IB05400426-modify]//
            }
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

        If(LEqual(PFLV,FDTP))
        {
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
          If (LNotEqual (TRPN, 7)) {
            Notify (\_SB.PCI0.RP08, 0)
          }
#else
          Notify (\_SB.PCI0.RP08, 0)
#endif
//[-end-130709-IB05400426-modify]//
        }
        Else
        {
          If (\ECON)
          {
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
            { // If not docked then it's hot plug
//[-start-130709-IB05400426-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
              If (LNotEqual (TRPN, 7)) {
                Notify (\_SB.PCI0.RP08, 0)
              }
#else
              Notify (\_SB.PCI0.RP08, 0)
#endif
//[-end-130709-IB05400426-modify]//
            }
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
  Method(_L02)
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

  Method(_L06)
  {
    If(LAnd(\_SB.PCI0.GFX0.GSSE, LNot(GSMI))) // Graphics software SCI event?
    {
      \_SB.PCI0.GFX0.GSCI() // Handle the SWSCI
    }
  }

  // SMBus Alert caused the wake event

  Method(_L07)
  {
    Store(0x20,\_SB.PCI0.SBUS.HSTS)
  }

  // RI# caused the wake event (COMA).

//[-start-120809-IB10370015-remove]//
//  Method(_L08)
//  {
//    Notify(\_SB.PCI0.LPCB.SM17.UAR1,0x02)
//  }
//[-end-120809-IB10370015-remove]//

//[-start-130307-IB03780481-modify]//
//[-start-120727-IB03780455-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-start-130628-IB03780498-remove]//
//  // OSUP method apply TB2P<->P2TB handshake procedure 
//  // with Command = OS_Up
//  // Arg0 - Memory mapped IO address of RR host router upstream port
//  Method (OSUP, 1)
//  {
//    Add (Arg0, 0x548, Local0)
//    OperationRegion (PXVD, SystemMemory, Local0, 0x08) 
//    Field (PXVD, DWordAcc, NoLock, Preserve)
//    {
//      TB2P, 32, 
//      P2TB, 32
//    }    
//
//    Store (100, Local1)
//    Store (0x0D, P2TB) // Write (OS_Up << 1) | 1 to PCIe2TBT
//    While (LGreater (Local1, 0))
//    {
//      Store (Subtract (Local1, 1), Local1)
//      Store (TB2P, Local2)
//      If (LEqual (Local2, 0xFFFFFFFF)) // Device gone
//      {
//        Return (2)
//      }
//      If (And (Local2, 1)) // Done
//      {
//        break
//      }
//      Sleep (50)
//    }
//    Store (0x00, P2TB) // Write 0 to PCIe2TBT
//    Return (1)
//  }
//
//  // Calculate Memory mapped IO address of RR host router upstream port
//  Method (MMTB)
//  {
//    // Calculate Memory mapped IO address of RR host router upstream port
//    // And put it into XXXXX
//    Store (TBTM, Local0)
//    Return (Local0)
//  }
//
//  // Check for 0xFFFFFFFF in TBT Vendor/Device ID
//  // And Call OSUP if true
//  Method (TBFF)
//  {
//    // Get mapped IO address of RR host router upstream port
//    Store (MMTB (), Local0) 
//    OperationRegion (PXVD, SystemMemory, Local0, 0x4) 
//    Field (PXVD, DWordAcc, NoLock, Preserve)
//    {
//      VEDI, 32 // Vendor/Device ID
//    }    
//
//    //Check Vendor/Device ID for 0xFFFFFFFF
//    Store (VEDI, Local1)
//    If (LEqual (Local1, 0xFFFFFFFF))
//    {
//      Return (OSUP (Local0))
//    }
//    Else
//    {
//      Return (0)
//    }
//  }  
//[-end-130628-IB03780498-remove]//

  Method(_E1B, 0) // GPIO11 on PCH wired to GPIO5 (CIO_PLUG_EVENT) on RR
  {
    //
    // Call TBFF() when Thuderbolt Chip is RR.
    //
    If (LEqual (TBTC, 1)) {
      Store (TBFF (), Local0)
      If (LOr (LEqual (Local0, 1), LEqual (Local0, 2))) // Only HR or Disconnect
      {
        Return ()
      }
    }

    //
    // Generate SMI for the Thunderbolt
    //
    Store (0xF1, \SSMP)

    //
    // Notify OS to do Bus Check
    //
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
//[-end-120727-IB03780455-modify]//
//[-end-130307-IB03780481-modify]//
  //
  // GPI014 = EC WAKETIME SCI
  //
  Method(_L1E)
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

  //
  //OS up handshake procedure to host router upstream port each time
  //exiting from Sx State .Avoids intermediate 
  //PCIe Scan by OS during resorce allocation
  //
  Method(OSUP, 1)
  {
    Add(Arg0, 0x548, Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x08) 
    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      TB2P, 32, 
      P2TB, 32
    }    

    Store(100, Local1)
    Store(0x0D, P2TB) // Write OS_Up to PCIe2TBT
    While(LGreater(Local1, 0))
    {
        Store(Subtract(Local1, 1), Local1)
        Store(TB2P, Local2)
        If(LEqual(Local2, 0xFFFFFFFF))// Device gone
        {
            Return(2)
        }
        If(And(Local2, 1)) // Done
        {
            break
        }
        Sleep(50)
    }
    Store(0x00, P2TB) // Write 0 to PCIe2TBT
    Return(1)
  }
  
  Method(MMTB)
  {
    ADBG("MMTB")
    Store(PEBS, Local0) // MMIO Base address
    
    Add(Local0, 0xE0000, Local0) // RP01
    Subtract(ToInteger(TBSE), 1, Local1)
    Multiply(Local1, 0x1000, Local1)
    Add(Local0, Local1, Local0) // RP0x

    OperationRegion (MMMM, SystemMemory, Local0, 0x1A)
    Field (MMMM, AnyAcc, NoLock, Preserve)
    {
        Offset(0x19),
        SBUS, 8
    }
    Store(SBUS, Local2)
    Store(PEBS, Local0)
    Multiply(Local2, 0x100000, Local2)
    Add(Local2, Local0, Local0) // TBT HR US port
    Return(Local0)
  }
  
  //
  //OS up handshake Method ,invokes OSUP based on 
  //Vendor/Device ID Check 
  //
  Method(OSUW)
  {
    ADBG("OSUW")
    Store(MMTB(), Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x4) 
    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      VEDI, 32 // Vendor/Device ID
    }    
    Store(VEDI, Local1)
    
    If(LNotEqual(Local1, 0xFFFFFFFF))
    {
        Return (OSUP(Local0))
    }
    Else
    {
        Return (0)
    }
  }
  
  Method(TBFF)// Check for FFFF in TBT
  {
    ADBG("TBFF")
    Store(MMTB(), Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x4) 
    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      VEDI, 32 // Vendor/Device ID
    }    
    Store(VEDI, Local1)

    If(LEqual(Local1, 0xFFFFFFFF))
    {
        Return (OSUP(Local0))
    }
    Else
    {
        Return (0)
    }
  }
  
  // Subordinate bus of TBT RP
  Method(TSUB)
  {
    Store(PEBS, Local0) // MMIO Base address

    Add(Local0, 0xE0000, Local0) // RP01
    Subtract(ToInteger(TBSE), 1, Local1)
    Multiply(Local1, 0x1000, Local1)
    Add(Local0, Local1, Local0) // RP0x

    OperationRegion (MMMM, SystemMemory, Local0, 0x1A)
    Field (MMMM, AnyAcc, NoLock, Preserve)
    {
        Offset(0x19),
        SBUS, 8
    }
    Return(SBUS)
  }
  
  // Wait for subordinate bus in TBT RP
  Method(WSUB)
  {
    Store(0, Local0)
    Store(0, Local1)
    While(1)
    {
        Store(TSUB(), Local1)
        If(Local1)
        {
            Break
        }
        Else
        {
            Add(Local0, 1, Local0)
            If(LGreater(Local0, 1000))
            {
                Sleep(1000)
                ADBG("WSUB Deadlock")
            }
            Else
            {
                Sleep(16)
            }
        }
    }
  }
  
  // Wait for _WAK finished
  Method(WWAK)
  {
    Store(0, Local0)
    Store(0, Local1)
    While(1)
    {
        Acquire(WFDM, 0xFFFF)
          Store(WKFN, Local0)
        Release(WFDM)
        
        If(Local0)
        {
            Break
        }
        Else
        {
            Add(Local1, 1, Local1)
            If(LGreater(Local1, 1000))
            {
                Sleep(1000)
                ADBG("WWAK Deadlock")
            }
            Else
            {
                Sleep(16)
            }
        }
    }
    Return(Local1)
  }
//[-start-130628-IB03780498-remove]//
//  //
//  // Method to Handle enumerate PCIe structure through
//  // SMI for Thunderbolt devices 
//  //
//  Method(_E2A)
//  {
//
//    WWAK()
//    WSUB()
//    OperationRegion(SPRT,SystemIO, 0xB2,2)
//    Field (SPRT, ByteAcc, Lock, Preserve)
//    {
//        SSMP, 8
//    }
//
//    ADBG("_E2A")
//    Acquire(OSUM, 0xFFFF)
//    Store(TBFF(), Local0)
//    If(LEqual(Local0, 1))// Only HR
//    {
//        Sleep(16)
//        Release(OSUM)
//        Return ()
//    }
//    If(LEqual(Local0, 2)) // Disconnect
//    {
//        If(NOHP)
//        {
//            ADBG("_E2A Ntfy")
//            If(LEqual(TBSE, 5))
//            {
//                Notify(\_SB.PCI0.RP05,0)
//            }
//            If(LEqual(TBSE, 3))
//            {
//                Notify(\_SB.PCI0.RP03,0)
//            }
//        }
//        Sleep(16)
//        Release(OSUM)
//        Return ()
//    }
//    
//    // HR and EP
//    If(LEqual(SOHP, 1))
//    {
//        // Trigger SMI to enumerate PCIe Structure
//        ADBG("_E2A SMI")
//        Store(21, TBSF)
//        Store(0xF7, SSMP)
//    }
//     If(LEqual(NOHP, 1))
//    {
//        //Notify Rootports
//        ADBG("_E2A Ntfy")
//        If(LEqual(TBSE, 5))
//        {
//        Notify(\_SB.PCI0.RP05,0)
//        }
//        If(LEqual(TBSE, 3))
//        {
//        Notify(\_SB.PCI0.RP03,0)
//        }
//    }
//    Sleep(16)
//    Release(OSUM)
//  }
//[-end-130628-IB03780498-remove]//



// For Reed Harbor only: Rotation Lock button is connected to GPI00.
  Method(_L00)
  {
    If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
    {
      ADBG("Rotation Lock")
      Sleep(1000)
      \_SB.PCI0.GFX0.IUEH(4) // Rotation lock
    }
  }


}
