/** @file

  The GUID definition for MeDataHob

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ME_DATA_HOB_H_
#define _ME_DATA_HOB_H_

#define ME_DATA_HOB_GUID \
  { 0x1E94F097, 0x5ACD, 0x4089, { 0xB2, 0xE3, 0xB9, 0xA5, 0xC8, 0x79, 0xA7, 0x0C }}

extern EFI_GUID gMeDataHobGuid;

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
  ///
  /// Guid specific data goes here
  ///
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  UINT64                FtpmBufferAddress;
} ME_DATA_HOB;

#endif
