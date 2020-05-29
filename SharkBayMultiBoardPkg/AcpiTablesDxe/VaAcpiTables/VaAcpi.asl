/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

DefinitionBlock (
	"VaAcpi.aml",
	"SSDT",
	1,
	"VaRef",
	"Va_Acpi",
	0x3000
    )
{
External(\_SB.PCI0, DeviceObj)
External(\_GPE._L02, MethodObj)  //VA_SUPPORT


  Scope(\_SB.PCI0)
  {
    Device (IEIT)
    {
      Name (_HID, EISAID("INT5400"))
      Name (_CID, EISAID("PNP0C02")) // prevents Yellow Bang
      Name (_UID, 0)

      //
      // Generating an SCI
      //
      OperationRegion (SGPE, SystemIO, 0x400, 0x44)
      Field (SGPE, WordAcc, NoLock, Preserve)
      {
        Offset(0x20),    
        , 2,
        SWGS, 1,

        Offset(0x28),    
        , 2,
        SWGE, 1,

        Offset(0x42),    
        , 1,
        SWGC, 1,
      }//End of SGPE

      OperationRegion (SMIP, SystemIO, 0xB2, 0x2)
      Field (SMIP, WordAcc, NoLock, Preserve)
      {
        SMIT, 8,
        SMID, 8,
      }//End of SMIP

      //
      // The address is updated before end of BIOS POST
      //
      OperationRegion(VA_M, SystemMemory, 0xFFFF55AA, 0x100)
      Field(VA_M, AnyAcc, NoLock, Preserve)
      {
        // Header
        SIGN,   0x40,
        SIZE,   0x20,
        OVER,   0x20,
        SUBF,   0x20,
        EITD,   0x20,
        FNCD,   0x20,
        VVER,   0x20,
        ATTR,   0x20,
        SINF,   0x20,
        //RSV1,   0xc0,
        
        // Command & Data
        Offset(0x40),
//        EITC,   0x20,
        EITI, 0x1,   // EITC[0]
        EITF, 0x7,   // EITC[7:1]
        EITP, 0x8,   // EITC[15:8]
        , 0x10,      // EITC[31:16]
        DAT0,   0x20,
        DAT1,   0x20,
        SPD0,   0x20,
        SPD1,   0x20,
        //RSV2,   0x20,
        
        // VA_INFO
        Offset(0x58),      //Pls check EFI_VA_INFO_OFFSET
        VISI,   0x20,
        VIFC,   0x20,
        VIST,   0x20,
        VILN,   0x20,
        VID0,   0x20,
        VID1,   0x20,
        VID2,   0x20,
        VID3,   0x20,
      }//End of VA_M

      //
      // reports presence of device, return 0F for desktop, 1F for mobile
      //
      Method (_STA)
      {
        Return (0x0F)   //Return (0x1F)
      }

      //
      // starting address of ACPI OpRegion, filled during POST
      //
      Method (ADDR)
      {
        Return (0x55AA55AA)   
      }

      //
      // control method of returning IO address in chipset to cause SCI trigger
      //
      Method (SCIT)
      {
        Return (0x442)             // GPE_CNTL
      }

      // control method of returning Bit Offset within IO address for SWSCI trigger
      Method (SCIB)
      {
        Return (0x1)               // SWGPE_CTRL
      }

      //
      // control method that BIOS invokes when it recognized SWSCI status bit, single launch point for all EIT sub-function calls. When the SCI gets invoked the system BIOS gets control from the OS in the _Lxx GPE handler and after checking the status bit and verifying the cause of the invocation, the system BIOS can invoke EITB control method.
      //
      Method (EITV)
      {
        //
        // Check Version, convert 2.6 to 3.0 format
        //
        //
        // Trigger VA SW SMI (0x30) for VA sub-functions
        //
        Store (0x30, SMIT)

  
       }//End of EITB
     }//End of IEIT
  } //Scope \_SB.PCI0

/*++
  Scope(\_GPE)
  {
    Method(_L02)
    {
      \_SB.PCI0.IEIT.EITV()
    }
  }
--*/
} //End of SSDT
