//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_AZALIA_CODEC_VERB_H_
#define _OEM_AZALIA_CODEC_VERB_H_

#include "Tiano.h"

#pragma pack (1)

typedef struct {
  UINT32        VendorDeviceId;
  UINT16        SubSystemId;
  UINT8         FrontPanelSupport;
  UINT16        NumberOfRearJacks;
  UINT16        NumberOfFrontJacks;
} EFI_AZALIA_CODEC_VERB_TABLE;

typedef struct {
  UINT32  VendorDeviceId;
  UINT16  SubSystemId;
  UINT8   RevisionId;
  UINT8   FrontPanelSupport;
  UINT16  NumberOfRearJacks;
  UINT16  NumberOfFrontJacks;
} AZALIA_VERB_TABLE_HEADER;

#pragma pack ()

#endif
