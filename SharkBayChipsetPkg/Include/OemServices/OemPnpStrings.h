/** @file
  Definition for Oem Pnp Dmi Update Strings.

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

#ifndef _PnP_DMI_UPDATE_STRINGS_H_
#define _PnP_DMI_UPDATE_STRINGS_H_

#include <Uefi.h>

#define DMI_UPDATA_STRING_SIGNATURE SIGNATURE_32 ('$', 'D', 'M', 'I')

#pragma pack (1)
typedef struct _DMI_STRING_STRUCTURE {
  UINT8                                 Type;
  UINT8                                 Offset;
  UINT8                                 Valid;
  UINT16                                Length;
  UINT8                                 String[1]; // Variable length field
} DMI_STRING_STRUCTURE;

typedef struct {
  UINT8                                 Type;           // The SMBIOS structure type
  UINT8                                 FixedOffset;    // The offset of the string reference
                                                        // within the structure's fixed data.
} DMI_UPDATABLE_STRING;

typedef struct {
  UINTN                                 UpdateStringNum;
  DMI_UPDATABLE_STRING                  *UpdateString;
} PNP_UPDATE_STRING;

#pragma pack ()
#endif
