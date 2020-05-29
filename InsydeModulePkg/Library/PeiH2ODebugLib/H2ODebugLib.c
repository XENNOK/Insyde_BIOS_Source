/** @file
  H2O Debug Library

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Ppi/DebugComm.h>
#include <Library/PeiServicesTablePointerLib.h>

VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  )
{
  CONST EFI_PEI_SERVICES        **PeiServices;
  EFI_STATUS                    Status;
  H2O_DEBUG_COMM_PPI            *DebugComm;
  VA_LIST                       Args;

  PeiServices = GetPeiServicesTablePointer ();
  Status = (**PeiServices).LocatePpi (PeiServices, &gH2ODebugCommPpiGuid, 0, NULL, (VOID **)&DebugComm);
  if (Status == EFI_SUCCESS) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}
