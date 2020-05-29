/** @file
  CPU Firmware Version Info implementation.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "CpuInitDxe.h"

#include <Library/RcFviDxeLib.h>
#include <Protocol/CpuInfo.h>


FVI_ELEMENT_AND_FUNCTION mCpuFviElementsData[] = {
  {
    DEFAULT_FVI_ELEMENT_DATA(CPU),
    NULL
  },
  {
    {
      1,
      0,
      UCODE_VERSION,
      UCODE_FVI_STRING,
      {
        0
      },
    },
    NULL
  },
  {
    {
      1,
      0,
      TXT_VERSION,
      TXT_FVI_STRING,
      {
        0
      },
    },
    NULL
  }
};

FVI_DATA_HUB_CALLBACK_CONTEXT mCpuFviVersionData = {
  MISC_SUBCLASS_FVI_HEADER_ENTRY(CPU),
  mCpuFviElementsData,
};

UINTN mCpuFviElements = sizeof (mCpuFviElementsData) / sizeof (FVI_ELEMENT_AND_FUNCTION);
