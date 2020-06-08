/** @file
  SmmApp_18 C Source File

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

#include <Protocol/CpuIo2.h>

#include <SmiTable.h>

#define SMI_TRIGGER_PORT 0xb2

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_CPU_IO2_PROTOCOL  *CpuIo2;
  UINT8                 AddressBuffer;
  
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  AddressBuffer = EFI_SMM_TEST;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                SMI_TRIGGER_PORT,
                1,
                &AddressBuffer
                );   
  
  return Status;
}