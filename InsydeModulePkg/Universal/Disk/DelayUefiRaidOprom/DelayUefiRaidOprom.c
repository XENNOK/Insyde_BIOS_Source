/** @file
  Delay UEFI RAID OpROM Driver

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

#include "DelayUefiRaidOprom.h"

EFI_HANDLE                                gDelayUefiRaidOpromDriverImageHandle;
EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL     *gDelayUefiRaidOpromPlatformDriverOverride;

EFI_EVENT   mDelayUefiRaidOpromEvent;
EFI_EVENT   mDelayUefiRaidOpromScsiEvent;
EFI_EVENT   mDelayUefiRaidOpromScsiIoEvent;

DELAY_UEFI_RAID_OPROM_PRIVATE *mDelayUefiRaidOpromPrivate;

EFI_DRIVER_BINDING_PROTOCOL gDelayUefiRaidOpromDriverBinding = {
  DelayUefiRaidOpromDriverBindingSupported,
  DelayUefiRaidOpromDriverBindingStart,
  DelayUefiRaidOpromDriverBindingStop,
  0x10,
  NULL,
  NULL
};

/**
 Check any condition to make AhciBus support when UEFI RAID OpROM dispathed.
 Current condition:
   1. HDD Password Support
   2. PUIS Support

  @retval TRUE                  Need AhciBus support
  @retval FALSE                 AhciBus not support

**/
BOOLEAN
EFIAPI
AhciBusSupportCheck (
  EFI_PCI_IO_PROTOCOL                       *PciIo
  )
{
  EFI_STATUS                            Status;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  KERNEL_CONFIGURATION                  KernelConfiguration;
  RAID_CONTROLLER_PRIVATE_DATA          *RaidControllerPrivateData;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Device;
  UINTN                                 Function;

  Status = PciIo->GetLocation (
                    PciIo,
                    &Seg,
                    &Bus,
                    &Device,
                    &Function
                    );

  RaidControllerPrivateData = (RAID_CONTROLLER_PRIVATE_DATA *)GetFirstNode (&mDelayUefiRaidOpromPrivate->RaidControllerInfoListHead);
  do {

    if (RaidControllerPrivateData->PciBus == Bus &&
        RaidControllerPrivateData->PciDevice == Device &&
        RaidControllerPrivateData->PciFunction == Function &&
        RaidControllerPrivateData->Checked != TRUE) {
      RaidControllerPrivateData->Checked = TRUE;

      mDelayUefiRaidOpromPrivate->RaidControllerRemain -= 1;

      if (mDelayUefiRaidOpromPrivate->RaidControllerRemain == 0) {
        mDelayUefiRaidOpromPrivate->RaidControllerAllConnected = TRUE;
      }
    }
    RaidControllerPrivateData = (RAID_CONTROLLER_PRIVATE_DATA *)GetNextNode (
                                                                  &mDelayUefiRaidOpromPrivate->RaidControllerInfoListHead, 
                                                                  &RaidControllerPrivateData->Link
                                                                  );
  } while (IsNodeAtEnd (&mDelayUefiRaidOpromPrivate->RaidControllerInfoListHead, &RaidControllerPrivateData->Link));

  //
  //  Condition 1: Check HddPassword
  //
  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&HddPasswordService
                  );

  if (Status == EFI_SUCCESS) {
    mDelayUefiRaidOpromPrivate->AhciBusSupport = TRUE;
    return TRUE;
  }

  //
  //  Condition 2: Check PUIS
  //

  Status = GetKernelConfiguration (&KernelConfiguration);

  if (Status == EFI_SUCCESS) {
    if (KernelConfiguration.PUISEnable == TRUE) {
      mDelayUefiRaidOpromPrivate->AhciBusSupport = TRUE;
      return TRUE;
    }
  }

  //
  //  No AhciBus support
  //
  return FALSE;
}

/**
  Retrieves the image handle of the platform override driver for a controller in the system.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_
                                PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller to check if a driver override
                                exists.
  @param  DriverImageHandle     On input, a pointer to the previous driver image handle returned
                                by GetDriver(). On output, a pointer to the next driver
                                image handle.

  @retval EFI_SUCCESS           The driver override for ControllerHandle was returned in
                                DriverImageHandle.
  @retval EFI_NOT_FOUND         A driver override for ControllerHandle was not found.
  @retval EFI_INVALID_PARAMETER The handle specified by ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImageHandle is not a handle that was returned on a
                                previous call to GetDriver().

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromPlatformDriverOverrideGetDriver (
  IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL              *This,
  IN     EFI_HANDLE                                     ControllerHandle,
  IN OUT EFI_HANDLE                                     *DriverImageHandle
  )
{
  EFI_STATUS                                            Status;
  EFI_PCI_IO_PROTOCOL                                   *PciIo;
  PCI_TYPE00                                            PciData;

  if (DriverImageHandle == NULL || ControllerHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*DriverImageHandle != NULL) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData),
                        &PciData
                        );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (PciData.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE &&
      PciData.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID) {

    if (mDelayUefiRaidOpromPrivate->RaidControllerInfoGet != TRUE) {
      ScanAllSataController ();
    }

    *DriverImageHandle = gDelayUefiRaidOpromDriverImageHandle;
    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}

/**
  Retrieves the device path of the platform override driver for a controller in the system.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller to check if a driver override
                                exists.
  @param  DriverImagePath       On input, a pointer to the previous driver device path returned by
                                GetDriverPath(). On output, a pointer to the next driver
                                device path. Passing in a pointer to NULL will return the first
                                driver device path for ControllerHandle.

  @retval EFI_SUCCESS           The driver override for ControllerHandle was returned in
                                DriverImageHandle.
  @retval EFI_UNSUPPORTED       The operation is not supported.
  @retval EFI_NOT_FOUND         A driver override for ControllerHandle was not found.
  @retval EFI_INVALID_PARAMETER The handle specified by ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImagePath is not a device path that was returned on a
                                previous call to GetDriverPath().

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromPlatformDriverOverrideGetDriverPath (
  IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL              *This,
  IN EFI_HANDLE                                         ControllerHandle,
  IN OUT EFI_DEVICE_PATH_PROTOCOL                       **DriverImagePath
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Used to associate a driver image handle with a device path that was returned on a prior call to the
  GetDriverPath() service. This driver image handle will then be available through the
  GetDriver() service.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_
                                PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller.
  @param  DriverImagePath       A pointer to the driver device path that was returned in a prior
                                call to GetDriverPath().
  @param  DriverImageHandle     The driver image handle that was returned by LoadImage()
                                when the driver specified by DriverImagePath was loaded
                                into memory.

  @retval EFI_SUCCESS           The association between DriverImagePath and
                                DriverImageHandle was established for the controller specified
                                by ControllerHandle.
  @retval EFI_UNSUPPORTED       The operation is not supported.
  @retval EFI_NOT_FOUND         DriverImagePath is not a device path that was returned on a prior
                                call to GetDriverPath() for the controller specified by
                                ControllerHandle.
  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImagePath is not a valid device path.
  @retval EFI_INVALID_PARAMETER DriverImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromPlatformDriverOverrideGetDriverLoaded (
  IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL          *This,
  IN EFI_HANDLE                                     ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL                       *DriverImagePath,
  IN EFI_HANDLE                                     DriverImageHandle
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get device data by identify command with AtaPassThru protocol

  @param  AtaPassThruProtocol   AtaPassThru protocol on the specific ATA controller
  @param  Port                  The port for device
  @param  PortMultiplierPort    The PortMultiplierPort for device
  @param  IdentifyData          The pointer for identify data buffer
  @param  IdentifyDataSize      The size for identify data buffer

  @retval EFI_SUCCESS           IdentifyData valid
  @retval EFI_INVALID_PARAMETER Parameter invalid
  @retval others                return by AtaPassThru Protocol

**/
EFI_STATUS
EFIAPI
IdentifyByAtaPassThru (
  IN EFI_ATA_PASS_THRU_PROTOCOL         *AtaPassThruProtocol,
  IN UINT16                             Port,
  IN UINT16                             PortMultiplierPort,
  IN OUT VOID                           *IdentifyData,
  IN OUT UINTN                          *IdentifyDataSize
  )
{
  EFI_STATUS                            Status;
  EFI_ATA_PASS_THRU_COMMAND_PACKET      AtaPassThruCmdPacket;
  EFI_ATA_COMMAND_BLOCK                 Acb;
  EFI_ATA_STATUS_BLOCK                  Asb;

  if (AtaPassThruProtocol == NULL || IdentifyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*IdentifyDataSize < IDENTIFY_TABLE_SIZE) {
    *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Initial Command packet
  //
  ZeroMem (&AtaPassThruCmdPacket, sizeof (EFI_ATA_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Acb, sizeof (EFI_ATA_COMMAND_BLOCK));
  ZeroMem (&Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  AtaPassThruCmdPacket.Acb = &Acb;
  AtaPassThruCmdPacket.Asb = &Asb;

  //
  // Fill in command packet
  //
  AtaPassThruCmdPacket.Acb->AtaCommand      = ATA_IDENTIFY_CMD;
  AtaPassThruCmdPacket.Acb->AtaDeviceHead   |= ((PortMultiplierPort<< 4) | 0xe0);
  AtaPassThruCmdPacket.InDataBuffer         = (VOID *)IdentifyData;
  AtaPassThruCmdPacket.InTransferLength     = IDENTIFY_TABLE_SIZE;
  AtaPassThruCmdPacket.Protocol             = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  AtaPassThruCmdPacket.Length               = EFI_ATA_PASS_THRU_LENGTH_BYTES;

  //
  // Processing AtaPassThru command
  //
  Status = AtaPassThruProtocol->PassThru (
                                  AtaPassThruProtocol,
                                  Port,
                                  PortMultiplierPort,
                                  &AtaPassThruCmdPacket,
                                  0
                                  );
  if(Status == EFI_SUCCESS) {
    *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
    return EFI_SUCCESS;
  }

  return Status;
}

/**
  Install DiskInfo protocol interface on disk handle by AtaPassThru protocol

  @param  DiskHandle            The handle of Disk
  @param  PciDevicePath         The DevicePath of Disk
  @param  CurrentSataDevicePath The CurrentSataDevicePath of Disk

  @retval EFI_SUCCESS           Install DiskInfo successfully
  @retval others                Install failed

**/
EFI_STATUS
EFIAPI
InstallDiskInfoProtocolByAta (
  EFI_HANDLE                            DiskHandle,
  PCI_DEVICE_PATH                       *PciDevicePath,
  SATA_DEVICE_PATH                      *CurrentSataDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_ATA_PASS_THRU_PROTOCOL            *AtaPassThruPtr;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *AtaPassThruDevPath;
  BOOLEAN                               IsMatched;
  UINT16                                PortMap;
  UINT16                                Port;
  UINT16                                PortMultiplierPort;
  UINTN                                 IdentifyTableSize;
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiAtaPassThruProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return Status;
  }

  AtaPassThruPtr     = NULL;
  AtaPassThruDevPath = NULL;

  TmpDevicePath = NULL;

  PortMap = 0;
  Port    = 0;
  PortMultiplierPort = 0;

  DevicePrivateData = NULL;

  //
  // Find out the handle installed AtaPassthru protocol, and match the SATA controller
  //
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->OpenProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &AtaPassThruDevPath,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    IsMatched = FALSE;
    TmpDevicePath = AtaPassThruDevPath;
    while (!IsDevicePathEnd (TmpDevicePath)) {
      if((TmpDevicePath->Type == HARDWARE_DEVICE_PATH) && (TmpDevicePath->SubType == HW_PCI_DP)) {
        if((((PCI_DEVICE_PATH*)TmpDevicePath)->Device == PciDevicePath->Device) &&
            (((PCI_DEVICE_PATH*)TmpDevicePath)->Function) == PciDevicePath->Function) {
            //
            // Controller matched
            //
            IsMatched = TRUE;
            break;
        }
      }
      TmpDevicePath = NextDevicePathNode (TmpDevicePath);
    }
    if(!IsMatched) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiAtaPassThruProtocolGuid,
                    (VOID **)&AtaPassThruPtr
                    );
    if(Status == EFI_SUCCESS) {
      //
      // Prepare the DiskInfo and install DiskInfo protocol to the Handle
      //
      PortMap = CurrentSataDevicePath->HBAPortNumber;
      for (Port = 0; PortMap != 0; Port++, PortMap >>= 1);
      Port--;
      PortMultiplierPort = 0;

      DevicePrivateData = AllocateZeroPool (sizeof (DEVICE_PRIVATE_DATA));
      if (DevicePrivateData == NULL) {
        gBS->FreePool (HandleBuffer);
        return EFI_OUT_OF_RESOURCES;
      }

      DevicePrivateData->Signature             = DEVICE_PRIVATE_DATA_SIGNATURE;
      DevicePrivateData->ControllerHandle      = HandleBuffer[HandleIndex];
      DevicePrivateData->DeviceHandle          = DiskHandle;
      DevicePrivateData->ControllerDevicePath  = AtaPassThruDevPath;
      DevicePrivateData->AtaPassThruPtr        = AtaPassThruPtr;
      DevicePrivateData->Port                  = Port;
      DevicePrivateData->PortMultiplierPort    = PortMultiplierPort;
      CopyMem (&DevicePrivateData->DiskInfo.Interface, &gEfiDiskInfoAhciInterfaceGuid, sizeof (EFI_GUID));

      //
      //  Check the device is HDD or ODD.
      //  If the device is ODD, if may be supported by AtaPassThru protocol provided by UEFI RAID OpROM.
      //
      IdentifyTableSize = IDENTIFY_TABLE_SIZE;

      Status = IdentifyByAtaPassThru (
                 AtaPassThruPtr,
                 Port,
                 PortMultiplierPort,
                 &(DevicePrivateData->DeviceIdentifyDataRaw),
                 &IdentifyTableSize
                 );
      if (Status == EFI_SUCCESS) {
        //
        //  The device can get identify data from AtaPassThru.
        //
        DevicePrivateData->DiskInfo.Identify  = AtaDiskInfoIdentify;
        DevicePrivateData->DiskInfo.Inquiry   = AtaDiskInfoInquiry;
        DevicePrivateData->DiskInfo.SenseData = AtaDiskInfoSenseData;
        DevicePrivateData->DiskInfo.WhichIde  = AtaDiskInfoWhichIde;

        Status = gBS->InstallProtocolInterface (
                        &DiskHandle,
                        &gEfiDiskInfoProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &(DevicePrivateData->DiskInfo)
                        );
      } else {
        //
        //  The ODD device can't get identify data from AtaPassThru and may get from ExtScsiPassThru.
        //

      }


      InsertTailList (
        &(mDelayUefiRaidOpromPrivate->DeviceInfoListHead),
        &DevicePrivateData->Link
        );


      break;
    }
  }

  gBS->FreePool (HandleBuffer);

  return Status;

}

/**

  Get device data by inquiry command with ExtScsiPassThru protocol

  @param  DevicePrivateData     The device data

  @retval EFI_SUCCESS           Get device data by ExtScsiPassThru protocol successfully.
  @retval others                Can not get device data and return error status by ExtScsiPassThru protocol.

**/
EFI_STATUS
EFIAPI
InquiryByExtScsiPassThru (
  DEVICE_PRIVATE_DATA                   *DevicePrivateData
  )
{
  EFI_STATUS                                 Status;
  UINT64                                     Lun;
  UINT8                                      *TargetId;
  SCSI_TARGET_ID                             ScsiTargetId;
  EFI_EXT_SCSI_PASS_THRU_SCSI_REQUEST_PACKET Packet;
  EFI_SCSI_CDB_6                             Cdb6;
  IDENTIFY_TAB                               *IdentifyData;
  EFI_SCSI_SENSE_DATA_EXT                    SenseDataExt;
  EFI_SCSI_ATA_VPD_PAGE                      AtaVpdPage;
  EFI_EXT_SCSI_PASS_THRU_PROTOCOL            *ExtScsiPassThruProtocol;
  CHAR8                                      *TempString;
  UINT16                                     Index;
  CHAR8                                      Temp8;

  TempString = NULL;

  ExtScsiPassThruProtocol = DevicePrivateData->ExtScsiPassThruPtr;

  TargetId = &ScsiTargetId.ScsiId.ExtScsi[0];
  SetMem (TargetId, TARGET_MAX_BYTES, 0xFF);
  *TargetId = (UINT8)(DevicePrivateData->Port);
  Lun = 0;

  //
  //  EFI_SCSI_OP_INQUIRY
  //
  ZeroMem (&Packet, sizeof (Packet));
  ZeroMem (&Cdb6, sizeof (Cdb6));
  ZeroMem (&SenseDataExt, sizeof (SenseDataExt));
  ZeroMem (&AtaVpdPage, sizeof (AtaVpdPage));

  Cdb6.OpCode = EFI_SCSI_OP_INQUIRY;
  Cdb6.AllocationLengthMsb = (UINT8) (sizeof (AtaVpdPage) >> 8);
  Cdb6.AllocationLengthLsb = (UINT8) (sizeof (AtaVpdPage) & 0xFF);

  Packet.Timeout = EFI_TIMER_PERIOD_SECONDS (1);
  Packet.Cdb = &Cdb6;
  Packet.CdbLength = (UINT8) sizeof (Cdb6);
  Packet.InDataBuffer = &AtaVpdPage;
  Packet.InTransferLength = sizeof (AtaVpdPage);
  Packet.SenseData = &(DevicePrivateData->ScsiSenseDataExt);
  Packet.SenseDataLength = (UINT8) sizeof (SenseDataExt);

  Status = ExtScsiPassThruProtocol->PassThru (
             ExtScsiPassThruProtocol,
             TargetId,
             Lun,
             &Packet,
             NULL
             );

  if (Status == EFI_SUCCESS) {
    IdentifyData = (IDENTIFY_TAB *)&(DevicePrivateData->DeviceIdentifyDataRaw);

    CopyMem (
      &(IdentifyData->ModelName),
      &(AtaVpdPage.SatVendorIdentification),
      24
      );

    TempString = AllocateZeroPool (0x100);
    if (TempString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempString, IdentifyData->ModelName, 24);
    //
    // Swap the string since Identify Table format is inverted
    //
    Index = 0;
    while (TempString[Index] != 0 || TempString[Index+1] != 0) {
      Temp8 = TempString[Index];
      TempString[Index] = TempString[Index+1];
      TempString[Index+1] = Temp8;
      Index +=2;
    }
    CopyMem (IdentifyData->ModelName, TempString, 24);

    CopyMem (
      &(IdentifyData->FirmwareVer),
      &(AtaVpdPage.SatProductRevisionLevel),
      4
      );

    ZeroMem (TempString, 0x100);
    CopyMem (TempString, IdentifyData->FirmwareVer, 4);
    //
    // Swap the string since Identify Table format is inverted
    //
    Index = 0;
    while (TempString[Index] != 0 || TempString[Index+1] != 0) {
      Temp8 = TempString[Index];
      TempString[Index] = TempString[Index+1];
      TempString[Index+1] = Temp8;
      Index +=2;
    }
    CopyMem (IdentifyData->FirmwareVer, TempString, 4);

    if (AtaVpdPage.PeripheralDeviceType == EFI_SCSI_TYPE_CDROM) {
      IdentifyData->config |= ATAPI_DEVICE_BIT;
    }
  }

  if (TempString != NULL) {
    gBS->FreePool (TempString);
  }

  return Status;

}

/**

  Install DiskInfo protocol interface on disk handle by ExtScsiPassThru protocol

  @param  DiskHandle            The handle of Disk
  @param  PciDevicePath         The DevicePath of Disk
  @param  DevicePrivateData     The davice data

  @retval EFI_SUCCESS           Install DiskInfo successfully
  @retval others                Install failed

**/
EFI_STATUS
EFIAPI
InstallDiskInfoProtocolByScsi (
  EFI_HANDLE                            DiskHandle,
  PCI_DEVICE_PATH                       *PciDevicePath,
  DEVICE_PRIVATE_DATA                   *DevicePrivateData
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_EXT_SCSI_PASS_THRU_PROTOCOL       *ExtScsiPassThruProtocol;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *ExtScsiPassThruDevPath;
  BOOLEAN                               IsMatched;
  SCSI_DEVICE_PATH                      *CurrentScsiDevicePath;

  HandleBuffer = NULL;
  HandleCount  = 0;

  ExtScsiPassThruDevPath = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiExtScsiPassThruProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if ((Status != EFI_SUCCESS) || (HandleBuffer == NULL)) {
    return Status;
  }

  //
  // Find out the handle installed ExtScsiPassThru protocol, and match the RAID controller
  //
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->OpenProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &ExtScsiPassThruDevPath,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }


    IsMatched = FALSE;
    TmpDevicePath = ExtScsiPassThruDevPath;
    while (!IsDevicePathEnd (TmpDevicePath)) {
      if((TmpDevicePath->Type == HARDWARE_DEVICE_PATH) && (TmpDevicePath->SubType == HW_PCI_DP)) {
        if((((PCI_DEVICE_PATH*)TmpDevicePath)->Device == PciDevicePath->Device) &&
            (((PCI_DEVICE_PATH*)TmpDevicePath)->Function) == PciDevicePath->Function) {
            //
            // Controller matched
            //
            IsMatched = TRUE;
            break;
        }
      }
      TmpDevicePath = NextDevicePathNode (TmpDevicePath);
    }
    if(!IsMatched) {
      continue;
    }

    TmpDevicePath = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *)PciDevicePath);

    if ((TmpDevicePath->Type == MESSAGING_DEVICE_PATH) &&
        (TmpDevicePath->SubType == MSG_SCSI_DP)) {
      //
      //  Set Device port number
      //
      CurrentScsiDevicePath = (SCSI_DEVICE_PATH*) TmpDevicePath;
      DevicePrivateData->Port                  = CurrentScsiDevicePath->Pun;
      DevicePrivateData->PortMultiplierPort    = 0;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiExtScsiPassThruProtocolGuid,
                    (VOID **)&ExtScsiPassThruProtocol
                    );
    if (Status == EFI_SUCCESS) {
      //
      // Prepare the DiskInfo and install DiskInfo protocol to the Handle
      //

      DevicePrivateData->ControllerDevicePath  = ExtScsiPassThruDevPath;
      DevicePrivateData->ExtScsiPassThruPtr    = ExtScsiPassThruProtocol;

      DevicePrivateData->DiskInfo.Identify     = ScsiDiskInfoIdentify;
      DevicePrivateData->DiskInfo.Inquiry      = ScsiDiskInfoInquiry;
      DevicePrivateData->DiskInfo.SenseData    = ScsiDiskInfoSenseData;
      DevicePrivateData->DiskInfo.WhichIde     = ScsiDiskInfoWhichIde;

      InquiryByExtScsiPassThru (DevicePrivateData);

      Status = gBS->InstallProtocolInterface (
                      &DiskHandle,
                      &gEfiDiskInfoProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &(DevicePrivateData->DiskInfo)
                      );

      break;
    }
  }

  gBS->FreePool (HandleBuffer);
  return Status;

}

/**
  Find out the handle of HDD or ODD and install DiskInfo protocol on it.

  @retval EFI_SUCCESS           Install DiskInfo successfully
  @retval others                Install failed

**/
EFI_STATUS
EFIAPI
SearchHardDiskHandle (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  PCI_DEVICE_PATH                       *PciDevicePath;
  SATA_DEVICE_PATH                      *CurrentSataDevicePath;

  PciDevicePath = NULL;
  CurrentSataDevicePath = NULL;

  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Get the device paths of all hard disks
  //
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->OpenProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }
    TmpDevicePath = DevicePath;
    while (!IsDevicePathEnd (TmpDevicePath)) {
      if ((DevicePathType (TmpDevicePath) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (TmpDevicePath) == HW_PCI_DP)) {
        PciDevicePath = (PCI_DEVICE_PATH *) TmpDevicePath;
      }
      if ((TmpDevicePath->Type == MESSAGING_DEVICE_PATH) &&
        (TmpDevicePath->SubType == MSG_SATA_DP) &&
        (((SATA_DEVICE_PATH*) TmpDevicePath)->PortMultiplierPortNumber & SATA_HBA_DIRECT_CONNECT_FLAG) &&
        ((NextDevicePathNode (TmpDevicePath))->Type == END_DEVICE_PATH_TYPE)) {
        //
        // Store the RAID driver device path and install DiskInfo on it's handle
        //
        CurrentSataDevicePath = (SATA_DEVICE_PATH*) TmpDevicePath;

        if (PciDevicePath != NULL) {
          InstallDiskInfoProtocolByAta (
            HandleBuffer[HandleIndex],
            PciDevicePath,
            CurrentSataDevicePath
            );
        }
      }

      TmpDevicePath = NextDevicePathNode (TmpDevicePath);
    }
  }

  if (CurrentSataDevicePath == NULL) {
    return EFI_NO_MEDIA;
  }

  return Status;
}

/**

  Check that UEFI RAID driver installs DiskInfo protocol or not.

  @retval EFI_SUCCESS           UEFI RAID driver already installs DiskInfo protocol
  @retval others                UEFI RAID driver doesn't support DiskInfo protocol

**/
EFI_STATUS
EFIAPI
CheckDiskInfoProtocol (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;

  HandleBuffer = NULL;
  HandleCount  = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return Status;
  }
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->OpenProtocol (
                HandleBuffer[HandleIndex],
                &gEfiDiskInfoProtocolGuid,
                (VOID **) &DiskInfo,
                NULL,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
    //
    // filter not HDD
    //
    if (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoUsbInterfaceGuid)) {
      continue;
    }

    if (Status == EFI_SUCCESS) {
      gBS->FreePool (HandleBuffer);
      return EFI_SUCCESS;
    }
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }
  return EFI_UNSUPPORTED;
}

/**
  After installing AtaPassThru protocol, the callback will check AhciBus support AtaPassThru protocol.
  1. If supporting, the callback installs gUefiRaidOpromReadyGuid to make UEFI RAID OpROM dispatch and run.
  2. If not supporting, the callback checks any DiskInfo protocol on HDD or ODD handle.
     If any one doesn't have DiskInfo Protocol, this driver will install.

  @param  Event                 Pointer to this event
  @param  Context               Event hanlder private data

  @retval EFI_SUCCESS

**/
VOID
EFIAPI
DelayUefiRaidOpromAtaPassThruCallBack (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_STATUS                            Status;

  if (mDelayUefiRaidOpromPrivate->AhciBusSupport) {
    if (mDelayUefiRaidOpromPrivate->RaidControllerAllConnected) {
      //
      //  Install this Guid to make UEFI RAID OpROM run
      //
      Status = gBS->InstallProtocolInterface (
                      &(mDelayUefiRaidOpromPrivate->ControllerHandle),
                      &gUefiRaidOpromReadyGuid,
                      EFI_NATIVE_INTERFACE,
                      &(mDelayUefiRaidOpromPrivate->UefiRaidOpromReadyProtocol)
                      );
    }
  } else {
    //
    // AhciBus does not support
    // DiskInfo will be installed due to UEFI RAID OpROM not support.
    //
    Status = CheckDiskInfoProtocol();
    if (Status != EFI_SUCCESS) {
      SearchHardDiskHandle();
    }
  }

}

/**
  After installing ExtScsiPassThru protocol, the callback will check any device in list.
  If any device in list, the callback tries to install DiskInfo protocol.

  @param  Event                 Pointer to this event
  @param  Context               Event hanlder private data

  @retval EFI_SUCCESS

**/
VOID
EFIAPI
DelayUefiRaidOpromExtScsiPassThruCallBack (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_HANDLE                            QueryHandle;
  EFI_EXT_SCSI_PASS_THRU_PROTOCOL       *ExtScsiPassThruProtocol;
  UINT64                                Lun;
  UINT8                                 *TargetId;
  SCSI_TARGET_ID                        ScsiTargetId;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  PCI_DEVICE_PATH                       *PciDevicePath;
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;
  LIST_ENTRY                            *Link;

  if (IsListEmpty (&(mDelayUefiRaidOpromPrivate->DeviceInfoListHead))) {
    //
    //  In some platform, no AtaPassThru, and DevicePath is processed after ScsiIo protocol installed
    //

    return;
  }


  TargetId = &ScsiTargetId.ScsiId.ExtScsi[0];
  SetMem (TargetId, TARGET_MAX_BYTES, 0xFF);
  Lun  = 0;

  HandleCount  = 0;
  HandleBuffer = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiExtScsiPassThruProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (Status == EFI_SUCCESS) {
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
      QueryHandle = HandleBuffer[HandleIndex];

      Status = gBS->HandleProtocol (
                      QueryHandle,
                      &gEfiExtScsiPassThruProtocolGuid,
                      (VOID**) &ExtScsiPassThruProtocol
                      );

      mDelayUefiRaidOpromPrivate->ExtScsiPassThruProtocol = ExtScsiPassThruProtocol;

      while (Status == EFI_SUCCESS) {
        Status = ExtScsiPassThruProtocol->GetNextTargetLun (
                   ExtScsiPassThruProtocol,
                   &TargetId,
                   &Lun
                   );
        if (Status != EFI_SUCCESS) {
          break;
        }

        Link              = GetFirstNode (&(mDelayUefiRaidOpromPrivate->DeviceInfoListHead));
        DevicePrivateData = DEVICE_PRIVATE_FROM_LINK (Link);

        do {
          if (DevicePrivateData->Port == *TargetId) {
            //
            //  Check device type, and check DiskInfo.
            //  If no DiskInfo, install DiskInfo
            //

            Status = gBS->HandleProtocol (
                            DevicePrivateData->DeviceHandle,
                            &gEfiDiskInfoProtocolGuid,
                            (VOID**) &DiskInfo
                            );

            if (Status == EFI_SUCCESS) {
              break;
            }

            Status = gBS->OpenProtocol (
                            DevicePrivateData->DeviceHandle,
                            &gEfiDevicePathProtocolGuid,
                            (VOID **) &DevicePath,
                            NULL,
                            NULL,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL
                            );
            if (Status != EFI_SUCCESS) {
              continue;
            }
            TmpDevicePath = DevicePath;
            while (!IsDevicePathEnd (TmpDevicePath)) {
              if ( ((DevicePathType (TmpDevicePath) == HARDWARE_DEVICE_PATH) && (DevicePathSubType (TmpDevicePath) == HW_PCI_DP))
                  && ( ((NextDevicePathNode (TmpDevicePath))->Type == MESSAGING_DEVICE_PATH) &&
                       ( ((NextDevicePathNode (TmpDevicePath))->SubType == MSG_SATA_DP) ||
                         ((NextDevicePathNode (TmpDevicePath))->SubType == MSG_SCSI_DP) ) )
                  ) {
                PciDevicePath = (PCI_DEVICE_PATH *) TmpDevicePath;

                InstallDiskInfoProtocolByScsi (
                  DevicePrivateData->DeviceHandle,
                  PciDevicePath,
                  DevicePrivateData
                  );
              }

              TmpDevicePath = NextDevicePathNode (TmpDevicePath);
            }
            break;
          }
          Link = GetNextNode (
                   &(mDelayUefiRaidOpromPrivate->DeviceInfoListHead),
                   &DevicePrivateData->Link
                   );
          DevicePrivateData = DEVICE_PRIVATE_FROM_LINK (Link);
        } while (!IsNull (&(mDelayUefiRaidOpromPrivate->DeviceInfoListHead), &(DevicePrivateData->Link)));

      }
      Status = EFI_SUCCESS;

    }
  }

}

/**

  After installing ScsiIo protocol, the callback will check DiskInfo protocol in device handles.
  The callback tries to install DiskInfo protocol if there is no DiskInfo protocol in device handles.

  @param  Event                 Pointer to this event
  @param  Context               Event hanlder private data

  @retval EFI_SUCCESS

**/
VOID
EFIAPI
DelayUefiRaidOpromScsiIoCallBack (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  PCI_DEVICE_PATH                       *PciDevicePath;
  SCSI_DEVICE_PATH                      *CurrentScsiDevicePath;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  HandleCount  = 0;
  HandleBuffer = NULL;

  CurrentScsiDevicePath = NULL;

  DevicePrivateData = NULL;

  //
  //  Find Handle
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiScsiIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount;HandleIndex++) {

    Status = gBS->HandleProtocol (
                HandleBuffer[HandleIndex],
                &gEfiDiskInfoProtocolGuid,
                (VOID**) &DiskInfo
                );
    if (Status == EFI_SUCCESS) {
      continue;
    }

    DevicePrivateData = AllocateZeroPool (sizeof (DEVICE_PRIVATE_DATA));

    if (DevicePrivateData == NULL) {
      gBS->FreePool (HandleBuffer);
      return;
    }

    DevicePrivateData->Signature    = DEVICE_PRIVATE_DATA_SIGNATURE;
    DevicePrivateData->DeviceHandle = HandleBuffer[HandleIndex];
    CopyMem (&DevicePrivateData->DiskInfo.Interface, &gEfiDiskInfoAhciInterfaceGuid, sizeof (EFI_GUID));
    InsertTailList (
      &(mDelayUefiRaidOpromPrivate->DeviceInfoListHead),
      &DevicePrivateData->Link
      );

    Status = gBS->HandleProtocol (
                DevicePrivateData->DeviceHandle,
                &gEfiDevicePathProtocolGuid,
                (VOID**) &DevicePath
                );

    if (Status == EFI_SUCCESS) {
      TmpDevicePath = DevicePath;
      while (!IsDevicePathEnd (TmpDevicePath)) {
        if ( (DevicePathType (TmpDevicePath) == HARDWARE_DEVICE_PATH) && (DevicePathSubType (TmpDevicePath) == HW_PCI_DP)
           && ( ((NextDevicePathNode (TmpDevicePath))->Type == MESSAGING_DEVICE_PATH) && ((NextDevicePathNode (TmpDevicePath))->SubType == MSG_SCSI_DP)  )
        ) {
          PciDevicePath = (PCI_DEVICE_PATH *) TmpDevicePath;

          TmpDevicePath = NextDevicePathNode (TmpDevicePath);

          CurrentScsiDevicePath = (SCSI_DEVICE_PATH*) TmpDevicePath;

          InstallDiskInfoProtocolByScsi (
            HandleBuffer[HandleIndex],
            PciDevicePath,
            DevicePrivateData
            );

          break;
        }

        TmpDevicePath = NextDevicePathNode (TmpDevicePath);
      }
    }

  }

}


EFI_STATUS
EFIAPI
UefiRaidOpromReadyInterface (
  IN  EFI_UEFI_RAID_OPROM_READY_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

/**
  Main entry for this driver.
  Only check BOOT type.
  ATA mode will be checked at DelayUefiRaidOpromPlatformDriverOverrideGetDriver().

  @param  ImageHandle           Image handle this driver.
  @param  SystemTable           Pointer to SystemTable.

  @retval EFI_SUCESS            This function always complete successfully.

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromEntry (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINT8                                 BootType;
  UINTN                                 Size;
  BOOLEAN                               InstallUefiRaidOpromReadyFlag;
  VOID                                  *HobList;
  UINT8                                 *Raw;

  InstallUefiRaidOpromReadyFlag = FALSE;
  HobList = NULL;
  Raw = NULL;

  gDelayUefiRaidOpromDriverImageHandle = ImageHandle;

  //
  // Get BootType by Variable
  //
  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &gSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );
  if (Status != EFI_SUCCESS) {
    //
    // Get Hob list to check BOOT type
    // when first boot after flashing BIOS.
    //
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);

    Raw = GetNextGuidHob (&gSetupDefaultHobGuid, HobList);
    if (Raw != NULL) {
      Raw = GET_GUID_HOB_DATA (Raw);
      if (((KERNEL_CONFIGURATION *)Raw)->BootTypeReserved == EFI_BOOT_TYPE) {
        InstallUefiRaidOpromReadyFlag = TRUE;
      }
    }

  } else {
    if (BootType == EFI_BOOT_TYPE) {
      InstallUefiRaidOpromReadyFlag = TRUE;
    }
  }

  if (InstallUefiRaidOpromReadyFlag) {
    Status = gBS->AllocatePool (
                     EfiBootServicesData,
                     sizeof (EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL),
                     (VOID **)&gDelayUefiRaidOpromPlatformDriverOverride
                     );
    if (Status != EFI_SUCCESS) {
      return Status;
    }
    gDelayUefiRaidOpromPlatformDriverOverride->GetDriver     = DelayUefiRaidOpromPlatformDriverOverrideGetDriver;
    gDelayUefiRaidOpromPlatformDriverOverride->GetDriverPath = DelayUefiRaidOpromPlatformDriverOverrideGetDriverPath;
    gDelayUefiRaidOpromPlatformDriverOverride->DriverLoaded  = DelayUefiRaidOpromPlatformDriverOverrideGetDriverLoaded;

    //
    // Install the PlatformDriverOverride Potocal of DelayUefiRaidOpromPlatformDriverOverride
    //
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gEfiPlatformDriverOverrideProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    gDelayUefiRaidOpromPlatformDriverOverride
                    );
    //
    // Install the DriverBindingPotocal of DelayUefiRaidOpromDriverBinding
    //
    Status = EfiLibInstallDriverBinding (
               ImageHandle,
               SystemTable,
               &gDelayUefiRaidOpromDriverBinding,
               ImageHandle
               );

    //
    // Store private data
    //
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (DELAY_UEFI_RAID_OPROM_PRIVATE),
                    (VOID *)&mDelayUefiRaidOpromPrivate
                    );

    mDelayUefiRaidOpromPrivate->Signature                  = DELAY_UEFI_RAID_OPROM_SIGNATURE;
    mDelayUefiRaidOpromPrivate->DriverBindingProtocol      = NULL;
    mDelayUefiRaidOpromPrivate->ControllerHandle           = NULL;
    mDelayUefiRaidOpromPrivate->ThisImageHandle            = ImageHandle;
    mDelayUefiRaidOpromPrivate->StartFinish                = FALSE;
    mDelayUefiRaidOpromPrivate->AhciBusSupport             = FALSE;
    mDelayUefiRaidOpromPrivate->RaidControllerInfoGet      = FALSE;
    mDelayUefiRaidOpromPrivate->RaidControllerAllConnected = FALSE;
    mDelayUefiRaidOpromPrivate->RaidControllerRemain       = 0;
    mDelayUefiRaidOpromPrivate->ExtScsiPassThruProtocol    = NULL;

    mDelayUefiRaidOpromPrivate->UefiRaidOpromReadyProtocol.UefiRaidOpromReadyInterface = UefiRaidOpromReadyInterface;
    InitializeListHead (&(mDelayUefiRaidOpromPrivate->DeviceInfoListHead));
    InitializeListHead (&(mDelayUefiRaidOpromPrivate->RaidControllerInfoListHead));

    mDelayUefiRaidOpromEvent       = NULL;
    mDelayUefiRaidOpromScsiEvent   = NULL;
    mDelayUefiRaidOpromScsiIoEvent = NULL;
  }

  return EFI_SUCCESS;
}

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                                Status;
  EFI_PCI_IO_PROTOCOL                       *PciIo;
  PCI_TYPE00                                PciData;
  EFI_DEVICE_PATH_PROTOCOL                  *ParentDevicePath;


  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo
                  );

  if (Status != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData),
                        &PciData
                        );
  if (Status != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  if (!(PciData.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE &&
      PciData.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Open the IO Abstraction(s) needed to perform the supported test.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID*) &ParentDevicePath,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_SUCCESS) {
    Status = gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle
                    );

  }

  if (mDelayUefiRaidOpromPrivate->StartFinish && mDelayUefiRaidOpromPrivate->RaidControllerAllConnected) {
    return EFI_SUCCESS;
  }

  mDelayUefiRaidOpromPrivate->AhciBusSupport = AhciBusSupportCheck (PciIo);

  if (!(mDelayUefiRaidOpromPrivate->AhciBusSupport) && mDelayUefiRaidOpromPrivate->RaidControllerAllConnected) {
    //
    //  Install gUefiRaidOpromReadyGuid to make UEFI RAID OpROM run if AhciBusSupport = FALSE
    //
    Status = gBS->InstallProtocolInterface (
                    &ControllerHandle,
                    &gUefiRaidOpromReadyGuid,
                    EFI_NATIVE_INTERFACE,
                    &(mDelayUefiRaidOpromPrivate->UefiRaidOpromReadyProtocol)
                    );
    if (Status != EFI_SUCCESS) {
      Status = gBS->InstallProtocolInterface (
                      &(mDelayUefiRaidOpromPrivate->ThisImageHandle),
                      &gUefiRaidOpromReadyGuid,
                      EFI_NATIVE_INTERFACE,
                      &(mDelayUefiRaidOpromPrivate->UefiRaidOpromReadyProtocol)
                      );
    }
  }

  return EFI_SUCCESS;
}

/**
  Starts a device controller or a bus controller.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS                                Status;
  VOID                                      *Registration;
  VOID                                      *RegistrationScsi;
  VOID                                      *RegistrationScsiIo;

  Status = EFI_SUCCESS;

  mDelayUefiRaidOpromPrivate->DriverBindingProtocol = This;
  mDelayUefiRaidOpromPrivate->ControllerHandle      = ControllerHandle;

  if (!(mDelayUefiRaidOpromPrivate->StartFinish)) {
    //
    //  AtaPassThruProtocool Nofity
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DelayUefiRaidOpromAtaPassThruCallBack,
                    NULL,
                    &mDelayUefiRaidOpromEvent
                    );

    if (Status == EFI_SUCCESS) {
      Status = gBS->RegisterProtocolNotify (
                      &gEfiAtaPassThruProtocolGuid,
                      mDelayUefiRaidOpromEvent,
                      &Registration
                      );
    }

    if (!(mDelayUefiRaidOpromPrivate->AhciBusSupport)) {
      //
      //  if AhciBus did NOT support, EfiExtScsiPassThruProtocol would process the ATA device information.
      //

      //
      //  ExtScsiPassThruProtocol Nofity
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      DelayUefiRaidOpromExtScsiPassThruCallBack,
                      NULL,
                      &mDelayUefiRaidOpromScsiEvent
                      );

      if (Status == EFI_SUCCESS) {
        Status = gBS->RegisterProtocolNotify (
                        &gEfiExtScsiPassThruProtocolGuid,
                        mDelayUefiRaidOpromScsiEvent,
                        &RegistrationScsi
                        );
      }

      //
      //  ScsiIoProtocol Nofity
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      DelayUefiRaidOpromScsiIoCallBack,
                      NULL,
                      &mDelayUefiRaidOpromScsiIoEvent
                      );

      if (Status == EFI_SUCCESS) {
        Status = gBS->RegisterProtocolNotify (
                        &gEfiScsiIoProtocolGuid,
                        mDelayUefiRaidOpromScsiIoEvent,
                        &RegistrationScsiIo
                        );
      }
    }
  }

  mDelayUefiRaidOpromPrivate->StartFinish = TRUE;

  return Status;
}

/**
  Stops a device controller or a bus controller.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
DelayUefiRaidOpromDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  )
/*++

  Routine Description:
    Stop this driver on ControllerHandle by removing Disk IO protocol and closing
    the Block IO protocol on ControllerHandle.

  Arguments:
    This              - Protocol instance pointer.
    ControllerHandle  - Handle of device to stop driver on.
    NumberOfChildren  - Not used.
    ChildHandleBuffer - Not used.

  Returns:
    EFI_SUCCESS         - This driver is removed ControllerHandle.
    other               - This driver was not removed from this device.
    EFI_UNSUPPORTED

--*/
{

  return EFI_SUCCESS;
}

/**
  Provides inquiry information for the controller type.

  This function is used by the IDE bus driver to get inquiry data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]     This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in,out] InquiryData       Pointer to a buffer for the inquiry data.
  @param[in,out] InquiryDataSize   Pointer to the value for the inquiry data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading InquiryData from device
  @retval EFI_BUFFER_TOO_SMALL   InquiryDataSize not big enough

**/
EFI_STATUS
EFIAPI
AtaDiskInfoInquiry (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *InquiryData,
  IN OUT UINT32                         *InquiryDataSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Provides identify information for the controller type.

  This function is used by the IDE bus driver to get identify data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]     This               Pointer to the EFI_DISK_INFO_PROTOCOL
                                    instance.
  @param[in,out] IdentifyData       Pointer to a buffer for the identify data.
  @param[in,out] IdentifyDataSize   Pointer to the value for the identify data
                                    size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading IdentifyData from device
  @retval EFI_BUFFER_TOO_SMALL   IdentifyDataSize not big enough

**/
EFI_STATUS
EFIAPI
AtaDiskInfoIdentify (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *IdentifyData,
  IN OUT UINT32                         *IdentifyDataSize
  )
{
  EFI_STATUS                            Status;
  EFI_ATA_PASS_THRU_COMMAND_PACKET      AtaPassThruCmdPacket;
  EFI_ATA_STATUS_BLOCK                  Asb;
  EFI_ATA_COMMAND_BLOCK                 Acb;
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);
  if (*IdentifyDataSize < IDENTIFY_TABLE_SIZE) {
    *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Initial Command packet
  //
  ZeroMem (&AtaPassThruCmdPacket, sizeof (EFI_ATA_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  ZeroMem (&Acb, sizeof (EFI_ATA_COMMAND_BLOCK));
  AtaPassThruCmdPacket.Asb = &Asb;
  AtaPassThruCmdPacket.Acb = &Acb;
  //
  // Fill in command packet
  //
  AtaPassThruCmdPacket.Acb->AtaCommand      = ATA_IDENTIFY_CMD;
  AtaPassThruCmdPacket.Acb->AtaDeviceHead   |= (((DevicePrivateData->PortMultiplierPort)<< 4) | 0xe0);
  AtaPassThruCmdPacket.InDataBuffer         = (VOID *)IdentifyData;
  AtaPassThruCmdPacket.InTransferLength     = ATA_BLOCK_UNIT;
  AtaPassThruCmdPacket.Protocol             = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  AtaPassThruCmdPacket.Length               = EFI_ATA_PASS_THRU_LENGTH_BYTES;

  //
  // Processing AtaPassThru command
  //
  Status = DevicePrivateData->AtaPassThruPtr->PassThru (
                                                DevicePrivateData->AtaPassThruPtr,
                                                DevicePrivateData->Port,
                                                DevicePrivateData->PortMultiplierPort,
                                                &AtaPassThruCmdPacket,
                                                0
                                                );
  if(Status == EFI_SUCCESS) {
    *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
  Provides sense data information for the controller type.

  This function is used by the IDE bus driver to get sense data.
  Data format of Sense data is defined by the Interface GUID.

  @param[in]     This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in,out] SenseData         Pointer to the SenseData.
  @param[in,out] SenseDataSize     Size of SenseData in bytes.
  @param[out]    SenseDataNumber   Pointer to the value for the sense data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class.
  @retval EFI_DEVICE_ERROR       Error reading SenseData from device.
  @retval EFI_BUFFER_TOO_SMALL   SenseDataSize not big enough.

**/
EFI_STATUS
EFIAPI
AtaDiskInfoSenseData (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *SenseData,
  IN OUT UINT32                         *SenseDataSize,
  OUT    UINT8                          *SenseDataNumber
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This function is used by the IDE bus driver to get controller information.

  @param[in]  This         Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[out] IdeChannel   Pointer to the Ide Channel number.  Primary or secondary.
  @param[out] IdeDevice    Pointer to the Ide Device number.  Master or slave.

  @retval EFI_SUCCESS       IdeChannel and IdeDevice are valid.
  @retval EFI_UNSUPPORTED   This is not an IDE device.

**/
EFI_STATUS
EFIAPI
AtaDiskInfoWhichIde (
  IN  EFI_DISK_INFO_PROTOCOL            *This,
  OUT UINT32                            *IdeChannel,
  OUT UINT32                            *IdeDevice
  )
{
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);

  *IdeChannel = DevicePrivateData->Port;
  *IdeDevice  = DevicePrivateData->PortMultiplierPort;

  return EFI_SUCCESS;
}

/**
  Provides inquiry information for the controller type.

  This function is used by the IDE bus driver to get inquiry data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]     This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in,out] InquiryData       Pointer to a buffer for the inquiry data.
  @param[in,out] InquiryDataSize   Pointer to the value for the inquiry data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading InquiryData from device
  @retval EFI_BUFFER_TOO_SMALL   InquiryDataSize not big enough

**/
EFI_STATUS
EFIAPI
ScsiDiskInfoInquiry (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *InquiryData,
  IN OUT UINT32                         *InquiryDataSize
  )
{
  EFI_STATUS                                 Status;
  DEVICE_PRIVATE_DATA                        *DevicePrivateData;
  EFI_EXT_SCSI_PASS_THRU_SCSI_REQUEST_PACKET Packet;
  EFI_SCSI_CDB_6                             Cdb6;
  UINT64                                     Lun;
  UINT8                                      *TargetId;
  SCSI_TARGET_ID                             ScsiTargetId;

  if (This == NULL || InquiryData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*InquiryDataSize < sizeof (EFI_SCSI_INQUIRY_DATA)) {
    *InquiryDataSize = sizeof (EFI_SCSI_INQUIRY_DATA);
    return EFI_BUFFER_TOO_SMALL;
  }

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);

  if (DevicePrivateData->ExtScsiPassThruPtr != NULL) {
    TargetId = &ScsiTargetId.ScsiId.ExtScsi[0];
    SetMem (TargetId, TARGET_MAX_BYTES, 0xFF);
    *TargetId = (UINT8)(DevicePrivateData->Port);
    Lun = 0;

    ZeroMem (&Packet, sizeof (Packet));
    ZeroMem (&Cdb6, sizeof (Cdb6));
    ZeroMem (&(DevicePrivateData->ScsiSenseDataExt), sizeof (EFI_SCSI_SENSE_DATA_EXT));
    ZeroMem (&(DevicePrivateData->InquiryData), sizeof (EFI_SCSI_INQUIRY_DATA));

    Cdb6.OpCode = EFI_SCSI_OP_INQUIRY;
    Cdb6.AllocationLengthMsb = (UINT8) (sizeof (EFI_SCSI_INQUIRY_DATA) >> 8);
    Cdb6.AllocationLengthLsb = (UINT8) (sizeof (EFI_SCSI_INQUIRY_DATA) & 0xFF);


    Packet.Timeout = EFI_TIMER_PERIOD_SECONDS (1);
    Packet.Cdb = &Cdb6;
    Packet.CdbLength = (UINT8) sizeof (Cdb6);
    Packet.InDataBuffer = &(DevicePrivateData->InquiryData);
    Packet.InTransferLength = sizeof (EFI_SCSI_INQUIRY_DATA);

    Packet.SenseData = &(DevicePrivateData->ScsiSenseDataExt);
    Packet.SenseDataLength = (UINT8) sizeof (EFI_SCSI_SENSE_DATA_EXT);

    Status = DevicePrivateData->ExtScsiPassThruPtr->PassThru (
               DevicePrivateData->ExtScsiPassThruPtr,
               TargetId,
               Lun,
               &Packet,
               NULL
               );

    CopyMem (
      InquiryData,
      &(DevicePrivateData->InquiryData),
      sizeof (EFI_SCSI_INQUIRY_DATA)
      );

  } else {
    return EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Provides identify information for the controller type.

  This function is used by the IDE bus driver to get identify data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]     This               Pointer to the EFI_DISK_INFO_PROTOCOL
                                    instance.
  @param[in,out] IdentifyData       Pointer to a buffer for the identify data.
  @param[in,out] IdentifyDataSize   Pointer to the value for the identify data
                                    size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading IdentifyData from device
  @retval EFI_BUFFER_TOO_SMALL   IdentifyDataSize not big enough

**/
EFI_STATUS
EFIAPI
ScsiDiskInfoIdentify (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *IdentifyData,
  IN OUT UINT32                         *IdentifyDataSize
  )
{
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);
  if (*IdentifyDataSize < IDENTIFY_TABLE_SIZE) {
    *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (
    IdentifyData,
    &(DevicePrivateData->DeviceIdentifyDataRaw),
    IDENTIFY_TABLE_SIZE
    );

  *IdentifyDataSize = IDENTIFY_TABLE_SIZE;
  return EFI_SUCCESS;

}

/**
  Provides sense data information for the controller type.

  This function is used by the IDE bus driver to get sense data.
  Data format of Sense data is defined by the Interface GUID.

  @param[in]     This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in,out] SenseData         Pointer to the SenseData.
  @param[in,out] SenseDataSize     Size of SenseData in bytes.
  @param[out]    SenseDataNumber   Pointer to the value for the sense data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class.
  @retval EFI_DEVICE_ERROR       Error reading SenseData from device.
  @retval EFI_BUFFER_TOO_SMALL   SenseDataSize not big enough.

**/
EFI_STATUS
EFIAPI
ScsiDiskInfoSenseData (
  IN     EFI_DISK_INFO_PROTOCOL         *This,
  IN OUT VOID                           *SenseData,
  IN OUT UINT32                         *SenseDataSize,
  OUT    UINT8                          *SenseDataNumber
  )
{
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);
  if (*SenseDataSize < sizeof (DevicePrivateData->ScsiSenseDataExt)) {
    *SenseDataSize = sizeof (DevicePrivateData->ScsiSenseDataExt);
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (
    SenseData,
    &(DevicePrivateData->ScsiSenseDataExt),
    sizeof (DevicePrivateData->ScsiSenseDataExt)
    );

  *SenseDataSize = sizeof (DevicePrivateData->ScsiSenseDataExt);
  *SenseDataNumber = 1;
  return EFI_SUCCESS;
}

/**
  This function is used by the IDE bus driver to get controller information.

  @param[in]  This         Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[out] IdeChannel   Pointer to the Ide Channel number.  Primary or secondary.
  @param[out] IdeDevice    Pointer to the Ide Device number.  Master or slave.

  @retval EFI_SUCCESS       IdeChannel and IdeDevice are valid.
  @retval EFI_UNSUPPORTED   This is not an IDE device.

**/
EFI_STATUS
EFIAPI
ScsiDiskInfoWhichIde (
  IN  EFI_DISK_INFO_PROTOCOL            *This,
  OUT UINT32                            *IdeChannel,
  OUT UINT32                            *IdeDevice
  )
{
  DEVICE_PRIVATE_DATA                   *DevicePrivateData;

  DevicePrivateData = DEVICE_PRIVATE_FROM_DISKINFO (This);

  *IdeChannel = DevicePrivateData->Port;
  *IdeDevice  = DevicePrivateData->PortMultiplierPort;

  return EFI_SUCCESS;
}

/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.

  @param  Bus                   PCI bus number
  @param  Device                PCI device number
  @param  Function              PCI function number

  @return TRUE                  This is a on board PCI device.
  @return FALSE                 Not on board device.

**/
BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  )
{
  PORT_NUMBER_MAP       *PortMappingTable;
  PORT_NUMBER_MAP       EndEntry;
  UINTN                 NoPorts;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    if ((PortMappingTable[NoPorts].Bus == Bus) &&
        (PortMappingTable[NoPorts].Device == Device) &&
        (PortMappingTable[NoPorts].Function == Function)) {
      return TRUE;
    }
    NoPorts++;
  }

  return FALSE;
}

/**
 Sacn all SATA controller.

 @retval EFI_SUCCESS  Scan SATA controller successfully.
 @retval Other        Scan SATA controller failed.
**/
EFI_STATUS
EFIAPI
ScanAllSataController (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleIndex;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Device;
  UINTN                                 Function;
  UINT8                                 SataClassCReg[3];
  RAID_CONTROLLER_PRIVATE_DATA          *RaidControllerPrivateData;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }
      
    Status = PciIo->GetLocation (
                      PciIo,
                      &Seg,
                      &Bus,
                      &Device,
                      &Function
                      );
    if (Status != EFI_SUCCESS || 
        (!IsOnBoardPciDevice((UINT32)Bus, (UINT32)Device, (UINT32)Function) && PciIo->RomImage != NULL)) {
      continue;
    }

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          PCI_CLASSCODE_OFFSET,
                          3,
                          SataClassCReg
                          );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    //
    // Test whether the controller belongs to RAID type
    //
    if (SataClassCReg[2] == PCI_CLASS_MASS_STORAGE && SataClassCReg[1] == PCI_CLASS_MASS_STORAGE_RAID) {
      RaidControllerPrivateData = AllocateZeroPool (sizeof (RAID_CONTROLLER_PRIVATE_DATA));
      if (RaidControllerPrivateData == NULL) {
        gBS->FreePool (HandleBuffer);
        return EFI_OUT_OF_RESOURCES;
      }

      RaidControllerPrivateData->RaidControllerHandle = HandleBuffer[HandleIndex];
      RaidControllerPrivateData->PciBus               = Bus;
      RaidControllerPrivateData->PciDevice            = Device;
      RaidControllerPrivateData->PciFunction          = Function;
      RaidControllerPrivateData->Checked              = FALSE;

      InsertTailList (
        &(mDelayUefiRaidOpromPrivate->RaidControllerInfoListHead),
        &RaidControllerPrivateData->Link
        ); 

      mDelayUefiRaidOpromPrivate->RaidControllerRemain += 1;
    }
  }

  gBS->FreePool (HandleBuffer);

  mDelayUefiRaidOpromPrivate->RaidControllerInfoGet = TRUE;

  return Status;
}

