/** @file
  Usb3 Host Controller Protocol Header

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _USB3_HC_H_
#define _USB3_HC_H_

#include <Uefi.h>
#include <IndustryStandard/Usb.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/UsbIo.h>
#include <Protocol/UsbHostController.h>
//
// Leak of definition before EDK-20060515
//
#ifndef USB_PORT_STAT_HIGH_SPEED
  #define USB_PORT_STAT_HIGH_SPEED                      0x0400
#endif
//
// Leak of definition before EDK-20060515
//
#ifndef USB_PORT_STAT_OWNER
  #define USB_PORT_STAT_OWNER                           0x0800
#endif
//
// Leak of definition before EDK-20060515
//
#ifndef EFI_USB2_HC_PROTOCOL_GUID
  #define EFI_USB_SPEED_FULL                            0x0000
  #define EFI_USB_SPEED_LOW                             0x0001
  #define EFI_USB_SPEED_HIGH                            0x0002
  #define EFI_USB_MAX_BULK_BUFFER_NUM                   10
  #define EFI_USB_MAX_ISO_BUFFER_NUM                    7
  #define EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG            0x0004
  #define EFI_USB_HC_RESET_HOST_WITH_DEBUG              0x0008
#endif
//
// Leak of definition before UEFI 2.3.1
//
#ifndef USB_PORT_STAT_SUPER_SPEED
  #define USB_PORT_STAT_SUPER_SPEED                     0x2000
#endif
//
// Leak of definition before UEFI 2.3.1
//
#ifndef EFI_USB_SPEED_SUPER
  #define EFI_USB_SPEED_SUPER                           0x0003
#endif

#define USB_PORT_STAT_SUPER_SPEED_MASK                  0x1c00
#define USB_PORT_STAT_SUPER_SPEED_POWER                 0x0200

#define USB_PORT_STAT_C_BH_RESET                        0x0020
#define USB_PORT_STAT_C_LINK_STATE                      0x0040
#define USB_PORT_STAT_C_CONFIG_ERROR                    0x0080

#define EfiUsbPortBhResetChange                         29
#define EfiUsbPortLinkStateChange                       25
#define EfiUsbPortConfigErrorChange                     26

#define USB_DESC_TYPE_HUB                               0x29
#define USB_DESC_TYPE_SSHUB                             0x2A
//
// Definition for Multi-LUN
//
#define USB_DEV_GET_MAX_LUN                             0xFE
#define USB_DEV_SET_MAX_LUN_REQ_TYPE                    0xA1

#define EFI_USB3_HC_PROTOCOL_GUID \
{ \
  0x152d34b7, 0xb4b6, 0x4b26, 0x97, 0xf, 0x1e, 0xc3, 0xd9, 0x9d, 0xa, 0x84 \
}
//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_USB3_HC_PROTOCOL    EFI_USB3_HC_PROTOCOL;

#pragma pack(1)
//
// USB HUB related definitions
//
typedef struct {
  UINT8         Length;
  UINT8         DescriptorType;
  UINT8         NbrPorts;
  UINT8         HubCharacteristics[2];
  UINT8         PwrOn2PwrGood;
  UINT8         HubContrCurrent;
  UINT8         Filler[8];
} EFI_USB_HUB_DESCRIPTOR;

typedef struct {
  UINT16        HubStatus;
  UINT16        HubChange;
} EFI_USB_HUB_STATUS;
//
// Super Speed Endpoint descriptor
//
typedef struct {
  UINT8         Length;
  UINT8         DescriptorType;
  UINT8         MaxBurst;
  UINT8         Attributes;
  UINT16        BytesPerInterval;
} EFI_USB_SS_ENDPOINT_DESCRIPTOR;

typedef struct {
  UINT8         TranslatorHubAddress;
  UINT8         TranslatorPortNumber;
  UINT8         RootHubPortNumber;
  UINT8         MultiTT;
  UINT32        RouteString;
} EFI_USB3_HC_TRANSACTION_TRANSLATOR;

#pragma pack()

//
// Protocol definitions
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_GET_CAPABILITY) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  OUT    UINT8                                          *MaxSpeed,
  OUT    UINT8                                          *PortNumber,
  OUT    UINT8                                          *Is64BitCapable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_RESET) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT16                                         Attributes
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_GET_STATE) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  OUT    EFI_USB_HC_STATE                               *State
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_SET_STATE) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     EFI_USB_HC_STATE                               State
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_CONTROL_TRANSFER) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          DeviceSpeed,
  IN     UINTN                                          MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST                         *Request,
  IN     EFI_USB_DATA_DIRECTION                         TransferDirection,
  IN OUT VOID                                           *Data       OPTIONAL,
  IN OUT UINTN                                          *DataLength OPTIONAL,
  IN     UINTN                                          TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR             *Translator,
  OUT    UINT32                                         *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_BULK_TRANSFER) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          EndPointAddress,
  IN     UINT8                                          DeviceSpeed,
  IN     UINTN                                          MaximumPacketLength,
  IN     UINT8                                          DataBuffersNumber,
  IN OUT VOID                                           *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                                          *DataLength,
  IN OUT UINT8                                          *DataToggle,
  IN     UINTN                                          TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR             *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK                CallBackFunction OPTIONAL,
  IN     VOID                                           *Context         OPTIONAL,
  OUT    UINT32                                         *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_INTERRUPT_TRANSFER) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          EndPointAddress,
  IN     UINT8                                          DeviceSpeed,
  IN     UINTN                                          MaximumPacketLength,
  IN OUT VOID                                           *Data,
  IN OUT UINTN                                          *DataLength,
  IN OUT UINT8                                          *DataToggle,
  IN     UINTN                                          TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR             *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK                CallBackFunction OPTIONAL,
  IN     VOID                                           *Context         OPTIONAL,
  IN     UINTN                                          PollingInterval  OPTIONAL,
  OUT    UINT32                                         *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_ISOCHRONOUS_TRANSFER) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          EndPointAddress,
  IN     UINT8                                          DeviceSpeed,
  IN     UINTN                                          MaximumPacketLength,
  IN     UINT8                                          DataBuffersNumber,
  IN OUT VOID                                           *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                                          *DataLength,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR             *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK                CallBackFunction OPTIONAL,
  IN     VOID                                           *Context         OPTIONAL,
  OUT    UINT32                                         *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          PortNumber,
  OUT    EFI_USB_PORT_STATUS                            *PortStatus
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          PortNumber,
  IN     EFI_USB_PORT_FEATURE                           PortFeature
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          PortNumber,
  IN     EFI_USB_PORT_FEATURE                           PortFeature
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_CANCEL_IO) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          EndPointAddress,
  IN     UINTN                                          PollingInterval,
  IN OUT UINT8                                          *DataToggle
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_QUERY_IO) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINT8                                          DeviceAddress,
  IN     UINT8                                          EndPointAddress,
  IN     UINTN                                          PollingInterval
  );

#define HC_EVENT_IO_COMPLETE                            0x0001
#define HC_EVENT_CONNECT_CHANGE                         0x0002
#define HC_EVENT_KBC_TRAPED                             0x0004
#define HC_EVENT_HCD_OWNER_CHANGED                      0x0008
#define HC_EVENT_TYPE_MASK                              0x00FF

typedef
VOID
(EFIAPI *EFI_USB3_HC_CALLBACK) (
  IN     UINTN                                          Event,
  IN     VOID                                           *Context
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_REGISTER_EVENT) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINTN                                          EventType,
  IN     EFI_USB3_HC_CALLBACK                           Callback,
  IN     VOID                                           *Content,
  IN OUT VOID                                           **Handle
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_UNREGISTER_EVENT) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     VOID                                           *Handle
  );

#define USB3_HC_HID_SUPPORT_SETUP_EMULATION             0
#define USB3_HC_HID_SUPPORT_GET_KBC_DATA_POINTER        1
#define USB3_HC_HID_SUPPORT_GET_KBC_STATUS_POINTER      2
#define USB3_HC_HID_SUPPORT_GENERATE_IRQ                3
#define USB3_HC_HID_SUPPORT_GET_CAPABILITY              4

#define CAP_ASYNC_CPU_TRAP                              0x00000001

typedef
EFI_STATUS
(EFIAPI *EFI_USB3_HC_PROTOCOL_LEGACY_HID_SUPPORT) (
  IN     EFI_USB3_HC_PROTOCOL                           *This,
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

struct _EFI_USB3_HC_PROTOCOL {
  EFI_USB3_HC_PROTOCOL_GET_CAPABILITY                   GetCapability;
  EFI_USB3_HC_PROTOCOL_RESET                            Reset;
  EFI_USB3_HC_PROTOCOL_GET_STATE                        GetState;
  EFI_USB3_HC_PROTOCOL_SET_STATE                        SetState;
  EFI_USB3_HC_PROTOCOL_CONTROL_TRANSFER                 ControlTransfer;
  EFI_USB3_HC_PROTOCOL_BULK_TRANSFER                    BulkTransfer;
  EFI_USB3_HC_PROTOCOL_INTERRUPT_TRANSFER               InterruptTransfer;
  EFI_USB3_HC_PROTOCOL_ISOCHRONOUS_TRANSFER             IsochronousTransfer;
  EFI_USB3_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS          GetRootHubPortStatus;
  EFI_USB3_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE         SetRootHubPortFeature;
  EFI_USB3_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE       ClearRootHubPortFeature;
  EFI_USB3_HC_PROTOCOL_CANCEL_IO                        CancelIo;
  EFI_USB3_HC_PROTOCOL_QUERY_IO                         QueryIo;
  EFI_USB3_HC_PROTOCOL_REGISTER_EVENT                   RegisterEvent;
  EFI_USB3_HC_PROTOCOL_UNREGISTER_EVENT                 UnregisterEvent;
  EFI_USB3_HC_PROTOCOL_LEGACY_HID_SUPPORT               LegacyHidSupport;
  UINT16                                                MajorRevision;
  UINT16                                                MinorRevision;
};

extern EFI_GUID gEfiUsb3HcProtocolGuid;

//
// The UsbLegacyContol protocol definitions
//
#define  USB_LEGACY_CONTROL_PROTOCOL_GUID \
 { 0x3084d2bd, 0xf589, 0x4be1, 0x8e, 0xf0, 0x26, 0xc6, 0xd6, 0x8a, 0x1b, 0xc8 }

typedef
EFI_STATUS
(EFIAPI *USB_LEGACY_CONTROL) (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

typedef struct {
  USB_LEGACY_CONTROL                                    UsbLegacyControl;
  EFI_USB3_HC_CALLBACK                                  SmiHandler;
  VOID                                                  *SmiContext;
  BOOLEAN                                               InSmm;
} USB_LEGACY_CONTROL_PROTOCOL;

#endif
