/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include <BdsPlatform.h>
#include <Protocol/FileSelectUI.h>
#include <Protocol/InternalFlashBios.h>
#include <Protocol/BiosFlashUI.h>

#define R_PCH_RST_CNT                 0xCF9
#define B_PCH_RST_CNT_FULL_RST        BIT3
#define B_PCH_RST_CNT_RST_CPU         BIT2
#define B_PCH_RST_CNT_SYS_RST         BIT1
#define V_PCH_RST_CNT_FULLRESET       0x0E
#define V_PCH_RST_CNT_HARDRESET       0x06
#define V_PCH_RST_CNT_SOFTRESET       0x04
#define V_PCH_RST_CNT_HARDSTARTSTATE  0x02
#define V_PCH_RST_CNT_SOFTSTARTSTATE  0x00


EFI_STATUS
BiosFlashUpdate (
  VOID
  )
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  INTERNAL_FLASH_BIOS_PROTOCOL       *InternalFlashBiosProtocol;
  BIOS_FLASH_UI_PROTOCOL             *BiosFlashUIProtocol;
  UINT8                              *BufferPtr = NULL;
  UINTN                              BufferSize;
  CHAR16                             *CmdLineParameter = NULL;
  EFI_INPUT_KEY                      Key;

  Status = gBS->LocateProtocol (&gBiosFlashUIProtocolGuid, NULL, (VOID **)&BiosFlashUIProtocol);
  if (!EFI_ERROR(Status)) {

    Status = BiosFlashUIProtocol->DrawBiosFlashUI ((VOID **)&BufferPtr,&BufferSize,&CmdLineParameter);

    if (!EFI_ERROR(Status)) {
      Status = gBS->LocateProtocol (&gInternalFlashBiosProtocolGuid, NULL, (VOID **)&InternalFlashBiosProtocol);
      if (!EFI_ERROR (Status)) {
        Status = InternalFlashBiosProtocol->FlashBios (BufferPtr,BufferSize,CmdLineParameter);
      }
    }

  }

  if (EFI_ERROR(Status)){
    CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_RED, &Key, L"Flash Update Fail!", L"Please Press Any Key to Reboot...", NULL);
  }
  else {
    CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, L"Flash Update Successful!", L"Please Press Any Key to Reboot...", NULL);
  }

  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTSTARTSTATE);
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTRESET);

  while(1);

  
  return Status;
}

