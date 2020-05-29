/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
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

DefinitionBlock (
    "DSDT.aml",
    "DSDT",
//[-start-140521-IB05400527-modify]//
#if FeaturePcdGet(Pcd64BitAmlSupported)
//[-end-140521-IB05400527-modify]//
    0x02, // DSDT revision.
        // A Revision field value greater than or equal to 2 signifies that integers 
        // declared within the Definition Block are to be evaluated as 64-bit values
#else
    1,
#endif
    "Insyde ", // OEM ID (6 byte string)
    "HSW-LPT", // OEM table ID  (8 byte string)
    0x0 // OEM version of DSDT table (4 byte Integer)
    )

// BEGIN OF ASL SCOPE
{

External(LHIH)
External(LLOW)
External(IGDS)
External(LIDS)
External(BRTL)
External(ALSE)
External(GSMI)
External(\_SB.PCI0.GFX0.ALSI)
External(\_SB.PCI0.GFX0.CDCK)
External(\_SB.PCI0.GFX0.CBLV)
External(\_SB.PCI0.GFX0.GSSE)
External(\_SB.PCI0.PEG0, DeviceObj)
External(\_SB.PCI0.PEG0.PEGP, DeviceObj)
External(\_SB.PCI0.PEG1, DeviceObj)
External(\_SB.PCI0.PEG2, DeviceObj)
External(\_SB.PCI0.GFX0.DD1F, DeviceObj)
External(\_SB.PCI0.GFX0.GDCK, MethodObj)
External(\_SB.PCI0.GFX0.GHDS, MethodObj)
External(\_SB.PCI0.GFX0.AINT, MethodObj)
External(\_SB.PCI0.GFX0.GLID, MethodObj)
External(\_SB.PCI0.GFX0.GSCI, MethodObj)
External(\_PR.CPU0, DeviceObj)
External(\_PR.CPU0._PSS, MethodObj)
//[-start-130328-IB10310017-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
  External(\PSLM, IntObj)
  External(\_PR.CPU0._PPC, MethodObj)
#else
  External(\_PR.CPU0._PPC, IntObj)
#endif
//[-end-130328-IB10310017-add]//


// Miscellaneous services enabled in Project
    include ("AMLUPD.asl")
    include ("token.asl")
    include ("GloblNvs.asl")
    include ("OemGloblNvs.asl")
    include ("PciTree.asl")
    include ("Platform.asl")
    include ("CPU.asl")
    include ("THERMAL.ASL")
    include ("PCI_DRC.ASL")
    include ("Video.asl")
//[-start-130625-IB05400421-add]//
    include ("IDE.ASL")
//[-end-130625-IB05400421-add]//
//[-start-120817-IB03780458-modify]//
//[-start-130523-IB05160449-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdUltFlag)
//[-end-130812-IB05670190-modify]//
//[-end-130523-IB05160449-modify]//
    include ("GpeUlt.asl")
#else
    include ("Gpe.asl")
#endif
//[-end-120817-IB03780458-modify]//

    include ("PCIEDOCK.ASL")
    include ("Pch.asl")  // Not in this package. Refer to the PCH Reference Code accordingly
//[-start-120502-IB06460395-add]//
    include ("Memdbg.ASL")
//[-end-120502-IB06460395-add]//
//[-start-120821-IB06150238-modify]//
//[-start-120823-IB06150240-modify]//
//[-start-130523-IB05160449-modify]//
//[-start-130812-IB05670190-modify]//
#if !FeaturePcdGet(PcdUltFlag)
//[-end-130812-IB05670190-modify]//
//[-end-130523-IB05160449-modify]//
    include ("zpodd.asl")
#endif
//[-end-120823-IB06150240-modify]//
//[-end-120821-IB06150238-modify]//
//[-start-130328-IB10310017-add]//
//[-start-130812-IB05670190-modify]//
#if (!FeaturePcdGet(PcdMeSupported)) && (FeaturePcdGet(PcdSpsMeSupported))
//[-end-130812-IB05670190-modify]//
    include ("NodeMagmnt.asl") 
#endif // FeaturePcdGet(PcdSpsMeSupported)
//[-end-130328-IB10310017-add]//
//----------------------------------------------------------------------
    Name(\_S0, Package(4){0x0,0x0,0,0}) // mandatory System state
//[-start-130204-IB02950496-modify]//    
    if(0) { Name(\_S1, Package(4){0x1,0x0,0,0})}
//[-end-130204-IB02950496-modify]//    
    if(SS3) { Name(\_S3, Package(4){0x5,0x0,0,0})}
    if(SS4) { Name(\_S4, Package(4){0x6,0x0,0,0})}
    Name(\_S5, Package(4){0x7,0x0,0,0}) // mandatory System state

    Method(PTS, 1) {        // METHOD CALLED FROM _PTS PRIOR TO ENTER ANY SLEEP STATE
        If(Arg0)            // entering any sleep state
        {
        }
    }
    Method(WAK, 1) {        // METHOD CALLED FROM _WAK RIGHT AFTER WAKE UP
    }
}// End of ASL File

