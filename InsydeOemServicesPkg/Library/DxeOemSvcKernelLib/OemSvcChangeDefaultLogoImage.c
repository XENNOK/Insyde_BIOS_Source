/** @file
  Provide OEM to customize changing default logo image. .

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  Get OEM customization logo image. 
  Customization changing logo image provides OEM to replace deafult logo image.

  @param[in, out]  Logo              On Input with Logo pointing to deafult OEM Logo Data.
  																	 On Ouput, OEM replaces the content of OEM Logo data that Logo points to. 
  @param[in, out]  ImageData         On Input with ImageData pointing to deafult image data.
                                     On Output with ImageData pointing to new image data found in ROM
  @param[in, out]  ImageSize         On Input with ImageSize pointing to deafult image size.  
                                     On Output with ImageSize pointing to new image size.                          
  
  @retval      EFI_UNSUPPORTED             Returns unsupported by default.
  @retval      EFI_MEDIA_CHANGED           The value of IN OUT parameter is changed. 
  @retval      Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcChangeDefaultLogoImage (
  IN OUT EFI_OEM_BADGING_LOGO_DATA       *Logo,
  IN OUT UINT8                           **ImageData,
  UINTN                                  *ImageSize
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

