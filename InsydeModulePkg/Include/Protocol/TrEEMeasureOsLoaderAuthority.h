/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL_H_
#define _TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL_H_

#include <Guid/ImageAuthentication.h>

#define EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL_GUID  \
  {0xCA8BC404, 0x5655, 0x4262, { 0xA5, 0x9A, 0x5E, 0xEF, 0x0B, 0xB5, 0x97, 0x9C } }

typedef struct _EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_MEASURE_OS_LOADER_AUTHORITY)(
  IN      EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL   *This,
  IN      EFI_SIGNATURE_DATA                              *Data,
  IN      UINTN                                           Size
  );

//
// The EFI_TREE Protocol abstracts TCG activity.
//
struct _EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL {
  BOOLEAN                                                 OsLoader;
  EFI_TREE_MEASURE_OS_LOADER_AUTHORITY                    MeasureOsLoaderAuthority;
};

extern EFI_GUID gEfiTrEEMeasureOsLoaderAuthorityProtocolGuid;
#endif //_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL_H_