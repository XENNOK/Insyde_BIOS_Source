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
//;   SystemInformation.h
//;
//; Abstract:
//;
//;   Header file for system information
//;

#ifndef _SYSTEM_INFORMATION_H_
#define _SYSTEM_INFORMATION_H_

#include "SetupUtilityLibCommon.h"

#include EFI_PROTOCOL_DEFINITION (DataHub)
#include EFI_GUID_DEFINITION (DataHubRecords)

typedef struct {
  UINT16                                SubInstance;
  STRING_REF                            TokenToUpdate1;
  STRING_REF                            TokenToUpdate2;
} UPDATE_INFO_STRUCT;

EFI_STATUS
UpdateSystemInfo (
  IN  EFI_HII_PROTOCOL                    *Hii,
  IN  EFI_HII_HANDLE                      HiiHandle
  );

#endif
