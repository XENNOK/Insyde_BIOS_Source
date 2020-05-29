//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SMART_BOOT_PROTOCOL_GUID_H_
#define _SMART_BOOT_PROTOCOL_GUID_H_

#include EFI_GUID_DEFINITION (SmartBootPolicy)

#define EFI_SMART_BOOT_PROTOCOL_GUID  \
  { \
    0x8e5862ff, 0xf5f4, 0x432a, 0xa5, 0x5d, 0xe2, 0x3f, 0x83, 0xc6, 0x6f, 0x38 \
  }

EFI_FORWARD_DECLARATION (EFI_SMART_BOOT_PROTOCOL);

#define EFI_FAST_BOOT_DISABLE       FAST_BOOT_DISABLE_STATE
#define EFI_FAST_BOOT_ENABLE        FAST_BOOT_ENABLE_STATE

typedef
EFI_STATUS
(EFIAPI *EFI_SMART_BOOT_QUERY_MODE) (
  IN      EFI_SMART_BOOT_PROTOCOL       *This,
  IN      UINT32                        *Mode
);
/*++

Routine Description:

  Query the fastboot mode is enable or disable

Arguments:

  This      - Protocol instance structure

  Mode      - The fastboot mode enable or disable

Returns:

  EFI_SUCCESS - Get fastboot mode success

--*/


typedef
EFI_STATUS
(EFIAPI *EFI_SMART_BOOT_DISABLE_FAST_BOOT) (
  IN      EFI_SMART_BOOT_PROTOCOL         *This
);
/*++

Routine Description:

  Disable fastboot ==> clear all SOR Enable drivers which add this
  condition in DSC file.

Arguments:

  This - Image handle of this driver.

Returns:

  EFI_SUCCESS - Disable fastboot success

  EFI_ALREADY_STARTED - The fastboot mode is alreay disable

  Other       - LocateHandleBuffer fail

--*/

typedef struct _EFI_SMART_BOOT_PROTOCOL {
  EFI_SMART_BOOT_QUERY_MODE             QueryMode;
  EFI_SMART_BOOT_DISABLE_FAST_BOOT      DisableFastBoot;
} EFI_SMART_BOOT_PROTOCOL;

extern EFI_GUID gEfiSmartBootProtocolGuid;

#endif
