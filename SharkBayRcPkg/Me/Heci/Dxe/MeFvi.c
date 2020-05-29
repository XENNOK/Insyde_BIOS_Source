/** @file

  ME Firmware Version Info implementation.

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

#include "HeciDxe.h"

FVI_ELEMENT_AND_FUNCTION      mMeFviElementsData[] = {
  {
    DEFAULT_FVI_ELEMENT_DATA(ME),
    NULL
  },
  {
    DEFAULT_FVI_ELEMENT_DATA(MEBX),
    NULL
  },
  {
    {
      1,
      3,
      MEFW_VERSION,
      MEFW_FVI_STRING,
      MEFW_1_5M_STRING,
    },
    NULL
  }
};

FVI_DATA_HUB_CALLBACK_CONTEXT mMeFviVersionData = {
  MISC_SUBCLASS_FVI_HEADER_ENTRY(ME),
  mMeFviElementsData,
};

UINTN                         mMeFviElements = sizeof (mMeFviElementsData) / sizeof (FVI_ELEMENT_AND_FUNCTION);
