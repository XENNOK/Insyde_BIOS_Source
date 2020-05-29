/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TREE_DATA_H_
#define _TREE_DATA_H_

#include <Protocol/TrEEProtocol.h>

typedef struct {
  TREE_BOOT_SERVICE_CAPABILITY            BsCap;
  EFI_PHYSICAL_ADDRESS                    LogAreaStartAddress;
  UINTN                                   EventLogSize;
  UINTN                                   MaxLogAreaSize;
  UINT8                                   *LastEvent;
  BOOLEAN                                 EventLogStarted;
  BOOLEAN                                 EventLogTruncated;
  EFI_TREE_PROTOCOL                       TrEEProtocol;
  TPM2B_AUTH                              Auth;
} TREE_DXE_DATA;

extern EFI_GUID gEfiTrEEServiceBindingProtocolGuid;

#endif

