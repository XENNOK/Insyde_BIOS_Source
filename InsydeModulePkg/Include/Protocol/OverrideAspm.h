/** @file
  OverrideAspm Protocol.

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

#ifndef __OVERRIDE_ASPM_PROTOCOL_H__
#define __OVERRIDE_ASPM_PROTOCOL_H__

#define EFI_OVERRIDE_ASPM_PROTOCOL_GUID \
  { \
    0x4CD150E9, 0x016B, 0x4e54, { 0xBD, 0x00, 0x5C, 0x86, 0xEB, 0xE6, 0x37, 0xD6 } \
  }

typedef
EFI_STATUS
(EFIAPI *OVERRIDE_ASPM_FUNC) (
  VOID             
);

typedef struct _EFI_OVERRIDE_ASPM_PROTOCOL {
  OVERRIDE_ASPM_FUNC    OverrideAspmFunc;
} EFI_OVERRIDE_ASPM_PROTOCOL;

extern EFI_GUID gEfiOverrideAspmProtocolGuid;

#endif
