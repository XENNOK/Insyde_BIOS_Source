/** @file
  USB Status Code related definitions and fundtions

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _USB_DEBUG_H_
#define _USB_DEBUG_H_

#define BYPASS_TIMEOUT         10

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


#define EUSBCMD_RS             BIT0  /* Run/Stop */
#define EUSBCMD_HCRESET        BIT1  /* Host reset */
#define EUSBCMD_PSE            BIT4  /* Periodic Schedule Enable */
#define EUSBCMD_ASE            BIT5  /* Asynchronous Schedule Enable */
#define EUSBCMD_IAAD           BIT6  /* Interrupt om Async Advance Doorbell */
#define EUSBCMD_ITC            BIT16 | BIT17 | BIT18 | BIT19 | BIT20 | BIT21 | BIT22 | BIT23 /* Interrupt Threshold Control */
#define EUSBSTS_HCHALTED       BIT12
#define	EUSBCONFIG_CF          BIT0
#define EUSBPORTSC_CCS         BIT0  /* Current Connect Status "device present" */
#define EUSBPORTSC_CSC         BIT1  /* Connect Status Change */
#define EUSBPORTSC_PED         BIT2  /* Port Enable / Disable */
#define EUSBPORTSC_PEDC        BIT3  /* Port Enable / Disable Change */
#define EUSBPORTSC_OVERA       BIT4  /* Overcurrent Active */
#define EUSBPORTSC_OVERC       BIT5  /* Overcurrent Change */
#define EUSBPORTSC_FPR         BIT6  /* Force Port Resume*/
#define EUSBPORTSC_SUSP        BIT7  /* Suspend */
#define EUSBPORTSC_PR          BIT8  /* Port Reset */
#define EUSBPORTSC_LS          BIT10 | BIT11  /* Line Status */
#define LINE_STATUS_J          BIT11
#define LINE_STATUS_K          BIT10

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

#endif