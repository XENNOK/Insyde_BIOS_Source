//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++
  
  Copyright (c) 2005 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval 
  system, or transmitted in any form or by any means without the express 
  written consent of Intel Corporation.
 
  Module Name:
  
    AHCI.ASL 
  
  Abstract:
  
    Pch Serial ATA devices and control methods
  
--*/
DefinitionBlock (
    "SATAAHCI.aml", // Output Filename
    "SSDT",         // Signature
    0x01,           // DSDT Compliance Revision
    "INTEL ",       // OEM ID    
    "SataAhci",     // Table ID
    0x1000          // OEM Revision
    )
{
External(\_SB.PCI0, DeviceObj)
Scope(\_SB.PCI0) {
Device(AHCI)
{
    Name(_ADR,0x001f0002)  
    Method(SDDA,2)
    {
        // Create a Local Buffer with NOP ATA Command as initial value
        Name(BUF0, Buffer(7) { 0x00,0x00,0x00,0x00,0x00,0x00,0x00 })

        CreateByteField(BUF0, 0, FEAT)
        CreateByteField(BUF0, 1, DESC)
        CreateByteField(BUF0, 5, DEVD)
        CreateByteField(BUF0, 6, COMD)

        // These registers are specific to the IDENTIFY
        // DEVICE = ECh Command found in the ATA Spec.
        // Word 78 Bit 3 indicates if Device Initiated
        // Power Management is supported.
        // Word 79 Bit 3 indicates if Device Initiated
        // Power Management is enabled.
        CreateWordfield(Arg0,156,W780)  // Word 78, Drive 0
        CreateWordfield(Arg0,158,W790)  // Word 79, Drive 0

        // Fill GTF Buffer with Command to enable Device
        // Initiated Aggressive Link Power Management
        // if Device supports it and has it disabled.
        // Otherwise, fill buffer with zeros. 
        Store (Arg1, DEVD)        // Device 
        Store (BUF0, Local1)
        If(LAnd(W780, 0x0002))    // If Supported
        {
            If(LEqual(And(W790, 0x0002), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x01, DESC)    // Non-zero buffer offset in DMA Setup FIS               
                Store(0xEF, COMD)    // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        If(LAnd(W780, 0x0004))    // If Supported
        {
            If(LEqual(And(W790, 0x0004), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x02, DESC)    // DMA Setup FIS Auto-Activate optimization
                Store(0xEF, COMD)    // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        If(LAnd(W780, 0x0008))    // If Supported
        {
            If(LEqual(And(W790, 0x0008), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x03, DESC)    // Device-initiated interface power state transitions
                Store(0xEF, COMD)     // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        If(LAnd(W780, 0x0010))    // If Supported
        {
            If(LEqual(And(W790, 0x0010), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x04, DESC)    // Guaranteed In-Order Data Delivery
                Store(0xEF, COMD)    // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        If(LAnd(W780, 0x0020))    // If Supported
        {
            If(LEqual(And(W790, 0x0020), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x05, DESC)    // Asynchronous Notification
                Store(0xEF, COMD)    // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        If(LAnd(W780, 0x0040))    // If Supported
        {
            If(LEqual(And(W790, 0x0040), 0x0000))    // If disabled
            {
                Store(0x10, FEAT)    // Enable SATA Features
                Store(0x06, DESC)    // Software Settings Preservation
                Store(0xEF, COMD)    // Set Features Command
                Concatenate (Local1, BUF0, Local1)
            }           
        }
        // Store(GBU0, GTF0)    // Finally Store the New GTF Buffer to during _GTF
        Return (Local1)
    }
    Name(GTF0, Buffer(56) { 0x00,0x00,0x00,0x00,0x00,0x00,0x00 })
    Device(PRT0)
    {
        Name(_ADR, 0x0000FFFF)        // Address for Port 0
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xA0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }
    }
    Device(PRT1)
    {
        Name(_ADR, 0x0001FFFF)        // Address for Port 1
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xB0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }
    }
    Device(PRT2)
    {
        Name(_ADR, 0x0002FFFF)        // Address for Port 2
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xA0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }
    }
    Device(PRT3)
    {
        Name(_ADR, 0x0003FFFF)        // Address for Port 3
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xB0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }
    }
    
    Device(PRT4)
    {
        Name(_ADR, 0x0004FFFF)        // Address for Port 4
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xA0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }        
    }
    
    Device(PRT5)
    {
        Name(_ADR, 0x0005FFFF)        // Address for Port 5
                        
        // Set Device Data
        
        Method (_SDD, 1) {
        
            Store (SDDA (Arg0, 0xB0), GTF0)
          
        }
        // Get Task File

        Method(_GTF)
        {
            Return(GTF0)
        }                
    }
}
}    
}
