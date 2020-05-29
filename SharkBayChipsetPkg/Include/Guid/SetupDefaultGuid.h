/** @file

  SETUP DEFAULT HOB.
  
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

#ifndef _SETUP_DEFAULT_GUID_H_
#define _SETUP_DEFAULT_GUID_H_

#include <ChipsetSetupConfig.h>

#define SETUP_DEFAULT_HOB_GUID \
  { \
    0xbdbd8c0c, 0xca7e, 0x4293, 0x81, 0x64, 0x28, 0xee, 0xaa, 0xa5, 0x27, 0xd5 \
  }

typedef struct {
  CHIPSET_CONFIGURATION  SetupNvData;
} DEFAULT_SETUP_VARIABLE_CONTENT;

#pragma pack (1)  
typedef struct {
  UINT8     Signature [4];
  EFI_GUID  VariableGuid;
  UINT32    VariableNameSize;
} SETUP_DEFAULT_HEADER;
#pragma pack ()  

extern EFI_GUID gSetupDefaultHobGuid;

#endif
