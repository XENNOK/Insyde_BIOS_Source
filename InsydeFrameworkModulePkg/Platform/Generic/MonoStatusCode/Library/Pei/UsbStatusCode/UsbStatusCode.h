//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/*++

Module Name:

  UsbStatusCode.h

Abstract:

  Lib to provide USB port status code routines

--*/

#ifndef _PEI_USB_STATUS_CODE_H_
#define _PEI_USB_STATUS_CODE_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "SimpleCpuIoLib.h"
#include "UsbStatusCodeLib.h"
#include "EfiCommonLib.h"

//
// GUID consumed
//
#include EFI_GUID_DEFINITION (StatusCodeDataTypeId)

#define PLATFORM_USB20_BAR     0xF0000000
#define PLATFORM_USB20_VID     0x00
#define PLATFORM_USB20_PCICMD  0x04
#define	PLATFORM_USB20_RID     0x08
#define	PLATFORM_USB20_MEMBASE 0x10

#define	DEBUG_DEVICE_ADDR      0x7F

// EHCI new added PID
#define SETUP_TOKEN_ID         0x2D
#define INPUT_TOKEN_ID         0x69
#define OUTPUT_TOKEN_ID        0xE1

#define DATA0_PID              0xC3
#define DATA1_PID              0x4B

#define PEI_TIMEOUT_TICKS      1000

// Get Descriptors
#define GET_DESC_REQ_TYPE      0x80
#define GET_DESC_REQ           0x06
#define DEVICE_TYPE            0x01
#define DEBUG_TYPE             0x0A

// Set Debug Mode
#define SET_FEATURE_REQ_TYPE   0x00
#define SET_FEATURE_REQ        0x03
#define DEBUG_MODE             0x06

// Set Address
#define SET_ADDRESS_REQ_TYPE   0x00
#define SET_ADDRESS_REQ        0x05

// Clear Feature
#define CLEAR_FEATURE_REQ_TYPE 0x02
#define CLEAR_FEATURE_REQ      0x01
#define ENDPOINT_HALT          0x00

#define bit(a)                 (1 << (a))

#define EUSBCMD_RS             bit(0)  /* Run/Stop */
#define EUSBCMD_HCRESET        bit(1)  /* Host reset */
#define EUSBCMD_PSE            bit(4)  /* Periodic Schedule Enable */
#define EUSBCMD_ASE            bit(5)  /* Asynchronous Schedule Enable */
#define EUSBCMD_IAAD           bit(6)  /* Interrupt om Async Advance Doorbell */
#define EUSBCMD_ITC            bit(16) | bit(17) | bit(18) | bit(19) | bit(20) | bit(21) | bit(22) | bit(23) /* Interrupt Threshold Control */
#define EUSBSTS_HCHALTED       bit(12)
#define	EUSBCONFIG_CF          bit(0)
#define EUSBPORTSC_CCS         bit(0)  /* Current Connect Status ("device present") */
#define EUSBPORTSC_CSC         bit(1)  /* Connect Status Change */
#define EUSBPORTSC_PED         bit(2)  /* Port Enable / Disable */
#define EUSBPORTSC_PEDC        bit(3)  /* Port Enable / Disable Change */
#define EUSBPORTSC_OVERA       bit(4)  /* Overcurrent Active */
#define EUSBPORTSC_OVERC       bit(5)  /* Overcurrent Change */
#define EUSBPORTSC_FPR         bit(6)  /* Force Port Resume*/
#define EUSBPORTSC_SUSP        bit(7)  /* Suspend */
#define EUSBPORTSC_PR          bit(8)  /* Port Reset */
#define EUSBPORTSC_LS          bit(10) | bit(11)  /* Line Status */
#define LINE_STATUS_J          bit(11)
#define LINE_STATUS_K          bit(10)

#define DEBUG_WRITE            1
#define DEBUG_READ             0

#pragma pack(1)

typedef struct {
  UINT8        RequestType;
  UINT8        Request;
  UINT16       Value;
  UINT16       Index;
  UINT16       Length;
} EFI_USB_DEVICE_REQUEST;

typedef struct {
  UINT8        Length;
  UINT8        DescriptorType;
  UINT16       BcdUSB;
  UINT8        DeviceClass;
  UINT8        DeviceSubClass;
  UINT8        DeviceProtocol;
  UINT8        MaxPacketSize0;
  UINT16       IdVendor;
  UINT16       IdProduct;
  UINT16       BcdDevice;
  UINT8        StrManufacturer;
  UINT8        StrProduct;
  UINT8        StrSerialNumber;
  UINT8        NumConfigurations;
} EFI_USB_DEVICE_DESCRIPTOR;

typedef  struct{
  UINT8        Length;
  UINT8        DescriptorType;
  UINT8        DebugInEndpoint;
  UINT8        DebugOutEndpoint;
} EFI_USB_DEBUG_DESCRIPTOR;

typedef struct {
  UINT32       DataLength : 4;
  UINT32       Write      : 1;
  UINT32       Go         : 1;
  UINT32       Error      : 1;
  UINT32       Exception  : 3;
  UINT32       InUse      : 1;
  UINT32       Reserve1   : 5;
  UINT32       Done       : 1;
  UINT32       Reserve2   : 11;
  UINT32       Enabled    : 1;
  UINT32       Reserve3   : 1;
  UINT32       Owner      : 1;
  UINT32       Reserve4   : 1;
} EFI_USB_DBG_SC;

typedef struct {
  UINT8        TokenPid;
  UINT8        SendPid;
  UINT8        ReceivedStatusPid;
  UINT8        Reserve;
} EFI_USB_DBG_PID;

typedef struct {
  UINT32       Endpoint : 4;
  UINT32       Reserve1 : 4;
  UINT32       Address  : 7;
  UINT32       Reserve2 : 17;
} EFI_USB_DBG_ADDR;

typedef struct {
  UINT8        EndpointIn;
  UINT8        ToggleIn;
  UINT8        EndpointOut;
  UINT8        ToggleOut;
} DEBUG_BULK;

typedef struct {
  UINT8        CapLength;
  UINT8        Reserve;
  UINT16       HCVersion;
  UINT32       HcsParams;
  UINT32       HccParams;
} EFI_USB_CAP_REG;

typedef struct {
  UINT32       UsbCmd;
  UINT32       UsbSts;
  UINT32       UsbIntr;
  UINT32       Frindex;
  UINT32       CtrlDsseMent;
  UINT32       Periodic;
  UINT32       Async;
  UINT32       Reserve[9];
  UINT32       Cfg;
  UINT32       PortSC[8];
} EFI_USB_CTRL_REG;

typedef struct {
  EFI_USB_DBG_SC      Sc;
  EFI_USB_DBG_PID     Pid;
  UINT32              Data0;
  UINT32              Data1;
  EFI_USB_DBG_ADDR    Addr;
} EFI_USB_DBG_REG;

#pragma pack()

VOID
DebugUsbPrint (
  IN UINT8    *OutputString
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  OutputString  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
DebugUsbWrite (
  IN OUT UINTN                  *BufferSize,
  IN VOID                       *Buffer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Character - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
