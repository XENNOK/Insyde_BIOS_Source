/** @file

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

#ifndef _DDR3L_VOLTAGE_HOB_H_
#define _DDR3L_VOLTAGE_HOB_H_

#include <Guid/DDR3LVoltageVariable.h>

#define DDR3L_VOLTAGE_HOB \
  { \
    0xB5F05414, 0xDC82, 0x4323, 0x9B, 0xB9, 0x8B, 0x61, 0x83, 0xA2, 0x76, 0x43 \
  }

extern EFI_GUID gDDR3LVoltageHobGuid;

#endif
