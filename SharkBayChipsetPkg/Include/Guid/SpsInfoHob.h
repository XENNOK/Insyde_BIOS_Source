/** @file

  This file contains definitions required for creation of SPS Info HOB.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_INFO_HOB_H_
#define _SPS_INFO_HOB_H_

#include <SpsInfo.h>

#define SPS_INFO_HOB_GUID \
  { 0xBA69FB72, 0x4C84, 0x4835, { 0x8A, 0xCE, 0x2A, 0x89, 0x12, 0xBD, 0x81, 0x07 }}

typedef struct _SPS_INFO               SPS_INFO_HOB;

extern EFI_GUID gSpsInfoHobGuid;

#endif
