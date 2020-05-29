/** @file

   Intel Corporation - ACPI Reference Code for the Ivy Bridge
   Family of Customer Reference Boards.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

DefinitionBlock (
  "IsctAcpi.aml",
  "SSDT",
  1,
  "Intel_",
  "IsctTabl",
  0x1000
  )

{
  External(ICNF) //Isct Configuration
  External(\_SB.PCI0.LPCB.H_EC.IBT1)            // Isct Byte1 for EC
  External(\_SB.PCI0.LPCB.H_EC.IBT2)            // Isct Byte2 for EC
  External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj) // EC Command method
  External(\_SB.PCI0.LPCB.H_EC.WTMS)            // EC Wake Timer Settings
  External(\_SB.PCI0.LPCB.H_EC.AWT0)            // EC Wake Timer Value (BIT7-0)
  External(\_SB.PCI0.LPCB.H_EC.AWT1)            // EC Wake Timer Value (BIT15-8)
  External(\_SB.PCI0.LPCB.H_EC.AWT2)            // EC Wake Timer Value (BIT23-16)
  External(\_SB.PCI0.LPCB.H_EC.LSTE)            // Lid State (Lid Open = 1)
  External(\_SB.IFFS.FFSS)

  Scope (\_SB)
  {
    Device (IAOE)
    {
      OperationRegion(ISCT,SystemMemory,0xFFFF0008,0xAA58)
      Field(ISCT,AnyAcc,Lock,Preserve)
      {
        WKRS,  8,      // (0) ISCT Wake Reason
        AOCE,  8,      // (1) ISCT is Enabled
        FFSE,  8,      // (2) IFFS Enabled
        ITMR,  8,      // (3) ISCT Timer Type: 0 = EC, 1 = RTC  
        ECTM,  32,     // (4) ISCT EC Timer
        RCTM,  32,     // (8) ISCT RTC Timer
        GNPT,  32,     // (12)ISCT GlobalNvs Ptr
        ATOW,  8,      // (16)ISCT timer over write, 1 = overwrited as ISCT timer
      }
      Name (_HID, "INT33A0")
      Name (_UID, 0x00)

      Name (IBT1, 0)   // Isct Byte1 for EC (local)
      Name (IBT2, 0)   // Isct Byte2 for EC (local)
      Name (INSB, 0)   // Isct Notification Status Bit
      Name (WTMS, 0)   // EC Wake Timer Settings (local) 
      Name (AWT0, 0)   // EC Wake Timer Value (BIT7-0) (local)
      Name (AWT1, 0)   // EC Wake Timer Value (BIT15-8) (local)
      Name (AWT2, 0)   // EC Wake Timer Value (BIT23-16) (local)
      Name (PTSL, 0)   // Platform Sleep Level
      Name (SLPD, 0)   // Sleep duration for when using NetDetect (dummy)
      Name (IMDS, 0)   // Isct Mode Select
      Name (IWDT, 0)   // Isct Wake Duration Time 
 
      //
      // GABS - Get ISCT BIOS Enablign Setting
      // Input:   None
      // Return:   
      // Bits   Description       
      // 0      ISCT Configured: 0 = Disabled, 1 = Enabled 
      // 1      ISCT Notification Control: 0 = Unsupported, 1 = Supported 
      // 2      ISCT WLAN Power Control : 0 = Unsupported, 1 = Supported 
      // 3      ISCT WWAN Power Control : 0 = Unsupported, 1 = Supported 
      // 4      Must be set to 1 (EC Timer Support)
      // 5      Sleep duration value format: 0 = Actual time, 1 = duration in seconds
      // 6 - 7  Reserved 
      //
      Method (GABS, 0, NotSerialized)
      {
        Return (ICNF)
      }
    
      //
      // GAOS - Get ISCT Function Status
      // Input:   None
      // Return:   
      // Bits   Description   
      // 0      ISCT Mode: 0 = Disabled, 1 = Enabled 
      // 1      ISCT Wake Mode Select: 0 = ISCT Wake Mode, 1 = Extended ISCT Wake 
      // 2 - 7  Reserved 
      //
      Method (GAOS, 0, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {
          //
          //Get Isct Mode Enable
          //
          And(IBT1, 0x01, Local0)
          //
          //Get Isct Mode Selection
          //
          Or(Local0, And(IMDS, 0x02), Local0)
          Return (Local0)
        }
        Else
        {
          //
          //Get Isct Mode Enable
          //
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local0)
          }
          Else
          {	
            Store(IBT1, Local0)
          }
          And(Local0, 0x01, Local0)
          //
          // Get Isct Wake Mode Select
          //
          Or(Local0, And(IMDS, 0x02), Local0)
          Return (Local0)
        }
      }
    
      //
      // SAOS - Set ISCT Function Status
      // Input:   
      // Bits   Description 
      // 0      ISCT Mode: 0 = Disabled, 1 = Enabled 
      // 1      ISCT Wake Mode Select: 0 = ISCT Wake Mode, 1 = Extended ISCT Wake 
      // 2 - 7  Reserved 
      //
      Method (SAOS, 1, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {
          //
          //Set Isct Mode Selection
          //
          Store(And(Arg0, 0x02), IMDS)
          //
          //Set Isct Mode Enable
          //
          And(IBT1, 0xFE, Local0)
          Or(Local0, And(Arg0, 0x01), IBT1)
        }
        Else
        {
          //
          //Set Isct Mode Enable
          //
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local0)
          }
          Else
          {	
            Store(IBT1, Local0)
          }
          And(Local0, 0xFE, Local0)
          Or(Local0, And(Arg0, 0x01), Local0)
          //
          // Set Isct Wake Mode Select
          //
          Store(And(Arg0, 0x02), IMDS)
            //TBD: Low Power Fan control
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(Local0, \_SB.PCI0.LPCB.H_EC.IBT1)
            //Send EC IsctUpdateCommand - 0x2B
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2B)
          }
          Else
          {	
            Store(Local0, IBT1)
          }
        }
      }
    
      //
      // GANS - Get ISCT Notification Status
      // Input:   None
      // Return:   
      // Bits   Description 
      // 0      ISCT Notification : 0 = Disabled, 1 = Enabled 
      // 1 - 7  Reserved 
      //
      Method (GANS, 0, NotSerialized)
      {
        Return (INSB)
      }
    
      //
      // SANS - Set ISCT Notification Status
      // Input:   
      // Bits   Description 
      // 0      ISCT Notification : 0 = Disabled, 1 = Enabled 
      // 1 - 7  Reserved 
      //
      Method (SANS, 1, NotSerialized)
      {
        Store(And(Arg0, 0x01), INSB)
      }
    
      //
      // GWLS - Get WLAN Module Status
      // Input:   None
      // Return:   
      // Bits   Description 
      // 0      WLAN Wireless Disable (W_DISABLE#) :0 = Disabled, 1 = Enabled
      // 1      WLAN Module Powered in S3: 0 = Disabled, 1 = Enabled
      // 2      WLAN Module Powered in S4: 0 = Disabled, 1 = Enabled
      // 3      WLAN Module Powered in S5: 0 = Disabled, 1 = Enabled
      // 4 - 7  Reserved
      //
      Method (GWLS, 0, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {
//          Return (And(IBT1,0x0E))
          Return (And(IBT1,0x0F))
        }
        Else
        {
          //
          // Set WLAN Wireless Disable Bit to 1 if EC
          // 
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT2))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT2, Local0)
            Or(Local0, 1, Local0)
          }

          //
          // Get WLAN Powered States
          //   
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local1)
          }
          Else
          {
            Store(IBT1, Local1)
          }
//          Or(Local0, And(Local1, 0x0E), Local0)
          Or(Local0, And(Local1, 0x0F), Local0)
 
          Return (Local0)
        }
      }
      //
      // SWLS - Set WLAN Module Status
      // Input:  
      // Bits   Description 
      // 0      N/A (WLAN Wireless Disable is Read only)  Always set to 0 
      // 1      WLAN Module Powered in S3: 0 = Disabled, 1 = Enabled
      // 2      WLAN Module Powered in S4: 0 = Disabled, 1 = Enabled
      // 3      WLAN Module Powered in S5: 0 = Disabled, 1 = Enabled
      // 4 - 7  Reserved
      //
      Method (SWLS, 1, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {          
//          Store(And(And(Arg0,0x0E), IBT1), IBT1)
          Or(And(Arg0,0x0F), And(IBT1,0xF0), IBT1)
        }
        Else
        {
          //
          // Read ISCTByte1 from EC
          //
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local0)
          }
          Else
          {
            Store(IBT1, Local0)
          }

          //
          // Set new Power States
          //
          And(Local0, 0xF1, Local0)
          Or(Local0, And(Arg0, 0x0E), Local0)

          //
          // If RapidStart is enabled and WLAN powered in S3, enable power in S4
          //
          If(CondRefOf(\_SB.IFFS.FFSS))
          {
            If(LAnd(And(\_SB.IFFS.FFSS, 0x03), And(Arg0,0x02)))
            {  
              Or(Local0, 0x04, Local0)
            }
          }

          //
          // Save ISCTByte1 to EC
          //   
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(Local0, \_SB.PCI0.LPCB.H_EC.IBT1)
            //Send EC IsctUpdateCommand - 0x2B
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2B)
          }
          Else
          {
            Store(Local0, IBT1)
          }
        }
      }
    
      //
      // GWWS - Get WWAN Module Status
      // Input:   None
      // Return:   
      // Bits   Description 
      // 0      WWAN Wireless Disable (W_DISABLE#) :0 = Disabled, 1 = Enabled
      // 1      WWAN Module Powered in S3: 0 = Disabled, 1 = Enabled
      // 2      WWAN Module Powered in S4: 0 = Disabled, 1 = Enabled
      // 3      WWAN Module Powered in S5: 0 = Disabled, 1 = Enabled
      // 4 - 7  Reserved
      //
      Method (GWWS, 0, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {
//          ShiftRight(And(IBT1,0x70), 3, Local0)
          ShiftRight(And(IBT1,0xF0), 4, Local0)
          Return (Local0)
        }
        Else
        {
          //
          // Set WWAN Wireless Disable Bit to 1 if EC
          //
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT2))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT2, Local0)
            ShiftRight(Or(Local0, 2), 1, Local0)
          }

          //
          // Get WWAN Powered States
          // 
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local1)
          }
          Else
          {
            Store(IBT1, Local1)
          }
//          Or(Local0, ShiftRight(And(Local1, 0x70), 3), Local0)
          Or(Local0, ShiftRight(And(Local1, 0xF0), 4), Local0)

          Return (Local0)
        }
      }
    
      //
      // SWWS - Set WWAN Module Status
      // Input:  
      // Bits   Description 
      // 0      N/A (WWAN Wireless Disable is Read only)  Always set to 0 
      // 1      WWAN Module Powered in S3: 0 = Disabled, 1 = Enabled
      // 2      WWAN Module Powered in S4: 0 = Disabled, 1 = Enabled
      // 3      WWAN Module Powered in S5: 0 = Disabled, 1 = Enabled
      // 4 - 7  Reserved
      //
      Method (SWWS, 1, NotSerialized)
      {
        //
        // Check for RTC Timer, else EC timer
        //
        If (LEqual(ITMR, 1))
        {
//          Shiftleft(And(And(Arg0,0x0E), IBT1), 3, IBT1)
          And(Shiftleft(And(Arg0,0x0F), 4, Local0), 0xF0, Local0)
          Or(Local0, And(IBT1,0x0F), IBT1)
        }
        Else
        {
          //
          // Read ISCTByte1 from EC
          //
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(\_SB.PCI0.LPCB.H_EC.IBT1, Local0)
          }
          Else
          {
            Store(IBT1, Local0)
          }

          //
          // Set new Power States
          //
          And(Local0, 0x8F, Local0)
          Or(Local0, ShiftLeft(And(Arg0, 0x0E), 3), Local0)
          
          //
          // If RapidStart is enabled and WWAN powered in S3, enable power in S4
          //
          If(CondRefOf(\_SB.IFFS.FFSS))
          {
            If(LAnd(And(\_SB.IFFS.FFSS, 0x03), And(Arg0,2)))
            {
               Or(Local0, ShiftLeft(0x04, 3), Local0)
            }
          }

          //
          // Save ISCTByte1 to EC
          //   
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1))
          {
            Store(Local0, \_SB.PCI0.LPCB.H_EC.IBT1)
            //Send EC IsctUpdateCommand - 0x2B
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2B)
          }
          Else
          {
            Store(Local0, IBT1)
          }
        }
      }
      
      //
      // SASD - Set Intel Smart Connect Technology Sleep Duration
      // Input:  
      // Bits   Description 
      // 0 - 30 Sleep Duration
      // 31  : 0 = Sleep Duration in seconds    
      //       1 = Actual time
      //
      Method (SASD, 1, NotSerialized)
      {
        //
        // Arg0  =  0 - NetDetect support
        //       >  0 - EC / RTC Based Timer
        //       Bit31 : 0 - Sleep Duration in seconds
        //               1 - Actual Time
        //
        If(LAnd(And(ICNF, 0x10), LGreater(Arg0, 0)))
        {
          //
          // See if RTC (1) or EC (0) Timer
          //
          If (LEqual(ITMR, 1))
          {
            Store(Arg0, RCTM)
          }
          Else
          {
            Store(Arg0, ECTM)
          }

          If (LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.WTMS), LEqual(PTSL, 0x03)))
          {
             Store(And(ECTM, 0xFF), \_SB.PCI0.LPCB.H_EC.AWT0)
             Store(ShiftRight(And(ECTM, 0xFF00), 8), \_SB.PCI0.LPCB.H_EC.AWT1)
             Store(ShiftRight(And(ECTM, 0xFF0000), 16), \_SB.PCI0.LPCB.H_EC.AWT2)
             //
             // Enable EC timer - BIT7
             // Enable wake from S3 on timer exiry - BIT0
             //
             Store(\_SB.PCI0.LPCB.H_EC.WTMS, Local0)
             Store(Or(0x81,Local0), \_SB.PCI0.LPCB.H_EC.WTMS)
          }
          Else
          {
             Store(And(ECTM, 0xFF), AWT0)
             Store(ShiftRight(And(ECTM, 0xFF00), 8), AWT1)
             Store(ShiftRight(And(ECTM, 0xFF0000), 16), AWT2)
             Store(WTMS, Local0)
             Store(Or(0x81,Local0), WTMS)
          }
        }
        Else
        {
          Store(Arg0, SLPD)
        }
      } // Method (SASD)
    
      //
      // GPWR - Get Wake Reason
      // Input:   None
      // Return:   
      // Bits   Description 
      // 0      User Pressed Power Button: 0 = False, 1 = True
      // 1      EC / RTC caused wake (requested by iSCT agent): 0 = False, 1 = True
      // 2      RTC Timer caused wake: 0 = False, 1 = True
      // 3      Wake due to PME: 0 = False, 1 = True 
      // 4      BIOS programs EC or RTC timer for Intel Smart Connect Technology wake
      // 5 - 7  Reserved
      //
      Method (GPWR, 0, NotSerialized)
      {
        //
        // Return Wake Reason
        //
        If(And(WKRS,0x1F))
        {
          Return(WKRS)
        }
        Else
        {
          Return(0)
        }   
      } // Method (GPWR)

      //
      // GPCS - Get Platform Component State
      // Input: None
      // Return:
      // Bits   Description 
      // 0      LID State: 0 - Closed, 1 - Open
      // 1 - 7  Reserved
      //
      Method (GPCS, 0, NotSerialized)
      {
        //
        // See if EC (0) or RTC (1) Timer
        //
        If (LEqual(ITMR, 0))
        {
          If (CondRefOf(\_SB.PCI0.LPCB.H_EC.LSTE))
          {
            Return(And(\_SB.PCI0.LPCB.H_EC.LSTE,0x01))
          }
        }
        Else
        {
          Return(1)
        }
      } // Method (GPCS)

      //
      // GAWD - Get Isct Wake Duration
      // Input: None
      // Return:
      // Bits   Description 
      // 0 - 7  Isct Wake Duration
      //
      Method (GAWD, 0, NotSerialized)
      {
        Return (IWDT)
      }

      //
      // SAWD - Set Isct Wake Duration
      // Input:
      // Bits   Description 
      // 0 - 7  Isct Wake Duration
      //
      Method (SAWD, 1, NotSerialized)
      {
        Store (Arg0, IWDT)
          //TBD Set EC Watchdog Timer
      }

    } // Device (IAOE)
  } // Scope (\_SB)
} // End SSDT   


