/** @file
  The TPM2 definition block in ACPI table for physical presence
  and MemoryClear.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

DefinitionBlock (
  "Tpm.aml",
  "SSDT",
  0x01,
  "Insyde",
  "TPMACPI",
  0x1000
  )
{
  Scope (\_SB)
  {
    Device (TPM2)
    {
      Name (_HID, "MSFT0101")
      Name (_CID, "MSFT0101")
      Name (_STR, Unicode ("TPM 2.0 Device"))

      //
      // Return the resource consumed by TPM device
      //
      Name (_CRS, ResourceTemplate () {
        Memory32Fixed (ReadOnly, 0xfed40000, 0x5000)
      })

      //
      // Operational region for Smi port access
      //
      OperationRegion (SMIP, SystemIO, FixedPcdGet16 (PcdSoftwareSmiPort), 1)
      Field (SMIP, ByteAcc, NoLock, Preserve)
      {
          IOB2, 8
      }

      //
      // Operational region for TPM access
      //
      OperationRegion (TPMR, SystemMemory, 0xfed40000, 0x5000)
      Field (TPMR, AnyAcc, NoLock, Preserve)
      {
        ACC0, 8
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
        MRET,   32, //   Memory Overwrite function return code
        CAIN,   8   //   Software SMI for Control Area Handler
      }

      //
      // Control Area
      //
      OperationRegion (CONA, SystemMemory, 0xFFFF0000, 0xF0)
      Field (CONA, DWordAcc, NoLock, Preserve)
      {
        HRTB,   32,
        ERRR,   32,
        CLFG,   32,
        STA1,   32,
                AccessAs (QWordAcc, 0x00),
        INTC,   64,
                AccessAs (DWordAcc, 0x00),
        CMSZ,   32,
                AccessAs (QWordAcc, 0x00),
        CMDB,   64,
                AccessAs (DWordAcc, 0x00),
        RPSZ,   32,
                AccessAs (QWordAcc, 0x00),
        RESP,   64
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
        if (LEqual (ACC0, 0xff))
        {
            Return (0)
        }
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
            Name(TPMV, Package () {0x01, Package () {0x02, 0x00}})
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

      Method (OASM, 3, Serialized, 0, {BuffObj, PkgObj, IntObj, StrObj}, {UnknownObj, UnknownObj, UnknownObj})
      {
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg1)) {

        Case (0) {
          //
          // Standard query, supports function 1
          //
          Return (Buffer () {0x03})
        }

        Case (1) {
          //
          // Start
          //
          While (LEqual (STA1, 1)) {
            Store (CAIN, IOB2)
            If (LEqual (STA1, 1)) {
              //
              // Wait 2s for Tpm response
              //
              Sleep (2000)
            }
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
        // Control Area Handler
        //
        If(LEqual (Arg0, ToUUID ("6bbf6cab-5463-4714-b7cd-f0203c0368d4")))
        {
          Return (OASM (Arg1, Arg2, Arg3))
        }

        Return (Buffer () {0})
      }
    }
  }
}
