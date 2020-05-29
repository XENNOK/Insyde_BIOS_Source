/** @file
   PPI definition for H2O_FLASH_PPI
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

#ifndef _H2O_FLASH_PPI_H_
#define _H2O_FLASH_PPI_H_

#include <Protocol/H2OFlash.h>

#define H2O_FLASH_PPI_GUID \
  { 0x12250CFA, 0x3583, 0x4FD5, {0x84, 0xA7, 0xDE, 0xC1, 0x6B, 0xF6, 0xFD, 0x48}}


typedef H2O_FLASH_PROTOCOL H2O_FLASH_PPI;

extern EFI_GUID gH2OFlashPpiGuid;

#endif
