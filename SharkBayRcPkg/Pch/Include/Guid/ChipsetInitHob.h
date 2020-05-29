/*++ @file
  Contains data used to determine if BIOS/ME/PMC are in sync
  with the required platform ChipsetInit settings.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved.
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
--*/

#ifndef _CHIPSET_INIT_INFO_HOBS_H__
#define _CHIPSET_INIT_INFO_HOBS_H__

#define CHIPSET_INIT_INFO_HOB_GUID \
  { 0xc1392859, 0x1f65, 0x446e, {0xb3, 0xf5, 0x84, 0x35, 0xfc, 0xc7, 0xd1, 0xc4} }

extern EFI_GUID gChipsetInitInfoHobGuid;

#pragma pack(push, 1)

#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
#ifndef __PI_HOB_H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct _CHIPSET_INIT_INFO_HOB {
  EFI_HOB_GUID_TYPE      Header;
  UINT32                 ChipsetInitTableLen;    // Size of the ChipsetInit table in bytes
  UINT8                  ChipsetInitTableUpdReq;
  UINT8                  ChipsetInitTable[384];
} CHIPSET_INIT_INFO_HOB;

#pragma pack(pop)
#endif

