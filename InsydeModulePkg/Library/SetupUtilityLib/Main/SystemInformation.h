/** @file
  Header file for system information

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

#ifndef _SYSTEM_INFORMATION_H_
#define _SYSTEM_INFORMATION_H_

#include "SetupUtilityLibCommon.h"

#include <Protocol/DataHub.h>
#include <Guid/DataHubRecords.h>

typedef struct {
  UINT16                                SubInstance;
  STRING_REF                            TokenToUpdate1;
  STRING_REF                            TokenToUpdate2;
} UPDATE_INFO_STRUCT;

EFI_STATUS
UpdateSystemInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  );

#endif
