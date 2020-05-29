/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef LEGACY_TPM_H_
#define LEGACY_TPM_H_

#include "LegacyBiosInterface.h"
#include <Protocol/TcgService.h>
#include <Protocol/TrEEProtocol.h>


/**
  Measure Legacy BIOS Image.

  @param  LegacyBiosImageAddress    Start address of the Legacy BIOS image
  @param  LegacyBiosImageSize       Size of the Legacy BIOS image

  @retval EFI_SUCCESS               Measure Legacy BIOS Image successfully.
**/
EFI_STATUS
TcgMeasureLegacyBiosImage (
  IN  EFI_PHYSICAL_ADDRESS      LegacyBiosImageAddress,
  IN  UINTN                     LegacyBiosImageSize
  );


/**
  Load Tpm Binary Image.

  @param  Private                    Legacy BIOS context data
  @param  Table                      EFI Campatibility16 Table

  @retval EFI_SUCCESS                Load Tpm Binary Image successfully.
**/
EFI_STATUS
LoadTpmBinaryImage (
  IN LEGACY_BIOS_INSTANCE          *Private,
  IN EFI_COMPATIBILITY16_TABLE     *Table
  );

#endif
