/** @file
  SpdRead_24 C Source File

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

#include "SpdRead_24.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_HC_PROTOCOL       *Smbus;
  EFI_SMBUS_DEVICE_ADDRESS    SmbusDeviceAddress;
  UINT8                       Buffer;
  UINTN                       Length;
  UINTN                       Offset;
  UINTN                       Address;

  Buffer = 0;
  Length = 1;

  Status = gBS->LocateProtocol(
                            &gEfiSmbusHcProtocolGuid, 
                            NULL, 
                            (VOID**)&Smbus
                            );
  if (Status != EFI_SUCCESS) {
    Print (L"%r\n", Status);
    return Status;
  }
  
  //
  // Search SPD (0xa0~0xa6)
  //
  for (Address = 0xa0; Address <= 0xa6; Address += 2) {
    Print(L"Address: 0x%x", Address);
    for (Offset = 0; Offset < 256; Offset++) {
      //
      // Address must be shift right 1 bit.
      //
      SmbusDeviceAddress.SmbusDeviceAddress = Address >> 1;
      Status = Smbus->Execute(
                           Smbus,
                           SmbusDeviceAddress,
                           Offset,
                           EfiSmbusReadByte,
                           FALSE,
                           &Length,
                           &Buffer
                           );
      
      if (EFI_ERROR(Status)) {
        Print(L" ---> %r\n", Status);
        goto NEXT;

      } else {
        if (Offset == 0) {
          Print(L"\n");
        }
        Print(L"%3.2x", Buffer);
        if (Offset % 16 == 15) {
          Print(L"\n");
        }
      }
    }
    NEXT:;
  }
  
  Status = EFI_SUCCESS;
  
  return Status;
}