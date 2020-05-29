/** @file
    This function provides an interface to update IGD OpRegion content.
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

#include <Library/DxeOemSvcChipsetLib.h>
#if 0 // Sample Implementation
#define WORD_FIELD_VALID_BIT  0x8000
#endif

/**

 This function provides an interface to update IGD OpRegion content.

 @param [in out]    *IgdOpRegion        On entry, points to memorys buffer for Internal graphics device, 
                                        this buffer set aside comminocate between ACPI code and POST.
                                        On exit , points to updated memory buffer for Internal Graphics Device.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcSetIgdOpRegion (
  IN OUT  IGD_OPREGION_PROTOCOL     *IgdOpRegion
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
//[-start-130110-IB11410040-modify]//
  IgdOpRegion->OpRegion->MBox1.CLID = 3;
//[-end-130110-IB11410040-modify]//
  //
  // <EXAMPLE> Create a static Backlight Brightness Level Duty cycle Mapping Table
  // Possible 20 entries (example used 10), each 16 bits as follows:
  // [15] = Field Valid bit, [14:08] = Level in Percentage (0-64h), [07:00] = Desired duty cycle (0 - FFh). 
  //
  //
  // For brightness adjustment inversely issue, please refer to 
  //   SCU->Advanced->Video Configuration->Internal Graphic Device->Backlight Control, choose inverted or normal in this option.
  //
  // For modify backlight level, you can modify below BCLM[n] Table.
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[0] = (0x00FF + WORD_FIELD_VALID_BIT);
  //
  // 0%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[1] = (0x0AE5 + WORD_FIELD_VALID_BIT);
  //
  // 10%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[2] = (0x14CC + WORD_FIELD_VALID_BIT);
  //
..// 20%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[3] = (0x1EB2 + WORD_FIELD_VALID_BIT);
  //
  // 30%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[4] = (0x2899 + WORD_FIELD_VALID_BIT);
  //
  // 40%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[5] = (0x327F + WORD_FIELD_VALID_BIT);
  //
  // 50%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[6] = (0x3C66 + WORD_FIELD_VALID_BIT);
  //
  // 60%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[7] = (0x464C + WORD_FIELD_VALID_BIT);
  //
  // 70%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[8] = (0x5033 + WORD_FIELD_VALID_BIT);
  //
  // 80%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[9] = (0x5A19 + WORD_FIELD_VALID_BIT);
  //
  // 90%
  //
  IgdOpRegion->OpRegion->MBox3.BCLM[10] = (0x6400 + WORD_FIELD_VALID_BIT);
  //
  // 100%
  //
#endif
  return EFI_UNSUPPORTED;
}
