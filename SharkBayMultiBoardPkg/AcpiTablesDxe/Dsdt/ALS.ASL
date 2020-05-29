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



Device(ALSD) 
{
  Name(_HID,"ACPI0008")

  Method(_STA,0)
  {
    If(LEqual(ALSE,2))
    {
      Return(0x000B)    // ALS Enabled.  Don't show it in UI.
    }

    Return(0x0000)      // ALS Disabled.  Hide it.
  }

  Method(_ALI)
  {
    Return (Or(ShiftLeft(LHIH,8),LLOW))
  }

  Name(_ALR, Package()
  {
    Package() {70,  0},
    Package() {73,  10},
    Package() {85,  80},
    Package() {100, 300},
    Package() {150, 1000}
  })
  
}
