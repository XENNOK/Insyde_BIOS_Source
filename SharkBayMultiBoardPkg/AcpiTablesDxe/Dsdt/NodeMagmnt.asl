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
;*    Intel Corporation - ACPI Reference Code for the Sandy Bridge        *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved   *;
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

External (\_SB.PRAD, DeviceObj)

//
// General Purpose Event
//
Scope (\_GPE)
{
  Method (_L24, 0x0, Serialized) // TCOSCI bit handler
  {
    //
    // Call the HECI handler. The handler will check
    // if the PCH SCI came from ME device;
    // otherwise it will simply return.
    //
    \_SB.PCI0.HEC2.HSCI()
  } // Method(...)
} // Scope

Scope(\)
{
  //
  // Current P-state and T-state limit per processor socket
  // set by Node Manager and P/T-state support flags
  //
  Name(\PETE, 0) // P-states supported + T-states supported flags
  Name(\PSLM, 0) // Current P-state limit for _PPC()
  Name(\TSLM, 0) // Current T-state limit for _TPC()

  //
  // Dynamic core allocation related objects. If DCA is not supported
  // (Processors Aggregator Device and _PUR method not implemented)
  // PURE flag remains 0 and HSCI handler will inform ME that
  // DCA is not supported.
  //
  // Go to GloblNvs.asl
  // Name(\THNU, 0) // Number of processor threads in the system
  Name(\PURA, 0) // Processor Utilization REQ
  Name(\PURB, 0) // HECI Processor Utilization ACK
  Name(\PURE, 0)
}

#define NM_CMD_MAXPTSTATE_REQ    0
#define NM_CMD_PROCUTIL_REQ      3

//
// The commands below are defined just for information.
// These symbols are not actually used because the \PETE
// object holds their values.
//
#define NM_CMD_MAXPTSTATE_NACK 0x00
#define NM_CMD_MAXPSTATE_ACK   0x80
#define NM_CMD_MAXTSTATE_ACK   0x40
#define NM_CMD_MAXPTSTATE_ACK  (NM_CMD_MAXPSTATE_ACK | NM_CMD_MAXTSTATE_ACK)
#define NM_CMD_PROCUTILN_ACK   0x03
#define NM_CMD_PROCUTIL_ACK    0x83

//
// The HECI-1 device should be defined under
// \_SB.PCI0 scope
//
Scope(\_SB.PCI0)
{
  Device(HEC1)
  {
    Name(_ADR, 0x00160000) //HECI-1 PCI address: dev 22, fun 0

    //
    // Define the region for CSR registers mapped to HECI-1 MBAR.
    // PCIBARTarget type seems to be not implemented in several
    // OSes, but it is not a problem since we do not actually
    // need it. PCIBARTarget is just used to declare HECI-1 as
    // ACPI only device.
    //
    // OperationRegion(H1CS, PCIBARTarget, 0x10, 0x10)
    // OperationRegion(H1CS, SystemMemory, 0x82B30000, 0x10)
    // Field(H1CS, DWordAcc, NoLock, Preserve)
    // {
    //   CBWW,32, // write window - R_HECI_HOST_CB_WW 
    //            // Bits of H_CSR (Host Control Status Register) 
    //   HIE, 1,
    //   HIS, 1,
    //   HIG, 1,
    //   HRD, 1,
    //   HRS, 1,
    //      , 3,
    //   HRP, 8, // host read pointer
    //   HWP, 8, // host write pointer
    //   HBD, 8, // host buffer depth
    //   CBRW,32, // read window - R_HECI_ME_CB_RW
    //            // Bits of ME_CSR_HA (ME Control Status Register)
    //   MIE, 1,
    //   MIS, 1,
    //   MIG, 1,
    //   MRD, 1,
    //   MRS, 1,
    //      , 3,
    //   MRP, 8, // ME read pointer
    //   MWP, 8, // ME write pointer
    //   MBD, 8  // ME buffer depth
    // }

    //
    // MEFS1 in HECI-1 HFS register
    // It holds ME Firmware Status #1
    //
    OperationRegion(MFS1, PCI_Config, 0x40, 0x4)
    Field(MFS1, DWordAcc, NoLock, Preserve)
    {
      MECS, 4,
      RVD, 28
    }

    //
    // Device status handler method - called by OSPM before INI
    // This device reports state of the HECI-1 device.
    // If ME firmware is not operational HECI-1 is reported
    // present but disabled. If ME is operational HECI-1 is
    // present and functioning, but hidden from UI.
    //
    Method(_STA, 0, NotSerialized)
    {
      If (LEqual(MECS, 5))
      {
        Return(0x0B) // functioning hidden from UI
      }
      Else
      {
        Return(0x09) // present but disabled
      }
    }
  } // Device(HEC1)
}

//
// The HECI-2 device should be defined under
// \_SB.PCI0 scope
//
Scope(\_SB.PCI0)
{
  Device(HEC2)
  {
    Name(_ADR, 0x00160001)              // HECI-2 PCI Address Dev:22, Fun:1

    //
    // Since PCIBARTarget type seems to be not implemented in
    // several OSes, PCIBARTarget is used only to declare HECI-2
    // as ACPI only device. Overlapping region of system memory
    // type is declared to actually access the HECI-2 CSR region.
    // The address is just stub and should be updated by BIOS
    // POST after PCI enumerator allocates one. Alternatively
    // some predefined address can be allocated for HECI-2 in BIOS
    // and then used in this region declaration.
    //
    // OperationRegion(H2ST, PCIBARTarget, 0x10, 0x10)
    // Field(H2ST, DWordAcc, NoLock, Preserve)
    // { H2ST, 128 }
    //
    // Address will be update by ChipsetUpdateAcpiTable.c
    //
    // OperationRegion(H2CS, PCIBARTarget, 0x10, 0x10)
    OperationRegion(H2CS, SystemMemory, 0x82B31000, 0x10)
    Field(H2CS, DWordAcc, NoLock, Preserve)
    {
      CBWW,32, // write window - R_HECI_HOST_CB_WW
               // Bits of H_CSR (Host Control Status Register)
      HIE, 1,  // interrupt enable
      HIS, 1,  // interrupt status
      HIG, 1,  // interrupt generate
      HRD, 1,  // ready signalization
      HRS, 1,  // reset request
       , 3,
      HRP, 8,  // host read pointer
      HWP, 8,  // host write pointer
      HBD, 8,  // host buffer depth
      CBRW,32,  // read window - R_HECI_ME_CB_RW
                // Bits of ME_CSR_HA (ME Control Status Register)
      MIE, 1,  // interrupt enable
      MIS, 1,  // interrupt status
      MIG, 1,  // interrupt generate
      MRD, 1,  // ready signalization
      MRS, 1,  // reset request
       , 3,
      MRP, 8, // ME read pointer
      MWP, 8, // ME write pointer
      MBD, 8  // ME buffer depth
    }

    //
    // NMFS in HECI-2 HFS register
    // It NM Firmware Status
    //
    OperationRegion(NMFS, PCI_Config, 0x40, 0x4)
    Field(NMFS, DWordAcc, NoLock, Preserve)
    {
          , 31,
      NMEN, 1
    }

    //
    // HECI-2 GS_SHDW2 register
    // It holds requested P-state limits
    //
    OperationRegion(H2RP, PCI_Config, 0x60, 0x4)
    Field(H2RP, DWordAcc, NoLock, Preserve)
    {
      RPS0, 4,
      RPRV,28  // unused on UP platform
    }

    //
    // HECI-2 GS_SHDW3 register
    // It holds requested T-state limits
    //
    OperationRegion(H2RT, PCI_Config, 0x64, 0x4)
    Field(H2RT, DWordAcc, NoLock, Preserve)
    {
      RTS0, 4,
      RTRV,28  // unused on UP platform
    }

    //
    // HECI-2 H_GS2 register
    // It holds current P-state limits
    //
    OperationRegion(H2CP, PCI_Config, 0x70, 0x4)
    Field(H2CP, DWordAcc, NoLock, Preserve)
    {
      CPS0, 4,
      CPRV,28  // unused on UP platform
    }

    //
    // HECI-2 H_GS3 register
    // It holds current T-state limits
    //
    OperationRegion(H2CT, PCI_Config, 0x74, 0x4)
    Field(H2CT, DWordAcc, NoLock, Preserve)
    {
      CTS0, 4,
      CTRV,28  // unused on UP platform
    }

    //
    // Device status handler method - called by OSPM before _INI
    // This device reports state of the HECI-2 device.
    // If ME firmware is not operational HECI-2 is reported
    // present but disabled. If ME is operational HECI-2 is
    // present and functioning, but hidden from UI.
    //
    Method(_STA, 0, NotSerialized)
    {
      If (LAnd(LEqual(\_SB.PCI0.HEC1.MECS, 5), NMEN))
      {
        Return(0x0B) // functioning hidden from UI
      }
      Else
      {
        Return(0x09)  // present but disabled
      }
    }

    //
    // Init method enables HECI-2 on OS boot
    // and loads processor aggregator of OS is capable
    //
    Method(_INI, 0, Serialized)
    {
      HWAK()
      If (CondRefOf(\_OSI,Local0))
      {
        If (\_OSI("Processor Aggregator Device"))
        {
          Store(One, \PURE)
          //LoadTable("PRAD", "INTEL", "PRADID",,,)
          //LoadTable("PRAD", "PRADID", "PRADTID",,,)
        }
      }
    } 

    //
    // Wake-up handler method
    //
    // Method(_PS0, 0, Serialized)
    Method(HWAK, 0, Serialized)
    {
      Store(1, HRD) // set host ready
      Store(1, HIE) // enable HECI interrupts
      Store(1, HIG) // kick ME - I'm ready now
    }

    //
    // Prepare-to-sleep handler method
    //
    // Method(_PS3, 0, Serialized)
    Method(HPTS, 0, Serialized)
    {
      Store(0, HIE) // disable HECI interrupts
      Store(0, HRD) // set host not ready
      Store(1, HIG) // kick ME - I'm not ready any more
    }

    //
    // Dynamic Cores NotiFy method - sends message via HECI-2
    // Used by Processor Aggregator Device to notify about
    // CPU threads idling status.
    // Arg0 ¡V The body of the message to send.
    //        It contains message sequence number and cores number
    //
#define HECIHDR_PROCUTL 0x80040011
    Method(DCNF, 1, Serialized)
    {
      Store(HECIHDR_PROCUTL, CBWW)
      Store(Arg0, CBWW)
      Store(1, HIG)
    }

    //
    // SCI handler method - called from _L24() SCI handler
    //
    Method(HSCI, 0, Serialized)
    {
      If (HIS) // If Host Interrupt Status
      {
        If (MRS) // If reset request from ME
        {
          Store(1, HRS)
          Store(1, HIG)
        }
        Else
        {
          If (MRD) // If ME side ready
          {
            If (LNot(HRD)) // ME signals ready,
            {              // make this side ready too
              Store(0, HRS)
              Store(1, HRD)
              Store(1, HIG) // kick ME - I'm ready
            }
            If (LNotEqual(MWP, MRP)) // If buffer not
            {                        // empty
              Store(CBRW, Local1) // message header
              Store(CBRW, Local0) // message body
              Store(1, HIG) // kick ME ¡V I got it
              If (LEqual(And(Local0, 0xff), NM_CMD_MAXPTSTATE_REQ))
              {
                //
                // Send ACK for P/T-state message
                //
                Store(Local1, CBWW)
                Store(Or(Local0, And(\PETE, 0xFF)), CBWW)
                Store(1, HIG)
                //
                // Extract Max P-state & T-state,
                // trigger _PPC & _TPC execution
                // for all the CPUs present in
                // the system by sending 0x80 code
                //
                Store(RPS0, \PSLM)
                Store(RTS0, \TSLM)
                //
                // Notify OSPM objects
                //
                Notify(\_PR.CPU0, 0x80)
                Notify(\_PR.CPU0, 0x82)
                Notify(\_PR.CPU1, 0x80)
                Notify(\_PR.CPU1, 0x82)
                Notify(\_PR.CPU2, 0x80)
                Notify(\_PR.CPU2, 0x82)
                Notify(\_PR.CPU3, 0x80)
                Notify(\_PR.CPU3, 0x82)
                Notify(\_PR.CPU4, 0x80)
                Notify(\_PR.CPU4, 0x82)
                Notify(\_PR.CPU5, 0x80)
                Notify(\_PR.CPU5, 0x82)
                Notify(\_PR.CPU6, 0x80)
                Notify(\_PR.CPU6, 0x82)
                Notify(\_PR.CPU7, 0x80)
                Notify(\_PR.CPU7, 0x82)
              } // If NM_CMD_MAXPTSTATE_REQ
              //If (LEqual(And(Local0, 0xff), HECI_CMD_PROCUTIL_REQ))
              If (LEqual(And(Local0, 0xff), NM_CMD_PROCUTIL_REQ))
              {
                Store(Local1, \PURA)
                Store(Local0, \PURB)

                If (\PURE)
                {
                  Store(Local0, \PURA)
                  // Notify PRAD device
//                  Notify(\_SB.PRAD, 0x80)
                }
                Else
                {
                  \_SB.PCI0.HEC2.DCNF(Local0)
                }
              } //If NM_CMD_PROCUTIL_REQ
            } // MRP Buffer
          } // If (MRD)
        } // If (MRS) Else
      } // If (HIS)
    } // Method(HSCI,...)
  } // Device(HEC2)
}

