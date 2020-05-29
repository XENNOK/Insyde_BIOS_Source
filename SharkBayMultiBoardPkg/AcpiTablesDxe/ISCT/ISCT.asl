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

//[-start-120604-IB06150223-modify]//
DefinitionBlock (
	"ISCT.aml",
	"SSDT",
	0x01,
	"INTEL_",
	"ISCT",
	0x1000
	)
{
External(\ISCT, IntObj)
External(\ALAT, IntObj)
External(\SLPS, IntObj)
//[-start-120220-IB04770204-add]//
External(PEWS, IntObj)
//[-end-120220-IB04770204-add]//
  Scope (\_SB)
  {
    Device (IAOE)
    {
        Name (_HID, "INT33A0")
        Name (_UID, 0x00)
        Name (AOS1, 0)
        Name (ANS1, 0)
        Name (WLS1, 1)
        Name (WWS1, 1)
        Name (ASDS, 0)
        Name (ALA1, 0)
        //
        // GABS - Get Intel Smart Connect Technology BIOS Enabled Setting
        // Input:   None
        // Return:   
        // Bits   Description
        // 0      Intel Smart Connect Technology Configured: 0 = Disabled, 1 = Enabled
        // 1      Intel Smart Connect Technology Notification Control: 0 = Unsupported, 1 = Supported
        // 2      Intel Smart Connect Technology WLAN Power Control:0 = Unsupported, 1 = Supported
        // 3      Intel Smart Connect Technology WWAN Power Control: 0 = Unsupported, 1 = Supported
//[-start-120525-IB03600485-modify]//
        // 4      Reserved (must set to 1)
        // 5      Sleep duration value format: 0 = Actual time, 1 = duration in seconds
        // 6      RF Kill Support (Radio On/Off): 0 = Soft Switch, 1 = Physical Switch
        // 7      Reserved (must set to 0)
        //
//[-end-120525-IB03600485-modify]//
        Method (GABS, 0, NotSerialized)
        {
//[-start-120113-IB04770191-remove]//
//           If (LEqual (ShiftRight (ASDS, 31), 0x01))
//           {
//             And (ISCT, Not (0x20), ISCT)
//           }
//           Else
//           {
//             Or (ISCT, 0x20, ISCT)
//           }
//[-end-120113-IB04770191-remove]//
          Return (ISCT)
        }
    
        //
        // GAOS - Get Intel Smart Connect Technology Function Status
        // Input:   None
        // Return:   
        // Bits   Description   
        // 0      Intel Smart Connect Technology Mode: 0 = Disabled, 1 = Enabled
        // 1 - 7  Reserved 
        //
        Method (GAOS, 0, NotSerialized)
        {
          Return (AOS1)
        }
    
        //
        // SAOS - Set Intel Smart Connect Technology Function Status
        // Input:   
        // Bits   Description 
        // 0      Intel Smart Connect Technology Mode: 0 = Disabled, 1 = Enabled
        // 1 - 7  Reserved 
        //
        Method (SAOS, 1, NotSerialized)
        {      
           If (LEqual (And(Arg0, 0x01), 0x01))
           {
             Store(1, AOS1)                      
           }
           Else
           {
             Store(0, AOS1)  
           }
        }

    
        //
        // GANS - Get Intel Smart Connect Technology Notification Status
        // Input:   None
        // Return:   
        // Bits   Description 
        // 0      Intel Smart Connect Technology Notification: 0 = Disabled, 1 = Enabled
        // 1 - 7  Reserved 
        //
        Method (GANS, 0, NotSerialized)
        {
          Return (ANS1)
        }
    
        //
        // SANS - Set Intel Smart Connect Technology Notification Status
        // Input:   
        // Bits   Description 
        // 0      Intel Smart Connect Technology Notification: 0 = Disabled, 1 = Enabled
        // 1 - 7  Reserved 
        //
        Method (SANS, 1, NotSerialized)
        {
           If (LEqual (And(Arg0, 0x01), 0x01))
           {
             Store(1, ANS1)                      
           }
           Else
           {
             Store(0, ANS1)  
           }            
        }
    
        //
        // GWLS - Get WLAN Module Status
        // Input:   None
        // Return:   
        // Bits   Description 
        // 0      WLAN Wireless Disable (W_DISABLE#):0 = Disabled, 1 = Enabled
        // 1      WLAN Module Powered in S3: 0 = Disabled, 1 = Enabled
        // 2      WLAN Module Powered in S4 (FFS): 0 = Disabled, 1 = Enabled
        // 3      WLAN Module Powered in S5: 0 = Disabled, 1 = Enabled
        // 4 - 7  Reserved
        //
        Method (GWLS, 0, NotSerialized)
        {
          // Code should be changed according to HW defintion 
          Return (WLS1)
        }
        //
        // SWLS - Set WLAN Module Status
        // Input:  
        // Bits   Description 
        // 0      N/A (WLAN Wireless Disable is Read only)  Always set to 0 
        // 1      WLAN Module Powered in S3: 0 = Disabled, 1 = Enabled
        // 2      WLAN Module Powered in S4 (SFF): 0 = Disabled, 1 = Enabled
        // 3      WLAN Module Powered in S5: 0 = Disabled, 1 = Enabled
        // 4 - 7  Reserved
        //
        Method (SWLS, 1, NotSerialized)
        {
          // Code should be changed according to HW defintion              
          Store(Arg0, WLS1)  
        }
    
        //
        // GWWS - Get WWAN Module Status
        // Input:   None
        // Return:   
        // Bits   Description 
        // 0      WWAN Wireless Disable (W_DISABLE#):0 = Disabled, 1 = Enabled
        // 1      WWAN Module Powered in S3: 0 = Disabled, 1 = Enabled
        // 2      WWAN Module Powered in S4 (SFF): 0 = Disabled, 1 = Enabled
        // 3      WWAN Module Powered in S5: 0 = Disabled, 1 = Enabled
        // 4 - 7  Reserved
        //
        Method (GWWS, 0, NotSerialized)
        {
          // Code should be changed according to HW defintion 
          Return (WWS1)
        }
    
        //
        // SWWS - Set WWAN Module Status
        // Input:  
        // Bits   Description 
        // 0      N/A (WWAN Wireless Disable is Read only)  Always set to 0 
        // 1      WWAN Module Powered in S3: 0 = Disabled, 1 = Enabled
        // 2      WWAN Module Powered in S4 (SFF): 0 = Disabled, 1 = Enabled
        // 3      WWAN Module Powered in S5: 0 = Disabled, 1 = Enabled
        // 4 - 7  Reserved
        //
        Method (SWWS, 1, NotSerialized)
        {
          // Code should be changed according to HW defintion 
          Store(Arg0, WWS1)  
        }

        // 
        // SASD - Set Intel Smart Connect Technology Sleep Duration 
        // Input:   
        // Bits   Description  
        // 0 - 31 Sleep Duration value in seconds (e.g. 900)
        // 
        Method (SASD, 1, NotSerialized) 
        { 
          // Code should be changed if EC utilizes this value
          Store(Arg0, ASDS)  
        } 
 
        // 
        // GPWR - Get Platform Wake Reason
        // Input:   None 
        // Return:    
        // Bits   Description  
        // 0      User pressed power button or HID event
        // 1      EC timer caused wake
        // 2      RTC timer caused wake
        // 3      Wake due to PME (This can be from any PCI-Ex based device 
        //        including from USB xHCI controllers caused by USB device 
        //        wake-up events).  BIOS must set PME_EN = 1 and examine the 
        //        PME_STS value and set this bit appropriately
        // 4      Set to 1 if BIOS programs EC or RTC timer for Intel Smart 
        //        Connect Technology wake (bit 1 or 2 set also) 
        // 5 - 7  Reserved (set to 0)
        // 
        Method (GPWR, 0, NotSerialized) 
        { 
          // Code should be changed according to wake activity  
          //
          // ISCT/ISCT for 2012 Platforms Platform Design Specification Revision 0.71
          //
          // the BIOS can compare the time passed by SASD and RTC value after resume from S3 or S4.
          // If the two values match, the BIOS informs the Agent by returning Bit 2 = 1 (RTC timer) 
          // when the GPWR ACPI control method is called by the Agent upon resume.
          //
          Store(ALAT, ALA1)
          Decrement(ALA1)
//[-start-120220-IB04770204-modify]//
          If (LAnd(LEqual (And(AOS1, 0x01), 0x01), LAnd(LEqual (SLPS, 0x03), LEqual(ALA1, ASDS)))) {
            Return (0x04) 
          } ElseIf (LEqual (PEWS, 0x01)) {
            Return (0x08) 
          } Else {
            Return (0x01)
          }
//[-end-120220-IB04770204-modify]//
        } 
    } // Device (IAOE)
  } // Scope (\_SB)
} 
//[-end-120604-IB06150223-modify]//
