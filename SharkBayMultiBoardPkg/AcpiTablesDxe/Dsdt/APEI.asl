//;******************************************************************************
//;* Copyright (c) 2013-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
//This method is used to turn on/off Whea support for BIOS 
//
Method (SWWE, 1)   //SW SMI Whea Enable
{
  //
  //SSMP - IO 0xB2 port
  //
  Store (Arg0, SSMP)
} //end SWWE

//
// Define WHEA device object with PNP0C33 (as per Whea platform guide 1.0) that will be 
// used by OS to install WHEA driver.
//
Device (WERR)
{
  Name(_HID,EISAID("PNP0C33"))

  Method (_STA)
  {
    //
    // Check the system to see if it is Longhorn Windows Server. If TRUE, return 0xF.
    //
    //   OSYS = 2009 = Windows 7 and Windows Server 2008 R2.
    //   OSYS = 2012 = Windows 8 and Windows Server 2012.
    //   OSYS = 2013 = Windows 8.1 and Windows Server 2012 R2
    If(LGreaterEqual(\OSYS,2009))
    {
      Return (0xF)
    }
    Else
    {
      Return (0)
    }
  }
}

//
// Our platform will generate SCI to indicate
// all Correctable/recovered errors. This GPE event will have to notify OS via WHEA device 
// about the error.
// Note: BIOS Error handler must build error data info before triggering SCI.
//
Method (\_GPE._L10, 0, NotSerialized)
{
  //
  // Notify a WHEA error event (actually for all CE's)
  //
  Notify (\_SB.WERR, 0x80)
}

//
// _OSC method will used by the OS the check platform capability for WHEA supprot. Also
// OS will use to indicate BIOS to enable WHEA support. BIOS will also can deny OS's request
// to enable WHEA if it's disable by platform or SCU policy (for e.g. WHEA siabled in Setup).
//    
//[-start-120402-IB06690168-modify]//
//[-start-130402-IB10310020-modify]//
//[-start-130402-IB10310020-remove]//
//Method (_OSC , 4)
////Method (OSCW , 4)
////[-end-130402-IB10310020-modify]//
//{
//  //
//  // Create DWORD Args from the Capabilities Buffer  
//  //
//  CreateDWordField(Arg3,0,WCB1) // WCB - WHEA Capabilities Buffer  //
//  CreateDWordField(Arg3,4,WCB2) // WCB2 - WHEA Capabilities Buffer 2 //
////[-start-120410-IB06690171-remove]//
////  CreateDWordField(Arg3,8,WCB3) // WCB3 - WHEA Capabilities Buffer 3 //
////[-end-120410-IB06690171-remove]//
//  
////[-start-111110-IB06040186-modify]//
//  // Check for WHEA UUID
//  //
//  If(LEqual(Arg0,ToUUID("ed855e0c-6c90-47bf-a62a-26de0fc5ad5c")))
//  {
//    //
//    // If OS not querying, enable WHEA in platform
//    //
//    If(Not(And(WCB1,1)))
//    {
//      //
//      // 2nd Dword of Arg3 bit 0: OS WHEA Implemented  
//      //
//      If(And(WCB2,0x01))
//      {
//        SWWE (0x9E)
////        Return(Arg3)
//      }
//    }
//  }
//  Else
//  {
//    //
//    // Set Unrecognized UUID
//    //
//    Or(WCB1,4,WCB1)
//  }
//
////  //
////  // Check for APEI -GUID
////  //
////  If(LEqual(Arg0, ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
////  {
////    //
////    // If OS not querying, enable WHEA in platform
////    //
////    If(Not(And(WCB1,1)))
////    {
////      // If OS not querying, enable WHEA in platform
////      SWWE (0x9E)
////      Return(Arg3)  
////    }
////  }
//  Return(Arg3)
////[-end-111110-IB06040186-modify]//
//}
//[-end-130402-IB10310020-remove]//
//[-end-120402-IB06690168-modify]//

