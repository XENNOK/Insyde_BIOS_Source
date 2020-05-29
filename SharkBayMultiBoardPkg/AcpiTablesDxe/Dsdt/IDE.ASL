//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/************************************************************************;
;*                                                                      *;
;*    Intel Confidential                                                *;
;*                                                                      *;
;*    Intel Corporation - ACPI Reference Code for the SandyBridge       *;
;*    Family of Customer Reference Boards.                              *;
;*                                                                      *;
;*    MPG-MSAE                                                          *;
;*                                                                      *;
;*    Copyright (c) 1999-2009 Intel Corporation.                        *;
;*                                                                      *;
;*    This program has been developed by Intel Corporation.             *;
;*    Licensee has Intel's permission to incorporate this source code   *;
;*    into their product, royalty free.  This source code may NOT be    *;
;*    redistributed to anyone without Intel's written permission.       *;
;*                                                                      *;
;*    Intel specifically disclaims all warranties, express or           *;
;*    implied, and all liability, including consequential and other     *;
;*    indirect damages, for the use of this code, including liability   *;
;*    for infringement of any proprietary rights, and including the     *;
;*    warranties of merchantability and fitness for a particular        *;
;*    purpose.  Intel does not assume any responsibility for any        *;
;*    errors which may appear in this code nor any responsibility to    *;
;*    update it.                                                        *;
;*                                                                      *;
;*    Version:  See README.TXT                                          *;
;*                                                                      *;
;************************************************************************/
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/   



//[-start-130625-IB05400421-modify]//
//
// Update GPI0, GDMA and SFLG metohds for IDE mode support under Windows XP
// Be careful when updating ASL reference code.

//
//
//  Get PIO Timing 
//  Arg0 Fast Timing Status
//  Arg1 DMA Timing Status
//  Arg2 RCT Timing
//  Arg3 ISP Timing
//
Method(GPI0,4)
{         
    If (LEqual (Or (Arg0, Arg1) , 0) ) {
        //
        //  No PIO Timing and DMA Timing support
        // 
        Return (0xFFFFFFFF)
    
    } ElseIf (And ( LEqual (Arg0, 0) , LEqual (Arg1, 1) ) ) {
        //
        //  Compatible PIO timing support 
        //
        Return (900)        
    }
  
    // 
    //  Using ISP and RCT timing , PCI Clock = 33 Mhz , 30ns per clock
    //        
    Return (Multiply(30,Subtract(9,Add(Arg2,Arg3))))      
}
//
//  Get DMA Timing
//  Arg0 Synchronous DMA Mode Supported
//  Arg1 Fast_PCB - ATA-100 support
//  Arg2 XCB - Base clock
//  Arg3 XCT - Cycle Time
//
Method (GDMA, 4)
{
    If (LEqual (Arg0, 1)) {
        //
        // Ultra DMA 5 (ATA-100) Support
        //
        If (LEqual (Arg1, 1)) {
            Return (20)
        } Else {
            If (LEqual (Arg2, 1)) {
                //     
                // Base Clock = 66 MHz (15 ns)
                // detemine Ultra DMA Mode 4/3
                //
                Return (Multiply (15, Subtract (4, Arg3)))
            } Else { 
                // 
                // Base Clock = 33 MHz (30 ns)
                // determine Ultra DMA Mode 2/1/0
                //
                Return (Multiply (30, Subtract (4, Arg3)))
            } 
        }   
    }
    // Doesn't support UDMA mode    
    Return (0xFFFFFFFF)
}
//
//  Set Flag
//  Arg0 Synchronous Ultra DMA Mode for drive 0
//  Arg1 IORDY for drive 0
//  Arg2 Synchronous Ultra DMA Mode for drive 1
//  Arg3 IORDY for drive 1
//  Arg4 indicates chipset can set timing independently for each drive
//  
Method(SFLG, 5)
{
    //
    // The Chipset always support separate timing setting and always support IORDY
    //
    Store (0, Local0)
    Or (Arg0, Local0, Local0)
    Or (ShiftLeft (Arg1,1) ,Local0, Local0)
    Or (ShiftLeft (Arg2,2) ,Local0, Local0)
    Or (ShiftLeft (Arg3,3) ,Local0, Local0)        
    Or (ShiftLeft (Arg4,4) ,Local0, Local0)
    Return (Local0)
}
//[-end-130625-IB05400421-modify]//
//
// Set PIO Timing
// Arg0 Timing
// Arg1 ATA Device PIO Mode Supported Flag
// Arg2 ATA Device PIO Mode Supported Timing
//
// PIO/Mode              Timing
// PIO0/Compatible       900 ns
// PIO2/SW2              240 ns
// PIO3/MW1              180 ns
// PIO4/MW2              120 ns
//

Method(SPIO , 3)
{
    Name(PBUF, Buffer(5) { 0x00,0x00,0x00,0x00,0x00})
    CreateByteField(PBUF, 0,  RCT)
    CreateByteField(PBUF, 1,  ISP)
    CreateByteField(PBUF, 2,  FAST) 
    CreateByteField(PBUF, 3,  DMAE)  
    CreateByteField(PBUF, 4,  PIOT) 
    If (LOr (LEqual (Arg0, 0x0), LEqual (Arg0, 0x0FFFFFFFF)) ) {
    
        Return (PBUF)
    }
    If (LGreater (Arg0, 240)) {
        //
        //  Compatible timing
        //
        Store (1, DMAE)   // PIO Mode 0
        Store (0, PIOT)   // Set to PIO Mode 0          
  
    } Else {
        //
        //  Fast Timing Enable
        //      
        Store (1, FAST)          
        
        If (And (Arg1, 0x002)) {
            //
            // ATA Device Supported PIO Mode Report
            //
            If (And (LEqual (Arg0, 120), And( Arg2 , 0x002) ) ) {
                //
                // Device support PIO Mode 4
                //   
                Store (3, RCT)   // RCT = 1 CLK
                Store (2, ISP)   // ISP = 3 CLK
                Store (4, PIOT)  // Set to PIO Mode 4 
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 180), And( Arg2 , 0x001) ) ) {
                //
                // Device support PIO Mode 3
                //
                Store (1, RCT)   // RCT = 3 CLK
                Store (2, ISP)   // ISP = 3 CLK   
                Store (3, PIOT)  // Set to PIO Mode 3     
                Return (PBUF)
            } Else {
                // 
                // PIO Mode 2
                // 
                Store (0, RCT)   // RCT = 4 CLK
                Store (1, ISP)   // ISP = 4 CLK 
                Store (2, PIOT)  // Set to PIO Mode 2                  
            }
        }
    }           
    Return (PBUF)    
}
//
// Set DMA Timing
// Arg0 Timing
// Arg1 ATA Device UDMA Mode Supported Flag
// Arg2 ATA Device UDMA Mode Supported Timing
//
// UDMA/Mode              Timing 
// UDMA5                   20 ns
// UDMA4                   30 ns
// UDMA3                   45 ns
// UDMA2                   60 ns
// UDMA1                   90 ns
// UDMA0                   120 ns
//

Method(SDMA , 3)
{
    Name(PBUF, Buffer(5) { 0x00,0x00,0x00,0x00})
    CreateByteField(PBUF, 0,  PCT)
    CreateByteField(PBUF, 1,  PCB)
    CreateByteField(PBUF, 2,  UDMT)     // ATA 100 Support
    CreateByteField(PBUF, 3,  UDME)     // Ultra DMA Enable
    CreateByteField(PBUF, 4,  DMAT) 
    If (LOr (LEqual (Arg0, 0x0), LEqual (Arg0, 0xFFFFFFFF)) ) {
                
        Return (PBUF)
    }
    If (LLessEqual (Arg0, 120)) {
        //
        //  Ultra DMA Supported
        //
        If (And (Arg1, 0x004)) {
            //
            // ATA Device Supported UDMA Mode Report
            //
            Store (1, UDME)   
             
            If (And (LLessEqual (Arg0, 15), And( Arg2 , 0x0040) ) ) {
                //
                // Ultra DMA 6
                //       
                Store (1, UDMT)
                Store (1, PCB)
                Store (1, PCT)
                Store (6, DMAT)  // Set to UDMA Mode 6
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 20), And( Arg2 , 0x0020) ) ) {
                //
                // Ultra DMA 5
                //       
                Store (1, UDMT)
                Store (1, PCB)
                Store (1, PCT)
                Store (5, DMAT)  // Set to UDMA Mode 5
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 30), And( Arg2 , 0x00010) ) ) {
                //
                // Ultra DMA 4
                //
                Store (1, PCB) 
                Store (2, PCT)
                Store (4, DMAT)  // Set to UDMA Mode 4
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 45), And( Arg2 , 0x0008) ) ) {
                //
                // Ultra DMA 3
                //           
                Store (1, PCB)
                Store (1, PCT)
                Store (3, DMAT)  // Set to UDMA Mode 3
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 60), And( Arg2 , 0x0004) ) ) {
                //
                // Ultra DMA 2
                //
                Store (2, PCT)
                Store (2, DMAT)  // Set to UDMA Mode 2
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 90), And( Arg2 , 0x0002) ) ) {
                //
                // Ultra DMA 1
                //      
                Store (1, PCT)   
                Store (1, DMAT)  // Set to UDMA Mode 1
                Return (PBUF)
            } 
            If (And (LLessEqual (Arg0, 120), And( Arg2 , 0x0001) ) ) {
                //
                // Ultra DMA 0
                //  
                Store (0, DMAT)  // Set to UDMA Mode 0        
            }                             
        }
    }
    Return (PBUF)    
}


// Set Timing for the Primary or Secondary Drive 1 using Slave
// IDE Timing registers.
//   Arg0 = PIO1 value passed into _STM.
//   Arg1 = Word 53 of Identify Device Command
//   Arg2 = Word 64 of Identify Device Command

Method(SETT,3,Serialized)
{
	// For PIO 2,3, and 4, ISPx and RCTx are set based upon the
	// PIOx value passed in and what the Drive supports.

	If(And(Arg1,0x0002))
	{
		// Registers used to check for PIO3/PIO4 are valid.

		If(LAnd(LLessEqual(Arg0,120),And(Arg2,0x0002)))
		{
			Return(0x0B)	// PIO4
		}

		If(LAnd(LLessEqual(Arg0,180),And(Arg2,0x0001)))
		{
			Return(9)	// PIO3
		}
	}

	Return(4)			// PIO2
}
