/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c) 2012 Intel Corporation. All rights reserved           *;
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
/** @file
  The TPM2 definition block in ACPI table for TrEE physical presence  
  and MemoryClear.
**/

DefinitionBlock (
  "Tpm.aml",
  "SSDT",
  1,
  "Intel_",
  "Tpm2Tabl",
  0x1000
  )
{
  External(PTTS)   // PTT  Solution Method Selection
  External(PTTB)   // PTT Buffer Address

  Scope (\_SB)
  {
    Device (TPM)
    {
      //
      // Define _HID, "PNP0C31" is defined in
      // "Secure Startup-FVE and TPM Admin BIOS and Platform Requirements"
      //
      Name (_HID, "MSFT0101")
      
      //
      // Readable name of this device, don't know if this way is correct yet
      //
      Name (_STR, Unicode ("TPM 2.0 Device"))

      //
      // Return the resource consumed by TPM device
      //
      Name (CRS, ResourceTemplate () {
        Memory32Fixed (ReadOnly, 0xFED70000, 0x1000,PCRS)
      })


      Method(_CRS,0)
      {
        CreateDWordField (CRS, \_SB.TPM.PCRS._BAS, CBAS) 
        Store (PTTB, CBAS)
        Return(CRS)
      }

      //
      // Operational region for Smi port access
      //
      OperationRegion (SMIP, SystemIO, 0xB2, 1)
      Field (SMIP, ByteAcc, NoLock, Preserve)
      { 
          IOB2, 8
      }

      OperationRegion (FHCI, SystemMemory, 0xFED70000, 0x1000)
      Field (FHCI, AnyAcc, NoLock, Preserve)
      {
        Offset(0x04),
        HERR, 32,
        Offset (0x40),
        HCMD, 32,
        Offset(0x44),
        HSTS, 32,
      }
      //
      // Operational region for TPM support, TPM Physical Presence and TPM Memory Clear
      // Region Offset 0xFFFF0000 and Length 0xF0 will be fixed in C code.
      //
      OperationRegion (TNVS, SystemMemory, 0xFFFF0000, 0xF0)
      Field (TNVS, AnyAcc, NoLock, Preserve)
      {
        PPIN,   8,  //   Software SMI for Physical Presence Interface
        PPIP,   32, //   Used for save physical presence paramter
        PPRP,   32, //   Physical Presence request operation response
        PPRQ,   32, //   Physical Presence request operation
        LPPR,   32, //   Last Physical Presence request operation
        FRET,   32, //   Physical Presence function return code
        MCIN,   8,  //   Software SMI for Memory Clear Interface
        MCIP,   32, //   Used for save the Mor paramter
        MORD,   32, //   Memory Overwrite Request Data
        MRET,   32  //   Memory Overwrite function return code
      }

      Method (PTS, 1, Serialized)
      {  
        //
        // Detect Sx state for MOR, only S4, S5 need to handle
        //
        If (LAnd (LLess (Arg0, 6), LGreater (Arg0, 3)))
        {   
          //
          // Bit4 -- DisableAutoDetect. 0 -- Firmware MAY autodetect.
          //
          If (LNot (And (MORD, 0x10)))
          {
            //
            // Triggle the SMI through ACPI _PTS method.
            //
            Store (0x02, MCIP)
              
            //
            // Triggle the SMI interrupt
            //
            Store (MCIN, IOB2)
          }
        }
        Return (0)
      }   

      Method (_STA, 0)
      {
        Return (0x0f)
      }

      //
      // TCG Hardware Information
      //
      Method (HINF, 3, Serialized, 0, {BuffObj, PkgObj}, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg1))
        {
          Case (0)
          {
            //
            // Standard query
            //
            Return (Buffer () {0x03})
          }
          Case (1)
          {
            //
            // Return failure if no TPM present
            //
            Name(TPMV, Package () {0x01, Package () {0x1, 0x20}})
            if (LEqual (_STA (), 0x00))
            {
              Return (Package () {0x00})
            }

            //
            // Return TPM version
            //
            Return (TPMV)
          }
          Default {BreakPoint}
        }
        Return (Buffer () {0})
      }

      Name(TPM2, Package (0x02){
        Zero, 
        Zero
      })

      Name(TPM3, Package (0x03){
        Zero, 
        Zero,
        Zero
      })

      //
      // TCG Physical Presence Interface
      //
      Method (TPPI, 3, Serialized, 0, {BuffObj, PkgObj, IntObj, StrObj}, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {        
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg1))
        {
          Case (0)
          {
            //
            // Standard query, supports function 1-8
            //
            Return (Buffer () {0xFF, 0x01})
          }
          Case (1)
          {
            //
            // a) Get Physical Presence Interface Version
            //
            Return ("1.2")
          }
          Case (2)
          {
            //
            // b) Submit TPM Operation Request to Pre-OS Environment
            //
                  
            Store (DerefOf (Index (Arg2, 0x00)), PPRQ)
            Store (0x02, PPIP)
              
            //
            // Triggle the SMI interrupt
            //
            Store (PPIN, IOB2)
            Return (FRET)


          }
          Case (3)
          {
            //
            // c) Get Pending TPM Operation Requested By the OS
            //
                  
            Store (PPRQ, Index (TPM2, 0x01))
            Return (TPM2)
          }
          Case (4)
          {
            //
            // d) Get Platform-Specific Action to Transition to Pre-OS Environment
            //
            Return (2)
          }
          Case (5)
          {
            //
            // e) Return TPM Operation Response to OS Environment
            //
            Store (0x05, PPIP)
                  
            //
            // Triggle the SMI interrupt
            //
            Store (PPIN, IOB2)
                  
            Store (LPPR, Index (TPM3, 0x01))
            Store (PPRP, Index (TPM3, 0x02))

            Return (TPM3)
          }
          Case (6)
          {

            //
            // f) Submit preferred user language (Not implemented)
            //

            Return (3)

          }
          Case (7)
          {
            //
            // g) Submit TPM Operation Request to Pre-OS Environment 2
            //
            Store (7, PPIP)
            Store (DerefOf (Index (Arg2, 0x00)), PPRQ)
                
            //
            // Triggle the SMI interrupt 
            //
            Store (PPIN, IOB2)  
            Return (FRET)
          }
          Case (8)
          {
            //
            // e) Get User Confirmation Status for Operation
            //
            Store (8, PPIP)
            Store (DerefOf (Index (Arg2, 0x00)), PPRQ)
                  
            //
            // Triggle the SMI interrupt
            //
            Store (PPIN, IOB2)
                  
            Return (FRET)
          }

          Default {BreakPoint}
        }
        Return (1)
      }

      Method (TMCI, 3, Serialized, 0, IntObj, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {
        //
        // Switch by function index
        //
        Switch (ToInteger (Arg1))
        {
          Case (0)
          {
            //
            // Standard query, supports function 1-1
            //
            Return (Buffer () {0x03})
          }
          Case (1)
          {
            //
            // Save the Operation Value of the Request to MORD (reserved memory)
            //
            Store (DerefOf (Index (Arg2, 0x00)), MORD)
                  
            //
            // Triggle the SMI through ACPI _DSM method.
            //
            Store (0x01, MCIP)
                  
            //
            // Triggle the SMI interrupt
            //
            Store (MCIN, IOB2)
            Return (MRET)
          }
          Default {BreakPoint}
        }
        Return (1)        
      }

      Method (TSMI, 3, Serialized, 0, IntObj, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {

        //
        // Operational region for TPM access
        //
        OperationRegion (TPMR, SystemMemory, PTTB, 0x1000)
        Field (TPMR, AnyAcc, NoLock, Preserve)
        {
          Offset(0x04),
          FERR, 32,
          Offset(0x0c),
          STRT, 32,
         }

        //
        // Switch by function index
        //
        Switch (ToInteger (Arg1))
        {
          Case (0)
          {
            //
            // Standard query, supports function 1-1
            //
            Return (Buffer () {0x03})
          }
          Case (1)
          {
            If(LEqual(PTTS, 0)) // GPDMA
            {
              If(LEqual(Or(And(HSTS,0x00000002),And(HSTS,0x00000001)),0x00000003))
              {
                //
                // Trigger the FTPM_CMD interrupt
                //
                Store (0x00000001, HCMD)
              }
              Else
              {
                //Set Error Bit
                Store(0x00000001,FERR)
                //Clear Start Bit
                Store(0x00000000,STRT)
              }
            }
            If(LEqual(PTTS, 1)) // MSFT QFE
            {
              //
              // Trigger the FTPM_CMD interrupt
              //
              Store (0x00000000, HCMD)
            }
            Return (0)
          }
          Default {BreakPoint}
        }
        Return (1)        
      }

      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {

        //
        // TCG Hardware Information
        //
        If(LEqual(Arg0, ToUUID ("cf8e16a5-c1e8-4e25-b712-4f54a96702c8")))
        {
          Return (HINF (Arg1, Arg2, Arg3))
        }

        //
        // TCG Physical Presence Interface
        //
        If(LEqual(Arg0, ToUUID ("3dddfaa6-361b-4eb4-a424-8d10089d1653")))
        {
          Return (TPPI (Arg1, Arg2, Arg3))
        }

        //
        // TCG Memory Clear Interface
        //
        If(LEqual(Arg0, ToUUID ("376054ed-cc13-4675-901c-4756d7f2d45d")))
        {
          Return (TMCI (Arg1, Arg2, Arg3))
        }

        //
        // TPM2 ACPI Start Method
        //
        If(LEqual(Arg0, ToUUID ("6bbf6cab-5463-4714-b7cd-f0203c0368d4")))
        {
          Return (TSMI (Arg1, Arg2, Arg3))
        }

        Return (Buffer () {0})
      }
    }
  }
}
