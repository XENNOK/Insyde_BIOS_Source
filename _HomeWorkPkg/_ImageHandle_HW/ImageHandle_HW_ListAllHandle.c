/** @file

  Image Handle
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"ImageHandle_HW.h"

EFI_STATUS
ListAllHandle (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16                                 *BaseFrametitle,
  IN CHAR16                                 **BaseFrameTail
  )
{
  EFI_STATUS         Status;
  UINTN              HandleCount = 0;
  EFI_HANDLE         *HandleBuffer = NULL;

  //
  // Locate Handle Buffer
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,   
                  NULL,
                  NULL,
                  &HandleCount, 
                  &HandleBuffer
                  );
  
  if (EFI_ERROR (Status)) {
      Print (L"###LocateHandleBuffer ERROR###\n");
      return Status;
  }

  Status = SelectHandle (
             InputEx,
             BaseFrametitle,
             BaseFrameTail,
             HandleCount,
             HandleBuffer,
             SearchAllHandles,
             NULL
             );
  if (EFI_ERROR (Status)) {
      Print (L"###SelectHandle ERROR###\n");
      return Status;
  }
  
  //
  // Free Handle Buffer
  //
  Status = gBS->FreePool (HandleBuffer);
  if (EFI_ERROR (Status)) {
      Print (L"###LocateHandleBuffer FreePool ERROR###\n");
      return Status;
  }

  return Status;
}