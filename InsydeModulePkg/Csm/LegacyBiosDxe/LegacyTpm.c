/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyTpm.h"

/**
  Measure Legacy BIOS Image.

  @param  LegacyBiosImageAddress    Start address of the Legacy BIOS image
  @param  LegacyBiosImageSize       Size of the Legacy BIOS image

  @retval EFI_SUCCESS               Measure Legacy BIOS Image successfully.
**/
EFI_STATUS
TcgMeasureLegacyBiosImage (
  IN  EFI_PHYSICAL_ADDRESS      LegacyBiosImageAddress,
  IN  UINTN                     LegacyBiosImageSize
  )
{
  EFI_STATUS                        Status;
  EFI_TCG_PROTOCOL                  *TcgProtocol;
  EFI_TREE_PROTOCOL                 *TrEEProtocol;
  VOID                              *TcgEvent;
  TrEE_EVENT                        *TrEEEvent;
  EFI_PLATFORM_FIRMWARE_BLOB        FvBlob;
  EFI_PHYSICAL_ADDRESS              EventLogLastEntry;
  UINT32                            EventNumber;

  FvBlob.BlobBase = LegacyBiosImageAddress;
  FvBlob.BlobLength = (UINTN) LegacyBiosImageSize;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
  if (!EFI_ERROR (Status)) {  
    TcgEvent = AllocateZeroPool (sizeof (TCG_PCR_EVENT_HDR) + sizeof (FvBlob));
    if (TcgEvent == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    ((TCG_PCR_EVENT *)TcgEvent)->PCRIndex = 2;
    CopyMem (((TCG_PCR_EVENT *)TcgEvent)->Event, &FvBlob, sizeof (FvBlob));
    ((TCG_PCR_EVENT *)TcgEvent)->EventSize = sizeof (FvBlob);
    ((TCG_PCR_EVENT *)TcgEvent)->EventType = EV_EFI_PLATFORM_FIRMWARE_BLOB;
    EventNumber = 1;
    Status = TcgProtocol->HashLogExtendEvent (
                            TcgProtocol,
                            (UINTN)FvBlob.BlobBase,
                            (UINTN)FvBlob.BlobLength,
                            TPM_ALG_SHA,
                            (TCG_PCR_EVENT *)TcgEvent,
                            &EventNumber,
                            &EventLogLastEntry
                            );
    FreePool (TcgEvent);
  } else {
    Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID **)&TrEEProtocol);
    if (!EFI_ERROR (Status)) {
      TrEEEvent = AllocateZeroPool (sizeof (FvBlob) + sizeof (TrEE_EVENT) - sizeof(TrEEEvent->Event));
      if (TrEEEvent == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }      
      
      TrEEEvent->Size = sizeof (FvBlob) + sizeof (TrEE_EVENT) - sizeof(TrEEEvent->Event);   
      TrEEEvent->Header.HeaderSize    = sizeof (TrEE_EVENT_HEADER);
      TrEEEvent->Header.HeaderVersion = TREE_EVENT_HEADER_VERSION;      
      TrEEEvent->Header.PCRIndex  = 2;
      TrEEEvent->Header.EventType = EV_EFI_PLATFORM_FIRMWARE_BLOB;
      CopyMem (TrEEEvent->Event, &FvBlob, sizeof (FvBlob)); 
      
      Status = TrEEProtocol->HashLogExtendEvent (
                 TrEEProtocol,
                 0,
                 LegacyBiosImageAddress,
                 LegacyBiosImageSize,
                 TrEEEvent
                 );
      
      FreePool (TrEEEvent);
    }
  }
  
  return Status;
}


/**
  Load Tpm Binary Image.

  @param  Private                    Legacy BIOS context data
  @param  Table                      EFI Campatibility16 Table

  @retval EFI_SUCCESS                Load Tpm Binary Image successfully.
**/
EFI_STATUS
LoadTpmBinaryImage (
  IN LEGACY_BIOS_INSTANCE          *Private,
  IN EFI_COMPATIBILITY16_TABLE     *Table
  )
{
  EFI_STATUS                        Status;
  EFI_TCG_PROTOCOL                  *TcgProtocol;
  UINT32                            TpmPointer;
  VOID                              *TpmBinaryImage;
  UINTN                             TpmBinaryImageSize;
  EFI_IA32_REGISTER_SET             Regs;
  UINTN                             Location;
  UINTN                             Alignment;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
  if (EFI_ERROR(Status)){
    return Status;
  }

  Location  = 0;
  Alignment = 0;
  Status = Private->LegacyBiosPlatform->GetPlatformInfo (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformBinaryTpmBinary,
                                          &TpmBinaryImage,
                                          &TpmBinaryImageSize,
                                          &Location,
                                          &Alignment,
                                          0,
                                          0
                                          );
  if (EFI_ERROR(Status)){
    return Status;
  }

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.CX = (UINT16) TpmBinaryImageSize;
  Regs.X.DX = 1;
  LegacyBiosFarCall86 (
    &Private->LegacyBios,
    Table->Compatibility16CallSegment,
    Table->Compatibility16CallOffset,
    &Regs,
    NULL,
    0
    );

  TpmPointer = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);
  if (Regs.X.AX != 0) {
    DEBUG ((EFI_D_ERROR, "TPM cannot be loaded\n"));
  } else {
    CopyMem ((VOID *) (UINTN)TpmPointer, TpmBinaryImage, TpmBinaryImageSize);
    Table->TpmSegment = Regs.X.DS;
    Table->TpmOffset  = Regs.X.BX;
  }

  return EFI_SUCCESS;
}
