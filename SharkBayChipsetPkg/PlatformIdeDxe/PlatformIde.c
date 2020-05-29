/** @file

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

#include "PlatformIde.h"
#include "OemPlatformIde.h"

/**

  Initialize Platform Ide

  @param	ImageHandle 	        EFI_HANDLE
  @param	SystemTable 		      EFI_SYSTEM_TABLE pointer

  @retval EFI_SUCCESS           PlatformIde loaded.
                other           PlatformIde Error

**/
EFI_STATUS
EFIAPI
InitializePlatformIde (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                            Status;
  PLATFORM_IDE_PRIVATE_DATA             *PlatformIdeData;

  PlatformIdeData = AllocateZeroPool (sizeof(PLATFORM_IDE_PRIVATE_DATA));
  if (PlatformIdeData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Locate SetupUtility for get setup value.
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&PlatformIdeData->SetupUtility);
  if (EFI_ERROR (Status)) {
    PlatformIdeData->SetupUtility = NULL;
  }

  PlatformIdeData->Signature = PLATFORM_IDE_SIGNATURE;
  //
  // [Mark for Reduce Code]
  //
  PlatformIdeData->PlatformIde.CheckController  = NULL;
  PlatformIdeData->PlatformIde.GetChannelInfo   = NULL;
  PlatformIdeData->PlatformIde.CalculateMode    = NULL;

  //
  // Install PlatformIde
  //
  PlatformIdeData->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                 &PlatformIdeData->Handle,
                 &gEfiPlatformIdeProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &PlatformIdeData->PlatformIde
                 );

  return Status;
}

//
// [Mark for Reduce Code]
//
#if 0
/**
 Checks Vendor ID and Device ID from PCI config registers and determine if
 they are supported, ie. if it's ID is either PCH.

 @param [in]   This
 @param [in]   Controller       EFI Handle

 @retval EFI_SUCCESS            Controller is supported.
 @retval EFI_UNSUPPORTED        Controller is not supported.

**/
EFI_STATUS
PlatformIdeCheckController (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  EFI_HANDLE                        *Controller
  )
{
  EFI_STATUS                            Status;
  PLATFORM_IDE_PRIVATE_DATA             *PlatformIdeData;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINT16                                VendorID, DeviceID;

  BOOLEAN                               Flag;

  Flag = TRUE;
  Status = OemPlatformIdeCheckController (This, Controller, &Flag);

  if (!Flag) {

  PlatformIdeData = PLATFORM_IDE_FROM_THIS (This);

  Status = gBS->HandleProtocol (Controller, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = PciIo->Pci.Read (PciIo,
                            EfiPciIoWidthUint16,
                            0,
                            1,
                            &VendorID
                            );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = PciIo->Pci.Read (PciIo,
                            EfiPciIoWidthUint16,
                            2,
                            1,
                            &DeviceID
                            );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if ((VendorID != V_ICH_IDE_VENDOR_ID) || (DeviceID != V_ICH_IDE_DEVICE_ID)) {//[*SYMBOLE_FOR_VID_DID*] //uncompiler
    return EFI_UNSUPPORTED;
  }

  PlatformIdeData->PciIo = PciIo;

    Flag = FALSE;
    Status = OemPlatformIdeCheckController (This, Controller, &Flag);
  }
  return Status;

}

EFI_STATUS
PlatformIdeGetChannelInfo (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  UINT8                             Channel,
  OUT BOOLEAN                           *Enabled,
  OUT UINT8                             *MaxDevices
  )
{
  PLATFORM_IDE_PRIVATE_DATA             *PlatformIdeData;
  CHIPSET_CONFIGURATION                  *SetupVariable;

  if (Channel >= ICH_MAX_CHANNEL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformIdeData = PLATFORM_IDE_FROM_THIS (This);
  SetupVariable = (CHIPSET_CONFIGURATION *)PlatformIdeData->SetupUtility->SetupNvData;

  return EFI_SUCCESS;
}

EFI_STATUS
PlatformIdeCalculateMode (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE        *SupportedModes
  )
{
  PLATFORM_IDE_PRIVATE_DATA             *PlatformIdeData;
  CHIPSET_CONFIGURATION                  *SetupVariable;
  PATA_IDE_OPTION                       *PataOption;
  UINT32                                SelectMode;
  EFI_IDE_TRANSFER_MODE_UDMA            SetupUdmaMode;

  EFI_STATUS                            Status;
  BOOLEAN                               Flag;

  Flag = TRUE;
  Status = OemPlatformIdeCalculateMode (This,
                                        Channel,
                                        Device,
                                        SupportedModes,
                                        &Flag);

  if (!Flag) {

  if (Channel >= ICH_MAX_CHANNEL || Device >= ICH_MAX_DEVICE) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformIdeData = PLATFORM_IDE_FROM_THIS (This);
  SetupVariable = (CHIPSET_CONFIGURATION *)PlatformIdeData->SetupUtility->SetupNvData;

  DetectCableType (PlatformIdeData->PciIo, SetupVariable, Channel);

  PataOption = (PATA_IDE_OPTION *)(&SetupVariable->PMType);

  if (PataOption[Channel * ICH_MAX_DEVICE + Device].Type == SETUP_IDE_TYPE_AUTO) {
    return EFI_SUCCESS;
  }

  //
  // Check Pio mode.
  //
  if (SupportedModes->PioMode.Valid) {
    SelectMode = SupportedModes->PioMode.Mode;
    SelectMode = EFI_MIN (SelectMode, TransferModeFpio4);
    SupportedModes->PioMode.Mode = SelectMode;
  }

  //
  // Check Ultra DMA mode
  //
  if (SupportedModes->UdmaMode.Valid) {
    SelectMode = SupportedModes->UdmaMode.Mode;
    SetupUdmaMode = TransferModeUdma5;
    switch (PataOption[Channel * ICH_MAX_DEVICE + Device].TransferMode) {
      case SETUP_IDE_TRANSFER_AUTO:
        break;
      case SETUP_IDE_TRANSFER_FPIO:
        SupportedModes->UdmaMode.Valid = FALSE;
        break;
      case SETUP_IDE_TRANSFER_UDMA_33:
        SetupUdmaMode = TransferModeUdma2;
        break;
      case SETUP_IDE_TRANSFER_UDMA_66:
        SetupUdmaMode = TransferModeUdma4;
        break;
      case SETUP_IDE_TRANSFER_UDMA_100:
        SetupUdmaMode = TransferModeUdma5;
        break;
    }

    SelectMode = EFI_MIN (SelectMode, (UINTN)SetupUdmaMode);
    SupportedModes->UdmaMode.Mode = SelectMode;
  }

    Flag = FALSE;
    Status = OemPlatformIdeCalculateMode (This,
                                          Channel,
                                          Device,
                                          SupportedModes,
                                          &Flag);
  }
  return Status;

}

#endif // [Mark for Reduce Code]


