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
RecognizeMethodOne (
  IN UINT64                             DeviceId
  )
/*++

Routine Description:

Arguments:

Returns:

  EFI status

--*/
{
  UINT16                                CheckId;

  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0x90);
  MEMORY_FENCE ();

  CheckId = *((UINT16 *)mMethodOne->IdAddress);

  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, EFI_METHOD_ONE_SOFT_ID_EXIT);
  MEMORY_FENCE ();

  if (CheckId == (UINT16)DeviceId) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
RecognizeMethodTwo (
  IN UINT64                             DeviceId
  )
/*++

Routine Description:

Arguments:

Returns:

  EFI status

--*/
{
  UINT16                                CheckId;

  EfiSetMem ((VOID *)mMethodTwo->IdAddress, 1, EFI_METHOD_TWO_ID_COMMAND);
  CheckId = *((UINT16 *)mMethodTwo->IdAddress);
  EfiSetMem ((VOID *)mMethodTwo->IdAddress, 1, FWH_READ_ARRAY_COMMAND);

  if (CheckId == (UINT16)DeviceId) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
RecognizeMethodThree (
  IN UINT64                             DeviceId
  )
/*++

Routine Description:

Arguments:

Returns:

  EFI status

--*/
{
  UINT16                                CheckId;

  EfiSetMem ((VOID *)mMethodThree->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodThree->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodThree->CmdPort55, 1, 0x90);
  MEMORY_FENCE ();

  CheckId = *((UINT16 *)mMethodThree->IdAddress);

  EfiSetMem ((VOID *)mMethodThree->CmdPort55, 1, EFI_METHOD_ONE_SOFT_ID_EXIT);
  MEMORY_FENCE ();

  if (CheckId == (UINT16)DeviceId) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}
