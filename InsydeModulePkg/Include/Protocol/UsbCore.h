/** @file
  Common Utility Header for USB

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

#ifndef _USB_CORE_H
#define _USB_CORE_H

#include <Uefi.h>
#include <PiDxe.h>
#include <Protocol/UsbIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/Usb3HostController.h>
#include <Protocol/UsbMassTransport.h>
#include <Protocol/UsbKbcEmulator.h>

#define EFI_USB_CORE_PROTOCOL_GUID \
  { 0xc965c76a, 0xd71e, 0x4e66, 0xab, 0x06, 0xc6, 0x23, 0x0d, 0x52, 0x84, 0x25 }

typedef struct _EFI_USB_CORE_PROTOCOL   EFI_USB_CORE_PROTOCOL;

#define KEEP_USBIO_FOR_IGNORED_DEVICE   0

#define	USB_CORE_SW_SMI                 0xFD

#define USB_CORE_BUS_MASTER_READ        0
#define USB_CORE_BUS_MASTER_WRITE       1
#define USB_CORE_BUS_MASTER_ALIGN4      2

#define USB_CORE_NATIVE_MODE            0x01
#define USB_CORE_TRANSITION_MODE        0x02
#define USB_CORE_RUNTIME_MODE           0x03

#define USB_CORE_PERIODIC_TIMER         0
#define USB_CORE_ONCE_TIMER             1

#define USB_CORE_KEYBOARD_CODE          0
#define USB_CORE_MOUSE_CODE             1

#define USB_CORE_USB_MASS_STORAGE       0
#define USB_CORE_USB_HID                1

#define MAXIMUM_POLLING_INTERVAL        512
#define MINIMUM_POLLING_INTERVAL        8
#define NON_SMM_POLLING_INTERVAL        10000000 / 2

#define LEGACY_FREE_HC_POLLING_INTERVAL (MINIMUM_POLLING_INTERVAL * 1000 * 10)

#define CONNECT_CONTROLLER              0
#define DISCONNECT_CONTROLLER           1

#define FDD_MEDIUM_TYPE_720K            0x1E
#define FDD_MEDIUM_TYPE_1440K           0x94

//
// Address converting type used for InsertAddressConvertTable
//
#define ACT_FUNCTION_POINTER            0
#define ACT_INSTANCE_BODY               1
#define ACT_INSTANCE_POINTER            2
#define ACT_LINKING_LIST                3
//
// Define the timeout as 3 seconds for whole control transfer
//
#define TIMEOUT_VALUE                   3 * 1000
//
// Allocate buffer alignment definition
//
#define ALIGNMENT_32                    0x00000020
#define ALIGNMENT_64                    0x00000040
#define ALIGNMENT_128                   0x00000080
#define ALIGNMENT_PAGE                  0x00001000
#define BOUNDARY_CHECK                  0x20000000
#define SMM_ADDRESS                     0x40000000
#define NON_SMM_ADDRESS                 0x80000000

//
// HID report types
//
#define HID_INPUT_REPORT                1
#define HID_OUTPUT_REPORT               2
#define HID_FEATURE_REPORT              3

//
// Definitions for legacy mode
//
#define EBDA(a)                         (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
#define EBDA_PS2_MOUSE_FLAG             0x026
#define EBDA_USB_MOUSE_FLAG             0x030
#define EBDA_POINTER_DEVICE_FLAG        0x032
#define EBDA_PS2_AUX_TYPE               0x03a
#define EBDA_MONITOR_KEY_TABLE          0x03b
#define EBDA_OPROM_PROCESSING_FLAG      0x116
#define EBDA_KEYBORD_SCAN_CODE          0x164
#define MOUSE_FLAG_IN_PROGRESS          0x080
#define PS2_LED_UPDATING                ((*(UINT8*)(UINTN)0x497) & 0x40)
#define USB_LED_UPDATE                  ((*(UINT8*)(UINTN)0x497) |= 0x08)
#define USB_LED_UPDATED                 ((*(UINT8*)(UINTN)0x497) &= ~0x08)

//
// Event type definition for Legacy Free HC
//
#define LEGACY_FREE_HC_EXIT_BOOT_SERVICE_EVENT  0x00000001
#define LEGACY_FREE_HC_ACPI_ENABLE_EVENT        0x00000002
#define LEGACY_FREE_HC_LEGACY_BOOT_EVENT        0x00000003
#define LEGACY_FREE_HC_TIMER_EVENT              0x00000004

//
// Common language code definition
//
#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"

//
// Member functions
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SMM_USB_DISPATCH) (
  IN     EFI_HANDLE                     DispatchHandle,
  IN     CONST VOID                     *DispatchContext,
  IN OUT VOID                           *CommBuffer,
  IN OUT UINTN                          *CommBufferSize
  );

typedef struct {
  UINT8                                 DevicePath[64];
  VOID                                  *Context;
  EFI_USB_CORE_SMM_USB_DISPATCH         Callback;
  EFI_HANDLE                            Handle;
} USB_SMI_TOKEN;

typedef struct {
  UINTN                                 Type;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  VOID                                  *Instance;
} USB_DEVICE_HEADER;

typedef struct {
  UINTN                                 Type;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  VOID                                  *Instance;
  USB_MASS_TRANSPORT_PROTOCOL           *Transport;
  EFI_BLOCK_IO_MEDIA                    *Media;
  UINT8                                 *MediumType;
  UINT8                                 *Manufacturer;
  UINT8                                 *Product;
  UINT8                                 *SerialNumber;
  UINT16                                VendorID;
  UINT16                                ProductID;
  UINT16                                HcLocation;
  UINT8                                 Lun;
  UINT8                                 InterfaceNumber;
  UINT8                                 HotPlugged;
} USB_MASS_STORAGE_DEVICE;

typedef struct {
  UINTN                                 Type;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  VOID                                  *Instance;
  EFI_USB3_HC_CALLBACK                  SyncLED;  
} USB_HID_DEVICE;

typedef union {
  USB_DEVICE_HEADER                     Header;
  USB_MASS_STORAGE_DEVICE               UsbMassStorage;
  USB_HID_DEVICE                        UsbHID;
} USB_DEVICE;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB3_HC_CALLBACK                  Callback;
  UINTN                                 Event;
  VOID                                  *Context;
} CALLBACK_TOKEN;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_HANDLE                            Controller;
  VOID                                  *Context;
  UINTN                                 Stage;
  UINTN                                 Scheduling;
  UINT64                                TargetTicker;
} SCHEDULAR_CONNECTION_TOKEN;

#define CPU_SAVE_STATE_SIGNATURE        SIGNATURE_32 ('$', 'C', 'P', 'U')

typedef struct {
  UINT32                                Signature;
  UINT8                                 Direction;
  UINT8                                 CpuIndex;
  UINT16                                DataSize;
  UINT32                                Data[1];
} CPU_SAVE_STATE_PACKAGE;
//
// Get Device Descriptor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            Value,
  IN  UINT16                            Index,
  IN  UINT16                            DescriptorLength,
  OUT VOID                              *Descriptor,
  OUT UINT32                            *Status
  );
//
// Set Device Descriptor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            Value,
  IN  UINT16                            Index,
  IN  UINT16                            DescriptorLength,
  IN  VOID                              *Descriptor,
  OUT UINT32                            *Status
  );
//
// Get device Interface
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_DEV_INTERFACE) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            Index,
  OUT UINT8                             *AltSetting,
  OUT UINT32                            *Status
  );
//
// Set device interface
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_DEV_INTERFACE) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            InterfaceNo,
  IN  UINT16                            AltSetting,
  OUT UINT32                            *Status
  );
//
// Get device configuration
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_DEV_CONFIGURATION) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  OUT UINT8                             *ConfigValue,
  OUT UINT32                            *Status
  );
//
// Set device configuration
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_DEV_CONFIGURATION) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            Value,
  OUT UINT32                            *Status
  );
//
//  Set Device Feature
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_DEV_FEATURE) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             Recipient,
  IN  UINT16                            Value,
  IN  UINT16                            Target,
  OUT UINT32                            *Status
  );
//
// Clear Device Feature
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CLR_DEV_FEATURE) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             Recipient,
  IN  UINT16                            Value,
  IN  UINT16                            Target,
  OUT UINT32                            *Status
  );
//
//  Get Device Status
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_DEV_STATUS) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             Recipient,
  IN  UINT16                            Target,
  OUT UINT16                            *DevStatus,
  OUT UINT32                            *Status
  );
//
// Usb Get String
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_DEV_STRING) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            LangID,
  IN  UINT8                             Index,
  IN  VOID                              *Buf,
  IN  UINTN                             BufSize,
  OUT UINT32                            *Status
  );
//
// Clear endpoint stall
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CLR_ENDPOINT_HALT) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             EndpointNo,
  OUT UINT32                            *Status
  );
//
// Get HID Descriptor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_HID_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             InterfaceNum,
  OUT VOID                              *HidDescriptor
  );

//
// Get Report Descriptor for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_REPORT_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             InterfaceNum,
  IN  UINT16                            DescriptorSize,
  OUT UINT8                             *DescriptorBuffer
  );
//
// Get Protocol for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_PROTOCOL_REQUEST) (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              Interface,
  IN UINT8                              *Protocol
  );
//
// Set Protocol for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_PROTOCOL_REQUEST) (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              Interface,
  IN UINT8                              Protocol
  );
//
// Get Idle for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_IDLE_REQUEST) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT8                             Interface,
  IN  UINT8                             ReportId,
  OUT UINT8                             *Duration
  );
//
// Set Idle for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_IDLE_REQUEST) (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              Interface,
  IN UINT8                              ReportId,
  IN UINT8                              Duration
  );
//
// Get Report for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_REPORT_REQUEST) (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              Interface,
  IN UINT8                              ReportId,
  IN UINT8                              ReportType,
  IN UINT16                             ReportLen,
  IN UINT8                              *Report
  );
//
// Set Report for HID
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_REPORT_REQUEST) (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              Interface,
  IN UINT8                              ReportId,
  IN UINT8                              ReportType,
  IN UINT16                             ReportLen,
  IN UINT8                              *Report
  );
//
// Allocate Pool
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_ALLOCATE_BUFFER) (
  IN  UINTN                             AllocSize,
  IN  UINTN                             Alignment,
  OUT VOID                              **Pool
  );
//
// Free Pool
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_FREE_BUFFER) (
  IN UINTN                              AllocSize,
  IN VOID                               *Pool
  );
//
// Memory Map
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_MEMORY_MAPPING) (
  IN  UINTN                             Direction,
  IN  EFI_PHYSICAL_ADDRESS              Addr,
  IN  UINTN                             Length,
  OUT VOID                              **AdjustedAddr,
  OUT VOID                              **Handle
  );
//
// Memory Unmap
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_MEMORY_UNMAPPING) (
  IN VOID                               *Handle
  );
//
// PCI Register Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_PCI_READ) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Register Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_PCI_WRITE) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Memory Bar Based Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_MEM_READ) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Memory Bar Based Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_MEM_WRITE) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI I/O Bar Based Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_IO_READ) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI I/O Bar Based Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_PCIIO_IO_WRITE) (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// I/O Byte Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_READ8) (
  IN     UINT16                         Port,
  IN OUT UINT8                          *Buffer
  );
//
// I/O Byte Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_WRITE8) (
  IN     UINT16                         Port,
  IN     UINT8                          Data
  );
//
// I/O Word Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_READ16) (
  IN     UINT16                         Port,
  IN OUT UINT16                         *Buffer
  );
//
// I/O Word Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_WRITE16) (
  IN     UINT16                         Port,
  IN     UINT16                         Data
  );
//
// I/O DWord Read
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_READ32) (
  IN     UINT16                         Port,
  IN OUT UINT32                         *Buffer
  );
//
// I/O DWord Write
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IO_WRITE32) (
  IN     UINT16                         Port,
  IN     UINT32                         Data
  );
//
// Bswap16
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_BSWAP16) (
  IN OUT UINT16                         *Data
  );
//
// Bswap32
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_BSWAP32) (
  IN OUT UINT32                         *Data
  );
//
// Stall
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_STALL) (
  IN     UINTN                          Timeout
  );
//
// CpuSaveState
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CPU_SAVE_STATE) (
  IN OUT CPU_SAVE_STATE_PACKAGE         *CpuSaveStatePackage
  );
//
// GetMode
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_MODE) (
  OUT    UINTN                          *Mode
  );
//
// SetMode
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SET_MODE) (
  IN     UINTN                          Mode
  );
//
// UsbSmiRegister
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SMI_REGISTER) (
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_CORE_SMM_USB_DISPATCH  Callback,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  );
//
// UsbSmiUnregister
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SMI_UNREGISTER) (
  IN     VOID                           *Handle
  );
//
// InsertKbcKeyCode
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_KBC_KEYCODE) (
  IN     UINT8                          QueueSlot,
  IN     UINT8                          *KeyCode,
  IN     UINTN                          Length
  );
//
// KbcProcessor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_KBC_TRAP_PROCESSOR) (
  IN     UINTN                          TrapType,
  IN     UINT8                          Data
  );
//
// GetLegacySupportProvider
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_LEGACY_SUPPORT_PROVIDER) (
  IN OUT EFI_USB3_HC_PROTOCOL           **Usb3Hc
  );
//
// InsertLegacySupportProvider
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_LEGACY_SUPPORT_PROVIDER) (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );
//
// RemoveLegacySupportProvider
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REMOVE_LEGACY_SUPPORT_PROVIDER) (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );
//
// InsertPeriodicTimer
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_PERIODIC_TIMER) (
  IN     UINTN                          Type,
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     VOID                           *Context,
  IN     UINTN                          MilliSecond,
  IN OUT VOID                           **Handle
  );
//
// RemovePeriodicTimer
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REMOVE_PERIODIC_TIMER) (
  IN     VOID                           *Handle
  );
//
// InsertPeriodicTimerProvider
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_PERIODIC_TIMER_PROVIDER) (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );
//
// RemovePeriodicTimerProvider
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REMOVE_PERIODIC_TIMER_PROVIDER) (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  );
//
// EnterCriticalSection
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_ENTER_CRITICAL_SECTION) (
  );
//
// LeaveCriticalSection
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_LEAVE_CRITICAL_SECTION) (
  );
//  
// DispatchHcCallback
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_DISPATCH_HC_CALLBACK) (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

//  
// Support
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_BINDING_SUPPORTED) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );
//  
// Start
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_BINDING_START) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  );
//  
// Stop
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_BINDING_STOP) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  );
//
// RegisterUsbBindingProtocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REGISTER_USB_BINDING_PROTOCOL) (
  IN     EFI_USB_BINDING_SUPPORTED      Support,
  IN     EFI_USB_BINDING_START          Start,
  IN     EFI_USB_BINDING_STOP           Stop
  );
//
// GetUsbDevices
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_USB_DEVICES) (
  IN     UINTN                          Type,
  IN OUT UINTN                          *Count,
  IN OUT USB_DEVICE                     ***Devices
  );
//
// InsertUsbIoDevice
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_USB_DEVICE) (
  IN     USB_DEVICE                     *UsbDevice
  );
//
// RemoveUsbIoDevice
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REMOVE_USB_DEVICE) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );
//
// ConnectUsbDevices
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CONNECT_USB_DEVICES) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );
//
// DisconnectUsbDevices
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_DISCONNECT_USB_DEVICES) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );
//
// RegisterNonSmmNotify
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REGISTER_NON_SMM_CALLBACK) (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );
//
// CheckIgnoredDevice
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CHECK_IGNORED_DEVICE) (
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );
//
// CheckDeviceDetached
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CHECK_DEVICE_DETACHED) (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  );
//
// CpuWbinvd
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_CPU_WBINVD) (
  );
//
// IsKbcExist
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IS_KBC_EXIST) (
  );
//
// ModuleRegistration
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_MODULE_REGISTRATION) (
  IN     EFI_HANDLE                     ImageHandle
  );
//
// IsInSmm
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IS_IN_SMM) (
  OUT    BOOLEAN                        *InSmm
  );
//
// InsertAddressConvertTable
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_INSERT_ADDRESS_CONVERT_TABLE) (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress,
  IN     UINTN                          Count
  );
//
// RemoveAddressConvertTable
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REMOVE_ADDRESS_CONVERT_TABLE) (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress
  );
//
// AddressConvert
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_ADDRESS_CONVERT) (
  IN     UINTN                          Type,
  IN     VOID                           *Address,
  OUT    VOID                           **ConvertedAddress
  );
//
// SchedularConnection
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SCHEDULAR_CONNECTION) (
  IN     EFI_HANDLE                     Controller,
  IN     EFI_DEVICE_PATH_PROTOCOL       *TargetDevicePath,
  IN OUT VOID                           **Context,
  IN     UINTN                          Timeout,
  OUT    UINTN                          *Result
  );
//
// IsCsmEnabled
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_IS_CSM_ENABLED) (
  );
//
// GetSwSmiPort
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_GET_SWSMI_PORT) (
  OUT    UINT16                         *SwSmiPort
  );
//
// RegisterLegacyFreeHcCallback
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REGISTER_LEGACYFREEHC_CALLBACK) (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINT32                         PciAddress,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  );
//
// UnregisterLegacyFreeHcCallback
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_UNREGISTER_LEGACYFREEHC_CALLBACK) (
  IN     VOID                           *Handle
  );
//
// SyncKbdLed
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_SYNC_KBD_LED) (
  );
//
// Register HID descriptor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_REGISTER_HID_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  EFI_USB_HID_DESCRIPTOR            *HidDescriptor
  );
//
// Unregister HID descriptor
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_CORE_UNREGISTER_HID_DESCRIPTOR) (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo
  );
//
// Here is the protocol
//
struct _EFI_USB_CORE_PROTOCOL {
  EFI_USB_CORE_GET_DESCRIPTOR                   UsbGetDescriptor;
  EFI_USB_CORE_SET_DESCRIPTOR                   UsbSetDescriptor;
  EFI_USB_CORE_GET_DEV_INTERFACE                UsbGetDeviceInterface;
  EFI_USB_CORE_SET_DEV_INTERFACE                UsbSetDeviceInterface;
  EFI_USB_CORE_GET_DEV_CONFIGURATION            UsbGetDeviceConfiguration;
  EFI_USB_CORE_SET_DEV_CONFIGURATION            UsbSetDeviceConfiguration;
  EFI_USB_CORE_SET_DEV_FEATURE                  UsbSetDeviceFeature;
  EFI_USB_CORE_CLR_DEV_FEATURE                  UsbClearDeviceFeature;
  EFI_USB_CORE_GET_DEV_STATUS                   UsbGetDeviceStatus;
  EFI_USB_CORE_GET_DEV_STRING                   UsbGetString;
  EFI_USB_CORE_CLR_ENDPOINT_HALT                UsbClearEndpointHalt;
  EFI_USB_CORE_GET_HID_DESCRIPTOR               UsbGetHidDescriptor;
  EFI_USB_CORE_GET_REPORT_DESCRIPTOR            UsbGetReportDescriptor;
  EFI_USB_CORE_GET_PROTOCOL_REQUEST             UsbGetProtocolRequest;
  EFI_USB_CORE_SET_PROTOCOL_REQUEST             UsbSetProtocolRequest;
  EFI_USB_CORE_GET_IDLE_REQUEST                 UsbGetIdleRequest;
  EFI_USB_CORE_SET_IDLE_REQUEST                 UsbSetIdleRequest;
  EFI_USB_CORE_GET_REPORT_REQUEST               UsbGetReportRequest;
  EFI_USB_CORE_SET_REPORT_REQUEST               UsbSetReportRequest;
  EFI_USB_CORE_ALLOCATE_BUFFER                  AllocateBuffer;
  EFI_USB_CORE_FREE_BUFFER                      FreeBuffer;
  EFI_USB_CORE_MEMORY_MAPPING                   MemoryMapping;
  EFI_USB_CORE_MEMORY_UNMAPPING                 MemoryUnmapping;
  EFI_USB_CORE_PCIIO_PCI_READ                   PciIoPciRead;
  EFI_USB_CORE_PCIIO_PCI_WRITE                  PciIoPciWrite;
  EFI_USB_CORE_PCIIO_MEM_READ                   PciIoMemRead;
  EFI_USB_CORE_PCIIO_MEM_WRITE                  PciIoMemWrite;
  EFI_USB_CORE_PCIIO_IO_READ                    PciIoIoRead;
  EFI_USB_CORE_PCIIO_IO_WRITE                   PciIoIoWrite;
  EFI_USB_CORE_IO_READ8                         IoRead8;
  EFI_USB_CORE_IO_WRITE8                        IoWrite8;
  EFI_USB_CORE_IO_READ16                        IoRead16;
  EFI_USB_CORE_IO_WRITE16                       IoWrite16;
  EFI_USB_CORE_IO_READ32                        IoRead32;
  EFI_USB_CORE_IO_WRITE32                       IoWrite32;
  EFI_USB_CORE_BSWAP16                          Bswap16;
  EFI_USB_CORE_BSWAP32                          Bswap32;
  EFI_USB_CORE_STALL                            Stall;
  EFI_USB_CORE_CPU_SAVE_STATE                   CpuSaveState;
  EFI_USB_CORE_GET_MODE                         GetMode;
  EFI_USB_CORE_SET_MODE                         SetMode;
  EFI_USB_CORE_SMI_REGISTER                     UsbSmiRegister;
  EFI_USB_CORE_SMI_UNREGISTER                   UsbSmiUnregister;
  EFI_USB_CORE_INSERT_KBC_KEYCODE               InsertKbcKeyCode;
  EFI_USB_CORE_KBC_TRAP_PROCESSOR               KbcTrapProcessor;
  EFI_USB_CORE_GET_LEGACY_SUPPORT_PROVIDER      GetLegacySupportProvider;
  EFI_USB_CORE_INSERT_LEGACY_SUPPORT_PROVIDER   InsertLegacySupportProvider;
  EFI_USB_CORE_REMOVE_LEGACY_SUPPORT_PROVIDER   RemoveLegacySupportProvider;
  EFI_USB_CORE_INSERT_PERIODIC_TIMER            InsertPeriodicTimer;
  EFI_USB_CORE_REMOVE_PERIODIC_TIMER            RemovePeriodicTimer;
  EFI_USB_CORE_INSERT_PERIODIC_TIMER_PROVIDER   InsertPeriodicTimerProvider;
  EFI_USB_CORE_REMOVE_PERIODIC_TIMER_PROVIDER   RemovePeriodicTimerProvider;
  EFI_USB_CORE_ENTER_CRITICAL_SECTION           EnterCriticalSection;
  EFI_USB_CORE_LEAVE_CRITICAL_SECTION           LeaveCriticalSection;
  EFI_USB_CORE_DISPATCH_HC_CALLBACK             DispatchHcCallback;
  EFI_USB_CORE_REGISTER_USB_BINDING_PROTOCOL    RegisterUsbBindingProtocol;
  EFI_USB_CORE_GET_USB_DEVICES                  GetUsbDevices;
  EFI_USB_CORE_INSERT_USB_DEVICE                InsertUsbDevice;
  EFI_USB_CORE_REMOVE_USB_DEVICE                RemoveUsbDevice;
  EFI_USB_CORE_CONNECT_USB_DEVICES              ConnectUsbDevices;
  EFI_USB_CORE_DISCONNECT_USB_DEVICES           DisconnectUsbDevices;
  EFI_USB_CORE_REGISTER_NON_SMM_CALLBACK        RegisterNonSmmCallback;
  EFI_USB_CORE_CHECK_IGNORED_DEVICE             CheckIgnoredDevice;
  EFI_USB_CORE_CHECK_DEVICE_DETACHED            CheckDeviceDetached;
  EFI_USB_CORE_CPU_WBINVD                       CpuWbinvd;
  EFI_USB_CORE_IS_KBC_EXIST                     IsKbcExist;
  EFI_USB_CORE_MODULE_REGISTRATION              ModuleRegistration;
  EFI_USB_CORE_IS_IN_SMM                        IsInSmm;
  EFI_USB_CORE_INSERT_ADDRESS_CONVERT_TABLE     InsertAddressConvertTable;
  EFI_USB_CORE_REMOVE_ADDRESS_CONVERT_TABLE     RemoveAddressConvertTable;
  EFI_USB_CORE_ADDRESS_CONVERT                  AddressConvert;
  EFI_USB_CORE_SCHEDULAR_CONNECTION             SchedularConnection;
  EFI_USB_CORE_IS_CSM_ENABLED                   IsCsmEnabled;
  EFI_USB_CORE_GET_SWSMI_PORT                   GetSwSmiPort;
  EFI_USB_CORE_REGISTER_LEGACYFREEHC_CALLBACK   RegisterLegacyFreeHcCallback;
  EFI_USB_CORE_UNREGISTER_LEGACYFREEHC_CALLBACK UnregisterLegacyFreeHcCallback;
  EFI_USB_CORE_SYNC_KBD_LED                     SyncKbdLed;
  EFI_USB_CORE_REGISTER_HID_DESCRIPTOR          RegisterHidDescriptor;
  EFI_USB_CORE_UNREGISTER_HID_DESCRIPTOR        UnregisterHidDescriptor;
};

extern EFI_GUID gEfiUsbCoreProtocolGuid;

#endif
