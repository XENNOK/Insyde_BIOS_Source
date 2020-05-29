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
#include <Library/HobLib.h>
#include <Library/PeiServicesTablePointerLib.h>

static EFI_GUID                mPeiDebugEnableGuid = { 0xe3055c82, 0x540b, 0x4c4f, 0x8f, 0x6a, 0x5d, 0xc4, 0xfe, 0x9b, 0xcd, 0xc4 };

VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  )
{
  CONST EFI_PEI_SERVICES        **PeiServices;
  EFI_STATUS                    Status;
  H2O_DEBUG_COMM_PPI            *DebugComm = NULL;
  VA_LIST                       Args;
  EFI_HOB_GUID_TYPE             *GuidHob;
  VOID                          *Hob;

  PeiServices = GetPeiServicesTablePointer ();
  Hob = GetFirstHob (EFI_HOB_TYPE_GUID_EXTENSION);
  if (Hob == NULL) return;
  GuidHob = GetNextGuidHob (&mPeiDebugEnableGuid, Hob);
  if (GuidHob == NULL) return;
  Status = (**PeiServices).LocatePpi (PeiServices, &gH2ODebugCommPpiGuid, 0, NULL, (VOID **)&DebugComm);
  if (DebugComm != NULL) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}
