/** @file
  Common Utility Header for UsbCoreDxe

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

#ifndef _USB_CORE_DXE_H
#define _USB_CORE_DXE_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UsbCore.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/UsbLegacyPlatform.h>
#include <Protocol/Cpu.h>
#include <Protocol/Timer.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AcpiSupport.h>
#include <Guid/EventGroup.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/HobList.h>
#include <IndustryStandard/Acpi.h>

#define SHELL_ENVIRONMENT_INTERFACE_PROTOCOL \
  { 0x47c7b221, 0xc42a, 0x11d2, 0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

#define EFI_SMM_RUNTIME_PROTOCOL_GUID \
  { 0xa56897a1, 0xa77f, 0x4600, 0x84, 0xdb, 0x22, 0xb0, 0xa8, 0x1, 0xfa, 0x9a  }

#define ACPI_ENABLE_CALLBACK_DONE_GUID \
  { 0xcc89c099, 0xd5dc, 0x4af6, 0xa3, 0x77, 0x8a, 0x26, 0xfd, 0x75, 0xf4, 0xad }

#define USB_WORKING_REGION_HOB_GUID \
  { 0x742aaece, 0x5493, 0x4533, 0x9e, 0xe4, 0x7e, 0x24, 0xc9, 0x3c, 0x27, 0xe9 }

//
// Memory management related definitions
//
#define MEMORY_BLOCK_UNIT_IN_PAGES      128
#define SMM_MEMORY_BLOCK_DEVIDER        4
#define MEMORY_BLOCK_SIZE_IN_BYTES      (MEMORY_BLOCK_UNIT_IN_PAGES * 4096)
#define BITARRAY_SIZE_IN_BYTES          (MEMORY_BLOCK_SIZE_IN_BYTES / 32 / 8)
//
// Misc definitions
//
#define MAX_DEFERRED_CALLBACK_TOKENS    16
#define MAX_IMAGE_ATTRIB                16
//
// UsbCore instance
//
#define USB_CORE_SIGNATURE              SIGNATURE_32 ('u', 'c', 'o', 'r')
#define GET_PRIVATE_FROM_USB_CORE(a)    CR (a, USB_CORE_PRIVATE, UsbCoreProtocol, USB_CORE_SIGNATURE)

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB3_HC_CALLBACK                  Callback;
  VOID                                  *Context;
  UINTN                                 Type;
  UINTN                                 Unit;
  UINTN                                 Timeout;
  INTN                                  Countdown;
} PERIODIC_TIMER_TOKEN;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB3_HC_PROTOCOL                  *Usb3Hc;
} PROVIDER_TOKEN;

typedef struct {
  LIST_ENTRY                            Link;
  USB_DEVICE                            UsbDevice;
} USB_DEVICE_TOKEN;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB_BINDING_SUPPORTED             Support;
  EFI_USB_BINDING_START                 Start;
  EFI_USB_BINDING_STOP                  Stop;
} USB_BINDING_TOKEN;

typedef struct {
  LIST_ENTRY                            Link;
  VOID                                  *BaseAddress;
  UINT16                                Act;
  UINT16                                Count;
} ADDRESS_CONVERT_TOKEN;

typedef struct {
  UINTN                                 ImageBase1;
  UINTN                                 ImageBase2;
  UINTN                                 ImageSize;
  INTN                                  ImageDist;
} IMAGE_ATTRIB;

#pragma pack(1)

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  UINTN                                 TotalLength;
  EFI_USB_HID_DESCRIPTOR                HidDescriptor;
} HID_DESCRIPTOR_TOKEN;

#pragma pack()
//
// The definition of partial SMM RUNTIME PROTOCOL used for backward compatible with ECP based SMM Runtime 
//
typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_PROTOCOL_NOTIFY_EVENT) (
  IN     EFI_EVENT_NOTIFY               CallbackFunction,
  IN     VOID                           *Context,
  IN     EFI_GUID                       *ProtocolGuid,
  OUT    EFI_EVENT                      *Event
  );

typedef struct {
  EFI_LOCATE_PROTOCOL                   LocateProtocol;
  VOID                                  *Dummy1;
  VOID                                  *Dummy2;
  VOID                                  *Dummy3;
  VOID                                  *Dummy4;
  EFI_ENABLE_PROTOCOL_NOTIFY_EVENT      EnableProtocolNotify;
} EFI_SMM_RUNTIME_PROTOCOL;

typedef struct {
  UINTN                                 Signature;
  EFI_USB_CORE_PROTOCOL                 UsbCoreProtocol;
  EFI_SMM_SYSTEM_TABLE2                 *Smst;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  EFI_USB_CORE_SMM_USB_DISPATCH         UsbLegacySwSmiCallback;
  EFI_SMM_USB_DISPATCH2_PROTOCOL        *UsbDispatch;
  EFI_SMM_CONTROL2_PROTOCOL             *SmmControl;
  EFI_KBC_EMULATOR_PROTOCOL             *KbcEmulator;
  EFI_USB3_HC_PROTOCOL                  *PeriodicTimerProvider;
  EFI_USB3_HC_PROTOCOL                  *LegacySupportProvider;
  EFI_SMM_CPU_PROTOCOL                  *SmmCpu;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *RootBridgeIo;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRuntime;
  EFI_CPU_ARCH_PROTOCOL                 *CpuArch;
  EFI_TIMER_ARCH_PROTOCOL               *TimerArch;
  EFI_LEGACY_8259_SET_MODE              Legacy8259SetMode;
  EFI_LEGACY_BIOS_INT86                 LegacyBiosInt86;
  EFI_TIMER_SET_TIMER_PERIOD            SetTimerPeriod;
  LIST_ENTRY                            PeriodicTimerListHead;
  LIST_ENTRY                            PeriodicTimerProviderListHead;
  LIST_ENTRY                            LegacySupportProviderListHead;
  LIST_ENTRY                            UsbDeviceListHead;
  LIST_ENTRY                            DeferredHcCallbackListHead;
  LIST_ENTRY                            UsbBindingListHead;
  LIST_ENTRY                            NonSmmCallbackListHead;
  LIST_ENTRY                            AddressConvertListHead;
  LIST_ENTRY                            SchedularConnectionListHead;
  LIST_ENTRY                            LegacyFreeHcCallbackListHead;
  LIST_ENTRY                            HidDescriptorListHead;
  USB_SMI_TOKEN                         *ProceedUsbSmiToken;
  CALLBACK_TOKEN                        DeferredCallbackTokens[MAX_DEFERRED_CALLBACK_TOKENS];
  USB_DEVICE                            **BootDevicesTable;
  EFI_EVENT                             NonSmmTimerEvent;
  EFI_HANDLE                            LegacyFreeHcTimerEvent;
  VOID                                  *UsbIgnoreDevices;
  UINTN                                 UsbIgnoreDevicesSize;
  UINT8                                 MemoryBitArray[2][BITARRAY_SIZE_IN_BYTES];
  UINT8                                 *MemoryBlockPtr[2];
  UINTN                                 MemoryLastBitPos[2];
  EFI_PHYSICAL_ADDRESS                  SmmRegionStart1;
  EFI_PHYSICAL_ADDRESS                  SmmRegionEnd1;
  EFI_PHYSICAL_ADDRESS                  SmmRegionStart2;
  EFI_PHYSICAL_ADDRESS                  SmmRegionEnd2;
  UINTN                                 NumImageAttrib;
  IMAGE_ATTRIB                          ImageAttrib[MAX_IMAGE_ATTRIB];
  UINTN                                 CurrMode;
  UINTN                                 CriticalSection;
  BOOLEAN                               SmmReady;
  BOOLEAN                               ShellInstalled;
  BOOLEAN                               ProceedAddressPatching;
  BOOLEAN                               ProceedDeferredProcedure;
  UINTN                                 NumInstances;
  BOOLEAN                               KbcChecked;
  EFI_STATUS                            KbcStatus;
  UINT64                                CpuFreq;
  BOOLEAN                               CsmEnabled;
  UINT16                                SwSmiPort;
  BOOLEAN                               UsbKbcOutputProhibit;
  VOID                                  *PeriodicTimerHandle;
  UINTN                                 PeriodicTimerPeriod;
  UINT32                                AcpiTimer;
} USB_CORE_PRIVATE;

extern USB_CORE_PRIVATE                 *mPrivate;

//
// Function prototype
//
EFI_STATUS
EFIAPI
UsbSmiRegister (
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_CORE_SMM_USB_DISPATCH  Callback,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  );

EFI_STATUS
EFIAPI
UsbSmiUnregister (
  IN     VOID                           *Handle
  );

EFI_STATUS
EFIAPI
UsbCoreSwSmiCallback(
  IN     EFI_HANDLE                     Handle,
  IN     CONST VOID                     *Context,
  IN OUT VOID                           *CommBuffer,
  IN OUT UINTN                          *CommBufferSize
  );

EFI_STATUS
EFIAPI
CpuIoRead8 (
  IN     UINT16                         Port,
  IN OUT UINT8                          *Buffer
  );

EFI_STATUS
EFIAPI
CpuIoWrite8 (
  IN     UINT16                         Offset,
  IN     UINT8                          Data
  );

EFI_STATUS
EFIAPI
CpuIoRead16 (
  IN     UINT16                         Port,
  IN OUT UINT16                         *Buffer
  );

EFI_STATUS
EFIAPI
CpuIoWrite16 (
  IN     UINT16                         Offset,
  IN     UINT16                         Data
  );

EFI_STATUS
EFIAPI
CpuIoRead32 (
  IN     UINT16                         Port,
  IN OUT UINT32                         *Buffer
  );

EFI_STATUS
EFIAPI
CpuIoWrite32 (
  IN     UINT16                         Offset,
  IN     UINT32                         Data
  );

EFI_STATUS
EFIAPI
CpuBswap16 (
  IN OUT UINT16                         *Data
  );

EFI_STATUS
EFIAPI
CpuBswap32 (
  IN OUT UINT32                         *Data
  );

EFI_STATUS
EFIAPI
CpuWbinvd (
  );

EFI_STATUS
EFIAPI
IsKbcExist (
  );

EFI_STATUS
EFIAPI
SyncKbdLed (
  );

EFI_STATUS
EFIAPI
ModuleRegistration (
  IN  EFI_HANDLE                        ImageHandle
  );

EFI_STATUS
EFIAPI
Stall (
  IN     UINTN                          Timeout
  );

EFI_STATUS
EFIAPI
CpuSaveState (
  IN OUT CPU_SAVE_STATE_PACKAGE         *CpuSaveStatePackage
  );
  
EFI_STATUS
EFIAPI
GetMode (
  OUT    UINTN                          *Mode
  );

EFI_STATUS
EFIAPI
SetMode (
  IN     UINTN                          Mode
  );

EFI_STATUS
EFIAPI
IsInSmm (
  IN     BOOLEAN                        *InSmm
  );

VOID
EFIAPI
DispatchLegacyFreeHcNotifyFunction (
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
ExitBootServicesNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
ReadyToBootNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
LegacyBootNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
KbcEmulatorNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
Legacy8259NotifyFunction (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

VOID
EFIAPI
ShellInstalledNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

VOID
EFIAPI
LegacyBiosNotifyFunction (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

VOID
EFIAPI
UsbLegacyPlatformNotifyFunction (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

EFI_STATUS
EFIAPI
CheckIgnoredDevice(
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );

EFI_STATUS
EFIAPI
SchedularConnection (
  IN     EFI_HANDLE                     Controller,
  IN     EFI_DEVICE_PATH_PROTOCOL       *TargetDevicePath,
  IN     VOID                           **Context,
  IN     UINTN                          Timeout,
  OUT    UINTN                          *Result
  );

EFI_STATUS
EFIAPI
IsCsmEnabled (
  );

EFI_STATUS
EFIAPI
GetSwSmiPort (
  OUT    UINT16                         *SwSmiPort
  );

BOOLEAN
CheckExist (
  IN LIST_ENTRY     *BaseList,
  IN LIST_ENTRY     *ThisList
  );
  
EFI_STATUS
EFIAPI
RegisterLegacyFreeHcCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINT32                         PciAddress,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  );

EFI_STATUS
EFIAPI
UnregisterLegacyFreeHcCallback (
  IN     VOID                           *Handle
  );

BOOLEAN
TriggerUsbSwSmi (
  );

EFI_STATUS
EFIAPI
RegisterHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  EFI_USB_HID_DESCRIPTOR            *HidDescriptor
  );

EFI_STATUS
EFIAPI
UnregisterHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo
  );

#endif
