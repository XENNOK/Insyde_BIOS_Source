/** @file

  Copies the memory related timing and configuration information into the
  Compatible BIOS data (BDAT) table.

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

#ifndef _BdatRmtHandler_h_
#define _BdatRmtHandler_h_

#ifndef BDAT_SUPPORT
#define BDAT_SUPPORT
#endif

#include "MemInfoHob.h"

#define EFI_BDAT_TABLE_SIGNATURE  SIGNATURE_32 ('B', 'D', 'A', 'T')
#define ACPI_BDAT_OEM_REV         0x00001000
#define EFI_BDAT_ACPI_POINTER     0x0

#endif /// _BdatRmtHandler_h_
