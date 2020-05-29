/** @file

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef DXE_GFX_PROTOCOL_H_
#define DXE_GFX_PROTOCOL_H_

#define DXE_GFX_PROTOCOL_GUID \
  { \
    0xc4d6994a, 0xaa3d, 0x47c0, 0xae, 0xa, 0xb0, 0xbf, 0xad, 0xba, 0xa6, 0x13 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gDxeGfxProtocolGuid;

#define BIOS_RESTORE    0 // 12 Mbytes space
#define DESKTOP_RESTORE 1 // 512 Kb Space

typedef
EFI_STATUS
(EFIAPI * GFX_ALLOCATE_DSS_BUFFER) (
  IN UINT8                            DisplayType
);

typedef
EFI_STATUS
(EFIAPI * GFX_GET_DSS_ADDRESS) (
  OUT UINT64                          **DssAddress,
  OUT UINT64                          *SizeOfDss
);

typedef
EFI_STATUS
(EFIAPI * GFX_SAVE_DISPLAY) (
  IN UINT8                            *RestoreImage
);

#pragma pack(1)

typedef struct _DXE_GFX_PROTOCOL {
  GFX_ALLOCATE_DSS_BUFFER   AllocateDssBuffer;
  GFX_GET_DSS_ADDRESS       GetDssAddress;
  GFX_SAVE_DISPLAY          SaveDisplay;
} DXE_GFX_PROTOCOL;

#pragma pack()

#endif
