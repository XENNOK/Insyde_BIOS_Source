/** @file
  Definition for Smm Oem Services Protocol.

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

#ifndef _SMM_OEM_SERVICES_PROTOCOL_H_
#define _SMM_OEM_SERVICES_PROTOCOL_H_

#define SMM_OEM_SERVICES_PROTOCOL_GUID \
  { \
  0xa03b8f9b, 0x1de4, 0x4d1a, 0x80, 0x49, 0x54, 0x22, 0x99, 0x7e, 0xa6, 0xd2 \
  }

typedef struct _SMM_OEM_SERVICES_PROTOCOL SMM_OEM_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *OEM_SERVICES_FUNCTION) (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

struct _SMM_OEM_SERVICES_PROTOCOL {
  OEM_SERVICES_FUNCTION             *Funcs;
  OEM_SERVICES_FUNCTION             *FuncsEx;
};

extern EFI_GUID gSmmOemServicesProtocolGuid;

#endif
