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
//;   BootType.h
//;
//; Abstract:
//;
//;   Header file for boot type
//;

#ifndef _BBS_TYPE_TABLE_H_
#define _BBS_TYPE_TABLE_H_

#include "SetupUtilityLibCommon.h"
#include "Pci22.h"

#define OTHER_TYPE_INDEX                    0x07

typedef struct {
  UINT8                                  DeviceType;
  STRING_REF                             StrToken;
  UINTN                                  DeviceTypeCount;
  UINT16                                 CurrentIndex;
  UINT16                                 KeyBootDeviceBase;
  EFI_FORM_LABEL                         BootDevicesLabel;
  UINTN                                  StringPtr;
} BBS_TYPE_TABLE;

extern BBS_TYPE_TABLE                    mBbsTypeTable[];
#endif
