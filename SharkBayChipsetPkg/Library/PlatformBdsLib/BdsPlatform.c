/** @file
 This file include all platform action which can be customized by IBV/OEM.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

//[-start-120625-IB03530375-modify]//
#include <BdsPlatform.h>
#include <FrontPage.h>
#include <OemHotKey.h>
#include <VideoOutputPortSelection.h>
#include <OemBootDisplayDevice.h>
#include <BootDevicesDisplay.h>
#include <BdsCR.h>
#include <AsfSupport.h>
//[-start-130403-IB09720138-modify]//
// #include <Protocol/ChipsetLibServices.h>
//[start-130916-IB05670200-modify]//
#include <Library/FdSupportLib.h>
//[end-130916-IB05670200-modify]//
//[-start-131011-IB05400449-add]//
#include <Library/DxeIpmiBdsLib.h>
//[-end-131011-IB05400449-add]//
//[-end-130403-IB09720138-modify]//
#include <Protocol/AmtPlatformPolicy.h>
//[-start-131204-IB09740048-add]//
#include <Protocol/CRBdsHook.h>
//[-end-131204-IB09740048-add]//
//[-start-140416-IB09740077-add]//
#include <Protocol/CrSrvManager.h>
//[-end-140416-IB09740077-add]//
EFI_HANDLE                        mAmtMEBxHandle = NULL;

//[-start-130218-IB07250301-modify]//
#include <Protocol/SwitchableGraphicsEvent.h>
//[-end-130218-IB07250301-modify]//
//[-start-130401-IB06690222-add]//
#include <Protocol/DxeSmmReadyToLock.h>
//[-end-130401-IB06690222-add]//
//[-start-130516-12360009-add]//
#ifdef TXT_SUPPORT
#include <ChipsetSetupConfig.h>
#include <Guid/Acpi.h>
#include <Base.h>
#endif
//[-end-130516-12360009-add]//
//[-start-130611-IB11120016-add]//
#include <Protocol/PlatformEventProtocolGuid.h>
//[-end-130611-IB11120016-add]//
//[-start-140514-IB10300110-add]//
#include <Protocol/EventLog.h>
#include <Protocol/WatchDogHookProtocol.h>
#include <Library/WatchDogLib.h>
//[-end-140514-IB10300110-add]//

EFI_HANDLE                        mAlertATHandle = NULL;
//[-start-131011-IB05400449-add]//
extern UINT16                     *mBootNext;
//[-end-131011-IB05400449-add]//

extern PLATFORM_ISA_SERIAL_OVER_LAN_DEVICE_PATH   gSerialOverLANDevicePath;
#define DP_TYPE_MESSAGING  0x03
#define DP_SUBTYPE_USB     0x05

typedef struct {
  UINT16  VendorId;
  UINT16  DeviceId;
  UINT16  SubsystemVendorID;
  UINT16  SubsystemID;
} CHECK_VGA_ID;

CHECK_VGA_ID CheckVgaId[] = {
  {
    0x10DE,
    0x06E4,
    0x0000,
    0x0000
  }
};

//[-start-130904-IB10920045-add]//
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS CheckPegId =
//  Reg Fun Dev Bus ExReg
  {
    0, 0, 1, 0, 0
  };
//[-end-130904-IB10920045-add]//

#define AMT_SOL_LEGACY_VGA_REDIR_ROM_FILE_GUID \
   { \
     0x346b4547, 0xfef7, 0x49d4, 0x9a, 0x4a, 0xf6, 0x66, 0x82, 0x5e, 0x25, 0xb7 \
   }

EFI_STATUS
DummyDisconnectCrsTerminal (
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *This,
  UINTN                                     TerminalNumber
  )
{
  return EFI_SUCCESS;
}

//[-start-130401-IB06690222-add]//
VOID
PlatformBdsAuth (
  VOID
  );
//[-end-130401-IB06690222-add]//

/**
  This routine install SOL option ROM if needed

**/
static
VOID
InstallSOLOptionROM (
  VOID
  )
{
  BOOLEAN                       AMTEnable = FALSE;
  BOOLEAN                       SOLFound = FALSE;
  UINTN                         NumHandles = 0;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  EFI_STATUS                    Status;
  PCI_TYPE00                    PciConfigHeader00;
  UINT8                         *VgaRedirIrqBda = NULL;
  UINT16                        *VgaRedirIoBaseBda = NULL;
  VOID                          *LocalRomImage;
  UINTN                         LocalRomSize;
  EFI_IA32_REGISTER_SET         RegisterSet;
  UINTN                         Flags;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_GUID                      AmtSolVgaRedirRomFileGuid = AMT_SOL_LEGACY_VGA_REDIR_ROM_FILE_GUID;
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *CRService;
  EFI_HANDLE                                ImageHandle = NULL;
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  DummyCRService = {NULL,
                                                              DummyDisconnectCrsTerminal
                                                              };

  //
  // Amt Library Init
  //
  Status = AmtLibInit();
  if (!EFI_ERROR (Status)) {
    if (ActiveManagementEnableSol()) {
      AMTEnable = TRUE;  // End the switch case, so SOL got loaded
    }
  }

  if (!AMTEnable) {
    return;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  for (Index = 0; Index < NumHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo)
                    ;
    ASSERT_EFI_ERROR (Status);

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof(PciConfigHeader00) / sizeof(UINT32),
                          &PciConfigHeader00
                          );
    ASSERT_EFI_ERROR (Status);

    if ((PciConfigHeader00.Hdr.VendorId == V_ME_SOL_VENDOR_ID) &&
        ((PciConfigHeader00.Hdr.DeviceId == V_ME_SOL_DEVICE_ID))) {
      SOLFound = TRUE;
      //
      // Special make up for SOL option ROM
      //
      VgaRedirIrqBda     = (UINT8 *)(UINTN)0x4A6;
      VgaRedirIoBaseBda  = (UINT16 *)(UINTN)0x4A4;
      *VgaRedirIrqBda    = PciConfigHeader00.Device.InterruptLine;
      *VgaRedirIoBaseBda = (UINT16)(PciConfigHeader00.Device.Bar[0] & 0xFFFE);
      break;
    }
  }

  if (!SOLFound) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return;
  }

  LocalRomImage = NULL;
  LocalRomSize = 0;
  Status = GetSectionFromAnyFv (
             &AmtSolVgaRedirRomFileGuid,
             EFI_SECTION_RAW,
             0,
             &LocalRomImage,
             (UINTN*)&LocalRomSize
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  LegacyBios->InstallPciRom (
                LegacyBios,
                NULL,
                &LocalRomImage,
                &Flags,
                NULL,
                NULL,
                NULL,
                NULL
                );

  // Need to fixup and test int10h clear screen stuff
  // Need to use int10h now as vgaredir oprom code has hooked int10h and int16h kybrd stuff
  // so can use console spilitter stuff anymore.

  //
  //  Need to add a clear screen msg after this for when oprom display is turned on.
  //
  //  Legacy INT 10h required for screen clear as we have already enabled console redir at this point.
  //
  // Set Video mode to cloer and 80X25
  //
  ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
  RegisterSet.X.AX = 0x0003;

  Status = LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);

  //
  // Scroll text upward to clear screen
  //
  ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
  RegisterSet.X.AX = 0x0600;
  RegisterSet.H.BH = 0x00;
  RegisterSet.X.CX = 0x0000;
  RegisterSet.X.DX = 0x1950;

  Status = LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);

  if (!FeaturePcdGet(PcdH2OConsoleRedirectionSupported)) {
    Status = gBS->LocateProtocol (&gConsoleRedirectionServiceProtocolGuid, NULL, (VOID **)&CRService);
    if (EFI_ERROR (Status)) {
     //
     // Install Dummy gConsoleRedirectionServiceProtocolGuid protocol, if SOL is working.
     //
     gBS->InstallProtocolInterface (
                           &ImageHandle,
                           &gConsoleRedirectionServiceProtocolGuid,
                           EFI_NATIVE_INTERFACE,
                           &DummyCRService
                           );
    }
  }
}

#ifdef ANTI_THEFT_SUPPORT
VOID
PlatformAlertAT (
  VOID
  )
{
  EFI_STATUS    Status;

  Status = EFI_SUCCESS;

  if (mAlertATHandle == NULL) {
    //
    // Alert Intel AtHandler
    //
    Status = gBS->InstallProtocolInterface (
                    &mAlertATHandle,
                    &gAlertAtHandlerGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
}
#endif

VOID
InvokeMebxHotKey (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT8                                     iAmtbxHotkeyPressed;
  DXE_AMT_POLICY_PROTOCOL                   *AMTPlatformPolicy;

  AMTPlatformPolicy = NULL;
  AMTPlatformPolicy = AllocateZeroPool (sizeof (DXE_AMT_POLICY_PROTOCOL));
  ASSERT ( AMTPlatformPolicy != NULL );

  Status = gBS->LocateProtocol (&gDxePlatformAmtPolicyGuid, NULL, (VOID **)&AMTPlatformPolicy);

  if (!EFI_ERROR (Status)) {
    AMTPlatformPolicy->AmtConfig.iAmtbxHotkeyPressed = 1;
    Status = gBS->InstallProtocolInterface (
                    &mAmtMEBxHandle,
                    &gDxePlatformAmtPolicyGuid,
                    EFI_NATIVE_INTERFACE,
                    AMTPlatformPolicy
                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    iAmtbxHotkeyPressed = 1;

    Status = gRT->SetVariable (
                    L"iAmtbxHotkeyPressed",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (iAmtbxHotkeyPressed),
                    &iAmtbxHotkeyPressed
                    );
    ASSERT_EFI_ERROR ( Status );
  }
}

VOID
InvokeRemoteAsstHotKey (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT8                                     iRemoteAssistHotkeyPressed;
  DXE_AMT_POLICY_PROTOCOL                   *AMTPlatformPolicy;

  AMTPlatformPolicy = NULL;
  AMTPlatformPolicy = AllocateZeroPool (sizeof (DXE_AMT_POLICY_PROTOCOL));
  ASSERT ( AMTPlatformPolicy != NULL );

  Status = gBS->LocateProtocol (&gDxePlatformAmtPolicyGuid, NULL, (VOID **)&AMTPlatformPolicy);
  if (!EFI_ERROR (Status)) {
    AMTPlatformPolicy->AmtConfig.CiraRequest = 1;
    Status = gBS->InstallProtocolInterface (
                    &mAmtMEBxHandle,
                    &gDxePlatformAmtPolicyGuid,
                    EFI_NATIVE_INTERFACE,
                    AMTPlatformPolicy
                    );
    ASSERT_EFI_ERROR ( Status );
  } else {
    iRemoteAssistHotkeyPressed = 1;

    Status = gRT->SetVariable (
                    L"iRemoteAsstHotkeyPressed",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (iRemoteAssistHotkeyPressed),
                    &iRemoteAssistHotkeyPressed
                    );
    ASSERT_EFI_ERROR ( Status );
  }
}


EFI_STATUS
EFIAPI
SetVgaCommandRegEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINTN                                 IndexGop;
  UINTN                                 IndexController;
  BOOLEAN                               PrimaryExit;
  EFI_HANDLE                            PrimaryHandle;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_STATUS                            Status;
  UINT8                                 Command;
  VGA_HANDLES_INFO                      *VgaHandlesInfo;
  UINTN                                 GopHandleCount;
  EFI_HANDLE                            *GopHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL              *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *GopDevicePath;

  VgaHandlesInfo = (VGA_HANDLES_INFO*) Context;

  if (VgaHandlesInfo == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Make sure Primary Handle exist.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  PrimaryHandle = NULL;
  PrimaryExit = FALSE;

  for (IndexController = 0; IndexController < VgaHandlesInfo->VgaHandleConut; IndexController++) {
    if (PrimaryExit) {
      break;
    }

    Status = gBS->HandleProtocol (
                    VgaHandlesInfo->VgaHandleBuffer[IndexController],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&ControllerDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (IndexGop = 0; IndexGop < GopHandleCount; IndexGop++) {
      Status = gBS->HandleProtocol (GopHandleBuffer[IndexGop], &gEfiDevicePathProtocolGuid, (VOID*)&GopDevicePath);
      if (EFI_ERROR (Status)) {
        continue;
      }
      if (CompareMem (ControllerDevicePath, GopDevicePath, GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
        PrimaryHandle = VgaHandlesInfo->VgaHandleBuffer[IndexController];
        if (PrimaryHandle == VgaHandlesInfo->PrimaryVgaHandle) {
          PrimaryExit = TRUE;
          break;
        }
      }
    }
  }

  if (PrimaryHandle == NULL) {
    goto EventExit;
  }

  Command = 0;

  for (IndexController = 0; IndexController < VgaHandlesInfo->VgaHandleConut; IndexController++) {
    if (VgaHandlesInfo->VgaHandleBuffer[IndexController] != PrimaryHandle) {
      //
      // Disconnect the driver of VgaHandle.
      //
      gBS->DisconnectController (VgaHandlesInfo->VgaHandleBuffer[IndexController], NULL, NULL);
      //
      // Clear the command register of Vga device.
      //
      Status = gBS->HandleProtocol (VgaHandlesInfo->VgaHandleBuffer[IndexController], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);

      if (!EFI_ERROR(Status)) {
        PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x04, 1, &Command);
      }
    }
  }

EventExit:

  gBS->FreePool (GopHandleBuffer);

  return EFI_SUCCESS;
}


//
// BDS Platform Functions
//
/**
  Platform Bds init. Include the platform firmware vendor, revision
  and so crc check.

**/
VOID
EFIAPI
PlatformBdsInit (
  VOID
  )
{
  InitializeFrontPage (TRUE);
}


/**
  Connect RootBridge

  @retval EFI_SUCCESS   Connect RootBridge successfully.
  @retval EFI_STATUS    Connect RootBridge fail.

**/
EFI_STATUS
ConnectRootBridge (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                RootHandle;

  //
  // Make all the PCI_IO protocols on PCI Seg 0 show up
  //
  BdsLibConnectDevicePath (gPlatformRootBridges[0]);

  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &gPlatformRootBridges[0],
                  &RootHandle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // PostCode = 0x13, PCI enumeration
  //
  POST_CODE (BDS_PCI_ENUMERATION_START);
  Status = gBS->ConnectController (RootHandle, NULL, NULL, FALSE);
  //
  // PostCode = 0x15, PCI enumeration complete
  //
  POST_CODE (BDS_PCI_ENUMERATION_END);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
GetAllVgaHandles (
  OUT EFI_HANDLE  **VgaHandleBuffer,
  OUT UINTN       *VgaHandleCount
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINTN                                 HandleIndex;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            Pci;
  EFI_HANDLE                            *TempVgaHandleBuffer;
  UINTN                                 BufferIndex;

  HandleBuffer        = NULL;
  HandleCount         = 0;
  BufferIndex         = 0;
  //
  // Start PciBus.
  //
  ConnectRootBridge ();
//[-start-140227-IB10920078-modify]//
//[-start-130218-IB07250301-add]//
  if ((FeaturePcdGet (PcdSwitchableGraphicsSupported)) || (FeaturePcdGet (PcdIntelSwitchableGraphicsSupported))) {
    EFI_HANDLE                          SgHandle;
    //
    // Install Switchable Graphics Protocol to trigger
    // Switchable Graphics DXE driver registered callback function.
    //
    SgHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &SgHandle,
                    &gH2OSwitchableGraphicsEventProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  }
//[-end-130218-IB07250301-add]//
//[-end-140227-IB10920078-modify]//
  //
  // check all the pci io to find all possible VGA devices
  //
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

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (HandleCount * sizeof(EFI_HANDLE)),
                  (VOID **) &TempVgaHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (HandleBuffer[HandleIndex], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    if (!EFI_ERROR (Status)) {
      //
      // Check for all VGA device
      //
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            0,
                            sizeof (Pci) / sizeof (UINT32),
                            &Pci
                            );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (!IS_PCI_VGA (&Pci)) {
        continue;
      }

      TempVgaHandleBuffer[BufferIndex] = HandleBuffer[HandleIndex];
      BufferIndex++;
    }
  }

  if (BufferIndex == 0) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (BufferIndex * sizeof(EFI_HANDLE)),
                  (VOID **) VgaHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CopyMem (*VgaHandleBuffer, TempVgaHandleBuffer, (BufferIndex * sizeof(EFI_HANDLE)));

  *VgaHandleCount = BufferIndex;

  gBS->FreePool (HandleBuffer);
  gBS->FreePool (TempVgaHandleBuffer);

  return EFI_SUCCESS;
}


EFI_STATUS
GetGopDevicePath (
   IN  EFI_DEVICE_PATH_PROTOCOL *PciDevicePath,
   OUT EFI_DEVICE_PATH_PROTOCOL **GopDevicePath
   )
{
  UINTN                           Index;
  EFI_STATUS                      Status;
  EFI_HANDLE                      PciDeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *TempPciDevicePath;
  UINTN                           GopHandleCount;
  EFI_HANDLE                      *GopHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath;

  RemainingDevicePath = NULL;

  if (PciDevicePath == NULL || GopDevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the GopDevicePath to be PciDevicePath
  //
  *GopDevicePath    = PciDevicePath;
  TempPciDevicePath = PciDevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &TempPciDevicePath,
                  &PciDeviceHandle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Try to connect this handle, so that GOP driver could start on this
  // device and create child handles with GraphicsOutput Protocol installed
  // on them, then we get device paths of these child handles and select
  // them as possible console device.
  //
  PlatformBdsDisplayPortSelection (PciDeviceHandle, &RemainingDevicePath);
  Status = gBS->ConnectController (PciDeviceHandle, NULL, RemainingDevicePath, FALSE);
  if (EFI_ERROR(Status)) {
    //
    // In uefi boot mode If RemainingDevicePath != NULL, and connected failed.
    //
    PlatformBdsVgaConnectedFailCallback (PciDeviceHandle, RemainingDevicePath);
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Add all the child handles as possible Console Device
    //
    for (Index = 0; Index < GopHandleCount; Index++) {
      Status = gBS->HandleProtocol (GopHandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID*)&TempDevicePath);
      if (EFI_ERROR (Status)) {
        continue;
      }
      if (CompareMem (
            PciDevicePath,
            TempDevicePath,
            GetDevicePathSize (PciDevicePath) - END_DEVICE_PATH_LENGTH
            ) == 0) {
        //
        // In current implementation, we only enable one of the child handles
        // as console device, i.e. sotre one of the child handle's device
        // path to variable "ConOut"
        // In futhure, we could select all child handles to be console device
        //

        *GopDevicePath = TempDevicePath;

        //
        // Delete the PCI device's path that added by GetPlugInPciVgaDevicePath()
        // Add the integrity GOP device path.
        //
        BdsLibUpdateConsoleVariable (L"ConOutDev", NULL, PciDevicePath);
        BdsLibUpdateConsoleVariable (L"ConOutDev", TempDevicePath, NULL);
        BdsLibUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, NULL, PciDevicePath);
        BdsLibUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, TempDevicePath, NULL);
      }
    }
    gBS->FreePool (GopHandleBuffer);
  }

  return EFI_SUCCESS;
}


/**
  Add PCI VGA to ConOut.
  PCI VGA: 03 00 00

  @param  DeviceHandle    Handle of PCIIO protocol.

  @retval EFI_SUCCESS     PCI VGA is added to ConOut.
  @retval EFI_STATUS      No PCI VGA device is added.

**/
EFI_STATUS
PreparePciVgaDevicePath (
  IN EFI_HANDLE                DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL  **GopDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  DevicePath = NULL;
  Status = gBS->HandleProtocol (
                  DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID*)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GetGopDevicePath (DevicePath, GopDevicePath);

  return EFI_SUCCESS;
}

//[-start-130903-IB10920045-add]//
BOOLEAN
IsPegDevice (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *PciIoRootBridge;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  PegDevice = {0};
  EFI_PCI_IO_PROTOCOL                   *PciIo = NULL;
  UINT64                                Address;
  EFI_STATUS                            Status;
  PCI_TYPE00                            PciType;
  PCI_TYPE01                            PciBridge;
  UINT8                                 BusTmp;
  UINTN                                 Segment;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINTN                                 HandleIndex;

  BusTmp = Bus;

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **)&PciIoRootBridge
                  );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  Address = EFI_PCI_ADDRESS (Bus, Device, Function, 0);
  PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint32, Address,sizeof (PciType) / sizeof (UINT32), &PciType);

  if (!IS_PCI_VGA (&PciType)) {
    return FALSE;
  }

  //
  //
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  do {
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
      Status = gBS->HandleProtocol (HandleBuffer[HandleIndex], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
      if (!EFI_ERROR (Status)) {
        Status = PciIo->Pci.Read (
                              PciIo,
                              EfiPciIoWidthUint32,
                              0,
                              sizeof (PciBridge) / sizeof (UINT32),
                              &PciBridge
                              );
        if (EFI_ERROR (Status)) {
          continue;
        }

        if (!IS_PCI_P2P(&PciBridge)) {
          continue;
        }

        if (PciBridge.Bridge.SecondaryBus == BusTmp) {
          BusTmp = PciBridge.Bridge.PrimaryBus;
          break;
        }
      }
    }
  }while (BusTmp != 0);

  if (PciIo == NULL) {
    return FALSE;
  }

  Status = PciIo->GetLocation (
                     PciIo,
                     &Segment,
                     (UINTN *)&PegDevice.Bus,
                     (UINTN *)&PegDevice.Device,
                     (UINTN *)&PegDevice.Function
                     );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (CheckPegId.Bus != PegDevice.Bus &&
      CheckPegId.Device != PegDevice.Device &&
      CheckPegId.Function != PegDevice.Function) {
      return FALSE;
  }

  return TRUE;
}
//[-end-130903-IB10920045-add]//

EFI_HANDLE
SelectVgaHandle (
  IN VGA_DEVICE_INFO                    *VgaDeviceList,
  IN UINTN                              VgaHandleCount
  )
{
  EFI_STATUS                            Status;
  UINT8                                 CheckSequenceIndex;
  UINT8                                 SelectedVgaIndex;
  UINT8                                 ListIndex;
//[-start-140619-IB05080432-add]//
  UINT8                                 PrimaryDisplay;
//[-end-140619-IB05080432-add]//
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  CHIPSET_CONFIGURATION                  *SystemConfiguration;
  POSSIBLE_VGA_TYPE                     CheckSequence[] = {Igfx, Peg, Pcie, Pci};

  SystemConfiguration = NULL;
  SelectedVgaIndex = 0xFF;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
//[-start-140619-IB05080432-modify]//
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;
    PrimaryDisplay = SystemConfiguration->PrimaryDisplay;
  } else {
    DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
    DEBUG ((EFI_D_ERROR, "Fail to locate SetupUtilityProtocol, set PrimaryDisplay value to Auto\n"));
    PrimaryDisplay = DisplayModeAuto;
  }
//[-end-140619-IB05080432-modify]//

//[-start-140619-IB05080432-modify]//
  if ((PrimaryDisplay == DisplayModeIgfx) ||
      (PrimaryDisplay == DisplayModeSg)) {
    CheckSequence[0] = Igfx;
    CheckSequence[1] = Peg;
    CheckSequence[2] = Pcie;
    CheckSequence[3] = Pci;
  }
  if ((PrimaryDisplay == DisplayModePeg) ||
      (PrimaryDisplay == DisplayModeAuto)) {
    CheckSequence[0] = Peg;
    CheckSequence[1] = Pcie;
    CheckSequence[2] = Pci;
    CheckSequence[3] = Igfx;
  }
  if (PrimaryDisplay == DisplayModePci) {
    CheckSequence[0] = Pci;
    CheckSequence[1] = Pcie;
    CheckSequence[2] = Peg;
    CheckSequence[3] = Igfx;
  }
//[-end-140619-IB05080432-modify]//

  for (CheckSequenceIndex = 0; CheckSequenceIndex < PossibleVgaTypeMax; CheckSequenceIndex++) {
    for (ListIndex = 0; ListIndex < VgaHandleCount; ListIndex++) {
//[-start-130614-IB11550059-modify]//
      //if ((VgaDeviceList[ListIndex].VgaType == CheckSequence[CheckSequenceIndex]) &&
      //    ((SelectedVgaIndex == 0xFF) ||
      //     ((SelectedVgaIndex != 0xFF) &&
      //      (VgaDeviceList[ListIndex].Priority < VgaDeviceList[SelectedVgaIndex].Priority)))) {
      //  SelectedVgaIndex = ListIndex;
      //}
      if (VgaDeviceList[ListIndex].VgaType == CheckSequence[CheckSequenceIndex]) {
        if (SelectedVgaIndex == 0xFF) {
          SelectedVgaIndex = ListIndex;
        } else if (VgaDeviceList[ListIndex].Priority < VgaDeviceList[SelectedVgaIndex].Priority) {
          SelectedVgaIndex = ListIndex;
        }
      }
//[-end-130614-IB11550059-modify]//
    }
    if (SelectedVgaIndex != 0xFF) {
      return VgaDeviceList[SelectedVgaIndex].Handle;
    }
  }

  return NULL;
}


EFI_HANDLE
ClassifyVgaHandleAndSelect (
  IN EFI_HANDLE                         *PciVgaHandleBuffer,
  IN UINTN                              PciVgaHandleCount
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  VGA_DEVICE_INFO                       *VgaDeviceList;
  UINTN                                 VgaHandleCount;
  BOOLEAN                               FoundFlag;
  EFI_HANDLE                            SelectedVgaHandle;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *PciIoRootBridge;
  UINT64                                Address;
  UINT8                                 *TmpPtr;
  UINT8                                 IndexCheckVgaId;
  UINT8                                 Bus, Device, Function;
  PCI_TYPE00                            PciType;
  UINT16                                DeviceType;
  UINT16                                VendorId, DeviceId, SubsystemVendorID, SubsystemID;
  UINT8                                 SecondaryBus, SubordinateBus;
  UINT8                                 CapabilityPtr, CapabilityId;
  UINT8                                 DevicePathLen;
  BOOLEAN                               FindVga;
  BOOLEAN                               FindIgfx;
  BOOLEAN                               FindPeg;
  BOOLEAN                               FindPci;
//[-start-130614-IB11550059-add]//
  UINT8                                 SecondaryBusBuf, SubordinateBusBuf;
//[-end-130614-IB11550059-add]//

  FindIgfx = FALSE;
  FindPeg = FALSE;
  FindPci = FALSE;
  VendorId = 0;
  DeviceId = 0;
  SubsystemVendorID = 0;
  SubsystemID = 0;

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **)&PciIoRootBridge
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  SelectedVgaHandle = NULL;
  VgaHandleCount = PciVgaHandleCount;
  if (VgaHandleCount == 0) {
    return NULL;
  }

  if (VgaHandleCount == 1) {
    return PciVgaHandleBuffer[0];
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (VgaHandleCount * sizeof(VGA_DEVICE_INFO)),
                  (VOID **) &VgaDeviceList
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  for (Index = 0; Index < VgaHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    PciVgaHandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );

    VgaDeviceList[Index].Handle = PciVgaHandleBuffer[Index];
    FoundFlag = FALSE;

    if (!FindIgfx || !FindPeg || !FindPci) {
      FindVga        = FALSE;
      SecondaryBus   = 0;
      SubordinateBus = 0;
      Bus            = 0;
      //
      // Get value from DevicePath
      //
      TmpPtr   = (UINT8 *) DevicePath;
      //
      //ignore PciRootBridge device path
      //
      TmpPtr  += sizeof(UINT16);
      DevicePathLen = *(UINT8 *) TmpPtr;
      TmpPtr += (DevicePathLen - sizeof(UINT16));
      //
      //jump to DevicePath->Function
      //
      TmpPtr  += sizeof(UINT32);
      Function = *(UINT8 *) TmpPtr;
      TmpPtr  += sizeof(UINT8);
      Device   = *(UINT8 *) TmpPtr;
//[-start-130614-IB11550059-modify]//
      do {
        Address = EFI_PCI_ADDRESS (Bus, Device, Function, 0);
        PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint32, Address,sizeof (PciType) / sizeof (UINT32), &PciType);
        if (PciType.Hdr.ClassCode[2] == PCI_CLASS_DISPLAY && PciType.Hdr.ClassCode[1] == PCI_CLASS_DISPLAY_VGA) {
          FindVga = TRUE;
          VendorId          = PciType.Hdr.VendorId;
          DeviceId          = PciType.Hdr.DeviceId;
          SubsystemVendorID = PciType.Device.SubsystemVendorID;
          SubsystemID       = PciType.Device.SubsystemID;
          break;
        } else if (PciType.Hdr.ClassCode[2] == PCI_CLASS_BRIDGE && PciType.Hdr.ClassCode[1] == PCI_CLASS_BRIDGE_P2P) {
          Address = EFI_PCI_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
          PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SecondaryBusBuf);
          Address = EFI_PCI_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
          PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SubordinateBusBuf);

          if (SecondaryBusBuf <= SubordinateBusBuf) {
            Bus = SecondaryBusBuf;
            SecondaryBus   = SecondaryBusBuf;
            SubordinateBus = SubordinateBusBuf;
          } else {
            DEBUG ((EFI_D_ERROR, "Pci Bridge SECONDARY_BUS and SUBORDINATE_BUS are fail value.\n"));
          }

          //
          //To check device type
          //
          TmpPtr  += sizeof (UINT8);
          do {
            DeviceType = *(UINT16 *) TmpPtr;
            if (DeviceType == 0x0101) {
              TmpPtr  += sizeof (UINT32);
              Function = *(UINT8 *) TmpPtr;
              TmpPtr  += sizeof (UINT8);
              Device   = *(UINT8 *) TmpPtr;
            } else if (DeviceType == 0x7FFF) {
              return NULL;
            } else {
              TmpPtr  += sizeof (UINT16);
              DevicePathLen = *(UINT8 *) TmpPtr;
              TmpPtr += (DevicePathLen - sizeof(UINT16));
            }
          } while (DeviceType != 0x0101);
        } else {
          Bus++;
        }
      } while ((!FindVga) && (Bus <= SubordinateBus));
//[-end-130614-IB11550059-modify]//
      if (FindVga) {
        if (Bus <= 0 && !FindIgfx) {
          VgaDeviceList[Index].VgaType = Igfx;
          VgaDeviceList[Index].Priority = 0;
          FindIgfx = TRUE;
          continue;
        }
//[-start-130903-IB10920045-add]//
        if (IsPegDevice (Bus, Device, Function) && !FindPeg) {
          VgaDeviceList[Index].VgaType = Peg;
          VgaDeviceList[Index].Priority = 0;
          FindPeg = TRUE;
          continue;

        }
//[-end-130903-IB10920045-add]//
        CapabilityId  = 0xFF;
        CapabilityPtr = 0xFF;
        //
        //To check VGA is PCI or PCIe
        //
        Address = EFI_PCI_ADDRESS (Bus, Device, Function, PCI_CAPBILITY_POINTER_OFFSET);
        PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &CapabilityPtr);
        while (CapabilityId != 0x10 && CapabilityPtr != 0) {
          Address = EFI_PCI_ADDRESS (Bus, Device, Function, CapabilityPtr);
          PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &CapabilityId);
          Address = EFI_PCI_ADDRESS (Bus, Device, Function, CapabilityPtr + 1);
          PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &CapabilityPtr);
        }
        //
        // Capability ID = 0x10, the device is PCIe
        //
        if (CapabilityId == 0x10) {
          for (IndexCheckVgaId = 0; IndexCheckVgaId < sizeof (CheckVgaId) / sizeof (CHECK_VGA_ID); IndexCheckVgaId++) {
            if (VendorId == CheckVgaId[IndexCheckVgaId].VendorId && \
                DeviceId == CheckVgaId[IndexCheckVgaId].DeviceId && \
                SubsystemVendorID == CheckVgaId[IndexCheckVgaId].SubsystemVendorID && \
                SubsystemID == CheckVgaId[IndexCheckVgaId].SubsystemID) {
              VgaDeviceList[Index].VgaType = Pci;
              VgaDeviceList[Index].Priority = 0;
              FindPci = TRUE;
              continue;
            }
          }
//[-start-130904-IB10920045-remove]//
//          if (!FindPeg) {
//            VgaDeviceList[Index].VgaType = Peg;
//            VgaDeviceList[Index].Priority = 0;
//            FindPeg = TRUE;
//            continue;
//          }
//[-end-130904-IB10920045-remove]//
        } else {
          if (!FindPci) {
            VgaDeviceList[Index].VgaType = Pci;
            VgaDeviceList[Index].Priority = 0;
            FindPci = TRUE;
            continue;
          }
        }
      }
    }

    //
    // VGA Information Default Setting
    //
    VgaDeviceList[Index].VgaType = Pci;
    VgaDeviceList[Index].Priority = 100;
  }

  SelectedVgaHandle = SelectVgaHandle (VgaDeviceList, VgaHandleCount);

  gBS->FreePool (VgaDeviceList);

  return SelectedVgaHandle;
}


EFI_STATUS
LockVgaControllerVgaDriverPolicy (
  IN EFI_HANDLE       SelectedVgaHandle,
  IN EFI_HANDLE       *VgaHandleBuffer,
  IN UINTN            VgaHandleCount
  )
{
  UINTN                          DriverBindingHandleCount;
  EFI_HANDLE                     *DriverBindingHandleBuffer;
  EFI_STATUS                     Status;
  UINTN                          Index;
  CHAR16                         *DriverName;
  EFI_COMPONENT_NAME2_PROTOCOL   *ComponentNameInterface;
  EFI_HANDLE                     LockGopImageHandleList[2];
  EFI_DEVICE_PATH_PROTOCOL       *SelectedVgaDevicePath;

  LockGopImageHandleList[0] = NULL;
  LockGopImageHandleList[1] = NULL;
  //
  // Get VgaDriverPolicy driver by Component name.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverBindingProtocolGuid,
                  NULL,
                  &DriverBindingHandleCount,
                  &DriverBindingHandleBuffer
                  );

  for (Index = 0; Index < DriverBindingHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    DriverBindingHandleBuffer[Index],
                    &gEfiComponentName2ProtocolGuid,
                    (VOID **)&ComponentNameInterface
                    );
    if (!EFI_ERROR(Status)) {
      ComponentNameInterface->GetDriverName (
                                ComponentNameInterface,
                                LANGUAGE_CODE_ENGLISH_RFC4646,
                                &DriverName
                                );
      if (!StrCmp (DriverName, VGA_DRIVER_POLICY_NAME)) {
        LockGopImageHandleList[0] = DriverBindingHandleBuffer[Index];
      }
    }
  }
  //
  // Get VgaDriverPolicy?
  //
  if (LockGopImageHandleList[0] == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Create the volatile variable ACTIVE_VGA_VAR_NAME
  // to notice VgaPolicyDriver to lock the non-Active Vga.
  //
  Status = gBS->HandleProtocol (
                  SelectedVgaHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&SelectedVgaDevicePath
                  );

  BdsLibUpdateConsoleVariable (ACTIVE_VGA_VAR_NAME, SelectedVgaDevicePath, NULL);
  //
  // lock none-primary Vga device.
  //
  if (!EFI_ERROR(Status)) {
    //
    // Add the NULL handle to denote the terminal of driver list.
    //
    LockGopImageHandleList[1] = NULL;
    for (Index = 0; Index < VgaHandleCount; Index++) {
      if (VgaHandleBuffer[Index] != SelectedVgaHandle) {
        Status = gBS->ConnectController (VgaHandleBuffer[Index], LockGopImageHandleList, NULL, FALSE);
      }
    }
  }

  return Status;
}


VOID
EFIAPI
GopHotPlugCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_HANDLE                             *HandleBuffer;
  UINTN                                  NumberOfHandles;
  UINTN                                  Index;
  EFI_DEVICE_PATH_PROTOCOL               *GopDevicePath;
  EFI_STATUS                             Status;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    BdsLibUpdateConsoleVariable (L"ConOut", GopDevicePath, NULL);
  }

  BdsLibConnectConsoleVariable (L"ConOut");

  gBS->FreePool (HandleBuffer);
}


/**
  Find the platform active vga, and base on the policy to enable the vga as the console out device.
  The policy is driven by one setup variable "VBIOS".

  @retval EFI_UNSUPPORTED   There is no active vga device
  @retval EFI_STATUS        Return the status of BdsLibGetVariableAndSize ()

**/
EFI_STATUS
PlatformBdsForceActiveVga (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HANDLE                            *VgaHandleBuffer;
  UINTN                                 VgaHandleCount;
  EFI_DEVICE_PATH_PROTOCOL              *ConOutDevPath;
  EFI_HANDLE                            *SelectedVgaHandle;
  VGA_HANDLES_INFO                      *VgaHandlesInfo;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  CHIPSET_CONFIGURATION                  *SystemConfiguration;
  EFI_EVENT                             ExitBootServicesEvent;
  EFI_EVENT                             LegacyBootEvent;
  EFI_EVENT                             GopHotPlugEvent;
  VOID                                  *Registration;

  SystemConfiguration = NULL;
  VgaHandleBuffer     = NULL;
  VgaHandleCount      = 0;
  ConOutDevPath       = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
//[-start-140619-IB05080432-modify]//
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData;
  } else {
    DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
    DEBUG ((EFI_D_ERROR, "Fail to locate SetupUtilityProtocol!!!\n"));
    goto Exit;
  }
//[-end-140619-IB05080432-modify]//

  Status = GetAllVgaHandles (&VgaHandleBuffer, &VgaHandleCount);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  SelectedVgaHandle = ClassifyVgaHandleAndSelect (
                        VgaHandleBuffer,
                        VgaHandleCount
                        );
  if ((SystemConfiguration->UefiDualVgaControllers == DUAL_VGA_CONTROLLER_ENABLE) &&
      (BdsLibGetBootType ()                        == EFI_BOOT_TYPE)) {
    for (Index = 0; Index < VgaHandleCount; Index++) {
      PreparePciVgaDevicePath (VgaHandleBuffer[Index], &ConOutDevPath);
    }
  } else {
    PreparePciVgaDevicePath (SelectedVgaHandle, &ConOutDevPath);
  }

  //
  // Register Event for Monitor Hot-plug
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  GopHotPlugCallback,
                  NULL,
                  &GopHotPlugEvent
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiGraphicsOutputProtocolGuid,
                    GopHotPlugEvent,
                    &Registration
                    );
  }

  //
  // In Uefi Boot mode and the all the native GOP are connected failed.
  // We rollback the CSM and try to connect the UefiBiosVideo.
  //
  LegacyBiosDependency (NULL);

  if (VgaHandleCount <= 1) {
    goto Exit;
  }

  if (SystemConfiguration->UefiDualVgaControllers != DUAL_VGA_CONTROLLER_ENABLE) {
    LockVgaControllerVgaDriverPolicy (SelectedVgaHandle, VgaHandleBuffer, VgaHandleCount);
  }

  VgaHandlesInfo = AllocateZeroPool (VgaHandleCount * sizeof(VGA_HANDLES_INFO));
  if (VgaHandlesInfo != NULL) {
    VgaHandlesInfo->VgaHandleBuffer = AllocateZeroPool (VgaHandleCount * sizeof(EFI_HANDLE));
    if (VgaHandlesInfo->VgaHandleBuffer == NULL) {
      goto Exit;
    }
  } else {
    goto Exit;
  }

  VgaHandlesInfo->PrimaryVgaHandle = SelectedVgaHandle;
  VgaHandlesInfo->VgaHandleConut   = VgaHandleCount;
  CopyMem (VgaHandlesInfo->VgaHandleBuffer, VgaHandleBuffer, (VgaHandleCount * sizeof(EFI_HANDLE)));

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_NOTIFY,
                  SetVgaCommandRegEvent,
                  VgaHandlesInfo,
                  &ExitBootServicesEvent
                  );
  Status = EfiCreateEventLegacyBootEx (
             TPL_NOTIFY,
             SetVgaCommandRegEvent,
             VgaHandlesInfo,
             &LegacyBootEvent
             );

Exit:
  if (VgaHandleBuffer) {
    gBS->FreePool (VgaHandleBuffer);
  }

  return Status;
}


/**
  Compare two device pathes

  @param  DevicePath1   Input device pathes.
  @param  DevicePath2   Input device pathes.

  @retval TRUE          Same.
  @retval FALSE         Different.

**/
BOOLEAN
CompareDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
  )
{
  UINTN Size1;
  UINTN Size2;

  Size1 = GetDevicePathSize (DevicePath1);
  Size2 = GetDevicePathSize (DevicePath2);

  if (Size1 != Size2) {
    return FALSE;
  }

  if (CompareMem (DevicePath1, DevicePath2, Size1)) {
    return FALSE;
  }

  return TRUE;
}


/**
  Connect the predefined platform default console device. Always try to find
  and enable the vga device if have.

  @param PlatformConsole          Predefined platform default console device array.

  @retval EFI_SUCCESS             Success connect at least one ConIn and ConOut
                                  device, there must have one ConOut device is
                                  active vga device.
  @return Return the status of BdsLibConnectAllDefaultConsoles ()

**/
EFI_STATUS
PlatformBdsConnectConsole (
  IN BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *VarConout;
  EFI_DEVICE_PATH_PROTOCOL           *VarConin;
  EFI_DEVICE_PATH_PROTOCOL           *VarErrout;
  EFI_DEVICE_PATH_PROTOCOL           *NewVarConout;
  EFI_DEVICE_PATH_PROTOCOL           *NewVarConin;
  EFI_DEVICE_PATH_PROTOCOL           *NewVarErrout;
  UINTN                              DevicePathSize;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
  CHIPSET_CONFIGURATION              *SystemConfiguration;
  //-start-131204-IB09740048-add]//
  EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL  *CRBdsHookProtocol;
  EFI_STATUS                                 CRBdsHookProtocolStatus;
  //[-end-131204-IB09740048-add]//
  EFI_DEVICE_PATH_PROTOCOL           *UsbConsoleOuputDevPath = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SystemConfiguration = (CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData;

  Index = 0;
  Status = EFI_SUCCESS;
  DevicePathSize = 0;

  VarConout = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );
  VarConin = BdsLibGetVariableAndSize (
               L"ConIn",
               &gEfiGlobalVariableGuid,
               &DevicePathSize
               );
  VarErrout = BdsLibGetVariableAndSize (
                L"ErrOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );
  if (VarConout == NULL || VarConin == NULL) {
    //
    // Have chance to connect the platform default console,
    // the platform default console is the minimue device group the platform should support
    //
    while (PlatformConsole[Index].DevicePath != NULL) {
      //
      // Update the console variable with the connect type
      //
      if ((PlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
        BdsLibUpdateConsoleVariable (L"ConIn"             , PlatformConsole[Index].DevicePath, NULL);
        BdsLibUpdateConsoleVariable (CON_IN_CANDIDATE_NAME, PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
        BdsLibUpdateConsoleVariable (L"ConOut", PlatformConsole[Index].DevicePath, NULL);
        BdsLibUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
        BdsLibUpdateConsoleVariable (L"ErrOut", PlatformConsole[Index].DevicePath, NULL);
        BdsLibUpdateConsoleVariable (ERR_OUT_CANDIDATE_NAME, PlatformConsole[Index].DevicePath, NULL);
      }

      Index ++;
    }
  }
  if (VarConout) {
    BdsLibUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME, VarConout, NULL);
  }

  if (VarConin) {
    BdsLibUpdateConsoleVariable (CON_IN_CANDIDATE_NAME, VarConin, NULL);
  }

  if (VarErrout) {
    BdsLibUpdateConsoleVariable (ERR_OUT_CANDIDATE_NAME, VarErrout, NULL);
  }

  //
  // If requested via ASF Intel OEM boot parameters, then we connect the SOL driver.
  // ASF Keyboard locking does not apply here
  //
  if (FeaturePcdGet(PcdMe5MbSupported)) {
    if (ActiveManagementEnableSol() == TRUE) {
      BdsLibUpdateConsoleVariable (CON_IN_CANDIDATE_NAME,   (EFI_DEVICE_PATH_PROTOCOL*) &gSerialOverLANDevicePath, NULL);
      BdsLibUpdateConsoleVariable (L"ConInNoLock",          (EFI_DEVICE_PATH_PROTOCOL*) &gSerialOverLANDevicePath, NULL);
      BdsLibUpdateConsoleVariable (CON_OUT_CANDIDATE_NAME,  (EFI_DEVICE_PATH_PROTOCOL*) &gSerialOverLANDevicePath, NULL);
    }
  }

  //
  // Make sure we have at least one active VGA, and have the right active VGA in console variable
  //
  Status = PlatformBdsForceActiveVga ();
  if (VarConout) {
    gBS->FreePool (VarConout);
  }
  VarConout = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );

//[-start-131204-IB09740048-add]//
  if (FeaturePcdGet(PcdH2OConsoleRedirectionSupported)){

    CRBdsHookProtocolStatus = gBS->LocateProtocol (&gCRBdsHookProtocolGuid, NULL, (VOID **)&CRBdsHookProtocol);
    if (CRBdsHookProtocolStatus == EFI_SUCCESS) {
        CRBdsHookProtocol->PlatformBDSConnectCRTerminal (
                           Status,
                           CON_IN_CANDIDATE_NAME,
                           &gEfiGenericVariableGuid,
                           CON_OUT_CANDIDATE_NAME,
                           &gEfiGenericVariableGuid
                         );
    }

  } else {
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
//[-end-131204-IB09740048-add]//

  //
  // Update default device paths with candicate device path
  //
  NewVarConout = BdsLibGetVariableAndSize (
                   CON_OUT_CANDIDATE_NAME,
                   &gEfiGenericVariableGuid,
                   &DevicePathSize
                   );
  ASSERT(NewVarConout != NULL);
  if (!CompareDevicePath(NewVarConout, VarConout)) {
    gRT->SetVariable (
           L"ConOut",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           GetDevicePathSize (NewVarConout),
           NewVarConout
           );
  }
  gBS->FreePool (NewVarConout);
  if (VarConout) {
    gBS->FreePool (VarConout);
  }

  NewVarConin = BdsLibGetVariableAndSize (
                  CON_IN_CANDIDATE_NAME,
                  &gEfiGenericVariableGuid,
                  &DevicePathSize
                  );
  ASSERT(NewVarConin != NULL);
  if (!CompareDevicePath(NewVarConin,VarConin)) {
    gRT->SetVariable (
           L"ConIn",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           GetDevicePathSize (NewVarConin),
           NewVarConin
           );
  }
  gBS->FreePool (NewVarConin);
  if (VarConin) {
    gBS->FreePool (VarConin);
  }

  NewVarErrout = BdsLibGetVariableAndSize (
                   ERR_OUT_CANDIDATE_NAME,
                   &gEfiGenericVariableGuid,
                   &DevicePathSize
                   );
  if (NewVarErrout != NULL) {
    if (!CompareDevicePath(NewVarErrout,VarErrout)) {
      gRT->SetVariable (
             L"ErrOut",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             GetDevicePathSize (NewVarErrout),
             NewVarErrout
             );
    }
    gBS->FreePool (NewVarErrout);
  }
  if (VarErrout != NULL) {
    gBS->FreePool (VarErrout);
  }

  if (!(SystemConfiguration->UsbHotKeySupport == 0 && BdsLibIsWin8FastBootActive ())) {
    BdsLibConnectUsbHID ();
    if (FoundUSBConsoleOutput (&UsbConsoleOuputDevPath)) {
      SetUsbConsoleOutToConOutVar (UsbConsoleOuputDevPath);
      FreePool (UsbConsoleOuputDevPath);
    }
  } else {
    BdsLibConnectUsbHIDNotifyRegister ();
  }
  //
  // Connect the all the default console with current console variable
  //
  Status = BdsLibConnectAllDefaultConsoles ();

  return Status;
}

/**
  Connect with predefined platform connect sequence,
  the OEM/IBV can customize with their own connect sequence.
**/
VOID
PlatformBdsConnectSequence (
  VOID
  )
{
  UINTN                     Index;

  Index = 0;

  //
  // Here we can get the customized platform connect sequence
  // Notes: we can connect with new variable which record the last time boots connect device path sequence
  //
  while (gPlatformConnectSequence[Index] != NULL) {
    //
    // Build the platform boot option
    //
    BdsLibConnectDevicePath (gPlatformConnectSequence[Index]);
    Index ++;
  }

  //
  // For the debug tip, just use the simple policy to connect all devices
  //
  BdsLibConnectAll ();
}

/**
  Load the predefined driver option, OEM/IBV can customize this
  to load their own drivers

  @param BdsDriverLists  - The header of the driver option link list.

**/
VOID
PlatformBdsGetDriverOption (
  IN OUT LIST_ENTRY              *BdsDriverLists
  )
{
  UINTN                              Index;

  Index = 0;

  //
  // Here we can get the customized platform driver option
  //
  while (gPlatformDriverOption[Index] != NULL) {
    //
    // Build the platform boot option
    //
    BdsLibRegisterNewOption (BdsDriverLists, gPlatformDriverOption[Index], NULL, L"DriverOrder", NULL, 0);
    Index ++;
  }
}


/**
  This function is used for some critical time if the the system have no any boot option,
  and there is no time out for user to add the new boot option.
  This can also treat as the platform default boot option.

  @param  BdsBootOptionList   The header of the boot option link list.

**/
VOID
PlatformBdsPredictBootOption (
  IN OUT LIST_ENTRY              *BdsBootOptionList
  )
{
  UINTN                              Index;

  Index = 0;

  //
  // Here give chance to get platform boot option data
  //
  while (gPlatformBootOption[Index] != NULL) {
    //
    // Build the platform boot option
    //
    BdsLibRegisterNewOption (BdsBootOptionList, gPlatformBootOption[Index], NULL, L"BootOrder", "RC", 2);
    Index ++;
  }
}


/**
  Pop up a warning message with text mode.
  This function will switch back to graphic mode after the warning message if it was.

**/
//[-start-130411-IB09720138-modify]//
EFI_STATUS
PopupSpiNotSupport (
  )
{
//   EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
//   EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;
//   EFI_STATUS                        Status;
  UINT32                            TmpAttribute;

//   Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
//   if (EFI_ERROR(Status)) {
//     return Status;
//   }
//
//   Status = ConsoleControl->GetMode (ConsoleControl, &ScreenMode, NULL, NULL);
//   if (EFI_ERROR(Status)) {
//     return Status;
//   }
//
//   if (ScreenMode == EfiConsoleControlScreenGraphics) {
//     ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
//   }

  TmpAttribute = gST->ConOut->Mode->Attribute;

  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_YELLOW);
  gST->ConOut->OutputString (gST->ConOut, L"Warning!!! SPI ROM is not in BIOS SPI support list.\n\rPlease contact your BIOS vendor");

  gBS->Stall (10 * ONE_SECOND);
  gST->ConOut->SetAttribute (gST->ConOut, TmpAttribute);
  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // Restore to Graphic mode if it was
  //
//   if (ScreenMode == EfiConsoleControlScreenGraphics) {
//     ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
//   }

//   return Status;
  return EFI_SUCCESS;
}
//[-end-130411-IB09720138-modify]//


/**
  Set "CurrentFd" variable to record the connected flash device.
  If the PCD "PcdCommonSpiSupport" is TRUE,   Check whether the connected flash device is supported. If not, do COMMON_DISPLAY_SPI_NOT_SUPPORT.

  @param  OemServices   Pointer to OemServices protocol instance

**/
//[-start-130411-IB09720138-modify]//
VOID
UpdateCurrentFdVariable (
  VOID
  )
{
  VOID                    *HobList;
  FD_TABLE_HOB_DATA       *FdTableHobPtr;
  EFI_STATUS              Status;
//   BOOLEAN                 IsUsingDefaultPrompt;
//[start-130916-IB05670200-modify]//
  UINT8                   FdSupportCount;
//[end-130916-IB05670200-modify]//
//   CHIPSET_LIB_SERVICES_PROTOCOL     *ChipsetLibServices;

  HobList = GetHobList ();
  FdTableHobPtr = GetNextGuidHob ((CONST EFI_GUID*) &gFdTableInfoHobGuid, HobList);
//[-start-140619-IB05080432-modify]//
  if ( FdTableHobPtr != NULL ) {
    if (FeaturePcdGet(PcdCommonSpiSupported)) {
//     Status = gBS->LocateProtocol (
//                     &gChipsetLibServicesProtocolGuid,
//                     NULL,
//                     &ChipsetLibServices
//                     );
//     ASSERT_EFI_ERROR (Status);
//
//
//     Status = ChipsetLibServices->GetSpiSupportNumber (&FdSupportCount);
//[start-130916-IB05670200-modify]//
      Status = GetPlatformSpiFlashNumber (&FdSupportCount);
      ASSERT_EFI_ERROR (Status);
//[end-130916-IB05670200-modify]//

      if (FdTableHobPtr->CurrentFdIndex == FdSupportCount - 1) {
        //
        // The value of FdSupportCount-1 means common Spi table used, do COMMON_DISPLAY_SPI_NOT_SUPPORT.
        //
        //
//       // This flag is for Project/Platform to decide whether to use the default prompt(PopupSpiNotSupport).
//       // If Project/Platform doesn't want the default prompt to be displayed, modify IsUsingDefaultPrompt to FALSE in COMMON_DISPLAY_SPI_NOT_SUPPORT OemService.
//       // (OemServices)
//       //
//     IsUsingDefaultPrompt = TRUE;
//     Status = DisplaySpiNotSupport (
//                &IsUsingDefaultPrompt
//                );
//     if (IsUsingDefaultPrompt) {
//       Status = PopupSpiNotSupport ();
//       ASSERT_EFI_ERROR (Status);
//     }
  
        //
        // This PCD is for Project/Platform to decide whether to use the default prompt(PopupSpiNotSupport).
        // If Project/Platform doesn't want the default prompt to be displayed, modify PcdDisplaySpiNotSupport to FALSE.
        //
        if (FeaturePcdGet(PcdDisplaySpiNotSupport)) {
          Status = PopupSpiNotSupport ();
          ASSERT_EFI_ERROR (Status);
        }
  
        return;
      }
    } //FeaturePcdGet(PcdCommonSpiSupported)

    Status = gRT->SetVariable (
                    L"CurrentFD",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (UINT8),
                    &FdTableHobPtr->CurrentFdIndex
                    );
    ASSERT_EFI_ERROR (Status);
  } else {  
    DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
    DEBUG ((EFI_D_ERROR, "Fail to get FdTableInfo hob!!!\n"));
    ASSERT (FdTableHobPtr != NULL);
  }
//[-end-140619-IB05080432-modify]//
}
//[-end-130411-IB09720138-modify]//


/**
  Perform the platform diagnostic, such like test memory. OEM/IBV also
  can customize this function to support specific platform diagnostic.

  @param MemoryTestLevel  The memory test intensive level
  @param QuietBoot        Indicate if need to enable the quiet boot
  @param BaseMemoryTest   A pointer to BdsMemoryTest()

**/
VOID
PlatformBdsDiagnostics (
  IN EXTENDMEM_COVERAGE_LEVEL    MemoryTestLevel,
  IN BOOLEAN                     QuietBoot,
  IN BASEM_MEMORY_TEST           BaseMemoryTest
  )
{
  EFI_STATUS                        Status;
  UINT8                             PrintLine = 0;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  UINT16                            FunctionKey;
  BOOLEAN                           KeyPressed;
  BOOLEAN                           EnableQuietBootState;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;

  //
  // PostCode = 0x1D, Display logo or system information
  //
  POST_CODE (BDS_DISPLAY_LOGO_SYSTEM_INFO);

  BdsLibOnStartOfBdsDiagnostics ();

  UpdateCurrentFdVariable ();

  //
  // OemServices
  //
  Status = OemSvcDisplayLogo (
             &QuietBoot
             );

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    ConsoleControl = NULL;
  }

  //
  // Here we can decide if we need to show the diagnostics screen
  // Notes: this quiet boot code should be remove from the graphic lib
  //
  BdsLibGetQuietBootState (&EnableQuietBootState);
  if (QuietBoot && EnableQuietBootState) {
    EnableQuietBoot (&gEfiUgaSplashProtocolGuid);
  }

  BdsLibSetHotKeyDelayTime ();

  //
  // Perform system diagnostic
  //
  if (!QuietBoot) {
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  BdsLibGetHotKey (&FunctionKey, &KeyPressed);
  Status = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (!EFI_ERROR (Status)) {
    if (KeyPressed) {
      if (ConsoleControl != NULL) {
        ShowOemString (Badging, TRUE, (UINT8) FunctionKey);
        BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
      } else {
        if (QuietBoot) {
          ShowOemString (Badging, TRUE, (UINT8) FunctionKey);
        } else {
          BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
        }
      }
    }
  }

  //
  // Perform system diagnostic
  //
  if (!QuietBoot) {
    gST->ConOut->ClearScreen (gST->ConOut);
//[-start-140325-IB13080003-modify]//
    if (KeyPressed) {
      Status = BdsLibShowOemStringInTextMode (TRUE, (UINT8)FunctionKey);
    } else {
      Status = BdsLibShowOemStringInTextMode (FALSE, 0);
    }
//[-end-140325-IB13080003-modify]//

    gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
    Status = ShowSystemInfo (&PrintLine);
  }

  Status = BaseMemoryTest (MemoryTestLevel, (UINTN) (PrintLine + 2));
  if (EFI_ERROR (Status)) {
    DisableQuietBoot ();
  }

  return;
}


/**
  Perform the platform diagnostic, such like test memory. OEM/IBV also
  can customize this fuction to support specific platform diagnostic.

  @param  LogoDisplay      Indicate if need to enable logo display
  @param  HotKeyService    Indicate if need to enable hotkey service
  @param  QuietBoot        Indicate if need to enable the quiet boot
  @param  BaseMemoryTest   The memory test function pointer

**/
VOID
PlatformBdsDiagnosticsMini (
  IN BOOLEAN                     LogoDisplay,
  IN BOOLEAN                     HotKeyService,
  IN BOOLEAN                     QuietBoot,
  IN BASEM_MEMORY_TEST           BaseMemoryTest
  )
{
  EFI_STATUS                        Status;
  UINT8                             PrintLine = 0;
  BOOLEAN                           EnableQuietBootState;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  UINT16                            FunctionKey;
  BOOLEAN                           KeyPressed;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;

  if (LogoDisplay) {
    //
    // PostCode = 0x1D, Display logo or system information
    //
    POST_CODE (BDS_DISPLAY_LOGO_SYSTEM_INFO);

    BdsLibOnStartOfBdsDiagnostics ();

    UpdateCurrentFdVariable ();

    //
    // OemServices
    //
    Status = OemSvcDisplayLogo (
               &LogoDisplay
               );

   //
   // Here we can decide if we need to show
   // the diagnostics screen
   // Notes: this quiet boot code should be remove
   // from the graphic lib
   //
    BdsLibGetQuietBootState (&EnableQuietBootState);
    if (EnableQuietBootState) {
      EnableQuietBoot (&gEfiUgaSplashProtocolGuid);
    }
  }

  if (HotKeyService) {
    Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
    if (EFI_ERROR (Status)) {
      ConsoleControl = NULL;
    }

    BdsLibGetHotKey (&FunctionKey, &KeyPressed);
    Status = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
    if (!EFI_ERROR (Status)){
      if (KeyPressed) {
        if (ConsoleControl != NULL) {
          ShowOemString(Badging, TRUE, (UINT8) FunctionKey);
          BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
        } else {
          if (QuietBoot) {
            ShowOemString(Badging, TRUE, (UINT8) FunctionKey);
          } else {
            BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
          }
        }
      }
    }
  }

  Status = BaseMemoryTest (IGNORE, (UINTN)(PrintLine + 2));
  if (EFI_ERROR (Status)) {
    DisableQuietBoot ();
  }

  return;
}


/**
  If the bootable device is legacy OS, delete BootPrevious Variable.

  @param  Event               The event that triggered this notification function
  @param  ParentImageHandle   Pointer to the notification functions context

  @retval EFI_STATUS    Success to delete BootPrevious Variable.

**/
EFI_STATUS
BdsLegacyBootEvent (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
  UINTN               BootPreviousSize;
  UINT16              *BootPrevious;

  BootPrevious = BdsLibGetVariableAndSize (
                   L"BootPrevious",
                   &gEfiGenericVariableGuid,
                   &BootPreviousSize
                   );
  if (BootPrevious != NULL) {
    //
    // OS has been changed from UEFI to Legacy
    //
    gBS->FreePool (BootPrevious);

    BootPrevious = NULL;
    gRT->SetVariable (
           L"BootPrevious",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           0,
           BootPrevious
           );
  }

  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

/**
  The function will execute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.

  @param  DriverOptionList        The header of the driver option link list
  @param  BootOptionList          The header of the boot option link list
  @param  ProcessCapsules         A pointer to ProcessCapsules()
  @param  BaseMemoryTest          A pointer to BaseMemoryTest()

**/
VOID
EFIAPI
PlatformBdsPolicyBehavior (
  IN LIST_ENTRY                      *DriverOptionList,
  IN LIST_ENTRY                      *BootOptionList,
  IN PROCESS_CAPSULES                ProcessCapsules,
  IN BASEM_MEMORY_TEST               BaseMemoryTest
  )
{
  EFI_STATUS                         Status;
  UINT16                             Timeout;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
  CHIPSET_CONFIGURATION               *SystemConfiguration;
  EXTENDMEM_COVERAGE_LEVEL           MemoryTestLevel;
  HOT_KEY_CONTEXT                    *HotKeyContext;
  UINT16                             FunctionKey;
  BOOLEAN                            EnterSetup;
  EFI_EVENT                          ReadyToBootEvent;
  EFI_DEVICE_PATH_PROTOCOL           *PlatformConnectLastBoot;
  UINT16                             *BootPrevious;
  EFI_HANDLE                         Handle;
  EFI_STATUS                         LocateDevicePathStatus;
  BOOLEAN                            HotKeyPressed;
  EFI_EVENT                          LegacyBootEvent;
  UINTN                              Size;
  EFI_CONSOLE_CONTROL_PROTOCOL       *ConsoleControl;
  UINT64                             OsIndications;
  UINT64                             OsIndicationsSupported;
  EFI_BOOT_MODE                      BootMode;
  UINT32                             Int1cHook;
  UINT32                             Int1cBeforeHook;
  UINTN                              Int1cAddress;
//[-start-140416-IB09740077-add]//
  CR_SRV_MANAGER_PROTOCOL            *CrSrvManager;
//[-end-140416-IB09740077-add]//
//[-start-140515-IB10300110-add]//
  UINT8                              WatchDogAction = 0;
  UINT8                              Data8[3]={0};
  UINT8                              Message1;
  UINT8                              Message2;
  EVENT_LOG_PROTOCOL                 *EventHandler;
  EVENT_TYPE_ID                      EventTypeId;
//[-end-140515-IB10300110-add]//
  
  ConsoleControl = NULL;
  SystemConfiguration = NULL;
  PlatformConnectLastBoot = NULL;
  BootMode = GetBootModeHob ();
  Int1cAddress = 0x1c * 4;
  Int1cHook = 0;

//[-start-130516-12360009-add]//
#ifdef TXT_SUPPORT
  //
  // Copy ACPI Root Pointer to CSM E/F segment, this is required by ACM.
  //
  Status = PrepareAcpiTableforTXT ();
  ASSERT_EFI_ERROR (Status);
#endif
//[-end-130516-12360009-add]//

  //
  // Get current Boot Mode
  //
  if (IsAdministerSecureBootSupport ()) {
    Status = PlatformBdsConnectConsole (gPlatformConsole);
    PlatformBdsBootDisplayDevice (SystemConfiguration);
    PlatformBdsConnectSequence ();
    BdsLibEnumerateAllBootOption (TRUE, BootOptionList);
    DisableQuietBoot ();
    CallSecureBootMgr ();
    //
    // Only assert here since this is the right behavior, system will be reset after user finishes secure boot manger.
    //
    ASSERT (FALSE);
  }


  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  SystemConfiguration = (CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData;

  Size = sizeof(OsIndicationsSupported);
  Status = gRT->GetVariable (L"OsIndicationsSupported", &gEfiGlobalVariableGuid, NULL, &Size, &OsIndicationsSupported);
  if (EFI_ERROR (Status)) {
    OsIndicationsSupported = 0;
  }

  Size = sizeof(OsIndications);
  Status = gRT->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, NULL, &Size, &OsIndications);
  if (EFI_ERROR (Status)) {
    OsIndications = 0;
  }

  HotKeyContext = NULL;
//[-start-140620-IB05080432-modify]//
  if (!(OsIndicationsSupported & OsIndications & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) &&
      !(BootMode == BOOT_ON_S4_RESUME && BdsLibGetBootType () != EFI_BOOT_TYPE)) {
    HotKeyContext = AllocateZeroPool (sizeof (HOT_KEY_CONTEXT));
    if (HotKeyContext != NULL) {
      HotKeyContext->EnableQuietBootPolicy = (BOOLEAN) SystemConfiguration->QuietBoot;
      HotKeyContext->CanShowString         = FALSE;
  
      //
      // OemServices
      //
      Status = OemSvcDisplayLogo (
                 &HotKeyContext->EnableQuietBootPolicy
                 );
  
      POST_CODE (BDS_INSTALL_HOTKEY);
      BdsLibInstallHotKeys (HotKeyContext);
    } else {
      DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
      DEBUG ((EFI_D_ERROR, "Fail to locate memory for HotKeyContext, Skip installing HotKeys\n"));
    }
  }
//[-end-140620-IB05080432-modify]//

  FunctionKey = NO_OPERATION;
  if (BootMode == BOOT_ASSUMING_NO_CONFIGURATION_CHANGES ||
      BootMode == BOOT_ON_S4_RESUME) {
    BootPrevious = BdsLibGetVariableAndSize (
                     L"BootPrevious",
                     &gEfiGenericVariableGuid,
                     &Size
                     );
    if (BootPrevious == NULL || Size == 0) {
      //
      // Cannot find BootPrevious variable, boot with full configuration.
      //
      BootMode = BOOT_WITH_FULL_CONFIGURATION;
    }
  } else {
    //
    // Clear the BootPrevious variable
    //
    BootPrevious = NULL;
    gRT->SetVariable (
           L"BootPrevious",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           0,
           BootPrevious
           );
  }

//[-start-140512-IB10300110-add]//
  if (FeaturePcdGet (PcdH2OWatchDogSupported)) {

    WatchDogAction = (SystemConfiguration->WDTOsLoad) << 2;
    WatchDogAction |= (SystemConfiguration->WDTOpromScan) << 1;
    WatchDogAction |= SystemConfiguration->BiosWDT;
    
    PcdSet8 (PcdH2OWatchDogMask, WatchDogAction);

    WatchDogMessageGet (&Message1, &Message2);

    if (Message1 == WATCH_DOG_ERROR) {
      Status = gBS->LocateProtocol (&gH2OEventLogProtocolGuid, NULL, (VOID **)&EventHandler);
      if (!EFI_ERROR (Status)) {
        //
        // Arrange data and log it.
        //
        EventTypeId.SensorType = 0x11;
        EventTypeId.SensorNum  = 0x00;
        EventTypeId.EventType  = 0x6F;
        Data8[0] = 0xE0;
        Data8[1] = 0xFF;
        Data8[2] = Message2;
        
        EventHandler->LogEvent (EVENT_STORAGE_ALL, &EventTypeId, Data8, 3);  
        WatchDogMessageClear ();

      }
    }
  }
//[-end-140512-IB10300110-add]//

  //
  // Create a Legacy Boot Event to delete variable, if the bootable device is legacy OS.
  //
  Status = EfiCreateEventLegacyBootEx (
             TPL_CALLBACK,
             BdsLegacyBootEvent,
             NULL,
             &LegacyBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  //
  // PostCode = 0x12, ASF Initial
  //
  POST_CODE (BDS_ASF_INIT);

  //
  // Get the ASF boot options that were discovered during ASF initialization.
  //
  if (FeaturePcdGet(PcdMe5MbSupported)) {
    Status = BdsAsfInitialization();
  }

  EnterSetup = FALSE;

  //
  // Init the time out value
  //
  if (FeaturePcdGet(PcdMe5MbSupported)) {
    if (ActiveManagementPauseBoot() == TRUE) {
      Timeout = 0xFFFF; // wait indefinetly at splash screen
    } else {
      Size = sizeof (Timeout);
      Status = gRT->GetVariable (L"Timeout", &gEfiGlobalVariableGuid, NULL, &Size, &Timeout);
    }
  } else {
    Size = sizeof (Timeout);
    Status = gRT->GetVariable (L"Timeout", &gEfiGlobalVariableGuid, NULL, &Size, &Timeout);
  }

  if (FeaturePcdGet(PcdMe5MbSupported)) {
    if (BootMode == BOOT_ASSUMING_NO_CONFIGURATION_CHANGES ||
        BootMode == BOOT_ON_S4_RESUME) {
      if (ActiveManagementEnterSetup() ||
          ActiveManagementEnableIdeR() ||
          ActiveManagementEnableKvm() ||
          ActiveManagementEnableSol() ||
          BdsCheckAsfBootCmd()) {
        //
        // If any of ME remote feature is enabled, force to change boot mode to BOOT_WITH_FULL_CONFIGURATION.
        //
        BootMode = BOOT_WITH_FULL_CONFIGURATION;
      }
    }
  }

  //
  // Load the driver option as the driver option list
  //
  PlatformBdsGetDriverOption (DriverOptionList);

  //
  // Clear the capsule variable as early as possible
  // which will avoid the next time boot after the capsule update will still into the capsule loop
  //
  gRT->SetVariable (
         L"CapsuleUpdateData",
         &gEfiCapsuleVendorGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         0,
         (VOID *) NULL
         );

  //
  // Set memory test level by platform requirement
  //
  MemoryTestLevel = (SystemConfiguration->QuickBoot == TRUE) ? IGNORE : EXTENSIVE;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ChipsetPrepareReadyToBootEvent,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Go the different platform policy with different boot mode
  // Notes: this part code can be change with the table policy
  //
  switch (BootMode) {

  case BOOT_WITH_MINIMAL_CONFIGURATION:
    //
    // This is sample code for BOOT_WITH_MINIMAL_CONFIGURATION.
    //
    PlatformBdsConnectConsoleMini (gPlatformConsole);
    PlatformBdsDiagnosticsMini (TRUE, TRUE, SystemConfiguration->QuietBoot,BaseMemoryTest);
    PlatformBdsConnectDevicePath (gPlatformConnectSata[0], TRUE, TRUE);
    break;

  case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
    //
    // Connect default console devices
    //
    Status = PlatformBdsConnectConsoleMini (gPlatformConsole);
    if (EFI_ERROR (Status)) {
      //
      // ConOut connect fail, force to boot with full configuration
      //
      goto FULL_CONFIGURATION;
    }

    PlatformBdsDiagnosticsMini (TRUE, TRUE, SystemConfiguration->QuietBoot,BaseMemoryTest);

    //
    // Update gPlatformConnectLastBoot via resotring saved device path
    // Its device path type should be other than BBS_DEVICE_PATH after transform
    //
    UpdateLastBootDevicePath (BootPrevious, &PlatformConnectLastBoot);

    //
    // Connect last boot device path
    //
    PlatformBdsConnectDevicePath (PlatformConnectLastBoot, TRUE, TRUE);
    LocateDevicePathStatus = gBS->LocateDevicePath (
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    &PlatformConnectLastBoot,
                                    &Handle
                                    );
    if (!EFI_ERROR (LocateDevicePathStatus)) {
      BdsLibGetHotKey (&FunctionKey, &HotKeyPressed);
      if (!HotKeyPressed) {
        Status = PlatformBdsBootSelection (FunctionKey, 0);
        if (FeaturePcdGet(PcdMe5MbSupported)) {
        //
        // Parse ASF Boot options and execute any remote control boot
        // If we return, then there was no ASF boot option, or the option was not available on this platform
        //
          Status = BdsBootViaAsf();
        }
        break;
      } else {
        //
        // Hot has been pressed, boot with full configuration
        //
        goto FULL_CONFIGURATION;
      }
    } else {
      goto FULL_CONFIGURATION;
    }

  case BOOT_ON_FLASH_UPDATE:
    //
    // Boot with the specific configuration
    //
    PlatformBdsConnectConsole (gPlatformConsole);
    if (HotKeyContext != NULL) {
      HotKeyContext->CanShowString = TRUE;
    }
    PlatformBdsDiagnostics (EXTENSIVE, FALSE, BaseMemoryTest);
    BdsLibConnectAll ();
    Status = ShowAllDevice ();
    ProcessCapsules (BOOT_ON_FLASH_UPDATE);
    break;

  case BOOT_IN_RECOVERY_MODE:
    //
    // In recovery mode, just connect platform console and show up the front page
    //
    PlatformBdsConnectConsole (gPlatformConsole);
    if (HotKeyContext != NULL) {
      HotKeyContext->CanShowString = TRUE;
    }

    BdsLibConnectAll ();

//[-start-130822-IB10310032-modify]//
    //
    // BIOS Region will be protected after publish ExitPmAuthProtocol
    // Bring fast recovery forward to here.
    //
    if (FeaturePcdGet(PcdUseFastCrisisRecovery)) {
    //
    // PostCode = 0x35, Fast recovery start flash
    //
    POST_CODE (BDS_RECOVERY_START_FLASH);
      RecoveryPopUp (DEFAULT_FLASH_DEVICE_TYPE);
    }

    //
    // Recovery don't support HotKey, goto Boot Manager
    //
    BdsLibEnumerateAllBootOption (TRUE, BootOptionList);
    PlatformBdsBootSelection(FunctionKey, Timeout);
    BdsLibStartSetupUtility (TRUE);
//[-end-130822-IB10310032-modify]//

    break;

FULL_CONFIGURATION:
  case BOOT_ON_S4_RESUME:
  case BOOT_WITH_FULL_CONFIGURATION:
  case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
  case BOOT_WITH_DEFAULT_SETTINGS:
  default:
    Status = PlatformBdsConnectConsole (gPlatformConsole);
    if (EFI_ERROR (Status)) {
      //
      // Here OEM/IBV can customize with defined action
      //
      PlatformBdsNoConsoleAction ();
    }

#ifdef ANTI_THEFT_SUPPORT
    PlatformAlertAT();
#endif

    if (HotKeyContext != NULL) {
      HotKeyContext->CanShowString = TRUE;
    }

    PlatformBdsBootDisplayDevice (SystemConfiguration);
    PlatformBdsDiagnostics (MemoryTestLevel, SystemConfiguration->QuietBoot, BaseMemoryTest);

//[-start-130611-IB11120016-add]//
    EfiNamedEventSignal (&gStartOfShadowRomProtocolGuid);
//[-end-130611-IB11120016-add]//

    //
    // Perform some platform specific connect sequence
    //
    BdsLibConnectLegacyRoms ();

//[-start-130611-IB11120016-add]//
    EfiNamedEventSignal (&gEndOfShadowRomProtocolGuid);
//[-end-130611-IB11120016-add]//

    if (FeaturePcdGet(PcdMe5MbSupported)) {
      Int1cBeforeHook = *(volatile UINT32*) Int1cAddress;
      InstallSOLOptionROM ();
      if (ActiveManagementEnableSol()) {
        Int1cHook = *(volatile UINT32*) Int1cAddress;
        *(volatile UINT32*)Int1cAddress = Int1cBeforeHook;
      }
    }
    if (BdsLibIsWin8FastBootActive ()) {
      BdsLibConnectTargetDev ();
    } else {
      PlatformBdsConnectSequence ();
    }

    if (!(SystemConfiguration->QuietBoot)) {
      Status = ShowAllDevice();
    }

    //
    // Here we have enough time to do the enumeration of boot device
    //
    BdsLibEnumerateAllBootOption (FALSE, BootOptionList);

    if (FeaturePcdGet(PcdMe5MbSupported)) {
      if (ActiveManagementEnableSol()) {
        DisableQuietBoot ();
        gST->ConOut->EnableCursor (gST->ConOut, TRUE);
        Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
        if (!EFI_ERROR (Status)) {
          ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
        }
        //
        // Force to set mode 0 (80*25), due to this mode is set to default value by AMT testing tool.
        //
        gST->ConOut->SetMode (gST->ConOut, 0);
      }
    }

    //
    // If Resume From S4 BootMode is set become BOOT_ON_S4_RESUME
    //
    // move the check boot mode is whether BOOT_ON_S4_RESUME to PlatformBdsBootSelection ()
    // to make sure the memory mapping is the same between normal boot and S4
    //
    PlatformBdsBootSelection (FunctionKey, Timeout);

//[-start-140416-IB09740077-add]//
    //
    // Callback for CrSrvManager BDS function after Hotkey monitor.
    //
    CrSrvManager = NULL;
    Status = gBS->LocateProtocol (&gCrSrvManagerProtocolGuid, NULL, (VOID **)&CrSrvManager);
    if (Status == EFI_SUCCESS) {
      CrSrvManager->CrSrvBdsCallback ();
    }
 //[-end-140416-IB09740077-add]//

    //
    // Parse ASF Boot options and execute any remote control boot
    // If we return, then there was no ASF boot option, or the option was not available on this platform
    //
    if (FeaturePcdGet(PcdMe5MbSupported)) {
      if (ActiveManagementEnableSol()) {
        *(volatile UINT32*)Int1cAddress = Int1cHook;
      }
      Status = BdsBootViaAsf();
    }

    //
    // In default boot mode, always find all boot option and do enumerate all the default boot option
    //
    if (Timeout == 0) {
      BdsLibBuildOptionFromVar (BootOptionList, L"BootOrder");
      if (IsListEmpty(BootOptionList)) {
        PlatformBdsPredictBootOption (BootOptionList);
      }
    }

    break;
  }

  ExitBootServiceSetVgaMode (SystemConfiguration, BootMode);

  return;
}

/**
  Hook point after a boot attempt succeeds. We don't expect a boot option to
  return, so the UEFI 2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in this case. This
  is also a platform implementation and can be customized by IBV/OEM.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
PlatformBdsBootSuccess (
  IN  BDS_COMMON_OPTION *Option
  )
{
  CHAR16  *TmpStr;

  //
  // If Boot returned with EFI_SUCCESS and there is not in the boot device
  // select loop then we need to pop up a UI and wait for user input.
  //
  TmpStr = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_SUCCEEDED));
  if (TmpStr != NULL) {
    BdsLibOutputStrings (gST->ConOut, TmpStr, Option->Description, L"\n\r", NULL);
    gBS->FreePool(TmpStr);
  }
}

/**
  Hook point after a boot attempt fails.

  @param  Option                  Pointer to Boot Option that failed to boot.
  @param  Status                  Status returned from failed boot.
  @param  ExitData                Exit data returned from failed boot.
  @param  ExitDataSize            Exit data size returned from failed boot.

**/
VOID
EFIAPI
PlatformBdsBootFail (
  IN  BDS_COMMON_OPTION  *Option,
  IN  EFI_STATUS         Status,
  IN  CHAR16             *ExitData,
  IN  UINTN              ExitDataSize
  )
{
  CHAR16                           *BootErrorString;
  CHAR16                           *PrintString;
  H2O_DIALOG_PROTOCOL              *H2ODialog;
  EFI_INPUT_KEY                    Key;
  EFI_STATUS                       LocateStatus;

  LocateStatus = gBS->LocateProtocol (
                        &gH2ODialogProtocolGuid,
                        NULL,
                        (VOID **)&H2ODialog
                        );
  ASSERT_EFI_ERROR (LocateStatus);
  if (EFI_ERROR (LocateStatus)) {
    return;
  }

//[-start-140619-IB05080432-add]//
  BootErrorString = NULL;
//[-end-140619-IB05080432-add]//
  PrintString = NULL;
//[-start-140619-IB05080432-modify]//
  if (Status == EFI_SECURITY_VIOLATION) {
    //
    // Generate output string for boot failed caused by security violation
    //
    if (Option->Description != NULL) {
      BootErrorString = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_FAILED_BY_SECURITY));
      if (BootErrorString != NULL) {
        PrintString = AllocateZeroPool (StrSize (BootErrorString) + StrSize (Option->Description) - sizeof (CHAR16));
        if (PrintString != NULL) {
          StrCpy (PrintString, Option->Description);
          StrCat (PrintString, BootErrorString);
        } else {
          DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
          DEBUG ((EFI_D_ERROR, "Fail to locate memory for PrintString!!!\n"));
        }
      } else {
        DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
        DEBUG ((EFI_D_ERROR, "Fail to get BootError string!!!\n"));
        ASSERT (BootErrorString != NULL);
      }
    } else {
      PrintString = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_FILE_FAILED_BY_SECURITY));
      if (PrintString == NULL) {
        DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
        DEBUG ((EFI_D_ERROR, "Fail to locate memory for PrintString!!!\n"));
        ASSERT (PrintString != NULL);
      }
    }
  } else {
    //
    // Generate common boot failed string for other reason.
    //
    if (Option->Description != NULL) {
      BootErrorString = PlatformBdsGetStringById (STRING_TOKEN (STR_COMMON_BOOT_FAILED));
      if (BootErrorString != NULL) {
        PrintString = AllocateZeroPool (StrSize (BootErrorString) + StrSize (Option->Description) - sizeof (CHAR16));
        if (PrintString != NULL) {
          StrCpy (PrintString, Option->Description);
          StrCat (PrintString, BootErrorString);
        } else {
          DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
          DEBUG ((EFI_D_ERROR, "Fail to locate memory for PrintString!!!\n"));
        }
      } else {
        DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
        DEBUG ((EFI_D_ERROR, "Fail to get BootError string!!!\n"));
        ASSERT (BootErrorString != NULL);
      }
    } else {
      PrintString = PlatformBdsGetStringById (STRING_TOKEN (STR_COMMON_BOOT_FILE_FAILED));
      if (PrintString == NULL) {
        DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
        DEBUG ((EFI_D_ERROR, "Fail to locate memory for PrintString!!!\n"));
        ASSERT (PrintString != NULL);
      }
    }
  }
//[-end-140619-IB05080432-modify]//

  DisableQuietBoot ();
  gST->ConOut->ClearScreen (gST->ConOut);
//[-start-140619-IB05080432-modify]//
  if (PrintString != NULL) {
    H2ODialog->ConfirmDialog (
                      DlgOk,
                      FALSE,
                      0,
                      NULL,
                      &Key,
                      PrintString
                      );
  }
//[-end-140619-IB05080432-modify]//
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);

//[-start-140619-IB05080432-modify]//
  if (BootErrorString != NULL)
    gBS->FreePool (BootErrorString);
  
  if (PrintString != NULL)
    gBS->FreePool (PrintString);
//[-end-140619-IB05080432-modify]//

  return;
}


/**
  This function will connect console device base on the console
  device variable ConOut.

  @retval EFI_SUCCESS    At least one of the ConIn and ConOut device have
                         been connected success.
  @retval Other          Return the status of BdsLibConnectConsoleVariable ().

**/
EFI_STATUS
BdsConnectMiniConsoles (
  VOID
  )
{
  EFI_STATUS                Status;

#ifdef Q2LSERVICE_SUPPORT
  //
  // PostCode = 0x17, Video device initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_OUT);
  Status = BdsLibConnectConsoleVariable (L"ConOut");
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // PostCode = 0x16, Keyboard Controller, Keyboard and Moust initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_IN);
  Status = BdsLibConnectConsoleVariable (L"ConIn");
  if (EFI_ERROR (Status)) {
    return Status;
  }
#else
  //
  // PostCode = 0x16, Keyboard Controller, Keyboard and Moust initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_IN);
  //
  // Because possibly the platform is legacy free, in such case,
  // ConIn devices (Serial Port and PS2 Keyboard ) does not exist,
  // so we need not check the status.
  //
  //
  // PostCode = 0x17, Video device initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_OUT);

  Status = BdsLibConnectConsoleVariable (L"ConOut");
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif

  //
  // Special treat the err out device, becaues the null
  // err out var is legal.
  //
  return EFI_SUCCESS;
}


/**
  Connect the predefined platform default console device. Always try to find
  and enable the vga device if have.

  @param  PlatformConsole   Predfined platform default console device array.

  @retval EFI_STATUS    Success connect at least one ConIn and ConOut
                        device, there must have one ConOut device is
                        active vga device.
  @retval Other         Return the status of BdsLibConnectAllDefaultConsoles ()

**/
EFI_STATUS
PlatformBdsConnectConsoleMini (
  IN  BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *VarConout;
  EFI_DEVICE_PATH_PROTOCOL           *VarConin;
  UINTN                              DevicePathSize;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
  CHIPSET_CONFIGURATION               *SystemConfiguration;
  EFI_DEVICE_PATH_PROTOCOL           *UsbConsoleOuputDevPath = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;

  //
  // Connect RootBridge
  //
  Status = PlatformBdsConnectDevicePath (gPlatformRootBridges[0], FALSE, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarConout = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );

  VarConin  = BdsLibGetVariableAndSize (
                L"ConIn",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );

  if (VarConout == NULL || VarConin == NULL) {
    //
    // Have chance to connect the platform default console,
    // the platform default console is the minimue device group
    // the platform should support
    //
    for (Index = 0; PlatformConsole[Index].DevicePath != NULL; ++Index) {
      //
      // Update the console variable with the connect type
      //
      if ((PlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
        BdsLibUpdateConsoleVariable (L"ConIn", PlatformConsole[Index].DevicePath, NULL);
      }
      if ((PlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
        BdsLibUpdateConsoleVariable (L"ConOut", PlatformConsole[Index].DevicePath, NULL);
      }
      if ((PlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
        BdsLibUpdateConsoleVariable (L"ErrOut", PlatformConsole[Index].DevicePath, NULL);
      }
    }
  }

  if (!(SystemConfiguration->UsbHotKeySupport == 0 && BdsLibIsWin8FastBootActive ())) {
    BdsLibConnectUsbHID ();
    if (FoundUSBConsoleOutput (&UsbConsoleOuputDevPath)) {
      SetUsbConsoleOutToConOutVar (UsbConsoleOuputDevPath);
      FreePool (UsbConsoleOuputDevPath);
    }
  } else {
    BdsLibConnectUsbHIDNotifyRegister ();
  }
  //
  // Connect the all the default console with current cosole variable
  //
  Status = BdsConnectMiniConsoles ();

  return Status;
}


EFI_STATUS
PlatformBdsConnectDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL    *HandleDevicePath,
  IN  BOOLEAN                     ConnectChildHandle,
  IN  BOOLEAN                     DispatchPossibleChild
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;

  BdsLibConnectDevicePath (HandleDevicePath);

  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &HandleDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  do {
    gBS->ConnectController (Handle, NULL, NULL, ConnectChildHandle);

    //
    // Check to see if it's possible to dispatch an more DXE drivers
    //
    if (DispatchPossibleChild) {
      Status = gDS->Dispatch ();
    }
  } while (DispatchPossibleChild && !EFI_ERROR (Status));

  return EFI_SUCCESS;
}


/**
  This function is remained for IBV/OEM to do some platform action,
  if there no console device can be connected.

  @return EFI_SUCCESS      Direct return success now.

**/
EFI_STATUS
PlatformBdsNoConsoleAction (
  VOID
  )
{
  //
  // PostCode = 0x1C, Console device initial fail
  //
  POST_CODE (BDS_NO_CONSOLE_ACTION);

  return EFI_SUCCESS;
}


/**
  This function locks platform flash that is not allowed to be updated during normal boot path.
  The flash layout is platform specific.
**/
VOID
EFIAPI
PlatformBdsLockNonUpdatableFlash (
  VOID
  )
{
  return;
}

/**
  Perform no boot device operation (show message and wait for user to restart system)

**/
VOID
PlatformBdsNoBootDevice (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_INPUT_KEY                      Key;
  CHAR16                             *BootErrorString = NULL;
  UINTN                              StringLen = 0;
  EFI_KEY_DATA                       KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx;

  SimpleTextInEx = NULL;
  gBS->HandleProtocol (
         gST->ConsoleInHandle,
         &gEfiSimpleTextInputExProtocolGuid,
         (VOID **)&SimpleTextInEx
         );

  //
  // PostCode = 0xF9, No Boot Device
  //
  POST_CODE (POST_BDS_NO_BOOT_DEVICE);

  BootErrorString = PlatformBdsGetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_NO_BOOT_DEVICE));
  if (BootErrorString != NULL) {
    AutoFailoverPolicyBehavior (FALSE, BootErrorString);
    gBS->FreePool(BootErrorString);
  }

  DisableQuietBoot ();

  BootErrorString = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_DEVICE_ERROR_MESSAGE));
//[-start-140623-IB05080432-modify]//
  if (BootErrorString != NULL) {
    StringLen = StrLen (BootErrorString);
  }
//[-end-140623-IB05080432-modify]//
  if (BootErrorString == NULL || StringLen == 0) {
    BootErrorString = L"No bootable device -- Please restart system\n";
  }

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  Print (BootErrorString);

  while (TRUE) {
    if (SimpleTextInEx != NULL) {
      Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
      Key = KeyData.Key;
    } else {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    }

    if (!EFI_ERROR (Status)) {
      Print (BootErrorString);
    }
  }
}


EFI_STATUS
PlatformBdsBootDisplayDevice (
  IN CHIPSET_CONFIGURATION        *SetupNVRam
  )
{
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_STATUS                    Status;
  BOOLEAN                       SkipOriginalCode;

  if (SetupNVRam == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SkipOriginalCode = FALSE;

  Status = BdsLibDisplayDeviceReplace (&SkipOriginalCode);
  if (SkipOriginalCode || !EFI_ERROR (Status)) {
   return Status;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = OemBootDisplayDeviceCallBack (SetupNVRam, LegacyBios);

  return Status;
}


/**
  Platform Bds Boot Selection

  @param  Selection     HotKey Selection

  @retval EFI_STATUS

**/
EFI_STATUS
PlatformBdsBootSelection (
  UINT16                                    Selection,
  UINT16                                    Timeout
  )
{
  BOOLEAN                            NoBootDevices;
  UINT8                              *VariablePtr;
  UINTN                              VariableSize;
  EFI_STATUS                         Status;
  EFI_BOOT_MODE                      BootMode;
  BOOLEAN                            KeyPressed;
  UINTN                              StringSetNum;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL   *Badging;
  UINT64                             OsIndications;
  UINT64                             OsIndicationsSupported;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
  CHIPSET_CONFIGURATION               *SystemConfiguration;

  NoBootDevices = FALSE;
  VariablePtr = NULL;
  VariableSize = 0;

  //
  // It needs display dialog for user input Hdd password to unlock Hdd in S4 resume.
  //
  BdsLibGetBootMode (&BootMode);

  //
  // Disable Hot Key event and check Hot Key result
  //
  BdsLibStopHotKeyEvent ();

//[-start-130401-IB06690222-add]//
  //
  // Prepare Acpi S3 save information and publish ExitPmAuthProtocol for inform
  // RC do post-initializations.
  //
  PlatformBdsAuth ();
//[-end-130401-IB06690222-add]//

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return EFI_SUCCESS;
  }

  //
  // If there is no Boot option, go into SetupUtility & keep in SetupUtility
  //
  Status  = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    NoBootDevices = TRUE;
  }

  BdsLibGetHotKey (&Selection, &KeyPressed);
  if (KeyPressed && Selection != FRONT_PAGE_HOT_KEY) {
    Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
    if (EFI_ERROR (Status)) {
      ConsoleControl = NULL;
    }

    Status = gBS->LocateProtocol (
                    &gEfiSetupUtilityProtocolGuid,
                    NULL,
                    (VOID **)&SetupUtility
                    );
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
    SystemConfiguration = (CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData;

    Status = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
    if (!EFI_ERROR (Status)) {
      //
      // BOOT_OS string always in the last one of array of OemBadgingString.
      //
      Badging->GetStringCount (Badging, &StringSetNum);
      if (ConsoleControl != NULL) {
        ShowOemString(Badging, TRUE, (UINT8) StringSetNum);
        BdsLibShowOemStringInTextMode (TRUE, (UINT8) StringSetNum);
      } else {
        if (SystemConfiguration->QuietBoot) {
          ShowOemString(Badging, TRUE, (UINT8) StringSetNum);
        } else {
          BdsLibShowOemStringInTextMode (TRUE, (UINT8) StringSetNum);
        }
      }
    }
  }

  OnEndOfDisableQuietBoot ();
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  //
  // Based on the key that was set, we can determine what to do
  //
  BdsLibOnEndOfBdsBootSelection ();

//[-start-131011-IB05400449-add]//
  //
  // Update mBootNext after IpmiMiscDxe process boot option.
  //
  IpmiSetBootNext (&mBootNext);
//[-end-131011-IB05400449-add]//

  VariableSize = sizeof(OsIndicationsSupported);
  Status = gRT->GetVariable (L"OsIndicationsSupported", &gEfiGlobalVariableGuid, NULL, &VariableSize, &OsIndicationsSupported);
  if (EFI_ERROR (Status)) {
    OsIndicationsSupported = 0;
  }

  VariableSize = sizeof(OsIndications);
  Status = gRT->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, NULL, &VariableSize, &OsIndications);
  if (!EFI_ERROR (Status)) {
    if (OsIndications & OsIndicationsSupported & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) {
      Selection = FRONT_PAGE_HOT_KEY;
    }
    OsIndications &= (~ (UINT64) EFI_OS_INDICATIONS_BOOT_TO_FW_UI);

    gRT->SetVariable (
           L"OsIndications",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           VariableSize,
           &OsIndications
           );
  }

  if (FeaturePcdGet(PcdMe5MbSupported)) {
    if (ActiveManagementEnterSetup() == TRUE) {
      BdsLibStartSetupUtility (FALSE);
    } else {
      Status = OemHotKeyCallback (
                 Selection,
                 Timeout,
                 BootMode,
                 NoBootDevices
                 );
    }
  } else {
    Status = OemHotKeyCallback (
               Selection,
               Timeout,
               BootMode,
               NoBootDevices
               );
  }

  return EFI_SUCCESS;
}


VOID
PlatformBdsAuth (
  VOID
  )
{
  EFI_HANDLE    Handle;
  EFI_STATUS    Status;
  EFI_ACPI_S3_SAVE_PROTOCOL  *AcpiS3Save;

  //
  // Prepare S3 information, this MUST be done before ExitPmAuth
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **)&AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }


  //
  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gExitPmAuthProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
}

VOID
ChipsetPrepareReadyToBootEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                Handle;
//[-start-130710-IB05160465-modify]//
  EFI_SETUP_UTILITY_PROTOCOL                *SetupUtility;
  CHIPSET_CONFIGURATION                     *SystemConfiguration;
//[-end-130710-IB05160465-modify]//

  Handle = NULL;

//[-start-130710-IB05160465-modify]//
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  ASSERT_EFI_ERROR (Status);
  SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;

  if (FeaturePcdGet(PcdMe5MbSupported)) {
    if (!FeaturePcdGet(PcdMrcRmt)) {
      if (SystemConfiguration->MeFwImageType == FW_IMAGE_TYPE_5MB) {
        BdsKvmInitialization();
      }
    }
  }
//[-end-130710-IB05160465-modify]//

//[-start-130401-IB06690222-remove]//
//
// It's too late to perform PlatformBdsAuth () function here, because SMRAM has
// been locked at SmmIplReadyToLockEventNotify () function in PiSmmIpl driver
// when ready to boot event triggered.
//
// Move this step to PlatformBdsBootSelection () ...
//
//  //
//  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter
//  //
//  PlatformBdsAuth();
//[-end-130401-IB06690222-remove]//

//[-start-130401-IB06690222-add]//
//
// Publish DxeSmmReadyToLockProtocol to lock SMRAM.
// However it's too late publish here, PiSmmIpl driver has triggered this event ...
//
// TODO: Find a proper place to publish DxeSmmReadyToLockProtocol
//
  //
  // NOTE: We need install DxeSmmReadyToLock directly here because many boot script is added via ExitPmAuth callback.
  // If we install them at same callback, these boot script will be rejected because BootScript Driver runs first to lock them done.
  // So we seperate them to be 2 different events, ExitPmAuth is last chance to let platform add boot script. DxeSmmReadyToLock will
  // make boot script save driver lock down the interface.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
//[-end-130401-IB06690222-add]//

  gBS->InstallProtocolInterface (
         &Handle,
         &gConOutDevStartedProtocolGuid,
         EFI_NATIVE_INTERFACE,
         NULL
         );
//[-start-140512-IB10300110-add]//
  WatchDogReadyToBootEvent ();
//[-end-140512-IB10300110-add]//
  return;
}


EFI_STATUS
EFIAPI
SetVgaCommandRegReadyToBootEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                            Status;
  PCI_DEV_DEF                           *VgaDevContext;
  UINTN                                 Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *PciRootBridgeIo;
  UINT64                                Address;
  UINT8                                 Date8;

  Status  = EFI_SUCCESS;
  VgaDevContext = (PCI_DEV_DEF *)Context;
  //
  // Index 0 is primary display VGA.
  //
  Index = 1;
  Date8 = 0x00;
  while ((VgaDevContext[Index].Seg != 0xFF) |
         (VgaDevContext[Index].Bus != 0xFF) |
         (VgaDevContext[Index].Dev != 0xFF) |
         (VgaDevContext[Index].Fun != 0xFF)) {
    Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
    ASSERT_EFI_ERROR (Status);
    if ((VgaDevContext[0].Seg == VgaDevContext[Index].Seg) &
        (VgaDevContext[0].Bus == VgaDevContext[Index].Bus) &
        (VgaDevContext[0].Dev == VgaDevContext[Index].Dev) &
        (VgaDevContext[0].Fun == VgaDevContext[Index].Fun)) {
      Index++;
      continue;
    }
    Address = EFI_PCI_ADDRESS (
                VgaDevContext[Index].Bus,
                VgaDevContext[Index].Dev,
                VgaDevContext[Index].Fun,
                PCICMD
                );
    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint8,
                                    Address,
                                    1,
                                    &Date8
                                    );
    Date8 = Date8 & (~0x01);
    Status = PciRootBridgeIo->Pci.Write (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint8,
                                    Address,
                                    1,
                                    &Date8
                                    );
    DEBUG ((EFI_D_ERROR, "Set Pci command Reg(0x04) of (%02x/%02x/%02x) as 0.\n", (VgaDevContext[Index].Bus), (VgaDevContext[Index].Dev), (VgaDevContext[Index].Dev)));
    Index++;
  }

  gBS->FreePool (VgaDevContext);
  gBS->CloseEvent (Event);

  return Status;
}


/**
  Create a ExitBootService event for setting VGA text mode

  @param  SetupVariable     A pointer to setup variable
  @param  BootMode          System boot mode

  @retval EFI_SUCCESS   Success test all the system memory and update
  @retval Other         Error return value from get memory function

**/
EFI_STATUS
ExitBootServiceSetVgaMode (
  IN CHIPSET_CONFIGURATION                   *SetupVariable,
  IN EFI_BOOT_MODE                          BootMode
  )
{
  EFI_STATUS                                Status;
  EFI_EVENT                                 ExitBootServicesEvent;
  EFI_LEGACY_BIOS_PROTOCOL                  *LegacyBios;

  if ((SetupVariable->BootNormalPriority == 0) && (BootMode == BOOT_ON_S4_RESUME)) {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = gBS->CreateEvent (
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_NOTIFY,
                    ExitBootServiceSetVgaModeCallback,
                    LegacyBios,
                    &ExitBootServicesEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}


/**
  Callback function for exit boot service event

  @param  Event         The memory test intensive level.
  @param  Context       Event context

**/
VOID
ExitBootServiceSetVgaModeCallback (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_IA32_REGISTER_SET         Regs;

  LegacyBios = (EFI_LEGACY_BIOS_PROTOCOL*) Context;
  //
  // Set the 80x25 Text VGA Mode
  //
  Regs.H.AH = 0x00;
  Regs.H.AL = 0x83;
  LegacyBios->Int86 (LegacyBios, 0x10, &Regs);

  Regs.H.AH = 0x11;
  Regs.H.AL = 0x14;
  Regs.H.BL = 0;
  LegacyBios->Int86 (LegacyBios, 0x10, &Regs);
}


/**
  Update gLastLegacyBootDevicePath if the last boot device is from legacy BBS table.
  Duplicate a new EFI device path if the last boot device is from native boot option.

  @param  BootPrevious
  @param  LastBootDevicePath    Point to device path of the last boot device for connection

  @retval EFI_SUCCESS

**/
EFI_STATUS
UpdateLastBootDevicePath (
  IN   UINT16                     *BootPrevious,
  OUT  EFI_DEVICE_PATH_PROTOCOL   **LastBootDevicePath
  )
{
  CHAR16                     Buffer[20];
  UINT8                      *Variable;
  UINTN                      VariableSize;
  UINT8                      *Ptr;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL   *FullDevicePath;

  //
  // Check the last boot device exists.
  //
  if (BootPrevious == NULL) {
    return EFI_UNSUPPORTED;
  }

  FullDevicePath = NULL;
  //
  // Get the last boot device's information from boot####.
  //
  UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", *BootPrevious);

  Variable = BdsLibGetVariableAndSize (
               Buffer,
               &gEfiGlobalVariableGuid,
               &VariableSize
               );

  //
  // If the device path is an EFI device path, duplicate device path directly.
  //
  Ptr = Variable;
  Ptr += sizeof (UINT32) + sizeof (UINT16);
  Ptr += StrSize ((CHAR16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;

  if (DevicePath->Type == MEDIA_DEVICE_PATH && DevicePath->SubType == MEDIA_HARDDRIVE_DP) {
    Variable = BdsLibGetVariableAndSize (
                 L"BootPreviousData",
                 &gEfiGenericVariableGuid,
                 &VariableSize
                 );
    FullDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Variable;
  }

  if (FullDevicePath == NULL) {
    *LastBootDevicePath = DuplicateDevicePath (DevicePath);
  } else {
    *LastBootDevicePath = DuplicateDevicePath (FullDevicePath);
  }

  return EFI_SUCCESS;
}


/**
  Show boot fail message. If auto failover is disable, it will go into firmware UI.

  @param  AutoFailover           Auto failover polciy
  @param  Message                Boot fail message

  @retval EFI_SUCCESS            Perform auto failover policy success.
  @retval EFI_INVALID_PARAMETER  String pointer is NULL.
  @retval Other                  Locate protocol fail or pop message fail.

**/
EFI_STATUS
AutoFailoverPolicyBehavior (
  IN BOOLEAN    AutoFailover,
  IN CHAR16     *Message
  )
{
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  H2O_DIALOG_PROTOCOL               *H2ODialog;
  UINT16                            Timeout;
  LIST_ENTRY                        BootLists;

  if (Message == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **)&H2ODialog
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DisableQuietBoot ();

  Status = H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, Message);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // If auto failover is disable, go into firmware UI.
  //
  if (!AutoFailover) {
    InitializeListHead (&BootLists);
    BdsLibConnectAll ();
    BdsLibEnumerateAllBootOption (TRUE, &BootLists);

    if (FeaturePcdGet(PcdFrontPageSupported)) {
      Timeout = 0xffff;
      PlatformBdsEnterFrontPage (Timeout, FALSE);
    } else {
      CallBootManager ();
    }
  }

  return EFI_SUCCESS;
}
//[-end-120625-IB03530375-modify]//

//[-start-130516-12360009-add]//
#ifdef TXT_SUPPORT
/**
  Find the EFI_COMPATIBILITY16_TABLE in E/F segment.

  @retval Table                  Found EFI_COMPATIBILITY16_TABLE.
  @retval NULL                   EFI_COMPATIBILITY16_TABLE not found.

**/
UINT8 *
FindCompatibility16Table (
  VOID
  )
{
  UINT8  *Table;

  for (Table = (UINT8 *) (UINTN) 0xE0000;
      Table < (UINT8 *) (UINTN) 0x100000;
      Table = (UINT8 *) Table + 0x10
      ) {
    if (*(UINT32 *) Table == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      return Table;
    }
  }

  return NULL;
}

/**
  This function calculates and updates an UINT8 checksum.

  @param  Buffer                 Pointer to buffer to checksum.
  @param  Size                   Number of bytes to checksum.
  @param  ChecksumOffset         Offset to place the checksum result in.

  @retval EFI_SUCCESS            The function completed successfully.

**/
STATIC
EFI_STATUS
AcpiPlatformChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
{
  UINT8 Sum;
  UINT8 *Ptr;

  Sum = 0;
  //
  // Initialize pointer
  //
  Ptr = Buffer;

  //
  // set checksum to 0 first
  //
  Ptr[ChecksumOffset] = 0;

  //
  // add all content of buffer
  //
  while (Size--) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  //
  // set checksum
  //
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}

/**
  This function is used to update ACPI table root pointer to CSM E/F segment.
  TXT ACM will check ACPI table from legacy region, EFI OS will fail to update
  that if it does an EFI boot. So we update it here.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
PrepareAcpiTableforTXT (
  VOID
  )
{
  EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;
  UINT16                       *CopyBuffer;
  VOID                         *AcpiTable;
  EFI_STATUS                   Status;
  UINTN                        CopySize;
  CHIPSET_CONFIGURATION        *ChipsetConfig;
  EFI_COMPATIBILITY16_TABLE    *Table;
  EFI_LEGACY_REGION_PROTOCOL   *LegacyRegionProtocol;
  EFI_SETUP_UTILITY_PROTOCOL   *SetupUtility;

  ChipsetConfig = NULL;
  SetupUtility  = NULL;

  DEBUG ((EFI_D_INFO, " Prepare ACPI table for TXT.\n"));

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);

  if (Status == EFI_SUCCESS) {
    ChipsetConfig = (CHIPSET_CONFIGURATION *) SetupUtility->SetupNvData;
    if ((ChipsetConfig->TXTSupport != 1) || (ChipsetConfig->TXT != 1)) {
      return EFI_SUCCESS;
    }
  }

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    //
    // No CSM, do not need to handle it.
    //

    DEBUG ((EFI_D_ERROR, " Prepare ACPI table for TXT - NO CSM, do nothing\n"));
    return EFI_SUCCESS;
  }

  Table = (EFI_COMPATIBILITY16_TABLE *) FindCompatibility16Table ();
  ASSERT (Table != NULL);

  Status = gBS->LocateProtocol (&gEfiLegacyRegionProtocolGuid, NULL, (VOID **)&LegacyRegionProtocol);
  ASSERT_EFI_ERROR (Status);

  LegacyRegionProtocol->UnLock (
                           LegacyRegionProtocol,
                           0xE0000,
                           0x20000,
                           NULL
                           );

  //
  // Copy to legacy region
  //
  CopyBuffer = (VOID *)(UINTN) Table->AcpiRsdPtrPointer;

  AcpiTable = NULL;
  Status = EfiGetSystemConfigurationTable (
             &gEfiAcpi20TableGuid,
             (VOID **)&AcpiTable
             );
  if (EFI_ERROR (Status)) {
    Status = EfiGetSystemConfigurationTable (
               &gEfiAcpiTableGuid,
               (VOID **)&AcpiTable
               );
  }
  ASSERT (AcpiTable != NULL);

  if (*((UINT8 *) AcpiTable + 15) == 0) {
    CopySize = 20;
    AcpiPlatformChecksum (
        AcpiTable,
        sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
        OFFSET_OF (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER, Checksum)
        );
  } else {
    CopySize = (*(UINT32 *) ((UINT8 *) AcpiTable + 20));
    AcpiPlatformChecksum (
        AcpiTable,
        sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
        OFFSET_OF (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER, Checksum)
        );
    AcpiPlatformChecksum (
        AcpiTable,
        sizeof (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
        OFFSET_OF (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER, ExtendedChecksum)
        );
  }

  CopyMem (
      (VOID *)(UINTN)CopyBuffer,
      (VOID *)(UINTN)AcpiTable,
      CopySize
      );

  LegacyRegionProtocol->Lock (
                           LegacyRegionProtocol,
                           0xE0000,
                           0x20000,
                           NULL
                           );
  DEBUG ((EFI_D_INFO, " Prepare ACPI table for TXT - Successful!\n"));

  return EFI_SUCCESS ;
}
#endif // TXT_SUPPORT
//[-end-130516-12360009-add]//

BOOLEAN
FoundUSBConsoleOutput (
  OUT EFI_DEVICE_PATH_PROTOCOL  **UsbConsoleOuputDevPath
  )
{
  EFI_STATUS                            Status;
  UINTN                                 NumberOfHandles;
  EFI_HANDLE                            *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL              *GopDevicePath;
  UINTN                                 Index;
  EFI_DEVICE_PATH_PROTOCOL              *DevPathNode;
  EFI_DEVICE_PATH_PROTOCOL              *NewDevPath;
  BOOLEAN                               FoundUsbConsole;

  //
  // init locals
  //
  NumberOfHandles = 0;
  HandleBuffer = NULL;
  GopDevicePath = NULL;
  Index = 0;
  DevPathNode = NULL;
  NewDevPath = NULL;
  FoundUsbConsole = FALSE;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                );
  if (EFI_ERROR (Status)) {
    return FoundUsbConsole;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                  &gEfiDevicePathProtocolGuid,
                  (VOID*)&GopDevicePath
                  );
    if (EFI_ERROR (Status)) {
      continue;
    }

    DevPathNode = GopDevicePath;
    while (!IsDevicePathEnd (DevPathNode)) {
      if (DevicePathType (DevPathNode) == DP_TYPE_MESSAGING &&
          DevicePathSubType (DevPathNode) == DP_SUBTYPE_USB
          ) {
        NewDevPath = AppendDevicePathInstance (*UsbConsoleOuputDevPath, GopDevicePath);
        if (*UsbConsoleOuputDevPath != NULL) {
          FreePool (*UsbConsoleOuputDevPath);
        }
        *UsbConsoleOuputDevPath = NewDevPath;
        FoundUsbConsole = TRUE;
        break;
      }
      DevPathNode = NextDevicePathNode (DevPathNode);
    }
  }
  FreePool (HandleBuffer);

  return FoundUsbConsole;
}

VOID
SetUsbConsoleOutToConOutVar (
  IN EFI_DEVICE_PATH_PROTOCOL  *UsbConsoleOuputDevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *VarConOut;
  UINTN                     DevPathSize;
  EFI_DEVICE_PATH_PROTOCOL  *NextDevPathInst;
  EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_STATUS                Status;
  UINTN                     Size;

  //
  //init locals
  //
  VarConOut = NULL;
  DevPathSize = 0;
  NextDevPathInst = NULL;
  TempDevicePath = NULL;
  RemainingDevicePath = UsbConsoleOuputDevPath;
  Size = 0;

  VarConOut = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevPathSize
                );

  do {
    NextDevPathInst = GetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInst != NULL) {
      Status = BdsMatchDevicePaths(VarConOut, NextDevPathInst, NULL, FALSE);
      if (EFI_ERROR (Status)) {
        TempDevicePath = AppendDevicePathInstance (VarConOut, NextDevPathInst);
        if (VarConOut != NULL) {
          FreePool (VarConOut);
        }
        VarConOut = TempDevicePath;
      }
      FreePool (NextDevPathInst);
    }
  } while (RemainingDevicePath != NULL);

  gRT->SetVariable (
        L"ConOut",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
        GetDevicePathSize (VarConOut),
        VarConOut
        );
}

EFI_STATUS
BdsMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  * Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  * Single,
  IN  EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath OPTIONAL,
  IN  BOOLEAN                   Delete
  )
/*++

Routine Description:
  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

Arguments:
  Multi        - A pointer to a multi-instance device path data structure.

  Single       - A pointer to a single-instance device path data structure.

  NewDevicePath - If Delete is TRUE, this parameter must not be null, and it
                  points to the remaining device path data structure.
                  (remaining device path = Multi - Single.)

  Delete        - If TRUE, means removing Single from Multi.
                  If FALSE, the routine just check whether Single matches
                  with any instance in Multi.

Returns:


  The function returns EFI_SUCCESS if the Single is contained within Multi.
  Otherwise, EFI_NOT_FOUND is returned.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath1;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath2;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  //
  // The passed in DevicePath should not be NULL
  //
  if ((!Multi) || (!Single)) {
    return EFI_NOT_FOUND;
  }
  //
  // if performing Delete operation, the NewDevicePath must not be NULL.
  //
  TempDevicePath1 = NULL;

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst) {
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      if (!Delete) {
        gBS->FreePool (DevicePathInst);
        return EFI_SUCCESS;
      }
    } else {
      if (Delete) {
        TempDevicePath2 = AppendDevicePathInstance (
                            TempDevicePath1,
                            DevicePathInst
                            );
        gBS->FreePool (TempDevicePath1);
        TempDevicePath1 = TempDevicePath2;
      }
    }

    gBS->FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  if (Delete) {
    *NewDevicePath = TempDevicePath1;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

//[-start-140512-IB10300110-add]//
VOID
WatchDogReadyToBootEvent (
  VOID
  )
{
  EFI_STATUS                                Status;
  WATCH_DOG_HOOK_PROTOCOL                   *WatchDogHook;
  
  Status = gBS->LocateProtocol (
                  &gWatchDogHookProtocolGuid,
                  NULL,
                  (VOID **)&WatchDogHook
                  );

  if (!EFI_ERROR (Status)) {
    WatchDogHook->OemWatchDogHook ();
  }

  if (PcdGet8 (PcdH2OWatchDogMask) & 0x04) { // bit 2
    WatchDogStart (OS_WATCH_DOG);
  }
  else {
    WatchDogStop ();
  }
}
//[-end-140512-IB10300110-add]//

