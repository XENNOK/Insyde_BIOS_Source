/** @file

  This file contains definitions required for creation of SPS Policy HOB.

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

#ifndef _SPS_POLICY_HOB_H_
#define _SPS_POLICY_HOB_H_

#include <SpsPolicy.h>

#define SPS_POLICY_HOB_GUID \
  { 0xD84ADC93, 0x1EC5, 0x4821, { 0x96, 0x26, 0x0E, 0xF4, 0x0C, 0x90, 0x6C, 0x21 }}

typedef struct _SPS_POLICY             SPS_POLICY_HOB;

extern EFI_GUID gSpsPolicyHobGuid;

#endif
