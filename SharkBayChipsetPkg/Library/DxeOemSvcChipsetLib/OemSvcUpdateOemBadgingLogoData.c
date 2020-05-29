/** @file
  This function provides an interface to modify OEM Logo and POST String.
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
#include <Uefi.h>
#include <Guid/Pcx.h>

#define EFI_INSYDE_BOOT_BADGING_GUID \
  { \
    0x931F77D1, 0x10FE, 0x48bf, 0xAB, 0x72, 0x77, 0x3D, 0x38, 0x9E, 0x3F, 0xAA \
  }

EFI_OEM_BADGING_LOGO_DATA gOemBadgingData[] = {
  //
  // BIOS Vendor Insyde Badge
  //
  { EFI_INSYDE_BOOT_BADGING_GUID,
   EfiBadgingSupportFormatBMP,
   EfiBadgingSupportDisplayAttributeCustomized,
   0,
   0,
   NULL,
   EfiBadgingSupportImageBoot
  },
  { EFI_DEFAULT_PCX_LOGO_GUID,
   EfiBadgingSupportFormatPCX,
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageLogo
  }
};

OEM_BADGING_STRING gOemBadgingString[] = {
  //
  // OEM can modify the background and foreground color of the OEM badging string through through the below data
  // for example:
  //   { 50, 280, { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
  //     { 0x00, 0x00, 0x00, 0x00 } indicate the foreground color { Blue, Green, Red, Reserved }
  //     { 0xFF, 0xFF, 0xFF, 0x00 } indicate the background color { Blue, Green, Red, Reserved }
  //
  { 30, 30, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL },
  { 50,  0, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC   ), NULL }
};
#endif
//[-start-121115-IB10820164-add]//
/**
 This function provides an interface to modify OEM Logo and POST String.

 @param[in out]     *EFI_OEM_BADGING_LOGO_DATA                   On entry, points to a structure that specifies image data.
                                                                 On exit , points to updated structure.
 @param[in out]     *BadgingDataSize    On entry, the size of EFI_OEM_BADGING_LOGO_DATA matrix.
                                        On exit , the size of updated EFI_OEM_BADGING_LOGO_DATA matrix.
 @param[in out]     *OemBadgingString   On entry, points to OEM_BADGING_STRING matrix.
                                        On exit , points to updated OEM_BADGING_STRING matrix.
 @param[in out]     *OemBadgingStringInTextMode                  On entry, points to OEM_BADGING_STRING matrix in text mode.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix in text mode.
 @param[in out]     *StringCount        The number is POST string count.
                                        On entry, base on SetupVariable->QuietBoot 
                                                  1 : The number of entries in OemBadgingString,
                                                  0 : The number of entries in OemBadgingStringInTextMode.
                                        On exit , base on SetupVariable->QuietBoot 
                                                  1 : The number of entries in updated OemBadgingString,
                                                  0 : The number of entries in updated OemBadgingStringInTextMode.
 @param[in out]     *OemBadgingStringAfterSelectWithMe           On entry, points to OEM_BADGING_STRING matrix after selected.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix after selected.
 @param[in out]     *OemBadgingStringAfterSelectWithMeInTextMode On entry, points to OEM_BADGING_STRINGmatrix after selected  in text mode.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix after selected  in text mode.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
//[-start-121128-IB05280020-modify]//
EFI_STATUS
OemSvcUpdateOemBadgingLogoData (
  IN OUT EFI_OEM_BADGING_LOGO_DATA            **EfiOemBadgingLogoData,
  IN OUT UINTN                                *BadgingDataSize,
  IN OUT OEM_BADGING_STRING                   **OemBadgingString,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringInTextMode,
  IN OUT UINTN                                *StringCount,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringAfterSelectWithMe,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringAfterSelectWithMeInTextMode
  )
//[-end-121128-IB05280020-modify]//
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  (*EfiOemBadingLogoData) = gOemBadgingData;
  *BadgingDataSize = sizeof (gOemBadgingData) / sizeof (EFI_OEM_BADGING_LOGO_DATA);
  (*OemBadgingString) = gOemBadgingString;
  (*OemBadgingStringInTextMode)[0].X =60;
#endif
  return EFI_UNSUPPORTED;
}


