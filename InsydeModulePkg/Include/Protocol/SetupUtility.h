/** @file
  Protocol for describing Setup Utility..

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

#ifndef _SETUP_UTILITY_H_
#define _SETUP_UTILITY_H_

#define SETUP_UTILITY_PROTOCOL_GUID  \
  { 0xFE3542FE, 0xC1D3, 0x4EF8, 0x65, 0x7c, 0x80, 0x48, 0x60, 0x6f, 0xf6, 0x70 }

typedef struct _EFI_SETUP_UTILITY_PROTOCOL EFI_SETUP_UTILITY_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_START_SETUP_UTILITY) (
  IN  struct _EFI_SETUP_UTILITY_PROTOCOL        *This
);

typedef
EFI_STATUS
(EFIAPI *EFI_POWERON_SECURITY) (
  IN  struct _EFI_SETUP_UTILITY_PROTOCOL        *This
);

struct _EFI_SETUP_UTILITY_PROTOCOL {
  BOOLEAN                               FirstIn;
  UINT8                                 *SetupNvData;
  EFI_START_SETUP_UTILITY               StartEntry;
  EFI_POWERON_SECURITY                  PowerOnSecurity;
};


extern EFI_GUID gEfiSetupUtilityProtocolGuid;

#endif
