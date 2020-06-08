/** @file
  PciUtilityNDMDapp_16 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include "PciUtilityNDMD_16.h"

// VOID
// CallFunction (
//   VOID
//   );

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS      Status;
  EFI_PCI_UTILITY_PROTOCOL  *PciUtility;

  Status = gBS->LocateProtocol (&gEfiPciUtilityProtocolGuid, NULL, &PciUtility);
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);

  } else {
    Print(L"Apple = %d\n", PciUtility->Apple);
    // PciUtility->MainPage(PciUtility);
  }

  Status = EFI_SUCCESS;

  return Status; 
}