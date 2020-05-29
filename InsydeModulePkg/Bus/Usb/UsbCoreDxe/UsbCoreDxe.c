/** @file
  Common Utility for USB

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "UsbCmds.h"
#include "MemMng.h"
#include "UsbPciIo.h"
#include "Legacy.h"

STATIC
UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL           *DevicePath
  );

USB_CORE_PRIVATE                *mPrivate;
STATIC EFI_GUID                 mEfiKbcEmulatorProtocolGuid = EFI_KBC_EMULATOR_PROTOCOL_GUID;
STATIC EFI_GUID                 mEfiShellEnvProtocolGuid = SHELL_ENVIRONMENT_INTERFACE_PROTOCOL;
STATIC EFI_GUID                 mEfiSmmRuntimeProtocolGuid = EFI_SMM_RUNTIME_PROTOCOL_GUID;
STATIC EFI_GUID                 mEfiAcpiEnableCallbackDoneGuid = ACPI_ENABLE_CALLBACK_DONE_GUID;
STATIC EFI_USB_CORE_PROTOCOL    mUsbCoreProtocol = 
{
  UsbGetDescriptor,
  UsbSetDescriptor,
  UsbGetDeviceInterface,
  UsbSetDeviceInterface,
  UsbGetDeviceConfiguration,
  UsbSetDeviceConfiguration,
  UsbSetDeviceFeature,
  UsbClearDeviceFeature,
  UsbGetDeviceStatus,
  UsbGetString,
  UsbClearEndpointHalt,
  UsbGetHidDescriptor,
  UsbGetReportDescriptor,
  UsbGetProtocolRequest,
  UsbSetProtocolRequest,
  UsbGetIdleRequest,
  UsbSetIdleRequest,
  UsbGetReportRequest,
  UsbSetReportRequest,
  AllocateBuffer,
  FreeBuffer,
  MemoryMapping,
  MemoryUnmapping,
  PciIoPciRead,
  PciIoPciWrite,
  PciIoMemRead,
  PciIoMemWrite,
  PciIoIoRead,
  PciIoIoWrite,
  CpuIoRead8,
  CpuIoWrite8,
  CpuIoRead16,
  CpuIoWrite16,
  CpuIoRead32,
  CpuIoWrite32,
  CpuBswap16,
  CpuBswap32,
  Stall,
  CpuSaveState,
  GetMode,
  SetMode,
  UsbSmiRegister,
  UsbSmiUnregister,
  InsertKbcKeyCode,
  KbcTrapProcessor,
  GetLegacySupportProvider,
  InsertLegacySupportProvider,
  RemoveLegacySupportProvider,
  InsertPeriodicTimer,
  RemovePeriodicTimer,
  InsertPeriodicTimerProvider,
  RemovePeriodicTimerProvider,
  EnterCriticalSection,
  LeaveCriticalSection,
  DispatchHcCallback,
  RegisterUsbBindingProtocol,
  GetUsbDevices,
  InsertUsbDevice,
  RemoveUsbDevice,
  ConnectUsbDevices,
  DisconnectUsbDevices,
  RegisterNonSmmCallback,
  CheckIgnoredDevice,
  CheckDeviceDetached,
  CpuWbinvd,
  IsKbcExist,
  ModuleRegistration,
  IsInSmm,
  InsertAddressConvertTable,
  RemoveAddressConvertTable,
  AddressConvert,
  SchedularConnection,
  IsCsmEnabled,
  GetSwSmiPort,
  RegisterLegacyFreeHcCallback,
  UnregisterLegacyFreeHcCallback,
  SyncKbdLed,
  RegisterHidDescriptor,
  UnregisterHidDescriptor
};

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

**/
EFI_STATUS
EFIAPI
UsbCoreDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  BOOLEAN                               InSmm;
  EFI_HANDLE                            Handle;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_USB_CORE_PROTOCOL                 *UsbCoreProtocol;
  EFI_LEGACY_8259_PROTOCOL              *Legacy8259;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL      *UsbLegacyPlatform;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }
  if (!InSmm) {
    //
    // Now in boot service, initial memory manager first to setup private storage
    //
    Status = InitialMemoryManagement(0);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Initialize private data
    //
    mPrivate->Signature = USB_CORE_SIGNATURE;
    mPrivate->CurrMode  = USB_CORE_NATIVE_MODE;
    //
    // Initial linking list heads
    //
    InitializeListHead(&mPrivate->PeriodicTimerListHead);
    InitializeListHead(&mPrivate->PeriodicTimerProviderListHead);
    InitializeListHead(&mPrivate->LegacySupportProviderListHead);
    InitializeListHead(&mPrivate->UsbDeviceListHead);
    InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
    InitializeListHead(&mPrivate->UsbBindingListHead);
    InitializeListHead(&mPrivate->NonSmmCallbackListHead);
    InitializeListHead(&mPrivate->AddressConvertListHead);
    InitializeListHead(&mPrivate->SchedularConnectionListHead);
    InitializeListHead(&mPrivate->LegacyFreeHcCallbackListHead);
    InitializeListHead(&mPrivate->HidDescriptorListHead);
    //
    // For UEFI 2.0 and the future use an Event Group
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK - 1,
                    ReadyToBootNotifyFunction,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &Event
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Registers the KbcEmulator protocol notification
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    KbcEmulatorNotifyFunction,
                    NULL,
                    &Event
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->RegisterProtocolNotify (
                    &mEfiKbcEmulatorProtocolGuid,
                    Event,
                    &Registration
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Registers the Shell protocol notification
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ShellInstalledNotifyFunction,
                    NULL,
                    &Event
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->RegisterProtocolNotify (
                    &mEfiShellEnvProtocolGuid,
                    Event,
                    &Registration
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Create event for USB hot plug, use CALLBACK level to insure
    // the hot plug procedure don't interrupt FAT driver which using 
    // CALLBACK level lock to against any interrupt
    //
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    NonSmmCallback,
                    NULL,
                    &mPrivate->NonSmmTimerEvent
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = gBS->SetTimer (
                    mPrivate->NonSmmTimerEvent,
                    TimerPeriodic,
                    NON_SMM_POLLING_INTERVAL
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = gBS->LocateProtocol (
                    &gEfiLegacy8259ProtocolGuid,
                    NULL,
                    (VOID **)&Legacy8259
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Notify the Legacy8259 callback directly
      //
      Legacy8259NotifyFunction (0, 0);
    } else {
      //
      // Registers the Legacy8259 protocol notification
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      Legacy8259NotifyFunction,
                      NULL,
                      &Event
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = gBS->RegisterProtocolNotify (
                      &gEfiLegacy8259ProtocolGuid,
                      Event,
                      &Registration
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Check is it legacy free system (no CSM installed)
    //
    Status = gBS->LocateProtocol (
                    &gEfiLegacyBiosProtocolGuid,
                    NULL,
                    (VOID **)&LegacyBios
                    );
    if (!EFI_ERROR (Status)) {
      //
      // CSM enabled system, notify the LegacyBios callback directly
      //
      LegacyBiosNotifyFunction (0, 0);
    } else {
      //
      // Register the notification for LegacyBios installed
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      LegacyBiosNotifyFunction,
                      NULL,
                      &Event
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = gBS->RegisterProtocolNotify (
                      &gEfiLegacyBiosProtocolGuid,
                      Event,
                      &Registration
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Setup the Usb policy from UsbLegacyPlatform protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiUsbLegacyPlatformProtocolGuid,
                    NULL,
                    (VOID **)&UsbLegacyPlatform
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Setup the Usb policy
      //
      UsbLegacyPlatformNotifyFunction (0, 0);
    }
    //
    // Always register the notification for UsbLegacyPlatform installation in case its being reinstalled
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UsbLegacyPlatformNotifyFunction,
                    NULL,
                    &Event
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->RegisterProtocolNotify (
                    &gEfiUsbLegacyPlatformProtocolGuid,
                    Event,
                    &Registration
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Locate Root Bridge Io protocol for use of native system
    //
    gBS->LocateProtocol (
           &gEfiPciRootBridgeIoProtocolGuid,
           NULL,
           (VOID **)&mPrivate->RootBridgeIo
           );
    //
    // Locate Cpu protocol for use of schedule connection
    //
    gBS->LocateProtocol (
           &gEfiCpuArchProtocolGuid,
           NULL,
           (VOID **)&mPrivate->CpuArch
           );
    //
    // Locate Timer protocol for use of legacy free timer interrupt measuring
    //
    gBS->LocateProtocol (
           &gEfiTimerArchProtocolGuid,
           NULL,
           (VOID **)&mPrivate->TimerArch
           );
    //
    // Backup the SetTimerPeriod for use of legacy free HC 
    //
    mPrivate->SetTimerPeriod = mPrivate->TimerArch->SetTimerPeriod;
    //
    // Setup the UsbCore protocol instance
    //
    CopyMem (
      &mPrivate->UsbCoreProtocol,
      &mUsbCoreProtocol,
      sizeof (EFI_USB_CORE_PROTOCOL)
      );
    //
    // Install the UsbCore protocol for boot service
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiUsbCoreProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mPrivate->UsbCoreProtocol
                    );
    //
    // Register module in DXE instance
    //
    ModuleRegistration (ImageHandle);
  } else {
    //
    // Now in SMM, get private storage first
    //
    Status = gBS->LocateProtocol (
                    &gEfiUsbCoreProtocolGuid,
                    NULL,
                    (VOID **)&UsbCoreProtocol
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    mPrivate = GET_PRIVATE_FROM_USB_CORE(UsbCoreProtocol);
    mPrivate->SmmBase  = SmmBase;
    mPrivate->SmmReady = TRUE;
    //
    // Register module in SMM instance
    //
    ModuleRegistration (ImageHandle);
    //
    // Setup Smm address convert table for Smm security policy
    //
    InsertAddressConvertTable (
      ACT_FUNCTION_POINTER,
      UsbCoreProtocol,
      sizeof(EFI_USB_CORE_PROTOCOL) / sizeof(VOID*)
      );
    //
    // Setup SmmControl protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiSmmControl2ProtocolGuid,
                    NULL,
                    (VOID **)&mPrivate->SmmControl
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Get Smm Syatem Table
    //
    SmmBase->GetSmstLocation(
               SmmBase,
               &mPrivate->Smst
               );
    //
    // Now in SMM, initial UsbHwSMI/UsbSwSMI
    //
    Status = mPrivate->Smst->SmmLocateProtocol (
                               &gEfiSmmUsbDispatch2ProtocolGuid,
                               NULL,
                               (VOID **)&mPrivate->UsbDispatch
                               );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Register a SwSMI callback for for UsbHwSMI registration and
    // UsbLegacySwSMI functional support
    //
    Status = mPrivate->Smst->SmmLocateProtocol (
                               &gEfiSmmSwDispatch2ProtocolGuid,
                               NULL,
                               (VOID **)&SwDispatch
                               );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Register a SwSMI callback
    //
    SwContext.SwSmiInputValue = USB_CORE_SW_SMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           UsbCoreSwSmiCallback,
                           &SwContext,
                           &Handle
                           );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Locate Smm Cpu protocol for Cpu save state manipulation
    //
    Status = mPrivate->Smst->SmmLocateProtocol (
                               &gEfiSmmCpuProtocolGuid,
                               NULL,
                               (VOID **)&mPrivate->SmmCpu
                               );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Setup memory manager for SMM memory
    //
    InitialMemoryManagement(1);
    //
    // Setup Smm address convert table for Smm security policy
    //
    InsertAddressConvertTable (
      ACT_INSTANCE_BODY,
      mPrivate,
      sizeof (USB_CORE_PRIVATE)
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &mPrivate,
      1
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &mPrivate->UsbIgnoreDevices,
      1
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &mPrivate->PeriodicTimerProvider,
      1
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &mPrivate->LegacySupportProvider,
      1
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &mPrivate->BootDevicesTable,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->PeriodicTimerListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->PeriodicTimerProviderListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->LegacySupportProviderListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->UsbDeviceListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->UsbBindingListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->LegacyFreeHcCallbackListHead,
      1
      );
    InsertAddressConvertTable (
      ACT_LINKING_LIST,
      &mPrivate->HidDescriptorListHead,
      1
      );
  }
  return Status;
}

/**

  Triggers USB SW SMI

**/
BOOLEAN
TriggerUsbSwSmi (
  )
{
  UINT8                         SmiDataValue;
  UINT8                         SmiDataSize;
  
  if (!mPrivate->SmmReady) return FALSE;
  SmiDataValue = USB_CORE_SW_SMI;
  SmiDataSize  = 1;
  //
  // Trigger a SMI to register callback
  //
  mPrivate->SmmControl->Trigger(
                          mPrivate->SmmControl,
                          &SmiDataValue,
                          &SmiDataSize,
                          0,
                          0
                          );
  return TRUE;
}

/**

  Register a USB SMI Callback

  @param  DevicePath            USB Device path
  @param  Callback              Callback function
  @param  Context               Private context for Callback
  @param  Handle                Handle of instance

**/
EFI_STATUS
EFIAPI
UsbSmiRegister (
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_CORE_SMM_USB_DISPATCH  Callback,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  )
{
  STATIC USB_SMI_TOKEN          UsbSmiToken;
  BOOLEAN                       InSmm;

  if (!DevicePath) {
    //
    // Called from UsbLegacy to register SwSmi for Atapi service
    //
    mPrivate->UsbLegacySwSmiCallback = Callback;
    return EFI_SUCCESS;
  }
  if (!mPrivate->SmmReady) {
    //
    // Unsupported on native system 
    //
    return EFI_UNSUPPORTED;
  }
  ZeroMem(
    &UsbSmiToken,
    sizeof(USB_SMI_TOKEN)
    );
  UsbSmiToken.Callback = Callback;
  CopyMem(
    UsbSmiToken.DevicePath,
    DevicePath,
    DevicePathSize(DevicePath)
    );
  //
  // Setup the context
  //
  UsbSmiToken.Context = Context;
  //
  // Setup the ProceedUsbSmiToken for UsbCoreSwSmiCallback
  //
  mPrivate->ProceedUsbSmiToken = &UsbSmiToken;
  //
  // Check is it in SMM
  //
  IsInSmm (&InSmm);
  if (!InSmm) {
    TriggerUsbSwSmi();
  } else {
    UsbCoreSwSmiCallback(NULL, NULL, NULL, NULL);
  }
  *Handle = mPrivate->ProceedUsbSmiToken;
  //
  // Set ProceedUsbSmiToken as NULL to insure the registration procedure stopped
  //
  mPrivate->ProceedUsbSmiToken = NULL;
  return EFI_SUCCESS;
}

/**

  Unregister a USB SMI Callback

  @param  Handle                Handle of instance

**/
EFI_STATUS
EFIAPI
UsbSmiUnregister (
  OUT    VOID                           *Handle
  )
{
  BOOLEAN                       InSmm;

  if (!mPrivate->SmmReady) {
    //
    // Unsupported on native system 
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Setup the ProceedUsbSmiToken for UsbCoreSwSmiCallback
  //
  mPrivate->ProceedUsbSmiToken = Handle;
  //
  // Check is it in SMM
  //
  IsInSmm (&InSmm);
  if (!InSmm) {
    TriggerUsbSwSmi();
  } else {
    UsbCoreSwSmiCallback(NULL, NULL, NULL, NULL);
  }
  //
  // Set ProceedUsbSmiToken as NULL to insure the registration procedure stopped
  //
  mPrivate->ProceedUsbSmiToken = NULL;
  return EFI_SUCCESS;
}

/**

  The notification of AcpiEnableCallbackDone used for stop HC

  @param  Protocol              Points to the protocol's unique identifier.
  @param  Interface             Points to the interface instance.
  @param  Handle                The handle on which the interface was installed.

**/
STATIC
EFI_STATUS
EFIAPI
AcpiEnableCallbackDone (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  DispatchLegacyFreeHcNotifyFunction (0, (VOID*)(UINTN)LEGACY_FREE_HC_ACPI_ENABLE_EVENT);
  return EFI_SUCCESS;
}

/**

  UsbCore SW SMI callback for UsbHwSMI registration and
  UsbLegacySwSMI functional support

  @param  Handle                Image handle
  @param  Context               EFI_SMM_SW_DISPATCH_CONTEXT

**/
EFI_STATUS
EFIAPI
UsbCoreSwSmiCallback(
  IN     EFI_HANDLE                    Handle,
  IN     CONST VOID                    *Context,
  IN OUT VOID                          *CommBuffer,
  IN OUT UINTN                         *CommBufferSize
  )
{
  EFI_STATUS                    Status;
  USB_SMI_TOKEN                 *UsbSmiToken;
  EFI_SMM_USB_REGISTER_CONTEXT  SmmUsbContext;
  UINT8                         Value;
  EFI_SMM_RUNTIME_PROTOCOL      *SmmRT;
  EFI_EVENT                     Event;

  if (mPrivate->ProceedAddressPatching) {
    //
    // Switch to runtime mode
    //
    SetMode(USB_CORE_RUNTIME_MODE);
    //
    // LegacyFreeHc checking for HC which doesn't support SMI in legacy mode
    //
    if (!IsListEmpty(&mPrivate->LegacyFreeHcCallbackListHead)) {
      //
      // To register notification on ACPI enable event to stop HC for HC which doesn't support SMI in legacy mode
      //
      mPrivate->Smst->SmmRegisterProtocolNotify (
                        &mEfiAcpiEnableCallbackDoneGuid,
                        AcpiEnableCallbackDone,
                        &Event
                        );
      //
      // To register notification for ECP based SMM notification for backward compatible 
      //
      if (mPrivate->SmmRuntime) {
      	Status = mPrivate->SmmRuntime->LocateProtocol (
                                         &mEfiSmmRuntimeProtocolGuid,
                                         NULL,
                                         (VOID **)&SmmRT
                                         );
        if (!EFI_ERROR (Status)) {
          //
          // Register notification on ACPI enable event to stop HC for legacy mode
          //
          SmmRT->EnableProtocolNotify (
                   (EFI_EVENT_NOTIFY)DispatchLegacyFreeHcNotifyFunction,
                   (VOID*)(UINTN)LEGACY_FREE_HC_ACPI_ENABLE_EVENT,
                   &mEfiAcpiEnableCallbackDoneGuid,
                   &Event
                   );
        }
      }
    }
    //
    // Patching the boot service address into SMM address 
    //
    AddressPatching();
    mPrivate->ProceedAddressPatching = FALSE;
  } else if (mPrivate->ProceedUsbSmiToken) {
    //
    // Called from UsbSmiRegister or UsbSmiUnregister
    //
    if (mPrivate->ProceedUsbSmiToken->Handle == NULL) {
      //
      // Allocate the UsbSmiToken buffer in the SMM memory
      //
      Status = AllocateBuffer(
                 sizeof(USB_SMI_TOKEN),
                 ALIGNMENT_32 | SMM_ADDRESS,
                 (VOID **)&UsbSmiToken
                 );
      if (Status != EFI_SUCCESS) {
        //
        // Set ProceedUsbSmiToken as NULL to stop the registration procedure
        //
        mPrivate->ProceedUsbSmiToken = NULL;
        return EFI_SUCCESS;
      }
      CopyMem (
        UsbSmiToken,
        mPrivate->ProceedUsbSmiToken,
        sizeof(USB_SMI_TOKEN)
        );
      //
      // Enforce the callback to SMM instance due to it is for SMI handler
      //
      AddressConvert (
        SMM_ADDRESS,
        (VOID*)(UINTN)UsbSmiToken->Callback,
        (VOID**)&UsbSmiToken->Callback
        );
      //
      // Register SMM Callback
      //
      SmmUsbContext.Type = UsbLegacy;
      SmmUsbContext.Device = (EFI_DEVICE_PATH_PROTOCOL*)UsbSmiToken->DevicePath;
      Status = mPrivate->UsbDispatch->Register(
                                        mPrivate->UsbDispatch,
                                        UsbSmiToken->Callback,
                                        &SmmUsbContext,
                                        &UsbSmiToken->Handle
                                        );
      if (EFI_ERROR(Status) && 
          DevicePathSize((EFI_DEVICE_PATH_PROTOCOL*)UsbSmiToken->DevicePath) == sizeof(ACPI_HID_DEVICE_PATH) + sizeof(PCI_DEVICE_PATH) + END_DEVICE_PATH_LENGTH) {
        //
        // Try to use old format of USB device path(onboard device only)
        //
        Value = UsbSmiToken->DevicePath[0x02];
        UsbSmiToken->DevicePath[0x02] = UsbSmiToken->DevicePath[0x03];
        UsbSmiToken->DevicePath[0x03] = Value;
        UsbSmiToken->DevicePath[0x0e] = UsbSmiToken->DevicePath[0x0f];
        UsbSmiToken->DevicePath[0x0f] = Value;
        UsbSmiToken->DevicePath[0x10] = UsbSmiToken->DevicePath[0x11];
        UsbSmiToken->DevicePath[0x11] = Value;
        UsbSmiToken->DevicePath[0x14] = UsbSmiToken->DevicePath[0x15];
        UsbSmiToken->DevicePath[0x15] = Value;
        UsbSmiToken->DevicePath[0x12] = 0xff;
        Status = mPrivate->UsbDispatch->Register(
                                          mPrivate->UsbDispatch,
                                          UsbSmiToken->Callback,
                                          &SmmUsbContext,
                                          &UsbSmiToken->Handle
                                          );
        if (EFI_ERROR(Status)) {
          FreeBuffer(
            sizeof(USB_SMI_TOKEN),
            UsbSmiToken
            );
          //
          // Set ProceedUsbSmiToken as NULL to stop the registration procedure
          //
          mPrivate->ProceedUsbSmiToken = NULL;
          return EFI_SUCCESS;
        }
      }
      //
      // Pass UsbSmiToken back to registration procedure
      //
      mPrivate->ProceedUsbSmiToken = UsbSmiToken;
      //
      // Setup Smm address convert table for Smm security policy
      //
      InsertAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &UsbSmiToken->Context,
        1
        );
    } else {
      //
      // Unregister SMM Callback
      //
      mPrivate->UsbDispatch->UnRegister(
                               mPrivate->UsbDispatch,
                               mPrivate->ProceedUsbSmiToken->Handle
                               );
      FreeBuffer(
        sizeof(USB_SMI_TOKEN),
        mPrivate->ProceedUsbSmiToken
        );
      //
      // Remove Smm address convert table
      //
      RemoveAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &mPrivate->ProceedUsbSmiToken->Context
        );
    }
  } else if (mPrivate->ProceedDeferredProcedure) {
    //
    // Process deferred procedures
    //
    ProcessDeferredProcedure();
  } else {
    //
    // Called from CSM16
    //
    if (mPrivate->UsbLegacySwSmiCallback) mPrivate->UsbLegacySwSmiCallback(NULL, NULL, NULL, NULL);
  }
  return EFI_SUCCESS;
}

/**

  Performs a I/O Byte Read Cycle

  @param  Port                  I/O Port
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
CpuIoRead8 (
  IN     UINT16                         Port,
  IN OUT UINT8                          *Buffer
  )
{
  *(UINT8*)Buffer = IoRead8 (Port);
  return EFI_SUCCESS;
}

/**

  Performs a I/O Byte Write Cycle

  @param  Port                  I/O Port
  @param  Data                  Output Data

**/
EFI_STATUS
EFIAPI
CpuIoWrite8 (
  IN     UINT16                         Port,
  IN     UINT8                          Data
  )
{
  IoWrite8 (Port, Data);
  return EFI_SUCCESS;
}

/**

  Performs a I/O Word Read Cycle

  @param  Port                  I/O Port
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
CpuIoRead16 (
  IN     UINT16                         Port,
  IN OUT UINT16                         *Buffer
  )
{
  *Buffer = IoRead16 (Port);
  return EFI_SUCCESS;
}

/**

  Performs a I/O Word Write Cycle

  @param  Port                  I/O Port
  @param  Data                  Output Data

**/
EFI_STATUS
EFIAPI
CpuIoWrite16 (
  IN     UINT16                         Port,
  IN     UINT16                         Data
  )
{
  IoWrite16 (Port, Data);
  return EFI_SUCCESS;
}

/**

  Performs a I/O DWord Read Cycle

  @param  Port                  I/O Port
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
CpuIoRead32 (
  IN     UINT16                         Port,
  IN OUT UINT32                         *Buffer
  )
{
  *Buffer = IoRead32 (Port);
  return EFI_SUCCESS;
}

/**

  Performs a I/O DWord Write Cycle

  @param  Port                  I/O Port
  @param  Data                  Output Data

**/
EFI_STATUS
EFIAPI
CpuIoWrite32 (
  IN     UINT16                         Port,
  IN     UINT32                         Data
  )
{
  IoWrite32 (Port, Data);
  return EFI_SUCCESS;
}

/**

  Bswap16

  @param  Data                  Pointer of UINT16 for bswap

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
CpuBswap16 (
  IN OUT UINT16                         *Data
  )
{
  *Data = SwapBytes16(*Data);
  return EFI_SUCCESS;
}

/**

  Bswap32

  @param  Data                  Pointer of UINT32 for bswap

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
CpuBswap32 (
  IN OUT UINT32                         *Data
  )
{
  *Data = SwapBytes32(*Data);
  return EFI_SUCCESS;
}

/**

  Wbinvd

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
CpuWbinvd (
  )
{
#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)
  AsmWbinvd();
#endif
  return EFI_SUCCESS;
}

/**

  Checks the physical KBC exist or not

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
IsKbcExist (
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  //
  // Only check once to preventing checking procedure called after emulation on
  //
  if (!mPrivate->KbcChecked) {
    //
    // Checks physical KBC exist or not
    //
    if (IoRead8 (0x64) == 0xff) Status = EFI_NOT_FOUND;
    mPrivate->KbcChecked = TRUE;
    mPrivate->KbcStatus  = Status;
  } else {
    Status = mPrivate->KbcStatus;
  }
  return Status;
}

/**

  Sync Keyboard LED

  @retval EFI_SUCCESS           Success
  @retval EFI_NOT_FOUND         Fail

**/
EFI_STATUS
EFIAPI
SyncKbdLed (
  )
{
  EFI_STATUS     Status;
  UINTN          Count;
  UINTN          Index;
  USB_HID_DEVICE **UsbHid;

  Status = GetUsbDevices (
             USB_CORE_USB_HID,
             &Count,
             (USB_DEVICE***)&UsbHid
             );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }
  for (Index = 0; Index < Count; Index ++) {
    if (UsbHid[Index]->SyncLED != NULL) {
      //
      // This is Usb Keyboard device. Call it out
      //
      UsbHid[Index]->SyncLED(0, UsbHid[Index]->Instance);
    }
  }
  //
  // Release memory allocated by GetUsbDevices
  //
  FreeBuffer (
    sizeof (USB_HID_DEVICE*) * Count,
    UsbHid
    );

  return EFI_SUCCESS;
}

/**

  Relocate the module into T-Segment

  @param  ImageHandle           ImageHandle
  @param  SystemTable           SystemTable

  @retval EFI_SUCCESS
  @retval EFI_UNLOAD_IMAGE

**/
EFI_STATUS
EFIAPI
ModuleRegistration (
  IN  EFI_HANDLE                        ImageHandle
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       InSmm;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  IMAGE_ATTRIB                  *ImageAttrib;
  UINTN                         Index;
  //
  // Get the ImageBase/ImageSize for image instance
  //
  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for (Index = 0, ImageAttrib = &mPrivate->ImageAttrib[0]; Index < mPrivate->NumImageAttrib; Index ++, ImageAttrib ++) {
    //
    // Searching for existance record
    //
    if (ImageAttrib->ImageSize == LoadedImage->ImageSize) break;
  }
  IsInSmm (&InSmm);
  if (Index == mPrivate->NumImageAttrib) {
    //
    // Not exist, append into the record
    //
    ImageAttrib = &mPrivate->ImageAttrib[mPrivate->NumImageAttrib];
    if (!InSmm) {
      ImageAttrib->ImageBase1 = (UINTN)LoadedImage->ImageBase;
    } else {
      ImageAttrib->ImageBase2 = (UINTN)LoadedImage->ImageBase;
    }
    ImageAttrib->ImageSize  = (UINTN)LoadedImage->ImageSize;
    ImageAttrib->ImageDist  = 0;
    mPrivate->NumImageAttrib ++;
  } else {
    //
    // Records the new image attribute in used for SMM address conversion
    //
    if (!InSmm) {
      ImageAttrib->ImageBase1 = (UINTN)LoadedImage->ImageBase;
    } else {
      ImageAttrib->ImageBase2 = (UINTN)LoadedImage->ImageBase;
    }
    ImageAttrib->ImageDist  = (INTN)((UINTN)ImageAttrib->ImageBase2 - (UINTN)ImageAttrib->ImageBase1);
  }
  return EFI_SUCCESS;
}

/**

  Data Polling for Both Native and Legacy Mode

  @param  Timeout               MicroSecond for Timeout

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

**/
EFI_STATUS
EFIAPI
Stall (
  IN     UINTN                          MicroSecond
  )
{
  UINTN  Elapse;
  UINT8  Toggle;
  UINT32 BaseTick;
  INT32  Delta0;
  INT32  Delta1;
  
  if (mPrivate->CurrMode == USB_CORE_RUNTIME_MODE) {
    if (mPrivate->AcpiTimer) {
      //
      // Use ACPI timer as the timer base
      //
      BaseTick = IoRead32 (mPrivate->AcpiTimer) & 0x00ffffff;
      Delta0   = (INT32)RShiftU64 (MultU64x32 (DivU64x32 (LShiftU64 ((UINT64)MicroSecond, 32), 1000000), 3579545), 32);
      do {
        Delta1 = (IoRead32 (mPrivate->AcpiTimer) & 0x00ffffff) - BaseTick;
        if (Delta1 < 0) Delta1 += 0x1000000;
      } while (Delta1 < Delta0);
    } else {
      //
      // Use 8259 port 0x61 toggle bit as the timer base
      //
      Toggle = IoRead8 (0x61) & 0x10;
      for (Elapse = 0; Elapse < MicroSecond; Elapse += (1000000 / 33333)) {
        while (Toggle == (IoRead8 (0x61) & 0x10));
        while (Toggle != (IoRead8 (0x61) & 0x10));
      }
    }
  } else {
    //
    // Use UEFI stall for native system
    //
    gBS->Stall (MicroSecond);
  }
  return EFI_SUCCESS;
}

/**

  Get/Set Cpu Save State

  @param  CpuSaveStatePackage   Pointer of CpuSaveStatePackage

  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
CpuSaveState (
  IN OUT CPU_SAVE_STATE_PACKAGE         *CpuSaveStatePackage
  )
{
  EFI_SMM_SAVE_STATE_REGISTER Register;
  UINTN                       Width;
  VOID                        *Buffer;
  UINTN                       Index;
  
  if (CpuSaveStatePackage->Signature != CPU_SAVE_STATE_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }
  if (CpuSaveStatePackage->CpuIndex >= mPrivate->Smst->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }
  for (Index = 0; Index < CpuSaveStatePackage->DataSize; Index ++) {
    Register = CpuSaveStatePackage->Data[Index * 2];
    Buffer   = &CpuSaveStatePackage->Data[(Index * 2) + 1];
    Width    = sizeof (UINT32);
    if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {
      Width = sizeof (EFI_SMM_SAVE_STATE_IO_INFO);
      ZeroMem (Buffer, Width);
    } else if (Register == EFI_SMM_SAVE_STATE_REGISTER_CS || 
               Register == EFI_SMM_SAVE_STATE_REGISTER_DS ||
               Register == EFI_SMM_SAVE_STATE_REGISTER_ES ||
               Register == EFI_SMM_SAVE_STATE_REGISTER_SS ||
               Register == EFI_SMM_SAVE_STATE_REGISTER_FS ||
               Register == EFI_SMM_SAVE_STATE_REGISTER_GS) {
      Width = sizeof (UINT16);
    }
    if (CpuSaveStatePackage->Direction) {
      //
      // Data out
      //
      mPrivate->SmmCpu->WriteSaveState (
                          mPrivate->SmmCpu,
                          Width,
                          Register,
                          CpuSaveStatePackage->CpuIndex,
                          Buffer
                          );
    } else {
      //
      // Data in
      //
      mPrivate->SmmCpu->ReadSaveState (
                          mPrivate->SmmCpu,
                          Width,
                          Register,
                          CpuSaveStatePackage->CpuIndex,
                          Buffer
                          );
    }
  }
  return EFI_SUCCESS;
}

/**

  Set current mode

  @param  Mode                  Pointer to mode

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetMode (
  OUT    UINTN                          *Mode
  )
{
  if (mPrivate->CurrMode != USB_CORE_RUNTIME_MODE && mPrivate->ShellInstalled == TRUE) {
    //
    // We have to return USB_CORE_NATIVE_MODE if under EFI shell
    //
    *Mode = USB_CORE_NATIVE_MODE;
    return EFI_SUCCESS;
  }
  *Mode = mPrivate->CurrMode;
  return EFI_SUCCESS;
}

/**

  Set current mode

  @param  Mode          Mode

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetMode (
  IN     UINTN                          Mode
  )
{
  mPrivate->CurrMode = Mode;
  return EFI_SUCCESS;
}

/**

  Check is it in SMM

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IsInSmm (
  OUT    BOOLEAN                        *InSmm
  )
{
  EFI_TPL OldTpl;
  
  if (mPrivate->CurrMode != USB_CORE_RUNTIME_MODE) {
    //
    // Check if in SMM
    //
    if (!mPrivate->SmmBase) {
      //
      // Try to locate SmmBase2 if installed
      //
      gBS->LocateProtocol (
             &gEfiSmmBase2ProtocolGuid,
             NULL,
             (VOID **)&mPrivate->SmmBase
             );
    }
    if (mPrivate->SmmBase) {
      mPrivate->SmmBase->InSmm (mPrivate->SmmBase, InSmm);
      if (*InSmm == FALSE) {
        //
        // We need to check is it under timer interrupt due to we are use it if USB HC doesn't support SMI
        //
        OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
        gBS->RestoreTPL (OldTpl);
        if (OldTpl == TPL_NOTIFY + 1) *InSmm = TRUE;
      }
    } else {
      //
      // SmmBase didn't installed yet
      //
      *InSmm = FALSE;
    }
  } else {
    //
    // This is must in SMM if now in legacy mode 
    //
    *InSmm = TRUE;
  }
  return EFI_SUCCESS;
}

/**

  Checks the ThisList exist or not

  @param  BaseList              Pointer to start of LIST_ENTRY list
  @param  ThisList              Pointer to the link of EHCI_ASYNC_REQUEST

**/
BOOLEAN
CheckExist (
  IN LIST_ENTRY                         *BaseList,
  IN LIST_ENTRY                         *ThisList
  )
{
  LIST_ENTRY  *NextLink;
  
  if (IsListEmpty (BaseList) && BaseList != ThisList) {
    return FALSE;
  }
  NextLink = BaseList;
  do {
    if (NextLink == ThisList) return TRUE;
    NextLink  = NextLink->ForwardLink;
  } while (NextLink != BaseList);
  return FALSE;
}

/**

  Dispatches Stop HC callback routine

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
DispatchLegacyFreeHcNotifyFunction (
  IN     UINTN                          Event,
  IN     VOID                           *Context
  )
{
  CALLBACK_TOKEN *LegacyFreeHcCallbackToken;
  CALLBACK_TOKEN *PreviousToken;
  UINTN          CallbackEvent = (UINTN)Context;
  
  if (IsListEmpty(&mPrivate->LegacyFreeHcCallbackListHead)) return;
  //
  // Use of Event to distinguish is it come from ExitBootService or AcpiEnabled event
  //
  if (CallbackEvent != LEGACY_FREE_HC_EXIT_BOOT_SERVICE_EVENT && CallbackEvent != LEGACY_FREE_HC_ACPI_ENABLE_EVENT) {
    //
    // Normal operation for legacy free HC event dispatching
    //    
    LegacyFreeHcCallbackToken = (CALLBACK_TOKEN*)&mPrivate->LegacyFreeHcCallbackListHead;
    do {
      PreviousToken             = LegacyFreeHcCallbackToken;
      LegacyFreeHcCallbackToken = (CALLBACK_TOKEN*)GetFirstNode((LIST_ENTRY*)LegacyFreeHcCallbackToken);
      LegacyFreeHcCallbackToken->Callback(CallbackEvent, LegacyFreeHcCallbackToken->Context);
      //
      // Safety check
      //
      if (!CheckExist(&mPrivate->LegacyFreeHcCallbackListHead, (LIST_ENTRY*)LegacyFreeHcCallbackToken)) {
        LegacyFreeHcCallbackToken = PreviousToken;
      }
    } while (!IsNodeAtEnd(&mPrivate->LegacyFreeHcCallbackListHead, (LIST_ENTRY*)LegacyFreeHcCallbackToken));
  } else {
    //
    // Remove the LegacyFreeHcCallbackListHead linking after dispatched
    //
    while (!IsListEmpty(&mPrivate->LegacyFreeHcCallbackListHead)) {
      LegacyFreeHcCallbackToken = (CALLBACK_TOKEN*)GetFirstNode(&mPrivate->LegacyFreeHcCallbackListHead);
      LegacyFreeHcCallbackToken->Callback(CallbackEvent, LegacyFreeHcCallbackToken->Context);
      UnregisterLegacyFreeHcCallback (LegacyFreeHcCallbackToken);
    }
  }
}

/**

  Signal handlers for ExitBootServices event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
ExitBootServicesNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  )
{
  //
  // Close the NonSmm timer callback event
  //
  if (mPrivate->NonSmmTimerEvent) {
    gBS->CloseEvent (
           mPrivate->NonSmmTimerEvent
           );
    mPrivate->NonSmmTimerEvent = NULL;
  }
  //
  // Close the LegacyFreeHc timer callback event
  //
  if (mPrivate->PeriodicTimerProvider == NULL && mPrivate->LegacyFreeHcTimerEvent) {
    gBS->CloseEvent (
           mPrivate->LegacyFreeHcTimerEvent
           );
    mPrivate->LegacyFreeHcTimerEvent = NULL;
  }
  //
  // Dispatching the LegacyFreeHc callback for HC which doesn't support SMI
  //
  DispatchLegacyFreeHcNotifyFunction(0, (VOID*)(UINTN)LEGACY_FREE_HC_EXIT_BOOT_SERVICE_EVENT);
  //
  // Patching the boot service address into SMM address through UsbSwSmi
  //
  mPrivate->ProceedAddressPatching = TRUE;
  if (TriggerUsbSwSmi() == FALSE) {
    //
    // Switch to runtime mode on native system
    //
    SetMode(USB_CORE_RUNTIME_MODE);
  }
}

/**

  Signal handlers for ReadyToBoot event
  Sets CurrMode as USB_CORE_TRANSITION_MODE

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
ReadyToBootNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_SUPPORT_PROTOCOL     *AcpiSupport;
  UINTN                         Index;
  EFI_ACPI_DESCRIPTION_HEADER   *Table;
  UINTN                         Handle;
  EFI_ACPI_TABLE_VERSION        Version;
  //
  // Set to transition mode to prevent PciShadowRoms disconnect USB drivers
  //
  SetMode(USB_CORE_TRANSITION_MODE);
  gBS->CloseEvent(Event);
  //
  // Make sure the priority of LegacyBoot/ExitBootService notifications is in the 
  // last group(TPL_CALLBACK - 1) with first priority(later registration will
  // be called earlier)
  //
  gBS->CreateEventEx (
         EVT_NOTIFY_SIGNAL,
         TPL_CALLBACK - 1,
         LegacyBootNotifyFunction,
         NULL,
         &gEfiEventLegacyBootGuid,
         &Event
         );
  gBS->CreateEventEx (
         EVT_NOTIFY_SIGNAL,
         TPL_CALLBACK - 1,
         ExitBootServicesNotifyFunction,
         NULL,
         &gEfiEventExitBootServicesGuid,
         &Event
         );
  //
  // Get SmmRuntime protocol used for ECP based SMM notification for backward compatible
  //
  gBS->LocateProtocol(
         &mEfiSmmRuntimeProtocolGuid,
         NULL,
         (VOID **)&mPrivate->SmmRuntime
         );
  //
  // Go through the ACPI table to get the AcpiTimer for stall function under SMM mode
  //
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid,
                  NULL,
                  (VOID **)&AcpiSupport
                  );
  if (EFI_ERROR(Status)) {
    return;
  }
  for (Index = 0, Status = EFI_SUCCESS; Status == EFI_SUCCESS; Index ++) {
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            (VOID **)&Table,
                            &Version,
                            &Handle
                            );
    if (!EFI_ERROR (Status)) {
      if (Table->Signature == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
        mPrivate->AcpiTimer = ((EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE*)Table)->PmTmrBlk;
        Status = EFI_ABORTED;
      }
      gBS->FreePool (Table);
    }
  }
}

/**

  Signal handlers for LegacBoot event
  Sets CurrMode as USB_CORE_RUNTIME_MODE

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
LegacyBootNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_USB3_HC_PROTOCOL *Usb3Hc = NULL;
  //
  // Startup the legacy HID device emulation mechanism if physical KBC not found
  //
  if (mPrivate->KbcEmulator && IsKbcExist() == EFI_NOT_FOUND && GetLegacySupportProvider(&Usb3Hc) == EFI_SUCCESS) {
    Usb3Hc->LegacyHidSupport(
              Usb3Hc,
              USB3_HC_HID_SUPPORT_SETUP_EMULATION,
              (VOID*)(UINTN)(0x01)
              );
  }
  //
  // Set the pointer device flag to disable as default
  //
  EBDA(EBDA_POINTER_DEVICE_FLAG) = 0x10;
  //
  // Close the NonSmm timer callback event
  //
  if (mPrivate->NonSmmTimerEvent) {
    gBS->CloseEvent (
           mPrivate->NonSmmTimerEvent
           );
    mPrivate->NonSmmTimerEvent = NULL;
  }
  //
  // Close the LegacyFreeHc timer callback event
  //
  if (mPrivate->PeriodicTimerProvider == NULL && mPrivate->LegacyFreeHcTimerEvent) {
    gBS->CloseEvent (
           mPrivate->LegacyFreeHcTimerEvent
           );
    mPrivate->LegacyFreeHcTimerEvent = NULL;
  }
  //
  // Launch the LegacyFreeHc callback to notice HC to setup the IRQ interrupt environment
  //
  DispatchLegacyFreeHcNotifyFunction(0, (VOID*)(UINTN)LEGACY_FREE_HC_LEGACY_BOOT_EVENT);
  //
  // Patching the boot service address into SMM address through UsbSwSmi
  //
  mPrivate->ProceedAddressPatching = TRUE;
  if (TriggerUsbSwSmi() == FALSE) {
    //
    // Switch to runtime mode on native system
    //
    SetMode(USB_CORE_RUNTIME_MODE);
  }
}

/**

  Signal handlers for KbcEmulator event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
KbcEmulatorNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Locate the KbcEmulator for use of legacy HID system 
  //
  gBS->LocateProtocol (&mEfiKbcEmulatorProtocolGuid, NULL, (VOID **)&mPrivate->KbcEmulator);
}

/**

  Legacy8259 SetMode filter function for KBC emulator

  @param  Same as Legacy8259 SetMode

  @retval Return value from Legacy8259 SetMode

**/
EFI_STATUS
EFIAPI
UsbCoreLegacy8259SetMode (
  IN  EFI_LEGACY_8259_PROTOCOL  *This,
  IN  EFI_8259_MODE             Mode,
  IN  UINT16                    *Mask,
  IN  UINT16                    *EdgeLevel
  )
{
  UINT8                 Type;
  EFI_USB3_HC_PROTOCOL  *Usb3Hc = NULL;
  
  Type = (Mode == Efi8259LegacyMode) ? 1 : 0;
  //
  // Setup the flag for USB keyboard. Set bit 1 in order to distinguish with OpRom dispatching flag(bit 0)
  //
  if (mPrivate->CsmEnabled) {
    //
    // Don't set the flag when USB KBC output been prohibited
    //
    if (Type && mPrivate->UsbKbcOutputProhibit) Type = 0; 
    EBDA(EBDA_OPROM_PROCESSING_FLAG) = (Type << 1);
  }
  if (mPrivate->CurrMode != USB_CORE_RUNTIME_MODE) {
    if (IsKbcExist() == EFI_NOT_FOUND && GetLegacySupportProvider(&Usb3Hc) == EFI_SUCCESS) {
      //
      // Setup KBC emulation
      //
      Usb3Hc->LegacyHidSupport(
                Usb3Hc,
                USB3_HC_HID_SUPPORT_SETUP_EMULATION,
                (VOID*)(UINTN)Type
                );
    }
  } else {
    //
    // Restore the SetMode function due to no longer used
    //
    This->SetMode = mPrivate->Legacy8259SetMode;
  }
  return mPrivate->Legacy8259SetMode (
                     This,
                     Mode,
                     Mask,
                     EdgeLevel
                     );
}

/**

  Signal handlers for Legacy8259 event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
Legacy8259NotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_LEGACY_8259_PROTOCOL *Legacy8259;
  //
  // Replace the Legacy8259 SetMode
  //
  gBS->LocateProtocol (
         &gEfiLegacy8259ProtocolGuid,
         NULL,
         (VOID **)&Legacy8259
         );
  mPrivate->Legacy8259SetMode = Legacy8259->SetMode;
  Legacy8259->SetMode         = UsbCoreLegacy8259SetMode;
}

/**

  Signal handlers for ShellEnvProtocol event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
ShellInstalledNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Signal the Shell Installed flag
  //
  mPrivate->ShellInstalled = TRUE;
}

/**

  LegacyBio Int86 filter function used for prohibit USB KBC output

  @param  Same as LegacyBios Int86

  @retval Return value from LegacyBios Int86

**/
STATIC
BOOLEAN
EFIAPI
UsbCoreLegacyBiosInt86 (
  IN  EFI_LEGACY_BIOS_PROTOCOL      *This,
  IN  UINT8                         BiosInt,
  IN  EFI_IA32_REGISTER_SET         *Regs
  )
{
  BOOLEAN Result;
  //
  // See is it Int16 to prohibit the USB KBC output
  //
  if (BiosInt == 0x16) mPrivate->UsbKbcOutputProhibit = TRUE;
  Result = mPrivate->LegacyBiosInt86 (This, BiosInt, Regs);
  if (BiosInt == 0x16) mPrivate->UsbKbcOutputProhibit = FALSE;
  return Result;
}

/**

  Signal handlers for Legacy Bios installed event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
LegacyBiosNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT8                    *Ptr;
  EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
  //
  // Signal the LegacyBios Installed flag
  //
  mPrivate->CsmEnabled = TRUE;
  //
  // Find out the S/W SMI port through $H2O table
  //
  for (Ptr = (UINT8*)((UINTN)0xFE000); Ptr < (UINT8*)((UINTN)0xFF000); Ptr += 0x10) {
    if (*(UINT32*)Ptr == SIGNATURE_32 ('O', '2', 'H', '$')) {
      mPrivate->SwSmiPort = *(UINT16*)(Ptr + 4);
    }
  }
  //
  // Replace the LegacyBios Int86
  //
  gBS->LocateProtocol (
         &gEfiLegacyBiosProtocolGuid,
         NULL,
         (VOID **)&LegacyBios
         );
  mPrivate->LegacyBiosInt86 = LegacyBios->Int86;
  LegacyBios->Int86         = UsbCoreLegacyBiosInt86;
}

/**

  Signal handlers for Usb Legacy Platform installed event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
UsbLegacyPlatformNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL      *UsbLegacyPlatform;
  USB_LEGACY_MODIFIERS                  UsbLegacyPolicy;
  UINT32                                UsbIgnoreDevicesPtr;
  UINT8                                 *Ptr;
  UINTN                                 Size;
  UINT16                                Length;
  
  Status = gBS->LocateProtocol (
                  &gEfiUsbLegacyPlatformProtocolGuid,
                  NULL,
                  (VOID **)&UsbLegacyPlatform
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Get the Usb Legacy Policy. Clear UsbLegacyModifiers first (GetUsbPlatformOptions will only capable to "set", can't "clear")
    //
    ZeroMem(&UsbLegacyPolicy, sizeof(USB_LEGACY_MODIFIERS));
    UsbLegacyPlatform->GetUsbPlatformOptions (
                         UsbLegacyPlatform,
                         &UsbLegacyPolicy
                         );
    //
    // Get the Usb Ignore Devices. In order to make backward compatible, check the size of USB_LEGACY_MODIFIERS
    //    
    if (sizeof(USB_LEGACY_MODIFIERS) > sizeof(UINT32)) {
      UsbIgnoreDevicesPtr = *(UINT32*)((UINT8*)&UsbLegacyPolicy + sizeof(UINT32));
      if (UsbIgnoreDevicesPtr != 0) {
        for (Ptr = (UINT8*)(UINTN)UsbIgnoreDevicesPtr, Size = sizeof(UINT32); *(UINT32*)Ptr != 0;) {
          Length = *(UINT16*)&Ptr[2];
          Size += Length;
          Ptr += Length;
        }
        if (mPrivate->UsbIgnoreDevices) {
          //
          // Destroy previous instance if it been allocated
          //
          RemoveAddressConvertTable (
            ACT_INSTANCE_BODY,
            mPrivate->UsbIgnoreDevices
            );
          FreeBuffer (
            mPrivate->UsbIgnoreDevicesSize,
            mPrivate->UsbIgnoreDevices
            );
          mPrivate->UsbIgnoreDevices = NULL;
        }
        mPrivate->UsbIgnoreDevicesSize = Size;
        Status = AllocateBuffer(
                   Size,
                   ALIGNMENT_32,
                   &mPrivate->UsbIgnoreDevices
                   );
        if (!EFI_ERROR (Status)) {
          CopyMem (
            mPrivate->UsbIgnoreDevices,
            (VOID*)(UINTN)UsbIgnoreDevicesPtr,
            Size
            );
          //
          // Setup Smm address convert table for Smm security policy
          //
          InsertAddressConvertTable (
            ACT_INSTANCE_BODY,
            mPrivate->UsbIgnoreDevices,
            Size
            );
        }
      }
    }
  }
}

/**

  Is CSM enabled

  @retval EFI_SUCCESS           CSM enabled
  @retval EFI_NOT_FOUND         CSM disabled

**/
EFI_STATUS
EFIAPI
IsCsmEnabled (
  )
{
  return (mPrivate->CsmEnabled) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**

  Get S/W SMI port number through CSM $H2O table

  @param  SwSmiPort             S/W SMI port

  @retval EFI_SUCCESS           S/W SMI port vaild
  @retval EFI_NOT_FOUND         S/W SMI port invaild

**/
EFI_STATUS
EFIAPI
GetSwSmiPort (
  OUT    UINT16                         *SwSmiPort
  )
{
  *SwSmiPort = mPrivate->SwSmiPort;
  return (mPrivate->SwSmiPort) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
CheckIgnoredDevice(
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  )
{
  EFI_STATUS                Status;
  EFI_DEV_PATH_PTR          DevPath, DevPath1;
  UINT8                     *Ptr, *Ptr1;
  UINTN                     NumDevPaths = 0;
  UINTN                     NumMatchedDevPaths = 0;
  EFI_USB_DEVICE_DESCRIPTOR DeviceDescriptor;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath1 = DevicePath;

  if (!mPrivate->UsbIgnoreDevices) return EFI_SUCCESS;
  //
  // Get device VID/PID
  //
  if (UsbIo) {
    //
    // Used to get the VID/PID
    //
    Status = UsbIo->UsbGetDeviceDescriptor (
                      UsbIo,
                      &DeviceDescriptor
                      );
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
  } else {
    //
    // Set the target VID/PID as 0 to caused comparison fail
    //
    DeviceDescriptor.IdVendor  = 0;
    DeviceDescriptor.IdProduct = 0;
  }
  //
  // Parsing the device paths
  //
  Ptr = (UINT8*)mPrivate->UsbIgnoreDevices;
  while (*(UINT32*)Ptr != 0) {
    DevPath = *(EFI_DEV_PATH_PTR*)&Ptr;
    switch (DevPath.DevPath->Type) {
      //
      // Check the Usb controller
      //
      case HARDWARE_DEVICE_PATH:
        if (DevPath.DevPath->SubType == HW_PCI_DP) {
          for (Ptr1 = (UINT8*)DevicePath1; *Ptr1 != END_DEVICE_PATH_TYPE;) {
            DevPath1 = *(EFI_DEV_PATH_PTR*)&Ptr1;
            Ptr1 += DevicePathNodeLength (DevPath1.DevPath);
            if (DevPath1.DevPath->Type == HARDWARE_DEVICE_PATH && DevPath1.DevPath->SubType == HW_PCI_DP) {
              if (DevPath.Pci->Function == DevPath1.Pci->Function &&
                  DevPath.Pci->Device == DevPath1.Pci->Device) {
                NumMatchedDevPaths ++;
              }
              break;
            }
          }
          //
          // Write back to DevicePath to test the rest of DevicePath
          //
          DevicePath1 = (EFI_DEVICE_PATH_PROTOCOL*)Ptr1;
        }
        NumDevPaths ++;
        break;
      //
      // Check the Usb port or device
      //
      case MESSAGING_DEVICE_PATH:
        if (DevPath.DevPath->SubType == MSG_USB_DP) {
          for (Ptr1 = (UINT8*)DevicePath1; *Ptr1 != END_DEVICE_PATH_TYPE;) {
            DevPath1 = *(EFI_DEV_PATH_PTR*)&Ptr1;
            Ptr1 += DevicePathNodeLength (DevPath1.DevPath);
            if (DevPath1.DevPath->Type == MESSAGING_DEVICE_PATH && DevPath1.DevPath->SubType == MSG_USB_DP) {
              if (DevPath.Usb->ParentPortNumber == DevPath1.Usb->ParentPortNumber) {
                NumMatchedDevPaths ++;
              }
              break;
            }
          }
          //
          // Write back to DevicePath to test the rest of DevicePath
          //
          DevicePath1 = (EFI_DEVICE_PATH_PROTOCOL*)Ptr1;
        } else if (DevPath.DevPath->SubType == MSG_USB_CLASS_DP) {
          if ((DevPath.UsbClass->VendorId == 0xffff || DeviceDescriptor.IdVendor == DevPath.UsbClass->VendorId) &&
              (DevPath.UsbClass->ProductId == 0xffff || DeviceDescriptor.IdProduct == DevPath.UsbClass->ProductId)) {
            NumMatchedDevPaths ++;
          }
        }
        NumDevPaths ++;
        break;
      case END_DEVICE_PATH_TYPE:
        if (NumDevPaths == NumMatchedDevPaths) return EFI_UNSUPPORTED;
        NumDevPaths = NumMatchedDevPaths = 0;
        DevicePath1 = DevicePath;
        break;
      default:
        NumDevPaths ++;
        break;
    }
    Ptr += DevicePathNodeLength (DevPath.DevPath);
  }
  return EFI_SUCCESS;
}

/**

  Schedular connection for connecting simultaneously

  @param  Controller            The controller handle
  @param  Timeout               Timeout value in millisecond

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SchedularConnection (
  IN     EFI_HANDLE                     Controller,
  IN     EFI_DEVICE_PATH_PROTOCOL       *TargetDevicePath,
  IN OUT VOID                           **Context,
  IN     UINTN                          Timeout,
  OUT    UINTN                          *Result
  )
{
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINT32                        UsbClassCReg[2];
  UINTN                         Index;
  SCHEDULAR_CONNECTION_TOKEN    *SchedularConnectionToken;
  SCHEDULAR_CONNECTION_TOKEN    *Token;
  BOOLEAN                       Connection;
  UINT64                        CurrentTicker;
  UINTN                         RemainingControllers;
  UINTN                         Scheduling;
  UINT64                        CpuFreq;

  if (mPrivate->ShellInstalled) {
    if (Timeout) {
      Stall (Timeout);
    }
    *Result = 0;
    return EFI_SUCCESS;
  }
  if (IsListEmpty(&mPrivate->SchedularConnectionListHead)) {
    //
    // Create a new SchedularConnectionToken on first time
    //
    Status = gBS->HandleProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
    UsbClassCReg[0] = UsbClassCReg[1] = 0;
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          9,
                          3,
                          &UsbClassCReg[0]
                          );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
    //
    // Enumerate/filter whole devices with same class code
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo
                      );
      if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
      }
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint8,
                            9,
                            3,
                            &UsbClassCReg[1]
                            );
      if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
      }
      //
      // Test whether the controller belongs to same type with controller
      //
      if (UsbClassCReg[0] == UsbClassCReg[1]) {
      	//
      	// Make it as connection target
      	//
        Status = AllocateBuffer(
                   sizeof (SCHEDULAR_CONNECTION_TOKEN),
                   ALIGNMENT_32,
                   (VOID **)&SchedularConnectionToken
                   );
        if (Status != EFI_SUCCESS) {
          return EFI_UNSUPPORTED;
        }
        InsertTailList (&mPrivate->SchedularConnectionListHead, (LIST_ENTRY*)SchedularConnectionToken);
        SchedularConnectionToken->Controller = HandleBuffer[Index];
      }
    }
    gBS->FreePool (HandleBuffer);
    *Result = 0;
    return EFI_SUCCESS;
  }
  Connection           = FALSE;
  RemainingControllers = 0;
  SchedularConnectionToken = (SCHEDULAR_CONNECTION_TOKEN*)&mPrivate->SchedularConnectionListHead;
  do {
    SchedularConnectionToken = (SCHEDULAR_CONNECTION_TOKEN*)GetFirstNode((LIST_ENTRY*)SchedularConnectionToken);
    if (SchedularConnectionToken->Controller == Controller) {
      if (Timeout == 0) {
        //
        // Do timer measurement
        //
        *Context = SchedularConnectionToken->Context;
        if (SchedularConnectionToken->TargetTicker) {
          SchedularConnectionToken->Scheduling ++;
          mPrivate->CpuArch->GetTimerValue (mPrivate->CpuArch, 0, &CurrentTicker, NULL);
          if (CurrentTicker < SchedularConnectionToken->TargetTicker) {
            return EFI_NOT_READY;
          } else {
            SchedularConnectionToken->Stage ++;
            *Result = SchedularConnectionToken->Stage;
            return EFI_SUCCESS;
          }
        } else {
          *Result = 0;
          return EFI_SUCCESS;
        }
      } else {
      	//
      	// Do timer initialization
      	//
      	if (SchedularConnectionToken->TargetTicker == 0) Connection = TRUE;
        mPrivate->CpuArch->GetTimerValue (mPrivate->CpuArch, 0, &CurrentTicker, &CpuFreq);
        if (!mPrivate->CpuFreq) {
          mPrivate->CpuFreq = DivU64x32 (1000000000000LL, (UINT32)CpuFreq);
        }
        SchedularConnectionToken->TargetTicker = CurrentTicker + DivU64x32 (MultU64x32 (mPrivate->CpuFreq, (UINT32)Timeout), 1000);
        SchedularConnectionToken->Context      = *Context;
      }
    }
    if (SchedularConnectionToken->TargetTicker == 0) {
      RemainingControllers ++;
    }
  } while (!IsNodeAtEnd(&mPrivate->SchedularConnectionListHead, (LIST_ENTRY*)SchedularConnectionToken));
  //
  // Do connection if it is last one
  //
  if (RemainingControllers == 0 && Connection == TRUE) {
    do {
      Connection = FALSE;
      SchedularConnectionToken = (SCHEDULAR_CONNECTION_TOKEN*)&mPrivate->SchedularConnectionListHead;
      do {
      	if (IsListEmpty(&mPrivate->SchedularConnectionListHead)) break;
        Token   = (SCHEDULAR_CONNECTION_TOKEN*)GetFirstNode((LIST_ENTRY*)SchedularConnectionToken);
        Scheduling = Token->Scheduling;
        Status = gBS->ConnectController (
                        Token->Controller,
                        NULL,
                        TargetDevicePath,
                        FALSE
                        );
        if (Status == EFI_SUCCESS || Status != EFI_NOT_FOUND || Scheduling == Token->Scheduling) {
          //
          // Release token
          //
          RemoveEntryList((LIST_ENTRY*)Token);
          FreeBuffer (
            sizeof (SCHEDULAR_CONNECTION_TOKEN),
            Token
            );
        } else {
          //
          // Connecting failed, keep trying
          //
          SchedularConnectionToken = Token;
          Connection = TRUE;
        }
      } while (!IsNodeAtEnd(&mPrivate->SchedularConnectionListHead, (LIST_ENTRY*)SchedularConnectionToken));
    } while (Connection);
  }
  return EFI_NOT_READY;
}

/**

  Register HID descriptor for use of GetHidDescriptor command

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  HidDescriptor         Pointer of HID descriptor
  
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI 
RegisterHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  EFI_USB_HID_DESCRIPTOR            *HidDescriptor
  )
{
  EFI_STATUS                    Status;
  HID_DESCRIPTOR_TOKEN          *HidDescriptorToken;
  UINTN                         TotalLength;
  UINTN                         Index;
  BOOLEAN                       RecordFound;
  //
  // Check is it existing
  //
  RecordFound        = FALSE;
  HidDescriptorToken = NULL;
  if (!IsListEmpty(&mPrivate->HidDescriptorListHead)) {
    HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)&mPrivate->HidDescriptorListHead;
    do {
      HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)GetFirstNode((LIST_ENTRY*)HidDescriptorToken);
      if (HidDescriptorToken->UsbIo == UsbIo) {
        RecordFound = TRUE;
        break;
      }
    } while (!IsNodeAtEnd(&mPrivate->HidDescriptorListHead, (LIST_ENTRY*)HidDescriptorToken));
  }
  //
  // Calculate the actual length for whole descriptors
  //
  TotalLength   = sizeof (HID_DESCRIPTOR_TOKEN) - sizeof (EFI_USB_HID_DESCRIPTOR) + HidDescriptor->Length;
  for (Index = 0; Index < HidDescriptor->NumDescriptors; Index ++) {
    if (HidDescriptor->HidClassDesc[Index].DescriptorType == USB_DESC_TYPE_REPORT) {
      TotalLength += HidDescriptor->HidClassDesc[Index].DescriptorLength;
      break;
    }
  }
  //
  // Make sure this is only record in the linking list
  //
  if (RecordFound) {
    if (TotalLength > HidDescriptorToken->TotalLength) {
      //
      // Reallocate the pool to accommodate new HID descriptor
      //
      UnregisterHidDescriptor (UsbIo);
    } else {
      //
      // Update the new HID descriptor
      //
      ZeroMem (&HidDescriptorToken->HidDescriptor, TotalLength);
      CopyMem (&HidDescriptorToken->HidDescriptor, HidDescriptor, HidDescriptor->Length);
      return EFI_SUCCESS;
    }
  }
  Status = AllocateBuffer(
             TotalLength,
             ALIGNMENT_32,
             (VOID **)&HidDescriptorToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to HidDescriptorListHead
  //
  HidDescriptorToken->UsbIo       = UsbIo;
  HidDescriptorToken->TotalLength = TotalLength;
  CopyMem (&HidDescriptorToken->HidDescriptor, HidDescriptor, HidDescriptor->Length);
  InsertTailList (&mPrivate->HidDescriptorListHead, (LIST_ENTRY*)HidDescriptorToken);
  //
  // Setup Smm address convert table for Smm security policy
  //
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    HidDescriptorToken,
    TotalLength
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &HidDescriptorToken->UsbIo,
    1
    );
  return EFI_SUCCESS;
}

/**

  Unregister HID descriptor

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI 
UnregisterHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo
  )
{
  HID_DESCRIPTOR_TOKEN  *HidDescriptorToken;
  //
  // Make sure this is only record in the linking list
  //
  if (!IsListEmpty(&mPrivate->HidDescriptorListHead)) {
    HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)&mPrivate->HidDescriptorListHead;
    do {
      HidDescriptorToken = (HID_DESCRIPTOR_TOKEN*)GetFirstNode((LIST_ENTRY*)HidDescriptorToken);
      if (HidDescriptorToken->UsbIo == UsbIo) {
        //
        // Remove HID descriptor token
        //
        RemoveEntryList ((LIST_ENTRY*)HidDescriptorToken);
        FreeBuffer(
          HidDescriptorToken->TotalLength,
          HidDescriptorToken
          );
        RemoveAddressConvertTable (
          ACT_INSTANCE_BODY,
          HidDescriptorToken
          );
        RemoveAddressConvertTable (
          ACT_INSTANCE_POINTER,
          &HidDescriptorToken->UsbIo
          );
        return EFI_SUCCESS;
      }
    } while (!IsNodeAtEnd(&mPrivate->HidDescriptorListHead, (LIST_ENTRY*)HidDescriptorToken));
  }
  return EFI_SUCCESS;
}

/**

  Calculate the size of device path

  @param  DevicePath            Pointer of EFI_DEVICE_PATH_PROTOCOL

  @retval Size of device path

**/
STATIC
UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }
  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    DevicePath = NextDevicePathNode (DevicePath);
  }
  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}