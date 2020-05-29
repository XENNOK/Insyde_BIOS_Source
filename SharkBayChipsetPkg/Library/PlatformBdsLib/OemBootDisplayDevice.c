/** @file
  PlatformBdsLib

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "OemBootDisplayDevice.h"


/**
  Oem Boot Display Device CallBack Function

  @param  SetupNVRam     
  @param  LegacyBios     

  @retval TRUE           There was a BIOS erro in the target code.
  @retval FALSE          Thunk completed, and there were no BIOS errors in the target code.
                         See Regs for status.

**/
EFI_STATUS
OemBootDisplayDeviceCallBack (
  IN  CHIPSET_CONFIGURATION      *SetupNVRam,
  IN  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios
  )
{
  EFI_STATUS                    Status;
  EFI_IA32_REGISTER_SET         Regs;   

  Status = EFI_SUCCESS;
  if (SetupNVRam->PEGFlag != 1) {             // IGD
    //
    //  VBIOS INT 10h : 5F64h, 00h - Set Display Device
    //
    //  Calling Registers: 
    //    AX  = 5F64h, Switch Display Device function 
    //    BH  = 00h, Set Display Device sub-function 
    //    BL  = Set Display Device Options: 
    //         Bits 7 - 1    = Reserved 
    //         Bit 0      = Force complete execution even if same display device 
    //    CX = Display Combination to Set (1 = Enable display, 0 = Disable display): 
    //         Bit 15 = Pipe B - LFP2  
    //         Bit 14 = Pipe B - EFP2  
    //         Bit 13 = Pipe B - EFP3
    //         Bit 12 = Pipe B - CRT2  
    //         Bit 11 = Pipe B - LFP  
    //         Bit 10 = Pipe B - EFP  
    //         Bit 9  = Pipe B - TV  
    //         Bit 8  = Pipe B - CRT  
    //         Bit 7  = Pipe A - LFP2 
    //         Bit 6  = Pipe A - EFP2 
    //         Bit 5  = Pipe A - EFP3
    //         Bit 4  = Pipe A - CRT2 
    //         Bit 3  = Pipe A - LFP 
    //         Bit 2  = Pipe A - EFP 
    //         Bit 1  = Pipe A - TV 
    //         Bit 0  = Pipe A - CRT 
    //  Return Registers: 
    //    AX  = Return Status (function not supported if AL != 5Fh): 
    //         = 005Fh, Function supported and successful 
    //         = 015Fh, Function supported but failed 
    //                  
    if (SetupNVRam->IGDBootType != 0) {      // Not VBT Default
      Regs.X.CX = (UINT16)SetupNVRam->IGDBootType;
      if (SetupNVRam->IGDBootType != 0 && (SetupNVRam->IGDBootType & SetupNVRam->IGDBootTypeSecondary) == 0) {
        Regs.X.CX |= (UINT16)SetupNVRam->IGDBootTypeSecondary << 8;
      }
      Regs.X.AX = 0x5F64;
      Regs.X.BX = 0x0001;                    // Force complete execution even if same display device
      Status = LegacyBios->Int86 (LegacyBios, 0x10, &Regs);
    }
  }

  return Status;
}
