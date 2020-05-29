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

EFI_STATUS
ConvertPointerMethodOne (
  VOID
  )
{
  if (mMethodOne->ConvertPointer) {
    return EFI_SUCCESS;
  }

  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodOne->CmdPort55));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodOne->CmdPort2A));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodOne->IdAddress));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID **) &mMethodOne);
  mMethodOne->ConvertPointer = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
ConvertPointerMethodTwo (
  VOID
  )
{
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID **) &mMethodTwo);
  mMethodTwo->ConvertPointer = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
ConvertPointerMethodThree (
  VOID
  )
{
  if (mMethodThree->ConvertPointer) {
    return EFI_SUCCESS;
  }

  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodThree->CmdPort55));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodThree->CmdPort2A));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mMethodThree->IdAddress));
  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID **) &mMethodThree);
  mMethodThree->ConvertPointer = TRUE;

  return EFI_SUCCESS;
}




