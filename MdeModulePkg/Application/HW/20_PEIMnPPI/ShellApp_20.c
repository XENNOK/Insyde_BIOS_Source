/** @file
  ShellApp_20 C Source File

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

#include <Pi/PiBootMode.h> // must before <Library/HobLib.h>
#include <Pi/PiHob.h>      // must before <Pi/PiHob.h>

#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/HobLib.h>

#define HOB_GUID                                                                  \
  {                                                                               \
    0x20000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_GUID      HobGuid = HOB_GUID;
  VOID          *HobList;
  UINT8         *HobPoint;
  UINT8         *DataPoint;
  UINT32        i;                  // for loop count
  UINT32        DataSize = 2;
  
  //
  // Get Hob list's PHIT address.
  //
  HobList = GetHobList();

  //
  // Search indicated Hob by GUID from Hob list.
  // Rerurn indicated Hob address.
  //
  HobPoint = GetNextGuidHob(&HobGuid, HobList); 
  if (HobPoint != NULL) {
    
    //
    // Get the data address from indicated Hob.
    //
    DataPoint = HobPoint + sizeof(EFI_HOB_GUID_TYPE);
    Print(L"Data:\n");
    for (i = 0; i < DataSize; i++) {
      Print(L"%3.2x", *(DataPoint + i));
    }
    Print(L"\n");
  }
  if (HobPoint == NULL) {
    Print(L"HoB not found\n");
  }

  Status = EFI_SUCCESS;

  return Status; 
}