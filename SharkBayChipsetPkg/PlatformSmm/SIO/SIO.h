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

#ifndef _SIO_H
#define _SIO_H

#include "Platform.h"

#define SMM_SIO_INDEX              0x164E
#define SMM_SIO_DATA               0x164F
#define SIO_SEL_LDN                0x07


typedef struct {
  UINT8           LdnNum;
  UINTN           RegNum;
  UINT8           *SioRegTable;
  UINT8           *SioRegTableSave;

} SIO_LDN_DEVICE;


#endif
