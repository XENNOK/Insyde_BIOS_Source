/** @file
  HddSpindown driver.

//;******************************************************************************
//;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
*/

#include "HddSpindown.h"

/**
  Get the AtaPassThru protocol installed on the target device and issue command.

  @param[in] ControllerMode     Indicate that now is IDE mode or AHCI mode.
  @param[in] CmdPacket          AtaPassThru command packet.
  @param[in] MappingTable       Port mapping table.

  @return Status Code

**/
EFI_STATUS
CommandIssue (
  IN UINT8                             ControllerMode,
  IN EFI_ATA_PASS_THRU_COMMAND_PACKET  *CmdPacket,
  IN PORT_NUMBER_MAP                   *MappingTable
  )
{
  EFI_STATUS                   Status;
  UINT8                        HandleCount;
  UINT8                        HandleIndex;
  UINTN                        BufferSize;
  UINT16                       Port;
  UINT16                       MultiplierPort;
  EFI_HANDLE                   *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePathNode;
  PCI_DEVICE_PATH              *PciDevicePath;
  EFI_ATA_PASS_THRU_PROTOCOL   *AtaPassThruPtr;

  Port = 0;
  MultiplierPort = 0;
  HandleCount = 0;
  BufferSize = 0;
  HandleBuffer = NULL;
  PciDevicePath = NULL;
  DevicePath = NULL;
  AtaPassThruPtr = NULL;
  //
  // Get the target AtaPassThruProtocol.
  //
  Status = gSmst->SmmLocateHandle (
                     ByProtocol,
                     &gEfiAtaPassThruProtocolGuid,
                     NULL,
                     &BufferSize,
                     HandleBuffer
                     );
  if ((Status != EFI_SUCCESS) && (Status != EFI_BUFFER_TOO_SMALL)) {
    return EFI_NOT_FOUND;
  }
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      BufferSize,
                      (VOID*) &HandleBuffer
                      );
    if (HandleBuffer == NULL) {
      return EFI_NOT_FOUND;
    }
    Status = gSmst->SmmLocateHandle (
                      ByProtocol,
                      &gEfiAtaPassThruProtocolGuid,
                      NULL,
                      &BufferSize,
                      HandleBuffer
                      );
  }
  HandleCount = (UINT8) (BufferSize / sizeof (EFI_HANDLE));
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gSmst->SmmHandleProtocol (
                       HandleBuffer[HandleIndex],
                       &gEfiDevicePathProtocolGuid,
                       (VOID *) &DevicePath
                       );
    if (!EFI_ERROR (Status)) {
      DevicePathNode = DevicePath;
      while (!IsDevicePathEnd (DevicePathNode)) {
        if (DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH &&
            DevicePathSubType (DevicePathNode) == HW_PCI_DP) {
          PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
          break;
        }
        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }

      if (PciDevicePath == NULL) {
        continue;
      }

      if (((ControllerMode == 0x00) &&
           (PciDevicePath->Device == MappingTable->Device) &&
           (PciDevicePath->Function== MappingTable->Function)) ||
          (ControllerMode == 0x01)) {
        Status = gSmst->SmmHandleProtocol (
                           HandleBuffer[HandleIndex],
                           &gEfiAtaPassThruProtocolGuid,
                           &AtaPassThruPtr
                           );
        if (EFI_ERROR (Status)) {
          continue;
        }
        if (ControllerMode == 0x00) {
          Port = MappingTable->PrimarySecondary;
          MultiplierPort = MappingTable->SlaveMaster;
        } else {
          Port = (UINT16) MappingTable->PortNum;
          MultiplierPort = 0;
        }
        break;
      }
    }
  }

  if ((AtaPassThruPtr == NULL) || (Status != EFI_SUCCESS)) {
    gSmst->SmmFreePool (HandleBuffer);
    return EFI_NOT_FOUND;
  }
  Status = AtaPassThruPtr->PassThru (
                             AtaPassThruPtr,
                             Port,
                             MultiplierPort,
                             CmdPacket,
                             0
                             );
  
  gSmst->SmmFreePool (HandleBuffer);
  
  return Status;
}

/**
  Issue STANDBY_IMMEDIATE command by AtaPassThru protocol..

  @param[in] This               HDD_SPINDOWN_PROTOCOL instance.
  @param[in] TargetPort         Target port (device).

  @return Status Code

**/
EFI_STATUS
EFIAPI
HDDSpinDown (
  IN HDD_SPINDOWN_PROTOCOL  *This,
  IN UINT16                 TargetPort
  )
{
  EFI_STATUS                          Status;
  EFI_ATA_PASS_THRU_COMMAND_PACKET    AtaPassThruCmdPacket;
  EFI_ATA_STATUS_BLOCK                Asb;
  EFI_ATA_COMMAND_BLOCK               Acb;
  UINT8                               NumOfPorts;
  UINT8                               Index;
  HDD_SPINDOWN_PRIVATE_DATA           *PrivateData;
  PORT_NUMBER_MAP                     *PortMappingTable;
  PORT_NUMBER_MAP                     EndEntry;

  PrivateData = HDD_SPINDOWN_FROM_HDDSPINDOWN (This);

  //
  // Initialize Command packet
  //
  ZeroMem (&AtaPassThruCmdPacket, sizeof (EFI_ATA_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  ZeroMem (&Acb, sizeof (EFI_ATA_COMMAND_BLOCK));

  AtaPassThruCmdPacket.Asb = &Asb;
  AtaPassThruCmdPacket.Acb = &Acb;

  PortMappingTable = (PORT_NUMBER_MAP*) PcdGetPtr (PcdPortNumberMapTable);
  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));
  NumOfPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NumOfPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NumOfPorts++;
  }

  if (NumOfPorts == 0) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NumOfPorts; Index++) {
    if (PortMappingTable[Index].PortNum == TargetPort) {
      break;
    }
  }

  if (PrivateData->ControllerMode == 0x00) {
    //
    // IDE mode
    //
    AtaPassThruCmdPacket.Acb->AtaDeviceHead |= ((PortMappingTable[Index].SlaveMaster << 4) | 0xE0);
  }
  //
  // Send "STANDBY IMMEDIATE¡¨command to HDD.
  //
  AtaPassThruCmdPacket.Acb->AtaCommand = STANDBY_IMMEDIATE;
  AtaPassThruCmdPacket.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  AtaPassThruCmdPacket.Length = EFI_ATA_PASS_THRU_LENGTH_NO_DATA_TRANSFER;

  Status = CommandIssue (
             PrivateData->ControllerMode,
             &AtaPassThruCmdPacket,
             &PortMappingTable[Index]
             );
  return Status;
}

/**
  HddSpindown driver Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
  @param[in] Handle             The handle on which the interface was installed.

  @return Status Code

**/
EFI_STATUS
EFIAPI
HddSpindownEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle;
  UINT8                          HandleIndex;
  UINT8                          HandleCount;
  BOOLEAN                        DevicePathMatch;
  EFI_HANDLE                     *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL         *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePathNode;
  HDD_SPINDOWN_PRIVATE_DATA      *PrivateData;

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (HDD_SPINDOWN_PRIVATE_DATA),
                    &PrivateData
                    );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  PrivateData->Signature = EFI_HDD_SPINDOWN_SIGNATURE;
  PrivateData->HddSpindown.HddSpinDown = HDDSpinDown;

  HandleCount = 0;
  //
  // Collect all disk device information
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  ((UINTN*) &HandleCount),
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS) {
    gSmst->SmmFreePool (PrivateData);
    return Status;
  }
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiDiskInfoProtocolGuid,
                    ((VOID**) &DiskInfo)
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }
    if (!((CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)) ||
          (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid)))) {
      continue;
    }
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    ((VOID**) &DevicePath)
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }
    DevicePathMatch = FALSE;
    DevicePathNode = DevicePath;
    while (!IsDevicePathEnd (DevicePathNode)) {
      if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == MSG_ATAPI_DP)) {
        //
        // IDE mode
        //
        PrivateData->ControllerMode = 0x00;
        DevicePathMatch = TRUE;
        break;
      }
      if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == MSG_SATA_DP)) {
        //
        // AHCI mode
        //
        PrivateData->ControllerMode = 0x01;
        DevicePathMatch = TRUE;
        break;
      }
      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }
    if (!DevicePathMatch) {
      gSmst->SmmFreePool (HandleBuffer);
      gSmst->SmmFreePool (PrivateData);
      return EFI_DEVICE_ERROR;
    }
  }
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                     &Handle,
                     &gHddSpindownProtocolGuid,
                     EFI_NATIVE_INTERFACE,
                     &PrivateData->HddSpindown
                    );
  gSmst->SmmFreePool (HandleBuffer);
  return Status;
}