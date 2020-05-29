//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;   Protocol for describing Setup Utility..
//;

#ifndef _EFI_SETUP_UTILITY_H_
#define _EFI_SETUP_UTILITY_H_

#define EFI_SETUP_UTILITY_PROTOCOL_GUID  \
  { 0xFE3542FE, 0xC1D3, 0x4EF8, 0x65, 0x7c, 0x80, 0x48, 0x60, 0x6f, 0xf6, 0x70 }

EFI_FORWARD_DECLARATION (EFI_SETUP_UTILITY_PROTOCOL);

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

typedef struct _EFI_SETUP_UTILITY_PROTOCOL {
  BOOLEAN                               FirstIn;
  UINT8                                 *SetupNvData;
  EFI_START_SETUP_UTILITY               StartEntry;
  EFI_POWERON_SECURITY                  PowerOnSecurity;
} EFI_SETUP_UTILITY_PROTOCOL;


extern EFI_GUID gEfiSetupUtilityProtocolGuid;

#endif
