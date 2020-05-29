/** @file
  Common Header for Skip Scan Removable Device

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

#ifndef _SKIP_SCAN_REMOVABLE_DEV_H
#define _SKIP_SCAN_REMOVABLE_DEV_H

#define EFI_SKIP_SCAN_REMOVABLE_DEV_PROTOCOL_GUID \
  {0xed0d91de, 0xd828, 0x4ea2, 0x83, 0x85, 0xef, 0x73, 0x55, 0x5b, 0x3b, 0xb7}

typedef struct _EFI_SKIP_SCAN_REMOVABLE_DEV_PROTOCOL   EFI_SKIP_SCAN_REMOVABLE_DEV_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SKIP_SCAN_STATE) (
  OUT BOOLEAN                              *SkipSacnEnable
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_SET_SKIP_SCAN_STATE) (
  IN BOOLEAN                               SkipSacnEnable
  );

//
// Protocol definition
//
struct _EFI_SKIP_SCAN_REMOVABLE_DEV_PROTOCOL {
  EFI_GET_SKIP_SCAN_STATE              GetSkipScanState;
  EFI_SET_SKIP_SCAN_STATE              SetSkipScanState;
};

extern EFI_GUID gEfiSkipScanRemovableDevProtocolGuid;

#endif
