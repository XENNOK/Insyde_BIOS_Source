/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _ACPI_PLATFORM_COMMON_H_
#define _ACPI_PLATFORM_COMMON_H_

//
// Statements that include other header files
//
#include <Library/UefiLib/UefiLibInternal.h>
// MdePkg
#include <PiDxe.h>
#include <ChipsetSmiTable.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SLP2_0.h>
#include <IndustryStandard/Oa3_0.h>
//
// OemServicesPkg
//
#include <OemServices/Kernel.h>
#include <OemServicesSupport.h>
//
// Consumed protocols
//
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/AcpiSupport.h>
//[-start-130911-IB05670199-remove]//
//#include <Protocol/OemServices.h>
//[-end-130911-IB05670199-remove]//
#include <Guid/AcpiTableStorage.h>

/**
 Load a firmware volume image to buffer given its Guid.

 @param[in]          NameGuid    Guid of image to be found.
 @param[in, out]   Buffer          buffer to place image.
 @param[in, out]   Size             size of image to load.
 
 @retval   EFI_SUCCESS            If image is successfully loaded
 @retval   EFI_NOT_FOUND       If image is not found.
 @retval   EFI_LOAD_ERROR      If we have problems loading the image.
*/
EFI_STATUS
GetFvImage (
  IN EFI_GUID   *NameGuid,
  IN OUT VOID   **Buffer,
  IN OUT UINTN  *Size
  );

#endif
