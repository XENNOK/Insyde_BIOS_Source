/** @file
  Header file for Reference code Firmware Version Info Interface Lib implementation.

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

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  RcFviDxeLib.h

@brief
  Header file for Reference code Firmware Version Info Interface Lib implementation.

**/
#ifndef _RC_FVI_DXE_LIB_H_
#define _RC_FVI_DXE_LIB_H_

#include <IndustryStandard/SmBios.h>
#include <FrameworkDxe.h>
#include <Guid/DataHubRecords.h>

#pragma pack(1)

///
/// FviSmbios Type table -
/// {
///   FVI_HEADER;
///   FVI_ELEMENTS;
///...FVI_ELEMENTS;
///   .....
/// }
///
typedef struct {
  UINT8   MajorVersion;
  UINT8   MinorVersion;
  UINT8   Revision;
  UINT16  BuildNum;
} RC_VERSION;

///
/// If string is implemented for ComponentName or VersionString, and then string index of
/// ComponentName or VersionString can't be zero. The string index of ComponentName and
/// VersionString will be updated and calculated when collect all elements.
/// String index must contain zero if not implemented.
///
typedef struct {
  UINT8       ComponentName;
  UINT8       VersionString;
  RC_VERSION  Version;
  UINT8       NameString[SMBIOS_STRING_MAX_LENGTH];
  UINT8       VerString[SMBIOS_STRING_MAX_LENGTH];
} FVI_ELEMENTS;

#define FVI_ELEMENTS_SIZE_NOSTRING (sizeof(FVI_ELEMENTS) - SMBIOS_STRING_MAX_LENGTH * 2)
#define DEFAULT_FVI_ELEMENT_DATA(Name) \
{ \
  0x1, \
  0x00, \
  { \
    (UINT8)   (((Name ## _RC_VERSION) & 0xFF000000) >> 24), \
    (UINT8)   (((Name ## _RC_VERSION) & 0x00FF0000) >> 16), \
    (UINT8)   (((Name ## _RC_VERSION) & 0x0000FF00) >> 8), \
    (UINT16)  (((Name ## _RC_VERSION) & 0x000000FF)), \
  }, \
  Name ## _FVI_STRING, \
  0 \
}

///
/// This is the definitions for SMBIOS FviSmbios Type table
///
typedef struct {
  SMBIOS_STRUCTURE_HDR          Header;
  UINT8                         Count;  ///< Number of elements included
} FVI_HEADER;

///
/// This is definition for Misc sub class data hub
///
typedef struct {
  EFI_SUBCLASS_TYPE1_HEADER     Header;
  FVI_HEADER                    FviHdr;
} MISC_SUBCLASS_FVI_HEADER;

///
/// Use the OEM Data Record for SMBIOS Type 0x80-0xFF
///
#define MISC_SUBCLASS_TYPE1_HEADER_DATA(Name) \
{ \
  EFI_MISC_SUBCLASS_VERSION, \
  sizeof(EFI_SUBCLASS_TYPE1_HEADER), \
  Name ## _FVI_SMBIOS_INSTANCE, \
  0x1, \
  EFI_MISC_SMBIOS_STRUCT_ENCAP_RECORD_NUMBER \
}

#define DEFAULT_FVI_HEADER_DATA(Name) \
{ \
  { \
    Name##_FVI_SMBIOS_TYPE, \
    sizeof(FVI_HEADER), \
    0x00, \
  }, \
  0x1 \
}

///
/// Initialize per-record portion of subclass header and fvi header, also fill
/// static data into data portion of record
///
#define MISC_SUBCLASS_FVI_HEADER_ENTRY(Name) \
{\
  MISC_SUBCLASS_TYPE1_HEADER_DATA(Name), \
  DEFAULT_FVI_HEADER_DATA(Name) \
}

///
/// The function to update the element before log to Data Hub
///
typedef EFI_STATUS (EFIAPI FVI_ELEMENT_FUNCTION) (
  IN OUT FVI_ELEMENTS               *Element
  );

typedef struct {
  FVI_ELEMENTS                      Element;
  FVI_ELEMENT_FUNCTION              *Function;
} FVI_ELEMENT_AND_FUNCTION;

typedef struct {
  MISC_SUBCLASS_FVI_HEADER  FviHeader;
  FVI_ELEMENT_AND_FUNCTION  *Elements;  ///< Pointer to elements.
} FVI_DATA_HUB_CALLBACK_CONTEXT;

#pragma pack()

/**
  Initialize callback context for Firmware Version Info (FVI) Interface Spec v0.7
  implementation.

  Invoke this routine to initialize data hub and context for log,
  all elements can be updated before execute CreateRcFviDatahub or updated by
  the element hook that registered as FVI_ELEMENT_FUNCTION

  @param[in] Type                 Value is defined in SMBIOS Type 14 - Group Associaction structure - item type.
  @param[in] Count                Number of elements included by this SMBIOS table
  @param[in] FviContext           Context of FVI elements for data hub log

  @retval None
**/
VOID
InitFviDataHubCbContext (
  IN UINT8                         Type,
  IN UINT8                         Count,
  IN FVI_DATA_HUB_CALLBACK_CONTEXT *FviContext
  );

/**
  Create the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.7
  to Data Hub.

  Invoke this routine to log record when all Fvi elements are finialized

  @param[in] FviContext           Pointer to the notification functions context, which is context of FVI
                                  elements for data hub log

  @retval None
**/
VOID
CreateRcFviDatahub (
  IN FVI_DATA_HUB_CALLBACK_CONTEXT *FviContext
  )
;
#endif
