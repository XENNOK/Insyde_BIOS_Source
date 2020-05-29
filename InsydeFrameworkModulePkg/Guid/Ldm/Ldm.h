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
//; Module Name:
//;
//;   Ldm.h
//;
//; Abstract:
//;
//;   The definition of LDM partition on dynamic disk
//;

#ifndef _LDM_GUID_H_
#define _LDM_GUID_H_

#define EFI_PART_TYPE_LDM_METADATA_PART_GUID \
  { \
    0x5808c8aa, 0x7e8f, 0x42e0, 0x85, 0xd2, 0xe1, 0xe9, 0x04, 0x34, 0xcf, 0xb3 \
  }

#define EFI_PART_TYPE_LDM_DATA_PART_GUID \
  { \
    0xaf9b60a0, 0x1431, 0x4f62, 0xbc, 0x68, 0x33, 0x11, 0x71, 0x4a, 0x69, 0xad \
  }

extern EFI_GUID gEfiPartTypeLdmMetadataPartGuid;
extern EFI_GUID gEfiPartTypeLdmDataPartGuid;

#endif
