//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FWH_LPC.h"

METHOD_ONE_ADDRESS                      mMethodOneInstance;
METHOD_TWO_ADDRESS                      mMethodTwoInstance;
METHOD_THREE_ADDRESS                    mMethodThreeInstance;

EFI_STATUS
InitMethodOne (
  EFI_SMM_SYSTEM_TABLE                    *mSmst
  )
{
  EFI_STATUS                            Status;

  if ((mMethodOne != NULL) && mMethodOne->Inited) {
    return EFI_SUCCESS;
  }

  if (mSmst == NULL) {
    mMethodOne = &mMethodOneInstance;
  } else {
   Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (METHOD_ONE_ADDRESS), &mMethodOne);
   EfiZeroMem (mMethodOne, sizeof (METHOD_ONE_ADDRESS));
  }

  mMethodOne->Inited = TRUE;
  mMethodOne->CmdPort55 = EFI_METHOD_ONE_MAGIC_55;
  mMethodOne->CmdPort2A = EFI_METHOD_ONE_MAGIC_2A;
  mMethodOne->IdAddress = EFI_METHOD_ONE_ID_ADDRESS;

  return EFI_SUCCESS;
}


EFI_STATUS
InitMethodTwo (
  EFI_SMM_SYSTEM_TABLE                    *mSmst
  )
{
  EFI_STATUS                            Status;

  if ((mMethodTwo != NULL) && mMethodTwo->Inited) {
     return EFI_SUCCESS;
  }

  if (mSmst == NULL) {
    mMethodTwo = &mMethodTwoInstance;
  } else {
   Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (METHOD_TWO_ADDRESS), &mMethodTwo);
   EfiZeroMem (mMethodTwo, sizeof (METHOD_TWO_ADDRESS));
  }

  mMethodTwo->Inited = TRUE;
  mMethodTwo->IdAddress = EFI_METHOD_TWO_ID_ADDRESS;

  return EFI_SUCCESS;
}

EFI_STATUS
InitMethodThree (
  EFI_SMM_SYSTEM_TABLE                    *mSmst
  )
{
  EFI_STATUS                            Status;

  if ((mMethodThree != NULL) && mMethodThree->Inited) {
    return EFI_SUCCESS;
  }
  
  if (mSmst == NULL) {
    mMethodThree = &mMethodThreeInstance;
  } else {
   Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (METHOD_THREE_ADDRESS), &mMethodThree);
   EfiZeroMem (mMethodThree, sizeof (METHOD_THREE_ADDRESS));
  }

  mMethodThree->Inited = TRUE;
  mMethodThree->CmdPort55 = EFI_METHOD_ONE_MAGIC_55;
  mMethodThree->CmdPort2A = EFI_METHOD_ONE_MAGIC_2A;
  mMethodThree->IdAddress = EFI_METHOD_THREE_ID_ADDRESS;

  return EFI_SUCCESS;
}
