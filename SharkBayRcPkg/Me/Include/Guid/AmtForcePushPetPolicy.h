/** @file

  AmtForcePushPetPolicy Guid definitions

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

#ifndef _EFI_AMT_FORCE_PUSH_PET_POLICY_GUID_H_
#define _EFI_AMT_FORCE_PUSH_PET_POLICY_GUID_H_

#define AMT_FORCE_PUSH_PET_POLICY_GUID \
  { 0xACC8E1E4, 0x9F9F, 0x4E40, { 0xA5, 0x7E, 0xF9, 0x9e, 0x52, 0xf3, 0x4c, 0xa5 }}

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  INT32             MessageType[1];
} AMT_FORCE_PUSH_PET_POLICY_HOB;

extern EFI_GUID gAmtForcePushPetPolicyGuid;

#endif
