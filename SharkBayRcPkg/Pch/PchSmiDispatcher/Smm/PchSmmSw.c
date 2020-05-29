/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmmSw.c

@brief
  File to contain all the hardware specific stuff for the Smm Sw dispatch protocol.

**/
#include "PchSmmHelpers.h"

const PCH_SMM_SOURCE_DESC SW_SOURCE_DESC = {
  PCH_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        R_PCH_SMI_EN
      },
      S_PCH_SMI_EN,
      N_PCH_SMI_EN_APMC
    },
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        ACPI_ADDR_TYPE,
        R_PCH_SMI_STS
      },
      S_PCH_SMI_STS,
      N_PCH_SMI_STS_APM
    }
  }
};

/**
  Get the Software Smi value

  @param[in] Record               No use
  @param[out] Context             The context that includes Software Smi value to be filled

  @retval None
**/
VOID
EFIAPI
SwGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  )
{
  UINT8 ApmCnt;

  ApmCnt                      = IoRead8 ((UINTN) R_PCH_APM_CNT);

  Context->Sw.SwSmiInputValue = ApmCnt;
}

/**
  Check whether software SMI value of two contexts match

  @param[in] Context1             Context 1 that includes software SMI value 1
  @param[in] Context2             Context 2 that includes software SMI value 2

  @retval FALSE                   Software SMI value match
  @retval TRUE                    Software SMI value don't match
**/
BOOLEAN
EFIAPI
SwCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  )
{
  return (BOOLEAN) (Context1->Sw.SwSmiInputValue == Context2->Sw.SwSmiInputValue);
}
