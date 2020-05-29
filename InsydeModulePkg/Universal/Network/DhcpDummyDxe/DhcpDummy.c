// @file
//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   DhcpDummy.c
//;
//; Abstract:
//;
//;    This driver will install
//;      DHCP4_SERVICE_BINDING_PROTOCOL
//;      DHCP6_SERVICE_BINDING_PROTOCOL.
//;    When the real DHCP4 and DHCP6 not dispatch but has loaded.
//;
//;    In first phase, the dummy DHCP only support one PCI network card.
//;    The first binding LAN card will be use to BitLocker Network Unlock.
//;

#include "DhcpDummy.h"
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
DhcpDummyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
Dhcp4DummyCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  );

EFI_STATUS
EFIAPI
Dhcp4DummyDestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  );

EFI_STATUS
EFIAPI
DisableDhcp6CreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  );

EFI_STATUS
EFIAPI
Dhcp6DummyCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  );

EFI_STATUS
EFIAPI
Dhcp6DummyDestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  );

EFI_STATUS
DispatchNetworkDevice (
  IN     DUMMY_DHCP                    *DummyDhcp
  );

EFI_STATUS
EFIAPI
DisableDhcp6DestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  );

EFI_SERVICE_BINDING_PROTOCOL DisableServiceBinding    = {DisableDhcp6CreateChild, DisableDhcp6DestoryChild};
EFI_SERVICE_BINDING_PROTOCOL Dhcp4DummyServiceBinding = {Dhcp4DummyCreateChild, Dhcp4DummyDestoryChild};
EFI_SERVICE_BINDING_PROTOCOL Dhcp6DummyServiceBinding = {Dhcp6DummyCreateChild, Dhcp6DummyDestoryChild};
EFI_EVENT                    mReadyToBootEvent;
BOOLEAN                      mIPv4Enable;
BOOLEAN                      mIPv6Enable;

EFI_STATUS
DhcpDummyCommonCall (
  VOID
  )
{
  return EFI_NOT_READY;
}

/**
  Compare two device path, return the same node bytes

  @param  DevicePath1                  Compare device 1/2.
  @param  DevicePath2                  Compare device 2/2.
  @param  SameBytes                    Return the same bytes number.

**/
VOID
CompareDevicePath (
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath1,
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath2,
  UINTN                                *SameBytes
  )
{
  UINTN                                Length;
  UINT8                                *DevicePath1Ptr;
  UINT8                                *DevicePath2Ptr;

  Length = GetDevicePathSize (DevicePath1);

  DevicePath1Ptr = (UINT8 *)((VOID *) DevicePath1);
  DevicePath2Ptr = (UINT8 *)((VOID *) DevicePath2);

  *SameBytes = 0;
  while (Length > 0) {
    if (*DevicePath1Ptr != *DevicePath2Ptr) {
      break;
    }
    (*SameBytes)++;
    DevicePath1Ptr++;
    DevicePath2Ptr++;
    Length--;
  }
  //
  // If the DevicePath not end, add finish node length.
  //
  if (Length != 0) {
    *SameBytes = (*SameBytes) + 4;
  }
}

/**
  Detect DHCP Service Binding protocol has been binded.

  @retval TRUE                       DHCP_SERVICE_BINDING dispatchd
  @retval FALSE                      DHCP_SERVICE_BINDING not binded

**/
BOOLEAN
IsDhcpDispatched (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SERVICE_BINDING_PROTOCOL   *DhcpSB;

  Status = gBS->LocateProtocol (
                  &gEfiDhcp6ServiceBindingProtocolGuid,
                  NULL,
                  (VOID **) &DhcpSB
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDhcp4ServiceBindingProtocolGuid,
                  NULL,
                  (VOID **) &DhcpSB
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Detect DHCP Service protocol has been binded.

  @retval TRUE                       DHCP_PROTOCOL dispatchd
  @retval FALSE                      DHCP_PROTOCOL not binded

**/
BOOLEAN
IsDhcpServiceDispatched (
  VOID
  )
{
  EFI_STATUS                     Status;
  VOID                           *ProtocolContent;

  Status = gBS->LocateProtocol (
                  &gEfiDhcp6ProtocolGuid,
                  NULL,
                  (VOID **)&ProtocolContent
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDhcp4ProtocolGuid,
                  NULL,
                  (VOID **)&ProtocolContent
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}


/**
  Detect Network Stack driver has been loaded.

  @retval EFI_SUCCESS                Driver has loaded
  @retval EFI_NOT_READY              Driver not loaded

**/
EFI_STATUS
IsNetworkStackLoaded (
  VOID
  )
{
  EFI_STATUS                           Status;
  VOID                                 *Temp;
  NETWORK_LOCKER_PROTOCOL              *NetworkLocker;

  mIPv4Enable = FALSE;
  mIPv6Enable = FALSE;
  Status = gBS->LocateProtocol (&gEfiNetworkStackIPv4SupportGuid, NULL, (VOID **)&Temp);
  if (!EFI_ERROR (Status)) {
    mIPv4Enable = TRUE;
  }
  Status = gBS->LocateProtocol (&gEfiNetworkStackIPv6SupportGuid, NULL, (VOID **)&Temp);
  if (!EFI_ERROR (Status)) {
    mIPv6Enable = TRUE;
  }
  Status = gBS->LocateProtocol (&gEfiNetworkStackDHCPSupportGuid, NULL, (VOID **)&Temp);
  if (!EFI_ERROR (Status)) {
    mIPv4Enable = TRUE;
    mIPv6Enable = TRUE;
    return EFI_SUCCESS;
  }
  Status = gBS->LocateProtocol (&gNetworkLockerProtocolGuid, NULL, (VOID **) &NetworkLocker);
  if (!EFI_ERROR (Status)) {
    Status = NetworkLocker->NetworkStackCallback (NetworkStackSnpCallback);
    if (!EFI_ERROR (Status)) {
      mIPv4Enable = TRUE;
      mIPv6Enable = TRUE;
    }
  }
  if (mIPv4Enable || mIPv6Enable) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_READY;
  }
}

/**
  Install Dummy DHCP, will record the PCI handle and install instance of this PCI LAN.

  @param  PciHandle                  The found LAN PCI card handle.

  @retval EFI_SUCCESS                Protocol install success.
  @retval EFI_NOT_READY              Protocol has not installed.

**/
EFI_STATUS
InstallDummyProtocol (
  EFI_HANDLE            PciHandle
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            Handle;
  DUMMY_DHCP            *DummyDhcp;

  //
  // Initialize the instance of DummyDhcp protocol, both IPv4 and IPv6.
  //
  Handle    = NULL;
  DummyDhcp = NULL;
  Status    = gBS->AllocatePool (EfiBootServicesData, sizeof (DUMMY_DHCP), (VOID **) &DummyDhcp);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Keep the device path information, to check the SimpleNetworkProtocol is the same device.
  //
  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID *) &(DummyDhcp->PciDevicePath)
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  DummyDhcp->Signature     = DUMMY_DHCP_SIGNATURE;
  CopyMem (&(DummyDhcp->Dhcp4DummySB),  &Dhcp4DummyServiceBinding, sizeof (EFI_SERVICE_BINDING_PROTOCOL));
  CopyMem (&(DummyDhcp->Dhcp6DummySB),  &Dhcp6DummyServiceBinding, sizeof (EFI_SERVICE_BINDING_PROTOCOL));
  DummyDhcp->Dhcp4SB       = NULL;
  DummyDhcp->Dhcp6SB       = NULL;
  DummyDhcp->UndiHandle    = PciHandle;

  if (mIPv4Enable) {
    gBS->InstallMultipleProtocolInterfaces (
           &Handle,
           &gEfiDhcp4ServiceBindingProtocolGuid,
           &(DummyDhcp->Dhcp4DummySB),
           &gEfiDhcp4ProtocolGuid,
           &Dhcp4Dummy,
           NULL
           );
  }

  if (mIPv6Enable) {
    gBS->InstallMultipleProtocolInterfaces (
           &Handle,
           &gEfiDhcp6ServiceBindingProtocolGuid,
           &(DummyDhcp->Dhcp6DummySB),
           &gEfiDhcp6ProtocolGuid,
           &Dhcp6Dummy,
           NULL
           );
  }
  return EFI_SUCCESS;

Error:
  if (DummyDhcp != NULL) {
    gBS->FreePool (DummyDhcp);
  }

  return EFI_NOT_READY;
}


/**
  This function will run when ReadyToBoot.
  Install Dummy DHCP when DHCP not install but has loaded.

**/
VOID
EFIAPI
DhcpDummyInstallDummyProtocol (
  EFI_EVENT       Event,
  VOID            *Context
  )
{
  EFI_STATUS           Status;
  EFI_HANDLE           *HandleBuffer;
  UINTN                NumOfHandleBuf;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT8                PciClassCode[4];
  EFI_HANDLE           Handle;
  UINTN                Index;

  //
  // Detect the DHCP(4/6) has been installed.
  //
  Handle = NULL;
  if (IsDhcpDispatched ()) {
    //
    // Detect the DHCP_SERVICE_BINDING_PROTOCOL installed.
    // If not, driver will install a dummy for pass WHCK test.
    //
    if (!IsDhcpServiceDispatched ()) {
      if (!EFI_ERROR (IsNetworkStackLoaded ())) {
        if (mIPv4Enable) {
          gBS->InstallProtocolInterface (&Handle, &gEfiDhcp4ProtocolGuid,EFI_NATIVE_INTERFACE, &Dhcp4Dummy);
        }
        if (mIPv6Enable) {
          gBS->InstallProtocolInterface (&Handle, &gEfiDhcp6ProtocolGuid,EFI_NATIVE_INTERFACE, &Dhcp6Dummy);
        }
      }
    }
    goto Exit;
  }

  if (PcdGetBool (PcdDummyDhcpHintFunctionDisable)) {
    //
    // Detect Network stack driver has been loaded.
    // The Function will update mIPv4Enable and mIPv6Enable.
    //
    Status = IsNetworkStackLoaded ();
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
  } else {
    //
    // This section will enable hint of BitLocker Unlock need NetworkStack Support"
    //
    IsNetworkStackLoaded ();
    if (!mIPv4Enable && !mIPv6Enable) {
      gBS->InstallMultipleProtocolInterfaces (
             &Handle,
             &gEfiDhcp6ServiceBindingProtocolGuid,
             &DisableServiceBinding,
             NULL
             );
      goto Exit;
    }
  }

  //
  // Scan and count all network device on PCI.
  // Install counted number of dummy network dummy DHCP protocol.
  //
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumOfHandleBuf,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  for (Index = 0; Index < NumOfHandleBuf; Index++) {
    Status = gBS->HandleProtocol (
                    *(HandleBuffer + Index),
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciWidthUint32,
                          PCI_CLASS_CODE_OFFSET,
                          1,
                          &PciClassCode
                          );
    if (EFI_ERROR (Status) || (PciClassCode[3] != PCI_CLASS_NETWORK)) {
      continue;
    }

    //
    // PCI LAN Found!!!!
    //
    InstallDummyProtocol (*(HandleBuffer + Index));
  }

Exit:
  gBS->CloseEvent (Event);
}

/**
  Search all handle, find NETWORK device handle, then binding all network stack driver.

  @param  IPv6                       Indicate IP capability.

  @retval EFI_SUCCESS                Dispatch success.
  @retval EFI_NOT_READY              Dispatch not ready.

**/
EFI_STATUS
DispatchNetworkDevice (
  IN     DUMMY_DHCP                    *DummyDhcp
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           *HandleBuffer;
  UINTN                                NumOfHandleBuf;
  UINTN                                Index;
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  UINTN                                SameBytes;
  UINTN                                Length;

  //
  // Connect driver with UNDI PCI Handle
  //
  gBS->ConnectController (DummyDhcp->UndiHandle, NULL, NULL, TRUE);

  //
  // Wait for device connect complete.
  //
  Status = EFI_SUCCESS;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleNetworkProtocolGuid,
                  NULL,
                  &NumOfHandleBuf,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto DummyCreateChildExit;
  }

  for (Index = 0; Index < NumOfHandleBuf; Index++) {
    Status = gBS->HandleProtocol (
                    *(HandleBuffer + Index),
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Check the Device path is same without MAC node.
    // The Device will like two type:
    //   ACPI\PCI
    //   ACPI\PCI\MAC
    // To find the same node, ACPI\PCI, to find the same device service.
    //
    Length = GetDevicePathSize (DummyDhcp->PciDevicePath);
    CompareDevicePath (DummyDhcp->PciDevicePath, DevicePath, &SameBytes);
    if (Length != SameBytes) {
      continue;
    }

    if (mIPv4Enable) {
      Status = gBS->HandleProtocol (
                      *(HandleBuffer + Index),
                      &gEfiDhcp4ServiceBindingProtocolGuid,
                      (VOID *) &(DummyDhcp->Dhcp4SB)
                      );
      if (EFI_ERROR (Status)) {
        DummyDhcp->Dhcp4SB = NULL;
      }
    }

    if (mIPv6Enable) {
      Status = gBS->HandleProtocol (
                      *(HandleBuffer + Index),
                      &gEfiDhcp6ServiceBindingProtocolGuid,
                      (VOID *) &(DummyDhcp->Dhcp6SB)
                      );
      if (EFI_ERROR (Status)) {
        DummyDhcp->Dhcp4SB = NULL;
      }
    }
    break;
  }

  if (Index >= NumOfHandleBuf) {
    Status = EFI_NOT_FOUND;
  } else {
    Status = EFI_SUCCESS;
  }

DummyCreateChildExit:
  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }

  return Status;

}

/**
  Dummy DHCP4 CreateChild.

**/
EFI_STATUS
EFIAPI
Dhcp4DummyCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  )
{
  EFI_STATUS                           Status;
  DUMMY_DHCP                           *DummyDhcp;

  //
  // This point is Windows BitLocker Unlock Entry Point
  //
  DummyDhcp = LOCATE_DUMMY_DHCP4_INSTANCE (This);
  if (DummyDhcp->Signature != DUMMY_DHCP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DummyDhcp->Dhcp4SB == NULL) {
    Status = DispatchNetworkDevice (DummyDhcp);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }
  }

  if (DummyDhcp->Dhcp4SB != NULL) {
    return DummyDhcp->Dhcp4SB->CreateChild (DummyDhcp->Dhcp4SB, ChildHandle);
  } else {
    return EFI_NOT_READY;
  }
}

/**
  Dummy DHCP6 CreateChild when Network Stack not enable.

**/
EFI_STATUS
EFIAPI
DisableDhcp6CreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  )
{
  EFI_STATUS                           Status;
  EFI_INPUT_KEY                        Key;
  H2O_DIALOG_PROTOCOL                  *H2ODialog;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *Gop;
  UINT32                               OldMode;
  CHAR16                               *HintString;

  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **) &H2ODialog);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &Gop);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }
  OldMode   = Gop->Mode->Mode;
  HintString = (CHAR16 *) PcdGetPtr (PcdDummyDhcpHintString);

  DisableQuietBoot ();
  H2ODialog->ConfirmDialog (
                DlgOk,
                FALSE,
                0,
                NULL,
                &Key,
                HintString
                );
  Gop->SetMode (Gop, OldMode);

  return EFI_NOT_READY;
}

/**
  Dummy DHCP6 DestoryChild.

**/
EFI_STATUS
EFIAPI
DisableDhcp6DestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  )
{
  return EFI_NOT_READY;
}

/**
  Dummy DHCP6 CreateChild.

**/
EFI_STATUS
EFIAPI
Dhcp6DummyCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  )
{
  EFI_STATUS                           Status;
  DUMMY_DHCP                           *DummyDhcp;

  //
  // This point is Windows BitLocker Unlock Entry Point
  //
  DummyDhcp = LOCATE_DUMMY_DHCP6_INSTANCE (This);
  if (DummyDhcp->Signature != DUMMY_DHCP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DummyDhcp->Dhcp6SB == NULL) {
    Status = DispatchNetworkDevice (DummyDhcp);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }
  }

  if (DummyDhcp->Dhcp6SB != NULL) {
    return DummyDhcp->Dhcp6SB->CreateChild (DummyDhcp->Dhcp6SB, ChildHandle);
  } else {
    return EFI_NOT_READY;
  }
}

/**
  Dummy DHCP4 DestoryChild.

**/
EFI_STATUS
EFIAPI
Dhcp4DummyDestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  )
{
  DUMMY_DHCP                        *DummyDhcp;

  DummyDhcp = LOCATE_DUMMY_DHCP4_INSTANCE (This);
  if (DummyDhcp->Signature != DUMMY_DHCP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DummyDhcp->Dhcp4SB != NULL) {
    return DummyDhcp->Dhcp4SB->DestroyChild (DummyDhcp->Dhcp4SB, ChildHandle);
  }
  return EFI_NOT_READY;
}

/**
  Dummy DHCP6 DestoryChild.

**/
EFI_STATUS
EFIAPI
Dhcp6DummyDestoryChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    ChildHandle
  )
{
  DUMMY_DHCP                        *DummyDhcp;

  DummyDhcp = LOCATE_DUMMY_DHCP6_INSTANCE (This);
  if (DummyDhcp->Signature != DUMMY_DHCP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DummyDhcp->Dhcp6SB != NULL) {
    return DummyDhcp->Dhcp6SB->DestroyChild (DummyDhcp->Dhcp6SB, ChildHandle);
  }
  return EFI_NOT_READY;
}

/**
  The Dummy DHCP will active when ReadyToBoot event active.

**/
EFI_STATUS
EFIAPI
DhcpDummyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;

  mReadyToBootEvent = NULL;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  DhcpDummyInstallDummyProtocol,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &mReadyToBootEvent
                  );

  return Status;
}
