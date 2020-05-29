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
;*    Copyright (c)  2010 - 2014 Intel Corporation. All rights reserved   *;
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

      Method(_DEP){
        If(LEqual(S0ID, 1)){
          Return(Package() {\_SB.PEPD})
        }Else{
          Return(Package() {})
        }
      }
        
      OperationRegion(XPRT,PCI_Config,0x00,0x100)
      Field(XPRT,AnyAcc,NoLock,Preserve)
      {
        DVID, 16,
        Offset(0x40),
            , 11,
        SWAI,  1,  // 0x40 BIT[11]
            , 20,
        Offset(0x44),
            , 12,
        SAIP,  2,  // 0x44 BIT[13:12]
            , 18,
        Offset(0x74),
        D0D3,  2,  // 0x74 BIT[1:0]
            ,  6,
        PMEE,  1,  // PME Enable
            ,  6,
        PMES,  1,  // PME Status
        Offset(0xB0), // SSCFG Reg for LPTLP
            , 13,
        MB13,  1,  // 0xB0 BIT[13]
        MB14,  1,  // 0xB0 BIT[14]
            , 17,
        Offset(0xD0),
        PR2,  32,  // XUSB2PR: xHC USB 2.0 Port Routing Register.
        PR2M, 32,  // XUSB2PRM: xHC USB 2.0 Port Routing Mask Register.
        PR3,  32,  // USB3_PSSEN: USB3.0 Port SuperSpeed Enable Register.
        PR3M, 32   // USB3PRM: USB3.0 Port Routing Mask Register
      }

      OperationRegion(XHCP, SystemMemory, Add(PEBS, 0xA0000), 0x100)
      Field(XHCP,AnyAcc,Lock,Preserve)
      {
        Offset(0x4),
        PDBM, 16,
        Offset(0x10),
        MEMB, 64
      }

      //
      // for each HS port, this method returns its corresponding selection bit in USB2PR register
      //
      Method(PR2S,1,Serialized) {
      
        If(LEqual(And(CDID,0xF000), 0x8000)) { // LPT-H
          Switch(Arg0) {
            Case( 1) {return (0x0001)}
            Case( 2) {return (0x0002)}
            Case( 3) {return (0x0004)}
            Case( 4) {return (0x0008)}
            Case( 5) {return (0x0100)}
            Case( 6) {return (0x0200)}
            Case( 7) {return (0x0400)}
            Case( 8) {return (0x0800)}
            Case( 9) {return (0x0010)}
            Case(10) {return (0x0020)}
            Case(11) {return (0x1000)}
            Case(12) {return (0x2000)}
            Case(13) {return (0x0040)}
            Case(14) {return (0x0080)}
            Case(15) {return (0x4000)}
          }
        } else { // LPT-LP
          Switch(Arg0) {
            Case( 1) {return (0x0001)}
            Case( 2) {return (0x0002)}
            Case( 3) {return (0x0004)}
            Case( 4) {return (0x0008)}
            Case( 5) {return (0x0010)}
            Case( 6) {return (0x0020)}
            Case( 7) {return (0x0040)}
            Case( 8) {return (0x0080)}
            Case( 9) {return (0x0100)}
          }
        }
      }

      Name(XRST, Zero)
      //
      // Workaround for XHCI
      //
      // At D3 exit (AKA PS_ON)
      // For LPT-LP
      //   Clear PCI CFG offset 0xB0[14:13]
      //   Clear MMIO Offset 0x816C[14]
      //   Clear MMIO Offset 0x816C[2]
      // For LPT-H, LPT-LP
      //   Set MMIO Offset 8154[31]
      // For LPT-LP
      //   Wait until all SS ports are out of polling
      //   For each SS port which is disconnected (i.e. PORTS.PLS=5h) and CSC=0
      //     Issue Warm Port Reset
      //   Wait 101ms
      //   Write '1' to all Port Change Status bits if reset
      //   Set MMIO Offset 0x80E0[15]
      // For LPT-H, LPT-LP
      //   Clear XHCI CFG REG 0x40[11]
      //   Clear XHCI CFG REG 0x44[13:12]
      //
      // Prior to D3 entry (AKA PS_OFF)
      // For LPT-LP
      //   Set PCI CFG offset 0xB0[14:13]
      //   Set MMIO Offset 0x816C[14]
      //   Set MMIO Offset 0x816C[2]
      // For LPT-H, LPT-LP
      //   Clear MMIO Offset 8154[31]
      // For LPT-LP
      //   Clear MMIO Offset 0x80E0[15]
      // For LPT-H, LPT-LP
      //   Set XHCI CFG REG 0x40[11]
      //   Set XHCI CFG REG 0x44[13:12] = '01'
      //
      External(\_SB.PCI0.XHC.PS0X, MethodObj)
      External(\_SB.PCI0.XHC.PS3X, MethodObj)
      //
      // D0 Method for xHCI Host Controller
      //
      Method(_PS0,0,Serialized)
      {
        // Uses:
        //  Local0 - Temporary
        //  Local1 - Original command register
        //  Local2 - Original MBAR
        //  Local4 - Port reset mask
        //
        If(LEqual(^DVID,0xFFFF))
        {
          Return()
        }

        //
        // MBAR Programming
        //
        Store(^MEMB,Local2)         // Save MBAR
        Store(^PDBM,Local1)         // Save CMD
        And(^PDBM,Not(0x06),^PDBM)  // Clear MSE/BME

        //
        // Switch to D0
        //
        Store(^D0D3,Local3)
        Store(0,^D0D3)

        Store(\SRMB,^MEMB)          // Set MBAR
        Or(Local1,0x0002,^PDBM)     // Set MSE

        OperationRegion(MCA1,SystemMemory,\SRMB,0x9000)
        Field(MCA1,DWordAcc,Lock,Preserve)
        {
          Offset(0x510),
          PSC1, 32,
          Offset(0x520),
          PSC2, 32,
          Offset(0x530),
          PSC3, 32,
          Offset(0x540),
          PSC4, 32,
          Offset(0x80E0),  // AUX Reset Control 1
              , 15,
          AX15,  1,
          Offset(0x8154),  // AUX Domain PM control register 2
              , 31,
          CLK2,  1,        // [31]
          Offset(0x816C),  // AUXCLKCTL
              ,  2,
          CLK0,  1,        // [2]
              , 11,
          CLK1,  1,        // [14] - USB3 Port Aux/Core clock gating enable
        }

        If(LEqual(PCHS, 2))   // LPT-LP
        {
          //
          // Clear PCI CFG offset 0xB0[14:13]
          //
          Store(0,^MB13)
          Store(0,^MB14)
          //
          // Clear MMIO Offset 0x816C[14]
          // Clear MMIO Offset 0x816C[2]
          //
          Store(0,CLK0)
          Store(0,CLK1)
        }

        //
        // Set MMIO Offset 8154[31] for both LPT-LP and LPT-H
        //
        Store(1,CLK2)

        If(LEqual(PCHS, 2))   // LPT-LP
        {
          //
          // Wait until all ports are out of polling (PP=1, PLS=7)
          //
          while(LOr(LOr(LEqual(And(PSC1,0x3F8),0x2E0),
                        LEqual(And(PSC2,0x3F8),0x2E0)),
                    LOr(LEqual(And(PSC3,0x3F8),0x2E0),
                        LEqual(And(PSC4,0x3F8),0x2E0))))
          {
            Stall(10)
          }

          //
          // Bitmask of SS ports for which warm reset was performed
          //
          Store(0, Local4)

          //
          // For each SS port which is disconnected (i.e. PORTS.PLS=5h) and CSC=0
          //    Issue Warm Port Reset
          //
          And(PSC1,Not(0x02),Local0)               // Mask PED
          If(LEqual(And(Local0,0x203F9),0x2A0))    // If SS PSC1 PP=1, PLS=5, CSC=0
          {
            Or(Local0,0x80000000,PSC1)             // Set WPR and clear change flags
            Or(Local4,0x1,Local4)
          }
          And(PSC2,Not(0x02),Local0)               // Mask PED
          If(LEqual(And(Local0,0x203F9),0x2A0))    // If SS PSC2 PP=1, PLS=5, CSC=0
          {
            Or(Local0,0x80000000,PSC2)             // Set WPR and clear change flags
            Or(Local4,0x2,Local4)
          }
          And(PSC3,Not(0x02),Local0)               // Mask PED
          If(LEqual(And(Local0,0x203F9),0x2A0))    // If SS PSC3 PP=1, PLS=5, CSC=0
          {
            Or(Local0,0x80000000,PSC3)             // Set WPR and clear change flags
            Or(Local4,0x4,Local4)
          }
          And(PSC4,Not(0x02),Local0)               // Mask PED
          If(LEqual(And(Local0,0x203F9),0x2A0))    // If SS PSC4 PP=1, PLS=5, CSC=0
          {
            Or(Local0,0x80000000,PSC4)             // Set WPR and clear change flags
            Or(Local4,0x8,Local4)
          }
          //
          // Wait 101ms
          // Write '1' to all Port Change Status bits if reset
          //
          If (Local4)
          {
            Sleep(101)

            If (And(Local4,0x1))
            {
              And(PSC1,Not(0x02),Local0)             // Mask PED
              Or(Local0, 0x00FE0000,PSC1)            // Clear SS PSC1 Bit 23:17
            }
            If (And(Local4,0x2))
            {
              And(PSC2,Not(0x02),Local0)             // Mask PED
              Or(Local0,0x00FE0000,PSC2)             // Clear SS PSC2 Bit 23:17
            }
            If (And(Local4,0x4))
            {
              And(PSC3,Not(0x02),Local0)             // Mask PED
              Or(Local0,0x00FE0000,PSC3)             // Clear SS PSC3 Bit 23:17
            }
            If (And(Local4,0x8))
            {
              And(PSC4,Not(0x02),Local0)             // Mask PED
              Or(Local0,0x00FE0000,PSC4)             // Clear SS PSC4 Bit 23:17
            }
          }

          //
          // Set MMIO Offset 0x80E0[15]
          //
          Store(1,AX15)  //0x80E0 - BIT15, AUX Reset Control 1
        }

        //
        // Clear PCI CFG offset 0x40[11] for both LPT-LP and LPT-H
        //
        Store(0,^SWAI)
        //
        // Clear PCI CFG offset 0x44[13:12] for both LPT-LP and LPT-H
        //
        Store(0,^SAIP)

        //
        // Call platform XHC PS0 method if present
        //
        If(CondRefOf(\_SB.PCI0.XHC.PS0X))
        {
          \_SB.PCI0.XHC.PS0X()
        }
      
        //
        // Leave device in D0 to avoid spurious PME event upon D3 entry
        //

        //
        // Restoring MBAR
        //
        And(^PDBM,Not(0x02),^PDBM)  // Clear MSE
        Store(Local2,^MEMB)         // Restore MBAR
        Store(Local1,^PDBM)         // Restore CMD
      }

      //
      // D3 Method for xHCI Host Controller
      //
      Method(_PS3,0,Serialized)
      {
        // Uses:
        //  Local0 - Temporary
        //  Local1 - Original command register
        //  Local2 - Original MBAR
        //  Local3 - D0D3 temporary
        //
        If(LEqual(^DVID,0xFFFF))
        {
          Return()
        }
        Store(1, ^PMES) //Clear PME status
        Store(1, ^PMEE) //Enable PME


        //
        // MBAR Programming
        //
        Store(^MEMB,Local2)         // Save MBAR
        Store(^PDBM,Local1)         // Save CMD
        And(^PDBM,Not(0x06),^PDBM)  // Clear MSE/BME
        Store(\SRMB,^MEMB)          // Set MBAR
        Or(^PDBM,0x02,^PDBM)        // Set MSE

        OperationRegion(MCA1,SystemMemory,\SRMB,0x9000)
        Field(MCA1,DWordAcc,Lock,Preserve)
        {
          Offset(0x80E0),     // AUX Reset Control 1
              , 15,
          AX15,  1,
          Offset(0x8154),     // AUX Domain PM control register 2
              , 31,
          CLK2,  1,           // BIT31
          Offset(0x816C),     // 40.2.3.6.6040.2.3.6.59	AUXCLKCTL
              ,  2,
          CLK0,  1,           // BIT2
              , 11,
          CLK1,  1,           // BIT14 - USB3 Port Aux/Core clock gating enable
              , 17,
        }

        //
        // If device is in D3, set to back to D0
        //
        Store(^D0D3,Local3)
        if(LEqual(Local3,3))
        {
          Store(0,^D0D3)
        }

        If(LEqual(PCHS, 2))   // LPT-LP
        {
          //
          // Set PCI CFG offset 0xB0[14:13]
          //
          Store(1,^MB13)
          Store(1,^MB14)
          //
          // Set MMIO Offset 0x816C[14]
          // Set MMIO Offset 0x816C[2]
          //
          Store(1,CLK0)
          Store(1,CLK1)
        }

        //
        // Clear MMIO Offset 8154[31] for both LPT-LP and LPT-H
        //
        Store(0,CLK2)

        If(LEqual(PCHS, 2))   // LPT-LP
        {
          //
          // Clear MMIO Offset 0x80E0[15]
          //
          Store(0,AX15) //0x80E0 - BIT15
        }

        //
        // Set PCI CFG offset 0x40[11] = '1' for both LPT-LP and LPT-H
        //
        Store(1,^SWAI)
        //
        // Set PCI CFG offset 0x44[13:12] = '01' for both LPT-LP and LPT-H
        //
        Store(1,^SAIP)

        //
        // Call platform XHC PS3 method if existed.
        // in the PS3X, MBAR is ready, CMD is ready, and Device is in D0.
        //
        If(CondRefOf(\_SB.PCI0.XHC.PS3X))
        {
          \_SB.PCI0.XHC.PS3X()
        }

        //
        // Restoring device back to D3
        //
        if(LEqual(Local3,3))
        {
          Store(3,^D0D3)
        }
        //
        // Restoring MBAR
        //
        And(^PDBM,Not(0x02),^PDBM)  // Clear MSE
        Store(Local2,^MEMB)         // Restore MBAR
        Store(Local1,^PDBM)         // Restore CMD
      }

      //
      //
      // Check for XHCI switch UUID
      //
      // Arguments:
      //  Arg0 (Buffer) : UUID
      //
      // Returns:
      //  1: It's valid UUID
      //  0: Invalid UUID
      //
      Method(CUID,1,Serialized) {
        If(LEqual(Arg0,ToUUID("7c9512a9-1705-4cb4-af7d-506a2423ab71"))) {
          Return(1)
        }
        Return(0)
      }

      //
      // _OSC for xHCI
      // This method enables XHCI controller if available.
      //
      // Arguments:
      //  Arg0 (Integer): Revision ID - should be set to 1
      //  Arg1 (Integer): Count
      //  Arg2 (Buffer) : Capabilities Buffer
      //                  DWORD#1 (Status/Error):
      //                  Bit 0 - Query Support Flag
      //                  Bit 1 - Always clear(0)
      //                  Bit 2 - Always clear(0)
      //                  Bit 3 - Always clear(0)
      //                  All others - reserved (return 0)
      //
      //                  DWORD#3 (Controlled):
      //                  Bit 0 - If set OS request routing back to EHCI
      //                  All others - reserved (return 0)
      // Returns:
      //  Capabilities Buffer: 
      //                  DWORD#1 (Status):
      //                  Bit 0 - Reserved (not used) 
      //                  Bit 1 - _OSC failure. Platform Firmware was unable to process the request or query.
      //                          Capabilities bits may have been masked. 
      //                  Bit 2 - Unrecognized UUID. This bit is set to indicate that the platform firmware does not
      //                          recognize the UUID passed in _OSC Arg0.
      //                          Capabilities bits are preserved. 
      //                  Bit 3 - Unrecognized Revision. This bit is set to indicate that the platform firmware does not
      //                          recognize the Revision ID passed in via Arg1.
      //                          Capabilities bits beyond those comprehended by the firmware will be masked. 
      //                  Bit 4 - Capabilities Masked. This bit is set to indicate 
      //                          that capabilities bits set by driver software
      //                          have been cleared by platform firmware. 
      //                  All others - reserved (return 0)
      //
      Method(POSC,3,Serialized) {
        //
        // Create DWord field from the Capabilities Buffer
        //
        CreateDWordField(Arg2,0,CDW1)
        CreateDWordField(Arg2,8,CDW3)

        //
        // Set failure if xHCI is disabled by BIOS
        //
        If (LEqual(XHCI, 0)) {
          Or(CDW1,0x2,CDW1)
        }

        //
        // Query flag clear
        //
        If(LNot(And(CDW1,0x1))) {
          If (And(CDW3,0x1)) {
            //
            // Perform switch back to EHCI
            //
            ESEL()
          }
          Else {
            If (LEqual(And(CDID,0xF000), 0x8000)) {  // if LPT-H chipset
              If (LGreater(Arg0, 0x1)) {
                //
                // Perform switch to xHCI
                //
                XSEL()
              } Else {
                //
                // Set failure if revision is not supported
                //
                Or(CDW1,0xA,CDW1)
              }
            } Else {  // if LPT-LP chipset
              If (LGreater(Arg0, 0x2)) {
                //
                // Perform switch to xHCI
                //
                XSEL()
              } Else {
                //
                // Set failure if revision is not supported
                //
                Or(CDW1,0xA,CDW1)
              }
            }
          }
        }

        Return(Arg2)
      }

//[-start-130205-IB02950497-modify]//
      Method(XSEL, 0, Serialized)
      {
        
        If (LEqual(MAUL,1)) {
          If (LOr(LEqual(XHCI,2), LEqual(XHCI,3))) {
            //
            // Set B0:D31:F0 ACh[16] to indicate begin of Driver phase of USB port routing
            //
            Store(1, XUSB)
            Store(1, XRST) // Backup XUSB, cause it might lost in iRST G3 or DeepSx
            //
            // Enable selected SS ports, route corresponding HS ports to xHCI
            //
            Store(U3SS, PR3)  
            Store(U2PR, PR2)      
          } 
        } 
        Else {
          //
          // xHCI in auto or smart auto mode
          //
          If (LOr(LEqual(XHCI,2), LEqual(XHCI,3))) {
            //
            // Set B0:D31:F0 ACh[16] to indicate begin of Driver phase of USB port routing
            //
            Store(1, XUSB)
            Store(1, XRST) // Backup XUSB, cause it might lost in iRST G3 or DeepSx
            //
            // Enable selected SS ports, route corresponding HS ports to xHCI
            //
            Store(0, Local0)
            And(PR3, 0xFFFFFFC0, Local0)
            Or(Local0, PR3M, PR3)
            Store(0, Local0)
            And(PR2, 0xFFFF8000, Local0)
            Or(Local0, PR2M, PR2)
          }
        }
      }
//[-end-130205-IB02950497-modify]//

      Method(ESEL, 0, Serialized)
      {
        //
        // xHCI in auto or smart auto mode 
        //
        If (LOr(LEqual(XHCI,2), LEqual(XHCI,3))) {
          //
          // Disable all SS ports, route all HS ports to EHCI
          //
          And(PR3, 0xFFFFFFC0, PR3)
          And(PR2, 0xFFFF8000, PR2)

          //
          // Mark as not routed.
          //
          Store(0, XUSB)
          Store(0, XRST)
        }
      }

      Method(XWAK, 0, Serialized)
      {
        //
        // Ports were routed to xHCI before sleep
        //
        If (LOr(LEqual(XUSB,1), LEqual(XRST,1))) {
          //
          // Restore back to xHCI
          //
          XSEL()
        }
      }

      Method(_S3D, 0, NotSerialized)
      {
        Return(0x02)
      }

      Method(_S4D, 0, NotSerialized)
      {
        Return(0x02)
      }

      Device(RHUB)
      {
        Name(_ADR, Zero)

        //
        // High Speed Ports
        //
        Device(HS01)
        {
          Name(_ADR, 0x01)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })

            If(LNot(And(PR2S(1), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }

            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(1), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(HS02)
        {
          Name(_ADR, 0x02)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR2S(2), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(2), PR2))) { 
              And(VIS,0,VIS) 
            }

            Return (PLDP)
          }
        }

        Device(HS03)
        {
          Name(_ADR, 0x03)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR2S(3), PR2))) { 
              Store(0x00,Index(UPCP,0x00)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(3), PR2))) { 
              And(VIS,0,VIS) 
            }
            If(LEqual(And(CDID,0xF000), 0x9000)) {  // on LPT-LP platforms this port is internal
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(HS04)
        {
          Name(_ADR, 0x04)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR2S(4), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(4), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(HS05)
        {
          Name(_ADR, 0x05)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })  // Change Type from "USB 3 Standard-A connector" to "Type 'A' connector".
            If(LNot(And(PR2S(5), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Changes in bits 63:0 -
                //     1. Set bit 7, as this is standard 'A'connector
                //     2. Clear bits 31:8 (Color), which will be ignored as described in #1.
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(5), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS06)
        {
          Name(_ADR, 0x06)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })  // Change Type from "USB 3 Standard-A connector" to "Type 'A' connector".
            If(LNot(And(PR2S(6), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Changes in bits 63:0 -
                //     1. Set bit 7, as this is standard 'A' connector
                //     2. Clear bits 31:8 (Color), which will be ignored as described in #1.
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(6), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS07)
        {
          Name(_ADR, 0x07)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })
            If(LNot(And(PR2S(7), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Changes in bits 63:0 -
                //     1. Set bit 7, as this is standard 'A' connector
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(7), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS08)
        {
          Name(_ADR, 0x08)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })
            If(LNot(And(PR2S(8), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Changes in bits 63:0 -
                //     1. Set bit 7, as this is standard 'A' connector
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(8), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS09)
        {
          Name(_ADR, 0x09)
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })  // HS09 is routed to a USB3 A Connector
            If(LNot(And(PR2S(9), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Use color of USB 3 Type A connector
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(9), PR2))) { 
              And(VIS,0,VIS) 
            }
            If(LEqual(And(CDID,0xF000), 0x9000)) {
              And(VIS,0,VIS) // invisible because on LPT-LP HS09 is KVM's USBR port
            }
            Return (PLDP)
          }
        }

      Device(HS10)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xA)
            } Else {
              Return (0xFA) // on LPT-LP, 0xA is assigned to SSP so move this port's address away
            }
          }
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) // this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })  // HS10 is routed to a USB3 A Connector
            If(LNot(And(PR2S(10), PR2))) {
              Store(0x00,Index(UPCP,0))
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                // Use color of USB 3 Type A connector
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(10), PR2))) {
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS11)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xB)
            } Else {
              Return (0xFB) // on LPT-LP, 0xB is assigned to SSP so move this port's address away
            }
          }
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 }) // Proprietary connector (internal header)
            If(LNot(And(PR2S(11), PR2))) {
              Store(0x00,Index(UPCP,0))
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                // Changes in bits 127:64 -
                //     1. Clear bit 64 (User Visible), set bits 69:67 (Panel) to 110b (Unknown).
                //     2. Clear bits 71:70 (Vertical Position) and 73:72 (Horizontal Position),
                //            these will be ignored because User Visible = 0 (this is documented).
                //     3. Set bits 77:74 (Shape) to 111b (Unknown).
                //     4. Clear bits 86:79 (Group Token) and 94:87 (Group Position), these should be
                //            ignored because User Visible = 0 (this is not documented, but assumed). 
                0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(11), PR2))) {
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS12)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xC)
            } Else {
              Return (0xFC) // on LPT-LP, 0xC is assigned to SSP so move this port's address away
            }
          }
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 }) // Proprietary connector (internal header)
            If(LNot(And(PR2S(12), PR2))) {
              Store(0x00,Index(UPCP,0))
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                // Changes in bits 127:64 -
                //     1. Clear bit 64 (User Visible), set bits 69:67 (Panel) to 110b (Unknown).
                //     2. Clear bits 71:70 (Vertical Position) and 73:72 (Horizontal Position),
                //            these will be ignored because User Visible = 0 (this is documented).
                //     3. Set bits 77:74 (Shape) to 111b (Unknown).
                //     4. Clear bits 86:79 (Group Token) and 94:87 (Group Position), these should be
                //            ignored because User Visible = 0 (this is not documented, but assumed). 
                0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(12), PR2))) {
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS13)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xD)
            } Else {
              Return (0xFD) // on LPT-LP, 0xD is assigned to SSP so move this port's address away
            }
          }
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 })  // Proprietary connector (internal header)
            If(LNot(And(PR2S(13), PR2))) {
              Store(0x00,Index(UPCP,0))
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                // Changes in bits 127:64 -
                //     1. Clear bit 64 (User Visible), set bits 69:67 (Panel) to 110b (Unknown).
                //     2. Clear bits 71:70 (Vertical Position) and 73:72 (Horizontal Position),
                //            these will be ignored because User Visible = 0 (this is documented).
                //     3. Set bits 77:74 (Shape) to 111b (Unknown).
                //     4. Clear bits 86:79 (Group Token) and 94:87 (Group Position), these should be
                //            ignored because User Visible = 0 (this is not documented, but assumed). 
                0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(13), PR2))) {
              And(VIS,0,VIS) 
            }

            Return (PLDP)
          }
                    
        }

      Device(HS14)
        {
          Name(_ADR, 0xE)
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 }) // Proprietary connector (internal header)
            If(LNot(And(PR2S(14), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                // Changes in bits 127:64 -
                //     1. Clear bit 64 (User Visible), set bits 69:67 (Panel) to 110b (Unknown).
                //     2. Clear bits 71:70 (Vertical Position) and 73:72 (Horizontal Position),
                //            these will be ignored because User Visible = 0 (this is documented).
                //     3. Set bits 77:74 (Shape) to 111b (Unknown).
                //     4. Clear bits 86:79 (Group Token) and 94:87 (Group Position), these should be
                //            ignored because User Visible = 0 (this is not documented, but assumed). 
                0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(14), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

      Device(HS15)
        {
          Name(_ADR, 0xF)
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0x00,0x00,0x00,0x00 }) // Not connectable, USBR not enabled in H87
            If(LNot(And(PR2S(15), PR2))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                // Changes in bits 127:64 -
                //     1. Clear bit 64 (User Visible), set bits 69:67 (Panel) to 110b (Unknown).
                //     2. Clear bits 71:70 (Vertical Position) and 73:72 (Horizontal Position),
                //            these will be ignored because User Visible = 0 (this is documented).
                //     3. Set bits 77:74 (Shape) to 111b (Unknown).
                //     4. Clear bits 86:79 (Group Token) and 94:87 (Group Position), these should be
                //            ignored because User Visible = 0 (this is not documented, but assumed). 
                0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // not visible - this is KVM's USBR port
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR2S(15), PR2))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        //
        // Super Speed Ports - must match _UPC declarations of the coresponding Full Speed Ports.
        //
        Device(SSP1)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0x10)
            } Else {
              Return (0xA)
            }
          }
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x01))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x01))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(SSP2)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0x11)
            } Else {
              Return (0xB)
            }
          }
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x02))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x69, 0x0C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x02))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(SSP3)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0x12)
            } Else {
              Return (0xC)
            }
          }
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x04))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x04))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(SSP4)
        {
          Method(_ADR, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0x13)
            } Else {
              Return (0xD)
            }
          }
          Name(_STA, 0xF)
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x08))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x08))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(SSP5)
        {
          Name(_ADR, 0x14)
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x10))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x10))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }

        Device(SSP6)
        {
          Name(_ADR, 0x15)
          Method(_STA, 0, Serialized) {
            If(LEqual(And(CDID,0xF000), 0x8000)) {  // check for LPT-H chipset
              Return (0xF)
            } Else {
              Return (0) //this port doesn't exist on LPT-LP
            }
          }
          Method(_UPC,0,Serialized) {
            Name(UPCP, Package() { 0xFF,0x03,0x00,0x00 })
            If(LNot(And(PR3, 0x20))) { 
              Store(0x00,Index(UPCP,0)) 
            }
            Return(UPCP)
          }
          Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {
              Buffer(0x10)
              {
                0x01, 0xC6, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x71, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00
              }
            })
            CreateBitField(DeRefOf(Index(PLDP,0)),64,VIS)
            If(LNot(And(PR3, 0x20))) { 
              And(VIS,0,VIS) 
            }
            Return (PLDP)
          }
        }
      }
      
