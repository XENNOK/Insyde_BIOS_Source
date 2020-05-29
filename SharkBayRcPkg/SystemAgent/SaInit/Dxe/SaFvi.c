/** @file
  SA Firmware Version Info implementation.

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#include "SaInit.h"

FVI_ELEMENT_AND_FUNCTION      mSaFviElementsData[] = {
  {
    DEFAULT_FVI_ELEMENT_DATA(SA),
    NULL
  },
  {
    {
      1,
      0,
      MEM_RC_VERSION,
      MEM_FVI_STRING,
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
      PCIE_RC_VERSION,
      PCIE_FVI_STRING,
      {
        0
      },
    },
    NULL
  },
  {
    {
      1,
      2,
      SA_CRID_VERSION,
      SA_CRID_STATUS,
      SA_CRID_DISABLED,
    },
    NULL
  },
  {
    {
      1,
      0,
      SA_CRID_VERSION,
      SA_CRID_ORIGINAL_VALUE,
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
      SA_CRID_VERSION,
      SA_CRID_NEW_VALUE,
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
      VBIOS_RC_VERSION,
      VBIOS_FVI_STRING,
      {
        0
      },
    },
    NULL
  }
};

FVI_DATA_HUB_CALLBACK_CONTEXT mSaFviVersionData = {
  MISC_SUBCLASS_FVI_HEADER_ENTRY(SA),
  mSaFviElementsData,
};

UINTN                         mSaFviElements = sizeof (mSaFviElementsData) / sizeof (FVI_ELEMENT_AND_FUNCTION);
