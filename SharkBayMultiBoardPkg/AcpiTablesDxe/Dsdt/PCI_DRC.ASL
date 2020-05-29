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


Scope (\_SB.PCI0){

  Device(PDRC)
  {
    //
    // PCI Device Resource Consumption
    //

    Name(_HID,EISAID("PNP0C02"))        

    Name(_UID,1)

    Name(BUF0,ResourceTemplate()
    {
      //
      // RCRB BAR _BAS will be updated in _CRS below according to B0:D31:F0:Reg.F0
      //
      Memory32Fixed(ReadWrite,0,0x04000,RCRB)
      //
      // MCH BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.48h
      //
      Memory32Fixed(ReadWrite,0,0x08000,MCHB) 
      //
      // DMI BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.68h
      //
      Memory32Fixed(ReadWrite,0,0x01000,DMIB) 
      //
      // EP BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.40h
      //
      Memory32Fixed(ReadWrite,0,0x01000,EGPB)
      //
      // PCI Express BAR _BAS and _LEN will be updated in _CRS below according to B0:D0:F0:Reg.60h
      //
      Memory32Fixed(ReadWrite,0,0,PCIX)  

      //
      // MISC ICH TTT base address reserved for the TxT module use. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadWrite,0xFED20000,0x20000)   
      
      //
      // VTD engine memory range. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadOnly, 0xFED90000, 0x00004000)
      
//[-start-120403-IB05300308-remove]//
//      //
//      // MISC ICH. Check if the hard code meets the real configuration.
//      // If not, dynamically update it like the _CRS method below.
//      //
//      Memory32Fixed(ReadWrite,0xFED45000,0x4B000,TPMM)
//[-end-120403-IB05300308-remove]//
      
      //
      // FLASH range
      //
      Memory32Fixed (ReadOnly, 0xFF000000, 0x1000000, FIOH) //16MB as per IOH spec
      
      //
      // Local APIC range(0xFEE0_0000 to 0xFEEF_FFFF)
      //
      Memory32Fixed (ReadOnly, 0xFEE00000, 0x100000, LIOH)
      
      //
      // Thermal Base Address. TBAR _BAS and _LEN will be updated in _CRS below according to B0:D31:F6:Reg.40h
      //    
      Memory32Fixed (ReadWrite, 0, 0x01000, TBAR)

      //
      // S3 Save And Restore temporay memory bar
      //
      Memory32Fixed (ReadWrite, 0, 0x10000, SNRB)

    })
 

    Method(_CRS,0,Serialized)
    {
      CreateDwordField(BUF0,^RCRB._BAS,RBR0)
      ShiftLeft(\_SB.PCI0.LPCB.RCBA,14,RBR0)

      CreateDwordField(BUF0,^TBAR._BAS,TBR0)    
      Store(TBAB,TBR0)
      CreateDwordField(BUF0,^TBAR._LEN,TBLN)
      If(LEqual(TBAB,0)){
        Store(0,TBLN)
      }

      CreateDwordField(BUF0,^SNRB._BAS,SNR0)
      Store(\SRMB,SNR0)  // SRMB is defined in PCH.asl
      

      CreateDwordField(BUF0,^MCHB._BAS,MBR0)
      ShiftLeft(\_SB.PCI0.MHBR,15,MBR0)

      CreateDwordField(BUF0,^DMIB._BAS,DBR0)
      ShiftLeft(\_SB.PCI0.DIBR,12,DBR0)

      CreateDwordField(BUF0,^EGPB._BAS,EBR0)
      ShiftLeft(\_SB.PCI0.EPBR,12,EBR0)

      CreateDwordField(BUF0,^PCIX._BAS,XBR0)
      ShiftLeft(\_SB.PCI0.PXBR,26,XBR0)

      CreateDwordField(BUF0,^PCIX._LEN,XSZ0)
      ShiftRight(0x10000000, \_SB.PCI0.PXSZ,XSZ0)

      Return(BUF0)
    }
  } //end of PDRC
} // end of SB
